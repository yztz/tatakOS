#ifndef _H_MM_
#define _H_MM_

#include "types.h"

#define mappages(pagetable, va, sz, pa, perm) \
    _mappages(pagetable, va, sz, pa, perm, PGSPEC_NORMAL)


void        kvminit(void);
void        kvminithart(void);
void        kvmmap(uint64 va, uint64 pa, size_t sz, int perm, int pg_spec);
int         _mappages(pagetable_t pagetable, uint64 va, size_t sz, uint64 pa, int perm, int spec);
// int         map_large_pages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, int perm); 
pagetable_t uvmcreate(void);
void        uvminit(pagetable_t, uchar *, uint);
uint64      uvmalloc(pagetable_t, uint64, uint64);
uint64      uvmdealloc(pagetable_t, uint64, uint64);
int         uvmcopy(pagetable_t, pagetable_t, uint64);
void        uvmfree(pagetable_t, uint64);
void        uvmunmap(pagetable_t, uint64, uint64, int);
void        uvmclear(pagetable_t, uint64);
uint64      walkaddr(pagetable_t, uint64);
int         copyout(pagetable_t, uint64, char *, uint64);
int         copyin(pagetable_t, char *, uint64, uint64);
int         copyinstr(pagetable_t, char *, uint64, uint64);
// void *      valloc(size_t size);
void *      kalloc(void);

void        backtrace(void);


#endif