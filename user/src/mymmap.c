#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "generated/syscall.h"
#include "syscall.h"
#include "stddef.h"

char data[4098];

int main() {

    for(int i = 0; i < 4096; i++) {
        data[i] == 1;
    }
    data[4096] = 2;
    data[4097] = 3;

    int fd = open("test_mmap.txt", O_RDWR | O_CREATE);
    write(fd, data, sizeof(data));


    char *array = mmap(NULL, 4098, PROT_WRITE | PROT_READ, MAP_PRIVATE, fd, 0);
    printf("array is %p\n", array);
    int i;
    printf("< 4096:\n");
    for(i = 0; i < 4096; i++) {
        printf("%d ", array[i]);
        if(i != 0 && i % 128 == 0) printf("\n");
    }
    printf("4096: %d", array[4096]);
    printf("4097: %d", array[4097]);
    return 0;
}