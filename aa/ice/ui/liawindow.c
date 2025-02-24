
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
    LIA_MENU_RANGE = 0,
    LIA_MENU_TIMECON = 1,
    LIA_MENU_SLOPE = 2,
    LIA_MENU_MOVAVG = 3,
    LIA_MENU_DYRESV = 4,
    LIA_MENU_SAVE = 5,
    LIA_MENU_EXIT = 6,
} LiaMenuIndex;

#define n_choices (sizeof(choices) / sizeof(choices[0]))
static const char *choices[] = {
    "Range   ",
    "TimeCon ",
    "Slope   ",
    "MovAvg  ",
    "DyResv  ",
    "Save    ",
    "Cancel  ",
};

static void show_panel_data(WINDOW *w, const lia_ui_data *d);
static void show_cursor(WINDOW *w, LiaMenuIndex i, const lia_ui_data *d);
static void ui_left_handler(LiaWindow *lw);
static void ui_right_handler(LiaWindow *lw);
static void ui_del_handler(LiaWindow *lw);
static void ui_char_handler(LiaWindow *lw, int ch);

static float from_string_to_movavg(const char *s)
{
    for (size_t i = 0; s[i] != 0x00; i++)
    {
        if (s[i] == 'A')
        {
            return LIA_SET_AUTO;
        }
        if (s[i] == 'O')
        {
            return LIA_SET_OFF;
        }
    }
    return strtof(s, NULL);
}

static unsigned char from_double_to_range_index(double d)
{
    for (size_t i = 0; i < sizeof(range_data) / sizeof(range_data[0]); i++)
    {
        if (d == range_data[i].range)
        {
            return i;
        }
    }
    return 0;
}
static void data_to_config(LiaConfig *conf, lia_ui_data *data)
{
    conf->dr = data->dr;
    conf->slop = data->slop;
    conf->tcon = strtof(data->tcon, NULL);
    conf->mov = from_string_to_movavg(data->mov);
    conf->range = range_data[data->index_range].range;
}
static void config_to_data(LiaConfig *conf, lia_ui_data *data)
{
    data->dr = conf->dr;
    data->slop = conf->slop;
    data->index_range = from_double_to_range_index(conf->range);
    snprintf(data->tcon, MAX_FORM_STRINGS, FORMAT_LIA_TCON, conf->tcon);
    data->index_tcon = strnlen(data->tcon, MAX_FORM_STRINGS);
    lia_mov_to_string(data->mov, conf->mov);
    data->index_mov = strnlen(data->mov, MAX_FORM_STRINGS);
}

void create_lia_window(LiaWindow *lw, LiaConfig *conf)
{
    config_to_data(conf, &lw->data);
    lw->items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
    for (int i = 0; i < n_choices; i++)
    {
        lw->items[i] = new_item(choices[i], NULL);
    }
    lw->items[n_choices] = (ITEM *)NULL;
    lw->menu = new_menu(lw->items);

    lw->win = newwin(LINES - 5, COLS / 2 - 1, 0, COLS / 2 + 1);
    keypad(lw->win, TRUE);
    set_menu_win(lw->menu, lw->win);
    set_menu_sub(lw->menu, derwin(lw->win, LINES - 7, COLS / 8, 1, 1));
    set_menu_format(lw->menu, n_choices, 1);
    set_menu_mark(lw->menu, ">");
    box(lw->win, 0, 0);
    post_menu(lw->menu);
    show_panel_data(lw->win, &lw->data);
    wrefresh(lw->win);
}

int lia_menu_handler(int ch, LiaWindow *lw, LiaConfig *conf)
{
    int r = 0;
    switch (ch)
    {
    case KEY_DOWN:
        menu_driver(lw->menu, REQ_DOWN_ITEM);
        break;
    case KEY_UP:
        menu_driver(lw->menu, REQ_UP_ITEM);
        break;
    case KEY_LEFT:
        ui_left_handler(lw);
        break;
    case KEY_RIGHT:
        ui_right_handler(lw);
        break;
    case KEY_ENTER:
    case 0x0a:
    {
        switch (item_index(current_item(lw->menu)))
        {
        case LIA_MENU_RANGE ... LIA_MENU_DYRESV: // range, timecon, slope, movavg, dyresv
            menu_driver(lw->menu, REQ_DOWN_ITEM);
            break;
        case LIA_MENU_SAVE:
            data_to_config(conf, &lw->data);
            config_to_data(conf, &lw->data);
            menu_driver(lw->menu, REQ_DOWN_ITEM);
            break;
        case LIA_MENU_EXIT:
            r = -1;
            break;
        }
        break;
    }
    case 0x14a:
    case 0x7f: // delete
        ui_del_handler(lw);
        break;
    case 0x08: // backspace
    case KEY_BACKSPACE:
    case 'A' ... 'z':
    case '0' ... '9':
    case '.':
    case '-':
        ui_char_handler(lw, ch);
        break;
    default:
        break;
    }
    pos_menu_cursor(lw->menu);
    show_panel_data(lw->win, &lw->data);
    show_cursor(lw->win, item_index(current_item(lw->menu)), &lw->data);
    wrefresh(lw->win);
    return r;
}

