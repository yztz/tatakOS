#ifndef _H_MM_
#define _H_MM_

#include "types.h"
#include "list.h"
#include "mm/page.h"
#include "atomic/spinlock.h"

#define PROGRAM_BREAK(mm) ((mm)->uheap->addr + (mm)->uheap->len)

#define PAGECACHE_TAG_DIRTY 0
#define PAGECACHE_TAG_WRITEBACK 1

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


/// @brief Process memory area (consecutive, page-aligned)
struct vma {
    /// @brief map address, page-aligned
    uint64 addr;
    /// @brief real map address
    uint64 raddr;
    /// @brief map length, page-aligned
    uint64 len;
    /// @brief real map length, <= len
    uint64 rlen;
    /// @brief page index offset, in pages
    off_t offset;
    /// @brief map flags
    uint flags;
    /// @brief map protection flags
    uint prot;
    /// @brief vma list head
    list_head_t head;
    /// @brief map file
    struct file *map_file;
    /// @brief vma belongs to
    mm_t *mm;
};


/// @brief Process memory map
struct mmlayout {
    /// @brief 
    int ref;
    /// @brief 
    spinlock_t mm_lock;
    /// @brief user stack
    vma_t *ustack;
    /// @brief user heap
    vma_t *uheap;
    /// @brief vma list
    list_head_t vma_head;
    /// @brief user pagetable
    pagetable_t pagetable;
};



vma_t *vma_new(mm_t *mm,
        struct file *fp, 
        off_t foff, 
        uint64_t addr, 
        uint64_t len, 
        int flags, 
        int prot);
void vma_print(vma_t *vma);
void vma_free(vma_t **vma);
vma_t *vma_clone(mm_t *newmm, vma_t *vma);

mm_t *mmap_new();
void mmap_free(mm_t **pmm);

/**
 * @brief Map a memory area with specific flags and prot.
 * @note Mapped memory regions are lazily allocated,
 *       in other words, not immediately allocated.
 *
 * @param mm
 * @param fp file pointer that specifies this memory segment as a file map, NULL else
 * @param off file offset, valid only after a file pointer has been passed
 * @param addr map virtual address. If NULL, the system automatically specifies the value.
 * @param len map length
 * @param flags map flags
 * @param prot  protection flags
 * @return uint64_t map address
 */
uint64_t mmap_map(mm_t *mm, struct file *fp, off_t off, uint64_t addr, uint64_t len, int flags, int prot);
uint64_t mmap_map_alloc(mm_t *mm, uint64_t addr, uint64_t len, int flags, int prot);
void mmap_unmap(mm_t *mm, uint64_t addr);
vma_t *vma_exist(mm_t *mm, uint64_t addr, uint64_t len);

int mmap_ext_heap(mm_t *mm, uint64_t newsize);
int mmap_ext_stack(mm_t *mm, uint64_t newsize);

/**
 * @brief Find the **first** vma whose **end** address is greater than addr.
 * 
 * @param mm 
 * @param addr 
 * @return vma_t* 
 */
vma_t *__vma_find_greater(mm_t *mm, uint64 addr);


/**
 * @brief Find the **last** vma whose **start** address is less than addr.
 * 
 * @param mm 
 * @param addr 
 * @return vma_t* 
 */
vma_t *__vma_find_less(mm_t *mm, uint64 addr);


/**
 * @brief Just like __vma_find_greater but require 
 *        **start** address is less than or equal to addr.
 * 
 * @param mm 
 * @param addr 
 * @return vma_t* 
 */
vma_t *__vma_find_strict(mm_t *mm, uint64 addr);
int mmap_map_stack(mm_t *mm, uint64_t stacksize);

mm_t *mmap_clone(mm_t *mm);
void mmap_print(mm_t *mm);

void mmap_ref(mm_t *self);
void mmap_deref(mm_t *self);

void switchuvm(mm_t *mm);
void switchkvm();

#endif
