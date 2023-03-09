#include "utils.h"
#include "driver/console.h"

volatile int panicked = 0;

/* console.c */
extern console_io_op_t console_op;

/* used by printf */
void _putchar(char c) {
    if (panicked) {
        LOOP();
    }
    
    console_op.console_putchar_sync(c);
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