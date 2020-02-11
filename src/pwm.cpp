// created 150312 - STM32 C
// Created 200114 - GD32V C+

#include "pwm.hpp"
#include "gd32vf103_rcu.h"
#include "gd32vf103_gpio.h"
#include "gd32vf103_timer.h"

namespace pwm
{

#define FB_PWM	GPIO_PIN_1
#define FB_GND	GPIO_PIN_7

#define LR1	GPIO_PIN_5
#define LR2	GPIO_PIN_6

void gpio_config(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_AF);

	/*Configure PA1 PA2 PA3(TIMER1 CH1 CH2 CH3) as alternate function*/
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LR1);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LR2);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
	gpio_bit_reset(GPIOA, GPIO_PIN_7);
	gpio_bit_reset(GPIOA, LR1);
	gpio_bit_reset(GPIOA, LR2);
}

void timer_config(void)
{
	/* -----------------------------------------------------------------------
	   TIMER1 configuration: generate 3 PWM signals with 3 different duty cycles:
	   TIMER1CLK = SystemCoreClock / 108 = 1MHz

	   TIMER1 channel1 duty cycle = (4000/ 16000)* 100  = 25%
	   TIMER1 channel2 duty cycle = (8000/ 16000)* 100  = 50%
	   TIMER1 channel3 duty cycle = (12000/ 16000)* 100 = 75%
	   ----------------------------------------------------------------------- */
	timer_oc_parameter_struct timer_ocinitpara;
	timer_parameter_struct timer_initpara;

	rcu_periph_clock_enable(RCU_TIMER1);

	timer_deinit(TIMER1);
	/* initialize TIMER init parameter struct */
	timer_struct_para_init(&timer_initpara);
	/* TIMER1 configuration */
	// timer_initpara.prescaler         = 107;
	timer_initpara.prescaler         = 431;
	timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
	timer_initpara.counterdirection  = TIMER_COUNTER_UP;
	timer_initpara.period            = 15999;
	timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
	timer_initpara.repetitioncounter = 0;
	timer_init(TIMER1, &timer_initpara);

	/* initialize TIMER channel output parameter struct */
	timer_channel_output_struct_para_init(&timer_ocinitpara);
	/* CH0, CH1 and CH2 configuration in PWM mode */
	timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
	timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
	timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
	timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
	timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
	timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

	timer_channel_output_config(TIMER1,TIMER_CH_1,&timer_ocinitpara);
	timer_channel_output_config(TIMER1,TIMER_CH_2,&timer_ocinitpara);
	timer_channel_output_config(TIMER1,TIMER_CH_3,&timer_ocinitpara);

	/* CH1 configuration in PWM mode1,duty cycle 25% */
	// timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,3999);
	timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,0);
	timer_channel_output_mode_config(TIMER1,TIMER_CH_1,TIMER_OC_MODE_PWM0);
	timer_channel_output_shadow_config(TIMER1,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);

	/* CH2 configuration in PWM mode1,duty cycle 50% */
	timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_2,7999);
	timer_channel_output_mode_config(TIMER1,TIMER_CH_2,TIMER_OC_MODE_PWM0);
	timer_channel_output_shadow_config(TIMER1,TIMER_CH_2,TIMER_OC_SHADOW_DISABLE);

	/* CH3 configuration in PWM mode1,duty cycle 75% */
	timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_3,11999);
	timer_channel_output_mode_config(TIMER1,TIMER_CH_3,TIMER_OC_MODE_PWM0);
	timer_channel_output_shadow_config(TIMER1,TIMER_CH_3,TIMER_OC_SHADOW_DISABLE);

	/* auto-reload preload enable */
	timer_auto_reload_shadow_enable(TIMER1);
	/* auto-reload preload enable */
	timer_enable(TIMER1);
}

void pwm_set_freq(uint32_t freq)
{
	timer_parameter_struct timer_initpara;

	uint8_t cpu_freq = 108;

	// timer_initpara.prescaler         = 107;	(108 MHz / 107+1 = 1 MHz)
	timer_initpara.prescaler         = 431;
	timer_init(TIMER1, &timer_initpara);
}

void pwm_set_duty(uint8_t n)
{
	// if (n > 100)
	// {
	// 	eprintf("duty cycle can't be more than 100%\r\n");
	// }

	uint32_t duty = n * 160;
	if (duty == 0)
	{
		duty = 1;
	}

	if (n == 'z')
	{
		printf("left\r\n");
		gpio_bit_set(GPIOA, LR1);
		gpio_bit_reset(GPIOA, LR2);
		return;
	}
	else if (n == 'c')
	{
		printf("right\r\n");
		gpio_bit_reset(GPIOA, LR1);
		gpio_bit_set(GPIOA, LR2);
		return;
	}
	else if (n == 'x')
	{
		printf("no left or right\r\n");
		gpio_bit_reset(GPIOA, LR1);
		gpio_bit_reset(GPIOA, LR2);
		return;
	}

	printf("setting duty cycle to n = %d\r\n", n);

	/* CH1 configuration in PWM mode1,duty cycle 25% */
	// timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,3999);
	timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1, duty - 1);
	timer_channel_output_mode_config(TIMER1,TIMER_CH_1,TIMER_OC_MODE_PWM0);
	timer_channel_output_shadow_config(TIMER1,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);
}


void example(void)
{
	printf("PWM start\r\n");
	gpio_config();
	timer_config();
	printf("PWM end\r\n");

}

#ifdef SHELL
uint8_t cmd(char *argv[])
{
	// pwm [d,f] <uint_32>
	// pwm d 10		set duty to 10%
#ifdef MCU_RUN
	uint8_t argc = get_argc(argv);
	if (argc < 2)
	{
		eprintf("Not enough arguments: %d\r\n", argc);
		printf("--> Usage: %s d <duty_cycle>\r\n", argv[0]);
		printf("--> Usage: %s f <freq>\r\n", argv[0]);
		return 0;
	}

	const char* name = argv[0];
	const char action = argv[1][0];
	char* string_part_read;
	uint32_t duty_or_freq = strtol(argv[2], &string_part_read, 10);

	switch (action)
	{
		case 'd':
		case 'D':
			if (strlen(string_part_read) == 0)	// conversion was successful
			{
				pwm_set_duty(duty_or_freq);
			}
			break;
		default:
			eprintf("unknown action: %c\r\n", action);
			return 0;
	}


#else
#endif

	return 1;
}
#endif // SHELL

} // namespace
