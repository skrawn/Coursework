
#include <stdbool.h>
#include <stdio.h>

#include "data.h"

#if FRDM
#define printf(...) (void)0
#endif

#define ASCII_TO_INT			0x30
#define INT_TO_HEX_LETTER		0x37

#define MAX_INT_CHAR_LEN		10
#define ERR_UNKNOWN				-1
#define ERR_NULLPTR				-2
#define ERR_INVINPUT			-3

#define DUMP_BYTES_PER_LINE     16

int8_t *my_itoa(uint8_t *str, int32_t data, int32_t base)
{
	bool isNeg = false, leading_zero = true;
	uint32_t i, mask;
	uint8_t hex_nib;

	// Check for non-null string and valid base
	if (str == NULL)
		return NULL;

	if (base != 2 && base != 8 && base != 10 && base != 16)
		return NULL;

	// Decimal number, check if negative
	if (base == 10 && data < 0)
		isNeg = true;

	if (base == 2) {

	}
	else if (base == 8) {
	
	}
	else if (base == 10) {
	
	}
	else {
		for (i = 7; i != 0; i--) {
			mask = (0xF << (i*4));
			hex_nib = (uint8_t) ((data & mask) >> (i*4));
			if (hex_nib == 0 && leading_zero)
			{
				// Do nothing	
			}
			else
			{
				leading_zero = false;
				if (hex_nib <= 9)
					*(str++) = hex_nib + ASCII_TO_INT;
				else
					*(str++) = hex_nib + INT_TO_HEX_LETTER;
			}
		}
	}

	// Add null terminating character
	*(str) = '\0';

	return (int8_t *) str;
}

int32_t my_atoi(uint8_t *str)
{
    uint32_t i = 0;
    uint32_t retval = 0, pow10;
    uint8_t *astr = str;
	uint8_t pow10_cnt;
    bool isNeg = false;

    // Null string
    if (*str == '\0')
        return ERR_NULLPTR;
	
    // Iterate to the end of the string, look for non-integer characters
    while (*astr != '\0' && i < (MAX_INT_CHAR_LEN - 1)) {
		if (i == 0 && *astr == '-') {
			isNeg = true;
			astr++;
		}
		else if (*astr >= '0' && *astr <= '9') {
			i++; 
			astr++;
		}
		else
			return ERR_INVINPUT;
	}

    astr = str;
    // Is it negative?
    if (isNeg)
        astr++;

    // 32-bit signed integers range from -2,147,483,648 to 2,147,483,647 (10 digits)
    // Can this string fit in that?
    if (i > MAX_INT_CHAR_LEN || (*astr == '3' && i > 9))
        return ERR_INVINPUT;

	// Adjust the offset 
    while(i > 1) {                    
		// Calculate the power of ten offset        
		pow10 = 10;
   		for (pow10_cnt = 0; pow10_cnt < (i-2); pow10_cnt++)
			 pow10 = 10*pow10;
		retval += (*astr - ASCII_TO_INT) * pow10;
		astr++;
		i--;
    }

	// Adding in the 1's place
	retval += (*astr) - 0x30;

	if (isNeg && retval > ((uint32_t) (INT32_MAX) + 1)) {
		// Number is too big to fit in a signed integer
		return ERR_INVINPUT;
	}
	else if (isNeg)
		return (-1)*((int32_t) retval);	
	else
		return (int32_t) retval;
}

void dump_memory(uint8_t *start, uint32_t length)
{
    uint32_t i = length;
    uint8_t *ptr8 = start;
    uint8_t line_cnt = 0;
    if (start != NULL && length > 0) {
        while (i-- > 0) {
            // Print 16 bytes per line
            printf("0x%x ", *ptr8++);
            if (line_cnt++ > DUMP_BYTES_PER_LINE) {
                line_cnt = 0;
                printf("\n");        
            }              
        }
    }
}

int32_t big_to_little(int32_t data)
{
    int32_t shifted = data;
    int8_t *ptr8 = (int8_t *) &shifted;
    int8_t temp;
    
    temp = *ptr8;
    *ptr8 = *(ptr8 + 3);
    *(ptr8 + 3) = temp;
    temp = *(ptr8 + 1);
    *(ptr8 + 1) = *(ptr8 + 2);
    *(ptr8 + 2) = temp;
    return shifted;
}

int32_t little_to_big(int32_t data)
{
    return big_to_little(data);
}
