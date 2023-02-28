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


typedef struct cpu cpu_t;
typedef struct proc proc_t;
typedef void (*kthread_callback_t)(proc_t *);
typedef struct mmlayout mm_t;
typedef struct vma vma_t;
typedef struct fdset fdset_t;
typedef struct fdt_entry fdt_entry_t;
typedef struct fdtable fdtable_t;
typedef struct file file_t;
typedef struct fat_entry entry_t;
typedef struct address_space address_space_t;
typedef struct bio bio_t;
typedef struct bio_vec bio_vec_t;
typedef struct request_queue request_queue_t;
typedef struct list_head list_head_t;
typedef struct _device_t device_t;
typedef struct pollfd pollfd_t;
typedef struct page_state page_state_t;
typedef struct pagevec pagevec_t;
typedef struct page_t page_t;
typedef uint8_t pgref_t;
typedef struct radix_tree_root radix_tree_root_t;
typedef struct radix_tree_node radix_tree_node_t;
typedef struct radix_tree_path radix_tree_path_t;
typedef struct rw_page rw_page_t;
typedef struct rw_page_list rw_page_list_t;
typedef struct rb_node rb_node_t;
typedef struct rb_root rb_root_t;
typedef uint64_t sigset_t;
typedef struct siginfo siginfo_t;
typedef struct sigaction sigaction_t;
typedef struct signal signal_t;
typedef struct trapframe tf_t;
typedef struct k_trapframe ktf_t;
typedef struct pte_chain pte_chain_t;
typedef enum _plic_irq plic_irq_t;
/* 返回0表示正常处理，否则失败 */
typedef int (*plic_irq_callback_t)(void *ctx);


#endif

