#include "common.h"
#include "driver/timer.h"
#include "kernel/sched.h"
#include "kernel/taskqueue.h"
#include "kernel/thread_group.h"
#include "kernel/futex.h"
#include "mm/alloc.h"
#include "mm/mmap.h"
#include "mm/trapframe.h"
#include "mm/vm.h"
#include "fs/fcntl.h"
#include "fs/stat.h"
#include "fs/fs.h"
#include "fs/file.h"

#define QUIET
#define __MODULE_NAME__ PROC
#include "debug.h"

proc_t proc[NPROC];

proc_t *initproc;

atomic_t proc_cnt = INIT_ATOMIC();

static atomic_t nextpid = INIT_ATOMIC();

#ifdef DEBUG
int first_user_pid = -1;
#endif

#define get_next_pid() (atomic_inc(&nextpid))

#define IS_MAIN_THREAD(p) ((p)->tg->master_pid==p->pid)


/*! @brief helps ensure that wakeups of wait()ing parents are not lost.
    helps obey the memory model when using p->parent.
    must be acquired before any p->lock.
*/
struct spinlock wait_lock;


static inline void procstate_init() {
    for (struct proc *p = proc; p < &proc[NPROC]; p++) {
        initlock(&p->lock, "proc");
        p->__state = UNUSED;
        p->wait_channel = NULL;
        p->mm = NULL;
        p->kstack = 0;
        p->trapframe = NULL;
        pstate_list_offer(UNUSED, p);
    }
}

// initialize the proc table at boot time.
void process_init() {
    initlock(&wait_lock, "wait_lock");
    procstate_init();
}


struct proc *myproc(void) {
    push_off();
    struct cpu *c = mycpu();
    struct proc *p = c->proc;
    struct proc *cur = current;
    pop_off();
    assert(cur == p);
    return p;
}


int get_proc_cnt() {
    return atomic_get(&proc_cnt);
}

void pstate() {
    for (struct proc *p = proc; p < &proc[NPROC]; p++) {
        if (p->state_head.next == NULL) ER();
    }
}

extern void newproc_callback_stage0();
void newproc_callback_stage1(kthread_callback_t callback) {
    proc_t *p = current;
    release(&p->lock);

    callback(p);

    panic("bad ret");
}

static uint64_t alloc_kstack() {
    void *kstack = kmalloc(KSTACK_SZ);
    if (kstack == NULL) {
        return 0;
    }

    // *(uint32_t *)kstack = KSTACK_CANARIES_COOKIE;

    return (uint64_t)kstack;
}

proc_t *proc_new(kthread_callback_t callback) {
    struct proc *p;

    pstate_list_lock(UNUSED);
    if ((p = pstate_list_poll(UNUSED)) == NULL) {
        pstate_list_unlock(UNUSED);
        return NULL;
    }
    acquire(&p->lock);
    pstate_list_unlock(UNUSED);

    p->pid = get_next_pid();
    p->killed = 0;
    p->set_tid_addr = 0;
    p->clear_tid_addr = 0;
    p->parent = NULL;
    p->futex_chan = NULL;

    INIT_LIST_HEAD(&p->thrd_head);

    if ((p->kstack = alloc_kstack()) == 0) {
        debug("kstack alloc failure");
        goto bad;
    }

    memset(&p->context, 0, sizeof(p->context));
    // We want to release the proc lock automatically, 
    // so a middle stage(newproc_callback_stage1) need to be called
    // and the real callback should be passed as a param.
    // Out of a0 is not a callee-saved register, so we do it with the help of s1.
    // An asm routine(newproc_callback_stage0 in Swtch.S) is written to help us pass the param
    // call chain:
    // (sched) -> newproc_callback_stage0 -> newproc_callback_stage1 -> callback
    //             (pass the `callback`)     (release process lock)     (finally)
    p->context.ra = (uint64_t)newproc_callback_stage0;
    p->context.s1 = (uint64_t)callback;

    // kernel stack
    p->context.sp = p->kstack + KSTACK_SZ;

    atomic_inc(&proc_cnt);

    pstate_migrate(p, USED);

    return p;


bad:
    freeproc(p);
    release(&p->lock);
    return NULL;
}

// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
void freeproc(struct proc *p) {
    fdtbl_free(&p->fdtable);
    mmap_free(&p->mm);
    tg_free(&p->tg);
    tf_free(&p->trapframe);

    if (p->kstack) kfree_safe(&p->kstack);
    if (p->exe) eput(p->exe);

    pstate_migrate(p, UNUSED);

    p->exe = NULL;
    p->pid = -1;
    p->parent = 0;
    p->name[0] = 0;
    p->chan = 0;
    p->futex_chan = 0;
    p->killed = 0;
    p->xstate = 0;

    atomic_dec(&proc_cnt);
}

static void init_std(proc_t *p) {
    struct file *stdin = filealloc();
    struct file *stdout = filealloc();

    fdtbl_setfile(p->fdtable, 0, stdin, 0);
    fdtbl_setfile(p->fdtable, 1, stdout, 0);
    fdtbl_setfile(p->fdtable, 2, stdout, 0);

    stdin->type = FD_DEVICE;
    stdout->type = FD_DEVICE;
    stdin->readable = 1;
    stdin->writable = 0;
    stdout->readable = 0;
    stdout->writable = 1;
    stdin->dev = &devs[CONSOLE];
    stdout->dev = &devs[CONSOLE];
}


static void __user0_init(proc_t *p) {
    // File system initialization must be run in the context of a
    // regular process (e.g., because it calls sleep), and thus cannot
    // be run from main().
    extern fat32_t *fat;
    debug("mount fs");
    fat_mount(ROOTDEV, &fat);
    p->cwd = namee(NULL, "/");

#ifdef DEBUG
    // save the first user pid for the debug convenience
    first_user_pid = atomic_get(&nextpid);
#endif

    // load init0
    // this procedure cannot move to `userinit` 
    // because it will trigger demand paging
    extern int exec(char *path, char *argv[], char *envp[]);
    char *argv[] = {USER0, NULL};
    char *envp[] = {NULL};
    if (exec(USER0, argv, envp) != 0) {
        panic(USER0 " load");
    }

    usertrapret();
}

// Set up first user process.
void user0_init() {
    proc_t *p;
    mm_t *mm;
    fdtable_t *fdtable;
    tg_t *tg;
    utf_t *tf;

    initproc = p = proc_new(__user0_init);

    if (p == NULL) {
        panic("alloc");
    }

    if ((mm = mmap_new()) == NULL ||
        (fdtable = fdtbl_new()) == NULL ||
        (tg = tg_new(p)) == NULL ||
        (tf = tf_new(p)) == NULL) {
        panic("alloc");
    }

    proc_switchmm(p, mm);
    proc_setfdtbl(p, fdtable);
    proc_settg(p, tg);
    proc_settf(p, tf);

    // We need not set sp, pc for her
    // because they will be set in the next stage `__user0_init:exec`

    init_std(p);

    pstate_migrate(p, RUNNABLE);

    release(&p->lock);
}

uint64_t growproc(uint64_t newbreak) {
    struct proc *p = myproc();
    if (newbreak == 0) {
        return PROGRAM_BREAK(p->mm);
    }
    if (mmap_ext_heap(p->mm, newbreak) == -1) {
        return -1;
    }

    return PROGRAM_BREAK(p->mm);
}


proc_t *kthread_create(char *name, kthread_callback_t callback) {
    proc_t *np = proc_new(callback);

    if (np == NULL)
        return NULL;

    extern mm_t *public_map;

    proc_setmm(np, public_map);
    strncpy(np->name, name, 20);
    pstate_migrate(np, RUNNABLE);

    release(&np->lock);

    return np;
}

// Pass p's abandoned children to init.
// Caller must hold wait_lock.
void reparent(struct proc *p) {
    proc_t *pp;

    for (pp = proc; pp < &proc[NPROC]; pp++) {
        if (pp->parent == p) {
            pp->parent = initproc;
            wakeup(initproc);
        }
    }
}


void exit(int status) {
    proc_t *p = current;
    int thrdcnt = tg_quit(p->tg);

    if (p == initproc) {
        panic("init exiting");
    }

    debug("PID %d EXIT %d", p->pid, status);

    if (thrdcnt == 0) {
        fdtbl_closeall(p->fdtable);
    }

    eput(p->cwd);
    p->cwd = NULL;


    acquire(&wait_lock);

    // Give any children to user0.
    reparent(p);

    if (thrdcnt == 0) {
        // Parent might be sleeping in wait().
        wakeup(p->parent);
    }

    acquire(&p->lock);

    p->xstate = status << 8; // ref: WEXITSTATUS
    pstate_migrate(p, ZOMBIE);

    release(&wait_lock);

    if (p->clear_tid_addr) {
        // clear tid addr and futex wake
extern int __futex_wake(wq_t *, void *, int, int, void *, int);

        wq_t *futex_queue = &current->tg->futex_wq;
        acquire(&futex_queue->wq_lock);
        memset_user(p->clear_tid_addr, 0, sizeof(int));
        __futex_wake(futex_queue, (void *)p->clear_tid_addr, 1, 0, 0, 0);
        release(&futex_queue->wq_lock);
    }

    // Jump into the scheduler, never to return.
    sched();
    panic("zombie exit");
}


