#ifndef _H_TYPES_
#define _H_TYPES_

#include <stdint.h>
#include <stddef.h>
#include "gcc_attr.h"

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

/* Coupled Types 
   That is, when a structure is nested within another structure, 
   we need to directly include the header file in which the nested structure is defined. 
   If a structure contains only refers to another structure, 
   we only need to declare the structure. 
   The structures that only need to be declared will be defined here,
   to reduce duplicate declaration efforts and discover coupling.
*/

typedef struct address_space address_space_t;
typedef struct device device_t;
typedef struct fat_entry entry_t;
typedef struct fdtable fdtable_t;
typedef struct file file_t;
typedef struct ktrapframe ktf_t;
typedef struct kstat kstat_t;
typedef struct mmlayout mm_t;
typedef struct page_t page_t;
typedef struct proc proc_t;
typedef struct signal signal_t;
typedef uint64_t sigset_t;
typedef struct utrapframe utf_t;
typedef struct vma vma_t;

typedef void (*kthread_callback_t)(proc_t *);

#endif

