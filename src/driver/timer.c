#include "platform.h"
#include "mm/io.h"
#include "atomic/spinlock.h"
#include "sbi.h"
#include "kernel/waitqueue.h"


static uint64_t *clint_mtime;
struct spinlock tickslock;
uint64 ticks;

WQ_INIT(timer_wq);

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


void clockintr() {
  acquire(&tickslock);
  ticks++;
  // timespec_t time = TICK2TIMESPEC(ticks);
  // time_print(&time);
  // printf("%ld\n", ticks);
  wakeup(&ticks);
  release(&tickslock);
}