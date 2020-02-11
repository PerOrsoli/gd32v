// Copyright © 2020 by P.Orsolic. All right reserved
// Created 191227
// internal header
// HW specific registers and register bits definitions

#ifndef GPIO_HW_H
#define GPIO_HW_H
// namespace gpio {

#include <stdint.h>
#include "gpio.hpp"

// addresses
#define ADDR_GPIO_AFIO	0x40010000
#define ADDR_GPIOA		0x40010800
#define ADDR_GPIOB		0x40010C00
#define ADDR_GPIOC		0x40011000
#define ADDR_GPIOD		0x40011400
#define ADDR_GPIOE		0x40011800

typedef struct
{
	volatile uint32_t CTL0;	// setup pin0..7
	volatile uint32_t CTL1;	// setup pin8..15
	volatile uint32_t ISTAT;// read state of input pin
	volatile uint32_t OCTL;	// write 1 t0 set, write 0 to clear bit N, read to check state
	volatile uint32_t BOP;	// write 1 to set bit N
	volatile uint32_t BC;	// write 1 to clear bit N
	volatile uint32_t LOCK;
} GpioReg;

typedef struct
{
	volatile uint32_t EC;
	volatile uint32_t PCF0;
	volatile uint32_t EXTISS[4];
	volatile uint32_t reserved;
	volatile uint32_t PCF1;
} GpioAfioReg;

#define GPIOA		((GpioReg *)ADDR_GPIOA)
#define GPIOB		((GpioReg *)ADDR_GPIOB)
#define GPIOC		((GpioReg *)ADDR_GPIOC)
#define GPIOD		((GpioReg *)ADDR_GPIOD)
#define GPIOE		((GpioReg *)ADDR_GPIOE)
#define GPIO_AFIO	((GpioAfioReg *)ADDR_GPIO_AFIO)

using namespace gpio;
typedef struct
{
	GpioPin		pin;
	GpioReg*	reg;
	uint8_t		bit;
} GpioMap;

typedef struct
{
	Mode	mode;
	uint8_t	hwbits;
} GpioModeMap;

extern const GpioMap GpioPins[];
extern const GpioModeMap GpioModeHw[];

// } // namespace
#endif // GPIO_HW_H
