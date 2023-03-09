#ifndef _ASM_RISCV_BITOPS_H
#define _ASM_RISCV_BITOPS_H

/**
 * NOTE:是否每个作为flags的变量都要设置为64bit？如果变量为int，设置bit时转为long，因为机器是小端
 * 的缘故，似乎不会有问题。
 */

/* flags数组每个元素为64bit，可以表示的flag数量为：数组大小*64 */
#define BITS_PER_LONG 64
/* nr位于的，flags数组的元素下标 */
#define BIT_WORD(nr)		((nr) / BITS_PER_LONG)
/* 那个元素的哪一位 */
#define BIT_MASK(nr)		(1UL << ((nr) % BITS_PER_LONG))


#if (BITS_PER_LONG == 64)
#define __AMO(op)	"amo" #op ".d"
#elif (BITS_PER_LONG == 32)
#define __AMO(op)	"amo" #op ".w"
#else
#error "Unexpected BITS_PER_LONG"
#endif

/**
 * 内嵌汇编，格式：
 * 1. 汇编命令模板
 * 2. 输出参数，在模板中对应的名字为 %0，%1
 * 3. 输入参数，在模板中对应的名字为 %2
 */
#define __test_and_op_bit_ord(op, mod, nr, addr, ord)		\
({								\
	unsigned long __res, __mask;				\
	__mask = BIT_MASK(nr);					\
	__asm__ __volatile__ (					\
		__AMO(op) #ord " %0, %2, %1"			\
		: "=r" (__res), "+A" (addr[BIT_WORD(nr)])	\
		: "r" (mod(__mask))				\
		: "memory");					\
	((__res & __mask) != 0);				\
})

#define __op_bit_ord(op, mod, nr, addr, ord)			\
	__asm__ __volatile__ (					\
		__AMO(op) #ord " zero, %1, %0"			\
		: "+A" (addr[BIT_WORD(nr)])			\
		: "r" (mod(BIT_MASK(nr)))			\
		: "memory");

#define __test_and_op_bit(op, mod, nr, addr) 			\
	__test_and_op_bit_ord(op, mod, nr, addr, .aqrl)
#define __op_bit(op, mod, nr, addr)				\
	__op_bit_ord(op, mod, nr, addr, )
/* 最后一个参数为空 */

/* Bitmask modifiers */
#define __NOP(x)	(x)
#define __NOT(x)	(~(x))

/**
 * test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation may be reordered on other architectures than x86.
 */
static inline int test_and_set_bit(int nr, volatile unsigned long *addr)
{
	return __test_and_op_bit(or, __NOP, nr, addr);
}

/**
 * test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 *
 * This operation can be reordered on other architectures other than x86.
 */
static inline int test_and_clear_bit(int nr, volatile unsigned long *addr)
{
	return __test_and_op_bit(and, __NOT, nr, addr);
}

/**
 * test_and_change_bit - Change a bit and return its old value
 * @nr: Bit to change
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It also implies a memory barrier.
 */
static inline int test_and_change_bit(int nr, volatile unsigned long *addr)
{
	return __test_and_op_bit(xor, __NOP, nr, addr);
}

/**
 * set_bit - Atomically set a bit in memory
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * Note: there are no guarantees that this function will not be reordered
 * on non x86 architectures, so if you are writing portable code,
 * make sure not to rely on its reordering guarantees.
 *
 * Note that @nr may be almost arbitrarily large; this function is not
 * restricted to acting on a single-word quantity.
 */
static inline void set_bit(int nr, volatile unsigned long *addr)
{
	__op_bit(or, __NOP, nr, addr);
}

/**
 * clear_bit - Clears a bit in memory
 * @nr: Bit to clear
 * @addr: Address to start counting from
 *
 * Note: there are no guarantees that this function will not be reordered
 * on non x86 architectures, so if you are writing portable code,
 * make sure not to rely on its reordering guarantees.
 */
static inline void clear_bit(int nr, volatile unsigned long *addr)
{
	__op_bit(and, __NOT, nr, addr);
}

/**
 * change_bit - Toggle a bit in memory
 * @nr: Bit to change
 * @addr: Address to start counting from
 *
 * change_bit()  may be reordered on other architectures than x86.
 * Note that @nr may be almost arbitrarily large; this function is not
 * restricted to acting on a single-word quantity.
 */
static inline void change_bit(int nr, volatile unsigned long *addr)
{
	__op_bit(xor, __NOP, nr, addr);
}

/**
 * test_and_set_bit_lock - Set a bit and return its old value, for lock
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is atomic and provides acquire barrier semantics.
 * It can be used to implement bit locks.
 */
static inline int test_and_set_bit_lock(
	unsigned long nr, volatile unsigned long *addr)
{
	return __test_and_op_bit_ord(or, __NOP, nr, addr, .aq);
}

/**
 * clear_bit_unlock - Clear a bit in memory, for unlock
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * This operation is atomic and provides release barrier semantics.
 */
static inline void clear_bit_unlock(
	unsigned long nr, volatile unsigned long *addr)
{
	__op_bit_ord(and, __NOT, nr, addr, .rl);
}

/**
 * __clear_bit_unlock - Clear a bit in memory, for unlock
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * This operation is like clear_bit_unlock, however it is not atomic.
 * It does provide release barrier semantics so it can be used to unlock
 * a bit lock, however it would only be used if no other CPU can modify
 * any bits in the memory until the lock is released (a good example is
 * if the bit lock itself protects access to the other bits in the word).
 *
 * On RISC-V systems there seems to be no benefit to taking advantage of the
 * non-atomic property here: it's a lot more instructions and we still have to
 * provide release semantics anyway.
 */
static inline void __clear_bit_unlock(
	unsigned long nr, volatile unsigned long *addr)
{
	clear_bit_unlock(nr, addr);
}


/**
 * 非架构相关，但是还是放在这里了。
 */
static inline int
test_bit(unsigned int nr, const volatile unsigned long *addr)
{
	return 1UL & (addr[BIT_WORD(nr)] >> (nr & (BITS_PER_LONG-1)));
}

/**
 * 补充函数。
 * test_bit可以不是atomic。
 * 如果addr是64位的指针，在test_bit转化为32位，会不会有问题？
 */
// static inline int
// test_bit (int nr, const volatile void *addr)
// {
// 	return 1 & (((const volatile int *) addr)[nr >> 5] >> (nr & 31));
// }


// #undef __test_and_op_bit
// #undef __op_bit
// #undef __NOP
// #undef __NOT
// #undef __AMO

// #include <asm-generic/bitops/non-atomic.h>
// #include <asm-generic/bitops/le.h>
// #include <asm-generic/bitops/ext2-atomic.h>

#endif /* _ASM_RISCV_BITOPS_H */