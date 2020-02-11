// Copyright © 2020 by P.Orsolic. All right reserved
// Created 200101

#ifndef EXTI_H
#define EXTI_H

#include <stdint.h>
#include "debug.h"

namespace exti
{

enum class Mode: uint8_t
{
	Interrupt,	// normal interrupt
	Event,		// event can wake up core from sleep
	Both,
};

enum class Edge: uint8_t
{
	Rising,
	Falling,
	Both,
};

enum class Source: uint8_t
{
	Pin0 = 0,
	Pin1,
	Pin2,
	Pin3,
	Pin4,
	Pin5,
	Pin6,
	Pin7,
	Pin8,
	Pin9,
	Pin10,
	Pin11,
	Pin12,
	Pin13,
	Pin14,
	Pin15,
	LVD,	// Low voltage drop
	RTC,
	USB_wakeup,
};

void init(Source pin, Mode mode, Edge edge);
void interrupt_flag_clear(Source pin);
bool interrupt_flag_get(Source pin);
void interrupt_sw(Source pin, bool state);
void test(void);

}	// namespace exti

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

// call C function from C++:
EXTERNC void exti_test_sw_interrupt(void);

#endif	// EXTI_H
