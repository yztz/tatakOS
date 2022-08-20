注：关于本文中读取文件偏移，可以使用FatFs提供的flseek接口。

# 写在前面

这是一篇记录在xv6上移植Fatfs文件系统过程的日志，在写下这段话时，我还没有开始移植，不知道会遇到什么困难。因为不是移植成功后再回顾过程写的，做到哪里写到哪里，所以组织上会很乱。但是可以完整的记录下我遭遇和解决问题的过程。希望能给有需要的读者提供一些宝贵的经验。本人由于水平有限，难免错误，欢迎大佬指正。



# 制作文件镜像

本次移植主要在qemu上面，首先需要制作一个文件镜像`fs.img`。下面是制作fat32格式文件镜像的命令，放在一个`.sh`脚本中。先在里面预先放置一些文件。测试时使用。

```sh
#!/bin/sh
# dst = /mnt  space aside "=" is wrong!!!
dst=/mnt
# echo $dst
dd if=/dev/zero of=fs.img bs=512k count=512 
# echo $dst
mkfs.vfat -F 32 -s 4 fs.img
# echo $dst
sudo mount fs.img $dst
# echo $dst
# echo hi
cd $dst
sudo mkdir dir1
# pwd
sudo mkdir dir2
sudo mkdir dir3
sudo dash -c "echo hello world > file1"
cd dir1
sudo mkdir dir4
sudo sh -c "echo hi my bro! > file2"
cd dir4
sudo sh -c "echo yes my girl! > file3"
cd /
sudo umount $dst
```

没仔细学过shell脚本，将就着用吧。



将新制作的`fs.img`替换掉原来的，重起os，很高兴的看到如下错误：

![image-20220509105828275](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220509105828275.png)

可见是在调用`fsinit`的过程中出现了问题。

接下来我们要做的工作就是移植FATFS，让它正确的运行起来了。

# 磁盘访问函数对接

文件镜像有了，解决下来我的想法是测试一下上层的函数，比如`f_mount`，`f_read`等。先不急着改系统调用，先试一试能不能从文件镜像之中正确的读写文件。

那么接下来需要把`Media Access Interface`和qemu的虚拟磁盘接口对接。这是一个棘手的地方。比如怎么fatfs中`diskio.c`的函数`disk_read`和qemu虚拟磁盘读写函数`virtio_disk_rw`连接起来呢？这就要去研究一下了，真令人头大啊……

仔细思索一番，决定重新定义一个`disk_rw_struct`来连接接口。

```c
struct disk_rw_struct{
	const uchar *data;
	int disk;
	uint64 sectorno;
};
```



```c
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	for(int i = 0; i < count; i++){
		struct disk_rw_struct a;
		a.data = buff;
		a.sectorno = sector + i;
		virtio_disk_rw(&a, 0);
	}
	return 0;
}
```

使用下面的测试函数测试一下基本功能，放在os启动的`main`函数中。如果能够成功输出我们在上面设置的目录的话，就算是成功了。

```c
int fs_test(void)
{
    static FATFS sdcard_fs;
    FRESULT status;
    DIR dj;
    FILINFO fno;

    printf("/********************fs test*******************/\n");
    status = f_mount(&sdcard_fs, _T("0:"), 1);
    printf("mount sdcard:%d\n", status);
    if (status != FR_OK)
        return status;

    printf("printf filename\n");
    status = f_findfirst(&dj, &fno, _T("0:"), _T("*"));
    while (status == FR_OK && fno.fname[0]) {
        if (fno.fattrib & AM_DIR)
            printf("dir:%s\n", fno.fname);
        else
            printf("file:%s\n", fno.fname);
        status = f_findnext(&dj, &fno);
    }
    f_closedir(&dj);
    return 0;
}
```

然后更改好了上述接口，解决了头文件的冲突等众多问题之后，总算能够正常编译链接了，启动！运行！结果……

![image-20220509191857165](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220509191857165.png)

`scause`的值为D代表`load page fault`。通过回溯发现`f_mount`就错了，但是到底哪里有问题呢？似乎又陷入了一筹莫展的境地……

经过分析，有很多bug，一个套着一个。这里被好好折磨了一番。最终靠着队友找出了问题，有大腿抱就是好，汗颜！

首先`f_test`最终会调用到`sleep`，`yield`进程时要考虑是否初始化了进程？基于这个考虑，似乎不能放在`main`中了，我们把这个函数应该在`fork_ret`里面调用。

