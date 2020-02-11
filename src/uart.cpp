// Copyright © 2020 by P.Orsolic. All right reserved
#include "uart.hpp"
#include "libc-bits.h"	// memset()
#include "exti.hpp"
#include "gd32vf103_rcu.h"	// for clocks, for now

namespace uart
{

#define UART1_RX_BUFFER_SIZE 128
static volatile uint8_t UART1_RX_buffer[UART1_RX_BUFFER_SIZE];
volatile bool uart1_rx_done = false;
static uint8_t current_buffer_position = 0;

// UART hw driver				 											{{{
// ----------------------------------------------------------------------------
// // created 200102
static const uint32_t address_UART0 = 0x40013800;
static const uint32_t address_UART1 = 0x40004400;
static const uint32_t address_UART2 = 0x40004800;
static const uint32_t address_UART3 = 0x40004C00;
static const uint32_t address_UART4 = 0x40005000;

typedef struct
{
	volatile uint32_t STAT;	// status bits
	volatile uint32_t DATA;	// write for TX, read for RX
	volatile uint32_t BAUD;	// baud rate
	volatile uint32_t CTL0;	// config
	volatile uint32_t CTL1;	// config
	volatile uint32_t CTL2;	// config
	volatile uint32_t GP;	// used for SmartCard and IrDA
} UartReg;

static volatile UartReg* UART0 = (UartReg *)address_UART0;
static volatile UartReg* UART1 = (UartReg *)address_UART1;
static volatile UartReg* UART2 = (UartReg *)address_UART2;
static volatile UartReg* UART3 = (UartReg *)address_UART3;
static volatile UartReg* UART4 = (UartReg *)address_UART4;

typedef struct
{
	Uart				uart;
	volatile UartReg*	reg;
} UartMap;

UartMap UartMaps[] = {
	{Uart::Uart0,		UART0},
	{Uart::Uart1,		UART1},
	{Uart::Uart2,		UART2},
	{Uart::Uart3,		UART3},
	{Uart::Uart4,		UART4},
};

enum class StatBits: uint32_t
{
	CTSF	= (1 << 9),		// sync UART...
	LBDF	= (1 << 8),		// LIN...
	// TBE		= (1 << 7),		// transmit data buffer empty
	// TC		= (1 << 6),		// transmission complete
	// RBNE	= (1 << 5),		// read data buffer not empty
	IDLEF	= (1 << 4),		// idle frame detection flag
	ORERR	= (1 << 3),		// overrun error flag
	NERR	= (1 << 2),		// noise error flag
	FERR	= (1 << 1),		// frame error flag
	PERR	= (1 << 0),		// parity error
};

enum class Flag: uint8_t
{
	// bits in STAT register
	TxBufferEmpty	= 7,
	TxComplete		= 6,
	RxNotEmpty		= 5,
};

bool get_flag(Uart uart, Flag flag)
{
	volatile UartReg* reg = UartMaps[(uint8_t)uart].reg;

	return (reg->STAT & (1 << (uint8_t)flag)) >> (uint8_t)flag;
}

void clear_flag(Uart uart, Flag flag)
{
	volatile UartReg* reg = UartMaps[(uint8_t)uart].reg;

	reg->STAT &= ~(1 << (uint8_t)flag);
}

// unused
enum class BaudBits: uint32_t
{
	INTDIV	= (0b11111111111 << 4),		// [11:0]
	FRADIV	= (0b1111 << 0),			// [3:0]
};

enum class Ctl0Bits: uint32_t
{
	UEN		= (13),	// UART enable
	WL		= (1 << 12),	// word length
	WM		= (1 << 11),	// sync... wake up by idle frame or address match
	PCEN	= (1 << 10),	// parity check enable
	PM		= (1 << 9),		// parity odd/even
	// PERRIE	= (1 << 8),		// parity error interrupt enable
	// TBEIE	= (1 << 7),		// TX buffer empty interrupt enable
	// TCIE	= (1 << 6),		// TX complete interrupt enable
	// RBNEIE	= (1 << 5),		// RX buffer not empty interrupt enable
	// IDLEIE	= (1 << 4),		// idle frame detection interrupt enable
	TEN		= (3),		// TX enable
	REN		= (2),		// RX enable
	RWU		= (1 << 1),		// RX wake up from mute mode
	SBKCMD	= (1 << 0),		// send brake command
};

enum class Interrupt: uint8_t
{
	ParityError		= 8,
	TxBufferEmpty	= 7,	// FIFO empty, but data are still being transmitted
	TxComplete		= 6,		// all data sent to the bus
	RxNotEmpty		= 5,
	Rx				= 5,
	Idle			= 4,
};

// private type, used for readability
// enum class WordLength: uint8_t
enum class WordLength: uint32_t
{
	// values for CTL0.WL
	EightBits	= (0 << 12),
	NineBits	= (1 << 12),
};

// private type, used for readability
enum class StopBits: uint32_t
{
	// values for CTL1.STB[1:0]
	One			= (0b00 << 13),
	Half		= (0b01 << 13),		// not for UART3 and 4
	Two			= (0b10 << 13),
	OneAndHalf	= (0b11 << 13),		// not for UART3 and 4
};

typedef struct
{
	Mode		mode;
	WordLength	length;
	StopBits	stop;
} ModeMap;

ModeMap ModeMaps[] = {
	{Mode::EightNoneOne,	WordLength::EightBits,	StopBits::One},
};

enum class Ctl1Bits: uint32_t
{
	LIN		= (1 << 14),	// LIN mode enable/disable
	STB		= (0b11 << 13),	// stop bit length
	CKEN	= (1 << 11),	// sync UART, CK pin
	CPL		= (1 << 10),	// sync UART, CK polarity
	CPH		= (1 << 9),		// sync UART, CK phase
	CLEN	= (1 << 8),		// sync UART, CK length
	LBDIE	= (1 << 6),		// LIN...
	LBLEN	= (1 << 5),		// LIN...
	ADDR	= (0b111 << 0),		// address
};

enum class Ctl2Bits: uint32_t
{
	CTSIE	= (1 << 10),	// sync UART...
	CTSEN	= (1 << 9),		// sync UART...
	RTSEN	= (1 << 8),		// sync UART...
	DENT	= (1 << 7),		// DMA TX
	DENR	= (1 << 6),		// DMA RX
	SCEN	= (1 << 5),		// smartcard mode
	HKEN	= (1 << 4),		// smartcard mode
	HDEN	= (1 << 3),		// half duplex mode
	IRLP	= (1 << 2),		// IrDa
	IREN	= (1 << 1),		// IrDa
	ERRIE	= (1 << 0),		// error interrupt enable
};

uint8_t read_ch(Uart uart)
{
	// read from DR for RX, write in it for TX
	volatile UartReg* reg = UartMaps[(uint8_t)uart].reg;
	return (uint8_t)(reg->DATA & 0xFF);
	// RBNE is cleared automatically after reading DATA
}

void write_ch(Uart uart, uint8_t ch)
{
	volatile UartReg* reg = UartMaps[(uint8_t)uart].reg;

	// wait until TX buffer is empty
	while (get_flag(uart, Flag::TxBufferEmpty) != 1);
	// write data
	reg->DATA = ch;
	// wait for completition of TX (until it is sent on the bus)
	while (get_flag(uart, Flag::TxComplete) != 1);
}

// TODO 200103 replace with my own implementation of RCU driver
// TODO move it to RCU part
static uint32_t get_uart_clock(Uart uart)
{
	switch (uart)
	{
		case Uart::Uart0:
			return rcu_clock_freq_get(CK_APB2);
		case Uart::Uart1:
		case Uart::Uart2:
		case Uart::Uart3:
		case Uart::Uart4:
			return rcu_clock_freq_get(CK_APB1);
	}
}

static void set_mode(Uart uart, Mode mode)
{
	volatile UartReg* reg = UartMaps[(uint8_t)uart].reg;
	uint8_t word_length = (uint8_t)ModeMaps[(uint8_t)mode].length;
	uint8_t stop_bits   = (uint8_t)ModeMaps[(uint8_t)mode].stop;

	// set data bit length
	reg->CTL0 &= ~word_length;
	reg->CTL0 |=  word_length;

	// set stop bits
	reg->CTL1 &= ~stop_bits;
	reg->CTL1 |=  stop_bits;
}

static void set_baudrate(Uart uart, Speed speed)
{
	volatile UartReg* reg = UartMaps[(uint8_t)uart].reg;
	uint32_t baud       = (uint32_t)speed;
	uint32_t bus_clk = get_uart_clock(uart);
	uint16_t intdiv =     bus_clk / (16 * baud);
	uint16_t fradiv = 16*(bus_clk % (16 * baud)) / (16 * baud);
	uint16_t regvalue = (intdiv << 4) | fradiv;

	reg->BAUD &= ~regvalue;
	reg->BAUD |=  regvalue;
}

static void enable(Uart uart, bool state)
{
	volatile UartReg* reg = UartMaps[(uint8_t)uart].reg;

	reg->CTL0 &=    ~(1 << (uint32_t)Ctl0Bits::UEN);
	reg->CTL0 |=  state << (uint32_t)Ctl0Bits::UEN;
}

static void enable_tx(Uart uart, bool state)
{
	volatile UartReg* reg = UartMaps[(uint8_t)uart].reg;

	reg->CTL0 &=    ~(1 << (uint32_t)Ctl0Bits::TEN);
	reg->CTL0 |=  state << (uint32_t)Ctl0Bits::TEN;
}

static void enable_rx(Uart uart, bool state)
{
	volatile UartReg* reg = UartMaps[(uint8_t)uart].reg;

	reg->CTL0 &=    ~(1 << (uint32_t)Ctl0Bits::REN);
	reg->CTL0 |=  state << (uint32_t)Ctl0Bits::REN;
}

// enable/disable interrupts
static void interrupt(Uart uart, Interrupt interrupt, bool state)
{
	volatile UartReg* reg = UartMaps[(uint8_t)uart].reg;

	reg->CTL0 |= (state << (uint8_t)interrupt);
}

void init2(Uart uart, Speed speed, Mode mode)
{
	// GPIO init:
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_USART0);
	gpio_init2(UART0_TX, gpio::AfioPP,     gpio::Speed50MHz);
	gpio_init2(UART0_RX, gpio::InFloating, gpio::SpeedNA);

