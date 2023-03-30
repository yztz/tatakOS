#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


void test_brk(){
    
    intptr_t cur_pos, alloc_pos, alloc_pos_1;

    cur_pos = brk(0);
    printf("Before alloc,heap pos: %d\n", cur_pos);
    brk((void *)(cur_pos + 6400));
    alloc_pos = brk(0);
    printf("After alloc,heap pos: %d\n",alloc_pos);
    brk((void *)(alloc_pos + 64000));
    alloc_pos_1 = brk(0);
    printf("Alloc again,heap pos: %d\n",alloc_pos_1);
    
}

int main(void) {
    test_brk();
    return 0;
}

