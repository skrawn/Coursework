
#include <stdlib.h>
#include "hw1.h"

char reverse_string(char *str, int length) 
{
	int i = 0;
	char *left, *right, temp;

	if (str == NULL)
		return -1;
	if (length <= 0)
		return -2;

	left = str;
	right = str+(length-2);
	
	i = length / 2;	
	while (i > 0) {
		temp = *left;
		*(left++) = *right;
		*(right--) = temp;
		i--;	
	}
	return 0;
}

