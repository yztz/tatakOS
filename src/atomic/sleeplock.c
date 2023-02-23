// Sleeping locks

#include "kernel/proc.h"
#include "kernel/waitqueue.h"
#include "atomic/sleeplock.h"
#include "atomic/spinlock.h"

void initsleeplock(struct sleeplock *lk, char *name) {
    initlock(&lk->lk, "sleep lock");
    lk->waitqueue = (wq_t)INIT_WAIT_QUEUE(lk->waitqueue);
    lk->name = name;
    lk->locked = 0;
    lk->pid = 0;
}

/**
 * @brief 睡眠锁被某个进程第一次获得，lk->locked不为1，所以这个进程不会
 * 睡眠，设置为为1后继续执行。其他进程想要再获得睡眠锁时，因为lk->locked
 * 已经为1了，所以会睡眠。
 *
 * @param slk
 */
void acquiresleep(struct sleeplock *slk) {
    DECLARE_WQ_ENTRY(entry);
    acquire(&slk->lk);
    for (;;) {
        if (!slk->locked)
            break;
        wq_prepare(&slk->waitqueue);
        release(&slk->lk);
        wq_sleep(&slk->waitqueue, &entry);
        acquire(&slk->lk);
    }

    slk->locked = 1;
    slk->pid = current->pid;
    release(&slk->lk);
}

void releasesleep(struct sleeplock *lk) {
    acquire(&lk->lk);
    lk->locked = 0;
    lk->pid = 0;
    wq_wakeup(&lk->waitqueue);
    release(&lk->lk);
}

int holdingsleep(struct sleeplock *lk) {
    int r;

    acquire(&lk->lk);
    r = lk->locked && (lk->pid == myproc()->pid);
    release(&lk->lk);
    return r;
}



