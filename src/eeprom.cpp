// Copyright © 2020 by P.Orsolic. All right reserved
// created 141205 - STM32F1 C
// Created 200104 - GD32V C+

// #define DEBUG_EEPROM
#ifndef DEBUG_EEPROM
#undef DEBUG
#endif // DEBUG_EEPROM

#include "eeprom.hpp"
#include "delay.h"
#include "libc-bits.h"	// strlen()
#ifdef SHELL
#include "shell-cmd.hpp"
#include "utils.hpp"
#endif // SHELL

#define I2C_DTCY_2  ((uint32_t)0x00000000U)
#define I2C0        I2C_BASE

static i2c::Device dev = i2c::Device::wrong;

namespace eeprom
{

	using namespace i2c;
	using namespace eeprom;

void init(i2c::Device arg_dev)
{
	// GPIO init
	using namespace gpio;
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_AF);

	dev = arg_dev;

	if (dev == i2c::Device::myI2C0)
	{
		rcu_periph_clock_enable(RCU_I2C0);
		gpio_init2(PB6, AfioOD, Speed10MHz);	// SCL
		gpio_init2(PB7, AfioOD, Speed10MHz);	// SDA
		init(Device::myI2C0, i2c::Speed::Speed400kHz, DutyCycle::Duty2);
	}
	else if (dev == i2c::Device::myI2C1)
	{
		rcu_periph_clock_enable(RCU_I2C1);
		gpio_init2(PB10, AfioOD, Speed10MHz);	// SCL
		gpio_init2(PB11, AfioOD, Speed10MHz);	// SDA
		init(Device::myI2C1, i2c::Speed::Speed400kHz, DutyCycle::Duty2);
	}
	else
	{
		eprintf("Wrong I2C device: %d\r\n", dev);
	}
}

