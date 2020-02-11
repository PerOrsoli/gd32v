#ifndef STRLEN_H
#define STRLEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#define size_t	uint32_t

size_t strlen(const char *str);

#ifdef __cplusplus
}
#endif	// __cplusplus
#endif // STRLEN_H
