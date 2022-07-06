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
// int mmap_fetch(){
//     struct proc *p = myproc();
//     uint64 va = r_stval(), pa;

//     struct vma *v = 0;
//     int i, j;

//     for(i = 0; i < VMA_NUM; i++){
//       v = &(p->vma[i]);
//       if(v->addr <= va && va < v->addr + v->len)
//         break;
//     }

//     if(i == VMA_NUM) {
//       return -1; // cannot find
//     }

//     for (j = 0; j * PGSIZE < v->len; j++) {
//         if (v->addr + j * PGSIZE <= va && va < v->addr + (j + 1) * PGSIZE) {
//             break;
//             pa = (uint64)kalloc();

//             memset((void*)pa, 0, PGSIZE);
//             if (mappages(p->pagetable, PGROUNDDOWN(va), PGSIZE, pa,
//                          PTE_R | PTE_W | PTE_X | PTE_U) == -1) {
//                 panic("map page failed!");
//             }

//             if (reade(v->map_file->ep, 1, PGROUNDDOWN(va), v->off + j * PGSIZE,
//                       min(PGSIZE, v->end - PGROUNDDOWN(va))) == -1) {
//                 panic("read file failed!");
//             }

//         } else {
//             p->killed = 1;
//             panic("va not find in vma!! lazy allocation is not implemented!");
//         }
//     }

//     pa = (uint64)kalloc();
//     memset((void*)pa, 0, PGSIZE);
//     if (mappages(p->pagetable, PGROUNDDOWN(va), PGSIZE, pa,
//                  PTE_R | PTE_W | PTE_X | PTE_U) == -1) {
//         panic("map page failed!");
//     }

//     // if(reade(v->map_file->ep, 1, PGROUNDDOWN(va), j*PGSIZE, PGSIZE) == -1){
//     if (reade(v->map_file->ep, 1, PGROUNDDOWN(va), v->off + j * PGSIZE,
//               PGSIZE) == -1) {
//         // printf("%d\n", r);
//         panic("read file failed!");
//     }

//     return 0;
// }

/**
 * @return -1 if exception unhandled else 0
*/
int handle_pagefault(uint64_t scause) {
    // printf(grn("%d\n"), scause);
    proc_t *p = myproc();
    vma_t *vma;
    uint64_t va = read_csr(stval);

    // illegal address
    // if(va >= p->cur_mmap_sz) 
    //     goto bad;

    // 地址翻译与访问顺序为：VMA ---> MMU ---> PMA ---> PMP ---> ACCESSED
    // 在特权级1.12下，所有与MMU相关的错误都将触发xx_page_fault
    // 而对于PMP(Physical Memory Protection)相关的错误，都将触发xx_access_fault。
    // 在特权级1.9下，由于没有pagefault(ref: p51)，因此SBI帮我在底层做了一下转换
    // 对于缺页的错误，它将非缺页以及非PMP访问的异常都归结到了xx_access_fault中，因此这里需要加一层宏判断
    // 这似乎违背了SBI的宗旨
    
    #if PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_12
    if(scause == EXCP_STORE_PAGE_FAULT)
    #elif PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_9
    if(scause == EXCP_STORE_FAULT)
    #else
    if(0)
    #endif
    { // store page fault
        if((vma = __vma_find_strict(p->mm, va)) == NULL) {
            goto bad;
        }
        mmap_print_vma(vma);
        if(vma->prot & MAP_PROT_WRITE) {
            pte_t *pte = walk(p->mm->pagetable, va, 1);
            // lazy
            if((*pte & PTE_V) == 0) {
                uint64_t newpage = (uint64)kzalloc(PGSIZE);
                if(newpage == 0 || mappages(p->mm->pagetable, va, PGSIZE, newpage, vma->prot) == -1)
                    goto bad;
                return 0;
            }
            // cow
            if(*pte & PTE_COW) {
                debug("cow trigger...");
                if(cow_copy(va, pte) == -1){
                    goto bad;
                } else {
                    return 0;
                }
            }
        } else {
            goto bad;
        }
        
        return 0;
    }
    

    if(scause == EXCP_LOAD_PAGE_FAULT)
    { 
        // mmap_fetch();
        if((vma = __vma_find_strict(p->mm, va)) == NULL) {
            mmap_print_vmas(p->mm);
            goto bad;
        }
        if(vma->prot & MAP_PROT_READ) {
            pte_t *pte = walk(p->mm->pagetable, va, 1);
            // lazy
            if((*pte & PTE_V) == 0) {
                uint64_t newpage = (uint64)kzalloc(PGSIZE);
                if(newpage == 0 || mappages(p->mm->pagetable, va, PGSIZE, newpage, vma->prot) == -1)
                    goto bad;
                return 0;
            }
        } else {
            goto bad;
        }
    }

    return -1;

    bad:
    debug("page fault va is %lx sepc is %lx scause %lx", va, r_sepc(), scause);
    myproc()->killed = 1;
    return 0;
}