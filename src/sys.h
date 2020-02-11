// Copyright © 2020 by P.Orsolic. All right reserved
// Created 191229
#ifndef SYS_H
#define SYS_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "printf.h"

void system_info_memory(void);
void sysinfo_print(void);
void reset(void);

#ifdef __cplusplus
}
#endif	// __cplusplus
#endif // SYS_H
