#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "stdlib.h"


int main() {
    char buf[255];

    printf("please input something: ");
    int n = read(0, buf, 255);
    assert(n > 0);
    buf[n] = '\0';
    printf("read %d bytes: %s\n", buf);

    return 0;
}