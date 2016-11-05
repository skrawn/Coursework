
#include <stdint.h>
#include <stdio.h>

#include "proc_init.h"
#include "project_1.h"
#include "project_2.h"
#include "project_3.h"
#include "data.h"
#include "log.h"
#include "memory.h"
#include "uart.h"

#define RUN_PROJECT1_TESTS	(0)
#define RUN_PROJECT2_TESTS	(0)

#define PROJECT_1			(0)
#define PROJECT_2			(0)
#define PROJECT_3			(1)

#if FRDM
#define printf(...) (void)0
#endif

int main(void)
{
	proc_init();

#if RUN_PROJECT1_TESTS
	// Test itoa
	int32_t data = 4759392;
	uint8_t outstr[33] = {'\0'};	
	my_itoa(outstr, data, 16);	
	printf("%s = itoa(%d, 16)\n", outstr, data);
	my_itoa(outstr, data, 10);
	printf("%s = itoa(%d, 10)\n", outstr, data);
	my_itoa(outstr, data, 8);
	printf("%s = itoa(%d, 8)\n", outstr, data);
	my_itoa(outstr, data, 2);
	printf("%s = itoa(%d, 2)\n", outstr, data);

	data = -41835;
	my_itoa(outstr, data, 16);
	printf("%s = itoa(%d, 16)\n", outstr, data);
	my_itoa(outstr, data, 10);
	printf("%s = itoa(%d, 10)\n", outstr, data);
	my_itoa(outstr, data, 8);
	printf("%s = itoa(%d, 8)\n", outstr, data);
	my_itoa(outstr, data, 2);
	printf("%s = itoa(%d, 2)\n", outstr, data);

	// Test atoi
	uint8_t str[] = "32567\0";
	uint32_t integer = my_atoi(str);
	printf("%d = atoi(%s)\n", integer, str);
	
	uint8_t str2[] = "-256423465\0";
	uint32_t int2 = my_atoi(str2);
	printf("%d = atoi(%s)\n", int2, str2);

	// Test big to little
	data = 0xDEADBEEF;
	uint32_t big_data = big_to_little(data);
	printf("big to little: 0x%x -> 0x%x\n", data, big_data);
	uint32_t little_data = little_to_big(big_data);
	printf("little to big: 0x%x -> 0x%x\n", big_data, little_data);

	// Test memmove
	printf("\nMemory Operations\n");
	uint8_t data_arr[25];
	uint8_t dst_arr[25];
	uint8_t i;
	for (i = 0; i < sizeof(data_arr); i++) {
		data_arr[i] = i;
	}
	my_memmove(data_arr, dst_arr, sizeof(data_arr));
	printf("\nOriginal array:\n");
	dump_memory(data_arr, sizeof(data_arr));
	printf("\nMoved array:\n");
	dump_memory(dst_arr, sizeof(dst_arr));

	// Test memzero
	printf("\nBefore memzero:\n");
	dump_memory(dst_arr, sizeof(dst_arr));
	printf("\nAfter memzero:\n");
	my_memzero(dst_arr, sizeof(dst_arr));
	dump_memory(dst_arr, sizeof(dst_arr));

	// Test reverse
	printf("\nBefore reverse:\n");
	dump_memory(data_arr, sizeof(data_arr));
	printf("\nAfter reverse:\n");
	my_reverse(data_arr, sizeof(data_arr));
	dump_memory(data_arr, sizeof(data_arr));
	printf("\n");

#endif

#if RUN_PROJECT2_TESTS
	// Test ftoa
	float num1 = 34.56293;
	float num2 = -128.8929;
	uint8_t str_num1[12] = {0}, str_num2[12] = {0};
	my_ftoa(num1, str_num1);
	my_ftoa(num2, str_num2);
	printf("my_ftoa(%f) is %s\n", num1, str_num1);
	printf("my_ftoa(%f) is %s\n", num2, str_num2);

	// Test logging features
	uint8_t log_str_1[] = "Testing123, Serial Print Test, no params";
	uint8_t log_str_2[] = "This is an integer number: ";
	uint8_t log_str_3[] = "This is a floating point number: ";
	uint8_t temp8 = 200; 
	uint16_t temp16 = 4096; 
	uint32_t temp32 = 123456; 
	float tempf = 1543.321;
	log_0(log_str_1, sizeof(log_str_1));	
	log_1(log_str_2, sizeof(log_str_2), (void *) &temp8, log_uint8_t);
	log_1(log_str_2, sizeof(log_str_2), (void *) &temp16, log_uint16_t);
	log_1(log_str_2, sizeof(log_str_2), (void *) &temp32, log_uint32_t);
	log_1(log_str_3, sizeof(log_str_3), (void *) &tempf, log_float_t);	
#endif

#if PROJECT_1
	project_1_report();
#endif

#if PROJECT_2
	project_2_report();
#endif

#if PROJECT_3
	project_3_report();
#endif

#ifdef FRDM
	// Wait here for a bit if the UART is still transmitting
	uint32_t i;
	while (!uart_tx_buf_empty()) {
		// Also add some waiting time since uart_tx_buf_empty does disable/re-enable
		// interrupts a bunch of times.
		for (i = 0; i < 10000; i++) {}
	}
#endif

	return 0;
}
