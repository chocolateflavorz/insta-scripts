

#ifndef CONF_H
#define CONF_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

    typedef enum
    {
        LIA_DR_LOW1 = 0,
        LIA_DR_LOW2 = 1,
        LIA_DR_MID = 2,
        LIA_DR_HIGH = 3,
    } LiaDynamicReserve;

    typedef enum
    {
        LIA_SLOP_OFF = 0,
        LIA_SLOP_6 = 6,
        LIA_SLOP_12 = 12,
        LIA_SLOP_18 = 18,
        LIA_SLOP_24 = 24,
    } LiaSlope;

    typedef struct {
        uint16_t flag;
        const char *str;
    } lia_status_to_string_t;

    typedef struct {
        double range;
        const char *str;
    } range_data_t;

    extern const lia_status_to_string_t lia_status_to_string[9];
    extern const range_data_t range_data[25];
#define LIA_SET_AUTO (-1.0)
#define LIA_SET_OFF (0.0)
    /*
        typedef struct
        {
            int type;
            const char *name;
            const char *description;
            const char *ip;
            int port;
        } EqConfig;
    */

#define DEFAULT_SGCONFIG_INITIALIZER \
    {                                \
        1e6 , 0, 0, false                \
    }
    typedef struct
    {
        double freq;
        float amplitude;
        float phase;
        bool output;
    } SgConfig;

/*
SLOP/MOV
    0.0  for OFF
    -1.0 for AUTO
*/
#define DEFAULT_LIACONFIG_INITIALIZER                       \
    {                                                       \
        LIA_DR_LOW1, LIA_SLOP_12, 1.0, LIA_SET_AUTO, 1.0e-6 \
    }
    typedef struct
    {
        LiaDynamicReserve dr;
        LiaSlope slop;
        float tcon;
        float mov;
        double range;
    } LiaConfig;

    typedef struct
    {
        double r_a;
        double phase_a;
        double r_b;
        double phase_b;
        int status_flag;
    } LiaMeasurements;

#define DEFAULT_SWEEP_CONFIG_INITIALIZER                             \
    {                                                                \
        750e6, 1100e6, 1e6, 1, 2, -1.0e6, "thracia776.dat" \
    }
    typedef struct
    {
        double start;
        double stop;
        double step;
        unsigned char which_is_base; // 0 or 1?
        unsigned char order;      
        double bias;        
        char filename[25];
    } SweepConfig;

    // pass the json string and the struct array to fill
    // int eq_conf(const char *json, EqConfig *eq);

    // command for freq, amp and output, each command is terminated by 0x0a
    // return the length of the all command (write to buf)
#include "commandq.h"

    int dpl32_conf_check(SgConfig *conf);
    int li5502_conf_check(LiaConfig *conf);

    int dpl32_conf_to_str(const SgConfig *conf, char *buf);
    int dpl32_conf_to_queue(const SgConfig *conf, CommandQ *q);

    int lia_initializer_to_queue(CommandQ *q);
    int lia_conf_to_queue(const LiaConfig *conf, CommandQ *q);
    int lia_mdata_from_str(const char *str, LiaMeasurements *m, LiaConfig *conf);

    const char *lia_dr_to_string(LiaDynamicReserve dr);
    const char *lia_slop_to_string(LiaSlope s);
    void lia_mov_to_string(char *s, float mov);

    // due to need to wait a bit for the trigger, the command is specialized and manually thread spawn
    int lia_fetch_to_queue(CommandQ *q);


/*
const struct dr_s	dr[] = {
	{0, "HIGH", 0, 16},
	{1, "MED" , 0, 18},
	{2, "LOW2", 3, 21},
	{3, "LOW1", 6, 24}
} ;
struct	range_s
{
	int 	index ;
	double	fs ;
	char	label[6] ;
	double	mul ;
	char	unit[3] ;
} ;
const struct range_s	range[] = {
  { 0,  1.0e0, "   1V",1e0,"V "},
  { 1,500.0e-3,"500mV",1e3,"mV"},{ 2,200.0e-3,"200mV",1e3,"mV"},{ 3,100.0e-3,"100mV",1e3,"mV"},
  { 4, 50.0e-3," 50mV",1e3,"mV"},{ 5, 20.0e-3," 20mV",1e3,"mV"},{ 6, 10.0e-3," 10mV",1e3,"mV"},
  { 7,  5.0e-3,"  5mV",1e3,"mV"},{ 8,  2.0e-3,"  2mV",1e3,"mV"},{ 9,  1.0e-3,"  1mV",1e3,"mV"},
  {10,500.0e-6,"500uV",1e6,"uV"},{11,200.0e-6,"200uV",1e6,"uV"},{12,100.0e-6,"100uV",1e6,"uV"},
  {13, 50.0e-6," 50uV",1e6,"uV"},{14, 20.0e-6," 20uV",1e6,"uV"},{15, 10.0e-6," 10uV",1e6,"uV"},
  {16,  5.0e-6,"  5uV",1e6,"uV"},{17,  2.0e-6,"  2uV",1e6,"uV"},{18,  1.0e-6,"  1uV",1e6,"uV"},
  {19,500.0e-9,"500nV",1e9,"nV"},{20,200.0e-9,"200nV",1e9,"nV"},{21,100.0e-9,"100nV",1e9,"nV"},
  {22, 50.0e-9," 50nV",1e9,"nV"},{23, 20.0e-9," 50nV",1e9,"nV"},{24, 10.0e-9," 10nV",1e9,"nV"}
} ;

*/
#ifdef __cplusplus
}
#endif

#endif