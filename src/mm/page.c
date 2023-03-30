/**
 * This file define the basic operations for pages.
*/
#include "mm/page.h"
#include "mm/alloc.h"
#include "atomic/spinlock.h"
#include "atomic/sleeplock.h"
#include "common.h"

#define __MODULE_NAME__ PAGE
#include "debug.h"

page_t pages[PAGE_NUMS];
struct spinlock reflock;
 
extern void __free_pages(page_t *first_page);

void reset_page(page_t *page) {
  page->flags = 0;
  INIT_LIST_HEAD(&page->lru);
  page->mapping = 0;
  page->index = 0;
#ifdef RMAP
  page->pte.direct = 0;
#endif
}

void page_init(void) {
  initlock(&reflock, "reflock");
  for(int i = 0; i < PAGE_NUMS; i++) {
    pages[i].refcnt = (atomic_t){0};
    pages[i].order = 0;
    pages[i].alloc = 1;
    reset_page(&pages[i]);
  }
}


pte_t *__walk(pagetable_t pagetable, uint64_t va, int alloc, int pg_spec) {
    if(va >= MAXVA) {
        panic("walk");
    }

    // end level just equals pg_spec...
    // so it's so tricky way
    for(int level = 2; level > pg_spec; level--) {
        pte_t *pte = &pagetable[PX(level, va)];
        if(*pte & PTE_V) {
            pagetable = (pagetable_t)PTE2PA(*pte);
        } else {
            if(!alloc || (pagetable = (pde_t*)kzalloc(PGSIZE)) == 0) {
                return 0;
            }
                
            *pte = PA2PTE(pagetable) | PTE_V;
        }
    }

    return &pagetable[PX(pg_spec, va)];
}

void freewalk(pagetable_t pagetable) {
    // there are 2^9 = 512 PTEs in a page table.
    for (int i = 0; i < 512; i++) {
        pte_t pte = pagetable[i];
        if ((pte & PTE_V) && (pte & (PTE_R | PTE_W | PTE_X)) == 0) { // 目录节点
            // this PTE points to a lower-level page table.
            uint64_t child = PTE2PA(pte);
            freewalk((pagetable_t)child);
            pagetable[i] = 0;
        } else if (pte & PTE_V) { // 叶子节点
            kprintf("pa: %p\n", PTE2PA(pte));
            panic("freewalk: leaf");
        }
    }

    kfree((void *)pagetable);
}


int __map_pages(pagetable_t pagetable, uint64_t __va, uint64_t size, uint64_t pa, int prot, int spec) {
    prot = riscv_map_prot(prot);
    
    uint64_t begin = PGROUNDDOWN_SPEC(__va, spec);
    uint64_t end = PGROUNDUP_SPEC(__va + size, spec);
    uint64_t pagesize = PGSIZE_SPEC(spec);

    uint64_t va;
    for(va = begin; va < end; va+=pagesize, pa+=pagesize) {
        pte_t *pte;
        // No PTE found
        if((pte = __walk(pagetable, va, 1, spec)) == 0) {
            goto bad;
        }

        if(*pte & PTE_V) {
            pte_print(pte);
            panic("remap");
        }
        *pte = PA2PTE_SPEC(pa, spec) | prot | PTE_V;
    }

    return 0;

    bad:
    warn("mappages fail");
    __unmap_pages(pagetable, begin, va-begin, 0, spec);
    
    return -1;
}


// IMPROVE ME: ASID
void __unmap_pages(pagetable_t pagetable, uint64_t __va, uint64_t size, int do_free, int spec) {
    uint64_t begin = PGROUNDDOWN_SPEC(__va, spec);
    uint64_t end = PGROUNDUP_SPEC(__va + size, spec);
    uint64_t pagesize = PGSIZE_SPEC(spec);
    int need_flush = get_pgtbl() == pagetable;

    uint64_t va;
    for(va = begin; va < end; va+=pagesize) {
        pte_t *pte;
        // No PTE found
        if((pte = __walk(pagetable, va, 0, spec)) == 0){
            continue;
        }
        if((*pte & PTE_V) == 0) {
            continue;
        }
        if((*pte & (PTE_R | PTE_W | PTE_X)) == 0) {
            panic("uvmunmap: not a leaf");
        }
        if(do_free){
            uint64_t pa = PTE2PA(*pte);
            put_page(pa);
        }

        *pte = 0;

        if(need_flush) {
            sfence_vma_addr(va);
        } 
    }
}

void pte_print(pte_t *pte) {
  uint64_t pa = PTE2PA(*pte);
  char rwxuvc[7];
  rwxuvc[0] = *pte & PTE_R ? 'r' : '-';
  rwxuvc[1] = *pte & PTE_W ? 'w' : '-';
  rwxuvc[2] = *pte & PTE_X ? 'x' : '-';
  rwxuvc[3] = *pte & PTE_U ? 'u' : '-';
  rwxuvc[4] = *pte & PTE_V ? 'v' : '-';
  rwxuvc[5] = *pte & PTE_COW ? 'c' : '-';
  rwxuvc[6] = '\0';
  
  kprintf("pte %#lx pa %#lx %s\n", *pte, pa, rwxuvc);
}


pgref_t __page_refcnt_pointer(page_t *page) {
  return atomic_get(&page->refcnt);
}

pgref_t __page_refcnt_paddr(uint64_t pa) {
  return __page_refcnt_pointer(ADDR_TO_PG(pa));
}


pgref_t __deref_page_pointer(page_t *page) {
  return atomic_dec(&page->refcnt) - 1;
}

pgref_t __deref_page_paddr(uint64_t pa) {
  return __deref_page_pointer(&pages[PG_TO_NR(pa)]);
}

pgref_t __get_page_pointer(page_t *page) {
  return atomic_inc(&page->refcnt) + 1;
}

pgref_t __get_page_paddr(uint64_t pa) {
  return __get_page_pointer(&pages[PG_TO_NR(pa)]);
}


pgref_t __put_page_pointer(page_t *page) {
  pgref_t ret;

  if(page_refcnt(page) <= 0)
    ER();

  ret = atomic_dec(&page->refcnt) - 1;
  if(ret == 0) {
    __free_pages(page);
  }
  return ret; 

}

pgref_t __put_page_padder(uint64_t pa) {
  return __put_page_pointer(&pages[PG_TO_NR(pa)]);
}

/**
 * 使用类睡眠锁实现的lock page。
 */
void lock_page(page_t *page) {
  page_spin_lock(page);
  while(unlikely(TestSetPageLocked(page))){
    while(PageLocked(page))
      sleep(page, NULL);
  }
  page_spin_unlock(page);
}


void unlock_page(page_t *page) {
  page_spin_lock(page);
  if (!TestClearPageLocked(page))
    ER();
  wakeup(page);
  page_spin_unlock(page);
}

void get_lock_page(page_t *page) {
  get_page(page);
  lock_page(page);
}

void unlock_put_page(page_t *page) {
  unlock_page(page);
  put_page(page);
}

/**
 * @brief 之前flag位是带移位的，原PG_diry = 1 << 现PG_diry 
 * 
 */
void set_page_dirty(uint64_t pa) {
  page_t *page = &pages[PG_TO_NR(pa)];

  // page->flags |= PG_dirty;
  SetPageDirty(page);
}


void clear_page_dirty(uint64_t pa) {
  page_t *page = &pages[PG_TO_NR(pa)];

  ClearPageDirty(page);
}

int page_is_dirty(uint64_t pa) {
  page_t *page = &pages[PG_TO_NR(pa)];

  return PageDirty(page);
}
