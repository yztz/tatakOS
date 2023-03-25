#ifndef _H_SLEEPLOCK_
#define _H_SLEEPLOCK_

#include "atomic/spinlock.h"
#include "kernel/waitqueue.h"

struct sleeplock {
    /// @brief Is the lock held?
    int locked;
    wq_t waitqueue;

    // For debugging:

    /// @brief Name of lock.
    char *name;
    /// @brief Process holding lock
    proc_t *process;
};

typedef struct sleeplock sleeplock_t;

void acquiresleep(struct sleeplock *);
void releasesleep(struct sleeplock *);
int holdingsleep(struct sleeplock *);
void initsleeplock(struct sleeplock *, char *);

#endif