	// RCU clock must be enabled before configuring UART part

	// set word length, parity and stop bits:
	set_mode(uart, mode);
	// enable DMA TODO
	set_baudrate(uart, speed);

	enable_tx(uart, 1);
	enable_rx(uart, 1);
	interrupt(uart, Interrupt::Rx, 1);

	// some bits must be changed before UART is enabled, so enabled it at end:
	enable(uart, 1);
}

static void test_address(void)
{
	ASSERT_EQ(address_UART0, 0x40013800);
	ASSERT_EQ(address_UART1, 0x40004400);
	ASSERT_EQ(address_UART2, 0x40004800);
	ASSERT_EQ(address_UART3, 0x40004C00);
	ASSERT_EQ(address_UART4, 0x40005000);

	ASSERT_EQ((uint32_t)&UART0->STAT,	address_UART0 + 0x00);
	ASSERT_EQ((uint32_t)&UART0->DATA,	address_UART0 + 0x04);
	ASSERT_EQ((uint32_t)&UART0->BAUD,	address_UART0 + 0x08);
	ASSERT_EQ((uint32_t)&UART0->CTL0,	address_UART0 + 0x0C);
	ASSERT_EQ((uint32_t)&UART0->CTL1,	address_UART0 + 0x10);
	ASSERT_EQ((uint32_t)&UART0->CTL2,	address_UART0 + 0x14);
	ASSERT_EQ((uint32_t)&UART0->GP,		address_UART0 + 0x18);
}

