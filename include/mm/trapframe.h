#ifndef _H_TRAPFRAME_
#define _H_TRAPFRAME_

#include "types.h"


struct ktrapframe {
    uint64_t ra;
    uint64_t sp;
    uint64_t gp;
    uint64_t tp;
    uint64_t t0;
    uint64_t t1;
    uint64_t t2;
    uint64_t s0;
    uint64_t s1;
    uint64_t a0;
    uint64_t a1;
    uint64_t a2;
    uint64_t a3;
    uint64_t a4;
    uint64_t a5;
    uint64_t a6;
    uint64_t a7;
    uint64_t s2;
    uint64_t s3;
    uint64_t s4;
    uint64_t s5;
    uint64_t s6;
    uint64_t s7;
    uint64_t s8;
    uint64_t s9;
    uint64_t s10;
    uint64_t s11;
    uint64_t t3;
    uint64_t t4;
    uint64_t t5;
    uint64_t t6;
};

struct utrapframe {
  /*   0 */ uint64_t proc;
  /*   8 */ uint64_t kernel_sp;     ///< top of process's kernel stack
  /*  16 */ uint64_t kernel_trap;   ///< usertrap()
  /*  24 */ uint64_t epc;           ///< saved user program counter
  /*  32 */ uint64_t kernel_hartid; ///< saved kernel tp
  /*  40 */ uint64_t ra;
  /*  48 */ uint64_t sp;
  /*  56 */ uint64_t gp;
  /*  64 */ uint64_t tp;
  /*  72 */ uint64_t t0;
  /*  80 */ uint64_t t1;
  /*  88 */ uint64_t t2;
  /*  96 */ uint64_t s0;
  /* 104 */ uint64_t s1;
  /* 112 */ uint64_t a0;
  /* 120 */ uint64_t a1;
  /* 128 */ uint64_t a2;
  /* 136 */ uint64_t a3;
  /* 144 */ uint64_t a4;
  /* 152 */ uint64_t a5;
  /* 160 */ uint64_t a6;
  /* 168 */ uint64_t a7;
  /* 176 */ uint64_t s2;
  /* 184 */ uint64_t s3;
  /* 192 */ uint64_t s4;
  /* 200 */ uint64_t s5;
  /* 208 */ uint64_t s6;
  /* 216 */ uint64_t s7;
  /* 224 */ uint64_t s8;
  /* 232 */ uint64_t s9;
  /* 240 */ uint64_t s10;
  /* 248 */ uint64_t s11;
  /* 256 */ uint64_t t3;
  /* 264 */ uint64_t t4;
  /* 272 */ uint64_t t5;
  /* 280 */ uint64_t t6;
  char resv[8];
/* float registers below*/
  /* 296 */ uint64_t f0;
  /* 304 */ uint64_t f1;
  /* 312 */ uint64_t f2;
  /* 320 */ uint64_t f3;
  /* 328 */ uint64_t f4;
  /* 336 */ uint64_t f5;
  /* 344 */ uint64_t f6;
  /* 352 */ uint64_t f7;
  /* 360 */ uint64_t f8;
  /* 368 */ uint64_t f9;
  /* 376 */ uint64_t f10;
  /* 384 */ uint64_t f11;
  /* 392 */ uint64_t f12;
  /* 400 */ uint64_t f13;
  /* 408 */ uint64_t f14;
  /* 416 */ uint64_t f15;
  /* 424 */ uint64_t f16;
  /* 432 */ uint64_t f17;
  /* 440 */ uint64_t f18;
  /* 448 */ uint64_t f19;
  /* 456 */ uint64_t f20;
  /* 464 */ uint64_t f21;
  /* 472 */ uint64_t f22;
  /* 480 */ uint64_t f23;
  /* 488 */ uint64_t f24;
  /* 496 */ uint64_t f25;
  /* 504 */ uint64_t f26;
  /* 512 */ uint64_t f27;
  /* 520 */ uint64_t f28;
  /* 528 */ uint64_t f29;
  /* 536 */ uint64_t f30;
  /* 544 */ uint64_t f31;
  /* 552 */ uint64_t fcsr;
  struct utrapframe *sigtf; ///< save copy when handling signals
};


utf_t *tf_new(struct proc *p);
utf_t *tf_new_clone(struct proc *p, utf_t *old);
void tf_free(utf_t **pptf);
void tf_backup(utf_t *self);
void tf_restore(utf_t *self);
void tf_reset(utf_t *self, uint64_t pc, uint64_t sp);
void tf_print(utf_t *tf);
void tf_flstore(utf_t *self);
void tf_flrestore(utf_t *self);

#endif