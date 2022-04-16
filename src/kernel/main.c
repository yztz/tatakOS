#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "mm.h"
#include "platform.h"
#include "driver/plic.h"

volatile static int started = 0;
__attribute__ ((aligned (16))) char stack0[4096 * NUM_CORES];

// start() jumps here in supervisor mode on all CPUs.

#ifdef K210
extern void sd();
#endif

void
main()
{
  if(cpuid() == 0){
    printf("\nOS TATAKAI!\n\n");
    /* PM */
    kinit();         // physical page allocator
    /* VM */
    kvminit();       // create kernel page table
    kvminithart();   // turn on paging
    /* TRAP */
    trapinit();      // trap vectors
    trapinithart();  // install kernel trap vector
    /* PLIC */
    plic_init();      // set up interrupt controller
    platform_plic_init();
    plic_init_hart();  // ask PLIC for device interrupts
    platform_plic_init_hart();
    printf("plic init success!\n");
    /* DRIVER */
    platform_dirver_init(); // driver
    /* CONSOLE */
    consoleinit(); // console
    printf("console init success!\n");

    procinit();      // process table
    /* FILE SYSTEM */
    binit();         // buffer cache
    iinit();         // inode table
    fileinit();      // file table

    userinit();      // first user process
    __sync_synchronize();
    started = 1;
  } else {
    while(started == 0)
      ;
    __sync_synchronize();
    printf("hart %d starting\n", cpuid());
    kvminithart();    // turn on paging
    trapinithart();   // install kernel trap vector
    plic_init_hart();   // ask PLIC for device interrupts
    platform_plic_init_hart();
  }

  scheduler();        
}
