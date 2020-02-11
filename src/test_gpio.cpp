// Copyright © 2020 by P.Orsolic. All right reserved
#ifdef RUN_TESTS

#include "test_gpio.hpp"

static void register_backup(GpioReg *GPIOX);
static void register_restore(GpioReg *GPIOX);
static void register_backup_afio(void);
static void register_restore_afio(void);

// register backup and restore												{{{
// ----------------------------------------------------------------------------
static uint32_t ctl0;
static uint32_t ctl1;
static uint32_t octl;

static void register_backup(GpioReg *GPIOX)
{
	// backup registers which will be tested
	ctl0 = GPIOX->CTL0;
	ctl1 = GPIOX->CTL1;
	octl = GPIOX->OCTL;
}

static void register_restore(GpioReg *GPIOX)
{
	// restore registers
	GPIOX->CTL0 = ctl0;
	GPIOX->CTL1 = ctl1;
	GPIOX->OCTL = octl;
}

static uint32_t extiss0;
static uint32_t extiss1;
static uint32_t extiss2;
static uint32_t extiss3;

static void register_backup_afio(void)
{
	extiss0 = GPIO_AFIO->EXTISS[0];
	extiss1 = GPIO_AFIO->EXTISS[1];
	extiss2 = GPIO_AFIO->EXTISS[2];
	extiss3 = GPIO_AFIO->EXTISS[3];
}

static void register_restore_afio(void)
{
	GPIO_AFIO->EXTISS[0] = extiss0;
	GPIO_AFIO->EXTISS[1] = extiss1;
	GPIO_AFIO->EXTISS[2] = extiss2;
	GPIO_AFIO->EXTISS[3] = extiss3;
}
// ------------------------------------------------------------------------ }}}
// check register addresses		 											{{{
// ----------------------------------------------------------------------------
static void test_check_addresses(void)
{
	// check if addresses of registers are correct before proceeding
	ASSERT_EQ((uint32_t)GPIOA, 0x40010800);
	ASSERT_EQ((uint32_t)GPIOB, 0x40010C00);
	ASSERT_EQ((uint32_t)GPIOC, 0x40011000);
	ASSERT_EQ((uint32_t)GPIOD, 0x40011400);
	ASSERT_EQ((uint32_t)GPIOE, 0x40011800);
	ASSERT_EQ((uint32_t)GPIO_AFIO, 0x40010000);

	// check if addresses of subregister are correct
	ASSERT_EQ((uint32_t)&GPIOA->CTL0,  0x40010800 + 0x0);
	ASSERT_EQ((uint32_t)&GPIOA->CTL1,  0x40010800 + 0x4);
	ASSERT_EQ((uint32_t)&GPIOA->ISTAT, 0x40010800 + 0x8);
	ASSERT_EQ((uint32_t)&GPIOA->OCTL,  0x40010800 + 0xC);
	ASSERT_EQ((uint32_t)&GPIOA->BOP,   0x40010800 + 0x10);
	ASSERT_EQ((uint32_t)&GPIOA->BC,    0x40010800 + 0x14);
	ASSERT_EQ((uint32_t)&GPIOA->LOCK,  0x40010800 + 0x18);

	ASSERT_EQ((uint32_t)&GPIO_AFIO->EC,        0x40010000 + 0x0);
	ASSERT_EQ((uint32_t)&GPIO_AFIO->PCF0,      0x40010000 + 0x4);
	ASSERT_EQ((uint32_t)&GPIO_AFIO->EXTISS[0], 0x40010000 + 0x8);
	ASSERT_EQ((uint32_t)&GPIO_AFIO->EXTISS[1], 0x40010000 + 0xC);
	ASSERT_EQ((uint32_t)&GPIO_AFIO->EXTISS[2], 0x40010000 + 0x10);
	ASSERT_EQ((uint32_t)&GPIO_AFIO->EXTISS[3], 0x40010000 + 0x14);
	ASSERT_EQ((uint32_t)&GPIO_AFIO->PCF1,      0x40010000 + 0x1C);
}
// ------------------------------------------------------------------------ }}}

