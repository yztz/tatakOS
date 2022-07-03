//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#include "atomic/sleeplock.h"
#include "atomic/spinlock.h"
#include "defs.h"
#include "fs/fcntl.h"
#include "fs/file.h"
#include "fs/fs.h"
#include "fs/stat.h"
#include "kernel/proc.h"
#include "mm/vm.h"
#include "param.h"
#include "riscv.h"
#include "types.h"

#define QUIET
#define __MODULE_NAME__ SYS_FILE
#include "debug.h"
#include "utils.h"
#include "mm/mm.h"

// void TODO(char *s, int d)
// {
//   printf(ylw("The functionality is not yet implemented!"));
//   printf(grn("file: %s\tline: %d\r\n"), s, d);
//   for(;;);
// }
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.

static struct file *get_file(int fd) {
    proc_t *p = myproc();
    if(fd < 0 || fd > p->nfd)
        return NULL;
    if(fd < NOFILE) 
        return p->ofile[fd];
    else 
        return p->ext_ofile[fd - NOFILE];
}

static void set_file(int fd, struct file *f) {
    proc_t *p = myproc();
    if(fd < 0 || fd > p->nfd)
        return;
    if(fd < NOFILE)
        p->ofile[fd] = f;
    else 
        p->ext_ofile[fd - NOFILE] = f; 
}

static int argfd(int n, int* pfd, struct file** pf) {
    int fd;
    struct file* f;

    if (argint(n, &fd) < 0)
        return -1;
    if ((f = get_file(fd)) == NULL)
        return -1;
    if (pfd)
        *pfd = fd;
    if (pf)
        *pf = f;
    return 0;
}

