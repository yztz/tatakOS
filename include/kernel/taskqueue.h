#ifndef _H_TASKQUEUE_
#define _H_TASKQUEUE_

#include "atomic/spinlock.h"
#include "list.h"
#include "kernel/proc.h"


struct taskqueue {
    spinlock_t tq_lock;
    list_head_t head;
};


typedef struct taskqueue tq_t;

static inline int tq_is_empty(tq_t *self) {
    return list_empty(&self->head);
}

static inline proc_t *tq_poll(tq_t *self) {
    if(tq_is_empty(self)) return NULL;
    list_head_t *entry = self->head.next;
    list_del_init(entry);
    return list_entry(entry, proc_t, state_head);
}



static inline void tq_offer(tq_t *self, proc_t *p) {
    list_add_tail(&p->state_head, &self->head);
}


static inline void tq_remove(tq_t *self, proc_t *p) {
    // self并不是必要的
    list_del_init(&p->state_head);
}

static inline void tq_lock(tq_t *self) {
    acquire(&self->tq_lock);
}

static inline void tq_unlock(tq_t *self) {
    release(&self->tq_lock);
}


#define TASK_QUEUE_INIT(name) tq_t name = {.tq_lock=INIT_SPINLOCK(name), .head=LIST_HEAD_INIT(name.head)}

#endif