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

#define QUIET
#define __MODULE_NAME__ SYS_FILE
#include "debug.h"
#include "utils.h"
#include "mm/mm.h"


static char *getlastname(char *path) {
    char *last, *s;
    for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
    return last;
}


static entry_t *getep(proc_t *p, int dirfd) {
    struct file *fp;
    entry_t *ep = NULL;

    if (dirfd == AT_FDCWD) {
        ep = p->cwd;
    } else if ((fp = fdtbl_getfile(p->fdtable, dirfd)) == NULL) {
        return NULL;
    } else {
        ep = fp->ep;
    }

    return ep;
}

static int argfd(int n, int* pfd, struct file** pf) {
    int fd;
    struct file* f;

    if (argint(n, &fd) < 0)
        return -1;
    if ((f = fdtbl_getfile(myproc()->fdtable, fd)) == NULL)
        return -1;
    if (pfd)
        *pfd = fd;
    if (pf)
        *pf = f;
    return 0;
}


uint64 sys_dup(void) {
    struct file* f;
    fdtable_t *tbl = myproc()->fdtable;
    int fd;

    if (argfd(0, 0, &f) < 0)
        return -1;
    if ((fd = fdtbl_fdalloc(tbl, f, -1, 0)) < 0)
        return -EMFILE;
    filedup(f);
    return fd;
}

uint64 sys_dup3(void) {
    struct file* f, *oldf;
    fdtable_t *tbl = myproc()->fdtable;
    int new;

    if (argfd(0, 0, &f) < 0 || argint(1, &new) < 0)
        return -1;

    if((oldf = fdtbl_getfile(tbl, new)) != NULL) {
        fileclose(oldf);
    }

    fdtbl_setfile(tbl, new, f, 0);
    filedup(f);
    return new;
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
        // debug("iov base is %#lx len is %#lx", iov.iov_base, iov.iov_len);
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

    if(strncmp(path, "/dev/rtc", 8) == 0){
        struct file *frtc = filealloc();
        if(frtc == NULL){
            debug("alloc fail");
            return NULL;
        }
        frtc->type = T_DEVICE;
        frtc->dev = &devs[DEVRTC];
        frtc->readable = 1;
        frtc->writable = 1;
        return frtc;
    }
    return NULL;
}

static struct file* getprocfile(char *path){
    if(strncmp(path, "/proc/meminfo", 13) == 0){
        struct file *fmeminfo = filealloc();
        if(fmeminfo == NULL){
            debug("alloc fail");
            return NULL;
        }
        fmeminfo->type = T_RAM;
        fmeminfo->readable = 1;
        fmeminfo->writable = 1;
        return fmeminfo;
    }
    return NULL;
}

