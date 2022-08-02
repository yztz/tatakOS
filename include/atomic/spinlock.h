#ifndef _H_SPINLOCK_
#define _H_SPINLOCK_

#include "types.h"
#include "bitops.h"

#define INIT_SPINLOCK(ln) (spinlock_t){.locked=0, .name=#ln, .cpu=NULL};

// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
  int pid;
};

typedef struct spinlock spinlock_t;

#define spin_lock(lock) acquire(lock)
#define spin_unlock(lock) release(lock)

void            acquire(struct spinlock*);
int             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            push_off(void);
void            pop_off(void);

#define DEFINE_SPINLOCK(x, name) spinlock_t x = INIT_SPINLOCK(name)



/*
 *  bit-based spin_lock()
 *
 * Don't use this unless you really need to: spin_lock() and spin_unlock()
 * are significantly faster.
 */
static inline int bit_spin_lock(int bitnum, unsigned long *addr){
  /*
	 * Assuming the lock is uncontended, this never enters
	 * the body of the outer loop. If it is contended, then
	 * within the inner loop a non-atomic test is used to
	 * busywait with less bus contention for a good time to
	 * attempt to acquire the lock bit.
   * 如果锁没被获得，则不会进入到循环内部；
   * 如果锁已经被获得了，进入循环内，一直test这个位，直到它被释放。
	 */
  while(unlikely(test_and_set_bit_lock(bitnum, addr))){
    while(test_bit(bitnum, addr))
      ;
  }
}

static inline void bit_spin_unlock(int bitnum, unsigned long *addr){
  if(!test_bit(bitnum, addr))
    ER();
  clear_bit_unlock(bitnum, addr);
}


#define pte_chain_lock(page)	bit_spin_lock(PG_chainlock, &page->flags)
#define pte_chain_unlock(page)	bit_spin_unlock(PG_chainlock, &page->flags)

#endif