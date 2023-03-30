#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "syscall.h"
#include "stddef.h"
#include "stdlib.h"


int main() {
    syscall(235);
    return 0;
}