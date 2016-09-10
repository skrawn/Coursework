
#include <stdint.h>
#include <stdio.h>

#include "project_1.h"
#include "data.h"

#define RUN_TESTS	(1)

#define PROJECT_1	(1)

#if FRDM
#define printf(...) (void)0
#endif

int main(void)
{
#if RUN_TESTS
	// Test itoa
	int32_t data = 4759392;
	uint8_t outstr[11] = {'\0'};	
	my_itoa(outstr, data, 16);	
	printf("itoa(%d, 16) = %s\n", data, outstr);

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

#endif

#if PROJECT_1
	project_1_report();
#endif

	return 0;
}
