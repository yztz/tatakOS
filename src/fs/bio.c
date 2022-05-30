// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "atomic/spinlock.h"
#include "atomic/sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs/fs.h"
#include "fs/blk_device.h"
#include "bio.h"

#ifdef K210
extern uint8_t sd_read_sector_dma(uint8_t *data_buff, uint32_t sector, uint32_t count);
extern uint8_t sd_write_sector_dma(uint8_t *data_buff, uint32_t sector, uint32_t count);
#else 
extern void virtio_disk_rw(struct buf *, int);
#endif


static void (disk_io)(struct buf *b, int write) {
  #ifdef K210
  if(write) {
    sd_write_sector_dma(b->data, b->blockno, 1);
  } else {
    sd_read_sector_dma(b->data, b->blockno, 1);
  }
  #else
  virtio_disk_rw(b, write);
  #endif
}

struct {
  struct spinlock lock;
  struct buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  struct buf head;
} bcache;


#ifdef DEBUG
uint64_t bio_cache_hit;
uint64_t bio_cache_miss;
#endif

void
binit(void)
{
  struct buf *b;

  initlock(&bcache.lock, "bcache");

  #ifdef DEBUG
  bio_cache_hit = 0;
  bio_cache_miss = 0;
  #endif

  // Create linked list of buffers
  bcache.head.prev = &bcache.head;
  bcache.head.next = &bcache.head;
  for(b = bcache.buf; b < bcache.buf+NBUF; b++){
    b->refcnt = 0;
    b->next = bcache.head.next;
    b->prev = &bcache.head;
    b->blockno = 0;
    b->dev = -1;
    initsleeplock(&b->lock, "buffer");
    bcache.head.next->prev = b;
    bcache.head.next = b;
  }
}



// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;

  acquire(&bcache.lock);

  // Is the block already cached?
  for(b = bcache.head.next; b != &bcache.head; b = b->next){
    if(b->dev == dev && b->blockno == blockno){
      #ifdef DEBUG
      bio_cache_hit++;
      #endif
      b->refcnt++;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      return b;
    }
  }
  #ifdef DEBUG
  bio_cache_miss++;
  #endif
  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  for(b = bcache.head.prev; b != &bcache.head; b = b->prev){
    if(b->refcnt == 0) {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      return b;
    }
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf*
(bread)(uint dev, uint blockno)
{
  struct buf *b;
  
  b = bget(dev, blockno);

  if(!b->valid) {
    disk_io(b, 0);
    b->valid = 1;
    b->dirty = 0;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
(bwrite)(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  // disk_io(b, 1);
  b->dirty = 1;
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
(brelse)(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");
  
  if(b->dirty == 1) {
      disk_io(b, 1);
      b->dirty = 0;
  }

  releasesleep(&b->lock);

  acquire(&bcache.lock);
  b->refcnt--;
  if (b->refcnt == 0) {
    // no one is waiting for it.
    b->next->prev = b->prev;
    b->prev->next = b->next;
    b->next = bcache.head.next;
    b->prev = &bcache.head;
    bcache.head.next->prev = b;
    bcache.head.next = b;
  }
  
  release(&bcache.lock);
}

void
bpin(struct buf *b) {
  acquire(&bcache.lock);
  b->refcnt++;
  release(&bcache.lock);
}

void
bunpin(struct buf *b) {
  acquire(&bcache.lock);
  b->refcnt--;
  release(&bcache.lock);
}



/*************************new add func**************************/

struct request_queue rq = {.queue_head = NULL, .rq_lock = {0, 0, 0, 0}};

void submit_bio(uint8 rw, struct bio *bio){
  /* 如果持有这个锁，然后都磁盘，进程切换，会报错 */
  // acquire(&rq.rq_lock);
  bio->bi_next = rq.queue_head;
  rq.queue_head = bio;  
  make_request(rw);
  // release(&rq.rq_lock);
}


void make_request(uint8 rw){
  struct bio *cur_bio;
  struct bio_vec *cur_bio_vec;

  cur_bio = rq.queue_head;
  while(cur_bio){

    cur_bio_vec = cur_bio->bi_io_vec;
    /* 对于一次I/O请求的每个段 */
    while(cur_bio_vec){
      for(int i = 0; i < cur_bio_vec->bv_count; i++){
        #if defined K210
          panic("make_request: not implementation!");
        #elif defined QEMU
        /* 先调用一个bio 函数, debug用 */
        struct buf *b = bread(cur_bio->bi_dev, cur_bio_vec->bv_start_num+i);
        memmove(cur_bio_vec->bv_buff, (void *)b->data, BSIZE); 
        brelse(b);
        #else
          panic("no defined platform");
        #endif
        cur_bio_vec->bv_buff += BSIZE;
      }
      cur_bio_vec = cur_bio_vec->bv_next;
    }
  
  cur_bio = cur_bio->bi_next;
  }
    
}