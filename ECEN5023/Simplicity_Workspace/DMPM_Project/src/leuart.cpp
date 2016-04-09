/**************************************************************************//**
 * @file leuart.cpp
 * @brief LEUART Driver
 * @author Sean Donohue
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2016 Sean Donohue</b>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 *******************************************************************************/

#include <string.h>
#include "bme280.h"
#include "config.h"
#include "dma.h"
#include "em_cmu.h"
#include "em_dma.h"
#include "em_gpio.h"
#include "em_int.h"
#include "em_leuart.h"
#include "leuart.h"
#include "mbed.h"
#include "sleepmodes.h"

#define LEUART_LOOPBACK		false

#define END_OF_CMD_DELIMITER	'!'
#define END_OF_CMD_DELIM_SIZE	1

// LEUART TX PD4
#define LEUART_TX_Port		gpioPortD
#define LEUART_TX_Pin		4
#define LEUART_TX_Mode		gpioModePushPull

// LEUART RX PD5
#define LEUART_RX_Port		gpioPortD
#define LEUART_RX_Pin		5
#define LEUART_RX_Mode		gpioModeInputPull

#define LEUART_BAUD_RATE	9600
#define LEUART_STOP_BITS	leuartStopbits1
#define LEUART_DATA_BITS	leuartDatabits8
#define LEUART_PARITY		leuartNoParity

#if LEUART_BAUD_RATE > 9600
#error LEUART does not support baud rates greater than 9600.
#endif

DMA_CB_TypeDef LEUART_TX_Done_CB;
DMA_CB_TypeDef LEUART_RX_Done_CB;

uint8_t LEUART_RX_Buf[LEUART_RX_DMA_BUF_SIZE] __attribute__((aligned(256)));
uint8_t LEUART_TX_Buf[LUEART_NUM_TX_BUF][LEUART_TX_DMA_BUF_SIZE] __attribute__((aligned(256)));

uint8_t active_buf = 0;
uint32_t active_buf_empty_idx = 0;

typedef struct {
	const char *str;
	void (*cmd_func)(void);
} BT_Cmd_t;

static void returnTemperature(void);

// Commands
const BT_Cmd_t CMD_TABLE[] = {
	{"RetTemp\0", returnTemperature},
	{"RetHum\0", 0},
	{'\0', 0}
};

#define MAX_CMD_SIZE 	sizeof(CMD_TABLE[0].str)

/**************************************************************************//**
 * @brief Initializes LEUART
 * @verbatim LEUART_Initialize(void); @endverbatim
 *****************************************************************************/
void LEUART_Initialize(void)
{
	LEUART_Init_TypeDef LEUART_InitVal = LEUART_INIT_DEFAULT;

	// Set LEUART pins for high idle state on TX and pull-up on RX
	GPIO_PinModeSet(LEUART_TX_Port, LEUART_TX_Pin, LEUART_TX_Mode, 1);
	GPIO_PinModeSet(LEUART_RX_Port, LEUART_RX_Pin, LEUART_RX_Mode, 1);

	// Enable the clock to LEUART0
	CMU_ClockEnable(cmuClock_LEUART0, true);

	// Configure LEUART0
	LEUART_InitVal.databits = LEUART_DATA_BITS;
	LEUART_InitVal.parity = LEUART_PARITY;
	LEUART_InitVal.stopbits = LEUART_STOP_BITS;
	LEUART_InitVal.baudrate = LEUART_BAUD_RATE;
	LEUART_InitVal.refFreq = 0;	 // Let the baudrate calc function figure out the frequency
	LEUART_InitVal.enable = leuartEnable;

	// Set the signal frame interrupt to END_OF_CMD_DELIMITER
	LEUART0->SIGFRAME = END_OF_CMD_DELIMITER;

	// Route the TX and RX pins to PD4 and PD5 and enable the RX and TX pin
	LEUART0->ROUTE = LEUART_ROUTE_LOCATION_LOC0 | LEUART_ROUTE_RXPEN | LEUART_ROUTE_TXPEN;

	// Enable signal frame interrupts
	LEUART_IntEnable(LEUART0, LEUART_IEN_SIGF);
	NVIC_EnableIRQ(LEUART0_IRQn);

	LEUART_DMA_Init();

	// Finally, enable LEUART0
	LEUART_Init(LEUART0, &LEUART_InitVal);

	// Allow RXs to wake the DMA controller up when in EM2
	LEUART0->CTRL |= LEUART_CTRL_RXDMAWU;

#if LEUART_LOOPBACK
	// Enable loopback
	LEUART0->CTRL |= LEUART_CTRL_LOOPBK;
#endif

	LEUART_DMA_Init();

	// LEUART will always be listening for bluetooth data, so never let the
	// processor go lower than EM2
	blockSleepMode(EM2);
}

