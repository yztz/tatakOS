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