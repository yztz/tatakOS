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
// #include "mm/mm.h"

#include "fs/mpage.h"
#include "swap.h"
#include "config.h"
/**
 * 定义了关于file map相关的函数，函数声明在fs.h
 *
 */

void __remove_from_page_cache(page_t *page)
{
	struct address_space *mapping = page->mapping;

	radix_tree_delete(&mapping->page_tree, page->index);
	page->mapping = NULL;
	mapping->nrpages--;
  /* pagecache页的总数-1*/
	// pagecache_acct(-1);
}

/*
 * Remove a page from the page cache and free it. Caller has to make
 * sure the page is locked and that nobody else uses it - or that usage
 * is safe.  The caller must hold a write_lock on the mapping's tree_lock.
 */
void remove_from_page_cache(page_t *page)
{
	struct address_space *mapping = page->mapping;

  /* 一个页添加到多个pagecache中，在remove_from_page_cache中put */
  // put_page(page);

	if (unlikely(!PageLocked(page)))
    ER();
	spin_lock(&mapping->tree_lock);
	__remove_from_page_cache(page);
	spin_unlock(&mapping->tree_lock);
}

page_t *find_page(struct address_space *mapping, unsigned long offset){
  // uint64 pa = 0;
  page_t *page;

  acquire(&mapping->tree_lock);
  // pa = (uint64)radix_tree_lookup(&mapping->page_tree, offset);
  page = (page_t *)radix_tree_lookup(&mapping->page_tree, offset);
  release(&mapping->tree_lock);
  return page;
}
/*
 * a rather lightweight function, finding and getting a reference to a
 * hashed page atomically.
 */
page_t *find_get_page(struct address_space *mapping, unsigned long offset)
{
  // uint64 pa = 0;
  page_t *page;

  page = find_page(mapping, offset);
  // printf(rd("pa: %p\n"), pa);
  /* increase the ref counts of the page that pa belongs to*/
  if (page)
    get_page(page);
  return page;
}

page_t *find_get_lock_page(struct address_space *mapping, unsigned long offset){
  // uint64_t pa = 0;
  page_t *page;
  page = find_get_page(mapping, offset);
  if(page)
    lock_page(page);
  return page;
}

// /**
//  * @brief read file into memory when page fault hapened
//  * mmap file 的pagefault处理函数。
//  */
// int filemap_nopage(uint64_t address)
// {
//   struct proc *p = myproc();
//   struct mm_struct *mm = p->mm;
//   struct vm_area_struct *area = mm->mmap;
//   uint64 pgoff, endoff, size;
//   // page_t *page;
//   uint64 pa;

//   // find area that contains the address
//   while (area)
//   {
//     if (area->vm_start <= address && area->vm_end > address)
//       break;
//     area = area->vm_next;
//   }
//   struct file *file = area->vm_file;
//   struct address_space *mapping = file->ep->i_mapping;

//   pgoff = ((address - area->vm_start) >> PAGE_CACHE_SHIFT) + area->vm_pgoff;
//   endoff = ((area->vm_end - area->vm_start) >> PAGE_CACHE_SHIFT) + area->vm_pgoff;
//   size = (file->ep->raw.size + PAGE_CACHE_SIZE - 1) >> PAGE_CACHE_SHIFT;

//   // the page need to read greater than the total page
//   if (pgoff >= size)
//     panic("mmap fetch 1");

//   /*
//    * The "size" of the file, as far as mmap is concerned, isn't bigger
//    * than the mapping
//    */
//   if (size > endoff)
//     size = endoff;

//   /* find from page cache first, if find page, increase it's ref count */
//   pa = find_get_page(mapping, pgoff);
//   // printf(ylw("pa: %p\n"), pa);
//   /* 页缓存命中，把address和pa映射 */
//   if (pa)
//   {
//     mark_page_accessed(&pages[PAGE2NUM(pa)]);
//     pagetable_t pagetable = myproc()->mm->pagetable;
//     if (mappages(pagetable, PGROUNDDOWN(address), PGSIZE, pa, PTE_U | PTE_V | PTE_W | PTE_R) < 0)
//       panic("filemap no page 2");

//     return 0;
//   }
//   // for(;;);

//   // printf(rd("not hit\n"));
//   // 没有命中，分配页，读磁盘
//   pa = (uint64)kalloc();
//   // printf(bl("pa: %p\n"), pa);
//   if (mappages(myproc()->mm->pagetable, PGROUNDDOWN(address), PGSIZE, pa, PTE_U | PTE_V | PTE_W | PTE_R) < 0)
//     panic("filemap no page 3");

