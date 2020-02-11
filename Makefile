# Copyright © 2020 by P.Orsolic. All right reserved
# updated 191225 RISC-V MCU (GCC & LLVM)
include config.mk

NAME	= main
CLANG_VERSION = 9

OS = $(shell uname)
ifeq "$(TOOLCHAIN)" "gcc"
# PlatformIO toolchain:
TARGET	= -march=rv32imac -mabi=ilp32 -mcmodel=medlow

PREFIX=~/.platformio/packages/toolchain-gd32v/bin/riscv-nuclei-elf-
CC			= $(PREFIX)gcc
CXX			= $(PREFIX)c++
AS			= $(PREFIX)gcc
LD			= $(PREFIX)gcc
OBJCOPY		= $(PREFIX)objcopy
OBJDUMP		= $(PREFIX)objdump
SIZE		= $(PREFIX)size

LD_FLAGS	= $(TARGET) -T $(LINKER_FILE) -nostartfiles -Xlinker --gc-sections --specs=nano.specs
LINK_LIBS	+= -lc

else ifeq "$(TOOLCHAIN)" "llvm"
TARGET	= -target riscv32 -march=rv32imac -mabi=ilp32

ifeq "$(OS)" "FreeBSD"
CLANG_VERSION2 = $(CLANG_VERSION)0
OBJCOPY		= riscv32-unknown-freebsd12.0-objcopy
else ifeq "$(OS)" "Linux"
CLANG_VERSION2 = -$(CLANG_VERSION)
OBJCOPY		= ~/.platformio/packages/toolchain-gd32v/bin/riscv-nuclei-elf-objcopy
else
$(error Wrong OS: $(OS))
endif

CC			= clang$(CLANG_VERSION2)
CXX			= clang++$(CLANG_VERSION2)
AS			= clang$(CLANG_VERSION2)
LD			= ld.lld$(CLANG_VERSION2)
# OBJCOPY	= # 191226 llvm-objcopy90 won't work - generated file will be too large
OBJDUMP		= llvm-objdump$(CLANG_VERSION2)
SIZE		= llvm-size$(CLANG_VERSION2)

LD_FLAGS	= --Bstatic --gc-sections --Map=$(DIR_BUILD)/$(NAME).map --script $(LINKER_FILE)

# libgcc includes and libs:
DIRS	+= -I ./lib/gcc/include \
		   -I ./lib/gcc/include-fixed
DIR_LIBS	+= -L ./lib/gcc
LINK_LIBS	+= -lgcc
else
$(error Wrong toolchain: $(TOOLCHAIN))
endif

OPTS	?= -Os -g3 -Werror=return-type

DIRS	+= -I . -I src \
		   -I lib/periph_lib/ \
		   -I lib/RISCV/ \
		   -I lib/printf -I src/3rd_party

LINKER_FILE		= lib/linker/linker.ld
DEFINES += -DDEBUG -DRUN_TESTS
DIR_BUILD	= ./build

COMPILE_JSON 	= -MJ $@.json
COMMON_FLAGS	= $(TARGET) $(CPU) $(OPTS) -Wall -ffreestanding $(DEFINES) $(DIRS) $(COMPILE_JSON)

CCFLAGS		= $(COMMON_FLAGS) -std=c99 -gdwarf-2
CXXFLAGS	= $(COMMON_FLAGS) -std=c++11 -gdwarf-2 -fno-exceptions -fno-non-call-exceptions -fno-rtti
ASFLAGS		= $(COMMON_FLAGS) -x assembler-with-cpp

FLAGS_3RD_PARTY= -DPRINTF_INCLUDE_CONFIG_H=src/printf_config.h

