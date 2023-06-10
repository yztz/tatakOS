#ifndef _H_PIPE_
#define _H_PIPE_

#include "types.h"
#include "atomic/spinlock.h"


struct pipe {
    uint id;
    struct spinlock lock;
    uint nread;     // number of bytes read
    uint nwrite;    // number of bytes written
    int readopen;   // read fd is still open
    int writeopen;  // write fd is still open
    char data[];
};

// #define RPIPESIZE (4 * PGSIZE)
#define RPIPESIZE (PGSIZE)
#define PIPESIZE (RPIPESIZE - sizeof(struct pipe))

int pipealloc(struct file **f0, struct file **f1);
void pipeclose(struct pipe *pi, int writable);
int pipewrite(struct pipe *pi, int user, uint64_t addr, int n);
int piperead(struct pipe *pi, uint64_t addr, int n);
int pipe_full(struct pipe *pi);
int pipe_empty(struct pipe *pi);


#endif