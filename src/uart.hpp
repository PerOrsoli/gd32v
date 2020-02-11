// Copyright © 2020 by P.Orsolic. All right reserved
// Created 191229

#ifndef UART_H
#define UART_H

#include <stdint.h>
#include "debug.h"
#include "gd32vf103_rcu.h"
#include "gpio.hpp"

#define UART0_TX	gpio::PA9
#define UART0_RX	gpio::PA10

namespace uart
{
	enum class Uart: uint8_t
	{
		Uart0,
		Uart1,
		Uart2,
		Uart3,
		Uart4,
	};

	enum class Speed
	{
		speed115200		= 115200,
		speed230400		= 230400,
		speed460800		= 460800,
		speed921600		= 921600,
	};

	// index for ModeMaps[]
	enum class Mode
	{
		EightNoneOne = 0,	// 8N1
		// other values will probably never be used
	};

void init2(Uart uart, Speed speed, Mode mode);
void clear(void);
void clear_rx_buffer(void);
void test(void);

}	// namespace uart

// C functions:
extern "C" const uint8_t uart1_get_rx1(void);
void _putchar(char ch);

#endif // UART_H
