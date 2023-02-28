#ifndef _H_DEVICE_
#define _H_DEVICE_

#include "types.h"
#include "param.h"

struct _device_t {
  int (*read)(int, uint64, int);
  int (*write)(int, uint64, int);
};

#define CONSOLE 1

extern device_t devs[];


#endif