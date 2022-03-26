// #include "types.h"
// #include "param.h"
// #include "memlayout.h"
// #include "riscv.h"
// #include "defs.h"

// void main();

// entry.S needs one stack per CPU.

// a scratch area per CPU for machine-mode timer interrupts.
// uint64 timer_scratch[NCPU][5];

// assembly code in kernelvec.S for machine-mode timer interrupt.

// entry.S jumps here in machine mode on stack0.
// void
// start(int hartid, uint64 dtb_pa)
// {   
//   // disable paging for now.
//   // w_satp(0);

//   // 保存hartid
//   w_tp(hartid);
  

//   main();
// }