//   mark_page_accessed(&pages[PAGE2NUM(pa)]);

//   //如果文件的最后一个页的内容不满一页，reade返回值大于0。
//   entry_t *entry = area->vm_file->ep;
//   if (fat_read(entry->fat, entry->clus_start, 1, PGROUNDDOWN(address), pgoff * PGSIZE, PGSIZE) < 0)
//     panic("filemap no page 4");

//   // if (do_generic_mapping_read(entry->i_mapping, 1, PGROUNDDOWN(address), pgoff * PGSIZE, PGSIZE) < 0)
//     // panic("filemap no page 4");
//   /* add page to page cache*/
//   // printf(ylw("aaa: %p"), mapping->page_tree.rnode);
//   add_to_page_cache(pa, mapping, pgoff);
//   // printf_radix_tree(&mapping->page_tree);

//   return 0;
// }

void add_to_page_cache(page_t *page, struct address_space *mapping, pgoff_t offset)
{
  page->mapping = mapping;
  page->index = offset;

  /* 一个页添加到多个pagecache中，在remove_from_page_cache中put */
  // get_page(page);

  acquire(&mapping->tree_lock);
  radix_tree_insert(&mapping->page_tree, offset, (void *)page);
  release(&mapping->tree_lock);
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
char tab[][10] = {
  "",
  "  ",
  "   ",
};

extern void print_buddy();

void walk_free_rdt(struct radix_tree_node *node, uint8 height, uint8 c_h)
{
  /* 下面的i小于表达式右边不用-1，这里之前少释放了最后一个页 */
  for (int i = 0; i < (1 << RADIX_TREE_MAP_SHIFT); i++)
  {
    if (node->slots[i] != NULL)
    {
      /* the leaf node, 释放叶节点记录的物理地址的页 */
      if (c_h == height)
      {
        page_t *page = (page_t *)(node->slots[i]);
        // /* 是释放一整个物理页吗？ */
        // printf(bl("walk free pa: %p\n"), pa);
        // kfree(pa);
        put_page(page);
        // printf("pa: %p\n", pa);
        // print_buddy(); 
      }
      else
      {
        /* 递归释放下一级节点的内容 */
        walk_free_rdt((struct radix_tree_node *)node->slots[i], height, c_h + 1);
      }
    }
  }
  // printf("free node %x\n", node);
  kfree((void *)node);
  // printf("node freed\n");
}

/**
 * @brief 注意，只需释放rdt节点，不释放已经映射的物理内存，
 * 物理内存不在文件关闭时释放，而在unmap或者进程退出时释放，这是因为mmap系统调用说明了，文件
 * 关闭，映射仍然存在。 
 * 
 * 补充：以上似乎不对，一个页加入某个文件的page cache，似乎引用数加1，这个文件关闭时，kfree
 * 减去这个引用数，引用数为0则释放。或者区分一下page cache中哪些页是mmap的？然后分别处理。
 * 
 * 以上论述都是错的，mmap会增加文件引用数，到了这一步说明文件引用数为0，释放。
 * 
 */
void free_mapping(entry_t *entry)
{
  struct radix_tree_root *root = &(entry->i_mapping->page_tree);
  void *addr;

/**
 * 当高度为0且node != NULL，表示里面存储的是一个页指针，否则为一个rdt node指针。
 */
  if (root->height > 0)
  {
    /* 高度超过限制 */
    if(root->height > RADIX_TREE_MAX_PATH)
      ER();
    walk_free_rdt(root->rnode, root->height, 1);
  } 
  else if(root->height == 0 && root->rnode){
    put_page((page_t *)root->rnode);
  }
  /* free i_mapping */
  addr = entry->i_mapping;
  kfree(addr);
  entry->i_mapping = NULL;
}

/**
 * 从entry的index页开始，读取pg_cnt个页，加入到pagecache和lru
 */
void readahead(entry_t *entry, uint64_t index, int pg_cnt){
  int i;
  rw_page_list_t *pg_list = kzalloc(sizeof(rw_page_list_t));
  // todo("allocate more than one page has bug!");
  // uint64_t pa = (uint64_t)kzalloc(pg_cnt * PGSIZE);
  // uint64_t cur_pa = pa;
  uint64_t cur_index = index;


  for(i = 0; i < pg_cnt; i++){
    if(find_page(entry->i_mapping, cur_index) == NULL){
      rw_page_t *read_page= kzalloc(sizeof(rw_page_t));
      uint64_t cur_pa = (uint64_t)kalloc();
      page_t *page = PATOPAGE(cur_pa);
      add_to_page_cache(page, entry->i_mapping, cur_index);
      lru_cache_add(page);  
      read_page->pa = cur_pa;
      // cur_pa += PGSIZE;
      read_page->pg_id = cur_index;
      cur_index++;
      read_page->next  = NULL;

      /* the first page */
      if(pg_list->head == NULL){
        pg_list->head = read_page;
        pg_list->tail = read_page;
      }
      else{
        pg_list->tail->next = read_page;
        pg_list->tail = read_page;
      }
    }
  }

  read_pages(entry, pg_list);

  // cur_pa = pa, cur_index = index;

  // for(i = 0; i < pg_cnt; i++){
  //   page_t *page = PATOPAGE(cur_pa);
  //   add_to_page_cache(page, entry->i_mapping, cur_index);
  //   lru_cache_add(page);
  //   cur_pa += PGSIZE;
  //   cur_index++;
  // }
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
  page_t *page;
  entry_t *entry = mapping->host;

  rest = n;
  cur_off = off;
  /* the file size */
  // file_size = E_FILESIZE(mapping->host);
  file_size = mapping->host->size_in_mem;
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
       * 当偏移到了最后一页，偏移大于等于此条件，说明读到了文件末尾。
       */
      if (pgoff >= (file_size & ~PGMASK))
        break;
    }

retry:
    page = find_get_page(mapping, index);

    /* the content is not cached in page cache, read from disk and cache it */
    if (!page)
    {
      /* 剩余的页数 */
      int remain = ROUND_COUNT(rest);

      if(remain == 1) {
        pa = (uint64_t)kalloc();
        page = PATOPAGE(pa);

        get_page(page);
        read_one_page(entry, pa, index);

        /* 添加到page cache */
        add_to_page_cache(page, mapping, index);
        /* 添加到 inactive list(因为是先缓存到cache里的，素以执行mark_page_accessed的时候可能还不在inactive list上) */
        lru_cache_add(page);
      }
      else{
        /* 发挥连续读多块的优势，减小I/O次数 */
        readahead(entry, index, remain);
        goto retry;
      }
    }
    else{
      pa = PAGETOPA(page);
    }

    mark_page_accessed(page);
    /* 当前页内读取字节数，取总剩余字节数和当前页可读字节数的最小值 */
    int len = min(rest, PGSIZE - pgoff);
    /* 文件最后一页 */
    len = min(len, file_size - pgoff);

    // printf(ylw("buff: %p pa: %p\n"), buff, pa);
    either_copyout(user, buff, (void *)(pa + pgoff), len);

    put_page(page);

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
  page_t *page;
  entry_t *entry = mapping->host;
  int len;
  uint64_t pgnums_in_disk;

  pgnums_in_disk = ROUNDUP(entry->raw.size, PAGE_CACHE_SIZE) >> PAGE_CACHE_SHIFT;

  cur_off = off;
  rest = n;

  while(rest > 0){
    pg_id = cur_off >> PGSHIFT;
    pg_off = cur_off & ~PGMASK;


    page = find_get_lock_page(mapping, pg_id);
    if(!page){
      pa = (uint64_t)kalloc();
      page = PATOPAGE(pa);

      get_lock_page(page);
      /* 整个页都要重新写过的，就没必要从磁盘中读了；或者要写的文件偏移大于文件在磁盘上的大小，也没必要读磁盘  */
      if(!(pg_off == 0 && rest >= PGSIZE))
        if(pg_id < pgnums_in_disk)
          read_one_page(entry, pa, pg_id);
      add_to_page_cache(page, mapping, pg_id);
      lru_cache_add(page);
    }
    else{
      pa = PAGETOPA(page);
    }

    mark_page_accessed(page);

    // SetPageDirect(page);

    len = min(rest, PGSIZE - pg_off);
    either_copyin((void* )(pa + pg_off), user, buff, len);

    // ERROR("to handle the only index 0 set tag and clear tag");
    set_pg_rdt_dirty(page, &mapping->page_tree, pg_id, PAGECACHE_TAG_DIRTY);

    unlock_put_page(page);

    rest -= len;
    cur_off += len; 
    buff += len;
  }

  return n - rest;
}

