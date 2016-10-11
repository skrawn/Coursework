
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "circ_buf.h"
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
static void memopts_profiling(void);
static void ftoa(float fl, char *output);
static void itoa(int integer, char *output, int base);

uint8_t buffer_5000[5000];
uint8_t dest_arr[5000];

void project_2_report(void)
{
	uint32_t start_time, end_time, time_diff;

	log_0((uint8_t *) "\nPART1: ftoa\n", sizeof("\nPART1: ftoa\n"));
	log_0((uint8_t *) "test cases: ", sizeof("test cases: "));

	float num = 1245.25;
	uint8_t float_string[40] = {0};
	my_ftoa(num, float_string);
	log_1((uint8_t *) "1245.25: ", sizeof("1245.25: "), (void *) &float_string, log_string_t);

	memset(float_string, 0, 40);	
	num = 34.56293; 
	my_ftoa(num, float_string);
	log_1((uint8_t *) "34.56293: ", sizeof("34.56293: "), (void *) &float_string, log_string_t);

	memset(float_string, 0, 40);	
	num = -128.8929;
	uart_wait_tx_buf_empty();
	start_time = profiler_get_time();
	my_ftoa(num, float_string);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "-128.8929: ", sizeof("-128.8929: "), (void *) &float_string, log_string_t);
	log_1((uint8_t *) "my_ftoa runtime for -128.8929: ", sizeof("my_ftoa runtime for -128.8929: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	memset(float_string, 0, 40);	
	start_time = profiler_get_time();
	sprintf((char *) float_string, "%f", num);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "sprintf float runtime for -128.8929: ", sizeof("sprintf float runtime for -128.8929: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	log_0((uint8_t *) "\nPART2: Circular Buffer Init Tests\n", sizeof("\nPART2: Circular Buffer Init Tests\n"));
	uart_wait_tx_buf_empty();	
#ifdef UNIT_TEST
	cb_ut_create();
	unit_test_run();
#else
	log_0((uint8_t *) "Unit tests not enabled. Skipping", sizeof("Unit tests not enabled. Skipping"));
#endif

	log_0((uint8_t *) "\nPART3: UART Circular Buffer\n", sizeof("\nPART3: UART Circular Buffer\n"));
	log_0((uint8_t *) "See https://www.youtube.com/watch?v=Tz-G2oc5RRU\n", 
		sizeof("See https://www.youtube.com/watch?v=Tz-G2oc5RRU\n"));
	uart_wait_tx_buf_empty();	

	log_0((uint8_t *) "\nPART4: Profiling\n", sizeof("\nPART4: Profiling\n"));
	memmove_profiling();
	memzero_profiling();
	reverse_profiling();
	xtox_profiling();
	memopts_profiling();
}

static void memmove_profiling(void)
{
	uint32_t start_time, end_time, time_diff;

	log_0((uint8_t *) "memmove", sizeof("memmove"));
	
	uart_wait_tx_buf_empty();
	start_time = profiler_get_time();
	memmove(buffer_5000, dest_arr, 10);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "memmove 10 bytes: ", sizeof("memmove 10 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	memmove(buffer_5000, dest_arr, 100);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "memmove 100 bytes: ", sizeof("memmove 100 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	memmove(buffer_5000, dest_arr, 1000);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "memmove 1000 bytes: ", sizeof("memmove 1000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	memmove(buffer_5000, dest_arr, sizeof(buffer_5000));
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "memmove 5000 bytes: ", sizeof("memmove 5000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	log_0((uint8_t *) "\nmy_memmove", sizeof("\n my_memmove"));
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_memmove(buffer_5000, dest_arr, 10);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_memmove 10 bytes: ", sizeof("my_memmove 10 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_memmove(buffer_5000, dest_arr, 100);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_memmove 100 bytes: ", sizeof("my_memmove 100 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_memmove(buffer_5000, dest_arr, 1000);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_memmove 1000 bytes: ", sizeof("my_memmove 1000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_memmove(buffer_5000, dest_arr, sizeof(buffer_5000));
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_memmove 5000 bytes: ", sizeof("my_memmove 5000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();
}

static void memzero_profiling(void)
{
	uint32_t start_time, end_time, time_diff;

	log_0((uint8_t *) "\nmemset", sizeof("\nmemset"));
	
	uart_wait_tx_buf_empty();
	start_time = profiler_get_time();
	memset(buffer_5000, 0, 10);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "memset 10 bytes: ", sizeof("memset 10 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	memset(buffer_5000, 0, 100);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "memset 100 bytes: ", sizeof("memset 100 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	memset(buffer_5000, 0, 1000);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "memset 1000 bytes: ", sizeof("memset 1000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	memset(buffer_5000, 0, sizeof(buffer_5000));
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "memset 5000 bytes: ", sizeof("memset 5000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	log_0((uint8_t *) "\nmy_memzero", sizeof("\nmy_memzero"));
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_memzero(buffer_5000, 10);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_memzero 10 bytes: ", sizeof("my_memzero 10 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_memzero(buffer_5000, 100);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_memzero 100 bytes: ", sizeof("my_memzero 100 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_memzero(buffer_5000, 1000);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_memzero 1000 bytes: ", sizeof("my_memzero 1000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_memzero(buffer_5000, sizeof(buffer_5000));
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_memzero 5000 bytes: ", sizeof("my_memzero 5000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();
}

static void reverse_profiling(void)
{
	uint32_t start_time, end_time, time_diff;

	log_0((uint8_t *) "\nreverse", sizeof("\nreverse"));
	
	uart_wait_tx_buf_empty();
	/*start_time = profiler_get_time();
	reverse(buffer_5000, 10);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "reverse 10 bytes: ", sizeof("reverse 10 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	reverse(buffer_5000, 100);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "reverse 100 bytes: ", sizeof("reverse 100 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	reverse(buffer_5000, 1000);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "reverse 1000 bytes: ", sizeof("reverse 1000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	reverse(buffer_5000, sizeof(buffer_5000));
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "reverse 5000 bytes: ", sizeof("reverse 5000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();*/

	log_0((uint8_t *) "\nmy_reverse", sizeof("\nmy_reverse"));
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_reverse(buffer_5000, 10);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_reverse 10 bytes: ", sizeof("my_reverse 10 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_reverse(buffer_5000, 100);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_reverse 100 bytes: ", sizeof("my_reverse 100 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_reverse(buffer_5000, 1000);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_reverse 1000 bytes: ", sizeof("my_reverse 1000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	my_reverse(buffer_5000, sizeof(buffer_5000));
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_reverse 5000 bytes: ", sizeof("my_reverse 5000 bytes: "), 
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();
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
	uart_wait_tx_buf_empty();

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
	uart_wait_tx_buf_empty();
	
	start_time = profiler_get_time();
	my_ftoa(test_float, (uint8_t *) output_array);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_ftoa: ", sizeof("my_ftoa: "), (void *) &time_diff, log_uint32_t);	
	uart_wait_tx_buf_empty();
	
	start_time = profiler_get_time();
	output_int = my_atoi((uint8_t *) ascii_int);
	end_time = profiler_get_time();
	output_int++;	// Make the compiler stop complaining
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "my_atoi: ", sizeof("my_atoi: "), (void *) &time_diff, log_uint32_t);	
	uart_wait_tx_buf_empty();
}

static void memopts_profiling(void)
{
	uint32_t start_time, end_time, time_diff;
	uint8_t *malloc_ptr;

	log_0((uint8_t *) "\nmemory operations", sizeof("\nmemory operations"));
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	malloc_ptr = malloc(10);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "malloc - empty - 10: ", sizeof("malloc - empty - 10: "),
		(void *) &time_diff, log_uint32_t);
	free(malloc_ptr);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	malloc_ptr = malloc(100);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "malloc - empty - 100: ", sizeof("malloc - empty - 100: "),
		(void *) &time_diff, log_uint32_t);
	free(malloc_ptr);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	malloc_ptr = malloc(500);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "malloc - empty - 500: ", sizeof("malloc - empty - 500: "),
		(void *) &time_diff, log_uint32_t);
	free(malloc_ptr);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	malloc_ptr = malloc(1000);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "malloc - empty - 1000: ", sizeof("malloc - empty - 1000: "),
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	uint8_t *dummy1, *dummy2;

	// Allocate some more from heap on weird boundaries
	dummy1 = malloc(13); dummy2 = malloc(7);

	start_time = profiler_get_time();
	malloc_ptr = malloc(10);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "malloc - not empty - 10: ", sizeof("malloc - not empty - 10: "),
		(void *) &time_diff, log_uint32_t);
	free(malloc_ptr);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	malloc_ptr = malloc(100);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "malloc - not empty - 100: ", sizeof("malloc - not empty - 100: "),
		(void *) &time_diff, log_uint32_t);
	free(malloc_ptr);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	malloc_ptr = malloc(500);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "malloc - not empty - 500: ", sizeof("malloc - not empty - 500: "),
		(void *) &time_diff, log_uint32_t);
	free(malloc_ptr);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	malloc_ptr = malloc(1000);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "malloc - not empty - 1000: ", sizeof("malloc - not empty - 1000: "),
		(void *) &time_diff, log_uint32_t);	
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	free(malloc_ptr);	
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "free: ", sizeof("free: "),
		(void *) &time_diff, log_uint32_t);
	free(dummy1); free(dummy2);
	uart_wait_tx_buf_empty();

	// Create a circular buffer
	cb_t *circ_buf = cb_alloc(10);
	start_time = profiler_get_time();
	cb_push(circ_buf, 123);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "circ buf - add item: ", sizeof("circ buf - add item: "),
		(void *) &time_diff, log_uint32_t);
	uart_wait_tx_buf_empty();

	start_time = profiler_get_time();
	uint8_t data;
	cb_pop(circ_buf, &data);
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "circ buf - remove item: ", sizeof("circ buf - remove item: "),
		(void *) &time_diff, log_uint32_t);
	cb_destroy(circ_buf);
	uart_wait_tx_buf_empty();

