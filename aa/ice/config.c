

#include <string.h>
#include <stdio.h>
#include "./../../headers/config.h"
#include "./../../headers/commandq.h"
#include "./../../headers/format.h"

#define convert_r(r, range) (r / (double)(1 << 19) * 1.2 * range)
#define convert_p(p) (p / (double)(1 << 19) * 180.0)

const lia_status_to_string_t lia_status_to_string[9] = {
        {8, "RATIOOVER"},
        {16, "MOVOVER_B"},
        {32, "MOVOVER_A"},
        {64, "LPFOVER_B"},
        {128, "LPFOVER_A"},
        {256, "ADCOVER_B"},
        {512, "ADCOVER_A"},
        {16384, "OSCOVER"},
        {32768, "UNLOCK"},
};

const range_data_t range_data[25] = {
    {1.0e0, "1V"},
    {500.0e-3, "500mV"},
    {200.0e-3, "200mV"},
    {100.0e-3, "100mV"},
    {50.0e-3, " 50mV"},
    {20.0e-3, " 20mV"},
    {10.0e-3, " 10mV"},
    {5.0e-3, "  5mV"},
    {2.0e-3, "  2mV"},
    {1.0e-3, "  1mV"},
    {500.0e-6, "500uV"},
    {200.0e-6, "200uV"},
    {100.0e-6, "100uV"},
    {50.0e-6, " 50uV"},
    {20.0e-6, " 20uV"},
    {10.0e-6, " 10uV"},
    {5.0e-6, "  5uV"},
    {2.0e-6, "  2uV"},
    {1.0e-6, "  1uV"},
    {500.0e-9, "500nV"},
    {200.0e-9, "200nV"},
    {100.0e-9, "100nV"},
    {50.0e-9, " 50nV"},
    {20.0e-9, " 50nV"},
    {10.0e-9, " 10nV"},
};

// todo : write this
 int dpl32_conf_check(SgConfig *conf) {
    int ret = 0;
    if (conf->freq > 3.2e9) {
        conf->freq = 3.2e9;
        ret++;
    }

    return ret;
 }
 int li5502_conf_check(LiaConfig *conf){
    return 0;
 }

int dpl32_conf_to_str(const SgConfig *conf, char *buf)
{
    return snprintf(buf, 64, "F %f\n A %f\n %s", conf->freq, conf->amplitude, conf->output ? "ON" : "OFF");
}

int dpl32_conf_to_queue(const SgConfig *conf, CommandQ *q)
{
    char buf[COMMAND_BUFFER_SIZE];
    snprintf(buf, COMMAND_BUFFER_SIZE - 2, "%s", conf->output ? "ON" : "OFF");
    commandq_push(q, buf, COMMANDQ_TYPE_SENDUNTIL);
    snprintf(buf, COMMAND_BUFFER_SIZE - 2, "F %.2f", conf->freq);
    commandq_push(q, buf, COMMANDQ_TYPE_SENDUNTIL);
    snprintf(buf, COMMAND_BUFFER_SIZE - 2, "A %2.1f", conf->amplitude);
    commandq_push(q, buf, COMMANDQ_TYPE_SENDUNTIL);
    return 0;
}

int lia_mdata_from_str(const char *str, LiaMeasurements *m, LiaConfig *conf)
{
    sscanf(str, "%d,%lf,%lf,%lf,%lf",
           &m->status_flag, &m->r_a, &m->phase_a, &m->r_b, &m->phase_b);

    m->r_a = convert_r(m->r_a, conf->range);
    m->r_b = convert_r(m->r_b, conf->range);
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

    return 0;
}

int lia_initializer_to_queue(CommandQ *q)
{
    commandq_push(q, ":DATA:FEED 1633", COMMANDQ_TYPE_SENDONLY);
    commandq_push(q, ":ROUT:TERM IOSC", COMMANDQ_TYPE_SENDONLY);
    commandq_push(q, ":SENS:ROSC:SOUR EXT", COMMANDQ_TYPE_SENDONLY);
    commandq_push(q, "*CLE", COMMANDQ_TYPE_SENDONLY);
    return 0;
}

