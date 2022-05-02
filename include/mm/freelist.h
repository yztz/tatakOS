#ifndef _H_FREELIST_
#define _H_FREELIST_

void    freelist_init(void);
void*   freelist_alloc(void);
void    freelist_free(void *addr);
void    freelist_freerange(void *pa_start, void *pa_end);

#endif