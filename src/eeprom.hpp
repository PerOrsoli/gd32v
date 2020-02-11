// Copyright © 2020 by P.Orsolic. All right reserved
// created 141205 - STM32F1 C
// Created 200104 - GD32V C+
// Atmel 24C256 I2C EEPROM driver

#ifndef EEPROM_H
#define EEPROM_H

#include "i2c.hpp"

namespace eeprom
{
#define EEPROM_ADDR	0b10100000

void init(i2c::Device dev);
uint8_t read(uint16_t addr);
void read_many(uint16_t addr, uint8_t* data, uint8_t n);
void write(uint16_t addr, uint8_t data);
void write_many(uint16_t addr, const uint8_t data[], uint16_t n);
void erase(uint16_t addr);
void erase_many(uint16_t addr, uint16_t n);

void test(void);
void example(i2c::Device dev);
uint8_t cmd(char *argv[]);

} // namespace

#endif	// EEPROM_H
