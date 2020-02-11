// 140918 - C STM32
// 200111 - C+ GD32V

#include "wii-nunchuck.hpp"
#include "debug.h"
#include "libc-bits.h"	// abs()

#define Timed(x) Timeout = 0xFFFF; while (x) { if (Timeout-- == 0) printf("timeout: line: %d\r\n", __LINE__);}

namespace wii_nunchuck
{
	using namespace i2c;
#define DEV		i2c::Device::myI2C0
	static void write(Device dev, const uint8_t* data, uint32_t nbyte, uint8_t address);

typedef struct
{
	uint8_t		joystick_x;
	uint8_t		joystick_y;
	uint16_t	accel_x: 10;
	uint16_t	accel_y: 10;
	uint16_t	accel_z: 10;
	uint8_t		buttonZ: 1;
	uint8_t		buttonC: 1;
} wii_data_t;

static uint8_t id[6] = {};
static uint8_t data[6] = {};
static uint8_t calibration[16];
static wii_data_t wii_data;
static wii_data_t old_wii_data;

static const uint8_t reg_id = 0xFA;
static const uint8_t reg_calib = 0x20;

void init(void)
{
	// GPIO init
	using namespace gpio;
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_AF);
	rcu_periph_clock_enable(RCU_I2C0);
	gpio_init2(PB6, AfioOD, Speed50MHz);	// SCL
	gpio_init2(PB7, AfioOD, Speed50MHz);	// SDA

	init(i2c::Device::myI2C0, i2c::Speed::Speed400kHz, i2c::DutyCycle::Duty2);

	const uint8_t buf_init [] = {0xf0 , 0x55 };
	const uint8_t buf2_init [] = {0xfb , 0x00 };
	write(DEV, buf_init, 2, ADDR );
	write(DEV, buf2_init, 2, ADDR );
}

using namespace i2c;

static void write(Device dev, const uint8_t* data, uint32_t nbyte, uint8_t address)
{
	__IO uint32_t Timeout = 0;

	if (nbyte)
	{
		wait_until_flag(dev, Flag::BSY, 1);

		start(dev);
		Timed(!check_event(Event::MasterModeSelected));

		send_addr(dev, address, Mode::Write);
		i2c::write(dev, *data++);

		while (--nbyte)
		{
			wait_until_flag(dev, Flag::BTC, 0);
			i2c::write(dev, *data++);
		}

		wait_until_flag(dev, Flag::BTC, 0);

		stop(dev);
		wait_until_flag(dev, Flag::STPDET, 1);
	}
}

bool read(Device dev, uint8_t *data, uint32_t n, uint8_t address)
{
	__IO uint32_t Timeout = 0;

	if (!n)
		return 1;

	wait_until_flag(dev, Flag::BSY, 1);
	ack(dev);
	nack_position(dev, NackPosition::Current);

	start(dev);
	Timed(!check_event(Event::MasterModeSelected));

	send_addr(dev, address, Mode::Read);

	if (n == 1)
	{
		nack(dev);

		clear_flag(dev, Flag::ADDSEND);
		stop(dev);

		wait_until_flag(dev, Flag::RBNE, 0);
		*data++ = read(dev);
	}
	else if (n == 2)
	{
		nack_position(dev, NackPosition::Next);
		clear_flag(dev, Flag::ADDSEND);

		nack(dev);
		wait_until_flag(dev, Flag::BTC, 0);

		stop(dev);
		*data++ = read(dev);
		*data++ = read(dev);
	}
	else
	{
		clear_flag(dev, Flag::ADDSEND);
		while (n-- != 3)
		{
			wait_until_flag(dev, Flag::BTC, 0);
			*data++ = read(dev);
		}

		wait_until_flag(dev, Flag::BTC, 0);

		nack(dev);
		*data++ = read(dev);	// receive byte N-2
		stop(dev);

		*data++ = read(dev);	// receive byte N-1

		Timed(!check_event(Event::MasterByteReceived));
		*data++ = read(dev);

		n = 0;
	}

	wait_until_flag(dev, Flag::STPDET, 1);

	return 1;
}

void read_id(void)
{
	// ID for nunchuck should be:
	// 0x00, 0x00, 0xa4, 0x20, 0x00, 0x00
	uint8_t bufread [] = {0};
	bufread[0] = reg_id;
	write(DEV, bufread, 1, ADDR);
	read(DEV, id, 6, ADDR);

	printf("ID: ");
	for (uint8_t i = 0; i <6 ; i++)
	{
		printf("[%d]0x%x ", i, id[i]);
	}
	printf("\r\n");
}

