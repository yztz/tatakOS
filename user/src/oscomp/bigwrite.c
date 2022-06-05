#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "generated/syscall.h"
#include "syscall.h"
#include "stddef.h"

void print_cache() {
    cache_rate_t rate;
    syscall(NR_bio_cache, &rate);
    printf("hit: %d miss: %d\n", rate.hit, rate.miss);
}

#define BUF_SIZE 4096 // 4KB
#define FILE_SIZE (512 * 1024) // 1MB
// #define FILE_SIZE (2 * 1024 * 1024) // 2MB

int main() {
    char buff[BUF_SIZE];
    int fd = open("test_write.txt", O_CREATE|O_RDWR);
    
    if(fd < 0) {
        printf("open fail\n");
        return 0;
    }

    memset(buff, 'A', BUF_SIZE);

    printf("start write\n");
    print_cache();

    for(int i = 0; i < FILE_SIZE / BUF_SIZE; i++) { // 50MB
        int nwrite = write(fd, buff, BUF_SIZE);
        if(nwrite < 0) {
            printf("write fail\n");
            return 0;
        } else {
            printf("%d: %d written\n", i, nwrite);
        }
    }

    printf("end write\n");
    print_cache();
    syscall(NR_profile);
    
    close(fd);

    return 0;
}