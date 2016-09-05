
#include <stdint.h>
#include <stdio.h>

#include "project_1.h"
#include "data.h"

#define PROJECT_1 (1)

int main(void)
{
	// Test itoa
	

	// Test atoi
	uint8_t str[] = "32567\0";
	uint32_t integer = my_atoi(str);
	printf("%d = atoi(%s)\n", integer, str);
	
	uint8_t str2[] = "-256423465\0";
	uint32_t int2 = my_atoi(str2);
	printf("%d = atoi(%s)\n", int2, str2);
#if PROJECT_1
	project_1_report();
#endif

	return 0;
}
