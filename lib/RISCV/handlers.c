//See LICENSE for license details.
#include <stdint.h>
#include "riscv_encoding.h"
#include "n200_func.h"

__attribute__((weak)) uintptr_t handle_nmi(void)
{
	return 0;
}

__attribute__((weak)) uintptr_t handle_trap(uintptr_t mcause, uintptr_t sp)
{
	if (mcause == 0xFFF)
	{
		handle_nmi();
	}
	// printf("In trap handler, the mcause is %d\n", mcause);
	// printf("In trap handler, the mepc is 0x%x\n", read_csr(mepc));
	// printf("In trap handler, the mtval is 0x%x\n", read_csr(mbadaddr));
	return 0;
}
