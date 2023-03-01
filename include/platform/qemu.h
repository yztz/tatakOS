#ifndef _H_PLATFORM_QEMU
#define _H_PLATFORM_QEMU

#define PRIVILEGE_VERSION PRIVILEGE_VERSION_1_12

#ifndef CPUS
#error CPUS NOT DEFINED!
#endif

#define NUM_CORES CPUS
#define PLIC_BASE_ADDR (0x0C000000L)

#define UART0 0x10000000L
#define VIRTIO0 0x10001000

#define KERN_BASE 0x80200000L
#define MEM_START (0x80000000)
#define MEM_END (MEM_START + 64*1024*1024)

// #define CLOCK_FREQ 4000000
// #define CLOCK_FREQ 100000
#define CLOCK_FREQ 10000000

#define CLINT 0x2000000L
#define CLINT_MTIME (CLINT + 0xBFF8)

#define PLIC_MODE S_MODE

/* supported source is platform-specific */
enum plic_irq {
    VIRTIO0_IRQ = 1,
    // ...
    UART0_IRQ = 10,
    IRQN_MAX
};




#endif