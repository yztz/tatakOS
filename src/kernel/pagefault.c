#include "common.h"
#include "riscv.h"
#include "mm/alloc.h"
#include "fs/fs.h"
#include "fs/file.h"
#include "mm/vm.h"
#include "kernel/proc.h"
#include "defs.h"

#define __MODULE_NAME__ TRAP
#include "debug.h"

/**
 * @return -1 if exception unhandled else 0
*/
int handle_pagefault(uint64_t scause) {
    // printf(grn("%d\n"), scause);
    proc_t *p = myproc();
    uint64_t va = read_csr(stval);

    // illegal address
    if(va >= p->sz) 
        goto bad;
    // if(va >= p->sz){

    // printf(ylw("va: %d\n"), va);
    // printf(ylw("p->sz: %d\n"), p->sz);
    // for(;;);
    // }

    
    // for(;;);
    // store page fault
    #ifdef QEMU
    if(scause == EXCP_STORE_PAGE_FAULT) {
    #else
    if(scause == EXCP_STORE_FAULT) {
    #endif
        // cow
        if(cow_copy(p->pagetable, va, NULL) == -1)
            goto bad;
        return 0;
    }
    //zyy: mmap or lazy
    else if(scause == EXCP_LOAD_PAGE_FAULT){
        // for(;;);
        uint64 va = r_stval(), pa;
        struct proc *p = myproc();
        struct vma *v = 0;
        int i, j;

        for(i = 0; i < VMA_NUM; i++){
          v = &(p->vma[i]);
          if(v->addr <= va && va < v->addr + v->len)
            break;
        }

        if(i < VMA_NUM){

          for(j = 0; j*PGSIZE < v->len; j++){
            if(v->addr + j*PGSIZE <= va && va < v->addr + (j+1)*PGSIZE){
              break;
            }
          }

          pa = (uint64)kalloc();
          memset((void *)pa, 0, PGSIZE);
          if(mappages(p->pagetable, PGROUNDDOWN(va), PGSIZE, pa, PTE_R|PTE_W|PTE_X|PTE_U) == -1){
            panic("map page failed!");
          }

          if(reade(v->map_file->ep, 1, PGROUNDDOWN(va), j*PGSIZE, PGSIZE) == -1){
            // printf("%d\n", r);
            panic("read file failed!");
          }

        } else{
          p->killed = 1;
          panic("va not find in vma!! lazy allocation is not implemented!");
        }

        return 0;

    }
    // for(;;);
    return -1;

    bad:
    debug("page fault va is %lx sepc is %lx", va, r_sepc());
    // for(;;);
    p->killed = 1;
    return 0;
}