#ifndef _H_BUDDY_
#define _H_BUDDY_

#include "types.h"

void buddy_init(void);
void *buddy_alloc(size_t size);
void buddy_free(void *addr);

#endif