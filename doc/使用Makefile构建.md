# 使用Makefile构建

本项目基于xv6出发构建。xv6基于UNIX v6衍生而来，作为教学用操作系统，其项目工程目录有着简单、迷你等特点，无论是从`.o`文件的生成还是最终目标文件的构建，都将被存放在原始的`kernel`以及`user`目录下。对于教学用操作系统来说，这样做无可厚非，同时可以降低同学们理解项目构建过程的门槛，更好地把握项目整体的结构，建立起整体系统的观念。

## 缺陷

然而，这样的构建不难看出充满着种种问题。一方面，各种类型文件混杂将会使得工程目录复杂化与臃肿化；另一方面，操作系统突出模块化的设计思想，若将所有代码放在同一目录下，必将导致模块代码之间难以难分，更甚者，可能存在同名冲突，无名可起的问题。此外，从开发维护的角度出发来看，这也极其不友好。

## 我的工作

### 目录结构

工程文件将被分目录存放：

```
.
├── bootloader # bootloader相关（SBI）
│   ├── rustsbi-qemu
│   └── rustsbi-k210
├── build # 构建目录
│   ├── objs # 对应与src目录结构
|   |   └── ...
│   └── user_prog # 用户程序
├── doc # 文档
├── include	# 公共头文件目录
│   └── kernel
├── script # 脚本文件
└── src # 源文件
    ├── kernel # 内核
    │   ├── asm
    │   ├── fs
    │   ├── lib
    │   ├── mm
    │   └── platform # 平台相关
    │       ├── k210
    │       │   └── include
    │       └── qemu
    │           ├── driver
    │           └── include
    └── user # 用户程序
        ├── cat
        ├── echo
        ├── forktest
        ├── grep
        ├── grind
        ├── include # 用户头文件
        ├── init
        ├── kill
        ├── lib # 用户库
        ├── ln
        ├── ls
        ├── mkdir
        ├── rm
        ├── sh
        ├── stressfs
        ├── usertests
        ├── wc
        └── zombie
```

用户程序与内核程序被分开存放，其实从宏观意义上来看，内核程序其实也可以被理解为是一种用户程序，它的构建方式与普通的用户程序构建大同小异，只多了些平台相关的内容，因此我们在构建内核与构建普通用户程序时的整体思维应该是一致的，这将在下文得到体现。

### 递归构建

Makefile被编写为递归目录构建，这意味着每个src下的子目录都将含有Makefile，虽然这样可能会导致Makefile编写上的一些*冗余*，但是极大地便于后期子目录的编译**定制化**。比如说，未来可能我想在用户目录下构建自己的一个[shell程序]([yztz/shell: a shell (github.com)](https://github.com/yztz/shell))，但是这个shell不是一个单文件的工程，可能需要许多模块的链接，我可能需要为这个项目的构建单独写一个Makefile，如果没有递归构建的策略，那么将会导致项目构建变得异常复杂！

每个文件夹下的Makefile将只会编译当前的文件夹下的源文件（不会去编译子目录或父目录中的源文件！），同时将目标`.o`文件输出到与`/path/to/proj/src`拥有相同目录结构的`/path/to/ptoj/build/objs`目录下，这样做的目的在于结构化输出文件，与源文件形成一一对应的关系，便于查看以及维护。

递归构建遵循**子目录优先构建**的原则，也就是说，子目录将总会被优先编译，此后再编译当前目录下的源文件。这样的原则利于解决上述「只编译当前目录源文件」而可能产生的依赖问题。

讲完些许概念，再聊聊我具体是如何实现的。

正如上文所说，构建内核与用户程序的整体思维应该是一致的，那到底是怎么个一致法呢？首先回忆一下一个普通的构建流程：预处理，编译，汇编，链接，最终产生出我们想要的程序，而在链接的对象是基于`.o`来进行的，再简单点说，我们其实只需要得到所有的`.o`文件（食材），再来链接一下（一锅炖），事情就算完了。想象这样一个模式：我们不断地去子目录下编译出我们需要的`.o`，直到叶子目录，然后再反向地一层层将`.o`文件「带」出来，这样在父目录我们就得到了所有的`.o`文件，最后再通过`LD`链接一下，整个构建就完成了。这里最大的一个问题就在于我们该如何把`.o`文件「带」出来呢？最简单的方式莫过于是直接遍历所有子目录找到所有的`.o`文件就好了，但是这样做我认为不够优雅，最后链接的对象过多，输出可能会非常长，甚至导致难以阅读调试！因此，这里我将采用Linux Makefile构建中的思想来做，它的原理主要基于链接器（`LD`）的`-r`选项，它的全称为`-relocatable`，官方定义它用于`partial linking`，不知到这是否已经有了思路？我们其实可以把子目录下所有的`.o`事先进行链接，产生一个局部的链接对象`built-in.o`，然后递交给父目录来继续链接！也就是说每个子目录最终将产生一个`built-in.o`文件，假设现在有3个子目录`a/`，`b/`，`c/`，那么经过子目录编译后，代表每个子目录的最终`.o`将是`a/built-in.o`，`b/built-in.o`，`c/built-in.o`，届时，我们再将当前目录下生成的所有`.o`文件与上述三者进行`partial linking`，继而产生当前目录下的`./built-in.o`文件，如此反复递归，也实现了我们的目标，同时更加优雅，简洁！

以上对构建过程进行了简单的阐述，更加详细的内容可以参考`script/Makefile.build`，`src/kernel/Makefile`。

### 杂项

内核的平台相关代码将基于给定的`platform`参数来决定编译目录，从而避免了源代码内出现大量`IFDEFINE QEMU`的宏判断语句，但是出于以后一些特殊的需要，构建参数也根据`platform`加入了`-DQEMU`或`-DK210`的编译宏选项。

此外，针对调试，构建程序将基于`debug`参数，来选择是否加入`-DDEBUG`的编译宏选项

<p align="right">by 杨宗振<br />2022/3/31</p>

## 参考文献

1. [GNU make](https://www.gnu.org/software/make/manual/make.html)
2. [跟我一起写Makefile 1.0 文档 (seisman.github.io)](https://seisman.github.io/how-to-write-makefile/overview.html)
3. [【linux】Makefile简要知识+一个通用Makefile - 李柱明 - 博客园 (cnblogs.com)](https://www.cnblogs.com/lizhuming/p/13956017.html#tid-fTkKQB)











