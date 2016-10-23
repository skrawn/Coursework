/**
 * @file rgb_led.c
 * @author Sean Donohue
 * @date 12 Sep 2016
 * @brief Functions to control the RGB LED on the KL25Z
 */

#ifdef FRDM

#include "rgb_led.h"
#include "MKL25Z4.h"

#define PWM_FREQUENCY	100	//Hz
#define RED_TPM0_CH		0
#define GREEN_TPM0_CH	1
#define BLUE_TPM0_CH	1

void rgb_led_init(void)
{
	// Ungate the PORTB and PORTD clock
	SIM->SCGC5 |= SIM_SCGC5_PORTB(1) | SIM_SCGC5_PORTD(1);

	// Red = PTB18, Green = PTB19, Blue = PTD1
	// Select Alt Mode 3 for these pins for Red and Green
	PORTB->PCR[18] = (PORTB->PCR[18] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(3);
	PORTB->PCR[19] = (PORTB->PCR[19] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(3);
	// Select Alt Mode 4 for Blue
	PORTD->PCR[1] = (PORTD->PCR[1] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(4);

	// Blue is on TPM0 CH1
	// Ungate the clock for TPM0
	SIM->SCGC6 |= SIM_SCGC6_TPM0(1);

	// Select MCGPLLCLK/2 as the clock source
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

	TPM0->SC = TPM_SC_CPWMS(1) |	// Center-aligned PWM mode
			TPM_SC_PS(7);			// Prescaler of 128, 48M/128 = 325kHz

	// Set the period which is 2x MOD
	TPM0->MOD = (325000 / PWM_FREQUENCY) / 2;
	// 2x CnV register controls the duty cycle (pulse-width). CnV > MOD = 100% DC

	// Want to configure center-aligned PWMs with active-low. So, set the CnSC registers
	// based on Table 31-34.
	TPM0->CONTROLS[BLUE_TPM0_CH].CnSC = 0x00 |
				TPM_CnSC_MSB(1) |
				TPM_CnSC_MSA(0) |
				TPM_CnSC_ELSB(0) |
				TPM_CnSC_ELSA(1);

	TPM0->CONF = 0x00000000 |
			TPM_CONF_DBGMODE(3);		// Counter operates in debug mode

	// Red and Green are on TPM2
	// Ungate the clock
	SIM->SCGC6 |= SIM_SCGC6_TPM2(1);

	TPM2->SC = TPM_SC_CPWMS(1) |	// Center-aligned PWM mode
				TPM_SC_PS(7);			// Prescaler of 128, 48M/128 = 325kHz

	// Set the period which is 2x MOD
	TPM2->MOD = (325000 / PWM_FREQUENCY) / 2;

	// Configure center-aligned PWMs again
	TPM2->CONTROLS[RED_TPM0_CH].CnSC = 0x00 |
			TPM_CnSC_MSB(1) |
			TPM_CnSC_MSA(0) |
			TPM_CnSC_ELSB(0) |
			TPM_CnSC_ELSA(1);

	TPM2->CONTROLS[GREEN_TPM0_CH].CnSC = 0x00 |
				TPM_CnSC_MSB(1) |
				TPM_CnSC_MSA(0) |
				TPM_CnSC_ELSB(0) |
				TPM_CnSC_ELSA(1);

	// Start the timers
	TPM0->SC = (TPM0->SC & ~TPM_SC_CMOD_MASK) | TPM_SC_CMOD(1);
	TPM2->SC = (TPM2->SC & ~TPM_SC_CMOD_MASK) | TPM_SC_CMOD(1);
}

void rgb_led_set_brightness(uint8_t red, uint8_t green, uint8_t blue)
{
	uint16_t mod_val = TPM0->MOD;

	// Brightness can be from 0-255 for each color. This needs to be scaled
	// into a number for the CnV register.
	uint16_t red_cnv_val = mod_val * red / UINT8_MAX;
	uint16_t green_cnv_val = mod_val * green / UINT8_MAX;
	uint16_t blue_cnv_val = mod_val * blue / UINT8_MAX;

	// Update the CnV register with the new count
	TPM0->CONTROLS[BLUE_TPM0_CH].CnV = blue_cnv_val;
	TPM2->CONTROLS[RED_TPM0_CH].CnV = red_cnv_val;
	TPM2->CONTROLS[GREEN_TPM0_CH].CnV = green_cnv_val;
}

#endif
