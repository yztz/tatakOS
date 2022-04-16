#include "k210.h"
#include "driver/plic.h"
#include "fpioa.h"
#include "sbi.h"
#include "printf.h"
#include "uarths.h"
#include "sysctl.h"

#include "sdcard.h"
#include "ff.h"

int test(void);

static int sd_status = 0xFF;

extern void uarths_enable_irq(uarths_interrupt_mode_t interrupt_mode);

void platform_plic_init() {
    plic_set_priority(UART_IRQ, 1);
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
}

void platform_dirver_init() {
    /* SYSCTL */
    sysctl_init();
    /* FPIOA */
    fpioa_init();
        
    /* SDCARD */
    sd_status = sd_init();

    /* TEST */
    test();
    

}

static int sdcard_test(void);
static int fs_test(void);
int test(void)
{
    // sysctl_pll_set_freq(SYSCTL_PLL0, 320000000UL);
    // sysctl_pll_set_freq(SYSCTL_PLL1, 160000000UL);
    // sysctl_pll_set_freq(SYSCTL_PLL2, 45158400UL);
    // dmac_init();
    // plic_init();
    // sysctl_enable_irq();
    if(sdcard_test())
    {
        printf("SD card err\n");
        return -1;
    }
    if(fs_test())
    {
        printf("FAT32 err\n");
        return -1;
    }

    return 0;
}

static int sdcard_test(void)
{

    printf("/******************sdcard test*****************/\n");
    printf("sd init %d\n", sd_status);
    if (sd_status != 0)
    {
        return sd_status;
    }

    printf("card info status %d\n", sd_status);
    printf("CardCapacity:%ld\n", cardinfo.CardCapacity);
    printf("CardBlockSize:%d\n", cardinfo.CardBlockSize);
    return 0;
}

static int fs_test(void)
{
    static FATFS sdcard_fs;
    FRESULT status;
    DIR dj;
    FILINFO fno;

    printf("/********************fs test*******************/\n");
    status = f_mount(&sdcard_fs, _T("0:"), 1);
    printf("mount sdcard:%d\n", status);
    if (status != FR_OK)
        return status;

    printf("printf filename\n");
    status = f_findfirst(&dj, &fno, _T("0:"), _T("*"));
    while (status == FR_OK && fno.fname[0]) {
        if (fno.fattrib & AM_DIR)
            printf("dir:%s\n", fno.fname);
        else
            printf("file:%s\n", fno.fname);
        status = f_findnext(&dj, &fno);
    }
    f_closedir(&dj);
    return 0;
}