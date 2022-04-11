#ifndef _H_PLATFORM_
#define _H_PLATFORM_

#include "device.h"
// platform list here
///////////////////////
#if defined K210
#include "k210.h"
#elif defined QEMU
#include "qemu.h"

#endif
///////////////////////

#include "memlayout.h"

/* define num of cores */
#ifndef NUM_CORES
#error NUM_CORES NOT DEFINED!
#endif
/* define PLIC base address */
#ifndef PLIC_BASE_ADDR
#error PLIC_BASE_ADDR NOT DEFINED!
#endif
/* define UART for console */
#ifndef UART
#error UART NOT DEFINED!
#endif
#ifndef UART_IRQ
#error UART_IRQ NOT DEFINED!
#endif
/* supported source */
enum _plic_irq;



///// platform should impl interfaces below /////
/* PLIC */
extern void platform_plic_init(); // execute once
extern void platform_plic_init_hart(); // execute by every hart
/* DIRVER */
extern void platform_dirver_init();
//////////// platform interface end /////////////

#endif