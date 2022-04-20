/**
 * This file defines the interface that paltform should implement.
 * In order to compile your platform(assume it's name PLAT_XXX) with kernel:
 * 1. mkdir kernel/platform/PLAT_XXX and code there
 * 2. impl PLAT_XXX.h header file which defines all MACRO symbol below
 * 3. add your PLAT_XXX.h in the platform list below
 * 4. add your platform in the root makefile to enable it  
*/

#ifndef _H_PLATFORM_
#define _H_PLATFORM_

#include "device.h"


/////////////////////////////////////////////////
////////////// platform list here ///////////////
/////////////////////////////////////////////////
#if defined K210
#include "k210.h"
#elif defined QEMU
#include "qemu.h"
#endif
/////////////////////////////////////////////////
////////////// platform list end ////////////////
/////////////////////////////////////////////////

#include "memlayout.h"

/////////////////////////////////////////////////
/////// platform-specific macro list here ///////
/////////////////////////////////////////////////
/* define num of cores */
#ifndef NUM_CORES
#define NUM_CORES
#endif
/* define PLIC base address */
#ifndef PLIC_BASE_ADDR
#define PLIC_BASE_ADDR
#endif
/* define UART IRQ for console */
#ifndef UART_IRQ
#define UART_IRQ
#endif
#ifndef KERNBASE
#define KERNBASE
#endif
#ifndef PHYSTOP
#define PHYSTOP
#endif
#ifndef CLOCK_FREQ
#define CLOCK_FREQ
#endif

/* supported source */
enum _plic_irq;
/////////////////////////////////////////////////
//////// platform-specific macro list end ///////
/////////////////////////////////////////////////


/////////////////////////////////////////////////
///// platform should impl interfaces below /////
/////////////////////////////////////////////////
/* PLIC */
extern void platform_plic_init(); // execute once
extern void platform_plic_init_hart(); // execute by every hart
/* DIRVER */
extern void platform_dirver_init();
/////////////////////////////////////////////////
//////////// platform interface end /////////////
/////////////////////////////////////////////////

#endif