// GPIOA tests																{{{
// ----------------------------------------------------------------------------
static void test_init_PA0(void)
{
	const uint8_t n = 0;
	gpio_init2(PA0, Analog, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	// Speed should not be written into register when Output mode is not selected
	// only test speed in Analog mode, only for GPIOA - it should be enough
	gpio_init2(PA0, Analog, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA0, Analog, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA0, Analog, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA0, InFloating, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PA0, InPD, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n), (0 << n));

	gpio_init2(PA0, InPU, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n), (1 << n));

	gpio_init2(PA0, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PA0, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PA0, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PA0, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PA0, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PA0, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PA0, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PA0, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PA0, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PA0, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PA0, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PA0, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1111 << n*4));
}
static void test_init_PA1(void)
{
	const uint8_t n = 1;
	gpio_init2(PA1, Analog, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA1, Analog, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA1, Analog, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA1, Analog, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA1, InFloating, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PA1, InPD, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n), (0 << n));

	gpio_init2(PA1, InPU, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n), (1 << n));

	gpio_init2(PA1, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PA1, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PA1, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PA1, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PA1, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PA1, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PA1, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PA1, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PA1, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PA1, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PA1, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PA1, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PA2(void)
{
	const uint8_t n = 2;
	gpio_init2(PA2, Analog, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA2, Analog, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA2, Analog, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA2, Analog, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA2, InFloating, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PA2, InPD, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n), (0 << n));

	gpio_init2(PA2, InPU, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n), (1 << n));

	gpio_init2(PA2, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PA2, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PA2, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PA2, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PA2, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PA2, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PA2, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PA2, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PA2, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PA2, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PA2, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PA2, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PA3(void)
{
	const uint8_t n = 3;
	gpio_init2(PA3, Analog, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA3, Analog, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA3, Analog, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA3, Analog, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA3, InFloating, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PA3, InPD, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n), (0 << n));

	gpio_init2(PA3, InPU, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n), (1 << n));

	gpio_init2(PA3, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PA3, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PA3, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PA3, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PA3, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PA3, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PA3, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PA3, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PA3, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PA3, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PA3, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PA3, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PA4(void)
{
	const uint8_t n = 4;
	gpio_init2(PA4, Analog, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA4, Analog, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA4, Analog, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA4, Analog, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA4, InFloating, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PA4, InPD, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n), (0 << n));

	gpio_init2(PA4, InPU, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n), (1 << n));

	gpio_init2(PA4, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PA4, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PA4, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PA4, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PA4, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PA4, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PA4, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PA4, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PA4, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PA4, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PA4, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PA4, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PA5(void)
{
	const uint8_t n = 5;
	gpio_init2(PA5, Analog, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA5, Analog, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA5, Analog, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA5, Analog, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA5, InFloating, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PA5, InPD, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n), (0 << n));

	gpio_init2(PA5, InPU, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n), (1 << n));

	gpio_init2(PA5, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PA5, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PA5, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PA5, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PA5, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PA5, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PA5, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PA5, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PA5, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PA5, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PA5, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PA5, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PA6(void)
{
	const uint8_t n = 6;
	gpio_init2(PA6, Analog, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA6, Analog, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA6, Analog, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA6, Analog, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA6, InFloating, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PA6, InPD, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n), (0 << n));

	gpio_init2(PA6, InPU, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n), (1 << n));

	gpio_init2(PA6, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PA6, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PA6, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PA6, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PA6, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PA6, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PA6, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PA6, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PA6, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PA6, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PA6, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PA6, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PA7(void)
{
	const uint8_t n = 7;
	gpio_init2(PA7, Analog, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA7, Analog, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA7, Analog, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA7, Analog, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA7, InFloating, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PA7, InPD, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n), (0 << n));

	gpio_init2(PA7, InPU, SpeedNA);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n), (1 << n));

	gpio_init2(PA7, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PA7, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PA7, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PA7, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PA7, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PA7, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PA7, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PA7, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PA7, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PA7, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PA7, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PA7, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PA8(void)
{
	const uint8_t n  = 0;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PA8, Analog, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA8, Analog, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA8, Analog, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA8, Analog, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA8, InFloating, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PA8, InPD, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PA8, InPU, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PA8, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PA8, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PA8, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PA8, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PA8, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PA8, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PA8, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PA8, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PA8, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PA8, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PA8, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PA8, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PA9(void)
{
	const uint8_t n  = 1;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PA9, Analog, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA9, Analog, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA9, Analog, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA9, Analog, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA9, InFloating, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PA9, InPD, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PA9, InPU, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PA9, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PA9, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PA9, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PA9, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PA9, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PA9, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PA9, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PA9, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PA9, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PA9, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PA9, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PA9, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PA10(void)
{
	const uint8_t n  = 2;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PA10, Analog, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA10, InFloating, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PA10, Analog, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA10, Analog, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA10, Analog, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA10, InPD, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PA10, InPU, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PA10, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PA10, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PA10, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PA10, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PA10, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PA10, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PA10, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PA10, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PA10, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PA10, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PA10, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PA10, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PA11(void)
{
	const uint8_t n  = 3;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PA11, Analog, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA11, Analog, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA11, Analog, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA11, Analog, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA11, InFloating, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PA11, InPD, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PA11, InPU, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PA11, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PA11, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PA11, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PA11, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PA11, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PA11, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PA11, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PA11, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PA11, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PA11, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PA11, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PA11, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PA12(void)
{
	const uint8_t n  = 4;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PA12, Analog, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA12, Analog, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA12, Analog, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA12, Analog, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA12, InFloating, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PA12, InPD, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PA12, InPU, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PA12, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PA12, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PA12, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PA12, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PA12, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PA12, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PA12, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PA12, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PA12, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PA12, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PA12, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PA12, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PA13(void)
{
	const uint8_t n  = 5;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PA13, Analog, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA13, Analog, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA13, Analog, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA13, Analog, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA13, InFloating, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PA13, InPD, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PA13, InPU, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PA13, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PA13, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PA13, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PA13, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PA13, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PA13, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PA13, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PA13, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PA13, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PA13, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PA13, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PA13, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PA14(void)
{
	const uint8_t n  = 6;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PA14, Analog, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA14, Analog, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA14, Analog, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA14, Analog, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA14, InFloating, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PA14, InPD, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PA14, InPU, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PA14, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PA14, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PA14, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PA14, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PA14, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PA14, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PA14, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PA14, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PA14, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PA14, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PA14, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PA14, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PA15(void)
{
	const uint8_t n  = 7;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PA15, Analog, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA15, Analog, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA15, Analog, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA15, Analog, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PA15, InFloating, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PA15, InPD, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PA15, InPU, SpeedNA);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOA->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PA15, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PA15, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PA15, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PA15, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PA15, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PA15, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PA15, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PA15, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PA15, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PA15, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PA15, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PA15, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOA->CTL1 & (0xF << n*4), (0b1111 << n*4));
}
// ------------------------------------------------------------------------ }}}
// GPIOB tests																{{{
// ----------------------------------------------------------------------------
static void test_init_PB0(void)
{
	const uint8_t n = 0;
	gpio_init2(PB0, Analog, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PB0, InFloating, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PB0, InPD, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n), (0 << n));

	gpio_init2(PB0, InPU, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n), (1 << n));

	gpio_init2(PB0, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PB0, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PB0, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PB0, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PB0, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PB0, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PB0, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PB0, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PB0, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PB0, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PB0, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PB0, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1111 << n*4));
}
static void test_init_PB1(void)
{
	const uint8_t n = 1;
	gpio_init2(PB1, Analog, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PB1, InFloating, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PB1, InPD, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n), (0 << n));

	gpio_init2(PB1, InPU, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n), (1 << n));

	gpio_init2(PB1, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PB1, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PB1, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PB1, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PB1, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PB1, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PB1, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PB1, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PB1, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PB1, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PB1, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PB1, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PB2(void)
{
	const uint8_t n = 2;
	gpio_init2(PB2, Analog, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PB2, InFloating, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PB2, InPD, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n), (0 << n));

	gpio_init2(PB2, InPU, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n), (1 << n));

	gpio_init2(PB2, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PB2, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PB2, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PB2, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PB2, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PB2, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PB2, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PB2, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PB2, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PB2, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PB2, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PB2, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PB3(void)
{
	const uint8_t n = 3;
	gpio_init2(PB3, Analog, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PB3, InFloating, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PB3, InPD, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n), (0 << n));

	gpio_init2(PB3, InPU, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n), (1 << n));

	gpio_init2(PB3, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PB3, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PB3, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PB3, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PB3, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PB3, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PB3, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PB3, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PB3, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PB3, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PB3, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PB3, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PB4(void)
{
	const uint8_t n = 4;
	gpio_init2(PB4, Analog, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PB4, InFloating, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PB4, InPD, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n), (0 << n));

	gpio_init2(PB4, InPU, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n), (1 << n));

	gpio_init2(PB4, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PB4, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PB4, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PB4, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PB4, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PB4, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PB4, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PB4, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PB4, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PB4, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PB4, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PB4, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PB5(void)
{
	const uint8_t n = 5;
	gpio_init2(PB5, Analog, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PB5, InFloating, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PB5, InPD, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n), (0 << n));

	gpio_init2(PB5, InPU, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n), (1 << n));

	gpio_init2(PB5, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PB5, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PB5, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PB5, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PB5, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PB5, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PB5, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PB5, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PB5, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PB5, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PB5, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PB5, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PB6(void)
{
	const uint8_t n = 6;
	gpio_init2(PB6, Analog, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PB6, InFloating, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PB6, InPD, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n), (0 << n));

	gpio_init2(PB6, InPU, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n), (1 << n));

	gpio_init2(PB6, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PB6, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PB6, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PB6, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PB6, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PB6, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PB6, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PB6, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PB6, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PB6, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PB6, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PB6, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PB7(void)
{
	const uint8_t n = 7;
	gpio_init2(PB7, Analog, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PB7, InFloating, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PB7, InPD, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n), (0 << n));

	gpio_init2(PB7, InPU, SpeedNA);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n), (1 << n));

	gpio_init2(PB7, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PB7, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PB7, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PB7, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PB7, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PB7, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PB7, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PB7, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PB7, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PB7, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PB7, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PB7, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PB8(void)
{
	const uint8_t n  = 0;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PB8, Analog, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PB8, InFloating, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PB8, InPD, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PB8, InPU, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PB8, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PB8, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PB8, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PB8, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PB8, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PB8, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PB8, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PB8, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PB8, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PB8, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PB8, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PB8, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PB9(void)
{
	const uint8_t n  = 1;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PB9, Analog, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PB9, InFloating, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PB9, InPD, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PB9, InPU, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PB9, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PB9, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PB9, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PB9, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PB9, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PB9, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PB9, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PB9, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PB9, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PB9, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PB9, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PB9, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PB10(void)
{
	const uint8_t n  = 2;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PB10, Analog, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PB10, InFloating, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PB10, InPD, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PB10, InPU, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PB10, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PB10, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PB10, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PB10, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PB10, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PB10, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PB10, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PB10, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PB10, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PB10, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PB10, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PB10, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PB11(void)
{
	const uint8_t n  = 3;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PB11, Analog, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PB11, InFloating, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PB11, InPD, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PB11, InPU, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PB11, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PB11, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PB11, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PB11, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PB11, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PB11, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PB11, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PB11, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PB11, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PB11, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PB11, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PB11, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PB12(void)
{
	const uint8_t n  = 4;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PB12, Analog, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PB12, InFloating, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PB12, InPD, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PB12, InPU, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PB12, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PB12, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PB12, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PB12, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PB12, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PB12, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PB12, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PB12, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PB12, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PB12, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PB12, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PB12, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PB13(void)
{
	const uint8_t n  = 5;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PB13, Analog, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PB13, InFloating, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PB13, InPD, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PB13, InPU, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PB13, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PB13, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PB13, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PB13, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PB13, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PB13, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PB13, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PB13, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PB13, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PB13, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PB13, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PB13, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PB14(void)
{
	const uint8_t n  = 6;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PB14, Analog, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PB14, InFloating, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PB14, InPD, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PB14, InPU, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PB14, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PB14, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PB14, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PB14, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PB14, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PB14, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PB14, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PB14, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PB14, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PB14, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PB14, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PB14, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PB15(void)
{
	const uint8_t n  = 7;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PB15, Analog, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PB15, InFloating, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PB15, InPD, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PB15, InPU, SpeedNA);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOB->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PB15, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PB15, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PB15, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PB15, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PB15, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PB15, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PB15, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PB15, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PB15, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PB15, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PB15, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PB15, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOB->CTL1 & (0xF << n*4), (0b1111 << n*4));
}
// ------------------------------------------------------------------------ }}}
// GPIOC tests																{{{
// ----------------------------------------------------------------------------
static void test_init_PC0(void)
{
	const uint8_t n = 0;
	gpio_init2(PC0, Analog, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PC0, InFloating, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PC0, InPD, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n), (0 << n));

	gpio_init2(PC0, InPU, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n), (1 << n));

	gpio_init2(PC0, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PC0, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PC0, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PC0, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PC0, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PC0, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PC0, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PC0, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PC0, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PC0, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PC0, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PC0, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1111 << n*4));
}
static void test_init_PC1(void)
{
	const uint8_t n = 1;
	gpio_init2(PC1, Analog, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PC1, InFloating, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PC1, InPD, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n), (0 << n));

	gpio_init2(PC1, InPU, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n), (1 << n));

	gpio_init2(PC1, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PC1, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PC1, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PC1, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PC1, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PC1, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PC1, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PC1, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PC1, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PC1, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PC1, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PC1, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PC2(void)
{
	const uint8_t n = 2;
	gpio_init2(PC2, Analog, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PC2, InFloating, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PC2, InPD, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n), (0 << n));

	gpio_init2(PC2, InPU, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n), (1 << n));

	gpio_init2(PC2, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PC2, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PC2, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PC2, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PC2, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PC2, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PC2, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PC2, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PC2, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PC2, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PC2, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PC2, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PC3(void)
{
	const uint8_t n = 3;
	gpio_init2(PC3, Analog, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PC3, InFloating, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PC3, InPD, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n), (0 << n));

	gpio_init2(PC3, InPU, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n), (1 << n));

	gpio_init2(PC3, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PC3, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PC3, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PC3, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PC3, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PC3, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PC3, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PC3, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PC3, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PC3, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PC3, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PC3, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PC4(void)
{
	const uint8_t n = 4;
	gpio_init2(PC4, Analog, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PC4, InFloating, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PC4, InPD, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n), (0 << n));

	gpio_init2(PC4, InPU, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n), (1 << n));

	gpio_init2(PC4, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PC4, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PC4, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PC4, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PC4, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PC4, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PC4, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PC4, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PC4, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PC4, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PC4, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PC4, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PC5(void)
{
	const uint8_t n = 5;
	gpio_init2(PC5, Analog, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PC5, InFloating, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PC5, InPD, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n), (0 << n));

	gpio_init2(PC5, InPU, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n), (1 << n));

	gpio_init2(PC5, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PC5, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PC5, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PC5, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PC5, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PC5, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PC5, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PC5, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PC5, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PC5, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PC5, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PC5, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PC6(void)
{
	const uint8_t n = 6;
	gpio_init2(PC6, Analog, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PC6, InFloating, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PC6, InPD, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n), (0 << n));

	gpio_init2(PC6, InPU, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n), (1 << n));

	gpio_init2(PC6, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PC6, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PC6, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PC6, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PC6, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PC6, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PC6, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PC6, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PC6, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PC6, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PC6, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PC6, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PC7(void)
{
	const uint8_t n = 7;
	gpio_init2(PC7, Analog, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PC7, InFloating, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PC7, InPD, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n), (0 << n));

	gpio_init2(PC7, InPU, SpeedNA);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n), (1 << n));

	gpio_init2(PC7, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PC7, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PC7, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PC7, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PC7, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PC7, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PC7, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PC7, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PC7, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PC7, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PC7, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PC7, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PC8(void)
{
	const uint8_t n  = 0;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PC8, Analog, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PC8, InFloating, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PC8, InPD, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PC8, InPU, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PC8, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PC8, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PC8, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PC8, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PC8, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PC8, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PC8, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PC8, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PC8, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PC8, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PC8, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PC8, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PC9(void)
{
	const uint8_t n  = 1;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PC9, Analog, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PC9, InFloating, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PC9, InPD, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PC9, InPU, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PC9, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PC9, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PC9, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PC9, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PC9, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PC9, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PC9, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PC9, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PC9, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PC9, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PC9, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PC9, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PC10(void)
{
	const uint8_t n  = 2;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PC10, Analog, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PC10, InFloating, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PC10, InPD, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PC10, InPU, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PC10, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PC10, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PC10, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PC10, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PC10, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PC10, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PC10, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PC10, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PC10, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PC10, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PC10, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PC10, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PC11(void)
{
	const uint8_t n  = 3;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PC11, Analog, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PC11, InFloating, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PC11, InPD, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PC11, InPU, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PC11, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PC11, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PC11, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PC11, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PC11, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PC11, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PC11, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PC11, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PC11, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PC11, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PC11, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PC11, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PC12(void)
{
	const uint8_t n  = 4;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PC12, Analog, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PC12, InFloating, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PC12, InPD, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PC12, InPU, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PC12, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PC12, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PC12, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PC12, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PC12, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PC12, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PC12, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PC12, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PC12, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PC12, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PC12, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PC12, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PC13(void)
{
	const uint8_t n  = 5;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PC13, Analog, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PC13, InFloating, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PC13, InPD, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PC13, InPU, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PC13, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PC13, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PC13, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PC13, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PC13, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PC13, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PC13, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PC13, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PC13, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PC13, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PC13, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PC13, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PC14(void)
{
	const uint8_t n  = 6;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PC14, Analog, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PC14, InFloating, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PC14, InPD, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PC14, InPU, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PC14, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PC14, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PC14, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PC14, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PC14, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PC14, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PC14, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PC14, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PC14, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PC14, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PC14, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PC14, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PC15(void)
{
	const uint8_t n  = 7;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PC15, Analog, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PC15, InFloating, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PC15, InPD, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PC15, InPU, SpeedNA);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOC->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PC15, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PC15, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PC15, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PC15, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PC15, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PC15, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PC15, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PC15, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PC15, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PC15, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PC15, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PC15, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOC->CTL1 & (0xF << n*4), (0b1111 << n*4));
}
// ------------------------------------------------------------------------ }}}
// GPIOD tests																{{{
// ----------------------------------------------------------------------------
static void test_init_PD0(void)
{
	const uint8_t n = 0;
	gpio_init2(PD0, Analog, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PD0, InFloating, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PD0, InPD, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n), (0 << n));

	gpio_init2(PD0, InPU, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n), (1 << n));

	gpio_init2(PD0, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PD0, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PD0, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PD0, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PD0, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PD0, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PD0, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PD0, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PD0, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PD0, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PD0, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PD0, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1111 << n*4));
}
static void test_init_PD1(void)
{
	const uint8_t n = 1;
	gpio_init2(PD1, Analog, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PD1, InFloating, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PD1, InPD, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n), (0 << n));

	gpio_init2(PD1, InPU, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n), (1 << n));

	gpio_init2(PD1, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PD1, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PD1, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PD1, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PD1, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PD1, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PD1, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PD1, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PD1, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PD1, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PD1, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PD1, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PD2(void)
{
	const uint8_t n = 2;
	gpio_init2(PD2, Analog, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PD2, InFloating, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PD2, InPD, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n), (0 << n));

	gpio_init2(PD2, InPU, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n), (1 << n));

	gpio_init2(PD2, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PD2, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PD2, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PD2, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PD2, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PD2, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PD2, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PD2, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PD2, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PD2, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PD2, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PD2, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PD3(void)
{
	const uint8_t n = 3;
	gpio_init2(PD3, Analog, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PD3, InFloating, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PD3, InPD, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n), (0 << n));

	gpio_init2(PD3, InPU, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n), (1 << n));

	gpio_init2(PD3, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PD3, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PD3, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PD3, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PD3, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PD3, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PD3, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PD3, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PD3, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PD3, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PD3, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PD3, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PD4(void)
{
	const uint8_t n = 4;
	gpio_init2(PD4, Analog, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PD4, InFloating, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PD4, InPD, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n), (0 << n));

	gpio_init2(PD4, InPU, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n), (1 << n));

	gpio_init2(PD4, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PD4, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PD4, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PD4, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PD4, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PD4, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PD4, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PD4, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PD4, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PD4, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PD4, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PD4, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PD5(void)
{
	const uint8_t n = 5;
	gpio_init2(PD5, Analog, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PD5, InFloating, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PD5, InPD, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n), (0 << n));

	gpio_init2(PD5, InPU, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n), (1 << n));

	gpio_init2(PD5, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PD5, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PD5, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PD5, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PD5, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PD5, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PD5, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PD5, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PD5, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PD5, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PD5, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PD5, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PD6(void)
{
	const uint8_t n = 6;
	gpio_init2(PD6, Analog, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PD6, InFloating, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PD6, InPD, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n), (0 << n));

	gpio_init2(PD6, InPU, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n), (1 << n));

	gpio_init2(PD6, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PD6, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PD6, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PD6, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PD6, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PD6, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PD6, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PD6, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PD6, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PD6, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PD6, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PD6, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PD7(void)
{
	const uint8_t n = 7;
	gpio_init2(PD7, Analog, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PD7, InFloating, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PD7, InPD, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n), (0 << n));

	gpio_init2(PD7, InPU, SpeedNA);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n), (1 << n));

	gpio_init2(PD7, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PD7, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PD7, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PD7, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PD7, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PD7, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PD7, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PD7, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PD7, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PD7, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PD7, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PD7, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PD8(void)
{
	const uint8_t n  = 0;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PD8, Analog, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PD8, InFloating, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PD8, InPD, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PD8, InPU, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PD8, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PD8, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PD8, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PD8, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PD8, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PD8, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PD8, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PD8, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PD8, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PD8, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PD8, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PD8, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PD9(void)
{
	const uint8_t n  = 1;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PD9, Analog, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PD9, InFloating, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PD9, InPD, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PD9, InPU, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PD9, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PD9, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PD9, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PD9, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PD9, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PD9, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PD9, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PD9, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PD9, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PD9, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PD9, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PD9, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PD10(void)
{
	const uint8_t n  = 2;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PD10, Analog, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PD10, InFloating, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PD10, InPD, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PD10, InPU, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PD10, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PD10, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PD10, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PD10, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PD10, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PD10, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PD10, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PD10, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PD10, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PD10, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PD10, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PD10, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PD11(void)
{
	const uint8_t n  = 3;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PD11, Analog, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PD11, InFloating, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PD11, InPD, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PD11, InPU, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PD11, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PD11, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PD11, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PD11, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PD11, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PD11, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PD11, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PD11, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PD11, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PD11, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PD11, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PD11, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PD12(void)
{
	const uint8_t n  = 4;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PD12, Analog, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PD12, InFloating, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PD12, InPD, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PD12, InPU, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PD12, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PD12, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PD12, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PD12, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PD12, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PD12, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PD12, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PD12, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PD12, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PD12, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PD12, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PD12, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PD13(void)
{
	const uint8_t n  = 5;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PD13, Analog, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PD13, InFloating, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PD13, InPD, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PD13, InPU, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PD13, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PD13, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PD13, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PD13, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PD13, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PD13, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PD13, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PD13, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PD13, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PD13, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PD13, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PD13, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PD14(void)
{
	const uint8_t n  = 6;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PD14, Analog, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PD14, InFloating, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PD14, InPD, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PD14, InPU, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PD14, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PD14, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PD14, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PD14, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PD14, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PD14, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PD14, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PD14, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PD14, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PD14, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PD14, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PD14, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PD15(void)
{
	const uint8_t n  = 7;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PD15, Analog, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PD15, InFloating, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PD15, InPD, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PD15, InPU, SpeedNA);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOD->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PD15, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PD15, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PD15, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PD15, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PD15, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PD15, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PD15, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PD15, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PD15, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PD15, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PD15, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PD15, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOD->CTL1 & (0xF << n*4), (0b1111 << n*4));
}
// ------------------------------------------------------------------------ }}}
// GPIOE tests																{{{
// ----------------------------------------------------------------------------
static void test_init_PE0(void)
{
	const uint8_t n = 0;
	gpio_init2(PE0, Analog, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PE0, InFloating, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PE0, InPD, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n), (0 << n));

	gpio_init2(PE0, InPU, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n), (1 << n));

	gpio_init2(PE0, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PE0, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PE0, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PE0, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PE0, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PE0, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PE0, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PE0, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PE0, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PE0, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PE0, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PE0, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PE1(void)
{
	const uint8_t n = 1;
	gpio_init2(PE1, Analog, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PE1, InFloating, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PE1, InPD, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n), (0 << n));

	gpio_init2(PE1, InPU, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n), (1 << n));

	gpio_init2(PE1, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PE1, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PE1, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PE1, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PE1, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PE1, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PE1, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PE1, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PE1, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PE1, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PE1, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PE1, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PE2(void)
{
	const uint8_t n = 2;
	gpio_init2(PE2, Analog, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PE2, InFloating, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PE2, InPD, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n), (0 << n));

	gpio_init2(PE2, InPU, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n), (1 << n));

	gpio_init2(PE2, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PE2, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PE2, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PE2, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PE2, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PE2, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PE2, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PE2, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PE2, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PE2, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PE2, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PE2, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PE3(void)
{
	const uint8_t n = 3;
	gpio_init2(PE3, Analog, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PE3, InFloating, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PE3, InPD, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n), (0 << n));

	gpio_init2(PE3, InPU, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n), (1 << n));

	gpio_init2(PE3, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PE3, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PE3, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PE3, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PE3, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PE3, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PE3, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PE3, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PE3, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PE3, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PE3, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PE3, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PE4(void)
{
	const uint8_t n = 4;
	gpio_init2(PE4, Analog, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PE4, InFloating, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PE4, InPD, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n), (0 << n));

	gpio_init2(PE4, InPU, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n), (1 << n));

	gpio_init2(PE4, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PE4, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PE4, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PE4, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PE4, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PE4, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PE4, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PE4, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PE4, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PE4, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PE4, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PE4, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PE5(void)
{
	const uint8_t n = 5;
	gpio_init2(PE5, Analog, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PE5, InFloating, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PE5, InPD, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n), (0 << n));

	gpio_init2(PE5, InPU, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n), (1 << n));

	gpio_init2(PE5, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PE5, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PE5, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PE5, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PE5, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PE5, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PE5, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PE5, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PE5, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PE5, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PE5, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PE5, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PE6(void)
{
	const uint8_t n = 6;
	gpio_init2(PE6, Analog, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PE6, InFloating, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PE6, InPD, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n), (0 << n));

	gpio_init2(PE6, InPU, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n), (1 << n));

	gpio_init2(PE6, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PE6, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PE6, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PE6, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PE6, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PE6, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PE6, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PE6, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PE6, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PE6, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PE6, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PE6, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PE7(void)
{
	const uint8_t n = 7;
	gpio_init2(PE7, Analog, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PE7, InFloating, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PE7, InPD, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n), (0 << n));

	gpio_init2(PE7, InPU, SpeedNA);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n), (1 << n));

	gpio_init2(PE7, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PE7, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PE7, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PE7, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PE7, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PE7, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PE7, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PE7, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PE7, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PE7, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PE7, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PE7, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL0 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PE8(void)
{
	const uint8_t n = 0;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PE8, Analog, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PE8, InFloating, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PE8, InPD, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PE8, InPU, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PE8, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PE8, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PE8, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PE8, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PE8, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PE8, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PE8, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PE8, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PE8, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PE8, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PE8, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PE8, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PE9(void)
{
	const uint8_t n  = 1;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PE9, Analog, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PE9, InFloating, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PE9, InPD, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PE9, InPU, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PE9, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PE9, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PE9, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PE9, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PE9, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PE9, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PE9, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PE9, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PE9, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PE9, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PE9, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PE9, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PE10(void)
{
	const uint8_t n = 2;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PE10, Analog, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PE10, InFloating, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PE10, InPD, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PE10, InPU, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PE10, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PE10, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PE10, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PE10, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PE10, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PE10, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PE10, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PE10, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PE10, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PE10, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PE10, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PE10, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PE11(void)
{
	const uint8_t n  = 3;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PE11, Analog, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PE11, InFloating, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PE11, InPD, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PE11, InPU, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PE11, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PE11, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PE11, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PE11, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PE11, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PE11, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PE11, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PE11, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PE11, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PE11, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PE11, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PE11, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PE12(void)
{
	const uint8_t n  = 4;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PE12, Analog, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PE12, InFloating, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PE12, InPD, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PE12, InPU, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PE12, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PE12, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PE12, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PE12, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PE12, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PE12, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PE12, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PE12, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PE12, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PE12, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PE12, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PE12, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PE13(void)
{
	const uint8_t n  = 5;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PE13, Analog, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PE13, InFloating, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PE13, InPD, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PE13, InPU, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PE13, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PE13, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PE13, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PE13, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PE13, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PE13, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PE13, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PE13, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PE13, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PE13, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PE13, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PE13, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PE14(void)
{
	const uint8_t n  = 6;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PE14, Analog, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PE14, InFloating, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PE14, InPD, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PE14, InPU, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PE14, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PE14, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PE14, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PE14, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PE14, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PE14, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PE14, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PE14, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PE14, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PE14, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PE14, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PE14, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1111 << n*4));
}

static void test_init_PE15(void)
{
	const uint8_t n  = 7;
	const uint8_t n2 = n+8;	// how much to shift for OCTL register
	gpio_init2(PE15, Analog, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0000 << n*4));

	gpio_init2(PE15, InFloating, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0100 << n*4));

	gpio_init2(PE15, InPD, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n2), (0 << n2));

	gpio_init2(PE15, InPU, SpeedNA);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1000 << n*4));
	ASSERT_EQ(GPIOE->OCTL & (1 << n2), (1 << n2));

	gpio_init2(PE15, OutPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0001 << n*4));
	gpio_init2(PE15, OutPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0010 << n*4));
	gpio_init2(PE15, OutPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0011 << n*4));

	gpio_init2(PE15, OutOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0101 << n*4));
	gpio_init2(PE15, OutOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0110 << n*4));
	gpio_init2(PE15, OutOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b0111 << n*4));

	gpio_init2(PE15, AfioPP, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1001 << n*4));
	gpio_init2(PE15, AfioPP, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1010 << n*4));
	gpio_init2(PE15, AfioPP, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1011 << n*4));

	gpio_init2(PE15, AfioOD, Speed10MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1101 << n*4));
	gpio_init2(PE15, AfioOD, Speed2MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1110 << n*4));
	gpio_init2(PE15, AfioOD, Speed50MHz);
	ASSERT_EQ(GPIOE->CTL1 & (0xF << n*4), (0b1111 << n*4));
}
// ------------------------------------------------------------------------ }}}

