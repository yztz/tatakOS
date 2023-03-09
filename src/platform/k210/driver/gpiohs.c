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
#include "gpiohs.h"
#include "utils.h"
#include "fpioa.h"
#include "mm/io.h"
#include "bit.h"

#define GPIOHS_MAX_PINNO 32

volatile gpiohs_t *gpiohs = (volatile gpiohs_t*)GPIOHS_BASE_ADDR;


void gpiohs_init() {
    gpiohs = (volatile gpiohs_t *)ioremap((uint64_t)gpiohs, 0x1000);
}


void gpiohs_set_pin(uint8_t pin, gpio_pin_value_t value)
{
    configASSERT(pin < GPIOHS_MAX_PINNO);
    set_gpio_bit(gpiohs->output_val.u32, pin, value);
}

void gpiohs_set_drive_mode(uint8_t pin, gpio_drive_mode_t mode)
{
    configASSERT(pin < GPIOHS_MAX_PINNO);
    int io_number = fpioa_get_io_by_function(FUNC_GPIOHS0 + pin);
    configASSERT(io_number >= 0);

    fpioa_pull_t pull;
    uint32_t dir;

    switch (mode)
    {
    case GPIO_DM_INPUT:
        pull = FPIOA_PULL_NONE;
        dir = 0;
        break;
    case GPIO_DM_INPUT_PULL_DOWN:
        pull = FPIOA_PULL_DOWN;
        dir = 0;
        break;
    case GPIO_DM_INPUT_PULL_UP:
        pull = FPIOA_PULL_UP;
        dir = 0;
        break;
    case GPIO_DM_OUTPUT:
        pull = FPIOA_PULL_DOWN;
        dir = 1;
        break;
    default:
        configASSERT(!"GPIO drive mode is not supported.") break;
    }

    fpioa_set_io_pull(io_number, pull);
    volatile uint32_t *reg = dir ? gpiohs->output_en.u32 : gpiohs->input_en.u32;
    volatile uint32_t *reg_d = !dir ? gpiohs->output_en.u32 : gpiohs->input_en.u32;
    set_gpio_bit(reg_d, pin, 0);
    set_gpio_bit(reg, pin, 1);
}