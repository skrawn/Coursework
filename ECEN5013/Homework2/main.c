

#include <stdint.h>
#include <stdio.h>

#include "hw1.h"

int main(void)
{
	printf("Hello world!\n");

	char str1[] = "This is a string";
	char str2[] = "some NUMmbers12345";
	char str3[] = "Does it reverse \n\0\t correctly?";	
	
	printf("Original: %s - ", str1);
	reverse_string(str1, sizeof(str1));
	printf("Reversed: %s\n", str1);

	printf("Original: %s - ", str2);
	reverse_string(str2, sizeof(str2));
	printf("Reversed: %s\n", str2);

	printf("Original: %s - ", str3);
	reverse_string(str3, sizeof(str3));
	printf("Reversed: %s\n", str3);
	return 0;
}
