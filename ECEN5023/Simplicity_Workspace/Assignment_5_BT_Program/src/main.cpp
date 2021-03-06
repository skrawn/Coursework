/**************************************************************************//**
 * @file main.cpp
 * @brief Assignment 5 - BT Programming
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

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "mbed.h"
#include "sleepmodes.h"
#include "em_adc.h"
#include "em_chip.h"
#include "em_device.h"
#include "em_dma.h"
#include "em_gpio.h"
#include "em_prs.h"
#include "em_letimer.h"
#include "em_leuart.h"

#define Debug				false
#define LEUART_LOOPBACK		false
#define BLE_Program			true
#define BLE_Factory_Reset	false

#define ULFRCO_FREQ			850	// Hz

// LEUART TX PD4
#define LEUART_TX_Port		gpioPortD
#define LEUART_TX_Pin		4
#define LEUART_TX_Mode		gpioModePushPull

// LEUART RX PD5
#define LEUART_RX_Port		gpioPortD
#define LEUART_RX_Pin		5
#define LEUART_RX_Mode		gpioModeInputPull

// Bluefruit CTS pins
#define BluefruitCTSPort	gpioPortD
#define BluefruitCTSPin		15
#define BluefruitCTSMode	gpioModePushPullDrive
#define BluefruitCTSDrive	gpioDriveModeLowest

#define END_OF_CMD_DELIMITER	'!'
#define END_OF_CMD_DELIM_SIZE	1

#define ADC_DMA_CH			2
#define ADC_DMA_PRI			0
#define ADC_DMA_ARB			dmaArbitrate1
#define ADC_DMA_BUF_SIZE	200
#define ADC_DMA_N_XFERS		ADC_DMA_BUF_SIZE - 1

#define LEUART_TX_DMA_CH		1
#define LEUART_TX_DMA_PRI		0
#define LEUART_TX_DMA_ARB		dmaArbitrate1
#define LEUART_TX_DMA_BUF_SIZE	64
#define LEUART_TX_DMA_N_XFERS	LEUART_TX_DMA_BUF_SIZE - 1

#define LEUART_RX_DMA_CH		0
#define LEUART_RX_DMA_PRI		1
#define LEUART_RX_DMA_ARB		dmaArbitrate1
#define LEUART_RX_DMA_BUF_SIZE	512
#define LEUART_RX_DMA_N_XFERS	LEUART_RX_DMA_BUF_SIZE - 1 - END_OF_CMD_DELIM_SIZE

#define SAMPLE_TIMER_PERIOD	4000	// ms

#define UPPER_TEMP_LIMIT	30		// C
#define LOWER_TEMP_LIMIT	15		// C
#define DEG_C_TO_TENTHS_C	10

#define LEUART_BAUD_RATE	9600
#define LEUART_STOP_BITS	leuartStopbits1
#define LEUART_DATA_BITS	leuartDatabits8
#define LEUART_PARITY		leuartNoParity

#define N_DMA_CH_IN_USE		3

// Bluefruit Commands
#define BLE_MAX_CONNECTION_INTERVAL
#define BLE_MIN_CONNECTION_INTERVAL
#define BLE_ADVERTISING_INTERVAL
#define BLE_ADVERTISING_TIMEOUT

#define Bluefruit_Mode_Change	"+++\n"

//#define BLE_Command_1			"AT+BLEPOWERLEVEL?\r"

// These are my settings obtained through steps 1 through 3 connecting
// to an HTC One M8 running Android 6.0
//#define BLE_Command_1			"AT+BLEPOWERLEVEL=-12\n"
//#define BLE_Command_3			"AT+GAPINTERVALS=50,107,187,50\n"

#define BLE_Command_2			"AT+HWMODELED=0\n"

// These settings are for Steps 5 and 6.
#define BLE_Command_1			"AT+BLEPOWERLEVEL=0\n"
// Step 5
//#define BLE_Command_3			"AT+GAPINTERVALS=20,150,100,30\n"
// Step 6
#define BLE_Command_3			"AT+GAPINTERVALS=20,100,250,30\n"

// Min Conn Interval,Max Conn Interval,Adv Interval,Adv Timeout (in milliseconds)
//#define BLE_Command_3			"AT+GAPINTERVALS=20,100,100,30\n"	// Default
//#define BLE_Command_3			"AT+GAPINTERVALS?\r"

#define BLE_Command_4			"AT+FACTORYRESET\n"

// With my custom settings, the average current is 1.59mA when disconnected and 2.09mA
// when connected.

// The average current when the Max Conn Interval is 150ms is 1.59mA when disconnected
// and 2.10mA when connected. My settings improve the average current by 0mA when
// disconnected and by 0.01mA when connected.

// The average current when the Max Adv Interval is 250ms is 1.79mA when disconnected
// and 2.10mA when connected. My settings improve the average current by 0.20mA when
// disconnected and by 0.01mA when connected.

#if ADC_DMA_BUF_SIZE > 1024 || LEUART_TX_DMA_BUF_SIZE > 1024 || LEUART_RX_DMA_BUF_SIZE > 1024
#error Too many samples for a single channel. Choose a number that is 1024 or less.
#endif

#if LEUART_BAUD_RATE > 9600
#error LEUART does not support baud rates greater than 9600.
#endif

#if Debug
Serial pc(USBTX, USBRX);
#endif

// Global Variables
DMA_CB_TypeDef ADC_Done_CB;
DMA_CB_TypeDef LEUART_TX_Done_CB;
DMA_CB_TypeDef LEUART_RX_Done_CB;
int16_t current_temp; // 0.1C units

// Buffers MUST be aligned on a 0x100 (256 byte) boundary to work properly
uint16_t ADC_Result_Buf[ADC_DMA_BUF_SIZE] __attribute__((aligned(256)));
uint8_t LEUART_RX_Buf[LEUART_RX_DMA_BUF_SIZE] __attribute__((aligned(256)));
uint8_t LEUART_TX_Buf[LEUART_TX_DMA_BUF_SIZE] __attribute__((aligned(256)));

// DMA descriptor block must be aligned on an 8 byte boundary
DMA_DESCRIPTOR_TypeDef DMA_DESCR_CTRL_BLOCK[N_DMA_CH_IN_USE] __attribute__((aligned(256)));

void GPIO_Initialize(void);
void LETIMER_Initialize(void);
void ADC_Initialize(void);
void ADC_DMA_Init(void);
void LEUART_Initialize(void);
void LEUART_DMA_Init(void);
void DMA_Initialize(void);
void ADC_DMA_Done_CB(unsigned int channel, bool primary, void *user);
void LEUART_TX_DMA_Done_CB(unsigned int channel, bool primary, void *user);
void LEUART_TX_Buffer(void);
void LEUART_TX_Wait(uint16_t tx_length);
void LEUART_RX_DMA_Done_CB(unsigned int channel, bool primary, void *user);
float convertToCelsius(int16_t adcSample);
void returnTemperature(void);
void BSP_TraceSwoSetup(void);

typedef struct {
	const char *str;
	void (*cmd_func)(void);
} BT_Cmd_t;

// Commands
const BT_Cmd_t CMD_TABLE[] = {
	{"RetTemp\0", returnTemperature},
	{'\0', 0}
};

#define MAX_CMD_SIZE 	sizeof(CMD_TABLE[0].str)

/**************************************************************************//**
 * @brief Initializes GPIOs needed for this project
 * @verbatim GPIO_Initialize(void); @endverbatim
 *****************************************************************************/
