
#include "riscv.h"
#include "mm/vm.h"
#include "fs/stat.h"
#include "fs/fs.h"
#include "atomic/spinlock.h"
#include "atomic/sleeplock.h"
#include "fs/file.h"
#include "fs/fcntl.h"
#include "rbtree.h"
#include "common.h"
#include "list.h"
#include "kernel/proc.h"

// #define QUIET
#define __MODULE_NAME__ PDFLUSH 
#include "debug.h"


/**
 * @brief pdflush是内核中的写回线程，用来执行dirty页的写回操作。当一个进程需要写回
 * 时，可以把写回任务委托给(delegate)一个pdflush。然后这个进程就可以继续执行，避免
 * 了阻塞。由pdflush在后台（background）执行写回操作。
 * 
 */

/* 原来注释掉的定义没初始化 */
// spinlock_t pdflush_lock;
// list_head_t pdflush_list;
DEFINE_SPINLOCK(pdflush_lock, "pdflush lock");
LIST_HEAD(pdflush_list);

/* 这里定义pdflush的数量 */
#define PDFLUSH_THREADS_CNTS 1

/*
 * A structure for passing work to a pdflush thread.  Also for passing
 * state information between pdflush threads.  Protected by pdflush_lock.
 */
struct pdflush_work {
	proc_t *who;	/* The thread */
	void (*fn)(unsigned long);	/* A callback function */
	uint64_t arg0;		/* An argument to the callback */
	struct list_head list;		/* On pdflush_list, when idle */
	unsigned long when_i_went_to_sleep; /* 暂时没用到 */
};

typedef struct pdflush_work pdflush_work_t;

static int __pdflush(pdflush_work_t *my_work) {
  my_work->fn = NULL;
  my_work->who = myproc();
  INIT_LIST_HEAD(&my_work->list);

  spin_lock(&pdflush_lock);
  for( ; ; ){
    // pdflush_work_t *pdf;
    
    /* 把当前pdflush对应的work结构体移到全局链表pdflush_list上 */
    list_move(&my_work->list, &pdflush_list);
    // my_work->when_i_went_to_sleep = jiffies;
    /* 这里使用sleep，而非yield，sleep时释放锁，返回时获得锁 */
    sleep(NULL, &pdflush_lock);
    // spin_unlock(&pdflush_lock);
    // yield();

    /* my_work仍在pdflush_list上，说明没有任务（被安排了任务会从pdflush_list上取下，见
    pdflush_operation）*/
    if (!list_empty(&my_work->list)) {
			kprintf("pdflush: bogus wakeup!\n");
			my_work->fn = NULL;
			continue;
		}
		if (my_work->fn == NULL) {
			kprintf("pdflush: NULL work function\n");
			continue;
		}
		spin_unlock(&pdflush_lock);

    /* 执行委托的任务 */
    (*my_work->fn)(my_work->arg0);
    
    /* 这里可以选择是否创建新的pdflush */

    spin_lock(&pdflush_lock);
    my_work->fn = NULL;

    /* 这里决定是否销毁此pdflush */
  }
  return 0;
}

/*
 * Of course, my_work wants to be just a local in __pdflush().  It is
 * separated out in this manner to hopefully prevent the compiler from
 * performing unfortunate optimisations against the auto variables.  Because
 * these are visible to other tasks and CPUs.  (No problem has actually
 * been observed.  This is just paranoia).
 */
static void pdflush(proc_t *p)
{
  /* 注释里说my_work被设置为了一个局部变量，my_work对于每个pdflush thread是私有的，
  确实没有设置为全局变量的必要 */
	struct pdflush_work my_work;
	__pdflush(&my_work);
}


/*
 * Attempt to wake up a pdflush thread, and get it to do some work for you.
 * Returns zero if it indeed managed to find a worker thread, and passed your
 * payload to it.
 */
int pdflush_operation(void (*fn)(uint64_t), unsigned long arg0)
{
	// unsigned long flags;
	int ret = 0;

	if (fn == NULL)
    ER();

	spin_lock(&pdflush_lock);
	if (list_empty(&pdflush_list)) {
		spin_unlock(&pdflush_lock);
		/* 没有空闲的pdflush */
		ER();
		ret = -1;
	} else {
		struct pdflush_work *pdf;

		pdf = list_entry(pdflush_list.next, struct pdflush_work, list);
		list_del_init(&pdf->list);
		// if (list_empty(&pdflush_list))
			// last_empty_jifs = jiffies;
		pdf->fn = fn;
		pdf->arg0 = arg0;
		wake_up_process(pdf->who);
		spin_unlock(&pdflush_lock);
	}
	return ret;
}

static void start_one_pdflush_thread(int no)
{
	char name[20];
	snprintf(name, 20, "pdflush-%d", no);
	// kthread_run(pdflush, NULL, "pdflush");
	kthread_create(name, pdflush);
}

void pdflush_init(void)
{
	int i;

	for (i = 0; i < PDFLUSH_THREADS_CNTS; i++)
		start_one_pdflush_thread(i);
}