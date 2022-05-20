#include "common.h"
#include "platform.h"
#include "mm/alloc.h"
#include "fs/fs.h"
#include "fs/file.h"
#include "mm/vm.h"
#include "kernel/proc.h"
#include "defs.h"

#define __MODULE_NAME__ PAGEFAULT
#include "debug.h"

/**
 * @brief read file into memory when page fault hapened
 * 
 * @return int 
 */
int mmap_fetch(uint64 address){
  struct mm_struct *mm = myproc()->mm;
  struct vm_area_struct *area = mm->mmap;
  uint64 pgoff, endoff, size;

  //find area that contains the address
  while(area){
    if(area->vm_start <= address && area->vm_end > address)
      break;
    area = area->vm_next;
  }
  struct file *file = area->vm_file;
  struct address_space *mmaping = 

  pgoff = ((address - area->vm_start) >> PAGE_CACHE_SHIFT) + area->vm_pgoff;
	endoff = ((area->vm_end - area->vm_start) >> PAGE_CACHE_SHIFT) + area->vm_pgoff;
  size = 


  return 0;
}

/**
 * @return -1 if exception unhandled else 0
*/
int handle_pagefault(uint64_t scause) {
    // printf(grn("%d\n"), scause);
    proc_t *p = myproc();
    uint64_t va = read_csr(stval);

    // illegal address
    if(va >= p->cur_mmap_sz) 
        goto bad;

    // 地址翻译与访问顺序为：VMA ---> MMU ---> PMA ---> PMP ---> ACCESSED
    // 在特权级1.12下，所有与MMU相关的错误都将触发xx_page_fault
    // 而对于PMP(Physical Memory Protection)相关的错误，都将触发xx_access_fault。
    // 在特权级1.9下，由于没有pagefault(ref: p51)，因此SBI帮我在底层做了一下转换
    // 对于缺页的错误，它将非缺页以及非PMP访问的异常都归结到了xx_access_fault中，因此这里需要加一层宏判断
    
    #if PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_12
    if(scause == EXCP_STORE_PAGE_FAULT)
    #elif PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_9
    if(scause == EXCP_STORE_FAULT)
    #else
    if(0)
    #endif
    { // store page fault
        // cow
        if(cow_copy(p->pagetable, va, NULL) == -1){
            if(mmap_fetch(va) == -1)
              goto bad;
        }
        return 0;
    }
    
    

    if(scause == EXCP_LOAD_PAGE_FAULT)
    { 
        mmap_fetch(va);
        return 0;
    }

    return -1;

    bad:
    debug("page fault va is %lx sepc is %lx", va, r_sepc());
    p->killed = 1;
    return 0;
}