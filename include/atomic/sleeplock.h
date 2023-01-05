#ifndef _H_SLEEPLOCK_
#define _H_SLEEPLOCK_

#include "atomic/spinlock.h"
#include "kernel/waitqueue.h"

// Long-term locks for processes
struct sleeplock {
  uint locked;       // Is the lock held?
  struct spinlock lk; // spinlock protecting this sleep lock
  wq_t waitqueue;
  
  // For debugging:
  char *name;        // Name of lock.
  int pid;           // Process holding lock
};

typedef struct sleeplock sleeplock_t;


void            acquiresleep(struct sleeplock*);
void            releasesleep(struct sleeplock*);
int             holdingsleep(struct sleeplock*);
void            initsleeplock(struct sleeplock*, char*);

#endif