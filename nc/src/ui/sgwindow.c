
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
    SG_MENU_FREQ = 0,
    SG_MENU_AMP = 1,
    SG_MENU_OUT = 2,
    SG_MENU_SAVE = 3,
    SG_MENU_EXIT = 4,
} SgMenuIndex;
#define n_choices (sizeof(choices) / sizeof(choices[0]))
static const char *choices[] = {
    "Freq   ",
    "Amp    ",
    "Out    ",
    "Save   ",
    "Cancel ",
};

static void show_panel_data(WINDOW *w, const sg_ui_data *d);
static void show_cursor(WINDOW *w, SgMenuIndex i, const sg_ui_data *d);
static void data_to_config(SgConfig *conf, sg_ui_data *data)
{
    conf->freq = from_string_to_double(data->freq);
    conf->amplitude = atof(data->amp);
    conf->output = data->output;
}
static void config_to_data(SgConfig *conf, sg_ui_data *data)
{
    from_double_to_string(conf->freq, data->freq, FORMAT_SG_FREQUENCY, MAX_FORM_STRINGS);
    data->index_freq = strnlen(data->freq, MAX_FORM_STRINGS);
    snprintf(data->amp, MAX_FORM_STRINGS, FORMAT_SG_AMPLITUDE, conf->amplitude);
    data->index_amp = strnlen(data->amp, MAX_FORM_STRINGS);
    data->output = conf->output;
}

void create_sg_window(SgWindow *sw, SgConfig *conf)
{
    config_to_data(conf, &sw->data);
    sw->items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
    for (int i = 0; i < n_choices; i++)
    {
        sw->items[i] = new_item(choices[i], NULL);
    }
    sw->items[n_choices] = (ITEM *)NULL;
    sw->menu = new_menu(sw->items);

    sw->win = newwin(LINES - 5, COLS / 2 - 1, 0, COLS / 2 + 1);
    keypad(sw->win, TRUE);
    set_menu_win(sw->menu, sw->win);
    set_menu_sub(sw->menu, derwin(sw->win, LINES - 7, COLS / 8, 1, 1));
    set_menu_format(sw->menu, n_choices, 1);
    set_menu_mark(sw->menu, ">");
    box(sw->win, 0, 0);
    post_menu(sw->menu);
    show_panel_data(sw->win, &sw->data);
    wrefresh(sw->win);
}

