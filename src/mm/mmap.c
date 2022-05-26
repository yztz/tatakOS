#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "mm/vm.h"
#include "param.h"
#include "fs/stat.h"
#include "fs/fs.h"
#include "atomic/spinlock.h"
#include "kernel/proc.h"
#include "atomic/sleeplock.h"
#include "fs/file.h"
#include "fs/fcntl.h"
#include "mm/page.h"

// #define QUIET
#define __MODULE_NAME__ MMAP
#include "debug.h"

#include "mm/mm_types.h"
#include "mm/mm.h"
#include "rbtree.h"
#include "utils.h"

uint64
get_unmapped_area(uint64 len){
	uint64 old_addr;
	struct proc *p = myproc();

	old_addr = p->cur_mmap_sz;
  // printf(rd("cur_de 1: %p\n"), myproc()->cur_mmap_sz);
	p->cur_mmap_sz += PGROUNDUP(len);
  // printf(ylw("cur_de 2: %p\n"), myproc()->cur_mmap_sz);

	return old_addr;
}

uint64
do_mmap(struct file *file, unsigned long addr,
	unsigned long len, int prot,
	int flag, unsigned long offset)
{
	// int ret;
	if((offset + PGROUNDUP(len)) < offset)//if len < 0
		panic("do mmap 1");
	if(offset & ~PGMASK)
		panic("do mmap 2");// offset is not page aligned


	// print_all_vma();
	return do_mmap_pgoff(file, addr, len, prot, flag, offset >> PGSHIFT);	
	
}

uint64 do_mmap_pgoff(struct file * file, unsigned long addr,
			unsigned long len, int prot,
			int flags, unsigned long pgoff)
{
	struct proc *p = myproc();
	struct vm_area_struct *vma = kmalloc(sizeof(struct vm_area_struct));
	struct mm_struct *mm = p->mm;

	vma->vm_file = file;
	vma->vm_start = get_unmapped_area(len);
	vma->vm_end = vma->vm_start + len;
	vma->vm_flags = flags;
	vma->vm_page_prot = prot;
	vma->vm_pgoff = pgoff;

	// add to mm_struct list
	vma->vm_next = mm->mmap;
	acquire(&mm->lock);
	mm->mmap = vma;
	mm->map_count++;
	release(&mm->lock);

	return vma->vm_start;
}

// /*
//  * We cannot adjust vm_start, vm_end, vm_pgoff fields of a vma that
//  * is already present in an i_mmap tree without adjusting the tree.
//  * The following helper function should be used when such adjustments
//  * are necessary.  The "insert" vma (if any) is to be inserted
//  * before we drop the necessary locks.
//  */
// void vma_adjust(struct vm_area_struct *vma, unsigned long start,
// 	unsigned long end, pgoff_t pgoff, struct vm_area_struct *insert)
// {
// 	struct mm_struct *mm = vma->vm_mm;
// 	struct vm_area_struct *next = vma->vm_next;
// 	struct vm_area_struct *importer = NULL;
// 	struct address_space *mapping = NULL;
// 	struct prio_tree_root *root = NULL;
// 	struct file *file = vma->vm_file;
// 	// struct anon_vma *anon_vma = NULL;
// 	long adjust_next = 0;
// 	int remove_next = 0;

// 	if (next && !insert) {
// 		if (end >= next->vm_end) {
// 			/*
// 			 * vma expands, overlapping all the next, and
// 			 * perhaps the one after too (mprotect case 6).
// 			 */
// again:
// 			remove_next = 1 + (end > next->vm_end);
// 			end = next->vm_end;
// 			// anon_vma = next->anon_vma;
// 			importer = vma;
// 		} else if (end > next->vm_start) {
// 			/*
// 			 * vma expands, overlapping part of the next:
// 			 * mprotect case 5 shifting the boundary up.
// 			 */
// 			adjust_next = (end - next->vm_start) >> PAGE_SHIFT;
// 			// anon_vma = next->anon_vma;
// 			importer = vma;
// 		} else if (end < vma->vm_end) {
// 			/*
// 			 * vma shrinks, and !insert tells it's not
// 			 * split_vma inserting another: so it must be
// 			 * mprotect case 4 shifting the boundary down.
// 			 */
// 			adjust_next = - ((vma->vm_end - end) >> PAGE_SHIFT);
// 			// anon_vma = next->anon_vma;
// 			importer = next;
// 		}
// 	}
// }

// //zyy: a region is represented as [addr, end), end is not included.
// /* Look up the first VMA which satisfies  addr < vm_end,  NULL if none. */
// struct vm_area_struct * find_vma(struct mm_struct * mm, unsigned long addr)
// {
// 	struct vm_area_struct *vma = NULL;

// 	if (mm) {
// 		/* Check the cache first. */
// 		/* (Cache hit rate is typically around 35%.) */
// 		vma = mm->mmap_cache;
// 		if (!(vma && vma->vm_end > addr && vma->vm_start <= addr)) {
// 			struct rb_node * rb_node;

// 			rb_node = mm->mm_rb.rb_node;
// 			vma = NULL;

// 			while (rb_node) {
// 				struct vm_area_struct * vma_tmp;

// 				vma_tmp = rb_entry(rb_node,
// 						struct vm_area_struct, vm_rb);

