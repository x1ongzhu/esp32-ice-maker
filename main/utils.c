#include <stdio.h>
#include "utils.h"
void convertBuffer(uint8_t *buffer, char *str, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        str[i] = buffer[i];
    }
}

unsigned int cmd_equal(char *str0, char *str1, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
    {
        if (str0[i] != str1[i])
        {
            return 0;
        }
    }
    return 1;
}