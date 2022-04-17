/** 
 * This file defines public memlayout
 */

/* use 38 in sv39 to avoid sign-extend ref: riscv-privileged-20211203 p84 */
#define MAXVA (1L << (9 + 9 + 9 + 12 - 1))
#define IO_BASE_ADDRESS 0x1F00000000
#define TRAMPOLINE (MAXVA - PGSIZE)
// #define KSTACK(p) (TRAMPOLINE - ((p)+1)* 2*PGSIZE)
#define KSTACK (TRAMPOLINE - 2 * PGSIZE)
#define TRAPFRAME (TRAMPOLINE - PGSIZE)

