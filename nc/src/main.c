#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <menu.h>
#include <panel.h>
#include <unistd.h>
#include <errno.h>

#include "./../headers/config.h"
#include "./../headers/ui.h"
#include "./../headers/con.h"
#include "./../headers/commandq.h"
#include "./../headers/format.h"
#include "./../headers/configloader.h"
#include "./../headers/main.h"

/*

WARNING: The code that follow may make you cry:
		  A Safety Cat has been provided below for your benefit

														   _...---.._
													   _.'`       -_ ``.
												   .-'`                 `.
												.-`                     q ;
											 _-`                       __  \
										 .-'`                  . ' .   \ `;/
									 _.-`                    /.      `._`/
							 _...--'`                        \_`..._
						  .'`                         -         `'--:._
					   .-`                           \                  `-.
					  .                `              `-..__.....----...., `.
					 '                 `  '''---..-''`'              : :  : :
				   .` -                '`.  `-.                       `'   `'
				.-` .` '             .`'.__ ;
			_.-` .-`   '            .
		_.-` _.-`    .' '         .`
(`''--'' _.-`      .'  '        .'
 `'----''        .'  .`       .`
			   .'  .'     .-'`
			 .'   :    .-`
			 `. .`   ,`
			  .'   .'
			 '   .`
			'  .`
			`  '.
			`.___;


*/

int main()
{

	SgConfig config_5025 = DEFAULT_SGCONFIG_INITIALIZER;
    SgConfig config_5026 = DEFAULT_SGCONFIG_INITIALIZER;
	LiaConfig config_lia = DEFAULT_LIACONFIG_INITIALIZER;

struct
{
	ui_state_t state;
	PANEL *p_main;
	PANEL *p_status;
	PANEL *p_eq[EQNUM];
	WINDOW *w_main;
	MENU *m_main;
	ITEM **i_main;
	WINDOW *w_status;
	SgWindow sg5025;
	SgWindow sg5026;
	LiaWindow lia;
} ui;

 bool is_available[EQNUM] = {false, false, false};

 LiaMeasurements lia_meas_state = {
	.r_a = 0.0,
	.phase_a = 0.0,
	.r_b = 0.0,
	.phase_b = 0.0,
	.status_flag = 0,
};

CommandQ queue;
con_params_t connections[EQNUM] = {
	{
		.con = (Con)DEFAULT_CON_INITIALIZER,
		.data = {
#ifdef DEBUG
			.ip_addr = "127.0.1.202",
#else
			.ip_addr = "192.168.3.202",
#endif
			.port = 5025,
			.delimiter = 0x0d,
			.recv_delimiter = '*',
		},
	},
	{
		.con = (Con)DEFAULT_CON_INITIALIZER,
		.data = {
			.ip_addr = "192.168.3.202",
			.port = 5026,
			.delimiter = 0x0d,
			.recv_delimiter = '*',
		},
	},
	{
		.con = (Con)DEFAULT_CON_INITIALIZER,
		.data = {
			.ip_addr = "192.168.3.201",
			.port = 5025,
			.delimiter = 0x0a,
			.recv_delimiter = 0x00,
		},
	}};

	initscr();
	start_color();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	set_colors();
	commandq_init(&queue);

	ui.state = MAIN_MENU;
	create_main_ui();
	create_status_window();
	create_sg_window(&ui.sg5025, &config_5025);
	create_sg_window(&ui.sg5026, &config_5026);
	create_lia_window(&ui.lia, &config_lia);
	init_panels();
	show_status();

	int ch;
	while ((ch = getch()) != KEY_F(1))
	{
		switch (ui.state)
		{
		case MAIN_MENU:
			show_panel(ui.p_status);
			ch = main_ui_handler(ch);
			if (ch == -1)
			{
				goto $end_app;
			}
			break;
		case SG5025_MENU:
			ch = sg_menu_handler(ch, &ui.sg5025, &config_5025);
			if (ch == -1)
			{
				state_for_main();
				break;
			}
			break;
		case SG5026_MENU:
			ch = sg_menu_handler(ch, &ui.sg5026, &config_5026);
			if (ch == -1)
			{
				state_for_main();
				break;
			}
			break;
		case LIA_MENU:
			ch = lia_menu_handler(ch, &ui.lia, &config_lia);
			if (ch == -1)
			{
				state_for_main();
				break;
			}
			break;
		default:
			break;
		}
		update_panels();
		doupdate();
		refresh();
	}

$end_app:
	destroy_all();
	endwin();
	for (int i = 0; i < EQNUM; ++i)
	{
		if (is_available[i] == true)
		{
			close_con(&connections[i].con);
		}
	}
	return 0;
}

