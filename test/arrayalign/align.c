#include <stdio.h>

struct array {
    char name[63];
    char sex[66];
    char intersts[65];
};

int main() {
    printf("size %ld\n", sizeof(struct array));
    return 0;
}