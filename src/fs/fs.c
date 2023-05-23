/**
 * @file fs.c
 * @author YangZongzhen
 * @brief Admittedly, `entry` is a poor design, a byproduct of rapid iteration. 
 *        In theory it plays the same role as the `inode` in linux, 
 *        but in practice it is ambiguous with the upper and lower interfaces, 
 *        confusing the relationship between abstract file objects (`inode`) and 
 *        concrete file objects (`fat_entry`).
 * 
 *        But it is well-intentioned, designed to broker file manipulation to a concrete file system (fat32), 
 *        while taking on the functions of page cache and file exclusive access.
 * 
 * @version 0.1
 * @date 2023-03-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "common.h"
#include "fs/fs.h"
#include "fs/stat.h"
#include "fs/mpage.h"
#include "writeback.h"
#include "kernel/proc.h"
#include "mm/alloc.h"
#include "mm/mmap.h"

// #define QUIET
#define __MODULE_NAME__ FS
#include "debug.h"

fat32_t *fat;

entry_t pool[NENTRY];

void fs_init() {
  for(int i = 0; i < NENTRY; i++) {
    memset(&pool[i], 0, sizeof(entry_t));
    initsleeplock(&pool[i].lock, "pool_entry");
    INIT_LIST_HEAD(&pool[i].e_list);
  }
}


static char *__namepath(entry_t *entry, char *buf) {
  char *end;
  if(entry->parent == NULL) { // root
    goto slash;
  }
  end = __namepath(entry->parent, buf);
  int len = strlen(entry->name);
  if(end - buf > MAXPATH - (len + 1)) {
    debug("too long");
    return end;
  }
  strncpy(end, entry->name, len);
  buf = end + len;
slash:
  *buf = '/';
  buf++;
  *buf = '\0';
  return buf;
}


char *namepath(entry_t *entry, char *buf) {
  char *end = __namepath(entry, buf);
  if(end == buf || end == buf + 1) return end;
  // 去除末尾'/'
  *(--end) = '\0';
  return end;
}

static entry_t *eget(entry_t *parent, uint32_t clus_offset, dir_item_t *item, const char *name) {
  entry_t *entry, *empty = NULL;
  
  acquire(&fat->cache_lock);
  for(entry = &pool[0]; entry < &pool[NENTRY]; entry++){
    if(entry->parent == entry) 
      panic("what?");
    if(entry->ref > 0 && entry->parent == parent && 
        entry->clus_offset == clus_offset){
      entry->ref++;
      release(&fat->cache_lock);
      return entry;
    }

    /* 这里新加了一条判断，因为回收的entry，只有当其写回磁盘了，才会释放i_mapping */
    if(!empty && entry->ref == 0 && entry->i_mapping == NULL)    // Remember empty slot.
      empty = entry;
  }

  if(empty == 0)
    panic("eget: no entries");
  
  
  entry = empty;
  entry->ref = 1;
  entry->raw = *item;
  entry->clus_offset = clus_offset;
  entry->clus_start = FAT_FETCH_CLUS(item);
  entry->fat = parent->fat;
  entry->nlink = 1; // always 1
  entry->parent = parent;
  strncpy(entry->name, name, MAX_FILE_NAME);
  parent->ref++;
  
  /* 只有当entry的类型为FILE时，才需要i_mapping，否则为DIR时不需要 */
  if (item->attr == FAT_ATTR_ARCHIVE) {
    entry->i_mapping  = kzalloc(sizeof(struct address_space));
    entry->i_mapping->host = entry;
  } else {
    entry->i_mapping = NULL;
  }

  entry->clus_end = 0;// initialize 
  entry->clus_cnt = 0;
  entry->size_in_mem = entry->raw.size;

  entry->dirty = 0;
  release(&fat->cache_lock);
  return entry;

}

entry_t *edup(entry_t *entry) {
  acquire(&entry->fat->cache_lock);
  entry->ref++;
  release(&entry->fat->cache_lock);
  return entry;
}


static void unlink(entry_t *entry) {
  if(entry == entry->fat->root) {
    panic("rm root? fuck");
  }

  if(fat_unlink(entry->fat, entry->parent->clus_start, entry->clus_offset, &entry->raw) == FR_ERR) {
    debug("try to rm an entry that doesn't exist");
  }
}



extern void background_writeout(uint64_t min_pages);
extern int pdflush_operation(void (*fn)(uint64_t), unsigned long arg0);
// 递归地解引用
// under lock
static void __eput(entry_t *entry) {
  if(entry->ref == 0) 
    panic("eput: zero ref");
  entry->ref--;
  // 引用为0时，说明没有子目录缓存存在了
  // 所以在删除的时候无需考虑子目录的并发访问问题
  if(entry->ref == 0) { 

    if(entry == entry->fat->root) 
      panic("eput: root?");

    if(entry->nlink <= 0) { // deleted
      acquiresleep(&entry->lock);
      release(&entry->fat->cache_lock);

      unlink(entry);
      
      releasesleep(&entry->lock);
      acquire(&entry->fat->cache_lock);

      if(entry->raw.attr == FAT_ATTR_ARCHIVE) {
        free_mapping(entry);
      }
        
      goto no_writeback;
    }


    // /* 如果为普通文件，则写回。（为目录则不写回） */
    if(entry->raw.attr == FAT_ATTR_ARCHIVE){
        release(&entry->fat->cache_lock);
          /* 只单写一个entry，不要写其他entry */
        if(entry->dirty) {
          writeback_single_entry_idx(entry - pool);
        }
        acquire(&entry->fat->cache_lock);

        /* 释放掉pagecache */
        free_mapping(entry);
    }
no_writeback:
    __eput(entry->parent);
  }
}

