#include "param.h"
#include "mm/vm.h"
#include "defs.h"


static inline int __cow_copy(uint64_t va, pte_t *pte) {
  uint64 pa = PTE2PA(*pte);
  if(page_ref(pa) == 1) {
    *pte |= PTE_W;
    *pte &= ~PTE_COW;
  } else {
    char *mem;
    if((mem = kalloc()) == 0) {
      return -1;
    }
    memmove(mem, (char *)pa, PGSIZE);
    uint flag = PTE_FLAGS(*pte);
    flag |= PTE_W;
    flag &= ~PTE_COW;

    *pte = PA2PTE(mem) | flag;

    kfree((void *)pa);
  }
  // IMPORTANT! Flush TLB
  sfence_vma_addr(va);

  return 0;
}

int cow_copy(pagetable_t pagetable, uint64_t va, pte_t **pppte) {
  pte_t *pte;
  if(va >= MAXVA)
    return -1;
  pte = walk(pagetable, va, 0);
  if(pte == 0 || (*pte & PTE_U) == 0 || (*pte & PTE_V) == 0 || (*pte & PTE_COW) == 0)
    return -1;

  if(pppte) 
    *pppte = pte;

  return __cow_copy(va, pte);
}

// Copy from kernel to user.
// Copy len bytes from src to virtual address dstva in a given page table.
// Return 0 on success, -1 on error.
#include "utils.h"
int
copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len)
{
  uint64 n, va0, pa0;
  pte_t *pte;
  while(len > 0){
    va0 = PGROUNDDOWN(dstva);
    if(va0 >= MAXVA) 
      return -1;
    pte = walk(pagetable, va0, 0);
    if(pte == 0 || (*pte & PTE_U) == 0 || (*pte & PTE_V) == 0)
      return -1;
    
    #ifdef COW
    if((*pte & PTE_COW)) {
      if(__cow_copy(va0, pte))
        return -1;
    }
    #endif
    
    n = PGSIZE - (dstva - va0);
    if(n > len) n = len;
    pa0 = PTE2PA(*pte);
    memmove((void *)(pa0 + (dstva - va0)), src, n);
    // printf("fd=%d\n", *(int *)src);
    len -= n;
    src += n;
    dstva = va0 + PGSIZE;
  }
  return 0;
}

// Copy from user to kernel.
// Copy len bytes to dst from virtual address srcva in a given page table.
// Return 0 on success, -1 on error.
int
copyin(pagetable_t pagetable, char *dst, uint64 srcva, uint64 len)
{
  uint64 n, va0, pa0;

//   #ifndef COMPETITION
//   if()
//   #endif

  while(len > 0){
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    if(pa0 == 0)
      return -1;
    n = PGSIZE - (srcva - va0);
    if(n > len)
      n = len;
    memmove(dst, (void *)(pa0 + (srcva - va0)), n);

    len -= n;
    dst += n;
    srcva = va0 + PGSIZE;
  }
  return 0;
}

// Copy a null-terminated string from user to kernel.
// Copy bytes to dst from virtual address srcva in a given page table,
// until a '\0', or max.
// Return 0 on success, -1 on error.
int
copyinstr(pagetable_t pagetable, char *dst, uint64 srcva, uint64 max)
{
  uint64 n, va0, pa0;
  int got_null = 0;

  while(got_null == 0 && max > 0){
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    if(pa0 == 0)
      return -1;
    n = PGSIZE - (srcva - va0);
    if(n > max)
      n = max;

    char *p = (char *) (pa0 + (srcva - va0));
    while(n > 0){
      if(*p == '\0'){
        *dst = '\0';
        got_null = 1;
        break;
      } else {
        *dst = *p;
      }
      --n;
      --max;
      p++;
      dst++;
    }

    srcva = va0 + PGSIZE;
  }
  if(got_null){
    return 0;
  } else {
    return -1;
  }
}
