/**************************************************************************//**
 * @file main.cpp
 * @brief Assignment 3 - ADC
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
#include "sleepmodes.h"
#include "em_adc.h"
#include "em_chip.h"
#include "em_device.h"
#include "em_dma.h"
#include "em_gpio.h"
#include "em_letimer.h"

#define Debug				false

#define ULFRCO_FREQ			850	// Hz

// Cold temperature
#define LED0PinPort			gpioPortE
#define LED0PinPin			2
#define LED0PinMode			gpioModePushPullDrive
#define LED0DriveMode		gpioDriveModeLowest

// Hot temperature
#define LED1PinPort			gpioPortE
#define LED1PinPin			3
#define LED1PinMode			gpioModePushPullDrive
#define LED1DriveMode		gpioDriveModeLowest

#define N_ADC_SAMPLES		200
#define SAMPLE_TIMER_PERIOD	2000	// ms
#define UPPER_TEMP_LIMIT	30		// C
#define LOWER_TEMP_LIMIT	15		// C

#if Debug
Serial pc(USBTX, USBRX);
#endif

// Global Variables
DMA_CB_TypeDef ADC_Done_CB;

void GPIO_Initialize(void);
void LETIMER_Initialize(void);
void ADC_Initialize(void);
void DMA_Initialize(void);
void Set_HFRCO_17_5MHz(void);
void BSP_TraceSwoSetup(void);

/**************************************************************************//**
 * @brief Initializes GPIOs needed for this project
 * @verbatim GPIO_Initialize(void); @endverbatim
 *****************************************************************************/
void GPIO_Initialize(void)
{
	// Enable clock to the GPIO module
	CMU_ClockEnable(cmuClock_GPIO, true);

	GPIO_DriveModeSet(LED1PinPort, LED1DriveMode);
	GPIO_PinModeSet(LED1PinPort, LED1PinPin, LED1PinMode, 0);

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

	CMU_ClockEnable(cmuClock_ADC0, true);

	// Set the ADC clock to HFRCO/2 = 8.75MHz
	ADC_InitValues.prescale = 1;
	// 1us * (1/12.992) - 1 = ~12
	ADC_InitValues.timebase = 12;
	ADC_InitValues.lpfMode = adcLPFilterBypass;
}

/**************************************************************************//**
 * @brief Initializes DMA
 * @verbatim DMA_Initialize(void); @endverbatim
 *****************************************************************************/
void DMA_Initialize(void)
{

}

/**************************************************************************//**
 * @brief Tunes the HFRCO clock to 17.5MHz
 * @verbatim Set_HFRCO_17_5MHz(void); @endverbatim
 *****************************************************************************/
void Set_HFRCO_17_5MHz(void)
{
	// Increasing or decreasing the TUNING field in HFRCOCTRL can adjust
	// the HFRCO frequency up or down by 0.3%. To get a 13MHz HFRCO
	// clock, 14M - 24*0.003*14M = 12.992MHz
	uint32_t hfrcoctrl = CMU->HFRCOCTRL;
	uint8_t tune_value = ((uint8_t) (hfrcoctrl & 0xFF)) - 24;
	uint32_t tuned_hfrco = (hfrcoctrl & 0xFFFFFF00) + tune_value;
	CMU->HFRCOCTRL = tuned_hfrco;
}

/**************************************************************************//**
 * @brief LETIMER0 IRQ Handler
 * @verbatim LETIMER0_IRQHandler(void); @endverbatim
 *****************************************************************************/
void LETIMER0_IRQHandler(void)
{
	uint32_t intflags = LETIMER_IntGet(LETIMER0);

	if (intflags & LETIMER_IF_UF) {

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
	blockSleepMode(EM3);
#endif

	Set_HFRCO_13MHz();

	GPIO_Initialize();
	LETIMER_Initialize();
	ADC_Initialize();
	DMA_Initialize();

	LETIMER_Enable(LETIMER0, true);

	/* Infinite loop */
	while (1) {
		sleep();
	}
}
