# 前言和背景

目前页回收系统主要考虑page cache中的页和mmaped page，anonymous page需要swap技术，先不考虑。



# 页引用数的增减



| 增加 | lru_cache_add调用get_page       | do_generic_mapping_read中找到或分配新的页。 | refill_inactive_list中遍历active list，加入l_hold时get_page  | shrink_inactive_list中添加到page_list          |
| :--- | ------------------------------- | ------------------------------------------- | ------------------------------------------------------------ | ---------------------------------------------- |
| 减少 | __pagevec_lru_add 调用 put_page | do_generic_mapping_read                     | 相同函数，加入active list和inactive list时，调用pagevec_release。 | shrink_list释放，或者同函数的pagevec_release。 |
| 增加 |                                 |                                             |                                                              |                                                |
| 减少 |                                 |                                             |                                                              |                                                |

# 页各种状态的设置和清除

所有的页flag在`rest_page`中clear。

## lock

| set   | shrink_list |      |      |
| ----- | ----------- | ---- | ---- |
| clear | shrink_list |      |      |
| test  |             |      |      |

## dirty

| set   | do_generic_mapping_write调用set_pg_rdt_dirty |      |      |
| ----- | -------------------------------------------- | ---- | ---- |
| clear | pageout                                      |      |      |
| test  | shrink_list                                  |      |      |

## writeback

| set   | rw_one_page | rw_pages          |
| ----- | ----------- | ----------------- |
| clear | rw_one_page | free_rw_page_list |
| test  | shrink_list |                   |

## direct

| set   |      |      |
| ----- | ---- | ---- |
| clear |      |      |
| test  |      |      |

# lru

| set   |      |      |
| ----- | ---- | ---- |
| clear |      |      |
|       |      |      |
|       |      |      |



# RMAP反向映射

页表映射了用户空间需要建立rmap，映射内核空间不需要，

## 内核建立map的地方

1. `userinit`中，调用`do_mmap_alloc`map initcode程序，调用`mappages`。
2. `uvmcopy`中，调用`mappages`。
3. `exec`中map 用户栈，调用`mappages`。
4. `do_anonymous_page`。发现可执行程序也走这个页，这个页回收使用swap，但其实不用，因为可执行程序由backing store。
5. `do_cow_page`调用`cow_copy`。
6. `do_filemap_page`。两处（private和shared）。注意这里`MMAP_BASE`的地址。
7. `do_swap_page`。

前三个都是调用`mappages`，后四个是通过缺页异常。

## 内核取消map的地方

1. `do_cow_page`调用`cow_copy`。
2. `mmap_free`->`unmap_vmas`-> `uvmunmap`，以及其他到 `uvmunmap` 的路径。
3. `try_to_unmap_one`

## rmap的建立和取消

对应上两项。



# lru链表

页加入活跃链表。

linux中一直“批量”处理页，似乎目的是为了避免锁的竞争？



遇到一种情况：

一个page cache中的页，添加到pagevec中，当pagevec还没有添加到list中就被释放掉了。

# 页回收的途径

## 1. 通过页回收算法

### 页不足

在buddy_alloc中，分配失败了，调用free_more_memory释放一些页，然后重新分配。



### 回收内核线程

当内存不足时，不能由当前进程回收内存，而启动内核线程来回收内存。



## 2. 程序执行过程中正常的回收



## 3.通过写回回收

在`writee`中，把dirty的entry加入全局链表。

在`writeback_single_entry`中，从全局链表上删去entry。

# 页回收算法中不同的页的回收策略





# 调试记录

## 观察一个页的refcnt增减

![image-20220731114239399](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220731114239399.png)

refcnt是page_t的第一个field，gdb使用：

```
watch *(char *)"页指针"
```

## 怎么判断一个页是否还在pagevec中？

进入pagevec.h，把PAGEVEC_SIZE设为1。

## 遗漏的点

1. 设置page->mapping和page->index。
2. setpagedirty。
3. 不能把所有的页申请完之后再free_more_memory，因为free时写回也要申请memory，所以要留一些内存供写回时的申请。

## pageout的问题

在pageout时，需要先给disk上的entry分配足够的簇，然后写回。之前通过页找sector，如果到了末尾还没有找到所需的偏移，会返回0，现在修改后直接报错。

