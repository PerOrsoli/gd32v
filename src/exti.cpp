// Copyright © 2020 by P.Orsolic. All right reserved
// Created 200101

#include "exti.hpp"

namespace exti
{

static const uint32_t address_EXTI = 0x40010400;

typedef struct
{
	volatile uint32_t INTEN;	// interrupt enable
	volatile uint32_t EVEN;		// event enable
	volatile uint32_t RTEN;		// rising edge trigger enable
	volatile uint32_t FTEN;		// falling edge trigger enable
	volatile uint32_t SWIEV;	// software interrupt event
	volatile uint32_t PD;		// pending register
} ExtiReg;

static volatile ExtiReg* EXTI = (ExtiReg *)address_EXTI;

void init(Source pin, Mode mode, Edge edge)
{
	uint8_t bit = (uint8_t)pin;
	EXTI->INTEN &= ~(1 << bit);
	EXTI->EVEN  &= ~(1 << bit);
	EXTI->RTEN  &= ~(1 << bit);
	EXTI->FTEN  &= ~(1 << bit);
	EXTI->SWIEV &= ~(1 << bit);

	if (mode == Mode::Both)
	{
		EXTI->INTEN |= (1 << bit);
		EXTI->EVEN  |= (1 << bit);
	}
	else if (mode == Mode::Interrupt)
	{
		EXTI->INTEN |= (1 << bit);
	}
	else if (mode == Mode::Event)
	{
		EXTI->EVEN |= (1 << bit);
	}

	if (edge == Edge::Both)
	{
		EXTI->RTEN |= (1 << bit);
		EXTI->FTEN |= (1 << bit);
	}
	else if (edge == Edge::Rising)
	{
		EXTI->RTEN |= (1 << bit);
	}
	else if (edge == Edge::Falling)
	{
		EXTI->FTEN |= (1 << bit);
	}
}

bool interrupt_flag_get(Source pin)
{
	return (EXTI->PD >> (uint8_t)pin);
}

void interrupt_flag_clear(Source pin)
{
	// pending flag is cleared by writting 1 to it
	EXTI->PD |= (1 << (uint8_t)pin);
}

// trigger software EXTI interrupt
void interrupt_sw(Source pin, bool state)
{
	// writting to SWIEW will trigger that ISR, not just enable it (as it is
	// written in documentation
	// eg: writting (1 << 0) to SWIEW will trigger EXTI0 interrupt (if it is enabled)

	if (state == 1)
	{
		EXTI->SWIEV |= (1 << (uint8_t)pin);
	}
	else if (state == 0)
	{
		EXTI->SWIEV &= ~(1 << (uint8_t)pin);
	}
}

// tests						 											{{{
// ----------------------------------------------------------------------------
// backup and restore registers 											{{{
// ----------------------------------------------------------------------------
static uint32_t inten;
static uint32_t even;
static uint32_t rten;
static uint32_t ften;

static void backup_registers(void)
{
	inten = EXTI->INTEN;
	even  = EXTI->EVEN;
	rten  = EXTI->RTEN;
	ften  = EXTI->FTEN;
	// no need to backup swiev - it will reset itself after triggering ISR
}

static void restore_registers(void)
{
	EXTI->INTEN = inten;
	EXTI->EVEN  = even;
	EXTI->RTEN  = rten;
	EXTI->FTEN  = ften;
	// no need to backup swiev - it will reset itself after triggering ISR
}
// ------------------------------------------------------------------------ }}}
// address						 											{{{
// ----------------------------------------------------------------------------
static void test_address(void)
{
	ASSERT_EQ((uint32_t)address_EXTI, 0x40010400);

	ASSERT_EQ((uint32_t)&EXTI->INTEN,	address_EXTI + 0x00);
	ASSERT_EQ((uint32_t)&EXTI->EVEN,	address_EXTI + 0x04);
	ASSERT_EQ((uint32_t)&EXTI->RTEN,	address_EXTI + 0x08);
	ASSERT_EQ((uint32_t)&EXTI->FTEN,	address_EXTI + 0x0C);
	ASSERT_EQ((uint32_t)&EXTI->SWIEV,	address_EXTI + 0x10);
	ASSERT_EQ((uint32_t)&EXTI->PD,		address_EXTI + 0x14);
}
// ------------------------------------------------------------------------ }}}

// init PinX					 											{{{
// ----------------------------------------------------------------------------
static void test_init_pin(Source pinX, const char* name, const uint16_t line)
{
	printf("EXTI test %s\r\n", name);

	uint8_t n = (uint8_t)pinX;
	// pinX test Edge
	backup_registers();
	init(pinX, Mode::Interrupt, Edge::Rising);
	ASSERT_EQ(EXTI->INTEN & (1 << n), (1 << n));
	ASSERT_EQ(EXTI->EVEN, 0);
	ASSERT_EQ(EXTI->RTEN & (1 << n), (1 << n));
	ASSERT_EQ(EXTI->FTEN, 0);
	restore_registers();

	backup_registers();
	init(pinX, Mode::Interrupt, Edge::Falling);
	ASSERT_EQ(EXTI->INTEN & (1 << n), (1 << n));
	ASSERT_EQ(EXTI->EVEN, 0);
	ASSERT_EQ(EXTI->RTEN, 0);
	ASSERT_EQ(EXTI->FTEN &  (1 << n), (1 << n));
	restore_registers();

	backup_registers();
	init(pinX, Mode::Interrupt, Edge::Both);
	ASSERT_EQ(EXTI->INTEN & (1 << n), (1 << n));
	ASSERT_EQ(EXTI->EVEN, 0);
	ASSERT_EQ(EXTI->RTEN &  (1 << n), (1 << n));
	ASSERT_EQ(EXTI->FTEN &  (1 << n), (1 << n));
	restore_registers();

	// pinX test Mode::Event
	backup_registers();
	init(pinX, Mode::Event, Edge::Rising);
	ASSERT_EQ(EXTI->INTEN, 0);
	ASSERT_EQ(EXTI->EVEN &  (1 << n), (1 << n));
	ASSERT_EQ(EXTI->RTEN &  (1 << n), (1 << n));
	ASSERT_EQ(EXTI->FTEN, 0);
	restore_registers();

	backup_registers();
	init(pinX, Mode::Event, Edge::Falling);
	ASSERT_EQ(EXTI->INTEN, 0);
	ASSERT_EQ(EXTI->EVEN &  (1 << n), (1 << n));
	ASSERT_EQ(EXTI->RTEN, 0);
	ASSERT_EQ(EXTI->FTEN &  (1 << n), (1 << n));
	restore_registers();

	backup_registers();
	init(pinX, Mode::Event, Edge::Both);
	ASSERT_EQ(EXTI->INTEN, 0);
	ASSERT_EQ(EXTI->EVEN & (1 << n), (1 << n));
	ASSERT_EQ(EXTI->RTEN & (1 << n), (1 << n));
	ASSERT_EQ(EXTI->FTEN & (1 << n), (1 << n));
	restore_registers();

	// pinX test Mode::Both
	backup_registers();
	init(pinX, Mode::Both, Edge::Rising);
	ASSERT_EQ(EXTI->INTEN & (1 << n), (1 << n));
	ASSERT_EQ(EXTI->EVEN  & (1 << n), (1 << n));
	ASSERT_EQ(EXTI->RTEN  & (1 << n), (1 << n));
	ASSERT_EQ(EXTI->FTEN, 0);
	restore_registers();

	backup_registers();
	init(pinX, Mode::Both, Edge::Falling);
	ASSERT_EQ(EXTI->INTEN & (1 << n), (1 << n));
	ASSERT_EQ(EXTI->EVEN  & (1 << n), (1 << n));
	ASSERT_EQ(EXTI->RTEN, 0);
	ASSERT_EQ(EXTI->FTEN  & (1 << n), (1 << n));
	restore_registers();

	backup_registers();
	init(pinX, Mode::Both, Edge::Both);
	ASSERT_EQ(EXTI->INTEN & (1 << n), (1 << n));
	ASSERT_EQ(EXTI->EVEN  & (1 << n), (1 << n));
	ASSERT_EQ(EXTI->RTEN  & (1 << n), (1 << n));
	ASSERT_EQ(EXTI->FTEN  & (1 << n), (1 << n));
	restore_registers();
}
// ------------------------------------------------------------------------ }}}
// init																		{{{
// ----------------------------------------------------------------------------
static void test_init(void)
{
	test_init_pin(Source::Pin0,  "Pin0",  __LINE__);
	test_init_pin(Source::Pin1,  "Pin1",  __LINE__);
	test_init_pin(Source::Pin2,  "Pin2",  __LINE__);
	test_init_pin(Source::Pin3,  "Pin3",  __LINE__);
	test_init_pin(Source::Pin4,  "Pin4",  __LINE__);
	test_init_pin(Source::Pin5,  "Pin5",  __LINE__);
	test_init_pin(Source::Pin6,  "Pin6",  __LINE__);
	test_init_pin(Source::Pin7,  "Pin7",  __LINE__);
	test_init_pin(Source::Pin8,  "Pin8",  __LINE__);
	test_init_pin(Source::Pin9,  "Pin9",  __LINE__);
	test_init_pin(Source::Pin10, "Pin10", __LINE__);
	test_init_pin(Source::Pin11, "Pin11", __LINE__);
	test_init_pin(Source::Pin12, "Pin12", __LINE__);
	test_init_pin(Source::Pin13, "Pin13", __LINE__);
	test_init_pin(Source::Pin14, "Pin14", __LINE__);
	test_init_pin(Source::Pin15, "Pin15", __LINE__);
	test_init_pin(Source::LVD,   "LVD",   __LINE__);
	test_init_pin(Source::RTC,   "RTC",   __LINE__);
	test_init_pin(Source::USB_wakeup, "USB", __LINE__);
}
// ------------------------------------------------------------------------ }}}

void test(void)
{
	printf("EXTI test start!\r\n");

	test_address();
	test_init();

	printf("EXTI test done!\r\n");
}
// ------------------------------------------------------------------------ }}}

}	// namespace exti

extern "C"	// don't mangle
{
void exti_test_sw_interrupt(void)
{
	dprintf("EXTI doing software interrupt\r\n");
	interrupt_sw(exti::Source::Pin8, 1);
}
}	// extern "C"	// don't mangle
