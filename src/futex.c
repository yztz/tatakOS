#include "kernel/proc.h"
#include "kernel/futex.h"
#include "kernel/time.h"
#include "mm/vm.h"
#include "defs.h"

#define QUIET
#define __MODULE_NAME__ FUTEX
#include "debug.h"

extern proc_t proc[];


uint64_t futex_sleep(void *chan, spinlock_t *futex_lock, timespec_t *time) {
    uint64_t res = -110;
    struct proc *p = myproc();
    timespec_t cur_time, wake_time;
    sig_handle(p->signal);
    // p->fuext_waiting = 1;
    
    acquire(&p->lock);
    release(futex_lock);

    // Go to sleep.
    // p->chan = 0;
    if(time) {
        // printf("timeout is:\n");
        // time_print(time);
        cur_time = TICK2TIMESPEC(ticks);
        p->chan = &ticks;
    }
    
    p->futex_chan = chan;

    do {
        p->state = SLEEPING;
        sched();
        if(p->futex_chan == 0) {
            res = 0;
            break;
        } else if(time == NULL) {
            panic("bad wake up");
        }
        // only time intr
        wake_time = TICK2TIMESPEC(ticks);
        // printf("current time:\n");
        // time_print(&wake_time);
        uint64_t ds = wake_time.ts_sec - cur_time.ts_sec;
        // uint64_t dus = wake_time.tv_nsec - cur_time.tv_nsec;
        // if(ds > time->tv_sec || (ds == time->tv_sec && dus > time->tv_nsec)) {
        if(ds > 2) { // FIXME: 超时时间不准确可能导致错误的超时行为
            debug("PID %d futex timeout", p->pid);
            break;
        }
    } while(1);

    // Tidy up.
    // p->futex_chan = 0;
    p->chan = 0;

    // Reacquire original lock.
    acquire(futex_lock);
    release(&p->lock);

    return res;
}

int __futex_wake(void *chan, int n, int requeue, void *newaddr, int requeue_lim) {
    proc_t *me = myproc();
    struct proc *p;
    int i = 0;
    debug("PID %d start wake %#lx", me->pid, chan);
    for(p = proc; p < &proc[NPROC] && i < n; p++) {
        if(p != me) {
            acquire(&p->lock);
            // if(p->state == SLEEPING && p->futex_chan == chan) {
            if((p->state == SLEEPING || p->state == RUNNABLE) && p->futex_chan == chan) {
                p->futex_chan = 0;
                p->state = RUNNABLE;
                i++;
                debug("PID %d woken", p->pid);
            }
            release(&p->lock);
        }
    }
    debug("PID %d wake end", me->pid);
    if(requeue) {
        debug("PID %d start requeue", me->pid);
        int num = 0;
        for(p = proc; p < &proc[NPROC] && num < requeue_lim; p++) {
            if(p != myproc()) {
                acquire(&p->lock);
                if((p->state == SLEEPING || p->state == RUNNABLE) && p->futex_chan == chan) {
                    p->futex_chan = newaddr;
                    num++;
                    debug("PID %d requeued to %#lx", p->pid, newaddr);
                }
                release(&p->lock);
            }
        }
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

uint64_t sys_set_robust_list(void) {
    debug("set robust called");
    return 0;
}

uint64_t sys_get_robust_list(void) {
    debug("get robust called");
    return 0;
}