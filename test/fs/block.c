#include <stdio.h>

#include "blk_device.h"

FILE *disk;

void block_init() {
    disk = fopen("disk.img", "r+");
    
}



