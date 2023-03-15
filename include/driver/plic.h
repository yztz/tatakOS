#ifndef _H_PLIC_
#define _H_PLIC_

#include "platform.h"

typedef void (*plic_irq_callback_t)(void *ctx);

/* 设置中断源的优先级 */
void plic_set_priority(plic_irq_t source, uint32_t priority);

/* 开关当前线程指定mode的中断源 */
void plic_irq_enable(plic_irq_t source);
void plic_irq_disable(plic_irq_t source);
/* 设置当前线程指定mode的优先级阈值 */
void plic_set_threshold(uint32_t threshould);

int plic_claim(void);
void plic_complete(plic_irq_t source);

/**
 * @brief Register IRQ handler
 * 
 * @param source IRQ source
 * @param callback IRQ callback
 * @param ctx callback param
 */
void plic_register_handler(plic_irq_t source, plic_irq_callback_t callback, void *ctx);

#endif