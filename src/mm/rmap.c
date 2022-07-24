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
#define __MODULE_NAME__ RMAP
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
#include "swap.h"
#include "writeback.h"
#include "mm/rmap.h"
#include "page-flags.h"

/**
 * @brief 反向映射模块，参考linux2.6.0
 * 
 */

/**
 * @brief next_and_idx低位用来存idx，高位存next地址。
 * 
 */
static inline struct pte_chain *pte_chain_next(struct pte_chain *pte_chain)
{
	return (struct pte_chain *)(pte_chain->next_and_idx & ~NRPTE);
}

static inline int pte_chain_idx(struct pte_chain *pte_chain)
{
	return pte_chain->next_and_idx & NRPTE;
}

/**
 * page_referenced - test if the page was referenced
 * @page: the page to test
 *
 * Quick test_and_clear_referenced for all mappings to a page,
 * returns the number of processes which referenced the page.
 * Caller needs to hold the pte_chain_lock.
 *
 * If the page has a single-entry pte_chain, collapse that back to a PageDirect
 * representation.  This way, it's only done under memory pressure.
 */
int page_referenced(page_t *page){
  pte_chain_t *pc;
  int referenced = 0;

  if (TestClearPageReferenced(page))
		referenced++;

  /* 映射了一次，清掉valid位 */
  if (PageDirect(page)) {
    pte_t *pte = (pte_t *)(page->pte.direct);
    if(ptep_test_and_clear_valid(pte))
			referenced++;
	} else {
    int nr_chains = 0;
    /* Check all the page tables mapping this page. */
    for (pc = page->pte.chain; pc; pc = pte_chain_next(pc)) {
      int i;
      
      for(int i = NRPTE-1; i >= 0; i--){
        pte_addr_t pte_paddr = pc->ptes[i];
        pte_t *p;

        if(!pte_paddr)
          break;
        p = (pte_t *)pte_paddr;
        if(ptep_test_and_clear_valid(p))
          referenced++;

        nr_chains++;
      }
    }
    if (nr_chains == 1) {
			pc = page->pte.chain;
      /* 在pte_chain中时倒放的，从NRPTE开始 */
			page->pte.direct = pc->ptes[NRPTE-1];
			SetPageDirect(page);
			pc->ptes[NRPTE-1] = 0;
			kfree(pc);
		}
  } 

	return referenced;
}

/**
 * try_to_unmap_one - worker function for try_to_unmap
 * @page: page to unmap
 * @ptep: page table entry to unmap from page
 * 问题：不知道包含pte的页表属于哪个address space（mm_strcut），也不知道其
 * 对应的虚拟地址，这样子不知道是否需要刷新当前的tlb及其对应的地址，但是是否有必要
 * 保存这两项？
 * 
 * 最好的做法是得到这两项，然后在这里刷新对应地址的tlb
 */
static int try_to_unmap_one(page_t *page, pte_addr_t paddr)
{
  pte_t *ptep = (pte_t *)(paddr);
	pte_t pte;

	/* clear pte */
	pte = *ptep;
	*ptep = 0;

	/* Move the dirty bit to the physical page now the pte is gone. */
	if(pte_dirty(pte))
		set_page_diry(page);

	/* 这里释放页 */
	put_page(page);
	return 0;
}

/**
 * try_to_unmap - try to remove all page table mappings to a page
 * @page: the page to get unmapped
 *
 * Tries to remove all the page table entries which are mapping this
 * page, used in the pageout path.  Caller must hold the page lock
 * and its pte chain lock.  Return values are:
 */
int try_to_unmap(page_t * page)
{
	struct pte_chain *pc, *next_pc, *start;
	int victim_i = -1;

	// /* This page should not be on the pageout lists. */
	// if (PageReserved(page))
	// 	BUG();
	if (!PageLocked(page))
    ER();
	/* We need backing store to swap out a page. */
	if (!page->mapping)
    ER();

	/* one mappig */
	if (PageDirect(page)) {
		try_to_unmap_one(page, page->pte.direct);
		page->pte.direct = 0;
		ClearPageDirect(page);
		goto out;
	}

	/* many mappings */
	start = page->pte.chain;
	for(pc = start; pc; pc = next_pc){
		int i;

		next_pc = pte_chain_next(pc);
		for(i = pte_chain_idx(pc); i < NRPTE; i++){
			pte_addr_t pte_addr = pc->ptes[i];

			if(!pte_addr)
				continue;
			
			try_to_unmap_one(page, pte_addr);
		}

		page->pte.chain = pte_chain_next(start);
		kfree(start);
		start = page->pte.chain;
	}

out:
	if (!page_mapped(page))
		dec_page_state(nr_mapped);
	return 0;
}