void GPIO_Initialize(void)
{
	// Enable clock to the GPIO module
	CMU_ClockEnable(cmuClock_GPIO, true);

	// Drive the Bluefruit CTS pin low
	GPIO_DriveModeSet(BluefruitCTSPort, BluefruitCTSDrive);
	GPIO_PinModeSet(BluefruitCTSPort, BluefruitCTSPin, BluefruitCTSMode, 0);

	// Set LEUART pins for high idle state on TX and pull-up on RX
	GPIO_PinModeSet(LEUART_TX_Port, LEUART_TX_Pin, LEUART_TX_Mode, 1);
	GPIO_PinModeSet(LEUART_RX_Port, LEUART_RX_Pin, LEUART_RX_Mode, 1);
}

/**************************************************************************//**
 * @brief Initializes LETIMER to run off of the ULFRCO clock.
 * @verbatim LETIMER_Initialize(void); @endverbatim
 *****************************************************************************/
void LETIMER_Initialize(void)
{
	LETIMER_Init_TypeDef LETIMER_InitValues = LETIMER_INIT_DEFAULT;

	// Turn on ULFRCO clock and set it as the source of the LFA branch
	CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);

	// Enable the clock to the LETIMER0
	CMU_ClockEnable(cmuClock_LETIMER0, true);

	// Enable the low energy core clock
	CMU_ClockEnable(cmuClock_CORELE, true);

	// Initialize LETIMER
	LETIMER_InitValues.enable = false;
	LETIMER_InitValues.comp0Top = true;

	LETIMER_CompareSet(LETIMER0, 0, (SAMPLE_TIMER_PERIOD * ULFRCO_FREQ) / 1000);

	// Pre-load CNT
	LETIMER0->CNT = LETIMER_CompareGet(LETIMER0, 0);

	// Enable underflow interrupt
	LETIMER_IntEnable(LETIMER0, LETIMER_IEN_UF);
	NVIC_EnableIRQ(LETIMER0_IRQn);

	LETIMER_Init(LETIMER0, &LETIMER_InitValues);
}