// test translation from Uart0 -> address
static const uint32_t test_mapping_one(Uart uart)
{
	volatile UartReg* reg = UartMaps[(uint8_t)uart].reg;
	return (uint32_t)reg;
}

static void test_set_mode(void)
{
	volatile UartReg* reg = UartMaps[(uint8_t)Uart::Uart0].reg;
	set_mode(Uart::Uart0, Mode::EightNoneOne);
	ASSERT_EQ(reg->CTL0 & (uint32_t)Ctl0Bits::WL,	(0 << 12));
	ASSERT_EQ(reg->CTL0 & (uint32_t)Ctl0Bits::PCEN,	(0 << 11));		// parity enable
	ASSERT_EQ(reg->CTL0 & (uint32_t)Ctl0Bits::PM,	(0 << 10));		// parity odd/even
	ASSERT_EQ(reg->CTL1 & (uint32_t)Ctl1Bits::STB,	(0b00 << 13));
}

static void test_enable(void)
{
	enable_tx(Uart::Uart0, 1);
	enable_rx(Uart::Uart0, 1);
	enable(Uart::Uart0, 1);
}

void test(void)
{
	test_address();

	uint32_t ret;
	ret = test_mapping_one(Uart::Uart0);
	ASSERT_EQ(ret, address_UART0);
	ret = test_mapping_one(Uart::Uart1);
	ASSERT_EQ(ret, address_UART1);
	ret = test_mapping_one(Uart::Uart2);
	ASSERT_EQ(ret, address_UART2);
	ret = test_mapping_one(Uart::Uart3);
	ASSERT_EQ(ret, address_UART3);
	ret = test_mapping_one(Uart::Uart4);
	ASSERT_EQ(ret, address_UART4);

	test_set_mode();
}
// ------------------------------------------------------------------------ }}}

