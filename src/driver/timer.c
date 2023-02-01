#include "platform.h"
#include "mm/io.h"
#include "atomic/spinlock.h"
#include "sbi.h"
#include "kernel/proc.h"
#include "kernel/time.h"
#include "kernel/waitqueue.h"
#include "driver/timer.h"
#include "defs.h"
#include "mm/vm.h"


static uint64_t *clint_mtime;
struct spinlock tickslock;
uint64 ticks;

WAIT_QUEUE_INIT(timer_waiters);


SPINLOCK_INIT(timer_lock);
timer_t timer_head = {.head=LIST_HEAD_INIT(timer_head.head)};



void init_timer() {
    initlock(&tickslock, "time");
    clint_mtime = (uint64_t *)(ioremap(CLINT, 0x10000) + 0Xbff8);
    ticks = 0;
}

uint64_t get_time() {
    return *clint_mtime;
}

void reset_timer() {
    sbi_legacy_set_timer(get_time() + CLOCK_FREQ * TICK_GRANULARITY / 1000);
}


void wait_tick() {
    DECLARE_WQ_ENTRY(entry);
    wq_prepare(&timer_waiters);
    wq_sleep(&timer_waiters, &entry);
}


void set_timer(timer_t *timer, timer_callback_t callback, int timeout, void *param) {
    timer->callback = callback;
    timer->param = param;
    timer->expires = MS2TICK(timeout);

    acquire(&timer_lock);
    list_add(&timer->head, &timer_head.head);
    release(&timer_lock);
}


void remove_timer(timer_t *timer) {
    acquire(&timer_lock);
    list_del_init(&timer->head);
    release(&timer_lock);
}



void clockintr() {
    acquire(&tickslock);
    ticks++;
    // wakeup(&ticks);
    wq_wakeup_all(&timer_waiters);

    acquire(&timer_lock);
    timer_t *cur, *n;
    list_for_each_entry_safe(cur, n, &timer_head.head, head) {
        cur->expires--;
        // out of date
        if(cur->expires < 0) {
            cur->callback(cur->param);
            list_del_init(&cur->head);
        }
    }

    release(&timer_lock);

    release(&tickslock);
}