## pageused输出不正确

buddy_alloc分配页失败（page用完了），会启动free_more_memory，在这个函数前后打印页的使用情况。

预期的情况是每次free前接近100%，因为只有页用完了才会进入这个语句，然而输出和预期不符合。

![image-20220801201522926](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220801201522926.png)

经过排查，是因为retry返回的位置不正确，因为order的值增加了，所以retry的位置应该重新计算order。

## rmap debug

在初赛的测试用例中添加系统调用。

在 `/user/lib/syscall.c`添加函数原型，在 `user/include/unistd.h`添加声明，在 `/user/lib/arch/syscall_ids.h.in`添加系统调用号。



# 回顾与总结

反思：这次做页回收，我的实现方法实在是有问题。应该步步为营，比如先把lru list调通了，然后再想回收的事情。

回收也是先只考虑page cache，然后再加mmaped page，anonymous page等等。每次先考虑最简单的，步步为营，稳扎稳打。

## entry的lru list

1. 初始化。
2. 添加到头eget。
3. 添加到尾eput。



# 记录page_state

思考：如果cow了一个shared mmap的页，然后在这个页上发生了写，现在的机制都是变成了private。

每种状态的页的数量。

1. `nr_mapped`在rmap中`page_add_rmap`中增加，`page_remove_rmap` 减少。
2. `nr_page_table_pages`：增加：`kvminit`，`__walk`，`mmap_init`；减少：`freewalk`。使用两个函数封装了起来。
3. `nr_slob`；增加：`__alloc_one_page`；减少：`__free_one_page`。
4. `nr_buddy`：注意不用在`buddy_init`中设置为total，buddy_free会增加的。增加：`buddy_free`，`buddy_free_one_page`；减少：`buddy_alloc`。
5. `nr_pagecache`；增加：`readahead`，`do_generic_mapping_read`，`do_generic_mapping_write`，`filemap_nopage`；减少：`free_mapping`，`walk_free_rdt`，`remove_put_pages_in_pagecache`，`__uvmunmap`，`shirnk_list`。
6. `nr_anonymous`；增加：`exec`,`filemap_nopage`，`do_anonymous_page`，`cow_copy`，`swap_in_page`，`do_mmap_alloc`；减少：`__uvmunmap`，`shirnk_list`。   连续多页：见下面（有些匿名页不会映射，比如proc_new中的内核栈）
7. `nr_trapframe`；增加：`tf_new`；减少：`tf_free`；
8. `nr_pipe`；增加：`pipealloc`；减少：`pipeclose`； 连续多页：见下面

## 一些特殊的情况

连续申请释放多页：

1. `proc_new` 给进程分配连续两个页的内核栈，似乎没办法进行页回收；`free_porc`。
2. `pipealloc`；`pipeclose`。



`tf_new`函数也会申请空间，但是不在上面的情况中，这里算在了。

页表要不纳入到匿名页，然后给他也换出？



第二天补充（遗漏的点）：

没有算：trapframe，pipe的页。

忘记了free_proc中内核栈释放减少匿名页。



用户栈只有一个页行吗？

# 数目不对的问题：



![image-20220819171736395](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220819171736395.png)

![](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/QQ20220819171754.png)

下图used多了一个（116），实际上多的这个在pvec中；把这个值设为1可以解决？试想一种情况：匿名页添加到。

多注意pvec！！



能不能回收掉一些进程？

# debug swap

1. `sched loack`：解决方案：注释掉。
2. 



# TODO LIST

- [x] bitlock。
- [x] lock_page，unlock_page。

- [ ] 将page的refcnt修改为atomic。

- [x] 检查remove_from_page_cache是否正确的从page cache中移除了页，检查文件镜像是否正确。
- [ ] bio的优化。提交写请求的进程是否等待写的完成（或者是否亲自接收写完的中断？）。
- [ ] 重写buffer cache？和page cache结合。
- [ ] mmaped页和anonymous页的回收。
- [ ] vma查找。
- [ ] vfs。
- [ ] 簇的分配（正向）。
- [ ] 进程链表（睡眠队列）。
- [ ] exec缺页执行程序。

  
