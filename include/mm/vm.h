#ifndef _H_VM_
#define _H_VM_

#include "types.h"


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
int copyout(uint64_t dstva, char *src, uint64_t len);

/**
 * @brief Copy from user to kernel.
 * @deprecated use copy_from_user instead
 * 
 * @param dst kernel buffer address
 * @param srcva user address
 * @param len copy size
 * @return 0 on success, -1 on error.
 */
int copyin(void *dst, uint64_t srcva, uint64_t len);

int memset_user(uint64_t dstva, int val, uint64_t len) _section(.copy_set_user);
int copyinstr(char *, uint64_t, uint64_t) _section(.copy_from_user_str);

/**
 * @brief Copy from user to kernel.
 * 
 * @param dst kernel buffer address
 * @param srcva user address
 * @param len copy size
 * @return 0 on success, -1 on error.
 */
int copy_from_user(void *to, uint64_t from, size_t len) _section(.copy_from_user);

/**
 * @brief Copy from kernel to user.
 * 
 * @param to user buffer address
 * @param from kernel address
 * @param len copy size
 * @return 0 on success, -1 on error.
 */
int copy_to_user(uint64_t to, void *from, uint64_t len) _section(.copy_to_user);

int either_copyout(int user_dst, uint64_t dst, void *src, uint64_t len);
int either_copyin(void *dst, int user_src, uint64_t src, uint64_t len);
int setupkvm(pagetable_t pagetable);
void erasekvm(pagetable_t pagetable);
void switchuvm(mm_t *mm);
void switchkvm();

#include "mm/alloc.h"

#endif