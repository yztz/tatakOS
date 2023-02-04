/**
 * ref: https://www.cnblogs.com/alantu2018/p/8527821.html
*/

#include "atomic/spinlock.h"
#include "atomic/atomic.h"
#include "common.h"
#include "mm/page.h"
#include "mm/buddy.h"
#include "mm/mmap.h"

#define __MODULE_NAME__ BUDDY
#include "debug.h"

#define INVAIL_ORDER (0xf)

#define PARTNER_NR(pgnum, order) ((pgnum) ^ (1 << (order)))
#define MERGE_NR(pgnum, order) ((pgnum) & ~(1 << (order)))

#define BUDDY_INIT_HEAD(head) {(head) = (buddy_t){&(head), &(head)};}

typedef struct _buddy_t {
    struct _buddy_t *next, *pre;
} buddy_t;

typedef struct _buddy_list_t {
    buddy_t head;
    struct spinlock lock;
} buddy_list_t;

extern char end[];

buddy_list_t lists[MAX_ORDER];

atomic_t used;
uint total;

void print_order(int order) {
    printf(grn("[order %d head: %p] "), order, &lists[order].head);
    for (buddy_t *cur = lists[order].head.next; cur != &lists[order].head; cur = cur->next) {
        printf(bl("-> %p"), cur);
    }
}


void print_buddy() {
    for (int i = 0; i < MAX_ORDER; i++) {
        print_order(i);
        printf("\n");
    }
    print_not_freed_pages();
}



uint64_t buddy_getfree() {
    return total - atomic_get(&used);
}

uint64_t buddy_gettotal() {
    return total;
}

void buddy_init() {
    for (int i = 0; i < MAX_ORDER; i++) {
        BUDDY_INIT_HEAD(lists[i].head);
        initlock(&lists[i].lock, "buddy list");
    }
    total = 0;
    /* 从kernel end处一直释放到MEM_END */
    for (uint64_t p = (uint64_t)end; p < MEM_END; p += PGSIZE) {
        buddy_free((void *)p);
        total++;
    }
    used = INIT_ATOMIC();
    // buddy_print_free();
}

static inline void insert(int order, buddy_t *b) {
    b->next = lists[order].head.next;
    b->pre = &lists[order].head;
    lists[order].head.next->pre = b;
    lists[order].head.next = b;
}

static inline void remove(buddy_t *b) {
    if ((uint64_t)b > MEM_END || (uint64_t)b->next > MEM_END || (uint64_t)b->pre > MEM_END)
        ER();

    b->next->pre = b->pre;
    b->pre->next = b->next;
    b->next = b;
    b->pre = b;
}


static inline int empty(int order) {
    return lists[order].head.next == &lists[order].head;
}


void *buddy_alloc(size_t size) {

    int pgnums;
    int order, oorder;
    buddy_t *b;

    if (!size)
        panic("buddy_alloc: size");

    pgnums = ROUND_COUNT(size);
    // 获取最小的大于size的2^order
    oorder = is_pow2(pgnums) ? log2(pgnums) : log2(pgnums) + 1;
    if (oorder >= MAX_ORDER)
        return NULL;

    order = oorder;
    // 从当前order向上，直到寻找到有空闲空间的order
    acquire(&lists[order].lock);
    while (order + 1 < MAX_ORDER && empty(order)) {
        release(&lists[order].lock);
        order++;
        acquire(&lists[order].lock);
    }


    // No Rooms
    int u = atomic_get(&used);
    if (empty(order) || (u * 100 / total) >= 99) {
        // if(empty(order)) {
        release(&lists[order].lock);

        // ER();
        /* 这里里要修改，否则有重复循环执行的可能 */
        // printf(ylw("size: %d\tpgnums: %d\torder: %d\toorder: %d\n"), size, pgnums, order, oorder); 
        // buddy_print_free();
        // free_more_memory();
        // buddy_print_free();
        // // printf(ylw("size: %d\tpgnums: %d\torder: %d\toorder: %d\n"), size, pgnums, order, oorder); 
        // printf("\n");

        // /* 释放后重试 */
        // goto retry;

        // ERROR("out of memory!!");
        return NULL;
    }

    // 当当前的order大于预订的order时，需要分裂
    while (order > oorder) {
        int pgnum, ppgnum;

        b = lists[order].head.next;
        remove(b);
        // 当前buddy的页号
        pgnum = PG_TO_NR(b);
        // 兄弟buddy的页号
        // 为什么是order-1，因为我们这里的兄弟页号是指分裂后的兄弟页号
        ppgnum = PARTNER_NR(pgnum, order - 1);
        // 为了避免作为兄弟页被合并的风险
        // 比如当兄弟buddy被释放了，而当前的buddy正在处于分裂状态，所以存在竞争
        pages[pgnum].order = INVAIL_ORDER;
        pages[ppgnum].order = INVAIL_ORDER;
        release(&lists[order].lock);

        order--;

        acquire(&lists[order].lock);
        // 先插大地址，后插小地址，尽可能保证后续分配地址连续
        insert(order, (buddy_t *)NR_TO_ADDR(ppgnum));
        insert(order, b);
        pages[pgnum].order = order;
        pages[ppgnum].order = order;
    }
    b = lists[order].head.next;
    remove(b);
    // 设置页面属性
    // 为什么需要在锁范围内设置？因为无论是在分裂还是合并页的时候，我们都是根据地址特征来获取兄弟页面的
    // 因此如果在锁外设置页面属性，就可能导致我们的页面属性还没被设置为alloc，结果被提前合并的现象
    page_t *page = ADDR_TO_PAGE(b);

    assert(page->alloc == 0);
    page->alloc = 1;

    assert(page_refcnt(page) == 0);
    get_page(page);

    mark_page((uint64_t)b, ALLOC_BUDDY);

    release(&lists[order].lock);


    atomic_add(&used, 1 << oorder);
    return (void *)b;
}

