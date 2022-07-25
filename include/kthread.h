#ifndef _LINUX_KTHREAD_H
#define _LINUX_KTHREAD_H

#include "kernel/proc.h"
/* Simple interface for creating and stopping kernel threads without mess. */

/**
 * kthread_create: create a kthread.
 * @threadfn: the function to run until signal_pending(current).
 * @data: data ptr for @threadfn.
 * @namefmt: printf-style name for the thread.
 *
 * Description: This helper function creates and names a kernel
 * thread.  The thread will be stopped: use wake_up_process() to start
 * it.  See also kthread_run(), kthread_create_on_cpu().
 *
 * When woken, the thread will run @threadfn() with @data as its
 * argument. @threadfn can either call do_exit() directly if it is a
 * standalone thread for which noone will call kthread_stop(), or
 * return when 'kthread_should_stop()' is true (which means
 * kthread_stop() has been called).  The return value should be zero
 * or a negative error number: it will be passed to kthread_stop().
 *
 * Returns a task_struct or ERR_PTR(-ENOMEM).
 */
proc_t *kthread_create(int (*threadfn)(void *data),
				   void *data,
				   const char namefmt[]);

/**
 * kthread_run: create and wake a thread.
 * @threadfn: the function to run until signal_pending(current).
 * @data: data ptr for @threadfn.
 * @namefmt: printf-style name for the thread.
 *
 * Description: Convenient wrapper for kthread_create() followed by
 * wake_up_process().  Returns the kthread, or ERR_PTR(-ENOMEM). */
#define kthread_run(threadfn, data, namefmt)			   \
({									   \
	proc_t *__k						   \
		= kthread_create(threadfn, data, namefmt); \
	if (__k)						   \
		wake_up_process(__k);					   \
	__k;								   \
})


#endif