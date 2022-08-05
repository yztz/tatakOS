#ifndef _H_SOCKET_
#define _H_SOCKET_

#include "common.h"


struct socket {
    int pos;
    char buf[20];
};

typedef struct socket socket_t;

#endif