// init tests																{{{
// ----------------------------------------------------------------------------
static void init_test(void)
{
	// by purpose everyhing here is so primitive, unrolled and hardcoded
	// this code will not be always included in build, but dead simplicity matters in this case

	printf("Starting with GPIO init test\r\n");
	// It seems that problem is somewhere in printf()
	// How to reproduce: enable only PA8 and PB3. In tests enable only first Analog check

	printf("GPIO init test: GPIOA\r\n");
	register_backup(GPIOA);
	rcu_periph_clock_enable(RCU_GPIOA);
	test_init_PA0();
	test_init_PA1();
	test_init_PA2();
	test_init_PA3();
	test_init_PA4();
	test_init_PA5();
	test_init_PA6();
	test_init_PA7();
	test_init_PA8();	// button
	test_init_PA9();	// UART TX
	test_init_PA10();	// UART RX
	test_init_PA11();
	test_init_PA12();
	test_init_PA13();
	test_init_PA14();
	test_init_PA15();
	register_restore(GPIOA);

	printf("GPIO init test: GPIOB\r\n");
	register_backup(GPIOB);
	rcu_periph_clock_enable(RCU_GPIOB);
	test_init_PB0();
	test_init_PB1();
	test_init_PB2();
	test_init_PB3();
	test_init_PB4();
	test_init_PB5();
	test_init_PB6();
	test_init_PB7();
	test_init_PB8();
	test_init_PB9();
	test_init_PB10();
	test_init_PB11();
	test_init_PB12();
	test_init_PB13();
	test_init_PB14();
	test_init_PB15();
	register_restore(GPIOB);

	printf("GPIO init test: GPIOC\r\n");
	register_backup(GPIOC);
	rcu_periph_clock_enable(RCU_GPIOC);
	test_init_PC0();
	test_init_PC1();
	test_init_PC2();
	test_init_PC3();
	test_init_PC4();
	test_init_PC5();
	test_init_PC6();
	test_init_PC7();
	test_init_PC8();
	test_init_PC9();
	test_init_PC10();
	test_init_PC11();
	test_init_PC12();
	test_init_PC13();
	test_init_PC14();
	test_init_PC15();
	register_restore(GPIOC);

	printf("GPIO init test: GPIOD\r\n");
	register_backup(GPIOD);
	rcu_periph_clock_enable(RCU_GPIOD);
	test_init_PD0();
	test_init_PD1();
	test_init_PD2();
	test_init_PD3();
	test_init_PD4();
	test_init_PD5();
	test_init_PD6();
	test_init_PD7();
	test_init_PD8();
	test_init_PD9();
	test_init_PD10();
	test_init_PD11();
	test_init_PD12();
	test_init_PD13();
	test_init_PD14();
	test_init_PD15();
	register_restore(GPIOD);

	printf("GPIO init test: GPIOE\r\n");
	register_backup(GPIOE);
	rcu_periph_clock_enable(RCU_GPIOE);
	test_init_PE0();
	test_init_PE1();
	test_init_PE2();
	test_init_PE3();
	test_init_PE4();
	test_init_PE5();
	test_init_PE6();
	test_init_PE7();
	test_init_PE8();
	test_init_PE9();
	test_init_PE10();
	test_init_PE11();
	test_init_PE12();
	test_init_PE13();
	test_init_PE14();
	test_init_PE15();
	register_restore(GPIOE);

	printf("GPIO init test done!\r\n");
}
// ------------------------------------------------------------------------ }}}
// EXTI tests					 											{{{
// ----------------------------------------------------------------------------
static void test_exti(void)
{
	rcu_periph_clock_enable(RCU_AF);
	uint8_t n = 0;

	// Pin0
	register_backup_afio();
	n = 0;
	gpio_select_exti(PA0);
	ASSERT_EQ(GPIO_AFIO->EXTISS[0] & (0b1111 << n*4), (0b0000 << n*4));
	gpio_select_exti(PB0);
	ASSERT_EQ(GPIO_AFIO->EXTISS[0] & (0b1111 << n*4), (0b0001 << n*4));
	gpio_select_exti(PC0);
	ASSERT_EQ(GPIO_AFIO->EXTISS[0] & (0b1111 << n*4), (0b0010 << n*4));
	gpio_select_exti(PD0);
	ASSERT_EQ(GPIO_AFIO->EXTISS[0] & (0b1111 << n*4), (0b0011 << n*4));
	gpio_select_exti(PE0);
	ASSERT_EQ(GPIO_AFIO->EXTISS[0] & (0b1111 << n*4), (0b0100 << n*4));
	register_restore_afio();

	// Pin1
	register_backup_afio();
	n = 1;
	gpio_select_exti(PA1);
	ASSERT_EQ(GPIO_AFIO->EXTISS[0] & (0b1111 << n*4), (0b0000 << n*4));
	gpio_select_exti(PB1);
	ASSERT_EQ(GPIO_AFIO->EXTISS[0] & (0b1111 << n*4), (0b0001 << n*4));
	gpio_select_exti(PC1);
	ASSERT_EQ(GPIO_AFIO->EXTISS[0] & (0b1111 << n*4), (0b0010 << n*4));
	gpio_select_exti(PD1);
	ASSERT_EQ(GPIO_AFIO->EXTISS[0] & (0b1111 << n*4), (0b0011 << n*4));
	gpio_select_exti(PE1);
	ASSERT_EQ(GPIO_AFIO->EXTISS[0] & (0b1111 << n*4), (0b0100 << n*4));
	register_restore_afio();

	// Pin2
	register_backup_afio();
	n = 2;
	gpio_select_exti(PA2);
	ASSERT_EQ(GPIO_AFIO->EXTISS[0] & (0b1111 << n*4), (0b0000 << n*4));
	gpio_select_exti(PB2);
	ASSERT_EQ(GPIO_AFIO->EXTISS[0] & (0b1111 << n*4), (0b0001 << n*4));
	gpio_select_exti(PC2);
	ASSERT_EQ(GPIO_AFIO->EXTISS[0] & (0b1111 << n*4), (0b0010 << n*4));
	gpio_select_exti(PD2);
	ASSERT_EQ(GPIO_AFIO->EXTISS[0] & (0b1111 << n*4), (0b0011 << n*4));
	gpio_select_exti(PE2);
	ASSERT_EQ(GPIO_AFIO->EXTISS[0] & (0b1111 << n*4), (0b0100 << n*4));
	register_restore_afio();

	// Pin3
	register_backup_afio();
	n = 3;
	gpio_select_exti(PA3);
	ASSERT_EQ(GPIO_AFIO->EXTISS[0] & (0b1111 << n*4), (0b0000 << n*4));
	gpio_select_exti(PB3);
	ASSERT_EQ(GPIO_AFIO->EXTISS[0] & (0b1111 << n*4), (0b0001 << n*4));
	gpio_select_exti(PC3);
	ASSERT_EQ(GPIO_AFIO->EXTISS[0] & (0b1111 << n*4), (0b0010 << n*4));
	gpio_select_exti(PD3);
	ASSERT_EQ(GPIO_AFIO->EXTISS[0] & (0b1111 << n*4), (0b0011 << n*4));
	gpio_select_exti(PE3);
	ASSERT_EQ(GPIO_AFIO->EXTISS[0] & (0b1111 << n*4), (0b0100 << n*4));
	register_restore_afio();

	// Pin4 - EXTISS1
	register_backup_afio();
	n = 0;
	gpio_select_exti(PA4);
	ASSERT_EQ(GPIO_AFIO->EXTISS[1] & (0b1111 << n*4), (0b0000 << n*4));
	gpio_select_exti(PB4);
	ASSERT_EQ(GPIO_AFIO->EXTISS[1] & (0b1111 << n*4), (0b0001 << n*4));
	gpio_select_exti(PC4);
	ASSERT_EQ(GPIO_AFIO->EXTISS[1] & (0b1111 << n*4), (0b0010 << n*4));
	gpio_select_exti(PD4);
	ASSERT_EQ(GPIO_AFIO->EXTISS[1] & (0b1111 << n*4), (0b0011 << n*4));
	gpio_select_exti(PE4);
	ASSERT_EQ(GPIO_AFIO->EXTISS[1] & (0b1111 << n*4), (0b0100 << n*4));
	register_restore_afio();

	// Pin5
	register_backup_afio();
	n = 1;
	gpio_select_exti(PA5);
	ASSERT_EQ(GPIO_AFIO->EXTISS[1] & (0b1111 << n*4), (0b0000 << n*4));
	gpio_select_exti(PB5);
	ASSERT_EQ(GPIO_AFIO->EXTISS[1] & (0b1111 << n*4), (0b0001 << n*4));
	gpio_select_exti(PC5);
	ASSERT_EQ(GPIO_AFIO->EXTISS[1] & (0b1111 << n*4), (0b0010 << n*4));
	gpio_select_exti(PD5);
	ASSERT_EQ(GPIO_AFIO->EXTISS[1] & (0b1111 << n*4), (0b0011 << n*4));
	gpio_select_exti(PE5);
	ASSERT_EQ(GPIO_AFIO->EXTISS[1] & (0b1111 << n*4), (0b0100 << n*4));
	register_restore_afio();

	// Pin6
	register_backup_afio();
	n = 2;
	gpio_select_exti(PA6);
	ASSERT_EQ(GPIO_AFIO->EXTISS[1] & (0b1111 << n*4), (0b0000 << n*4));
	gpio_select_exti(PB6);
	ASSERT_EQ(GPIO_AFIO->EXTISS[1] & (0b1111 << n*4), (0b0001 << n*4));
	gpio_select_exti(PC6);
	ASSERT_EQ(GPIO_AFIO->EXTISS[1] & (0b1111 << n*4), (0b0010 << n*4));
	gpio_select_exti(PD6);
	ASSERT_EQ(GPIO_AFIO->EXTISS[1] & (0b1111 << n*4), (0b0011 << n*4));
	gpio_select_exti(PE6);
	ASSERT_EQ(GPIO_AFIO->EXTISS[1] & (0b1111 << n*4), (0b0100 << n*4));
	register_restore_afio();

	// Pin7
	register_backup_afio();
	n = 3;
	gpio_select_exti(PA7);
	ASSERT_EQ(GPIO_AFIO->EXTISS[1] & (0b1111 << n*4), (0b0000 << n*4));
	gpio_select_exti(PB7);
	ASSERT_EQ(GPIO_AFIO->EXTISS[1] & (0b1111 << n*4), (0b0001 << n*4));
	gpio_select_exti(PC7);
	ASSERT_EQ(GPIO_AFIO->EXTISS[1] & (0b1111 << n*4), (0b0010 << n*4));
	gpio_select_exti(PD7);
	ASSERT_EQ(GPIO_AFIO->EXTISS[1] & (0b1111 << n*4), (0b0011 << n*4));
	gpio_select_exti(PE7);
	ASSERT_EQ(GPIO_AFIO->EXTISS[1] & (0b1111 << n*4), (0b0100 << n*4));
	register_restore_afio();

	// Pin8 - EXTISS2
	register_backup_afio();
	n = 0;
	gpio_select_exti(PA8);
	ASSERT_EQ(GPIO_AFIO->EXTISS[2] & (0b1111 << n*4), (0b0000 << n*4));
	gpio_select_exti(PB8);
	ASSERT_EQ(GPIO_AFIO->EXTISS[2] & (0b1111 << n*4), (0b0001 << n*4));
	gpio_select_exti(PC8);
	ASSERT_EQ(GPIO_AFIO->EXTISS[2] & (0b1111 << n*4), (0b0010 << n*4));
	gpio_select_exti(PD8);
	ASSERT_EQ(GPIO_AFIO->EXTISS[2] & (0b1111 << n*4), (0b0011 << n*4));
	gpio_select_exti(PE8);
	ASSERT_EQ(GPIO_AFIO->EXTISS[2] & (0b1111 << n*4), (0b0100 << n*4));
	register_restore_afio();

	// Pin9
	register_backup_afio();
	n = 1;
	gpio_select_exti(PA9);
	ASSERT_EQ(GPIO_AFIO->EXTISS[2] & (0b1111 << n*4), (0b0000 << n*4));
	gpio_select_exti(PB9);
	ASSERT_EQ(GPIO_AFIO->EXTISS[2] & (0b1111 << n*4), (0b0001 << n*4));
	gpio_select_exti(PC9);
	ASSERT_EQ(GPIO_AFIO->EXTISS[2] & (0b1111 << n*4), (0b0010 << n*4));
	gpio_select_exti(PD9);
	ASSERT_EQ(GPIO_AFIO->EXTISS[2] & (0b1111 << n*4), (0b0011 << n*4));
	gpio_select_exti(PE9);
	ASSERT_EQ(GPIO_AFIO->EXTISS[2] & (0b1111 << n*4), (0b0100 << n*4));
	register_restore_afio();

	// Pin6
	register_backup_afio();
	n = 2;
	gpio_select_exti(PA10);
	ASSERT_EQ(GPIO_AFIO->EXTISS[2] & (0b1111 << n*4), (0b0000 << n*4));
	gpio_select_exti(PB10);
	ASSERT_EQ(GPIO_AFIO->EXTISS[2] & (0b1111 << n*4), (0b0001 << n*4));
	gpio_select_exti(PC10);
	ASSERT_EQ(GPIO_AFIO->EXTISS[2] & (0b1111 << n*4), (0b0010 << n*4));
	gpio_select_exti(PD10);
	ASSERT_EQ(GPIO_AFIO->EXTISS[2] & (0b1111 << n*4), (0b0011 << n*4));
	gpio_select_exti(PE10);
	ASSERT_EQ(GPIO_AFIO->EXTISS[2] & (0b1111 << n*4), (0b0100 << n*4));
	register_restore_afio();

	// Pin11
	register_backup_afio();
	n = 3;
	gpio_select_exti(PA11);
	ASSERT_EQ(GPIO_AFIO->EXTISS[2] & (0b1111 << n*4), (0b0000 << n*4));
	gpio_select_exti(PB11);
	ASSERT_EQ(GPIO_AFIO->EXTISS[2] & (0b1111 << n*4), (0b0001 << n*4));
	gpio_select_exti(PC11);
	ASSERT_EQ(GPIO_AFIO->EXTISS[2] & (0b1111 << n*4), (0b0010 << n*4));
	gpio_select_exti(PD11);
	ASSERT_EQ(GPIO_AFIO->EXTISS[2] & (0b1111 << n*4), (0b0011 << n*4));
	gpio_select_exti(PE11);
	ASSERT_EQ(GPIO_AFIO->EXTISS[2] & (0b1111 << n*4), (0b0100 << n*4));
	register_restore_afio();

	// Pin12 - EXTISS3
	register_backup_afio();
	n = 0;
	gpio_select_exti(PA12);
	ASSERT_EQ(GPIO_AFIO->EXTISS[3] & (0b1111 << n*4), (0b0000 << n*4));
	gpio_select_exti(PB12);
	ASSERT_EQ(GPIO_AFIO->EXTISS[3] & (0b1111 << n*4), (0b0001 << n*4));
	gpio_select_exti(PC12);
	ASSERT_EQ(GPIO_AFIO->EXTISS[3] & (0b1111 << n*4), (0b0010 << n*4));
	gpio_select_exti(PD12);
	ASSERT_EQ(GPIO_AFIO->EXTISS[3] & (0b1111 << n*4), (0b0011 << n*4));
	gpio_select_exti(PE12);
	ASSERT_EQ(GPIO_AFIO->EXTISS[3] & (0b1111 << n*4), (0b0100 << n*4));
	register_restore_afio();

	// Pin9
	register_backup_afio();
	n = 1;
	gpio_select_exti(PA13);
	ASSERT_EQ(GPIO_AFIO->EXTISS[3] & (0b1111 << n*4), (0b0000 << n*4));
	gpio_select_exti(PB13);
	ASSERT_EQ(GPIO_AFIO->EXTISS[3] & (0b1111 << n*4), (0b0001 << n*4));
	gpio_select_exti(PC13);
	ASSERT_EQ(GPIO_AFIO->EXTISS[3] & (0b1111 << n*4), (0b0010 << n*4));
	gpio_select_exti(PD13);
	ASSERT_EQ(GPIO_AFIO->EXTISS[3] & (0b1111 << n*4), (0b0011 << n*4));
	gpio_select_exti(PE13);
	ASSERT_EQ(GPIO_AFIO->EXTISS[3] & (0b1111 << n*4), (0b0100 << n*4));
	register_restore_afio();

	// Pin6
	register_backup_afio();
	n = 2;
	gpio_select_exti(PA14);
	ASSERT_EQ(GPIO_AFIO->EXTISS[3] & (0b1111 << n*4), (0b0000 << n*4));
	gpio_select_exti(PB14);
	ASSERT_EQ(GPIO_AFIO->EXTISS[3] & (0b1111 << n*4), (0b0001 << n*4));
	gpio_select_exti(PC14);
	ASSERT_EQ(GPIO_AFIO->EXTISS[3] & (0b1111 << n*4), (0b0010 << n*4));
	gpio_select_exti(PD14);
	ASSERT_EQ(GPIO_AFIO->EXTISS[3] & (0b1111 << n*4), (0b0011 << n*4));
	gpio_select_exti(PE14);
	ASSERT_EQ(GPIO_AFIO->EXTISS[3] & (0b1111 << n*4), (0b0100 << n*4));
	register_restore_afio();

	// Pin15
	register_backup_afio();
	n = 3;
	gpio_select_exti(PA15);
	ASSERT_EQ(GPIO_AFIO->EXTISS[3] & (0b1111 << n*4), (0b0000 << n*4));
	gpio_select_exti(PB15);
	ASSERT_EQ(GPIO_AFIO->EXTISS[3] & (0b1111 << n*4), (0b0001 << n*4));
	gpio_select_exti(PC15);
	ASSERT_EQ(GPIO_AFIO->EXTISS[3] & (0b1111 << n*4), (0b0010 << n*4));
	gpio_select_exti(PD15);
	ASSERT_EQ(GPIO_AFIO->EXTISS[3] & (0b1111 << n*4), (0b0011 << n*4));
	gpio_select_exti(PE15);
	ASSERT_EQ(GPIO_AFIO->EXTISS[3] & (0b1111 << n*4), (0b0100 << n*4));
	register_restore_afio();
}
// ------------------------------------------------------------------------ }}}

