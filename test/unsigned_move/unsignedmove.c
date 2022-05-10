#include <stdio.h>
#include <stdint.h>

int main() {
    uint32_t a = 0xffffffff;
    printf("a is %x \na >> 16 is %x\n", a, a >> 16);
    return 0;
}