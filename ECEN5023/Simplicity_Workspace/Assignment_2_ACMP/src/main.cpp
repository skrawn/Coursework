/**************************************************************************//**
 * @file main.cpp
 * @brief Assignment 2 - ACMP
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

#include "mbed.h"
#include "em_chip.h"
#include "em_acmp.h"
#include "em_gpio.h"
#include "em_dac.h"
#include "sleepmodes.h"
#include "em_letimer.h"

#define ULFRCO_FREQ			850	// Hz

#define LED0PinPort					gpioPortE
#define LED0PinPin					2
#define LED0PinMode					gpioModePushPullDrive
#define LED0DriveMode				gpioDriveModeLowest

#define LES_LIGHT_EXCITE_Port		gpioPortD
#define LES_LIGHT_EXCITE_Pin		6
#define LES_LIGHT_EXCITE_Mode		gpioModePushPullDrive
#define LES_LIGHT_EXCITE_Drive		gpioDriveModeLowest

#define LES_LIGHT_SENSE_Port		gpioPortC
#define LES_LIGHT_SENSE_Pin			6
#define LES_LIGHT_SENSE_ACMP0_CH	acmpChannel6

#define ACMP_BIASPROG				0
#define ACMP_HYSTSEL				acmpHysteresisLevel7

#define DesiredPeriod				2000 // ms
#define SensorExciteTime			1 	 // ms
#define SleepTime					(DesiredPeriod - SensorExciteTime)

#define MIN_POWER_MODE		1

#if !MIN_POWER_MODE
Serial pc(USBTX, USBRX);
#endif

const uint16_t SleepTimePRDVal = (SleepTime * ULFRCO_FREQ) / 1000;
const uint16_t SensorExcitePRDVal = ((SensorExciteTime * ULFRCO_FREQ) / 1000) < 1 ? 1 : ((SensorExciteTime * ULFRCO_FREQ) / 1000);
const uint16_t DAC_Dark_Threshold = 2293;	// About 0.7V with 1.25V reference

void GPIO_Initialize(void);
void LETIMER_Initialize(void);
void ACMP0_Initialize(void);
void DAC0_Initialize(void);
void BSP_TraceSwoSetup(void);

/**************************************************************************//**
 * @brief Initializes GPIOs needed for this project
 * @verbatim GPIO_Initialize(void); @endverbatim
 *****************************************************************************/
void GPIO_Initialize(void)
{
	// Enable clock to the GPIO module
	CMU_ClockEnable(cmuClock_GPIO, true);

	GPIO_DriveModeSet(LES_LIGHT_EXCITE_Port, LES_LIGHT_EXCITE_Drive);
	GPIO_PinModeSet(LES_LIGHT_EXCITE_Port, LES_LIGHT_EXCITE_Pin, LES_LIGHT_EXCITE_Mode, 0);

	GPIO_DriveModeSet(LED0PinPort, LED0DriveMode);
	GPIO_PinModeSet(LED0PinPort, LED0PinPin, LED0PinMode, 0);
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
	LETIMER_InitValues.enable = true;

	// Pre-load CNT
	LETIMER0->CNT = SleepTimePRDVal;

	// Enable underflow interrupt
	LETIMER_IntEnable(LETIMER0, LETIMER_IEN_UF);
	NVIC_EnableIRQ(LETIMER0_IRQn);

	LETIMER_Init(LETIMER0, &LETIMER_InitValues);

	// Resync the core to the peripheral clock.
	__DSB();
}

/**************************************************************************//**
 * @brief Initializes ACMP0.
 * @verbatim ACMP0_Initialize(void); @endverbatim
 *****************************************************************************/
void ACMP0_Initialize(void)
{
	ACMP_Init_TypeDef ACMP0_InitValues = ACMP_INIT_DEFAULT;

	// Enable the clock to ACMP0
	CMU_ClockEnable(cmuClock_ACMP0, true);

	// Configure ACMP for measurement
	ACMP0_InitValues.biasProg = ACMP_BIASPROG;
	ACMP0_InitValues.halfBias = true;
	ACMP0_InitValues.fullBias = false;
	ACMP0_InitValues.hysteresisLevel = ACMP_HYSTSEL;
	ACMP0_InitValues.inactiveValue = false;
	ACMP0_InitValues.enable = false;
	ACMP0_InitValues.lowPowerReferenceEnabled = false;
	ACMP0_InitValues.warmTime = acmpWarmTime128;

	// Interrupt on rising edge of the comparator.
	ACMP0_InitValues.interruptOnRisingEdge = true;

	ACMP_Init(ACMP0, &ACMP0_InitValues);
	ACMP0->CTRL |= ACMP_CTRL_MUXEN;

	// Set CH6 as the positive comparator input and DAC0 CH0 as the negative input
	ACMP_ChannelSet(ACMP0, acmpChannelDAC0Ch0, LES_LIGHT_SENSE_ACMP0_CH);

	// Enable ACMP0 edge interrupt
	ACMP_IntEnable(ACMP0, ACMP_IEN_EDGE);
}

