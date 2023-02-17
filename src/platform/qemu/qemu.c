#include "platform/qemu.h"
#include "driver/plic.h"
#include "driver/console.h"
#include "riscv.h"

extern void virtio_disk_init(void);
extern void uartinit(void);

void platform_early_init() {
    // set_csr(sstatus, SSTATUS_FS);

    // enable_sum();
}

void platform_early_init_hart() {
    set_csr(sstatus, SSTATUS_FS);
}

void platform_plic_init() {
    plic_set_priority(VIRTIO0_IRQ, 1);
    plic_set_priority(UART0_IRQ, 1);
}

void platform_plic_init_hart() {
    plic_irq_enable(VIRTIO0_IRQ);
    plic_irq_enable(UART0_IRQ);
}

void platform_dirver_init() {
    virtio_disk_init(); // emulated hard disk
    uartinit();
}
