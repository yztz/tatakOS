#ifndef _H_MM_
#define _H_MM_
#include "list.h"
#include "common.h"
// #include "mm/vm.h"
// #include "proc.h"

#define USERSPACE_END (0x80000000) // 2GB
#define PROGRAM_BREAK(mm) ((mm)->uheap->addr + (mm)->uheap->addr)


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
};

#define VMA_NUM 16
/* 用于描述特定一段内存区域（段） */
struct vma{
  uint64 addr;
  uint64 len;
  uint64_t pa;
  uint offset; /* 目前用来记录申请时给定地址与其所在页面的偏移量 */

  int flags;
  int prot;

  list_head_t head; /* 用于串联VMA结构 */

  struct file *map_file;
};

typedef struct vma vma_t;

// flags
#define MAP_ANONYMOUS (1L << 31) // 匿名，为0表示FILE
#define MAP_SHARED    (1L << 30) // 共享，为0表示私有
#define MAP_STACK (1L << 29) // 表示向下扩展的栈
// prot(与pte同)
#define MAP_PROT_READ (PTE_R)
#define MAP_PROT_WRITE (PTE_W)
#define MAP_PROT_EXEC (PTE_X)
#define MAP_PROT_USER (PTE_U)

/* 描述进程的内存段 */
struct mmlayout
{
    uint64_t kstack; /* 内核栈 */
    uint64_t trapframe; /* 陷入栈帧 */

    vma_t *ustack; /* 用户栈 */
    vma_t *uheap;  /* 用户堆 */

    list_head_t vma_head; /* 其他内存区域 */

    pagetable_t pagetable;       // User page table
    // proc_t *owner;
};

typedef struct mmlayout mm_t;


int do_mmap(mm_t *mm, struct file *fp, uint64_t addr, uint64_t len, int flags, int prot);
int do_mmap_alloc(mm_t *mm, struct file *fp, uint64_t addr, uint64_t len, int flags, int prot);
void do_unmap(mm_t *mm, uint64_t addr, int do_free);
int mmap_init(mm_t *mm, mm_t *oldmm);
void mmap_free(mm_t **pmm);
vma_t *vma_find(mm_t *mm, uint64 addr);
vma_t *vma_exist(mm_t *mm, uint64_t addr, uint64_t len);
struct trapframe *get_trapframe(mm_t *mm);
uint64_t get_kstack(mm_t *mm);
int mmap_dup(mm_t *newm, mm_t *oldm);
int mmap_ext_heap(mm_t *mm, int newsize);
vma_t *__vma_find_strict(mm_t *mm, uint64 addr);
void mmap_print_vmas(mm_t *mm);
void mmap_print_vma(vma_t *vma);

void switchuvm(mm_t *mm);
void switchkvm();

#endif