其次修改`BSIZE`的值，从1024改为512。这个错误和前面一个错误，经过回溯，都是在函数`virtio_disk_rw`调用`sleep`之后`panic`了。错误是`D`，但是光有这些信息，实在是很难猜测具体哪里有问题。令人头疼！

另外`virtio_disk_init`在`main`中调用正常，但是在进程的环境下，通过`disk_initialize`调用又有异常了。我猜测是因为页表的原因？`virtio_disk_init`只用初始化一次，干脆就放在`main`中。

不管如何，现在终于好了，没有panic了，但是没有按照预期的输出文件名：

![image-20220509211539337](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220509211539337.png)

直接卡在了`fs_test`之后的`for(;;);`。为什么没有输出呢？是不是`ffconf.h`中的设置问题？难道好不容易迎来了胜利的曙光……

最终发现是语言格式的问题，修改`ffconf.h`中的`FF_CODE_PAGE`为`437`，问题得到解决。可能Fatfs的作者是日本人？所以字符编码默认是日语？

![image-20220509214314524](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220509214314524.png)

最终总算是读出了，虽然文件名是都是粗暴的大写（听队友说是linux和windows的不同，如果是windows格式化镜像文件可以正常显示大小写）。

# 修改系统调用

经过昨天一天的苦战~~“划水”~~，part1总算是完成了，接下来进行part2，修改系统调用。这一个阶段的目的是修改几个系统调用之后，可以正确运行类似`ls`之类的基本的用户程序。

就先拿`open`来“开刀”。

这里岔开一下，在做的过程中，突然想到了一个问题，我能不能直接在用户态调用Fatfs提供的接口呢？比如写一个`ls.c`用户程序，正常的途径是通过`open`，`read`，`close`，`readdir`等系统调用，进入内核执行，也还是调用FatFs提供的接口，那么我为什么不直接使用Fatfs的接口写`ls.c`的程序呢？

这样做肯定是不可以的。因为用户程序二进制文件没有和kernel二进制文件链接在一起，FatFs的接口都链接在kernel中，所以用户程序直接访问这些接口肯定是访问不到的。

用户程序肯定是只能直接调用操作系统提供的接口（系统调用函数），不能直接使用操作系统内部的函数（例如FatFs的接口）。

## `sys_open`

这里我们借用内核中的`sys_open`来实现用户层上的`open`函数。

原来一开始想直接上手`openat`，结果发现要考虑的有点多，因为FatFs中file和directory是分开的两个结构体，处理起来比较麻烦。这也是一个经验，在构建复杂的程序时，先一切从简，把程序跑起来，后面可以再考虑进行优化之类的。

这里再做的时候，发现了一些问题，比如`f_open`这类函数的路径，是只能传入绝对路径吗？能不能是相对路径呢？

实验了一下，似乎是可以的，这样就好办了。

重新定义了`file`结构体：

```c
struct file{
  enum {FD_NONE, FD_FILE, FD_DIR} type;
  union obj
  {
    FIL f;
    DIR d;
  }obj; 
  int ref;
};
```

然后sys_open先利用`f_stat`找到相关路径的文件，判断其类型，再根据其类型决定调用`f_open`还是`f_opendir`。

ok了，进行到这里，新的`sys_open`就算是写好了，那么先测试一下吧。

怎么测试呢？本来想通过用户态下写一个`fstest`程序，调用`open`，然后在新的`sys_open`里面结束时打印出所有打开的文件。

这样做的话，os就要和原来的文件系统解耦了。于是，我满怀信心的注释掉了原文件系统（fs）的一些函数，取消了一些文件，潇洒的解决了编译错误。make！run！结果：

![image-20220510154629458](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220510154629458.png)

一堆链接错误！主要是因为fs相关的代码我虽然取消了，但是其他的模块，如`exec.c`，`proc.c`等，还用到了文件系统相关的代码，需要将其替换。瞬间萎了……好吧，看来步子迈的太大了。回去一点一点来……

不管怎么样，替换整个文件系统都是必须要做的，看来是逃不掉了，做好背水一战的准备！就按照上图中的错误一点一点解决！开始！见“替换文件系统”一节。

对于文件名为console的文件，在`init`打开时初始化，要特殊处理。



## `sys_write`

调用`filewrite`，一般文件调用Fatfs提供的接口。区分DIR和FIL即可，其他的细节可以参考原xv6。为了实现用户空间的`printf`（需要调用`write`系统调用），可以先把类型为设备的文件的逻辑先写了。

## `sys_read`

