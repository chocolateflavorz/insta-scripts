#ifndef STRUTIL_HEADER_LIB
#define STRUTIL_HEADER_LIB

#include <stddef.h>
#include "./tinyexpr.h"
#include "./types.h"


// slice string by words
static char *next(char *restrict s, char *restrict dest);
// replace variable names with values itselves
static int format_var(Vars *v, char *dest, char *str);
// calc expression with variables
static double calc_expr(Vars *v, char *str);

// replace variable names with values itselves
static int format_var(Vars *v, char *dest, char *str)
{
	dest[0] = 0x00;
	char slice[64];
	char *p = str;
	do
	{
		p = next(p, slice);
		if (strncmp(slice, "$", 1) == 0)  // is variable
		{
			if (strlen(slice) < 2) {
				puts("variables must have names: format_var()");
				return 1;
			}
			double value;
			if (get_var(v, &slice[1], &value) == 0)
			{
				char buf[64];
				// fix: variable print format specifier
				sprintf(buf, "%.1lf", value);
				strcat(dest, buf);
			}
			else
			{
				puts("variable not found: format_var()");
				return 1;
			}
		} 
		else  // not a variable
		{
			strcat(dest, slice);
		}

		if (p != 0x00)
		{
			strcat(dest, " ");
		}
	} while (p != 0x00);

	return 0;
}
// slice string by words
static char *next(char *restrict s, char *restrict dest)
{
	size_t w = 0;
	if (s == 0x00 || dest == 0x00 || *s == 0x00)
	{
		return 0x00;
	}
	for (char *p = s;; p++)
	{
		switch (*p)
		{
		case ' ':
		case '\t':
			if (w > 0)
			{
				dest[w] = 0x00;
				return p;
			}
			break;
		case 0x0a:
		case 0x00:
			dest[w] = 0x00;
			return 0x00;
			break;
		default:
			dest[w++] = *p;
			break;
		}
	}
	return 0x00;
}

// calc expression with variables
static double calc_expr(Vars *v, char *str) {
	if (str == NULL) {
		puts("NULLLL: calc_expr()\n");
		return -1;
	}
	int err;
	double answer = 0;
	te_expr *expr = te_compile(str, v->tv, MAX_VARS, &err);
	if (expr) {
		answer = te_eval(expr);
		te_free(expr);
	} else {
		printf("Parse error at %d : calc_expr()\n", err);
	}
	return answer;
}

static void trim(char *str) {
	if (str == NULL) {
		puts("NULLLL: trim()\n");
		return;
	}
	size_t len = strlen(str);
	if (len == 0) {
		return;
	}
	for (char *i = str; *i != 0x00; i++) {
		if (*i == ' ' || *i == '\t' || *i == '\n') {
			*i = 0x00;
		}
	}
}

#endif