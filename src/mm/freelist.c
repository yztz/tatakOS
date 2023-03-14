#include "atomic/atomic.h"
#include "atomic/spinlock.h"
#include "common.h"
#include "debug.h"
#include "mm/page.h"

extern char end[];

typedef struct page_list_head {
    list_head_t head;
    int count;
} page_list_head_t;

static page_list_head_t freelist = {.head=LIST_HEAD_INIT(freelist.head), .count=0};
static atomic_t used = INIT_ATOMIC();
static int total;

static SPINLOCK_INIT(freelist_lock);

void freelist_free(page_t *page);
void freelist_init() {
    for(int i = 0; i < PAGE_NUMS; i++) {
        pages[i].pgnum = 1;
    }

    /* 从kernel end处一直释放到MEM_END */
    for (uint64_t p = (uint64_t)end; p < MEM_END; p += PGSIZE) {
        freelist_free(ADDR_TO_PG(p));
        total++;
    }

    used = INIT_ATOMIC();
}

void *freelist_alloc(int pgnum) {
    // Only support single page allocation
    if (pgnum > (uint8_t)-1)
        panic("too much pages");

    acquire(&freelist_lock);

    page_list_head_t *b = NULL;

    list_for_each_entry(b, &freelist.head, head) {
        if (b->count > pgnum) {
            page_list_head_t *newb = (page_list_head_t *) ((uint64_t)b + pgnum * PGSIZE);
            newb->count = b->count - pgnum;
            list_add(&newb->head, &b->head);
            break;
        } else if (b->count == pgnum) { 
            break;
        }
    }

    if (!b) {
        release(&freelist_lock);
        return NULL;
    }

    list_del(&b->head);
    
    page_t *page = ADDR_TO_PG(b);
    page->pgnum = pgnum;

    assert(page_refcnt(page) == 0);
    get_page(page);

    release(&freelist_lock);

    atomic_add(&used, pgnum);

    return (void *)b;
}

void freelist_free(page_t *page) {
    assert(page_refcnt(page) == 0);
    page_list_head_t *pre = &freelist;
    page_list_head_t *b = (page_list_head_t *) PG_TO_ADDR(page);
    uint8_t pgnum = page->pgnum;
    b->count = pgnum;

    reset_page(page);

    acquire(&freelist_lock);

    page_list_head_t *cur = list_next_entry(pre, head);

    while (cur != &freelist) {
        uint64_t end_page_addr = (uint64_t)cur + PGSIZE * cur->count;
    
        if ((uint64_t)b == end_page_addr) { 
            // Consecutive
            cur->count += pgnum;
            goto out;
        } else if ((uint64_t)b < end_page_addr) { 
            // Insert before cur, because it is inconsecutive to pre
            list_add(&b->head, &pre->head);
            goto out;
        }
        pre = cur;
        cur = list_next_entry(pre, head);
    }

    list_add(&b->head, &cur->head);

out:
    release(&freelist_lock);

    atomic_add(&used, -pgnum);
}


/* Below is alloc interface implementation */

void __alloc_page_init() {
    freelist_init();
}


void *__alloc_pages(int pgnum) {
    return freelist_alloc(pgnum);
}


void __free_pages(page_t *first_page) {
    freelist_free(first_page);
}

int __page_count(page_t *first_page) {
    return 1;
}

int __page_gettotal() {
    return total;
}

int __page_getfree() {
    return total - atomic_get(&used);
}

int __page_getused() {
    return atomic_get(&used);
}