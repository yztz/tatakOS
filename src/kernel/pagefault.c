#include "common.h"
#include "platform.h"
#include "mm/alloc.h"
#include "fs/fs.h"
#include "fs/file.h"
#include "mm/vm.h"
#include "kernel/proc.h"
#include "defs.h"
#include "mm/mm.h"

#define __MODULE_NAME__ PAGEFAULT

#include "debug.h"

extern char cp_start;
extern char cp_end;


extern int filemap_nopage(pte_t *pte, vma_t *area, uint64_t address);

/* 复制COW页 */
static inline int cow_copy(uint64_t va, pte_t *pte) {
  uint64 pa = PTE2PA(*pte);
  if(page_refcnt(pa) == 1) { // 如果页引用数为1，则直接设置为可写，取消COW标志
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

    /* 引用数-1 */
    kfree((void *)pa);
  }
  // IMPORTANT! Flush TLB
  sfence_vma_addr(va);

  return 0;
}


// static inline void file_copy(uint64_t va, uint64_t pa, vma_t *vma) {
//     // TODO: check file range
//     uint64_t len = PGSIZE;
//     off_t off = vma->offset;
//     struct file *fp = vma->map_file;
//     if(va == vma->addr) {
//         uint64_t delta = vma->raddr - vma->addr;
//         pa = pa + delta;
//         len = len - delta;
//     } else {
//         off += va - vma->raddr;
//     }
//     elock(fp->ep);
//     reade(fp->ep, 0, pa, off, len);
//     eunlock(fp->ep);
// }

typedef enum {
    PF_LOAD,
    PF_STORE,
    PF_UNKNOWN,
} pagefault_t;

static pagefault_t get_pagefault(uint64 scause) {
    // 地址翻译与访问顺序为：VMA ---> MMU ---> PMA ---> PMP ---> ACCESSED
    // 在特权级1.12下，所有与MMU相关的错误都将触发xx_page_fault
    // 而对于PMP(Physical Memory Protection)相关的错误，都将触发xx_access_fault。
    // 在特权级1.9下，由于没有pagefault(ref: p51)，因此SBI帮我在底层做了一下转换
    // 对于缺页的错误，它将非缺页以及非PMP访问的异常都归结到了xx_access_fault中，因此这里需要加一层宏判断
    // 这似乎违背了SBI的宗旨
    switch (scause)
    {
        // #if PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_12
        case EXCP_STORE_PAGE_FAULT:return PF_STORE;
        // #elif PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_9
        case EXCP_STORE_FAULT:return PF_STORE;
        // #endif
        case EXCP_LOAD_PAGE_FAULT:return PF_LOAD;
        case EXCP_LOAD_FAULT:return PF_LOAD;
        default:return PF_UNKNOWN;
    }
}
#include "mm/buddy.h"

// /**
//  * @return 处理失败返回-1，成功返回1
//  */
// int __handle_pagefault(pagefault_t fault, proc_t *p, vma_t *vma, uint64 rva) {
//     if(fault == PF_STORE) { // store page fault
//         if(vma->prot & PROT_WRITE) {
//             pte_t *pte = walk(p->mm->pagetable, rva, 1);
//             // lazy
//             if((*pte & PTE_V) == 0) {
//                 uint64_t newpage = (uint64)kzalloc(PGSIZE);

//                 if(newpage == 0) {
//                     debug("map fault");
//                     return -1;
//                 }
//                 // load file content
//                 if(vma->map_file) {
//                     file_copy(rva, newpage, vma);
//                 }

//                 *pte = PA2PTE(newpage) | riscv_map_prot(vma->prot) | PTE_V;
//                 sfence_vma_addr(rva);
                
//                 return 0;
//             }
//             // cow
//             if(*pte & PTE_COW) {
//                 if(cow_copy(rva, pte) == -1){
//                     debug("cow fault");
//                     return -1;
//                 } else {
//                     return 0;
//                 }
//             }

//             panic("here?");
//         } else {
//             debug("no write prot");
//             return -1;
//         }
//     }
    
//     if(fault == PF_LOAD) { 
//         if(vma->prot & PROT_READ) {
//             pte_t *pte = walk(p->mm->pagetable, rva, 1);
//             // lazy
//             if((*pte & PTE_V) == 0) {
//                 uint64_t newpage = (uint64)kzalloc(PGSIZE);
//                 if(newpage == 0) {
//                     return -1;
//                 }
//                 // load file content
//                 if(vma->map_file) {
//                     file_copy(rva, newpage, vma);
//                 }
//                 *pte = PA2PTE(newpage) | riscv_map_prot(vma->prot) | PTE_V;
//                 sfence_vma_addr(rva);
//                 return 0;
//             }
//         } else {
//             return -1;
//         }
//     }

//     return -1;
// }

