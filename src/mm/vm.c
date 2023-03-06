#include "common.h"
#include "platform.h"
#include "riscv.h"
#include "mm/mmap.h"
#include "mm/vm.h"
#include "utils.h"

#define __MODULE_NAME__ VM
#include "debug.h"


kmap_t kmap[MAX_MAP];
static int nxt_mapid; // maybe add a lock for nxt_mapid?

#include "mm/io.h"
/*
 * the kernel's page table.
 */
pagetable_t kernel_pagetable;

extern char etext[];  // kernel.ld sets this to end of kernel code.

void kvminit(void) {
    kernel_pagetable = (pagetable_t)kzalloc(PGSIZE);
    nxt_mapid = 0;
    // map kernel text executable and read-only.
    kvmmap(KERN_BASE, KERN_BASE, (uint64)etext - KERN_BASE, PROT_READ | PROT_EXEC, PGSPEC_NORMAL);
    // map kernel data and the physical RAM we'll make use of.
    uint64_t aligned_data = PGROUNDUP_SPEC(etext, PGSPEC_LARGE);
    // map free mem
    kvmmap((uint64)etext, (uint64)etext, aligned_data - (uint64)etext, PROT_READ | PROT_WRITE | PROT_EXEC, PGSPEC_NORMAL);
    kvmmap(aligned_data, aligned_data, MEM_END - aligned_data, PROT_READ | PROT_WRITE | PROT_EXEC, PGSPEC_LARGE);

    debug("init success!");
}


void kvminithart() {
    set_pgtbl(kernel_pagetable);
    sfence_vma();
}

void kvmmap(uint64 va, uint64 pa, size_t sz, int prot, int spec) {
    if (va % PGSIZE_SPEC(spec)) // we need va aligned
        panic("kvmmap: pgsize");
    if (nxt_mapid == MAX_MAP)
        panic("no map space");
    if (__mappages(kernel_pagetable, va, sz, pa, prot, spec) != 0)
        panic("kvmmap");

    kmap[nxt_mapid++] = (kmap_t){ .va = va,.pa = pa,.size = sz,.pg_spec = spec,.prot = prot };
}

int setupkvm(pagetable_t pagetable) {
    if (!pagetable || pagetable == kernel_pagetable)
        panic("setupkvm");
    kmap_t *map;
    int i;
    for (i = 0; i < nxt_mapid; i++) {
        map = &kmap[i];
        if (__mappages(pagetable, map->va, map->size, map->pa, map->prot, map->pg_spec) == -1)
            goto bad;
    }

    return 0;
bad:
    // 卸载之前成功映射的页面
    for (int j = 0; j < i; j++) {
        map = &kmap[j];
        __uvmunmap(pagetable, map->va, ROUND_COUNT_SPEC(map->size, map->pg_spec), 0, map->pg_spec);
    }
    return -1;
}

void erasekvm(pagetable_t pagetable) {
    if (!pagetable || pagetable == kernel_pagetable)
        panic("erasekvm");
    kmap_t *map;
    for (int i = 0; i < nxt_mapid; i++) {
        map = &kmap[i];
        __uvmunmap(pagetable, map->va, ROUND_COUNT_SPEC(map->size, map->pg_spec), 0, map->pg_spec);
    }
}


// Given a parent process's page table, copy
// its memory into a child's page table.
// Copies both the page table and the
// physical memory.
// returns 0 on success, -1 on failure.
// frees any allocated pages on failure.'
int uvmcopy(pagetable_t old, pagetable_t new, vma_t *vma) {
    pte_t *pte;
    uint64 va;

    for (va = vma->addr; va < vma->addr + vma->len; va += PGSIZE) {

        if ((pte = walk(old, va, 0)) == 0)
            continue;
        if ((*pte & PTE_V) == 0)
            continue;
        uint64 pa = PTE2PA(*pte);

        *pte |= PTE_COW;  // mark cow
        *pte &= ~PTE_W; // read only
        /* IMPORTANT!
          We have changed origin pte, so we need to flash the TLB to make it effective */
        sfence_vma_addr(va);

        get_page(pa);

        // uint rprot = PTE_FLAGS(*pte);
        // uint lprot = riscv_map_prot((vma->prot & ~PROT_WRITE) | PROT_COW);
        // // 1 1101 1011
        // //     11 1010
        // assert(rprot == lprot);

        if (mappages(new, va, PGSIZE, pa, (vma->prot & ~PROT_WRITE) | PROT_COW) != 0) {
            /* Free pa here is ok for COW, because we have added refcnt for it */
            kfree((void *)pa);
            goto err;
        }
    }
    return 0;

err:
    /* This is only to unmap the pages which have been mapped before */
    uvmunmap(new, vma->addr, va / PGSIZE, 1);
    return -1;
}


// Recursively free page-table pages.
// All leaf mappings must already have been removed.
void freewalk(pagetable_t pagetable) {
    // there are 2^9 = 512 PTEs in a page table.
    for (int i = 0; i < 512; i++) {
        pte_t pte = pagetable[i];
        if ((pte & PTE_V) && (pte & (PTE_R | PTE_W | PTE_X)) == 0) { // 目录节点
            // this PTE points to a lower-level page table.
            uint64 child = PTE2PA(pte);
            freewalk((pagetable_t)child);
            pagetable[i] = 0;
        } else if (pte & PTE_V) { // 叶子节点
            printf("pa: %p\n", PTE2PA(pte));
            panic("freewalk: leaf");
        }
    }

    kfree((void *)pagetable);
}

void print_kmap(kmap_t map) {
  printf("map:%p => %p, size: %#x type: %d\n", map.pa, map.va, map.size, map.pg_spec);
}
