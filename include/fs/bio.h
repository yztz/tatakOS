#include "types.h"

#define READ 0
#define WRITE 1

/*
 * main unit of I/O for the block layer and lower layers (ie drivers and
 * stacking drivers)
 */

struct bio {
  struct bio_vec *bi_io_vec;
  uint8 bi_rw;
};


/**
 * @brief 描述了一个I/O操作的段（连续的sectors）
 * 
 */
struct bio_vec {
  sector_t bv_start_num;/* the number of the first sector */
  uint32 count;/* the counts of sectors */
  struct bio_vec *next;/* the pointer of next bio segment */
  void *buff; /* the address to begin read/write */
};
