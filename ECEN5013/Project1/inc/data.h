
/**
 * @file data.h
 * @author Sean Donohue
 * @date 12 Sep 2016
 * @brief Custom implementation of various library functions.
 */

#include <stdint.h>

/**
 * @brief Converts an integer to a string in the specified base format
 * @param str Pointer to where the integer string is stored.
 * @param data Signed integer to create a string from
 * @param base Base of the number: 2, 8, 10, or 16
 * @return Returns 0 if successful
 */
int8_t *my_itoa(uint8_t *str, int32_t data, int32_t base);

/**
 * @brief Converts an integer string into a signed integer.
 * @param str Pointer to the integer string to convert.
 * @return Returns the integer of the input string or 0 if an error was found.
 */
int32_t my_atoi(uint8_t *str);

/**
 * @brief Converts floating point number into an ASCII string
 * @param fp Floating point number to conver
 * @param str Pointer string to place the result.
 * @return Returns a pointer to str.
 */
uint8_t *ftoa(float fp, uint8_t *str);

/**
 * @brief Prints a section of memory to the terminal.
 * @param start Pointer to the start of the memory section to be dumped.
 * @param length Length of the memory section to be dumped.
 * @return @c NULL is always returned
 */
void dump_memory(uint8_t *start, uint32_t length);

/**
 * @brief Converts a big-endian signed integer to a big-endian signed integer
 * @param data Big-endian integer to convert
 * @return Returns the little-endian signed integer.
 */
int32_t big_to_little(int32_t data);

/**
 * @brief Converts a little-endian signed integer to a big-endian signed integer
 * @param data Little-endian integer to convert
 * @return Returns the big-endian signed integer.
 */
int32_t little_to_big(int32_t data);

