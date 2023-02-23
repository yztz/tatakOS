#include "atomic/semaphore.h"

void sem_down(sem_t *sem) {
    acquire(&sem->lock);

    while (sem->count <= 0) {
        DECLARE_WQ_ENTRY(entry);
        wq_prepare(&sem->waitqueue);

        release(&sem->lock);
        wq_sleep(&sem->waitqueue, &entry);
        acquire(&sem->lock);
    };

    sem->count--;

    release(&sem->lock);
}


void sem_up(sem_t *sem) {
    acquire(&sem->lock);

    if (++sem->count > 0) {
        wq_wakeup(&sem->waitqueue);
    }

    release(&sem->lock);
}