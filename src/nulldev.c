#include "device.h"
#include "common.h"

int devnull_read(int user_dst, uint64_t dst, int n) {
    return 0;
}

int devnull_write(int user_src, uint64_t src, int n) {
    return n;
}


void devnull_init() {
    devs[DEVNULL].read = devnull_read;
    devs[DEVNULL].write = devnull_write;
}