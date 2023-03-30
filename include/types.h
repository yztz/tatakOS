#ifndef _H_TYPES_
#define _H_TYPES_

#include <stdint.h>
#include <stddef.h>
#include "gcc_attr.h"

#define NULL ((void *)0)

typedef unsigned char uchar;
typedef unsigned int uint;

typedef long off_t;


typedef uint64_t pde_t;
typedef uint64_t pte_t;
typedef uint64_t *pagetable_t;
typedef pte_t *pte_addr_t;

typedef uint64_t pgoff_t;
typedef uint64_t sector_t;


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
typedef struct thread_group tg_t;
typedef struct utrapframe utf_t;
typedef struct vma vma_t;
typedef struct waitqueue wq_t;
typedef struct wq_entry wq_entry_t;

typedef void (*kthread_callback_t)(proc_t *);


/// @brief Generic type to describe a buffer
typedef struct buf_desc {
    char *buf;
    int size;
} buf_desc_t;

#endif