/**************************************************************************//**
 * @brief Initializes DMA for LEUART
 * @verbatim LEUART_DMA_Init(void); @endverbatim
 *****************************************************************************/
void LEUART_DMA_Init(void)
{
	DMA_CfgChannel_TypeDef LEUART_TX_CfgChannel;
	DMA_CfgChannel_TypeDef LEUART_RX_CfgChannel;
	DMA_CfgDescr_TypeDef LEUART_TX_Cfg_Descr;
	DMA_CfgDescr_TypeDef LEUART_RX_Cfg_Descr;

	// Configure the RX channel
	LEUART_RX_Done_CB.cbFunc = LEUART_RX_DMA_Done_CB;
	LEUART_RX_Done_CB.userPtr = NULL;
	LEUART_RX_CfgChannel.cb = &LEUART_RX_Done_CB;
	LEUART_RX_CfgChannel.enableInt = true;
	LEUART_RX_CfgChannel.highPri = LEUART_RX_DMA_PRI;
	LEUART_RX_CfgChannel.select = DMA_CH_CTRL_SOURCESEL_LEUART0 | DMA_CH_CTRL_SIGSEL_LEUART0RXDATAV;
	DMA_CfgChannel(LEUART_RX_DMA_CH, &LEUART_RX_CfgChannel);

	LEUART_RX_Cfg_Descr.arbRate = LEUART_RX_DMA_ARB;
	LEUART_RX_Cfg_Descr.dstInc = dmaDataInc1;
	LEUART_RX_Cfg_Descr.size = dmaDataSize1;
	LEUART_RX_Cfg_Descr.srcInc = dmaDataIncNone;
	LEUART_RX_Cfg_Descr.hprot = 0;
	DMA_CfgDescr(LEUART_RX_DMA_CH, true, &LEUART_RX_Cfg_Descr);

	// Enable the LEUART0 RX Channel interrupt
	DMA->IEN |= (1 << LEUART_RX_DMA_CH);

	// Start the RX channel
	DMA_ActivateBasic(LEUART_RX_DMA_CH, true, false, LEUART_RX_Buf,
			(void *) &LEUART0->RXDATA, LEUART_RX_DMA_N_XFERS);

	// Configure the TX channel
	LEUART_TX_Done_CB.cbFunc = LEUART_TX_DMA_Done_CB;
	LEUART_TX_Done_CB.userPtr = NULL;
	LEUART_TX_CfgChannel.cb = &LEUART_TX_Done_CB;
	LEUART_TX_CfgChannel.enableInt = true;
	LEUART_TX_CfgChannel.highPri = LEUART_TX_DMA_PRI;
	LEUART_TX_CfgChannel.select = DMA_CH_CTRL_SOURCESEL_LEUART0 | DMA_CH_CTRL_SIGSEL_LEUART0TXBL;
	DMA_CfgChannel(LEUART_TX_DMA_CH, &LEUART_TX_CfgChannel);

	LEUART_TX_Cfg_Descr.arbRate = LEUART_TX_DMA_ARB;
	LEUART_TX_Cfg_Descr.dstInc = dmaDataIncNone;
	LEUART_TX_Cfg_Descr.size = dmaDataSize1;
	LEUART_TX_Cfg_Descr.srcInc = dmaDataInc1;
	LEUART_TX_Cfg_Descr.hprot = 0;
	DMA_CfgDescr(LEUART_TX_DMA_CH, true, &LEUART_TX_Cfg_Descr);

	// Enable the LEUART0 TX Channel interrupt
	DMA->IEN |= (1 << LEUART_TX_DMA_CH);
}

/**************************************************************************//**
 * @brief LEUART TX done callback
 * @verbatim LEUART_TX_DMA_Done_CB(unsigned int channel, bool primary, void *user);
 * @endverbatim
 *****************************************************************************/
