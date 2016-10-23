/**
 * @file project_1.c
 * @author Sean Donohue
 * @date 12 Sep 2016
 * @brief Output for project 1
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "memory.h"
#include "project_1.h"

void project_1_report(void)
{

	uint8_t array[32];
	uint8_t *aptr_1, *aptr_2, *aptr_3, i;

	aptr_1 = array;
	aptr_2 = array + 8;
	aptr_3 = array + 16;

	for (i = 0; i < 16; i++)
		*(aptr_1 + i) = 31 + i;

	my_memzero(aptr_3, sizeof(array) - (aptr_3 - aptr_1));
		
	my_memmove(aptr_1, aptr_3, 8);
	
	my_memmove(aptr_2, aptr_1, 16);

	my_reverse(aptr_1, 32);

	printf("\nProject 1 Output:\n");
	dump_memory(aptr_1, sizeof(array));
	printf("\n");
}