/**************************************************************************//**
 * @brief Initializes ADC
 * @verbatim ADC_Initialize(void); @endverbatim
 *****************************************************************************/
void ADC_Initialize(void)
{
	ADC_Init_TypeDef ADC_InitValues = ADC_INIT_DEFAULT;
	ADC_InitSingle_TypeDef ADC_Single = ADC_INITSINGLE_DEFAULT;

	CMU_ClockEnable(cmuClock_ADC0, true);

	// Set the ADC clock to HFRCO/4 = 1.75MHz
	ADC_InitValues.prescale = ADC_PrescaleCalc(1750000, 7000000);
	ADC_InitValues.timebase = ADC_TimebaseCalc(0);
	ADC_InitValues.lpfMode = adcLPFilterBypass;
	ADC_InitValues.warmUpMode = adcWarmupKeepADCWarm;
	ADC_Init(ADC0, &ADC_InitValues);

	// Configure the ADC for single channel scan of the temperature sensor
	// T_conv = (16+12)*(1/1.75M) = 16us
	// f_conv = (1/16us) = 62.5kHz
	ADC_Single.acqTime = adcAcqTime16;
	ADC_Single.reference = adcRef1V25;
	ADC_Single.input = adcSingleInpTemp;
	ADC_Single.rep = true;
	ADC_InitSingle(ADC0, &ADC_Single);

	// Initialize DMA for the ADC
	ADC_DMA_Init();
}

/**************************************************************************//**
 * @brief Initializes DMA for ADC
 * @verbatim ADC_DMA_Init(void); @endverbatim
 *****************************************************************************/
void ADC_DMA_Init(void)
{
	DMA_CfgChannel_TypeDef ADC_CfgChannel;
	DMA_CfgDescr_TypeDef ADC_Cfg_Descr;

	// Assign the call back and select the request source as ADC0 Single
	ADC_Done_CB.cbFunc = ADC_DMA_Done_CB;
	ADC_Done_CB.userPtr = NULL;
	ADC_CfgChannel.cb = &ADC_Done_CB;
	ADC_CfgChannel.select = DMA_CH_CTRL_SOURCESEL_ADC0 | DMA_CH_CTRL_SIGSEL_ADC0SINGLE;
	ADC_CfgChannel.enableInt = true;
	ADC_CfgChannel.highPri = ADC_DMA_PRI;
	DMA_CfgChannel(ADC_DMA_CH, &ADC_CfgChannel);

	// Configure the descriptor for word sized transfers
	ADC_Cfg_Descr.arbRate = ADC_DMA_ARB;
	ADC_Cfg_Descr.size = dmaDataSize2;
	ADC_Cfg_Descr.dstInc = dmaDataInc2;
	ADC_Cfg_Descr.srcInc = dmaDataIncNone;
	ADC_Cfg_Descr.hprot = 0;

	DMA_CfgDescr(ADC_DMA_CH, true, &ADC_Cfg_Descr);

	// Enable the ADC Channel interrupt
	DMA->IEN |= (1 << ADC_DMA_CH);
}

/**************************************************************************//**
 * @brief Initializes LEUART
 * @verbatim LEUART_Initialize(void); @endverbatim
 *****************************************************************************/
void LEUART_Initialize(void)
{
	LEUART_Init_TypeDef LEUART_InitVal = LEUART_INIT_DEFAULT;

	// Enable LFXO clock and set it as the source for the LFB tree
	CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
	CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);

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
 * @brief Initializes DMA
 * @verbatim DMA_Initialize(void); @endverbatim
 *****************************************************************************/
