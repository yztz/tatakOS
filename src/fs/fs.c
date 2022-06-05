#include "fs/fs.h"
#include "printf.h"
#include "common.h"
#include "kernel/proc.h"
#include "param.h"
#include "hlist.h"
#include "mm/alloc.h"
#include "str.h"
#include "profile.h"

#define QUIET
#define __MODULE_NAME__ FS
#include "debug.h"

fat32_t *fat;

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

// todo:可能还需要考虑到挂载问题
char *getcwd(entry_t *entry, char *buf) {
  char *end;
  if(entry->parent == NULL) { // root
    goto slash;
  }
  end = getcwd(entry->parent, buf);
  if(end - buf > MAXPATH - (MAX_FILE_NAME + 1)) {
    debug("too long");
    return end;
  }
  int len = strlen(entry->name);
  strncpy(end, entry->name, len);
  buf = end + len;
slash:
  *buf = '/';
  buf++;
  *buf = '\0';
  return buf;
}

/* 没什么实际意义，仅仅用来保存状态 */
struct dents_state {
  buf_desc_t desc;
  uint32_t *offset;
};

FR_t dents_handler(dir_item_t *item, const char *name, int offset, void *s) {
  const int dirent_size = sizeof(struct linux_dirent64);
  struct dents_state *state = (struct dents_state *) s;
  struct linux_dirent64 *dirent = (struct linux_dirent64 *) state->desc.buf;
  int namelen = strlen(name) + 1;
  int total_size = ALIGN(dirent_size + namelen, 8); // 保证8字节对齐
  // debug("total size is %d desc size is %d", total_size, desc->size);
  if(total_size > state->desc.size) 
    return FR_OK;

  dirent->d_ino = (uint64_t)offset << 32 | FAT_FETCH_CLUS(item);
  dirent->d_off = offset;
  dirent->d_reclen = total_size;
  dirent->d_type = FAT_IS_DIR(item->attr) ? T_DIR : T_FILE; 

  strncpy(dirent->d_name, name, namelen);
  // printf("name: %s totalsize: %d offset: %d\n", name, total_size, offset);
  state->desc.buf += total_size;
  state->desc.size -= total_size;
  *(state->offset) = offset + sizeof(*item);

  return FR_CONTINUE;
}

// caller holds lock
int read_dents(entry_t *entry, uint32_t *offset, char *buf, int n) {
  struct dents_state state = {{.buf = buf, .size = n}, .offset = offset};
  fat_traverse_dir(fat, entry->clus_start, *offset, dents_handler, &state);
  return n - state.desc.size;
}


static entry_t *eget(entry_t *parent, uint32_t clus_offset, dir_item_t *item, const char *name) {
  entry_t *entry, *empty = NULL;

  acquire(&fat->cache_lock);
  for(entry = &pool[0]; entry < &pool[NENTRY]; entry++){
    if(entry->ref > 0 && entry->parent == parent && 
        entry->clus_offset == clus_offset){
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
  entry->clus_offset = clus_offset;
  entry->clus_start = FAT_FETCH_CLUS(item);
  entry->fat = parent->fat;
  entry->nlink = 1; // always 1
  entry->parent = parent;
  strncpy(entry->name, name, MAX_FILE_NAME);
  parent->ref++;
  release(&fat->cache_lock);
  return entry;

}


// caller holds lock
void estat(entry_t *entry, struct kstat *stat) {
  dir_item_t *item = &entry->raw;
  stat->st_ino = (uint64_t)entry->clus_offset << 32 | entry->clus_start;
  stat->st_gid = 0;
  stat->st_uid = 0;
  stat->st_dev = entry->fat->dev;
  stat->st_rdev = entry->fat->dev;
  stat->st_mode = item->attr;
  stat->st_blksize = entry->fat->bytes_per_sec;
  stat->st_blocks = 0;
  stat->st_size = item->size;
  stat->st_atime_nsec = 0;
  stat->st_atime_sec = 0;
  stat->st_mtime_sec = 0;
  stat->st_mtime_nsec = 0;
  stat->st_ctime_sec = 0;
  stat->st_ctime_nsec = 0;
  stat->st_nlink = entry->nlink;
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

    if(entry->nlink <= 0) {
      acquiresleep(&entry->lock);
      release(&entry->fat->cache_lock);

      unlink(entry);
      
      releasesleep(&entry->lock);
      acquire(&entry->fat->cache_lock);
    }
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

// 我们不需要考虑特殊处理.与..因为他们也作为实际的目录项而存在
// caller hold parent lock
static entry_t *dirlookup(entry_t *parent, const char *name) {
  dir_item_t item;
  uint32_t offset;
  //todo: root 下的..

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


entry_t *create(entry_t *from, char *path, short type) {
  entry_t *ep, *dp;
  char name[MAX_FILE_NAME];

  // 查找父目录
  if((dp = nameeparent(from, path, name)) == 0)
    return 0;

  elock(dp);

  // 目录项是否已经存在？
  if((ep = dirlookup(dp, name)) != 0){ // 存在
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
  fat_alloc_entry(fat, dp->clus_start, name, 
          type == T_DIR ? FAT_ATTR_DIR : FAT_ATTR_FILE, &item, &offset);

  eunlockput(dp);
  ep = eget(dp, offset, &item, name);
  elock(ep);
  return ep;
}

static char *skipelem(char *path, char *name) {
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


/* caller holds entry lock */
void etrunc(entry_t *entry) {
  if(E_ISDIR(entry)) {
    debug("try trunc dir?");
    return;
  }
  
  fat_trunc(entry->fat, entry->parent->clus_start, entry->clus_offset, &entry->raw);
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

// caller holds lock
int writee(entry_t *entry, int user, uint64_t buff, int off, int n) {
  int ret = fat_write(entry->fat, entry->clus_start, user, buff, off, n);
  int newsize = off + ret;
  if(ret > 0 && newsize > entry->raw.size) { // 文件长度变化
    entry->raw.size = newsize;
    debug("update size");
    fat_update(entry->fat, entry->parent->clus_start, entry->clus_offset, &entry->raw);
  }
  return ret;
}


int reade(entry_t *entry, int user, uint64_t buff, int off, int n) {
  if(off >= E_FILESIZE(entry)) 
    return 0;
  int ret = fat_read(entry->fat, entry->clus_start, user, buff, off, n);
  return ret;
}



entry_t *namee(entry_t *from, char *path){
  char name[MAX_FILE_NAME];
  return namex(from, path, 0, name);
}

entry_t *nameeparent(entry_t *from, char *path, char *name){
  return namex(from, path, 1, name);
}

