#include <stdint.h>

#include "ch554.h"
#include "debug.h"

void putchar(char c)
{
    while (!TI); /* assumes UART is initialized */
    TI = 0;
    SBUF = c;
}
