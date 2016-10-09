
#include <stdint.h>
#include <stdio.h>

#include "proc_init.h"
#ifdef FRDM
// TODO Remove later
#include "uart.h"
#endif

#include "project_1.h"
#include "data.h"
#include "memory.h"

#define RUN_PROJECT1_TESTS	(0)
#define RUN_PROJECT2_TESTS	(1)

#define PROJECT_1			(0)
#define PROJECT_2			(1)

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
	ftoa(num1, str_num1);
	ftoa(num2, str_num2);
	printf("ftoa(%f) is %s\n", num1, str_num1);
	printf("ftoa(%f) is %s\n", num2, str_num2);

#ifdef FRDM
	uint8_t string[] = "Hello strang!\n";
	uart_put_tx_buf(string, sizeof(string));
	uint32_t i;
	while (1) {
		for (i = 0; i < 1000000; i++) {}
	}
#endif
#endif

#if PROJECT_1
	project_1_report();
#endif

#if PROJECT_2

#endif

	return 0;
}
