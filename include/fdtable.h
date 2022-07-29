#ifndef _H_FDTABLE_
#define _H_FDTABLE_

#include "common.h"
#include "atomic/spinlock.h"
#include "fs/file.h"

struct fdtable {
  int ref;
  spinlock_t fdlock;
  file_t *ofile[NOFILE];  // Open files
  file_t **ext_ofile;
  int fdflags;
  int nfd;
};

typedef struct fdtable fdtable_t;

fdtable_t *fdtbl_new();
file_t  *fdtbl_getfile(fdtable_t *self, int fd);
int fdtbl_setfile(fdtable_t *self, int fd, file_t *file);
int fdtbl_fdalloc(fdtable_t *self, file_t* file);
void fdtbl_closeall(fdtable_t *self);
fdtable_t *fdtbl_clone(fdtable_t *self);
void fdtbl_free(fdtable_t **pself);
void fdtbl_setflags(fdtable_t *self, int flags);
int fdtbl_getflags(fdtable_t *self);

void fdtbl_ref(fdtable_t *self);
void fdtbl_deref(fdtable_t *self);

#endif