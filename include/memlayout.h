/** 
 * This file defines public memlayout
 */

#define IO_BASE_ADDRESS 0x1F00000000
#define TRAMPOLINE (MAXVA - PGSIZE)
// #define KSTACK(p) (TRAMPOLINE - ((p)+1)* 2*PGSIZE)
#define KSTACK (TRAMPOLINE - 2 * PGSIZE)
#define TRAPFRAME (TRAMPOLINE - 3 * PGSIZE)
#define MMAP_BASE 0x60000000

#define USER_TASK_SIZE 0x80000000 //2g
#define TASK_UNMAPPED_BASE USER_TASK_SIZE / 2
// #define USER_START  0x1000
#define USER_START  0x0