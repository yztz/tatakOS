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


// /**
//  * @return -1 if exception unhandled else 0
// */
// int handle_pagefault(uint64_t scause) {
//     // printf(grn("%d\n"), scause);
//     proc_t *p = myproc();
//     uint64_t va = read_csr(stval);
//     // printf(grn("%p\n"), va);

//     // illegal address
//     if(va >= p->cur_mmap_sz) 
//         goto bad;

//     // 地址翻译与访问顺序为：VMA ---> MMU ---> PMA ---> PMP ---> ACCESSED
//     // 在特权级1.12下，所有与MMU相关的错误都将触发xx_page_fault
//     // 而对于PMP(Physical Memory Protection)相关的错误，都将触发xx_access_fault。
//     // 在特权级1.9下，由于没有pagefault(ref: p51)，因此SBI帮我在底层做了一下转换
//     // 对于缺页的错误，它将非缺页以及非PMP访问的异常都归结到了xx_access_fault中，因此这里需要加一层宏判断

//     /*如果是写触发的pagefault， 可能是cow，filemap，lazy；如果是
//         读触发的pagefault，可能是filemap*/
    
//     #if PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_12
//     if(scause == EXCP_STORE_PAGE_FAULT)
//     #elif PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_9
//     if(scause == EXCP_STORE_FAULT)
//     #else
//     if(0)
//     #endif
//     { // store page fault
//         // cow
//         if(cow_copy(p->pagetable, va, NULL) == -1){
//             if(filemap_nopage(va) == -1)
//               goto bad;
//         }
//         return 0;
//     }

//     #if PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_12
//     if(scause == EXCP_LOAD_PAGE_FAULT)
//     #elif PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_9
//     if(scause == EXCP_LOAD_FAULT)
//     #else
//     if(0)
//     #endif
//     { 
//         filemap_nopage(va);
//         return 0;
//     }

//     return -1;

//     bad:
//     debug("page fault va is %lx sepc is %lx", va, r_sepc());
//     p->killed = 1;
//     return 0;
// }


/**
 * @brief a more sophisticated function to handle pagefault using vmas
 * 
 */
int do_page_fault(uint64_t scause){
    proc_t *p = myproc();
    uint64_t address = read_csr(stval);
    vm_area_struct_t *vma;
    mm_struct_t *mm = p->mm;

    acquire(&mm->mm_lock);

    vma = find_vma(mm, address);
    if(!vma)
        goto bad_area;

    /* 如果address在找到的vma的范围内 */
    if(vma->vm_start <= address){
        goto good_area;
    }
    /* 如果不在vma范围内 */
    else{
        /*注意：这么写是默认栈在用户空间的最上层，现在并不是这种情况，考虑后期更改*/
        /* maybe stack overflow (push too many) 可能是栈溢出，这时候需要扩充栈 */
        if(!(vma->type == STACK))
            goto bad_area;
        // if (expand_stack(vma, address))
		//     goto bad_area;
        ERROR("stack overflow");
    }

good_area:
    int write = 0;

    #if PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_12
    if(scause == EXCP_STORE_PAGE_FAULT)
    #elif PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_9
    if(scause == EXCP_STORE_FAULT)
    #else
    ER();
    #endif
    {
        write = 1;
    }


    handle_mm_fault(mm, vma, address, write); 
    release(&mm->mm_lock);
    return 0;
bad_area:
    release(&mm->mm_lock);
    debug("page fault va is %lx, sepc is %lx, scause is %lx.", address, r_sepc(), scause);
    p->killed = 1;
    // vmprint(p->pagetable);
    ER();
    return 0;
}

int is_pagefault(uint64_t scause){
    #if PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_12
    if(scause == EXCP_STORE_PAGE_FAULT || scause == EXCP_LOAD_PAGE_FAULT)
    #elif PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_9 
    /* 1.9 的pagefault触发的错误号是否有遗漏？ */
    if(scause == EXCP_STORE_FAULT)
    #else
    ER();
    #endif
    {
        return 1;
    }

    return 0;
}