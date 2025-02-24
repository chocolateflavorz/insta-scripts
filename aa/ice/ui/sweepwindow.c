
#include <stdlib.h>
#include <string.h>

#include <ncurses.h>
#include <menu.h>

#include "./../../headers/ui.h"
#include "./../../headers/config.h"
#include "./../../headers/format.h"
#include "./../../headers/commandq.h"

typedef enum
{
    SWEEP_MENU_START = 0,
    SWEEP_MENU_STOP = 1,
    SWEEP_MENU_STEP = 2,
    SWEEP_MENU_WHICH = 3,
    SWEEP_MENU_ORDER = 4,
    SWEEP_MENU_BIAS = 5,
    SWEEP_MENU_FILE = 6,
    SWEEP_MENU_SAVE = 7,
    SWEEP_MENU_EXIT = 8,
} SweepMenuIndex;

#define n_choices (sizeof(choices) / sizeof(choices[0]))
static const char *choices[] = {
    "START_F",
    "STOP_F ",
    "STEP_F ",
    "F_0 SG ",
    "F_ORDER",
    "F_BIAS ",
    "FileN  ",
    "Save   ",
    "exit   ",
};

static void show_panel_data(WINDOW *w, const sweep_ui_data *d);
static void show_cursor(WINDOW *w,SweepMenuIndex i, const sweep_ui_data *d);
static void ui_left_handler(SweepWindow *w);
static void ui_right_handler(SweepWindow *w);
static void ui_del_handler(SweepWindow *w);
static void ui_char_handler(SweepWindow *w, int ch);

static void data_to_config(SweepConfig *conf, sweep_ui_data *data)
{
    conf->start = from_string_to_double(data->start);
    conf->stop = from_string_to_double(data->stop);
    conf->step = from_string_to_double(data->step);
    conf->which_is_base = data->which_is_base;
    conf->order = data->order;
    conf->bias = from_string_to_double(data->bias);
    strncpy(conf->filename, data->filename, MAX_FORM_STRINGS);
}
static void config_to_data(SweepConfig *conf, sweep_ui_data *data)
{
    data->dr = conf->dr;
    data->slop = conf->slop;
    snprintf(data->tcon, MAX_FORM_STRINGS, FORMAT_LIA_TCON, conf->tcon);
    data->index_tcon = strnlen(data->tcon, MAX_FORM_STRINGS);
    lia_mov_to_string(data->mov, conf->mov);
    data->index_mov = strnlen(data->mov, MAX_FORM_STRINGS);
    snprintf(data->range, MAX_FORM_STRINGS, FORMAT_LIA_RANGE, conf->range);
    data->index_range = strnlen(data->range, MAX_FORM_STRINGS);
}

void create_sweep_window(SweepWindow *w, SweepConfig *conf)
{
    config_to_data(conf, &w->data);
    w->items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
    for (int i = 0; i < n_choices; i++)
    {
        w->items[i] = new_item(choices[i], NULL);
    }
    w->items[n_choices] = (ITEM *)NULL;
    w->menu = new_menu(w->items);

    w->win = newwin(LINES - 5, COLS / 2 - 1, 0, COLS / 2 + 1);
    keypad(w->win, TRUE);
    set_menu_win(w->menu, w->win);
    set_menu_sub(w->menu, derwin(w->win, LINES - 7, COLS / 8, 1, 1));
    set_menu_format(w->menu, n_choices, 1);
    set_menu_mark(w->menu, ">");
    box(w->win, 0, 0);
    post_menu(w->menu);
    wrefresh(w->win);
}