// UART0 clear screen
void clear(void)
{
	const char* clear_string = "\033c";
	write_ch(Uart::Uart0, clear_string[0]);
	write_ch(Uart::Uart0, clear_string[1]);
}
// print UART RX buffer			 											{{{
// ----------------------------------------------------------------------------
static void print_uart1_rx_buffer(void)
{
	printf("UART1 RX buffer:\r\n");
	for (uint8_t i = 0; i<UART1_RX_BUFFER_SIZE; i++)
	{
		const char ch = UART1_RX_buffer[i];

		if (isprintable(ch) == 1)
		{
			printf("%c", ch);
		}
		else
		{
			printf("[0x%x]", ch);
		}
	}
	printf("\r\n");
}
// ------------------------------------------------------------------------ }}}

// clear UART RX buffer			 											{{{
// ----------------------------------------------------------------------------
void clear_rx_buffer(void)
{
	memset((void *)UART1_RX_buffer, '\0', UART1_RX_BUFFER_SIZE);
	current_buffer_position = 0;

	printf("after clearing:\r\n");
	print_uart1_rx_buffer();
}
// ------------------------------------------------------------------------ }}}
}	// namespace uart

// ----------------------------------------------------------------------------

extern "C"	// don't mangle
{
	using namespace uart;
// uart 1 ISR																{{{
// ----------------------------------------------------------------------------
void USART0_IRQHandler(void)
{
	if (uart::get_flag(Uart::Uart0, Flag::RxNotEmpty) == 1)
	{
		uint8_t recv = uart::read_ch(Uart::Uart0);

		if (current_buffer_position < UART1_RX_BUFFER_SIZE)
		{
			UART1_RX_buffer[current_buffer_position++] = recv;
		}
		else
		{
			eprintf("UART1 RX buffer is full!\r\n");
		}
	}
}
// ------------------------------------------------------------------------ }}}
// uart 1 RX																{{{
// ----------------------------------------------------------------------------
const uint8_t uart1_get_rx1(void)
{
	// return char by char, handling should be in upper layers
	static uint8_t counter = 0;
	uint8_t last = strlen((const char*)UART1_RX_buffer);

	while (counter < last)
	{
		return UART1_RX_buffer[counter++];
	}
	return 0;
}
// ------------------------------------------------------------------------ }}}

// 191226 3rd party printf:
void _putchar(char ch)
{
	uart::write_ch(Uart::Uart0, ch);
}

}	// extern "C"	// don't mangle
