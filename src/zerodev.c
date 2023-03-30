#include "device.h"
#include "common.h"
#include "str.h"
#include "mm/vm.h"

int devzero_read(int user_dst, uint64_t dst, int n) {
    if(!user_dst) {
        memset((void *)dst, 0, n);
    } else {
        memset_user(dst, 0, n);
    }
    return n;
}

int devzero_write(int user_src, uint64_t src, int n) {
    return 0;
}


void devzero_init() {
    devs[DEVZERO].read = devzero_read;
    devs[DEVZERO].write = devzero_write;
}