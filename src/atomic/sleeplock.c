// Sleeping locks

#include "kernel/proc.h"
#include "kernel/waitqueue.h"
#include "atomic/sleeplock.h"
#include "atomic/spinlock.h"

void initsleeplock(struct sleeplock *lk, char *name) {
    lk->waitqueue = (wq_t)INIT_WAIT_QUEUE(lk->waitqueue);
    lk->name = name;
    lk->locked = 0;
    lk->process = NULL;
}

/**
 * @brief 睡眠锁被某个进程第一次获得，lk->locked不为1，所以这个进程不会
 * 睡眠，设置为为1后继续执行。其他进程想要再获得睡眠锁时，因为lk->locked
 * 已经为1了，所以会睡眠。
 *
 * @param slk
 */
void acquiresleep(struct sleeplock *lk) {
    acquire(&lk->waitqueue.wq_lock);
    
    wait_event_locked(&lk->waitqueue, lk->locked == 0);
    lk->locked = 1;
    lk->process = current;

    release(&lk->waitqueue.wq_lock);
}

void releasesleep(struct sleeplock *lk) {
    acquire(&lk->waitqueue.wq_lock);
    lk->locked = 0;
    lk->process = NULL;
    wq_wakeup_locked(&lk->waitqueue);
    release(&lk->waitqueue.wq_lock);
}

int holdingsleep(struct sleeplock *lk) {
    int r;

    acquire(&lk->waitqueue.wq_lock);
    r = lk->locked && (lk->process == current);
    release(&lk->waitqueue.wq_lock);
    return r;
}



