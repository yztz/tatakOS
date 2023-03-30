#ifndef H_BIO
#define H_BIO

#include "types.h"
#include "atomic/sleeplock.h"
#include "list.h"

#define BIO_READ 0
#define BIO_WRITE 1

/// @brief block buffer
struct block_buffer {
    /// @brief Has data been read from disk?
    int valid;
    /// @brief Has block been dirty (be written).
    int dirty;
    /// @brief dev_no
    uint dev;
    /// @brief block no
    uint blockno;
    /// @brief The lock to protect self field.
    struct sleeplock lock;
    /// @brief reference count
    uint refcnt;
    /// @brief LRU cache list
    list_head_t lru;
    /// @brief data buffer
    uchar data[BSIZE];
};

typedef struct block_buffer blk_buf_t;

/**
 * @note bio is private to process, there must only one process access it
 *        at a time, so no need lock protect list
 * 
 */
struct bio {
    /// @brief the first bio_vec
    struct bio_vec *bi_io_vec;
    /// @brief bio's r/w type
    int bi_rw;
    /// @brief device number
    uint bi_dev;
};

typedef struct bio bio_t;


/// @brief describe a consecutive I/O segment
struct bio_vec {
    /// @brief the number of the first sector
    sector_t bv_start_num;
    /// @brief the counts of sectors
    uint32_t bv_count;
    /// @brief the pointer of next bio segment
    struct bio_vec *bv_next;
    /// @brief the address to begin read/write
    void *bv_buff;
    /// @brief bio transfer state
    int disk;
};

typedef struct bio_vec bio_vec_t;

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
