// Copyright © 2020 by P.Orsolic. All right reserved
// Created 200128
#define DEBUG_RTC
#ifndef DEBUG_RTC
#undef DEBUG
#endif // DEBUG_RTC

#include "config.h"
#include "rtc.hpp"
#ifdef SHELL
#include "shell-cmd.hpp"
#endif // SHELL
#include "utils.hpp"
#include "gd32vf103_bkp.h"
#include "gd32vf103_pmu.h"
#include "gd32vf103_eclic.h"

namespace rtc
{

// HW registers and bits		 											{{{
// ----------------------------------------------------------------------------
const uint32_t address_RTC = 0x40002800;

typedef struct
{
	volatile uint32_t INTEN;
	volatile uint32_t CTL;
	volatile uint32_t PSCH;
	volatile uint32_t PSCL;
	volatile uint32_t DIVH;
	volatile uint32_t DIVL;
	volatile uint32_t CNTH;
	volatile uint32_t CNTL;
	volatile uint32_t ALRMH;
	volatile uint32_t ALRML;
} RtcReg;

static volatile RtcReg* RTC = (RtcReg *)address_RTC;

// ------------------------------------------------------------------------ }}}

// HW tests						 											{{{
// ----------------------------------------------------------------------------
static void test_address(void)
{
	ASSERT_EQ((uint32_t)address_RTC, 0x40002800);

	ASSERT_EQ((uint32_t)&RTC->INTEN,	address_RTC+ 0x00);
	ASSERT_EQ((uint32_t)&RTC->CTL,		address_RTC+ 0x04);
	ASSERT_EQ((uint32_t)&RTC->PSCH,		address_RTC+ 0x08);
	ASSERT_EQ((uint32_t)&RTC->PSCL,		address_RTC+ 0x0C);
	ASSERT_EQ((uint32_t)&RTC->DIVH,		address_RTC+ 0x10);
	ASSERT_EQ((uint32_t)&RTC->DIVL,		address_RTC+ 0x14);
	ASSERT_EQ((uint32_t)&RTC->CNTH,		address_RTC+ 0x18);
	ASSERT_EQ((uint32_t)&RTC->CNTL,		address_RTC+ 0x1C);
	ASSERT_EQ((uint32_t)&RTC->ALRMH,	address_RTC+ 0x20);
	ASSERT_EQ((uint32_t)&RTC->ALRML,	address_RTC+ 0x24);
}

void test(void)
{
	test_address();
}
// ------------------------------------------------------------------------ }}}


void set_interrupt(Interrupt interrupt, bool state)
{
	RTC->INTEN &= ~(1 << (uint8_t)interrupt);
	RTC->INTEN |= (state << (uint8_t)interrupt);
}

bool is_interrupt_enabled(Interrupt interrupt)
{
	return ((RTC->INTEN >> (uint8_t)interrupt) & 0x1);
}

enum class Flag
{
	// bits in CTL register
	LWOFF  = 5,	// RO last write operation finished
	CMF    = 4,	// Configuration mode (0 exit, 1 enter)
	RSYNF  = 3,	// Registers synchronized with APB1 clock
	OVIF   = 2,	// Overflow interrupt
	ALRMIF = 1,	// Alarm interrupt
	SCIF   = 0,	// Second interrupt

};

bool get_flag(Flag flag)
{
	// flags which will be set on various events
	// can be checked in ISR if appropriate bit in INTEN is set
	bool bit = RTC->CTL >> (uint8_t)flag;
	return bit;
}

void clear_flag(Flag flag)
{
	RTC->CTL = ~(1 << (uint8_t)flag);
}

void wait_for(Flag flag)
{
	RTC->CTL &= ~(1 << (uint8_t)flag);
	while ((RTC->CTL & (1 << (uint8_t)flag)) == 0);
}

void set_prescaler(uint32_t prescaler)
{
	// write 20b prescaler to PSCH and PSCL (WO 16b registers)
	ASSERT((prescaler & 0xFFF00000) != 0);	// only 20 bits allowed
	uint16_t prescaler_high = prescaler >> 16;
	uint16_t prescaler_low  = prescaler & 0xFFFF;

	RTC->PSCH = prescaler_high;
	RTC->PSCL = prescaler_low;
}

uint32_t get_divider(void)
{
	// read from DIVH and DIVL (16b RO registers)
	uint16_t div_h = RTC->DIVH;
	uint16_t div_l = RTC->DIVL;
	return (div_h | div_l);
}

uint32_t get_counter(void)
{
	// CNTH + CNTL = 32 bit value which increments every second
	// overflow after 49,710 days = 136 years
	uint16_t low  = RTC->CNTL;
	uint16_t high = RTC->CNTH;
	uint32_t seconds = (high << 16) | low;

	return seconds;
}

// ----------------------------------------------------------------------------
void init(void)
{
	// rcu_periph_clock_enable(RCU_RTC);
	rcu_periph_clock_enable(RCU_BKPI);
	rcu_periph_clock_enable(RCU_PMU);

	pmu_backup_write_enable();

	rcu_osci_on(RCU_LXTAL);
	rcu_osci_stab_wait(RCU_LXTAL);
	rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
	rcu_periph_clock_enable(RCU_RTC);

	wait_for(Flag::RSYNF);
	wait_for(Flag::LWOFF);
}

void write(uint32_t counter)
{
	dprintf("writing counter %d\r\n", counter);
	uint16_t low  = counter & 0xFFFF;
	uint16_t high = counter >> 16;

	// pmu_backup_write_enable();
	wait_for(Flag::LWOFF);
	RTC->CTL |= (1 << (uint8_t)Flag::CMF);	// 	rtc_configuration_mode_enter();
	RTC->CNTH = high;
	RTC->CNTL = low;
	RTC->CTL &= ~(1 << (uint8_t)Flag::CMF);	// 	rtc_configuration_mode_exit();
	wait_for(Flag::LWOFF);

	dprintf("new counterL: %d\r\n", RTC->CNTL);
}

void example(void)
{
	init();

	eclic_global_interrupt_enable();
	eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);
	eclic_irq_enable(RTC_IRQn, 1, 0);
	eclic_irq_enable(RTC_ALARM_IRQn, 1, 0);