int sweep_menu_handler(int ch, SweepWindow *w, SweepConfig *conf)
{
    int r = 0;
    switch (ch)
    {
    case KEY_DOWN:
        menu_driver(w->menu, REQ_DOWN_ITEM);
        break;
    case KEY_UP:
        menu_driver(w->menu, REQ_UP_ITEM);
        break;
    case KEY_LEFT:
        ui_left_handler(w);
        break;
    case KEY_RIGHT:
        ui_right_handler(w);
        break;
    case KEY_ENTER:
    case 0x0a:
    {
        switch (item_index(current_item(w->menu)))
        {
        case LIA_MENU_RANGE ... LIA_MENU_DYRESV: // range, timecon, slope, movavg, dyresv
            menu_driver(w->menu, REQ_DOWN_ITEM);
            break;
        case LIA_MENU_SAVE:
            data_to_config(conf, &w->data);
            config_to_data(conf, &w->data);
            menu_driver(w->menu, REQ_DOWN_ITEM);
            break;
        case LIA_MENU_EXIT:
            r = -1;
            break;
        }
        break;
    }
    case 0x14a:
    case 0x7f: // delete
        ui_del_handler(w);
        break;
    case 0x08: // backspace
    case KEY_BACKSPACE:
    case 'A' ... 'z':
    case '0' ... '9':
    case '.':
    case '-':
        ui_char_handler(w, ch);
        break;
    default:
        break;
    }
    show_panel_data(w->win, &w->data);
    show_cursor(w->win, item_index(current_item(w->menu)), &w->data);
    wrefresh(w->win);
    pos_menu_cursor(w->menu);
    return r;
}

void destroy_sweep_window(SweepWindow *w)
{
    unpost_menu(w->menu);
    for (int i = 0; i < n_choices; ++i)
    {
        free_item(w->items[i]);
    }
    free_menu(w->menu);
    werase(w->win);
    wrefresh(w->win);
    delwin(w->win);
    return;
}

static void ui_left_handler(SweepWindow *w)
{
    if (item_index(current_item(w->menu)) == LIA_MENU_SLOPE)
    {
        if (w->data.slop == LIA_SLOP_OFF)
        {
            return;
        }
        else
        {
            w->data.slop = w->data.slop - 6; // 6 is super special magical number that makes it work
        }
    }
    else if (item_index(current_item(w->menu)) == LIA_MENU_DYRESV)
    {
        if (w->data.dr == LIA_DR_LOW1)
        {
            return;
        }
        else
        {
            w->data.dr = w->data.dr - 1; // 1 is super special magical number that makes it work
        }
    }
}
static void ui_right_handler(SweepWindow *w)
{
    if (item_index(current_item(w->menu)) == LIA_MENU_SLOPE)
    {
        if (w->data.slop == LIA_SLOP_24)
        {
            return;
        }
        else
        {
            w->data.slop = w->data.slop + 6; // 6 is super special magical number that makes it work
        }
    }
    else if (item_index(current_item(w->menu)) == LIA_MENU_DYRESV)
    {
        if (w->data.dr == LIA_DR_HIGH)
        {
            return;
        }
        else
        {
            w->data.dr = w->data.dr + 1; // 1 is super special magical number that makes it work
        }
    }
}

static void ui_del_handler(SweepWindow *w)
{
    ITEM *cur = current_item(w->menu);
    switch (item_index(cur))
    {
    case LIA_MENU_RANGE:
        w->data.index_range = 0;
        w->data.range[0] = 0x00;
        break;
    case LIA_MENU_TIMECON:
        w->data.index_tcon = 0;
        w->data.tcon[0] = 0x00;
        break;
    case LIA_MENU_MOVAVG:
        w->data.index_mov = 0;
        w->data.mov[0] = 0x00;
        break;
    default:
        break;
    }
}

static void ui_char_handler(SweepWindow *w, int ch)
{
    ITEM *cur = current_item(w->menu);
    switch (item_index(cur))
    {
    case LIA_MENU_RANGE:
        if (ch == 0x08 || ch == KEY_BACKSPACE)
        {
            if (w->data.index_range > 0)
            {
                w->data.index_range--;
                w->data.range[w->data.index_range] = 0x00;
            }
        }
        else
        {
            if (w->data.index_range < (MAX_FORM_STRINGS - 1))
            {
                w->data.range[w->data.index_range] = ch;
                w->data.index_range++;
            }
        }
        break;
    default:
        break;
    }
}

