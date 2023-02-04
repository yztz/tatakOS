#include "printf.h"
#include "mm/alloc.h"
#include "common.h"

#define DIVIDER() {printf("\n---------------------------------\n");}

void mm_test() {
    uint64 mem1 = (uint64_t)kmalloc(257);
    DIVIDER();
    uint64 mem2 = (uint64_t)kmalloc(257);
    DIVIDER();
    printf("mem1 addr is %p\n", mem1);
    printf("mem2 addr is %p\n", mem2);
    DIVIDER();
    kfree((void *)mem1);
    DIVIDER();
    kfree((void *)mem2);
    DIVIDER();
    mem1 = (uint64_t)kmalloc(128);
    DIVIDER();
    mem2 = (uint64_t)kmalloc(256);
    DIVIDER();
    printf("mem1 addr is %p\n", mem1);
    printf("mem2 addr is %p\n", mem2);
    DIVIDER();
    kfree((void *)mem2);
    DIVIDER();
    kfree((void *)mem1);
    DIVIDER();

    LOOP();
}