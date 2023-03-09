#include "common.h"
#include "driver/timer.h"
#include "kernel/proc.h"
#include "kernel/sched.h"
#include "kernel/time.h"

#define __MODULE_NAME__ IRQ_HANDLER
#include "debug.h"

#define K210_IRQ_SIGNAL 9

extern int handle_ext_irq();
extern void clockintr();

int handle_irq(uint64_t scause) {
    proc_t *p = current;
    switch (scause) {
        when_softirq {
#ifdef K210 // we handle external interrupt with softirq in k210
            if (read_csr(stval) == K210_IRQ_SIGNAL) {
                int ret = handle_ext_irq();
                clear_csr(sip, SIP_SSIP);
                sbi_set_mext();
                if (ret != 0) {
                    goto fail;
                }
                // success, so break.
                break;
            }
#endif
            // we don't handle softirq currently
            goto fail;
        }
        when_hardirq {
            if (handle_ext_irq() == -1) {
                goto fail;
            }
            break;
        }
        when_clock {
            if (cpuid() == 0) {
                // debug("clock...\n");
                clockintr();
            }
            reset_timer();
            if (p && p->state == RUNNING && !p->killed) {
                // give up the CPU if this is a timer interrupt.
                yield();
            }
            break;
        }
        default: goto fail;
    }

    return 0;
  fail:
    return -1;
}