#include "common.h"
#include "platform.h"
#include "mm/alloc.h"
#include "fs/fs.h"
#include "fs/file.h"
#include "mm/vm.h"
#include "kernel/proc.h"
#include "defs.h"
#include "debug.h"
#include "mm/page.h"
#include "radix-tree.h"
#include "mm/mm.h"

/**
 * @brief 定义了关于file map相关的函数，函数声明在mm.h
 * 
 */


/*
 * a rather lightweight function, finding and getting a reference to a
 * hashed page atomically.
 */
uint64 find_get_page(struct address_space *mapping, unsigned long offset)
{
	// page_t *page;
  uint64 pa;

	/*
	 * We scan the hash list read-only. Addition to and removal from
	 * the hash-list needs a held write-lock.
	 */
	acquire(&mapping->page_lock);
	pa = (uint64)radix_tree_lookup(&mapping->page_tree, offset);
  /* increase the ref counts of the page that pa belongs to*/
	if (pa)
		get_page(&pages[PAGE2NUM(pa)]);
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
  // page_t *page;
  uint64 pa;

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
  // 页缓存命中，把address和pa映射
  if(pa){
    if(mappages(myproc()->pagetable, PGROUNDDOWN(address), PGSIZE, pa, PTE_U|PTE_V|PTE_W|PTE_R) < 0)
      panic("filemap no page 2");

    return 0;
  }

  // 没有命中，分配页，读磁盘
  pa = (uint64 )kalloc(); 
  if(mappages(myproc()->pagetable, PGROUNDDOWN(address), PGSIZE, pa, PTE_U|PTE_V|PTE_W|PTE_R) < 0)
    panic("filemap no page 3");
  
  //如果文件的最后一个页的内容不满一页，reade返回值大于0。
  if(reade(area->vm_file->ep, 1, PGROUNDDOWN(address), pgoff*PGSIZE, PGSIZE) < 0)
    panic("filemap no page 4");

  /* add page to page cache*/
  add_to_page_cache(pa, mapping, pgoff);
  return 0;
}

void
add_to_page_cache(uint64 pa, struct address_space *mapping, pgoff_t offset)
{
  acquire(&mapping->page_lock);
  radix_tree_insert(&mapping->page_tree, offset, (void *)pa);
  release(&mapping->page_lock);
}