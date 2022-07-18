#include "common.h"
#include "platform.h"
#include "mm/alloc.h"
#include "fs/fs.h"
#include "fs/file.h"
#include "mm/vm.h"
#include "kernel/proc.h"
#include "defs.h"
#include "debug.h"
#include "mm/page.h"
#include "radix-tree.h"
#include "mm/mm.h"

#include "fs/mpage.h"
#include "writeback.h"

extern fat32_t *fat;

extern void buddy_print_free();

void writeback_single_entry(entry_t *entry){
  /* wbc暂时无用 */
  // wbc = NULL;
  address_space_t *mapping = entry->i_mapping;

  printf(ylw("begin append clusters!\n"));
  /* bigger than disk */
  if(entry->size_in_mem >= entry->raw.size){
    fat_alloc_append_clusters(entry->fat, entry->clus_start, &entry->clus_end, &entry->clus_cnt, entry->size_in_mem);
  }
  /* smaller than disk */
  else {
    ERROR("consider delete part of file!");
  }
  printf(ylw("end append clusters!\n"));

  printf(ylw("begin fat_update!\n"));
  entry->raw.size = entry->size_in_mem;
  fat_update(entry->fat, entry->parent->clus_start, entry->clus_offset, &entry->raw);
  printf(ylw("end fat_update!\n"));

  printf(ylw("begin write pages!\n"));
  mpage_writepages(mapping);
  printf(ylw("end write pages!\n"));

  printf(ylw("begin free mapping!\n"));
  free_mapping(entry);
  printf(ylw("end free mapping!\n"));

  buddy_print_free();  
}

extern entry_t pool[NENTRY];

void writeback_single_entry_idx(uint64_t idx){
  writeback_single_entry(&pool[idx]); 
}

/**
 * @brief 我们的文件系统中没有struct inode，对应的数据结构是struct fat_entry
 * linux 中的原型函数为writeback_inodes
 */
void
writeback_entrys(struct writeback_control *wbc){
  while(!list_empty(&fat->fat_dirty)){
    entry_t *entry = list_entry(fat->fat_dirty.prev, entry_t, e_list);
    // address_space_t *mapping = entry->i_mapping;

    /* 只设置一个pdfulsh，不用考虑两条pdflush并发问题 */
    acquiresleep(&entry->lock);
    writeback_single_entry(entry);
    releasesleep(&entry->lock);

    list_del(&entry->e_list);
  }
}
