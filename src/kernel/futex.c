#include "kernel/proc.h"
#include "kernel/futex.h"
#include "kernel/time.h"
#include "kernel/syscall.h"
#include "kernel/thread_group.h"
#include "mm/vm.h"

#define QUIET
#define __MODULE_NAME__ FUTEX
#include "debug.h"


/**
 * @brief wait chan
 * @note futex_queue's lock MUST be held
 * 
 * @param futex_queue 
 * @param chan 
 * @param time 
 * @return uint64_t 
 */
uint64_t futex_sleep(wq_t *futex_queue, void *chan, timespec_t *time) {
    uint64_t res = 0;
    proc_t *p = current;

    // note: process' futex_chan is protected by futex_queue's lock
    // so we need not hold process' lock
    p->futex_chan = chan;
    
    debug("PID %d slept on %#lx", p->pid, chan);

    if(time) {
        int timeout = time->ts_sec * 1000 + time->ts_nsec / 1000000;
        res = wait_event_timeout_locked(futex_queue, p->futex_chan == NULL, timeout);
    } else {
        res = wait_event_locked(futex_queue, p->futex_chan == NULL);
    }

    assert(current->state == RUNNING);
    debug_if(res == -EINTR, "FUTEX_WAIT finished with interrupt")
    debug_if(res == -ETIMEDOUT, "FUTEX_WAIT finished with timeout")
    return res;
}

/**
 * @brief wakeup process sleeping in futex.
 * @note futex_queue's lock MUST be held
 * 
 * @param futex_queue 
 * @param chan 
 * @param n 
 * @param requeue 
 * @param newaddr 
 * @param requeue_lim 
 * @return int 
 */
int __futex_wake(wq_t *futex_queue, void *chan, int n, int requeue, void *newaddr, int requeue_lim) {
    debug_statement(proc_t *me = current);
    proc_t *p;
    wq_entry_t *entry, *tmp;
    debug("PID %d start wake %#lx", me->pid, chan);
    
    int wakeup_num = 0;

    list_for_each_entry_safe_condition(entry, tmp, &futex_queue->head, head, wakeup_num < n) {
        p = entry->private;
        // assert((uint64_t)p > PGSIZE);
        debug("Find PID %d slept on %#lx", p->pid, p->futex_chan);
        if(p->futex_chan == chan) {
            // clear process futex channel
            p->futex_chan = NULL;
            // wake up
            entry->func(futex_queue, entry);

            wakeup_num++;
            debug("PID %d woken", p->pid);
        }
    }

    debug("PID %d wake end", me->pid);
    if(requeue) {
        debug("PID %d start requeue", me->pid);
        int requeue_num = 0;

        list_for_each_entry_condition(entry, &futex_queue->head, head, requeue_num < requeue_lim) {
            p = entry->private;

            if(p->futex_chan == chan) {
                if(p->futex_chan == chan) {
                    p->futex_chan = newaddr;
                    requeue_num++;
                    debug("PID %d requeued to %#lx", p->pid, newaddr);
                }
            }

        }

        debug("PID %d requeue end", me->pid);
    }
    
    return wakeup_num;
}



uint64_t sys_futex(void) {
    uint64_t addr, addr2;
    int op;
    uint val;
    uint val2;
    uint val3;
    uint cntval;

    uint64_t tm_addr;

    if(argaddr(0, &addr) < 0 || argint(1, &op) < 0 || argint(2, (int *)&val) < 0 ||
      argint(3, (int *)&val2) < 0 || argaddr(3, &tm_addr) < 0 || argaddr(4, &addr2) < 0 || argint(5, (int *)&val3) < 0) {
        return -1;
    }

    wq_t *futex_queue = &current->tg->futex_wq;

    // debug("pid %d op %d val %u", p->pid, op, val);
    int ret = -1;

    acquire(&futex_queue->wq_lock);
    if((op & 0xf) == FUTEX_WAIT) {
        // debug("cntval is %d val is %d", cntval, val);
        timespec_t tm;
        
        if(tm_addr && copy_from_user(&tm, tm_addr, sizeof(timespec_t)) < 0) {
            goto rls_bad;
        }
        
        if(copy_from_user(&cntval, addr, sizeof(cntval)) < 0) {
            goto rls_bad;
        }

        if(cntval == val) {
            // debug("PID %d sleep on %#lx timeout: %#lx", p->pid, addr, tm_addr);
            ret = futex_sleep(futex_queue, (void *)addr, tm_addr ? &tm : NULL);
        }
    } else if((op & 0xf) == FUTEX_WAKE) {
        // debug("wake up others cnt is %ld", val);
        ret = __futex_wake(futex_queue, (void *)addr, val, 0, 0, 0);
    } else if((op & 0xf) == FUTEX_REQUEUE) {
        ret = __futex_wake(futex_queue, (void *)addr, val, 1, (void *)addr2, val2);
    } else {
        debug("op = %#lx", op);
        panic("unknown op");
    }
    release(&futex_queue->wq_lock);

    return ret;

rls_bad:
    release(&futex_queue->wq_lock);
    return -1;
}