uint8_t *get_calibration(void)
{
	write(DEV, &reg_calib, 1, ADDR);
	read(DEV, calibration, 16, ADDR);

	for (uint8_t i=0; i<16; i++)
	{
		printf("calib[%d] = %d\r\n", i, calibration[i]);
	}
	return calibration;
}

wii_data_t* raw_to_struct(uint8_t raw[])
{
	// [0] joystick X
	// [1] joystick Y
	// [2] accelerometer X [9:2]
	// [3] accelerometer Y [9:2]
	// [4] accelerometer Z [9:2]

	// [5] b7b6: accelerometer X [1:0]
	// [5] b5b4: accelerometer Y [1:0]
	// [5] b3b2: accelerometer Z [1:0]
	// [5] b1: button C
	// [5] b0: button Z
	wii_data.joystick_x = data[0];
	wii_data.joystick_y = data[1];
	wii_data.accel_x    = data[2] << 2 | (data[5] & 0b11000000);	// b7 b6
	wii_data.accel_y    = data[3] << 2 | (data[5] & 0b00110000);	// b5 b4
	wii_data.accel_z    = data[4] << 2 | (data[5] & 0b00001100);	// b3 b2
	wii_data.buttonZ    = ~((data[5] & (1 << 0)) >> 0);	// b1
	wii_data.buttonC    = ~((data[5] & (1 << 1)) >> 1);	// b0

	return &wii_data;
}

void print_accel(wii_data_t* p, wii_data_t* o)
{
	// print only changes when nothing is pressed:
#define CHANGE 50
	if ((o->accel_x - p->accel_x) >= CHANGE)
	{
		rprintf("X- %d -> %d\r\n", o->accel_x, p->accel_x);
	}
	else if ((p->accel_x - o->accel_x) >= CHANGE)
	{
		gprintf("X+ %d -> %d\r\n", o->accel_x, p->accel_x);
	}
	if ((o->accel_y - p->accel_y) >= CHANGE)
	{
		yprintf("Y- %d -> %d\r\n", o->accel_y, p->accel_y);
	}
	else if ((p->accel_y - o->accel_y) >= CHANGE)
	{
		bprintf("Y+ %d -> %d\r\n", o->accel_y, p->accel_y);
	}
	if ((o->accel_z - p->accel_z) >= CHANGE)
	{
		mprintf("Z- %d -> %d\r\n", o->accel_z, p->accel_z);
	}
	else if ((p->accel_z - o->accel_z) >= CHANGE)
	{
		yprintf("Z+ %d -> %d\r\n", o->accel_z, p->accel_z);
	}
}

void print_joystick(wii_data_t* p)
{
	printf("Joystick: %d %d\r\n", p->joystick_x, p->joystick_y);
}

void wiiread(void)
{
	uint8_t bufread [] = {0};

	while(1)
	{
		write(DEV, bufread, 1, ADDR);
		read(DEV, data, 6, ADDR);
		// read(data, 6, ADDR);
		wii_data_t* p = raw_to_struct(data);
		wii_data_t* o = &old_wii_data;

		static bool printed  = 0;
		static bool printedZ = 0;
		static bool printedC = 0;
		static bool printed_raw = 0;

		print_accel(p, o);
		print_joystick(p);

		old_wii_data.accel_x = wii_data.accel_x;
		old_wii_data.accel_y = wii_data.accel_y;
		old_wii_data.accel_z = wii_data.accel_z;

		if (printed == 0)
		{
			printed = 1;
			printf("print button C to print raw data\r\n");
		}

		if ((p->buttonZ == 1) && (printedZ == 0))
		{
			printedZ = 1;
			printf("Release button Z for some action\r\n");
		}
		else if ((p->buttonZ == 1) && (printedZ == 1))
		{
			// do nothing
		}
		else if ((p->buttonC == 1) && (printedC == 0))
		{
			printedC = 1;

			if (printed_raw == 0)
			{
				printed_raw = 1;
				printf("Accel X: %4d Y: %4d Z: %4d\t", p->accel_x, p->accel_y, p->accel_z);
				printf("Joystick X: %3d Y: %3d\t", p->joystick_x, p->joystick_y);
				printf("buttons: C: %d Z: %d\r\n", p->buttonC, p->buttonZ);
			}
		}
		else if (p->buttonC == 0)
		{
			// buttonC released after raw stats are printed, reset that flag:
			printed_raw = 0;
		}
		else
		{
			printedZ = 0;
			printedC = 0;
		}
	}
}

void example(void)
{
	init();

	// (void)get_calibration();
	read_id();

	wiiread();
}

} // namespace
