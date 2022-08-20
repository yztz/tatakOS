riscv amo（Atomic Memory Operation）指令

# 指令介绍

![image-20220802163048735](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220802163048735.png)

amo指令主要用作多处理器同步，用来实现自旋锁，位操作。

>These AMO instructions atomically load a data value from the address in rs1, place the value into register rd, apply a binary operator to the loaded value and the original value in rs2, then store the result back  to the address in rs1. 

amo指令从rs1所指向的地址addr中加载一个数据，把数据放到rd中，并把加载的数据和rs2中的数据进行运算，把结果放到rs1指向的addr中。

# 自旋锁

自旋锁的示例如下：

发现再两个不同版本的手册中，有略微的差别。

较新的版本[2]如下：

![image-20220802165059732](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220802165059732.png)

1. 先在寄存器`t0`中放入`1`（`li t0, 1`)。
2. 然后把`a0`地址中的内容放到`t1`中，`a0`为锁的地址。
3. 比较`t1`和0`(bnez)`，如果不为0，说明锁已经被持有，回到`again`。
4. 把`t0`（1）放到`a0`表示的地址中，含义为上锁，把原`(a0)`放到`t1`中。
5. 同3，检查是否已经上锁了，如果上锁了，跳到again。
6. 执行关键区代码。
7. 解锁，把`x0`(0)，的值交给`(a0)`，把`(a0)`的值交给 `x0`（遗弃）。

较旧版本[3]如下：

![image-20220802171746321](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220802171746321.png)

更加简洁一些，不知道为什么改掉了。

# linux相关实现

## bitops.h

分析linux riscv版本的bitops.h[4]。

`set_bit`函数：

```c
static inline void __set_bit(int nr, volatile unsigned long *addr)
{
	__op_bit(or, __NOP, nr, addr);
}
```

宏展开为：

```c
__op_bit_ord(or, __NOP, nr, addr, )
```

继续展开为：

```c
__asm__ __volatile__ ( 
    amoor.d  zero, %1, %0
    : "+A" (addr[BIT_WORD(nr)]) 
    : "r" (BIT_MASK(nr)) 
    : "memory");
```

其中：

```c
/* flags数组每个元素为64bit，可以表示的flag数量为：数组大小*64 */
#define BITS_PER_LONG 64
/* nr位于的，flags数组的元素下标 */
#define BIT_WORD(nr)		((nr) / BITS_PER_LONG)
/* 那个元素的哪一位 */
#define BIT_MASK(nr)		(1UL << ((nr) % BITS_PER_LONG))
```

再看一个test_and_set_bit函数的实现：

```c
static inline int test_and_clear_bit(int nr, volatile unsigned long *addr)
{
	return __test_and_op_bit(and, __NOT, nr, addr);
}
```

展开为：

```c
__test_and_op_bit_ord(and, __NOT, nr, addr, .aqrl)
```

继续展开为：

```c
unsigned long __res, __mask; 
__mask = BIT_MASK(nr); 
__asm__ __volatile__ ( 
    amoand.d.aqrl %0, %2, %1
    : "=r" (__res), "+A" (addr[BIT_WORD(nr)]) 
    : "r" (~__mask) 
    : "memory"); 
((__res & __mask) != 0);
```

把`addr[BIT_WORD(nr)]`旧的值保存在`__res`中，然后clear bit(与 `~mask`)，然后返回旧的值与 `__mask`。

## bit spinlock的实现（简版）

```c
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
```



# 参考资料

[1] [Multithreaded Application Synchronization pt. II (Mutual Exclusion Locks a'la RISC-V)](https://www.youtube.com/watch?v=dAVZTHxC16w)

[2] The RISC-V Instruction Set Manual Volume I: Unprivileged ISA Document Version 20191213

[3] The RISC-V Instruction Set Manual Volume I: User-Level ISA Document Version 2.2

[4] [/](https://elixir.bootlin.com/linux/latest/source)[arch](https://elixir.bootlin.com/linux/latest/source/arch)/[riscv](https://elixir.bootlin.com/linux/latest/source/arch/riscv)/[include](https://elixir.bootlin.com/linux/latest/source/arch/riscv/include)/[asm](https://elixir.bootlin.com/linux/latest/source/arch/riscv/include/asm)/[bitops.h](https://elixir.bootlin.com/linux/latest/source/arch/riscv/include/asm/bitops.h)