/**************************************************************************//**
 * @brief Initializes DAC0.
 * @verbatim DAC0_Initialize(void); @endverbatim
 *****************************************************************************/
void DAC0_Initialize(void)
{
	DAC_Init_TypeDef DAC0_InitValues = DAC_INIT_DEFAULT;

	// Enable the clock to DAC0
	CMU_ClockEnable(cmuClock_DAC0, true);

	// Configure DAC0
	DAC0_InitValues.convMode = dacConvModeSampleOff;
	//DAC0_InitValues.convMode = dacConvModeSampleHold;
	DAC0_InitValues.reference = dacRef1V25;
	DAC0_InitValues.outMode = dacOutputADC;
	DAC0_InitValues.outEnablePRS = false;

	// Set the DAC clock to 14M / (2^4) = 875k
	DAC0_InitValues.prescale = 4;

	DAC_Init(DAC0, &DAC0_InitValues);

	// Lowest power mode
	DAC0->BIASPROG = DAC_BIASPROG_HALFBIAS;

	// Enable the CH1 conversion complete interrupt
	DAC_IntEnable(DAC0, DAC_IEN_CH0);

}

/**************************************************************************//**
 * @brief LETIMER0 IRQ Handler
 * @verbatim LETIMER0_IRQHandler(void); @endverbatim
 *****************************************************************************/
void LETIMER0_IRQHandler(void)
{
	uint32_t intflags = LETIMER_IntGet(LETIMER0);
	DAC_InitChannel_TypeDef DAC0_CH0_Init = DAC_INITCHANNEL_DEFAULT;

#if !MIN_POWER_MODE
	printf("Entered IRQ\r\n");
#endif
	LETIMER_IntClear(LETIMER0, intflags);

	// Underflow interrupt
	if (intflags & LETIMER_IF_UF) {

		if (!GPIO_PinOutGet(LES_LIGHT_EXCITE_Port, LES_LIGHT_EXCITE_Pin)) {
			// Excite the sensor
			GPIO_PinOutSet(LES_LIGHT_EXCITE_Port, LES_LIGHT_EXCITE_Pin);

			// Set the counter register to the excite time
			LETIMER0->CNT = SensorExcitePRDVal;
		}
		else
		{
			// Set the counter register to the sleep time
			LETIMER0->CNT = SleepTimePRDVal;

			// Start the DAC
			DAC0_CH0_Init.enable = true;
			DAC_InitChannel(DAC0, &DAC0_CH0_Init, 0);
			DAC_Channel0OutputSet(DAC0, DAC_Dark_Threshold);

			ACMP0->CTRL |= ACMP_CTRL_EN;

			// Wait for ACMP warmup to be complete
			while (!(ACMP0->IF & ACMP_IF_WARMUP)) {
			}

			//if (ACMP0->IF & ACMP_IF_EDGE) {
			if (ACMP0->STATUS & ACMP_STATUS_ACMPOUT) {
				// Light detected, turn off LED
				GPIO_PinOutClear(LED0PinPort, LED0PinPin);
			}
			else
				GPIO_PinOutSet(LED0PinPort, LED0PinPin);

			// Turn off comparator
			ACMP0->CTRL = ACMP0->CTRL & ~(ACMP_CTRL_EN);

			// Shut down the excitation signal
			GPIO_PinOutClear(LES_LIGHT_EXCITE_Port, LES_LIGHT_EXCITE_Pin);

			// Turn off DAC
			DAC0_CH0_Init.enable = false;
			DAC_InitChannel(DAC0, &DAC0_CH0_Init, 0);

			// Clear flags
			intflags = DAC_IntGet(DAC0);
			DAC_IntClear(DAC0, intflags);
			intflags = ACMP_IntGet(ACMP0);
			ACMP_IntClear(ACMP0, intflags);
		}
	}

	// Resync the core to the peripheral clock.
	__DSB();
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

#if !MIN_POWER_MODE
	blockSleepMode(EM1);
	BSP_TraceSwoSetup();
#else
	blockSleepMode(EM3);
#endif

	GPIO_Initialize();

	LETIMER_Initialize();
	ACMP0_Initialize();
	DAC0_Initialize();

	/* Infinite loop */
	while (1) {
		sleep();
	}
}
