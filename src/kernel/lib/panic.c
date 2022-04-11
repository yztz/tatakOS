#include "defs.h"

volatile int panicked = 0;

void panic(char *s)
{
  printf("panic: ");
  printf(s);
  printf("\n");
  panicked = 1; // freeze uart output from other CPUs
  for(;;)
    ;
}