#ifndef _H_SLOB_
#define _H_SLOB_

#include "types.h"

void   slob_init(void);
void  *slob_alloc(size_t size);
void   slob_free(void *addr);

#endif