static void set_colors()
{
	init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_CYAN, COLOR_BLACK);
	init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(5, COLOR_RED, COLOR_BLACK);
	init_pair(6, COLOR_YELLOW, COLOR_BLACK);
}

static void create_main_ui()
{
	int n_choices = ARRAY_SIZE(choices);
	ui.i_main = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
	for (int i = 0; i < n_choices; i++)
	{
		ui.i_main[i] = new_item(choices[i], descriptions[i]);
		set_item_userptr(ui.i_main[i], functions[i]);
	}
	ui.i_main[n_choices] = (ITEM *)NULL;
	ui.m_main = new_menu((ITEM **)ui.i_main);

	ui.w_main = newwin(LINES - 5, COLS / 2 - 1, 0, 0);
	keypad(ui.w_main, TRUE);
	set_menu_win(ui.m_main, ui.w_main);
	set_menu_sub(ui.m_main, derwin(ui.w_main, LINES - 7, COLS / 2 - 2, 1, 1));
	set_menu_format(ui.m_main, 12, 1);
	set_menu_mark(ui.m_main, "*");
	box(ui.w_main, 0, 0);
	footer();
	post_menu(ui.m_main);
	wrefresh(ui.w_main);
	return;
}

static void create_status_window()
{
	ui.w_status = newwin(LINES - 5, COLS / 2 - 1, 0, COLS / 2 + 1);
	wrefresh(ui.w_status);
	return;
}

static void show_status()
{
	char s[64];
	from_double_to_string(config_5025.freq, s, FORMAT_SG_FREQUENCY, 64);
	mvwprintw(ui.w_status, 1, 1, "CON? S5: %d, S6: %d, L: %d", is_available[SG5025], is_available[SG5026], is_available[LIA]);
	mvwprintw(ui.w_status, 2, 1, "S5 OUT: %s F: %s", config_5025.output ? "ON" : "OFF", s);
	mvwprintw(ui.w_status, 3, 1, "   AMP: " FORMAT_SG_AMPLITUDE, config_5025.amplitude);
	from_double_to_string(config_5026.freq, s, FORMAT_SG_FREQUENCY, 64);
	mvwprintw(ui.w_status, 4, 1, "S6 OUT: %s F: %s", config_5026.output ? "ON" : "OFF", s);
	mvwprintw(ui.w_status, 5, 1, "   AMP: " FORMAT_SG_AMPLITUDE, config_5026.amplitude);

	const char *dr = lia_dr_to_string(config_lia.dr);
	const char *slop = lia_slop_to_string(config_lia.slop);
	char tcon[16];
	char mov[16];

	snprintf(tcon, 16, FORMAT_LIA_TCON, config_lia.tcon);
	lia_mov_to_string(mov, config_lia.mov);
	mvwprintw(ui.w_status, 6, 1, "LIA DR: %s TCON: %s", dr, tcon);
	mvwprintw(ui.w_status, 7, 1, "    SLOP: %s MOV: %s", slop, mov);
	from_double_to_string(config_lia.range, s, FORMAT_LIA_RANGE, 64);
	wmvclr(ui.w_status, 8);
	mvwprintw(ui.w_status, 8, 1, "    RANGE: %s", s);

	wattron(ui.w_status, COLOR_PAIR(3) | A_BOLD);
	if (lia_meas_state.status_flag != 0)
	{
		wattron(ui.w_status, COLOR_PAIR(6));
	}
	from_double_to_string(lia_meas_state.r_a, s, FORMAT_LIA_R, 64);
	wmvclr(ui.w_status, 10);
	mvwprintw(ui.w_status, 10, 1, " ::R_A: %s", s);
	wmvclr(ui.w_status, 11);
	mvwprintw(ui.w_status, 11, 1, " ::P_A: " FORMAT_LIA_THETA, lia_meas_state.phase_a);
	from_double_to_string(lia_meas_state.r_b, s, FORMAT_LIA_R, 64);
	wmvclr(ui.w_status, 12);
	mvwprintw(ui.w_status, 12, 1, " ::R_B: %s", s);
	wmvclr(ui.w_status, 13);
	mvwprintw(ui.w_status, 13, 1, " ::P_B: " FORMAT_LIA_THETA, lia_meas_state.phase_b);
	wmvclr(ui.w_status, 14);
	print_lia_status_flag(lia_meas_state.status_flag, 14, 1);
	if (lia_meas_state.status_flag != 0)
	{
		wattroff(ui.w_status, COLOR_PAIR(6));
	}
	wattroff(ui.w_status, COLOR_PAIR(3) | A_BOLD);
	wrefresh(ui.w_status);
	return;
}

