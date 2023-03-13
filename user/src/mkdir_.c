#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

void test_mkdir(void){
    TEST_START(__func__);
    int rt, fd;

    rt = mkdir("test_mkdir", 0666);
    printf("mkdir ret: %d\n", rt);
    assert(rt != -1);
    fd = open("test_mkdir", O_RDONLY | O_DIRECTORY);
    if(fd > 0){
        printf("  mkdir success.\n");
        close(fd);
    }
    else printf("  mkdir error.\n");
    TEST_END(__func__);
}

int main(void){
    test_mkdir();
    return 0;
}
