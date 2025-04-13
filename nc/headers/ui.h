

#ifndef SETUI_H
#define SETUI_H

#ifdef __cplusplus 
extern "C" {
#endif

#include "config.h"
#include "commandq.h"

#define MAX_FORM_STRINGS (20)

typedef struct
{
    unsigned char index_freq;
    char freq[MAX_FORM_STRINGS];
    unsigned char index_amp;
    char amp[MAX_FORM_STRINGS];
    bool output;
} sg_ui_data;

typedef struct
{
    LiaDynamicReserve dr;
    LiaSlope slop;
    unsigned char index_range;  
    unsigned char index_mov;
    char mov[MAX_FORM_STRINGS];
    unsigned char index_tcon;
    char tcon[MAX_FORM_STRINGS];
} lia_ui_data;

typedef struct
{
    unsigned char index_start;
    char start[MAX_FORM_STRINGS];
    unsigned char index_stop;
    char stop[MAX_FORM_STRINGS];
    unsigned char index_step;
    char step[MAX_FORM_STRINGS];
    bool which_is_base;
    unsigned char order;
    unsigned char index_bias;
    char bias[MAX_FORM_STRINGS];
    unsigned char index_filename; 
    char filename[MAX_FORM_STRINGS];
} sweep_ui_data;

typedef struct  {
    WINDOW *win;
    MENU *menu;
    ITEM **items;
    sg_ui_data data;
} SgWindow;

typedef struct  {
    WINDOW *win;
    MENU *menu;
    ITEM **items;
    lia_ui_data data;
} LiaWindow;

typedef struct  {
    WINDOW *win;
    MENU *menu;
    ITEM **items;
    sweep_ui_data data;
} SweepWindow;

// common
void print_term(const char *s);
void print_term_eq(int i, const char *s);
void footer();


void create_sg_window(SgWindow *sw, SgConfig *conf);
int sg_menu_handler(int ch, SgWindow *sw, SgConfig *conf);
void destroy_sg_window(SgWindow *sw);

void create_lia_window(LiaWindow *lw, LiaConfig *conf);
int lia_menu_handler(int ch, LiaWindow *lw, LiaConfig *conf);
void destroy_lia_window(LiaWindow *lw);



#ifdef __cplusplus
}
#endif

#endif