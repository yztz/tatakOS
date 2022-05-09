#include <stdio.h>

#define HASH_LENGTH 20

int hash(int dev, int clus) {
    return clus % 20;
}

int main() {

    int dev, clus;
    while(1) {
        scanf("%d %d", &dev, &clus);
        printf("hash = %d\n", hash(dev, clus));
    }
}