
#include "riscv.h"
#include "mm/vm.h"
#include "fs/stat.h"
#include "fs/fs.h"
#include "atomic/spinlock.h"
#include "atomic/sleeplock.h"
#include "kernel/proc.h"
#include "fs/file.h"
#include "fs/fcntl.h"
#include "rbtree.h"
#include "common.h"
#include "list.h"
#include "pagevec.h"

// #define QUIET
#define __MODULE_NAME__ SWAP
#include "debug.h"


zone_t memory_zone = (zone_t){.lru_lock = {.locked=0, .name="zone lock", .cpu=NULL}, 
                              .active_list = {&memory_zone.active_list, &memory_zone.active_list}, 
                              .inactive_list = {&memory_zone.inactive_list, &memory_zone.inactive_list}, 
                              .nr_active = 0, 
                              .nr_inactive = 0};

/* 2个cpu */
pagevec_t lru_add_pvecs[NUM_CORES], lru_add_active_pvecs[NUM_CORES];
// pagevec_t lru_add_pvecs, lru_add_active_pvecs;


void pvec_init(){
	push_off();
	struct cpu *c = mycpu();
	int id = cpuid();
	c->inactive_pvec = &lru_add_pvecs[id];
	c->active_pvec = &lru_add_active_pvecs[id];
	lru_add_pvecs[id].nr = 0;
	lru_add_active_pvecs[id].nr = 0;
	pop_off();
}
/**
 * @brief 从inactive链表上删除，添加到active链表头。
 * 
 */
void activate_page(page_t *page)
{
	zone_t *zone = &memory_zone;

	spin_lock(&zone->lru_lock);
	if (PageLRU(page) && !PageActive(page)) {
		del_page_from_inactive_list(zone, page);
		SetPageActive(page);
		add_page_to_active_list(zone, page);
	}
	spin_unlock(&zone->lru_lock);
}

/*
 * Mark a page as having seen activity.
 *
 * inactive,unreferenced	->	inactive,referenced
 * inactive,referenced		->	active,unreferenced
 * active,unreferenced		->	active,referenced
 * 问：那么是怎么移动到lru上，即inactive list上的呢？
 */
void mark_page_accessed(page_t *page)
{
	if (!PageActive(page) && PageReferenced(page) && PageLRU(page)) {
		activate_page(page);
		ClearPageReferenced(page);
	} else if (!PageReferenced(page)) {
		SetPageReferenced(page);
	}
}

/**
 * 添加到inactive list cache中
 */
void lru_cache_add(page_t *page)
{
	// pagevec_t *pvec = &lru_add_pvecs;
	pagevec_t *pvec = my_inactive_pvec();

	if(pvec == NULL)
		ER();
	get_page(page);
	if (!pagevec_add(pvec, page))
		pagevec_lru_add(pvec);
	pop_off();
}

/**
 * 添加到active list cache
 */
void lru_cache_add_active(page_t *page)
{
	// struct pagevec *pvec = &lru_add_active_pvecs;
	struct pagevec *pvec = my_active_pvec();

	get_page(page);
	if (!pagevec_add(pvec, page))
		pagevec_lru_add_active(pvec);
	pop_off();
}

/**
 * @brief 所有传入的页引用数减1，如果为0从lru移除并free。
 * 
 */
void put_pages(page_t **pages, int nr){
  int i;

  for(i = 0; i < nr; i++){
    page_t *page = pages[i];
		put_page(page);
  }
}

/*
 * Add the passed pages to the LRU, then drop the caller's refcount
 * on them.  Reinitialises the caller's pagevec.
 * 把pagevec中的页添加到inactive list
 */
void pagevec_lru_add(struct pagevec *pvec)
{
	int i;
	struct zone *zone = &memory_zone;

	if(!pagevec_count(pvec))
		return;

	spin_lock(&zone->lru_lock);
	for (i = 0; i < pagevec_count(pvec); i++) {
		page_t *page = pvec->pages[i];

		if (TestSetPageLRU(page))
      ER();
		add_page_to_inactive_list(zone, page);
		/* 可能这个页要回收，put_page放在里面的话会有锁的竞争问题 */
		// put_page(page);
	}
	spin_unlock(&zone->lru_lock);

	/* 减去lru_cache_add的get_page */
	put_pages(pvec->pages, pvec->nr);
	pagevec_reinit(pvec);
}

/*
 * pagevec添加到active list
 */
void pagevec_lru_add_active(struct pagevec *pvec)
{
	int i;
	struct zone *zone = &memory_zone;

	if(!pagevec_count(pvec))
		return;

  spin_lock(&zone->lru_lock);
	for (i = 0; i < pagevec_count(pvec); i++) {
		page_t *page = pvec->pages[i];
	
		if (TestSetPageLRU(page))
      ER();
		if (TestSetPageActive(page))
      ER();
		add_page_to_active_list(zone, page);
	}
  spin_unlock(&zone->lru_lock);

	put_pages(pvec->pages, pvec->nr);
	pagevec_reinit(pvec);
}

/**
 * @brief 把在pagevec缓冲区的页添加到相应的链表中
 * 
 */
void lru_add_drain(void)
{
	// struct pagevec *pvec = &(lru_add_pvecs);
	struct pagevec *pvec = my_inactive_pvec();

	if (pagevec_count(pvec))
		pagevec_lru_add(pvec);

	pop_off();
	// pvec = &(lru_add_active_pvecs);
	pvec = my_active_pvec();

	if (pagevec_count(pvec))
		pagevec_lru_add_active(pvec);
	pop_off();
}

/*
 * The pages which we're about to release may be in the deferred lru-addition
 * queues.  That would prevent them from really being freed right now.  That's
 * OK from a correctness point of view but is inefficient - those pages may be
 * cache-warm and we want to give them back to the page allocator ASAP.
 *
 * So __pagevec_release() will drain those queues here.  pagevec_lru_add()
 * and pagevec_lru_add_active() call put_pages() directly to avoid
 * mutual recursion.
 * 要释放的页可能还在pagevec中，所以调用lru_add_drain
 */
void pagevec_release(struct pagevec *pvec)
{
	if(!pagevec_count(pvec))
		return;
	lru_add_drain();
	put_pages(pvec->pages, pagevec_count(pvec));
	pagevec_reinit(pvec);
}