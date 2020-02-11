// Copyright © 2020 by P.Orsolic. All right reserved
#include "sys.h"
#include "gd32vf103_eclic.h"

void system_info_memory(void)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
	// linker variables
	extern uint32_t __ram_start;
	extern uint32_t __ram_size;
	extern uint32_t __ram_end;
	extern uint32_t __flash_start;
	extern uint32_t __flash_end;
	extern uint32_t __flash_size;
	extern uint32_t __section_text_start;
	extern uint32_t __section_text_end;
	extern uint32_t _edata;

	uint32_t ram_start          = (uint32_t)&__ram_start;
	uint32_t ram_size           = (uint32_t)&__ram_size;
	uint32_t ram_end            = (uint32_t)&__ram_end;
	uint32_t flash_start        = (uint32_t)&__flash_start;
	uint32_t flash_end          = (uint32_t)&__flash_end;
	uint32_t flash_size         = (uint32_t)&__flash_size;
	uint32_t section_text_start = (uint32_t)&__section_text_start;
	uint32_t section_text_end   = (uint32_t)&__section_text_end;
	uint32_t edata				= (uint32_t)&_edata;
	/* RAM layout:
	 * [data] [bss] [user heap stack] ......... [initial stack pointer]
	 *
	 * flash layout:
	 * [vectors] [text] .......
	 */

	printf("Flash start: 0x%08x size: 0x%x\r\n", flash_start, flash_size);
	printf("RAM start:   0x%08x size: 0x%x\r\n", ram_start, ram_size);
	printf("edata: 0x%x\r\n", edata);

#pragma GCC diagnostic pop	// "-Wunused-variable"
}

void sysinfo_print(void)
{
#define ADDRESS_MEMORY_DENSITY	0x1FFFF7E0
#define ADDRESS_UID1			0x1FFFF7E8
#define ADDRESS_UID2			0x1FFFF7EC
#define ADDRESS_UID3			0x1FFFF7F0

	uint32_t memory_density = *((uint32_t *)ADDRESS_MEMORY_DENSITY);
	uint16_t sram_density  = (memory_density & 0xFFFF0000) >> 16;
	uint16_t flash_density = (memory_density & 0x0000FFFF);
	uint32_t uid[3];
	uid[0] = *((uint32_t *)ADDRESS_UID1);
	uid[1] = *((uint32_t *)ADDRESS_UID2);
	uid[2] = *((uint32_t *)ADDRESS_UID3);

	printf("SRAM: %d kB flash: %d kB\r\n", sram_density, flash_density);
	printf("UID: 0x%x 0x%x 0x%x\r\n", uid[0], uid[1], uid[2]);

	printf("Linker variables:\r\n");
	system_info_memory();
}

void reset(void)
{
	eclic_system_reset();
}
