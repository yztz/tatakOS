/** 
 * This file defines public memlayout
 */

#ifndef _H_MEMLAYOUT_
#define _H_MEMLAYOUT_
#include "mm/page.h"

#define IO_BASE_ADDRESS 0x1F00000000
#define KSTACK_SZ PGSIZE
#define TRAPFRAME (MAXVA - PGSIZE)
#define KSTACK (TRAPFRAME - KSTACK_SZ)

#define INTERP_BASE 0x1000000000
#define MMAP_BASE 0x0F00000000

#endif
