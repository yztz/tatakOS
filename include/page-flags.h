#ifndef _H__PAGE_FLAGS
#define _H__PAGE_FLAGS

/* from linux 2.6.11 page-flags.h*/
#define PG_locked	 	 0	/* Page is locked. Don't touch. */
#define PG_error		 1
#define PG_referenced	 2
#define PG_uptodate		 3

#define PG_dirty	 	 4
#define PG_lru			 5
#define PG_active		 6
#define PG_slab			 7	/* slab debug (Suparna wants this) 用作slob */

#define PG_writeback		13	/* Page is under writeback */
#define PG_chainlock		15	/* lock bit for ->pte_chain */
#define PG_direct		16	/* ->pte_chain points directly at pte */

/**
 * lock_page时使用，防止sleep还没完成，就被wakeup了。
 */
#define PG_spinlock 17


#define PageLocked(page)		\
		test_bit(PG_locked, &(page)->flags)
#define SetPageLocked(page)		\
		set_bit(PG_locked, &(page)->flags)
#define TestSetPageLocked(page)		\
		test_and_set_bit_lock(PG_locked, &(page)->flags)
#define ClearPageLocked(page)		\
		clear_bit_unlock(PG_locked, &(page)->flags)
#define TestClearPageLocked(page)	\
		test_and_clear_bit(PG_locked, &(page)->flags)

// #define PageError(page)		test_bit(PG_error, &(page)->flags)
// #define SetPageError(page)	set_bit(PG_error, &(page)->flags)
// #define ClearPageError(page)	clear_bit(PG_error, &(page)->flags)

// #define PageReferenced(page)	test_bit(PG_referenced, &(page)->flags)
// #define SetPageReferenced(page)	set_bit(PG_referenced, &(page)->flags)
// #define ClearPageReferenced(page)	clear_bit(PG_referenced, &(page)->flags)
// #define TestClearPageReferenced(page) test_and_clear_bit(PG_referenced, &(page)->flags)


#define PageDirty(page)		test_bit(PG_dirty, &(page)->flags)
#define SetPageDirty(page)	set_bit(PG_dirty, &(page)->flags)
#define TestSetPageDirty(page)	test_and_set_bit(PG_dirty, &(page)->flags)
#define ClearPageDirty(page)	clear_bit(PG_dirty, &(page)->flags)
#define TestClearPageDirty(page) test_and_clear_bit(PG_dirty, &(page)->flags)

// #define SetPageLRU(page)	set_bit(PG_lru, &(page)->flags)
// #define PageLRU(page)		test_bit(PG_lru, &(page)->flags)
// #define TestSetPageLRU(page)	test_and_set_bit(PG_lru, &(page)->flags)
// #define TestClearPageLRU(page)	test_and_clear_bit(PG_lru, &(page)->flags)

// #define PageActive(page)	test_bit(PG_active, &(page)->flags)
// #define SetPageActive(page)	set_bit(PG_active, &(page)->flags)
// #define ClearPageActive(page)	clear_bit(PG_active, &(page)->flags)
// #define TestClearPageActive(page) test_and_clear_bit(PG_active, &(page)->flags)
// #define TestSetPageActive(page) test_and_set_bit(PG_active, &(page)->flags)

#endif