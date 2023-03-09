#ifndef _H_SPINLOCK_
#define _H_SPINLOCK_

#include "types.h"
#include "bitops.h"

#define INIT_SPINLOCK(NAME) (spinlock_t){.locked=0, .name=#NAME, .cpu=NULL}
#define SPINLOCK_INIT(NAME) spinlock_t NAME = (spinlock_t){.locked=0, .name=#NAME, .cpu=NULL}

// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
};

typedef struct spinlock spinlock_t;

#define spin_lock(lock) acquire(lock)
#define spin_unlock(lock) release(lock)

int             try_acquire(struct spinlock *lk);
void            acquire(struct spinlock*);
int             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            push_off(void);
void            pop_off(void);

#define DEFINE_SPINLOCK(x, name) spinlock_t x = INIT_SPINLOCK(name)



void bit_spin_lock(int bitnum, unsigned long *addr);
void bit_spin_unlock(int bitnum, unsigned long *addr);

#endif