#ifndef _H_MM_
#define _H_MM_

#include "types.h"

/* 用于表示内核内存的映射关系 */
/* Used to represent memory map*/
typedef struct _kmap_t {
    uint64_t va;
    uint64_t pa;
    size_t   size;
    int      pg_spec;
    int      perm;
} kmap_t;

/* 出于简洁性与兼容性，我们定义了各个函数的默认行为 */
/* Out of convenient and compatibility, we define the default behavior for the func below */
#define mappages(pagetable, va, sz, pa, perm) \
    _mappages(pagetable, va, sz, pa, perm, PGSPEC_NORMAL)
#define uvmunmap(pagetable, va, npages, do_free) \
    _uvmunmap(pagetable, va, npages, do_free, PGSPEC_NORMAL)
#define walk(pagetable, va, alloc) \
    _walk(pagetable, va, alloc, PGSPEC_NORMAL)

void        kvminit(void);
void        kvminithart(void);
void        kvmmap(uint64 va, uint64 pa, size_t sz, int perm, int pg_spec);
int         _mappages(pagetable_t pagetable, uint64 va, size_t sz, uint64 pa, int perm, int spec);
pagetable_t uvmcreate(void);
void        uvminit(pagetable_t, uchar *, uint);
uint64      uvmalloc(pagetable_t, uint64, uint64);
uint64      uvmdealloc(pagetable_t, uint64, uint64);
int         uvmcopy(pagetable_t, pagetable_t, uint64);
void        uvmfree(pagetable_t, uint64);
void        _uvmunmap(pagetable_t, uint64, uint64, int, int);
void        uvmclear(pagetable_t, uint64);
uint64      walkaddr(pagetable_t, uint64);
int         copyout(pagetable_t, uint64, char *, uint64);
int         copyin(pagetable_t, char *, uint64, uint64);
int         copyinstr(pagetable_t, char *, uint64, uint64);
void *      kalloc(void);
int         setupkvm(pagetable_t pagetable);
void        erasekvm(pagetable_t pagetable);

#include "proc.h"
void switchuvm(struct proc *p);
void switchkvm();


#endif