#ifdef FRDM
	start_time = profiler_get_time();		
	log_0((uint8_t *) "log_0 test", sizeof("log_0 test"));
	end_time = profiler_get_time();
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "log_0 runtime: ", sizeof("log_0 runtime: "),
		(void *) &time_diff, log_uint32_t);	
	uart_wait_tx_buf_empty();
#else
	start_time = profiler_get_time();
	printf("20 character string!");
	end_time = profiler_get_time();
	printf("\n");
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "printf - 20 characters: ", sizeof("printf - 20 characters: "),
		(void *) &time_diff, log_uint32_t);	

	start_time = profiler_get_time();
	printf("%d", 219305325);
	end_time = profiler_get_time();
	printf("\n");
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "printf - 1 integer: ", sizeof("printf - 1 integer: "),
		(void *) &time_diff, log_uint32_t);	

	start_time = profiler_get_time();
	printf("%d %d", 219305325, 10294185);
	end_time = profiler_get_time();
	printf("\n");
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "printf - 2 integers: ", sizeof("printf - 2 integers: "),
		(void *) &time_diff, log_uint32_t);	

	start_time = profiler_get_time();
	printf("%d %d %d", 219305325, 10294185, 391591);
	end_time = profiler_get_time();
	printf("\n");
	time_diff = profiler_get_time_diff(start_time, end_time);
	log_1((uint8_t *) "printf - 3 integers: ", sizeof("printf - 3 integers: "),
		(void *) &time_diff, log_uint32_t);	
#endif	
}

// Library versions of these do not exist?
static void ftoa(float fl, char *output)
{
	sprintf(output, "%f", fl);
}

static void itoa(int integer, char *output, int base)
{
	sprintf(output, "%d", integer);
}


