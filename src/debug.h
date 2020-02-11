// Copyright © 2020 by P.Orsolic. All right reserved
// Created 190311

#ifndef DEBUG_H
#define DEBUG_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "printf.h"
#include "colors_ansi.h"
#include "delay.h"

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#if defined(__amd64__) || defined(__amd64) || defined(__x86_64) \
	|| defined(__i386) || defined(__i386__)
#define PC_RUN
#endif
#if defined(__arm) || defined(__arm__) || defined(__thumb__) \
	|| defined(__riscv)
#define MCU_RUN
#endif
#if !defined PC_RUN && !defined MCU_RUN
#error Nor PC_RUN nor MCU_RUN defined!
#endif

void assert_early(uint32_t a, uint32_t b);
void assert_eq(uint32_t a, uint32_t b);
void assert_eq2(uint32_t a, uint32_t b, const char *file, const uint16_t line);
void assert(uint32_t a, const char *file, const uint16_t line);
void panic(void);
#define ASSERT_EQ(a, b)		assert_eq2(a, b, __FILE__, __LINE__);
#define ASSERT(a)			assert(a, __FILE__, __LINE__);

#if defined(__arm) || defined(__arm__) || defined(__thumb__)
#define _bkpt()		__asm__("BKPT")
#elif defined(__riscv)
#define _bkpt()		__asm__("ebreak")
#endif

#define UNUSED(x)		(void)(x)
#define UNUSED_ARG(x)	(void)(x)

#ifdef DEBUG
#define dprintf(fmt, ...) \
do { if (DEBUG) printf(ANSI_COLOR_YELLOW "DBG INFO %s:%d %s(): " \
		ANSI_COLOR_RESET fmt, __FILE__, __LINE__, __func__,\
		##__VA_ARGS__); } while (0)
#else
#define dprintf(...)
#endif

#define MAX_VALUE_OF(type)	((1 << (sizeof(type))*8)-1)	// u8 -> 255

// error printf
#define eprintf(fmt, ...) \
printf(ANSI_COLOR_RED "ERROR %s:%d %s(): " \
		ANSI_COLOR_RESET fmt, __FILE__, __LINE__, __func__,\
		##__VA_ARGS__);

// color prints
#define rprintf(fmt, ...) printf(ANSI_COLOR_RED     fmt ANSI_COLOR_RESET,   __VA_ARGS__)
#define gprintf(fmt, ...) printf(ANSI_COLOR_GREEN   fmt ANSI_COLOR_RESET,   __VA_ARGS__)
#define bprintf(fmt, ...) printf(ANSI_COLOR_BLUE    fmt ANSI_COLOR_RESET,   __VA_ARGS__)
#define wprintf(fmt, ...) printf(ANSI_COLOR_WHITE   fmt ANSI_COLOR_RESET,   __VA_ARGS__)
#define yprintf(fmt, ...) printf(ANSI_COLOR_YELLOW  fmt ANSI_COLOR_RESET, ##__VA_ARGS__)
#define mprintf(fmt, ...) printf(ANSI_COLOR_MAGENTA fmt ANSI_COLOR_RESET, ##__VA_ARGS__)
#define cprintf(fmt, ...) printf(ANSI_COLOR_CYAN    fmt ANSI_COLOR_RESET, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif
#endif // DEBUG_H
