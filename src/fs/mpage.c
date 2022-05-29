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

/**
 * @brief 这个文件里定义了关于页page的操作函数
 * 
 */

struct bio *do_readpage(entry_t *entry, uint64 buff, uint32 flpgnum);

/**
 * @brief 得到bio结构体，记录了此次读操作涉及到的sector信息。
 * 
 * @param entry the entry of the file to read
 * @param buff the physical address of the page to store the result
 * @param flpgnum the index of page in the file
 * @return int 
 */
int readpage(entry_t *entry, uint64 buff, uint32 flpgnum){
  
  struct bio *bio = do_readpage(entry, buff, flpgnum);
  if(bio)
    submit_bio(bio);
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
  struct bio *bio;
  struct bio_vec *first_bio_vec, *cur_bio_vec;
  int sect_num = 0;
  uint32 bps = entry->fat->bytes_per_sec;

  first_bio_vec = fat_get_sectors(entry->fat, entry->clus_start, flpgnum*PGSIZE, PGSIZE);
  cur_bio_vec = first_bio_vec;
  while(cur_bio_vec != NULL){
    cur_bio_vec->buff = (void *)buff;
    buff += cur_bio_vec->count * bps;
    sect_num += cur_bio_vec->count;

    cur_bio_vec = cur_bio_vec->next;
  }

  if(PGSIZE / bps != sect_num)
    panic("do_readpage: sector num is wrong!");

  bio->bi_io_vec = first_bio_vec; 
  bio->bi_rw = READ;

  return bio;
}