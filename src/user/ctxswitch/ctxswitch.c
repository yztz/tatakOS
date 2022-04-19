#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main() {
    uint64 start, end;
    start = timetag();
    for(int i = 0; i < 100000; i++) ktest();
    end = timetag();
    printf("\nstart: %x, end: %x, time: %x\n", start, end, end - start);
    exit(0);
}