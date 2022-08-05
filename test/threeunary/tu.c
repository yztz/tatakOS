#include <stdio.h>



int foo(int t) {
    return t + 1;
}


int main() {
    int a = 3;
    int ans = foo(a) ? foo(a) : 0;

    printf("ans is %d\n", ans);

    return 0;
}