void DMA_Initialize(void)
{
	DMA_Init_TypeDef DMA_InitVal;

	// Assign the control block and initialize DMA
	DMA_InitVal.controlBlock = DMA_DESCR_CTRL_BLOCK;
	DMA_InitVal.hprot = 0;
	DMA_Init(&DMA_InitVal);

	NVIC_EnableIRQ(DMA_IRQn);
}

/**************************************************************************//**
 * @brief ADC DMA done callback
 * @verbatim ADC_DMA_Done_CB(unsigned int channel, bool primary, void *user);
 * @endverbatim
 *****************************************************************************/
void ADC_DMA_Done_CB(unsigned int channel, bool primary, void *user)
{
	uint32_t temp_sum = 0, transfer_size;
	float float_temp;

	// Stop conversions
	ADC0->CMD = ADC_CMD_SINGLESTOP;
	ADC0->CTRL &= (~_ADC_CTRL_WARMUPMODE_MASK);

	// Average the samples
	for (int i = 0; i < ADC_DMA_BUF_SIZE; i++)
		temp_sum += ADC_Result_Buf[i];
	temp_sum /= ADC_DMA_BUF_SIZE;
	float_temp = convertToCelsius(temp_sum);
	current_temp = (int16_t) (float_temp * DEG_C_TO_TENTHS_C);

	// Determine if the temperature is within range
	if (current_temp > UPPER_TEMP_LIMIT*DEG_C_TO_TENTHS_C) {
		transfer_size = sprintf((char *) LEUART_TX_Buf, "Temperature ABOVE set minimum = %d.%dC\n\r",
				current_temp / DEG_C_TO_TENTHS_C, abs(current_temp % DEG_C_TO_TENTHS_C));
		LEUART_TX_Buf[transfer_size] = '\0';
		LEUART_TX_Buffer();
 	}
	else if (current_temp < LOWER_TEMP_LIMIT*DEG_C_TO_TENTHS_C) {
		transfer_size = sprintf((char *) LEUART_TX_Buf, "Temperature BELOW set minimum = %d.%dC\n\r",
				current_temp / DEG_C_TO_TENTHS_C, abs(current_temp % DEG_C_TO_TENTHS_C));
		LEUART_TX_Buf[transfer_size] = '\0';
		LEUART_TX_Buffer();
	}

	// Go back to sleep
	unblockSleepMode(EM1);
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
			LEUART_TX_Buf, strlen((const char *) LEUART_TX_Buf) - 1);
}

/**************************************************************************//**
 * @brief Transmits the contents of the LEUART TX buffer but does not use DMA.
 * @verbatim LEUART_TX_Wait(void);
 * @endverbatim
 *****************************************************************************/
