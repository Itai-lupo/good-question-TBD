#pragma once
#include <string.h>
#include "keycodes.hpp"

struct keyData
{
    keyData(char *c, keycodes value): value(value)    
    {
        strncpy(utf8Buffer, c, 4);
    }
    char utf8Buffer[6];
    keycodes value;
};
