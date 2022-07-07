// vm.h is defined as _H_MM_
#ifndef _H_MM2_
#define _H_MM2_

#include "fs/fs.h"
/* 这行加上之后，两个头文件相互引用，导致出错 */
// #include "kernel/proc.h"

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

struct mm_struct {
  struct vm_area_struct *mmap;//list of vmas
  struct rb_root mm_rb;
  struct vm_area_struct *mmap_cache;
  int map_count;              //number of vmas
  spinlock_t mm_lock;        // 保护mmap链表和mm_rb树,页表
  uint64_t free_area_cache; /* 用来查找地址空间中的未映射区域 */
  uint32_t total_vm; /* 地址空间包含的总页数 */
  uint32_t rss;     /*分配的物理页框数*/
  uint64_t start_brk; // initial address of the heap
  uint64_t brk; // current final address of the heap
  uint64_t start_stack; /* initial adress of user mode stack */
  // spinlock_t page_table_lock; /* memory regions' and page tables' spin lock */
};

typedef struct mm_struct mm_struct_t;

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
unsigned long do_brk(unsigned long addr, unsigned long len);
struct vm_area_struct * find_vma_intersection(struct mm_struct * mm, unsigned long start_addr, unsigned long end_addr);
struct vm_area_struct * find_vma(struct mm_struct * mm, unsigned long addr);

/* memory.c */
int handle_mm_fault(struct mm_struct *mm, struct vm_area_struct *vma,
		unsigned long address, unsigned int flags);

void unmap_vmas(vm_area_struct_t *vma, uint64_t start_addr, uint64_t end_addr);
#endif