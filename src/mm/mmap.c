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
#include "memlayout.h"

/*
 * Optimisation macro.  It is equivalent to:
 *      (x & bit1) ? bit2 : 0
 * but this version is faster.
 * ("bit1" and "bit2" must be single bits)
 * bit1和bit2中的较大值除以较小值
 * (x & bit1)等于bit1或者0
 */
#define _calc_vm_trans(x, bit1, bit2) \
  ((bit1) <= (bit2) ? ((x) & (bit1)) * ((bit2) / (bit1)) \
   : ((x) & (bit1)) / ((bit1) / (bit2)))

/*
 * Combine the mmap "prot" argument into "vm_flags" used internally.
 */
static inline unsigned long
calc_vm_prot_bits(unsigned long prot)
{
	return _calc_vm_trans(prot, PROT_READ,  VM_READ ) |
	       _calc_vm_trans(prot, PROT_WRITE, VM_WRITE) |
	       _calc_vm_trans(prot, PROT_EXEC,  VM_EXEC );
}

// /*
//  * Combine the mmap "flags" argument into "vm_flags" used internally.
//  */
// static inline unsigned long
// calc_vm_flag_bits(unsigned long flags)
// {
// 	return _calc_vm_trans(flags, MAP_GROWSDOWN,  VM_GROWSDOWN ) |
// 	       _calc_vm_trans(flags, MAP_DENYWRITE,  VM_DENYWRITE ) |
// 	       _calc_vm_trans(flags, MAP_EXECUTABLE, VM_EXECUTABLE) |
// 	       _calc_vm_trans(flags, MAP_LOCKED,     VM_LOCKED    );
// }


uint64
file_get_unmapped_area(uint64 len){
	uint64 old_addr;
	struct proc *p = myproc();

	old_addr = p->cur_mmap_sz;
  // printf(rd("cur_de 1: %p\n"), myproc()->cur_mmap_sz);
	p->cur_mmap_sz += PGROUNDUP(len);
  // printf(ylw("cur_de 2: %p\n"), myproc()->cur_mmap_sz);

	return old_addr;
}

uint64
arch_get_unmapped_area(struct file *file, unsigned long addr, unsigned long len,
		unsigned long pgoff, unsigned long flags){

	uint64_t start_addr;
	mm_struct_t *mm = myproc()->mm;
	vm_area_struct_t *vma;

	if(len > USER_TASK_SIZE)
		panic("arch_get_unmapped_area 1");	
	addr = PGROUNDUP(addr);
	/* the mapping area is not zero(means the mapping address is specified(predefined)) */
	if(addr && addr + len <= USER_TASK_SIZE){
		vma = find_vma(mm, addr);
		if(!vma || addr + len <= vma->vm_start)
			return addr;
	}

	/* the "addr" is zero, means that the addr is not specified, the kernel find one free memory region */
	start_addr = addr = mm->free_area_cache;
	for(vma = find_vma(mm, addr); ; vma = vma->vm_next){
		if(addr + len > USER_TASK_SIZE){
			/* no mapping area(run out) */
			if(start_addr == PGROUNDUP(USER_TASK_SIZE/2)){
				panic("arch_get_unmapped_area 2");	
			}
			/* 默认从用户空间的一半开始找 */
			start_addr = addr = PGROUNDUP(USER_TASK_SIZE/2);
			vma = find_vma(mm, addr);
		}	
		/* find a free memory region */
		if(!vma || addr + len <= vma->vm_start){
			mm->free_area_cache = addr + len;
			return addr;
		}
		addr = vma->vm_end;
	}
}

/**
 * @brief the method it invoked depends on whether the linear address interval 
 * is used for file memory mapping or for anonymous memory mapping.
 * 
 * 一般情况是file不指定addr，arch指定addr
 * @param len 
 * @return uint64 
 */
uint64
get_unmapped_area(struct file *file, unsigned long addr, unsigned long len,
		unsigned long pgoff, unsigned long flags){
	if(flags & MAP_FIXED){
		panic("get unmapped area 1: not implemented");
	}

	/* mapping for file */ 
	if(file != NULL)
		return file_get_unmapped_area(len);
	
	return arch_get_unmapped_area(file, addr, len, pgoff, flags);
}

static struct vm_area_struct *
find_vma_prepare(struct mm_struct *mm, unsigned long addr,
		struct vm_area_struct **pprev, struct rb_node ***rb_link,
		struct rb_node ** rb_parent)
{
	struct vm_area_struct * vma;
	struct rb_node ** __rb_link, * __rb_parent, * rb_prev;

	__rb_link = &mm->mm_rb.rb_node;
	rb_prev = __rb_parent = NULL;
	vma = NULL;

