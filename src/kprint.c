#include "utils.h"
#include "driver/console.h"

volatile int panicked = 0;

/* used by printf */
void _putchar(char c) {
    if (panicked) {
        LOOP();
    }
    
    console_putchar_sync(c);
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