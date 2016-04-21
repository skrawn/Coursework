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
#include "capsense.h"
#include "config.h"
#include "dma.h"
#include "em_chip.h"
#include "em_device.h"
#include "em_gpio.h"
#include "em_prs.h"
#include "em_letimer.h"
#include "flash.h"
#include "hmc5883l.h"
#include "i2c_drv.h"
#include "leuart.h"
#include "mbed.h"
#include "mma8452q.h"
#include "sleepmodes.h"

#define Debug	(0)

#define VECTOR_SIZE (16+30)
uint32_t vectorTable[VECTOR_SIZE] __attribute__ ((aligned(256)));

// Bluefruit CTS pins
#define BluefruitCTSPort	gpioPortD
#define BluefruitCTSPin		15
#define BluefruitCTSMode	gpioModePushPullDrive
#define BluefruitCTSDrive	gpioDriveModeLowest

#define N_DMA_CH_IN_USE		3

// Bluefruit Commands
#define BLE_MAX_CONNECTION_INTERVAL
#define BLE_MIN_CONNECTION_INTERVAL
#define BLE_ADVERTISING_INTERVAL
#define BLE_ADVERTISING_TIMEOUT

#define Bluefruit_Mode_Change	"+++\n"

//#define BLE_Command_1			"AT+BLEPOWERLEVEL?\r"

#define BLE_Command_2			"AT+HWMODELED=0\n"
#define BLE_Command_1			"AT+BLEPOWERLEVEL=0\n"
#define BLE_Command_3			"AT+GAPINTERVALS=20,100,250,30\n"
#define BLE_Command_4			"AT+FACTORYRESET\n"

void GPIO_Initialize(void);
void moveInterruptVectorToRam(void);
void printData(void);
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
	//GPIO_DriveModeSet(BluefruitCTSPort, BluefruitCTSDrive);
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

	// Enable the AUXHFRCO clock and set it to 7MHz
	CMU_ClockEnable(cmuClock_AUX, true);
	CMU_AUXHFRCOBandSet(cmuAUXHFRCOBand_7MHz);

	// Turn on the LFXO
	CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

	// Set LFXO as the source of the LFA and LFB tree.
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
	CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);

	// Enable the low energy core clock
	CMU_ClockEnable(cmuClock_CORELE, true);
}

/**************************************************************************//**
 * @brief Moves the interrupt vector table to RAM
 * @verbatim moveInterruptVectorToRam(void); @endverbatim
 *****************************************************************************/
void moveInterruptVectorToRam(void)
{
  memcpy(vectorTable, (uint32_t*)SCB->VTOR, sizeof(uint32_t) * VECTOR_SIZE);
  SCB->VTOR = (uint32_t)vectorTable;
}

/**************************************************************************//**
 * @brief Prepares the data transmit data for the LEUART
 * @verbatim printData(void); @endverbatim
 *****************************************************************************/
void printData(void)
{
	uint8_t tx_buf[80] = {0};
	uint32_t tx_size = 0;

	// BME280 data
	/*tx_size = sprintf((char *) tx_buf, "Temperature: %d.%d C\r\nPressure: %d.%d inHg\r\nHumidity: %d.%d %%\r\n",
			BME280_Get_Temp() / 100, abs(BME280_Get_Temp() % 100), BME280_Get_Pres() / 100, BME280_Get_Pres() % 100,
			BME280_Get_Humidity() / 10, BME280_Get_Humidity() % 10);
	LEUART_Put_TX_Buffer(tx_buf, tx_size);*/

	// Accelerometer data
	/*tx_size = sprintf((char *) tx_buf, "X: %d.%02dg Y: %d.%02dg Z: %d.%02dg\r\n",
				MMA8452Q_GetXData() / 100, abs(MMA8452Q_GetXData() % 100), MMA8452Q_GetYData() / 100, abs(MMA8452Q_GetYData() % 100),
				MMA8452Q_GetZData() / 100, (MMA8452Q_GetZData() % 100));
	LEUART_Put_TX_Buffer(tx_buf, tx_size);*/

	// Magnetometer data
	/*tx_size = sprintf((char *) tx_buf, "X: %d.%03d G Y: %d.%03d G Z: %d.%03d G\r\n", HMC5883L_GetXData() / 1000,
			abs(HMC5883L_GetXData() % 1000), HMC5883L_GetYData() / 1000, abs(HMC5883L_GetYData() % 1000),
			HMC5883L_GetZData() / 1000, abs(HMC5883L_GetZData() % 1000));*/
	tx_size = sprintf((char *) tx_buf, "Heading: %d\r\n", HMC5883L_GetHeading());
	LEUART_Put_TX_Buffer(tx_buf, tx_size);
}

/**************************************************************************//**
 * @brief Initializes LETIMER to run off of the ULFRCO clock.
 * @verbatim LETIMER_Initialize(void); @endverbatim
 *****************************************************************************/
