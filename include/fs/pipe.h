#ifndef _H_PIPE_
#define _H_PIPE_

#include "common.h"

#define PIPESIZE 512

struct pipe {
  struct spinlock lock;
  char data[PIPESIZE];
  uint nread;     // number of bytes read
  uint nwrite;    // number of bytes written
  int readopen;   // read fd is still open
  int writeopen;  // write fd is still open
};

struct file;

int pipealloc(struct file **f0, struct file **f1);
void pipeclose(struct pipe *pi, int writable);
int pipewrite(struct pipe *pi, int user, uint64 addr, int n);
int piperead(struct pipe *pi, uint64 addr, int n);
int pipe_full(struct pipe *pi);
int pipe_empty(struct pipe *pi);


#endif