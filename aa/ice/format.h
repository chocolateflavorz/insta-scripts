

#ifndef FORMAT_H
#define FORMAT_H

#ifdef __cplusplus
extern "C"
{
#endif

#define FORMAT_SG_FREQUENCY "%3.1f"
#define FORMAT_SG_AMPLITUDE "%2.1f"

// dr, slop and mov are defined in config.h / config.c as functions
#define FORMAT_LIA_RANGE "%3.0f"
#define FORMAT_LIA_TCON "%1.1f" // 'MOV' is also use this format
#define FORMAT_LIA_R "%3.6f"
#define FORMAT_LIA_THETA "%3.1f"
#define from_double_to_string(f, str, form, len) macro_internal_from_double_to_string(f, str, form "%c", len)

    // if the string is a valid frequency, return the frequency, otherwise return just a floating point number (same result as strtod)
    double from_string_to_double(const char *str);
    int macro_internal_from_double_to_string(double f, char *str, char *format, int len);
#ifdef __cplusplus
}
#endif

#endif