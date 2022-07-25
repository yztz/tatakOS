#ifndef _LINUX_SWAP_H
#define _LINUX_SWAP_H

#define SWAP_CLUSTER_MAX 32

/* return values of try_to_unmap */
#define	SWAP_SUCCESS	0
#define	SWAP_AGAIN	1
#define	SWAP_FAIL	2

/* linux/mm/swap.c */
extern void lru_cache_add(page_t *);
extern void lru_cache_add_active(page_t *);
extern void activate_page(page_t *);
extern void mark_page_accessed(page_t *);
extern void lru_add_drain(void);
extern int rotate_reclaimable_page(page_t *page);
extern void swap_setup(void);

/* linux/mm/rmap.c */
int page_referenced(page_t *);
int try_to_unmap(page_t * page);
#endif