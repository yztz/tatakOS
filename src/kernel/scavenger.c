/**
 * @file scavenger.c
 * @author YangZongzhen
 * @brief 
 *  Why do we need it?
 *    In fact, I don't know the free time of a thread very clearly, 
 *    (maybe it will quit when process' exiting?).
 *    So I set up such a kthread with a brutal name
 *    to help us to recycle these dangling threads.
 * @version 0.1
 * @date 2023-03-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "common.h"
#include "kernel/sched.h"
#include "kernel/thread_group.h"

#define __MODULE_NAME__ SCAVENGER
#include "debug.h"

extern proc_t proc[];
proc_t *scavenger;

static void scavenger_routine(proc_t *me) {
    proc_t *p;
    for(;;) {
        for(p = proc; p < &proc[NPROC]; p++) {
            if(p == me) continue;
            if(try_acquire(&p->lock)) {
                if(p->state != ZOMBIE || tg_pid(p->tg) == p->pid) {
                    release(&p->lock);
                    continue;
                }

                // debug(grn("SCAVENGER: PID %d freed\n"), p->pid);

                freeproc(p);
                release(&p->lock);
            }
        }
        yield();
    }
    
}

void scavenger_init() {
    scavenger = kthread_create("scavenger", scavenger_routine); // thread for cleaning dangling threads
    if (scavenger == NULL) {
        panic("scavenger init");
    }
    debug("scavenger init successfully");
}