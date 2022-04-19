#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main() {
    uint64 start, end;
    for(int time = 0; time < 10; time++) {
        start = timetag();
        for(int i = 0; i < 100000; i++) ktest();
        end = timetag();
        printf("start: %d, end: %d, time: %d\n", start, end, end - start);
    }
    
    exit(0);
}