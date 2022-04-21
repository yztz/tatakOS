/**
 * This file define the basic operations for pages.
*/
#include "mm/page.h"
#include "mm/alloc.h"
#include "defs.h"
#include "printf.h"
#include "param.h"

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
      //printf("a is %p\n", a);
      panic("uvmunmap: walk");
    }
    if((*pte & PTE_V) == 0){
      printf("va is: %p\n", a);
      panic("uvmunmap: not mapped");
    }
    if(PTE_FLAGS(*pte) == PTE_V)
      panic("uvmunmap: not a leaf");
    if(do_free){
      uint64 pa = PTE2PA_SPEC(*pte, spec);
      kfree((void*)pa);
    }
    *pte = 0;
  }
}