static int have_prot(pagefault_t fault, vma_t *vma){
    if(fault == PF_STORE){
        if(vma->prot & PROT_WRITE)
            return 1;
    }
    if(fault == PF_LOAD){
        if(vma->prot & PROT_READ)
            return 1;
    }
    return 0;
}

static int do_filemap_page(pte_t *pte, vma_t *vma, uint64_t address){
    return filemap_nopage(pte, vma, address);
}

static int do_swap_page(){
    ER();
    return 1;
}

/**
 * lazy allocation
 */
static int do_anonymous_page(pte_t *pte, vma_t *vma, uint64_t address){
    uint64_t newpage = (uint64_t) kzalloc(PGSIZE);

    *pte = PA2PTE(newpage) | riscv_map_prot(vma->prot) | PTE_V;

    /* 本来就为0，是否有必要？ */
    sfence_vma_addr(address);
    return 1;
}

static int do_cow_page(uint64_t address, pte_t *pte){
    return cow_copy(address, pte);
}

/**
 * @return 处理失败返回-1，成功返回1
 */
int __handle_pagefault(pagefault_t fault, proc_t *p, vma_t *vma, uint64 rva) {
    if(!have_prot(fault, vma)){
        ERROR("have no prot");
    }
    pte_t *pte, entry;

    pte = walk(p->mm->pagetable, rva, 1);
    entry = *pte;

    /* 如果valid位不存在，意味着对应的页没有被map过（entry为0），或者不存在内存中（swap） */
    if(!pte_valid(entry)) {
        if(pte_none(entry)){
            if(vma->map_file)
                return do_filemap_page(pte, vma, rva);
            /* lazy allocation */
            return do_anonymous_page(pte, vma, rva);
        }
        return do_swap_page();
    }

    /* cow, 类型为store， 并且write位为0 */
    if(fault == PF_STORE && !pte_write(entry) && (entry & PTE_COW))
        return do_cow_page(rva, pte);

    /* 未知页错误 */
    ER();
    return -1;
}

/**
 * @return 不是对应的异常类型，返回-1，否则返回0，表示处理完成
 */
int handle_pagefault(uint64_t scause) {
    int ktrap = r_sstatus() & SSTATUS_SPP;
    proc_t *p = myproc();
    uint64_t epc = read_csr(sepc);
    uint64_t rva = PGROUNDDOWN(read_csr(stval));
    vma_t *vma = NULL;

    pagefault_t fault = get_pagefault(scause);

    if(fault == PF_UNKNOWN) return -1;

    // 只允许内核通过copyXXX访问用户内存空间
    if(ktrap) {
        // debug("k");
        if(rva == 0) {
            info(rd("Nullpointer Exception: %#lx"), epc);
            goto kernel_fail;
        }

        if(epc < (uint64_t)&cp_start || epc >= (uint64_t)&cp_end) {
            info("access illegal");
            goto kernel_fail;
        }

        if(p == NULL) {
            info("no user context");
            goto kernel_fail;
        }

        vma = __vma_find_strict(p->mm, rva);

        if(vma == NULL) {
            info("access noexistent uvm");
            goto kernel_fail;
        }
        
        if(__handle_pagefault(fault, p, vma, rva) == -1) {
            info("pagefault handle fault");
            goto kernel_fail;
        }
    } else {
        // debug("u");
        // if(rva == 0xF00022000) {
        //     tf_print(p->trapframe);
        //     debug("MAP HERE pc is %#lx pid %d va %#lx", epc, p->pid, r_stval());
        // }
        
        vma = __vma_find_strict(p->mm, rva);

        if(vma == NULL) {
            /* maybe userstack overflow! */
            info("noexistent uvm");
            goto user_fail;
        }

        if((vma->prot & PROT_USER) == 0) {
            info("vma perm");
            goto user_fail;
        }

        if(__handle_pagefault(fault, p, vma, rva) == -1) {
            info("pagefault handle fault");
            goto user_fail;
        }
    }

    return 0;

    kernel_fail:
    info("[Kernel] "rd("%s")" PID %d: epc %#lx va %#lx", riscv_cause2str(scause), p->pid, read_csr(sepc), read_csr(stval));
    
    panic("pagefault handle fault");
    user_fail:
    info("[User] "rd("%s")" PID %d: epc %#lx va %#lx", riscv_cause2str(scause), p->pid, read_csr(sepc), read_csr(stval));
    mmap_print(p->mm);
    p->killed = 1;
    // vmprint(p->pagetable);
    // ER();
    return 0;
}

// int is_pagefault(uint64_t scause){
//     #if PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_12
//     if(scause == EXCP_STORE_PAGE_FAULT || scause == EXCP_LOAD_PAGE_FAULT)
//     #elif PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_9 
//     /* 1.9 的pagefault触发的错误号是否有遗漏？ */
//     if(scause == EXCP_STORE_FAULT)
//     #else
//     ER();
//     #endif
//     {
//         return 1;
//     }

//     return 0;

// }