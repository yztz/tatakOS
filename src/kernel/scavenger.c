#include "common.h"
#include "kernel/proc.h"

#define __MODULE_NAME__ SCAVENGER
#include "debug.h"

extern proc_t proc[];

void scavenger_routine(proc_t *me) {
    proc_t *p;
    for(;;) {
        for(p = proc; p < &proc[NPROC]; p++) {
            if(p == me) continue;
            if(try_acquire(&p->lock)) {
                if(p->state != ZOMBIE || p->tg->tg_id == p->pid) {
                    release(&p->lock);
                    continue;
                }

                debug(grn("SCAVENGER: PID %d freed\n"), p->pid);

                freeproc(p);
                release(&p->lock);
            }
        }
        yield();
    }
}