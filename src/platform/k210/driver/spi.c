/* Copyright 2018 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "platform/k210.h"
#include "spi.h"
#include "fpioa.h"
#include "common.h"
#include "gpiohs.h"
#include "sysctl.h"
#include "atomic/spinlock.h"
#include "mm/io.h"
#include "mm/vm.h"
#include "bit.h"

uint64_t spi_pa[4] =
{
    SPI0_BASE_ADDR,
    SPI1_BASE_ADDR,
    SPI_SLAVE_BASE_ADDR,
    SPI3_BASE_ADDR
};

volatile spi_t *spi[4];

#define SPI_DR(spi_num) ((void *)(((spi_t *)spi_pa[(spi_num)])->dr))

static spi_frame_format_t spi_get_frame_format(spi_device_num_t spi_num)
{
    uint8_t frf_offset;
   switch(spi_num)
   {
       case 0:
       case 1:
           frf_offset = 21;
           break;
       case 2:
           configASSERT(!"Spi Bus 2 Not Support!");
           break;
       case 3:
       default:
           frf_offset = 22;
           break;
   }
   volatile spi_t *spi_adapter = spi[spi_num];
   return ((spi_adapter->ctrlr0 >> frf_offset) & 0x3);
}

static spi_transfer_width_t spi_get_frame_size(size_t data_bit_length)
{
    if (data_bit_length < 8)
        return SPI_TRANS_CHAR;
    else if (data_bit_length < 16)
        return SPI_TRANS_SHORT;
    return SPI_TRANS_INT;
}


static int spi_clk_init(uint8_t spi_num)
{
    configASSERT(spi_num < SPI_DEVICE_MAX && spi_num != 2);
    if(spi_num == 3)
        sysctl_clock_set_clock_select(SYSCTL_CLOCK_SELECT_SPI3, 1);
    sysctl_clock_enable(SYSCTL_CLOCK_SPI0 + spi_num);
    sysctl_clock_set_threshold(SYSCTL_THRESHOLD_SPI0 + spi_num, 0);
    return 0;
}

static void spi_set_tmod(uint8_t spi_num, uint32_t tmod)
{
    configASSERT(spi_num < SPI_DEVICE_MAX);
    volatile spi_t *spi_handle = spi[spi_num];
    uint8_t tmod_offset = 0;
    switch(spi_num)
    {
        case 0:
        case 1:
        case 2:
            tmod_offset = 8;
            break;
        case 3:
        default:
            tmod_offset = 10;
            break;
    }
    set_bit_mask(&spi_handle->ctrlr0, 3 << tmod_offset, tmod << tmod_offset);
}

void spi_io_init(spi_device_num_t spi_num) {
    spi[spi_num] = (volatile spi_t *)ioremap(spi_pa[spi_num], 0x1000000);
}

void spi_init(spi_device_num_t spi_num, spi_work_mode_t work_mode, spi_frame_format_t frame_format,
              size_t data_bit_length, uint32_t endian)
{
    configASSERT(data_bit_length >= 4 && data_bit_length <= 32);
    configASSERT(spi_num < SPI_DEVICE_MAX && spi_num != 2);
    spi_clk_init(spi_num); // 初始化时钟

    uint8_t dfs_offset, frf_offset, work_mode_offset;
    switch(spi_num)
    {
        case 0:
        case 1:
            dfs_offset = 16;
            frf_offset = 21;
            work_mode_offset = 6;
            break;
        case 2:
            configASSERT(!"Spi Bus 2 Not Support!");
            break;
        case 3:
        default:
            dfs_offset = 0;
            frf_offset = 22;
            work_mode_offset = 8;
            break;
    }

    switch (frame_format)
    {
        case SPI_FF_DUAL:
            configASSERT(data_bit_length % 2 == 0);
            break;
        case SPI_FF_QUAD:
            configASSERT(data_bit_length % 4 == 0);
            break;
        case SPI_FF_OCTAL:
            configASSERT(data_bit_length % 8 == 0);
            break;
        default:
            break;
    }
    volatile spi_t *spi_adapter = spi[spi_num];
    if(spi_adapter->baudr == 0)
        spi_adapter->baudr = 0x14;
    spi_adapter->imr = 0x00;
    spi_adapter->dmacr = 0x00;
    spi_adapter->dmatdlr = 0x10;
    spi_adapter->dmardlr = 0x00;
    spi_adapter->ser = 0x00;
    spi_adapter->ssienr = 0x00;
    spi_adapter->ctrlr0 = (work_mode << work_mode_offset) | (frame_format << frf_offset) | ((data_bit_length - 1) << dfs_offset);
    spi_adapter->spi_ctrlr0 = 0;
    spi_adapter->endian = endian;
}


uint32_t spi_set_clk_rate(spi_device_num_t spi_num, uint32_t spi_clk)
{
    uint32_t spi_baudr = sysctl_clock_get_freq(SYSCTL_CLOCK_SPI0 + spi_num) / spi_clk;
    // printf("1 %d\n", sysctl_clock_get_freq(SYSCTL_CLOCK_SPI0 + spi_num));
    if(spi_baudr < 2 )
    {
        spi_baudr = 2;
    }
    else if(spi_baudr > 65534)
    {
        spi_baudr = 65534;
    }
    
    volatile spi_t *spi_adapter = spi[spi_num];
    spi_adapter->baudr = spi_baudr;
    // printf("2 %d\n", sysctl_clock_get_freq(SYSCTL_CLOCK_SPI0 + spi_num));
    return sysctl_clock_get_freq(SYSCTL_CLOCK_SPI0 + spi_num) / spi_baudr;
}

void spi_send_data_normal(spi_device_num_t spi_num, spi_chip_select_t chip_select, const uint8_t *tx_buff, size_t tx_len)
{
    configASSERT(spi_num < SPI_DEVICE_MAX && spi_num != 2);

    size_t index, fifo_len;
    // 设置传输模式
    spi_set_tmod(spi_num, SPI_TMOD_TRANS); 

    volatile spi_t *spi_handle = spi[spi_num];

    uint8_t dfs_offset;
    switch(spi_num)
    {
        case 0:
        case 1:
            dfs_offset = 16;
            break;
        case 2:
            configASSERT(!"Spi Bus 2 Not Support!");
            break;
        case 3:
        default:
            dfs_offset = 0;
            break;
    }
    // 获取数据帧大小
    uint32_t data_bit_length = (spi_handle->ctrlr0 >> dfs_offset) & 0x1F;
    // 计算数据帧宽度（8位对齐）
    spi_transfer_width_t frame_width = spi_get_frame_size(data_bit_length);

    uint8_t v_misalign_flag = 0;
    uint32_t v_send_data;
    if((uintptr_t)tx_buff % frame_width) // 缓存地址不对齐
        v_misalign_flag = 1;

    spi_handle->ssienr = 0x01; // 使能SSI
    spi_handle->ser = 1U << chip_select; // 使能片选SLAVE
    uint32_t i = 0;
    while (tx_len)
    {
        fifo_len = 32 - spi_handle->txflr; // 为什么是32？
        fifo_len = fifo_len < tx_len ? fifo_len : tx_len;
        switch(frame_width)
        {
            case SPI_TRANS_INT:
                fifo_len = fifo_len / 4 * 4;
                if(v_misalign_flag)
                {
                    for(index = 0; index < fifo_len; index +=4)
                    {
                        memcpy(&v_send_data, tx_buff + i , 4);
                        spi_handle->dr[0] = v_send_data;
                        i += 4;
                    }
                }
                else
                {
                    for (index = 0; index < fifo_len / 4; index++)
                        spi_handle->dr[0] = ((uint32_t *)tx_buff)[i++];
                }
                break;
            case SPI_TRANS_SHORT:
                fifo_len = fifo_len / 2 * 2;
                if(v_misalign_flag)
                {
                    for(index = 0; index < fifo_len; index +=2)
                    {
                        memcpy(&v_send_data, tx_buff + i, 2);
                        spi_handle->dr[0] = v_send_data;
                        i += 2;
                    }
                }
                else
                {
                    for (index = 0; index < fifo_len / 2; index++)
                        spi_handle->dr[0] = ((uint16_t *)tx_buff)[i++];
                }
                break;
            default:
                for (index = 0; index < fifo_len; index++)
                    spi_handle->dr[0] = tx_buff[i++];
                break;
        }
        tx_len -= fifo_len;
    }
    while ((spi_handle->sr & 0x05) != 0x04)
        ;
    spi_handle->ser = 0x00;
    spi_handle->ssienr = 0x00;

}

void spi_send_data_standard(spi_device_num_t spi_num, spi_chip_select_t chip_select, const uint8_t *cmd_buff,
                            size_t cmd_len, const uint8_t *tx_buff, size_t tx_len)
{
    configASSERT(spi_num < SPI_DEVICE_MAX && spi_num != 2);
    // uint8_t *v_buf = malloc(cmd_len + tx_len);
    uint8_t *v_buf = kmalloc(cmd_len + tx_len);
    size_t i;
    for(i = 0; i < cmd_len; i++)
        v_buf[i] = cmd_buff[i];
    for(i = 0; i < tx_len; i++)
        v_buf[cmd_len + i] = tx_buff[i];
    // printf("ready to send...\n");
    spi_send_data_normal(spi_num, chip_select, v_buf, cmd_len + tx_len);
    // printf("done...\n");
    // free();
    kfree((void *)v_buf);
}

void spi_send_data_standard_dma(dmac_channel_number_t channel_num, spi_device_num_t spi_num,
                                spi_chip_select_t chip_select,
                                const uint8_t *tx_buff, size_t tx_len)
{
    // static uint32_t buffer[512]; // 2KB
    configASSERT(spi_num < SPI_DEVICE_MAX && spi_num != 2);

    volatile spi_t *spi_handle = spi[spi_num];

    uint8_t dfs_offset;
    switch(spi_num)
    {
        case 0:
        case 1:
            dfs_offset = 16;
            break;
        case 2:
            configASSERT(!"Spi Bus 2 Not Support!");
            break;
        case 3:
        default:
            dfs_offset = 0;
            break;
    }
    uint32_t data_bit_length = (spi_handle->ctrlr0 >> dfs_offset) & 0x1F;
    spi_transfer_width_t frame_width = spi_get_frame_size(data_bit_length);

    // uint32_t *buf;
    size_t v_send_len;
    // int i;
    switch(frame_width)
    {
        case SPI_TRANS_INT:
            // buf = kmalloc(cmd_len + tx_len);
            // for(i = 0; i < cmd_len / 4; i++)
            //     buf[i] = ((uint32_t *)cmd_buff)[i];
            // for(i = 0; i < tx_len / 4; i++)
            //     buf[cmd_len / 4 + i] = ((uint32_t *)tx_buff)[i];
            // v_send_len = (cmd_len + tx_len) / 4;
            // for(i = 0; i < tx_len / 4; i++)
            //     buffer[i] = ((uint32_t *)tx_buff)[i];
            v_send_len = tx_len / 4;
            break;
        case SPI_TRANS_SHORT:
            // buf = kmalloc((cmd_len + tx_len) / 2 * sizeof(uint32_t));
            // for(i = 0; i < cmd_len / 2; i++)
            //     buf[i] = ((uint16_t *)cmd_buff)[i];
            // for(i = 0; i < tx_len / 2; i++)
            //     buf[cmd_len / 2 + i] = ((uint16_t *)tx_buff)[i];
            // v_send_len = (cmd_len + tx_len) / 2;
            // for(i = 0; i < tx_len / 2; i++)
            //     buffer[i] = ((uint16_t *)tx_buff)[i];
            v_send_len = tx_len / 2;
            break;
        default:
            // buf = kmalloc((cmd_len + tx_len) * sizeof(uint32_t));
            // for(i = 0; i < cmd_len; i++)
            //     buf[i] = cmd_buff[i];
            // for(i = 0; i < tx_len; i++)
            //     buf[cmd_len + i] = tx_buff[i];
            // v_send_len = cmd_len + tx_len;
            // for(i = 0; i < tx_len; i++)
            //     buffer[i] = tx_buff[i];
            v_send_len = tx_len;
            break;
    }

    spi_send_data_normal_dma(channel_num, spi_num, chip_select, tx_buff, v_send_len);

    // kfree((void *)buf);
}

void spi_send_data_normal_dma(dmac_channel_number_t channel_num, spi_device_num_t spi_num,
                              spi_chip_select_t chip_select,
                              const void *tx_buff, size_t tx_len)
{
    configASSERT(spi_num < SPI_DEVICE_MAX && spi_num != 2);
    spi_set_tmod(spi_num, SPI_TMOD_TRANS);
    volatile spi_t *spi_handle = spi[spi_num];
    uint32_t *buf = (uint32_t *)tx_buff;
    // int i;
    // switch(spi_transfer_width)
    // {
    //     case SPI_TRANS_SHORT:
    //         buf = kmalloc((tx_len) * sizeof(uint32_t));
    //         for(i = 0; i < tx_len; i++)
    //             buf[i] = ((uint16_t *)tx_buff)[i];
    //     break;
    //     case SPI_TRANS_INT:
    //         buf = (uint32_t *)tx_buff;
    //     break;
    //     case SPI_TRANS_CHAR:
    //     default:
    //         buf = kmalloc((tx_len) * sizeof(uint32_t));
    //         for(i = 0; i < tx_len; i++)
    //             buf[i] = ((uint8_t *)tx_buff)[i];
    //     break;
    // }
    spi_handle->dmacr = 0x2;    /*enable dma transmit*/
    spi_handle->ssienr = 0x01;

    sysctl_dma_select((sysctl_dma_channel_t) channel_num, SYSCTL_DMA_SELECT_SSI0_TX_REQ + spi_num * 2);
    dmac_set_single_mode(channel_num, buf, SPI_DR(spi_num), DMAC_ADDR_INCREMENT, DMAC_ADDR_NOCHANGE,
                                DMAC_MSIZE_4, DMAC_TRANS_WIDTH_32, tx_len);
    spi_handle->ser = 1U << chip_select;

    dmac_wait_done(channel_num);
    // if(spi_transfer_width != SPI_TRANS_INT)
    //     kfree((void *)buf);

    while ((spi_handle->sr & 0x05) != 0x04)
        ;
    spi_handle->ser = 0x00;
    spi_handle->ssienr = 0x00;
}


