#include "common.h"
#include "atomic/spinlock.h"
#include "atomic/sleeplock.h"
#include "riscv.h"
#include "fs/fs.h"
#include "fs/blk_device.h"
#include "bio.h"
#include "utils.h"
#include "driver/disk.h"
#include "mm/alloc.h"
    
#define __MODULE_NAME__ BLOCK_IO
#include "debug.h"


void free_bio(bio_t *bio) {
    bio_vec_t *cur_bio_vec = bio->bi_io_vec;
    while (cur_bio_vec) {
        bio_vec_t *t = cur_bio_vec;
        cur_bio_vec = t->bv_next;
        kfree(t);
    }
    kfree(bio);
}


void __submit_bio(bio_t *bio) {
    bio_vec_t *cur_bio_vec;
    cur_bio_vec = bio->bi_io_vec;
    while (cur_bio_vec) {
        disk_io(cur_bio_vec, bio->bi_rw);
        cur_bio_vec = cur_bio_vec->bv_next;
    }
}

void submit_bio(bio_t *bio) {
    __submit_bio(bio);
    free_bio(bio);
}

struct {
    struct spinlock lock;
    blk_buf_t buf[NBUF];

    /**
     * @brief Linked list of all buffers.
     *  Sorted by how recently the buffer was used.
     *  lru_head.next is most recent, lru_head.prev is least.
     */
    list_head_t lru_head;
} bcache;


void bcache_init() {
    blk_buf_t *b;

    initlock(&bcache.lock, "bcache");

    INIT_LIST_HEAD(&bcache.lru_head);
    for (b = bcache.buf; b < bcache.buf + NBUF; b++) {
        b->refcnt = 0;
        b->blockno = 0;
        b->dev = -1;
        initsleeplock(&b->lock, "buffer");
        list_add(&b->lru, &bcache.lru_head);
    }
}



// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static blk_buf_t *bget(uint dev, uint blockno) {
    blk_buf_t *b;

    acquire(&bcache.lock);

    // Is the block already cached?
    list_for_each_entry(b, &bcache.lru_head, lru) {
        if (b->dev == dev && b->blockno == blockno) {
            b->refcnt++;
            release(&bcache.lock);
            acquiresleep(&b->lock);
            return b;
        }
    }

    // Not cached.
    // Recycle the least recently used (LRU) unused buffer.
    list_for_each_entry_reverse(b, &bcache.lru_head, lru) {
        if (b->refcnt == 0) {
            b->dev = dev;
            b->blockno = blockno;
            b->valid = 0;
            b->dirty = 0;
            b->refcnt = 1;
            release(&bcache.lock);
            acquiresleep(&b->lock);
            return b;
        }
    }
    panic("bget: no buffers");
}

/**
 * @brief sync block between memory and disk
 * @note possible states:
 * 
 *      | vaild | dirty |  description  |
 *      | ----- | ----- | ------------- |
 *      |   0   |   0   | disk is newer |
 *      |   0   |   1   |    illegal    |
 *      |   1   |   0   |  synchronized |
 *      |   1   |   1   |  mem is newer |
 * 
 * @param b 
 */
static void bsync(blk_buf_t *b) {
    if (b->valid && !b->dirty) return;

    bio_vec_t vec = {.bv_buff=b->data, .bv_count=1, .bv_start_num=b->blockno, .bv_next=NULL};
    bio_t bio = {.bi_dev=b->dev,.bi_io_vec=&vec};

    if (!b->valid) {
        assert(!b->dirty);
        bio.bi_rw = BIO_READ;
        
    } else if (b->dirty) {
        bio.bi_rw = BIO_WRITE;
    }

    __submit_bio(&bio);
    b->valid = 1;
    b->dirty = 0;
}

blk_buf_t *bread(uint dev, uint blockno) {
    blk_buf_t *b;

    b = bget(dev, blockno);

    bsync(b);
    return b;
}

void bwrite(blk_buf_t *b) {
    if (!holdingsleep(&b->lock))
        panic("bwrite");

    b->dirty = 1;
}


void brelse(blk_buf_t *b) {
    if (!holdingsleep(&b->lock))
        panic("brelse");

    bsync(b);

    releasesleep(&b->lock);

    acquire(&bcache.lock);
    b->refcnt--;
    if (b->refcnt == 0) {
        // no one is waiting for it.
        // delete from list and move to the first
        list_del(&b->lru);
        list_add(&b->lru, &bcache.lru_head);
    }

    release(&bcache.lock);
}


