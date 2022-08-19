#ifndef _H_WAITQUEUE_
#define _H_WAITQUEUE_

#include "common.h"
#include "atomic/spinlock.h"
#include "list.h"
#include "kernel/proc.h"


struct wait_queue {
    spinlock_t wq_lock;
    list_head_t head;
};


typedef struct wait_queue wq_t;

static inline int wq_is_empty(wq_t *self) {
    return list_empty(&self->head);
}

static inline list_head_t *__wq_poll(wq_t *self) {
    if(wq_is_empty(self)) return NULL;
    list_head_t *entry = self->head.next;
    list_del_init(entry);
    return entry;
}


#define wq_poll(self) ({\
    list_head_t *head = __wq_poll(self);\
    head ? list_entry(head, proc_t, state_head) : NULL;\
})


static inline void wq_offer(wq_t *self, proc_t *p) {
    list_add_tail(&p->state_head, &self->head);
}


static inline void wq_remove(wq_t *self, proc_t *p) {
    // self并不是必要的
    list_del_init(&p->state_head);
}

static inline void wq_lock(wq_t *self) {
    acquire(&self->wq_lock);
}

static inline void wq_unlock(wq_t *self) {
    release(&self->wq_lock);
}

// static inline void wq_sleep(wq_t *self, proc_t *p) {
//     wq_lock(self);
//     wq_offer(self, p);
//     p->__state = SLEEPING;
// }

// static inline void wq_wakeup(wq_t *self) {
//     proc_t *p, *tmp;
//     wq_lock(self);
//     list_for_each_entry_safe(p, tmp, &self->head, state_head) {
//         int hold = holding(&p->lock);
      
//         if(!hold) acquire(&p->lock);
//         assert(p->state == SLEEPING);

//         wq_remove(self, p);
//         pstate_migrate(p, RUNNABLE);
      
//         if(!hold) release(&p->lock);
//     }
//     wq_unlock(self);
// }


#define WQ_INIT(name) wq_t name = {.wq_lock=INIT_SPINLOCK(name), .head=LIST_HEAD_INIT(name.head)}

#endif