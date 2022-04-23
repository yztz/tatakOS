/**
 * ref: https://www.cnblogs.com/alantu2018/p/8527821.html
*/

#include "atomic/spinlock.h"
#include "common.h"
#include "mm/page.h"
#include "mm/buddy.h"

#define INVAIL_ORDER (0xf)

#define PARTNER_NO(pgnum, order) ((pgnum) ^ (1 << (order)))
#define MERGE_NO(pgnum, order) ((pgnum) & ~(1 << (order)))

#define BUDDY_INIT_HEAD(head) {(head) = (buddy_t){&(head), &(head)};}

typedef struct _buddy_t {
  struct _buddy_t *next, *pre;
} buddy_t;

typedef struct _buddy_list_t {
  buddy_t head;
  struct spinlock lock;
} buddy_list_t;

extern char end[];

buddy_list_t lists[MAX_ORDER];

void buddy_init() {
  int knum = PAGE2NUM(end) + 1;

  for(int i = 0; i < MAX_ORDER; i++) {
    BUDDY_INIT_HEAD(lists[i].head);
    initlock(&lists[i].lock, "buddy list");
  }

  for(int i = knum; i < PAGE_NUMS; i++) {
    buddy_free(NUM2PAGE(i));
  }
  
}

static inline void insert(int order, buddy_t *b) {
  b->next = lists[order].head.next;
  b->pre = &lists[order].head;
  lists[order].head.next->pre = b;
  lists[order].head.next = b;
}

static inline void remove(buddy_t *b) {
  b->next->pre = b->pre;
  b->pre->next = b->next;
  b->next = b;
  b->pre = b;
}

// static inline int empty(int order) {
//   return lists[order].head.next == &lists[order].head;
// }

static inline int empty(int order) {
  return lists[order].head.next == &lists[order].head;
}


#include "printf.h"
void *buddy_alloc(size_t size) {
  int pgnums;
  int order, oorder;
  buddy_t *b;

  if(!size)
    panic("buddy_alloc: size");
  
  pgnums = ROUND_COUNT(size);
  oorder = order = IS_POW2(pgnums) ? get_order(pgnums) : get_order(pgnums) + 1;
  if(order >= MAX_ORDER) 
    return NULL;

  // acquire lock
  // avoid last not empty list's block being allocated
  acquire(&lists[order].lock);
  while(order + 1 < MAX_ORDER && empty(order)){
    release(&lists[order].lock);
    order++;
    acquire(&lists[order].lock);
  }
  // no rooms
  if(empty(order)) {
    release(&lists[order].lock);
    return NULL;
  }

  while(order > oorder) {
    int pgnum, ppgnum;

    b = lists[order].head.next;
    remove(b);
    pgnum = PAGE2NUM(b);
    ppgnum = PARTNER_NO(PAGE2NUM(b), order - 1);
    pages[pgnum].order = INVAIL_ORDER;
    pages[ppgnum].order = INVAIL_ORDER;
    release(&lists[order].lock);

    order--;

    acquire(&lists[order].lock);
    insert(order, b);
    insert(order, (buddy_t *)NUM2PAGE(ppgnum));
    pages[pgnum].order = order;
    pages[ppgnum].order = order;
  }
  b = lists[order].head.next;
  remove(b);
  pages[PAGE2NUM(b)].alloc = 1;
  release(&lists[order].lock);
  
  return (void *) b;
}

void buddy_free(void *va) {
  int pgnum, ppgnum;
  page_t *page, *ppage;
  buddy_t *b;

  pgnum = PAGE2NUM(va);

  if(pgnum >= PAGE_NUMS) {
    panic("buddy_free: out of range");
  }

  page = &pages[pgnum];

  if(page->alloc == 0) {
    print_page(pgnum);
    panic("buddy_free: page not allocated");
  }
  b = (buddy_t *) va;
  //release
  page->alloc = 0;
  // insert back
  acquire(&lists[page->order].lock);
  insert(page->order, b);

  // printf("va: %p, pgnum: %d, macro va: %p\n", va, pgnum, NUM2PAGE(pgnum));
  ppgnum = PARTNER_NO(pgnum, page->order);
  ppage = &pages[ppgnum];
  // we need hold lock to avoid partner being allocated
  while(page->order + 1 < MAX_ORDER && ppage->alloc == 0 && page->order == ppage->order) {
    int order;
    // remove from list
    remove((buddy_t *)NUM2PAGE(pgnum));
    remove((buddy_t *)NUM2PAGE(ppgnum));
    order = page->order;
    page->order = INVAIL_ORDER;
    ppage->order = INVAIL_ORDER;
    release(&lists[order].lock);
    
    acquire(&lists[order + 1].lock);
    pgnum = MERGE_NO(pgnum, order);
    page = &pages[pgnum];
    page->order = order + 1;
    b = (buddy_t *) NUM2PAGE(pgnum);

    insert(page->order, b);

    ppgnum = PARTNER_NO(pgnum, page->order);
    ppage = &pages[ppgnum];
  }
  release(&lists[page->order].lock);

}




