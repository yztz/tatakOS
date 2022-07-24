#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "mm/vm.h"
#include "platform.h"
#include "kernel/proc.h"
#include "driver/plic.h"
#include "test.h"
#include "fs/blk_device.h"
#include "common.h"
#include "fs/file.h"
#include "fs/fs.h"

volatile static int started = 0;
_section(".kstack0") char stack0[4096 * NUM_CORES];

static inline void clear_bss() {
  extern char bss_start, bss_end;
  uint64_t len = (uint64_t)&bss_end - (uint64_t)&bss_start;
  memset(&bss_start, 0, len);
  printf("\n.bss %#lx-%#lx (%#lx) cleared\n", &bss_start, &bss_end, len);
}

void
main()
{
  if(cpuid() == 0){
    clear_bss();
    printf("\nOS TATAKAI!\n\n");
    platform_early_init();
    /* PRCO && CPU */
    procinit();      // process table
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
    /* DRIVER */
    platform_dirver_init(); // platform driver init
    /* CONSOLE */
    consoleinit(); // console
    /* DEV */
    devnull_init();
    devzero_init();
    // #ifdef K210
    // for(;;); // we haven't impl fs for K210, so spin here to avoid panic.
    // #endif
    /* FILE SYSTEM */
    binit();         // buffer cache
    fileinit();      // file table 
    fs_init();

    userinit();      // first user process

    for (int i = 1; i < NUM_CORES; i ++) {
			unsigned long mask = 1 << i;
			sbi_send_ipi(mask, 0);
		}

    __sync_synchronize();
    started = 1;
  } else {
    while(started == 0)
      ;
    __sync_synchronize();
    // extern uint64 ticks;
    while(1) {
      // uint64 a1 = ticks;
      // uint64 i = 0;
      // while(i++ < 100000000);
      // __sync_synchronize();
      // uint64 a2 = ticks;
      // printf("a1 is %ld a2 is %ld\n", a1, a2);
      // if(a1 == a2) panic("wuwuwu");
    }
    // kvminithart();    // turn on paging
    // trapinithart();   // install kernel trap vector
    // plic_init_hart();   // ask PLIC for device interrupts
    // platform_plic_init_hart();
  }
  scheduler();        
}
