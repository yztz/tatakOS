#ifndef _H_MM_
#define _H_MM_

#include "types.h"
#include "mm/page.h"
#include "mm/alloc.h"

/* 用于表示内核内存的映射关系 */
/* Used to represent memory map*/
typedef struct _kmap_t {
    uint64_t va;
    uint64_t pa;
    size_t   size;
    int      pg_spec;
    int      perm;
} kmap_t;


void        kvminit(void);
void        kvminithart(void);

void        kvmmap(uint64 va, uint64 pa, size_t sz, int perm, int pg_spec);
pagetable_t uvmcreate(void);
void        uvminit(pagetable_t, uchar *, uint);
uint64      uvmalloc(pagetable_t, uint64, uint64);
uint64      uvmdealloc(pagetable_t, uint64, uint64);
int         uvmcopy(pagetable_t, pagetable_t, uint64);
void        uvmfree(pagetable_t, uint64);
void        uvmclear(pagetable_t, uint64);
uint64      walkaddr(pagetable_t, uint64);
int         copyout(pagetable_t, uint64, char *, uint64);
int         copyin(pagetable_t, char *, uint64, uint64);
int         copyinstr(pagetable_t, char *, uint64, uint64);

// void *      kalloc(void);

int         setupkvm(pagetable_t pagetable);
void        erasekvm(pagetable_t pagetable);

#include "kernel/proc.h"
void switchuvm(struct proc *p);
void switchkvm();

#endif