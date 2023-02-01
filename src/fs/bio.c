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

static bio_t *buf_to_bio(struct buf *b, int rw){
  bio_t *bio = kzalloc(sizeof(bio_t));
  bio_vec_t *bio_vec = kzalloc(sizeof(bio_vec_t));

  bio_vec->bv_buff = (void *)b->data;
  bio_vec->bv_count = 1;
  bio_vec->bv_start_num = b->blockno;
  bio_vec->bv_next = NULL;

  bio->bi_dev = b->dev;
  bio->bi_io_vec = bio_vec;
  bio->bi_rw = rw;
  bio->bi_next = NULL;

  return bio;
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
    // disk_io(b, 0);
    bio_t *bio = buf_to_bio(b, READ);
    submit_bio(bio);

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
      // disk_io(b, 1);
    
      bio_t *bio = buf_to_bio(b, WRITE);
      submit_bio(bio);
    
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


void free_bio(bio_t *bio) {
  bio_vec_t **cur_bio_vec = &bio->bi_io_vec;
  while(*cur_bio_vec){
    bio_vec_t *t = *cur_bio_vec;
    *cur_bio_vec = t->bv_next;
    kfree(t);
  } 
  kfree(bio);
}

// request_queue_t rq = {.rq_head = NULL, .rq_tail = NULL, .rq_lock = {0, 0, 0, 0}};

void submit_bio(bio_t *bio) {
    bio_vec_t *cur_bio_vec;
    cur_bio_vec = bio->bi_io_vec;
    while(cur_bio_vec){
        disk_io(cur_bio_vec, bio->bi_rw);
        cur_bio_vec = cur_bio_vec->bv_next;
    }
    free_bio(bio);
}


// void make_request(){
//   struct bio *cur_bio;
//   struct bio_vec *cur_bio_vec;

//   cur_bio = rq.rq_head;
//   while(cur_bio){
//     // print_bio_vec(cur_bio);
//     cur_bio_vec = cur_bio->bi_io_vec;
//     /* 对于一次I/O请求的每个段 */
//     while(cur_bio_vec){
//       for(int i = 0; i < cur_bio_vec->bv_count; i++){
//         /* qemu */
//         /* 根据bio读读写位，判断是读还是写 */
//         // printf(rd("s: %d\n"), cur_bio_vec->bv_start_num+i);
//         struct buf *b = bread(cur_bio->bi_dev, cur_bio_vec->bv_start_num+i);
//         memmove(cur_bio_vec->bv_buff, (void *)b->data, BSIZE); 
//         brelse(b);
//         // printf(bl("buff: %p\n"), cur_bio_vec->bv_buff);
//         cur_bio_vec->bv_buff += BSIZE;
//       }
//       cur_bio_vec = cur_bio_vec->bv_next;
//     }
  
//   cur_bio = cur_bio->bi_next;
//   }

//   rq.rq_head = NULL; 
//   todo("free the IO request queue!");
// }

// void make_request(){
//   while(1){
//     acquire(&rq.rq_lock);

//   }
// }
