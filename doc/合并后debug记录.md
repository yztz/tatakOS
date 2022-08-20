# 合并后debug记录

## fdopen没有输出（failed）的问题：

先检查写入的情况：

打印pa，已经写到内存中了：

![image-20220728202431342](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220728202431342.png)

最终得到是没有使用mem中的size，导致软件误以为到了文件末尾，提前退出了。

## fscanf卡住的问题

在fat_next_cluster中，发现entry的开始簇为643，但是643后一个簇为0，不对。

![image-20220729024111041](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220729024111041.png)

计算，37号扇区，12开始为0。



使用watch检查。

![image-20220729030156165](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220729030156165.png)

watch这个点：

```
watch *(int *)0x802244bc
```

![image-20220729031534544](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220729031534544.png)

原来的值为0x0fff ffff，表示下一个簇为end，这里643号簇被回收了！所以后面写回的时候为0！

![image-20220729032412976](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220729032412976.png)

解决方案：在_eput中unlink的entry不需要写回。

## feof返回不正确

![image-20220729153527184](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220729153527184.png)

文件大小(size_in_mem)没有1024，返回值却为1024（说明读了1024byte）。

最终发现是读取长度错误，不应该超过文件的大小。

## 疑似内存泄漏和entry用完

![image-20220729161235458](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220729161235458.png)

这个内存没有释放。

![image-20220729201935142](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220729201935142.png)

经过排查是改上面fscanf的bug引入的问题，unlink之后没有free_mapping。

## readahead bug

似乎是由下面这个bug搞出来的，现象是在vritio里面，读的时候指针是乱的，然后不用readahead好了很多。

最终发现是因为readahead中，先申请的内存由于内存不足被回收掉了。

![QQ20220811121030](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/QQ20220811121030.png)

优化方法是限制readahead申请的页，为当前空余内存的10%。（或者其他）

教训，如果早点这么做，那么这个bug也不会遇到。所以功能还是要完善一下，避免这个bug。

## 从pagecache中删除的bug

先是yzz buddy `remove` 内存不对的bug，然后我是`virtio`里面不对。然后关掉readahead，发现卡住。

rdt height为0的bug。

现象：从页回收算法回收一个页，从pagecache中 移除时发现了锁卡住了，猜测内存被改写。发现这个页的mapping已经被释放掉了。最终发现是在free_mapping中没有释放高度为0的情况。

```c
if(root->height == 0 && root->rnode){
    put_page((page_t *)root->rnode);
}
```



## bug？

这个bug的现象和上面的很像。

找到一种情景

见上面readahead bug。

## mmap导致的bug

![](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/QQ20220811191009.png)

`munmap`已经把`page` free了，但是页还在pagecache里面，然后页从pagecache中找出来，又释放掉了，结果报错：

![image-20220811191254027](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220811191254027.png)

之前还一直很好奇为什么报的错不是`put_page`中的`refcnt`为0，为了避免这种情况，再`get_page`中加一个判断，get之前为0则ER。



# busybox debug记录

## 功能完成



1. `touch test.txt`在文件夹下没有文件时，未能创建此文件。
   在系统调用 `sys_utimensat`中当找不到文件时创建文件，感觉不太好，不符合系统调用的功能要求，还要进行修改。

2. `echo "hello world" > test.txt`创建了文件，表面上没问题。但是执行下一步`cat`时无输出，检查文件发现写入的是11个0。发现地址很奇怪，应该是uart的地址？

   ![image-20220808015111790](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220808015111790.png)

   等于 `0xf 0000 3040` ，然后发现这个地址是无法访问的，如果按照地址打印会发生页错误（应该是没有开启`sum`位导致的）。

   进入复制的语句，打开sum位之后查看内存，发现是正确的。

   ![image-20220808023603893](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220808023603893.png)

   然后确实复制到pagecache里面了，但是挂载打开文件镜像，发现不对。

   找到问题了，是因为pagecahe现在高度为0也会存放一个页了，但是返回找dirty的函数 `lookup_tag`假设height至少为1。set dirty和clear dirty都忽略了高度为0的情况。

   重新改写了rdt之后，`cat`成功，查看内容也是对的。

3. `cat test.txt`成功。

4. `cut -c 3 test.txt`输出`l`，和linux比对正确。

5. ...

