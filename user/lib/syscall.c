#include <stddef.h>
#include <unistd.h>

#include "syscall.h"

int open(const char *path, int flags)
{
    return syscall(SYS_openat, AT_FDCWD, path, flags, 0666);
}

int openat(int dirfd,const char *path, int flags)
{
    return syscall(SYS_openat, dirfd, path, flags, 0600);
}

int close(int fd)
{
    return syscall(SYS_close, fd);
}

ssize_t read(int fd, void *buf, size_t len)
{
    return syscall(SYS_read, fd, buf, len);
}

ssize_t write(int fd, const void *buf, size_t len)
{
    return syscall(SYS_write, fd, buf, len);
}

pid_t getpid(void)
{
    return syscall(SYS_getpid);
}

pid_t gettid(void)
{
    return syscall(SYS_gettid);
}

pid_t getppid(void)
{
    return syscall(SYS_getppid);
}

int sched_yield(void)
{
    return syscall(SYS_sched_yield);
}

pid_t fork(void)
{
    return syscall(SYS_clone, 0, 0, 0, 0, 0);
}

pid_t clone(int (*fn)(void *arg), void *arg, void *stack, size_t stack_size, unsigned long flags, void* ptid, void *tls, void *ctid)
{
    if (stack)
	    stack += stack_size;
    // __clone(func, stack, flags, arg, ptid, tls, ctid)
    return __clone(fn, stack, flags, arg, ptid, tls, ctid);
    //return syscall(SYS_clone, fn, stack, flags, NULL, NULL, NULL);
}
void exit(int code)
{
    syscall(SYS_exit, code);
}

int waitpid(int pid, int *code, int options)
{
    return syscall(SYS_wait4, pid, code, options, 0);
}

int exec(char *name)
{
    return syscall(SYS_execve, name);
}

int execve(const char *name, char *const argv[], char *const argp[])
{
    return syscall(SYS_execve, name, argv, argp);
}

int times(void *mytimes)
{
	return syscall(SYS_times, mytimes);
}

int64_t get_time()
{
    TimeVal time;
    int err = sys_get_time(&time, 0);
    if (err == 0)
    {
        return ((time.sec & 0xffff) * 1000 + time.usec / 1000);
    }
    else
    {
        return -1;
    }
}

int sys_get_time(TimeVal *ts, int tz)
{
    return syscall(SYS_gettimeofday, ts, tz);
}

int time(unsigned long *tloc)
{
    return syscall(SYS_time, tloc);
}

int sleep(unsigned long long time)
{
    TimeVal tv = {.sec = time, .usec = 0};
    if (syscall(SYS_nanosleep, &tv, &tv)) return tv.sec;
    return 0;
}


void *mmap(void *start, size_t len, int prot, int flags, int fd, off_t off)
{
    return (void *)syscall(SYS_mmap, start, len, prot, flags, fd, off);
}

int munmap(void *start, size_t len)
{
    return syscall(SYS_munmap, start, len);
}

int wait(int *code)
{
    return waitpid((int)-1, code, 0);
}

int spawn(char *file)
{
    return syscall(SYS_spawn, file);
}


int fstat(int fd, struct kstat *st)
{
    return syscall(SYS_fstat, fd, st);
}

int sys_linkat(int olddirfd, char *oldpath, int newdirfd, char *newpath, unsigned int flags)
{
    return syscall(SYS_linkat, olddirfd, oldpath, newdirfd, newpath, flags);
}

int sys_unlinkat(int dirfd, char *path, unsigned int flags)
{
    return syscall(SYS_unlinkat, dirfd, path, flags);
}

int link(char *old_path, char *new_path)
{
    return sys_linkat(AT_FDCWD, old_path, AT_FDCWD, new_path, 0);
}

int unlink(char *path)
{
    return sys_unlinkat(AT_FDCWD, path, 0);
}

int uname(void *buf)
{
    return syscall(SYS_uname, buf);
}

int brk(void *addr)
{
    return syscall(SYS_brk, addr);
}

char *getcwd(char *buf, size_t size){
    return (char *)syscall(SYS_getcwd, buf, size);
}

int chdir(const char *path){
    return syscall(SYS_chdir, path);
}

int mkdir(const char *path, mode_t mode){
    return syscall(SYS_mkdirat, AT_FDCWD, path, mode);
}

int getdents(int fd, struct linux_dirent64 *dirp64, unsigned long len){
    //return syscall(SYS_getdents64, fd, dirp64, len);
    return syscall(SYS_getdents64, fd, dirp64, len);
}

int pipe(int fd[2]){
    return syscall(SYS_pipe2, fd, 0);
}

int dup(int fd){
    return syscall(SYS_dup, fd);
}

int dup2(int old, int new){
    return syscall(SYS_dup3, old, new, 0);
}

int mount(const char *special, const char *dir, const char *fstype, unsigned long flags, const void *data)
{
        return syscall(SYS_mount, special, dir, fstype, flags, data);
}

int umount(const char *special)
{
        return syscall(SYS_umount2, special, 0);
}

void halt() 
{
    syscall(SYS_halt);
}

void memuse() 
{
    syscall(SYS_memuse);
}
