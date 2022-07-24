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

/**
 * test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 */
static inline int
test_and_clear_bit(int nr, volatile void *addr){
	int old = test_bit(nr, addr);
	__clear_bit(nr, addr);
	return old;
}

/**
 * test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 */
static inline int
test_and_set_bit(int nr, volatile void *addr){
	int old = test_bit(nr, addr);
	__set_bit(nr, addr);
	return old;
}
#endif