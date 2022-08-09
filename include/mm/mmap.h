#ifndef _H_MM_
#define _H_MM_
#include "list.h"
#include "common.h"
#include "mm/page.h"
#include "atomic/spinlock.h"

#define PROGRAM_BREAK(mm) ((mm)->uheap->addr + (mm)->uheap->len)

#define PAGECACHE_TAG_DIRTY 0
#define PAGECACHE_TAG_WRITEBACK 1

/* 用于描述特定一段内存区域（段） */
struct vma{
  uint64 addr;
  uint64 len;
  uint64_t pa;
  off_t offset;

  uint flags;
  uint prot;

  uint64 raddr;
  list_head_t head; /* 用于串联VMA结构 */

  struct file *map_file;
};

typedef struct vma vma_t;

// flags
#define MAP_SHARED     0x01
#define MAP_PRIVATE    0x02
#define MAP_SHARED_VALIDATE 0x03
#define MAP_TYPE       0x0f
#define MAP_FIXED      0x10
#define MAP_ANON       0x20
#define MAP_ANONYMOUS  MAP_ANON
#define MAP_NORESERVE  0x4000
#define MAP_GROWSDOWN  0x0100
#define MAP_DENYWRITE  0x0800
#define MAP_EXECUTABLE 0x1000
#define MAP_LOCKED     0x2000
#define MAP_POPULATE   0x8000
#define MAP_NONBLOCK   0x10000
#define MAP_STACK      0x20000
#define MAP_HUGETLB    0x40000
#define MAP_SYNC       0x80000
#define MAP_FIXED_NOREPLACE 0x100000
#define MAP_FILE       0
// prot
#define PROT_NONE      0
#define PROT_READ      1
#define PROT_WRITE     2
#define PROT_EXEC      4
#define PROT_USER      8
#define PROT_COW     16

/* 描述进程的内存段 */
struct mmlayout
{
    int ref;
    spinlock_t mm_lock;
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

// int mmap_init(mm_t *mm);
mm_t *mmap_new();
void mmap_free(mm_t **pmm);
uint64_t do_mmap(mm_t *mm, struct file *fp, off_t off, uint64_t addr, uint64_t len, int flags, int prot);
uint64_t do_mmap_alloc(mm_t *mm, uint64_t addr, uint64_t len, int flags, int prot);
void do_unmap(mm_t *mm, uint64_t addr, int do_free);
vma_t *vma_find(mm_t *mm, uint64 addr);
vma_t *vma_exist(mm_t *mm, uint64_t addr, uint64_t len);

int mmap_ext_heap(mm_t *mm, uint64_t newsize);
int mmap_ext_stack(mm_t *mm, uint64_t newsize);
vma_t *__vma_find_strict(mm_t *mm, uint64 addr);
int mmap_map_stack(mm_t *mm, uint64_t stacksize);

mm_t *mmap_clone(mm_t *mm);
void mmap_print(mm_t *mm);

void mmap_ref(mm_t *self);
void mmap_deref(mm_t *self);

void switchuvm(mm_t *mm);
void switchkvm();

#endif