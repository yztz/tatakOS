#ifndef _H_FDTABLE_
#define _H_FDTABLE_

#include "types.h"
#include "param.h"
#include "atomic/spinlock.h"

/// @brief file entry in file description table
struct fdt_entry {
    /// @brief file
    file_t *f;
    /// @brief file flags
    int flag;
};

typedef struct fdt_entry fdt_entry_t;

/// @brief file description table 
struct fdtable {
    /// @brief reference count
    int ref;
    /// @brief lock
    spinlock_t fdlock;
    /// @brief open files
    fdt_entry_t ofile[NOFILE]; 
    /// @brief more open files
    fdt_entry_t *ext_ofile;
    /// @brief current fd number
    int nfd;
    /// @brief max fd number
    int max_nfd;
};


fdtable_t *fdtbl_new();
file_t *fdtbl_getfile(fdtable_t *self, int fd);
int fdtbl_setfile(fdtable_t *self, int fd, file_t *file, int flag);
int fdtbl_fdalloc(fdtable_t *self, file_t *file, int min, int flag);
void fdtbl_closeall(fdtable_t *self);
fdtable_t *fdtbl_clone(fdtable_t *self);
void fdtbl_free(fdtable_t **pself);
void fdtbl_setflags(fdtable_t *self, int fd, int flag);
int fdtbl_setmaxnfd(fdtable_t *self, int max);
int fdtbl_close(fdtable_t *self, int fd);
void fdtbl_addflag(fdtable_t *self, int fd, int flag);
void fdtbl_ref(fdtable_t *self);
void fdtbl_deref(fdtable_t *self);
void fdtbl_closexec(fdtable_t *self);
int fdtbl_getflags(fdtable_t *self, int fd);

#endif
