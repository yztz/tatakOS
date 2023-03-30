/*
                    Sv39 Virtual Address
+--------+-------------+-------------+-------------+--------+
| (zero) | level-2 idx | level-1 idx | level-0 idx | offset |
| 63..39 |   38..30    |   29..21    |    20..12   |  11..0 |
+--------+-------------+-------------+-------------+--------+
zero: We don't use bit 39 so that bits 63-40 must be same with bit 39(zero).
*/

#ifndef _H_PAGE_
#define _H_PAGE_

#include "bitops.h"
#include "list.h"
#include "platform.h"
#include "atomic/spinlock.h"
#include "atomic/atomic.h"
#include "page-flags.h"

/* Use 38 in sv39 to avoid sign-extend ref: riscv-privileged-20211203 p84 */
#define MAXVA (1L << (9 + 9 + 9 + 12 - 1))
/* Equal with page level */
#define PGSPEC_NORMAL 0
#define PGSPEC_LARGE  1
#define PGSPEC_SUPER  2 // unused

/* PGSIZE */
#define PGSIZE_SPEC(spec) (1UL << (12 + 9 * (spec)))
#define PGSIZE PGSIZE_SPEC(PGSPEC_NORMAL) // 4KB 2^12
#define PGSIZE_LARGE PGSIZE_SPEC(PGSPEC_LARGE) // 2MB 2^21

/* PGMASK */
#define PGMASK_SPEC(spec) (~(PGSIZE_SPEC(spec) - 1))
#define PGMASK PGMASK_SPEC(PGSPEC_NORMAL)

/* Kinds of round */
#define PGROUNDUP_SPEC(sz, sepc) ((((uint64_t)sz)+(PGSIZE_SPEC(sepc))-1) & ~((PGSIZE_SPEC(sepc))-1))
#define PGROUNDDOWN_SPEC(sz, sepc) (((uint64_t)sz) & ~((PGSIZE_SPEC(sepc))-1))

#define PGROUNDUP(sz)  PGROUNDUP_SPEC((uint64_t)sz, PGSPEC_NORMAL)
#define PGROUNDDOWN(sz) PGROUNDDOWN_SPEC((uint64_t)sz, PGSPEC_NORMAL)

#define PGROUNDUP_LARGE(sz) PGROUNDUP_SPEC(sz, PGSPEC_LARGE)
#define PGROUNDDOWN_LARGE(sz) PGROUNDDOWN_SPEC(sz, PGSPEC_LARGE)

/* Calculate page numbers (round-up) */
#define ROUND_COUNT_SPEC(sz, spec) (PGROUNDUP_SPEC(sz, spec) / PGSIZE_SPEC(spec))
#define ROUND_COUNT(sz) ROUND_COUNT_SPEC(sz, PGSPEC_NORMAL)
#define ROUND_COUNT_LARGE(sz) ROUND_COUNT_SPEC(sz, PGSPEC_LARGE)

/* Shift a physical address to the right place for a PTE. */
#define PA2PTE_SPEC(pa, level) (((((uint64_t)pa) >> (12 + (level) * 9 ))) << (10 + (level) * 9))
#define PA2PTE(pa) PA2PTE_SPEC(pa, PGSPEC_NORMAL)

#define PTE2PA(pte) (((pte) >> 10) << 12)

#define PGSHIFT 12  // bits of offset within a page
/* Extract the three 9-bit page table indices from a virtual address. */
#define PXMASK          0x1FF // 9 bits
#define PXSHIFT(level)  (PGSHIFT+(9*(level)))
#define PX(level, va) ((((uint64_t) (va)) >> PXSHIFT(level)) & PXMASK)

#define PTE_FLAGS(pte) ((pte) & 0x3FF)
#define PTE_V (1L << 0) // valid
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)
#define PTE_U (1L << 4) // 1 -> user can access
#define PTE_COW (1L << 8) // cow


typedef uint8_t pgref_t;

struct page_t {
    /// @brief reference count
    atomic_t refcnt;        
    /// @brief allocator-specific fields
    union {
        /// @brief buddy info
        struct {
            /// @brief max 14 (15 is used as invaild)
            uint8_t order : 4;
            /// @brief is page allocated?
            uint8_t alloc : 2;
            /// @brief reserved
            uint8_t resv  : 2;
        };
        /// @brief freelist info
        struct {
            /// @brief consecutive page nums
            uint8_t pgnum;
        };
    };
    /// @brief page flags
    uint64_t flags;
    /// @brief active/inactive list
    list_head_t lru;
    /// @brief pagecache belongs to
    address_space_t *mapping;
    /// @brief pagecache index
    uint32_t index;
};

