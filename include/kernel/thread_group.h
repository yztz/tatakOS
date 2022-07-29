#ifndef _H_THREAD_GROUP_
#define _H_THREAD_GROUP_

#include "common.h"
#include "atomic/spinlock.h"

#include "list.h"

struct proc;

typedef struct tg {
    int ref;
    spinlock_t lock;
    int master_pid;
    list_head_t member;
} tg_t;

tg_t *tg_new(struct proc *p) ;
void tg_free(tg_t **pself);
// void tg_ref(tg_t *self);
// void tg_deref(tg_t *self);
void tg_join(tg_t *self, struct proc *p);
struct proc *tg_main_thrd(tg_t *self);


#endif