static void show_panel_data(WINDOW *w, const sweep_ui_data *d)
{

    const char eraser[21] = "                     ";
    // range
    mvwaddnstr(w, 1, COLS / 8 + 3, eraser, MAX_FORM_STRINGS);
    mvwaddnstr(w, 1, COLS / 8 + 1, ": ", 2);
    mvwaddnstr(w, 1, COLS / 8 + 3, d->range, d->index_range);
    // timecon
    mvwaddnstr(w, 2, COLS / 8 + 3, eraser, MAX_FORM_STRINGS);
    mvwaddnstr(w, 2, COLS / 8 + 1, ": ", 2);
    mvwaddnstr(w, 2, COLS / 8 + 3, d->tcon, d->index_tcon);

    // slope
    mvwaddstr(w, 3, COLS / 8 + 1, "                        ");
    mvwaddstr(w, 3, COLS / 8 + 1, ": OFF : 6 : 12 : 18 : 24");
    wattron(w, A_REVERSE | A_UNDERLINE | A_ITALIC);
    switch (d->slop)
    {
    case LIA_SLOP_OFF:
        mvwaddnstr(w, 3, COLS / 8 + 3, "OFF", 3);
        break;
    case LIA_SLOP_6:
        mvwaddnstr(w, 3, COLS / 8 + 9, "6", 1);
        break;
    case LIA_SLOP_12:
        mvwaddnstr(w, 3, COLS / 8 + 13, "12", 2);
        break;
    case LIA_SLOP_18:
        mvwaddnstr(w, 3, COLS / 8 + 18, "18", 2);
        break;
    case LIA_SLOP_24:
        mvwaddnstr(w, 3, COLS / 8 + 23, "24", 2);
        break;
    }
    wattroff(w, A_REVERSE | A_UNDERLINE | A_ITALIC);
    // movavg
    mvwaddnstr(w, 4, COLS / 8 + 3, eraser, MAX_FORM_STRINGS);
    mvwaddnstr(w, 4, COLS / 8 + 1, ": ", 2);
    mvwaddnstr(w, 4, COLS / 8 + 3, d->mov, d->index_mov);

    // dyresv
    mvwaddstr(w, 5, COLS / 8 + 1, "                          ");
    mvwaddstr(w, 5, COLS / 8 + 1, ": LOW1 : LOW2 : MID : HIGH");
    wattron(w, A_REVERSE | A_UNDERLINE | A_ITALIC);
    switch (d->dr)
    {
    case LIA_DR_LOW1:
        mvwaddnstr(w, 5, COLS / 8 + 3, "LOW1", 4);
        break;
    case LIA_DR_LOW2:
        mvwaddnstr(w, 5, COLS / 8 + 10, "LOW2", 4);
        break;
    case LIA_DR_MID:
        mvwaddnstr(w, 5, COLS / 8 + 17, "MID", 3);
        break;
    case LIA_DR_HIGH:
        mvwaddnstr(w, 5, COLS / 8 + 23, "HIGH", 4);
        break;
    }
    wattroff(w, A_REVERSE | A_UNDERLINE | A_ITALIC);
}

static void show_cursor(WINDOW *w, SweepMenuIndex i, const sweep_ui_data *d)
{
    attrset(A_REVERSE);
    switch (i)
    {
    case LIA_MENU_RANGE:
        mvwaddnstr(w, 1, COLS / 8 + 3 + d->index_range, "_", 1);
        mvwaddnstr(w, 1, COLS / 8 + 4 + d->index_range, " ", 1);
        break;
    case LIA_MENU_TIMECON:
        mvwaddnstr(w, 2, COLS / 8 + 3 + d->index_tcon, "_", 1);
        mvwaddnstr(w, 2, COLS / 8 + 4 + d->index_tcon, " ", 1);
        break;
    case LIA_MENU_MOVAVG:
        mvwaddnstr(w, 4, COLS / 8 + 3 + d->index_mov, "_", 1);
        mvwaddnstr(w, 4, COLS / 8 + 4 + d->index_mov, " ", 1);
        break;
    default:
        break;
    }
}