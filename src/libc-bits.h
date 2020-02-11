// Copyright © 2020 by P.Orsolic. All right reserved
// Created 191130

#ifndef LIBC_BITS_H
#define LIBC_BITS_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "debug.h"	// for MCU_RUN

#define isspace(c) \
	((c) == ' ' || (c) == '\t' || (c) == '\n' || \
	 (c) == '\r' || (c) == '\f' || (c) == '\v')

#ifdef MCU_RUN

#include <stdint.h>
#ifndef size_t
#define size_t uint32_t
#endif	// size_t
#ifndef NULL
#define	NULL	((void *)0)
#endif	// NULL

extern void panic(void);

void * memset(void *dest, int c, size_t len);

int abs(int x);
void * memchr(const void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

char * strchr(const char *p, int ch);
size_t strlen(const char *str);
int strncmp(const char *s1, const char *s2, size_t n);
char * strncpy(char * __restrict dst, const char * __restrict src, size_t n);
char *strstr(const char *h, const char *n);
char * strtok_r(char *s, const char *delim, char **last);
char * strtok(char *s, const char *delim);
long strtol(const char * __restrict nptr, char ** __restrict endptr, int base);

// non-standard functions:
// int isdigit(int c);
bool isdigit(char c);		// INFO 200126: non-standard function
bool ishexdigit(char c);	// INFO 200126: non-standard function
bool isprintable(const char ch);

#endif	// MCU_RUN

#ifdef __cplusplus
}
#endif	// __cplusplus
#endif // LIBC_BITS_H
