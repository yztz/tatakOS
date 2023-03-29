/**
 * @file waitqueue.h
 * @author YangZongzhen
 * @brief ref: https://elixir.bootlin.com/linux/v2.6.39.4/source/include/linux/wait.h
 * @version 0.1
 * @date 2023-03-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _H_WAITQUEUE_
#define _H_WAITQUEUE_

#include "atomic/spinlock.h"
#include "riscv.h"
#include "list.h"
#include "kernel/sched.h"
#include "sys/error.h"

typedef void (*wakeup_callback_t)(wq_t *wq, wq_entry_t *entry);

struct wq_entry {
    proc_t *private;
    list_head_t head;
    wakeup_callback_t func;
};

struct waitqueue {
    spinlock_t wq_lock;
    list_head_t head;
};

static inline void __add_to_waitqueue(wq_t *self, wq_entry_t *entry) {
    list_add_tail(&entry->head, &self->head);
}

static inline void __rm_from_waitqueue(wq_t *self, wq_entry_t *entry) {
    list_del_init(&entry->head);
}
extern int kprintf(const char *format, ...);
extern void wake_up_process(proc_t *p);
extern void wake_up_process_locked(proc_t *p);

static void auto_remove_callback(wq_t *wq, wq_entry_t *entry) {
    proc_t *p = entry->private;
    if (list_empty(&entry->head)) {
        kprintf("bad1\n");
        for(;;);
    }
    __rm_from_waitqueue(wq, entry);
    acquire(&p->lock);
    if(entry->private->state == RUNNING) {
        kprintf("bad2\n");
        for(;;);
    }
    wake_up_process_locked(p);
    release(&p->lock);
    // wake_up_process(entry->private);
}

#define INIT_WAIT_QUEUE(name) {.wq_lock=INIT_SPINLOCK((name).wq_lock), .head=LIST_HEAD_INIT((name).head)}
#define WAIT_QUEUE_INIT(name) wq_t name = INIT_WAIT_QUEUE(name)
#define DECLARE_WQ_ENTRY(name) wq_entry_t name = {.private=current, .head=LIST_HEAD_INIT((name).head), .func=auto_remove_callback}


#define __wait_event_timeout(wq, condition, locked, timeout) ({ \
    __label__ out;                                              \
    int __ret = timeout;                                        \
                                                                \
    DECLARE_WQ_ENTRY(__entry);                                  \
    proc_t *p = current;                                        \
    if (!(locked))                                              \
        acquire(&(wq)->wq_lock);                                \
                                                                \
    do {                                                        \
        acquire(&p->lock);                                      \
        p->wait_channel = (wq);                                 \
        p->__state = SLEEPING;                                  \
        if (list_empty(&__entry.head))                          \
            __add_to_waitqueue(wq, &__entry);                   \
        release(&(wq)->wq_lock);                                \
        __ret = sched_timeout(__ret);                           \
        p->wait_channel = NULL;                                 \
        release(&p->lock);                                      \
        acquire(&(wq)->wq_lock);                                \
        if (__ret <= 0) {                                       \
            __ret = -ETIMEDOUT;                                 \
            goto out;                                           \
        }                                                       \
    } while (!(condition) && __ret > 0);                        \
    out:                                                        \
    __rm_from_waitqueue(wq, &__entry);                          \
    if (!(locked))                                              \
        release(&(wq)->wq_lock);                                \
    __ret;                                                      \
})

#define __wait_event(wq, condition, locked) ({   \
    int __ret = 0;                                              \
                                                                \
                                                                \
    DECLARE_WQ_ENTRY(__entry);                                  \
    proc_t *p = current;                                        \
    if (!(locked))                                              \
        acquire(&(wq)->wq_lock);                                \
                                                                \
    do {                                                        \
        acquire(&p->lock);                                      \
        p->wait_channel = (wq);                                 \
        p->__state = SLEEPING;                                  \
        if (list_empty(&__entry.head))                          \
            __add_to_waitqueue(wq, &__entry);                   \
        release(&(wq)->wq_lock);                                \
        sched();                                                \
        p->wait_channel = NULL;                                 \
        release(&p->lock);                                      \
        acquire(&(wq)->wq_lock);                                \
    } while (!(condition));                                     \
    __rm_from_waitqueue(wq, &__entry);                          \
    if (!(locked))                                              \
        release(&(wq)->wq_lock);                                \
    __ret;                                                      \
})

#define wait_event_locked(wq, condition) \
    ((condition) ? 0 : (__wait_event(wq, condition, 1)));

#define wait_event_timeout_locked(wq, condition, timeout) \
    ((condition) ? 0 : (__wait_event_timeout(wq, condition, 1, timeout)));

static inline void __wq_wakeup(wq_t *self) {
    wq_entry_t *entry, *tmp;

    list_for_each_entry_safe(entry, tmp, &self->head, head) {
        (entry->func)(self, entry);
    }
}


#define wq_wakeup_locked(wq) __wq_wakeup(wq)

#define wq_wakeup(wq) do {      \
    acquire(&(wq)->wq_lock);    \
    wq_wakeup_locked(wq);       \
    release(&(wq)->wq_lock);    \
} while (0);

#endif
