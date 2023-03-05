#ifndef H_BIO
#define H_BIO

#include "types.h"
#include "atomic/sleeplock.h"
#include "list.h"

#define BIO_READ 0
#define BIO_WRITE 1


struct block_buffer {
    /// @brief Has data been read from disk?
    int valid;
    int dirty;
    uint dev;
    uint blockno;
    struct sleeplock lock;
    uint refcnt;
    /// @brief LRU cache list
    list_head_t lru;
    uchar data[BSIZE];
};

typedef struct block_buffer blk_buf_t;

/**
 * @note bio is private to process, there must only one process access it
 *        at a time, so no need lock protect list
 * 
 */
struct bio {
    struct bio_vec *bi_io_vec;
    struct bio *bi_next;
    uint8 bi_rw;
    uint bi_dev;
};

typedef struct bio bio_t;


/**
 * @brief 描述了一个I/O操作的段（连续的sectors）
 *
 */
struct bio_vec {
    /// @brief the number of the first sector
    sector_t bv_start_num;
    /// @brief the counts of sectors
    uint32 bv_count;
    /// @brief the pointer of next bio segment
    struct bio_vec *bv_next;
    /// @brief the address to begin read/write
    void *bv_buff;
    // int disk;
};

typedef struct bio_vec bio_vec_t;

/**
 * @brief Init block cache.
 * 
 */
void bcache_init();

/**
 * @brief Submit a bio.
 * 
 * @param bio 
 */
void submit_bio(bio_t *bio);
/**
 * @brief Return a locked buf with the contents of the indicated block.
 * 
 * @param dev 
 * @param blockno 
 * @return blk_buf_t* 
 */
blk_buf_t *bread(uint dev, uint blockno);

/**
 * @brief Write b's contents. Must be locked.
 *        Its only function is to mark buffer dirty.
 */
void bwrite(blk_buf_t *buf);

/**
 * @brief Release a locked buffer. 
 *        Move to the head of the most-recently-used list.
 *        It will trigger writeback if it's marked dirty.
 */
void brelse(blk_buf_t *buf);

#endif
