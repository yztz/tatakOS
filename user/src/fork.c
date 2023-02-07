#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"
/*
 * 成功测试时父进程的输出：
 * "  parent process."
 * 成功测试时子进程的输出：
 * "  child process."
 */

void test_fork(void){
    
    int cpid, wstatus;
    cpid = fork();
    assert(cpid != -1);

    if(cpid > 0){
	wait(&wstatus);
	printf("  parent process. wstatus:%d\n", wstatus);
    }else{
	printf("  child process.\n");
	exit(0);
    }
    
}

int main(void){
    test_fork();
    return 0;
}