6. 一直到执行到md5sum都没问题，直到再次启动`echo`写入，然后cat出来发现结果不对。经过排查，发现每次打开文件设置 `f->off`为0，但是写入文件是从这里开始写的，就把原来的文件内容覆盖掉了。添加了一个 `O_APPEND`标志符，当这个符号设置时，把 `f->off`设置为文件末尾。

7. 继续测试，一直到`rm test.txt`都算正常。但是这个命令未能删除文件。（正常的，确定要输入y。直接回车不行）。

8. `mv test_dir test`失败，`test`开始不存在。需要先实现 `rename`系统调用。

9.  `rmdir test`发生页错误。我的锅，是在__eput，当 `nlink`小于等于0的时候，`free_mapping`前要加判断，必须是文件类型。修改后成功。

10. `cp busybox_cmd.txt busybox_cmd.bak`发生找不到文件的错误。发现是 `fstat`返回值的问题，找不到文件返回了-1，导致程序直接`exit`，应该返回`ENOENT`。
    但是继续执行发生了错误，找不到`vma`。发现是原来的做法中没有走pagecache，而是进入了一个 `send_file`系统调用， 利用了一个缓存buf（内核中），导致找不到vma。看linux strace中，似乎是调用了`read` 和 `write`，读到用户空间中，再写进去。

## 待修

1. `readahead`和`pagecache`冲突。`pagecache`的页被回收了，然后`readahead`读的页的范围较大，可能添加到`pagecache`中没被回收的页了。
2. 页`ref` 为0：
   ![image-20220809142308132](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220809142308132.png)



# lmbench

## lmdd

