// Copyright © 2020 by P.Orsolic. All right reserved
// Created 200130
#ifndef DATE_H
#define DATE_H

#include <stdint.h>
#include "debug.h"

namespace date
{
uint8_t get_sec(void);
uint8_t get_min(void);
uint8_t get_hour(void);
void print();

uint8_t cmd(char *argv[]);

} // namespace

#endif // DATE_H
