
#include <stdint.h>

#ifdef FRDM
#include "core_cm0plus.h"
#include "MKL25Z4.h"

#define KL25Z_XTAL0_CLK_HZ	8000000

static void frdm_init(void);
#endif

void proc_init(void)
{
#ifdef FRDM
	frdm_init();
#endif


}

#ifdef FRDM
static void frdm_init(void)
{
	// External reference clock enable
	OSC0->CR |= OSC_CR_ERCLKEN(1);

	MCG->C2 |= 0x00 |
			MCG_C2_LOCRE0(1) |	// Loss of clock reset enabled
			MCG_C2_RANGE0(1);	// Select Range 1 for clock <= 8MHz

	// Wait for the enabled external clock to stablize
	while (!(MCG->S & MCG_S_OSCINIT0_MASK)) {}

	// Switch to the external clock
	MCG->C1 &= ~MCG_C1_IREFS_MASK;

	// Wait for the clock switch
	while ((MCG->S & (MCG_S_IREFST_MASK | MCG_S_CLKST_MASK)) !=
			(MCG_S_CLKST(2) | MCG_S_IREFST(0))) { }

	// Disable the PLL
	MCG->C6 &= ~MCG_C6_PLL_MASK;

	// Configure the PLL
	// Divide down 8MHz to 4MHz-2MHz, so divider of 2
	MCG->C5 |= MCG_C5_PRDIV0(1);
	// VCO Output is now 4Mhz, multiply by 24. Sets the MCGOUTCLK to 96MHz
	MCG->C6 |= MCG_C6_VDIV0(0);

	// Enable the PLL
	MCG->C5 |= MCG_C5_PLLCLKEN0(1);
	// Wait for lock
	while (!(MCG->S & MCG_S_LOCK0_MASK)) {}

	// Switch the PLL source to PLL
	MCG->C6 |= MCG_C6_PLLS_MASK;
	while (!(MCG->S & MCG_C6_PLLS_MASK)) {}

	// Finally, switch the system clock to the PLL
	MCG->C1 = (MCG->C1 & ~MCG_C1_CLKST_MASK) | MCG_C1_CLKST(3);
	while ((MCG-S & MCG_S_CLKST_MASK) != MCG_C1_CLKST(3)) {}
}
#endif
