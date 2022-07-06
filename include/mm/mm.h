

// vm.h is defined as _H_MM_
#ifndef _H_MM2_
#define _H_MM2_

#include "fs/fs.h"
#define PAGECACHE_TAG_DIRTY 0
#define PAGECACHE_TAG_WRITEBACK 1

enum vma_type { NONE, LOAD, TEXT, DATA, BSS, HEAP, MMAP_FILE, STACK, MMAP_ANON};

struct vm_area_struct
{
  struct mm_struct *vm_mm;
  uint64 vm_start;
  uint64 vm_end;

  /* 按照树中 中序遍历 的顺序链起来？ */
  struct vm_area_struct *vm_next;//linked list of vm areas per task
  
  /* 感觉下面两个标志功能有点重复 */
  // int vm_page_prot;
  int vm_flags;

  struct rb_node vm_rb;

  /* 对于文件映射区间，为映射的文件的偏移量（以页为单位）；对于匿名区间，为0或vm_start/PAGE_SIZE*/
  uint64 vm_pgoff;
  struct file *vm_file;
  enum vma_type type;
};

typedef struct vm_area_struct vm_area_struct_t;

/*
 * vm_flags..
 */
#define VM_READ		0x00000001	/* currently active flags */
#define VM_WRITE	0x00000002
#define VM_EXEC		0x00000004
#define VM_SHARED	0x00000008

#define VM_GROWSDOWN	0x00000100	/* general info on the segment 向下增长，栈区域使用 */
#define VM_GROWSUP	0x00000200
#define VM_SHM		0x00000400	/* shared memory area, don't swap out */
#define VM_DENYWRITE	0x00000800	/* ETXTBSY on write attempts.. */

#define VM_EXECUTABLE	0x00001000
#define VM_LOCKED	0x00002000
#define VM_IO           0x00004000	/* Memory mapped I/O or similar */

#define VM_DATA_DEFAULT_FLAGS		(VM_READ | VM_WRITE | VM_EXEC)

/* functions defined in mmap.c */
uint64 do_mmap(struct file *file, unsigned long addr, unsigned long len, int prot, int flag, unsigned long offset, int type);
unsigned long do_mmap_pgoff(struct file * file, unsigned long addr, unsigned long len, int prot, int flags, unsigned long pgoff, int type);
void exit_mmap(mm_struct_t *mm);
uint64_t do_munmap(mm_struct_t *mm, uint64_t start, uint64_t len);

/* functions defined in filemap.c */
uint64 find_get_page(struct address_space *mapping, unsigned long offset);
int filemap_nopage(uint64 address);
void add_to_page_cache(uint64 pa, struct address_space *mapping, pgoff_t offset);
void free_mapping(entry_t *entry);
int do_generic_mapping_read(struct address_space *mapping, int user, uint64_t buff, int off, int n);
uint64_t do_generic_mapping_write(struct address_space *mapping, int user, uint64_t buff, int off, int n);
pages_be_found_head_t * find_pages_tag(address_space_t *mapping, uint32_t tag);
void writeback_file_to_disk(entry_t *entry);



/* functions defined in write-pageback.c */
void set_pg_rdt_dirty(uint64_t pa, radix_tree_root_t *root, uint64_t pg_id, uint tag_type);


/* memory.c */
int handle_mm_fault(struct mm_struct *mm, struct vm_area_struct *vma,
		unsigned long address, unsigned int flags);
#endif