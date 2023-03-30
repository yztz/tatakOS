#include "stddef.h"
#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include "syscall.h"


// typedef struct tls {
//     int tid;
// } tls_t;

#define THREAD_STACK_SIZE 4096
#define THREAD_CLONE_FLAGS (CLONE_VM | CLONE_FILES | CLONE_THREAD | CLONE_SIGHAND | CLONE_CHILD_CLEARTID | CLONE_CHILD_SETTID | CLONE_PARENT_SETTID)


// static tls_t *get_tls() {
//     uint64 x;
//     asm volatile("mv %0, tp" : "=r" (x) );
//     return (tls_t *)x;
// }


static int thread_entry(void *arg) {
    thread_t *thread = (thread_t *) arg;
    // printf("thread: %p,  ready to jump entry: %p with arg %p\n", thread, thread->entry, thread->arg);
    thread->entry(thread->arg);
    exit(0);
    return 0;
}

void thread_join(thread_t *thread) {
    int retry = 100;
    
    while (retry --> 0) {
        if (thread->tid == 0) {
            printf("join end1\n");
            return;
        }
    }
    
    syscall(SYS_futex, &thread->tid, FUTEX_WAIT, thread->tid, NULL);
    printf("join end2\n");
}

int thread_create(thread_t *thread, thread_fn_t fn, void *arg) {
    void *addr = mmap(NULL, THREAD_STACK_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS, 0, 0);
    if (addr == (void *)-1) {
        return -1;
    }
    
    thread->entry = fn;
    thread->arg = arg;
    // tls_t *tls = (tls_t *)(addr + THREAD_STACK_SIZE - sizeof(tls_t));
    // thread->__tls = tls;
    // const int size = THREAD_STACK_SIZE - sizeof(tls_t);
    int tid = clone(thread_entry, thread, addr, THREAD_STACK_SIZE, THREAD_CLONE_FLAGS, &thread->tid, NULL, &thread->tid);
    // printf("after clone entry: %p\n", thread->entry);
    if (tid < 0) {
        munmap(addr, THREAD_STACK_SIZE);
        return -1;
    }

    thread->stack_base = addr;
    // printf("after base: %p\n", thread->entry);
    return 0;
}
