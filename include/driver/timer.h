#ifndef _H_TIMER_
#define _H_TIMER_

#include"types.h"

/**
 * @brief Init the CLINT timer
 */
void init_timer();

/**
 * @brief Get the CLINT timer value
 * 
 * @return uint64_t 
 */
uint64_t get_time();

/**
 * @brief Reset CLINT timer
 */
void reset_timer();

/**
 * @brief sleep for usec
 * 
 * @param usec 
 * @return int 
 */
int usleep(uint64_t usec);

typedef void (*timer_callback_t)(void *param);

/**
 * @brief Timer which records proc's sleeping info
 */
struct timer {
    /// @brief Rest ticks of the timer
    int expires;
    /// @brief Callback after expiration
    timer_callback_t callback;
    /// @brief Callback's param
    void *param;
    /// @brief For Link
    list_head_t head;
};

typedef struct timer timer_t;

/**
 * @brief Wait for a tick
 * @deprecated Only some unimplemented function depend on it
 */
void wait_tick();

/**
 * @brief Set a specific timer
 * 
 * @param timer     Used to receive timer [OUT]
 * @param callback  Timer callback
 * @param timeout   Timer timeout(ms)
 * @param param     Callback's param
 */
void set_timer(timer_t *timer, timer_callback_t callback, int timeout, void *param);

/**
 * @brief Remove the timer
 * 
 * @param timer The timer which will be removed
 */
void remove_timer(timer_t *timer);


extern struct spinlock tickslock;
extern uint64_t ticks;

#endif