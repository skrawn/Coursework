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
#include "adc.h"
#include "bme280.h"
#include "config.h"
#include "dma.h"
#include "em_chip.h"
#include "em_device.h"
#include "em_gpio.h"
#include "em_prs.h"
#include "em_letimer.h"
#include "i2c_drv.h"
#include "leuart.h"
#include "mbed.h"
#include "sleepmodes.h"

// Bluefruit CTS pins
#define BluefruitCTSPort	gpioPortD
#define BluefruitCTSPin		15
#define BluefruitCTSMode	gpioModePushPullDrive
#define BluefruitCTSDrive	gpioDriveModeLowest

#define SAMPLE_TIMER_PERIOD	4000	// ms

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

#if Debug
Serial pc(USBTX, USBRX);
#endif

void GPIO_Initialize(void);
void LETIMER_Initialize(void);
void BSP_TraceSwoSetup(void);

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
}

/**************************************************************************//**
 * @brief Initializes GPIOs needed for this project
 * @verbatim GPIO_Initialize(void); @endverbatim
 *****************************************************************************/
void Clock_Setup(void)
{
	// Ensure the correct HFRCO band and clock source is set
	CMU_HFRCOBandSet(cmuHFRCOBand_7MHz);
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);

	// Turn off HFXO if it was on
	CMU_OscillatorEnable(cmuOsc_HFXO, false, false);

	// Set the HFRCO as the clock source of the HFPER tree
	CMU_ClockSelectSet(cmuClock_HFPER, cmuSelect_HFRCO);

	// Turn on the LFXO
	CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

	// Set LFXO as the source of the LFA and LFB tree.
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
	CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);

	// Enable the low energy core clock
	CMU_ClockEnable(cmuClock_CORELE, true);
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
	uint8_t temp_buf[15];
	uint32_t transfer_size = sprintf((char *) temp_buf, "%d.%dC\n\r", current_temp / DEG_C_TO_TENTHS_C, abs(current_temp % DEG_C_TO_TENTHS_C));

	// Set the last item in the transmit buffer to a null char so it knows when to stop transmitting
	temp_buf[transfer_size] = '\0';

	LEUART_Put_TX_Buffer(temp_buf, transfer_size + 1);
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
			LEUART_Reset_RX_Buffer();
			first_int = true;
		}

		ADC_Restart();
		blockSleepMode(EM1);
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

	GPIO_Initialize();
	LETIMER_Initialize();
	DMA_Initialize();
	ADC_Initialize();
	LEUART_Initialize();

	I2C_Initialize();
	BME280_Init();

#if BLE_Program
	// Disable the RX DMA channel
	DMA_ChannelEnable(LEUART_RX_DMA_CH, false);

	// Place the device into CMD mode
	// Subtract 1 to account for the null character the compiler places at the end of each string
	LEUART_Put_TX_Buffer((uint8_t *) Bluefruit_Mode_Change, sizeof(Bluefruit_Mode_Change));
	LEUART_TX_Wait(sizeof(Bluefruit_Mode_Change) - 1);

#if BLE_Factory_Reset
	// For when you've jacked everything up
	LEUART_Put_TX_Buffer((uint8_t *) BLE_Command_4, sizeof(BLE_Command_4));
	LEUART_TX_Wait(sizeof(BLE_Command_4) - 1);
#else
	// Set the TX power
	//LEUART_Put_TX_Buffer((uint8_t *) BLE_Command_1, sizeof(BLE_Command_1));
	//LEUART_TX_Wait(sizeof(BLE_Command_1) - 1);

	// Set the hardware LED state
	LEUART_Put_TX_Buffer((uint8_t *) BLE_Command_2, sizeof(BLE_Command_2));
	LEUART_TX_Wait(sizeof(BLE_Command_2) - 1);

	// Set the GAP intervals
	//LEUART_Put_TX_Buffer((uint8_t *) BLE_Command_3, sizeof(BLE_Command_3));
	//LEUART_TX_Wait(sizeof(BLE_Command_3) - 1);
#endif

	// Back to UART mode
	LEUART_Put_TX_Buffer((uint8_t *) Bluefruit_Mode_Change, sizeof(Bluefruit_Mode_Change));
	LEUART_TX_Wait(sizeof(Bluefruit_Mode_Change) - 1);

	LEUART_Reset_RX_Buffer();
#endif

	// Start the LETIMER
	LETIMER_Enable(LETIMER0, true);

	/* Infinite loop */
	while (1) {
		sleep();
	}
}
