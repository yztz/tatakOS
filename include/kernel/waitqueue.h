#ifndef _H_WAITQUEUE_
#define _H_WAITQUEUE_


#include "atomic/spinlock.h"
#include "list.h"

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


#define INIT_WAIT_QUEUE(name) (wq_t){.wq_lock=INIT_SPINLOCK(name.wq_lock), .head=LIST_HEAD_INIT(name.head)}
#define WAIT_QUEUE_INIT(name) wq_t name =  {.wq_lock=INIT_SPINLOCK(name.wq_lock), .head=LIST_HEAD_INIT(name.head)}
#define DECLARE_WQ_ENTRY(name) wq_entry_t name = {.private=myproc(), .head=LIST_HEAD_INIT(name.head)}


void wq_prepare(wq_t *self);
void wq_sleep(wq_t *self, wq_entry_t* entry);
int wq_sleep_timeout(wq_t *self, wq_entry_t* entry, int timeout);
void wq_wakeup(wq_t *self);
void wq_wakeup_all(wq_t *self);


#endif