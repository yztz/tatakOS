#include "common.h"
#include "types.h"
#include "platform.h"
#include "elf.h"
#include "riscv.h"
#include "defs.h"
#include "mm/vm.h"
#include "printf.h"
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

// extern char trampoline[]; // trampoline.S


// Initialize the one kernel_pagetable
void
kvminit(void)
{
  kernel_pagetable = (pagetable_t)kzalloc(PGSIZE);
  nxt_mapid = 0;

  // uint64 UNUSED(va) = ioremap(0x2000000, 0x10000);
  // vmprint(kernel_pagetable);
  // va = ioremap(0xc000000, 0x4000000);
  // vmprint(kernel_pagetable);
  // va = ioremap(0x50440000, 0x10000);
  // vmprint(kernel_pagetable);
  // va = ioremap(0x502b0000, 0x10000);
  // vmprint(kernel_pagetable);
  // va = ioremap(0x52000000U, 0x1000000);
  // vmprint(kernel_pagetable);
  // ioremap(0x50000000U, 0x1000);
  // vmprint(kernel_pagetable);
  //   uint64 va = 0x1f06000000UL;
  //   kvmmap(va, 0x52000000U , 0x1000000, PTE_R | PTE_W, PGSPEC_LARGE);
  //   vmprint(kernel_pagetable);
  // for(;;);
  // map kernel text executable and read-only.
  kvmmap(KERN_BASE, KERN_BASE, (uint64)etext-KERN_BASE, PROT_READ | PROT_EXEC, PGSPEC_NORMAL);
  // map kernel data and the physical RAM we'll make use of.
  uint64_t aligned_data = PGROUNDUP_SPEC(etext, PGSPEC_LARGE);
  // map free mem
  kvmmap((uint64)etext, (uint64)etext, aligned_data-(uint64)etext, PROT_READ | PROT_WRITE | PROT_EXEC, PGSPEC_NORMAL);
  kvmmap(aligned_data, aligned_data, MEM_END-aligned_data, PROT_READ | PROT_WRITE | PROT_EXEC, PGSPEC_LARGE);

  debug("init success!");
}



// Switch h/w page table register to the kernel's page table,
// and enable paging.
void
kvminithart()
{
  w_satp(MAKE_SATP(kernel_pagetable));
  sfence_vma();
}


// add a mapping to the kernel page table.
// only used when booting.
// does not flush TLB or enable paging.
void
kvmmap(uint64 va, uint64 pa, size_t sz, int perm, int spec)
{
  if(va % PGSIZE_SPEC(spec)) // we need va aligned
    panic("kvmmap: pgsize");
  if(nxt_mapid == MAX_MAP)
    panic("no map space");
  if(_mappages(kernel_pagetable, va, sz, pa, riscv_map_prot(perm), spec) != 0)
    panic("kvmmap");
  
  kmap[nxt_mapid++] = (kmap_t) {.va=va,.pa=pa,.size=sz,.pg_spec=spec,.perm=perm};
}


int setupkvm(pagetable_t pagetable) {
  if(!pagetable || pagetable == kernel_pagetable) 
    panic("setupkvm");
  for (int i = 0; i < nxt_mapid; i++) {
    kmap_t map = kmap[i];
    // print_map(map);
    if(_mappages(pagetable, map.va, map.size, map.pa, riscv_map_prot(map.perm), map.pg_spec) == -1) {
      // 卸载之前成功映射的页面
      for(int j = 0; j < i; j++) {
        map = kmap[j];
        _uvmunmap(pagetable, map.va, ROUND_COUNT_SPEC(map.size, map.pg_spec), 0, map.pg_spec);
      }
      return -1;
    }
  }
  return 0;
}

void erasekvm(pagetable_t pagetable) {
  if(!pagetable || pagetable == kernel_pagetable)
    panic("erasekvm");
  for (int i = 0; i < nxt_mapid; i++) {
    kmap_t map = kmap[i];
    _uvmunmap(pagetable, map.va, ROUND_COUNT_SPEC(map.size, map.pg_spec), 0, map.pg_spec);
  }
}


// Given a parent process's page table, copy
// its memory into a child's page table.
// Copies both the page table and the
// physical memory.
// returns 0 on success, -1 on failure.
// frees any allocated pages on failure.'
// #include "kernel/proc.h"
// int
int uvmcopy(pagetable_t old, pagetable_t new, vma_t *vma)
{
  pte_t *pte;
  uint64 pa, i;
  uint prot;

  for(i = vma->addr; i < PGROUNDUP(vma->addr + vma->len); i += PGSIZE) {
    if((pte = walk(old, i, 0)) == 0)
      continue;
    if((*pte & PTE_V) == 0)
      continue;
    pa = PTE2PA(*pte);

    *pte |= PTE_COW;  // mark cow
    *pte &= ~PTE_W; // read only
    /* IMPORTANT!
      We have changed origin pte, so we need to flash the TLB to make it effective */
    sfence_vma_addr(i);
    get_page(pa);
    
    prot = PTE_FLAGS(*pte);
    if(mappages(new, i, PGSIZE, pa, prot) != 0){
      /* Free pa here is ok for COW, because we have added refcnt for it */
      kfree((void *)pa);
      goto err;
    }
  }
  return 0;

 err:
  /* This is only to unmap the pages which have been mapped before(not include i(th)) */
  uvmunmap(new, 0, i / PGSIZE, 1);
  return -1;
}


// Recursively free page-table pages.
// All leaf mappings must already have been removed.
void freewalk(pagetable_t pagetable) {
  // there are 2^9 = 512 PTEs in a page table.
  for(int i = 0; i < 512; i++){
    pte_t pte = pagetable[i];
    if((pte & PTE_V) && (pte & (PTE_R|PTE_W|PTE_X)) == 0){ // 目录节点
      // this PTE points to a lower-level page table.
      uint64 child = PTE2PA(pte);
      freewalk((pagetable_t)child);
      pagetable[i] = 0;
    } else if(pte & PTE_V){ // 叶子节点
      printf("va: %p pa: %p\n", PTE2PA(pte));
      panic("freewalk: leaf");
    }
  }
  /* memset 放在kfree前没事 */
    // memset(pagetable, 0, PGSIZE);
  kfree((void*)pagetable);
  // printf("pagetable: %p\n", pagetable);
  // printf("kstack: %p,  sp: %p\n", myproc()->kstack, r_sp());
  // if((uint64)pagetable == 0x80265000)
  //   memset(pagetable, 0, PGSIZE);
  // memcpy(pagetable, (void*)myproc()->kstack, PGSIZE);
  // memcpy(pagetable, pagetable, PGSIZE);
}


