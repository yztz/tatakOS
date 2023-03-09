#ifndef _H_K210_BIT_
#define _H_K210_BIT_

#include "types.h"
static inline void set_bit_mask(volatile uint32_t *bits, uint32_t mask, uint32_t value)
{
    uint32_t org = (*bits) & ~mask;
    *bits = org | (value & mask);
}

static inline void set_bit_offset_mask(volatile uint32_t *bits, uint32_t mask, size_t offset, uint32_t value)
{
    set_bit_mask(bits, mask << offset, value << offset);
}

static inline void set_gpio_bit(volatile uint32_t *bits, size_t offset, uint32_t value)
{
    set_bit_offset_mask(bits, 1, offset, value);
}

static inline uint32_t get_bit_mask(volatile uint32_t *bits, uint32_t mask, size_t offset)
{
    return ((*bits) & (mask << offset)) >> offset;
}

static inline uint32_t get_gpio_bit(volatile uint32_t *bits, size_t offset)
{
    return get_bit_mask(bits, 1, offset);
}


#endif