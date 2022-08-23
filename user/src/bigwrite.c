#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "generated/syscall.h"
#include "syscall.h"
#include "stddef.h"
#include "stdlib.h"

typedef uint64 uint64_t;

struct timespec {
	uint64_t ts_sec;        /* 秒 */
	uint64_t ts_nsec;       /* 纳秒 */
};

typedef struct timespec ts_t; 


uint64_t tsdelta(ts_t *start, ts_t *stop) {
	ts_t td;
	uint64_t usecs;

	if(start->ts_nsec > stop->ts_nsec) {
        td.ts_sec = stop->ts_sec - start->ts_sec - 1;
        td.ts_nsec = stop->ts_nsec + 1000*1000*1000 - start->ts_nsec;
    } else {
        td.ts_sec = stop->ts_sec - start->ts_sec;
        td.ts_nsec = stop->ts_nsec - start->ts_nsec;
    }
	usecs = td.ts_sec * 1000 * 1000;
	usecs += td.ts_nsec / 1000;
	return (usecs);
}


// void print_cache() {
//     cache_rate_t rate;
//     syscall(NR_bio_cache, &rate);
//     printf("hit: %d miss: %d\n", rate.hit, rate.miss);
// }

/* 当缓冲区变大，速度变慢，找出是什么原因*/
// #define BUF_SIZE 4096 * 4 // 4KB
#define BUF_SIZE 4096 // 4KB
char buff[BUF_SIZE];
// #define FILE_SIZE (512 * 1024) // 1MB

// #define FILE_SIZE (1 * 1024 * 1024) //1mb, the last letter is 'V'
// #define FILE_SIZE (10 * 1024 * 1024) //10 mb the last letter is 'L'

// #define FILE_SIZE (1400 * 4096) 
#define FILE_SIZE (1 * 1024 * 1024) // 1500 % 26 = 18 "R"


void gettime(ts_t *ts) {
    syscall(NR_clock_gettime, 0, ts);
}

int main() {
    
    int fd = open("test_write.txt", O_CREATE|O_RDWR);
    
    if(fd < 0) {
        printf("open fail\n");
        return 0;
    }

    char letter = 'A';

    int mb = FILE_SIZE / 1024 / 1024;

    printf("start write %d MB batchSize %d batch %d\n", mb, BUF_SIZE, FILE_SIZE / BUF_SIZE);


    ts_t t1, t2, t3, t4;

    gettime(&t1);
    for(int i = 0; i < FILE_SIZE / BUF_SIZE; i++) {
        letter = 'A' + i % 26;
        memset(buff, letter, BUF_SIZE);

        int nwrite = write(fd, buff, BUF_SIZE);
        if(nwrite < 0) {
            printf("write fail\n");
            return 0;
        } else {
            // printf("%d: %d written\n", i, nwrite);
        }
        // printf("Batch %d\n", i);
    }
    gettime(&t2);
    printf(grn("write use %d us\n"), tsdelta(&t1, &t2));


    gettime(&t3);
    close(fd);
    gettime(&t4);
    
	printf(grn("writeback use %d us\n"), tsdelta(&t3, &t4));

    printf("\n");
    uint64_t total = tsdelta(&t1, &t2) + tsdelta(&t3, &t4);
    printf(grn("Total: %d us. Speed %d kB/s\n"), total, mb * 1024 * 1000 * 1000L / total);

    return 0;
}