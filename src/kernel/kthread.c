#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "mm/vm.h"
#include "param.h"
#include "fs/stat.h"
#include "fs/fs.h"
#include "atomic/spinlock.h"
#include "kernel/proc.h"
#include "atomic/sleeplock.h"
#include "fs/file.h"
#include "fs/fcntl.h"
#include "mm/page.h"

// #define QUIET
#define __MODULE_NAME__ KTHREAD 
#include "debug.h"

#include "mm/mm_types.h"
#include "mm/mm.h"
#include "rbtree.h"
#include "utils.h"
#include "memlayout.h"
#include "list.h"

/**
 * 内核线程只需要使用到内核空间，所以可以和初始进程共用mm_struct以节省内存。
 * 
 */

extern struct proc *initproc;


void init_new_context_kthread(proc_t *tsk, uint64_t threadfn){
  memset(&tsk->context, 0, sizeof(tsk->context));
  tsk->context.ra = (uint64_t)threadfn;
  tsk->context.sp = tsk->kstack + PGSIZE;
}
/**
 * @brief 创建内核线程，参考userinit
 * 
 */
proc_t *kthread_create(int (*threadfn)(void *data),
				   void *data,
				   const char namefmt[])
{
  proc_t *kthread;

  kthread = allocproc(1);

  /* 设置kthread的mm为initproc */
  kthread->mm = initproc->mm;

  init_new_context_kthread(kthread, (uint64_t)threadfn);

  safestrcpy(kthread->name, namefmt, sizeof(kthread->name));

  kthread->state = RUNNABLE;
  release(&kthread->lock);
  return NULL;
}