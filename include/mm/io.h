#ifndef _H_IO_
#define _H_IO_

#include "types.h"
#include "param.h"

#define readb(addr) (*(volatile uint8_t *)(addr))
#define readw(addr) (*(volatile uint16_t *)(addr))
#define readl(addr) (*(volatile uint32_t *)(addr))
#define readq(addr) (*(volatile uint64_t *)(addr))

#define writeb(v, addr)                         \
    {                                           \
        (*(volatile uint8_t *)(addr)) = (v);    \
    }
#define writew(v, addr)                         \
    {                                           \
        (*(volatile uint16_t *)(addr)) = (v);   \
    }
#define writel(v, addr)                         \
    {                                           \
        (*(volatile uint32_t *)(addr)) = (v);   \
    }
#define writeq(v, addr)                         \
    {                                           \
        (*(volatile uint64_t *)(addr)) = (v);   \
    }
/* Set bits that v sets */
#define setb(v, addr)                           \
    {                                           \
        (*(volatile uint8_t *)(addr)) |= (v);   \
    }
#define setw(v, addr)                           \
    {                                           \
        (*(volatile uint16_t *)(addr)) |= (v);  \
    }
#define setl(v, addr)                           \
    {                                           \
        (*(volatile uint32_t *)(addr)) |= (v);  \
    }
#define setq(v, addr)                           \
    {                                           \
        (*(volatile uint64_t *)(addr)) |= (v);  \
    }
    
/* Clear bits that v sets */
#define clearb(v, addr)                         \
    {                                           \
        (*(volatile uint8_t *)(addr)) &= ~(v);  \
    }
#define clearw(v, addr)                         \
    {                                           \
        (*(volatile uint16_t *)(addr)) &= ~(v); \
    }
#define clearl(v, addr)                         \
    {                                           \
        (*(volatile uint32_t *)(addr)) &= ~(v); \
    }
#define clearq(v, addr)                         \
    {                                           \
        (*(volatile uint64_t *)(addr)) &= ~(v); \
    }

struct iovec {
    void  *iov_base;    /* Starting address */
    size_t iov_len;     /* Number of bytes to transfer */
};

extern uint64_t ioremap(uint64_t addr, size_t size);


#endif