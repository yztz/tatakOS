/** 
 * This file defines public memlayout
 */

#ifndef _H_MEMLAYOUT_
#define _H_MEMLAYOUT_
#include "mm/page.h"

#define KSTACK_SZ (PGSIZE * 2)
#define KSTACK_BASE (MAXVA - KSTACK_SZ)

#define USERSPACE_END (0x80000000) // 2GB
#define IO_BASE 0x1F00000000
#define INTERP_BASE 0x1000000000
#define MMAP_BASE 0x0F00000000

#endif
