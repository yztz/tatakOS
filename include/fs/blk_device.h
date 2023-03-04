#ifndef _H_BLK_DEVICE_
#define _H_BLK_DEVICE_

#include "param.h"
#include "atomic/sleeplock.h"

struct buf {
  int valid;   // has data been read from disk?
  int dirty;
  int disk;    // does disk "own" buf?
  uint dev;
  uint blockno;
  struct sleeplock lock;
  uint refcnt;
  struct buf *prev; // LRU cache list
  struct buf *next;
  uchar data[BSIZE];
  // void *data;
  // todo:
};

struct buffer_head {
  int disk;
  uint blockno;
  void *data;
  struct buffer_head *next;
};

void            binit(void);
struct buf*     bread(uint dev, uint blockno);
void            brelse(struct buf*);
void            bwrite(struct buf*);

#endif