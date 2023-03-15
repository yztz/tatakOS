#include "kernel/fdtable.h"
#include "fs/file.h"
#include "mm/alloc.h"

#define __MODULE_NAME__ FD_TABLE
#include "debug.h"


/* 我们需要在fdtable中添加相关文件操作的接口吗？（打开/关闭） */

// lock hold
static int fdrealloc(fdtable_t *self, int nfd) {
    if(nfd > self->max_nfd || nfd < NOFILE) 
        return -1;
    if(nfd == NOFILE)
      return 0;

    int oldsz = max(self->nfd - NOFILE, 0) * sizeof(fdt_entry_t);
    int newsz = (nfd - NOFILE) * sizeof(fdt_entry_t);
    fdt_entry_t *newfdarray;

    if((newfdarray = (fdt_entry_t *)kzalloc(newsz)) == NULL) {
        debug("fdrealloc fail");
        return -1;
    }
    memmove(newfdarray, self->ext_ofile, min(oldsz, newsz));
    self->nfd = nfd;
    kfree((void *)self->ext_ofile);
    self->ext_ofile = newfdarray;
    // debug("alloc newfdarray %#lx nfd %d addr %lx", newfdarray, nfd, &p->ext_ofile);
    return 0;
}

void fdtbl_ref(fdtable_t *self) {
    acquire(&self->fdlock);
    self->ref++;
    release(&self->fdlock);
}

void fdtbl_deref(fdtable_t *self) {
    acquire(&self->fdlock);
    self->ref--;
    release(&self->fdlock);
}


static fdtable_t *__fdtbl_new(int nfd) {
    fdtable_t *self = (fdtable_t *)kzalloc(sizeof(fdtable_t));
    if(!self)
        return NULL;
    initlock(&self->fdlock, "fdlock");
    self->max_nfd = MAX_FD;
    
    if(nfd > NOFILE && fdrealloc(self, nfd) < 0) {
        kfree(self);
        return NULL;
    } else if(nfd <= NOFILE){
        self->nfd = NOFILE;
    }

    return self;
}

fdtable_t *fdtbl_new() {
    return __fdtbl_new(NOFILE);
}

static fdt_entry_t *__fdtbl_getentry(fdtable_t *self, int fd) {
    if(fd < 0 || fd >= self->nfd)
        return NULL;
    // debug("nfd is %d fd is %d ext_ofile is %#x", p->nfd, fd, p->ext_ofile);
    if(fd < NOFILE) 
        return &self->ofile[fd];
    else 
        return &self->ext_ofile[fd - NOFILE];
}

file_t *fdtbl_getfile(fdtable_t *self, int fd) {
    file_t *f;

    acquire(&self->fdlock);
    fdt_entry_t *entry = __fdtbl_getentry(self, fd);
    f = entry ? entry->f : NULL;
    release(&self->fdlock);

    return f;
}

static void __fdtbl_setentry(fdtable_t *self, int fd, fdt_entry_t *entry) {
    fdt_entry_t *e = __fdtbl_getentry(self, fd);
    memcpy(e, entry, sizeof(fdt_entry_t));
}

int fdtbl_setfile(fdtable_t *self, int fd, file_t *file, int flag) {
    if(fd < 0)
        return -1;
    acquire(&self->fdlock);
    if(fd >= self->nfd) {
        if(fd >= self->max_nfd) return -1;
        if(self->nfd == self->max_nfd || fdrealloc(self, fd + 1) < 0) {
            release(&self->fdlock);
            return -1;
        }
    }
    fdt_entry_t *entry = __fdtbl_getentry(self, fd);
    entry->f = file;
    entry->flag = flag;
    release(&self->fdlock);
    return 0;
}

int fdtbl_setmaxnfd(fdtable_t *self, int max) {
    acquire(&self->fdlock);
    if(max < self->nfd) {
        release(&self->fdlock);
        return -1;
    }
    self->max_nfd = max;
    release(&self->fdlock);
    return 0;
}

