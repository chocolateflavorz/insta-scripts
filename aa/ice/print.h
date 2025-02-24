#ifndef PRINT_H
#define PRINT_H

#include <ncurses.h>
#include <string.h>

static void print_in_middle(WINDOW *win, int y, const char *str, chtype color)
{
    if (win == NULL)
        win = stdscr;

    size_t length;
    int x, width;

    width = getmaxx(win);
    length = strnlen(str, width - 4);
    x = (width - length) / 2;
    wattron(win, color);
    mvwaddnstr(win, y, x, str, length);
    wattroff(win, color);
}

#endif