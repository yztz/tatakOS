#ifndef _H_ALLOC_
#define _H_ALLOC_

#include "page.h"

void mem_init();

/**
 * @brief Allocate for a specified size of memory. 
 *        The real allocated memory size is uncertained.
 *        Never try to read/write beyond legal addr.
 * 
 * @param size 
 * @return void* memory address pointer, NULL if fail.
 */
void *kmalloc(size_t size);

/**
 * @brief Another version of kmalloc, but set memory to Zero(0)
 * 
 * @param size 
 * @return void* 
 */
void *kzalloc(size_t size);

/**
 * @brief free memory
 * @note kfree does not check the legality of addr now,
 *       so never try to free an unallocated mem or try to free a truncated mem.
*/
void kfree(void *addr);

/**
 * @brief Get the total memory(page) count.
 * 
 * @return uint64_t 
 */
uint64_t get_total_mem();

/**
 * @brief Get the free memory(page) count.
 * 
 * @return uint64_t 
 */
uint64_t get_free_mem();

/**
 * @brief Get the used memory(page) count
 * 
 * @return uint64_t 
 */
uint64_t get_used_mem();

void  _kfree_safe(void **paddr);

/**
 * @brief If param is not NULL, free *param and set *free = NULL.
 * @param pp reference to a memory pointer (means pointer to pointer)
 */
#define kfree_safe(pp) _kfree_safe((void **)(pp))

// Allocate memory from stack, so use it carefully.
#define alloca(size) __builtin_alloca(size)

#endif