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
  spinlock_t page_table_lock; /* page tables' spin lock */

  pagetable_t pagetable;       // User page table
  uint64 kstack;               // Virtual address of kernel stack
  struct trapframe *trapframe; // data page for trampoline.S

  struct list_head mmlist;
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



/*
 * cloning flags:
 */
#define CSIGNAL		0x000000ff	/* signal mask to be sent at exit */
/* 使用这个标志后，再do_clone中copy_mm时，子进程使用父进程的mm_struct，共用 */
#define CLONE_VM	0x00000100	/* set if VM shared between processes */
#define CLONE_FS	0x00000200	/* set if fs info shared between processes */
#define CLONE_FILES	0x00000400	/* set if open files shared between processes */
#define CLONE_SIGHAND	0x00000800	/* set if signal handlers and blocked signals shared */
#define CLONE_PTRACE	0x00002000	/* set if we want to let tracing continue on the child too */
#define CLONE_VFORK	0x00004000	/* set if the parent wants the child to wake it up on mm_release */
#define CLONE_PARENT	0x00008000	/* set if we want to have the same parent as the cloner */
#define CLONE_THREAD	0x00010000	/* Same thread group? */
#define CLONE_NEWNS	0x00020000	/* New namespace group? */
#define CLONE_SYSVSEM	0x00040000	/* share system V SEM_UNDO semantics */
#define CLONE_SETTLS	0x00080000	/* create a new TLS for the child */
#define CLONE_PARENT_SETTID	0x00100000	/* set the TID in the parent */
#define CLONE_CHILD_CLEARTID	0x00200000	/* clear the TID in the child */
#define CLONE_DETACHED		0x00400000	/* Unused, ignored */
#define CLONE_UNTRACED		0x00800000	/* set if the tracing process can't force CLONE_PTRACE on this clone */
#define CLONE_CHILD_SETTID	0x01000000	/* set the TID in the child */
#define CLONE_STOPPED		0x02000000	/* Start in stopped state */


/* functions defined in mmap.c */
uint64 do_mmap(struct file *file, unsigned long addr, unsigned long len, int prot, int flag, unsigned long offset, int type);
unsigned long do_mmap_pgoff(struct file * file, unsigned long addr, unsigned long len, int prot, int flags, unsigned long pgoff, int type);
void exit_mmap(mm_struct_t *mm);
uint64_t do_munmap(mm_struct_t *mm, uint64_t start, uint64_t len);
unsigned long do_brk(unsigned long addr, unsigned long len);
struct vm_area_struct * find_vma_intersection(struct mm_struct * mm, unsigned long start_addr, unsigned long end_addr);
struct vm_area_struct * find_vma(struct mm_struct * mm, unsigned long addr);
void __vma_link_rb(struct mm_struct *mm, struct vm_area_struct *vma, struct rb_node **rb_link, struct rb_node *rb_parent);

/* memory.c */
int handle_mm_fault(struct mm_struct *mm, struct vm_area_struct *vma,
		unsigned long address, unsigned int flags);

void unmap_vmas(mm_struct_t *mm, vm_area_struct_t *vma, uint64_t start_addr, uint64_t end_addr);
#endif