static void init_panels()
{
	ui.p_main = new_panel(ui.w_main);
	ui.p_status = new_panel(ui.w_status);
	ui.p_eq[SG5025] = new_panel(ui.sg5025.win);
	ui.p_eq[SG5026] = new_panel(ui.sg5026.win);
	ui.p_eq[LIA] = new_panel(ui.lia.win);

	for (int i = 0; i < EQNUM; ++i)
	{
		hide_panel(ui.p_eq[i]);
	}

	update_panels();
	doupdate();
}

static int state_for_main() {
	ui.state = MAIN_MENU;
	hide_panel(ui.p_eq[SG5025]);
	hide_panel(ui.p_eq[SG5026]);
	hide_panel(ui.p_eq[LIA]);
	show_status();
	show_panel(ui.p_status);
	return 0;

}

static int main_ui_handler(int ch)
{
	pos_menu_cursor(ui.m_main);
	switch (ch)
	{
	case KEY_DOWN:
		menu_driver(ui.m_main, REQ_DOWN_ITEM);
		break;
	case KEY_UP:
		menu_driver(ui.m_main, REQ_UP_ITEM);
		break;
	case KEY_ENTER:
	case 10:
	{
		ITEM *cur = current_item(ui.m_main);
		ITEM_CALLBACK p = item_userptr(cur);
		if (p == NULL)
		{
			// undefined behaviour
			return -1;
		}
		return p();
		break;
	}
	default:
		return ch;
	}
	return 0;
}

static void destroy_main_ui()
{
	unpost_menu(ui.m_main);
	for (int i = 0; i < ARRAY_SIZE(choices); ++i)
		free_item(ui.i_main[i]);
	free_menu(ui.m_main);
	werase(ui.w_main);
	wrefresh(ui.w_main);
	delwin(ui.w_main);
	return;
}

static void destroy_status_window()
{
	werase(ui.w_status);
	wrefresh(ui.w_status);
	delwin(ui.w_status);
	return;
}

static void destroy_all()
{
	del_panel(ui.p_main);
	del_panel(ui.p_status);
	for (int i = 0; i < EQNUM; ++i)
		del_panel(ui.p_eq[i]);
	destroy_sg_window(&ui.sg5025);
	destroy_sg_window(&ui.sg5026);
	destroy_lia_window(&ui.lia);
	destroy_main_ui();
	destroy_status_window();
	commandq_free(&queue);
	refresh();
}

static int state_for_exit()
{
	return -1;
}

static int state_for_sg5025()
{
	ui.state = SG5025_MENU;
	hide_panel(ui.p_status);
	show_panel(ui.p_eq[SG5025]);
	top_panel(ui.p_eq[SG5025]);
	return 0;
}

static int state_for_sg5026()
{
	ui.state = SG5026_MENU;
	hide_panel(ui.p_status);
	show_panel(ui.p_eq[SG5026]);
	top_panel(ui.p_eq[SG5026]);
	return 0;
}

