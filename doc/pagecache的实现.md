page cache的实现和应用

# mmap和munmap

```c
void *mmap(void *addr, size_t length, int prot, int flags,
                  int fd, off_t offset);
int munmap(void *addr, size_t length);
```

在开始之前，先认真阅读linux manual，搞清楚参数的含义和要求（我就是开始没有仔细阅读，瞎搞，纠结了好久）。

`mmap`中，`addr`可以是NULL或者不是。`length`是要读取的文件的长度。`offset`是开始读文件的偏移位置，必须是page size的整数倍！



# mmap机制

![image-20220516193748184](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220516193748184.png)



# address_space



## radix-tree

叶子的slot指针，指向一个物理地址pa。



## 释放

我们使用函数`free_mapping`释放文件在内存中的映射， 包括释放`address space`结构体， `radix tree(rdt)`的节点， 已经映射的物理页 。这个函数在`__eput`中，当一个文件`entry`的引用数为0时调用。



# 在Page cache读文件

原来在fat32中读文件的方法是：

1. 调用`reade`。
2. 调用`fat_read`。需要从读取的文件的偏移和文件的开始簇，找到对应的扇区，调用底层的磁盘buffer接口。

现在我们需要一个通用的读文件的方法，从page cache中读文件。



只有文件才有`inode`结构体，因此存放文件相关的块放在page cache中。一些存放元数据的块……

# 约束写入内存的瓶颈

测试在qemu中往内存中写入10mb

1.文件大小变了之后，扩大文件访问磁盘分配簇，影响速度。这段代码没注释前测速需要100多个tick。

2.文件大小变了之后，改变其在父目录中的元数据，影响速度。这段代码没注释前测速需要39个tick。

3.把2相关代码注释掉，测速大概需要8~9个tick。

4.把do_generic_mapping_write注释掉，write函数什么也不做，需要4个ticks。

为了提高精度，接下来测试写入100mb。

# bug记录

![image-20220525165533814](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220525165533814.png)

在`freewalk`中发生了以上bug。没有释放的页正是mmap映射的页。因为`uvmfree`只负责释放`0~sz`的内存，而`mmap`映射的内存在这之上。推测是`free_mapping`未正常释放已经映射的物理页。经过排查，发现`walk_free_rdt`只是调用了`kfree`释放了物理页，没有清空相关的pte(`uvmunmap`)。

那么文件关闭，调用free_mmapping时，是否需要释放已经映射过的物理页呢？这些内存的释放是在文件关闭时，还是在进程退出时？在man中有提示：

```c
After the mmap() call has returned, the file descriptor, fd, can be closed immediately without invalidating the mapping.
```

所以推测是在进程退出时释放已经映射过的物理页，而文件关闭时只需要释放查询页的结构体（rdt）。



注意：页引用数在kfree里已经考虑到了。

物理页的引用数，是有几个虚拟内存页对其进行了引用。不过这些虚拟内存页是不是属于一个进程。只要都释放了，最后它也会释放。

## write back

## bug1

超过一个页的内容会写错：

原因：低级错误，mpage_writepages中判断相等合并页（==）写作了（=）。

## bug2

超过65个页出错（刚好是一颗rdt数的最大节点）。

在页超过了64个，要拓展extend rdt，没有设置新分配的节点dirty。
