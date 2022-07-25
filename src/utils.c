#include "printf.h"
#include "utils.h"
#include "mm/page.h"
#include "fs/fat.h"
#include "str.h"
#include "kernel/proc.h"
#include "debug.h"
#include "bio.h"

#include "radix-tree.h"
// #include "mm/mm.h"


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

/* TODO */
void TD(char *info, char *s, const char *s1, int d)
{
  printf(ylw("TODO INFO:\n"));
  printf(bl("%s\n"), info);
  printf(grn("file: %s\t func: %s\t line: %d\r\n"), s, s1, d);
  // for(;;);
}


void error(char *info, char *s, const char *s1, int d)
{
  printf(rd("ERROR INFO:\n"));
  printf(rd("%s\n"), info);
  printf(rd("file: %s\t func: %s\t line: %d\r\n"), s, s1, d);
  for(;;);
}

char *vma_type_string[10] =  { "NONE", "LOAD", "TEXT", "DATA", "BSS", "HEAP", "MMAP_FILE", "STACK", "MMAP_ANON", "GUARD"};

void
print_pa(pagetable_t pagetable, uint64_t start, uint64_t end);

// void
// print_all_vma(mm_struct_t *mm){
//   struct vm_area_struct *vma;
//   int i = 0;

//   vma = mm->mmap;
//   printf(ylw("number of vmas: %d\n"), mm->map_count);
//   while(vma != NULL){
//    printf(grn("no.%d start: %p end: %p type: %s\n"), i++, vma->vm_start, vma->vm_end, vma_type_string[vma->type]); 
//    print_pa(mm->pagetable, vma->vm_start, vma->vm_end);
//    vma = vma->vm_next;
//   }
//   printf("\n");
// }

/**
 * @brief 给出一段连续的va，打印出pa
 * 
 */
void
print_pa(pagetable_t pagetable, uint64_t start, uint64_t end){
  for (int i = start; i < end; i+=PGSIZE)
  {
    pte_t *pte;
    pte = walk(pagetable, i, 1);
    uint64_t pa = PTE2PA(*pte);
    printf("va: %p\tpa: %p\n", i, pa);
  }
  
}

// void _printf_radix_tree(struct radix_tree_node *node, uint8 height, uint8 c_h){
//   printf(bl("count: %-3d, dirty tags: %5p\n"), node->count, node->tags[0][0]);
//   for(int i = 0; i < (1 << RADIX_TREE_MAP_SHIFT); i++){
//     if(node->slots[i] != NULL){
//       for(int j = 0; j < c_h; j++){
//         printf(".. ");
//       }
//       /* the leaf node */
//       if(c_h == height){
//         printf("%-3d  "grn("pa: %p\n"), i, node->slots[i]);
//       }
//       else{
//         printf("%-3d  ", i);
//         // printf(bl("count: %-3d, dirty tags: %5p\n"), node->count, node->tags[0][0]);
//         _printf_radix_tree((struct radix_tree_node *)node->slots[i], height, c_h+1);
//       }
//     }
//   }
// }

// void
// printf_radix_tree(struct radix_tree_root *root){
//   printf(rd("root:  "));
//   // printf(bl("count: %-3d, dirty tags: %5p"), root->rnode->count, root->rnode->tags[0][0]);
//   printf("\n");
//   _printf_radix_tree(root->rnode, root->height, 1); 
//   printf("\n");
// }


/* 打印空闲的物理页数,统计剩余的物理内存 */
void
print_empty_page(){

}

void
print_bio_vec(struct bio *cur_bio){
  struct bio_vec *cur_bio_vec = cur_bio->bi_io_vec;
  while(cur_bio_vec != NULL){
    printf(ylw("bio_vec start: %d, bio_vec counts: %d\n"),
    cur_bio_vec->bv_start_num, cur_bio_vec->bv_count);

    cur_bio_vec = cur_bio_vec->bv_next;
  }
}

/* 打印一个页的内容, 以块为单位*/
void print_page_contents(char *pa){
  for(int i = 0; i < PGSIZE; i++){
    if(i % BSIZE == 0){
      printf(rd("\n%d\n"), i);
    }
    printf(grn("%c"), *pa);
    pa++;
  }
  printf("\n");
}
