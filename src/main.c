#include "riscv.h"
#include "mm/vm.h"
#include "platform.h"
#include "kernel/sched.h"
#include "driver/plic.h"
#include "driver/console.h"
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

extern void devnull_init(void);
extern void devzero_init(void);

extern void scavenger_routine();
extern int  pdflush_init(void);
extern void trapinit(void);
extern void trapinithart(void);
extern char *logo;

void main() {
  if(cpuid() == 0){
    clear_bss();
    
    platform_early_init();
    platform_early_init_hart();

    printf("\n%s\n", logo);

    /* Process && CPU */
    procinit();      // process table & cpu state init

    /* Physical Memory */
    mem_init();      // pages & physical page allocator(buddy, slob) init

    /* Virtual Memory */
    kvminit();       // kernel page table init
    kvminithart();   // turn on paging

    /* Trap */
    trapinit();      // trap vectors
    trapinithart();  // install kernel trap vector

    /* PLIC */
    plic_init();          // set all sources' priorities to 0
    platform_plic_init(); // platform enable required source
    plic_init_hart();     // disable all interrupts and set ctx threshould to 0 for hart
    platform_plic_init_hart(); // enable required interrupts for current hart 

    /* Driver */
    platform_dirver_init();

    /* Console */
    consoleinit();

    /* File System */
    bcache_init();    // buffer cache(or block cache) init
    fs_init();        // pool of file entry in memory init

    userinit();       // first user process

    /* Kthread */
    pdflush_init();   // thread for page writeback
    kthread_create("scavenger", scavenger_routine); // thread for cleaning dangling trheads

    #ifdef K210
    for (int i = 1; i < NUM_CORES; i++) {
			unsigned long mask = 1 << i;
			sbi_send_ipi(mask, 0);
    }
    #endif

    __sync_synchronize();
    started = 1;

  } else {
    while(started == 0)
      ;
    __sync_synchronize();
    platform_early_init_hart();
    kvminithart();    // turn on paging
    trapinithart();   // install kernel trap vector
    plic_init_hart();   // ask PLIC for device interrupts
    platform_plic_init_hart();
  }
  printf("hart %d start\n", cpuid());
  scheduler();        
}
