#include "sbi.h"

/* used by printf */
void _putchar(char c) {
    /* Always use sbi */
    sbi_putchar(c);
}
