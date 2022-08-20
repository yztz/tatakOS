# 修bug记录

## 1

copy_mm中，mm_init栈了8个页表，dup_mmap占了3个页表。

但是在exec中执行exit_mm时，只释放了3个页表，有内存泄漏。

最终原因是没有释放掉整个页表（调用freewalk）。



## 2

页表和内核栈冲突，释放了旧内核栈，但sp用的还是旧的内核栈，原来内核栈的页被分配给了页表，造成冲突。



问：为什么切换了新的内核栈，仍旧跑在老的内核栈上面？

下一问中，把内核栈地址寄存器sp赋值时，设置为了物理地址。在切换内核栈后，没有重新设置sp。

![image-20220716005700415](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220716005700415.png)

在usertrapret中，设置kernel_sp如上图所示，返回执行exec时，如果内核栈被设置为物理地址，那么换页表不影响，仍然使用原来的内核栈；否则换页表影响，使用地址翻译后新的内核栈。

问：内核栈是在哪里设置的？

答：在init_new_context.



问：寄存器sp里面存放的是物理地址还是虚拟地址，走不走地址翻译？



## 3

关于清零释放的页表，程序崩溃的问题。

kfree前，要释放的页表页的内容。

![image-20220715195650071](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220715195650071.png)

kfree之后的内容：

![image-20220715195755568](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220715195755568.png)

这个页已经在buddy里面了，接下来使用memset将其清零，是否对buddy产生影响？

会的，因为buddy需要这个页记录链表中的前后元素，所以清零就错了。

## 4

程序执行前后有内存泄漏，打印出buddy：

前：

![image-20220715230048309](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220715230048309.png)

后：

![image-20220715230118453](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220715230118453.png)

可以看出是8025d000被分配，但是没有释放。

经过探查，是分配到了pagecache里面，但是文件关闭没有释放，是因为这个页的引用数释放后大于0。

![image-20220715234702928](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220715234702928.png)

这里增加了引用数，但是后面却没有减去。所以kfree的时候，引用数不为0，不能还原到buddy里面。

## 5

buddy的疑惑：

在qemu中，从这个位置，按照计算是32256。

```c
#define KERN_BASE 0x80200000L
#define MEM_START (0x80000000)
#define MEM_END (MEM_START + 128*1024*1024) //0x8800 0000
```



```c
#define PAGE_NUMS ((MEM_END - KERN_BASE)/PGSIZE)
```

但是实际上少于这个数，只有32183，是被内核占用了。

<img src="https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220716173014374.png" alt="image-20220716173014374" style="zoom: 67%;" />

(0x8800 0000 - 0x8024 9000) / 0x1000 = 32183

查看kernel文件

![image-20220716175131312](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220716175131312.png)

可见它的memsize正式0x49000，和end的位置符合。（0x80249000 - 0x80200000)

## 6

测试bigwrite之后，发现还有两页内存泄漏，得到了这两页的地址，放到kmalloc里面，发现不行，其实这两页没有经过kmaloc，二十经过slob，直接从buddy里面分配的，在page的ref上面打断点可以。

最后是因为记录脏页的结构体没有释放。

## 7

偶然看了一下kernel镜像编译后的大小：

![image-20220720134817491](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220720134817491.png)

发现memsize暴增，推测是再page里面加了list_head导致的。

一共32256，每个list head为16个字节，总增加的页为32256 * 16 / 4096 = 126。k210上总共2000多个页，所以会少一些。