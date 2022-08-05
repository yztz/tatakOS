#include "printf.h"
#include "defs.h"
#include "mm/freelist.h"
#include "mm/alloc.h"
#include "mm/buddy.h"
#include "mm/slob.h"
#include "param.h"
#include "utils.h"

#define __MODULE_NAME__ ALLOC
#include "debug.h"
#include "pagevec.h"

#define JUNK 1

extern char end[];

void kinit(void) {
    page_init();
    buddy_init();
    slob_init();

    debug("init success");
    // freelist_init();
    // #ifdef BUDDY
    // freelist_start = buddy_alloc(FREELIST_MEM);
    // freelist_end = (void *)((uint64_t)freelist_start + FREELIST_MEM)
    // #else
    // freelist_start = end;
    // freelist_end = (void *)PHYSTOP;
    // #endif
    // freelist_freerange(freelist_start, freelist_end);
}

void *kmalloc(size_t size) {
    void *ret = NULL;
    if(size < PGSIZE) { // Smaller, we use slob
        // printf("alloc from slob\n");
        // if(size == 1)
        //     panic("one byte?");
        ret = slob_alloc(size);
    } else { // more than one page, We use buddy
        ret = buddy_alloc(size);
    }
    // if((uint64_t)ret == 0x8025d000)
        // for(;;);
    // printf(ylw("pa: %p\n"), ret);
    return ret;
}

void *kzalloc(size_t size) {
    void *ret = kmalloc(size);
    if(ret) {
        memset(ret, 0, size);
    }
    return ret;
}

/**
 * @deprecated use kmalloc(PGSIZE) instead
*/
void *kalloc(void) {
    return kzalloc(PGSIZE);
}

/**
 * IMPORTANT!
 * Note: kfree does not check the legality of addr now,
 * so never try to free an unallocated mem or try to free a truncated mem.
*/
void kfree(void *addr) {
    //todo: do more checks...
    if(addr == NULL) return;

    if((uint64)addr & ~PGMASK) { // piece
        slob_free(addr);
    } else {
        buddy_free(addr);
    }
    
}


void _kfree_safe(void **paddr) {
    if(paddr && *paddr) {
        kfree(*paddr);
        *paddr = NULL;
    }
}

void free_one_page(page_t *page){
    buddy_free_one_page(page);
}


/* new added functions for page frame reclaiming */
// void __pagevec_free(pagevec_t *pvec){
//     int i = pagevec_count(pvec);

//     /* buddy/kfree的接口可以考虑换一下，参数可选择页指针 */
//     while(--i >= 0){
//         kfree(NUM2PAGE(pvec->pages[i] - pages));
//     }
// }
uint64_t get_total_mem() {
    return buddy_gettotal();
}

uint64_t get_free_mem() {
    return buddy_getfree();
}
