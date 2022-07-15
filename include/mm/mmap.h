#ifndef _H_MM_
#define _H_MM_
#include "list.h"
#include "common.h"
#include "mm/page.h"

#define USERSPACE_END (0x80000000) // 2GB
#define PROGRAM_BREAK(mm) ((mm)->uheap->addr + (mm)->uheap->len)

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
    // uint64_t kstack; /* 内核栈 */
    // uint64_t trapframe; /* 陷入栈帧 */

    vma_t *ustack; /* 用户栈 */
    vma_t *uheap;  /* 用户堆 */

    list_head_t vma_head; /* 其他内存区域 */

    pagetable_t pagetable;       // User page table
    // proc_t *owner;
};

typedef struct mmlayout mm_t;


vma_t *vma_new();
void vma_print(vma_t *vma);
void vma_free(vma_t **vma);

int mmap_init(mm_t *mm);
void mmap_free(mm_t **pmm);
uint64_t do_mmap(mm_t *mm, struct file *fp, uint64_t addr, uint64_t len, int flags, int prot);
uint64_t do_mmap_alloc(mm_t *mm, struct file *fp, uint64_t addr, uint64_t len, int flags, int prot);
void do_unmap(mm_t *mm, uint64_t addr, int do_free);
vma_t *vma_find(mm_t *mm, uint64 addr);
vma_t *vma_exist(mm_t *mm, uint64_t addr, uint64_t len);
int mmap_dup(mm_t *newm, mm_t *oldm);
int mmap_ext_heap(mm_t *mm, uint64_t newsize);
int mmap_ext_stack(mm_t *mm, uint64_t newsize);
vma_t *__vma_find_strict(mm_t *mm, uint64 addr);
int mmap_map_stack_heap(mm_t *mm, uint64_t stacksize, uint64_t heapsize);
void mmap_print(mm_t *mm);

void switchuvm(mm_t *mm);
void switchkvm();

#endif