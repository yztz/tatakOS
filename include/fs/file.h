#ifndef _H_FILE_
#define _H_FILE_

#include "device.h"
#include "fs/fs.h"
#include "fs/fcntl.h"
#include "fs/pipe.h"

struct file {
  enum { FD_NONE, FD_PIPE, FD_ENTRY, FD_DEVICE } type;
  int ref; // reference count
  
  char readable;
  char writable;
  spinlock_t f_lock;
  union {
    struct pipe *pipe; // FD_PIPE
    entry_t *ep; // FD_ENTRY
    device_t *dev;
  };
  off_t off;
};



#define major(dev)  ((dev) >> 16 & 0xFFFF)
#define minor(dev)  ((dev) & 0xFFFF)
#define	mkdev(m,n)  ((uint)((m)<<16| (n)))



file_t *    filealloc(void);
file_t *    filealloc(void);
void        fileclose(file_t *);
file_t *    filedup(file_t *);
int         fileread(file_t *, uint64_t, int n);
int         filestat(file_t *, struct kstat *stat);
int         filewrite(file_t *, uint64_t addr, int n);
void        file_print(file_t *self);

#endif