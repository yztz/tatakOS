#include "common.h"
#include "kernel/proc.h"

#define __MODULE_NAME__ SCAVENGER
#include "debug.h"

extern proc_t proc[];

void scavenger_routine() {
    proc_t *me = myproc();
    proc_t *p;
    release(&me->lock);
    for(;;) {
        for(p = proc; p < &proc[NPROC]; p++) {
            if(p == me) continue;
            if(p->state != ZOMBIE || p->tg->master_pid == p->pid) continue;
            if(try_acquire(&p->lock)) {
                #ifdef DEBUG
                printf(grn("SCAVENGER: PID %d freed\n"), p->pid);
                #endif
                freeproc(p);
                release(&p->lock);
            }
        }
        yield();
    }
}