	set_interrupt(Interrupt::Second, 1);

	rcu_periph_clock_enable(LEDR_CLK);
	gpio::gpio_init2(LEDR, gpio::Mode::AfioPP, gpio::Speed::Speed10MHz);
	gpio_set(LEDR);		// inverse logic
}

static void print_registers(void)
{
	printf("RTC registers:\r\n");
	printf("  RTC->INTEN: 0x%x\r\n", RTC->INTEN);
	printf("  RTC->CTL:   0x%x\r\n", RTC->CTL);
	printf("  RTC->PSCH:  %d\r\n", RTC->PSCH);
	printf("  RTC->PSCL:  %d\r\n", RTC->PSCL);
	printf("  RTC->DIVH:  %d\r\n", RTC->DIVH);
	printf("  RTC->DIVL:  %d\r\n", RTC->DIVL);
	printf("  RTC->CNTH:  %d\r\n", RTC->CNTH);
	printf("  RTC->CNTL:  %d\r\n", RTC->CNTL);
	printf("  RTC->ALRMH: %d\r\n", RTC->ALRMH);
	printf("  RTC->ALRML: %d\r\n", RTC->ALRML);
}

// ----------------------------------------------------------------------------
#ifdef SHELL
uint8_t cmd(char *argv[])
{
#ifndef MCU_RUN
	printf("PC run, here is %s() in %s\r\n", __func__, __FILE__);
#else
	const char* name   = argv[0];		// name of this command
	const char  action = argv[1][0];	// 'r' or 't' - 1 char
	const char* value  = argv[2];		//
	uint32_t ncounter = 0;

	switch (action)
	{
		case 'r':
			print_registers();
			break;
		case 'w':
			if (str2num(value, &ncounter) != 0)
			{
				eprintf("error converting \"%s\" to number\r\n", value);
				return SHELL_RETURN_NOK;
			}
			dprintf("will write new counter: %d (string: %s)\r\n", ncounter, value);
			write(ncounter);
			break;
		default:
			break;
	}

#endif
	return SHELL_RETURN_OK;
}
#endif // SHELL

} // namespace

extern "C"	// don't mangle
{
void RTC_IRQHandler(void)
{
	clear_flag(rtc::Flag::SCIF);
	// dprintf("Here is %s()\r\n", __func__);
	gpio_toggle(LEDR);

	static uint8_t counter = 0;
	static uint8_t min;

	if (counter++ == 60)
	{
		counter = 0;
		printf("RTC uptime: %d minute\r\n", ++min);
	}
}

void RTC_Alarm_IRQHandler(void)
{
	// dprintf("Here is %s()\r\n", __func__);
}
}	// extern "C"	// don't mangle
