#include "common.h"
#include "driver/timer.h"
#include "kernel/proc.h"
#include "kernel/sched.h"
#include "kernel/syscall.h"
#include "mm/trapframe.h"

// #define QUIET
#define __MODULE_NAME__ TRAP
#include "debug.h"

extern int handle_pagefault(uint64_t scause);
extern int handle_irq(uint64_t scause);
extern void kernelvec();

void trap_init(void) {
    init_timer();
}

/**
 * @brief Set up to take exceptions and traps while in the kernel.
 */
void trap_init_hart(void) {
    // setup trap vector
    write_csr(stvec, (uint64_t)kernelvec);
    // enable interrupt
    write_csr(sie, SIE_SEIE | SIE_STIE | SIE_SSIE);
    // reset timer
    reset_timer();
}

/**
 * @brief handle an interrupt, exception, or system call from user space.
 */
void usertrap(void) {
    uint64_t sstatus = read_csr(sstatus);
    uint64_t scause = read_csr(scause);
    uint64_t sepc = read_csr(sepc);
    uint64_t stval = read_csr(stval);
    struct proc *p = current;

    if ((sstatus & SSTATUS_SPP) != 0) {
        kprintf("scause %p\n", scause);
        kprintf("sepc=%p stval=%p\n", sepc, stval);
        panic("usertrap: not from user mode");
    }

    if (intr_get() != 0) {
        kprintf("scause %s\n", riscv_cause2str(scause));
        kprintf("sepc=%p stval=%p\n", sepc, stval);
        panic("utrap: interrupts enabled");
    }

    // send interrupts and exceptions to kerneltrap(),
    // since we're now in the kernel.
    write_csr(stvec, (uint64_t)kernelvec);

    // save user program counter.
    p->trapframe->epc = sepc;
    tf_flstore(p->trapframe);

    if (p->killed)
        exit(-1);

    switch (scause) {
        when_syscall {
            // sepc points to the ecall instruction,
            // but we want to return to the next instruction.
            p->trapframe->epc += 4;
            p->stub_time = ticks;
            intr_on();
            syscall();
            p->s_time += ticks - p->stub_time;
            break;
        }
        when_misalign {
            warn("software misaligned access is not support now. sepc is %lx\n", sepc);
            goto kill;
        }
        when_pagefault {
            if (p == NULL || handle_pagefault(scause) != 0) {
                goto kill;
            }
            break;
        }
        when_irq {
            if (handle_irq(scause) == -1) {
                goto kill;
            }
            break;
        }
        default: {
          kill:
            info("pid is %d sepc is %lx scause is "rd("%s(%d)")" stval is %lx", p->pid, sepc, riscv_cause2str(scause), scause, stval);
            p->killed = 1;
        }

    }

    if (p->killed)
        exit(-1);

    usertrapret();
}

/**
 * @brief Return to user space
 * 
 * @param trapfram user trapframe address
 */
void userret(utf_t *);

/**
 * @brief entry of user trap
 * 
 */
extern void uservec();

void usertrapret(void) {
    struct proc *p = current;
    // we're about to switch the destination of traps from
    // kerneltrap() to usertrap(), so turn off interrupts until
    // we're back in user space, where usertrap() is correct.
    intr_off();

    // set user trap entry
    write_csr(stvec, uservec);

    // set up trapframe values that uservec will need when
    // the process next re-enters the kernel.
    p->trapframe->kernel_sp = p->kstack + KSTACK_SZ; // process's kernel stack
    p->trapframe->kernel_trap = (uint64_t)usertrap;
    p->trapframe->kernel_hartid = cpuid();         // hartid for cpuid()
    // restore float registers
    tf_flrestore(p->trapframe);

    clear_csr(sstatus, SSTATUS_SPP); // clear SPP to 0 for user mode
    set_csr(sstatus, SSTATUS_SPIE); // enable interrupts in user mode

    // set S Exception Program Counter to the saved user pc.
    write_csr(sepc, p->trapframe->epc);

    assert(p->trapframe->proc != 0);

    userret(p->trapframe);
}


/**
 * @brief interrupts and exceptions from kernel code go here via kernelvec,
 *        on whatever the current kernel stack is.
 * 
 * @param context kernel trapframe on kernel stack
 */
void kerneltrap(ktf_t *context) {
    uint64_t scause = read_csr(scause);
    uint64_t sepc = read_csr(sepc);
    uint64_t sstatus = read_csr(sstatus);
    uint64_t stval = read_csr(stval);
    proc_t *p = current;

    if ((sstatus & SSTATUS_SPP) == 0)
        panic("kerneltrap: not from supervisor mode");

    if (intr_get() != 0) {
        kprintf("scause %s\n", riscv_cause2str(scause));
        kprintf("sepc=%p stval=%p\n", sepc, stval);
        panic("kerneltrap: interrupts enabled");
    }

    if (p) {
        p->k_trapframe = context;
    }

    switch (scause) {
        when_misalign {
            kprintf("sepc=%p ", sepc);
            panic("software misaligned access is not support now.");
        }
        when_pagefault {
            if (p == NULL || handle_pagefault(scause) != 0) {
                goto fail;
            }
            break;
        }
        when_irq {
            if (handle_irq(scause) == -1) {
                goto fail;
            }
            break;
        }
        default: goto fail;
    }

    // the yield() may have caused some traps to occur,
    // so restore trap registers for use by kernelvec.S's sepc instruction.
    write_csr(sepc, sepc);
    write_csr(sstatus, sstatus);
    return;

  fail:
    kprintf("scause %s\n", riscv_cause2str(scause));
    kprintf("sepc=%p stval=%p\n", sepc, stval);
    panic("kerneltrap");
}



