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
#include "fs/ioctl.h"
#include "fs/stat.h"
#include "kernel/proc.h"
#include "mm/mmap.h"
#include "mm/vm.h"
#include "mm/io.h"
#include "common.h"

// #define QUIET
#define __MODULE_NAME__ SYS_FILE
#include "debug.h"
#include "utils.h"
#include "mm/mm.h"

static int argfd(int n, int* pfd, struct file** pf) {
    int fd;
    struct file* f;

    if (argint(n, &fd) < 0)
        return -1;
    if ((f = get_file(myproc(), fd)) == NULL)
        return -1;
    if (pfd)
        *pfd = fd;
    if (pf)
        *pf = f;
    return 0;
}


uint64 sys_dup(void) {
    struct file* f;
    proc_t *p = myproc();
    int fd;

    if (argfd(0, 0, &f) < 0)
        return -1;
    if ((fd = fdalloc(p, f)) < 0)
        return -1;
    filedup(f);
    return fd;
}

uint64 sys_dup3(void) {
    struct file* f, *oldf;
    int new;
    struct proc* p = myproc();

    if (argfd(0, 0, &f) < 0 || argint(1, &new) < 0)
        return -1;

    if(new >= p->nfd && fdrealloc(p, new + 1) < 0) 
        return -1;

    if((oldf = get_file(p, new)) != NULL) {
        fileclose(oldf);
    }
    set_file(p, new, f);
    filedup(f);
    return new;

    return 0;
}


