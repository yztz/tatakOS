#include "riscv.h"
#include "atomic/spinlock.h"
#include "atomic/sleeplock.h"
#include "kernel/proc.h"
#include "fs/fs.h"
#include "fs/file.h"
#include "fs/pipe.h"
#include "mm/vm.h"
#include "mm/alloc.h"

#define __MODULE_NAME__ PIPE
#include "debug.h"

static int __pipe_full(struct pipe *pi) {
    return pi->nwrite == pi->nread + PIPESIZE;
}

static int __pipe_empty(struct pipe *pi) {
    return pi->nread == pi->nwrite;
}

static atomic_t pipeid = INIT_ATOMIC();

int pipealloc(struct file **f0, struct file **f1) {
    struct pipe *pi;

    pi = 0;
    *f0 = *f1 = 0;
    if ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
        goto bad;
    if ((pi = (struct pipe *)kmalloc(RPIPESIZE)) == 0)
        goto bad;
    pi->readopen = 1;
    pi->writeopen = 1;
    pi->nwrite = 0;
    pi->nread = 0;
    pi->id = atomic_inc(&pipeid);
    initlock(&pi->lock, "pipe");
    (*f0)->type = FD_PIPE;
    (*f0)->readable = 1;
    (*f0)->writable = 0;
    (*f0)->pipe = pi;
    (*f1)->type = FD_PIPE;
    (*f1)->readable = 0;
    (*f1)->writable = 1;
    (*f1)->pipe = pi;
    return 0;

bad:
    if (pi)
        kfree((char *)pi);
    if (*f0)
        fileclose(*f0);
    if (*f1)
        fileclose(*f1);
    return -1;
}

void pipeclose(struct pipe *pi, int writable) {
    acquire(&pi->lock);
    if (writable) {
        pi->writeopen = 0;
        wakeup(&pi->nread);
    } else {
        pi->readopen = 0;
        wakeup(&pi->nwrite);
    }
    if (pi->readopen == 0 && pi->writeopen == 0) {
        release(&pi->lock);
        kfree((char *)pi);
    } else
        release(&pi->lock);
}

int pipewrite(struct pipe *pi, int user, uint64_t addr, int n) {
    int i = 0;
    struct proc *pr = myproc();

    acquire(&pi->lock);
    while (i < n) {
        if (pi->readopen == 0 || pr->killed) {
            release(&pi->lock);
            return -1;
        }
        if (__pipe_full(pi)) {
            wakeup(&pi->nread);
            sleep(&pi->nwrite, &pi->lock);
        } else {
            // 本次可写数据长度
            int rest = min(pi->nread + PIPESIZE - pi->nwrite, (uint64_t)n);
            int cur = pi->nwrite % PIPESIZE;
            // 管道数组到尾部的长度
            int avail = PIPESIZE - cur;
            if (rest <= avail) {
                if (either_copyin(&pi->data[cur], user, addr, rest) == -1) break;
            } else {
                if (either_copyin(&pi->data[cur], user, addr, avail) == -1) break;
                if (either_copyin(&pi->data[0], user, addr + avail, rest - avail) == -1) break;
            }

            pi->nwrite += rest;
            i += rest;
        }
    }
    wakeup(&pi->nread);

    debug("PID %d ---> PIPE %d(0x%lx) : %d bytes", pr->pid, pi->id, pi, i);

    release(&pi->lock);

    return i;
}

int piperead(struct pipe *pi, uint64_t addr, int n) {
    struct proc *pr = myproc();

    acquire(&pi->lock);
    while (pi->writeopen && __pipe_empty(pi)) {
        if (pr->killed) {
            goto bad;
        }
        sleep(&pi->nread, &pi->lock);
    }

    // 本次可读数据长度
    int rest = min(pi->nwrite - pi->nread, (uint)n);
    int cur = pi->nread % PIPESIZE;
    // 管道数组到尾部的长度
    int avail = PIPESIZE - cur;
    if (rest <= avail) {
        if (copy_to_user(addr, &pi->data[cur], rest) == -1) goto bad;
    } else {
        if (copy_to_user(addr, &pi->data[cur], avail) == -1) goto bad;
        if (copy_to_user(addr + avail, &pi->data[0], rest - avail) == -1) goto bad;
    }
    pi->nread += rest;

    wakeup(&pi->nwrite);

    debug("PID %d <--- PIPE %d(0x%lx) : %d bytes", pr->pid, pi->id, pi, rest);

    release(&pi->lock);

    return rest;

bad:
    release(&pi->lock);
    return -1;
}


int pipe_full(struct pipe *pi) {
    acquire(&pi->lock);
    int ans = __pipe_full(pi);
    release(&pi->lock);
    return ans;
}

int pipe_empty(struct pipe *pi) {
    acquire(&pi->lock);
    int ans = __pipe_empty(pi);
    release(&pi->lock);
    return ans;
}
