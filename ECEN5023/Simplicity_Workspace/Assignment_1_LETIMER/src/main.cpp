/***************************************************************************//**
 * @file main.cpp
 * @brief ECEN5023 - Assignment #1 - LETIMER LED blink
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * Blinks an LED at a period of 2 seconds where the LED is turned on for 2.5%
 * (50ms) of the timer period.
 *
 ******************************************************************************/

#include "mbed.h"
#include "em_chip.h"
#include "sleepmodes.h"
#include "em_letimer.h"

// Although the ULFRCO has a nominal frequency of 1kHz, I observed the actual
// frequency to be 850Hz
#define ULFRCO_FREQ				850	// Hz
//#define ULFRCO_FREQ				1000	// Hz

#define DesiredPeriod		2000	// ms
#define LEDOnDutyCycle		25		// 0.1%

#define MIN_POWER_MODE		1

uint16_t LEDOnPRDVal = (DesiredPeriod * LEDOnDutyCycle / 1000) * ULFRCO_FREQ / 1000;
uint16_t LEDOffPRDVal = (DesiredPeriod - LEDOnPRDVal) * ULFRCO_FREQ / 1000;

// LED0 on PE2
DigitalOut myLed0(LED0);

#if !MIN_POWER_MODE
Serial pc(USBTX, USBRX);
#endif

void LETIMER_Initialize(void);
void BSP_TraceSwoSetup(void);

/**************************************************************************//**
 * @brief Initializes LETIMER to run off of the ULFRCO clock.
 * @verbatim LETIMER_Initialize(void); @endverbatim
 *****************************************************************************/
void LETIMER_Initialize(void)
{
	LETIMER_Init_TypeDef LETIMER_InitValues;

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
	LETIMER_CompareSet(LETIMER0, 0, LEDOffPRDVal);

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
	if (intflags & LETIMER_IFS_UF) {

		if (LETIMER_CompareGet(LETIMER0, 0) == LEDOnPRDVal) {
			LETIMER_CompareSet(LETIMER0, 0, LEDOffPRDVal);
			myLed0 = 1;
		}
		else {
			LETIMER_CompareSet(LETIMER0, 0, LEDOnPRDVal);
			myLed0 = 0;
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
 * @brief main
 * @verbatim main(void); @endverbatim
 *****************************************************************************/
int main (void)
{
	CHIP_Init();
#if !MIN_POWER_MODE
	BSP_TraceSwoSetup();
#endif
	blockSleepMode(EM3);
	myLed0 = 0;

	LETIMER_Initialize();

	while(1) {
		sleep();
	}
}
