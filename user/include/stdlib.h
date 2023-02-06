#ifndef __STDLIB_H__
#define __STDLIB_H__

void panic(char *);

#define WEXITSTATUS(s) (((s) & 0xff00) >> 8)

#ifndef assert
#define assert(f) \
    if (!(f))     \
	panic("\n --- Assert Fatal ! ---\n")
#endif

typedef void (*thread_fn_t)(void *);

typedef struct thread { 
    int tid;
    thread_fn_t entry;
    void *arg;
    void *stack_base;
    void *__tls;
} thread_t;

int thread_create(thread_t *thread, thread_fn_t fn, void *arg);
void thread_join(thread_t *thread);

#endif //__STDLIB_H__