在没有实现时（空函数，返回0），shell程序会自动退出（进程退出），然后`init.c`里面wait的进程再重启shell。所以可以先实现设备的读取。

# 替换文件系统

## 修改`exec.c`

写到这里已经是第三天了，继续加油！

原`exec`函数中是使用`namei`获得`ip`，然后使用`readi`读取`ip`中的内容。我使用`f_open`替代`namei`，读取文件，然后使用`f_read`读取文件。但是发现`f_read`居然不支持偏移（off），难道每次读取都只能从文件开头读吗？在Load program into memory的for循环中，可见是需要off的。

看到FIL结构体中的`fprt`字段，我灵机一动，能否通过修改这个字段来达到偏移读取的效果呢？验证一下。

似乎不行……没有输出。

通过阅读`f_read`的源代码，发现只有`fptr`在sector的边界，才会读磁盘，一开始就手动给它赋一个非0的初值，会导致`f_read`跳过了

```c
if (fp->fptr % SS(fs) == 0)
```

这条判断语句，导致没有把磁盘中的内容读到`FIL`结构体的buf中，直接用`memcpy`复制其中的内容，所以没有输出。知道了这一点，我们现在需要修改`f_read`源代码。

阅读代码`f_read`的代码，记录一下细节：

1. ```c
   csect = (UINT)(fp->fptr / SS(fs) & (fs->csize - 1));
   ```

   `csize`是一个cluster包含的sector的数量。`csect`计算了当前的`fptr`位于一个cluster中的sector的号。

2. 如果`fptr`是sector对齐的，则先计算出要读的sector块号：

   ```c
   csect = (UINT)(fp->fptr / SS(fs) & (fs->csize - 1));	/* Sector offset in the cluster */
   
   sect = clst2sect(fs, fp->clust);	/* Get current sector */
   
   sect += csect;
   ```

   其中如果`csect`为0，需要重新计算`fp->clust`。

   上面`clst2sect`计算得到的sect是每个cluster开始的sector号，csect是sector在簇中的偏移号，二者相加。

   如果要读的内容（`btr`)大于一整个sector（`cc = btr / SS(fs);`)，cc大于0，则把位于当前cluster的sector直接读入目标地址，continue。

   否则读入当前的文件buf。

   退出for循环之后，把buf的值复制到目标地址。

   总的来说，这样的代码设计，没有考虑到文件不按顺序，自己指定偏移的情况。

   总结下来，我只需要补充一种情况的处理。传进来的文件，如果偏移是sector对齐的，则没有问题；如果不是对齐的，但是buf中有缓存（f->sect == sect)，也没有问题；只要处理非对齐但是又没有缓存的情况。

   我们解决的方法是在`f_read`的for循环加一段代码：如果`fptr`非sector对齐，判断其属于的sector号，如果没有缓存，则缓存。

   ​                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 

千言万语，在for循环前加上这么一段代码：

```c
/*****************************new added zyy*******************************/
				csect = (UINT)(fp->fptr / SS(fs) & (fs->csize - 1));	/* Sector offset in the cluster */
			if (csect == 0) {					/* On the cluster boundary? */
//从 ==0 改为 < 512，说明是第一个块
				if (fp->fptr < 512) {			/* On the top of the file? 小于512，说明是第一个块*/

					clst = fp->obj.sclust;		/* Follow cluster chain from the origin */
				} else {						/* Middle or end of the file */
#if FF_USE_FASTSEEK
					if (fp->cltbl) {
						clst = clmt_clust(fp, fp->fptr);	/* Get cluster# from the CLMT */
					} else
#endif
					{
						clst = get_fat(&fp->obj, fp->clust);	/* Follow cluster chain on the FAT */
					}
				}
				if (clst < 2) ABORT(fs, FR_INT_ERR);
				if (clst == 0xFFFFFFFF) ABORT(fs, FR_DISK_ERR);
				fp->clust = clst;				/* Update current cluster */
			}
			sect = clst2sect(fs, fp->clust);	/* Get current sector */
			if (sect == 0) ABORT(fs, FR_INT_ERR);
			sect += csect;


#if !FF_FS_TINY
			if (fp->sect != sect) {			/* Load data sector if not in cache */
#if !FF_FS_READONLY
				if (fp->flag & FA_DIRTY) {		/* Write-back dirty sector cache */
					if (disk_write(fs->pdrv, fp->buf, fp->sect, 1) != RES_OK) ABORT(fs, FR_DISK_ERR);
					fp->flag &= (BYTE)~FA_DIRTY;
				}
#endif
				if (disk_read(fs->pdrv, fp->buf, sect, 1) != RES_OK)	ABORT(fs, FR_DISK_ERR);	/* Fill sector cache */
			}
#endif
			fp->sect = sect;


/*****************************new added zyy*******************************/
```