void spi_receive_data_standard(spi_device_num_t spi_num, spi_chip_select_t chip_select, const uint8_t *cmd_buff,
                               size_t cmd_len, uint8_t *rx_buff, size_t rx_len)
{
    configASSERT(spi_num < SPI_DEVICE_MAX && spi_num != 2);
    size_t index, fifo_len;
    if(cmd_len == 0)
        spi_set_tmod(spi_num, SPI_TMOD_RECV);
    else
        spi_set_tmod(spi_num, SPI_TMOD_EEROM);
    volatile spi_t *spi_handle = spi[spi_num];

    uint8_t dfs_offset;
    switch(spi_num)
    {
        case 0:
        case 1:
            dfs_offset = 16;
            break;
        case 2:
            configASSERT(!"Spi Bus 2 Not Support!");
            break;
        case 3:
        default:
            dfs_offset = 0;
            break;
    }
    uint32_t data_bit_length = (spi_handle->ctrlr0 >> dfs_offset) & 0x1F;
    spi_transfer_width_t frame_width = spi_get_frame_size(data_bit_length);

    uint32_t i = 0;
    size_t v_cmd_len = cmd_len / frame_width;
    uint32_t v_rx_len = rx_len / frame_width;

    spi_handle->ctrlr1 = (uint32_t)(v_rx_len - 1);
    spi_handle->ssienr = 0x01;

    while (v_cmd_len)
    {
        fifo_len = 32 - spi_handle->txflr;
        fifo_len = fifo_len < v_cmd_len ? fifo_len : v_cmd_len;
        switch(frame_width)
        {
            case SPI_TRANS_INT:
                for (index = 0; index < fifo_len; index++)
                    spi_handle->dr[0] = ((uint32_t *)cmd_buff)[i++];
                break;
            case SPI_TRANS_SHORT:
                for (index = 0; index < fifo_len; index++)
                    spi_handle->dr[0] = ((uint16_t *)cmd_buff)[i++];
                break;
            default:
                for (index = 0; index < fifo_len; index++)
                    spi_handle->dr[0] = cmd_buff[i++];
                break;
        }
        spi_handle->ser = 1U << chip_select;
        v_cmd_len -= fifo_len;
    }

    if(cmd_len == 0)
    {
        spi_handle->dr[0] = 0xffffffff;
        spi_handle->ser = 1U << chip_select;
    }

    i = 0;
    while (v_rx_len)
    {
        fifo_len = spi_handle->rxflr;
        fifo_len = fifo_len < v_rx_len ? fifo_len : v_rx_len;
        switch(frame_width)
        {
            case SPI_TRANS_INT:
                for (index = 0; index < fifo_len; index++)
                  ((uint32_t *)rx_buff)[i++] = spi_handle->dr[0];
                break;
            case SPI_TRANS_SHORT:
                for (index = 0; index < fifo_len; index++)
                  ((uint16_t *)rx_buff)[i++] = (uint16_t)spi_handle->dr[0];
                 break;
            default:
                  for (index = 0; index < fifo_len; index++)
                      rx_buff[i++] = (uint8_t)spi_handle->dr[0];
                break;
        }

        v_rx_len -= fifo_len;
    }

    spi_handle->ser = 0x00;
    spi_handle->ssienr = 0x00;
}

