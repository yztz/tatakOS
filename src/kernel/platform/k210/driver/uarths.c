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

#include <stdint.h>
#include "uarths.h"
#include "sysctl.h"

volatile uarths_t *const uarths = (volatile uarths_t *)UARTHS_BASE_ADDR;

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

// int uarths_putchar(char c)
// {
//     while (uarths->txdata.full)
//         continue;
//     uarths->txdata.data = (uint8_t)c;

//     return (c & 0xff);
// }

// int uarths_getchar(void)
// {
//     /* while not empty */
//     uarths_rxdata_t recv = uarths->rxdata;

//     if (recv.empty)
//         return EOF;
//     else
//         return (recv.data & 0xff);
// }

// /* [Deprecated] this function will remove in future */
// int uarths_getc(void) __attribute__ ((weak, alias ("uarths_getchar")));

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

// int uarths_puts(const char *s)
// {
//     while (*s)
//         if (uarths_putchar(*s++) != 0)
//             return -1;
//     return 0;
// }

// void uarths_init(void)
// {
//     uint32_t freq = sysctl_clock_get_freq(SYSCTL_CLOCK_CPU);
//     uint16_t div = freq / 115200 - 1;

//     /* Set UART registers */
//     uarths->div.div = div;
//     uarths->txctrl.txen = 1;
//     uarths->rxctrl.rxen = 1;
//     uarths->txctrl.txcnt = 0;
//     uarths->rxctrl.rxcnt = 0;
//     uarths->ip.txwm = 1;
//     uarths->ip.rxwm = 1;
//     uarths->ie.txwm = 0;
//     uarths->ie.rxwm = 1;
// }

// void uarths_config(uint32_t baud_rate, uarths_stopbit_t stopbit)
// {
//     uint32_t freq = sysctl_clock_get_freq(SYSCTL_CLOCK_CPU);
//     uint16_t div = freq / baud_rate - 1;
//     uarths->div.div = div;
//     uarths->txctrl.nstop = stopbit;
// }
