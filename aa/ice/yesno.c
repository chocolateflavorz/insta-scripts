#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "./../headers/yesno.h"

static size_t linelen(const char *s)
{
    size_t len = 1;
    for (size_t i = 0; s[i] != 0x00; i++)
    {
        if (s[i] == 0x0a)
        {
            len++;
        }
    }
    return len;
}

static size_t onelinelen(const char *s)
{
    size_t i = 0;
    for (; s[i] != 0x00 && s[i] != 0x0a; i++)
        ;
    return i;
}

YesNo yesno(const char *msg)
{
    WINDOW *msg_win;
    int msg_len = onelinelen(msg);
    int msg_win_h = linelen(msg) + 4;
    int msg_win_w = msg_len + 4;
    int msg_win_y = (LINES - msg_win_h) / 2;
    int msg_win_x = (COLS - msg_win_w) / 2;
    YesNo ret = NO;

    msg_win = newwin(msg_win_h, msg_win_w, msg_win_y, msg_win_x);
    keypad(msg_win, TRUE);
    mvwaddstr(msg_win, 1, 1, msg);
    mvwaddstr(msg_win, msg_win_h - 1, msg_win_w / 2 - 5, "YES");
    mvwaddstr(msg_win, msg_win_h - 1, msg_win_w / 2 - 2, " : ");
    wattron(msg_win, A_REVERSE | A_UNDERLINE | A_BOLD | A_ITALIC);
    mvwaddstr(msg_win, msg_win_h - 1, msg_win_w / 2 + 1, "NO");
    wattroff(msg_win, A_REVERSE | A_UNDERLINE | A_BOLD | A_ITALIC);
    wrefresh(msg_win);

    int c;
    while ((c = wgetch(msg_win)) != 10)
    {
        if (c == KEY_RIGHT || c == KEY_LEFT)
        {
            ret = !ret;
        }
        if (c == KEY_ENTER || c == 0x0a) // 0x0a = 10 = 0x0d
        {
            break;
        }
        if (ret)
        {
            wattron(msg_win, A_REVERSE | A_UNDERLINE | A_BOLD | A_ITALIC);
            mvwaddstr(msg_win, msg_win_h - 1, msg_win_w / 2 - 5, "YES");
            wattroff(msg_win, A_REVERSE | A_UNDERLINE | A_BOLD | A_ITALIC);
            mvwaddstr(msg_win, msg_win_h - 1, msg_win_w / 2 - 2, " : ");
            mvwaddstr(msg_win, msg_win_h - 1, msg_win_w / 2 + 1, "NO");
        }
        else
        {
            mvwaddstr(msg_win, msg_win_h - 1, msg_win_w / 2 - 5, "YES");
            mvwaddstr(msg_win, msg_win_h - 1, msg_win_w / 2 - 2, " : ");
            wattron(msg_win, A_REVERSE | A_UNDERLINE | A_BOLD | A_ITALIC);
            mvwaddstr(msg_win, msg_win_h - 1, msg_win_w / 2 + 1, "NO");
            wattroff(msg_win, A_REVERSE | A_UNDERLINE | A_BOLD | A_ITALIC);
        }
    }
    wclear(msg_win);
    wrefresh(msg_win);
    delwin(msg_win);
    return ret;
}