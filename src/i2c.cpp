// Copyright © 2020 by P.Orsolic. All right reserved
// created 141205 - STM32F1 C
// Created 200104 - GD32V C+

#include "i2c.hpp"
#include "i2c-patch.h"
#include "delay.h"
#include "libc-bits.h"	// strlen()
#include "gd32vf103_rcu.h"
#include "gd32vf103_i2c.h"

#define I2C_DTCY_2		((uint32_t)0x00000000U)
#define I2C0			I2C_BASE

namespace i2c
{
	using namespace gpio;

// private types:

// HW registers and bits		 											{{{
// ----------------------------------------------------------------------------
const uint32_t address_I2C0 = 0x40005400;
const uint32_t address_I2C1 = 0x40005800;

typedef struct
{
	volatile uint32_t	CTL0;		// control 0
	volatile uint32_t	CTL1;		// control 1
	volatile uint32_t	SADDR0;		// slave address
	volatile uint32_t	SADDR1;		// slave address for dual address mode
	volatile uint32_t	DATA;
	volatile uint32_t	STAT0;
	volatile uint32_t	STAT1;
	volatile uint32_t	CKCFG;
	volatile uint32_t	RT;
} I2cReg;

static volatile I2cReg* myI2C0 = (I2cReg*)address_I2C0;
static volatile I2cReg* myI2C1 = (I2cReg*)address_I2C1;

typedef struct
{
	Device				dev;
	volatile I2cReg*	reg;
	GpioPin				scl;
	GpioPin				sda;
} DevMap;

DevMap DevMaps[] = {
	// dev				reg		scl		sda
	{Device::myI2C0,	myI2C0, PB6,	PB7},
	{Device::myI2C1,	myI2C1, PB10,	PB11},
};

enum class Ctl0Bits: uint8_t
{
	SRESET		= 15,	// software reset
	SALT		= 13,	// SMBus ...
	PECTRANS	= 12,	// PEC ...
	POAP		= 11,	// NACK position
	ACKEN		= 10,	// to ACK or to not ACK
	STOP		= 9,	// generate STOP
	START		= 8,	// generate START
	SS			= 7,	// stretch clock when data is not ready in slave mode
	GCEN		= 6,	// general call
	PECEN		= 5,	// PEC...
	ARPEN		= 4,	// SMBus...
	SMBSEL		= 3,	// SMBus...
	SMBEN		= 1,	// SMBus...
	I2CEN		= 0,	// enable
};

bool get_flag(Device dev, Flag flag);
void ack(Device dev);
void nack(Device dev);

// Basic error "handling":
// - don't stuck until forever if there is error on I2C
// - has simple SW timeout
// TODO: return something and check value later in code (one day, for proper error handling which will be hard to implement correctly and which will be easy to use)
#define wait_until_flag(dev, flag, condition) _wait_until_flag(dev, flag, condition, __func__, __FILE__, __LINE__);
void _wait_until_flag(Device dev, Flag flag, uint8_t condition, const char* fn_name, const char* file, uint32_t line)
{
	uint32_t timeout = I2C_TIMEOUT_MAX;

	while (get_flag(dev, flag) == condition)
	{
		if ((timeout--) == 0)
		{
			rprintf("TIMEOUT for I2C dev: %d in func: %s()\r\n", dev, fn_name);
			return;
		}
	}
}

// TODO 200126: replace Timed()
// #define wait_while(condition) \
// 	uint32_t timeout2 = 0xFFF;\
// 	while(condition) { if ((timeout2--) == 0) { \
// 		rprintf("I2C TIMEOUT in func: %s()\r\n", __func__); return; \
// 	} }

void reset_bus(Device dev)
{
	volatile I2cReg* reg = DevMaps[(uint8_t)dev].reg;
	// while (reg->STAT1 & (1 << (uint8_t)Stat1Bits::I2CBSY));
	reg->STAT0 |= (1 << (uint8_t)Ctl0Bits::SRESET);
}

void ack(Device dev)
{
	volatile I2cReg* reg = DevMaps[(uint8_t)dev].reg;
	reg->CTL0 |= (1 << (uint8_t)Ctl0Bits::ACKEN);
	// dprintf("wait ack\r\n");
	while (!(reg->CTL0 & (1 << (uint8_t)Ctl0Bits::ACKEN)));
}

void nack(Device dev)
{
	volatile I2cReg* reg = DevMaps[(uint8_t)dev].reg;
	reg->CTL0 &= ~(1 << (uint8_t)Ctl0Bits::ACKEN);
	while ((reg->CTL0 & (1 << (uint8_t)Ctl0Bits::ACKEN)));
}

void nack_position(Device dev, NackPosition pos)
{
	volatile I2cReg* reg = DevMaps[(uint8_t)dev].reg;

	if (pos == NackPosition::Current)
	{
		reg->CTL0 &= ~(1 << (uint8_t)Ctl0Bits::POAP);
	}
	if (pos == NackPosition::Next)
	{
		reg->CTL0 |= (1 << (uint8_t)Ctl0Bits::POAP);
	}
}

enum class Ctl1Bits: uint8_t
{
	DMALST	= 12,	// something DMA related
	DMAON	= 11,	// DMA enable/disable
	BUFIE	= 10,	// interrupt when TBE = 1 or RBNE = 1
	EVIE	= 9,	// event interrupt enable (when ADDSEND = 1, BTC = 1, or (if BUFIE =1) RBNE=1 or TBE=1
	ERRIE	= 8,	// error interrupt enable
	I2CLK	= 0,	// [5b] value of APB1 clock in MHz. Must be between 2 MHz (0b10) and 54 MHz (110110)
	// If SCL == 100 kHz -> I2CLK must be >= 2MHz
	// If SCL == 400 kHz -> I2CLK must be >= 8MHz
	// If SCL >  400 kHz -> I2CLK must be >= 24MHz
};

enum class Saddr0Bits: uint8_t
{
	ADDFORMAT	= 15,
	ADDRESS		= 0,	// 10 or 7 bit part of address
						// b0 - R or W bit of address
};

enum class AddressLength: uint8_t
{
	// bit SADDR0.ADDFORMAT
	SevenBits	= 0,
	TenBits		= 1,
};

enum class Stat0Bits: uint8_t
{
	SMBALT		= 15,	// SMBus...
	SMBTO		= 14,	// SMBus...
	PECERR		= 12,	// PEC...
	OUERR		= 11,	// over/under-run in slave mode, when SCL stretching id siabled
	AERR		= 10,	// ACK error
	LOSTARB		= 9,	// arbitration lost
	BERR		= 8,	// unexpected START or STOP occurs
	TBE			= 7,	// TX empty
	RBNE		= 6,	// RX not empty
	STPDET		= 4,	// STOP detected in slave mode
	ADD10SEND	= 3,	// 10 bit address sent
	BTC			= 2,	// byte transmission completed
	ADDSEND		= 1,	// address is sent to slave
	SBSEND		= 0,	// START sent to slave
};

enum class Stat1Bits: uint8_t
{
	PECV		= 8,	// [8b] PEC...
	DUMODF		= 7,	// related to dual address mode thing
	HSTSMB		= 6,	// SMBus..
	DEFSMB		= 5,	// SMBus..
	RXGC		= 4,	// general call
	TR			= 2,	// Tx or RX? 0 = Rx, 1 = TX
	I2CBSY		= 1,	// 1 when busy
	MASTER		= 0,	// 0 = slave, 1 = master mode
};


// void set_i2c_clock(

// ------------------------------------------------------------------------ }}}
// HW tests						 											{{{
// ----------------------------------------------------------------------------
static void test_address(void)
{
	ASSERT_EQ((uint32_t)address_I2C0, 0x40005400);
	ASSERT_EQ((uint32_t)address_I2C1, 0x40005800);

	ASSERT_EQ((uint32_t)&myI2C0->CTL0,		address_I2C0 + 0x00);
	ASSERT_EQ((uint32_t)&myI2C0->CTL1,		address_I2C0 + 0x04);
	ASSERT_EQ((uint32_t)&myI2C0->SADDR0,	address_I2C0 + 0x08);
	ASSERT_EQ((uint32_t)&myI2C0->SADDR1,	address_I2C0 + 0x0C);
	ASSERT_EQ((uint32_t)&myI2C0->DATA,		address_I2C0 + 0x10);
	ASSERT_EQ((uint32_t)&myI2C0->STAT0,		address_I2C0 + 0x14);
	ASSERT_EQ((uint32_t)&myI2C0->STAT1,		address_I2C0 + 0x18);
	ASSERT_EQ((uint32_t)&myI2C0->CKCFG,		address_I2C0 + 0x1C);
	ASSERT_EQ((uint32_t)&myI2C0->RT,		address_I2C0 + 0x20);
}

static void test_map(void)
{
	volatile I2cReg* reg = DevMaps[(uint8_t)Device::myI2C0].reg;
	ASSERT_EQ((uint32_t)reg, address_I2C0);

	reg = DevMaps[(uint8_t)Device::myI2C1].reg;
	ASSERT_EQ((uint32_t)reg, address_I2C1);
}

void test(void)
{
	test_address();
	test_map();
}
// ------------------------------------------------------------------------ }}}

bool get_flag(Device dev, Flag flag)
{
	// not in map[] because there are 2 buses and 2 registers per bus
	volatile I2cReg* reg = DevMaps[(uint8_t)dev].reg;
	uint8_t bit;
	switch (flag)
	{
		case Flag::IsTransmitter:
			bit = (uint8_t)Stat1Bits::TR;
			return ((reg->STAT1) & (1 << bit)) >> bit;
		// case Flag::IsBusy:
		case Flag::BSY:
			bit = (uint8_t)Stat1Bits::I2CBSY;
			return ((reg->STAT1) & (1 << bit)) >> bit;
		case Flag::IsMaster:
			bit = (uint8_t)Stat1Bits::MASTER;
			return ((reg->STAT1) & (1 << bit)) >> bit;
		case Flag::TxEmpty:
			bit = (uint8_t)Stat0Bits::TBE;
			return ((reg->STAT0) & (1 << bit)) >> bit;
		case Flag::RxNotEmpty:
		case Flag::RBNE:
			bit = (uint8_t)Stat0Bits::RBNE;
			return ((reg->STAT0) & (1 << bit)) >> bit;
		// case Flag::ByteTxComplete:
		case Flag::BTC:
			bit = (uint8_t)Stat0Bits::BTC;
			return ((reg->STAT0) & (1 << bit)) >> bit;
		case Flag::ADDSEND:
			bit = (uint8_t)Stat0Bits::ADDSEND;
			return ((reg->STAT0) & (1 << bit)) >> bit;
		// case Flag::StartSent:
		case Flag::SBSEND:
			bit = (uint8_t)Stat0Bits::SBSEND;
			return ((reg->STAT0) & (1 << bit)) >> bit;
		case Flag::STPDET:
			bit = (uint8_t)Stat0Bits::STPDET;
			return ((reg->STAT0) & (1 << bit)) >> bit;
	};
}

void clear_flag(Device dev, Flag flag)
{
	volatile I2cReg* reg = DevMaps[(uint8_t)dev].reg;
	switch (flag)
	{
		case Flag::ADDSEND:
			// cleared by reading STAT0 and STAT1
			(void)reg->STAT0;
			(void)reg->STAT1;
			break;
		default:
			eprintf("Flag not cleared: 0x%x\r\n", flag);
			break;
	};
}

void init(Device dev, Speed speed, DutyCycle duty)
{
	volatile I2cReg* reg = DevMaps[(uint8_t)dev].reg;

	printf("CTL1.I2CLK: %d\r\n",	reg->CTL1 & 0x1F);
	i2c_clock_config2(I2C0, 400000, I2C_DTCY_2);
	printf("CTL1.I2CLK: %d\r\n",	reg->CTL1 & 0xFF);
	printf("CTL1: 0x%d\r\n",	reg->CTL1);
	// 100 kHz: CLK: 54, CTL1: 0x54 = 84 = 1010100
	// 200 kHz: CLK: 54, CTL1:

	// write APB1 clock in MHz to CTL1.I2CLK
	uint8_t clk_apb1 = rcu_clock_freq_get(CK_APB1) / 1000000;		// TODO 200106: replace with my RCU
	printf("APB1 clock: %d MHz\r\n", clk_apb1);

	switch (speed)
	{
		case Speed::Speed100kHz:
			ASSERT(clk_apb1 >= 2);
			break;
	}

	reg->CTL1 |= clk_apb1 << (uint8_t)Ctl1Bits::I2CLK;
	enable(dev);
}

// generate start
void start(Device dev)
{
	volatile I2cReg* reg = DevMaps[(uint8_t)dev].reg;

	// wait_until_flag(dev, Flag::IsBusy, 1);
	reg->CTL0 |= (1 << (uint8_t)Ctl0Bits::START);
	wait_until_flag(dev, Flag::SBSEND, 0);
}

void restart(Device dev)
{
	// restart is just start without previous stop
	volatile I2cReg* reg = DevMaps[(uint8_t)dev].reg;

	reg->CTL0 |= (1 << (uint8_t)Ctl0Bits::START);
	wait_until_flag(dev, Flag::SBSEND, 0);
}

void stop(Device dev)
{
	volatile I2cReg* reg = DevMaps[(uint8_t)dev].reg;
	reg->CTL0 |= (1 << (uint8_t)Ctl0Bits::STOP);
}

void enable(Device dev)
{
	volatile I2cReg* reg = DevMaps[(uint8_t)dev].reg;
	reg->CTL0 |= (1 << (uint8_t)Ctl0Bits::I2CEN);
}

void disable(Device dev)
{
	wait_until_flag(dev, Flag::BSY, 1);
	volatile I2cReg* reg = DevMaps[(uint8_t)dev].reg;
	reg->CTL0 &= ~(1 << (uint8_t)Ctl0Bits::I2CEN);
}

// send slave address to the bus. direction depends on mode (RX or TX)
// address is send to the bus, SADDRx are used then MCU I2C is in slave mode
// only implemented for 7 bit adresses, don't have any 10bit i2c device
void send_addr(Device dev, uint8_t address, Mode mode)
{
	volatile I2cReg* reg = DevMaps[(uint8_t)dev].reg;

	// clear SBSEND by reading STAT0 and putting header to DATA
	reg->STAT0;

	reg->DATA = ((address & 0xFE) | (uint8_t)mode);
	wait_until_flag(dev, Flag::ADDSEND, 0);

	// clear ADDSEND flag:
	reg->STAT0;
	reg->STAT1;
}

uint8_t read(Device dev)
{
	volatile I2cReg* reg = DevMaps[(uint8_t)dev].reg;

	wait_until_flag(dev, Flag::RxNotEmpty, 0);
	uint8_t data = reg->DATA;
	// RBNE is automatically cleared

	return data;
}

void write(Device dev, uint8_t data)
{
	volatile I2cReg* reg = DevMaps[(uint8_t)dev].reg;

	reg->DATA = data;
	wait_until_flag(dev, Flag::TxEmpty, 0);
}
























#define I2C_FLAG_RXNE                   ((uint32_t)0x10000040)
#define I2C_FLAG_ADDR                   ((uint32_t)0x10000002)
#define I2C_FLAG_BUSY                   ((uint32_t)0x00020000)
#define I2C_FLAG_STOPF                  ((uint32_t)0x10000010)
#define  I2C_Direction_Receiver         ((uint8_t)0x01)
#define I2C_NACKPosition_Next           ((uint16_t)0x0800)
#define I2C_NACKPosition_Current        ((uint16_t)0xF7FF)
typedef struct
{
	__IO uint16_t CR1;
	uint16_t  RESERVED0;
	__IO uint16_t CR2;
	uint16_t  RESERVED1;
	__IO uint16_t OAR1;
	uint16_t  RESERVED2;
	__IO uint16_t OAR2;
	uint16_t  RESERVED3;
	__IO uint16_t DR;
	uint16_t  RESERVED4;
	__IO uint16_t SR1;
	uint16_t  RESERVED5;
	__IO uint16_t SR2;
	uint16_t  RESERVED6;
	__IO uint16_t CCR;
	uint16_t  RESERVED7;
	__IO uint16_t TRISE;
	uint16_t  RESERVED8;
} I2C_TypeDef;

#define FLAG_Mask               ((uint32_t)0x00FFFFFF)
#define  I2C_EVENT_MASTER_MODE_SELECT	((uint32_t)0x00030001)	// BUSY, MSL and SB flag
#define  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED        ((uint32_t)0x00070082)  /* BUSY, MSL, ADDR, TXE and TRA flags */
#define  I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED           ((uint32_t)0x00030002)  /* BUSY, MSL and ADDR flags */
#define  I2C_EVENT_MASTER_BYTE_RECEIVED                    ((uint32_t)0x00030040)  /* BUSY, MSL and RXNE flags */

bool check_event(Event event)
{
	uint32_t lastevent = 0;

	uint32_t stat0 = myI2C0->STAT0;
	uint32_t stat1 = myI2C0->STAT1;

	const uint32_t mask = 0xFFFFFF;		// STAT1 has only 8 bits for flags/events
	lastevent = (stat1 << 16 | stat0) & mask;

	if ((lastevent & (uint32_t)event) == (uint32_t)event)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// TODO:
// interrupt enable/disable
// interrupt flag get/set/clear

} // namespace