void fdtbl_addflag(fdtable_t *self, int fd, int flag) {
    acquire(&self->fdlock);
    fdt_entry_t *entry = __fdtbl_getentry(self, fd);
    entry->flag |= flag;
    release(&self->fdlock);
}

void fdtbl_setflags(fdtable_t *self, int fd, int flag) {
    acquire(&self->fdlock);
    fdt_entry_t *entry = __fdtbl_getentry(self, fd);
    entry->flag = flag;
    release(&self->fdlock);
}

int fdtbl_getflags(fdtable_t *self, int fd) {
    fdt_entry_t *entry = __fdtbl_getentry(self, fd);
    return entry ? entry->flag : -1;
}

int fdtbl_fdalloc(fdtable_t *self, file_t* file, int min, int flag) {
    if(min >= self->max_nfd)
        return -1;
    int fd;
    fdt_entry_t *entry;

    acquire(&self->fdlock);

    for(fd = 0; fd < self->nfd; fd++) {
        entry = __fdtbl_getentry(self, fd);
        if(entry->f == NULL && fd >= min) {
            goto found;
        }
    }
    
    if(self->nfd == self->max_nfd || fdrealloc(self, min(self->nfd + 10, self->max_nfd)) < 0) {
        goto error;
    }

    for (; fd < self->nfd; fd++) {
        entry = __fdtbl_getentry(self, fd);
        if(entry->f == NULL && fd >= min) {
            goto found;
        }
    }
    panic("unreached");

  found:
    entry->f = file;
    entry->flag = flag;
    release(&self->fdlock);
    return fd;
  error:
    release(&self->fdlock);
    return -1;
}

void fdtbl_print(fdtable_t *self) {
    for(int fd = 0; fd < self->nfd; fd++){
        file_t *f= fdtbl_getfile(self, fd);
        file_print(f);
    }
    kprintf("all: %d\n", self->nfd);
}

// lock hold
static int __fdtbl_close(fdtable_t *self, int fd) {
    fdt_entry_t *entry = __fdtbl_getentry(self, fd);
    if(entry && entry->f) {
        file_t *pf = entry->f;
        entry->f = NULL;
        
        release(&self->fdlock);
        fileclose(pf);
        acquire(&self->fdlock);

        return 0;
    }
    return -1;
}



void fdtbl_closeall(fdtable_t *self) {
    acquire(&self->fdlock);

    for(int fd = 0; fd < self->nfd; fd++)
        __fdtbl_close(self, fd);

    release(&self->fdlock);
}

void fdtbl_closexec(fdtable_t *self) {
    acquire(&self->fdlock);

    for(int fd = 0; fd < self->nfd; fd++) {
        fdt_entry_t *entry = __fdtbl_getentry(self, fd);
        if(entry->f && (entry->flag & O_CLOEXEC))
            __fdtbl_close(self, fd);
    }

    release(&self->fdlock);
}

int fdtbl_close(fdtable_t *self, int fd) {
    int ans;
    acquire(&self->fdlock);
    ans = __fdtbl_close(self, fd);
    release(&self->fdlock);
    return ans;
}

void fdtbl_free(fdtable_t **pself) {
    if(pself == NULL || *pself == NULL) {
        return;
    }
    fdtable_t *self = *pself;

    if(self->ref == 0)
        panic("ref");
        
    fdtbl_deref(self);
    if(self->ref > 0)
        return;

    // fdtbl_closeall(self);

    kfree(self->ext_ofile);
    kfree_safe(pself);
}


static void fdtbl_dup(fdtable_t *self, fdtable_t *newtbl) {
    fdt_entry_t *entry;
    for(int fd = 0; fd < self->nfd; fd++) {
        entry = __fdtbl_getentry(self, fd);
        if(entry->f) {
            filedup(entry->f);
            __fdtbl_setentry(newtbl, fd, entry);
        }
    }
}

fdtable_t *fdtbl_clone(fdtable_t *self) {
    fdtable_t *newtbl = __fdtbl_new(self->nfd);

    if(newtbl == NULL) 
        return NULL;

    fdtbl_dup(self, newtbl);
    
    return newtbl;
}       
