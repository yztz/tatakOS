#include "printf.h"
#include "utils.h"
#include "mm/page.h"

static char* indents[] = {
  ".. .. ..",
  ".. ..",
  "..",
};

void
_vmprint(pagetable_t pagetable, int level, int ignore_level) {
  if(level == ignore_level) return;
  char *indent = indents[level];
  for(int i = 0; i < 512; i++){
    pte_t pte = pagetable[i];
    pagetable_t pa = (pagetable_t)PTE2PA(pte);
    if(pte & PTE_V){  // 存在
      if((pte & (PTE_R|PTE_W|PTE_X)) > 0) // 打印叶节点
        printf("%s %-3d: pte[LEAF] %p pa %p\n", indent, i, pte, PTE2PA_SPEC(pte, level));
      else {// 打印下级页表地址 
        printf("%s %-3d: pte %p pa %p\n", indent, i, pte, pa);
        _vmprint(pa, level - 1, ignore_level);
      }
    }
  }
}

void 
vmprint(pagetable_t pagetable) {
  printf("page table %p\n", pagetable);
  _vmprint(pagetable, 2, -1);
}

void 
backtrace(void) {
  uint64 fp, top;
  fp = r_fp();
  top = PGROUNDUP(fp);
  while(fp < top) {
    printf("%p\n", *(uint64*)(fp-8));
    fp = *(uint64*)(fp-16);
  }
}

void
print_map(kmap_t map) {
  printf("map:%p => %p, size: %#x type: %d\n", map.pa, map.va, map.size, map.pg_spec);
}

void
print_sbiret(sbiret_t ret) {
  printf("sbiret{err: %d, val: %d}\n", ret.error, ret.value);
}

void
print_page(int pgnum) {
  page_t *page = &pages[pgnum];
  printf("page %d {order: %d, refcnt: %d, alloc: %d}\n", page->order, page->refcnt, page->alloc);
}

// ref: lua fast get order 打表+快速移位x8
int luaO_log2 (unsigned int x) {
  static const unsigned char log_2[256] = {
    0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
  };

  int l = -1;
  while (x >= 256) { l += 8; x >>= 8; }
  return l + log_2[x];
}