void eput(entry_t *entry) {
  if(!entry) 
    panic("eput: entru is null");

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

#include "common.h"

// caller hold parent lock
static entry_t *dirlookup(entry_t *parent, const char *name) {
  dir_item_t item;
  uint32_t offset;
  //todo: root 下的..

  if(strncmp(name, "..", 2) == 0) {
    if(parent->parent != NULL) 
      return edup(parent->parent);
    return NULL;
  }

  if(strncmp(name, ".", 1) == 0) {
    return edup(parent);
  }

  if(!parent)
    panic("dirlookup: parent is null");

  if(fat_dirlookup(parent->fat, parent->clus_start, name, &item, &offset) == FR_ERR) { // not found
    return NULL;
  }

  // 找到了
  return eget(parent, offset, &item, name);

}

int entry_rename(entry_t *entry, const char *newname) {
  elock(entry);
  if(fat_rename(entry->fat, 
                entry->parent->clus_start, 
                entry->clus_offset, 
                &entry->raw, 
                newname, 
                &entry->clus_offset) != FR_OK) {
      eunlock(entry);
      return -1;
  }
  strncpy(entry->name, newname, MAX_FILE_NAME);
  eunlock(entry);
  return 0;
}

entry_t *create(entry_t *from, char *path, short type) {
  entry_t *ep, *dp;
  char name[MAX_FILE_NAME];

  // 查找父目录
  if((dp = nameeparent(from, path, name)) == 0)
    return 0;

  elock(dp);

  // 目录项是否已经存在？
  if((ep = dirlookup(dp, name)) != 0) { // 存在
    debug("entry %s exist", name);
    eunlockput(dp);
    elock(ep);
    if(type == T_FILE && E_ISFILE(ep))
      return ep;
    eunlockput(ep);
    return 0;
  }

  // 不存在，则创建之
  dir_item_t item;
  uint32_t offset;
  fat_create_entry(fat, dp->clus_start, name, 
          type == T_DIR ? FAT_ATTR_DIRECTORY : FAT_ATTR_ARCHIVE, &item, &offset);

  ep = eget(dp, offset, &item, name);
  eunlockput(dp);
  elock(ep);
  return ep;
}

char *skipelem(char *path, char *name) {
  char *s;
  int len;
  // 跳过路径起始的'/'
  while(*path == '/')
    path++;
  if(*path == 0)
    return NULL;
  // 记录name的起始点
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
  // 跳过尾部的'/'
  while(*path == '/')
    path++;
  
  return path;
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

  while((path = skipelem(path, name)) != NULL){
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


int writee(entry_t *entry, int user, uint64_t buff, off_t off, int n) {
  /* maybe overflow */
  if(off < 0 || n < 0)
    panic("writee");

  uint64_t ret = 0;

  // print_chars((char *)buff, n);
  ret = do_generic_mapping_write(entry->i_mapping, user, buff, off, n);

  int newsize = off + ret;
  // /* 更改文件在父目录中的元数据 */
  if(ret > 0){
    entry->dirty = 1;
    if(list_empty(&entry->e_list))
      list_add(&fat->fat_dirty, &entry->e_list);
    /* update the file's size in mem */
    if(newsize > entry->raw.size) {
      entry->size_in_mem = newsize;
    }
  }
  return ret;

}


// caller holds lock
int reade(entry_t *entry, int user, uint64_t buff, off_t off, int n) {
  int ret;

  if(off >= entry->size_in_mem)
    return 0;
  n = min((off_t)n, entry->size_in_mem - off);
  ret = do_generic_mapping_read(entry->i_mapping, user, buff, off, n);
  return ret;
}

entry_t *namee(entry_t *from, char *path){
  char name[MAX_FILE_NAME];
  return namex(from, path, 0, name);
}

entry_t *nameeparent(entry_t *from, char *path, char *name){
  return namex(from, path, 1, name);
}

/**
 * 根据entry在内存中的大小，更新其对应disk上的entry。
 * 1. 内存大于磁盘：
 *   1.1 给磁盘上的文件alloc并append clusters。
 *   1.2 更新其在父目录中的数据。
 * 2. 内存等于磁盘：
 *   不做任何事
 * 3. 内存小于磁盘
 *   需要做什么？
 *    
 */
void sych_entry_size_in_disk(entry_t *entry){
  if(entry->size_in_mem == entry->raw.size)
    return;

  if(entry->size_in_mem > entry->raw.size){
    fat_alloc_append_clusters(entry->fat, entry->clus_start, &entry->clus_end, &entry->clus_cnt, entry->size_in_mem);
  }
  else if(entry->size_in_mem < entry->raw.size){
    ER();
  }

  entry->raw.size = entry->size_in_mem;
  fat_update(entry->fat, entry->parent->clus_start, entry->clus_offset, &entry->raw);
}

/**
 * 把entry在pagecache中的内容写回磁盘。
 */
void sych_entry_pages_in_disk(entry_t *entry){
  address_space_t *mapping = entry->i_mapping;
  rw_page_list_t *pg_list;

  /* 找到entry所有的dirty page */
  pg_list = find_pages_tag(mapping, PAGECACHE_TAG_DIRTY);

  if(pg_list == NULL)
    return;
  
  /* 将dirty page写回磁盘 */
  write_pages(entry, pg_list);
}

/**
 * 同步(sychronize)内存和磁盘上的文件。
 */
void sych_entry_in_disk(entry_t *entry){
  sych_entry_size_in_disk(entry);
  sych_entry_pages_in_disk(entry);
}
