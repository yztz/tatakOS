#include "stddef.h"
#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"

#define TIMES 2000000

void thread_fn(void *arg) {
    int *l = (int *)arg;
    printf("Thread ID %d Starts.\n", gettid());
    for (int i = 0; i < TIMES; i++) {
        *l = *l + 1;
    }
    printf("Thread ID %d Ends.\n", gettid());
}

int main() {
    int ret;
    thread_t thread1;
    thread_t thread2;
    int local = 0;

    ret = thread_create(&thread1, thread_fn, &local);
    assert(ret == 0);
    ret = thread_create(&thread2, thread_fn, &local);
    assert(ret == 0);

    thread_join(&thread1);
    thread_join(&thread2);

    printf("The value of l is %d\n", local);
    return 0;
}