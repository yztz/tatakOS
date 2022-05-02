#ifndef _H_SLOB_
#define _H_SLOB_

void   slob_init(void);
void  *slob_alloc(size_t size);
void   slob_free(void *addr);

#endif