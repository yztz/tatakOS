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

#include "driver/console.h"
#include "driver/plic.h"
#include <stdint.h>
#include "uarths.h"
#include "sysctl.h"
#include "mm/io.h"

void uartintr(void *ctx);
void uarths_putchar(char c);
int uarths_getchar(void);

volatile uarths_t * uarths = NULL;


static console_io_op_t ioop = {
    .console_getchar = uarths_getchar,
    .console_putchar = uarths_putchar,
    .console_putchar_sync = uarths_putchar
};

void uarths_enable_irq(uarths_interrupt_mode_t interrupt_mode)
{
    switch(interrupt_mode)
    {
        case UARTHS_SEND:
            uarths->ie.txwm = 1;
            uarths->ie.rxwm = 0;
            break;
        case UARTHS_RECEIVE:
            uarths->ie.txwm = 0;
            uarths->ie.rxwm = 1;
            break;
        default:
            uarths->ie.txwm = 1;
            uarths->ie.rxwm = 1;
            break;
    }
}

void uarths_puts(const char *s)
{
    while (*s)
        uarths_putchar(*s++);
}

void uarths_putchar(char c)
{
    while (uarths->txdata.full)
        continue;
    uarths->txdata.data = (uint8_t)c;
}

int uarths_getchar(void)
{
    /* while not empty */
    uarths_rxdata_t recv = uarths->rxdata;

    if (recv.empty)
        return -1;
    else
        return (recv.data & 0xff);
}


// size_t uarths_receive_data(uint8_t *buf, size_t buf_len)
// {
//     size_t i;
//     for(i = 0; i < buf_len; i++)
//     {
//         uarths_rxdata_t recv = uarths->rxdata;
//         if(recv.empty)
//             break;
//         else
//             buf[i] = (recv.data & 0xFF);
//     }
//     return i;
// }

// size_t uarths_send_data(const uint8_t *buf, size_t buf_len)
// {
//     size_t write = 0;
//     while (write < buf_len)
//     {
//         uarths_putchar(*buf++);
//         write++;
//     }
//     return write;
// }

void uarths_init(void)
{
    uarths = (volatile uarths_t *)ioremap(UARTHS_BASE_ADDR, PGSIZE);

    uint32_t freq = sysctl_clock_get_freq(SYSCTL_CLOCK_CPU);
    uint16_t div = freq / 115200 - 1;

    /* Set UART registers */
    uarths->div.div = div;
    uarths->txctrl.txen = 1;
    uarths->rxctrl.rxen = 1;
    uarths->txctrl.txcnt = 0;
    uarths->rxctrl.rxcnt = 0;
    uarths->ip.txwm = 1;
    uarths->ip.rxwm = 1;
    uarths->ie.txwm = 0;
    uarths->ie.rxwm = 1;

    uarths_enable_irq(UARTHS_RECEIVE);

    plic_register_handler(IRQN_UARTHS_INTERRUPT, uartintr, NULL);

    console_register(&ioop);
    uarths_puts("switch to UARTHS driver\n");
}


void uartintr(void *ctx)
{
  // read and process incoming characters.
  while (1)
  {
    int c = uarths_getchar();
    if (c == -1)
      break;
    console_intr_callback(c);
  }

}

// void uarths_config(uint32_t baud_rate, uarths_stopbit_t stopbit)
// {
//     uint32_t freq = sysctl_clock_get_freq(SYSCTL_CLOCK_CPU);
//     uint16_t div = freq / baud_rate - 1;
//     uarths->div.div = div;
//     uarths->txctrl.nstop = stopbit;
// }

