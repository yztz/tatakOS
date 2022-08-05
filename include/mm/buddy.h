#ifndef _H_BUDDY_
#define _H_BUDDY_

#include "types.h"
struct _page_t;
void buddy_init(void);
void *buddy_alloc(size_t size);
void buddy_free(void *addr);
void buddy_print_free();
void buddy_free_one_page(struct _page_t *page);
uint64_t buddy_getfree();
uint64_t buddy_gettotal();

#endif