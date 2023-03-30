#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/*
理想结果：得到进程 pid，注意要关注 pid 是否符合内核逻辑，不要单纯以 Test OK! 作为判断。
*/

void test_getpid() {
    int pid = getpid();
    assert(pid >= 0);
    printf("getpid success.\npid = %d\n", pid);
    
}

int main(void) {
	test_getpid();
	return 0;
}
