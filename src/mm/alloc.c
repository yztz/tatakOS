#include "printf.h"
#include "defs.h"
#include "mm/freelist.h"
#include "mm/buddy.h"
#include "mm/page.h"
#include "param.h"

#define JUNK 1

void kinit(void) {
    page_init();
    #ifdef BUDDY
    buddy_init();
    #else
    freelist_init();
    #endif
}


void *kmalloc(size_t size) {
    panic("oops");
}


void *kalloc(void) {
    void *pa;
    #ifdef BUDDY
    pa = buddy_alloc(PGSIZE);
    #else
    pa = freelist_alloc();
    #endif

    if(pa && ref_page((uint64_t)pa) != 1) 
        panic("ref");
    if(pa)
        memset(pa, JUNK, PGSIZE);
    return pa;
}

void kfree(void *addr) {
    // Do we need a lock when we check this value ?
    if(deref_page((uint64_t)addr) > 0) 
        return;
    
    if(addr)
        memset(addr, JUNK, PGSIZE);

    #ifdef BUDDY
    buddy_free(addr);
    #else
    freelist_free(addr);
    #endif
}


