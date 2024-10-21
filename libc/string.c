#include "string.h"

/**
 * @brief Calculate the lenth of a string 
 * @param str The string to measure
 * @return The length of the string
 */
size_t 
strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

