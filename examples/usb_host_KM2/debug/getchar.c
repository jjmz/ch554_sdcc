#include <stdint.h>

#include "ch554.h"
#include "debug.h"

char getchar() {
    while(!RI); /* assumes UART is initialized */
    RI = 0;
    return SBUF;
}