// output tests					 											{{{
// ----------------------------------------------------------------------------
static void output_test_pin(GpioPin pin, const char* name, const uint16_t line)
{
	GpioReg* reg = GpioPins[pin].reg;
	uint8_t  bit = GpioPins[pin].bit;

	gpio_init2(pin, OutPP, Speed10MHz);
	gpio_set(pin);
	printf("GPIO output test: %s set... ", name);
	assert_eq2(reg->OCTL & (1 << bit), (1 << bit), __FILE__, line);
	printf("reset... ");
	gpio_reset(pin);
	assert_eq2(reg->OCTL & (0 << bit), (0 << bit), __FILE__, line);

	printf("toggle1... ");
	gpio_toggle(pin);
	assert_eq2(reg->OCTL & (1 << bit), (1 << bit), __FILE__, line);
	printf("toggle0... ");
	assert_eq2(reg->OCTL & (0 << bit), (0 << bit), __FILE__, line);

	printf("get0... ");
	gpio_reset(pin);
	assert_eq2(gpio_get(pin), 0, __FILE__, line);;

	printf("get1... ");
	gpio_set(pin);
	assert_eq2(gpio_get(pin), 1, __FILE__, line);;

	printf("done!\r\n");
}

static void output_test(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_GPIOD);
	rcu_periph_clock_enable(RCU_GPIOE);

	register_backup(GPIOA);
	output_test_pin(PA0,  "PA0",  __LINE__);
	output_test_pin(PA1,  "PA1",  __LINE__);
	output_test_pin(PA2,  "PA2",  __LINE__);
	output_test_pin(PA3,  "PA3",  __LINE__);
	output_test_pin(PA4,  "PA4",  __LINE__);
	output_test_pin(PA5,  "PA5",  __LINE__);
	output_test_pin(PA6,  "PA6",  __LINE__);
	output_test_pin(PA7,  "PA7",  __LINE__);
	// output_test_pin(PA8,  "PA8",  __LINE__);	// button, has pulldown R
	// output_test_pin(PA9,  "PA9",  __LINE__);	// UART TX, printf() will stuck
	// output_test_pin(PA10, "PA10", __LINE__); // UART RX, sometimes will fail on read 0
	output_test_pin(PA11, "PA11", __LINE__);
	output_test_pin(PA12, "PA12", __LINE__);
	// output_test_pin(PA13, "PA13", __LINE__);	// JTMS, will fail on read 0
	// output_test_pin(PA14, "PA14", __LINE__);	// fails - JTCK (not connected, but not in GPIO mode either) - will fail on read 1
	// output_test_pin(PA15, "PA15", __LINE__); // JTDI - will fail on read 0
	register_restore(GPIOA);

	register_backup(GPIOB);
	output_test_pin(PB0,  "PB0",  __LINE__);
	output_test_pin(PB1,  "PB1",  __LINE__);
	output_test_pin(PB2,  "PB2",  __LINE__);
	// output_test_pin(PB3,  "PB3",  __LINE__);	// JTDO - will fail on read 1
	// output_test_pin(PB4,  "PB4",  __LINE__);	// NJTRST - will fail on read 0
	output_test_pin(PB5,  "PB5",  __LINE__);
	output_test_pin(PB6,  "PB6",  __LINE__);
	output_test_pin(PB7,  "PB7",  __LINE__);
	output_test_pin(PB8,  "PB8",  __LINE__);
	output_test_pin(PB9,  "PB9",  __LINE__);
	output_test_pin(PB10, "PB10", __LINE__);
	output_test_pin(PB11, "PB11", __LINE__);
	output_test_pin(PB12, "PB12", __LINE__);
	output_test_pin(PB13, "PB13", __LINE__);
	output_test_pin(PB14, "PB14", __LINE__);
	output_test_pin(PB15, "PB15", __LINE__);
	register_restore(GPIOB);

	register_backup(GPIOC);
	output_test_pin(PC0,  "PC0",  __LINE__);
	output_test_pin(PC1,  "PC1",  __LINE__);
	output_test_pin(PC2,  "PC2",  __LINE__);
	output_test_pin(PC3,  "PC3",  __LINE__);
	output_test_pin(PC4,  "PC4",  __LINE__);
	output_test_pin(PC5,  "PC5",  __LINE__);
	output_test_pin(PC6,  "PC6",  __LINE__);
	output_test_pin(PC7,  "PC7",  __LINE__);
	output_test_pin(PC8,  "PC8",  __LINE__);
	output_test_pin(PC9,  "PC9",  __LINE__);
	output_test_pin(PC10, "PC10", __LINE__);
	output_test_pin(PC11, "PC11", __LINE__);
	output_test_pin(PC12, "PC12", __LINE__);
	output_test_pin(PC13, "PC13", __LINE__);
	output_test_pin(PC14, "PC14", __LINE__);
	output_test_pin(PC15, "PC15", __LINE__);
	register_restore(GPIOC);

	register_backup(GPIOD);
	output_test_pin(PD0,  "PD0",  __LINE__);
	output_test_pin(PD1,  "PD1",  __LINE__);
	output_test_pin(PD2,  "PD2",  __LINE__);
	output_test_pin(PD3,  "PD3",  __LINE__);
	output_test_pin(PD4,  "PD4",  __LINE__);
	output_test_pin(PD5,  "PD5",  __LINE__);
	output_test_pin(PD6,  "PD6",  __LINE__);
	output_test_pin(PD7,  "PD7",  __LINE__);
	output_test_pin(PD8,  "PD8",  __LINE__);
	output_test_pin(PD9,  "PD9",  __LINE__);
	output_test_pin(PD10, "PD10", __LINE__);
	output_test_pin(PD11, "PD11", __LINE__);
	output_test_pin(PD12, "PD12", __LINE__);
	output_test_pin(PD13, "PD13", __LINE__);
	output_test_pin(PD14, "PD14", __LINE__);
	output_test_pin(PD15, "PD15", __LINE__);
	register_restore(GPIOD);

	register_backup(GPIOE);
	output_test_pin(PE0,  "PE0",  __LINE__);
	output_test_pin(PE1,  "PE1",  __LINE__);
	output_test_pin(PE2,  "PE2",  __LINE__);
	output_test_pin(PE3,  "PE3",  __LINE__);
	output_test_pin(PE4,  "PE4",  __LINE__);
	output_test_pin(PE5,  "PE5",  __LINE__);
	output_test_pin(PE6,  "PE6",  __LINE__);
	output_test_pin(PE7,  "PE7",  __LINE__);
	output_test_pin(PE8,  "PE8",  __LINE__);
	output_test_pin(PE9,  "PE9",  __LINE__);
	output_test_pin(PE10, "PE10", __LINE__);
	output_test_pin(PE11, "PE11", __LINE__);
	output_test_pin(PE12, "PE12", __LINE__);
	output_test_pin(PE13, "PE13", __LINE__);
	output_test_pin(PE14, "PE14", __LINE__);
	output_test_pin(PE15, "PE15", __LINE__);
	register_restore(GPIOE);

	// void gpio_reset(GpioPin pin);
	// void gpio_toggle(GpioPin pin);
	// bool gpio_get(GpioPin pin);

}
// ------------------------------------------------------------------------ }}}
// input tests					 											{{{
// ----------------------------------------------------------------------------
static void input_test_pin(GpioPin pin, const char* name, const uint16_t line)
{
	GpioReg* reg = GpioPins[pin].reg;
	uint8_t  bit = GpioPins[pin].bit;

	printf("GPIO input test %s\r\n", name);

	gpio_init2(pin, InPD, Speed10MHz);
	assert_eq2(reg->ISTAT & (0 << bit), (0 << bit), __FILE__, line);

	gpio_init2(pin, InPU, Speed10MHz);
	assert_eq2(reg->ISTAT & (1 << bit), (1 << bit), __FILE__, line);
}

