// Copyright © 2020 by P.Orsolic. All right reserved
// Created 190311

#include "debug.h"

#ifdef MCU_RUN
#include "gd32vf103_rcu.h"
#endif // MCU_RUN
#ifdef PC_RUN
#endif // PC_RUN

void assert_early(uint32_t a, uint32_t b)
{
	if (a != b)
	{
		panic();
	}
}

void assert_eq(uint32_t a, uint32_t b)
{
	if (a != b)
	{
		printf(ANSI_COLOR_RED \
				"assert failed [%s:%d] 0x%x != 0x%x\r\n" \
				"assert failed, 0x%x != 0x%x\r\n" ANSI_COLOR_RESET, a, b);
		panic();
	}
}

void assert_eq2(uint32_t a, uint32_t b, const char *file, const uint16_t line)
{
	if (a != b)
	{
		printf(ANSI_COLOR_RED \
				"assert failed [%s:%d] 0x%x != 0x%x\r\n" \
				ANSI_COLOR_RESET, file, line, a, b);
		panic();
	}
}

void assert(uint32_t a, const char *file, const uint16_t line)
{
	if (!a)
	{
		printf(ANSI_COLOR_RED \
				"assert failed [%s:%d]\r\n" \
				ANSI_COLOR_RESET, file, line);
		panic();
	}
}

#ifdef MCU_RUN
void panic(void)
{
	eprintf("PANIC\r\n");

#define LEDR_CLK	RCU_GPIOC
#define LEDR		PC13
#define DELAY		200

	while (1)
	{
		// gpio_toggle(LEDR);
		delay_ms(DELAY);
	}
}

#endif // MCU_RUN
#ifdef PC_RUN
void panic(void)
{
	eprintf("PANIC\r\n");
}
#endif // PC_RUN
