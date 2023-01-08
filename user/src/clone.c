#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

size_t stack[1024] = {0};
static int child_pid;

static int child_func(void *arg){
    printf("  Child says successfully!\n");
    return 0;
}

void test_clone(void){
    TEST_START(__func__);
    int wstatus;
    child_pid = clone(child_func, NULL, stack, 1024, SIGCHLD);
    assert(child_pid != -1);
    if (child_pid == 0){
	exit(0);
    }else{
	if(wait(&wstatus) == child_pid)
	    printf("clone process successfully.\npid:%d\n", child_pid);
	else
	    printf("clone process error.\n");
    }

    TEST_END(__func__);
}

int main(void){
    test_clone();
    return 0;
}
