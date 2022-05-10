#ifndef _H_FILE_
#define _H_FILE_

#include "device.h"
#include "fs/fs.h"

struct file {
  enum { FD_NONE, FD_PIPE, FD_ENTRY, FD_DEVICE } type;
  int ref; // reference count
  
  char readable;
  char writable;
  union {
    struct pipe *pipe; // FD_PIPE
    entry_t *ep; // FD_ENTRY
    device_t *dev;
  };
  uint off;          // FD_INODE
};

#define major(dev)  ((dev) >> 16 & 0xFFFF)
#define minor(dev)  ((dev) & 0xFFFF)
#define	mkdev(m,n)  ((uint)((m)<<16| (n)))


#define CONSOLE 1


#endif