其中大部分是复制的。

思来想去，总感觉似乎还是不对。cluster是按照顺序找的，最开始的时候`fp->clust`为0，是找不到的。看来简单的修改是不行了，整个`f_read`函数要重写……

先放一放吧。不修改`f_read`，遇到读取偏移的情况，先顺序读取，定位到那里吧，先不管效率了，重新封装一下`f_read`函数。

```c
FRESULT f_read_off (
	FIL* fp, 	/* Open file to be read */
	void* buff,	/* Data buffer to store the read data */
	UINT btr,	/* Number of bytes to read */
	UINT* br,	/* Number of bytes read */
    uint off   // offset 
)
{
    //reset fp
    fp->fptr = 0;
    fp->sect = 0;
    
    char temp_buf[FF_MAX_SS];
    for(int i = 0; i < off/FF_MAX_SS; i++){
        f_read(fp, temp_buf, FF_MAX_SS, br);
    }
    f_read(fp, temp_buf, off % FF_MAX_SS, br);
    return f_read(fp, buff, btr, br);

}
```

接下来就是使用读取file替代读取inode了。

## 修改`proc.c`

主要是`fork`和`exit`中的问题。涉及到`p->cwd`的代码。这里它的类型换为`file *`。当然type是DIR。

突然又意识到一个问题。使用了FatFs后，当前目录（cwd）记录在文件系统里（不是想xv6那样记录在`struct proc`中），那么多个进程如果是不同的cwd怎么办？

## 初始化文件系统

在`forkret`里面调用一个函数`fatfs_init`，这个函数调用`f_mount`。

## 重新制作文件镜像

第四天！

现在我们使用用户程序测试内核函数，需要重新制作一下文件镜像。基本思想是生成一个fat32格式的文件镜像，然后把用户程序拷贝进去。

我们先把`init.c`生成的程序复制进`fs.img`。

然后执行。结果不知道为什么，在初始化内核后，本该执行进入用户空间执行`init.c`时卡住了哪里出了bug？

```c
__attribute__((section(".startup"))) void main() {
    __attribute__((unused)) char *argv[2];
    // set arg
    // for(;;);
    argv[0] = path;
    argv[1] = 0;
    ktest();
    exec(path, argv);
    for(;;);
}
```

如我们上图的`initcode`的代码所示。`forkret`之后，执行以上代码，然后通过`exec`执行`init.c`的程序。



好吧，以上代码的exec应该不是直接执行`exec.c`中的函数，而是调用`sys_exec`系统调用。而这个系统调用我还没有完成！

修改了`sys_exec`后，成功进入了`exec.c`中的`exec`函数，这个函数把elf文件从磁盘加载到内存并执行。然而又发生了可怕的kerneltrap！

![image-20220512105904576](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220512105904576.png)

问题很简单，上一个bug是因为在`f_open`的`br`参数传入了NULL。但是修改后，又发生了新的问题：

![image-20220512111058660](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220512111058660.png)

没有正确的返回用户空间执行`init.c`，问题在哪里？是没有正确的把文件读入内存吗？

使用`objdump`打印汇编。发现应该是进入内核`usertrap`处理系统调用的时候，发生了“not from user mode”。（无论是调用printf还是open，都是应该进入内核态）。

关键原因在于不是用户态？所以不能往a7里面加载指令（系统调用）。所以`li a7 x`这条指令是非法指令？

经过和队友痛苦的排查，推测是非法指令的问题，即`init`文件读入有误。为了验证猜测，把initcode的开始两个字节的指令改为`0x00 0x00`。发生了相似的错误。但是为什么会`panic: usertrap: not from user mode`呢？这个原因是我们对于xv6的内核更改之后发生的。

![image-20220512185119714](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220512185119714.png)

最终确定是我们使用了rustsbi，在u态发生指令异常错误时，没有直接进入s态，而是进入了m态，然后再进s态，设置了spp。这就要修改sbi的`do_transfer_trap`代码，这部分我之前没有熟读指令集手册，对于riscv的中断异常代理是一窍不通，说多了都是泪：

```rust
if ctx.mstatus.mpp() == MPP::User{
        mstatus::set_spp(SPP::User);
    } else{
        mstatus::set_spp(SPP::Supervisor);
    }
```

![image-20220512200305424](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220512200305424.png)

