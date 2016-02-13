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

#define ADC_DMA_CH			0

#define N_ADC_SAMPLES		200
#define SAMPLE_TIMER_PERIOD	2000	// ms
#define UPPER_TEMP_LIMIT	30		// C
#define LOWER_TEMP_LIMIT	15		// C

#define N_DMA_CH_IN_USE		1

#if N_ADC_SAMPLES > 512
#error Too many samples for a single channel. Choose a number that is 512 or less.
#endif

#if Debug
Serial pc(USBTX, USBRX);
#endif

// Global Variables
DMA_CB_TypeDef ADC_Done_CB;
// Buffer MUST be aligned on a 0x100 (256 byte) boundary to work properly
uint16_t ADC_Result_Buf[N_ADC_SAMPLES] __attribute__((aligned(256)));

DMA_DESCRIPTOR_TypeDef DMA_DESCR_CTRL_BLOCK[N_DMA_CH_IN_USE] __attribute__((aligned(8)));

void GPIO_Initialize(void);
void LETIMER_Initialize(void);
void ADC_Initialize(void);
void ADC_DMA_Done_CB(unsigned int channel, bool primary, void *user);
void DMA_Initialize(void);
void Set_HFRCO_17_5MHz(void);
float convertToCelsius(int16_t adcSample);
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
}

/**************************************************************************//**
 * @brief
 * @verbatim ADC_DMA_Done_CB(unsigned int channel, bool primary, void *user); @endverbatim
 *****************************************************************************/
void ADC_DMA_Done_CB(unsigned int channel, bool primary, void *user)
{
	uint32_t temp_sum = 0;
	float result;

	// Stop conversions
	ADC0->CMD = ADC_CMD_SINGLESTOP;
	ADC0->CTRL &= (~0x3);

	// Average the samples and determine if an LED needs to be lit
	for (int i = 0; i < N_ADC_SAMPLES; i++)
		temp_sum += ADC_Result_Buf[i];
	temp_sum /= N_ADC_SAMPLES;
	result = convertToCelsius(temp_sum);

	if (result > UPPER_TEMP_LIMIT)
		GPIO_PinOutSet(LED1PinPort, LED1PinPin);
	else
		GPIO_PinOutClear(LED1PinPort, LED1PinPin);

	if (result < LOWER_TEMP_LIMIT)
		GPIO_PinOutSet(LED0PinPort, LED0PinPin);
	else
		GPIO_PinOutClear(LED0PinPort, LED0PinPin);

	// Go back to sleep
	unblockSleepMode(EM1);
}

/**************************************************************************//**
 * @brief Initializes DMA
 * @verbatim DMA_Initialize(void); @endverbatim
 *****************************************************************************/
void DMA_Initialize(void)
{
	DMA_CfgChannel_TypeDef ADC_CfgChannel;
	DMA_CfgDescr_TypeDef ADC_Cfg_Descr;
	DMA_Init_TypeDef DMA_InitVal;

	// Assign the control block and initialize DMA
	DMA_InitVal.controlBlock = DMA_DESCR_CTRL_BLOCK;
	DMA_InitVal.hprot = 0;
	DMA_Init(&DMA_InitVal);

	// Assign the call back and select the request source as ADC0 Single
	ADC_Done_CB.cbFunc = ADC_DMA_Done_CB;
	ADC_Done_CB.userPtr = NULL;
	ADC_CfgChannel.cb = &ADC_Done_CB;
	ADC_CfgChannel.select = ((0b1000) << 16) | (0b0);
	ADC_CfgChannel.enableInt = true;
	ADC_CfgChannel.highPri = false;
	DMA_CfgChannel(ADC_DMA_CH, &ADC_CfgChannel);

	// Configure the descriptor for word sized transfers
	ADC_Cfg_Descr.arbRate = dmaArbitrate1;
	ADC_Cfg_Descr.size = dmaDataSize2;
	ADC_Cfg_Descr.dstInc = dmaDataInc2;
	ADC_Cfg_Descr.srcInc = dmaDataIncNone;
	ADC_Cfg_Descr.hprot = 0;

	DMA_CfgDescr(ADC_DMA_CH, true, &ADC_Cfg_Descr);

	DMA->IEN |= DMA_IEN_CH0DONE;
	NVIC_EnableIRQ(DMA_IRQn);
}

/**************************************************************************//**
 * @brief Tunes the HFRCO clock to 17.5MHz
 * @verbatim Set_HFRCO_17_5MHz(void); @endverbatim
 *****************************************************************************/
void Set_HFRCO_17_5MHz(void)
{
	// Ensure the correct HFRCO band and clock source is set
	CMU_HFRCOBandSet(cmuHFRCOBand_14MHz);
	//CMU_HFRCOBandSet(cmuHFRCOBand_7MHz);
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);

	// Turn off HFXO if it was on
	CMU_OscillatorEnable(cmuOsc_HFXO, false, false);

	// Increasing or decreasing the TUNING field in HFRCOCTRL can adjust
	// the HFRCO frequency up or down by 0.3%. To get a 17.5MHz HFRCO
	// clock, 14M + 83*0.003*14M = 17.486MHz
	uint32_t hfrcoctrl = CMU->HFRCOCTRL;
	uint8_t tune_value = ((uint8_t) (hfrcoctrl & 0xFF)) + 82;
	uint32_t tuned_hfrco = (hfrcoctrl & 0xFFFFFF00) + tune_value;
	CMU->HFRCOCTRL = tuned_hfrco;
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
 * @brief LETIMER0 IRQ Handler
 * @verbatim LETIMER0_IRQHandler(void); @endverbatim
 *****************************************************************************/
void LETIMER0_IRQHandler(void)
{
	uint32_t intflags = LETIMER_IntGet(LETIMER0);
	LETIMER_IntClear(LETIMER0, intflags);
	if (intflags & LETIMER_IF_UF) {
		ADC0->CTRL |= adcWarmupKeepADCWarm;
		DMA_ActivateBasic(ADC_DMA_CH, true, false, ADC_Result_Buf, (void *) &ADC0->SINGLEDATA, N_ADC_SAMPLES - 1);
		ADC_Start(ADC0, adcStartSingle);
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
	blockSleepMode(EM3);
#endif

	// Ensure the correct HFRCO band and clock source is set
	CMU_HFRCOBandSet(cmuHFRCOBand_7MHz);
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);

	// Turn off HFXO if it was on
	CMU_OscillatorEnable(cmuOsc_HFXO, false, false);

	//Set_HFRCO_17_5MHz();

	GPIO_Initialize();
	LETIMER_Initialize();
	ADC_Initialize();
	DMA_Initialize();

	GPIO_PinOutClear(LED0PinPort, LED0PinPin);
	GPIO_PinOutClear(LED1PinPort, LED1PinPin);

	LETIMER_Enable(LETIMER0, true);

	/* Infinite loop */
	while (1) {
		sleep();
	}
}
