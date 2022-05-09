#include "fs/fat.h"
#include "fs/fs.h"

#include "param.h"
#include "hlist.h"
#include "mm/alloc.h"
#include "printf.h"

#include "str.h"


entry_t pool[NENTRY];

void fs_init() {
  // for(int i = 0; i < ENTRY_HLIST_N; i++) {
  //   entry_pool[i].first = NULL;
  // }
  for(int i = 0; i < NENTRY; i++) {
    memset(&pool[i], 0, sizeof(entry_t));
    initsleeplock(&pool[i].lock, "pool_entry");
  }
}


// /* 申请一个新的entry */
// entry_t *alloc_entry(fat32_t *fat) {
//   entry_t *ret = (entry_t *)kmalloc(sizeof(entry_t));
//   if(!ret) 
//     panic("alloc_entry: kmalloc fail");
//   initsleeplock(&ret->lock, "fat_entry");
//   ret->fat = fat;
//   ret->ref = 0;

//   return ret;
// }

static entry_t *eget(fat32_t *fat, dir_item_t *item) {
  entry_t *entry, *empty = NULL;

  acquire(&fat->cache_lock);
  for(entry = &pool[0]; entry < &pool[NENTRY]; entry++){
    if(entry->ref > 0 && entry->fat == fat && 
        entry->clus_start == FAT_FETCH_CLUS(item)){
      entry->ref++;
      release(&fat->cache_lock);
      return entry;
    }
    if(!empty && entry->ref == 0)    // Remember empty slot.
      empty = entry;
  }

  if(empty == 0)
    panic("iget: no entries");
  
  entry = empty;
  entry->ref++;
  entry->raw = *item;
  entry->clus_start = FAT_FETCH_CLUS(item);
  entry->nlink = 1; // always 1
  release(&fat->cache_lock);
  return entry;

}

static entry_t *edup(entry_t *entry) {
  acquire(&entry->fat->cache_lock);
  entry->ref++;
  release(&entry->fat->cache_lock);
  return entry;
}

static void eput(entry_t *entry) {
  acquire(&entry->fat->cache_lock);
  if(entry->ref == 1 && entry->nlink == 0) {
    //todo:trunc
  }
  entry->ref--;
}



// /* 实际上当前的设备一般固定，所以只使用簇号作为分量计算 */
// #define _HASH(dev, clus) ((clus) % ENTRY_HLIST_N)





static entry_t *namex(char *path, int nameiparent, char *name)
{
  struct inode *ip, *next;

  if(*path == '/') 
    ip = idup();
  else
    ip = idup(myproc()->cwd);

  while((path = skipelem(path, name)) != 0){
    ilock(ip);
    if(ip->type != T_DIR){
      iunlockput(ip);
      return 0;
    }
    if(nameiparent && *path == '\0'){
      // Stop one level early.
      iunlock(ip);
      return ip;
    }
    if((next = dirlookup(ip, name, 0)) == 0){
      iunlockput(ip);
      return 0;
    }
    iunlockput(ip);
    ip = next;
  }
  if(nameiparent){
    iput(ip);
    return 0;
  }
  return ip;
}




// entry_t *namee(char *path) {

// }