/* 页的数量 */
#define PAGE_NUMS ((MEM_END - KERN_BASE)/PGSIZE)

extern page_t pages[PAGE_NUMS];

/* 页指针--->页号 */
#define PG_TO_NR(pa) (((uint64_t)(pa) - KERN_BASE) / PGSIZE)
/* 页号--->地址 */
#define NR_TO_ADDR(num) ((uint64_t)((num) * PGSIZE + KERN_BASE))
/* 页指针--->地址 */
#define PG_TO_ADDR(page) NR_TO_ADDR(page-pages)
/* 地址--->页指针 */
#define ADDR_TO_PG(pa) &pages[PG_TO_NR(pa)]

#define PAGE_CACHE_SHIFT PGSHIFT
#define PAGE_CACHE_SIZE PGSIZE

#define PAGE_SHIFT PGSHIFT

/* page fault 使用的位 */
#define pte_none(pte)           (!pte)
#define pte_valid(pte)          (pte & PTE_V) 
#define pte_write(pte)          (pte & PTE_W)

pgref_t __get_page_pointer(page_t *page);
pgref_t __get_page_paddr(uint64_t pa);
#define get_page(param) _Generic((param), uint64_t: __get_page_paddr, page_t *: __get_page_pointer)(param)  

pgref_t __put_page_pointer(page_t *page);
pgref_t __put_page_padder(uint64_t pa);
#define put_page(param) _Generic((param), uint64_t: __put_page_padder, page_t *: __put_page_pointer)(param)  

pgref_t __page_refcnt_pointer(page_t *page);
pgref_t __page_refcnt_paddr(uint64_t pa);
#define page_refcnt(param) _Generic((param), uint64_t: __page_refcnt_paddr, page_t *: __page_refcnt_pointer)(param)  
 
pgref_t __deref_page_pointer(page_t *page);
pgref_t __deref_page_paddr(uint64_t pa);
#define put_page_nofree(param) _Generic((param), uint64_t: __deref_page_paddr, page_t *: __deref_page_pointer)(param)   

void page_init(void);

void pte_print(pte_t *pte);

void lock_page(page_t *page);
void unlock_page(page_t *page);

void set_page_dirty(uint64_t pa);
void clear_page_dirty(uint64_t pa);
int  page_is_dirty(uint64_t pa);

void unlock_put_page(page_t *page);
void get_lock_page(page_t *page);
void print_page_info(page_t* page);
void print_not_freed_pages();
void reset_page(page_t *page);

/**
 * @brief Create PTEs for virtual addresses starting at va that refer to
 *        physical addresses starting at pa. va and size might not
 *        be page-aligned. Returns 0 on success, -1 if walk() couldn't
 *        allocate a needed page-table page.
 * 
 * @param pagetable
 * @param va vitrual address
 * @param size 
 * @param pa physical address
 * @param prot protection
 * @param spec page specification
 * @return int 
 */
int __map_pages(pagetable_t pagetable, uint64_t va, uint64_t size, uint64_t pa, int prot, int spec);
/**
 * @brief Remove mappings starting from va. va must be
 *        page-aligned. The mappings must exist.
 *        Optionally free the physical memory.
 * 
 * @param pagetable 
 * @param va 
 * @param size 
 * @param do_free free the memory
 * @param spec page specification
 */
void __unmap_pages(pagetable_t pagetable, uint64_t va, uint64_t size, int do_free, int spec);


/**
 * @brief Recursively free page-table pages.
 * All leaf mappings must already have been removed.
 * 
 * @param pagetable 
 */
void freewalk(pagetable_t pagetable);

pte_t *__walk(pagetable_t pagetable, uint64_t va, int alloc, int pg_spec);

static inline pte_t *walk(pagetable_t pagetable, uint64_t va, int alloc) {
    return __walk(pagetable, va, alloc, PGSPEC_NORMAL);
}


/* page frame reclaiming 页回收算法需要的数据结构 */

#ifdef RMAP
/*
 * Return true if this page is mapped into pagetables.
 */
static inline int page_mapped(page_t *page) {
    return page->pte.direct != 0;
}
#endif

#define page_spin_lock(page) bit_spin_lock(PG_spinlock, &(page->flags))
#define page_spin_unlock(page) bit_spin_unlock(PG_spinlock, &(page->flags))


#endif