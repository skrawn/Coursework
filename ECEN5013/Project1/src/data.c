
#include <stdbool.h>
#include <stdio.h>

#include "data.h"

#define DUMP_BYTES_PER_LINE     16

int8_t *my_itoa(uint8_t *str, int32_t data, int32_t base)
{
	return 0;
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
        return -1;
	
    // Iterate to the end of the string, look for non-integer characters
    while (*astr != '\0' && i < 11) {
		if (i == 0 && *astr == '-') {
			printf("is neg\n");
			isNeg = true;
			astr++;
		}
		else if (*astr >= '0' && *astr <= '9') {
			i++; 
			astr++;
		}
		else
			return -2;
	}

    astr = str;
    // Is it negative?
    if (isNeg)
        astr++;

    // 32-bit signed integers range from -2,147,483,648 to 2,147,483,647 (10 digits)
    // Can this string fit in that?
    if (i > 10 || (*astr == '3' && i > 9))
        return -2;

	// Adjust the offset 
    while(i > 1) {                    
		// Calculate the power of ten offset        
		pow10 = 10;
   		for (pow10_cnt = 0; pow10_cnt < (i-2); pow10_cnt++)
			 pow10 = 10*pow10;
		printf("pow10 = %d, string = %c\n", pow10, *astr);
		retval += (*astr - 0x30) * pow10;
		printf("retval = %d\n", retval);
		astr++;
		i--;
    }

	// Adding in the 1's place
	retval += (*astr) - 0x30;

	if (isNeg && retval > ((uint32_t) (INT32_MAX) + 1)) {
		// Number is too big to fit in a signed integer
		return -2;
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
            printf("0x%x ", *ptr8);
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
