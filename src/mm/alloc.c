#include "mm/freelist.h"
#include "mm/buddy.h"
#include "mm/page.h"

#define BUDDY

void kinit(void) {
    #ifdef BUDDY
    buddy_init();
    #else
    freelist_init();
    #endif
}


void *kalloc(void) {
    #ifdef BUDDY
    return buddy_alloc(PGSIZE);
    #else
    return freelist_alloc();
    #endif
}

void kfree(void *addr) {
    #ifdef BUDDY
    buddy_free(addr);
    #else
    freelist_free(addr);
    #endif
}


