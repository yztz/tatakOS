#include "stdio.h"
#include "stdint.h"

static uint8_t cal_checksum(char* shortname) {
    uint8_t sum = 0;
    for (int i = 11; i != 0; i--) {
        sum = ((sum & 1) ? 0x80 : 0) + (sum >> 1) + *shortname++;
    }
    return sum;
}


int main() {
    // 174 right
    // 163 gettimeofday
    // 249 gettim~1
    // 144 GETTIM~1
    // 48  GETTIM~2
    // 174 GETTIM~1\20\20\20   
    char *sn = "GETTIM~1   ";
    printf("check sum of %s is %d\n",sn,cal_checksum(sn));
    return 0;
}