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
};

typedef struct spinlock spinlock_t;

void            acquire(struct spinlock*);
int             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            push_off(void);
void            pop_off(void);


#endif