SRCS += ./lib/RISCV/init.c
SRCS += ./lib/RISCV/handlers.c
SRCS += ./lib/RISCV/n200_func.c
SRCS += ./lib/RISCV/start.s		# needed for disable_mcycle_minstret
SRCS += ./lib/RISCV/entry.s	# needed for trap_entry
SRCS += ./lib/periph_lib/system_gd32vf103.c
SRCS += ./lib/periph_lib/gd32vf103_rcu.c
SRCS += ./lib/periph_lib/gd32vf103_eclic.c
SRCS += ./lib/periph_lib/gd32vf103_timer.c
SRCS += ./src/delay.c
SRCS += $(wildcard src/3rd_party/str*.c)
SRCS += $(wildcard src/3rd_party/mem*.c)
SRCS += src/libc-bits.c	# memset()
SRCS += src/debug.c
SRCS += lib/printf/printf.c
SRCS += src/utils.cpp
SRCS += src/gpio.cpp
SRCS += src/gpio_hw.cpp
SRCS += src/test_gpio.cpp
SRCS += src/exti.cpp
SRCS += src/gpio-exti.cpp
SRCS += src/uart.cpp
SRCS += src/sys.c
SRCS += src/i2c.cpp
SRCS += src/i2c-patch.c
SRCS += src/eeprom.cpp
SRCS += src/baro.cpp
SRCS += src/wii-nunchuck.cpp
SRCS += src/pwm.cpp
SRCS += src/rtc.cpp
SRCS += ./lib/periph_lib/gd32vf103_pmu.c
SRCS += ./lib/periph_lib/gd32vf103_bkp.c
SRCS += src/date.cpp
SRCS += main.cpp
JSONS := $(wildcard ${DIR_BUILD}/*.json)

OBJS1 = $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRCS)))) # rename .c, .cpp and .s to .o
OBJS2 = $(notdir $(OBJS1))	# remove path, leave just file.o
OBJS3 = $(addprefix $(DIR_BUILD)/,$(OBJS2))
OBJS  = $(OBJS3)
OBJSi = $(patsubst %.o,%.i,$(OBJS))		# TODO 190714

# use color if make is run from terminal (-t 0 -t 1 and -t 2 == 1)
ifeq "$(shell if [ -t 0 ] ; then echo 1; fi)" "1"
	COLOR=y
endif
ifeq "$(COLOR)" "y"
COLOR_RED          = \033[0;31m
COLOR_GREEN        = \033[0;32m
COLOR_YELLOW       = \033[0;33m
COLOR_BLUE         = \033[0;34m
COLOR_MAGENTA      = \033[0;35m
COLOR_CYAN         = \033[0;36m
COLOR_RED_BOLD     = \033[1;31m
COLOR_GREEN_BOLD   = \033[1;32m
COLOR_YELLOW_BOLD  = \033[1;33m
COLOR_BLUE_BOLD    = \033[1;34m
COLOR_MAGENTA_BOLD = \033[1;35m
COLOR_CYAN_BOLD    = \033[1;36m
COLOR_RESET        = \033[m
endif

all: upload
elf: $(NAME).elf

# create dirs if they don't exist
dirs: $(DIR_BUILD)

multi:
	@mkdir -p $(DIR_BUILD)
	@${MAKE} -j4 elf

clean:
	@printf "$(COLOR_GREEN_BOLD)[cleaning]$(COLOR_RESET)\n";
	rm -rf $(DIR_BUILD)/*

$(DIR_BUILD)/%.o: lib/RISCV/%.c
	@printf "$(COLOR_YELLOW_BOLD)[system]       $(COLOR_GREEN) $< -> $@ $(COLOR_RESET)\n";
	$(CC) $(CCFLAGS) -c -o $@ $<

$(DIR_BUILD)/%.o: lib/RISCV/%.s
	@printf "$(COLOR_YELLOW_BOLD)[system asm]   $(COLOR_GREEN) $< -> $@ $(COLOR_RESET)\n";
	$(AS) $(ASFLAGS) -c -o $@ $<

$(DIR_BUILD)/%.o: lib/periph_lib/%.c
	@printf "$(COLOR_CYAN_BOLD)[periph lib]    $(COLOR_GREEN) $< -> $@ $(COLOR_RESET)\n";
	$(CC) $(CCFLAGS) -c -o $@ $<
	@#$(CC) -S $(CCFLAGS) -c -o $@.s $<

$(DIR_BUILD)/%.o: lib/printf/%.c
	@printf "$(COLOR_CYAN_BOLD)[user 3rd party]$(COLOR_GREEN) $< -> $@ $(COLOR_RESET)\n";
	$(CC) $(FLAGS_3RD_PARTY) $(CCFLAGS) -c -o $@ $<

$(DIR_BUILD)/%.o: src/3rd_party/%.c
	@printf "$(COLOR_CYAN_BOLD)[libc bits]     $(COLOR_GREEN) $< -> $@ $(COLOR_RESET)\n";
	$(CC) $(CCFLAGS) -c -o $@ $<

$(DIR_BUILD)/%.o: src/%.c
	@printf "$(COLOR_MAGENTA_BOLD)[user]          $(COLOR_GREEN) $< -> $@ $(COLOR_RESET)\n";
	$(CC) $(CCFLAGS) -c -o $@ $<
	@#$(CC) -S $(CCFLAGS) -c -o $@.s $<

$(DIR_BUILD)/%.o: src/%.cpp
	@printf "$(COLOR_MAGENTA_BOLD)[user C++]      $(COLOR_GREEN) $< -> $@ $(COLOR_RESET)\n";
	$(CXX) $(CXXFLAGS) -c -o $@ $<
	@# $(CXX) -MM -MF $(patsubst %.o,%.d,$@) $(CXXFLAGS) -c -o $@ $<
	@# $(CXX) -S $(CXXFLAGS) -c -o $@.s $<

$(DIR_BUILD)/version.o: src/version.c
	@printf "$(COLOR_MAGENTA)[version]$(COLOR_RESET)\n";
	$(CC) $(CCFLAGS) -DVERSION='$(VERSION)' -c -o $@ $<

$(DIR_BUILD)/%.json: ${SRC}
	@printf "$(COLOR_MAGENTA)[version]$(COLOR_RESET)\n";
	$(CC) $(CCFLAGS) -DVERSION='$(VERSION)' -c -o $@ $<

json: ${JSONS}
	@echo "["      > $(DIR_BUILD)/compile_commands.json
	@cat ${JSONS} >> $(DIR_BUILD)/compile_commands.json
	@echo "]"     >> $(DIR_BUILD)/compile_commands.json
	@# remove last comma, in Makefile extra '$' is needed:
	@sed -i '' -e x -e '$$ {s/,$$//;p;x;}' -e 1d $(DIR_BUILD)/compile_commands.json

# main.elf depends on created dirs and objects (order is important)
$(NAME).elf: dirs $(OBJS)
	@printf "$(COLOR_GREEN_BOLD)[linking]  $(COLOR_YELLOW) *.o -> $@ $(COLOR_RESET)\n";
	touch src/version.c
	$(LD) -o $(DIR_BUILD)/$(NAME).elf $(LD_FLAGS) $(OBJS) $(DIR_LIBS) $(LINK_LIBS)
	$(OBJDUMP) -x -s -S $(DIR_BUILD)/$(NAME).elf > $(DIR_BUILD)/$(NAME).lst
	$(SIZE) $(DIR_BUILD)/$(NAME).elf
	$(OBJCOPY) -O binary $(DIR_BUILD)/$(NAME).elf $(DIR_BUILD)/$(NAME).bin

$(DIR_BUILD):
	@mkdir -p $(DIR_BUILD)

upload: $(NAME).elf
	@printf "$(COLOR_GREEN_BOLD)[upload]$(COLOR_RESET)\n";
	doas ~/.opt/bin/dfu-util -d 28e9:0189 -a 0 --dfuse-address 0x08000000:leave -D $(DIR_BUILD)/$(NAME).bin
