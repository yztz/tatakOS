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
#include "riscv.h"

#define PRIVILEGE_VERSION_1_9  1
#define PRIVILEGE_VERSION_1_12 2

/////////////////////////////////////////////////
////////////// platform list here ///////////////
/////////////////////////////////////////////////
#if defined K210
#include "platform/k210.h"
#elif defined QEMU
#include "platform/qemu.h"
#else
#error "No platform"
#endif
/////////////////////////////////////////////////
////////////// platform list end ////////////////
/////////////////////////////////////////////////


/////////////////////////////////////////////////
/////// platform-specific macro list here ///////
/////////////////////////////////////////////////
#ifndef PRIVILEGE_VERSION
#define PRIVILEGE_VERSION -1
#endif
/* define num of cores */
#ifndef NUM_CORES
#define NUM_CORES -1
#endif
/* define PLIC base address */
#ifndef PLIC_BASE_ADDR
#define PLIC_BASE_ADDR
#endif
/* define memory */
#ifndef KERN_BASE
#define KERN_BASE
#endif
#ifndef MEM_START
#define MEM_START
#endif
#ifndef MEM_END
#define MEM_END
#endif
// Hz
#ifndef CLOCK_FREQ
#define CLOCK_FREQ
#endif

/* supported source */
typedef enum plic_irq plic_irq_t;
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
extern void platform_early_init();
extern void platform_early_init_hart();
extern void platform_dirver_init();
/////////////////////////////////////////////////
//////////// platform interface end /////////////
/////////////////////////////////////////////////

#endif