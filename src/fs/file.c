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
    file_t *f;
    if ((f = kzalloc(sizeof(file_t))) == NULL)
        return NULL;

    f->ref = 1;
    initlock(&f->f_lock, "flock");

    return f;
}

// Increment ref count for file f.
file_t *filedup(file_t *f) {
    acquire(&f->f_lock);
    if (f->ref < 1)
        panic("filedup");
    f->ref++;
    release(&f->f_lock);
    return f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void fileclose(file_t *f) {
    acquire(&f->f_lock);
    if (f->ref < 1)
        panic("fileclose");
    if (--f->ref > 0) {
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

int __filewrite(file_t *f, int user, uint64_t addr, int n) {
    int r, ret = 0;

    if (f->writable == 0)
        return -1;

    if (f->type == FD_PIPE) {
        ret = pipewrite(f->pipe, user, addr, n);
    } else if (f->type == FD_DEVICE) {
        ret = f->dev->write(user, addr, n);
    } else if (f->type == FD_ENTRY) {
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
int filewrite(file_t *f, uint64_t addr, int n) {
    return __filewrite(f, 1, addr, n);
}

// Read from file f.
// addr is a user virtual address.
int fileread(file_t *f, uint64_t addr, int n) {
    int r = -1;

    if (f->readable == 0)
        return -1;

    if (f->type == FD_PIPE) {
        r = piperead(f->pipe, addr, n);
    } else if (f->type == FD_DEVICE) {
        r = f->dev->read(1, addr, n);
    } else if (f->type == FD_ENTRY) {
        /* openat打开不了目录，这里不用区分目录和普通文件 */
        elock(f->ep);
        if ((r = reade(f->ep, 1, addr, f->off, n)) > 0) {
            f->off += r;
        }
        eunlock(f->ep);
    } else {
        panic("fileread");
    }

    return r;
}



void file_print(file_t *self) {
    if (self == NULL) {
        kprintf("file: NULL\n");
        return;
    }
    char rw[3] = { 0 };
    rw[0] = self->readable ? 'r' : '-';
    rw[1] = self->writable ? 'w' : '-';
    kprintf("file@%#lx: type %d ref %d %s\n", self, self->type, self->ref, rw);
}