int freechild() {
    proc_t *p = myproc();
    proc_t *np;
    int cnt = 0;
    for (np = proc; np < &proc[NPROC]; np++) {
        if (np->parent == p) {
            // Make sure the child isn't still in exit() or swtch().
            acquire(&np->lock);
            if (np->state == ZOMBIE) {
                // Found one.
                /* 进程退出的时候，有些页还在pagevec中，没有释放，看上去好像内存泄露了，所以这里加上这句。 */
                // lru_add_drain();
                freeproc(np);
                // release(&np->lock);
                cnt++;
            }
            release(&np->lock);
        }
    }
    return cnt;
}


int waitpid(int cid, uint64_t addr, int options) {
    proc_t *p = current;

    acquire(&wait_lock);

    for (;;) {
        // Scan through table looking for exited children.
        int havekids = 0; // 表示子进程存在
        int haveckid = 0; // 表示指定的cid存在
        for (proc_t *np = proc; np < &proc[NPROC]; np++) {
            if (np->parent == p) {
                // Make sure the child isn't still in exit() or swtch().
                acquire(&np->lock);
                int pid = np->pid;
                
                // Not main thread
                if (tg_pid(np->tg) != np->pid) {
                    release(&np->lock);
                    continue;
                }

                havekids = 1;

                if (cid != -1 && cid != pid) {
                    release(&np->lock);
                    continue;
                }

                haveckid = 1;
                if (np->state == ZOMBIE) {
                    // Found one.

                    if (addr != 0 && copyout(addr, (char *)&np->xstate,
                        sizeof(np->xstate)) < 0) {
                        release(&np->lock);
                        release(&wait_lock);
                        return -1;
                    }
                    freeproc(np);
                    release(&np->lock);
                    release(&wait_lock);
                    return pid;
                }
                release(&np->lock);
            }
        }

        // No point waiting if we don't have any children.
        if (!havekids || !haveckid || p->killed) {
            release(&wait_lock);
            return -1;
        }

        // Wait for a child to exit.
        sleep(p, &wait_lock);
    }
}

void proc_setmm(proc_t *p, mm_t *newmm) {
    mmap_ref(newmm);
    p->mm = newmm;
}


void proc_settg(proc_t *p, tg_t *tg) {
    tg_ref(tg);
    p->tg = tg;
}

void proc_settf(proc_t *p, utf_t *tf) {
    p->trapframe = tf;
}

void proc_setfdtbl(proc_t *p, fdtable_t *fdtbl) {
    fdtbl_ref(fdtbl);
    p->fdtable = fdtbl;
}

utf_t *proc_get_tf(proc_t *p) {
    return p->trapframe;
}


static void process_timeout(void *p) {
    proc_t *proc = (proc_t *)p;
    wake_up_process(proc);
}

int sched_timeout(int timeout) {
    assert(timeout >= 0);

    timer_t timer;
    set_timer(&timer, process_timeout, timeout, current);
    sched();
    // May not be woken because of timeout, so we need clear the timer
    remove_timer(&timer);

    return timer.expires;
}



void proc_switchmm(proc_t *p, mm_t *newmm) {
    proc_setmm(p, newmm);
    switchuvm(p->mm);
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void procdump(void) {
    static char *states[] = {
        [UNUSED]          "UNUSED        ",
        [SLEEPING]        "SLEEPING      ",
        [RUNNABLE]        "RUNNABLE      ",
        [RUNNING]         "RUNNING       ",
        [ZOMBIE]          "ZOMBIE        ",
    };
    struct proc *p;
    char *state;

    kprintf("\n");
    for (p = proc; p < &proc[NPROC]; p++) {
        if (p->state == UNUSED)
            continue;
        if (p->state >= 0 && p->state < NELEM(states) && states[p->state])
            state = states[p->state];
        else
            state = "???";
        kprintf("%d %s %s chan %#lx futex %#lx\n", p->pid, state, p->name, p->chan, p->futex_chan);
    }
}
