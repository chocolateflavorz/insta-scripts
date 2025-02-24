
#define _POSIX_C_SOURCE 200809L

// if you don't want to print debug messages, uncomment the following line
#define VERBOSE

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "./prompt.h"
#include "./strutil.h"
#include "./types.h"
#include "./var.h"
#include "../headers/con.h"
#include "./tinyexpr.h"
int main(int argc, char **argv)
{
	Env env = {
		.s1 = DEFAULT_CON_INITIALIZER,
		.s2 = DEFAULT_CON_INITIALIZER,
		.l = DEFAULT_CON_INITIALIZER,
		.loop_p = -1,
		.loop_n = 0,
		.fp_out = NULL,
		.m = MEAS_INITIALIZER,
	};
	init_vars(&env.vars);

	/*
	**  For communicating oscillators, 0x0d (Carriage Return) is used as a delimiter.
	**  For communicating Lock-in-amplifier, 0x0a (Line Feed) is used as a delimiter.
	**    open_con(  ##  , IP , Port, Delimiter, Timeout  )
	**    open_con(); wait for [Timeout] seconds to connect.
	*/

	if (open_con(&env.s1, "192.168.3.202", 5025, 0x0d, 2) != 0)
	{
		env.s1.socket = -1;
		perror("cannot open s1");
	}
	if (open_con(&env.s2, "192.168.3.202", 5026, 0x0d, 2) != 0)
	{
		env.s2.socket = -1;
		perror("cannot open s2");
	}
	if (open_con(&env.l, "192.168.3.201", 5025, 0x0a, 2) != 0)
	{
		env.l.socket = -1;
		perror("cannot open l");
	}

	// if any options supplied, load file
	if (argc > 1)
	{
		printf("load: %s\n", argv[1]);
		FILE *fp = fopen(argv[1], "r");
		if (fp == NULL)
		{
			perror("cannot open file");
			return 0;
		}
		while (prompt(fp, &env, 0) != 1)
		{
		}
	}
	else
	{ // immediate mode
		puts("q: quit, s1, s2, l, $[var] [expr]");
		puts("s1?, s2?, l?, output [filename], fetch");
		puts("loop [count], endloop, h: help");
		puts("variable: a, b, c, f, i, j, x, y");
		while (prompt(stdin, &env, 1) != 1)
		{
		}
	}

	if (env.fp_out != NULL)
	{
		fclose(env.fp_out);
	}
	return 0;
}

#ifdef BLYAT
static void BLYAT_TESTER(Env *env)
{
	puts("SALUTON. Mi estas kat");
	puts("Tu i de en musika?");
	double val;
	set_var(&env->vars, "a", 1.0);
	get_var(&env->vars, "a", &val);
	printf("a: %f\n", val);
	val = 600e8;
	printf(" assign %lf to b", val);
	set_var(&env->vars, "b", val);
	get_var(&env->vars, "b", &val);
	printf("b: %lf\n", val);
	PRINT_VARS(&env->vars);

	set_var(&env->vars, "c", 100.0);
	get_var(&env->vars, "c", &val);
	printf("c: %f\n", val);

	puts("FORMAT VAR");
	char aa[64] = "$a * $b + 1e8";
	char ss[128] = {0};
	puts(aa);
	format_var(&env->vars, ss, aa);
	puts(ss);

	int err;
	double answer = 0;
	te_expr *expr = te_compile(ss, env->vars.tv, MAX_VARS, &err);
	if (expr)
	{
		const double answer = te_eval(expr);
		printf("answer: %lf\n", answer);
		te_free(expr);
	}
	else
	{
		printf("Parse error at %d\n", err);
	}
}
#endif