static int fdrealloc(int nfd) {
    if(nfd < NOFILE) 
        return -1;

    proc_t *p = myproc();
    int oldsz = max(p->nfd - NOFILE, 0) * sizeof(struct file *);
    int newsz = (nfd - NOFILE) * sizeof(struct file *);
    struct file **newfdarray;

    if((newfdarray = (struct file **)kzalloc(newsz)) == NULL) {
        debug("alloc fail");
        return -1;
    }
    memmove(newfdarray, p->ext_ofile, oldsz);
    p->nfd = nfd;
    kfree((void *)p->ext_ofile);
    p->ext_ofile = newfdarray;
    return 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static int fdalloc(struct file* f) {
    int fd;
    struct proc* p = myproc();

    for (fd = 0; fd < NOFILE; fd++) {
        if (p->ofile[fd] == 0) {
            p->ofile[fd] = f;
            return fd;
        }
    }
    for (fd = 0; fd < p->nfd - NOFILE; fd++) {
        if (p->ext_ofile[fd] == 0) {
            p->ext_ofile[fd] = f;
            return fd;
        }
    }
    
    if(fdrealloc(p->nfd + 10) < 0)
        return -1;

    return fd;
}

uint64 sys_dup(void) {
    struct file* f;
    int fd;

    if (argfd(0, 0, &f) < 0)
        return -1;
    if ((fd = fdalloc(f)) < 0)
        return -1;
    filedup(f);
    return fd;
}

uint64 sys_dup3(void) {
    struct file* f;
    int new;
    struct proc* p = myproc();

    if (argfd(0, 0, &f) < 0 || argint(1, &new) < 0)
        return -1;

    if(new >= p->nfd && fdrealloc(new + 1) < 0) 
        return -1;

    if (get_file(new) == 0) {
        set_file(new, f);
        filedup(f);
        return new;
    }

    return -1;
}

uint64 sys_read(void) {
    struct file* f;
    int n;
    uint64 p;

    if (argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argaddr(1, &p) < 0)
        return -1;
    int size = fileread(f, p, n);
    // debug("size is %d", size);
    return size;
}

uint64 sys_write(void) {
    struct file* f;
    int n;
    uint64 p;

    if (argfd(0, 0, &f) < 0 || argaddr(1, &p) < 0 || argint(2, &n) < 0)
        return -1;

    return filewrite(f, p, n);
}

// OK:
uint64 sys_close(void) {
    int fd;
    struct file* f;

    if (argfd(0, &fd, &f) < 0)
        return -1;
    set_file(fd, NULL);
    fileclose(f);
    return 0;
}

uint64 sys_fstat(void) {
    proc_t* p = myproc();
    struct file* f;
    struct kstat stat;
    entry_t* entry;
    uint64_t addr;

    if (argfd(0, 0, &f) < 0 || argaddr(1, &addr) < 0)
        return -1;

    entry = f->ep;
    elock(entry);
    estat(entry, &stat);
    eunlock(entry);

    return copyout(p->pagetable, addr, (char*)&stat, sizeof(stat));
}

uint64 sys_getcwd(void) {
    int size;
    uint64_t addr;
    char buf[MAXPATH];
    proc_t* p = myproc();

    if (argaddr(0, &addr) < 0 || argint(1, &size) < 0) {
        return -1;
    }

    char* end = getcwd(p->cwd, buf);
    assert(*end == '\0');
    // debug("%s", buf);
    if (copyout(p->pagetable, addr, buf, size) == -1) {
        return 0;
    }

    return addr;
}

uint64_t sys_mount(void) {
    debug("mount");
    return 0;
}

uint64 sys_umount(void) {
    debug("umount");
    return 0;
}

uint64 sys_unlinkat(void) {
    entry_t *entry, *from;
    int dirfd;
    char path[MAXPATH];

    // todo: flag ignored
    if (argint(0, &dirfd) < 0 || argstr(1, path, MAXPATH) < 0)
        return -1;

    if (dirfd == AT_FDCWD) {
        from = myproc()->cwd;
    } else if (dirfd < 0 || dirfd >= NOFILE || myproc()->ofile[dirfd] == NULL) {
        return -1;
    } else {
        from = myproc()->ofile[dirfd]->ep;
    }

    if ((entry = namee(from, path)) == NULL)
        return -1;
    elock(entry);
    // 仅仅是减少链接数，当引用数为0时会自动检查链接数并决定是否将其释放
    entry->nlink--;
    eunlockput(entry);
    return 0;
}

uint64 sys_getdents64(void) {
    proc_t* p = myproc();
    struct file* f;
    uint64_t addr;
    int len;

    if (argfd(0, 0, &f) < 0 || argaddr(1, &addr) < 0 || argint(2, &len) < 0)
        return -1;

    char* buf = (char*)kmalloc(len);
    elock(f->ep);
    int ret = read_dents(f->ep, &f->off, buf, len);
    eunlock(f->ep);

    if (copyout(p->pagetable, addr, buf, len) == -1) {
        kfree(buf);
        return -1;
    }
    kfree(buf);
    return ret;
}

// todo: trunc
uint64 sys_openat(void) {
    // owner mode is ignored
    char path[MAXPATH];
    int dirfd, fd, omode;
    struct file* f;
    entry_t* ep;
    entry_t* from;
    int n;
    debug("entered");
    if (argint(0, &dirfd) || (n = argstr(1, path, MAXPATH)) < 0 ||
        argint(2, &omode) < 0)
        return -1;

    debug("dirfd is %d path is %s omode is %x", dirfd, path, omode);
    if (dirfd == AT_FDCWD) {
        from = myproc()->cwd;
    } else if (dirfd < 0 || dirfd >= NOFILE || myproc()->ofile[dirfd] == NULL) {
        return -1;
    } else {
        from = myproc()->ofile[dirfd]->ep;
    }

    if (omode & O_CREATE) {  // 创建标志
        if ((ep = create(from, path, T_FILE)) == 0) {
            return -1;
        }
    } else {
        if ((ep = namee(from, path)) == 0) {
            debug("not found");
            return -1;
        }

        elock(ep);
        if (E_ISDIR(ep) && omode != O_RDONLY && omode != O_DIRECTORY) {
            eunlockput(ep);
            return -1;
        }
    }

    if ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
        if (f)
            fileclose(f);
        eunlockput(ep);
        return -1;
    }

    f->ep = ep;
    f->readable = !(omode & O_WRONLY);
    f->writable = (omode & O_WRONLY) || (omode & O_RDWR);
    f->type = FD_ENTRY;
    f->off = 0;
    // if((omode & O_TRUNC) && E_ISFILE(ep)){ // todo:
    //   etrunc(ep);
    // }

    eunlock(ep);
    return fd;
}

