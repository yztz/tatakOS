#ifndef _H_DEVICE_
#define _H_DEVICE_

#include "types.h"
#include "param.h"

struct kstat;

typedef struct _device_t {
  int (*read)(int, uint64, int);
  int (*write)(int, uint64, int);
} device_t;

#define CONSOLE 1
#define DEVNULL 2
#define DEVZERO 3
#define DEVRTC  4

extern device_t devs[];

void dev_stat(device_t *dev, struct kstat *stat);

#endif