int lia_conf_to_queue(const LiaConfig *conf, CommandQ *q)
{
    char buf[COMMAND_BUFFER_SIZE] = {0};
    char mvstr[16];
    lia_mov_to_string(mvstr, conf->mov);

    snprintf(buf, COMMAND_BUFFER_SIZE - 2, ":SENS:DRES %s", lia_dr_to_string(conf->dr));
    commandq_push(q, buf, COMMANDQ_TYPE_SENDONLY);
    snprintf(buf, COMMAND_BUFFER_SIZE - 2, ":SENS:DRES2 %s", lia_dr_to_string(conf->dr));
    commandq_push(q, buf, COMMANDQ_TYPE_SENDONLY);
    snprintf(buf, COMMAND_BUFFER_SIZE - 2, ":SENS:FILT:SLOP %s", lia_slop_to_string(conf->slop));
    commandq_push(q, buf, COMMANDQ_TYPE_SENDONLY);
    snprintf(buf, COMMAND_BUFFER_SIZE - 2, ":SENS:FILT2:SLOP %s", lia_slop_to_string(conf->slop));
    commandq_push(q, buf, COMMANDQ_TYPE_SENDONLY);
    snprintf(buf, COMMAND_BUFFER_SIZE - 2, ":SENS:FILT:TCON "FORMAT_LIA_TCON, conf->tcon);
    commandq_push(q, buf, COMMANDQ_TYPE_SENDONLY);
    snprintf(buf, COMMAND_BUFFER_SIZE - 2, ":SENS:FILT:TCON "FORMAT_LIA_TCON, conf->tcon);
    commandq_push(q, buf, COMMANDQ_TYPE_SENDONLY);
    snprintf(buf, COMMAND_BUFFER_SIZE - 2, ":SENS:FILT:MOV %s", mvstr);
    commandq_push(q, buf, COMMANDQ_TYPE_SENDONLY);
    snprintf(buf, COMMAND_BUFFER_SIZE - 2, ":SENS:FILT2:MOV %s", mvstr);
    commandq_push(q, buf, COMMANDQ_TYPE_SENDONLY);
    snprintf(buf, COMMAND_BUFFER_SIZE - 2, ":SENS:VOLT:AC:RANGE %3.2e", conf->range);
    commandq_push(q, buf, COMMANDQ_TYPE_SENDONLY);
    snprintf(buf, COMMAND_BUFFER_SIZE - 2, ":SENS:VOLT2:AC:RANGE %3.2e", conf->range);
    commandq_push(q, buf, COMMANDQ_TYPE_SENDONLY);
    return 0;
}
int lia_fetch_to_queue(CommandQ *q)
{
    commandq_push(q, ":FETC?", COMMANDQ_TYPE_SENDRECV);
    commandq_push(q, "", COMMANDQ_TYPE_WAITTRIG);
    commandq_push(q, ":TRIG", COMMANDQ_TYPE_SENDONLY);
    return 0;
}

const char *lia_dr_to_string(LiaDynamicReserve dr)
{
    const char *str[] = {
        "LOW1",
        "LOW2",
        "MID",
        "HIGH",
    };
    return str[dr];
}
const char *lia_slop_to_string(LiaSlope s)
{
    const char *str[] = {
        "OFF",
        "6",
        "12",
        "18",
        "24",
    };
    return str[s / 6];
}

void lia_mov_to_string(char *s, float mov)
{
    if (mov == LIA_SET_AUTO)
    {
        strcpy(s, "AUTO");
    }
    else if (mov == LIA_SET_OFF)
    {
        strcpy(s, "OFF");
    }
    else
    {
        snprintf(s, 15, FORMAT_LIA_TCON, mov);
    }
}