// OK:
uint64 sys_mkdirat(void) {
    // ignore mode
    char path[MAXPATH];
    int dirfd;
    entry_t *ep, *from;

    if (argint(0, &dirfd) < 0 || argstr(1, path, MAXPATH) < 0) {
        return -1;
    }

    if (dirfd == AT_FDCWD) {
        from = myproc()->cwd;
    } else if (dirfd < 0 || dirfd >= NOFILE || myproc()->ofile[dirfd] == NULL) {
        return -1;
    } else {
        from = myproc()->ofile[dirfd]->ep;
    }

    if ((ep = create(from, path, T_DIR)) == NULL)
        return -1;

    eunlockput(ep);

    return 0;
}

// OK:
uint64 sys_chdir(void) {
    char path[MAXPATH];
    entry_t* ep;
    struct proc* p = myproc();
    debug("enter");
    if (argstr(0, path, MAXPATH) < 0 || (ep = namee(NULL, path)) == 0) {
        return -1;
    }
    elock(ep);
    if (!E_ISDIR(ep)) {
        eunlockput(ep);
        return -1;
    }
    eunlock(ep);
    eput(p->cwd);
    p->cwd = ep;
    debug("quit") return 0;
}

uint64 sys_exec(void) {
    char path[MAXPATH], *argv[MAXARG];
    int i;
    uint64 uargv, uarg;

    if (argstr(0, path, MAXPATH) < 0 || argaddr(1, &uargv) < 0) {
        return -1;
    }

    memset(argv, 0, sizeof(argv));
    for (i = 0;; i++) {
        if (i >= NELEM(argv)) {
            goto bad;
        }
        if (fetchaddr(uargv + sizeof(uint64) * i, (uint64*)&uarg) < 0) {
            goto bad;
        }
        if (uarg == 0) {
            argv[i] = 0;
            break;
        }
        argv[i] = kalloc();
        if (argv[i] == 0)
            goto bad;
        if (fetchstr(uarg, argv[i], PGSIZE) < 0)
            goto bad;
    }

    int ret = exec(path, argv);
    for (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
        kfree(argv[i]);

    return ret;

bad:
    for (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
        kfree(argv[i]);
    return -1;
}

// OK:
uint64 sys_pipe2(void) {
    uint64 fdarray;  // user pointer to array of two integers
    struct file *rf, *wf;
    int fd0, fd1;
    struct proc* p = myproc();

    if (argaddr(0, &fdarray) < 0)
        return -1;
    if (pipealloc(&rf, &wf) < 0)
        return -1;

    fd0 = -1;
    if ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
        if (fd0 >= 0)
            p->ofile[fd0] = 0;
        fileclose(rf);
        fileclose(wf);
        return -1;
    }
    if (copyout(p->pagetable, fdarray, (char*)&fd0, sizeof(fd0)) < 0 ||
        copyout(p->pagetable, fdarray + sizeof(fd0), (char*)&fd1, sizeof(fd1)) <
            0) {
        p->ofile[fd0] = 0;
        p->ofile[fd1] = 0;
        fileclose(rf);
        fileclose(wf);
        return -1;
    }
    return 0;
}

// uint64
// sys_mmap(void)
// {
//   uint64 addr, length, offset;
//   int prot, flags, fd, i;
//   struct proc *p = myproc();

//   if (argaddr(0, &addr) < 0 || argaddr(1, &length) < 0 ||
//       argint(2, &prot) < 0 || argint(3, &flags) < 0 ||
//       argint(4, &fd) < 0 || argaddr(5, &offset) < 0)
//     return -1;


//   if ((p->ofile[fd]->writable == 0) && (prot & PROT_WRITE) && (flags & MAP_SHARED))
//     panic("sys_mmap 1");
//     // return -1;

//   uint64 old_addr = p->cur_mmap_sz;

//   // printf(rd("old_addr: %p\n"), old_addr);
//   // pte_t *pte = walk(p->pagetable, old_addr, 0);
  
//   // printf(ylw("pte: %p\n"), *pte);
//   // printf("pa: %p\n", PTE2PA(*pte));
//   // printf("V: %d\n", *pte & PTE_V);
//   // printf("U: %d\n", *pte & PTE_U);

//   //有个问题，一个文件似乎必须占据整个页，如果两个文件映射到同一个页，那么
//   //第一个触发页错误的文件会导致第二个无法触发页错误，从而第二个文件可能读写
//   //第一个文件的mmap区域。
//   p->cur_mmap_sz += PGROUNDUP(length); 
//   for (i = 0; i < VMA_NUM; i++)
//   { // lock?
//     if (p->vma[i].state == VMA_UNUSED)
//     {
//       p->vma[i].state = VMA_USED;
//       if (addr == 0)
//         p->vma[i].addr = old_addr;
//       else
//         p->vma[i].addr = addr;
//       p->vma[i].len = length;
//       p->vma[i].prot = prot;
//       p->vma[i].flags = flags;
//       p->vma[i].pgoff = offset >> PGSHIFT;
//       p->vma[i].end = (old_addr + length);

//       filedup(p->ofile[fd]);
//       p->vma[i].map_file = p->ofile[fd];

//       break;
//     }
//   }
//     if (i == VMA_NUM)
//         panic("vma is full!");

//     return old_addr;
// }

uint64
sys_mmap(void)
{
  uint64 addr, length, offset;
  int prot, flags, fd;
  struct proc *p = myproc();

  if (argaddr(0, &addr) < 0 || argaddr(1, &length) < 0 ||
      argint(2, &prot) < 0 || argint(3, &flags) < 0 ||
      argint(4, &fd) < 0 || argaddr(5, &offset) < 0)
      panic("sys_mmap 1");


  if ((p->ofile[fd]->writable == 0) && (prot & PROT_WRITE) && (flags & MAP_SHARED))
    panic("sys_mmap 2");


//   filedup(p->ofile[fd]);
  #ifdef TODO
  todo("filedup here??");
  todo("manual said file close, i think it's entry close, put the last line filedup in do_mmap or last line?");
  #endif
  return do_mmap(p->ofile[fd], addr, length, prot, 
          flags, offset);

  // return 0;
}

uint64
sys_munmap(void){
  return 0;
}

// /**
//  * @brief 一个最简单的版本的munmap，因为v->addr都是页对齐的，所以
//  * 要求va是页对齐的，并且len为PGSIZE的整数倍，或者等于区域的总长度
//  * 否则需要考虑区域的分割等问题。
//  *
//  * @return uint64
//  */
// uint64 sys_munmap(void) {
//     uint64 va, len;
//     if (argaddr(0, &va) < 0 || argaddr(1, &len) < 0)
//         return -1;

//     struct proc* p = myproc();
//     struct vma* v = 0;
//     int i;
//     pte_t* pte;

//     for (i = 0; i < VMA_NUM; i++) {
//         v = &(p->vma[i]);
//         if (v->state == VMA_USED && v->addr == va)
//             break;
//     }
//     if (i == VMA_NUM)  // ummap null
//         panic("munmap: va is not equal to one of v->addr!");
//     // return -1;

//     if (len % PGSIZE != 0 && len != v->len)
//         panic("munmap: length is not a multiple of PGSIZE or vma length!");

//     va = PGROUNDUP(va);
//     // if a virtual address has been mapped to physic address,
//     // unmap it, otherwise do noting.
//     if (va <= PGROUNDDOWN(va + len))
//         for (int a = va; a <= PGROUNDDOWN(va + len); a += PGSIZE) {
//             // printf(rd("a: %p\n"), a);
//             if ((pte = walk(p->pagetable, a, 0)) == 0)
//                 continue;
//             if ((*pte & PTE_V) == 0)
//                 continue;

//             // write back to disk
//             if (v->flags & MAP_SHARED) {
//                 // writee(v->map_file->ep, 1, a, v->off + (a - v->addr),
//                 //        min(PGSIZE, (v->end - a)));
//                 // write back page first, than write back to disk
//                 todo();
//             }
//             uvmunmap(p->pagetable, a, 1, 1);
//         }
//     // free the entire vma
//     if (va == v->addr && len == v->len) {
//         // v->state = VMA_UNUSED;
//         // fileclose(v->map_file);
//     }

//     // for a simple version of mmap, we assume it's unmap from the head
//     // of a vma, namely va == v->addr
//     // if we unmap in the middle of a vma, the vma is split into two
//     v->addr += len;
//     v->len -= len;

//     return 0;
// }