#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "generated/syscall.h"
#include "syscall.h"

/*
 * 测试通过时应输出：
 * "  from fd 100"
 */
void test_dup2(){
	TEST_START(__func__);
	syscall(NR_memuse);
	int fd = dup2(STDOUT, 100);
	syscall(NR_memuse);
	assert(fd != -1);
	const char *str = "  from fd 100\n";
	write(100, str, strlen(str));
	TEST_END(__func__);
}

int main(void) {
	test_dup2();
	return 0;
}
