/**
 * @file alloc.c
 * @author YangZongzhen
 * @brief This file defines the interface to the memory request module. 
 * @note 
 * Currently, page allocation and small memory allocation are supported. 
 * The real allocator interface is as follows:
 * 
 *      void *__alloc_frags(size_t size)
 *      void *__free_frags(void *addr)
 *      void *__alloc_pages(int pgnum)
 *      void __free_pages(page_t *first_page)
 *      int __page_count(page_t *first_page)
 *      int __page_gettotal()
 *      int __page_getfree()
 *      void __alloc_page_init();
 *      void __alloc_frag_init();
 * 
 * Currently, the underlying allocator implementation is 
 *  the Buddy allocator, the Slob allocator and the Freelist allocator.
 * 
 * The dependences of the modules(alloc buddy slob) are just like below:
 * 
 *           alloc  
 *           /  \
 *          /    \
 *      buddy <- slob
 * 
 * Another point to note is how `kfree` frees page units.
 * As mentioned above, we currently handle references to pages in the same way
 *  (whether single page or multiple page).
 * So we simply call `put_page` in the free page branch in `kfree`. 
 * Then decide if call `free_page` for true free.
 * 
 * In other words, both `kfree` and `put_page` take into account the number of references on the page, 
 *  whereas the underlying allocator's interface implementation does not.
 * 
 * In fact, `kfree` is just a more general wrapper for `put_page` for page free.
 * 
 * @version 0.1
 * @date 2023-03-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "mm/alloc.h"
#include "common.h"

#define __MODULE_NAME__ ALLOC
#include "debug.h"

#define JUNK 1

extern void *__alloc_frags(size_t size);
extern void *__free_frags(void *addr);
extern void __alloc_page_init();
extern void __alloc_frag_init();
extern void *__alloc_pages(int pgnum);
extern int __page_count(page_t *first_page);
extern int __page_gettotal();
extern int __page_getfree();
extern int __page_getused();

void mem_init() {
    page_init();
    __alloc_page_init();
    __alloc_frag_init();

    debug("init success");
}


void *kmalloc(size_t size) {
    void *ret = NULL;

    if(size < PGSIZE) {
        ret = __alloc_frags(size);
    } else {
        ret = __alloc_pages(ROUND_COUNT(size));
    }
    /* Alloc failure */
    if(!ret){
        panic("No Mem");
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


#define is_frag(addr) ((uint64_t)addr & ~PGMASK)

void  kfree(void *addr) {
    //todo: do more checks...
    if(addr == NULL) return;

    if(is_frag(addr)) { // from fragment allocator
        __free_frags(addr);
    } else {
        page_t *first_page = ADDR_TO_PG(addr);
        put_page(first_page);
    }
}


void _kfree_safe(void **paddr) {
    if(paddr && *paddr) {
        kfree(*paddr);
        *paddr = NULL;
    }
}

uint64_t get_total_mem() {
    return __page_gettotal();
}

uint64_t get_free_mem() {
    return __page_getfree();
}

uint64_t get_used_mem() {
    return __page_getused();
}

void print_mm_free() {
    int u = get_used_mem();
    int t = get_total_mem();
    kprintf("page usage: %d%% ( %d used | %d total )\n", u * 100 / t, u, t);
}