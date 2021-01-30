#include <ch554.h>
#include <debug.h>
#include "small_print.h"

const char hextab[]="0123456789ABCDEF";

void printhex2(uint8_t h)
{
	putchar(hextab[h>>4]);
	putchar(hextab[h&0xF]);
}

