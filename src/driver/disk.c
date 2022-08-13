#include "common.h"
#include "atomic/spinlock.h"
#include "atomic/sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs/fs.h"
#include "fs/blk_device.h"
#include "bio.h"
#include "utils.h"


#ifdef K210
#include "sdcard.h"
#else 
extern void virtio_disk_rw(bio_vec_t *, int);
#endif

void (disk_io)(bio_vec_t *bio_vec, int write) {
  #ifdef K210
  if(write) {
    // printf("out no %d %d\n", bio_vec->bv_start_num, bio_vec->bv_count);
    sd_write_sector_dma(bio_vec->bv_buff, bio_vec);
    // printf("out done\n");
  } else {
    // printf("in no %d %d\n", bio_vec->bv_start_num, bio_vec->bv_count);
    sd_read_sector_dma(bio_vec->bv_buff, bio_vec);
  }
  #else
  // if(write) printf("out no %d %d\n", bio_vec->bv_start_num, bio_vec->bv_count);
  virtio_disk_rw(bio_vec, write);
  #endif
}