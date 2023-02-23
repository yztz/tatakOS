#ifndef _H_WAITQUEUE_
#define _H_WAITQUEUE_

#include "atomic/spinlock.h"
#include "riscv.h"

struct proc;

struct wq_entry {
    struct proc *private;
    list_head_t head;
};

struct waitqueue {
    spinlock_t wq_lock;
    list_head_t head;
};

typedef struct waitqueue wq_t;
typedef struct wq_entry wq_entry_t;

/**
 * @brief Call before calling wq_sleep, sometimes it is used to avoid "Lost wakeup".
 *        What is "Lost Wakeup" ? If A are going to sleep but the state hasn't changed, 
 *        B wakes up it at same time so nothing happened. The key problem here is that 
 *        there is no mechanism for atomically putting a process to sleep while it is sleeping.
 *          
 *        For example, a concurrent access to a disk block, when process A requests access to the disk block, 
 *        because the cache block is already held by process B, so process A is ready to go to sleep, 
 *        and while A is entering the state, process B releases the disk block cache, 
 *        triggering the wake-up signal of the block cache sleep queue. 
 *        However, because A does not enter the sleep state absolutely, it loses the wake-up signal and falls into sleep forever.
 * 
 * @param self 
 */
void wq_prepare(wq_t *self);
void wq_sleep(wq_t *self, wq_entry_t* entry);
int wq_sleep_timeout(wq_t *self, wq_entry_t* entry, int timeout);
void wq_wakeup(wq_t *self);
void wq_wakeup_all(wq_t *self);

#define INIT_WAIT_QUEUE(name) (wq_t){.wq_lock=INIT_SPINLOCK((name).wq_lock), .head=LIST_HEAD_INIT((name).head)}
#define WAIT_QUEUE_INIT(name) wq_t name = INIT_WAIT_QUEUE(name)
#define DECLARE_WQ_ENTRY(name) wq_entry_t name = {.private=current, .head=LIST_HEAD_INIT((name).head)}

#endif