//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "mm/vm.h"
#include "param.h"
#include "fs/stat.h"
#include "fs/fs.h"
#include "atomic/spinlock.h"
#include "kernel/proc.h"
#include "atomic/sleeplock.h"
#include "fs/file.h"
#include "fs/fcntl.h"

#define __MODULE_NAME__ SYS_FILE
#include "debug.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
static int argfd(int n, int *pfd, struct file **pf) {
  int fd;
  struct file *f;

  if(argint(n, &fd) < 0)
    return -1;
  if(fd < 0 || fd >= NOFILE || (f=myproc()->ofile[fd]) == 0)
    return -1;
  if(pfd)
    *pfd = fd;
  if(pf)
    *pf = f;
  return 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static int fdalloc(struct file *f) {
  int fd;
  struct proc *p = myproc();

  for(fd = 0; fd < NOFILE; fd++){
    if(p->ofile[fd] == 0){
      p->ofile[fd] = f;
      return fd;
    }
  }
  return -1;
}

uint64 sys_dup(void) {
  struct file *f;
  int fd;

  if(argfd(0, 0, &f) < 0)
    return -1;
  if((fd=fdalloc(f)) < 0)
    return -1;
  filedup(f);
  return fd;
}

uint64 sys_dup3(void) {
  return -1;
}

uint64 sys_read(void) {
  struct file *f;
  int n;
  uint64 p;

  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argaddr(1, &p) < 0)
    return -1;
  return fileread(f, p, n);
}

uint64 sys_write(void) {
  struct file *f;
  int n;
  uint64 p;

  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argaddr(1, &p) < 0)
    return -1;

  return filewrite(f, p, n);
}

//OK:
uint64 sys_close(void) {
  int fd;
  struct file *f;

  if(argfd(0, &fd, &f) < 0)
    return -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  return 0;
}

uint64 sys_fstat(void) {
  return -1;
}

uint64 sys_getcwd(void) {
  return -1;
}


uint64 sys_unlinkat(void) {
  return -1;
}

uint64 sys_getdents64(void) {
  return -1;
}


#define AT_FDCWD -100

//OK:
uint64 sys_openat(void) {
  // owner mode is ignored
  char path[MAXPATH];
  int dirfd, fd, omode;
  struct file *f;
  entry_t *ep;
  entry_t *from;
  int n;

  if(argint(0, &dirfd) || (n = argstr(1, path, MAXPATH)) < 0 || argint(2, &omode) < 0)
    return -1;
  
  debug("dirfd is %d path is %s omode is %x", dirfd, path, omode);
  if(dirfd == AT_FDCWD) {
    from = myproc()->cwd;
  } else {
    from = myproc()->ofile[dirfd]->ep;
  }

  if(omode & O_CREATE) { // 创建标志
    if((ep = create(from, path, T_FILE)) == 0){
      return -1;
    }
  } else {
    if((ep = namee(from, path)) == 0){
      debug("not found");
      return -1;
    }
    debug("found");
    elock(ep);
    if(E_ISDIR(ep) && omode != O_RDONLY && omode != O_DIRECTORY){
      eunlockput(ep);
      return -1;
    }
  }


  if((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0){
    if(f)
      fileclose(f);
    eunlockput(ep);
    return -1;
  }

  f->ep = ep;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);
  f->type = FD_ENTRY;
  // if((omode & O_TRUNC) && E_ISFILE(ep)){
  //   etrunc(ep);
  // }

  eunlock(ep);

  return fd;
}

//OK:
uint64 sys_mkdirat(void) {
  // ignore mode
  char path[MAXPATH];
  int dirfd;
  entry_t *ep, *from;

  if(argint(0, &dirfd) < 0 ||
    argstr(1, path, MAXPATH) < 0){
    return -1;
  }
  
  if(dirfd == AT_FDCWD) {
    from = myproc()->cwd;
  } else {
    from = myproc()->ofile[dirfd]->ep;
  }

  if((ep = create(from, path, T_DIR)) == NULL)
    return -1;

  eunlockput(ep);

  return 0;
}

//OK:
uint64 sys_chdir(void) {
  char path[MAXPATH];
  entry_t *ep;
  struct proc *p = myproc();
  
  if(argstr(0, path, MAXPATH) < 0 || (ep = namee(NULL, path)) == 0){
    return -1;
  }
  elock(ep);
  if(!E_ISDIR(ep)){
    eunlockput(ep);
    return -1;
  }
  eunlock(ep);
  eput(p->cwd);
  p->cwd = ep;
  return 0;
}

uint64 sys_exec(void) {
  char path[MAXPATH], *argv[MAXARG];
  int i;
  uint64 uargv, uarg;

  if(argstr(0, path, MAXPATH) < 0 || argaddr(1, &uargv) < 0){
    return -1;
  }

  memset(argv, 0, sizeof(argv));
  for(i=0;; i++){
    if(i >= NELEM(argv)){
      goto bad;
    }
    if(fetchaddr(uargv+sizeof(uint64)*i, (uint64*)&uarg) < 0){
      goto bad;
    }
    if(uarg == 0){
      argv[i] = 0;
      break;
    }
    argv[i] = kalloc();
    if(argv[i] == 0)
      goto bad;
    if(fetchstr(uarg, argv[i], PGSIZE) < 0)
      goto bad;
  }

  int ret = exec(path, argv);
  for(i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  return ret;

 bad:
  for(i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  return -1;
}


//OK:
uint64 sys_pipe2(void) {
  uint64 fdarray; // user pointer to array of two integers
  struct file *rf, *wf;
  int fd0, fd1;
  struct proc *p = myproc();

  if(argaddr(0, &fdarray) < 0)
    return -1;
  if(pipealloc(&rf, &wf) < 0)
    return -1;
  
  fd0 = -1;
  if((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0){
    if(fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    return -1;
  }
  if(copyout(p->pagetable, fdarray, (char*)&fd0, sizeof(fd0)) < 0 ||
     copyout(p->pagetable, fdarray+sizeof(fd0), (char *)&fd1, sizeof(fd1)) < 0){
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    return -1;
  }
  return 0;
}
