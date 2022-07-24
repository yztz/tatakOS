#ifndef _H_SPINLOCK_
#define _H_SPINLOCK_

#include "types.h"

#define INIT_SPINLOCK(ln) (spinlock_t){.locked=0, .name=#ln, .cpu=NULL};

// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
  int pid;
};

typedef struct spinlock spinlock_t;

#define spin_lock(lock) acquire(lock)
#define spin_unlock(lock) release(lock)

void            acquire(struct spinlock*);
int             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            push_off(void);
void            pop_off(void);

#define DEFINE_SPINLOCK(x, name) spinlock_t x = INIT_SPINLOCK(name)

#define pte_chain_lock(page)	bit_spin_lock(PG_chainlock, &page->flags)
#define pte_chain_unlock(page)	bit_spin_unlock(PG_chainlock, &page->flags)

/**
 * @brief 需要把test and set设置成原子的。
 * 
 */
static inline int bit_spin_lock(int bitnum, unsigned long *addr){

}

static inline void bit_spin_unlock(int bitnum, unsigned long *addr){

}

#endif