void spi_receive_data_normal_dma(dmac_channel_number_t dma_receive_channel_num,
                                  spi_device_num_t spi_num, spi_chip_select_t chip_select,
                                  void *rx_buff, size_t rx_len)
{
    configASSERT(spi_num < SPI_DEVICE_MAX && spi_num != 2);

    spi_set_tmod(spi_num, SPI_TMOD_RECV);

    volatile spi_t *spi_handle = spi[spi_num];

    spi_handle->ctrlr1 = (uint32_t)(rx_len - 1);
    spi_handle->dmacr = 0x3;
    spi_handle->ssienr = 0x01;

    sysctl_dma_select((sysctl_dma_channel_t)dma_receive_channel_num, SYSCTL_DMA_SELECT_SSI0_RX_REQ + spi_num * 2);

    dmac_set_single_mode(dma_receive_channel_num, SPI_DR(spi_num), rx_buff, DMAC_ADDR_NOCHANGE, DMAC_ADDR_INCREMENT,
                           DMAC_MSIZE_1, DMAC_TRANS_WIDTH_32, rx_len);

    if(spi_get_frame_format(spi_num) == SPI_FF_STANDARD)
        spi[spi_num]->dr[0] = 0xffffffff;

    spi_handle->ser = 1U << chip_select;

    dmac_wait_done(dma_receive_channel_num);

    spi_handle->ser = 0x00;
    spi_handle->ssienr = 0x00;
}


