#include "common.h"
#include "platform.h"
#include "kernel/sched.h"

volatile static int started = 0;
_section(".kstack0") char stack0[4096 * NUM_CORES];

static inline void clear_bss() {
  extern char bss_start, bss_end;
  uint64_t len = (uint64_t)&bss_end - (uint64_t)&bss_start;
  memset(&bss_start, 0, len);
  kprintf("\n.bss %#lx-%#lx (%#lx) cleared\n", &bss_start, &bss_end, len);
}

static inline void check_bad_core() {
    extern int bad_core;
    if (bad_core) {
        panic("CPUS change detected. Please clean and remake.");
    }
}

extern void bcache_init();
extern void console_init();
extern void fs_init();
extern void kvm_init();
extern void kvm_init_hart();
extern void mem_init();
extern void pdflush_init();
extern void plic_init();
extern void plic_init_hart();
extern void process_init();
extern void scavenger_init();
extern void trap_init();
extern void trap_init_hart();
extern void user0_init();

extern char *logo;

void main() {
    if (cpuid() == 0) {
        clear_bss();

        cpu_init();
        
        platform_early_init();
        platform_early_init_hart();

        kprintf("\n%s\n", logo);

        check_bad_core();

        /* Process */
        process_init();

        /* Physical Memory */
        mem_init();      // pages & physical page allocator(buddy, slob) init

        /* Virtual Memory */
        kvm_init();       // kernel page table init
        kvm_init_hart();   // turn on paging

/* !Note: output is unavailable here */
        /* Trap */
        trap_init();      // trap vectors
        trap_init_hart();  // install kernel trap vector

        /* PLIC */
        plic_init();          // set all sources' priorities to 0
        platform_plic_init(); // platform enable required source
        plic_init_hart();     // disable all interrupts and set ctx threshould to 0 for hart
        platform_plic_init_hart(); // enable required interrupts for current hart 
/* !Note: output is available here */

        /* Driver */
        platform_dirver_init();

        /* Console */
        console_init();

        /* File System */
        bcache_init();    // buffer cache(or block cache) init
        fs_init();        // pool of file entry in memory init

        pdflush_init();   // thread for page writeback
        scavenger_init(); // for recycling threads

        user0_init();     // first user process

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
        kvm_init_hart();      // turn on paging
        trap_init_hart();     // install kernel trap vector
        plic_init_hart();   // disable all interrupts and set ctx threshould to 0 for hart
        platform_plic_init_hart();
    }

    kprintf("hart %d start\n", cpuid());
    
    scheduler();  // start scheduler
}
