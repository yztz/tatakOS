#include "sys.h"
#include "defs.h"

uint64 sys_timetag(void) {
    uint64 addr;
    argaddr(0, &addr);
    return read_csr(cycle);
}


uint64 sys_ktest(void) {
  // sbi_putchar('H');
  // sbi_putchar('E');
  // sbi_putchar('L');
  // sbi_putchar('L');
  // sbi_putchar('O');
  // sbi_putchar('W');
  // sbi_putchar(' ');
  // sbi_putchar('W');
  // sbi_putchar('O');
  // sbi_putchar('R');
  // sbi_putchar('L');
  // sbi_putchar('D');
  // sbi_putchar('\n');
  return 0;
}