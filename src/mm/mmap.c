/**
 * @file mmap.c
 * @author DavidZyy (1929772352@qq.com)
 * @brief 这个c文件主要参考了linux2.6版本的代码。有的函数直接复制过来了，有的针对我们
 * 自己的情形作了简化或者很小的改动。在此向kernel作者们致敬。 
 * @version 0.1
 * @date 2022-06-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */
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

#ifdef DEBUG_MM_RB
static int browse_rb(struct rb_root *root)
{
	int i = 0, j;
	struct rb_node *nd, *pn = NULL;
	unsigned long prev = 0, pend = 0;

	for (nd = rb_first(root); nd; nd = rb_next(nd)) {
		struct vm_area_struct *vma;
		vma = rb_entry(nd, struct vm_area_struct, vm_rb);
		if (vma->vm_start < prev)
			printf("vm_start %lx prev %lx\n", vma->vm_start, prev), i = -1;
		if (vma->vm_start < pend)
			printf("vm_start %lx pend %lx\n", vma->vm_start, pend);
		if (vma->vm_start > vma->vm_end)
			printf("vm_end %lx < vm_start %lx\n", vma->vm_end, vma->vm_start);
		i++;
		pn = nd;
		prev = vma->vm_start;
		pend = vma->vm_end;
	}
	j = 0;
	for (nd = pn; nd; nd = rb_prev(nd)) {
		j++;
	}
	if (i != j)
		printf("backwards %d, forwards %d\n", j, i), i = 0;
	return i;
}

void validate_mm(struct mm_struct *mm)
{
	int bug = 0;
	int i = 0;
	struct vm_area_struct *tmp = mm->mmap;
	while (tmp) {
		tmp = tmp->vm_next;
		i++;
	}
	if (i != mm->map_count)
		printk("map_count %d vm_next %d\n", mm->map_count, i), bug = 1;
	i = browse_rb(&mm->mm_rb);
	if (i != mm->map_count)
		printk("map_count %d rb %d\n", mm->map_count, i), bug = 1;
	// BUG_ON(bug);
}
#else
#define validate_mm(mm) do { } while (0)
#endif


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

static inline void
__vma_link_list(struct mm_struct *mm, struct vm_area_struct *vma,
		struct vm_area_struct *prev, struct rb_node *rb_parent)
{
	if (prev) {
		vma->vm_next = prev->vm_next;
		prev->vm_next = vma;
	} else {
		mm->mmap = vma;
		if (rb_parent)
			vma->vm_next = rb_entry(rb_parent,
					struct vm_area_struct, vm_rb);
		else
			vma->vm_next = NULL;
	}
}

void __vma_link_rb(struct mm_struct *mm, struct vm_area_struct *vma,
		struct rb_node **rb_link, struct rb_node *rb_parent)
{
	rb_link_node(&vma->vm_rb, rb_parent, rb_link);
	rb_insert_color(&vma->vm_rb, &mm->mm_rb);
}


static void
__vma_link(struct mm_struct *mm, struct vm_area_struct *vma,
	struct vm_area_struct *prev, struct rb_node **rb_link,
	struct rb_node *rb_parent)
{
	__vma_link_list(mm, vma, prev, rb_parent);
	__vma_link_rb(mm, vma, rb_link, rb_parent);
}

static void vma_link(struct mm_struct *mm, struct vm_area_struct *vma,
			struct vm_area_struct *prev, struct rb_node **rb_link,
			struct rb_node *rb_parent)
{


	__vma_link(mm, vma, prev, rb_link, rb_parent);
	mm->map_count++;
	validate_mm(mm);
}

static inline void
__vma_unlink(struct mm_struct *mm, struct vm_area_struct *vma,
		struct vm_area_struct *prev)
{
	prev->vm_next = vma->vm_next;
	rb_erase(&vma->vm_rb, &mm->mm_rb);
	if (mm->mmap_cache == vma)
		mm->mmap_cache = prev;
}

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
	    vma_merge(mm, prev, addr, addr + len, vm_flags, NULL, pgoff))
		goto out;


	vma = (vm_area_struct_t *)kzalloc(sizeof(vm_area_struct_t));
	if(!vma)
		panic("dommapoff5");

	vma->vm_file = NULL;
	vma->vm_start = addr;
	vma->vm_end = vma->vm_start + len;
	vma->vm_flags = flags;
	vma->vm_page_prot = prot;
	vma->vm_pgoff = pgoff;

	if(file){
		vma->vm_file = file;
  	#ifdef TODO
  	todo("filedup here??");
  	#endif
		filedup(file);
	}
	else if(vm_flags & VM_SHARED){
		/* shared anonymous region, mainly used for interprocess communications */
	}
	vma_link(mm, vma, prev, rb_link, rb_parent);
	mm->total_vm += len >> PAGE_SHIFT;
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
 * 相比于linux的原版函数的简化版，省略了mprotect的情况的区间合并。
 * 由于vma可能是anonymous或者mapping-file，所以要对这两种情况分类讨论。
 * 根据vma->file和vma->anon哪个不为空来判断vma的类型。
 * 
 * 对一个进程的地址空间来说，调整vma在红黑树中的位置。
 * 对于反向映射来说：
 * 1.如果vma为匿名，调整其在anon_vma链表中的位置。
 * 2.如果为mapping-file,调整其（在linux2.6优先级树）中的位置。
 */
void vma_adjust(struct vm_area_struct *vma, unsigned long start,
	unsigned long end, pgoff_t pgoff)
{
	#ifdef TODO
	todo("adjust reverse mapping!");
	#endif
	struct mm_struct *mm = vma->vm_mm;
	struct vm_area_struct *next = vma->vm_next;

	if (next) {
		if (end == next->vm_end) {
			/*
			 * case 1，next vma需要合并进当前vma中，需要从红黑树和链表中移除。
			 */
			__vma_unlink(mm, next, vma);
		} 
	}

	/* 虽然vma->end改变了，但是相对于其他vma->end的大小没变，所以不需要调整
			其在rbtree和链表中的位置。 */
	vma->vm_start = start;
	vma->vm_end = end;
	vma->vm_pgoff = pgoff;	
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
	struct file *file, pgoff_t vm_pgoff)
{
	if (is_mergeable_vma(vma, file, vm_flags)) {
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
can_vma_merge_after(struct vm_area_struct *vma, unsigned long vm_flags, struct file *file, pgoff_t vm_pgoff)
{
	if (is_mergeable_vma(vma, file, vm_flags)) {
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
		  struct file *file, pgoff_t pgoff)
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
	if (prev && prev->vm_end == addr && can_vma_merge_after(prev, vm_flags, file, pgoff)) {
		/*
		 * OK, it can.  Can we now merge in the successor as well?
		 */
		if (next && end == next->vm_start &&
				can_vma_merge_before(next, vm_flags, file, pgoff+pglen))
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
			can_vma_merge_before(next, vm_flags, file, pgoff+pglen)) {
		/* cases 3*/
			vma_adjust(next, addr, next->vm_end, next->vm_pgoff - pglen);
		return next;
	}

	return NULL;
}