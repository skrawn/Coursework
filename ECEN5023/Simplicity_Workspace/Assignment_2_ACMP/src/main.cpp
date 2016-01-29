/**************************************************************************//**
 * @file
 * @brief Empty Project
 * @author Energy Micro AS
 * @version 3.20.2
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs Software License Agreement. See 
 * "http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt"  
 * for details. Before using this software for any purpose, you must agree to the 
 * terms of that agreement.
 *
 ******************************************************************************/
#include "mbed.h"
#include "em_chip.h"
#include "em_acmp.h"
#include "em_gpio.h"
#include "sleepmodes.h"
#include "em_letimer.h"

#define ULFRCO_FREQ			850	// Hz

#define LED0PinPort			gpioPortE
#define LED0PinPin			2
#define LED0PinMode			gpioModePushPullDrive
#define LED0DriveMode		gpioDriveModeLowest
#define LED0ACMP0Output		1

#define LES_LIGHT_EXCITE_Port		gpioPortD
#define LES_LIGHT_EXCITE_Pin		6
#define LES_LIGHT_EXCITE_Mode		gpioModePushPullDrive
#define LES_LIGHT_EXCITE_Drive		gpioDriveModeLowest

#define LES_LIGHT_SENSE_Port		gpioPortC
#define LES_LIGHT_SENSE_Pin			6
#define LES_LIGHT_SENSE_ACMP0_CH	acmpChannel6

#define ACMP_BIASPROG				4
#define ACMP_HYSTSEL				acmpHysteresisLevel4
#define ACMP_VDDLEVEL				63

#define DesiredPeriod		2000 // ms

#define MIN_POWER_MODE		1

#if !MIN_POWER_MODE
Serial pc(USBTX, USBRX);
#endif

const uint16_t TimerPRDVal = (DesiredPeriod / 1000) * ULFRCO_FREQ;
const uint8_t Dark_Threshold = 13;		// About 0.68V
const uint8_t Light_Threshold = 51;		// About 2.67V

bool lightDetected = false;

void GPIO_Initialize(void);
void LETIMER_Initialize(void);
void ACMP0_Initialize(void);
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
	LETIMER_InitValues.comp0Top = true;

	// Set COMP0
	LETIMER_CompareSet(LETIMER0, 0, TimerPRDVal);

	// Pre-load CNT
	LETIMER0->CNT = 1;

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
	ACMP0_InitValues.hysteresisLevel = ACMP_HYSTSEL;
	ACMP0_InitValues.vddLevel = Dark_Threshold;
	ACMP0_InitValues.inactiveValue = true;
	ACMP0_InitValues.enable = false;
	ACMP0_InitValues.lowPowerReferenceEnabled = true;

	// Interrupt on BOTH rising and falling edges of the comparator.
	//ACMP0_InitValues.interruptOnRisingEdge = true;
	//ACMP0_InitValues.interruptOnFallingEdge = true;

	ACMP_Init(ACMP0, &ACMP0_InitValues);

	// Set CH6 as the positive comparator input and VDD as the negative input
	ACMP_ChannelSet(ACMP0,acmpChannelVDD, LES_LIGHT_SENSE_ACMP0_CH);

	// Set the comparator GPIO output on pin #1, PE2
	//ACMP_GPIOSetup(ACMP0, LED0ACMP0Output, true, false);

	// Enable the comparator
	ACMP0->CTRL |= ACMP_CTRL_EN;

	// Take measurement and decide what the LED should do and what the new
	// threshold should be

	// Disable ACMP interrupt
}

/**************************************************************************//**
 * @brief LETIMER0 IRQ Handler
 * @verbatim LETIMER0_IRQHandler(void); @endverbatim
 *****************************************************************************/
void LETIMER0_IRQHandler(void)
{
	uint32_t intflags = LETIMER_IntGet(LETIMER0);
#if !MIN_POWER_MODE
	printf("Entered IRQ\r\n");
#endif
	LETIMER_IntClear(LETIMER0, intflags);

	// Underflow interrupt
	if (intflags & LETIMER_IFS_UF) {
		// Check the comparator output state
		if (ACMP0->STATUS & ACMP_STATUS_ACMPOUT) {
			GPIO_PinOutClear(LED0PinPort, LED0PinPin);

		}
		else
			GPIO_PinOutSet(LED0PinPort, LED0PinPin);

		// Excite the sensor
		//GPIO_PinOutSet(LES_LIGHT_EXCITE_Port, LES_LIGHT_EXCITE_Pin);

		// Set up ACMP for a measurement

		// Enable ACMP interrupt
	}

	// Resync the core to the peripheral clock.
	__DSB();
}

/**************************************************************************//**
 * @brief ACMP0 IRQ Handler
 * @verbatim ACMP0_IRQHandler(void); @endverbatim
 *****************************************************************************/
void ACMP0_IRQHandler(void)
{

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

	// Excite the sensor
	GPIO_PinOutSet(LES_LIGHT_EXCITE_Port, LES_LIGHT_EXCITE_Pin);

	/* Infinite loop */
	while (1) {
		sleep();
	}
}
