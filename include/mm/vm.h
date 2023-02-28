#ifndef _H_VM_
#define _H_VM_

#include "types.h"


/**
 * @brief Kernel memory map
 * 
 */
typedef struct kmap {
    uint64_t va;
    uint64_t pa;
    size_t   size;
    int      pg_spec;
    int      prot;
} kmap_t;

/**
 * @brief Initialize the one kernel_pagetable
 * 
 */
void kvminit(void);

/**
 * @brief  Switch h/w page table register to the kernel's page table, and enable paging.
 * 
 */
void kvminithart(void);

/**
 * @brief Add a mapping to the kernel page table only called when booting.
 * @warning Does not flush TLB or enable paging.
 * 
 * @param va virtual address
 * @param pa physical address
 * @param sz size
 * @param prot protection flags
 * @param pg_spec page specification
 */
void kvmmap(uint64 va, uint64 pa, size_t sz, int prot, int pg_spec);

int         uvmcopy(pagetable_t old, pagetable_t new, struct vma *vma);
void        freewalk(pagetable_t pagetable);

/**
 * @brief Copy from kernel to user.
 * @deprecated use copy_to_user instead
 * 
 * @param dstva user buffer address
 * @param src kernel address
 * @param len copy size
 * @return 0 on success, -1 on error.
 */
int copyout(uint64 dstva, char *src, uint64 len);

/**
 * @brief Copy from user to kernel.
 * @deprecated use copy_from_user instead
 * 
 * @param dst kernel buffer address
 * @param srcva user address
 * @param len copy size
 * @return 0 on success, -1 on error.
 */
int copyin(void *dst, uint64 srcva, uint64 len);

int memset_user(uint64 dstva, int val, uint64 len) _section(.copy_set_user);
int copyinstr(char *, uint64, uint64) _section(.copy_from_user_str);

/**
 * @brief Copy from user to kernel.
 * 
 * @param dst kernel buffer address
 * @param srcva user address
 * @param len copy size
 * @return 0 on success, -1 on error.
 */
int copy_from_user(void *to, uint64 from, size_t len) _section(.copy_from_user);

/**
 * @brief Copy from kernel to user.
 * 
 * @param to user buffer address
 * @param from kernel address
 * @param len copy size
 * @return 0 on success, -1 on error.
 */
int copy_to_user(uint64 to, void *from, uint64 len) _section(.copy_to_user);

int         either_copyout(int user_dst, uint64 dst, void *src, uint64 len);
int         either_copyin(void *dst, int user_src, uint64 src, uint64 len);
int         setupkvm(pagetable_t pagetable);
void        erasekvm(pagetable_t pagetable);

#include "mm/alloc.h"

#endif