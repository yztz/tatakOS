#include "device.h"
#include "common.h"

int devnull_read(int user_dst, uint64 dst, int n) {
    return 0;
}

int devnull_write(int user_src, uint64 src, int n) {
    return n;
}


void devnull_init() {
    devs[DEVNULL].read = devnull_read;
    devs[DEVNULL].write = devnull_write;
}