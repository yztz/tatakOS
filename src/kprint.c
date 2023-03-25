#include "atomic/spinlock.h"
#include "driver/console.h"
#include "printf.h"
#include "utils.h"

volatile int panicked = 0;

/* used by printf */
void _putchar(char c) {
    if (panicked) {
        LOOP();
    }
    
    console_putchar_sync(c);
}

#if KERNEL_OUTPUT_LOCK == 1
static SPINLOCK_INIT(kout_lock);
#endif

int kprintf(const char *format, ...) {
    va_list va;
    va_start(va, format);

#if KERNEL_OUTPUT_LOCK == 1
    // TODO: re-entry lock
    int held = holding(&kout_lock);
    if (!held) acquire(&kout_lock);
#endif

    const int ret = vprintf(format, va);

#if KERNEL_OUTPUT_LOCK == 1
    if (!held) release(&kout_lock);
#endif

    va_end(va);

    return ret;
}


void panic(char *s) {
  printf(rd("panic: "));
  printf(rd("%s"), s);
  printf("\n");
  panicked = 1; // freeze uart output from other CPUs
  // sbi_shutdown();
  // shutdown failure
  for(;;)
    ;
}