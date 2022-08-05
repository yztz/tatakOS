/**
 * This file define the basic operations for pages.
*/
#include "mm/page.h"
#include "mm/alloc.h"
#include "atomic/spinlock.h"
#include "defs.h"
#include "printf.h"
#include "param.h"

#define __MODULE_NAME__ PAGE
#include "debug.h"

#include "atomic/sleeplock.h"
#include "utils.h"
#include "page-flags.h"
#include "list.h"

page_t pages[PAGE_NUMS];
struct spinlock reflock;

extern void wakeup(void *chan);
extern void sleep(void *chan, struct spinlock *lk);

void reset_page(page_t *page){
  page->flags = 0;
  INIT_LIST_HEAD(&page->lru);
  page->pte.direct = 0;
  page->mapping = 0;
  page->index = 0;
}

void page_init(void) {
  initlock(&reflock, "reflock");
  for(int i = 0; i < PAGE_NUMS; i++) {
    pages[i].refcnt = 1;
    pages[i].order = 0;
    pages[i].alloc = 1;
    reset_page(&pages[i]);
    // pages[i].flags = 0;
    // INIT_LIST_HEAD(&pages[i].lru);
    // pages[i].pte.direct = 0;
    // pages[i].mapping = 0;
    // pages[i].index = 0;
  }
}

pgref_t ref_page(uint64_t pa) {
  pgref_t ret;
  acquire(&reflock);
  ret = ++pages[PAGE2NUM(pa)].refcnt;
  release(&reflock);
  return ret;
}

pgref_t deref_page(uint64_t pa) {
  pgref_t ret;
  acquire(&reflock);
  ret = --pages[PAGE2NUM(pa)].refcnt;
  release(&reflock);
  return ret;
}

pgref_t page_ref(uint64_t pa) {
  return pages[PAGE2NUM(pa)].refcnt;
}

void mark_page(uint64_t pa, int type) {
  pages[PAGE2NUM(pa)].type = type;
}

int page_type(uint64_t pa) {
  return pages[PAGE2NUM(pa)].type;
}


pte_t *
_walk(pagetable_t pagetable, uint64 va, int alloc, int pg_spec)
{
  if(va >= MAXVA)
    panic("walk");

  for(int level = 2; level > pg_spec; level--) {
    pte_t *pte = &pagetable[PX(level, va)];
    if(*pte & PTE_V) {
      pagetable = (pagetable_t)PTE2PA(*pte);
    } else {
      if(!alloc || (pagetable = (pde_t*)kzalloc(PGSIZE)) == 0)
        return 0;
      *pte = PA2PTE(pagetable) | PTE_V;
    }
  }
  return &pagetable[PX(pg_spec, va)];
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa. va and size might not
// be page-aligned. Returns 0 on success, -1 if walk() couldn't
// allocate a needed page-table page.
int
_mappages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, int prot, int spec)
{
  uint64 start, a, last;
  pte_t *pte;

  if(size == 0)
    panic("mappages: size");
  
  start = a = PGROUNDDOWN_SPEC(va, spec);
  last = PGROUNDDOWN_SPEC(va + size - 1, spec);
  for(;;){
    if((pte = _walk(pagetable, a, 1, spec)) == 0)
      goto bad;
    if(*pte & PTE_V)
      panic("mappages: remap");
    *pte = PA2PTE_SPEC(pa, spec) | prot | PTE_V;
    if(a == last)
      break;
    a += PGSIZE_SPEC(spec);
    pa += PGSIZE_SPEC(spec);
  }
  return 0;

  bad:
  debug("mappages fail");
  _uvmunmap(pagetable, start, (a-start)/PGSIZE_SPEC(spec), 0, spec);
  return -1;
}

