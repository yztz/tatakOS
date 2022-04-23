/**
 * This file define the basic operations for pages.
*/
#include "mm/page.h"
#include "mm/alloc.h"
#include "atomic/spinlock.h"
#include "defs.h"
#include "printf.h"
#include "param.h"

page_t pages[PAGE_NUMS];
struct spinlock reflock;

void page_init(void) {
  initlock(&reflock, "reflock");
  for(int i = 0; i < PAGE_NUMS; i++) {
    pages[i].refcnt = 0;
    #ifdef BUDDY
    pages[i].order = 0;
    pages[i].alloc = 1;
    #endif
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
      if(!alloc || (pagetable = (pde_t*)kalloc()) == 0)
        return 0;
      memset(pagetable, 0, PGSIZE);
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
_mappages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, int perm, int spec)
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
    *pte = PA2PTE_SPEC(pa, spec) | perm | PTE_V;
    if(a == last)
      break;
    a += PGSIZE_SPEC(spec);
    pa += PGSIZE_SPEC(spec);
  }
  return 0;

  bad:
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
    if((pte = _walk(pagetable, a, 0, spec)) == 0){
      panic("uvmunmap: walk");
    }
    if((*pte & PTE_V) == 0){
      panic("uvmunmap: not mapped");
    }
    if((*pte & (PTE_R | PTE_W | PTE_X)) == 0)
      panic("uvmunmap: not a leaf");
    if(do_free){
      uint64 pa = PTE2PA_SPEC(*pte, spec);
      kfree((void*)pa);
    }
    *pte = 0;
  }
}