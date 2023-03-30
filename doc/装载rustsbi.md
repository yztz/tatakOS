# 装载RUSTSBI

由于网上关于SBI的行为描述资料较少，官方提供参考的为[riscv-sbi-doc](https://github.com/riscv-non-isa/riscv-sbi-doc)，该文档中提供的都是二进制接口以及函数功能的定义，对于sbi引导的行为仍然难以把握，因此实际参考的多为往年参赛队伍的代码逻辑。以下罗列几个主要碰到的问题。

## 特权级转换

这点是具有共识性的，在M态sbi完成任务后，将执行权给到内核，因此状态由M转移至S，换句话说，从我们的代码入口点`entry`开始，我们已经处于S态了，因此原先的xv6初始化M态代码需要进行一大部分删减。

## 传参

在一些文档中可以看到rustsbi其实是有向入口点传参的，但是一开始不太清楚，直到真正出问题的时候才回头来看：在S态下似乎无法读取mhartid寄存器。通过观察rustsbi-qemu内置的test-kernel内核可以看到rustsbi实际传入了两个参数`a0=hartid`与`a1=dtb_pa`，其中前者顾名思义，而后者主要提供的是设备树相关，我目前也不甚清楚。

## 时钟

由于时钟设置被rustsbi接管了，我们要设置时钟来触发周期性的时钟中断需要通过sbi标准函数`set_timer( s_time)`，这里主要的问题有两个：

### 1. 这个参数的意义是什么？

参数的意义是什么？参数的单位又是什么？搞清楚它们很重要。通过查看其他队伍的代码可以学到，这个函数可以设置下次超时的时间，其实就是类似于设置相关时间寄存器中的`CMP`寄存器，一般的套路为`set_timer(now + clk_freq)`，第一次设置完后，在后续的每次时钟中断处理代码中都需要重新设置，否则下次时钟中断将无法触发（或者说触发周期将发生在下次wrap后）。同时我们还需要修改诸如`trapinithart`一类函数代码，详见changelog。
    
### 2. 这个接口似乎有点问题...

为什么这么说呢？请观察如下代码:
```C
#define RESET_TIMER() \
    sbi_set_timer(*( *)CLINT_MTIME + CLOCK_FREQ)

void trapinithart(void)
{
  // 设置中断向量
  w_stvec(()kernelvec);
  // 使能中断
  w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);
  // 重置计时器
  RESET_TIMER();
}
```

这里便是1)中提到需要修改的代码之一，我们在设置完中断向量以及使能中断后即可重置定时器，这也意味着开启时钟，但是在debug过程中，发现时钟中断根本没有被触发！起初我怀疑的是我在中断处理代码中没有考虑到些情况，但是实际没有，它真的是没有触发！或许是`CLOCK_FREQ`太短了？从而导致我在设置完定时器后，实际的machine time已经wrap了...但是经过测试也不是。后来我跑去看rustsbi的仅存的两条[issue](https://github.com/rustsbi/rustsbi/issues/16)第一条就是关于timer的！采用legacy版本后，问题得以修复，但是我很好奇上届的队伍也不是用的legacy版本，但也跑得起来，我猜想这可能是作者在版本迭代过程中引入的bug吧。

<p align="right">by 杨宗振<br/>2022/3/26</p>