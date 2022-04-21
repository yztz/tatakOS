/**
 * ref: https://www.cnblogs.com/alantu2018/p/8527821.html
 */
#define remove _remove
#include <stdio.h>
#include <stdlib.h>
#include "../../include/types.h"
#undef remove

// #include "atomic/spinlock.h"
// #include "common.h"
// #include "page.h"
// #include "list.h"
char* end;
#define acquire(a)
#define release(a)
#define panic(a) printf(a "\n");

#define PGSIZE_SPEC(spec) (1 << (12 + 9 * (spec)))
#define PGSIZE PGSIZE_SPEC(0)
#define PGROUNDUP_SPEC(sz, sepc) \
    (((sz) + (PGSIZE_SPEC(sepc)) - 1) & ~((PGSIZE_SPEC(sepc)) - 1))
#define PGROUNDUP(sz) PGROUNDUP_SPEC((uint64)sz, 0)

#define ROUND_COUNT_SPEC(sz, spec) \
    (PGROUNDUP_SPEC(sz, spec) / PGSIZE_SPEC(spec))
#define ROUND_COUNT(sz) ROUND_COUNT_SPEC(sz, 0)

#define POW2(n) (1 << n)
#define IS_POW2(n) ((n & (n - 1)) == 0)

#define MEM_SIZE (4 * 1024 * 1024) // 4M
uint64 KERNBASE;

#define PAGE_NUMS (MEM_SIZE / PGSIZE)
#define PAGE2NUM(va) (((uint64_t)(va)-KERNBASE) / PGSIZE)
#define NUM2PAGE(num) ((void*)((num)*PGSIZE + KERNBASE))

typedef struct _page_t {
    uint8_t refcnt;
    struct {
        uint8_t order : 4;  // use lowest 4 bits only: 0..10
        uint8_t alloc : 1;
    };
} page_t;

int get_order(unsigned int x) {
    static const unsigned char log_2[256] = {
        0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    };

    int l = -1;
    while (x >= 256) {
        l += 8;
        x >>= 8;
    }
    return l + log_2[x];
}

struct spinlock {
    uint locked;  // Is the lock held?

    // For debugging:
    char* name;  // Name of lock.
};

//==================================

#define MAX_ORDER 10

#define PARTNER_NO(pgnum, order) (pgnum ^ (1 << order))
#define MERGE_NO(pgnum, order) (pgnum & ~(1 << order))

#define BUDDY_INIT_HEAD(head) \
    { (head) = (buddy_t){&(head), &(head)}; }

typedef struct _buddy_t {
    struct _buddy_t *next, *pre;
} buddy_t;

typedef struct _buddy_list_t {
    buddy_t head;
    struct spinlock lock;
} buddy_list_t;

page_t pages[PAGE_NUMS];
buddy_list_t lists[MAX_ORDER];
void buddy_free(void* va);
// void stub() {
//   printf("this is a stub!\n");
// }
void buddy_init() {
    int knum = PAGE2NUM(PGROUNDUP(end));

    for (int i = 0; i < MAX_ORDER; i++) {
        BUDDY_INIT_HEAD(lists[i].head);
        // initlock(&lists[i].lock, "buddy list");
    }

    for (int i = 0; i < PAGE_NUMS; i++) {
        pages[i].refcnt = 0;
        pages[i].order = 0;
        pages[i].alloc = 1;
    }

    printf("knum is: %d, page_nums is: %d\n", knum, PAGE_NUMS);
    for (int i = knum; i < PAGE_NUMS; i++) {
        // printf("release page %d all: %d\n", i, PAGE_NUMS);
        // if(i == 2047) stub();
        buddy_free(NUM2PAGE(i));
    }
}

void print_order(int order) {
  printf("[order %d head: %p] ", order, &lists[order].head);
  for(buddy_t *cur = lists[order].head.next; cur != &lists[order].head; cur = cur->next) {
      printf("-> %p", cur);
  }
}


void print_buddy() {
  for(int i = 0; i < MAX_ORDER; i++) {
    print_order(i);
    printf("\n");
  }
}

