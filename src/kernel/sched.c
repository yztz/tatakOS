// #include "kernel/proc.h"
// #include "kernel/waitqueue.h"

// static WQ_INIT(runnable_queue);

// #define lock_runnable() wq_lock(&runnable_queue)
// #define unlock_runnable() wq_release(&runnable_queue)
// #define poll_runnable() wq_poll(&runnable_queue)

// // p->lock hold
// void pstate_add_runnable(proc_t *p) {
//   assert(p->state_head.next == NULL || &p->state_head == p->state_head.next);
//   assert(p->state == USED || p->state == SLEEPING);

//   lock_runnable();
//   pstate_list_offer(RUNNABLE, p);
//   unlock_runnable();

//   p->__state = RUNNABLE;
// }

// // Per-CPU process scheduler.
// // Each CPU calls scheduler() after setting itself up.
// // Scheduler never returns.  It loops, doing:
// //  - choose a process to run.
// //  - swtch to start running that process.
// //  - eventually that process transfers control
// //    via swtch back to the scheduler.
// void scheduler(void) {
//   struct proc *p;
//   struct cpu *c = mycpu();
  
//   c->proc = 0;
//   for(;;){
//     // Avoid deadlock by ensuring that devices can interrupt.
//     intr_on();

//     // Find a runnable
//     lock_runnable();
//     if((p = poll_runnable()) == NULL) {
//       unlock_runnable();
//       continue;
//     }
//     acquire(&p->lock);
//     unlock_runnable();

//     p->__state = RUNNING;
//     c->proc = p;
//     switchuvm(p->mm);
//     swtch(&c->context, &p->context);
//     switchkvm();
//     c->proc = 0;
//     release(&p->lock);
//   }
// }