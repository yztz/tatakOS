#include "printf.h"
#include "utils.h"
#include "debug.h"

volatile int panicked = 0;

void panic(char *s)
{
  printf(rd("panic: "));
  printf(rd("%s"), s);
  printf("\n");
  // printf("backtrace: \n");
  panicked = 1; // freeze uart output from other CPUs
  // sbi_shutdown();
  // shutdown failure
  for(;;)
    ;
}