void insert(int order, buddy_t *b) {
  b->next = lists[order].head.next;
  b->pre = &lists[order].head;
  lists[order].head.next->pre = b;
  lists[order].head.next = b;
  // printf("after insert: \n");
  // print_list(order);
}

// removed block must have existed in the list
void remove(buddy_t* b) {
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

  printf("select order: %d\n", order);
  // acquire lock
  // avoid last not empty list's block being allocated
  acquire(&lists[order].lock);
  while(order < MAX_ORDER && empty(order)){
    release(&lists[order].lock);
    order++;
    acquire(&lists[order].lock);
  }
  printf("select order: %d\n", order);

  // no rooms
  if(order == MAX_ORDER) {
    release(&lists[order].lock);
    return NULL;
  }

  int pgnum, ppgnum;

  while(order > oorder) {
    b = lists[order].head.next;
    remove(b);
    release(&lists[order].lock);
    order--;
    pgnum = PAGE2NUM(b);
    ppgnum = PARTNER_NO(PAGE2NUM(b), order);
    acquire(&lists[order].lock);
    insert(order, (buddy_t *)NUM2PAGE(ppgnum));
    insert(order, b);
    pages[pgnum].order = order;
    pages[ppgnum].order = order;
    printf("split order %d addr %#lx -> %#lx, %#lx\n", order + 1, (uint64_t)b, (uint64_t)b, (uint64_t)NUM2PAGE(ppgnum));
  }
  b = lists[order].head.next;
  remove(b);
  release(&lists[order].lock);
  
  pages[PAGE2NUM(b)].alloc = 1;
  return (void *) b;
}

void buddy_free(void *va) {
  int pgnum, ppgnum;
  page_t *page, *ppage;
  buddy_list_t *list;
  buddy_t *b;

  pgnum = PAGE2NUM(va);

  if(pgnum >= PAGE_NUMS) {
    panic("buddy_free: out of range");
  }

  page = &pages[pgnum];

  if(page->alloc == 0)
    panic("buddy_free: page not allocated");
  
  list = &lists[page->order];
  b = (buddy_t *) va;
  //release
  page->alloc = 0;
  // insert back
  acquire(&list->lock);
  insert(page->order, b);

  // printf("va: %p, pgnum: %d, macro va: %p\n", va, pgnum, NUM2PAGE(pgnum));

  ppgnum = PARTNER_NO(pgnum, page->order);
  ppage = &pages[ppgnum];
  // we need hold lock to avoid partner being allocated
  while(page->order + 1 < MAX_ORDER && ppage->alloc == 0 && page->order == ppage->order) {
    // remove from list
    remove((buddy_t *)NUM2PAGE(pgnum));
    remove((buddy_t *)NUM2PAGE(ppgnum));
    // printf("after remove: \n");
    // print_list(page->order);
    // release lock to continue others' alloc
    release(&list->lock);
    // the pgnum is always free because we merged from two free blocks.
    pgnum = MERGE_NO(pgnum, page->order);
    page = &pages[pgnum];
    page->order++; // no need to set alloc
    // so we can insert it back
    list = &lists[page->order];
    b = (buddy_t *) NUM2PAGE(pgnum);
    acquire(&list->lock);
    insert(page->order, b);

    ppgnum = PARTNER_NO(pgnum, page->order);
    ppage = &pages[ppgnum];
    // continue check...
  }
  release(&list->lock);

}

int main() {
    KERNBASE = (uint64_t) malloc(2 * MEM_SIZE);
    KERNBASE = PGROUNDUP(KERNBASE);
    end = (char *)(KERNBASE + 0x1234);
    buddy_init();
    while(1) {
      char c;
      uint64_t addr;
      scanf("%c", &c);
      if(c == '+') {
        size_t size;
        scanf("%ld", &size);
        addr = (uint64)buddy_alloc(size);
        printf("addr is: %#lx\n", addr);
      } else if(c == '-') {
        scanf("%lx", &addr);
        buddy_free((void *)addr);
        printf("released...\n");
      } else if(c == '=') {
        print_buddy();
      }
    }
    uint64_t addr = (uint64_t)buddy_alloc(PGSIZE);
    printf("addr is: %#lx\n", addr);

    return 0;
}
