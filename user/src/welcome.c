#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "syscall.h"
#include "stddef.h"

#define _STR(x) #x
#define STR(x) _STR(x)

int main() {
    printf("\nChapter "STR(CHAPTER)" - "STR(CHAPTER_DES)"\n\n");
    printf("🎉 Kernel starts successfully!\n");
    printf("😉 You are supposed to run chapter"STR(CHAPTER)" in initcode if you are ready to submit📤 your code.\n");
    printf("😘 Enjoy it!\n");
    return 0;
}