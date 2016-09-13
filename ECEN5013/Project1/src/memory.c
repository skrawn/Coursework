
#include <stdint.h>
#include <stdlib.h>

#include "memory.h"

int8_t my_memmove(uint8_t *src, uint8_t *dst, uint32_t length)
{
	if (src == NULL || dst == NULL || length == 0)
		return -1;

	while (length > 0) {
		*(dst++) = *(src++);
		length--;
	}
		
	return 0;
}

int8_t my_memzero(uint8_t *src, uint32_t length)
{
	if (src == NULL || length == 0)
		return -1;

	while (length > 0) {
		*(src++) = 0;
		length--;
	}	

	return 0;
}

int8_t my_reverse(uint8_t *src, uint32_t length)
{
	uint8_t temp;
	uint8_t *left, *right;

	if (src == NULL || length == 0)
		return -1;
	
	left = src; right = (src + length - 1);

	length /= 2;
	while (length > 0) {
		temp = *left;
		*(left++) = *(right);
		*(right--) = temp;
		length--;
	}

	return 0;
}