/**
 * @brief read file into memory when page fault hapened
 * mmap file 的pagefault处理函数。
 * 类似于do_generic_mapping_read
 */
int filemap_nopage(pte_t *pte, vma_t *area, uint64_t address){
  uint64 pgoff, endoff, size;
  page_t *page;
  uint64 pa;

  struct file *file = area->map_file;
  address_space_t *mapping = file->ep->i_mapping;

  /* address落在文件的pgoff页 */
  pgoff = ((address - area->addr) >> PAGE_CACHE_SHIFT) + area->offset;
  /* area所包含的最后一页 */
  endoff = (area->len >> PAGE_CACHE_SHIFT) + area->offset;
  /* 文件的总页数， 页号为（0 ~ size-1）*/
  size = ROUNDUP(file->ep->size_in_mem, PAGE_CACHE_SIZE) >> PAGE_CACHE_SHIFT;

  if(pgoff >= size || pgoff > endoff)
    ER();

  page = find_get_page(mapping, pgoff);

  if(!page) {
    pa = (uint64_t)kalloc();
    page = PATOPAGE(pa);

    get_page(page);
    entry_t *entry = mapping->host;
    read_one_page(entry, pa, pgoff);
    add_to_page_cache(page, mapping, pgoff);
    lru_cache_add(page);
  }
  else {
    pa = PAGETOPA(page);
  }
  // mark_page_accessed(page);
  put_page(page);

  /**
   * private mmap和shared mmap
   */
  if(area->flags & MAP_PRIVATE){
    uint64_t pa0 = (uint64_t)kalloc();
#ifdef SWAP
    /* 页替换算法需要用到swap */
    page_t *page0 = PAGETOPA(pa0);
    lru_cache_add(page0);
    mark_page_accessed(page0);
#endif
    memcpy((void *)pa0, (void *)pa, PGSIZE);
    *pte = PA2PTE(pa0) | riscv_map_prot(area->prot) | PTE_V;
    sfence_vma_addr(address);
  }
  else if(area->flags & MAP_SHARED){
    /* shared */
    /* 这里小心被页回收算法回收掉！要建立rmap，如果不支持rmap需要从lru上取下来，防止被回收 */
    *pte = PA2PTE(pa) | riscv_map_prot(area->prot) | PTE_V;
    sfence_vma_addr(address);
    /* 没有rmap，从lru链表上删除，不参与页回收 */
#ifndef RMAP
    del_page_from_lru(&memory_zone, page);
#endif
  }

  return 0; 
}

