// Copyright © 2020 by P.Orsolic. All right reserved
// Created 191227
// High level GPIO prototypes

#ifndef GPIO_H
#define GPIO_H

#include "gd32vf103_rcu.h"
#include <stdbool.h>

namespace gpio
{

typedef enum
{
	PA0 = 0, PA1, PA2,  PA3,  PA4,  PA5,  PA6,  PA7,
	PA8,     PA9, PA10, PA11, PA12, PA13, PA14, PA15,
	PB0,     PB1, PB2,  PB3,  PB4,  PB5,  PB6,  PB7,
	PB8,     PB9, PB10, PB11, PB12, PB13, PB14, PB15,
	PC0,     PC1, PC2,  PC3,  PC4,  PC5,  PC6,  PC7,
	PC8,     PC9, PC10, PC11, PC12, PC13, PC14, PC15,
	PD0,     PD1, PD2,  PD3,  PD4,  PD5,  PD6,  PD7,
	PD8,     PD9, PD10, PD11, PD12, PD13, PD14, PD15,
	PE0,     PE1, PE2,  PE3,  PE4,  PE5,  PE6,  PE7,
	PE8,     PE9, PE10, PE11, PE12, PE13, PE14, PE15,
} GpioPin;

typedef enum
{
	// direct mapping to HW register not possible - multiple same values
	// LUT will be used, this enum members will be used index
	Analog = 0,
	InFloating,
	InPD,
	InPU,
	OutPP,
	OutOD,
	AfioPP,
	AfioOD,
} Mode;

typedef enum
{
	// bitfields in CTRLx register, bits[1:0] MD
	// usefull only in output mode
	SpeedNA		= 0b00,
	Speed10MHz	= 0b01,
	Speed2MHz	= 0b10,
	Speed50MHz	= 0b11,
} Speed;

void gpio_init2(GpioPin pin, Mode mode, Speed speed);

void gpio_set(GpioPin pin);
void gpio_reset(GpioPin pin);
void gpio_toggle(GpioPin pin);
bool gpio_get(GpioPin pin);
void gpio_select_exti(GpioPin pin);

} // namespace
#endif // GPIO_H