// Remove npages of mappings starting from va. va must be
// page-aligned. The mappings must exist.
// Optionally free the physical memory.
void
_uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, int do_free, int spec)
{
  uint64 a;
  pte_t *pte;
  int pgsize = PGSIZE_SPEC(spec);

  if((va % pgsize) != 0)
    panic("uvmunmap: not aligned");

  for(a = va; a < va + npages*pgsize; a += pgsize){
    // printf(ylw("a: %p\n"), a);
    if((pte = _walk(pagetable, a, 0, spec)) == 0){
      continue;
    }
    if((*pte & PTE_V) == 0) {
      continue;
    }
    if((*pte & (PTE_R | PTE_W | PTE_X)) == 0)
      panic("uvmunmap: not a leaf");
    if(do_free){
      uint64 pa = PTE2PA(*pte);
      kfree((void*)pa);
    }
    *pte = 0;
    sfence_vma_addr(a);
  }
}

void pte_print(pte_t *pte) {
  uint64 pa = PTE2PA(*pte);
  char rwxuvc[7];
  rwxuvc[0] = *pte & PTE_R ? 'r' : '-';
  rwxuvc[1] = *pte & PTE_W ? 'w' : '-';
  rwxuvc[2] = *pte & PTE_X ? 'x' : '-';
  rwxuvc[3] = *pte & PTE_U ? 'u' : '-';
  rwxuvc[4] = *pte & PTE_V ? 'v' : '-';
  rwxuvc[5] = *pte & PTE_COW ? 'c' : '-';
  rwxuvc[6] = '\0';
  
  printf("pte %#lx pa %#lx %s\n", *pte, pa, rwxuvc);
}

pgref_t page_refcnt(page_t *page){
  pgref_t ret;
  acquire(&reflock);
  ret = page->refcnt;
  release(&reflock);
  return ret;
}

/**
 * @brief 增加1个引用，返回增加后的
 * 
 */
pgref_t get_page(page_t *page){
  pgref_t ret;
  acquire(&reflock);
  ret = ++page->refcnt;
  release(&reflock);
  return ret;
}

/**
 * @brief refcnt减1，返回最终的refcnt，如果refcnt为0，从
 * 
 */
pgref_t put_page(page_t *page){
  pgref_t ret;

  #ifdef TODO
  todo("use atomic to replace reflock may inprove performance");
  #endif

  if(page->refcnt <= 0)
    ER();
  acquire(&reflock);
  ret = --page->refcnt;
  release(&reflock);
  if(ret == 0){
    zone_t *zone = &memory_zone;
    spin_lock(&zone->lru_lock);
    if(TestClearPageLRU(page))
      del_page_from_lru(zone, page);
    spin_unlock(&zone->lru_lock);
    free_one_page(page);
  }
  return ret; 

}

/**
 * 使用类睡眠锁实现的lock page。
 */
void lock_page(page_t *page){
  page_spin_lock(page);
  while(unlikely(TestSetPageLocked(page))){
    while(PageLocked(page))
      sleep(page, NULL);
  }
  page_spin_unlock(page);
}


void unlock_page(page_t *page){
  page_spin_lock(page);
  if (!TestClearPageLocked(page))
    ER();
  wakeup(page);
  page_spin_unlock(page);
}

void get_lock_page(page_t *page){
  get_page(page);
  lock_page(page);
}

void unlock_put_page(page_t *page){
  unlock_page(page);
  put_page(page);
}

/**
 * @brief 之前flag位是带移位的，原PG_diry = 1 << 现PG_diry 
 * 
 */
void set_page_dirty(uint64_t pa){
  page_t *page = &pages[PAGE2NUM(pa)];

  // page->flags |= PG_dirty;
  SetPageDirty(page);
}


void clear_page_dirty(uint64_t pa){
  page_t *page = &pages[PAGE2NUM(pa)];

  // page->flags &= ~PG_dirty;
  ClearPageDirty(page);
}

int page_is_dirty(uint64_t pa){
  page_t *page = &pages[PAGE2NUM(pa)];

  // return page->flags & PG_dirty;
  return PageDirty(page);
}
