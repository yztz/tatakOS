#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"

void __run(char *argv[]);

#define run(name, ...) {char *__cmd[] = {name, ##__VA_ARGS__, 0};__run(__cmd);}

void main() {
    run("welcome");
    halt();
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
        printf("exec %s failed with %d\n", argv[0], ret);
    } else {          // 父进程
        int status;
        wait(&status);
        printf("child %s exited with %d\n", argv[0], WEXITSTATUS(status));
    }
}