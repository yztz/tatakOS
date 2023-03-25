/** 
 * This file defines public memlayout
 */

#ifndef _H_MEMLAYOUT_
#define _H_MEMLAYOUT_
#include "mm/page.h"

/* Max userspace address */
#define USERSPACE_END (0x80000000) // 2GB
/* Address zone for I/O remapping */
#define IO_BASE 0x2F00000000
/* Dynamic linker interpter mapping address */
#define INTERP_BASE 0x0E00000000
/* Base address for mmap */
#define MMAP_BASE 0x0F00000000

#endif