// check address				 											{{{
// ----------------------------------------------------------------------------
static bool is_addr_valid(uint16_t addr)
{
	// 256 kb = 32 kB
	// max address  = 32768 = 0x8000

	if ((addr > 0) || (addr < 32768))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

#define ADDR_CHECK(addr) \
	if (is_addr_valid(addr) == 0) \
	{ \
		eprintf("invalid address for 256K device: %d\r\n", addr);\
		return 0; \
	}

#define ADDR_CHECK_VOID(addr) \
	if (is_addr_valid(addr) == 0) \
	{ \
	eprintf("invalid address for 256K device: %d\r\n", addr);\
	}
// ------------------------------------------------------------------------ }}}

uint8_t read(uint16_t addr)
{
	ADDR_CHECK(addr);

	uint8_t addrH = addr >> 8;
	uint8_t addrL = addr & 0x00FF;
	uint8_t data = 0;

	using namespace i2c;
	start(dev);
	send_addr(dev, EEPROM_ADDR, Mode::Write);
	write(dev, addrH);
	write(dev, addrL);

	restart(dev);
	send_addr(dev, EEPROM_ADDR, Mode::Read);
	nack(dev);
	data = read(dev);

	stop(dev);

	return data;
}

void read_many(uint16_t addr, uint8_t* data, uint8_t n)
{
	ADDR_CHECK_VOID(addr);

	uint8_t addrH = addr >> 8;
	uint8_t addrL = addr & 0x00FF;

	using namespace i2c;
	start(dev);
	send_addr(dev, EEPROM_ADDR, Mode::Write);

	write(dev, addrH);
	write(dev, addrL);

	restart(dev);
	send_addr(dev, EEPROM_ADDR, Mode::Read);
	ack(dev);

	while (n-- > 1)
	{
		*data++ = read(dev);
	}
	nack(dev);
	*data++ = read(dev);


	stop(dev);
}

void write(uint16_t addr, uint8_t data)
{
	ADDR_CHECK_VOID(addr);
	uint8_t addrH = addr >> 8;
	uint8_t addrL = addr & 0x00FF;

	start(dev);
	send_addr(dev, EEPROM_ADDR, Mode::Write);
	write(dev, addrH);
	write(dev, addrL);
	write(dev, data);

	stop(dev);
	delay_ms(20);	// datasheet, page 5, max 20 ms write delay, after _stop()
}

void write_many(uint16_t addr, const uint8_t data[], uint16_t n)
{
	ADDR_CHECK_VOID(addr);
	uint8_t addrH = addr >> 8;
	uint8_t addrL = addr & 0x00FF;

	start(dev);
	send_addr(dev, EEPROM_ADDR, Mode::Write);
	write(dev, addrH);
	write(dev, addrL);

	for (uint8_t i=0; i<n; i++)
	{
		write(dev, data[i]);
	}

	stop(dev);
	delay_ms(20);	// datasheet, page 5, max 20 ms write delay, after _stop()
}

void erase(uint16_t addr)
{
	write(addr, 0xFF);
}

void erase_many(uint16_t addr, uint16_t n)
{
	write_many(addr, 0, n);
}

// ----------------------------------------------------------------------------

void example(i2c::Device dev)
{
	init(dev);
	ack(dev);

	uint8_t value;

	// write(0x0000, 0xDE);
	// write(0x0001, 0xAD);

	const char* data = "ovo je neki noviji tekst";
	// write_many(0x10, (uint8_t *)data, strlen(data));

	value = read(0x1234);
	printf("EEPROM read: 0x%x\r\n", value);
	value = read(0x1235);
	printf("EEPROM read: 0x%x\r\n", value);

	return;

	// for (uint16_t i=0; i<32768; i++)
	for (uint16_t i=0; i<0x30; i++)	// a little speedup in testing
	{
		value = read(i);
		if (value != 0)
		{
			printf("EEPROM [0x%04x]: 0x%02x ch: %c\r\n", i, value, value);
		}
	}
}

static void _cmd_read(uint16_t address, uint8_t n)
{
	dprintf("Reading address: 0x%x n: %d\r\n", address, n);
	uint8_t data[100] = {0};
	read_many(address, data, n);

	for (uint8_t i = 0; i < n; i++)
	{
		printf("[0x%x]: 0x%x (%d)\r\n", address, data[i], data[i]);
		address++;
	}
}

static void _cmd_write(uint16_t address, uint8_t value)
{
	dprintf("write address: 0x%x data: %d\r\n", address, value);

	write(address, value);
}

#ifdef SHELL
uint8_t cmd(char *argv[])
{
#ifdef MCU_RUN
	uint8_t argc = get_argc(argv);
	if (argc < 2)
	{
		eprintf("Not enough arguments: %d\r\n", argc);
		printf("--> Usage: %s r <address> <n bytes>\r\n", argv[0]);
		printf("--> Usage: %s w <address> <u8 value>\r\n", argv[0]);
		printf("--> Usage: %s e <address>\r\n", argv[0]);
		return 0;
	}
#else
	printf("PC run, here is %s() in %s\r\n", __func__, __FILE__);
#endif

	const char* name   = argv[0];		// name of this command
	const char  action = argv[1][0];	// 'r' or 'w' - 1 char
	const char* addr   = argv[2];		// address to read from or to write to
	const char* width  = argv[3];		// width when reading
	const char* data   = argv[3];		// data to write
	uint32_t naddr     = 0;	// data converted from char to number
	uint32_t ndata     = 0;	// data converted from char to number
	uint32_t nwidth    = 0;	// width converted from char to number

	if (str2num(addr, &naddr) != 0)
	{
		eprintf("error converting \"%s\" to number\r\n", addr);
		return 0;
	}

	dprintf("name: %s action: %c data: %c addr: 0x%x\r\n", name, action, data, naddr);

	switch (action)
	{
		case 'r':
		case 'R':
			if (argc > 2)	// 'width' is specified - use it
			{
				if (str2num(width, &nwidth) != 0)
				{
					eprintf("error converting \"%s\" to number\r\n", width);
					return 0;
				}
			}
			else
			{
				nwidth = 1;		// no width specified - use default '1'
			}
			_cmd_read(naddr, nwidth);
			break;
		case 'w':
		case 'W':
			if (argc < 3)
			{
				eprintf("Not enough arguments for write: %d\r\n", argc);
				return 0;
			}
			if (str2num(data, &ndata) != 0)
			{
				eprintf("error converting \"%s\" to number\r\n", data);
				return 0;
			}
			if (ndata > 0xFF)
			{
				yprintf("Warning, truncating data (%d -> %d)\r\n", ndata, (uint8_t)ndata);
			}
			_cmd_write(naddr, ndata);
			break;
		case 'i':
			// reinit ROM
			init(dev);
			break;
		case 'e':
			erase(naddr);
			break;
		default:
			eprintf("unknown action: %c\r\n", action);
			return 0;
	}

	return 1;
}
#endif // SHELL

} // namespace
