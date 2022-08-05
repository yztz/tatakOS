/**
 * ref: https://www.cnblogs.com/alantu2018/p/8527821.html
*/

#include "atomic/spinlock.h"
#include "atomic/atomic.h"
#include "common.h"
#include "mm/page.h"
#include "mm/buddy.h"

#define __MODULE_NAME__ BUDDY
#include "debug.h"

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

static atomic_t used;
static uint total;

uint64_t buddy_getfree() {
  return total - atomic_get(&used);
}

uint64_t buddy_gettotal() {
  return total;
}

void buddy_init() {
  for(int i = 0; i < MAX_ORDER; i++) {
    BUDDY_INIT_HEAD(lists[i].head);
    initlock(&lists[i].lock, "buddy list");
  }
  total = 0;
  /* 从kernel end处一直释放到MEM_END */
  for(uint64_t p = (uint64_t)end; p < MEM_END; p += PGSIZE) {
    buddy_free((void *)p);
    total++;
  }
  used = INIT_ATOMIC;
  // buddy_print_free();
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


static inline int empty(int order) {
  return lists[order].head.next == &lists[order].head;
}


void *buddy_alloc(size_t size) {
  int pgnums;
  int order, oorder;
  buddy_t *b;

  if(!size)
    panic("buddy_alloc: size");
  
  pgnums = ROUND_COUNT(size);
  // 获取最小的大于size的2^order
  oorder = order = IS_POW2(pgnums) ? get_order(pgnums) : get_order(pgnums) + 1;
  if(order >= MAX_ORDER) 
    return NULL;

  // 从当前order向上，直到寻找到有空闲空间的order
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
  // 当当前的order大于预订的order时，需要分裂
  while(order > oorder) {
    int pgnum, ppgnum;

    b = lists[order].head.next;
    remove(b);
    // 当前buddy的页号
    pgnum = PAGE2NUM(b);
    // 兄弟buddy的页号
    // 为什么是order-1，因为我们这里的兄弟页号是指分裂后的兄弟页号
    ppgnum = PARTNER_NO(pgnum, order - 1);
    // 为了避免作为兄弟页被合并的风险
    // 比如当兄弟buddy被释放了，而当前的buddy正在处于分裂状态，所以存在竞争
    pages[pgnum].order = INVAIL_ORDER;
    pages[ppgnum].order = INVAIL_ORDER;
    release(&lists[order].lock);

    order--;

    acquire(&lists[order].lock);
    // 先插大地址，后插小地址，尽可能保证后续分配地址连续
    insert(order, (buddy_t *)NUM2PAGE(ppgnum));
    insert(order, b); 
    pages[pgnum].order = order;
    pages[ppgnum].order = order;
  }
  b = lists[order].head.next;
  remove(b);
  release(&lists[order].lock);
  
  pages[PAGE2NUM(b)].alloc = 1;
  mark_page((uint64_t)b, ALLOC_BUDDY);
  ref_page((uint64_t)b);

  atomic_add(&used, 1 << oorder);
  return (void *) b;
}

void buddy_free(void *pa) {
  int pgnum, ppgnum;
  page_t *page, *ppage;
  buddy_t *b;

  pgnum = PAGE2NUM(pa);

  if(pgnum >= PAGE_NUMS) {
    panic("buddy_free: out of range");
  }

  if(deref_page((uint64_t)pa) > 0) 
    return;

  page = &pages[pgnum];

  if(page->alloc == 0) {
    print_page(pgnum);
    panic("buddy_free: page not allocated");
  }
  b = (buddy_t *) pa;
  // release
  page->alloc = 0;
  atomic_add(&used, -(1 << page->order));

  // insert back
  acquire(&lists[page->order].lock);
  insert(page->order, b);

  ppgnum = PARTNER_NO(pgnum, page->order);
  ppage = &pages[ppgnum];
  // 尝试合并
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

void buddy_print_free() {
  int u = atomic_get(&used);
  printf("page usage: %d%% ( %d used | %d total )\n", u * 100 / total, u, total);
}


