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

int main() {
    char buff[1024];
    int fd = open("test_write.txt", O_CREATE|O_RDWR);
    
    if(fd < 0) {
        printf("open fail\n");
        return 0;
    }

    memset(buff, 'A', 1024);

    printf("start write\n");
    print_cache();

    // for(int i = 0; i < 1; i++) { // 50MB
    //     int nwrite = write(fd, buff, 1024);
    //     if(nwrite < 0) {
    //         printf("write fail\n");
    //         return 0;
    //     } else {
    //         // printf("%d: %d written\n", i, nwrite);
    //     }
    // }

    printf("end write");
    print_cache();
    
    close(fd);

    return 0;
}