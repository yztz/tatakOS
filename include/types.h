#ifndef _H_TYPES_
#define _H_TYPES_

#include <stdint.h>
#include <stddef.h>

#define NULL ((void *)0)

typedef unsigned char uchar;
typedef unsigned char uint8;

typedef unsigned short ushort;
typedef unsigned short uint16;
typedef unsigned int uint;
typedef unsigned int uint32;
typedef unsigned long uint64;

typedef long off_t;


typedef uint64 pde_t;
typedef uint64 pte_t;
typedef uint64 *pagetable_t;
typedef pte_t *pte_addr_t;

typedef uint64 pgoff_t;
typedef uint64 sector_t;

#define BITS_PER_LONG 64
// #define pgoff_t unsigned long

#endif

