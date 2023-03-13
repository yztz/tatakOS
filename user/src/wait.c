#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

void test_wait(void){
    
    int cpid, wstatus;
    cpid = fork();
    if(cpid == 0){
	printf("This is child process\n");
        exit(0);
    }else{
	pid_t ret = wait(&wstatus);
	assert(ret != -1);
	if(ret == cpid)
	    printf("wait child success.\nwstatus: %d\n", wstatus);
	else
	    printf("wait child error.\n");
    }
    
}

int main(void){
    test_wait();
    return 0;
}
