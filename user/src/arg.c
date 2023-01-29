#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "syscall.h"
#include "stddef.h"


int main(int argc, char **argv) {
    printf("args list:\n");
    int i;
    for(i = 0; i < argc; i++) {
        printf("arg %d: %s\n", i, argv[i]);
    }

    return 0;
}