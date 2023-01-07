#include "common.h"
#include "types.h"
#include "platform.h"
#include "kernel/elf.h"
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

void freewalk(pagetable_t pagetable);

// Initialize the one kernel_pagetable
void
kvminit(void)
{
  kernel_pagetable = (pagetable_t)kalloc();
  memset(kernel_pagetable, 0, PGSIZE);
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
  kvmmap(KERN_BASE, KERN_BASE, (uint64)etext-KERN_BASE, PTE_R | PTE_X, PGSPEC_NORMAL);
  // map kernel data and the physical RAM we'll make use of.
  uint64_t aligned_data = PGROUNDUP_SPEC(etext, PGSPEC_LARGE);
  // map free mem
  kvmmap((uint64)etext, (uint64)etext, aligned_data-(uint64)etext, PTE_R | PTE_W, PGSPEC_NORMAL);
  kvmmap(aligned_data, aligned_data, MEM_END-aligned_data, PTE_R | PTE_W, PGSPEC_LARGE);

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



// Look up a virtual address, return the physical address,
// or 0 if not mapped.
// Can only be used to look up user pages.
uint64
_walkaddr(pagetable_t pagetable, uint64 va, int pg_spec)
{
  pte_t *pte;
  uint64 pa;

  if(va >= MAXVA)
    return 0;

  pte = _walk(pagetable, va, 0, pg_spec);

  if(pte == 0)
    return 0;
  if((*pte & PTE_V) == 0)
    return 0;
  if((*pte & PTE_U) == 0)
    return 0;
  pa = PTE2PA(*pte);
  return pa;
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
  if(_mappages(kernel_pagetable, va, sz, pa, perm, spec) != 0)
    panic("kvmmap");
  
  kmap[nxt_mapid++] = (kmap_t) {.va=va,.pa=pa,.size=sz,.pg_spec=spec,.perm=perm};
}


int setupkvm(pagetable_t pagetable) {
  if(!pagetable || pagetable == kernel_pagetable) 
    panic("setupkvm");
  for (int i = 0; i < nxt_mapid; i++) {
    kmap_t map = kmap[i];
    // print_map(map);
    if(_mappages(pagetable, map.va, map.size, map.pa, map.perm, map.pg_spec) == -1) {
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

// create an empty user page table.
// returns 0 if out of memory.
pagetable_t
uvmcreate()
{
  pagetable_t pagetable;
  pagetable = (pagetable_t) kalloc();
  if(pagetable == 0)
    return 0;
  memset(pagetable, 0, PGSIZE);
  if(setupkvm(pagetable) == -1) {
    uvmfree(pagetable, 0);
    return 0;
  }
  return pagetable;
}

// Load the user initcode into address 0 of pagetable,
// for the very first process.
// sz must be less than a page.
void
uvminit(pagetable_t pagetable, uchar *src, uint sz)
{
  char *mem;

  if(sz >= PGSIZE)
    panic("inituvm: more than a page");
  mem = kalloc();
  memset(mem, 0, PGSIZE);
  mappages(pagetable, 0, PGSIZE, (uint64)mem, PTE_W|PTE_R|PTE_X|PTE_U);
  memmove(mem, src, sz);
}

// Allocate PTEs and physical memory to grow process from oldsz to
// newsz, which need not be page aligned.  Returns new size or 0 on error.
uint64
uvmalloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz)
{
  char *mem;
  uint64 a;

  if(newsz < oldsz)
    return oldsz;

  oldsz = PGROUNDUP(oldsz);
  for(a = oldsz; a < newsz; a += PGSIZE){
    mem = kalloc();
    if(mem == 0){
      uvmdealloc(pagetable, a, oldsz);
      return 0;
    }
    memset(mem, 0, PGSIZE);
    if(mappages(pagetable, a, PGSIZE, (uint64)mem, PTE_W|PTE_X|PTE_R|PTE_U) != 0){
      kfree(mem);
      uvmdealloc(pagetable, a, oldsz);
      return 0;
    }
  }
  return newsz;
}

// Deallocate user pages to bring the process size from oldsz to
// newsz.  oldsz and newsz need not be page-aligned, nor does newsz
// need to be less than oldsz.  oldsz can be larger than the actual
// process size.  Returns the new process size.
uint64
uvmdealloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz)
{
  if(newsz >= oldsz)
    return oldsz;

  if(PGROUNDUP(newsz) < PGROUNDUP(oldsz)){
    int npages = (PGROUNDUP(oldsz) - PGROUNDUP(newsz)) / PGSIZE;
    uvmunmap(pagetable, PGROUNDUP(newsz), npages, 1);
  }

  return newsz;
}


// Free user memory pages,
// then free page-table pages.
void
uvmfree(pagetable_t pagetable, uint64 sz)
{
  if(sz > 0)
    uvmunmap(pagetable, 0, PGROUNDUP(sz)/PGSIZE, 1);
  freewalk(pagetable);
}

// Given a parent process's page table, copy
// its memory into a child's page table.
// Copies both the page table and the
// physical memory.
// returns 0 on success, -1 on failure.
// frees any allocated pages on failure.'
#include "kernel/proc.h"
int
uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
{
  pte_t *pte;
  uint64 pa, i;
  uint flags;

  for(i = 0; i < sz; i += PGSIZE){
    if((pte = walk(old, i, 0)) == 0)
      panic("uvmcopy: pte should exist");
    if((*pte & PTE_V) == 0)
      panic("uvmcopy: page not present");
    pa = PTE2PA(*pte);

    #ifdef COW
    *pte |= PTE_COW;  // mark cow
    *pte &= ~PTE_W; // read only
    /* IMPORTANT!
      We have changed origin pte, so we need to flash the TLB to make it effective */
    sfence_vma_addr(i);
    ref_page(pa);
    #else
    char *mem;
    if((mem = kalloc()) == 0)
      goto err;
    memmove(mem, (char*)pa, PGSIZE);
    pa = (uint64_t)mem;
    #endif

    flags = PTE_FLAGS(*pte);
    if(mappages(new, i, PGSIZE, pa, flags) != 0){
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

// mark a PTE invalid for user access.
// used by exec for the user stack guard page.
void
uvmclear(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;
  
  pte = walk(pagetable, va, 0);
  if(pte == 0)
    panic("uvmclear");
  *pte &= ~PTE_U;
}

// Recursively free page-table pages.
// All leaf mappings must already have been removed.
void
freewalk(pagetable_t pagetable)
{
  // there are 2^9 = 512 PTEs in a page table.
  for(int i = 0; i < 512; i++){
    pte_t pte = pagetable[i];
    if((pte & PTE_V) && (pte & (PTE_R|PTE_W|PTE_X)) == 0){ // 目录节点
      // this PTE points to a lower-level page table.
      uint64 child = PTE2PA(pte);
      freewalk((pagetable_t)child);
      pagetable[i] = 0;
    } else if(pte & PTE_V){ // 叶子节点
      printf("\nnormal pa: %p\n", PTE2PA(pte));
      panic("freewalk: leaf");
    }
  }
  kfree((void*)pagetable);
}

void switchuvm(struct proc *p) {
  if(p == 0)
    panic("switchuvm: no process");
  if(p->kstack == 0)
    panic("switchuvm: no kstack");
  if(p->pagetable == 0)
    panic("switchuvm: no pgdir");

  write_csr(satp, MAKE_SATP(p->pagetable));
  sfence_vma();
}

void switchkvm() {
  write_csr(satp, MAKE_SATP(kernel_pagetable));
  sfence_vma();
}

