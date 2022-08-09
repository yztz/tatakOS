#ifndef _H_ALLOC_
#define _H_ALLOC_

#include "page.h"

void  kinit(void);
void *kalloc(void);
void  kfree(void *addr);
void  _kfree_safe(void **paddr);
void *kzalloc(size_t size);
void *kmalloc(size_t size);
uint64_t get_total_mem();
uint64_t get_free_mem();

#define kfree_safe(pp) _kfree_safe((void **)(pp))
void free_one_page(page_t *page);


#define AF_KERNEL 0x0
#define AF_USER 0x1

#endif