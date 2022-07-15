#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "atomic/spinlock.h"
#include "kernel/proc.h"
#include "common.h"
#include "kernel/time.h"

#define QUIET
#define __MODULE_NAME__ SYS_PROC
#include "debug.h"
#include "mm/mm.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n << 8); // ref: WEXITSTATUS
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_getppid(void)
{
  return myproc()->parent->pid;
}

uint64
sys_fork(void)
{
  return do_clone(0);
}


uint64
sys_clone(void)
{
  int flag; // ignored
  uint64_t stack;
  // debug("clone: enter");
  if(argint(0, &flag) < 0 || argaddr(1, &stack) < 0)
    return -1;
  debug("clone: stack is %x", stack);
  return do_clone(stack);
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return waitpid(-1, p);
}


uint64
sys_wait4(void)
{
  int pid;
  uint64_t status;
  int options; // ignored

  if(argint(0, &pid) < 0 || argaddr(1, &status) || argint(2, &options) < 0)
    return -1;

  if(options > 0) 
    panic("not support");

  return waitpid(pid, status);

}

// uint64
// sys_sbrk(void)
// {
//   int addr;
//   int n;

//   if(argint(0, &n) < 0)
//     return -1;
//   addr = myproc()->sz;

//   if(n == 0)
//     return addr;
  
//   if(growproc(n - addr) < 0)
//     return -1;
    
//   return addr;
// }
uint64_t
sys_sbrk(void){
  int n;
  uint64_t newbrk, oldbrk, brk;
  mm_struct_t *mm = myproc()->mm;

  if(argint(0, &n) < 0)
    return -1;

  brk = mm->brk + n;

  /*下面和sys_brk一样*/
  /* 这个锁和vma_link的锁冲突，mm_struct_t的锁的设计还有待商榷 */
  // acquire(&mm->mm_lock);

  if(brk < mm->start_brk)
    ER();
  newbrk = PGROUNDUP(brk);
  oldbrk = PGROUNDUP(mm->brk);
  if(oldbrk == newbrk)
    goto set_brk;
  
  /* 减小堆区间 */
  if(brk <= mm->brk){
    if(!do_munmap(mm, newbrk, oldbrk - newbrk))
      goto set_brk;
    goto out; 
  }

  /* Check against existing mmap mappings. */
	if (find_vma_intersection(mm, oldbrk, newbrk+PGSIZE))
		goto out;
  
  /* Ok, looks good - let it rip. */
	if (do_brk(oldbrk, newbrk-oldbrk) != oldbrk)
		goto out;

set_brk:
  mm->brk = brk;

out:
  // release(&mm->mm_lock);
  return mm->brk;
}


/**
 * @brief modify the size of the heap directly, the addr parameter 
 * specifies the new value of current process's mm->brk.
 * 
 * @return uint64_t 
 * 
 * 用什么表示当前堆顶的位置？？mm->brk
 */
uint64_t
sys_brk(void){
  uint64_t newbrk, oldbrk, brk;
  mm_struct_t *mm = myproc()->mm;

  if(argaddr(0, &brk) < 0)
    return -1;

  // acquire(&mm->mm_lock);
  if(brk == 0)
    return mm->brk;

  if(brk < mm->start_brk)
    ER();
  newbrk = PGROUNDUP(brk);
  oldbrk = PGROUNDUP(mm->brk);
  if(oldbrk == newbrk)
    goto set_brk;
  
  /* 减小堆区间 */
  if(brk <= mm->brk){
    if(!do_munmap(mm, newbrk, oldbrk - newbrk))
      goto set_brk;
    goto out; 
  }

  /* Check against existing mmap mappings. */
	if (find_vma_intersection(mm, oldbrk, newbrk+PGSIZE))
		goto out;
  
  /* Ok, looks good - let it rip. */
	if (do_brk(oldbrk, newbrk-oldbrk) != oldbrk)
		goto out;

set_brk:
  mm->brk = brk;

out:
  // release(&mm->mm_lock);
  return mm->brk;
}

uint64
sys_nanosleep(void)
{
  timespec_t time;
  uint64_t addr;
  uint ticks0;

  if(argaddr(0, &addr) < 0)
    return -1;
  if(copy_from_user(&time, (void *)addr, sizeof(timespec_t)) == -1)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < SEC2TICK(time.tv_sec)){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_sched_yield(void)
{
  yield();
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
