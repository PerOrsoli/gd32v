// Copyright © 2020 by P.Orsolic. All right reserved
// created 141205 - STM32F1 C
// Created 200104 - GD32V C+

#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include "debug.h"
#include "gpio.hpp"
#include "gd32vf103_rcu.h"

#define I2C_TIMEOUT_MAX 0xFFFF	// for SW delay: how long to wait for I2C bus response

namespace i2c
{
// index for DevMap[]
enum class Device: uint8_t
{
	myI2C0 = 0,
	myI2C1,
	wrong,
};

enum class Speed
{
	// used for CKCFG.FAST
	Speed100kHz		= 0,	// Standard
	Speed400kHz		= 1,	// Fast
};

enum class DutyCycle
{
	// used for CKCFG.DTCY
	// only used for Fast/400kHz mode
	Duty2		= 0,	// Tlow/Thigh = 2
	Duty16_9	= 1,	// Tlow/Thigh = 16/9
};

enum class Mode
{
	// bit used in SADDR0.ADDRESS0
	Read	= 1,	// Receiver, last bit of address = 1
	Write	= 0,	// Transmitter, last bit of address = 0
};

enum class NackPosition
{
	Current,
	Next,
};

enum class Flag
{
	// bits in STAT1
	IsTransmitter,
	// IsBusy,			// I2CBSY
	BSY,
	IsMaster,
	// bits in STAT0
	TxEmpty,
	RxNotEmpty,				// RBNE
	RBNE,
	// ByteTxComplete,		// BTC
	BTC,
	// AddressSent,		// ADDSEND
	ADDSEND,
	// StartSent,
	SBSEND,
	STPDET,		// STOP detected in slave mode
};

enum class Event
{
	MasterModeSelected		= 0x00030001,	//
	MasterTxSelected		= 0x00070082,	// BSY, MASTER
	MasterRxSelected		= 0x00030002,	//
	MasterByteReceived		= 0x00030040,	//
};

void test(void);

void init(Device dev, Speed speed, DutyCycle duty);
void start(Device dev);
void restart(Device dev);
void stop(Device dev);
void enable(Device dev);
void disable(Device dev);
void send_addr(Device dev, uint8_t address, Mode mode);

uint8_t read(Device dev);
void write(Device dev, uint8_t data);

void ack(Device dev);
void nack(Device dev);
void nack_position(Device dev, NackPosition pos);

// 200128 Needed for wii nunchuck:
#define wait_until_flag(dev, flag, condition) _wait_until_flag(dev, flag, condition, __func__, __FILE__, __LINE__);
void _wait_until_flag(Device dev, Flag flag, uint8_t condition, const char* fn_name, const char* file, uint32_t line);
void clear_flag(Device dev, Flag flag);
bool check_event(Event event);

} // namespace

#endif	// I2C_H
