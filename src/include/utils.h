#ifndef _H_UTILS_
#define _H_UTILS_

#include "mm/vm.h"
#include "sbi.h"

#define LOOP() {while(1) continue;}
#define DIVIDER() {printf("\n---------------------------------\n");}

#define ALIGN(a, align) ((a + (align - 1)) & ~(align - 1))

#define max(a, b) ((a) > (b) ? (a) : (b))

#define _section(name) __attribute__((noinline, section(#name)))

static inline void set_bit(volatile uint32_t *bits, uint32_t mask, uint32_t value)
{
    uint32_t org = (*bits) & ~mask;
    *bits = org | (value & mask);
}

static inline void set_bit_offset(volatile uint32_t *bits, uint32_t mask, size_t offset, uint32_t value)
{
    set_bit(bits, mask << offset, value << offset);
}

static inline void set_gpio_bit(volatile uint32_t *bits, size_t offset, uint32_t value)
{
    set_bit_offset(bits, 1, offset, value);
}

static inline uint32_t get_bit(volatile uint32_t *bits, uint32_t mask, size_t offset)
{
    return ((*bits) & (mask << offset)) >> offset;
}

static inline uint32_t get_gpio_bit(volatile uint32_t *bits, size_t offset)
{
    return get_bit(bits, 1, offset);
}


#define configASSERT(x)                                 \
    if ((x) == 0)                                       \
    {                                                   \
        printf("(%s:%d) %s\r\n", __FILE__, __LINE__, #x); \
        for (;;)                                        \
            ;                                           \
    }
    
void vmprint(pagetable_t pagetable);
void backtrace(void);
void print_map(kmap_t map);
void print_sbiret(sbiret_t ret);
int  luaO_log2 (unsigned int x);
void print_page(int pgnum);

#define get_order(x) luaO_log2(x)


#endif