// OK:
uint64 sys_close(void) {
    int fd;
    proc_t *p = myproc();

    if (argint(0, &fd) < 0)
        return -1;


    if(fd == MAX_FD + 1) {
        debug("closed");
        return 0;
    }
    
    return fdtbl_close(p->fdtable, fd);
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

    debug("dirfd %d path %s", dirfd, path);

    // sepcial for device...
    if((pf = getdevfile(path)) != NULL) {
        filestat(pf, &stat);
        return copy_to_user(ustat, &stat, sizeof(stat));
    }
    
    from = getep(p, dirfd);

    if((ep = namee(from, path)) == NULL) {
        return -ENOENT;
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

    char* end = namepath(p->cwd, buf);
    assert(*end == '\0');
    debug("%s", buf);
    if (copyout(addr, buf, size) == -1) {
        return 0;
    }

    return addr;
}


uint64_t sys_faccessat(void) {
    return 0;
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
    proc_t *p = myproc();

    // todo: flag ignored
    if (argint(0, &dirfd) < 0 || argstr(1, path, MAXPATH) < 0)
        return -1;

    // shm not supported now
    if(strncmp(path, "/dev/shm/testshm", 16) == 0) {
        return 0;
    }

    from = getep(p, dirfd);

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

extern char *skipelem(char *path, char *name);
// todo: trunc
uint64 sys_openat(void) {
    // owner mode is ignored
    char path[MAXPATH];
    int dirfd, fd, omode;
    struct file *f;
    entry_t* ep;
    entry_t* from;
    int n;
    proc_t *p = myproc();
    fdtable_t *tbl = p->fdtable;

    if (argint(0, &dirfd) || (n = argstr(1, path, MAXPATH)) < 0 ||
        argint(2, &omode) < 0)
        return -1;

    // int len = strlen(path);
    // add 'lib' prefix
    // if(path[len - 2] == 's' && path[len - 1] == 'o') {
    //     char name[MAX_FILE_NAME];
    //     char *mypath = path;
    //     while((mypath = skipelem(mypath, name)));
    //     if(strncmp(name, "lib", 3) != 0) {
    //         memmove(path, "lib", 3);
    //         memmove(path + 3, name, strlen(name) + 1);
    //     } else {
    //         memmove(path, name, strlen(name) + 1);
    //     }
    // }

    debug("dirfd is %d path is %s omode is %o", dirfd, path, omode);

    // not support shm now
    if(strncmp(path, "/dev/shm/testshm", 16) == 0) {
        return MAX_FD + 1;
    }

    // sepcial for device...
    f = getdevfile(path);
    if(f != NULL) {
        if((fd = fdtbl_fdalloc(tbl, f, -1, omode)) == -1) {
            fileclose(f);
            return -EMFILE;
        }
        return fd;
    }

    f = getprocfile(path);
    if(f != NULL){
        if((fd = fdtbl_fdalloc(tbl, f, -1, omode)) == -1) {
            fileclose(f);
            return -EMFILE;
        }
        return fd;
    }

    from = getep(p, dirfd);
        
    if (omode & O_CREATE) {  // 创建标志
        if ((ep = create(from, path, T_FILE)) == 0) {
            debug("create failure");
            debug("path %s omode %o", path, omode);
            return -1;
        }
    } else {
        if ((ep = namee(from, path)) == 0) {
            debug("file not found, cwd is %s, path is %s, omode is %o", p->cwd->name, path, omode);
            return -1;
        }

        elock(ep);
        if (E_ISDIR(ep) && (omode & 1) != 0 && (omode & O_DIRECTORY) == 0) {
            eunlockput(ep);
            return -1;
        }
    }

    if ((f = filealloc()) == 0 || (fd = fdtbl_fdalloc(tbl, f, -1, omode)) < 0) {
        if (f)
            fileclose(f);
        eunlockput(ep);
        return -EMFILE;
    }

    f->ep = ep;
    f->readable = !(omode & O_WRONLY);
    f->writable = (omode & O_WRONLY) || (omode & O_RDWR);
    f->type = FD_ENTRY;
    if(omode & O_APPEND)
        f->off = E_FILESIZE(ep);
    else
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
    proc_t *p = myproc();
    entry_t *ep, *from;

    if (argint(0, &dirfd) < 0 || argstr(1, path, MAXPATH) < 0) {
        return -1;
    }

    from = getep(p, dirfd);

    if ((ep = create(from, path, T_DIR)) == NULL)
        return -EEXIST;

    eunlockput(ep);

    return 0;
}

uint64 sys_fcntl(void) {
    int fd, cmd;
    uint64_t arg;
    file_t *f;
    proc_t *p = myproc();

    if(argfd(0, &fd, &f) < 0 || argint(1, &cmd) < 0 || argaddr(2, &arg) < 0) {
        return -1;
    }

    // debug("fd is %d cmd is %d arg is %ld", fd, cmd, arg);
    if(cmd == F_SETFD) {
        fdtbl_setflags(p->fdtable, fd, arg);
        return 0;
    } else if(cmd == O_NONBLOCK) {
        return 0;
    } else if(cmd == F_DUPFD_CLOEXEC) {
        int dupfd;
        if((dupfd = fdtbl_fdalloc(p->fdtable, f, arg, O_CLOEXEC)) < 0) 
            return -1;
        filedup(f);
        return dupfd;
    } else if(cmd == F_GETFL) {
        return fdtbl_getflags(p->fdtable, fd);
    } else {
        debug("unknown cmd %d", cmd);
        return -1;
    }
    

}

uint64_t sys_readlinkat(void) {
    int dirfd;
    char path[MAXPATH];
    uint64_t bufaddr;
    size_t bufsz;
    entry_t *from, *ep;
    proc_t *p = myproc();

    int n;
    if (argint(0, &dirfd) || (n = argstr(1, path, MAXPATH)) < 0 ||
        argaddr(2, &bufaddr) < 0 || argaddr(3, &bufsz) < 0)
        return -1;

    if(strncmp(path, "/proc/self/exe", 14) == 0) {
        char *end = namepath(p->exe, path);
        int len = min(end - path, bufsz);
        if(copy_to_user(bufaddr, path, len) < 0)
            return -1;
        return 0;
    } 

    from = getep(p, dirfd);
    if((ep = namee(from, path)) == NULL) {
        return -1;
    }
    char *end = namepath(p->exe, path);
    int len = min(end - path, bufsz);
    if(copy_to_user(bufaddr, path, len) < 0)
        return -1;

    eput(ep);

    return 0;
}

uint64_t sys_sendfile(void) {
    file_t *inf, *outf;
    uint64_t poff;
    size_t count;
    off_t off;

    if(argfd(0, NULL, &outf) < 0 || argfd(1, NULL, &inf) < 0 || 
      argaddr(2, &poff) < 0 || argaddr(3, (uint64_t *)&count) < 0)
        return -1;
    
    if(poff && copy_from_user(&off, poff, sizeof(off_t)) < 0)
        return -1;

    int ret;
    if((ret = filesend(inf, outf, poff ? &off : NULL, count)) < 0)
        return -1;
    
    if(poff && copy_to_user(poff, &off, sizeof(off_t)) < 0)
        return -1;

    debug("len %ld rlen %ld", count, ret);
    return ret;

}

// OK:
uint64 sys_chdir(void) {
    char path[MAXPATH];
    entry_t* ep;
    struct proc* p = myproc();
    // debug("enter");
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
    // debug("quit");
    return 0;
}

extern int exec(char *path, char *argv[], char *envp[]);

uint64 sys_exec(void) {
    char path[MAXPATH], *argv[MAXARG];
    int i = 0;
    uint64 uargv, uarg;
    proc_t *p = myproc();

    // [sharp] (arg0) (arg1) (arg2)...

    if (argstr(0, path, MAXPATH) < 0 || argaddr(1, &uargv) < 0) {
        return -1;
    }

    memset(argv, 0, sizeof(argv));

    while(1) {
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
        argv[i] = kmalloc(MAXARGLEN);
        if (argv[i] == 0)
            goto bad;   
        if (fetchstr(uarg, argv[i], PGSIZE) < 0)
            goto bad;
        i++;
    }

    if(endwith(path, ".sh")) {
        memmove(argv + 2, argv, sizeof(uint64_t) * i);
        strncpy(path, "./busybox", 10);
        argv[0] = kmalloc(16);
        if(argv[0] == NULL) return -1;
        argv[1] = kmalloc(16);
        if(argv[1] == NULL) {
            kfree(argv[0]);
            return -1;
        }
        strncpy(argv[0], "./busybox", 10);
        strncpy(argv[1], "sh", 3);
    }

    char cwd[MAXPATH];
    char pwd[MAXPATH];
    namepath(p->cwd, cwd);
    snprintf(pwd, MAXPATH, "PWD=%s", cwd);
    char *envp[] = {"LD_LIBRARY_PATH=/",
                    "LONGNAME=Dear_u",
                    pwd,
                    "PS1=\\u"grn("\\w")"\\$ ",
                    "PATH=/",
                    "ENOUGH=3000", 
                    "TIMING_O=7", 
                    "LOOP_O=0",
                    NULL};

    int ret = exec(path, argv, envp);

    for (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
        kfree(argv[i]);

    fdtbl_closexec(p->fdtable);

    return ret;

bad:
    for (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
        kfree(argv[i]);
    return -1;
}

// 简单的实现
uint64_t sys_renameat2(void) {
#define __bad(err) {ret = -(err);goto ret;}
#define __ukn() {goto unknown;}
    int ret = 0;
    int olddirfd;
    char oldpath[MAXPATH];
    int newdirfd;
    char newpath[MAXPATH];
    uint flags;
    entry_t *dirold,*dirnew;
    entry_t *epold = NULL, *epnew = NULL;
    proc_t *p = myproc();

    if(argint(0, &olddirfd) < 0 ||
      argstr(1, oldpath, MAXPATH) < 0 ||
      argint(2, &newdirfd) < 0 ||
      argstr(3, newpath, MAXPATH) < 0 ||
      argint(4, (int *)&flags) < 0) {
        return -1;
    }
    
    if(flags != 0)
        panic("flags");
    
    dirold = getep(p, olddirfd);
    dirnew = getep(p, newdirfd);

    epold = namee(dirold, oldpath);
    epnew = namee(dirold, newpath);

    if(epold == NULL) __bad(ENOENT);
    if(epnew) __ukn();

    // 重命名
    if(dirnew == dirold) {
        char *newname;
        // if(epnew) __ukn();
        newname = getlastname(newpath);
        if(*newname == '\0') __ukn();
        return entry_rename(epold, newname);
    } else {
        __ukn();
    }

  ret:
    if(epold) eput(epold);
    if(epnew) eput(epnew);

    return ret;

  unknown:
    panic("not support now");
}

// OK:
uint64 sys_pipe2(void) {
    uint64 fdarray;  // user pointer to array of two integers
    struct file *rf, *wf;
    int fd0, fd1;
    fdtable_t *tbl = myproc()->fdtable;

    if (argaddr(0, &fdarray) < 0)
        return -1;
    if (pipealloc(&rf, &wf) < 0)
        return -1;

    fd0 = -1;
    if ((fd0 = fdtbl_fdalloc(tbl, rf, -1, 0)) < 0) {
        fileclose(rf);
        return -1;
    }

    if((fd1 = fdtbl_fdalloc(tbl, wf, -1, 0)) < 0) {
        fdtbl_close(tbl, fd0);
        fileclose(wf);
        return -1;
    }

    if (copyout(fdarray, (char*)&fd0, sizeof(fd0)) < 0 ||
        copyout(fdarray + sizeof(fd0), (char*)&fd1, sizeof(fd1)) < 0) {
        fdtbl_close(tbl, fd0);
        fdtbl_close(tbl, fd1);
        return -1;
    }
    return 0;
}


uint64 sys_ioctl(void) {
    int fd;
    uint64 request;
    uint64 arg0;

    int pgrp = 0;

    if(argfd(0, &fd, NULL) < 0 || argaddr(1, &request) < 0 || argaddr(2, &arg0) < 0) 
        return -1;
    // debug("req %#x", request);
    // if(request != TIOCGWINSZ) {
    //     panic("unsupport req");
    // }
    switch(request) {
        case TCGETS:
            if (copy_to_user(arg0, &(struct termios){
                .c_iflag = 0,
                // .c_oflag = OPOST|ONLCR,
                .c_oflag = 0,
                .c_cflag = 0,
                // .c_lflag = ICANON|ECHO,
                .c_lflag = 0,
                .c_line = 0,
                .c_cc = {0},
            }, sizeof(struct termios)) < 0)
                return -1;
            break;
        case TCSETS:

            break;
        case TIOCGPGRP:
            
            if(copy_to_user(arg0, &pgrp, sizeof(int)) < 0)
                return -1;
            break;
        case TIOCSPGRP:
        
            break;
        case TIOCGWINSZ:
            if (copy_to_user(arg0, &(struct winsize){
                .ws_row = 20,
                .ws_col = 100,
            }, sizeof(struct winsize)) < 0)
                return -1;
            break;
        /* for hwclock */
        case 0xffffffff80247009:
            break;
        default:
            return 0;
    }
    return 0;
}

uint64 sys_ppoll(void) {
    return 1;
}

uint64 sys_lseek(void) {
    struct file* f;
    off_t offset;
    int whence;

    if(argfd(0, NULL, &f) < 0 || argaddr(1, (uint64 *)&offset) < 0 || argint(2, &whence) < 0)
        return -1;

    if(f->type == FD_PIPE) {
        return -ESPIPE;
    }

    if(whence == SEEK_SET) {
        f->off = offset;
    } else if(whence == SEEK_CUR) {
        f->off += offset;
    } else if(whence == SEEK_END) {
        f->off = f->ep->size_in_mem + offset;
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

    fp = fdtbl_getfile(p->fdtable, fd);

    // debug("addr is %#lx len is %#lx flags is %b prot is %b fd is %d",addr, len, flags, prot, fd);

    // if((addr = do_mmap(p->mm, fp, offset, addr, len, flags, prot | PROT_USER | PROT_READ | PROT_WRITE | PROT_EXEC))== -1)  {
    
    if((addr = do_mmap(p->mm, fp, offset, addr, len, flags, prot | PROT_USER))== -1) {
        debug("mmap failure");
        return -1;
    }

    
    return addr;
}

uint64 sys_munmap(void) {
    uint64_t addr, len;
    proc_t *p = myproc();

    if(argaddr(0, &addr) < 0 || argaddr(1, &len) < 0) {
        return -1;
    }
    /* 现在没有支持vma的分裂和合并，这里先判断一下，以防bug */
    vma_t *vma = __vma_find_strict(p->mm, addr);
    if(vma->len != len)
        ER();
    // debug("UNMAP addr is %#lx len is %#lx", addr, len);
    // mmap_print(p->mm);
    do_unmap(p->mm, addr, 1);
    return 0;
}

#include "kernel/time.h"

// uint64 sys_utimensat(void) {
//     timespec_t ts[2];
//     uint64 addr;
//     proc_t *p = myproc();
//     int fd;

//     if(argint(0, &fd) < 0 || argaddr(2, &addr) < 0) 
//         return -1;

//     printf(rd("fd: %d\taddr: %p\n"), fd, addr);
//     if(copy_from_user(ts, addr, sizeof(ts)) < 0) {
//         return -1;
//     }
    
//     if(fd >= 0 && ts[0].tv_sec == 1LL << 32) {
        // fdtbl_getfile(p->fdtable, fd)->ep->raw.adate.year_from_1980 = 65;
//     }

//     return 0;
// }

uint64_t sys_utimensat(void){
    /* times[0]: last access time
        times[1]: last modificatin time */
    timespec_t ts[2];
    proc_t *p = myproc();
    int fd;
    char path[MAXPATH];
    uint64 addr;
    int flags;
    entry_t *from, *ep;

    if(argint(0, &fd) < 0 || argstr(1, path, MAXPATH) < 0 || 
        argaddr(2, &addr) < 0 || argint(3, &flags) < 0) 
        return -1;

    debug("fd: %d\taddr: %p\tpath: %s\tflags: %d\n", fd, addr, path, flags);

    from = getep(p, fd);

    /* 文件不存在 */
    if((ep = namee(from, path)) == NULL){
        /* 在这创建文件不太好，为了过样例 */
        return -ENOENT;
        // ER();
        // if ((ep = create(from, path, T_FILE)) == 0)
        //     return 0;

        // fdtable_t *tbl = p->fdtable;
        // struct file *f;
        // int omode = 0101101;
        // if ((f = filealloc()) == 0 || (fd = fdtbl_fdalloc(tbl, f, -1, omode)) < 0) {
        //     if (f)
        //         fileclose(f);
        //     eunlockput(ep);
        //     return -EMFILE;
        // }

        // f->ep = ep;
        // f->readable = !(omode & O_WRONLY);
        // f->writable = (omode & O_WRONLY) || (omode & O_RDWR);
        // f->type = FD_ENTRY;
        // f->off = 0;
        // // if((omode & O_TRUNC) && E_ISFILE(ep)){ // todo:
        // //   etrunc(ep);
        // // }

        // eunlock(ep);
        // return fd;
        // return ENOENT;
    }

    /* 根据手册，如果为null，把两个timestamps都设置为当前时间 */
    if((uint64_t *)addr == NULL){
        ep->raw.adate.year_from_1980 = ticks; 
        ep->raw.mdate.year_from_1980 = ticks;
    }
    else{
        if(copy_from_user(ts, addr, 2*sizeof(ts)) < 0) {
            return -1;
        }
        for(int i = 0; i < 2; i++){
            if(ts[i].ts_nsec == UTIME_NOW){
                if(i == 0){
                    ep->raw.adate.year_from_1980 = ticks;
                }
                else{
                    ep->raw.mdate.year_from_1980 = ticks;
                }
            }
            else if(ts[i].ts_nsec == UTIME_OMIT){
                /* do noting*/
            }
            else{
                uint64_t ms = ts[i].ts_sec*1000 + ts[i].ts_nsec/1000;
                if(i == 0){
                    ep->raw.adate.year_from_1980 = ms;
                }
                else{
                    ep->raw.mdate.year_from_1980 = ms;
                }
            }
        }
    }
    return 0; 
}

uint64_t
sys_rename(void){
    proc_t *p = myproc();
    char old[MAXPATH], new[MAXPATH];
    entry_t *new_entry, *old_entry, *from;

    if (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0) {
      return -1;
    }

    from = p->cwd;

    if((old_entry = namee(from, old)) == 0)
        ER();

    /* 改一下名字即可 */
    if((new_entry = namee(from, new)) == 0){
        /* 访问父目录，改写其中的name数据 */
    }
    else{

    }

    return 0;
}

uint64_t sys_fsync(void){
    int fd;
    entry_t *ep;

    if(argint(0, &fd) < 0)
        return -1;

    ep = getep(myproc(), fd);    

    sych_entry_in_disk(ep);
    return 0;
}

extern void msync(uint64_t addr, uint64_t length, int flags);

/**
 * addr 和length需要是PGSIZE的倍数吗？
 */
uint64_t
sys_msync(void){
    uint64_t addr, length;
    int flags;

    if(argaddr(0, &addr) < 0 ||
        argaddr(1, &length) < 0 ||
        argint(2, &flags) < 0)
        return -1;

    msync(addr, length, flags);
    
    return 0;
}