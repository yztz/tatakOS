#include "printf.h"
#include "utils.h"
#include "mm/page.h"
#include "fs/fat.h"
#include "str.h"
#include "kernel/proc.h"


void 
backtrace(proc_t *p) {
  uint64 fp, top;
  fp = p->ktrap_fp;
  top = PGROUNDUP(fp);
  while(fp < top) {
    printf("%p\n", *(uint64*)(fp-8));
    fp = *(uint64*)(fp-16);
  }
}

void
print_block(uint8_t *b) {
  for(int i = 0; i < BSIZE; i++) {
    if(i != 0 && i % 16 == 0) printf("\n");
    printf("%02x ", b[i]);
  }
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

void
print_dir_item(struct dir_item *item) {
  if(!item) {
    printf("bad item\n");
    return;
  }
  char name[FAT_SFN_LENGTH + 1];
  name[FAT_SFN_LENGTH] = '\0';
  strncpy(name, (char *)item->name, FAT_SFN_LENGTH);
  printf("[file: %s] attr: %b filesize: %d cluster: %d\n", name, item->attr, item->size, FAT_FETCH_CLUS(item));
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