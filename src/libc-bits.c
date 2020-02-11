// Copyright © 2020 by P.Orsolic. All right reserved
// Created 191130

#undef DEBUG

#include "libc-bits.h"

int abs(int x)
{
	return(x < 0 ? -x : x);
}

// copied from uart.c, written 190928
void * memset(void *dest, int c, size_t len)
{
	uint8_t *dest_current = (uint8_t *)dest;

	if (dest == NULL)
	{
		panic();
	}

	while (len > 0)
	{
		*dest_current = c;
		dest_current++;
		len--;
	}

	return dest;
}

// check if 'c' can be converted to decimal number (0..9)
bool isdigit(char c)		// INFO 200126: non-standard function
{
	if ((c >= 48) && (c <= 57))		// 0..9
	{
		dprintf("%c is digit\r\n", c);
		return 1;
	}

	dprintf("%c is not digit\r\n", c);
	return 0;
}

bool ishexdigit(char c)		// INFO 200126: non-standard function
{
	if (((c >= 48) && (c <= 57)) ||	// 0..9
		((c >= 65) && (c <= 70)) ||	// A..F
		((c >= 97) && (c <= 102)))	// a..f
	{
		dprintf("%c hex is digit\r\n", c);
		return 1;
	}

	dprintf("%c is not hex digit\r\n", c);
	return 0;
}

bool isprintable(const char ch)
{
	if ((ch >= 32) && (ch <= 126))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
