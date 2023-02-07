#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"

void test_open() {
	
	// O_RDONLY = 0, O_WRONLY = 1
	int fd = open("./text.txt", 0);
	assert(fd >= 0);
	char buf[256];
	int size = read(fd, buf, 256);
	if (size < 0) {
		size = 0;
	}
	write(STDOUT, buf, size);
	close(fd);
	
}

int main(void) {
	test_open();
	return 0;
}
