/*
 * include/linux/pagevec.h
 *
 * In many places it is efficient to batch an operation up against multiple
 * pages.  A pagevec is a multipage container which is used for that.
 */

/**
 * @brief 注：pagevec相当于一个缓冲区，在系统想在active list和inactive list添加
 * 元素时，不直接添加，而是添加到这里，等到pagevec满了再添加进相应的链表。据说这样
 * 可以避免频繁的获取lru lock。提升系统性能。
 * 
 * pagevec是否有必要设置为cpu私有?
 * 
 */

/* 14 pointers + two long's align the pagevec structure to a power of two */
#ifndef _H_PAGEVEC
#define _H_PAGEVEC

#include "types.h"
#include "mm/page.h"

#define PAGEVEC_SIZE	1

struct page;
struct address_space;

struct pagevec {
	unsigned long nr;
	// unsigned long cold;
	page_t *pages[PAGEVEC_SIZE];
};

typedef struct pagevec pagevec_t;

extern pagevec_t lru_add_pvecs, lru_add_active_pvecs;

void pagevec_release(struct pagevec *pvec);
void __pagevec_release_nonlru(struct pagevec *pvec);
void __pagevec_free(struct pagevec *pvec);
void pagevec_lru_add(struct pagevec *pvec);
void pagevec_lru_add_active(struct pagevec *pvec);
void pagevec_strip(struct pagevec *pvec);
unsigned pagevec_lookup(struct pagevec *pvec, struct address_space *mapping,
		pgoff_t start, unsigned nr_pages);
unsigned pagevec_lookup_tag(struct pagevec *pvec,
		struct address_space *mapping, pgoff_t *index, int tag,
		unsigned nr_pages);

static inline void pagevec_init(struct pagevec *pvec)
{
	pvec->nr = 0;
	// pvec->cold = cold;
}

static inline void pagevec_reinit(struct pagevec *pvec)
{
	pvec->nr = 0;
}

static inline unsigned pagevec_count(struct pagevec *pvec)
{
	return pvec->nr;
}

static inline unsigned pagevec_space(struct pagevec *pvec)
{
	return PAGEVEC_SIZE - pvec->nr;
}

/*
 * Add a page to a pagevec.  Returns the number of slots still available.
 */
static inline unsigned pagevec_add(struct pagevec *pvec, page_t *page)
{
	pvec->pages[pvec->nr++] = page;
	return pagevec_space(pvec);
}

// static inline void pagevec_release(struct pagevec *pvec)
// {
// 	if (pagevec_count(pvec))
// 		__pagevec_release(pvec);
// }

static inline void pagevec_release_nonlru(struct pagevec *pvec)
{
	if (pagevec_count(pvec))
		__pagevec_release_nonlru(pvec);
}

static inline void pagevec_free(struct pagevec *pvec)
{
	if (pagevec_count(pvec))
		__pagevec_free(pvec);
}

// static inline void pagevec_lru_add(struct pagevec *pvec)
// {
// 	if (pagevec_count(pvec))
// 		__pagevec_lru_add(pvec);
// }

#endif