#ifndef _H_TYPES_
#define _H_TYPES_

#define NULL ((void *)0)

typedef unsigned char uchar;
typedef unsigned char uint8;
typedef unsigned char uint8_t;

typedef unsigned short ushort;
typedef unsigned short uint16;
typedef unsigned short uint16_t;

typedef unsigned int uint;
typedef unsigned int uint32;
typedef unsigned int uint32_t;

typedef unsigned long uint64;
typedef unsigned long uint64_t;

typedef int bool;

typedef uint64 pde_t;
typedef uint64 pte_t;
typedef uint64 *pagetable_t;

#endif

