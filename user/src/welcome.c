#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "syscall.h"
#include "stddef.h"

int main() {
    printf("Lab "STR(LAB_ID)" - "STR(LAB_DES)"\n\n");
    printf("🎉 Kernel starts successfully!\n");
    printf("😉 Edit " cyan("user/src/init0.c") " to run your program🎯.\n");
    printf("😘 Enjoy it!\n");
    return 0;
}