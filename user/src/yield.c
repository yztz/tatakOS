#include <unistd.h>
#include <stdio.h>

void test_yield(){
    for (int i = 0; i < 3; ++i){
        if (fork() == 0) {
	        for (int j = 0; j < 5; ++j){
                sched_yield();
                printf("Child %d. Iteration %d.\n", getpid(), j);
	        }
	        exit(0);
        }
    }
    wait(NULL);
    wait(NULL);
    wait(NULL);
}

int main(void) {
    test_yield();
    return 0;
}
