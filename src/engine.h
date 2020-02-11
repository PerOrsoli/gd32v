// Copyright © 2020 by P.Orsolic. All right reserved
// Created 200103 by accident :)
#ifndef ENGINE_H
#define ENGINE_H
#ifdef __cplusplus
extern "C" {
#endif

#include "gd32vf103_rcu.h"	// for clocks, for now
#include "delay.h"
#include "gpio.h"
#include "printf.h"

// "digital V8"
// left block:
#define L1	PA4
#define L2	PA5
#define L3	PA6
#define L4	PA7
// right block:
#define R1	PA0
#define R2	PA1
#define R3	PA2
#define R4	PA3

void engine_init(void);
void engine_isr(char ch);
void engine_run(char ch);

#ifdef __cplusplus
}
#endif	// __cplusplus
#endif	// ENGINE_H
