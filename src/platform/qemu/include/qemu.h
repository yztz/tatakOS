#ifndef _H_PLATFORM_QEMU
#define _H_PLATFORM_QEMU

#ifndef CPUS
#error CPUS NOT DEFINED!
#endif

#define NUM_CORES CPUS
#define PLIC_BASE_ADDR (0x0C000000L)

#define UART 0x10000000L
#define VIRTIO0 0x10001000

#define KERNBASE 0x80200000L
#define PHYSTOP (KERNBASE + 128*1024*1024)

#define CLOCK_FREQ 1000000

#define CLINT 0x2000000L
#define CLINT_MTIME (CLINT + 0xBFF8)

#define PLIC_MODE S_MODE

/* supported source is platform-specific */
enum _plic_irq {
    VIRTIO0_IRQ = 1,
    // ...
    UART0_IRQ = 10,
    IRQN_MAX
};

#define UART_IRQ UART0_IRQ



#endif