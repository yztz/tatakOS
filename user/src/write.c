#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

void test_write(){
	
	const char *str = "Hello operating system contest.\n";
	int str_len = strlen(str);
	assert(write(STDOUT, str, str_len) == str_len);
	
}

int main(void) {
	test_write();
	return 0;
}
