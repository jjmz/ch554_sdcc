#include <ch554.h>
#include <debug.h>
#include "small_print.h"

void printhex4(uint16_t i)
{
	printhex2(i>>8);
	printhex2(i&0xFF);
}
