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
#define __MODULE_NAME__ MEMORY 
#include "debug.h"

#include "mm/mm_types.h"
#include "mm/mm.h"
#include "rbtree.h"
#include "utils.h"
#include "memlayout.h"

static void
unmap_vmas(vm_area_struct_t *vma, uint64_t start_addr, uint64_t end_addr){
	uint64_t start = start_addr;

	for(; vma && vma->vm_start < end_addr; vma = vma->vm_next){
		uint64_t end;

		start = max(vma->vm_start, start_addr);
		if(start >= vma->vm_end)
			continue;
		end = min(vma->vm_end, end_addr);
		if (end <= vma->vm_start)
			continue;

    // write back to disk
    if (vma->vm_flags & MAP_SHARED) {
        // write back page first, than write back to disk
        todo("write back shared vma");
    }

		//zyy: my ugly code...
		uint64_t va = PGROUNDUP(start);
		pte_t *pte;
		struct proc *p = myproc();
    // if a virtual address has been mapped to physic address,
    // unmap it, otherwise do noting.
    if (va <= PGROUNDDOWN(end))
        for (int a = va; a <= PGROUNDDOWN(end); a += PGSIZE) {
            if ((pte = walk(p->pagetable, a, 0)) == 0)
                continue;
            if ((*pte & PTE_V) == 0)
                continue;

            uvmunmap(p->pagetable, a, 1, 1);
        }
	}
}

/**
 * @brief lazy allocation，访问未分配的堆导致缺页？
 * 
 */
static int do_anonymous_page(struct mm_struct *mm, unsigned long address, unsigned int write)
{
    uint64_t pa;

    /* 写触发的错误 */
    if(write){
        acquire(&mm->mm_lock);

        pa = (uint64)kalloc();
        if (mappages(myproc()->pagetable, PGROUNDDOWN(address), PGSIZE, pa, PTE_U | PTE_V | PTE_W | PTE_R) < 0)
            ER();

        release(&mm->mm_lock);
    }
    /* 读触发的错误，linux中将其映射到一个共享的0页中，清除write位，再要写这个页时，触发
    copy on write，分配一个新页 */
    ER();
}

/**
 * @brief mmap file导致的缺页
 * 
 */
static int do_linear_fault(uint64_t address){
    filemap_nopage(address);
}

/**
 * @brief 
 * 
 */
static int do_swap_page(struct mm_struct *mm, struct vm_area_struct *vma,
		unsigned long address, pte_t *page_table, unsigned int write, pte_t orig_pte)
{
    ERROR(" not implemented");
}

/**
 * @brief write protect page，多为copy on write
 * 
 */
static int do_wp_page( unsigned long address, pte_t *pte)
{
    if(pte == 0 || (*pte & PTE_U) == 0 || (*pte & PTE_V) == 0 || (*pte & PTE_COW) == 0)
        ER();

    return __cow_copy(address, pte);
}

static inline int handle_pte_fault(struct mm_struct *mm,
		struct vm_area_struct *vma, unsigned long address,
		pte_t *pte, unsigned int write)
{
    pte_t entry;
    spinlock_t *ptl;

    entry = *pte;
    /* 如果pte不存在(无效)  x86是present，riscv是valid */
    if (!pte_valid(entry)) {
		if (pte_none(entry)) {
			if (vma->vm_file) {
					return do_linear_fault(address);
			}
			return do_anonymous_page(mm, vma, address,
						 pte, write);
		}
        /* Linux非线性文件映射，我们没有实现 */
		// if (pte_file(entry))
		// 	return do_nonlinear_fault(mm, vma, address,
		// 			pte, pmd, write, entry);
		return do_swap_page(mm, vma, address,
					pte, write, entry);
	}

    /* pte valid (copy on write) */
    if(write){
        if(!pte_write(entry)){
            /* write protect 写保护，即cow*/
            return do_wp_page(address, pte);
        }
    }

    ER();
    return 0;
}

/**
 * @brief 如果pte valid，则为copy on write，否则为demand paging
 * 
 * @param mm 
 * @param vma 
 * @param address 
 * @param write 
 * @return int 
 */
int handle_mm_fault(struct mm_struct *mm, struct vm_area_struct *vma,
		unsigned long address, unsigned int write)
{
  pte_t *pte;
  proc_t *p = myproc();

  pte = walk(p->pagetable, address, 0);

  return handle_pte_fault(mm, vma, address, pte, write);
}