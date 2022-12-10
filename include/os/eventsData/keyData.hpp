#pragma once
#include <string.h>
#include "core.hpp"
#include "keycodes.hpp"

struct keyData
{
    keyData(surfaceId id, char *c, keycodes value, uint32_t scanCode): id(id), value(value), scanCode(scanCode)
    {
        strncpy(utf8Buffer, c, 4);
    }
    surfaceId id;
    char utf8Buffer[6];
    keycodes value;
    uint32_t scanCode;
};
