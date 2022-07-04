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

static inline int handle_pte_fault(struct mm_struct *mm,
		struct vm_area_struct *vma, unsigned long address,
		pte_t *pte, unsigned int flags)
{

}

/**
 * @brief 如果pte存在，则为copy on write，否则为demand paging
 * 
 * @param mm 
 * @param vma 
 * @param address 
 * @param flags 
 * @return int 
 */
int handle_mm_fault(struct mm_struct *mm, struct vm_area_struct *vma,
		unsigned long address, unsigned int flags)
{
  pte_t *pte;
  proc_t *p = myproc();

  pte = walk(p->pagetable, address, 0);

  return handle_pte_fault(mm, vma, address, pte, flags);
}