#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "atomic/spinlock.h"
#include "kernel/proc.h"
#include "defs.h"
#include "utils.h"
#include "driver/timer.h"

// #define QUIET
#define __MODULE_NAME__ TRAP
#include "debug.h"

extern int handle_pagefault(uint64_t scause);

void kernelvec();
int devintr();

extern pagetable_t kernel_pagetable;
void
trapinit(void)
{
  init_timer();
}

// set up to take exceptions and traps while in the kernel.
void
trapinithart(void)
{
  // 设置中断向量
  write_csr(stvec, (uint64)kernelvec);
  // 使能中断
  write_csr(sie, SIE_SEIE | SIE_STIE | SIE_SSIE);
  // 重置计时器
  reset_timer();
}

//
// handle an interrupt, exception, or system call from user space.
// called from trampoline.S
//
#include "mm/vm.h"
extern void vmprint(pagetable_t pagetable);
void
usertrap(void)
{
  uint64 scause = read_csr(scause);

  if((r_sstatus() & SSTATUS_SPP) != 0){
    printf("scause %p\n", scause);
    printf("sepc=%p stval=%p\n", read_csr(sepc), read_csr(stval));
    panic("usertrap: not from user mode");
  }

  if(intr_get() != 0) {
    printf("scause %s\n", riscv_cause2str(scause));
    printf("sepc=%p stval=%p\n", r_sepc(), r_stval());
    panic("utrap: interrupts enabled");
  }

  // send interrupts and exceptions to kerneltrap(),
  // since we're now in the kernel.
  write_csr(stvec, (uint64)kernelvec);

  struct proc *p = myproc();
  p->u_time += ticks - p->stub_time;
  // save user program counter.
  p->trapframe->epc = read_csr(sepc);
  tf_flstore(p->trapframe);

  if (scause == EXCP_SYSCALL) {
    if(p->killed) {
      exit(-1);
    }
    // sepc points to the ecall instruction,
    // but we want to return to the next instruction.
    p->trapframe->epc += 4;
    // an interrupt will change sstatus &c registers,
    // so don't enable until done with those registers.
    p->stub_time = ticks;
    intr_on();
    syscall();
    p->s_time += ticks - p->stub_time;
  } else if(devintr(scause) == 0) {
    // ok
  } else if(handle_pagefault(scause) == 0) {
    // do_page_fault(scause);
    // ok
  } else {
    info("pid is %d sepc is %lx scause is "rd("%s(%d)")" stval is %lx", p->pid, r_sepc(), riscv_cause2str(scause), scause, r_stval());
    ER();
    p->killed = 1;
  }

  if(p->killed) {
    exit(-1);
  }

  if (scause == INTR_TIMER) {
    yield();
  }

  sig_handle(p->signal);

  usertrapret();
}

// 保留了xv6的注释
// return to user space
//
void userret(tf_t *trapfram);
void uservec();
void
usertrapret(void)
{
  struct proc *p = myproc();
  // we're about to switch the destination of traps from
  // kerneltrap() to usertrap(), so turn off interrupts until
  // we're back in user space, where usertrap() is correct.
  intr_off();
  p->stub_time = ticks;
  // send syscalls, interrupts, and exceptions to trampoline.S
  w_stvec((uint64)uservec);

  // set up trapframe values that uservec will need when
  // the process next re-enters the kernel.        // kernel page table
  p->trapframe->kernel_sp = p->kstack + KSTACK_SZ; // process's kernel stack
  p->trapframe->kernel_trap = (uint64)usertrap;
  p->trapframe->kernel_hartid = r_tp();         // hartid for cpuid()
  tf_flrestore(p->trapframe);
  // set up the registers that trampoline.S's sret will use
  // to get to user space.
  
  // set S Previous Privilege mode to User.
  unsigned long x = r_sstatus();
  x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
  x |= SSTATUS_SPIE; // enable interrupts in user mode
  w_sstatus(x);

  // set S Exception Program Counter to the saved user pc.
  w_sepc(p->trapframe->epc);
  // tell trampoline.S the user page table to switch to.

  // jump to trampoline.S at the top of memory, which 
  // switches to the user page table, restores user registers,
  // and switches to user mode with sret.
  userret(p->trapframe);
}

// interrupts and exceptions from kernel code go here via kernelvec,
// on whatever the current kernel stack is.
void 
kerneltrap(ktf_t *context)
{
  uint64 sepc = r_sepc();
  uint64 sstatus = r_sstatus();
  uint64 scause = r_scause();
  proc_t *p = myproc();
  
  
  if((sstatus & SSTATUS_SPP) == 0)
    panic("kerneltrap: not from supervisor mode");

  if(intr_get() != 0) {
    printf("scause %s\n", riscv_cause2str(scause));
    printf("sepc=%p stval=%p\n", r_sepc(), r_stval());
    panic("kerneltrap: interrupts enabled");
  }

  if(p) p->ktf = context;

  if(devintr(scause) == 0) {
    // ok
  } else if(handle_pagefault(scause) == 0) {
    // ok
  } else{
    printf("scause %s\n", riscv_cause2str(scause));
    printf("sepc=%p stval=%p\n", r_sepc(), r_stval());
    panic("kerneltrap");
  }

  if(scause == INTR_TIMER) {
    // give up the CPU if this is a timer interrupt.
    if(p && p->state == RUNNING) {
      yield();
    }
  }

  // the yield() may have caused some traps to occur,
  // so restore trap registers for use by kernelvec.S's sepc instruction.
  w_sepc(sepc);
  w_sstatus(sstatus);
}

#include "kernel/time.h"

// for ext
extern int handle_ext_irq();
extern void clockintr();

int devintr(uint64 scause) {
  if (scause == INTR_SOFT) { // k210 ext passby soft
    #ifdef K210
    if(r_stval() == 9) {
      int ret;
      ret = handle_ext_irq();
      clear_csr(sip, SIP_SSIP);
      sbi_set_mext();
      return ret;
    }
    panic("handle fail?");
    #endif
  } else if (scause == INTR_EXT) { // only qemu
    #ifdef QEMU
    if(handle_ext_irq() != 0) return -1;
    #endif
  } else if (scause == INTR_TIMER) {

    if(cpuid() == 0){
      // debug("clock...\n");
      clockintr();
    }
    reset_timer();
  } else { // unknow
    return -1;
  }
  return 0;
}


