#include "stdio.h"
#include "generated/syscall.h"
#include "syscall.h"
#include "stddef.h"

void print_cache() {
    cache_rate_t rate;
    syscall(NR_bio_cache, &rate);
    printf("hit: %d miss: %d\n", rate.hit, rate.miss);
}

int main() {    
    char buf[512];
    int fd;
    struct linux_dirent64 *de = buf;
    print_cache();
    if((fd = open(".", 0)) < 0){
        printf("cannot open %s\n", ".");
        return;
    }
    int nread;

    while(1) {
        if((nread = getdents(fd, buf, 512)) == -1) {
        printf("getdentes fail\n");
        break;
        }

        if(nread == 0) break;

        int pos = 0;

        while(pos < nread) {
            de = &buf[pos];
            printf("read %s\n", de->d_name);
            int fd2 = open(de->d_name, O_RDONLY);
            char rdbuf[512];
            if(fd2 < 0) {
                printf("open %s fail\n", de->d_name);
                return -1;
            }
            while(read(fd2, rdbuf, 512) > 0);
            close(fd2);

            pos += de->d_reclen;
        }
    }
    print_cache();
    return 0;
}