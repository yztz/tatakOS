#ifndef _H_THREAD_GROUP_
#define _H_THREAD_GROUP_

#include "atomic/spinlock.h"
#include "list.h"

struct proc;

typedef struct tg {
    int tg_id;
    int ref;
    int thrdcnt;
    spinlock_t lock;
    list_head_t member;
} tg_t;

tg_t *tg_new(struct proc *p) ;
void tg_free(tg_t **pself);
void tg_ref(tg_t *self);
void tg_join(tg_t *self, struct proc *p);
int tg_thrd_cnt(tg_t *self);
int tg_quit(tg_t *self);

#endif