static void input_test(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_GPIOD);
	rcu_periph_clock_enable(RCU_GPIOE);

	register_backup(GPIOA);
	input_test_pin(PA0,  "PA0",  __LINE__);
	// input_test_pin(PA1,  "PA1",  __LINE__);
	// input_test_pin(PA2,  "PA2",  __LINE__);		// sometimes will fail
	input_test_pin(PA3,  "PA3",  __LINE__);
	input_test_pin(PA4,  "PA4",  __LINE__);
	input_test_pin(PA5,  "PA5",  __LINE__);
	input_test_pin(PA6,  "PA6",  __LINE__);
	input_test_pin(PA7,  "PA7",  __LINE__);
	// input_test_pin(PA8,  "PA8",  __LINE__);	// button, has pulldown R
	// input_test_pin(PA9,  "PA9",  __LINE__);	// UART, printf() will stuck
	input_test_pin(PA10, "PA10", __LINE__);
	// input_test_pin(PA11, "PA11", __LINE__);		// USB
	// input_test_pin(PA12, "PA12", __LINE__);		// USB
	input_test_pin(PA13, "PA13", __LINE__);
	// input_test_pin(PA14, "PA14", __LINE__);	// fails - JTCK (not connected, but not in GPIO mode either)
	input_test_pin(PA15, "PA15", __LINE__);
	register_restore(GPIOA);

	register_backup(GPIOB);
	input_test_pin(PB0,  "PB0",  __LINE__);
	input_test_pin(PB1,  "PB1",  __LINE__);
	// input_test_pin(PB2,  "PB2",  __LINE__);
	// input_test_pin(PB3,  "PB3",  __LINE__);
	input_test_pin(PB4,  "PB4",  __LINE__);
	input_test_pin(PB5,  "PB5",  __LINE__);
	input_test_pin(PB6,  "PB6",  __LINE__);
	input_test_pin(PB7,  "PB7",  __LINE__);
	input_test_pin(PB8,  "PB8",  __LINE__);
	input_test_pin(PB9,  "PB9",  __LINE__);
	input_test_pin(PB10, "PB10", __LINE__);
	input_test_pin(PB11, "PB11", __LINE__);
	input_test_pin(PB12, "PB12", __LINE__);
	input_test_pin(PB13, "PB13", __LINE__);
	input_test_pin(PB14, "PB14", __LINE__);
	input_test_pin(PB15, "PB15", __LINE__);
	register_restore(GPIOB);

	register_backup(GPIOC);
	input_test_pin(PC0,  "PC0",  __LINE__);
	input_test_pin(PC1,  "PC1",  __LINE__);
	input_test_pin(PC2,  "PC2",  __LINE__);
	input_test_pin(PC3,  "PC3",  __LINE__);
	input_test_pin(PC4,  "PC4",  __LINE__);
	input_test_pin(PC5,  "PC5",  __LINE__);
	input_test_pin(PC6,  "PC6",  __LINE__);
	input_test_pin(PC7,  "PC7",  __LINE__);
	input_test_pin(PC8,  "PC8",  __LINE__);
	input_test_pin(PC9,  "PC9",  __LINE__);
	input_test_pin(PC10, "PC10", __LINE__);
	input_test_pin(PC11, "PC11", __LINE__);
	input_test_pin(PC12, "PC12", __LINE__);
	// input_test_pin(PC13, "PC13", __LINE__);
	// input_test_pin(PC14, "PC14", __LINE__);
	// input_test_pin(PC15, "PC15", __LINE__);
	register_restore(GPIOC);

	register_backup(GPIOD);
	input_test_pin(PD0,  "PD0",  __LINE__);
	input_test_pin(PD1,  "PD1",  __LINE__);
	input_test_pin(PD2,  "PD2",  __LINE__);
	input_test_pin(PD3,  "PD3",  __LINE__);
	input_test_pin(PD4,  "PD4",  __LINE__);
	input_test_pin(PD5,  "PD5",  __LINE__);
	input_test_pin(PD6,  "PD6",  __LINE__);
	input_test_pin(PD7,  "PD7",  __LINE__);
	input_test_pin(PD8,  "PD8",  __LINE__);
	input_test_pin(PD9,  "PD9",  __LINE__);
	input_test_pin(PD10, "PD10", __LINE__);
	input_test_pin(PD11, "PD11", __LINE__);
	input_test_pin(PD12, "PD12", __LINE__);
	input_test_pin(PD13, "PD13", __LINE__);
	input_test_pin(PD14, "PD14", __LINE__);
	input_test_pin(PD15, "PD15", __LINE__);
	register_restore(GPIOD);

	register_backup(GPIOE);
	input_test_pin(PE0,  "PE0",  __LINE__);
	input_test_pin(PE1,  "PE1",  __LINE__);
	input_test_pin(PE2,  "PE2",  __LINE__);
	input_test_pin(PE3,  "PE3",  __LINE__);
	input_test_pin(PE4,  "PE4",  __LINE__);
	input_test_pin(PE5,  "PE5",  __LINE__);
	input_test_pin(PE6,  "PE6",  __LINE__);
	input_test_pin(PE7,  "PE7",  __LINE__);
	input_test_pin(PE8,  "PE8",  __LINE__);
	input_test_pin(PE9,  "PE9",  __LINE__);
	input_test_pin(PE10, "PE10", __LINE__);
	input_test_pin(PE11, "PE11", __LINE__);
	input_test_pin(PE12, "PE12", __LINE__);
	input_test_pin(PE13, "PE13", __LINE__);
	input_test_pin(PE14, "PE14", __LINE__);
	input_test_pin(PE15, "PE15", __LINE__);
	register_restore(GPIOE);
}
// ------------------------------------------------------------------------ }}}

// run tests																{{{
// ----------------------------------------------------------------------------
void gpio_test(void)
{
	test_check_addresses();
	init_test();
	test_exti();

	output_test();
	input_test();
}
#else
void gpio_test(void)
{
}
// ------------------------------------------------------------------------ }}}
#endif // RUN_TESTS
