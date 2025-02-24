

#ifndef MENUWIN_H
#define MENUWIN_H

#define MENUWIN_EXIT -1

#ifdef __cplusplus 
extern "C" {
#endif

// return -1 to exit the loop
typedef int (*ITEM_CALLBACK)();

int menu_window(const int n_choices, const char *choices[], const char *descriptions[], const ITEM_CALLBACK callbacks[], const window_data *w_data);

#ifdef __cplusplus
}
#endif

#endif