#ifndef PROMPT_HEADER_LIB
#define PROMPT_HEADER_LIB

#include <stddef.h>
#include <stdio.h>

#include <time.h>

#include "./../headers/con.h"
#include "./var.h"
#include "./types.h"
#include "./strutil.h"

#define convert_r(r, range) (r / (double)(1 << 19) * 1.2 * range)
#define convert_p(p) (p / (double)(1 << 19) * 180.0)
// Parse the buffer from the lock-in-amplifier
static void parse_lia_buffer(Env *env)
{
	Meas *m = &env->m;
	if (env->l.socket == -1 || env->l.receive_buffer == NULL)
	{
		puts("lia is not connected");
		return;
	}
	int ret = sscanf(env->l.receive_buffer, "%d,%lf,%lf,%lf,%lf",
		        &m->status_flag, &m->r_a, &m->phase_a, &m->r_b, &m->phase_b);
	
	if (ret != 5) {
		puts("cannot parse buffer");
		return;
	}
	m->r_a = convert_r(m->r_a, m->range_a);
	m->r_b = convert_r(m->r_b, m->range_b);
	m->phase_a = convert_p(m->phase_a);
	m->phase_b = convert_p(m->phase_b);
	if (m->phase_a > 180.0)
	{
		m->phase_a -= 360.0;
	}
	if (m->phase_b > 180.0)
	{
		m->phase_b -= 360.0;
	}
}

static void print_meas(Env *env)
{
	if (env->l.socket == -1|| env->l.receive_buffer == NULL)
	{
		puts("lia is not connected");
		return;
	}
	double f;
	get_var(&env->vars, "f", &f);
	printf("FETC %lf %lf %lf %lf %lf E:%d\n",
		   f,
		   env->m.r_a,
		   env->m.phase_a,
		   env->m.r_b,
		   env->m.phase_b,
		   env->m.status_flag);
}

static void write_to_file(Env *env)
{
	if (env->fp_out == NULL)
		return;
	double f;
	get_var(&env->vars, "f", &f);
	fprintf(env->fp_out, "%lf %lf %lf %lf %lf\n",
			f,
			env->m.r_a,
			env->m.phase_a,
			env->m.r_b,
			env->m.phase_b);
	fflush(env->fp_out);
}

