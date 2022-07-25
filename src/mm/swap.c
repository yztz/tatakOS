#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "mm/vm.h"
#include "param.h"
#include "fs/stat.h"
#include "fs/fs.h"
#include "atomic/spinlock.h"
#include "kernel/proc.h"
#include "atomic/sleeplock.h"
#include "fs/file.h"
#include "fs/fcntl.h"
#include "mm/page.h"

// #define QUIET
#define __MODULE_NAME__ SWAP
#include "debug.h"

#include "mm/mm_types.h"
#include "mm/mm.h"
#include "rbtree.h"
#include "utils.h"
#include "memlayout.h"
#include "list.h"
#include "kthread.h"
#include "mm/page.h"
#include "pagevec.h"

zone_t memory_zone = (zone_t){.lru_lock = {.locked=0, .name="zone lock", .cpu=NULL}, 
                              .active_list = {&memory_zone.active_list, &memory_zone.active_list}, 
                              .inactive_list = {&memory_zone.inactive_list, &memory_zone.inactive_list}, 
                              .nr_active = 0, 
                              .nr_inactive = 0};

pagevec_t lru_add_pvecs = {.nr = 0, .cold = 0}, 
          lru_add_active_pvecs = {.nr = 0, .cold = 0};

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
 * @brief 所有传入的页引用数减1，如果为0从lru移除并free。
 * 
 */
void release_pages(page_t **pages, int nr, int cold){
  int i;
  // pagevec_t pages_to_free;
  zone_t *zone = &memory_zone;

  // pagevec_init(&pages_to_free, cold);


  for(i = 0; i < nr; i++){
    page_t *page = pages[i];
    /* 在我们的机制中，对于一个页直接调用kfree，它会调用buddy_free，buddy_free会减去引用数
      然后根据引用数是否为0决定回收这个页。 */
    if(!put_page(page)) 
      continue;

    if(TestClearPageLRU(page)){
      spin_lock(&zone->lru_lock);
      del_page_from_lru(zone, page);
      spin_unlock(&zone->lru_lock);
    }

    // if(page->refcnt == 0){
    //   /* 满了（14页），先free掉 */
    //   if(!pagevec_add(&pages_to_free, page)){
    //     __pagevec_free(&pages_to_free);
    //     pagevec_reinit(&pages_to_free);
    //   }
    // }
  }

  // pagevec_free(&pages_to_free);
}

/*
 * Add the passed pages to the LRU, then drop the caller's refcount
 * on them.  Reinitialises the caller's pagevec.
 */
void __pagevec_lru_add(struct pagevec *pvec)
{
	int i;
	struct zone *zone = &memory_zone;

	spin_lock(&zone->lru_lock);
	for (i = 0; i < pagevec_count(pvec); i++) {
		page_t *page = pvec->pages[i];

		if (TestSetPageLRU(page))
      ER();
		add_page_to_inactive_list(zone, page);
	}
	spin_unlock(&zone->lru_lock);

  /* 需要释放页吗？*/
	release_pages(pvec->pages, pvec->nr, pvec->cold);
	pagevec_reinit(pvec);
}

void __pagevec_lru_add_active(struct pagevec *pvec)
{
	int i;
	struct zone *zone = &memory_zone;

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

	release_pages(pvec->pages, pvec->nr, pvec->cold);
	pagevec_reinit(pvec);
}

/**
 * @brief 把在pagevec缓冲区的页添加到相应的链表中
 * 
 */
void lru_add_drain(void)
{
	struct pagevec *pvec = &(lru_add_pvecs);

	if (pagevec_count(pvec))
		__pagevec_lru_add(pvec);
	pvec = &(lru_add_active_pvecs);
	if (pagevec_count(pvec))
		__pagevec_lru_add_active(pvec);
}

/*
 * The pages which we're about to release may be in the deferred lru-addition
 * queues.  That would prevent them from really being freed right now.  That's
 * OK from a correctness point of view but is inefficient - those pages may be
 * cache-warm and we want to give them back to the page allocator ASAP.
 *
 * So __pagevec_release() will drain those queues here.  __pagevec_lru_add()
 * and __pagevec_lru_add_active() call release_pages() directly to avoid
 * mutual recursion.
 */
void __pagevec_release(struct pagevec *pvec)
{
	lru_add_drain();
	release_pages(pvec->pages, pagevec_count(pvec), pvec->cold);
	pagevec_reinit(pvec);
}