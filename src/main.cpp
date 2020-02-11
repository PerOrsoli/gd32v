// Copyright © 2020 by P.Orsolic. All right reserved
#include "config.h"
#include "delay.h"
#include "utils.hpp"
#include "lib/printf/printf.h"
#include "gd32vf103.h"
#include "gd32vf103_rcu.h"
#include "gd32vf103_eclic.h"
#include "src/gpio.hpp"
#include "src/test_gpio.hpp"
#include "src/gpio-exti.hpp"
#include "src/uart.hpp"
#include "src/sys.h"
#include "src/exti.hpp"
// #include "eeprom.hpp"
#include "baro.hpp"
#include "wii-nunchuck.hpp"
#include "pwm.hpp"
// #include "shell.hpp"
#include "rtc.hpp"
#include "date.hpp"

extern "C" void _init(void);
#define DELAY 500

extern "C" {	// don't mangle main() it is called from startup code
void main(void)
{
	_init();

	uart::init2(uart::Uart::Uart0, uart::Speed::speed460800, uart::Mode::EightNoneOne);
	uart::clear();

	rcu_periph_clock_enable(LEDR_CLK);
	rcu_periph_clock_enable(LEDB_CLK);

	gpio::gpio_init2(LEDG, OutPP, Speed10MHz);
	gpio::gpio_init2(LEDB, OutPP, Speed10MHz);
	gpio::gpio_init2(LEDR, OutPP, Speed10MHz);

	// inverse logic - set all bits to high
	gpio_set(LEDR);
	gpio_set(LEDG);
	gpio_set(LEDB);

	eclic_global_interrupt_enable();
	eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);
	eclic_irq_enable(USART0_IRQn, 1, 0);

	printf("Here RISC-V MCU\r\n");

	sysinfo_print();
	// exti_example_init();

	// i2c::test();
	baro::init(i2c::Device::myI2C0);
	// eeprom::example(i2c::Device::myI2C0);
	baro::example(i2c::Device::myI2C0);
	// wii_nunchuck::example();
	// pwm::example();
	// rtc::test();
	rtc::example();

	// const uint32_t* DBG_ID = (uint32_t *)0xE0042000;
	// printf("DBG_ID: 0x%x\r\n", *DBG_ID);
	printf("date: ");
	date::print();

	printf("sad ide while\r\n");
	while(1)
	{
		gpio_toggle(LEDB);
		delay_ms(DELAY);
		gpio_toggle(LEDB);
		delay_ms(DELAY);

		using namespace baro;
		uint16_t temp = get_temperature() / 10;
		uint16_t temp_dec = get_temperature() % 10;
		int32_t pressure = get_pressure();
		printf("Temp: %d.%d°C pressure: %d\r\n", temp, temp_dec, pressure);

		// bool key = gpio_get(KEY);
		// if (key)
		// {
		// 	printf("key changed to state: %d\r\n", key);
		// }
	}
}
} // extern C
