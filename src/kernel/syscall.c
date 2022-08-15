#include "types.h"
#include "mm/vm.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "atomic/spinlock.h"
#include "kernel/proc.h"
#include "kernel/sys.h"
#include "defs.h"

#include "debug.h"

// Fetch the uint64 at addr from the current process.
int
fetchaddr(uint64 addr, uint64 *ip)
{
  struct proc *p = myproc();
  if(__vma_find_strict(p->mm, addr) == NULL)
    return -1;
  if(copyin((char *)ip, addr, sizeof(*ip)) != 0)
    return -1;
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Returns length of string, not including nul, or -1 for error.
int
fetchstr(uint64 addr, char *buf, int max)
{
  int err = copyinstr(buf, addr, max);
  if(err < 0)
    return err;
  return strlen(buf);
}

static uint64
argraw(int n)
{
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

// Fetch the nth 32-bit system call argument.
int
argint(int n, int *ip)
{
  *ip = argraw(n);
  return 0;
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
int
argaddr(int n, uint64 *ip)
{
  *ip = argraw(n);
  return 0;
}

int
agrlong(int n, uint64 *ip)
{
  *ip = argraw(n);
  return 0;
}


// Fetch the nth word-sized system call argument as a null-terminated string.
// Copies into buf, at most max.
// Returns string length if OK (including nul), -1 if error.
int
argstr(int n, char *buf, int max)
{
  uint64 addr;
  if(argaddr(n, &addr) < 0)
    return -1;
  return fetchstr(addr, buf, max);
}

#define __SYS_CALL(NUM, NAME, FUNC) extern uint64 FUNC(void);
#include "generated/syscall_gen.h"
#undef __SYS_CALL


#define __SYS_CALL(NUM, NAME, FUNC) [NUM] FUNC,
static uint64 (*syscalls[])(void) = {
  #include "generated/syscall_gen.h"
};
#undef __SYS_CALL


#ifdef DEBUG
#define __SYS_CALL(NUM, NAME, FUNC) [NUM] #NAME,
static char *__syscall_names[] = {
  #include "generated/syscall_gen.h"
};
#undef __SYS_CALL
char *syscall_name(int num) {
  return __syscall_names[num];
}
#else
char *syscall_name(int num) {
  return "syscall name: debug required";
}
#endif

void
syscall(void)
{
  int num;
  struct proc *p = myproc();

  num = proc_get_tf(p)->a7;
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    // debug_if(p->pid >= 3, "PID %d syscall " grn("%s") " from %#lx", p->pid, syscall_name(num), r_sepc());
    proc_get_tf(p)->a0 = syscalls[num]();
  } else {
    printf("PID %d %s: "rd("unknown sys call %d")" sepc %lx\n",
            p->pid, p->name, num, r_sepc());
    proc_get_tf(p)->a0 = -ENOSYS;
    panic("");
  }
}
