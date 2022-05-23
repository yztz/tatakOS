#include <stdio.h>
#include <string.h>

int counter = 0;

#define TEST(func, ...) ({ \
    __auto_type  a = func(__VA_ARGS__); \
    a; \
})

int foo(int t) {
    counter++;
    return 2;
}

int main() {
    int res = TEST(foo, 233);
    printf("%d counter is %d\n", res, counter);
    return 0;
}
