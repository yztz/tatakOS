#ifndef H_BIO
#define H_BIO

#include "types.h"

#define READ 0
#define WRITE 1

/*
 * main unit of I/O for the block layer and lower layers (ie drivers and
 * stacking drivers)
 */

/* bio is private to process, there must only one process access it 
at a time, so no need lock protect list */
typedef struct bio {
  struct bio_vec *bi_io_vec;
  uint8 bi_rw;
  struct bio *bi_next;
  uint bi_dev;
} bio_t;



/**
 * @brief 描述了一个I/O操作的段（连续的sectors）
 * 
 */
typedef struct bio_vec {
  sector_t bv_start_num;/* the number of the first sector */
  uint32 bv_count;/* the counts of sectors */
  struct bio_vec *bv_next;/* the pointer of next bio segment */
  void *bv_buff; /* the address to begin read/write */
  int disk;
} bio_vec_t;


typedef struct request_queue {
  struct bio *rq_head, *rq_tail;
  spinlock_t rq_lock;
} request_queue_t;

void make_request();
void submit_bio(struct bio *bio);

#endif