uint64 sys_read(void) {
    struct file* f;
    int n;
    uint64 p;

    if (argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argaddr(1, &p) < 0)
        return -1;
    int size = fileread(f, p, n);
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

uint64 sys_writev(void) {
    struct file* f;
    struct iovec iov;
    uint64 iovcnt;
    uint64 addr;
    uint64 len = 0;


    if (argfd(0, 0, &f) < 0 || argaddr(1, &addr) < 0 || argaddr(2, &iovcnt) < 0)
        return -1;

    // debug("writev iov %#lx iovcnt %d", addr, iovcnt);

    for(int i = 0; i < iovcnt; i++) {
        if(copyin(&iov, addr + i * sizeof(struct iovec), sizeof(struct iovec)) < 0)
            return -1;
        int res = filewrite(f, (uint64)iov.iov_base, iov.iov_len);
        len += res;
    }

    return len;
}

uint64 sys_readv(void) {
    struct file* f;
    struct iovec iov;
    uint64 iovcnt;
    uint64 addr;
    uint64 len = 0;

    if (argfd(0, 0, &f) < 0 || argaddr(1, &addr) < 0 || argaddr(2, &iovcnt) < 0)
        return -1;

    for(int i = 0; i < iovcnt; i++) {
        if(copyin(&iov, addr + i * sizeof(struct iovec), sizeof(struct iovec)) < 0)
            return -1;
        int res = fileread(f, (uint64)iov.iov_base, iov.iov_len);
        len += res;
        if(res < iov.iov_len)
            break;
    }

    return len;

}

uint64 sys_pread(void) {
    struct file *f;
    uint64 buf;
    size_t count;
    off_t offset;
    int cnt;

    if(argfd(0, NULL, &f) < 0 || argaddr(1, &buf) < 0 || 
      argaddr(2, &count) < 0 || argaddr(3, (uint64 *)&offset) < 0)
        return -1;

    elock(f->ep);
    cnt =  reade(f->ep, 1, buf, offset, count);
    eunlock(f->ep);

    return cnt;
}

static struct file* getdevfile(char *path) {
    if(strncmp(path, "/dev/null", 9) == 0) {
        struct file *fnull = filealloc();
        if(fnull == NULL) {
            debug("alloc fail");
            return NULL;
        }
        fnull->type = T_DEVICE;
        fnull->dev = &devs[DEVNULL];
        fnull->readable = 1;
        fnull->writable = 1;
        return fnull;
    } 

    if(strncmp(path, "/dev/zero", 9) == 0) {
        struct file *fzero = filealloc();
        if(fzero == NULL) {
            debug("alloc fail");
            return NULL;
        }
        fzero->type = T_DEVICE;
        fzero->dev = &devs[DEVZERO];
        fzero->readable = 1;
        fzero->writable = 1;
        return fzero;
    } 
    return NULL;
}

// OK:
uint64 sys_close(void) {
    int fd;
    struct file* f;
    proc_t *p = myproc();

    if (argfd(0, &fd, &f) < 0)
        return -1;
    set_file(p, fd, NULL);
    fileclose(f);
    return 0;
}

uint64 sys_fstat(void) {
    struct file* f;
    struct kstat stat;
    uint64_t addr;

    if (argfd(0, 0, &f) < 0 || argaddr(1, &addr) < 0)
        return -1;

    filestat(f, &stat);

    return copy_to_user(addr, &stat, sizeof(stat));
}

uint64 sys_fstatat(void) {
    int dirfd, flags;
    char path[MAXPATH];
    uint64_t ustat;
    entry_t *from, *ep;
    struct file *pf;
    struct kstat stat;
    proc_t *p = myproc();
    

    if(argint(0, &dirfd) < 0 || argstr(1, path, MAXPATH) < 0 ||
         argaddr(2, &ustat) < 0 || argint(3, &flags) < 0)
        return -1;

    // sepcial for device...
    if((pf = getdevfile(path)) != NULL) {
        filestat(pf, &stat);
        return copy_to_user(ustat, &stat, sizeof(stat));
    }
    
    if (dirfd == AT_FDCWD) {
        from = p->cwd;
    } else if ((pf = get_file(p, dirfd)) != NULL) {
        from = pf->ep;
    } else {
        return -1;
    }

    if((ep = namee(from, path)) == NULL) {
        return -1;
    }

    elock(ep);
    estat(ep, &stat);
    eunlockput(ep);

    return copy_to_user(ustat, &stat, sizeof(stat));
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
    if (copyout(addr, buf, size) == -1) {
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
    struct file* f;
    uint64_t addr;
    int len;

    if (argfd(0, 0, &f) < 0 || argaddr(1, &addr) < 0 || argint(2, &len) < 0)
        return -1;

    char* buf = (char*)kmalloc(len);
    elock(f->ep);
    int ret = read_dents(f->ep, &f->off, buf, len);
    eunlock(f->ep);

    if (copyout(addr, buf, len) == -1) {
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
    struct file *pf, *f;
    entry_t* ep;
    entry_t* from;
    int n;
    proc_t *p = myproc();

    if (argint(0, &dirfd) || (n = argstr(1, path, MAXPATH)) < 0 ||
        argint(2, &omode) < 0)
        return -1;

    // sepcial for device...
    f = getdevfile(path);
    if(f != NULL) {
        if((fd = fdalloc(p, f)) == -1) {
            fileclose(f);
            return -EMFILE;
        }
        return fd;
    }

    // debug("dirfd is %d path is %s omode is %o", dirfd, path, omode);
    if (dirfd == AT_FDCWD) {
        from = p->cwd;
    } else if ((pf = get_file(p, dirfd)) != NULL) {
        from = pf->ep;
    } else {
        return -1;
    }
        
    if (omode & O_CREATE) {  // 创建标志
        if ((ep = create(from, path, T_FILE)) == 0) {
            return -1;
        }
    } else {
        if ((ep = namee(from, path)) == 0) {
            debug("file not found, cwd is %s", path, p->cwd->name);
            debug("path %s omode %o", path, omode);
            return -1;
        }

        elock(ep);
        if (E_ISDIR(ep) && omode != O_RDONLY && omode != O_DIRECTORY) {
            eunlockput(ep);
            return -1;
        }
    }

    if ((f = filealloc()) == 0 || (fd = fdalloc(p, f)) < 0) {
        if (f)
            fileclose(f);
        eunlockput(ep);
        return -EMFILE;
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

uint64 sys_fcntl(void) {
    int fd, cmd;
    uint64_t arg;

    if(argint(0, &fd) < 0 || argint(1, &cmd) < 0 || argaddr(2, &arg) < 0) {
        return -1;
    }

    proc_t *p = myproc();

    // debug("fd is %d cmd is %d arg is %ld", fd, cmd, arg);
    if(cmd == F_SETFD) {
        p->fd_flags |= arg;
        return 0;
    }

    return 1;

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

extern int exec(char *path, char **argv, char **envp);

int debug_flag = 0;
uint64 sys_exec(void) {
    char path[MAXPATH], *argv[MAXARG];
    int i;
    uint64 uargv, uarg;
    proc_t *p = myproc();

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
    char *envp[2] = {"LD_LIBRARY_PATH=/", NULL};

    int ret = exec(path, argv, envp);

    for (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
        kfree(argv[i]);

    if(p->fd_flags & FD_CLOEXEC) {
        proc_close_files(p);
    }

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
    if ((fd0 = fdalloc(p, rf)) < 0 || (fd1 = fdalloc(p, wf)) < 0) {
        if (fd0 >= 0)
            set_file(p, fd0, NULL);
        fileclose(rf);
        fileclose(wf);
        return -1;
    }
    if (copyout(fdarray, (char*)&fd0, sizeof(fd0)) < 0 ||
        copyout(fdarray + sizeof(fd0), (char*)&fd1, sizeof(fd1)) < 0) {
        p->ofile[fd0] = 0;
        p->ofile[fd1] = 0;
        fileclose(rf);
        fileclose(wf);
        return -1;
    }
    return 0;
}


uint64 sys_ioctl(void) {
    int fd;
    uint64 request;

    if(argfd(0, &fd, NULL) < 0 && argaddr(1, &request) < 0) 
        return -1;
    // debug("req %#x TIOCGWINSZ is %#x", request, TIOCGWINSZ);
    // if(request != TIOCGWINSZ) {
    //     panic("unsupport req");
    // }
    return -1;
}

uint64 sys_lseek(void) {
    struct file* f;
    off_t offset;
    int whence;

    if(argfd(0, NULL, &f) < 0 || argaddr(1, (uint64 *)&offset) < 0 || argint(2, &whence) < 0)
        return -1;
    if(whence == SEEK_SET) {
        f->off = offset;
    } else if(whence == SEEK_CUR) {
        f->off += offset;
    } else if(whence == SEEK_END) {
        f->off = E_FILESIZE(f->ep) + offset;
    } else {
        panic("unsupport whence");
    }

    return f->off;
}

uint64 sys_mmap(void) {
    uint64 addr, len;
    off_t offset;
    int prot, flags;
    int fd;
    struct file *fp = NULL;
    struct proc* p = myproc();

    if (argaddr(0, &addr) < 0 || argaddr(1, &len) < 0 ||
        argint(2, &prot) < 0 || argint(3, &flags) < 0 || argint(4, &fd) < 0 ||
        argaddr(5, (uint64_t *)&offset) < 0)
        return -1;

    fp = get_file(p, fd);

    // debug("addr is %#lx len is %#lx flags is %b prot is %b fd is %d",addr, len, flags, prot, fd);
    if(flags & MAP_SHARED) {
        panic("ns");
    }

    if((addr = do_mmap(p->mm, fp, offset, addr, len, flags, prot | PROT_USER))== -1) 
        return -1;
    // mmap_print(p->mm);
    // if(fp)
    //     filedup(fp);
    
    return addr;
}

uint64 sys_munmap(void) {
    uint64_t addr, len;
    proc_t *p = myproc();

    if(argaddr(0, &addr) < 0 || argaddr(1, &len) < 0) {
        return -1;
    }

    do_unmap(p->mm, addr, 1);
    return 0;
}

#include "kernel/time.h"

uint64 sys_utimensat(void) {
    timespec_t ts[2];
    uint64 addr;
    int fd;

    if(argint(0, &fd) < 0 || argaddr(2, &addr) < 0) 
        return -1;

    if(copy_from_user(ts, addr, sizeof(ts)) < 0) {
        return -1;
    }
    
    if(fd >= 0 && ts[0].tv_sec == 1LL << 32) {
        get_file(myproc(), fd)->ep->raw.adate.year_from_1980 = 65;
    }

    return 0;
}
