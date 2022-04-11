#include "platform.h"
#include "driver/plic.h"
#include "sbi.h"

extern void virtio_disk_init(void);

void platform_plic_init() {
    plic_set_priority(VIRTIO0_IRQ, 1);
    plic_set_priority(UART0_IRQ, 1);
}

void platform_plic_init_hart() {
    plic_irq_enable(VIRTIO0_IRQ, S_MODE);
    plic_irq_enable(UART0_IRQ, S_MODE);
}

void platform_dirver_init() {
    virtio_disk_init(); // emulated hard disk
}

