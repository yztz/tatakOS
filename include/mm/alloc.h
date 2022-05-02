#ifndef _H_ALLOC_
#define _H_ALLOC_

void  kinit(void);
void *kalloc(void);
void  kfree(void *addr);
void *kzalloc(size_t size);
void *kmalloc(size_t size);

#endif