void spi_receive_data_standard_dma(dmac_channel_number_t dma_receive_channel_num,
                                   spi_device_num_t spi_num, spi_chip_select_t chip_select, 
                                   uint8_t *rx_buff, size_t rx_len)
{
    // static uint32_t buffer[512]; // 2KB

    configASSERT(spi_num < SPI_DEVICE_MAX && spi_num != 2);
    volatile spi_t *spi_handle = spi[spi_num];

    uint8_t dfs_offset;
    switch(spi_num)
    {
        case 0:
        case 1:
            dfs_offset = 16;
            break;
        case 2:
            configASSERT(!"Spi Bus 2 Not Support!");
            break;
        case 3:
        default:
            dfs_offset = 0;
            break;
    }
    uint32_t data_bit_length = (spi_handle->ctrlr0 >> dfs_offset) & 0x1F;
    spi_transfer_width_t frame_width = spi_get_frame_size(data_bit_length);

    // size_t i;
    // printf("frame width is %d\n", frame_width);
    size_t v_recv_len;

    switch(frame_width)
    {
        case SPI_TRANS_INT:
            v_recv_len = rx_len / 4;
            break;
        case SPI_TRANS_SHORT:
            v_recv_len = rx_len / 2;
            break;
        default:
            v_recv_len = rx_len;
            break;
    }

    spi_receive_data_normal_dma(dma_receive_channel_num, spi_num, chip_select, rx_buff, v_recv_len);
    // spi_receive_data_normal_dma(dma_receive_channel_num, spi_num, chip_select, rx_buff, v_recv_len);
    
    // switch(frame_width)
    // {
    //     case SPI_TRANS_INT:
    //         for(i = 0; i < v_recv_len; i++)
    //             ((uint32_t *)rx_buff)[i] = buffer[i];
    //         break;
    //     case SPI_TRANS_SHORT:
    //         for(i = 0; i < v_recv_len; i++)
    //             ((uint16_t *)rx_buff)[i] = buffer[i];
    //         break;
    //     default:
    //         for(i = 0; i < v_recv_len; i++)
    //             rx_buff[i] = buffer[i];
    //         break;
    // }

}