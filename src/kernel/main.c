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
    plic_init();     // set all sources' priorities to 0
    platform_plic_init(); // platform enable source as required
    plic_init_hart();  // disable all interrupts and set ctx threshould to 0 for hart
    platform_plic_init_hart(); // enable for current hart as required
    printf("plic init success!\n");
    /* DRIVER */
    platform_dirver_init(); // platform driver init
    /* CONSOLE */
    consoleinit(); // console
    printf("console init success!\n");
    #ifdef K210
    for(;;); // we haven't impl fs for K210, so spin here to avoid panic.
    #endif
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
