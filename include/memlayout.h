/** 
 * This file defines public memlayout
 */

#ifndef _H_MEMLAYOUT_
#define _H_MEMLAYOUT_
#include "mm/page.h"

/* Kernel stack size */
#define KSTACK_SZ (PGSIZE * 2)
/* Max userspace address */
#define USERSPACE_END (0x80000000) // 2GB
/* Address zone for I/O remapping */
#define IO_BASE 0x1F00000000
/* Dynamic linker interpter mapping address */
#define INTERP_BASE 0x1000000000
/* Base address for mmap */
#define MMAP_BASE 0x0F00000000

#endif
