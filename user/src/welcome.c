#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "syscall.h"
#include "stddef.h"

int main() {
    printf("Lab "STR(LAB_ID)" - "STR(LAB_DES)"\n\n");
    printf("🎉 Kernel starts successfully!\n");
    printf("😉 You are supposed to run " cyan("test()")" in " cyan("src/kernel/initcode.c") " if you are ready to submit📤 your code.\n");
    printf("😘 Enjoy it!\n");
    return 0;
}