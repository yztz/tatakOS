#ifndef _H_FDTABLE_
#define _H_FDTABLE_

#include "common.h"
#include "atomic/spinlock.h"
#include "fs/file.h"

struct fdt_entry {
  file_t *f;
  int flag;
};

typedef struct fdt_entry fdt_entry_t; 

struct fdtable {
  int ref;
  spinlock_t fdlock;
  fdt_entry_t ofile[NOFILE];  // Open files
  fdt_entry_t *ext_ofile;
  int nfd;
  int maxfd;
};

typedef struct fdtable fdtable_t;

fdtable_t *fdtbl_new();
file_t  *fdtbl_getfile(fdtable_t *self, int fd);
int fdtbl_setfile(fdtable_t *self, int fd, file_t *file, int flag);
int fdtbl_fdalloc(fdtable_t *self, file_t* file, int min, int flag);
void fdtbl_closeall(fdtable_t *self);
fdtable_t *fdtbl_clone(fdtable_t *self);
void fdtbl_free(fdtable_t **pself);
void fdtbl_setflags(fdtable_t *self, int fd, int flag) ;
int fdtbl_setmaxfd(fdtable_t *self, int max);
int fdtbl_close(fdtable_t *self, int fd);
void fdtbl_addflag(fdtable_t *self, int fd, int flag);
void fdtbl_ref(fdtable_t *self);
void fdtbl_deref(fdtable_t *self);
void fdtbl_closexec(fdtable_t *self);
int fdtbl_getflags(fdtable_t *self, int fd);


#endif