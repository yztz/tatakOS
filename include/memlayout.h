/** 
 * This file defines public memlayout
 */

#define IO_BASE_ADDRESS 0x1F00000000
#define TRAMPOLINE (MAXVA - PGSIZE)
// #define KSTACK(p) (TRAMPOLINE - ((p)+1)* 2*PGSIZE)
#define KSTACK (TRAMPOLINE - 2 * PGSIZE)
#define TRAPFRAME (TRAMPOLINE - 3 * PGSIZE)

#define MMAP_BASE 0x60000000