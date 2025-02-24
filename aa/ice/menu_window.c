#include <stdlib.h>
#include <ncurses.h>
#include <menu.h>


#include "../headers/window_data.h"
#include "../headers/menu_window.h"

int menu_window(const int n_choices, const char *choices[], const char *descriptions[], const ITEM_CALLBACK callbacks[], const window_data *w_data)
{
	ITEM **menu_items;
	MENU *menu;
	WINDOW *menu_win;

	menu_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
	for (int i = 0; i < n_choices; i++)
	{
		menu_items[i] = new_item(choices[i], descriptions[i]);
		set_item_userptr(menu_items[i], callbacks[i]);
	}
	menu_items[n_choices] = (ITEM *)NULL;
	menu = new_menu((ITEM **)menu_items);

	menu_win = newwin(w_data->w, w_data->h, w_data->y, w_data->x);
	keypad(menu_win, TRUE);
	set_menu_win(menu, menu_win);
	set_menu_sub(menu, derwin(menu_win, w_data->w-2, w_data->h-2, w_data->x+1, w_data->y+1));
	set_menu_format(menu, 12, 1);
	set_menu_mark(menu, " * ");
	box(menu_win, 0, 0);
	post_menu(menu);
	wrefresh(menu_win);

	int c;
	while ((c = wgetch(menu_win)) != KEY_F(1))
	{
		switch (c)
		{
		case KEY_DOWN:
			menu_driver(menu, REQ_DOWN_ITEM);
			break;
		case KEY_UP:
			menu_driver(menu, REQ_UP_ITEM);
			break;
		case KEY_ENTER:
		case 10:  // ENTER
		{
			ITEM_CALLBACK p = item_userptr(current_item(menu));
			if (p == NULL)
			{
				// callback is undefined
				goto $end_menu;
			}
			int r = p();
			if (r == MENUWIN_EXIT) 
			{
				// callback want to exit this loop
				goto $end_menu;
			}
			break;
		}
		default:
			break;
		}
		mvwprintw(menu_win, w_data->h-2, 1, ":%s", item_description(current_item(menu)));
		pos_menu_cursor(menu);
	}
$end_menu:
	unpost_menu(menu);
	for (int i = 0; i < n_choices; ++i)
	{
		free_item(menu_items[i]);
	}
	free_menu(menu);
	werase(menu_win);
	wrefresh(menu_win);
	delwin(menu_win);
	return 0;
}