/**
 * @file memory.c
 * @author Sean Donohue
 * @date 12 Sep 2016
 * @brief Custom implementation of various memory functions.
 */

#include <stdint.h>
#include <stdlib.h>

#include "memory.h"

#if defined(FRDM) && defined(USE_DMA)
#include "dma.h"

uint32_t zeros = 0x00000000;
#endif

int8_t my_memmove(uint8_t *src, uint8_t *dst, uint32_t length)
{
	uint32_t *src32, *dst32;

	if (src == NULL || dst == NULL || length == 0)
		return -1;

	// Check that the source and destination addresses are on 4-byte boundaries.
	// Do byte transfers until they align.
	while (length > 0 && (((uint32_t) src) % 4 > 0) && (((uint32_t) dst) % 4 > 0)) {
		*(dst++) = *(src++);
		length--;
	}

	// Do the remainder of the transfers on 4-byte boundaries
	dst32 = (uint32_t *) dst;
	src32 = (uint32_t *) src;

	while (length >= 4) {
		*(dst32++) = *(src32++);
		length -= 4;
	}

	// Finish any remaining transfers with byte transfers
	dst = (uint8_t *) dst32;
	src = (uint8_t *) src32;
	while (length > 0) {
		*(dst++) = *(src++);
		length--;
	}
		
	return 0;
}

int8_t my_memzero(uint8_t *src, uint32_t length)
{
	uint32_t *src32;

	if (src == NULL || length == 0)
		return -1;

	// Check that the source and destination addresses are on 4-byte boundaries.
	// Do byte transfers until they align.
	while (length > 0 && (((uint32_t) src) % 4 > 0)) {
		*(src++) = 0;
		length--;
	}

	src32 = (uint32_t *) src;

	// Do the remainder of the transfers on 4-byte boundaries
	while (length >= 4) {
		*(src32++) = 0;
		length -= 4;
	}

	// Finish any remaining transfers with byte transfers
	src = (uint8_t *) src32;
	while (length > 0) {
		*(src++) = 0;
		length--;
	}

	return 0;
}

int8_t my_memmove_dma(uint8_t *src, uint8_t *dst, uint32_t length)
{
#if defined(FRDM) && defined(USE_DMA)
	if (src == NULL || dst == NULL || length == 0)
		return -1;

	dma_desc_t desc;
	desc.src_addr = (uint32_t) src;
	desc.dst_addr = (uint32_t) dst;
	desc.num_bytes = length;
	desc.src_increment = DMA_INC_POST;
	desc.dst_increment = DMA_INC_POST;

	return dma_mem_transfer(&desc);
#else
	return my_memmove(src, dst, length);
#endif
}

int8_t my_memzero_dma(uint8_t *src, uint32_t length)
{
#if defined(FRDM) && defined(USE_DMA)
	if (src == NULL || length == 0)
		return -1;

	dma_desc_t desc;
	desc.src_addr = (uint32_t) &zeros;
	desc.dst_addr = (uint32_t) src;
	desc.num_bytes = length;
	desc.src_increment = DMA_INC_NONE;
	desc.dst_increment = DMA_INC_POST;

	return dma_mem_transfer(&desc);
#else
	return my_memzero(src, length);
#endif
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


