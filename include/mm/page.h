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
#include "atomic/spinlock.h"
#include "page-flags.h"
#include "config.h"
// #include "fs/fs.h"

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

struct address_space;
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

    uint64_t flags;      /* bit操作时将指针转化为int型，设置为uint8_t类型会不会有问题？ */
    list_head_t lru; /* 串联页，active/inactive list */
    struct address_space *mapping;
    uint32_t index;

#ifdef RMAP
    union {
		struct pte_chain *chain;/* Reverse pte mapping pointer.
					 * protected by PG_chainlock */
		pte_addr_t direct;
	} pte;
#endif
} page_t;

/* 页的数量 */
#define PAGE_NUMS ((MEM_END - KERN_BASE)/PGSIZE)
/* 地址--->页号 */
#define PAGE2NUM(pa) (((uint64_t)(pa) - KERN_BASE) / PGSIZE)
/* 页号--->地址 */
#define NUM2PAGE(num) ((uint64_t)((num) * PGSIZE + KERN_BASE))
/* 页指针--->地址 */
#define PAGETOPA(page) NUM2PAGE(page-pages)
/* 地址--->页指针 */
#define PATOPAGE(pa) &pages[PAGE2NUM(pa)]

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
void    pte_print(pte_t *pte);

pgref_t get_page(page_t *page);
pgref_t put_page(page_t *page);
pgref_t page_refcnt(page_t *page);

void lock_page(page_t *page);
void unlock_page(page_t *page);

void set_page_dirty(uint64_t pa);
void clear_page_dirty(uint64_t pa);
int page_is_dirty(uint64_t pa);

void unlock_put_page(page_t *page);
void get_lock_page(page_t *page);
void print_not_freed_pages();
void reset_page(page_t *page);

/* 出于简洁性与兼容性，我们定义了页面映射函数的默认行为(规格) */
/* Out of convenient and compatibility, we define the default behavior for the func below */
#define mappages(pagetable, va, sz, pa, prot) \
    _mappages(pagetable, va, sz, pa, prot, PGSPEC_NORMAL)

#define uvmunmap(pagetable, va, npages, do_free) \
    _uvmunmap(pagetable, va, npages, do_free, PGSPEC_NORMAL)

#define walk(pagetable, va, alloc) \
    _walk(pagetable, va, alloc, PGSPEC_NORMAL)

#define PAGE_CACHE_SHIFT PGSHIFT
#define PAGE_CACHE_SIZE PGSIZE

#define PAGE_SHIFT PGSHIFT

/* page fault 使用的位 */
#define pte_none(pte)           (!pte)
#define pte_valid(pte)          (pte & PTE_V) 
#define pte_write(pte)          (pte & PTE_W)


/* page frame reclaiming 页回收算法需要的数据结构 */

#ifdef RMAP
/*
 * Return true if this page is mapped into pagetables.
 */
static inline int page_mapped(page_t *page)
{
    return page->pte.direct != 0;
	// return page->_mapcount;
}
#endif
/* mmzone.h */
struct zone{

    /* Fields commonly accessed by the page reclaim scanner */
    spinlock_t lru_lock;
    list_head_t active_list;        /* 活跃页链表 */
    list_head_t inactive_list;      /* 非活跃页链表 */
    uint64_t nr_active;             /* 活跃页数量 */
    uint64_t nr_inactive;           /* 非活跃页数量 */
    // uint64_t pages_scanned;
};

typedef struct zone zone_t;

extern zone_t memory_zone;
/* mm_inline.h */
static inline void
add_page_to_active_list(struct zone *zone, page_t *page)
{
	list_add(&page->lru, &zone->active_list);
	zone->nr_active++;
}

static inline void
add_page_to_inactive_list(struct zone *zone, page_t *page)
{
	list_add(&page->lru, &zone->inactive_list);
	zone->nr_inactive++;
}

static inline void
del_page_from_active_list(struct zone *zone, page_t *page)
{
	list_del(&page->lru);
	zone->nr_active--;
}

static inline void
del_page_from_inactive_list(struct zone *zone, page_t *page)
{
	list_del(&page->lru);
	zone->nr_inactive--;
}

static inline void
del_page_from_lru(struct zone *zone, page_t *page)
{
	list_del(&page->lru);
	if (PageActive(page)) {
		ClearPageActive(page);
		zone->nr_active--;
	} else {
		zone->nr_inactive--;
	}
}

/* swap.c */
void mark_page_accessed(page_t *page);


#define PTE_VALID (0) // valid
#define PTE_READ (1)
#define PTE_WRITE (2)
#define PTE_EXECUTE (3)
#define PTE_USER (4) 

// static inline  int ptep_test_and_clear_valid(pte_t *ptep)	{ return test_and_clear_bit(PTE_VALID, ptep);}
#define DEF_PRIORITY 5
#endif