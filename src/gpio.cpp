// Copyright © 2020 by P.Orsolic. All right reserved
// Created 191227
// HW specific implementation

#include "debug.h"
#include "gd32vf103_rcu.h"
#include "gpio_hw.hpp"

namespace gpio
{

void gpio_set(GpioPin pin)
{
	GpioReg* reg = GpioPins[pin].reg;
	uint8_t  bit = GpioPins[pin].bit;
	// no need to read then write BOP/BC port
	reg->BOP = (1 << bit);
}

void gpio_reset(GpioPin pin)
{
	GpioReg* reg = GpioPins[pin].reg;
	uint8_t  bit = GpioPins[pin].bit;
	// no need to read then write BOP/BC port
	reg->BC = (1 << bit);
}

void gpio_toggle(GpioPin pin)
{
	GpioReg* reg = GpioPins[pin].reg;
	uint8_t  bit = GpioPins[pin].bit;
	reg->OCTL ^= (1 << bit);
}

bool gpio_get(GpioPin pin)
{
	GpioReg* reg = GpioPins[pin].reg;
	uint8_t  bit = GpioPins[pin].bit;

	return ((reg->ISTAT & (1 << bit)) >> bit);
}

// get state of pin which is used as output
bool gpio_get_out(GpioPin pin)
{
	GpioReg* reg = GpioPins[pin].reg;
	uint8_t  bit = GpioPins[pin].bit;

	return ((reg->OCTL & (1 << bit)) >> bit);
}

void gpio_init2(GpioPin pin, Mode mode, Speed speed)
{
	GpioReg* reg = GpioPins[pin].reg;
	uint8_t  bit = GpioPins[pin].bit;
	uint8_t bit2 = 0;	// how much to shift for OCTL register
	volatile uint32_t *CTLx = &reg->CTL0;
	uint8_t mode_hw = GpioModeHw[mode].hwbits;	// CTL[3:2]
	uint8_t MD = 0;								// MD [1:0]

	if (bit > 7)
	{
		bit -= 8;
		CTLx = &reg->CTL1;
		bit2 = bit+8;	// how much to shift for OCTL register
	}
	else
	{
		CTLx = &reg->CTL0;
		bit2 = bit;
	}

	if ((mode == OutPP) || (mode == OutOD) || (mode == AfioPP) || (mode == AfioOD))
	{
		// output speed only relevant in output mode
		MD = speed;
	}

	if (mode == InPD)
	{
		// delete old value:
		reg->OCTL &= ~(1 << bit2);
		// enable pulldown resistor
		reg->OCTL |=  (0 << bit2);
	}
	else if (mode == InPU)
	{
		// enable pullup resistor
		reg->OCTL |= (1 << bit2);
	}

	uint8_t to_shift = bit * 4;

	// delete old:
	*CTLx &= ~((0b1111) << to_shift);
	// write new:
	*CTLx |= ((mode_hw << 2) | MD) << to_shift;
}

void gpio_select_exti(GpioPin pin)
{
	GpioReg* reg = GpioPins[pin].reg;
	uint8_t  bit = GpioPins[pin].bit;
	uint8_t n = 0;
	uint8_t value = 0;

	// EXTISSx register layout (all fields 4bit wide):
	// EXTISS0 [pin3]  [pin2]  [pin1]  [pin0]
	// EXTISS1 [pin7]  [pin6]  [pin5]  [pin4]
	// EXTISS2 [pin11] [pin10] [pin9]  [pin8]
	// EXTISS3 [pin15] [pin14] [pin13] [pin12]
	// values in fields:
	// PAx: 0b0000
	// PBx: 0b0001
	// PCx: 0b0010
	// PDx: 0b0011
	// PEx: 0b0100

	if (reg == GPIOA)
	{
		value = 0b0000;
	}
	else if (reg == GPIOB)
	{
		value = 0b0001;
	}
	else if (reg == GPIOC)
	{
		value = 0b0010;
	}
	else if (reg == GPIOD)
	{
		value = 0b0011;
	}
	else if (reg == GPIOE)
	{
		value = 0b0100;
	}
	else
	{
		eprintf("Error, wrong EXTI pin: %d\r\n", pin);
	}

	ASSERT_EQ((bit >= 0), 1);
	ASSERT_EQ((bit <= 15), 1);

	if (bit > 11)
	{
		n = 3;
	}
	else if (bit > 7)
	{
		n = 2;
	}
	else if (bit > 3)
	{
		n = 1;
	}
	bit -= n*4;		// n/to_shift is 0..3 in EXTISSx

	// clear old value:
	GPIO_AFIO->EXTISS[n] &= ~(0b1111 << bit*4);
	// write new:
	GPIO_AFIO->EXTISS[n] |= (value << bit*4);

}

// void gpio_remap_pin(GpioRemap remap, bool state)
// {
// }

}	// namespace
