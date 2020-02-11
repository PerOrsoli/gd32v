// Copyright © 2020 by P.Orsolic. All right reserved
// created 140916 - STM32 C version
// created 200104 - GDM32V C version

#ifndef BARO_H
#define BARO_H

#include "i2c.hpp"

namespace baro
{
void init(i2c::Device arg_dev);
void reset(void);
uint8_t get_id(void);
uint16_t get_temperature(void);	// temperature / 10 = temperature in °C
int32_t get_pressure(void);		// returns pressure in Pa

void example(i2c::Device arg_dev);
uint8_t cmd(char *argv[]);

} // namespace

#endif // BARO_H
