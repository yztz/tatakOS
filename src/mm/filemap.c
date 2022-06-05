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

#include "fs/mpage.h"
/**
 * @brief 定义了关于file map相关的函数，函数声明在mm.h
 *
 */


uint64 find_page(struct address_space *mapping, unsigned long offset){
  uint64 pa = 0;

  acquire(&mapping->page_lock);
  pa = (uint64)radix_tree_lookup(&mapping->page_tree, offset);
  release(&mapping->page_lock);
  return pa;
}
/*
 * a rather lightweight function, finding and getting a reference to a
 * hashed page atomically.
 */
uint64 find_get_page(struct address_space *mapping, unsigned long offset)
{
  uint64 pa = 0;

  pa = find_page(mapping, offset);
  // printf(rd("pa: %p\n"), pa);
  /* increase the ref counts of the page that pa belongs to*/
  if (pa)
    get_page(pa);
  return pa;
}

/**
 * @brief read file into memory when page fault hapened
 *
 * @return int
 */
int filemap_nopage(uint64 address)
{
  struct proc *p = myproc();
  struct mm_struct *mm = p->mm;
  struct vm_area_struct *area = mm->mmap;
  uint64 pgoff, endoff, size;
  // page_t *page;
  uint64 pa;

  // find area that contains the address
  while (area)
  {
    if (area->vm_start <= address && area->vm_end > address)
      break;
    area = area->vm_next;
  }
  struct file *file = area->vm_file;
  struct address_space *mapping = file->ep->i_mapping;

  pgoff = ((address - area->vm_start) >> PAGE_CACHE_SHIFT) + area->vm_pgoff;
  endoff = ((area->vm_end - area->vm_start) >> PAGE_CACHE_SHIFT) + area->vm_pgoff;
  size = (file->ep->raw.size + PAGE_CACHE_SIZE - 1) >> PAGE_CACHE_SHIFT;

  // the page need to read greater than the total page
  if (pgoff >= size)
    panic("mmap fetch 1");

  /*
   * The "size" of the file, as far as mmap is concerned, isn't bigger
   * than the mapping
   */
  if (size > endoff)
    size = endoff;

  /* find from page cache first, if find page, increase it's ref count */
  pa = find_get_page(mapping, pgoff);
  // printf(ylw("pa: %p\n"), pa);
  /* 页缓存命中，把address和pa映射 */
  if (pa)
  {
    pagetable_t pagetable = myproc()->pagetable;
    if (mappages(pagetable, PGROUNDDOWN(address), PGSIZE, pa, PTE_U | PTE_V | PTE_W | PTE_R) < 0)
      panic("filemap no page 2");

    return 0;
  }
  // for(;;);

  // printf(rd("not hit\n"));
  // 没有命中，分配页，读磁盘
  pa = (uint64)kalloc();
  // printf(bl("pa: %p\n"), pa);
  if (mappages(myproc()->pagetable, PGROUNDDOWN(address), PGSIZE, pa, PTE_U | PTE_V | PTE_W | PTE_R) < 0)
    panic("filemap no page 3");

  //如果文件的最后一个页的内容不满一页，reade返回值大于0。
  entry_t *entry = area->vm_file->ep;
  if (fat_read(entry->fat, entry->clus_start, 1, PGROUNDDOWN(address), pgoff * PGSIZE, PGSIZE) < 0)
    panic("filemap no page 4");

  // if (do_generic_mapping_read(entry->i_mapping, 1, PGROUNDDOWN(address), pgoff * PGSIZE, PGSIZE) < 0)
    // panic("filemap no page 4");
  /* add page to page cache*/
  // printf(ylw("aaa: %p"), mapping->page_tree.rnode);
  add_to_page_cache(pa, mapping, pgoff);
  // printf_radix_tree(&mapping->page_tree);

  return 0;
}

void add_to_page_cache(uint64 pa, struct address_space *mapping, pgoff_t offset)
{
  acquire(&mapping->page_lock);
  radix_tree_insert(&mapping->page_tree, offset, (void *)pa);
  release(&mapping->page_lock);
}

/**
 * @brief 释放一颗rdt，包括释放其叶节点对应的物理页，注意，传入的node节点需要在上一级释放（现改为在本级最后释放）。
 * 问题：
 * 释放物理页时，是先减去其引用数，如果为0则free，还是直接free？目前在find_get_page
 * 先不增加页引用，直接free。
 *
 * @param node
 * @param height
 * @param c_h
 */
void walk_free_rdt(struct radix_tree_node *node, uint8 height, uint8 c_h)
{
  for (int i = 0; i < (1 << RADIX_TREE_MAP_SHIFT) - 1; i++)
  {
    if (node->slots[i] != NULL)
    {
      /* the leaf node, 释放叶节点记录的物理地址的页 */
      if (c_h == height)
      {
        // void *pa = (node->slots[i]);
        // /* 是释放一整个物理页吗？ */
        // printf(bl("walk free pa: %p\n"), pa);
        // kfree(pa);
        #ifdef TODO
        todo("is the page mapping, not kfree, else ,free the page");
        #endif
        break;
      }
      else
      {
        /* 递归释放下一级节点的内容 */
        walk_free_rdt((struct radix_tree_node *)node->slots[i], height, c_h + 1);
        // void *addr = node->slots[i];
        // kfree(addr);
      }
    }
  }
  kfree((void *)node);
}

/**
 * @brief 注意，只需释放rdt节点，不释放已经映射的物理内存，
 * 物理内存不在文件关闭时释放，而在unmap或者进程退出时释放，这是因为mmap系统调用说明了，文件
 * 关闭，映射仍然存在。 
 * 
 * 补充：以上似乎不对，一个页加入某个文件的page cache，似乎引用数加1，这个文件关闭时，kfree
 * 减去这个引用数，引用数为0则释放。或者区分一下page cache中哪些页是mmap的？然后分别处理。
 * 
 * 现在文件关闭时，还要检测脏页，并写回。
 * 
 * @param entry 
 */
