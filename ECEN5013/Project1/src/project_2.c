
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "log.h"
#include "memory.h"
#include "profiler.h"
#include "project_2.h"
#include "uart.h"

static void memmove_profiling(void);
static void memzero_profiling(void);
static void reverse_profiling(void);
static void xtox_profiling(void);
static void ftoa(float fl, char *output);
static void itoa(int integer, char *output, int base);
static void frdm_wait_tx_buf_empty(void);

uint8_t buffer_5000[5000];
uint8_t dest_arr[5000];

void project_2_report(void)
{
	log_0((uint8_t *) "PART4: Profiling\n", sizeof("PART4: Profiling\n"));
	memmove_profiling();
	memzero_profiling();
	reverse_profiling();
	xtox_profiling();
}

static void memmove_profiling(void)
{
	uint32_t start_time, end_time, time_diff;

	log_0((uint8_t *) "memmove", sizeof("memmove"));
	
	frdm_wait_tx_buf_empty();
	start_time = profiler_get_time();
	memmove(buffer_5000, dest_arr, 10);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "memmove 10 bytes: ", sizeof("memmove 10 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();

	start_time = profiler_get_time();
	memmove(buffer_5000, dest_arr, 100);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "memmove 100 bytes: ", sizeof("memmove 100 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();

	start_time = profiler_get_time();
	memmove(buffer_5000, dest_arr, 1000);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "memmove 1000 bytes: ", sizeof("memmove 1000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();

	start_time = profiler_get_time();
	memmove(buffer_5000, dest_arr, sizeof(buffer_5000));
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "memmove 5000 bytes: ", sizeof("memmove 5000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();

	log_0((uint8_t *) "\nmy_memmove", sizeof("\n my_memmove"));
	frdm_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_memmove(buffer_5000, dest_arr, 10);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_memmove 10 bytes: ", sizeof("my_memmove 10 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_memmove(buffer_5000, dest_arr, 100);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_memmove 100 bytes: ", sizeof("my_memmove 100 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_memmove(buffer_5000, dest_arr, 1000);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_memmove 1000 bytes: ", sizeof("my_memmove 1000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_memmove(buffer_5000, dest_arr, sizeof(buffer_5000));
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_memmove 5000 bytes: ", sizeof("my_memmove 5000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();
}

static void memzero_profiling(void)
{
	uint32_t start_time, end_time, time_diff;

	log_0((uint8_t *) "\nmemset", sizeof("\nmemset"));
	
	frdm_wait_tx_buf_empty();
	start_time = profiler_get_time();
	memset(buffer_5000, 0, 10);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "memset 10 bytes: ", sizeof("memset 10 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();

	start_time = profiler_get_time();
	memset(buffer_5000, 0, 100);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "memset 100 bytes: ", sizeof("memset 100 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();

	start_time = profiler_get_time();
	memset(buffer_5000, 0, 1000);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "memset 1000 bytes: ", sizeof("memset 1000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();

	start_time = profiler_get_time();
	memset(buffer_5000, 0, sizeof(buffer_5000));
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "memset 5000 bytes: ", sizeof("memset 5000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();

	log_0((uint8_t *) "\nmy_memzero", sizeof("\nmy_memzero"));
	frdm_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_memzero(buffer_5000, 10);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_memzero 10 bytes: ", sizeof("my_memzero 10 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_memzero(buffer_5000, 100);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_memzero 100 bytes: ", sizeof("my_memzero 100 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_memzero(buffer_5000, 1000);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_memzero 1000 bytes: ", sizeof("my_memzero 1000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_memzero(buffer_5000, sizeof(buffer_5000));
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_memzero 5000 bytes: ", sizeof("my_memzero 5000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();
}

static void reverse_profiling(void)
{
	uint32_t start_time, end_time, time_diff;

	log_0((uint8_t *) "\nreverse", sizeof("\nreverse"));
	
	frdm_wait_tx_buf_empty();
	/*start_time = profiler_get_time();
	reverse(buffer_5000, 10);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "reverse 10 bytes: ", sizeof("reverse 10 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();

	start_time = profiler_get_time();
	reverse(buffer_5000, 100);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "reverse 100 bytes: ", sizeof("reverse 100 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();

	start_time = profiler_get_time();
	reverse(buffer_5000, 1000);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "reverse 1000 bytes: ", sizeof("reverse 1000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();

	start_time = profiler_get_time();
	reverse(buffer_5000, sizeof(buffer_5000));
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "reverse 5000 bytes: ", sizeof("reverse 5000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();*/

	log_0((uint8_t *) "\nmy_reverse", sizeof("\nmy_reverse"));
	frdm_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_reverse(buffer_5000, 10);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_reverse 10 bytes: ", sizeof("my_reverse 10 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_reverse(buffer_5000, 100);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_reverse 100 bytes: ", sizeof("my_reverse 100 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_reverse(buffer_5000, 1000);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_reverse 1000 bytes: ", sizeof("my_reverse 1000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_reverse(buffer_5000, sizeof(buffer_5000));
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_reverse 5000 bytes: ", sizeof("my_reverse 5000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	frdm_wait_tx_buf_empty();
}

static void xtox_profiling(void)
{
	uint32_t start_time, end_time, time_diff;

	char output_array[32] = {0};
	char ascii_int[] = {"1840204584"};
	uint32_t test_int = 214859301;
	float test_float = 3.141591020;
	int32_t output_int;

	log_0((uint8_t *) " ", 1);
#ifndef FRDM
	start_time = profiler_get_time();
	itoa(test_int, output_array, 10);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "itoa: ", sizeof("itoa: "), (void *) &time_diff, log_uint32_t);	
	
	start_time = profiler_get_time();
	ftoa(test_float, output_array);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "ftoa: ", sizeof("ftoa: "), (void *) &time_diff, log_uint32_t);	
	
	start_time = profiler_get_time();
	output_int = atoi(ascii_int);
	end_time = profiler_get_time();
	output_int++;	// Make the compiler stop complaining
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "atoi: ", sizeof("atoi: "), (void *) &time_diff, log_uint32_t);	
#endif

	start_time = profiler_get_time();
	my_itoa((uint8_t *) output_array, test_int, 10);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_itoa: ", sizeof("my_itoa: "), (void *) &time_diff, log_uint32_t);	
	
	start_time = profiler_get_time();
	my_ftoa(test_float, output_array);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_ftoa: ", sizeof("my_ftoa: "), (void *) &time_diff, log_uint32_t);	
	
	start_time = profiler_get_time();
	output_int = my_atoi(ascii_int);
	end_time = profiler_get_time();
	output_int++;	// Make the compiler stop complaining
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_atoi: ", sizeof("my_atoi: "), (void *) &time_diff, log_uint32_t);	

}

static void ftoa(float fl, char *output)
{
	sprintf(output, "%f", fl);
}

static void itoa(int integer, char *output, int base)
{
	sprintf(output, "%d", integer);
}

// Wait for things to finish printing on FRDM board before 
// profiling so the time measurements are accurate.
static void frdm_wait_tx_buf_empty(void)
{
#ifdef FRDM
	uint32_t i;
	while (!uart_tx_buf_empty()) {
		for (i = 0; i < 10000; i++) {}
	}
#endif
}