void LEUART_TX_DMA_Done_CB(unsigned int channel, bool primary, void *user)
{
	// Enable the TXC interrupt
	LEUART_IntEnable(LEUART0, LEUART_IEN_TXC);
}

/**************************************************************************//**
 * @brief Transmits the contents of the LEUART TX buffer.
 * @verbatim LEUART_TX_Buffer(void);
 * @endverbatim
 *****************************************************************************/
void LEUART_TX_Buffer(void)
{
	// Enable the transmitter
	LEUART0->CMD |= LEUART_CMD_TXEN;

	// Enable TX DMA wakeup
	LEUART0->CTRL |= LEUART_CTRL_TXDMAWU;

	// Wait for any pending previous write operation to have been completed
	// in low frequency domain
	while (LEUART0->SYNCBUSY & (LEUART_SYNCBUSY_TXDATA | LEUART_SYNCBUSY_CMD | LEUART_SYNCBUSY_CTRL));
	DMA_ActivateBasic(LEUART_TX_DMA_CH, true, false, (void *) &LEUART0->TXDATA,
			LEUART_TX_Buf[active_buf], active_buf_empty_idx);

	// Update the active buffer
	if (active_buf)
		active_buf = 0;
	else
		active_buf = 1;
	active_buf_empty_idx = 0;
}

/**************************************************************************//**
 * @brief Transmits the contents of the LEUART TX buffer but does not use DMA.
 * @verbatim LEUART_TX_Wait(void);
 * @endverbatim
 *****************************************************************************/
void LEUART_TX_Wait(void)
{
	uint16_t tx_idx = 0;

	// Enable the transmitter
	LEUART0->CMD |= LEUART_CMD_TXEN;

	// Wait for any pending previous write operation to have been completed
	// in low frequency domain
	while (LEUART0->SYNCBUSY & (LEUART_SYNCBUSY_TXDATA | LEUART_SYNCBUSY_CMD));

	while (tx_idx < active_buf_empty_idx)
	{
		LEUART0->TXDATA = LEUART_TX_Buf[active_buf][tx_idx++];
		while (LEUART0->SYNCBUSY & (LEUART_SYNCBUSY_TXDATA | LEUART_SYNCBUSY_CMD));
		while (!(LEUART0->STATUS & LEUART_STATUS_TXC));
	}

	active_buf_empty_idx = 0;

	// Disable the transmitter and clear the TX shift register
	LEUART0->CMD |= LEUART_CMD_TXDIS | LEUART_CMD_CLEARTX;
	while (LEUART0->SYNCBUSY & LEUART_SYNCBUSY_CMD);

	// Clear Transmit Flags
	LEUART0->IFC |= LEUART_IFC_TXC;
}

/**************************************************************************//**
 * @brief Checks if a TX is in progress
 * @verbatim LEUART_TX_Active(void);
 * @endverbatim
 *****************************************************************************/
bool LEUART_TX_Active(void)
{
	// First check to see if the DMA channel is active
	if (DMA_ChannelEnabled(LEUART_TX_DMA_CH))
		return true;

	// Next check to see if the transmitter is still enabled
	return (LEUART0->STATUS & LEUART_STATUS_TXENS);
}

/**************************************************************************//**
 * @brief Puts the specified data into the TX buffer.
 * @verbatim LEUART_Put_TX_Buffer(uint8_t *data, uint32_t length);
 * @endverbatim
 *****************************************************************************/
void LEUART_Put_TX_Buffer(uint8_t *data, uint32_t length)
{
	// Truncate the buffer is there is not enough room
	if (length > (LEUART_TX_DMA_BUF_SIZE - active_buf_empty_idx))
		length = (LEUART_TX_DMA_BUF_SIZE - active_buf_empty_idx);

	// Disable interrupts while buffering
	INT_Disable();
	memcpy(&LEUART_TX_Buf[active_buf][active_buf_empty_idx], data, length);
	INT_Enable();

	active_buf_empty_idx += length;
}

/**************************************************************************//**
 * @brief Clears the RX buffer and resets the DMA channel
 * @verbatim LEUART_Reset_RX_Buffer(void);
 * @endverbatim
 *****************************************************************************/
void LEUART_Reset_RX_Buffer(void)
{
	DMA_ChannelEnable(LEUART_RX_DMA_CH, false);
	memset(LEUART_RX_Buf, 0, sizeof(LEUART_RX_Buf));
	DMA_ActivateBasic(LEUART_RX_DMA_CH, true, false, LEUART_RX_Buf, (void *) &LEUART0->RXDATA, LEUART_RX_DMA_N_XFERS);
}