void free_mapping(entry_t *entry)
{

  #ifdef TODO
  todo("(before free invoke )invoke write pages method to write dirty pages");
  #endif
  struct radix_tree_root *root = &(entry->i_mapping->page_tree);
  void *addr;
  if (root->height > 0)
  {
    walk_free_rdt(root->rnode, root->height, 1);
    /* free rnode */
    // addr = root->rnode;
    // kfree(addr);
  }
  /* freee i_mapping */
  addr = entry->i_mapping;
  kfree(addr);
  entry->i_mapping = NULL;
}

/**
 * @brief 正常的读文件一次读一个page（8 sectors）, 可以考虑使用"read ahead".
 * 
 * @param mapping 
 * @param user 
 * @param buff 
 * @param off 
 * @param n 
 * @return int 
 */
int do_generic_mapping_read(struct address_space *mapping, int user, uint64_t buff, int off, int n)
{
      // if(user == 1)
        // for(;;);
  uint index,  end_index;
  uint64 pa, pgoff;
  int rest, cur_off;
  uint32_t file_size;

  rest = n;
  cur_off = off;
  /* the file size */
  file_size = E_FILESIZE(mapping->host);
  /* the last page index of a file */
  end_index = file_size >> PGSHIFT;

  while (rest > 0)
  {
    /* page number */
    index = cur_off >> PGSHIFT;
    /* offset in a page */
    pgoff = cur_off & (~PGMASK);

    /* out of the size of file */
    if (index > end_index)
      break;
    if (index == end_index)
    {
      /**
       * if (pgoff >= (file_size & ~PGMASK))
       * 这里是大于等于，不是大于，文件大小为file_size的字节号为0 ~ file_size-1,
       *  试想两种情况：
       * 1.文件大小file size为4096， pgoff为4095.以上不正确。
       * 2.大小为4095，pgoff为4094.以上正确。
       */
      // if (pgoff > ((file_size-1) & ~PGMASK))
      if (pgoff >= (file_size & ~PGMASK))
        break;
    }

    pa = find_get_page(mapping, index);

    /* the content is not cached in page cache, read from disk and cache it */
    if (!pa)
    {
      // if(user == 1)
        // for(;;);
      pa = (uint64)kalloc();
      // printf(ylw("pa: %p\n"), pa);
      /* 这里不能像之前filemap_nopage一样，再返回去调用reade */
      entry_t *entry = mapping->host;
      /* 我这里user是用户地址，但是pa是物理(内核)地址, 不能直接填user， 要填0*/
      // fat_read(entry->fat, entry->clus_start, 0, pa, index*PGSIZE, PGSIZE);
      readpage(entry, pa, index);

      // print_page_contents((uint64 *)pa);
      // for(;;);
      // printf(ylw("pa: %p\n"), pa);

      add_to_page_cache(pa, mapping, index);
    }

    /* 当前页内读取字节数，取总剩余字节数和当前页可读字节数的最小值 */
    int len = min(rest, PGSIZE - pgoff);

    // printf(ylw("buff: %p pa: %p\n"), buff, pa);
    either_copyout(user, buff, (void *)(pa + pgoff), len);


    #ifdef TODO
    todo("put page?");
    #endif

    cur_off += len;
    buff += len;
    rest -= len;
  }
  /* 返回读取的字节数 */
  return n - rest;
}

/* this function is models on generic_file_aio_write_nolock in linux 2.6.0*/
uint64_t do_generic_mapping_write(struct address_space *mapping, int user, uint64_t buff, int off, int n){
  uint32_t pg_id, pg_off, rest, cur_off;
  uint64_t pa;
  entry_t *entry = mapping->host;
  int len;

  cur_off = off;
  rest = n;


  #ifdef TODO
  todo("if off biger the file size, expend tree");
  #endif
  while(rest > 0){
    pg_id = cur_off >> PGSHIFT;
    pg_off = cur_off & ~PGMASK;


    pa = find_get_page(mapping, pg_id);
    if(!pa){
      pa = (uint64_t)kalloc();
      get_page(pa);
      #ifdef TODO
      todo("use prepare_write");
      #endif
      readpage(entry, pa, pg_id);
      add_to_page_cache(pa, mapping, pg_id);
    }

    /* 函数位置有待商榷 */
    lock_page(pa);

    len = min(rest, PGSIZE - pg_off);
    either_copyin((void* )(pa + pg_off), 1, buff, len);

    set_pg_rdt_dirty(pa, &mapping->page_tree, pg_id, PAGECACHE_TAG_DIRTY);
    // memmove((void* )(pa + pg_off), (void *)buff, len);
    // for(;;);
    // todo("set page dirty");
    unlock_put_page(pa);

    rest -= len;
    cur_off += len; 
    buff += len;
  }

  return n - rest;
}



/**
 * @brief find all pages in the mapping with tag
 * 
 * @param mapping 
 * @param tag 
 * @return pages_be_found_head_t* 
 */
pages_be_found_head_t *
find_pages_tag(address_space_t *mapping, uint32_t tag){
  pages_be_found_head_t *pg_head = kzalloc(sizeof(pages_be_found_head_t));
  
  pg_head->head = NULL;
  pg_head->tail = NULL;
  pg_head->nr_pages = 0;

  radix_tree_find_tags(&mapping->page_tree, tag, pg_head);

  pg_head->tail->next = NULL;
  return pg_head;
}