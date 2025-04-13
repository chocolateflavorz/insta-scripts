#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <form.h>

#include "./../headers/form_dialog.h"
#include "./../headers/message_dialog.h"
#include "./../headers/print.h"

int form_dialog(const char *title, const char *msg, char *buffer, int buffer_size)
{
    if (title == NULL)
        title = "";

    if (msg == NULL)
        msg = "";

    if (buffer == NULL)
        buffer_size = 0;

    FIELD *field[2];
    FORM *form;
    WINDOW *win;
    int ch, rows, cols;

    field[0] = new_field(1, COLS /2, 4, 1, 0, 0);
    field[1] = NULL;

    set_field_back(field[0], A_UNDERLINE);
    field_opts_off(field[0], O_AUTOSKIP);
    form = new_form(field);
    scale_form(form, &rows, &cols);
    win = newwin(rows + 3, cols + 2, (LINES/2-rows), (COLS-cols)/2);
    keypad(win, TRUE);

    set_form_win(form, win);
    set_form_sub(form, derwin(win, rows, cols, 2, 1));

    box(win, 0, 0);
    print_in_middle(win, 0,title, COLOR_PAIR(1));
    print_in_middle(win, 1, msg, COLOR_PAIR(2));
    post_form(form);
    refresh();
    wrefresh(win);
    pos_form_cursor(form);
    refresh();

    while ((ch = wgetch(win)) != KEY_F(1))
    {
        switch (ch)
        {
        case KEY_DOWN:
            form_driver(form, REQ_NEXT_FIELD);
            form_driver(form, REQ_END_LINE);
            break;
        case KEY_UP:
            form_driver(form, REQ_PREV_FIELD);
            form_driver(form, REQ_END_LINE);
            break;
        case KEY_ENTER:
        case 10:  //Enter
            form_driver(form, REQ_END_FIELD);
            strncpy(buffer, field_buffer(field[0], 0), buffer_size);
            goto $end_form;
            break;
        case KEY_BACKSPACE:
        case 8: // Backspace
            form_driver(form, REQ_DEL_PREV);
            break;
        default:
            form_driver(form, ch);
            break;
        }
    }

$end_form:
    /* Un post form and free the memory */
    unpost_form(form);
    free_form(form);
    free_field(field[0]);
    free_field(field[1]);

    wclear(win);
    wrefresh(win);
    delwin(win);
    return cols;
}

