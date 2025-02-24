#include <ncurses.h>
#include "./../../headers/main.h"



void footer()
{
	attrset(COLOR_PAIR(2));
	mvaddstr(LINES - 1, 0, "(F1 to Exit) 'stty sane' or 'tput init' for broken term");
	refresh();
}

void print_term(const char *s)
{
	attron(A_UNDERLINE);
	move(LINES - 5, 0);
	clrtoeol();
	mvaddstr(LINES - 5, 0, s);
	attroff(A_UNDERLINE);
	refresh();
	return;
}

void print_term_eq(int i, const char *s)
{
	attron(COLOR_PAIR(4));
	move(LINES - 4 + i, 0);
	clrtoeol();
	mvaddstr(LINES - 4 + i, 0, short_name[i]);
	mvaddstr(LINES - 4 + i, 7, s);
	attroff(COLOR_PAIR(4));
	refresh();
	return;
}
