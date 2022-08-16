#ifndef _H_TRAPFRAME_
#define _H_TRAPFRAME_

#include "types.h"


struct k_trapframe {
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

struct trapframe {
  /*   0 @depercated */ uint64 kernel_satp;   // kernel page table 
  /*   8 */ uint64 kernel_sp;     // top of process's kernel stack
  /*  16 */ uint64 kernel_trap;   // usertrap()
  /*  24 */ uint64 epc;           // saved user program counter
  /*  32 */ uint64 kernel_hartid; // saved kernel tp
  /*  40 */ uint64 ra;
  /*  48 */ uint64 sp;
  /*  56 */ uint64 gp;
  /*  64 */ uint64 tp;
  /*  72 */ uint64 t0;
  /*  80 */ uint64 t1;
  /*  88 */ uint64 t2;
  /*  96 */ uint64 s0;
  /* 104 */ uint64 s1;
  /* 112 */ uint64 a0;
  /* 120 */ uint64 a1;
  /* 128 */ uint64 a2;
  /* 136 */ uint64 a3;
  /* 144 */ uint64 a4;
  /* 152 */ uint64 a5;
  /* 160 */ uint64 a6;
  /* 168 */ uint64 a7;
  /* 176 */ uint64 s2;
  /* 184 */ uint64 s3;
  /* 192 */ uint64 s4;
  /* 200 */ uint64 s5;
  /* 208 */ uint64 s6;
  /* 216 */ uint64 s7;
  /* 224 */ uint64 s8;
  /* 232 */ uint64 s9;
  /* 240 */ uint64 s10;
  /* 248 */ uint64 s11;
  /* 256 */ uint64 t3;
  /* 264 */ uint64 t4;
  /* 272 */ uint64 t5;
  /* 280 */ uint64 t6;
  char resv[8];
// 也许我们可以选择性地保存
  /* 296 */ uint64 f0;
  /* 304 */ uint64 f1;
  /* 312 */ uint64 f2;
  /* 320 */ uint64 f3;
  /* 328 */ uint64 f4;
  /* 336 */ uint64 f5;
  /* 344 */ uint64 f6;
  /* 352 */ uint64 f7;
  /* 360 */ uint64 f8;
  /* 368 */ uint64 f9;
  /* 376 */ uint64 f10;
  /* 384 */ uint64 f11;
  /* 392 */ uint64 f12;
  /* 400 */ uint64 f13;
  /* 408 */ uint64 f14;
  /* 416 */ uint64 f15;
  /* 424 */ uint64 f16;
  /* 432 */ uint64 f17;
  /* 440 */ uint64 f18;
  /* 448 */ uint64 f19;
  /* 456 */ uint64 f20;
  /* 464 */ uint64 f21;
  /* 472 */ uint64 f22;
  /* 480 */ uint64 f23;
  /* 488 */ uint64 f24;
  /* 496 */ uint64 f25;
  /* 504 */ uint64 f26;
  /* 512 */ uint64 f27;
  /* 520 */ uint64 f28;
  /* 528 */ uint64 f29;
  /* 536 */ uint64 f30;
  /* 544 */ uint64 f31;
  /* 552 */ uint64 fcsr;
  struct trapframe *sigtf;
};

typedef struct trapframe tf_t;
typedef struct k_trapframe ktf_t;

tf_t *tf_new();
void tf_free(tf_t **pptf);
void tf_backup(tf_t *self);
void tf_restore(tf_t *self);
void tf_reset(tf_t *self, uint64_t pc, uint64_t sp);
void tf_print(tf_t *tf);
void tf_flstore(tf_t *self);
void tf_flrestore(tf_t *self);

#endif