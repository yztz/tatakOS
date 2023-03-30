#include "common.h"
#include "platform.h"
#include "riscv.h"
#include "mm/mmap.h"
#include "mm/vm.h"
#include "utils.h"

#define __MODULE_NAME__ VM
#include "debug.h"


mm_t *public_map;

void switchuvm(mm_t *mm) {
    if(mm->pagetable == 0)
        panic("switchuvm: no pgtbl");

    set_pgtbl(mm->pagetable);
    sfence_vma();
}

void switchkvm() {
    set_pgtbl(public_map->pagetable);
}

extern char etext[];  // kernel.ld sets this to end of kernel code.
extern uint64_t mmap_map_io(mm_t *mm, uint64_t addr, uint64_t len, uint64_t pa, int prot, int pg_spec);

void kvm_init(void) {
    extern mm_t *__mmap_new();
    public_map = __mmap_new();
    if (!public_map) {
        panic("kvm alloc");
    }

    // map kernel text executable and read-only.
    mmap_map_io(public_map, KERN_BASE, (uint64_t)etext - KERN_BASE, KERN_BASE, PROT_READ | PROT_EXEC, PGSPEC_NORMAL);
    // map kernel data and the physical RAM we'll make use of.
    mmap_map_io(public_map, (uint64_t)etext, MEM_END - (uint64_t)etext, (uint64_t)etext, PROT_READ | PROT_WRITE, PGSPEC_NORMAL);

    debug("init success!");
}


void kvm_init_hart() {
    switchkvm();
    sfence_vma();
}


int setupkvm(pagetable_t pagetable) {
    if (!pagetable || pagetable == public_map->pagetable)
        panic("setupkvm");
    vma_t *vma;
    list_for_each_entry(vma, &public_map->vma_head, head) {
        if(__map_pages(pagetable, vma->addr, vma->len, (uint64_t)vma->io_addr, vma->prot, vma->page_spec) == -1) {
            goto bad;
        }
    }

    return 0;
bad:
    // 卸载之前成功映射的页面
    erasekvm(pagetable);
    return -1;
}

void erasekvm(pagetable_t pagetable) {
    if (!pagetable || pagetable == public_map->pagetable)
        panic("erasekvm");

    vma_t *vma;
    list_for_each_entry(vma, &public_map->vma_head, head) {
        __unmap_pages(pagetable, vma->addr, vma->len, 0, vma->page_spec);
    }
}

int uvmcopy(pagetable_t old, pagetable_t new, vma_t *vma) {
    uint64_t va;
    int pg_spec = vma->page_spec;
    uint64_t page_szie = PGSIZE_SPEC(pg_spec);

    for (va = vma->addr; va < vma->addr + vma->len; va+=page_szie) {
        pte_t *pte;
        if ((pte = __walk(old, va, 0, pg_spec)) == 0)
            continue;
        if ((*pte & PTE_V) == 0)
            continue;
        uint64_t pa = PTE2PA(*pte);

        *pte |= PTE_COW;  // mark cow
        *pte &= ~PTE_W; // read only
        /* IMPORTANT!
          We have changed origin pte, so we need to flash the TLB to make it effective */
        sfence_vma_addr(va);

        get_page(pa);

        if (__map_pages(new, va, page_szie, pa, (vma->prot & ~PROT_WRITE) | PROT_COW, pg_spec) != 0) {
            /* Free pa here is ok for COW, because we have added refcnt for it */
            put_page(pa);
            goto err;
        }
    }
    return 0;

err:
    /* This is only to unmap the pages which have been mapped before */
    __unmap_pages(new, vma->addr, va - vma->addr, 1, pg_spec);
    return -1;
}
