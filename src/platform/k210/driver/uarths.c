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
#include "types.h"
#include "uarths.h"
#include "sysctl.h"
#include "mm/io.h"
#include "mm/page.h"
#include "atomic/spinlock.h"

void uartintr(void *ctx);
void uarths_putchar_sync(char c);
int uarths_getchar(void);

volatile uarths_t * uarths = (volatile uarths_t *)UARTHS_BASE_ADDR;

// static spinlock_t uart_tx_lock;

void uarths_enable_irq(uarths_interrupt_mode_t interrupt_mode) {
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

void uarths_puts(const char *s) {
    while (*s)
        uarths_putchar_sync(*s++);
}

void uarths_putchar_sync(char c) {
    push_off();
    while (uarths->txdata.full)
        ;
    uarths->txdata.data = (uint8_t)c;
    pop_off();
}

int uarths_getchar(void) {
    /* while not empty */
    uarths_rxdata_t recv = uarths->rxdata;

    if (recv.empty)
        return -1;
    else
        return (recv.data & 0xff);
}



void uarths_init0(void) {

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

    // initlock(&uart_tx_lock, "uarths");

}

void uarths_init1(void) {
    uarths = (volatile uarths_t *)ioremap(UARTHS_BASE_ADDR, PGSIZE);
    
    uarths_enable_irq(UARTHS_RECEIVE);

    plic_register_handler(IRQN_UARTHS_INTERRUPT, uartintr, NULL);

    uarths_puts("uarths init successfully\n");
}


void uartintr(void *ctx) {
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

void console_putchar(char c) {
    uarths_putchar_sync(c);
}

void console_putchar_sync(char c) {
    uarths_putchar_sync(c);
}

int console_getchar() {
    return uarths_getchar();
}
