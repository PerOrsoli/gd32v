// Copyright © 2020 by P.Orsolic. All right reserved
// Created 200126
// inital use was for misc shell functions

#include "utils.hpp"
#ifdef SHELL
#include "shell-config.hpp"
#else
#define MAX_ARGV 10
#endif // SHELL
#include "libc-bits.h"

uint8_t get_argc(char *argv[])
{
	uint8_t n = 0;
	for (uint8_t i=1; (i<MAX_ARGV && argv[i] != NULL); i++)
	{
		n++;
	}
	return n;
}

// convert string to hex or decimal number
static uint32_t _convert(const char* input, uint8_t base)
{
	dprintf("converting str -> hex: %s base: %d\r\n", input, base);
	char* string_part;
	uint32_t converted = strtol(input, &string_part, base);

	if (strlen(string_part) == 0)	// conversion was successful
	{
		return converted;
	}
	else
	{
		eprintf("error converting: \"%s\" to hex\r\n", input);
		return 0;
	}
}

bool is_hex_string(const char* input)
{
	dprintf("checking if \"%s\" is hex number", input);

	size_t length = strlen(input);
	if (length == 0)
	{
		eprintf("string length is zero\r\n");
		return 0;
	}

	if (!((strncmp(input, "0x", 2) == 0) ||
		(strncmp(input, "0X", 2) == 0)))
	{
		// eprintf("Not hex - missing '0x' prefix\r\n");
		dprintf("Not hex - missing '0x' prefix\r\n");
		return 0;
	}

	// don't compare '0x' prefix
	input += 2;
	length -= 2;
	dprintf("-> new input: %s\r\n", input);

	for (uint8_t i = 0; i < length; i++)
	{
		dprintf("checking if %c is hex digit\r\n", input[i]);
		if (ishexdigit(input[i]) == 0)
		{
			dprintf("NO\r\n");
			return 0;
		}
	}

	dprintf("All good for input: %s\r\n", input);
	return 1;
}

// check if all chars in string are decimal valus which can be converted to (u)int
bool is_dec_string(const char* input)
{
	size_t length = strlen(input);
	if (length == 0)
	{
		eprintf("string length is zero\r\n");
		return 0;
	}

	for (uint8_t i = 0; i < length; i++)
	{
		dprintf("checking if %c is digit\r\n", input[i]);
		if (isdigit(input[i]) == 0)
		{
			return 0;
		}
	}

	dprintf("All good for input: %s\r\n", input);
	return 1;
}

// convert str to dec/hex
// uint32_t str2num(const char* input)
bool str2num(const char* input, uint32_t* num)
{
	if (is_hex_string(input) == 1)
	{
		*num = _convert(input, 16);
		return 0;
	}
	else if (is_dec_string(input) == 1)
	{
		*num = _convert(input, 10);
		return 0;
	}
	else
	{
		eprintf("Can't convert string \"%s\" to digit!\r\n", input);
		return 1;
	}
}

uint8_t cmd_utils(char *argv[])
{
	uint32_t converted;
	if (str2num(argv[1], &converted) == 0)
	{
		printf("converted: 0x%x dec: %d\r\n", converted, converted);
		return 1;
	}
	else
	{
		eprintf("error while converting\r\n");
		return 0;
	}
}
