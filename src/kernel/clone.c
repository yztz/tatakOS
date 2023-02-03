#include "kernel/clone.h"
#include "mm/vm.h"
#include "debug.h"

extern struct spinlock wait_lock;

int do_clone(proc_t *p, uint64_t stack, int flags, uint64_t ptid, uint64_t tls, uint64_t ctid) {
    int pid;
    struct proc *np;
    mm_t *newmm;
    fdtable_t *newfdtbl;
    signal_t *newsig;
    tg_t *newtg;
    tf_t *newtf;

    // Allocate process.
    if ((np = proc_new(forkret)) == NULL) {
        return -1;
    }

    if ((newtf = tf_new_clone(np, proc_get_tf(p))) == NULL) {
        debug("trapframe alloc failure");
        goto bad;
    }

    if ((flags & CLONE_VM)) {
        newmm = p->mm;
    } else {
        if ((newmm = mmap_clone(p->mm)) == NULL)
            goto bad;
    }

    if ((flags & CLONE_FILES)) {
        newfdtbl = p->fdtable;
    } else {
        if ((newfdtbl = fdtbl_clone(p->fdtable)) == NULL)
            goto bad;
    }

    if ((flags & CLONE_THREAD)) {
        // debug("new thread statk is %#lx", stack);
        newtg = p->tg;
        tg_join(newtg, np);
    } else {
        if ((newtg = tg_new(np)) == NULL)
            goto bad;
    }

    if ((flags & CLONE_SIGHAND)) {
        newsig = p->signal;
    } else {
        if ((newsig = sig_clone(p->signal)) == NULL)
            goto bad;
    }

    if ((flags & CLONE_CHILD_CLEARTID)) {
        np->clear_tid_addr = ctid;
    }

    if ((flags & CLONE_CHILD_SETTID)) {
        np->set_tid_addr = ctid;
    }

    if ((flags & CLONE_PARENT_SETTID)) {
        copy_to_user(ptid, &np->pid, sizeof(int));
    }

    proc_setmm(np, newmm);
    proc_setfdtbl(np, newfdtbl);
    proc_setsig(np, newsig);
    proc_settg(np, newtg);
    proc_settf(np, newtf);

    if ((flags & CLONE_SETTLS)) {
        proc_get_tf(np)->tp = tls;
    }

    // 将返回值置为0
    proc_get_tf(np)->a0 = 0;

    // 如果指定了栈，那么重设sp
    if (stack) {
        proc_get_tf(np)->sp = stack;
    }


    np->cwd = edup(p->cwd);
    if (p->exe) {
        np->exe = edup(p->exe);
    }

    safestrcpy(np->name, p->name, sizeof(p->name));

    pid = np->pid;

    release(&np->lock);

    acquire(&wait_lock);
    np->parent = p;
    release(&wait_lock);

    acquire(&np->lock);
    pstate_migrate(np, RUNNABLE);
    release(&np->lock);

    return pid;


bad:
    ER();
    debug("clone fail");
    freeproc(np);
    release(&np->lock);
    return -1;
}


void forkret(proc_t *p) {
    if (p->set_tid_addr)
        copy_to_user(p->set_tid_addr, &p->pid, 4);

    usertrapret();
}