int sg_menu_handler(int ch, SgWindow *sw, SgConfig *conf)
{
    int r = 0;
    switch (ch)
    {
    case KEY_DOWN:
        menu_driver(sw->menu, REQ_DOWN_ITEM);
        break;
    case KEY_UP:
        menu_driver(sw->menu, REQ_UP_ITEM);
        break;
    case KEY_LEFT:
    case KEY_RIGHT:
        if (item_index(current_item(sw->menu)) == SG_MENU_OUT)
        {
            sw->data.output = !sw->data.output;
        }
        break;
    case KEY_ENTER:
    case 0x0a:
    {
        switch (item_index(current_item(sw->menu)))
        {
        case 0 ... 2: // FREQ, AMP, OUT
            menu_driver(sw->menu, REQ_DOWN_ITEM);
            break;
        case SG_MENU_SAVE:
            data_to_config(conf, &sw->data);
            config_to_data(conf, &sw->data);
            menu_driver(sw->menu, REQ_DOWN_ITEM);
            break;
        case SG_MENU_EXIT:
            r = -1;
            break;
        }
        break;
    }
    case 0x14a:
    case 0x7f: // delete
        switch (item_index(current_item(sw->menu)))
        {
        case 0:
            sw->data.index_freq = 0;
            sw->data.freq[0] = 0x00;
            break;
        case 1:
            sw->data.index_amp = 0;
            sw->data.amp[0] = 0x00;
            break;
        }
        break;
    case 0x08: // backspace
    case KEY_BACKSPACE:
    case 'A' ... 'z':
    case '0' ... '9':
    case '.':
    case '-':
    {
        ITEM *cur = current_item(sw->menu);
        switch (item_index(cur))
        {
        case SG_MENU_FREQ:
            if (ch == 0x08 || ch == KEY_BACKSPACE)
            {
                if (sw->data.index_freq > 0)
                {
                    sw->data.index_freq--;
                    sw->data.freq[sw->data.index_freq] = 0x00;
                }
            }
            else
            {
                if (sw->data.index_freq < (MAX_FORM_STRINGS - 1))
                {
                    sw->data.freq[sw->data.index_freq] = ch;
                    sw->data.index_freq++;
                }
            }
            break;
        case SG_MENU_AMP:
            if (ch == 0x08 || ch == KEY_BACKSPACE)
            {
                if (sw->data.index_amp > 0)
                {
                    sw->data.index_amp--;
                    sw->data.amp[sw->data.index_amp] = 0x00;
                }
            }
            else
            {
                if (sw->data.index_amp < (MAX_FORM_STRINGS - 1))
                {
                    sw->data.amp[sw->data.index_amp] = ch;
                    sw->data.index_amp++;
                }
            }
            break;
        case SG_MENU_OUT:
            sw->data.output = !sw->data.output;
            break;
        default:
            break;
        }
    }
    default:
        break;
    }
    pos_menu_cursor(sw->menu);
    show_panel_data(sw->win, &sw->data);
    show_cursor(sw->win, item_index(current_item(sw->menu)), &sw->data);
    wrefresh(sw->win);
    return r;
}

void destroy_sg_window(SgWindow *sw)
{
    unpost_menu(sw->menu);
    for (int i = 0; i < n_choices; ++i)
    {
        free_item(sw->items[i]);
    }
    free_menu(sw->menu);
    werase(sw->win);
    wrefresh(sw->win);
    delwin(sw->win);
    return;
}

static void show_panel_data(WINDOW *w, const sg_ui_data *d)
{

    const char eraser[21] = "                    ";
    // freq
    mvwaddnstr(w, 1, COLS / 8 + 3, eraser, MAX_FORM_STRINGS);
    mvwaddnstr(w, 1, COLS / 8 + 1, ": ", 2);
    mvwaddnstr(w, 1, COLS / 8 + 3, d->freq, d->index_freq);
    // amp
    mvwaddnstr(w, 2, COLS / 8 + 3, eraser, MAX_FORM_STRINGS);
    mvwaddnstr(w, 2, COLS / 8 + 1, ": ", 2);
    mvwaddnstr(w, 2, COLS / 8 + 3, d->amp, d->index_amp);
    // out
    mvwaddstr(w, 3, COLS / 8 + 1, ": ON : OFF");
    wattron(w, A_REVERSE | A_UNDERLINE | A_ITALIC);
    if (d->output)
    {
        mvwaddnstr(w, 3, COLS / 8 + 3, "ON", 2);
    }
    else
    {
        mvwaddnstr(w, 3, COLS / 8 + 8, "OFF", 3);
    }
    wattroff(w, A_REVERSE | A_UNDERLINE | A_ITALIC);
}

static void show_cursor(WINDOW *w, SgMenuIndex i, const sg_ui_data *d)
{
    attrset(A_REVERSE);
    switch (i)
    {
    case SG_MENU_FREQ:
        mvwaddnstr(w, 1, COLS / 8 + 3+d->index_freq, "_",1);
        mvwaddnstr(w, 1, COLS / 8 + 4+d->index_freq, " ",1);
        break;
    case SG_MENU_AMP:
        mvwaddnstr(w, 2, COLS / 8 + 3+d->index_amp, "_",1);
        mvwaddnstr(w, 2, COLS / 8 + 4+d->index_amp, " ",1);
        break;
    default:
        break;
    }
    wrefresh(w);
}