void init_pg_head(rw_page_list_t *pg_list){
  pg_list->head = NULL;
  pg_list->tail = NULL;
  pg_list->nr_pages = 0;
}

/**
 * @brief find all pages in the mapping with tag
 */
rw_page_list_t *
find_pages_tag(address_space_t *mapping, uint32_t tag){
  rw_page_list_t *pg_list = kzalloc(sizeof(rw_page_list_t));
  
  init_pg_head(pg_list);
  radix_tree_find_tags(&mapping->page_tree, tag, pg_list);

  /* if no taged page, pg_list and pg_tail is null */
  if(pg_list->head != NULL)
    pg_list->tail->next = NULL;
  else{
    /* 没有标签页，free pg_list */ 
    kfree(pg_list);
    pg_list = NULL;
  }
  return pg_list;
}


/* 被writeback_single_entry函数所取代了 */
/**
 * @brief 把内存中的文件映像写回磁盘。对于fat32来说，包括写fat表（给文件分配适当的簇），写文件的页，改写
 * 父目录中的文件元数据。
 * 
 */
// void writeback_file_to_disk(entry_t *entry){
//   /* 如果文件在内存中的大小和磁盘上的不一样（变大或变小）*/
//   // if(entry->size_in_mem != entry->raw.size){
//   if(entry->dirty){

//     // /* bigger than disk */
//     // if(entry->size_in_mem > entry->raw.size){
//     //   fat_alloc_append_clusters(entry->fat, entry->clus_start, &entry->clus_end, &entry->clus_cnt, entry->size_in_mem);
//     // }
//     // /* smaller than disk */
//     // else {
//     //   todo("consider delete part of file!");
//     // }

//     // entry->raw.size = entry->size_in_mem;
//     // fat_update(entry->fat, entry->parent->clus_start, entry->clus_offset, &entry->raw);
//     sych_entry_size_in_disk(entry);
//     mpage_writepages(entry->i_mapping);
//   }
//     /* wirte back dirty pages to disk, 即使大小没变，但是内容可能也变了，所以要写回 */
//     // if(entry->dirty)
//       // mpage_writepages(entry->i_mapping);

// }
