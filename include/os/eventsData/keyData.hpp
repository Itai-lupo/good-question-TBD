#pragma once
#include <string.h>
#include "keycodes.hpp"

struct keyData
{
    keyData(char *c, keycodes value, uint32_t scanCode): value(value), scanCode(scanCode)
    {
        strncpy(utf8Buffer, c, 4);
    }
    char utf8Buffer[6];
    keycodes value;
    uint32_t scanCode;
};
