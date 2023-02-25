#ifndef _H_ALLOC_
#define _H_ALLOC_

#include "page.h"

void mem_init();

void *kzalloc(size_t size);
void *kmalloc(size_t size);
void  kfree(void *addr);

uint64_t get_total_mem();
uint64_t get_free_mem();

void  _kfree_safe(void **paddr);
#define kfree_safe(pp) _kfree_safe((void **)(pp))

// Allocate memory from stack, so use it carefully.
#define alloca(size) __builtin_alloca(size)

#endif