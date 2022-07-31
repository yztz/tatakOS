#ifndef _H_ALLOC_
#define _H_ALLOC_

#include "page.h"

void  kinit(void);
void *kalloc(void);
void  kfree(void *addr);
void  _kfree_safe(void **paddr);
void *kzalloc(size_t size);
void *kmalloc(size_t size);

#define kfree_safe(pp) _kfree_safe((void **)(pp))
void free_one_page(page_t *page);

#endif