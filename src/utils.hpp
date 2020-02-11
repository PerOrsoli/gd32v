// Copyright © 2020 by P.Orsolic. All right reserved
// Created 200126
// inital use was for misc shell functions

#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint8_t get_argc(char *argv[]);
bool is_hex_string(const char* input);
bool is_dec_string(const char* input);
// uint32_t str2num(const char* input);
bool str2num(const char* input, uint32_t* num);
uint8_t cmd_utils(char *argv[]);

#define COUNT_OF(x)		sizeof(x)/sizeof(x[0])

#ifdef __cplusplus
}
#endif	// __cplusplus
#endif // UTILS_H
