#include "platform/k210.h"
#include "driver/plic.h"
#include "fpioa.h"
#include "spi.h"
#include "sbi.h"
#include "uarths.h"
#include "sysctl.h"
#include "gpiohs.h"
#include "dmac.h"
#include "wdt.h"
#include "riscv.h"

#include "sdcard.h"

#define __MODULE_NAME__ k210
#include "debug.h"

extern void uarths_init0(void);
extern void uarths_init1(void);


void platform_early_init() {
    /* 开启8M内存 */
    // sysctl_pll_enable(SYSCTL_PLL1);
    // sysctl_clock_enable(SYSCTL_CLOCK_PLL1);
    /* uart init */
    uarths_init0();
}

void platform_early_init_hart() {
    // for 'current'
    write_csr(sscratch, 0);
}

void platform_plic_init() {
    plic_set_priority(IRQN_UARTHS_INTERRUPT, 1);
    plic_set_priority(IRQN_DMA0_INTERRUPT, 1);
    plic_set_priority(IRQN_WDT0_INTERRUPT, 1);
}


void platform_plic_init_hart() {
    plic_irq_enable(IRQN_UARTHS_INTERRUPT);
    plic_irq_enable(IRQN_DMA0_INTERRUPT);
    plic_irq_enable(IRQN_WDT0_INTERRUPT);
}

void platform_dirver_init() {
    uarths_init1();
    /* SYSCTL */
    sysctl_init();
    debug("sysctl init success!");
    
    /* FPIOA */
    fpioa_init();
    debug("fpioa init success!"); 

    /* GPIOHS */
    // gpiohs_init();

    /* SPI */
    spi_io_init(SPI_DEVICE_0);
    debug("spi init success!");

    /* DMA */
    dmac_init();
    debug("dmac init success!");

    sysctl_clock_enable(SYSCTL_CLOCK_APB1);
    sysctl_clock_set_threshold(SYSCTL_THRESHOLD_SPI0, 0);
    /* SDCARD */
    if(sd_init() != 0) 
        panic("sd init fail");
    debug("sdcard init success!");

    /* Watch Dog */
    // int delay = wdt_init(10000);
    // debug("WDT timeout: %d", delay);
    // wdt_feed();
    // wdt_start();

    debug("driver init success!");
}
