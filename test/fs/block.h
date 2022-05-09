#ifndef _H_BLOCK_
#define _H_BLOCK_

#include "types.h"

#define BSIZE 512

struct buf {
  int valid;   // has data been read from disk?
  int disk;    // does disk "own" buf?
  uint dev;
  uint blockno;
  uint refcnt;
  struct buf *prev; // LRU cache list
  struct buf *next;
  uchar data[BSIZE];
};

#endif