// Copyright © 2020 by P.Orsolic. All right reserved
// Created 191231

#ifndef GPIO_EXTI_H
#define GPIO_EXTI_H

#include "config.h"
#include "gd32vf103.h"
#include "gd32vf103_rcu.h"
#include "gd32vf103_eclic.h"
#include "printf.h"
#include "gpio.hpp"
#include "gpio_hw.hpp"
#include "exti.hpp"

void exti_example_init(void);
void exti_example(void);

#endif	// GPIO_EXTI_H
