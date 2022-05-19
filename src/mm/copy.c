#include "utils.h"
#include "mm/vm.h"
#include "defs.h"
#include "platform.h"

/* 复制COW页 */
static inline int __cow_copy(uint64_t va, pte_t *pte) {
  uint64 pa = PTE2PA(*pte);
  if(page_ref(pa) == 1) { // 如果页引用数为1，则直接设置为可写，取消COW标志
    *pte |= PTE_W;
    *pte &= ~PTE_COW;
  } else {
    char *mem;
    if((mem = kalloc()) == 0) {
      return -1;
    }
    // 复制页
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

#define check_range(va, n, limit, mmap_limit) \
  (((uint64_t)(va) + (uint64_t)(n) > (uint64_t)(va)) && \
  (((uint64_t)(va) + (uint64_t)(n) <= (uint64_t)(limit)) || \
  (((uint64_t)(va) + (uint64_t)(n) >= MMAP_BASE) && ((uint64_t)(va) + (uint64_t)(n) <= mmap_limit))))

// Copy from user to kernel.
// Copy len bytes to dst from virtual address srcva in a given page table.
// Return 0 on success, -1 on error.
int
copyin(pagetable_t pagetable, char *dst, uint64 srcva, uint64 len)
{
  return copy_from_user(dst, (void *)srcva, len);
}

/* We always think kernel address is legal... */
int copy_from_user(void *to, void *from, size_t n) {
  proc_t *p = myproc();
  if(!p)
    panic("copy_from_user: no process ctx");

  if(!check_range(from, n, p->sz, p->cur_mmap_sz))
    return -1;
  // todo: more checks, such as: guard pages, **mmap**...
  
  /* 在特权级1.9版本中，SUM位为PUM为，其功能位与SUM作用相反 */
  #if PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_12
  enable_sum();
  #endif
  memmove(to, from, n);
  #if PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_12
  disable_sum();
  #endif
  return 0;
}

// Copy a null-terminated string from user to kernel.
// Copy bytes to dst from virtual address srcva in a given page table,
// until a '\0', or max.
// Return 0 on success, -1 on error.
int
copyinstr(pagetable_t pagetable, char *dst, uint64 srcva, uint64 max)
{
  int got_null = 0;
  proc_t *proc = myproc();
  if(!proc)
    panic("copyinstr: no process context");
  #if PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_12
  enable_sum();
  #endif
  char *p = (char *)srcva;
  // no consider wrap
  while(max > 0 && (uint64_t)p < proc->sz){
    *dst = *p;
    if(*p == '\0'){
      got_null = 1;
      break;
    }
    max--;
    p++;
    dst++;
  }
  #if PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_12
  disable_sum();
  #endif
  return (got_null ? 0 : -1);
}