/**************************************************************************//**
 * @brief LEUART RX done callback
 * @verbatim LEUART_RX_DMA_Done_CB(unsigned int channel, bool primary, void *user);
 * @endverbatim
 *****************************************************************************/
void LEUART_RX_DMA_Done_CB(unsigned int channel, bool primary, void *user)
{
	// RX buffer is full. Assume that no valid commands have been received
	// and flush the buffer
	memset(LEUART_RX_Buf, 0, sizeof(LEUART_RX_Buf));

	// Restart the RX channel
	DMA_ActivateBasic(LEUART_RX_DMA_CH, true, false, LEUART_RX_Buf, (void *) &LEUART0->RXDATA, LEUART_RX_DMA_N_XFERS);
}

/**************************************************************************//**
 * @brief LEUART0 IRQ Handler
 * @verbatim LEUART0_IRQHandler(void); @endverbatim
 *****************************************************************************/
void LEUART0_IRQHandler(void)
{
	uint8_t tx_buf[40] = {0};
	uint32_t intflags = LEUART_IntGet(LEUART0), i = 0, j = 0;
	uint32_t str_length, transfer_size;
	bool cmd_matched = false;

	LEUART_IntClear(LEUART0, intflags);

	if (intflags & LEUART_IF_SIGF) {
		// Disable the LEUART receiver
		LEUART0->CMD |= LEUART_CMD_RXDIS;

		// Find the delimiter
		while (LEUART_RX_Buf[i] != END_OF_CMD_DELIMITER && i < LEUART_RX_DMA_N_XFERS) {
			i++;
		}

		if (i < (LEUART_RX_DMA_BUF_SIZE - END_OF_CMD_DELIM_SIZE)) {
			// Check for a valid command
			while (CMD_TABLE[j].str != '\0' && !cmd_matched) {
				str_length = strlen(CMD_TABLE[j].str);
				if (memcmp(CMD_TABLE[j].str, &LEUART_RX_Buf[i - str_length], str_length) == 0) {
					cmd_matched = true;
					if (CMD_TABLE[j].cmd_func != NULL)
						CMD_TABLE[j].cmd_func();
					// Insert a null char after the delimiter so that string functions can be used
					LEUART_RX_Buf[i + 1] = '\0';
				}
				else
					j++;
			}

			if (!cmd_matched) {
				transfer_size = sprintf((char *) tx_buf, "%s ERROR: Not valid input!\n\r", (char *) &LEUART_RX_Buf[0]);

				LEUART_Put_TX_Buffer(tx_buf, transfer_size);

				LEUART_TX_Buffer();
			}
		}

		// Reset the RX DMA channel
		transfer_size = LEUART_RX_DMA_N_XFERS - DMA_Get_Transfers_Remaining(LEUART_RX_DMA_CH);
		memset(LEUART_RX_Buf, 0, transfer_size);
		LEUART0->CMD |= (LEUART_CMD_CLEARRX | LEUART_CMD_RXEN);
		DMA_ActivateBasic(LEUART_RX_DMA_CH, true, false, LEUART_RX_Buf, (void *) &LEUART0->RXDATA, LEUART_RX_DMA_N_XFERS);
	}

	if (intflags & LEUART_IF_TXC) {
		// Check if the other DMA buffer is ready for transmit
		if (active_buf_empty_idx > 0)
		{
			LEUART_TX_Buffer();
		}
		else
		{
			// Disable the transmitter
			LEUART0->CMD |= LEUART_CMD_TXDIS;

			// Disable TXDMA wakeup
			LEUART0->CTRL &= ~LEUART_CTRL_TXDMAWU;

			// Disable the TXC interrupt
			LEUART_IntDisable(LEUART0, LEUART_IEN_TXC);
		}
	}
}

static void returnTemperature(void)
{
	uint8_t tx_buf[40] = {0};
	uint32_t tx_size;

	tx_size = sprintf((char *) tx_buf, "%d.%dC\r\n", BME280_Get_Temp() / 10, abs(BME280_Get_Temp() % 10));
	LEUART_Put_TX_Buffer(tx_buf, tx_size);
	LEUART_TX_Buffer();
}
