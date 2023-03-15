//
// Support functions for system calls that involve file descriptors.
//

#include "common.h"
#include "fs/fs.h"
#include "fs/file.h"
#include "fs/stat.h"
#include "kernel/proc.h"
#include "device.h"
#include "mm/alloc.h"
#include "mm/vm.h"

#define __MODULE__NAME__ FILE
#include "debug.h"

device_t devs[NDEV];

// Allocate a file structure.
file_t *filealloc(void) {
  struct file *f;
  if((f = kzalloc(sizeof(struct file))) == NULL)
    return NULL; 

  f->ref = 1;
  initlock(&f->f_lock, "flock");

  return f;
}

// Increment ref count for file f.
struct file *filedup(struct file *f) {
  acquire(&f->f_lock);
  if(f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&f->f_lock);
  return f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void fileclose(struct file *f) {
  acquire(&f->f_lock);
  if(f->ref < 1)
    panic("fileclose");
  if(--f->ref > 0){
    release(&f->f_lock);
    return;
  }
  release(&f->f_lock);

  switch (f->type) {
  case FD_PIPE:
    pipeclose(f->pipe, f->writable);
    break;
  case FD_ENTRY:
    eput(f->ep);
    break;
  default:
    break;
    // ER();
  }

  kfree(f);
}

int __filewrite(struct file *f, int user, uint64 addr, int n) {
  int r, ret = 0;

  if(f->writable == 0)
    return -1;

  if(f->type == FD_PIPE){
    ret = pipewrite(f->pipe, user, addr, n);
  } else if(f->type == FD_DEVICE){
    ret = f->dev->write(user, addr, n);
  } else if(f->type == FD_ENTRY){
    elock(f->ep);
    if ((r = writee(f->ep, user, addr, f->off, n)) > 0)
      f->off += r;
    eunlock(f->ep);
    ret = r;
  } else {
    panic("filewrite");
  }

  return ret;
}

// Write to file f.
// addr is a user virtual address.
int filewrite(struct file *f, uint64 addr, int n) {
  return __filewrite(f, 1, addr, n);
}

int filesend(struct file *infile, struct file *outfile, off_t *poff, size_t len) {
  // only support entry rw now...
  if(infile->type != FD_ENTRY)
    return -1;
  
  if(len == 0)
    return 0;

  // 目前暂时采用缓存区的办法，后续可以使用pagecahce来加速
  const int SEND_SIZE = PGSIZE; 
  // static char buf[PGSIZE];
  char *buf = kmalloc(PGSIZE);
  off_t off;
  int rest = len;

  entry_t *ie = infile->ep;

  elock(ie);
  off = poff ? *poff : infile->off;
  while(rest > 0) {
    int ret1 = reade(ie, 0, (uint64_t)buf, off, SEND_SIZE);
    eunlock(ie);
    int ret2 = __filewrite(outfile, 0, (uint64_t)buf, ret1);
    elock(ie);
    rest -= ret1;
    off += ret1;
    if(ret1 < SEND_SIZE || ret1 != ret2)
      break;
  }

  kfree(buf);
  
  // 更新offset
  if(!poff) {
    infile->off += len - rest;
  } else {
    *poff = off;
  }

  eunlock(ie);

  return len - rest;
}

// Get metadata about file f.
// addr is a user virtual address, pointing to a struct stat.
int filestat(struct file *f, struct kstat *stat) {
  
  if(f->type == FD_ENTRY) {
    elock(f->ep);
    estat(f->ep, stat);
    eunlock(f->ep);
  } else {
    panic("unknown ft");
  }
  return 0;
}

// Read from file f.
// addr is a user virtual address.
int fileread(struct file *f, uint64 addr, int n) {
  int r = -1;

  if(f->readable == 0)
    return -1;

  if(f->type == FD_PIPE) {
    r = piperead(f->pipe, addr, n);
  } else if(f->type == FD_DEVICE){
    r = f->dev->read(1, addr, n);
  } else if(f->type == FD_ENTRY){
    /* openat打开不了目录，这里不用区分目录和普通文件 */
    elock(f->ep);
    if((r = reade(f->ep, 1, addr, f->off, n)) > 0) {
      f->off += r;
    }
    eunlock(f->ep);
  }  else {
    panic("fileread");
  }

  return r;
}



void file_print(file_t *self) {
  if(self == NULL) {
    kprintf("file: NULL\n");
    return;
  }
  char rw[3] = {0};
  rw[0] = self->readable ? 'r' : '-';
  rw[1] = self->writable ? 'w' : '-';
  kprintf("file@%#lx: type %d ref %d %s\n", self, self->type, self->ref, rw);
}
