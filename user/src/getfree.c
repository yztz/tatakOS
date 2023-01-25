#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "generated/syscall.h"
#include "syscall.h"
#include "stddef.h"
#include "stdlib.h"


int main() {
    // syscall(236);
    syscall(235);
    return 0;
}