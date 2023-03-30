#include "types.h"
#include "mm/vm.h"
#include "mm/mmap.h"
#include "mm/trapframe.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "atomic/spinlock.h"
#include "kernel/proc.h"
#include "kernel/sys.h"

#define __MODULE_NAME__ SYSCALL
#include "debug.h"

int fetchaddr(uint64_t addr, uint64_t *ip) {
    struct proc *p = myproc();
    if (__vma_find_strict(p->mm, addr) == NULL)
        return -1;
    if (copyin((char *)ip, addr, sizeof(*ip)) != 0)
        return -1;
    return 0;
}


int fetchstr(uint64_t addr, char *buf, int max) {
    int err = copyinstr(buf, addr, max);
    if (err < 0)
        return err;
    return strlen(buf);
}

static uint64_t argraw(int n) {
    struct proc *p = myproc();
    switch (n) {
    case 0:
        return proc_get_tf(p)->a0;
    case 1:
        return proc_get_tf(p)->a1;
    case 2:
        return proc_get_tf(p)->a2;
    case 3:
        return proc_get_tf(p)->a3;
    case 4:
        return proc_get_tf(p)->a4;
    case 5:
        return proc_get_tf(p)->a5;
    }
    panic("argraw");
    return -1;
}

int argint(int n, int *ip) {
    *ip = argraw(n);
    return 0;
}

int argaddr(int n, uint64_t *ip) {
    *ip = argraw(n);
    return 0;
}

int arglong(int n, uint64_t *ip) {
    *ip = argraw(n);
    return 0;
}

int argstr(int n, char *buf, int max) {
    uint64_t addr;
    if (argaddr(n, &addr) < 0)
        return -1;
    return fetchstr(addr, buf, max);
}

// Syscall Declaration
#define __SYS_CALL(NUM, NAME, FUNC) extern uint64_t FUNC(void);
#include "generated/syscall_gen.h"
#undef __SYS_CALL

// Syscall Table
#define __SYS_CALL(NUM, NAME, FUNC) [NUM] FUNC,
static uint64_t(*syscalls[])(void) = {
    #include "generated/syscall_gen.h"
};
#undef __SYS_CALL

// Syscall Name Map
#define __SYS_CALL(NUM, NAME, FUNC) [NUM] #NAME,
static char *__syscall_names[] = {
    #include "generated/syscall_gen.h"
};
#undef __SYS_CALL

char *syscall_name(int num) {
    return __syscall_names[num];
}

void syscall(void) {
    struct proc *p = current;
    int num = proc_get_tf(p)->a7;
    uint64_t epc = read_csr(sepc);

    if (num > 0 && num < NELEM(syscalls) && syscalls[num]) {
        uint64_t ret = syscalls[num]();
        // debug_if(p->pid >= first_user_pid, "PID %d syscall " grn("%s = %ld") " from %#lx", p->pid, syscall_name(num), ret, epc);
        proc_get_tf(p)->a0 = ret;
    } else {
        kprintf("PID %d %s: "rd("unknown sys call %d")" sepc %lx\n",
            p->pid, p->name, num, epc);
        proc_get_tf(p)->a0 = -ENOSYS;
    }
}