void LETIMER_Initialize(void)
{
	LETIMER_Init_TypeDef LETIMER_InitValues = LETIMER_INIT_DEFAULT;

	// Enable the clock to the LETIMER0
	CMU_ClockEnable(cmuClock_LETIMER0, true);

	// Initialize LETIMER
	LETIMER_InitValues.enable = false;
	LETIMER_InitValues.comp0Top = true;
	LETIMER_InitValues.repMode = letimerRepeatOneshot;

	LETIMER_CompareSet(LETIMER0, 0, (SAMPLE_TIMER_PERIOD * LFXO_FREQ) / 3000);
	LETIMER_RepeatSet(LETIMER0, 0, 3);

	// Pre-load CNT
	LETIMER0->CNT = LETIMER_CompareGet(LETIMER0, 0);

	// Enable underflow interrupt
	LETIMER_IntEnable(LETIMER0, LETIMER_IEN_REP0);
	NVIC_EnableIRQ(LETIMER0_IRQn);

	LETIMER_Init(LETIMER0, &LETIMER_InitValues);
}

/**************************************************************************//**
 * @brief LETIMER0 IRQ Handler
 * @verbatim LETIMER0_IRQHandler(void); @endverbatim
 *****************************************************************************/
void LETIMER0_IRQHandler(void)
{
	uint32_t intflags = LETIMER_IntGet(LETIMER0);
	static bool first_init = false;

	LETIMER_IntClear(LETIMER0, intflags);
	//if (intflags & LETIMER_IF_UF) {
	if (intflags & LETIMER_IF_REP0) {
		// Reload the REP0 register and restart the timer
		LETIMER_RepeatSet(LETIMER0, 0, 3);
		LETIMER_Enable(LETIMER0, true);

		// Since there will be left-over data received from the LEUART after programming the
		// Bluefruit, the RX channel needs to be reset. So, on the first interrupt, clear
		// the RX DMA channel.
		if (!first_init) {
			LEUART_Reset_RX_Buffer();
			first_init = true;
		}

		// Read the environmental data from the BME280
		BME280_Read_All();

		// Check the interrupt registers of the MMA8452Q
		uint8_t reg = MMA8452Q_GetPulseIntStatus();
		if (reg != 0)
		{
			while(1) {}
		}

		printData();
		LEUART_TX_Buffer();

		// Prepare for the next sample
		BME280_Set_Mode(BME280_MODE_FORCED);
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

void wait_ms(uint32_t time_ms)
{
	// 1 millisecond is approximately 500 ticks.
	// 8,589,934 is the largest number that can be used here (2^32/500)
	volatile uint32_t i = 0;
	uint32_t n_ticks;

	if (time_ms > 8589934)
		n_ticks = 8589934 * 500;
	else
		n_ticks = time_ms*500;

	while (i++ <= n_ticks)
	{ }
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

	Clock_Setup();
	moveInterruptVectorToRam();
	GPIO_Initialize();

	LETIMER_Initialize();
	DMA_Initialize();
	LEUART_Initialize();
	Flash_Init();
	Capsense_Init();

	// Enable the I2C devices before initializes the I2C bus. Wait 300ms for all
	// 3 devices to finish initialization.
	HMC5883L_Enable(true);
	MMA8452Q_Enable(true);
	BME280_Enable(true);

	wait_ms(300);

	I2C_Initialize();
	//BME280_Init();
	//MMA8452Q_Init();
	HMC5883L_Init();

	//MMA8452Q_Realign();

#if BLE_Program
	// Disable the RX DMA channel
	DMA_ChannelEnable(LEUART_RX_DMA_CH, false);

	// Place the device into CMD mode
	// Subtract 1 to account for the null character the compiler places at the end of each string
	LEUART_Put_TX_Buffer((uint8_t *) Bluefruit_Mode_Change, sizeof(Bluefruit_Mode_Change));
	LEUART_TX_Wait();

#if BLE_Factory_Reset
	// For when you've jacked everything up
	LEUART_Put_TX_Buffer((uint8_t *) BLE_Command_4, sizeof(BLE_Command_4));
	LEUART_TX_Wait();
#else
	// Set the TX power
	//LEUART_Put_TX_Buffer((uint8_t *) BLE_Command_1, sizeof(BLE_Command_1));
	//LEUART_TX_Wait(sizeof(BLE_Command_1) - 1);

	// Set the hardware LED state
	LEUART_Put_TX_Buffer((uint8_t *) BLE_Command_2, sizeof(BLE_Command_2));
	LEUART_TX_Wait();

	// Set the GAP intervals
	//LEUART_Put_TX_Buffer((uint8_t *) BLE_Command_3, sizeof(BLE_Command_3));
	//LEUART_TX_Wait();
#endif

	// Back to UART mode
	LEUART_Put_TX_Buffer((uint8_t *) Bluefruit_Mode_Change, sizeof(Bluefruit_Mode_Change));
	LEUART_TX_Wait();

	LEUART_Reset_RX_Buffer();
#endif

	// Start the LETIMER
	LETIMER_Enable(LETIMER0, true);

	/* Infinite loop */
	bool race_mode = true;
	while (1) {
		while (race_mode)
		{
			// See what the current active buffer is
			//active_buffer = LEUART_GetActiveBuffer();

			//MMA8452Q_ReadAll();
			while (!HMC5883L_DataReady()) {}
			HMC5883L_ReadAll();
			printData();

			LEUART_TX_Wait();
			wait_ms(200);

			// If the DMA channel is not active, transmit now
			/*if (!LEUART_TX_Active())
				LEUART_TX_Buffer();
			else
			{
				// If the DMA channel is active, wait for the LEUART driver
				// to start using the active buffer before continuing the
				// loop.
				while (LEUART_GetActiveBuffer() == active_buffer)
				{}
			}*/
		}
		sleep();
	}
}
