#include "atomic/semaphore.h"

void sem_down(sem_t *sem) {
    acquire(&sem->waitqueue.wq_lock);
    wait_event_locked(&sem->waitqueue, sem->count > 0);
    sem->count--;
    release(&sem->waitqueue.wq_lock);
}


void sem_up(sem_t *sem) {
    acquire(&sem->waitqueue.wq_lock);
    if (++sem->count > 0) {
        wq_wakeup(&sem->waitqueue);
    }
    release(&sem->waitqueue.wq_lock);
}