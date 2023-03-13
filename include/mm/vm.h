#ifndef _H_VM_
#define _H_VM_

#include "types.h"


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
 * @brief Given a parent process's page table, copy
 * its memory into a child's page table.
 * Copies both the page table and the
 * physical memory with cow.
 * 
 * @param old 
 * @param new 
 * @param vma 
 * @return int 0 on success, -1 on failure.
 */
int uvmcopy(pagetable_t old, pagetable_t new, vma_t *vma);

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

int either_copyout(int user_dst, uint64 dst, void *src, uint64 len);
int either_copyin(void *dst, int user_src, uint64 src, uint64 len);
int setupkvm(pagetable_t pagetable);
void erasekvm(pagetable_t pagetable);
void switchuvm(mm_t *mm);
void switchkvm();

#include "mm/alloc.h"

#endif