#include <stdio.h>

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
#define ROUNDUP(size, align_size) (((size) + (align_size)-1) & ~((align_size) - 1))
#define TEST(num, round) {printf("num %d -- round %d --> %d\n", num, round, DIV_ROUND_UP(num, round));}

int main() {
    
    TEST(101, 100)
    TEST(199, 100)
    TEST(203, 100)

    return 0;
}