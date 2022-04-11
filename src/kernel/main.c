#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "platform.h"
#include "driver/plic.h"

volatile static int started = 0;
__attribute__ ((aligned (16))) char stack0[4096 * NUM_CORES];

// start() jumps here in supervisor mode on all CPUs.

#ifdef K210
extern void sd();
void ext_test() {
  uint64 scause = r_scause();
  printf("caught trap!\n");
  printf("scause is: %#lX", scause);
  for(;;);
}
#endif

void
main()
{
  if(cpuid() == 0){
    kinit();         // physical page allocator

    #ifdef K210
    sd();
    printf("====begin ext test====\n");
    printf("sie: %#b\n", r_sie());
    w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);
    printf("sie: %#b\n", r_sie());
    // uint64 sip = r_sip();
    // printf("sip: %#b\n", sip);
    // w_sip(sip | SIE_SSIE);
    // printf("sip: %#b\n", sip);
    // 设置中断向量
    w_stvec((uint64)ext_test);
    // 使能中断
    w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);
    // 模拟触发外部中断
    sbi_emulate_ext((uint64)ext_test);
    // sbi_putchar('A');
    printf("emmmm....\n");
    intr_on();
    for(;;);
    #endif

    // printfinit();
    printf("\n");
    printf("xv6 kernel is booting\n");
    printf("\n");
    // kinit();         // physical page allocator
    kvminit();       // create kernel page table
    kvminithart();   // turn on paging
    procinit();      // process table
    trapinit();      // trap vectors
    trapinithart();  // install kernel trap vector
    plic_init();      // set up interrupt controller
    platform_plic_init();
    plic_init_hart();  // ask PLIC for device interrupts
    platform_plic_init_hart();
    printf("SIE: %d\n", intr_get());
    consoleinit(); // console
    printf("SIE: %d\n", intr_get());
    printf("console init done\n");
    platform_dirver_init(); // driver
    printf("driver init done\n");
    binit();         // buffer cache
    printf("binit done\n");
    iinit();         // inode table
    printf("iinit done\n");
    fileinit();      // file table
    printf("fileinit done\n");
    userinit();      // first user process
    printf("user init done sie: %d\n",intr_get());
    printf("SIE: %d\n", intr_get());
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