	while (*__rb_link) {
		struct vm_area_struct *vma_tmp;

		__rb_parent = *__rb_link;
		vma_tmp = rb_entry(__rb_parent, struct vm_area_struct, vm_rb);

		if (vma_tmp->vm_end > addr) {
			vma = vma_tmp;
			if (vma_tmp->vm_start <= addr)
				return vma;
			__rb_link = &__rb_parent->rb_left;
		} else {
			rb_prev = __rb_parent;
			__rb_link = &__rb_parent->rb_right;
		}
	}

	*pprev = NULL;
	if (rb_prev)
		*pprev = rb_entry(rb_prev, struct vm_area_struct, vm_rb);
	*rb_link = __rb_link;
	*rb_parent = __rb_parent;
	return vma;
}

/**
 * @brief 映射一段内存区域
 * 
 * @param file 
 * @param addr 
 * @param len 
 * @param prot 
 * @param flag 
 * @param offset 
 * @return uint64 
 */
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
	struct vm_area_struct *vma, *prev;
	struct mm_struct *mm = p->mm;
	rb_node_t **rb_link, *rb_parent;
	uint32_t vm_flags;
	

	if(!len)
		panic("do mmap pgoff 1");
		// return addr;
	len = PGROUNDUP(len);
	if(!len || len > USER_TASK_SIZE)
		panic("do mmap pgoff 2");

	if(mm->map_count > DEFAULT_MAX_MAP_COUNT)
		panic("do mmap pgoff 3");

	addr = get_unmapped_area(file, addr, len, pgoff, flags);

	vm_flags = calc_vm_prot_bits(prot);

	for(;;){
		vma = find_vma_prepare(mm, addr, &prev, &rb_link, &rb_parent);
		if(!vma || vma->vm_start >= addr + len)
			break;
		if(do_munmap(mm, addr, len))
			panic("do mmap pgoff 4");
	}

	/*
	 * Can we just expand an old private anonymous mapping?
	 * The VM_SHARED test is necessary because shmem_zero_setup
	 * will create the file object for a shared anonymous map below.
	 * 如果当前的内存区域可以和已有的合并，就不需要再创建一个新的vma_area_strcut
	 * 来描述它了，合并进已有的即可。
	 */
	if (!file && !(vm_flags & VM_SHARED) &&
	    vma_merge(mm, prev, addr, addr + len, vm_flags, NULL, NULL, pgoff))
		goto out;

	// /* increase the ref to the file */
	// if(file != NULL){
		// filedup(file);
	// }

	vma->vm_file = file;
	vma->vm_start = addr;
	vma->vm_end = vma->vm_start + len;
	vma->vm_flags = flags;
	vma->vm_page_prot = prot;
	vma->vm_pgoff = pgoff;

	/* insert to mm_struct list */
	vma->vm_next = mm->mmap;
	acquire(&mm->lock);
	mm->mmap = vma;
	mm->map_count++;
	release(&mm->lock);

out:
	return vma->vm_start;
}


static void
remove_vm_struct(vm_area_struct_t *vma){
	struct file *f = vma->vm_file;

	if(f){
		fileclose(f);
	}
	kfree(vma);
}

void
exit_mmap(mm_struct_t *mm){
	vm_area_struct_t *vma;

	vma = mm->mmap;
	mm->mmap = mm->mmap_cache = NULL;

	while(vma){
		vm_area_struct_t *next = vma->vm_next;
		remove_vm_struct(vma);
		vma = next;
	}
}

/* zyy: a region is represented as [addr, end), end is not included in the region */
/* Look up the first VMA which satisfies  addr < vm_end,  NULL if none. */
struct vm_area_struct * find_vma(struct mm_struct * mm, unsigned long addr)
{
	struct vm_area_struct *vma = NULL;

	if (mm) {
		/* Check the cache first. */
		/* (Cache hit rate is typically around 35%.) */
		vma = mm->mmap_cache;
		if (!(vma && vma->vm_end > addr && vma->vm_start <= addr)) {
			struct rb_node * rb_node;

			rb_node = mm->mm_rb.rb_node;
			vma = NULL;

			while (rb_node) {
				struct vm_area_struct * vma_tmp;

				vma_tmp = rb_entry(rb_node,
						struct vm_area_struct, vm_rb);

				/* 只要存在vm_end大于addr， vma就不为NULL */
				if (vma_tmp->vm_end > addr) {
					vma = vma_tmp;
					if (vma_tmp->vm_start <= addr)
						break;
					rb_node = rb_node->rb_left;
				} else
					rb_node = rb_node->rb_right;
			}
			if (vma)
				mm->mmap_cache = vma;
		}
	}
	return vma;
}

