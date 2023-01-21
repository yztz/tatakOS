#ifndef _H_DEVICE_
#define _H_DEVICE_

#include "types.h"
#include "param.h"

typedef struct _device_t {
  int (*read)(int, uint64, int);
  int (*write)(int, uint64, int);
} device_t;

#define CONSOLE 1

extern device_t devs[];


#endif