extern zone_t memory_zone;
/**
 * 释放一个页
 */
void buddy_free(void *pa) {
    int pgnum, ppgnum;
    page_t *page, *ppage;
    buddy_t *b;

    pgnum = PG_TO_NR(pa);

    if (pgnum >= PAGE_NUMS) {
        panic("buddy_free: out of range");
    }

    assert(page_refcnt((uint64_t)pa) > 0);

    if (put_page_nofree((uint64_t)pa) > 0)
        return;

    page = &pages[pgnum];

    /* 不应该放在这里 */
    // if(!list_is_head(&page->lru, &page->lru))
      // del_page_from_lru(&memory_zone, page);
    /* 回收page时清除其状态 */
    reset_page(page);

    if (page->alloc == 0) {
        print_page(pgnum);
        panic("buddy_free: page not allocated");
    }
    b = (buddy_t *)pa;

    // insert back
    acquire(&lists[page->order].lock);
    insert(page->order, b);

    // release
    page->alloc = 0;
    atomic_add(&used, -(1 << page->order));

    ppgnum = PARTNER_NR(pgnum, page->order);
    ppage = &pages[ppgnum];
    // 尝试合并
    while (page->order + 1 < MAX_ORDER && ppage->alloc == 0 && page->order == ppage->order) {
        int order;
        // remove from list
        remove((buddy_t *)NR_TO_ADDR(pgnum));
        remove((buddy_t *)NR_TO_ADDR(ppgnum));
        order = page->order;
        page->order = INVAIL_ORDER;
        ppage->order = INVAIL_ORDER;
        release(&lists[order].lock);

        acquire(&lists[order + 1].lock);
        pgnum = MERGE_NR(pgnum, order);
        page = &pages[pgnum];
        page->order = order + 1;
        b = (buddy_t *)NR_TO_ADDR(pgnum);

        insert(page->order, b);

        ppgnum = PARTNER_NR(pgnum, page->order);
        ppage = &pages[ppgnum];
    }
    release(&lists[page->order].lock);
}

/**
 * 当page->refcnt为0时调用，释放一个页，buddy_free的改版
 */
void buddy_free_one_page(page_t *page) {
    int pgnum, ppgnum;
    page_t *ppage;
    buddy_t *b;
    uint64_t pa;

    // pgnum = PG_TO_NR(pa);
    pgnum = page - pages;
    pa = (uint64_t)NR_TO_ADDR(pgnum);

    if (pgnum >= PAGE_NUMS) {
        panic("buddy_free: out of range");
    }

    /**
     * page->refcnt为0才会调用free函数free掉页
     */
    if (page_refcnt(page) != 0)
        ER();

    /* 不应该放在这里 */
    // if(!list_is_head(&page->lru, &page->lru))
      // del_page_from_lru(&memory_zone, page);
    /* 回收page时清除其状态 */
    reset_page(page);

    if (page->alloc == 0) {
        print_page(pgnum);
        panic("buddy_free: page not allocated");
    }
    b = (buddy_t *)pa;

    // insert back
    acquire(&lists[page->order].lock);
    insert(page->order, b);

    // release
    page->alloc = 0;
    atomic_add(&used, -(1 << page->order));

    ppgnum = PARTNER_NR(pgnum, page->order);
    ppage = &pages[ppgnum];
    // 尝试合并
    while (page->order + 1 < MAX_ORDER && ppage->alloc == 0 && page->order == ppage->order) {
        int order;
        // remove from list
        remove((buddy_t *)NR_TO_ADDR(pgnum));
        remove((buddy_t *)NR_TO_ADDR(ppgnum));
        order = page->order;
        page->order = INVAIL_ORDER;
        ppage->order = INVAIL_ORDER;
        release(&lists[order].lock);

        acquire(&lists[order + 1].lock);
        pgnum = MERGE_NR(pgnum, order);
        page = &pages[pgnum];
        page->order = order + 1;
        b = (buddy_t *)NR_TO_ADDR(pgnum);

        insert(page->order, b);

        ppgnum = PARTNER_NR(pgnum, page->order);
        ppage = &pages[ppgnum];
    }
    release(&lists[page->order].lock);
}

void buddy_print_free() {
    int u = atomic_get(&used);
    printf("page usage: %d%% ( %d used | %d total )\n", u * 100 / total, u, total);
}


