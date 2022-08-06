#include "atomic/atomic.h"

void atomic_test() {
    atomic_t a = INIT_ATOMIC;
    printf("a is %d\n", atomic_get(&a));
    int ret = atomic_inc(&a);
    printf("ret is %d now a is %d\n", ret, atomic_get(&a));
    ret = atomic_dec(&a);
    printf("ret is %d now a is %d\n", ret, atomic_get(&a));
    LOOP();
}