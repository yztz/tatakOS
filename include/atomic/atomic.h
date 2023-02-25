/**
 * A simple generic atmoic integer implementation
 */
#ifndef _H_ATOMIC_
#define _H_ATOMIC_


#include "atomic/spinlock.h"


typedef struct atomic_t {
    int counter;
} atomic_t;


#define atomic_add(ptr, i) __atomic_add(ptr, i)
#define atomic_inc(ptr) __atomic_add(ptr, 1)
#define atomic_dec(ptr) __atomic_add(ptr, -1)
#define atomic_get(ptr) ((ptr)->counter)

/**
 * Add i atomically, return the old val.
*/
static inline int __atomic_add(atomic_t *v, int i) {
    register int ret;						
	__asm__ __volatile__ (					
		"	amoadd.w.aqrl  %1, %2, %0"	
		: "+A" (v->counter), "=r" (ret)	
		: "r" (i)
		: "memory");
	return ret;					
}

#define INIT_ATOMIC() (atomic_t){.counter=0}
#define INIT_ATOMIC_N(N) (atomic_t){.counter=N}

#endif