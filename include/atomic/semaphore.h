#ifndef _H_SEMAPHORE_
#define _H_SEMAPHORE_ 1

#include "atomic/spinlock.h"
#include "kernel/waitqueue.h"

typedef struct semaphore {
    // spinlock_t lock;
    int count;
    wq_t waitqueue;
} sem_t;


void sem_down(sem_t *sem);
void sem_up(sem_t *sem);

#define INIT_SEM(COUNT) \
    { .count=COUNT, .lock=INIT_SPINLOCK(sem_inner_lock), .waitqueue=INIT_WAIT_QUEUE(sem_inner_waitqueue) }
#define SEM_INIT(name, COUNT) sem_t name = INIT_SEM(COUNT)

#define MUTEX_INIT(name) SEM_INIT(name, 1)
#define INIT_MUTEX() INIT_SEM(1)

#endif