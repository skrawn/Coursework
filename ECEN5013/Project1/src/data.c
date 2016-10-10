
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

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

#define FLOAT_EXP_BIAS			-127
#define FLOAT_PRECISION			0.000000001

#define DUMP_BYTES_PER_LINE     16

int8_t *my_itoa(uint8_t *str, int32_t data, int32_t base)
{
	bool leading_zero = true;
	uint32_t mask, u_data, pow10;
	int64_t i;
	uint8_t nib, pow10_cnt;

	// Check for non-null string and valid base
	if (str == NULL)
		return NULL;

	if (base != 2 && base != 8 && base != 10 && base != 16)
		return NULL;

	if (data == 0) {
		*(str++) = ASCII_TO_INT;
		*str = '\0';
	}

	if (base == 2) {
		// Skip past any leading zeros
		i = 31;
		*str = '\0';
		while (!(data & (1 << i)) && i > 0) {i--;}
		while (i >= 0) {
			*(str++) = ((uint8_t) ((data & (1 << i)) >> i) >= 1 ? 1 : 0) + ASCII_TO_INT;
			i--;
		}
	}
	else if (base == 8) {
		// Take care of the first two bits
		if (data & 0xC0000000)
			*(str++) = ((uint8_t) ((data & 0xC0000000) >> 30)) + ASCII_TO_INT;
		i = 27;
		while (i >= 0) {
			nib = ((uint8_t) ((data & (0x7 << i)) >> i));
			if (nib == 0 && leading_zero) {
				// Do nothing
			}
			else {
				leading_zero = false;
				*(str++) = nib + ASCII_TO_INT;
			}
			i -= 3;
		}			
	}
	else if (base == 10) {
		if (data < 0) {
			*(str++) = '-';
			u_data = (uint32_t) (-1 * data);
		}
		else
			u_data = (uint32_t) data;

		i = 10;
		while (i > 1) {
			pow10 = 10;
			for (pow10_cnt = 0; pow10_cnt < (i-2); pow10_cnt++)
				pow10 = 10*pow10;
			
			nib = u_data / pow10;
			if (nib == 0 && leading_zero) {
				// Do nothing
			}
			else {
				leading_zero = false;
				*(str++) = nib + ASCII_TO_INT;
			}
			u_data = u_data % pow10;			
			i--;
		}
		
		*(str++) = u_data + ASCII_TO_INT;
	}
	else {
		for (i = 7; i >= 0; i--) {
			mask = (0xF << (i*4));
			nib = (uint8_t) ((data & mask) >> (i*4));	
			if (nib == 0 && leading_zero) {
				// Do nothing
			}
			else {
				leading_zero = false;
				if (nib <= 9)
					*(str++) = nib + ASCII_TO_INT;
				else
					*(str++) = nib + INT_TO_HEX_LETTER;
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

uint8_t *my_ftoa(float fp, uint8_t *str)
{

	// NOTE: This code was taken from a stack overflow post by androider
	// http://stackoverflow.com/questions/2302969/how-to-implement-char-ftoafloat-num-without-sprintf-library-function-i
	// A few things were fixed to make it work for this project and fit the coding standards
	// and the comments were added by me.

    int32_t digit, m, m1, useExp;
    uint8_t *c = str;
    int32_t neg = (fp < 0.0);
	double weight;

    // handle special cases
    if (isnan(fp)) {
        strcpy((char *) str, "nan");
    } 
	else if (isinf(fp)) {
        strcpy((char *) str, "inf");
    } 
	else if (fp == 0.0) {
        strcpy((char *) str, "0.0");
    } 
	else {
		
        if (neg)
            fp = -fp;

        // Determine with width of the integer part. If it's
		// bigger than what will fit in a normal integer,
		// e notation will be used.
        m = log10(fp);
        useExp = (m >= 14 || (neg && m >= 9) || m <= -9);
        if (neg)
            *(c++) = '-';
        // set up for scientific notation
        if (useExp) {
            if (m < 0)
               m -= 1.0;
            fp = fp / pow(10.0, m);
            m1 = m;
            m = 0;
        }
        if (m < 1.0) {
            m = 0;
        }
        // convert the number
        while (fp > FLOAT_PRECISION || m >= 0) {
            weight = pow(10.0, m);
            if (weight > 0 && !isinf(weight)) {
                digit = floor(fp / weight);
                fp -= (digit * weight);
                *(c++) = '0' + digit;
            }
            if (m == 0 && fp > 0)
                *(c++) = '.';
            m--;
        }
        if (useExp) {
            // convert the exponent
            int i, j;
            *(c++) = 'e';
            if (m1 > 0) {
                *(c++) = '+';
            } else {
                *(c++) = '-';
                m1 = -m1;
            }
            m = 0;
            while (m1 > 0) {
                *(c++) = '0' + m1 % 10;
                m1 /= 10;
                m++;
            }
            c -= m;
            for (i = 0, j = m-1; i<j; i++, j--) {
                // swap without temporary
                c[i] ^= c[j];
                c[j] ^= c[i];
                c[i] ^= c[j];
            }
            c += m;
        }
        *(c) = '\0';
    }
    return str;
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
            if (++line_cnt >= DUMP_BYTES_PER_LINE) {
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
