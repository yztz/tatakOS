#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

void __run(char *argv[]);

#define run(name, ...) {char *__cmd[] = {name, ##__VA_ARGS__, 0};__run(__cmd);}
#define shell(...) {char *__cmd[] = {"busybox", "sh", ##__VA_ARGS__, 0};__run(__cmd);}
#define lua(...) {char *__cmd[] = {"lua", ##__VA_ARGS__, 0};__run(__cmd);}
#define lmbench(...) {char *__cmd[] = {"lmbench_all", ##__VA_ARGS__, 0};__run(__cmd);}

void main() {
    mkdir("tmp", 0666);
    mkdir("proc", 0666);
    mkdir("proc/mounts", 0666);

    memuse();
    shell();
    // printf("Hello World\n");
    // run("qjs");
    memuse();
    halt();
    for(;;);
}

#define WEXITSTATUS(s) (((s) & 0xff00) >> 8)
void __run(char *argv[]) {
    char *env[] = {NULL};
    int npid = fork();
    if(npid < 0) {
        printf("fork failed");
        for(;;);
    }
    if (npid == 0) { //子进程
        int ret = execve(argv[0], argv, env);
        printf("exec %s fail with %d\n", argv[0], ret);
    } else {          // 父进程
        int status;
        wait(&status);
        printf("child %s exit with %d\n", argv[0], WEXITSTATUS(status));
    }
}