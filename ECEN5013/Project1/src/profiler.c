
#include "profiler.h"

#ifdef FRDM
	#include "MKL25Z4.h"
#else
	#include <time.h>
#endif

#define PROFILER_CH		0
#define NSEC_PER_USEC 	1000
#define TIMER_FREQUENCY	1000000

void profiler_init(void)
{
#ifdef FRDM
	// Need to initialize TPM1 for a 1usec counter
	// Ungate the clock for TPM1
	SIM->SCGC6 |= SIM_SCGC6_TPM1(1);
	
	TPM1->SC = TPM_SC_CPWMS(1) |	// Upcounting
			TPM_SC_PS(5);			// Prescaler of 32, 48M/32 = 1.5MHz

	// Set the period
	TPM1->MOD = (1500000 / TIMER_FREQUENCY);

	// Configure as simply a free-running counter
	TPM1->CONTROLS[PROFILER_CH].CnSC = 0x00 |
			TPM_CnSC_MSB(1) |
			TPM_CnSC_MSA(0);

	// Start the timer
	TPM1->SC = (TPM1->SC & ~TPM_SC_CMOD_MASK) | TPM_SC_CMOD(1);
#endif
}

uint32_t profiler_get_time(void)
{
// FRDM uses a hardware timer for profiling. BBB and host linux system
// can use the time functions
#ifdef FRDM
	return (TPM1->CNT & 0xFFFF);
#else
	struct timespec time;

	clock_gettime(CLOCK_REALTIME, &time);
	
	// Return time in microseconds
	return (time.tv_nsec / NSEC_PER_USEC);
#endif

}

uint32_t profiler_get_time_diff(uint32_t start, uint32_t end)
{
	// Handle wrap-around
	if (end > start) {
		// FRDM uses a 16-bit timer
#ifdef FRDM
		return (UINT16_MAX - start) + end;
#else
		return (UINT32_MAX - start) + end;
#endif
	}
	else
		return end-start;
}