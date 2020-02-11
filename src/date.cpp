// Copyright © 2020 by P.Orsolic. All right reserved
// Created 200130
#define DEBUG_RTC
#ifndef DEBUG_RTC
#undef DEBUG
#endif // DEBUG_RTC

#include "rtc.hpp"
#ifdef SHELL
#include "shell-cmd.hpp"
#endif // SHELL
#include "utils.hpp"

namespace date
{
using namespace rtc;

#define EPOCH		1970
#define EPOCH_DAY	4	// 1.1.1970 was Thursday

const char* days_in_week[] = {"Wrong", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

uint32_t days_since_epoch(void)
{
	return (rtc::get_counter() / 60 / 60 / 24);
}

bool is_leap_year(uint32_t year)
{
	// Wikipedia algorithm:
	// if (year is not divisible by 4) then (it is a common year)
	// else if (year is not divisible by 100) then (it is a leap year)
	// else if (year is not divisible by 400) then (it is a common year)
	// else (it is a leap year)
	if (year % 4 != 0)
	{
		return 0;
	}
	else if (year % 100 != 0)
	{
		return 1;
	}
	else if (year % 400 != 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

static const char* get_day_in_week(void)
{
	uint8_t n = ((EPOCH_DAY + days_since_epoch()) % 7);
	return days_in_week[n];
}

static uint8_t get_days_in_month(uint8_t month, uint32_t year)
{
	switch (month)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			return 31;
		case 4:
		case 6:
		case 9:
		case 11:
			return 30;
		case 2:
			if (is_leap_year(year) == 1)
			{
				return 29;
			}
			else
			{
				return 28;
			}
		default:
			eprintf("Wrong month: %d\r\n", month);
			panic();
	}
	panic();
	return 13;	// make compiler happy
}


void convert_to_string()
{
	uint32_t days = days_since_epoch();
	uint32_t current_year = EPOCH;
	uint8_t  current_month = 1;
	uint8_t  current_day = 1;

	/* printf("days: %d\n", days); */
	// namjerno nije razdvojeno u posebne funkcije, ovako je jednostavnije jer nema pointera izmedju raznih funkcijica

	// calculate year:
	while (days > 365)
	{
		if (is_leap_year(current_year) == 1)
		{
			days -= 366;
		}
		else
		{
			days -= 365;
		}
		current_year++;
	}

	// calculate month
	while (days > 31)
	{
		switch (current_month)
		{
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				days -= 31;
				current_month++;
				break;
			case 4:
			case 6:
			case 9:
			case 11:
				days -= 30;
				current_month++;
				break;
			case 2:
				if (is_leap_year(current_year) == 1)
				{
					days -= 29;
				}
				else
				{
					days -= 28;
				}
				current_month++;
				break;
			default:
				printf("error current_month: %d\r\n", current_month);
				return;
		}
	}

	// calculate day
	current_day += days;
	days = 0;

	printf("%s, %d.%d.%d\r\n", get_day_in_week(), current_year, current_month, current_day);
	// snprintf("%s, %d.%d.%d\r\n", get_day_in_week(), current_year, current_month, current_day);
}

void print(void)
{
	// printf("%s, %d.%d.%d\r\n", get_day_in_week(), current_year, current_month, current_day);
}

uint8_t get_sec(void)
{
	uint32_t seconds = get_counter();
	uint8_t sec  = seconds % 60;
	return sec;
}

uint8_t get_min(void)
{
	uint32_t seconds = get_counter();
	uint8_t min  = (seconds / 60) % 60;
	return min;
}

uint8_t get_hour(void)
{
	uint32_t seconds = get_counter();
	uint8_t hour = (seconds / 60 / 60) % 24;

	return hour;
}

#ifdef SHELL
uint8_t cmd(char *argv[])
{
	uint8_t argc = get_argc(argv);

	uint32_t seconds = get_counter();

	uint16_t day = (seconds / 60 / 60 / 24) % 365;
	uint8_t hour = (seconds / 60 / 60) % 24;
	uint8_t min  = (seconds / 60) % 60;
	uint8_t sec  = seconds % 60;

	if (argc == 0)
	{
		printf("time: day: %d %02d:%02d:%02d\r\n", day, hour, min, sec);
		printf("----------\r\n");
		print();
	}
	else
	{
		const char* new_date = argv[1];
		uint8_t length = strlen(new_date);
		uint32_t nhour = 0;
		uint32_t nmin  = 0;
		uint32_t nsec  = 0;

		// set time
		dprintf("argv[1]: %s length: %d\r\n", argv[1], length);

		if (length >= 4)
		{
			const char shour[3] = {new_date[0], new_date[1], 0};
			const char smin[3]  = {new_date[2], new_date[3], 0};
			// dprintf("strings: H:%s M:%s\r\n", shour, smin);

			if (str2num(shour, &nhour) != 0)
			{
				eprintf("error converting shour -> nhour\r\n");
				return 0;
			}

			if (str2num(smin, &nmin) != 0)
			{
				eprintf("error converting smin -> nmin\r\n");
				return 0;
			}

			dprintf("H:%d M:%d\r\n", nhour, nmin);

		}
		if (length >= 6)
		{
			const char ssec[2]  = {new_date[4], new_date[5]};
			// dprintf("strings sec: %s\r\n", ssec);

			if (str2num(ssec, &nsec) != 0)
			{
				eprintf("error converting ssec -> nsec\r\n");
				return 0;
			}

			dprintf("sec: %d\r\n", nsec);
		}
		uint32_t new_seconds = 0;

		rtc::write(new_seconds);
	}

	return SHELL_RETURN_OK;
}
#endif // SHELL

} // namespace
