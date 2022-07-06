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

// in kernelvec.S, calls kerneltrap().
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
  // w_stvec();
  // 使能中断
  write_csr(sie, SIE_SEIE | SIE_STIE | SIE_SSIE);
  // w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);
  // 重置计时器
  reset_timer();
}

//
// handle an interrupt, exception, or system call from user space.
// called from trampoline.S
//
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

  // if(!IS_INTR(scause)) {
  //   debug("sepc is %lx scause is %lx stval is %lx intr is %d", r_sepc(), scause, r_stval(), intr_get());
  //   LOOP();
  // }
  // send interrupts and exceptions to kerneltrap(),
  // since we're now in the kernel.
  // w_stvec((uint64)kernelvec);
  write_csr(stvec, (uint64)kernelvec);

  struct proc *p = myproc();

  // save user program counter.
  get_trapframe(p->mm)->epc = read_csr(sepc);

  if (scause == EXCP_SYSCALL) {
    if(p->killed) {
      exit(-1);
    }
    // sepc points to the ecall instruction,
    // but we want to return to the next instruction.
    get_trapframe(p->mm)->epc += 4;
    // an interrupt will change sstatus &c registers,
    // so don't enable until done with those registers.
    // debug("usertrap: proc is %s syscall num is %d", p->name, p->trapframe->a7);
    intr_on();
    syscall();
  } else if(devintr(scause) == 0) {
    // ok
  } else if(handle_pagefault(scause) == 0) {
    // ok
  } else {
    printf("usertrap(): unexpected scause %p pid=%d\n", scause, p->pid);
    printf("sepc=%p stval=%p\n", read_csr(sepc), read_csr(stval));
    p->killed = 1;
  }

  if(p->killed)
    exit(-1);

  if (scause == INTR_TIMER) yield();
    
  usertrapret();
}

//
// return to user space
//
void userret(uint64 trapfram);
void uservec();
void
usertrapret(void)
{
  struct proc *p = myproc();
  // we're about to switch the destination of traps from
  // kerneltrap() to usertrap(), so turn off interrupts until
  // we're back in user space, where usertrap() is correct.
  intr_off();
  // send syscalls, interrupts, and exceptions to trampoline.S
  w_stvec((uint64)uservec);

  // set up trapframe values that uservec will need when
  // the process next re-enters the kernel.        // kernel page table
  get_trapframe(p->mm)->kernel_sp = get_kstack(p->mm) + KSTACK_SZ; // process's kernel stack
  get_trapframe(p->mm)->kernel_trap = (uint64)usertrap;
  get_trapframe(p->mm)->kernel_hartid = r_tp();         // hartid for cpuid()

  // set up the registers that trampoline.S's sret will use
  // to get to user space.
  
  // set S Previous Privilege mode to User.
  unsigned long x = r_sstatus();
  x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
  x |= SSTATUS_SPIE; // enable interrupts in user mode
  w_sstatus(x);

  // set S Exception Program Counter to the saved user pc.
  w_sepc(get_trapframe(p->mm)->epc);
  // tell trampoline.S the user page table to switch to.
  // uint64 satp = MAKE_SATP(p->pagetable);

  // jump to trampoline.S at the top of memory, which 
  // switches to the user page table, restores user registers,
  // and switches to user mode with sret.
  userret((uint64)get_trapframe(p->mm));
}

// interrupts and exceptions from kernel code go here via kernelvec,
// on whatever the current kernel stack is.
void 
kerneltrap()
{
  uint64 sepc = r_sepc();
  uint64 sstatus = r_sstatus();
  uint64 scause = r_scause();
  
  
  if(myproc()) {
    myproc()->ktrap_fp = *(uint64*)(r_fp()-16);
  }

  if(!IS_INTR(scause)) {
    debug("sepc is %lx scause is %lx stval is %lx intr is %d", r_sepc(), scause, r_stval(), intr_get());
    debug("kstack: %lx", myproc()->mm->kstack);
    // backtrace(myproc());
    LOOP();
  }

  if((sstatus & SSTATUS_SPP) == 0)
    panic("kerneltrap: not from supervisor mode");
  if(intr_get() != 0)
    panic("kerneltrap: interrupts enabled");

  if(devintr(scause) == 0) {
    // ok
  } else if(handle_pagefault(scause) == 0) {
    // ok
  } else{
    printf("scause %p\n", scause);
    printf("sepc=%p stval=%p\n", r_sepc(), r_stval());
    panic("kerneltrap");
  }

  if(scause == INTR_TIMER) {
    // give up the CPU if this is a timer interrupt.
      if(myproc() != 0 && myproc()->state == RUNNING)
        yield();
  }

  // the yield() may have caused some traps to occur,
  // so restore trap registers for use by kernelvec.S's sepc instruction.
  w_sepc(sepc);
  w_sstatus(sstatus);
}

void
clockintr()
{
  acquire(&tickslock);
  ticks++;
  wakeup(&ticks);
  release(&tickslock);
}

// for ext
extern int handle_ext_irq();

int devintr(uint64 scause) {
  if (scause == INTR_SOFT) { // k210 ext passby soft
    #ifdef K210
    // printf("caught software intr!\n");
    if(r_stval() == 9) {
      int ret;
      ret = handle_ext_irq();
      clear_csr(sip, SIP_SSIP);
      sbi_set_mext();
      return ret;
    }
    
    #endif
  } else if (scause == INTR_EXT) { // only qemu
    #ifdef QEMU
    if(handle_ext_irq() != 0) return -1;
    #endif
  } else if (scause == INTR_TIMER) {
    if(cpuid() == 0){
      // printf("clock...\n");
      // printf("time is: %X\n", READ_TIME());
      clockintr();
    }
    // printf("timer...%d\n", ticks);
    reset_timer();
    w_sip(r_sip() & ~0x10000);
  } else { // unknow
    return -1;
  }
  return 0;
}


