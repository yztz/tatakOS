#include "stddef.h"
#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"

#define TIMES 200
int a = 0;
int b = 0;

void thread1_fn(void *arg) {
    for (int i = 0; i < TIMES; i++) {
        a--;
    }
    sleep(1);
}

void thread2_fn(void *arg) {
    for (int i = 0; i < TIMES; i++) {
        b++;
    }
    sleep(2);
}

int main() {
    int ret;
    thread_t thread1;
    thread_t thread2;

    ret = thread_create(&thread1, thread1_fn, NULL);
    assert(ret == 0);
    ret = thread_create(&thread2, thread2_fn, NULL);
    assert(ret == 0);

    for (int i = 0; i < TIMES; i++) {
        a++;
    }

    thread_join(&thread1);
    thread_join(&thread2);

    printf("The value of a is %d\n", a);
    printf("The value of b is %d\n", b);
    return 0;
}