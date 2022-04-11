// #include <stdio.h>
#include "fpioa.h"
#include "sysctl.h"
// #include "dmac.h"
#include "fpioa.h"
#include "sdcard.h"
#include "defs.h"
#include "ff.h"
// #include "i2s.h"
// #include "plic.h"
// #include "wav_decode.h"
// #include "uarths.h"
// #include "bsp.h"

static int sdcard_test(void);
static int fs_test(void);

void io_mux_init(void)
{
    fpioa_set_function(27, FUNC_SPI0_SCLK);
    fpioa_set_function(28, FUNC_SPI0_D0);
    fpioa_set_function(26, FUNC_SPI0_D1);
	// fpioa_set_function(32, FUNC_GPIOHS7);
    // 注意比赛平台片选功能为SS0！！！
    fpioa_set_function(29, FUNC_SPI0_SS3);

    // fpioa_set_function(33, FUNC_I2S0_OUT_D0);
    // fpioa_set_function(35, FUNC_I2S0_SCLK);
    // fpioa_set_function(34, FUNC_I2S0_WS);

}

int sd(void)
{
    // sysctl_pll_set_freq(SYSCTL_PLL0, 320000000UL);
    // sysctl_pll_set_freq(SYSCTL_PLL1, 160000000UL);
    // sysctl_pll_set_freq(SYSCTL_PLL2, 45158400UL);
    fpioa_init();
    io_mux_init();
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
    uint8_t status;

    printf("/******************sdcard test*****************/\n");
    status = sd_init();
    printf("sd init %d\n", status);
    if (status != 0)
    {
        return status;
    }

    printf("card info status %d\n", status);
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