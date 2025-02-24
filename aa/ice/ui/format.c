#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../../headers/format.h"

#define MAX_STRING_LENGTH (32)
typedef struct
{
    int exp;
    char unit;
    double scale;
} Unit;

Unit units[] = {
    {-6, 'n', 1e9},
    {-3, 'u', 1e6},
    {0, 'm', 1e3},
    {3, ' ', 1},
    {6, 'K', 1e-3},
    {9, 'M', 1e-6},
    {12, 'G', 1e-9}};

int macro_internal_from_double_to_string(double f, char *str, char *format, int len)
{
    if (len <= 1)
    {
        return 0;
    }
    if (f == 0.0)
    {
        str[0] = '0';
        str[1] = 0x00;
        return 1;
    }
    int exp = 0;
    exp = floor(log10(fabs(f)));
    if (exp < -9 || exp > 15)
    {
        str[0] = '0';
        str[1] = 0x00;
        return 1;
    }
    //char s = (f < 0) ? '-' : ' ';
    for (int i = 0; i < sizeof(units) / sizeof(units[0]); i++)
    {
        if (exp < units[i].exp)
        {
            return snprintf(str, len, format, f * units[i].scale, units[i].unit);
        }
    }
    return 0;
}

double from_string_to_double(const char *str)
{
    double f = 0;
    size_t len = strnlen(str, MAX_STRING_LENGTH);
    if (len == 0 || len == 32)
    {
        return f;
    }
    f = strtod(str, NULL);

    for (size_t i = 0; i < len; i++)
    {
        switch (str[i])
        {
        case 'n':
        case 'N':
            f = f * 1e-9;
            return f;
        case 'u':
        case 'U':
            f = f * 1e-6;
            return f;
        case 'm':
            f = f * 1e-3;
            return f;
        case 'K':
        case 'k':
            f = f * 1e3;
            return f;
        case 'M':
            // case 'm':
            f = f * 1e6;
            return f;
        case 'G':
        case 'g':
            f = f * 1e9;
            return f;
        default:
            break;
        }
    }
    return f;
}
