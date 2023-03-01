#include "kernel/proc.h"
#include "kernel/futex.h"
#include "kernel/time.h"
#include "kernel/syscall.h"
#include "mm/vm.h"

#define QUIET
#define __MODULE_NAME__ FUTEX
#include "debug.h"

extern proc_t proc[];


// TODO: 为futex设置单独的等待队列
WAIT_QUEUE_INIT(futex_queue);


uint64_t futex_sleep(void *chan, spinlock_t *futex_lock, timespec_t *time) {
    uint64_t res = 0;
    struct proc *p = myproc();
    sig_handle(p->signal);
    
    DECLARE_WQ_ENTRY(entry);
    wq_prepare(&futex_queue);
    release(futex_lock);
    p->futex_chan = chan;

    if(time) {
        int timeout = time->ts_sec * 1000 + time->ts_nsec / 1000000;
        if(wq_sleep_timeout(&futex_queue, &entry, timeout)) {
            printf("Futex end beacause of TIMEOUT\n");
            res = -ETIMEDOUT;
        }
    } else {
        wq_sleep(&futex_queue, &entry);  
    }

    // Reacquire original lock.
    acquire(futex_lock);
    // release(&p->lock);

    return res;
}

int __futex_wake(void *chan, int n, int requeue, void *newaddr, int requeue_lim) {
    debug_statement(proc_t *me = current);
    struct proc *p;
    wq_entry_t *entry;
    int i = 0;
    debug("PID %d start wake %#lx", me->pid, chan);
    acquire(&futex_queue.wq_lock);
    list_for_each_entry_condition(entry, &futex_queue.head, head, i < n) {
        p = entry->private;

        if(p->futex_chan == chan) {
            p->futex_chan = 0;
            wake_up_process(p);
            i++;
            debug("PID %d woken", p->pid);
        }
    }
    release(&futex_queue.wq_lock);
    debug("PID %d wake end", me->pid);
    if(requeue) {
        debug("PID %d start requeue", me->pid);
        int num = 0;
        
        acquire(&futex_queue.wq_lock);
        list_for_each_entry_condition(entry, &futex_queue.head, head, num < requeue_lim) {
            p = entry->private;
            acquire(&p->lock);
            if(p->futex_chan == chan) {
                if(p->futex_chan == chan) {
                    p->futex_chan = newaddr;
                    num++;
                    debug("PID %d requeued to %#lx", p->pid, newaddr);
                }
            }
            release(&p->lock);
        }
        release(&futex_queue.wq_lock);
        debug("PID %d requeue end", me->pid);
    }
    
    return i;
}

int futex_wake(void *chan, int n) {
    return __futex_wake(chan, n, 0, 0, 0);
}


uint64_t sys_futex(void) {
    uint64_t addr, addr2;
    int op;
    uint val;
    uint val2;
    uint val3;
    uint cntval;

    uint64_t tm_addr;

    proc_t *p = myproc();
    
    // uint64 real;
    // argaddr(2, &real);
    if(argaddr(0, &addr) < 0 || argint(1, &op) < 0 || argint(2, (int *)&val) < 0 ||
      argint(3, (int *)&val2) < 0 || argaddr(3, &tm_addr) < 0 || argaddr(4, &addr2) < 0 || argint(5, (int *)&val3) < 0) {
        return -1;
    }

    // debug("pid %d op %d val %u", p->pid, op, val);
    if((op & 0xf) == FUTEX_WAIT) {
        uint64_t res;
        // debug("cntval is %d val is %d", cntval, val);
        // debug("time addr is %#lx", tm_addr);
        timespec_t tm;
        
        if(tm_addr) {
            if(copy_from_user(&tm, tm_addr, sizeof(timespec_t)) < 0)
                return -1;
        }
        acquire(&p->tg->lock);
        if(copy_from_user(&cntval, addr, sizeof(cntval)) < 0) {
            release(&p->tg->lock);
            return -1;
        }
        if(cntval == val) {
            debug("PID %d sleep on %#lx timeout: %#lx", p->pid, addr, tm_addr);
            res = futex_sleep((void *)addr, &p->tg->lock, tm_addr ? &tm : NULL);
            // res = futex_sleep((void *)addr, &p->tg->lock, NULL);
        }
        release(&p->tg->lock);
        // debug("res is %ld", -res);
        return res;
    } else if((op & 0xf) == FUTEX_WAKE) {
        // debug("wake up others cnt is %ld", val);
        acquire(&p->tg->lock);
        int ans = futex_wake((void *)addr, val);
        release(&p->tg->lock);
        return ans;
    } else if((op & 0xf) == FUTEX_REQUEUE) {
        acquire(&p->tg->lock);
        int ans = __futex_wake((void *)addr, val, 1, (void *)addr2, val2);
        release(&p->tg->lock);
        return ans;
    } else {
        debug("op = %#lx", op);
        panic("unknown op");
    }

    return -1;
}
