#include "utils.h"
#include "mm/vm.h"
#include "mm/mmap.h"
#include "defs.h"
#include "platform.h"
#include "kernel/proc.h"

#define __MODULE_NAME__ COPY
#include "debug.h"


int copy_to_user(uint64 dstva, void *src, uint64 len) {
  vma_t *vma;
  proc_t *p = myproc();
  if(!p)
    panic("copyout: no process ctx");
  
  // 1. 首先确定目标段是否存在
  if((vma = vma_exist(p->mm, (uint64)dstva, len)) == NULL) {
    return -1;
  }
  // 2. 是否是用户段
  if((vma->prot & MAP_PROT_USER) == 0) {
    return -1;
  }
  // 3. 直接拷贝 
  #if PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_12
  enable_sum();
  #endif
  // memmove((void *)dstva, src, len);
  char *s = (char *)src;
  char *d = (char *)dstva;
  if(s < d && s + len > d){
    s += len;
    d += len;
    while(len-- > 0)
      *--d = *--s;
  } else
    while(len-- > 0)
      *d++ = *s++;
  #if PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_12
  disable_sum();
  #endif
  return 0;
}

// Copy from kernel to user.
// Copy len bytes from src to virtual address dstva in a given page table.
// Return 0 on success, -1 on error.
int
copyout(uint64 dstva, char *src, uint64 len)
{
  return copy_to_user(dstva, src, len);
}

/* We trust that kernel address is legal... */
int copy_from_user(void *to, uint64 from, size_t n) {
  proc_t *p = myproc();
  if(!p)
    panic("copy_from_user: no process ctx");

  // if(!check_range(from, n, p->sz, p->cur_mmap_sz))
  //   return -1;
  // todo: more checks, such as: guard pages, **mmap**...
  if(vma_exist(p->mm, (uint64)from, n) == NULL) {
    debug("not exist");
    return -1;
  }

  /* 在特权级1.9版本中，SUM位为PUM为，其功能位与SUM作用相反 */
  #if PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_12
  enable_sum();
  #endif
  // memmove(to, (void *)from, n);
  char *s = (char *)from;
  char *d = (char *)to;
  if(s < d && s + n > d){
    s += n;
    d += n;
    while(n-- > 0)
      *--d = *--s;
  } else
    while(n-- > 0)
      *d++ = *s++;
  #if PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_12
  disable_sum();
  #endif
  return 0;
}

// Copy from user to kernel.
// Copy len bytes to dst from virtual address srcva in a given page table.
// Return 0 on success, -1 on error.
int
copyin(void *dst, uint64 srcva, uint64 len)
{
  return copy_from_user(dst, srcva, len);
}

// Copy a null-terminated string from user to kernel.
// Copy bytes to dst from virtual address srcva in a given page table,
// until a '\0', or max.
// Return 0 on success, -1 on error.
int
copyinstr(char *dst, uint64 srcva, uint64 max)
{
  int got_null = 0;
  proc_t *proc = myproc();
  vma_t *vma;
  if(!proc)
    panic("copyinstr: no process context");
  if((vma = __vma_find_strict(proc->mm, srcva)) == NULL) {
    return -1;
  }
  max = min(max, vma->len - (srcva - vma->addr));

  #if PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_12
  enable_sum();
  #endif
  char *p = (char *)srcva;
  // no consider wrap
  while(max > 0){
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