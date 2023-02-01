#include "kernel/waitqueue.h"
#include "kernel/proc.h"

#define __MODULE_NAME__ WQ

#include "debug.h"

static inline void offer(wq_t *self, wq_entry_t* entry) {
    list_add_tail(&entry->head, &self->head);
}

static inline wq_entry_t *poll(wq_t *self) {
    if(list_empty(&self->head)) return NULL;

    list_head_t *entry = self->head.next;
    list_del_init(entry);
    return list_entry(entry, wq_entry_t, head);
}

static inline wq_entry_t *peek(wq_t *self) {
    if(list_empty(&self->head)) return NULL;

    return list_first_entry(&self->head, wq_entry_t, head);
}

void wq_prepare(wq_t *self) {
    acquire(&self->wq_lock);
}

void wq_sleep(wq_t *self, wq_entry_t* entry) {
    if(!holding(&self->wq_lock))
        panic("Sleep without preparing. Wakeup may be lost.");

    proc_t *p = entry->private;
    acquire(&p->lock);

    offer(self, entry);
    p->wait_channel = self;
    pstate_migrate(p, SLEEPING);
    release(&self->wq_lock);

    sched();

    // remove self from waitqueue
    acquire(&self->wq_lock);
    list_del_init(&entry->head);
    release(&self->wq_lock);

    p->wait_channel = NULL;
    release(&p->lock);
}

int wq_sleep_timeout(wq_t *self, wq_entry_t* entry, int timeout) {
    if(!holding(&self->wq_lock))
        panic("Sleep without preparing. Wakeup may be lost.");

    proc_t *p = entry->private;
    acquire(&p->lock);

    offer(self, entry);
    p->wait_channel = self;
    pstate_migrate(p, SLEEPING);
    release(&self->wq_lock);

    int is_timeout = sched_timeout(timeout);

    // remove self from waitqueue
    acquire(&self->wq_lock);
    list_del_init(&entry->head);
    release(&self->wq_lock);

    p->wait_channel = NULL;
    release(&p->lock);

    return is_timeout;
}


void wq_cancel(wq_t *self) {
    release(&self->wq_lock);
}


void wq_wakeup(wq_t *self) {
    wq_entry_t *entry;

    acquire(&self->wq_lock);
    if((entry = poll(self))) 
        wake_up_process(entry->private);
    release(&self->wq_lock);
}


void wq_wakeup_all(wq_t *self) {
    wq_entry_t *entry;

    acquire(&self->wq_lock);
    while((entry = poll(self))) 
        wake_up_process(entry->private);
    release(&self->wq_lock);
}