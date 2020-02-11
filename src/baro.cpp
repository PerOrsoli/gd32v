// Copyright © 2020 by P.Orsolic. All right reserved
// created 140916 - STM32 C version
// created 200104 - GDM32V C version

// #define DEBUG_BARO
#ifndef DEBUG_BARO
#undef DEBUG
#endif // DEBUG_BARO

#include "baro.hpp"
#include "debug.h"
// #include "gd32vf103_i2c.h"
#ifdef SHELL
#include "shell-cmd.hpp"
#endif // SHELL

namespace baro
{
	static void write_reg(uint8_t reg, uint8_t data);
	static uint16_t read_reg(uint8_t reg);
	static void calibration(void);
	static uint16_t get_ut(void);
	static uint16_t get_up(void);

	using namespace i2c;
	static i2c::Device dev = i2c::Device::wrong;
	static bool bmp_initialized = 0;

#define BARO_ADDR	0xEE
#define REG_ID		0xD0
#define REG_RESET	0xE0	// WO register

#define REG_MSB					0xF6
#define REG_CONTROL				0xF4
#define CMD_GET_TEMPERATURE		0x2E
#define CMD_GET_PRESSURE_OSS0	0x34
#define CMD_GET_PRESSURE_OSS1	0x74
#define CMD_GET_PRESSURE_OSS2	0xB4
#define CMD_GET_PRESSURE_OSS3	0xF4

// #define OSS						1
// #define CMD_OSS					CMD_GET_PRESSURE_OSS1
#define OSS						0
#define CMD_OSS					CMD_GET_PRESSURE_OSS0

// no need (nor support, for now) for pow()
// TODO: 2^15 = 1 << 15
#define pow(base,exp)	(1 << (exp+1))
#define pow2_4  16		// pow(2,4)
#define pow2_11 2048	// pow(2,11)
#define pow2_12 4096	// pow(2,12)
#define pow2_13 8192	// pow(2,13)
#define pow2_15 32768	// pow(2,15)
#define pow2_16 65536	// pow(2,15)

static int16_t  AC1;
static int16_t  AC2;
static int16_t  AC3;
static uint16_t AC4;
static uint16_t AC5;
static uint16_t AC6;
static int16_t  B1;
static int16_t  B2;
static int16_t  MB;
static int16_t  MC;
static int16_t  MD;

// static functions:
// write																	{{{
// ----------------------------------------------------------------------------
static void write_reg(uint8_t reg, uint8_t data)
{
	ack(dev);
	start(dev);
	send_addr(dev, BARO_ADDR, Mode::Write);
	write(dev, reg);
	write(dev, data);
	stop(dev);
}
// ------------------------------------------------------------------------ }}}
// read																		{{{
// ----------------------------------------------------------------------------
static uint16_t read_reg(uint8_t reg)
{
	uint16_t received = 0;
	uint8_t receivedH = 0;
	uint8_t receivedL = 0;

	start(dev);
	send_addr(dev, BARO_ADDR, Mode::Write);
	write(dev, reg);

	restart(dev);
	send_addr(dev, BARO_ADDR, Mode::Read);
	ack(dev);
	receivedH = read(dev);
	nack(dev);
	nack_position(dev, NackPosition::Current);
	receivedL = read(dev);
	stop(dev);

	received = (receivedH << 8) | receivedL;
	return received;
}
// ------------------------------------------------------------------------ }}}
// calibration																{{{
// ----------------------------------------------------------------------------
static void calibration(void)
{
	AC1 = read_reg(0xAA);
	AC2 = read_reg(0xAC);
	AC3 = read_reg(0xAE);
	AC4 = read_reg(0xB0);
	AC5 = read_reg(0xB2);
	AC6 = read_reg(0xB4);
	B1  = read_reg(0xB6);
	B2  = read_reg(0xB8);
	MB  = read_reg(0xBA);
	MC  = read_reg(0xBC);
	MD  = read_reg(0xBE);

	dprintf("AC1: %d\r\n", AC1);
	dprintf("AC2: %d\r\n", AC2);
	dprintf("AC3: %d\r\n", AC3);
	dprintf("AC4: %d\r\n", AC4);
	dprintf("AC5: %d\r\n", AC5);
	dprintf("AC6: %d\r\n", AC6);
	dprintf("B1:  %d\r\n", B1);
	dprintf("B2:  %d\r\n", B2);
	dprintf("MB:  %d\r\n", MB);
	dprintf("MC:  %d\r\n", MC);
	dprintf("MD:  %d\r\n", MD);
}
// ------------------------------------------------------------------------ }}}
// get UT						 											{{{
// ----------------------------------------------------------------------------
static uint16_t get_ut(void)
{
	write_reg(REG_CONTROL, CMD_GET_TEMPERATURE);
	delay_ms(5);		// max 4.5 ms
	uint16_t ut = read_reg(REG_MSB);
	return ut;
}
// ------------------------------------------------------------------------ }}}
// get UP																	{{{
// ----------------------------------------------------------------------------
static uint16_t get_up(void)
{
	write_reg(REG_CONTROL, CMD_OSS);
	delay_ms(8);		// max 7.5 ms
	uint16_t up = read_reg(REG_MSB);
	return up;
}
// ------------------------------------------------------------------------ }}}

// init							 											{{{
// ----------------------------------------------------------------------------
void init(i2c::Device arg_dev)
{
	dev = arg_dev;

	using namespace gpio;
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_AF);
	rcu_periph_clock_enable(RCU_I2C0);
	gpio_init2(PB6, AfioOD, Speed50MHz);	// SCL
	gpio_init2(PB7, AfioOD, Speed50MHz);	// SDA