static int state_for_lia()
{
	ui.state = LIA_MENU;
	hide_panel(ui.p_status);
	show_panel(ui.p_eq[LIA]);
	top_panel(ui.p_eq[LIA]);
	return 0;
}
static int state_for_lia_init()
{
	print_term_eq(LIA, "connecting...");
	try_connect_to_eq(LIA);
	if (is_available[LIA] == false)
	{
		attron(A_BOLD);
		print_term(strerror(errno));
		print_term_eq(LIA, "connection error");
		attroff(A_BOLD);
		return 0;
	}
	print_term_eq(LIA, "connected!");
	lia_initializer_to_queue(&queue);
	queue_to_device(LIA);
	return 0;
}

static int state_for_autoupdate()
{
	if (is_available[LIA] == false)
	{
		print_term("nothing for LIA");
		return 0;
	}
	print_term("query for LIA (F3 to abort)");
	timeout(1);
	while (true)
	{
		switch (getch())
		{
		case ERR:
			lia_fetch_to_queue(&queue);
			queue_to_device(LIA);
			lia_mdata_from_str(connections[LIA].con.receive_buffer, &lia_meas_state, &config_lia);
			show_status();
			break;
		case KEY_F(3):
			timeout(-1);
			print_term("aborted.");
			return 0;
		default:
			break;
		}
	}
	return 0;
}

static int command_sg5()
{
	print_term_eq(SG5025, "connecting...");
	try_connect_to_eq(SG5025);
	if (is_available[SG5025] == false)
	{
		attrset(A_BOLD);
		print_term(strerror(errno));
		print_term_eq(SG5025, "connection error");
		attroff(A_BOLD);
		return 0;
	}
	print_term_eq(SG5025, "connected!");
	dpl32_conf_to_queue(&config_5025, &queue);
	queue_to_device(SG5025);
	return 0;
}

static int command_lia()
{
	lia_conf_to_queue(&config_lia, &queue);
	queue_to_device(LIA);
	return 0;
}
static int not_implemented()
{
	print_term("Unimplemented feature");
	return 0;
}

/**
 * ToString
 */

static void print_lia_status_flag(int flag, int y, int x)
{
	wmove(ui.w_status, y, x);
	attron(COLOR_PAIR(5) | A_REVERSE);
	for (size_t i = 0; i < ARRAY_SIZE(lia_status_to_string); i++)
	{
		if (flag & lia_status_to_string[i].flag)
		{
			waddstr(ui.w_status, lia_status_to_string[i].str);
		}
	}
	attroff(COLOR_PAIR(5) | A_REVERSE);
}



/**
 * connection management
 */
void try_connect_to_eq(int i)
{
	if (is_available[i] == false)
	{
		if (open_con(&connections[i].con, connections[i].data.ip_addr,
					 connections[i].data.port, connections[i].data.delimiter, 3))
		{
			// perror(connections[LIA].data.ip_addr);
			return;
		}
		#ifdef DEBUG
		print_term(strerror(errno));
		#endif
		is_available[i] = true;
	}
	return;
}

void queue_to_device(int i)
{
	if (is_available[i] == false)
	{
		commandq_reset(&queue);
		print_term_eq(i, "not connected");
		refresh();
		return;
	}
	while (queue.index != 0)
	{
		char c[COMMAND_BUFFER_SIZE];
		char ty;
		commandq_pop(&queue, c, &ty);
		print_term_eq(i, c);
		switch (ty)
		{
		case COMMANDQ_TYPE_SENDONLY:
			send_command(&connections[i].con, c);
			break;
		case COMMANDQ_TYPE_SENDRECV:
			query_command(&connections[i].con, c);
			break;
		case COMMANDQ_TYPE_SENDUNTIL:
			query_until(&connections[i].con, c, connections[i].data.recv_delimiter);
			break;
		case COMMANDQ_TYPE_WAITTRIG:
			print_term_eq(i, "wait for trigger");
			sleep(LIA_WAIT_FOR_TRIG);
			break;
		default:
			print_term("undefined queue command");
			break;
		}
	}
	return;
}