void LEUART_TX_Wait(uint16_t tx_length)
{
	uint16_t tx_idx = 0;

	// Enable the transmitter
	LEUART0->CMD |= LEUART_CMD_TXEN;

	// Wait for any pending previous write operation to have been completed
	// in low frequency domain
	while (LEUART0->SYNCBUSY & (LEUART_SYNCBUSY_TXDATA | LEUART_SYNCBUSY_CMD));

	while (tx_idx < tx_length)
	{
		LEUART0->TXDATA = LEUART_TX_Buf[tx_idx++];
		while (LEUART0->SYNCBUSY & (LEUART_SYNCBUSY_TXDATA | LEUART_SYNCBUSY_CMD));
		while (!(LEUART0->STATUS & LEUART_STATUS_TXC));
	}

	// Disable the transmitter and clear the TX shift register
	LEUART0->CMD |= LEUART_CMD_TXDIS | LEUART_CMD_CLEARTX;
	while (LEUART0->SYNCBUSY & LEUART_SYNCBUSY_CMD);

	// Clear Transmit Flags
	LEUART0->IFC |= LEUART_IFC_TXC;
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
 * @brief Converts the Leopard Gecko internal temperature sensor ADC values into
 * degrees C.
 * @verbatim convertToCelsius(void); @endverbatim
 *****************************************************************************/
float convertToCelsius(int16_t adcSample)
{
	float temp;
	// Factory calibration temperature from device information page.
	float cal_temp_0 = (float) ((DEVINFO->CAL & _DEVINFO_CAL_TEMP_MASK)
			>> _DEVINFO_CAL_TEMP_SHIFT);
	float cal_value_0 = (float) ((DEVINFO->ADC0CAL2 & _DEVINFO_ADC0CAL2_TEMP1V25_MASK)
			>> _DEVINFO_ADC0CAL2_TEMP1V25_SHIFT);

	// Temperature gradient (from datasheet)
	float t_grad = -6.27;

	temp = (cal_temp_0 - ((cal_value_0 - adcSample) / t_grad));
	return temp;
}

/**************************************************************************//**
 * @brief Prints the last measured temperature to serial.
 * @verbatim returnTemperature(void); @endverbatim
 *****************************************************************************/
void returnTemperature(void)
{
	uint32_t transfer_size = sprintf((char *) LEUART_TX_Buf, "%d.%dC\n\r", current_temp / DEG_C_TO_TENTHS_C, abs(current_temp % DEG_C_TO_TENTHS_C));

	// Set the last item in the transmit buffer to a null char so it knows when to stop transmitting
	LEUART_TX_Buf[transfer_size] = '\0';

	LEUART_TX_Buffer();
}

/**************************************************************************//**
 * @brief LETIMER0 IRQ Handler
 * @verbatim LETIMER0_IRQHandler(void); @endverbatim
 *****************************************************************************/
void LETIMER0_IRQHandler(void)
{
	uint32_t intflags = LETIMER_IntGet(LETIMER0);
	static bool first_int = false;

	LETIMER_IntClear(LETIMER0, intflags);
	if (intflags & LETIMER_IF_UF) {
		// Since there will be left-over data received from the LEUART after programming the
		// Bluefruit, the RX channel needs to be reset. So, on the first interrupt, clear
		// the RX DMA channel.
		if (!first_int) {
			DMA_ChannelEnable(LEUART_RX_DMA_CH, false);
			memset(LEUART_RX_Buf, 0, sizeof(LEUART_RX_Buf));
			DMA_ActivateBasic(LEUART_RX_DMA_CH, true, false, LEUART_RX_Buf, (void *) &LEUART0->RXDATA, LEUART_RX_DMA_N_XFERS);
			first_int = true;
		}

		ADC0->CTRL |= adcWarmupKeepADCWarm;
		DMA_ActivateBasic(ADC_DMA_CH, true, false, ADC_Result_Buf, (void *) &ADC0->SINGLEDATA, ADC_DMA_N_XFERS);
		ADC_Start(ADC0, adcStartSingle);
		blockSleepMode(EM1);
	}
}

/**************************************************************************//**
 * @brief LEUART0 IRQ Handler
 * @verbatim LEUART0_IRQHandler(void); @endverbatim
 *****************************************************************************/
void LEUART0_IRQHandler(void)
{
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
				//memset(LEUART_TX_Buf, 0, sizeof(LEUART_TX_Buf));
				transfer_size = sprintf((char *) LEUART_TX_Buf, "%s ERROR: Not valid input!\n\r", (char *) &LEUART_RX_Buf[0]);

				// Set the last item in the transmit buffer to a null char so it knows when to stop transmitting
				LEUART_TX_Buf[transfer_size] = '\0';

				LEUART_TX_Buffer();
			}
		}

		// Reset the RX DMA channel
		transfer_size = LEUART_RX_DMA_N_XFERS - ((DMA_DESCR_CTRL_BLOCK[LEUART_RX_DMA_CH].CTRL & 0x3FF0) >> 4);
		memset(LEUART_RX_Buf, 0, transfer_size);
		LEUART0->CMD |= (LEUART_CMD_CLEARRX | LEUART_CMD_RXEN);
		DMA_ActivateBasic(LEUART_RX_DMA_CH, true, false, LEUART_RX_Buf, (void *) &LEUART0->RXDATA, LEUART_RX_DMA_N_XFERS);
	}

	if (intflags & LEUART_IF_TXC) {
		// Disable the transmitter
		LEUART0->CMD |= LEUART_CMD_TXDIS;

		// Disable TXDMA wakeup
		LEUART0->CTRL &= ~LEUART_CTRL_TXDMAWU;

		// Disable the TXC interrupt
		LEUART_IntDisable(LEUART0, LEUART_IEN_TXC);
	}
}

/**************************************************************************//**
 * @brief Configure trace output for energyAware Profiler
 * @note  Enabling trace will add 80uA current for the EFM32_Gxxx_STK.
 *        DK's needs to be initialized with SPI-mode:
 * @verbatim BSP_Init(BSP_INIT_DK_SPI); @endverbatim
 *****************************************************************************/
