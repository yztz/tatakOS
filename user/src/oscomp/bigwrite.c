#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "generated/syscall.h"
#include "syscall.h"
#include "stddef.h"
#include "stdlib.h"

void print_cache() {
    cache_rate_t rate;
    syscall(NR_bio_cache, &rate);
    printf("hit: %d miss: %d\n", rate.hit, rate.miss);
}

/* 当缓冲区变大，速度变慢，找出是什么原因*/
// #define BUF_SIZE 4096 * 4 // 4KB
#define BUF_SIZE 4096 // 4KB
// #define FILE_SIZE (512 * 1024) // 1MB

// #define FILE_SIZE (1 * 1024 * 1024) //1mb, the last letter is 'V'
// #define FILE_SIZE (10 * 1024 * 1024) //10 mb the last letter is 'L'

// #define FILE_SIZE (1400 * 4096) 
#define FILE_SIZE (1501 * 4096) // 1500 % 26 = 18 "R"

int main() {
    char buff[BUF_SIZE];
    int fd = open("test_write.txt", O_CREATE|O_RDWR);
    
    if(fd < 0) {
        printf("open fail\n");
        return 0;
    }

    char letter = 'A';

    printf("start write\n");
    print_cache();

    // memset(buff, 'A', BUF_SIZE);

    uint64 test_ret1 = syscall(NR_timetag);

    for(int i = 0; i < FILE_SIZE / BUF_SIZE; i++) { // 50MB
        printf("i: %d..\n", i);
        letter = 'A' + i % 26;
        memset(buff, letter, BUF_SIZE);

        int nwrite = write(fd, buff, BUF_SIZE);
        if(nwrite < 0) {
            printf("write fail\n");
            return 0;
        } else {
            // printf("%d: %d written\n", i, nwrite);
        }
    }
    uint64 test_ret2 = syscall(NR_timetag);
    // int test_ret2 = get_time();
	// if(test_ret1 > 0 && test_ret2 > 0){
		printf(grn("write pages to mem\n"));
		printf(grn("start:%d, end:%d\n"), test_ret1, test_ret2);
        printf(grn("interval: %d\n"), test_ret2 - test_ret1);
	// }

    printf("end write\n");
    print_cache();
    // syscall(NR_profile);
    

    uint64 test_ret3 = syscall(NR_timetag);
    // int test_ret3 = get_time();
    close(fd);
    // int test_ret4 = get_time();
    uint64 test_ret4 = syscall(NR_timetag);
    printf(rd("wating for writing pages to disk...\n"));
	if(test_ret3 > 0 && test_ret4 > 0){
		printf(bl("close file, write pages back to disk\n"));
		printf(bl("start:%d, end:%d\n"), test_ret3, test_ret4);
        printf(bl("interval: %d\n"), test_ret4 - test_ret3);
	}


    printf("bigwrite test end!\n");
    return 0;
}