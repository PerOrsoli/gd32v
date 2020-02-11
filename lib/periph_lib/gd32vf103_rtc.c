// Copyright (c) 2019, GigaDevice Semiconductor Inc.

#include "gd32vf103_rtc.h"

#define RTC_HIGH_BITS_MASK			((uint32_t)0x000F0000U) // RTC high bits mask
#define RTC_LOW_BITS_MASK			((uint32_t)0x0000FFFFU) // RTC low bits mask

/* RTC register high bits offset */
#define RTC_HIGH_BITS_OFFSET		((uint32_t)16U)

void rtc_configuration_mode_enter(void)
{
	RTC_CTL |= RTC_CTL_CMF;
}

void rtc_configuration_mode_exit(void)
{
	RTC_CTL &= ~RTC_CTL_CMF;
}

void rtc_counter_set(uint32_t cnt)
{
	rtc_configuration_mode_enter();
	/* set the RTC counter high bits */
	RTC_CNTH = (cnt >> RTC_HIGH_BITS_OFFSET);
	/* set the RTC counter low bits */
	RTC_CNTL = (cnt & RTC_LOW_BITS_MASK);
	rtc_configuration_mode_exit();
}

void rtc_prescaler_set(uint32_t psc)
{
	rtc_configuration_mode_enter();
	/* set the RTC prescaler high bits */
	RTC_PSCH = ((psc & RTC_HIGH_BITS_MASK) >> RTC_HIGH_BITS_OFFSET);
	/* set the RTC prescaler low bits */
	RTC_PSCL = (psc & RTC_LOW_BITS_MASK);
	rtc_configuration_mode_exit();
}

void rtc_lwoff_wait(void)
{
	/* loop until LWOFF flag is set */
	while ((RTC_CTL & RTC_CTL_LWOFF) == RESET)
	{
	}
}

void rtc_register_sync_wait(void)
{
	/* clear RSYNF flag */
	RTC_CTL &= ~RTC_CTL_RSYNF;
	/* loop until RSYNF flag is set */
	while ((RTC_CTL & RTC_CTL_RSYNF) == RESET)
	{
	}
}

// set RTC alarm value
void rtc_alarm_config(uint32_t alarm)
{
	rtc_configuration_mode_enter();
	/* set the alarm high bits */
	RTC_ALRMH = (alarm >> RTC_HIGH_BITS_OFFSET);
	/* set the alarm low bits */
	RTC_ALRML = (alarm & RTC_LOW_BITS_MASK);
	rtc_configuration_mode_exit();
}

uint32_t rtc_counter_get(void)
{
	uint32_t temp = 0x0U;

	temp = RTC_CNTL;
	temp |= (RTC_CNTH << RTC_HIGH_BITS_OFFSET);
	return temp;
}

// get RTC divider value
uint32_t rtc_divider_get(void)
{
	uint32_t temp = 0x00U;

	temp = ((RTC_DIVH & RTC_DIVH_DIV) << RTC_HIGH_BITS_OFFSET);
	temp |= RTC_DIVL;
	return temp;
}

// get RTC flag status
FlagStatus rtc_flag_get(uint32_t flag)
{
	if ((RTC_CTL & flag) != RESET)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/*!
  \brief      clear RTC flag status
  \param[in]  flag: specify which flag status to clear
  one or more parameters can be selected which are shown as below:
  \arg        RTC_FLAG_SECOND: second interrupt flag
  \arg        RTC_FLAG_ALARM: alarm interrupt flag
  \arg        RTC_FLAG_OVERFLOW: overflow interrupt flag
  \arg        RTC_FLAG_RSYN: registers synchronized flag
  \param[out] none
  \retval     none
  */
void rtc_flag_clear(uint32_t flag)
{
	/* clear RTC flag */
	RTC_CTL &= ~flag;
}

FlagStatus rtc_interrupt_flag_get(uint32_t flag)
{
	if ((RTC_CTL & flag) != RESET)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

void rtc_interrupt_flag_clear(uint32_t flag)
{
	/* clear RTC interrupt flag */
	RTC_CTL &= ~flag;
}

void rtc_interrupt_enable(uint32_t interrupt)
{
	RTC_INTEN |= interrupt;
}

void rtc_interrupt_disable(uint32_t interrupt)
{
	RTC_INTEN &= ~interrupt;
}
