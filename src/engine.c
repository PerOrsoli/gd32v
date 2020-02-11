// Copyright © 2020 by P.Orsolic. All right reserved
// Created 200103 by accident :)
#include "engine.h"

typedef struct
{
	GpioPin		pin;
	uint8_t		stroke;
} Cylinder;

// .stroke = (firing order - 4) - not all cynders start at the same position
Cylinder l1 = {L1, 1};
Cylinder l2 = {L2, 4};
Cylinder l3 = {L3, 2};
Cylinder l4 = {L4, 3};

Cylinder r1 = {R1, 4};
Cylinder r2 = {R2, 3};
Cylinder r3 = {R3, 1};
Cylinder r4 = {R4, 2};

static void engine_one_cylinder(Cylinder *c);
static void engine_all_cylinders(uint32_t delay);
static void engine_startup(uint8_t delay2, uint8_t delay1);
static void engine_stop(void);
static void engine(char ch);
static uint8_t delay;

void engine_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	gpio_init2(L1, OutPP, Speed10MHz);
	gpio_init2(L2, OutPP, Speed10MHz);
	gpio_init2(L3, OutPP, Speed10MHz);
	gpio_init2(L4, OutPP, Speed10MHz);
	gpio_init2(R1, OutPP, Speed10MHz);
	gpio_init2(R2, OutPP, Speed10MHz);
	gpio_init2(R3, OutPP, Speed10MHz);
	gpio_init2(R4, OutPP, Speed10MHz);

	engine_stop();
	engine_startup(24, 8);
}

static void engine_one_cylinder(Cylinder *c)
{
	if (c->stroke == 3)	// power stroke
	{
		gpio_reset(c->pin);
		c->stroke++;
	}
	else
	{
		// inverse logic
		gpio_set(c->pin);
		c->stroke++;
	}
	if (c->stroke > 4)
	{
		// printf("not toggle pin: %d n:%d\r\n", c->pin, c->stroke);
		c->stroke = 0;
	}
}

static void engine_all_cylinders(uint32_t delay)
{
	engine_one_cylinder(&l1);
	delay_ms(delay);
	engine_one_cylinder(&r4);
	delay_ms(delay);
	engine_one_cylinder(&r2);
	delay_ms(delay);
	engine_one_cylinder(&l2);
	delay_ms(delay);
	engine_one_cylinder(&r3);
	delay_ms(delay);
	engine_one_cylinder(&l3);
	delay_ms(delay);
	engine_one_cylinder(&l4);
	delay_ms(delay);
	engine_one_cylinder(&r1);
	delay_ms(delay);
}

static void engine_startup(uint8_t delay2, uint8_t delay1)
{
	do
	{
		engine_all_cylinders(delay2--);
		delay_ms(delay2*2);	// extra delay

	} while (delay2 > delay1);
	delay = delay2;
}

static void engine_stop(void)
{
	gpio_set(L1);
	gpio_set(L2);
	gpio_set(L3);
	gpio_set(L4);
	gpio_set(R1);
	gpio_set(R2);
	gpio_set(R3);
	gpio_set(R4);
}

static void engine(char ch)
{
	if (ch == '+')
	{
		delay += 1;
	}
	if ((ch == '-') && (delay > 0))
	{
		delay -= 1;
	}

	engine_all_cylinders(delay);
}

// to be put in UART ISR for control of "rpm"s
void engine_isr(char ch)
{
	if (ch == 1) // a
	{
		printf("engine + delay: %d\r", delay);
		engine('+');
	}
	if (ch == 3) // s
	{
		printf("engine - delay: %d\r", delay);
		engine('-');
	}
	if (ch == 2) // r
	{
		printf("engine restart:\r\n");

		engine_stop();
		delay_ms(500);
		engine_startup(24, 8);
	}
}

// to be put inside while(1)
void engine_run(char ch)
{
	engine(ch);
}