void destroy_lia_window(LiaWindow *lw)
{
    unpost_menu(lw->menu);
    for (int i = 0; i < n_choices; ++i)
    {
        free_item(lw->items[i]);
    }
    free_menu(lw->menu);
    werase(lw->win);
    wrefresh(lw->win);
    delwin(lw->win);
    return;
}

static void ui_left_handler(LiaWindow *lw)
{
    switch (item_index(current_item(lw->menu)))
    {
    case LIA_MENU_RANGE:
    {
        if (lw->data.index_range == (sizeof(range_data) / sizeof(range_data[0]) - 1))
        {
            return;
        }
        else
        {
            lw->data.index_range = lw->data.index_range + 1;
        }
        break;
    }
    case LIA_MENU_SLOPE:
    {
        if (lw->data.slop == LIA_SLOP_OFF)
        {
            return;
        }
        else
        {
            lw->data.slop = lw->data.slop - 6; // 6 is super special magical number that makes it work
        }
    }
    case LIA_MENU_DYRESV:
    {
        if (lw->data.dr == LIA_DR_LOW1)
        {
            return;
        }
        else
        {
            lw->data.dr = lw->data.dr - 1; // 1 is super special magical number that makes it work
        }
    }
    }
}

static void ui_right_handler(LiaWindow *lw)
{
    switch (item_index(current_item(lw->menu)))
    {
    case LIA_MENU_RANGE:
        if (lw->data.index_range == 0)
        {
            return;
        }
        else
        {
            lw->data.index_range = lw->data.index_range - 1;
        }
        break;
    case LIA_MENU_SLOPE:
        if (lw->data.slop == LIA_SLOP_24)
        {
            return;
        }
        else
        {
            lw->data.slop = lw->data.slop + 6; // 6 is super special magical number that makes it work
        }
        break;
    case LIA_MENU_DYRESV:
        if (lw->data.dr == LIA_DR_HIGH)
        {
            return;
        }
        else
        {
            lw->data.dr = lw->data.dr + 1; // 1 is super special magical number that makes it work
        }
        break;
    }
}

static void ui_del_handler(LiaWindow *lw)
{
    ITEM *cur = current_item(lw->menu);
    switch (item_index(cur))
    {
    case LIA_MENU_TIMECON:
        lw->data.index_tcon = 0;
        lw->data.tcon[0] = 0x00;
        break;
    case LIA_MENU_MOVAVG:
        lw->data.index_mov = 0;
        lw->data.mov[0] = 0x00;
        break;
    default:
        break;
    }
}

static void ui_char_handler(LiaWindow *lw, int ch)
{
    ITEM *cur = current_item(lw->menu);
    switch (item_index(cur))
    {
    case LIA_MENU_TIMECON:
        if (ch == 0x08 || ch == KEY_BACKSPACE)
        {
            if (lw->data.index_tcon > 0)
            {
                lw->data.index_tcon--;
                lw->data.tcon[lw->data.index_tcon] = 0x00;
            }
        }
        else
        {
            if (lw->data.index_tcon < (MAX_FORM_STRINGS - 1))
            {
                lw->data.tcon[lw->data.index_tcon] = ch;
                lw->data.index_tcon++;
            }
        }
        break;
    case LIA_MENU_MOVAVG:
        if (ch == 0x08 || ch == KEY_BACKSPACE)
        {
            if (lw->data.index_mov > 0)
            {
                lw->data.index_mov--;
                lw->data.mov[lw->data.index_mov] = 0x00;
            }
        }
        else
        {
            if (lw->data.index_mov < (MAX_FORM_STRINGS - 1))
            {
                lw->data.mov[lw->data.index_mov] = ch;
                lw->data.index_mov++;
            }
        }
        break;
    default:
        break;
    }
}

static void show_panel_data(WINDOW *w, const lia_ui_data *d)
{

    const char eraser[21] = "                     ";
    // range
    mvwaddnstr(w, 1, COLS / 8 + 3, eraser, MAX_FORM_STRINGS);
    mvwaddnstr(w, 1, COLS / 8 + 1, ": < ", 4);
    mvwaddstr(w, 1, COLS / 8 + 5, range_data[d->index_range].str);
    mvwaddnstr(w, 1, COLS / 8 + 10, " >", 2);
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

static void show_cursor(WINDOW *w, LiaMenuIndex i, const lia_ui_data *d)
{
    wattron(w, A_REVERSE);
    switch (i)
    {
    case LIA_MENU_RANGE:
        mvwaddstr(w, 1, COLS / 8 + 5, range_data[d->index_range].str);
        wattroff(w, A_REVERSE);
        break;
    case LIA_MENU_TIMECON:
        mvwaddnstr(w, 2, COLS / 8 + 3 + d->index_tcon, "_", 1);
        wattroff(w, A_REVERSE);
        mvwaddnstr(w, 2, COLS / 8 + 4 + d->index_tcon, " ", 1);
        break;
    case LIA_MENU_MOVAVG:
        mvwaddnstr(w, 4, COLS / 8 + 3 + d->index_mov, "_", 1);
        wattroff(w, A_REVERSE);
        mvwaddnstr(w, 4, COLS / 8 + 4 + d->index_mov, " ", 1);
        break;
    default:
        wattroff(w, A_REVERSE);
        break;
    }
}