#include "common.h"
#include "platform.h"
#include "mm/alloc.h"
#include "fs/fs.h"
#include "fs/file.h"
#include "mm/vm.h"
#include "kernel/proc.h"
#include "defs.h"
#include "debug.h"
#include "page.h"
#include "radix-tree.h"

/*
 * a rather lightweight function, finding and getting a reference to a
 * hashed page atomically.
 */
uint64 * find_get_page(struct address_space *mapping, unsigned long offset)
{
	// page_t *page;
  uint64 *pa;

	/*
	 * We scan the hash list read-only. Addition to and removal from
	 * the hash-list needs a held write-lock.
	 */
	acquire(&mapping->page_lock);
	pa = radix_tree_lookup(&mapping->page_tree, offset);
	if (pa)
		// get_page(&pages[PAGE2NUM(*pa)]);
	release(&mapping->page_lock);
	return pa;
}

/**
 * @brief read file into memory when page fault hapened
 * 
 * @return int 
 */
int filemap_nopage(uint64 address){
  struct mm_struct *mm = myproc()->mm;
  struct vm_area_struct *area = mm->mmap;
  uint64 pgoff, endoff, size;
  page_t *page;
  uint64 *pa;

  //find area that contains the address
  while(area){
    if(area->vm_start <= address && area->vm_end > address)
      break;
    area = area->vm_next;
  }
  struct file *file = area->vm_file;
  struct address_space *mapping = file->ep->i_mapping;

  pgoff = ((address - area->vm_start) >> PAGE_CACHE_SHIFT) + area->vm_pgoff;
	endoff = ((area->vm_end - area->vm_start) >> PAGE_CACHE_SHIFT) + area->vm_pgoff;
  size = (file->ep->raw.size + PAGE_CACHE_SIZE - 1) >> PAGE_CACHE_SHIFT;

  // the page need to read greater than the total page
  if(pgoff >= size)
    panic("mmap fetch 1");
  return 0;

  /*
	 * The "size" of the file, as far as mmap is concerned, isn't bigger
	 * than the mapping
	 */
	if (size > endoff)
		size = endoff;

  // find frome page cache first
  // page = find_get_page(mapping, pgoff);
  pa = find_get_page(mapping, pgoff);
  if(pa){
    mappages(myproc()->pagetable, PGROUNDDOWN(address), PGSIZE, pa, PTE_U|PTE_V|PTE_W|PTE_R);
    return 0;
  }

  pa = (uint64 *)kalloc();
  
  

}