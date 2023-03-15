#include "mm/alloc.h"
#include "common.h"

#define DIVIDER() {kprintf("\n---------------------------------\n");}

void mm_test() {
    uint64 mem1 = (uint64_t)kmalloc(257);
    DIVIDER();
    uint64 mem2 = (uint64_t)kmalloc(257);
    DIVIDER();
    kprintf("mem1 addr is %p\n", mem1);
    kprintf("mem2 addr is %p\n", mem2);
    DIVIDER();
    kfree((void *)mem1);
    DIVIDER();
    kfree((void *)mem2);
    DIVIDER();
    mem1 = (uint64_t)kmalloc(128);
    DIVIDER();
    mem2 = (uint64_t)kmalloc(256);
    DIVIDER();
    kprintf("mem1 addr is %p\n", mem1);
    kprintf("mem2 addr is %p\n", mem2);
    DIVIDER();
    kfree((void *)mem2);
    DIVIDER();
    kfree((void *)mem1);
    DIVIDER();

    LOOP();
}