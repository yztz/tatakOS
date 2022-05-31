/** 
 * This file defines public memlayout
 */

#ifndef _H_MEMLAYOUT_
#define _H_MEMLAYOUT_
#include "mm/page.h"

#define IO_BASE_ADDRESS 0x1F00000000
#define KSTACK_SZ PGSIZE
#define KSTACK (MAXVA - KSTACK_SZ)
#define MMAP_BASE 0x60000000

#endif