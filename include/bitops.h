#ifndef _BITOPS_H
#define _BITOPS_H

/*
 * WARNING: non atomic version.
 */
static inline void
__set_bit(unsigned long nr, volatile void * addr)
{
	int *m = ((int *) addr) + (nr >> 5);

	*m |= 1 << (nr & 31);
}


/*
 * WARNING: non atomic version.
 */
static inline void
__clear_bit(unsigned long nr, volatile void * addr)
{
	int *m = ((int *) addr) + (nr >> 5);

	*m &= ~(1 << (nr & 31));
}


static inline int
test_bit (int nr, const volatile void *addr)
{
	return 1 & (((const volatile int *) addr)[nr >> 5] >> (nr & 31));
}

#endif