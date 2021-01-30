#include <ch554.h>
#include <debug.h>
#include "small_print.h"

void printstr(__code const char *ptr)
{
	char c;

	while (c=*ptr++)
		putchar(c);
}

