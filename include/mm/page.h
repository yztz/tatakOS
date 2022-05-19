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

/* use 38 in sv39 to avoid sign-extend ref: riscv-privileged-20211203 p84 */
#define MAXVA (1L << (9 + 9 + 9 + 12 - 1))
/* Equal with page level */
#define PGSPEC_NORMAL 0
#define PGSPEC_LARGE  1
#define PGSPEC_SUPER  2 // unused

/* PGSIZE */
#define PGSIZE_SPEC(spec) (1L << (12 + 9 * (spec)))
#define PGSIZE PGSIZE_SPEC(PGSPEC_NORMAL) // 4KB 2^12
#define PGSIZE_LARGE PGSIZE_SPEC(PGSPEC_LARGE) // 2MB 2^21

/* PGMASK */
#define PGMASK_SPEC(spec) (~(PGSIZE_SPEC(spec) - 1))
#define PGMASK PGMASK_SPEC(PGSPEC_NORMAL)

/* Kinds of round */
#define PGROUNDUP_SPEC(sz, sepc) (((sz)+(PGSIZE_SPEC(sepc))-1) & ~((PGSIZE_SPEC(sepc))-1))
#define PGROUNDDOWN_SPEC(sz, sepc) ((sz) & ~((PGSIZE_SPEC(sepc))-1))

#define PGROUNDUP(sz)  PGROUNDUP_SPEC((uint64_t)sz, PGSPEC_NORMAL)
#define PGROUNDDOWN(sz) PGROUNDDOWN_SPEC((uint64_t)sz, PGSPEC_NORMAL)

#define PGROUNDUP_LARGE(sz) PGROUNDUP_SPEC(sz, PGSPEC_LARGE)
#define PGROUNDDOWN_LARGE(sz) PGROUNDDOWN_SPEC(sz, PGSPEC_LARGE)

/* Calculate page numbers (round-up) */
#define ROUND_COUNT_SPEC(sz, spec) (PGROUNDUP_SPEC(sz, spec) / PGSIZE_SPEC(spec))
#define ROUND_COUNT(sz) ROUND_COUNT_SPEC(sz, PGSPEC_NORMAL)
#define ROUND_COUNT_LARGE(sz) ROUND_COUNT_SPEC(sz, PGSPEC_LARGE)

/* Shift a physical address to the right place for a PTE. */
#define PA2PTE_SPEC(pa, level) (((((uint64)pa) >> (12 + (level) * 9 ))) << (10 + (level) * 9))
#define PA2PTE(pa) PA2PTE_SPEC(pa, PGSPEC_NORMAL)

#define PTE2PA(pte) (((pte) >> 10) << 12)

#define PGSHIFT 12  // bits of offset within a page
/* Extract the three 9-bit page table indices from a virtual address. */
#define PXMASK          0x1FF // 9 bits
#define PXSHIFT(level)  (PGSHIFT+(9*(level)))
#define PX(level, va) ((((uint64) (va)) >> PXSHIFT(level)) & PXMASK)

#define PTE_FLAGS(pte) ((pte) & 0x3FF)
#define PTE_V (1L << 0) // valid
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)
#define PTE_U (1L << 4) // 1 -> user can access
#define PTE_COW (1L << 8) // cow

#include "platform.h"

// use typedef to make type flexible.
typedef uint8_t pgref_t;

// 8MB/PAGESIZE = 2K * 2B
typedef struct _page_t {
    pgref_t refcnt;
    struct {
        uint8_t order : 4; // for BUDDY use lowest 4 bits only, max 14 (15 as invaild)
        uint8_t alloc : 2; // for BUDDY, acutally we use only one bit
#define ALLOC_BUDDY     0
#define ALLOC_FREELIST  1
#define ALLOC_SLOB      2
        uint8_t type  : 2; // page type ()
    };
    // uint8_t resv[2]; // reserved for special use
} page_t;

#define PAGE_NUMS (MEM_SIZE/PGSIZE)
#define PAGE2NUM(pa) (((uint64_t)(pa) - KERNBASE) / PGSIZE)
#define NUM2PAGE(num) ((uint64 *)((num) * PGSIZE + KERNBASE))

extern page_t pages[PAGE_NUMS];

void    page_init(void);

pgref_t ref_page(uint64_t pa);
pgref_t deref_page(uint64_t pa);
pgref_t page_ref(uint64_t pa);
void    mark_page(uint64_t pa, int type);
int     page_type(uint64_t pa);

int     _mappages(pagetable_t pagetable, uint64 va, size_t sz, uint64 pa, int perm, int spec);
pte_t*  _walk(pagetable_t pagetable, uint64 va, int alloc, int pg_spec);
void    _uvmunmap(pagetable_t, uint64, uint64, int, int);

/* 出于简洁性与兼容性，我们定义了页面映射函数的默认行为(规格) */
/* Out of convenient and compatibility, we define the default behavior for the func below */
#define mappages(pagetable, va, sz, pa, perm) \
    _mappages(pagetable, va, sz, pa, perm, PGSPEC_NORMAL)

#define uvmunmap(pagetable, va, npages, do_free) \
    _uvmunmap(pagetable, va, npages, do_free, PGSPEC_NORMAL)

#define walk(pagetable, va, alloc) \
    _walk(pagetable, va, alloc, PGSPEC_NORMAL)

#endif