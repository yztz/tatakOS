#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

int i = 1000;
void test_waitpid(void){
    
    int cpid, wstatus;
    cpid = fork();
    assert(cpid != -1);
    if(cpid == 0){
	while(i--);
	sched_yield();
	printf("This is child process\n");
        exit(3);
    }else{
	pid_t ret = waitpid(cpid, &wstatus, 0);
	assert(ret != -1);
	if(ret == cpid && WEXITSTATUS(wstatus) == 3)
	    printf("waitpid successfully.\nwstatus: %x\n", WEXITSTATUS(wstatus));
	else
	    printf("waitpid error.\n");

    }
    
}

int main(void){
    test_waitpid();
    return 0;
}
