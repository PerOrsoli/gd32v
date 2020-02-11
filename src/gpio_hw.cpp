// Copyright © 2020 by P.Orsolic. All right reserved
#include "gpio_hw.hpp"

const GpioMap GpioPins[] = {
	{PA0,	GPIOA,	0},
	{PA1,	GPIOA,	1},
	{PA2,	GPIOA,	2},
	{PA3,	GPIOA,	3},
	{PA4,	GPIOA,	4},
	{PA5,	GPIOA,	5},
	{PA6,	GPIOA,	6},
	{PA7,	GPIOA,	7},
	{PA8,	GPIOA,	8},
	{PA9,	GPIOA,	9},
	{PA10,	GPIOA,	10},
	{PA11,	GPIOA,	11},
	{PA12,	GPIOA,	12},
	{PA13,	GPIOA,	13},
	{PA14,	GPIOA,	14},
	{PA15,	GPIOA,	15},

	{PB0,	GPIOB,	0},
	{PB1,	GPIOB,	1},
	{PB2,	GPIOB,	2},
	{PB3,	GPIOB,	3},
	{PB4,	GPIOB,	4},
	{PB5,	GPIOB,	5},
	{PB6,	GPIOB,	6},
	{PB7,	GPIOB,	7},
	{PB8,	GPIOB,	8},
	{PB9,	GPIOB,	9},
	{PB10,	GPIOB,	10},
	{PB11,	GPIOB,	11},
	{PB12,	GPIOB,	12},
	{PB13,	GPIOB,	13},
	{PB14,	GPIOB,	14},
	{PB15,	GPIOB,	15},

	{PC0,	GPIOC,	0},
	{PC1,	GPIOC,	1},
	{PC2,	GPIOC,	2},
	{PC3,	GPIOC,	3},
	{PC4,	GPIOC,	4},
	{PC5,	GPIOC,	5},
	{PC6,	GPIOC,	6},
	{PC7,	GPIOC,	7},
	{PC8,	GPIOC,	8},
	{PC9,	GPIOC,	9},
	{PC10,	GPIOC,	10},
	{PC11,	GPIOC,	11},
	{PC12,	GPIOC,	12},
	{PC13,	GPIOC,	13},
	{PC14,	GPIOC,	14},
	{PC15,	GPIOC,	15},

	{PD0,	GPIOD,	0},
	{PD1,	GPIOD,	1},
	{PD2,	GPIOD,	2},
	{PD3,	GPIOD,	3},
	{PD4,	GPIOD,	4},
	{PD5,	GPIOD,	5},
	{PD6,	GPIOD,	6},
	{PD7,	GPIOD,	7},
	{PD8,	GPIOD,	8},
	{PD9,	GPIOD,	9},
	{PD10,	GPIOD,	10},
	{PD11,	GPIOD,	11},
	{PD12,	GPIOD,	12},
	{PD13,	GPIOD,	13},
	{PD14,	GPIOD,	14},
	{PD15,	GPIOD,	15},

	{PE0,	GPIOE,	0},
	{PE1,	GPIOE,	1},
	{PE2,	GPIOE,	2},
	{PE3,	GPIOE,	3},
	{PE4,	GPIOE,	4},
	{PE5,	GPIOE,	5},
	{PE6,	GPIOE,	6},
	{PE7,	GPIOE,	7},
	{PE8,	GPIOE,	8},
	{PE9,	GPIOE,	9},
	{PE10,	GPIOE,	10},
	{PE11,	GPIOE,	11},
	{PE12,	GPIOE,	12},
	{PE13,	GPIOE,	13},
	{PE14,	GPIOE,	14},
	{PE15,	GPIOE,	15},
};

// must be in sync with Mode in gpio.h
const GpioModeMap GpioModeHw[] = {
	{Analog,		0b00},
	{InFloating,	0b01},
	{InPD,			0b10},
	{InPU,			0b10},
	{OutPP,			0b00},
	{OutOD,			0b01},
	{AfioPP,		0b10},
	{AfioOD,		0b11},
};