static int parse_command(char *str, Env *env, FILE *restrict stream)
{
	char *p;
	char slice[64];
	char temp[64];

	p = next(str, slice);
	if (slice[0] == 0x00)
	{   // command is empty
		return 0;
	} else if (strncmp(slice, "#", 1) == 0)
	{   // comment
		return 0;
	}
	else if (strcmp(slice, "q") == 0)
	{   // quit
		return 1;
	}
	else if (strcmp(slice, "h") == 0)
	{   // help
		puts("q                   quit");
		puts("s1, s2, l           send command");
		puts("s1?, s2?, l?        query");
		puts("wait [second]       wait for [second]");
		puts("output [filename]   set output");
		puts("range_a [value]     set range a");
		puts("range_b [value]     set range b");
		puts("fetch  [waittime]   fetch data");
		puts("$[var] [expr]       set variable");
		puts("variables           a,b,c,f,i,j,x,y");
		puts("!! Please use variable 'f' for frequency.");
		puts("!! 'f' will write out to file as the first column.");
		puts("!! output file form is `f r_a phase_a r_b phase_b`");
		return 0;
	}
	else if (strcmp(slice, "s1") == 0)
	{	// send command to s1
		format_var(&env->vars, temp, p);
		printf(">>s1: %s\n", temp);

		/*
		**  For communicating oscillators,
		**  needs to receive until prompt '*'.
		**  Please see the manual for the device "DPL32GXF".
		*/
		query_until(&env->s1, temp, '*');
		return 0;
	}
	else if (strcmp(slice, "s2") == 0)
	{   // send command to s2
		format_var(&env->vars, temp, p);
		printf(">>s2: %s\n", temp);
		/*
		**  Same as s1.
		*/
		query_until(&env->s2, temp, '*');
		return 0;
	}
	else if (strcmp(slice, "l") == 0)
	{	// send command to lock-in-amplifier
		format_var(&env->vars, temp, p);
		printf(">>l: %s\n", temp);
		/*
		**  For communicating Lock-in-amplifier, just send the command.
		*/
		send_command(&env->l, p);
		return 0;
	}
	else if (strcmp(slice, "s1?") == 0)
	{	// query s1
		format_var(&env->vars, temp, p);
		printf(">>s1: %s\n", temp);
		query_until(&env->s1, temp, '*');
		printf("<<s1: %s\n", env->s1.receive_buffer);
		return 0;
	}
	else if (strcmp(slice, "s2?") == 0)
	{	// query s2
		format_var(&env->vars, temp, p);
		printf(">>s2: %s\n", temp);
		query_until(&env->s2, temp, '*');
		printf("<<s2: %s\n", env->s2.receive_buffer);
		return 0;
	}
	else if (strcmp(slice, "l?") == 0)
	{	// query lock-in-amplifier
		format_var(&env->vars, temp, p);
		printf(">>l: %s\n", temp);
		query_command(&env->l, p);
		printf("<<l: %s\n", env->l.receive_buffer);
		return 0;
	}
	else if (strncmp(slice, "$", 1) == 0)
	{	// set variable
		if (strlen(slice) < 2)
		{
			puts("variables: a,b,c,f,i,j,x,y");
			return 0;
		}
		if (set_var(&env->vars, &slice[1], calc_expr(&env->vars, p)) != 0)
		{
			puts("variable doesn't exist");
			return 0;
		}
		else
		{
			double newval;
			get_var(&env->vars, &slice[1], &newval);
			#ifdef VERBOSE
			printf("set %s to %lf\n", &slice[1], newval);
			#endif
			return 0;
		}
	}
	else if (strcmp(slice, "wait") == 0)
	{	// wait for [second]
		if (p == NULL)
		{
			puts("wait [second]");
			return 0;
		}
		double t = strtod(p, NULL);
		if (t <= 0.0)
		{
			puts("wait [second]");
			return 0;
		}
		struct timespec ts = {
			(time_t)t,
			(long int)((t - (time_t)t) * 1e9)
		};
		nanosleep(&ts, NULL);
		return 0;
	}
	else if (strcmp(slice, "output") == 0)
	{	// set output file
		if (p == NULL)
		{
			puts("output [filename]");
			return 0;
		}
		if (env->fp_out != NULL)
		{
			fclose(env->fp_out);
			env->fp_out = NULL;
		}
		p = next(p, slice);
		trim(slice);
		env->fp_out = fopen(slice, "wt");
		if (env->fp_out == NULL)
		{
			perror("cannot open file");
			return 0;
		}
		return 0;
	}
	else if (strcmp(slice, "fetch") == 0)
	{	// fetch data
		if (env->m.range_a == 0.0 || env->m.range_b == 0.0)
		{
			puts("please set range first");
			return 0;
		}
		send_command(&env->l, ":TRIG");
		double t = strtod(p, NULL);
		if (t <= 0.0)
			t = 5.0;
		struct timespec ts = {
			(time_t)t,
			(long int)((t - (time_t)t) * 1e9)
		};
		nanosleep(&ts, NULL);
		query_command(&env->l, ":FETC?");
		parse_lia_buffer(env);
		print_meas(env);
		write_to_file(env);
		return 0;
	}
	else if (strcmp(slice, "range_a") == 0)
	{	// set range a
		double range = strtod(p, NULL);
		if (range <= 0.0)
		{
			puts("range_a [value]");
			return 0;
		}
		env->m.range_a = range;
		return 0;
	}
	else if (strcmp(slice, "range_b") == 0)
	{	// set range b
		double range = strtod(p, NULL);
		if (range <= 0.0)
		{
			puts("range_b [value]");
			return 0;
		}
		env->m.range_b = range;
		return 0;
	}
	else if (strcmp(slice, "loop") == 0)
	{
		long f_pos = ftell(stream);
		if (f_pos == -1)
		{
			puts("cannot loop. please open file first");
			return 0;
		}
		if (p == NULL)
		{
			puts("loop [count]");
			return 0;
		}
		int i = atoi(p);
		if (i <= 0)
		{
			puts("loop [count]");
			return 0;
		}
		if (env->loop_n != 0)
		{
			puts("loop is already running");
			return 0;
		}
		env->loop_n = i;
		env->loop_p = f_pos;
		return 0;
	}
	else if (strcmp(slice, "endloop") == 0)
	{
		if (env->loop_n == 0)
		{
			return 0;
		}
		else
		{
			env->loop_n -= 1;
			fseek(stream, env->loop_p, SEEK_SET);
		}
		return 0;
	}

	puts("unknown command");
	return 0;
}

static int prompt(FILE *restrict stream, Env *env, int p_p)
{
	char buffer[64];
	if (p_p)
		putchar('>');
	if (fgets(buffer, sizeof(buffer), stream) == NULL)
	{
		puts(strerror(errno));
		return 1;
	}
	return parse_command(buffer, env, stream);
}

#endif