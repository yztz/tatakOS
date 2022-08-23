#include "k210.h"
#include "driver/plic.h"
#include "fpioa.h"
#include "spi.h"
#include "sbi.h"
#include "printf.h"
#include "uarths.h"
#include "sysctl.h"
#include "gpiohs.h"
#include "dmac.h"
#include "wdt.h"

#include "sdcard.h"

#define __MODULE_NAME__ k210
#include "debug.h"


void platform_early_init() {
    /* 开启8M内存 */
    sysctl_pll_enable(SYSCTL_PLL1);
    sysctl_clock_enable(SYSCTL_CLOCK_PLL1);

    // enable_sum();
    // set_csr(sstatus, SSTATUS_SUM);
}

void platform_early_init_hart() {
    
}

void platform_plic_init() {
    plic_set_priority(UART_IRQ, 1);
    plic_set_priority(IRQN_DMA0_INTERRUPT, 1);
    plic_set_priority(IRQN_WDT0_INTERRUPT, 1);
}


#define PLIC_PRIORITY (PLIC_BASE_ADDR + 0x0)
#define PLIC_PENDING (PLIC_BASE_ADDR + 0x1000)
#define PLIC_MENABLE(hart) (PLIC_BASE_ADDR + 0x2000 + (hart)*0x100)
#define PLIC_SENABLE(hart) (PLIC_BASE_ADDR + 0x2080 + (hart)*0x100)
#define PLIC_MPRIORITY(hart) (PLIC_BASE_ADDR + 0x200000 + (hart)*0x2000)
#define PLIC_SPRIORITY(hart) (PLIC_BASE_ADDR + 0x201000 + (hart)*0x2000)
#define PLIC_MCLAIM(hart) (PLIC_BASE_ADDR + 0x200004 + (hart)*0x2000)
#define PLIC_SCLAIM(hart) (PLIC_BASE_ADDR + 0x201004 + (hart)*0x2000)

void platform_plic_init_hart() {
    plic_irq_enable(UART_IRQ);
    plic_irq_enable(IRQN_DMA0_INTERRUPT);
    plic_irq_enable(IRQN_WDT0_INTERRUPT);
}

void platform_dirver_init() {
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
