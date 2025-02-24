#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include "../headers/con.h"
#include "./var.h"

#define MEAS_INITIALIZER {0, 0, 0, 0, 0, 0, 0}
typedef struct {
	double range_a;
	double range_b;

	int status_flag;
	double r_a;
	double phase_a;
	double r_b;
	double phase_b;
} Meas;

typedef struct {
	Con s1;
	Con s2;
	Con l;
	Vars vars;
	FILE *fp_out;
	long int loop_p;
	unsigned int loop_n;
	Meas m;
} Env;



#endif
