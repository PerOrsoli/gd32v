#include "delay.h"
#include "gd32vf103.h"
#include "n200_func.h"

void delay_ms(uint32_t count)
{
	uint64_t start_mtime, delta_mtime;

	/* Don't start measuruing until we see an mtime tick */
	uint64_t tmp = get_timer_value();
	do {
		start_mtime = get_timer_value();
	} while (start_mtime == tmp);

	do {
		delta_mtime = get_timer_value() - start_mtime;
	} while (delta_mtime < (SystemCoreClock/4000 * count));
}
