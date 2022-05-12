#include "common.h"
#include "riscv.h"
#include "mm/alloc.h"
#include "mm/vm.h"
#include "kernel/proc.h"
#include "defs.h"

/**
 * @return -1 if exception unhandled else 0
*/
int handle_pagefault(uint64_t scause) {
    proc_t *p = myproc();
    uint64_t va = read_csr(stval);

    // illegal address
    if(va >= p->sz) 
        goto bad;
    
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
    return -1;

    bad:
    p->killed = 1;
    return 0;
}