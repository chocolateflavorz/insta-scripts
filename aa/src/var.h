

#ifndef VAR_HEADER_LIB
#define VAR_HEADER_LIB

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "./tinyexpr.h"

#define MAX_VARS 8
#define MAX_VAR_NAME_LEN 1

    typedef struct
    {
        double values[MAX_VARS];
        te_variable tv[MAX_VARS];
    } Vars;

    static void init_vars(Vars *vars)
    {
        vars->tv[0] = (te_variable){"a", &vars->values[0]};
        vars->tv[1] = (te_variable){"b", &vars->values[1]};
        vars->tv[2] = (te_variable){"c", &vars->values[2]};
        vars->tv[3] = (te_variable){"f", &vars->values[3]};
        vars->tv[4] = (te_variable){"i", &vars->values[4]};
        vars->tv[5] = (te_variable){"j", &vars->values[5]};
        vars->tv[6] = (te_variable){"x", &vars->values[6]};
        vars->tv[7] = (te_variable){"y", &vars->values[7]};
    }

    static size_t VAR_INDEX(const char *name)
    {
        const char *names[MAX_VARS] = {"a", "b", "c", "f", "i", "j", "x", "y"};
        size_t indexes[MAX_VARS] = {0, 1, 2, 3, 4, 5, 6, 7};
        for (size_t i = 0; i < MAX_VARS; i++)
        {
            if (strcmp(names[i], name) == 0)
            {
                return indexes[i];
            }
        }
        return -1;
    }

    static int set_var(Vars *vars, const char *name, double value)
    {
        if (VAR_INDEX(name) != -1)
        {
            vars->values[VAR_INDEX(name)] = value;
            return 0;
        }

        // variable not found
        return 1;
    }

    static int get_var(Vars *vars, const char *name, double *out_value)
    {
        if (VAR_INDEX(name) != -1)
        {
            *out_value = vars->values[VAR_INDEX(name)];
            return 0;
        }

        // variable not found
        return 1;
    }

    /*
    static void PRINT_VARS(Vars *vars)
    {
        printf("size: %d\n", MAX_VARS);
        for (size_t i = 0; i < MAX_VARS; i++)
        {
            printf("%s: %f\n", vars->tv[i].name, vars->values[i]);
        }
    }
    */

#ifdef __cplusplus
}
#endif

#endif