// 				if (vma_tmp->vm_end > addr) {
// 					vma = vma_tmp;
// 					if (vma_tmp->vm_start <= addr)
// 						break;
// 					rb_node = rb_node->rb_left;
// 				} else
// 					rb_node = rb_node->rb_right;
// 			}
// 			if (vma)
// 				mm->mmap_cache = vma;
// 		}
// 	}
// 	return vma;
// }

// static inline int is_mergeable_vma(struct vm_area_struct *vma,
// 			struct file *file, unsigned long vm_flags)
// {
// 	if (vma->vm_flags != vm_flags)
// 		return 0;
// 	if (vma->vm_file != file)
// 		return 0;
// 	// if (vma->vm_ops && vma->vm_ops->close)
// 		// return 0;
// 	return 1;
// }


// static int
// can_vma_merge_before(struct vm_area_struct *vma, unsigned long vm_flags,
// 	struct file *file, pgoff_t vm_pgoff)
// {
// 	if (is_mergeable_vma(vma, file, vm_flags)) {
// 		if (vma->vm_pgoff == vm_pgoff)
// 			return 1;
// 	}
// 	return 0;
// }

// static int
// can_vma_merge_after(struct vm_area_struct *vma, unsigned long vm_flags,
// 	struct file *file, pgoff_t vm_pgoff)
// {
// 	if (is_mergeable_vma(vma, file, vm_flags)) {
// 		pgoff_t vm_pglen;
// 		vm_pglen = (vma->vm_end - vma->vm_start) >> PAGE_SHIFT;
// 		if (vma->vm_pgoff + vm_pglen == vm_pgoff)
// 			return 1;
// 	}
// 	return 0;
// }
// /*
//  * Given a mapping request (addr,end,vm_flags,file,pgoff), figure out
//  * whether that can be merged with its predecessor or its successor.
//  * Or both (it neatly fills a hole).
//  *
//  * In most cases - when called for mmap, brk or mremap - [addr,end) is
//  * certain not to be mapped by the time vma_merge is called; but when
//  * called for mprotect, it is certain to be already mapped (either at
//  * an offset within prev, or at the start of next), and the flags of
//  * this area are about to be changed to vm_flags - and the no-change
//  * case has already been eliminated.
//  *
//  * The following mprotect cases have to be considered, where AAAA is
//  * the area passed down from mprotect_fixup, never extending beyond one
//  * vma, PPPPPP is the prev vma specified, and NNNNNN the next vma after:
//  *
//  *     AAAA             AAAA                AAAA          AAAA
//  *    PPPPPPNNNNNN    PPPPPPNNNNNN    PPPPPPNNNNNN    PPPPNNNNXXXX
//  *    cannot merge    might become    might become    might become
//  *                    PPNNNNNNNNNN    PPPPPPPPPPNN    PPPPPPPPPPPP 6 or
//  *    mmap, brk or    case 4 below    case 5 below    PPPPPPPPXXXX 7 or
//  *    mremap move:                                    PPPPNNNNNNNN 8
//  *        AAAA
//  *    PPPP    NNNN    PPPPPPPPPPPP    PPPPPPPPNNNN    PPPPNNNNNNNN
//  *    might become    case 1 below    case 2 below    case 3 below
//  *
//  * Odd one out? Case 8, because it extends NNNN but needs flags of XXXX:
//  * mprotect_fixup updates vm_flags & vm_page_prot on successful return.
//  */
// struct vm_area_struct *vma_merge(struct mm_struct *mm,
// 			struct vm_area_struct *prev, unsigned long addr,
// 			unsigned long end, unsigned long vm_flags,
// 		     	struct anon_vma *anon_vma, struct file *file,
// 			pgoff_t pgoff, struct mempolicy *policy)
// {
// 	pgoff_t pglen = (end - addr) >> PAGE_SHIFT;
// 	struct vm_area_struct *area, *next;

// 	if(prev)
// 		next = prev->vm_next;
// 	else 
// 		next = mm->mmap;
// 	area = next;
// 	if (next && next->vm_end == end)		/* cases 6, 7, 8 */
// 		next = next->vm_next;

// 	/*
// 	 * Can it merge with the predecessor?
// 	 */
// 	if (prev && prev->vm_end == addr &&
// 			can_vma_merge_after(prev, vm_flags, file, pgoff)) {
// 		/*
// 		 * OK, it can.  Can we now merge in the successor as well?
// 		 */
// 		if (next && end == next->vm_start &&
// 				can_vma_merge_before(next, vm_flags, file, pgoff+pglen)) {
// 							/* cases 1, 6 */
// 			vma_adjust(prev, prev->vm_start,
// 				next->vm_end, prev->vm_pgoff, NULL);
// 		} else					/* cases 2, 5, 7 */
// 			vma_adjust(prev, prev->vm_start,
// 				end, prev->vm_pgoff, NULL);
// 		return prev;
// 	}

// 	/*
// 	 * Can this new request be merged in front of next?
// 	 */
// 	if (next && end == next->vm_start &&
// 			can_vma_merge_before(next, vm_flags, file, pgoff+pglen)) {
// 		if (prev && addr < prev->vm_end)	/* case 4 */
// 			vma_adjust(prev, prev->vm_start,
// 				addr, prev->vm_pgoff, NULL);
// 		else					/* cases 3, 8 */
// 			vma_adjust(area, addr, next->vm_end,
// 				next->vm_pgoff - pglen, NULL);
// 		return area;
// 	}

// 	return NULL;
// }