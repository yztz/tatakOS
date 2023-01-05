#ifndef _H_TIMER_
#define _H_TIMER_

#include"types.h"

void init_timer();
uint64_t get_time();
void reset_timer();

typedef void (*timer_callback_t)(void *param);

struct timer {
    int expires;
    timer_callback_t callback;
    void *param;
    list_head_t head;
};

typedef struct timer timer_t;

void wait_timer();
void set_timer(timer_t *timer, timer_callback_t callback, int timeout, void *param);
void remove_timer(timer_t *timer);



#endif