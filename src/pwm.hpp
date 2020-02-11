// created 150312 - STM32 C
// Created 200114 - GD32V C+

#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include "debug.h"

namespace pwm
{

void example(void);
void pwm_set_duty(uint8_t n);

uint8_t cmd(char *argv[]);

} // namespace
#endif // PWM_H
