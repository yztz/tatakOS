#ifndef _H_VM_
#define _H_VM_

#include "mm/mmap.h"
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
int         uvmcopy(pagetable_t old, pagetable_t new, vma_t *vma);
void        uvmfree(pagetable_t, uint64);
void        uvmclear(pagetable_t, uint64);
void        freewalk(pagetable_t pagetable);
uint64      _walkaddr(pagetable_t pagetable, uint64 va, int pg_spec);

void        __copyout(mm_t *mm, uint64_t dstva, char *src, uint64 len, int walk);
int         copyout(uint64 dstva, char *src, uint64 len);
/** @deprecated use copy_from_user instead */
int         copyin(void *dst, uint64 srcva, uint64 len);
int         copyinstr(char *, uint64, uint64) _section(.copy_from_user_str);
int         copy_from_user(void *to, uint64 from, size_t n) _section(.copy_from_user);
int         copy_to_user(uint64 dstva, void *src, uint64 len) _section(.copy_to_user);
int         either_copyout(int user_dst, uint64 dst, void *src, uint64 len);
int         either_copyin(void *dst, int user_src, uint64 src, uint64 len);

void        vmprint(pagetable_t pagetable);
void        print_map(struct _kmap_t map);

int         setupkvm(pagetable_t pagetable);
void        erasekvm(pagetable_t pagetable);

#define walkaddr(pagetable, va) \
    _walkaddr(pagetable, va, PGSPEC_NORMAL)

#endif