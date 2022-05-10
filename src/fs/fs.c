#include "fs/fs.h"

#include "kernel/proc.h"
#include "param.h"
#include "hlist.h"
#include "mm/alloc.h"
#include "printf.h"

#include "str.h"

fat32_t *fat;

entry_t pool[NENTRY];

void fs_init() {
  // for(int i = 0; i < ENTRY_HLIST_N; i++) {
  //   entry_pool[i].first = NULL;
  // }

  for(int i = 1; i < NENTRY; i++) {
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

static entry_t *eget(entry_t *parent, dir_item_t *item) {
  entry_t *entry, *empty = NULL;
  uint32_t item_clus = FAT_FETCH_CLUS(item);

  if(item_clus == 0) {
    panic("eget: 0 clus");
  }

  acquire(&fat->cache_lock);
  for(entry = &pool[0]; entry < &pool[NENTRY]; entry++){
    if(entry->ref > 0 && entry->parent == parent && 
        entry->clus_start == item_clus){
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
  entry->ref = 1;
  entry->raw = *item;
  entry->clus_start = FAT_FETCH_CLUS(item);
  entry->nlink = 1; // always 1
  entry->parent = parent;
  parent->ref++;
  release(&fat->cache_lock);
  return entry;

}

static entry_t *edup(entry_t *entry) {
  acquire(&entry->fat->cache_lock);
  entry->ref++;
  release(&entry->fat->cache_lock);
  return entry;
}

// under lock
// 递归地解引用
static void __eput(entry_t *entry) {
  if(entry->ref == 0) 
    panic("eput: zero ref");
  entry->ref--;
  if(entry->ref == 0) {
    if(entry->nlink == 0) {
      //todo:trunc
    }
    __eput(entry->parent);
  }
}

static void eput(entry_t *entry) {
  acquire(&entry->fat->cache_lock);
  __eput(entry);
  release(&entry->fat->cache_lock);
}

void elock(entry_t *entry) {
  if(!entry || entry->ref == 0)
    panic("elock: null or no ref");

  acquiresleep(&entry->lock);
}

void eunlock(entry_t *entry) {
  if(entry == 0 || !holdingsleep(&entry->lock) || entry->ref == 0)
    panic("eunlock");

  releasesleep(&entry->lock);
} 

void eunlockput(entry_t *entry) {
  eunlock(entry);
  eput(entry);
}

// 我们不需要考虑特殊处理.与..因为他们也作为实际的目录项而存在
// caller hold parent lock
static entry_t *dirlookup(entry_t *parent, const char *name) {
  dir_item_t item;

  if(!parent)
    panic("dirlookup: parent is null");

  if(fat_dirlookup(parent->fat, parent->clus_start, name, &item) == FR_ERR) { // not found
    return NULL;
  }
  // found
  return eget(parent, &item);

}


entry_t *create(entry_t *from, char *path, short type) {
  entry_t *ep, *dp;
  char name[MAX_FILE_NAME];
  
  // 查找父目录
  if((dp = nameeparent(from, path, name)) == 0)
    return 0;

  elock(dp);

  // 目录项是否已经存在？
  if((ep = dirlookup(dp, name)) != 0){ // 存在
    iunlockput(dp);
    elock(ep);
    if(type == T_FILE && E_ISFILE(ep))
      return ep;
    eunlockput(ep);
    return 0;
  }

  // 不存在，则创建之
  dir_item_t item;
  fat_alloc_entry(fat, dp->clus_start, name, 
          type == T_DIR ? FAT_ATTR_DIR : FAT_ATTR_FILE, &item);

  eunlockput(dp);
  ep = eget(dp, &item);
  elock(ep);
  return ep;
}

// int writee(entry_t *entry, int user, uint64_t src, int off, int n) {
  
// }

static char *skipelem(char *path, char *name) {
  char *s;
  int len;

  while(*path == '/')
    path++;
  if(*path == 0)
    return 0;
  s = path;
  while(*path != '/' && *path != 0)
    path++;
  len = path - s;
  if(len >= MAX_FILE_NAME)
    memmove(name, s, MAX_FILE_NAME);
  else {
    memmove(name, s, len);
    name[len] = 0;
  }
  while(*path == '/')
    path++;
  return path;
}

/* caller holds lock */
void etrunc(entry_t *entry) {
  acquire(&entry->parent->lock);
  fat_trunc(entry->fat, entry->parent->clus_start, &entry->raw);
  release(&entry->parent->lock);
}



// ref: xv6
static entry_t *namex(entry_t *parent, char *path, int nameiparent, char *name)
{
  entry_t*ep, *next;

  if(*path == '/') 
    ep = edup(fat->root); // use global fat now
  else if(parent) {
    ep = edup(parent);
  } else
    ep = edup(myproc()->cwd);

  while((path = skipelem(path, name)) != 0){
    elock(ep);
    if(!E_ISDIR(ep)){
      eunlockput(ep);
      return 0;
    }
    if(nameiparent && *path == '\0'){
      // Stop one level early. But still hold ref
      eunlock(ep);
      return ep;
    }
    if((next = dirlookup(ep, name)) == NULL){
      eunlockput(ep);
      return 0;
    }
    eunlockput(ep);
    ep = next;
  }
  if(nameiparent){
    eput(ep);
    return 0;
  }
  return ep;
}




entry_t *namee(entry_t *from, char *path){
  char name[MAX_FILE_NAME];
  return namex(from, path, 0, name);
}

entry_t *nameeparent(entry_t *from, char *path, char *name){
  return namex(from, path, 1, name);
}