/*
 * We cannot adjust vm_start, vm_end, vm_pgoff fields of a vma that
 * is already present in an i_mmap tree without adjusting the tree.
 * The following helper function should be used when such adjustments
 * are necessary.  The "insert" vma (if any) is to be inserted
 * before we drop the necessary locks.
 * 
 * 简化版，相比于linux的原版，省略了mprotect的情况的区间合并。
 */
void vma_adjust(struct vm_area_struct *vma, unsigned long start,
	unsigned long end, pgoff_t pgoff)
{
	#ifdef TODO
	todo("influence page reclaiming!");
	#endif
	struct mm_struct *mm = vma->vm_mm;
	struct vm_area_struct *next = vma->vm_next;
	struct vm_area_struct *importer = NULL;
	struct address_space *mapping = NULL;
	struct prio_tree_root *root = NULL;
	struct file *file = vma->vm_file;
	struct anon_vma *anon_vma = NULL;
	long adjust_next = 0;
	int remove_next = 0;

	if (next) {
		if (end == next->vm_end) {
			/*
			 * case 1
			 */
			remove_next = 1;
			// end = next->vm_end;
			// anon_vma = next->anon_vma;
			// importer = vma;
		} 
// 	else if (end > next->vm_start) {
// 			/*
// 			 * vma expands, overlapping part of the next:
// 			 * mprotect case 5 shifting the boundary up.
// 			 */
// 			adjust_next = (end - next->vm_start) >> PAGE_SHIFT;
// 			anon_vma = next->anon_vma;
// 			importer = vma;
// 		} else if (end < vma->vm_end) {
// 			/*
// 			 * vma shrinks, and !insert tells it's not
// 			 * split_vma inserting another: so it must be
// 			 * mprotect case 4 shifting the boundary down.
// 			 */
// 			adjust_next = - ((vma->vm_end - end) >> PAGE_SHIFT);
// 			anon_vma = next->anon_vma;
// 			importer = next;
// 		}
	}

	if(file) {
		mapping = file->ep->i_mapping;
		root = &mapping->i_mmap;
	}

	/*
	 * When changing only vma->vm_end, we don't really need
	 * anon_vma lock: but is that case worth optimizing out?
	 */
	if (vma->anon_vma)
		anon_vma = vma->anon_vma;
	if (anon_vma) {
		spin_lock(&anon_vma->lock);
		/*
		 * Easily overlooked: when mprotect shifts the boundary,
		 * make sure the expanding vma has anon_vma set if the
		 * shrinking vma had, to cover any anon pages imported.
		 */
		if (importer && !importer->anon_vma) {
			importer->anon_vma = anon_vma;
			/* 将importer添加到anon_vma链表中 */
			__anon_vma_link(importer);
		}
	}
	
}

static inline int is_mergeable_vma(struct vm_area_struct *vma,
			struct file *file, unsigned long vm_flags)
{
	if (vma->vm_flags != vm_flags)
		return 0;
	if (vma->vm_file != file)
		return 0;
	// if (vma->vm_ops && vma->vm_ops->close)
		// return 0;
	return 1;
}

static inline int is_mergeable_anon_vma(struct anon_vma *anon_vma1,
					struct anon_vma *anon_vma2)
{
	return !anon_vma1 || !anon_vma2 || (anon_vma1 == anon_vma2);
}

/*
 * Return true if we can merge this (vm_flags,anon_vma,file,vm_pgoff)
 * in front of (at a lower virtual address and file offset than) the vma.
 *
 * We cannot merge two vmas if they have differently assigned (non-NULL)
 * anon_vmas, nor if same anon_vma is assigned but offsets incompatible.
 *
 * We don't check here for the merged mmap wrapping around the end of pagecache
 * indices (16TB on ia32) because do_mmap_pgoff() does not permit mmap's which
 * wrap, nor mmaps which cover the final page at index -1UL.
 */
static int
can_vma_merge_before(struct vm_area_struct *vma, unsigned long vm_flags,
	struct anon_vma *anon_vma, struct file *file, pgoff_t vm_pgoff)
{
	if (is_mergeable_vma(vma, file, vm_flags) &&
	    is_mergeable_anon_vma(anon_vma, vma->anon_vma)) {
		if (vma->vm_pgoff == vm_pgoff)
			return 1;
	}
	return 0;
}

/*
 * Return true if we can merge this (vm_flags,anon_vma,file,vm_pgoff)
 * beyond (at a higher virtual address and file offset than) the vma.
 *
 * We cannot merge two vmas if they have differently assigned (non-NULL)
 * anon_vmas, nor if same anon_vma is assigned but offsets incompatible.
 */
