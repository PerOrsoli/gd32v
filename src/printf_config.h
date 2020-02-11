// created 190714
// https://github.com/mpaland/printf

/* PRINTF_INCLUDE_CONFIG_H */
/* Define this as compiler switch (e.g. gcc -DPRINTF_INCLUDE_CONFIG_H) to include a "printf_config.h" definition file */

// save around 6 kB of space in binary
#define PRINTF_DISABLE_SUPPORT_FLOAT
#define PRINTF_DISABLE_SUPPORT_EXPONENTIAL
#define PRINTF_DISABLE_SUPPORT_LONG_LONG
#define PRINTF_DISABLE_SUPPORT_PTRDIFF_T
