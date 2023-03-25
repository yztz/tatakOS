#include "kernel/thread_group.h"
#include "kernel/proc.h"
#include "mm/alloc.h"
#include "atomic/spinlock.h"

/* 
ref与thrdcnt不能混用
原因在于exit与freeproc是两个过程
*/

#define __MODULE_NAME__ THREAD_GROUP
#include "debug.h"

void tg_ref(tg_t *self) {
    acquire(&self->lock);
    self->ref++;
    release(&self->lock);
}

static void tg_deref(tg_t *self) {
    acquire(&self->lock);
    self->ref--;
    release(&self->lock);
}

tg_t *tg_new(proc_t *p) {
    tg_t *newtg = kzalloc(sizeof(tg_t));
    if(!newtg) {
        return NULL;
    }

    initlock(&newtg->lock, "threadgroup");
    INIT_LIST_HEAD(&newtg->member);
    
    // Set threadID as same as procID
    newtg->tg_id = p->pid;
    tg_join(newtg, p);

    newtg->futex_wq = (wq_t)INIT_WAIT_QUEUE(newtg->futex_wq);

    return newtg;
}

int tg_thrd_cnt(tg_t *self) {
    int ans;

    // 这里需要加锁吗？
    acquire(&self->lock);
    ans = self->thrdcnt;
    release(&self->lock);

    return ans;
}

void tg_join(tg_t *self, proc_t *p) {
    acquire(&self->lock);
    self->thrdcnt++;
    list_add_tail(&p->thrd_head, &self->member);
    release(&self->lock);
}

int tg_quit(tg_t *self) {
    int rest;
    proc_t *p = myproc();
    acquire(&self->lock);
    rest = --self->thrdcnt;
    list_del_init(&p->thrd_head);
    release(&self->lock);

    return rest;
}

int tg_pid(tg_t *self) {
    return self->tg_id;
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