static int
can_vma_merge_after(struct vm_area_struct *vma, unsigned long vm_flags, struct anon_vma *anon_vma, struct file *file, pgoff_t vm_pgoff)
{
	if (is_mergeable_vma(vma, file, vm_flags) &&
	    is_mergeable_anon_vma(anon_vma, vma->anon_vma)) {
		pgoff_t vm_pglen;
		vm_pglen = (vma->vm_end - vma->vm_start) >> PAGE_SHIFT;
		if (vma->vm_pgoff + vm_pglen == vm_pgoff)
			return 1;
	}
	return 0;
}

/*
 * Given a mapping request (addr,end,vm_flags,file,pgoff), figure out
 * whether that can be merged with its predecessor or its successor.
 * Or both (it neatly fills a hole).
 *
 * In most cases - when called for mmap, brk or mremap - [addr,end) is
 * certain not to be mapped by the time vma_merge is called; but when
 * called for mprotect, it is certain to be already mapped (either at
 * an offset within prev, or at the start of next), and the flags of
 * this area are about to be changed to vm_flags - and the no-change
 * case has already been eliminated.
 *
 * The following mprotect cases have to be considered, where AAAA is
 * the area passed down from mprotect_fixup, never extending beyond one
 * vma, PPPPPP is the prev vma specified, and NNNNNN the next vma after:
 *
 *     AAAA             AAAA                AAAA          AAAA
 *    PPPPPPNNNNNN    PPPPPPNNNNNN    PPPPPPNNNNNN    PPPPNNNNXXXX
 *    cannot merge    might become    might become    might become
 *                    PPNNNNNNNNNN    PPPPPPPPPPNN    PPPPPPPPPPPP 6 or
 *    mmap, brk or    case 4 below    case 5 below    PPPPPPPPXXXX 7 or
 *    mremap move:                                    PPPPNNNNNNNN 8
 *        AAAA
 *    PPPP    NNNN    PPPPPPPPPPPP    PPPPPPPPNNNN    PPPPNNNNNNNN
 *    might become    case 1 below    case 2 below    case 3 below
 *
 * Odd one out? Case 8, because it extends NNNN but needs flags of XXXX:
 * mprotect_fixup updates vm_flags & vm_page_prot on successful return.
 * 
 * 上面图示中，如果是mprotect调用此函数，则AAAA的区域已经映射过了，对应case 4,5,6,7,8；
 * 如果是mmap，brk等调用此函数，则AAAA的区域是没有映射过的，对应case 1,2,3
 * 
 * 为了简单起见，我们目前不实现mprotect的功能，这样只需要考虑case 1,2,3；
 * 从而该函数相对于linux中的原版简化了很多。
 * 
 */
struct vm_area_struct *vma_merge(struct mm_struct *mm,
			struct vm_area_struct *prev, unsigned long addr,
			unsigned long end, unsigned long vm_flags,
		     	struct anon_vma *anon_vma, struct file *file, pgoff_t pgoff)
{
	pgoff_t pglen = (end - addr) >> PAGE_SHIFT;
	struct vm_area_struct *next;

	/* 确定next vma，如果prev不存在，说明新的区间在所有区间的最前面，则next为mm现有vma的第一个（mm->mmap） */
	if(prev)
		next = prev->vm_next;
	else 
		next = mm->mmap;

	/*
	 * Can it merge with the predecessor?
	 */
	if (prev && prev->vm_end == addr && can_vma_merge_after(prev, vm_flags, anon_vma, file, pgoff)) {
		/*
		 * OK, it can.  Can we now merge in the successor as well?
		 * 最后is_mergeable_anon_vma(prev->anon_vma, next->anon_vma)做一遍检查，防止当前anon_vma为NULL，但是prev->anon_vma和next->anon_vma不同的情况
		 */
		if (next && end == next->vm_start &&
				can_vma_merge_before(next, vm_flags, anon_vma, file, pgoff+pglen) &&
				is_mergeable_anon_vma(prev->anon_vma, next->anon_vma)) 
		{
			/* cases 1, 新区间和前后两个区间合并成一个 */
			vma_adjust(prev, prev->vm_start, next->vm_end, prev->vm_pgoff);
		} else					/* cases 2, 新区间和前一个区间合并成一个 */
			vma_adjust(prev, prev->vm_start, end, prev->vm_pgoff);
		return prev;
	}

	/*
	 * Can this new request be merged in front of next?
	 */
	if (next && end == next->vm_start &&
			can_vma_merge_before(next, vm_flags, anon_vma, file, pgoff+pglen)) {
		/* cases 3*/
			vma_adjust(next, addr, next->vm_end, next->vm_pgoff - pglen);
		return next;
	}

	return NULL;
}