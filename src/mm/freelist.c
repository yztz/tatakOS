// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "common.h"
#include "platform.h"
#include "atomic/spinlock.h"
#include "riscv.h"
#include "defs.h"
#include "mm/freelist.h"
#include "mm/page.h"
#include "mm/buddy.h"

extern char end[];

#define FREELIST_ALLOC_SIZE (1 * 1024 * 1024)

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
freelist_init()
{
  initlock(&kmem.lock, "kmem");
  kmem.freelist = 0;
  // freerange(end, (void*)PHYSTOP);
}

void
freelist_freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE){
    mark_page((uint64_t)pa, ALLOC_FREELIST);
    freelist_free(p);
  }
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
freelist_free(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("freelist_free");

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
freelist_alloc(void)
{
  void *alloc = NULL;
  struct run *r = NULL;

  acquire(&kmem.lock);
  if(kmem.freelist) {
    r = kmem.freelist;
  } else {
    alloc = buddy_alloc(FREELIST_ALLOC_SIZE);
    if(alloc) {
      freelist_freerange(alloc, (void *)((uint64_t)alloc + FREELIST_ALLOC_SIZE));
      r = kmem.freelist;
    }
  }
  
  if(r) {
    kmem.freelist = r->next;
  }
  release(&kmem.lock);

  return (void*)r;
}
