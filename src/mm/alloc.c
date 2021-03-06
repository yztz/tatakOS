#include "printf.h"
#include "defs.h"
#include "mm/freelist.h"
#include "mm/buddy.h"
#include "mm/page.h"
#include "mm/slob.h"
#include "param.h"

#define JUNK 1

extern char end[];

void kinit(void) {
    page_init();
    buddy_init();
    slob_init();
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
        ret = slob_alloc(size);
    } else { // more than one page, We use buddy
        ret = buddy_alloc(size);
    }
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
    return kmalloc(PGSIZE);
}

/**
 * IMPORTANT!
 * Note: kfree does not check the legality of addr now,
 * so never try to free a unallocated mem or try to free a truncated mem.
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


