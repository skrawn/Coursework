/**
 * @file memory.h
 * @author Sean Donohue
 * @date 12 Sep 2016
 * @brief Custom implementation of various memory functions.
 */

#include <stdint.h>

/**
 * @brief Moves a section of memory from the source to the destination address.
 * @param src Pointer to the start of the memory section to be copied.
 * @param dst Pointer to the start of the memory section to be copied to.
 * @param length Length of the memory section to be copied.
 * @return Returns 0 if successful.
 */
int8_t my_memmove(uint8_t *src, uint8_t *dst, uint32_t length);

/**
 * @brief Moves a section of memory from the source to the destination address with DMA
 * @param src Pointer to the start of the memory section to be copied.
 * @param dst Pointer to the start of the memory section to be copied to.
 * @param length Length of the memory section to be copied.
 * @return Returns 0 if successful.
 */
int8_t my_memmove_dma(uint8_t *src, uint8_t *dst, uint32_t length);

/**
 * @brief Zeroes a section of memory of the specified length.
 * @param src Pointer to the start of the memory section to be zeroed.
 * @param length Length of the memory section to be zeroed.
 * @return Returns 0 if successful.
 */
int8_t my_memzero(uint8_t *src, uint32_t length);

/**
 * @brief Zeroes a section of memory of the specified length with DMA
 * @param src Pointer to the start of the memory section to be zeroed.
 * @param length Length of the memory section to be zeroed.
 * @return Returns 0 if successful.
 */
int8_t my_memzero_dma(uint8_t *src, uint32_t length);

/**
 * @brief Reverses a section of memory of the specified length.
 * @param src Pointer to the beginning of memory to be reversed.
 * @param length Length of the memory section to be reversed;
 * @return Returns 0 if successful.
 */
int8_t my_reverse(uint8_t *src, uint32_t length);
