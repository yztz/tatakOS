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



struct file *filealloc(void);
struct file *filealloc(void);
void        fileclose(struct file*);
struct file *filedup(struct file*);
int         fileread(struct file*, uint64, int n);
int         filestat(struct file *f, struct kstat *stat);
int         filewrite(struct file *f, uint64 addr, int n);
void        file_print(file_t *self);
int         filesend(struct file *infile, struct file *outfile, off_t *poff, size_t len);

#endif