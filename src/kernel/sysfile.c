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
#include "mm/mmap.h"
#include "mm/vm.h"
#include "mm/io.h"
#include "common.h"

#define QUIET
#define __MODULE_NAME__ SYS_FILE
#include "debug.h"
#include "utils.h"


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

    int ret = filewrite(f, p, n);
    // assert(ret == n);
    return ret;
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

uint64 sys_unlinkat(void) {
    entry_t *entry, *from;
    int dirfd;
    char path[MAXPATH];
    proc_t *p = myproc();

    // todo: flag ignored
    if (argint(0, &dirfd) < 0 || argstr(1, path, MAXPATH) < 0)
        return -1;


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

    debug("dirfd is %d path is %s omode is %o", dirfd, path, omode);

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

    // assert((omode & O_TRUNC )== 0);

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

// OK:
uint64 sys_chdir(void) {
    char path[MAXPATH];
    entry_t* ep;
    struct proc* p = myproc();
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

    char cwd[MAXPATH];
    char pwd[MAXPATH];
    namepath(p->cwd, cwd);
    snprintf(pwd, MAXPATH, "PWD=%s", cwd);
    char *envp[] = {"LD_LIBRARY_PATH=/",
                    "LOGNAME=root",
                    pwd,
                    "PS1=\\u"grn("\\w")"\\$ ",
                    "PATH=/:/bin",
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

    // 由于我们并没有实现mproctect所以如果按照它给的flag来设置会有后续访存问题
    // if((addr = do_mmap(p->mm, fp, offset, addr, len, flags, prot | PROT_USER))== -1) {
    if((addr = do_mmap(p->mm, fp, offset, addr, len, flags, prot | PROT_USER | PROT_READ | PROT_WRITE | PROT_EXEC))== -1) {
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
    if(vma->rlen != len)
        ER();
    // debug("UNMAP addr is %#lx len is %#lx", addr, len);
    // mmap_print(p->mm);
    do_unmap(p->mm, addr, 1);
    return 0;
}