可见没有`panic: usertrap: not from user mode`，非法指令的进程直接被killed了。这个问题算是告一段落。再次感叹，有个强大的队友真好。

接下来还是老问题，把elf文件加载到内存里失败了，还是要修好这个bug！



第五天！这几天搞得身心疲惫，但是还是要继续，这就是生活吧。

使用下列代码，打印内存中的值：

```c
uint64* temp = (uint64*)pa;
printf(green("%x"), *(uint64*)((uint64)temp + 0xaf8)); 
```

其中`0xaf8`为`init.asm`中的地址值，发现此地址中为0！所以产生了非法指令的情况！为什么是0呢？为什么？！为什么？！

把内存中pa的值打印，和`init.asm`对比：

```c
/*********************************************************/
  if(1){
    uint64* temp = (uint64*)pa;
    // printf(green("%08x"), *(uint64*)((uint64)temp + 0xb7c)); 
    for(int j = i; j < i+n; j += 4){
      printf(yellow("%x: "), j);

      printf(green("%08x\n"), *(uint64*)((uint64)temp + j)); 
    }
  }
//这里有错误，除了temp前的uint64，其他都改为uint32!这里使用了08x打印高8位，所以看上去没错？
/*********************************************************/ 
```

![image-20220513103117125](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220513103117125.png)

![image-20220513103143729](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220513103143729.png)

发现前面几条还一样，但是从188开始，指令码不一样了！后面有的正确，有的错误，为什么会有这种情况呢？现在读取的是pa，可见pa错了。那么是文件本身错了，还是文件读取到pa时出错了？

![image-20220513110813580](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220513110813580.png)

```c
/**********************直接从文件中读取，检测文件是否正确*******************/
    char buf[4];
    for(i = 0; i < ph.filesz; i+=4){
        printf(yellow("%x: "), i);
        f_read_off(&f, (void *)buf, 4, &br, ph.off+i);
        printf("%08x\n", *(uint32*)buf);
    }

    for(;;);

/**********************直接从文件中读取，检测文件是否正确*******************/
```

直接从文件中读，而不是pa，可见是正确的。我又看到了一点希望。说明是`loadseg`的问题。我开始怀疑了，难道是我的`f_read_off`设计的有问题？

这回我在`loadseg`里面改变了策略，先用`buf`从文件中读4个字节，然后复制到pa（先不考虑效率），这下子正确了。

![image-20220513115807788](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220513115807788.png)

因为open时，没有名为`console`的文件，所以`sys_open`报错panic。还是回到上一个问题，为什么`f_read_off`失败了，不解决始终是个隐患！不过由于时间问题，当务之急是先把程序跑通。

## `init.c`

在`init.c`中，使用open打开console文件，由于设备文件是特殊文件，所以我们需要在`sys_open`中特殊处理。

# 处理特殊文件

linux里“一切皆为文件“，除了普通文件，还有一些特殊的文件，像是设备文件，pipe等。由于我们移植了fatfs，这些文件就要特殊处理了，一定程度上丧失了一些“优雅”和设计上的精巧，不过这也是没有办法的事情。

## console

原来的xv6创建设备文件时，先使用mknod创建对应的inode，然后使用open打开文件。移植了fatfs后，似乎mknod没有了必要。

然后注意要实现一下`sys_dup`，不然`open`打开了文件描述符0，但是`printf`使用文件描述符1，而`sys_dup`未能复制文件描述符，会出现输出的错误。

# 最终结果

把`sh`程序复制进文件镜像里面。shell成功启动了。

![image-20220513164758658](https://raw.githubusercontent.com/DavidZyy/PicBed/master/img/image-20220513164758658.png)

接下来是写用户程序，并且继续完善系统调用的时候了。可能还是会有一些小问题，但是对于总体来说已经无关紧要了。关键是队友那边自己写的fat32已经调通了，我这边的工作就没有进行下去的必要了。如果以后有需要，再继续完善吧。

# 回顾总结

这次移植，历时五天，完全是把FatFs当做黑盒在用。除了在读取偏移值的时候仔细看了`f_read`，其他的部分基本没看。对于程序的行为不了解，用起来总是有那么一种束手束脚的感觉。比如在`loadseg`函数中读取偏移值发生的问题，至今未解。和队友交谈了一下，发现了读取失败的地址是连续的1024，512字节……似乎有那么一些规律，难道是缓存的问题？但是我已经无心再研究。

最后感谢我的队友，如果没有他的帮助，我还要花费更多的时间。