[LMDD(8) - LMBENCH man page](http://www.bitmover.com/lmbench/lmdd.8.html)

加了一个系统调用`fsych`。

## lat_fs

[1](https://manpages.ubuntu.com/manpages/trusty/lat_fs.8.html)

## bug1

![image-20220810202922779](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220810202922779.png)

页地址 `0x807e8000`。

# debug

## bug1

k210上卡住了 `0x1004`，打印指令，发现 `k210`上面的`pum`位没清除。

发现是`platform.h`和`riscv.h`引用顺序的缘故，导致了条件编译的失效。

## bug2

卡住的bug还在。

追踪记录，硬件bug？



## 内存泄露

使用 `rm`疑似有内存泄露。

使用`watch`命令，`watch` page的ref字段，发现是因位`lru_cache_add`的这个`ref`没有减掉！

# 调试相关

## 1 打印内存：x指令

```
x/3uh buf
表示从内存地址buf读取内容，

3表示三个单位，
u表示按十六进制显示
h表示以双字节为一个单位，

x/16cb from
从from地址打印出char
```

[gdb打印内存数据](https://blog.csdn.net/liangchen0322/article/details/50437597)

## 2 检测内存： watch指令

watch结构体变量：要加括号：

```c
 watch ((entry_t *)0x80234200)->i_mapping //对
 watch (entry_t *)0x80234200->i_mapping //错
   
```

![image-20220812105614908](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220812105614908.png)

原来的`i_mapping`已经被更改过了，为什么用的还是原来的`i_mapping`？？

## 3 调用函数： call指令



# shared_load

1. 没有跳过和当前proc一样的proc。
2. 复制的时候忘记改变量名（i->va)。
3. 共享了可写的文件导致错误！



# shared_lazy

似乎不可行，因为ph.vaddr和ph.off不能保证是页对齐的。

1. pagefault要支持instruction pagefault，改动几处。

2. entry在内存中大小大于在磁盘上的大小。

3. ```c
   ERROR INFO:
   
   file: fat32.c    func: fat_get_sectors   line: 1062
   ```

   第二段的memsz大于filesz，所以按照页错误的地址来，找到末尾页也找不到偏移。

4. ```c
   panic: sched locks
   ```

   这个处理页错误是从kerneltrap中来的。这次找bug的经验值得记录下来。

   ![image-20220816020258760](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220816020258760.png)

   最终原因是enable_sum会开中断，然后发生了页错误，然后去读，发生了错误。

5. 刚才又有了一个错误：

   ![image-20220816021339364](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220816021339364.png)

   但是改为第二段不使用lazy，居然可以启动了。使用了117页启动busybox。对比不使用lazy load，使用了594页，差距巨大！

# rmap



# 内存不足引发的冲突

当前文件正在读/写（mapping正在使用），发生了内存不足，把mapping释放掉，甚至i_mapping的地址释放掉。



# pvec引发的bug。

![image-20220815132055445](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220815132055445.png)

因为pvec不是cpu私有的，所以两个进程同时往一个pvec中添加页，导致了错误，解决方案是改为cpu私有。



# bug

引发是没有set lru，发现是两次clear lru，list从链表中删除了，却没有。

![image-20220818010954951](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220818010954951.png)

调用put_page，这样才会从lru中删除！

![image-20220818013051722](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220818013051722.png)

写回一个entry时内存不足，启动页回收的时候选择写回同一个entry。

![image-20220818014700119](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220818014700119.png)

在写回这个页所在的entry时内存不足，在回收页时这个页已经被页回收算法pageout写回了，结果回收了内存后又去写回了这个页，但是此时这个页已经被分配给了其他东西。



解决这个问题的办法很简单：只有在回收内存时才会写回磁盘；这样就避免了写回磁盘时回收内存，又触发了写回磁盘。或者写回磁盘时可以使用额外的内存？那么还要释放回来。



![image-20220818100043048](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220818100043048.png)

如上图，同一个页被添加了两次。

第一次：

![image-20220818100318632](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220818100318632.png)

第二次：

![image-20220818100454956](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220818100454956.png)

原因是这个页还没有来得及从缓存中添加到链表。



在写某个entry的时候内存不足了，然后页回收算法换出这个entry的页，结果似乎因为这个entry还处于锁定状态...

![image-20220818113006693](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220818113006693.png)



# 问题：

1. 一个进程持有某个entry的锁，然后发生了内存不足，pageout时因为`panic sched lock`无法切换。
2. 由于关闭文件（`ref=0`）时，没有直接释放掉文件的pagecache，entry还不能回收利用，后期可能造成entry数量不足。



# 最后阶段的debug记录

1. readahead只考虑了预读的页数，没有考虑文件的末尾。
   ![image-20220818171118918](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220818171118918.png)

2. 页id号搞错，如果一个页的大小为4096， 4096/4096=1，最后一个页号为1，答案是错的，算页号不能直接除以页大小，要向上对其后再除以页大小。

3. `elf` 的`magic`不对，应该是被改写了。

   ![image-20220818214317203](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220818214317203.png)

   被用户程序改写了。

   直觉告诉我是`open_exe`的锅，没想到还真是，下面是注释掉的：

   ```c
   /* 在哪里释放f？ */
   file_t *open_exe(entry_t *elf){
       struct file *f;
       // proc_t *p = myproc();
       // fdtable_t *tbl = p->fdtable;
       // int fd;
       int omode =  O_RDWR;
   
       edup(elf);
       f = filealloc();
       // if ((f = filealloc()) == 0 || (fd = fdtbl_fdalloc(tbl, f, -1, omode)) < 0) {
         // ER();
       // }
   
       f->ep = elf;
       f->readable = !(omode & O_WRONLY);
       f->writable = (omode & O_WRONLY) || (omode & O_RDWR);
       f->type = FD_ENTRY;
       f->off = 0;
       return f;
   }
   ```

   这个地方当时确实是为了尽快做掉，没有考虑仔细，唉！教训。

# map_file改map_entry

1. `sys_mmap`和`sys_munmap`改为dup/put entry。



# 记录page_state

思考：如果cow了一个shared mmap的页，然后在这个页上发生了写，现在的机制都是变成了private。

每种状态的页的数量。

1. `nr_mapped`在rmap中`page_add_rmap`中增加，`page_remove_rmap` 减少。
2. `nr_page_table_pages`：增加：`kvminit`，`__walk`，`mmap_init`；减少：`freewalk`。使用两个函数封装了起来。
3. `nr_slob`；增加：`__alloc_one_page`；减少：`__free_one_page`。
4. `nr_buddy`：注意不用在`buddy_init`中设置为total，buddy_free会增加的。增加：`buddy_free`，`buddy_free_one_page`；减少：`buddy_alloc`。
5. `nr_pagecache`；增加：`readahead`，`do_generic_mapping_read`，`do_generic_mapping_write`，`filemap_nopage`；减少：`free_mapping`，`walk_free_rdt`，`remove_put_pages_in_pagecache`，`__uvmunmap`，`shirnk_list`。
6. `nr_anonymous`；增加：`exec`,`filemap_nopage`，`do_anonymous_page`，`cow_copy`，`swap_in_page`，`do_mmap_alloc`；减少：`__uvmunmap`，`shirnk_list`。   连续多页：见下面
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