void BSP_TraceSwoSetup(void)
{
	/* Enable GPIO clock */
	CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;

	/* Enable Serial wire output pin */
	GPIO->ROUTE |= GPIO_ROUTE_SWOPEN;

	/* Set correct location */
	/* This location is valid for GG, LG and WG! */
	GPIO->ROUTE = (GPIO->ROUTE & ~(_GPIO_ROUTE_SWLOCATION_MASK)) | GPIO_ROUTE_SWLOCATION_LOC0;

	/* Enable output on correct pin. */
	/* This pin is valid for GG, LG and WG! */
	GPIO->P[5].MODEL &= ~(_GPIO_P_MODEL_MODE2_MASK);
	GPIO->P[5].MODEL |= GPIO_P_MODEL_MODE2_PUSHPULL;

	/* Enable debug clock AUXHFRCO */
	CMU->OSCENCMD = CMU_OSCENCMD_AUXHFRCOEN;

	/* Wait until clock is ready */
	while (!(CMU->STATUS & CMU_STATUS_AUXHFRCORDY)) ;

	/* Enable trace in core debug */
	CoreDebug->DHCSR |= CoreDebug_DHCSR_C_DEBUGEN_Msk;
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

	/* Enable PC and IRQ sampling output */
	DWT->CTRL = 0x400113FF;

	/* Set TPIU prescaler to 16. */
	TPI->ACPR = 15;

	/* Set protocol to NRZ */
	TPI->SPPR = 2;

	/* Disable continuous formatting */
	TPI->FFCR = 0x100;

	/* Unlock ITM and output data */
	ITM->LAR = 0xC5ACCE55;
	ITM->TCR = 0x10009;

	/* ITM Channel 0 is used for UART output */
	ITM->TER |= (1UL << 0);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
	/* Chip errata */
	CHIP_Init();
#if Debug
	BSP_TraceSwoSetup();
	blockSleepMode(EM1);
#else
	blockSleepMode(EM2);
#endif

	// Ensure the correct HFRCO band and clock source is set
	CMU_HFRCOBandSet(cmuHFRCOBand_7MHz);
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);

	// Turn off HFXO if it was on
	CMU_OscillatorEnable(cmuOsc_HFXO, false, false);

	GPIO_Initialize();
	LETIMER_Initialize();
	DMA_Initialize();
	ADC_Initialize();
	LEUART_Initialize();

#if BLE_Program
	// Disable the RX DMA channel
	DMA_ChannelEnable(LEUART_RX_DMA_CH, false);

	// Place the device into CMD mode
	// Subtract 1 to account for the null character the compiler places at the end of each string
	memcpy(LEUART_TX_Buf, Bluefruit_Mode_Change, sizeof(Bluefruit_Mode_Change));
	LEUART_TX_Wait(sizeof(Bluefruit_Mode_Change) - 1);

#if BLE_Factory_Reset
	// For when you've jacked everything up
	memcpy(LEUART_TX_Buf, BLE_Command_4, sizeof(BLE_Command_4));
	LEUART_TX_Wait(sizeof(BLE_Command_4) - 1);
#else
	// Set the TX power
	memcpy(LEUART_TX_Buf, BLE_Command_1, sizeof(BLE_Command_1));
	LEUART_TX_Wait(sizeof(BLE_Command_1) - 1);

	// Set the hardware LED state
	memcpy(LEUART_TX_Buf, BLE_Command_2, sizeof(BLE_Command_2));
	LEUART_TX_Wait(sizeof(BLE_Command_2) - 1);

	// Set the GAP intervals
	memcpy(LEUART_TX_Buf, BLE_Command_3, sizeof(BLE_Command_3));
	LEUART_TX_Wait(sizeof(BLE_Command_3) - 1);
#endif

	// Back to UART mode
	memcpy(LEUART_TX_Buf, Bluefruit_Mode_Change, sizeof(Bluefruit_Mode_Change));
	LEUART_TX_Wait(sizeof(Bluefruit_Mode_Change) - 1);

	DMA_ActivateBasic(LEUART_RX_DMA_CH, true, false, LEUART_RX_Buf, (void *) &LEUART0->RXDATA, LEUART_RX_DMA_N_XFERS);
#endif

	// Start the LETIMER
	LETIMER_Enable(LETIMER0, true);

	/* Infinite loop */
	while (1) {
		sleep();
	}
}