	// i2c_clock_config(I2C0, 100000, I2C_DTCY_2);
	// i2c_enable(I2C0);
	init(arg_dev, i2c::Speed::Speed400kHz, i2c::DutyCycle::Duty2);
	bmp_initialized = 1;

	delay_ms(10);	// start up time
	calibration();
}
// ------------------------------------------------------------------------ }}}
// reset																	{{{
// ----------------------------------------------------------------------------
void reset(void)
{
	write_reg(REG_RESET, 0xFF);
}
// ------------------------------------------------------------------------ }}}
// get id																	{{{
// ----------------------------------------------------------------------------
uint8_t get_id(void)
{
	// should return 0x55
	using namespace i2c;
	start(dev);
	send_addr(dev, BARO_ADDR, Mode::Write);
	write(dev, REG_ID);
	restart(dev);
	send_addr(dev, BARO_ADDR, Mode::Read);
	uint8_t id = read(dev);
	stop(dev);

	return id;
}
// ------------------------------------------------------------------------ }}}
// get temperature															{{{
// ----------------------------------------------------------------------------
uint16_t get_temperature(void)	// temperature / 10 = temperature in °C
{
	uint16_t UT = get_ut();

	int32_t X1 = (UT - AC6) * AC5 / pow2_15;
	int32_t X2 = MC * pow2_11 / (X1 + MD);
	int32_t B5 = X1 + X2;
	int32_t t = (B5 + 8) / pow2_4;
	dprintf("X1: %d\r\n", X1);
	dprintf("X2: %d\r\n", X2);
	dprintf("B5: %d\r\n", B5);
	dprintf("t: %d\r\n", t);
	dprintf("t: %d\r\n", t/10);

	return t;
}
// ------------------------------------------------------------------------ }}}
// get pressure																{{{
// ----------------------------------------------------------------------------
int32_t get_pressure(void)	// return pressure in Pa
{
	// Average sea-level pressure is 101.325 kPa (1013.25 hPa or mbar) or 29.92 inches (inHg) or 760 millimetres of mercury (mmHg).
	int32_t  X3;
	int32_t  B3;
	uint32_t B4;
	int32_t  B6;
	uint32_t B7;

	int32_t UP = get_up();
	int32_t p;

	uint32_t UT = get_ut();
	int32_t X1 = (UT - AC6) * AC5 / pow2_15;
	int32_t X2 = MC * pow2_11 / (X1 + MD);
	int32_t B5 = X1 + X2;

	B6 = B5 - 4000;
	dprintf("B6: %d\r\n", B6);
	X1 = (B2 * ((B6 * B6) >> 12)) >> 11;
	dprintf("X1: %d\r\n", X1);

	X2 = (AC2 * B6) >> 11;
	dprintf("X2: %d\r\n", X2);
	X3 = X1 + X2;
	dprintf("X3: %d\r\n", X3);
	B3 = (((AC1 * 4 + X3) << OSS) + 2) / 4;
	dprintf("B3: %d\r\n", B3);
	X1 = (AC3 * B6) >> 13;
	dprintf("X1: %d\r\n", X1);
	X2 = (B1 * (B6 * B6 >> 12)) >> 16;
	dprintf("X2: %d\r\n", X2);
	X3 = ((X1 + X2) + 2) / 4;
	dprintf("X3: %d\r\n", X3);
	B4 = (AC4 * (uint32_t)(X3 + 32768)) >> 15;
	dprintf("B4: %d\r\n", B4);
	B7 = ((uint32_t)(UP - B3) * (50000 >> OSS));
	dprintf("B7: %d\r\n", B7);

	if (B7 < 0x80000000)
	{
		p = (B7 * 2) / B4;
		dprintf("p1: %d\r\n", p);
	}
	else
	{
		p = (B7 / B4) * 2;
		dprintf("p2: %d\r\n", p);
	}

	X1 = (p >> 8) * (p >> 8);
	dprintf("X1: %d\r\n", X1);
	X1 = (X1 * 3038) >> 16;
	dprintf("X1: %d\r\n", X1);
	X2 = (-7357 * p) >> 16;
	dprintf("X2: %d\r\n", X2);
	p = p + ((X1 + X2 + 3791) >> 4);	// pressure in Pa

	return p;
}
// ------------------------------------------------------------------------ }}}

void example(i2c::Device arg_dev)
{
	init(arg_dev);
	printf("baro ID: 0x%x\r\n", get_id());
	printf("baro ID: 0x%x\r\n", get_id());

	uint16_t ut = get_ut();
	uint16_t up = get_up();
	int16_t temp = get_temperature() / 10;
	int16_t temp_dec = get_temperature() % 10;
	int32_t pressure = get_pressure();

	printf("BMP UT: %d temperature: %d.%d°C\r\n", ut, temp, temp_dec);
	printf("BMP UP: %d pressure: %d hPa\r\n", up, pressure/100);
}

#ifdef SHELL
// ----------------------------------------------------------------------------
uint8_t cmd(char *argv[])
{
	if (bmp_initialized == 0)
	{
		eprintf("BMP is not initalized!\r\n");
		return SHELL_RETURN_NOK;
	}
	// reset();
	uint16_t ut = get_ut();
	int16_t temp = get_temperature() / 10;
	int16_t temp_dec = get_temperature() % 10;
	uint16_t up = get_up();
	int32_t pressure = get_pressure();
	printf("BMP UT: %d t:%d.%d°C UP: %d pressure: %d hPa\r\n", ut, temp, temp_dec, up, pressure / 100);

	return SHELL_RETURN_OK;
}
#endif // SHELL

} // namespace
