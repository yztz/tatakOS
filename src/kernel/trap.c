#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "atomic/spinlock.h"
#include "kernel/proc.h"
#include "defs.h"
#include "utils.h"
#include "scause.h"
#include "mm/io.h"

static uint64_t *clint_mtime;
// #define RESET_TIMER() sbi_set_timer(*clint_mtime + CLOCK_FREQ)
#define READ_TIME() (*clint_mtime)
#define RESET_TIMER() sbi_legacy_set_timer(READ_TIME() + CLOCK_FREQ)
// #define RESET_TIMER() sbi_legacy_set_timer(read_csr(mtime) + CLOCK_FREQ)

struct spinlock tickslock;
uint64 ticks;

extern char trampoline[], uservec[], userret[];

// in kernelvec.S, calls kerneltrap().
void kernelvec();

extern int devintr();

extern pagetable_t kernel_pagetable;
void
trapinit(void)
{
  initlock(&tickslock, "time");
  // clint_mtime = (uint64_t *)(ioremap(CLINT_MTIME, 2 * PGSIZE));
  clint_mtime = (uint64_t *)(ioremap(CLINT, 0x10000) + 0Xbff8);
  ticks = 0;
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
  RESET_TIMER();
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
  if((r_sstatus() & SSTATUS_SPP) != 0)
    panic("usertrap: not from user mode");
  // send interrupts and exceptions to kerneltrap(),
  // since we're now in the kernel.
  // w_stvec((uint64)kernelvec);
  write_csr(stvec, (uint64)kernelvec);

  struct proc *p = myproc();

  // save user program counter.
  // p->trapframe->epc = r_sepc();
  p->trapframe->epc = read_csr(sepc);

  if (scause == EXCP_SYSCALL) {
    if(p->killed) {
      exit(-1);
    }
    // sepc points to the ecall instruction,
    // but we want to return to the next instruction.
    p->trapframe->epc += 4;
    // an interrupt will change sstatus &c registers,
    // so don't enable until done with those registers.
    intr_on();
    syscall();
  } else if(devintr(scause) == 0) {
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
void
usertrapret(void)
{
  struct proc *p = myproc();

  // we're about to switch the destination of traps from
  // kerneltrap() to usertrap(), so turn off interrupts until
  // we're back in user space, where usertrap() is correct.
  intr_off();
  // send syscalls, interrupts, and exceptions to trampoline.S
  w_stvec(TRAMPOLINE + (uservec - trampoline));

  // set up trapframe values that uservec will need when
  // the process next re-enters the kernel.
  // p->trapframe->kernel_satp = r_satp();         // kernel page table
  p->trapframe->kernel_sp = p->kstack + PGSIZE; // process's kernel stack
  p->trapframe->kernel_trap = (uint64)usertrap;
  p->trapframe->kernel_hartid = r_tp();         // hartid for cpuid()

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
  // uint64 satp = MAKE_SATP(p->pagetable);

  // jump to trampoline.S at the top of memory, which 
  // switches to the user page table, restores user registers,
  // and switches to user mode with sret.
  uint64 fn = TRAMPOLINE + (userret - trampoline);
  ((void (*)(uint64))fn)(TRAPFRAME);
}

// interrupts and exceptions from kernel code go here via kernelvec,
// on whatever the current kernel stack is.
void 
kerneltrap()
{
  uint64 sepc = r_sepc();
  uint64 sstatus = r_sstatus();
  uint64 scause = r_scause();

  if((sstatus & SSTATUS_SPP) == 0)
    panic("kerneltrap: not from supervisor mode");
  if(intr_get() != 0)
    panic("kerneltrap: interrupts enabled");

  // printf("scause %p\n", scause);
  // printf("sepc=%p stval=%p\n", r_sepc(), r_stval());
  if(devintr(scause) == -1) {
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
    if(handle_ext_irq() != 0) return -1;
    clear_csr(sip, SIP_SSIP);
    sbi_set_mext();
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
    RESET_TIMER();
    w_sip(r_sip() & ~0x10000);
  } else { // unknow
    return -1;
  }
  return 0;
}


/** @deprecated */
// check if it's an external interrupt or software interrupt,
// and handle it.
// returns 2 if timer interrupt,
// 1 if other device,
// 0 if not recognized.
// int
// old_devintr()
// {
//   uint64 scause = r_scause();

//   if((scause & 0x8000000000000000L) &&
//      (scause & 0xff) == 9){
//     // this is a supervisor external interrupt, via PLIC.

//     // irq indicates which device interrupted.
//     int irq = plic_claim();

//     if(irq == UART0_IRQ){
//       uartintr();
//     } else if(irq == VIRTIO0_IRQ){
//       virtio_disk_intr();
//     } else if(irq){
//       printf("unexpected interrupt irq=%d\n", irq);
//     }

//     // the PLIC allows each device to raise at most one
//     // interrupt at a time; tell the PLIC the device is
//     // now allowed to interrupt again.
//     if(irq)
//       plic_complete(irq);

//     return 1;
//   } 
//   // else if(scause == 0x8000000000000001L){
//   //   // software interrupt from a machine-mode timer interrupt,
//   //   // forwarded by timervec in kernelvec.S.

//   //   if(cpuid() == 0){
//   //     clockintr();
//   //   }
    
//   //   // acknowledge the software interrupt by clearing
//   //   // the SSIP bit in sip.
//   //   w_sip(r_sip() & ~2);

//   //   return 2;
//   // } 
//   else if(scause == 0x8000000000000005L){ // 时钟中断
//     if(cpuid() == 0){
//       clockintr();
//       RESET_TIMER();
//     }
//     w_sip(r_sip() & ~0x10000);
//     return 2;
//   } else {
//     return 0;
//   }
// }

