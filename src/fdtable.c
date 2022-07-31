#include "fdtable.h"
#include "mm/alloc.h"

#define __MODULE_NAME__ FD_TABLE
#include "debug.h"


/* 我们需要在fdtable中添加相关文件操作的接口吗？（打开/关闭） */

// lock hold
static int fdrealloc(fdtable_t *self, int nfd) {
    if(nfd > self->maxfd || nfd < NOFILE) 
        return -1;
    if(nfd == NOFILE)
      return 0;

    int oldsz = max(self->nfd - NOFILE, 0) * sizeof(file_t *);
    int newsz = (nfd - NOFILE) * sizeof(file_t *);
    file_t **newfdarray;

    if((newfdarray = (file_t **)kzalloc(newsz)) == NULL) {
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


fdtable_t *fdtbl_new() {
    fdtable_t *self = (fdtable_t *)kzalloc(sizeof(fdtable_t));
    if(!self)
        return NULL;
    initlock(&self->fdlock, "fdlock");
    self->nfd = NOFILE;
    self->maxfd = MAX_FD;
    return self;
}

static file_t **__fdtbl_getfile(fdtable_t *self, int fd) {
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
    file_t **pf = __fdtbl_getfile(self, fd);
    f = pf ? *pf : NULL;
    release(&self->fdlock);

    return f;
}

int fdtbl_setfile(fdtable_t *self, int fd, file_t *file) {
    if(fd < 0)
        return -1;
    acquire(&self->fdlock);
    if(fd >= self->nfd && fdrealloc(self, min(self->nfd + 10, self->maxfd)) < 0) {
        release(&self->fdlock);
        return -1;
    }
    if(fd < NOFILE)
        self->ofile[fd] = file;
    else 
        self->ext_ofile[fd - NOFILE] = file;
    release(&self->fdlock);
    return 0;
}

int fdtbl_setmaxfd(fdtable_t *self, int max) {
    acquire(&self->fdlock);
    if(max < self->nfd) {
        release(&self->fdlock);
        return -1;
    }
    self->maxfd = max;
    release(&self->fdlock);
    return 0;
}


int fdtbl_fdalloc(fdtable_t *self, file_t* file) {
    int fd;
    acquire(&self->fdlock);
    for (fd = 0; fd < NOFILE; fd++) {
        if (self->ofile[fd] == 0) {
            self->ofile[fd] = file;
            goto found;
        }
    }

    for (fd = 0; fd < self->nfd - NOFILE; fd++) {
        if (self->ext_ofile[fd] == 0) {
            self->ext_ofile[fd] = file;
            fd += NOFILE;
            goto found;
        }
    }
    
    if(self->nfd == self->maxfd || fdrealloc(self, min(self->nfd + 10, self->maxfd)) < 0) {
        goto error;
    }

    for (; fd < self->nfd - NOFILE; fd++) {
        if (self->ext_ofile[fd] == 0) {
            self->ext_ofile[fd] = file;
            fd += NOFILE;
            goto found;
        }
    }
    panic("unreached");

  found:
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
    printf("all: %d\n", self->nfd);
}

// lock hold
static int __fdtbl_close(fdtable_t *self, int fd) {
    file_t **f = __fdtbl_getfile(self, fd);
    if(f && *f) {
        file_t *pf = *f;
        *f = NULL;
        
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
    file_t *fp;
    for(int i = 0; i < self->nfd; i++) {
        if((fp = fdtbl_getfile(self, i)) != NULL)
            fdtbl_setfile(newtbl, i, filedup(fp));
    }
}

fdtable_t *fdtbl_clone(fdtable_t *self) {
    fdtable_t *newtbl = fdtbl_new();

    if(newtbl == NULL) 
        return NULL;

    // if(fdtbl_dup(self, newtbl) == -1) {
    //     fdtbl_free(&newtbl);
    //     return NULL;
    // }
    fdtbl_dup(self, newtbl);
    
    return newtbl;
}       

void fdtbl_setflags(fdtable_t *self, int flags) {
    acquire(&self->fdlock);
    self->fdflags |= flags;
    release(&self->fdlock);
}

int fdtbl_getflags(fdtable_t *self) {
    return self->fdflags;
}

