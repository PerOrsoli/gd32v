// Copyright © 2020 by P.Orsolic. All right reserved
// Created 200128
#ifndef RTC_H
#define RTC_H

#include <stdint.h>
#include "debug.h"
#include "gpio.hpp"

namespace rtc
{

enum class Interrupt
{
	// bits in INTEN
	Overflow = 2,	// OVIE
	Alarm = 1,		// ALRMIE
	Second = 0,		// SCIE
};

void test(void);
uint32_t get_counter(void);
void write(uint32_t counter);

void example(void);
uint8_t cmd(char *argv[]);

} // namespace

#endif // RTC_H
