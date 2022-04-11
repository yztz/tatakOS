#ifndef _H_PLIC_
#define _H_PLIC_

#include "platform.h"

typedef enum _plic_irq plic_irq_t;
typedef int (*plic_irq_callback_t)(void *ctx);

typedef enum _plic_irq_mode_t {
    S_MODE = 0x01, M_MODE = 0x10
} plic_irq_mode_t;

void plic_init(void);
void plic_init_hart(void);

/* 设置中断源的优先级 */
void plic_set_priority(plic_irq_t source, uint32_t priority);

/* 开关当前线程指定mode的中断源 */
void plic_irq_enable(plic_irq_t source, plic_irq_mode_t mode);
void plic_irq_disable(plic_irq_t source, plic_irq_mode_t mode);
/* 设置当前线程指定mode的优先级阈值 */
void plic_set_threshold(uint32_t threshould, plic_irq_mode_t mode);

int plic_claim(void);
void plic_complete(plic_irq_t source);

void plic_register_handler(plic_irq_t source, plic_irq_callback_t callback, void *ctx);

#endif