// Copyright © 2020 by P.Orsolic. All right reserved
// Created 191231

#include "gpio-exti.hpp"

using namespace exti;

void exti_example_init(void)
{
	rcu_periph_clock_enable(RCU_AF);
	rcu_periph_clock_enable(RCU_GPIOA);
	gpio::gpio_init2(KEY, InPD, Speed2MHz);

	eclic_global_interrupt_enable();
	eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);
	eclic_irq_enable(EXTI5_9_IRQn, 1, 0);

	gpio_select_exti(KEY);

	exti::init(Source::Pin8, exti::Mode::Interrupt, Edge::Falling);
	exti::interrupt_flag_clear(Source::Pin8);
}

extern "C"	// don't mangle
{
void EXTI5_9_IRQHandler(void)
{
	dprintf("EXTI ISR\r\n");
	if (exti::interrupt_flag_get(Source::Pin8) != false)
	{
		exti::interrupt_flag_clear(Source::Pin8);

		gpio_toggle(LEDG);
	}
}
}	// extern "C"	// don't mangle
