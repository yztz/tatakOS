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
#include "bio.h"
#include "utils.h"

#include "fs/mpage.h"
/**
 * @brief 这个文件里定义了关于页page的操作函数
 * 
 */


/**
 * @brief 得到bio结构体，记录了此次读操作涉及到的sector信息。
 * 
 * @param entry the entry of the file to read
 * @param buff the physical address of the page to store the result
 * @param flpgnum the index of page in the file
 * @return int 
 */
int readpage(entry_t *entry, uint64 buff, uint32 flpgnum){
  
  printf(grn("mapping: %p\n"), entry->i_mapping->host);
  struct bio *bio = do_readpage(entry, buff, flpgnum);
  printf(grn("mapping: %p\n"), entry->i_mapping->host);
  if(bio)
    submit_bio(bio);
  
  return 0;
}


/**
 * @brief 找出所有要读的磁盘块号，记录在bio中，把一个页(page)按照扇区(sector)连续的原则分为段(segment)
 * 
 * @param entry 
 * @param buff
 * @param flpgnum 
 * @return struct bio* 
 */
struct bio *do_readpage(entry_t *entry, uint64 buff, uint32 flpgnum){
  struct bio *bio = kzalloc(sizeof(struct bio));
  // struct bio *bio = kzalloc(PGSIZE);
  struct bio_vec *first_bio_vec, *cur_bio_vec;
  /* 用来统计扇区总数是否符合条件 */
  int sect_num = 0;
  uint32 bps = entry->fat->bytes_per_sec;

  printf(grn("mapping: %p\n"), entry->i_mapping->host);
  first_bio_vec = fat_get_sectors(entry->fat, entry->clus_start, flpgnum*PGSIZE, PGSIZE);
  cur_bio_vec = first_bio_vec;
  while(cur_bio_vec != NULL){
    cur_bio_vec->bv_buff = (void *)buff;
    buff += cur_bio_vec->bv_count * bps;
    sect_num += cur_bio_vec->bv_count;

    cur_bio_vec = cur_bio_vec->bv_next;
  }

  printf(grn("mapping: %p\n"), entry->i_mapping->host);
  if(PGSIZE / bps != sect_num)
    panic("do_readpage: sector num is wrong!");
  printf(grn("mapping: %p\n"), entry->i_mapping->host);
  bio->bi_io_vec = first_bio_vec; 
  printf(grn("mapping: %p\n"), entry->i_mapping->host);
  bio->bi_rw = READ;
  printf(grn("mapping: %p\n"), entry->i_mapping->host);
  bio->bi_dev = entry->fat->dev;
  printf(grn("mapping: %p\n"), entry->i_mapping->host);
  // print_bio_vec(bio);
  return bio;
}