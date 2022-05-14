#include "stdio.h"
#include "stdint.h"

struct linux_dirent64 {
        uint64_t        d_ino;
        int64_t         d_off;
        unsigned short  d_reclen;
        unsigned char   d_type;
        char            d_name[];
};

int main() {
    printf("%ld\n", sizeof(struct linux_dirent64));
    return 0;
}