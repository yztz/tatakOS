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


/**
 * @brief 得到bio结构体，记录了此次读操作涉及到的sector信息。
 * 
 * @param entry the entry of the file to read
 * @param buff the physical address of the page to store the result
 * @param flpgnum the index of page in the file
 * @return int 
 */
int readpage(entry_t *entry, uint64 buff, uint32 flpgnum){
  
  bio = do_readpage();
  if(bio)
    submit_bio(READ, bio, buff);
}


/**
 * @brief 找出所有要读的磁盘块号，记录在bio中
 * 
 * @param entry 
 * @param flpgnum 
 * @return struct bio* 
 */
struct bio *do_readpage(entry_t *entry, uint32 flpgnum){

}