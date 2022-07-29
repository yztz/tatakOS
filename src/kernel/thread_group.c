#include "kernel/thread_group.h"
#include "kernel/proc.h"
#include "mm/alloc.h"

#define __MODULE_NAME__ THREAD_GROUP
#include "debug.h"

static void tg_ref(tg_t *self) {
    acquire(&self->lock);
    self->ref++;
    release(&self->lock);
}

static void tg_deref(tg_t *self) {
    acquire(&self->lock);
    self->ref--;
    release(&self->lock);
}

proc_t *tg_main_thrd(tg_t *self) {
    proc_t *ans = list_first_entry(&self->member, proc_t, head);
    if(ans->pid != self->master_pid)
        panic("discord");
    return ans;
}

tg_t *tg_new(proc_t *p) {
    tg_t *newtg = kzalloc(sizeof(tg_t));
    if(newtg) {
        initlock(&newtg->lock, "threadgroup");
    }
    INIT_LIST_HEAD(&newtg->member);
    
    newtg->master_pid = p->pid;
    tg_join(newtg, p);

    return newtg;
}

void tg_join(tg_t *self, proc_t *p) {
    acquire(&self->lock);
    self->ref++;
    list_add_tail(&p->head, &self->member);
    release(&self->lock);
}

void tg_free(tg_t **pself) {
    if(pself == NULL || *pself == NULL)
        return;
    tg_t *self = *pself;

    if(self->ref == 0)
        panic("ref");
        
    tg_deref(self);

    if(self->ref > 0)
        return;

    kfree_safe(pself);
}