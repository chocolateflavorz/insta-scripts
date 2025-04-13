
/**
 * DO NOT INCLUDE THIS FILE EXCEPT IN main.c
 */

#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif
#define LIA_WAIT_FOR_TRIG (config_lia.tcon * 3)

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define wmvclr(w, y) wmove(w, y, 1); wclrtoeol(w);

#define EQNUM (3)
#define SG5025 (0)
#define SG5026 (1)
#define LIA (2)

	typedef int (*ITEM_CALLBACK)();
	typedef enum
	{
		MAIN_MENU,
		SG5025_MENU,
		SG5026_MENU,
		LIA_MENU,
	} ui_state_t;

	typedef struct
	{
		const char *ip_addr;
		int port;
		char delimiter;
		char recv_delimiter;
	} connection_data_t;

	static const char *short_name[] = {
		"SG5",
		"SG6",
		"LIA",
	};

	static const char *choices[] = {
		"setting sg5",
		"setting sg6",
		"setting lia",
		"CONNECT/SEND S5",
		"CONNECT/INIT LIA",
		"SEND LIA",
		"AUTOUPDATE LIA",
		"SWEEP",
		"Exit",
	};

	static const char *descriptions[] = {
		"DPL32 port 5025",
		"DPL32 port 5026",
		"for LI5502",
		"con and send cmds to sg(dpl32 port5025)",
		"con and init cmds to lia(li5502)",
		"send setting cmd to lia(li5502)",
		"auto update lia(li5502)",
		"sweep measurement(unimplemented)",
		"Exit",
	};

	typedef struct
	{
		Con con;
		connection_data_t data;
	} con_params_t;

	// UI
	static void set_colors();
	static void create_main_ui();
	static void create_status_window();
	static void init_panels();
	static void destroy_main_ui();
	static void destroy_status_window();
	static void destroy_all();

	static int main_ui_handler(int ch);
	static void show_status();

	static int not_implemented();
	static int state_for_main();
	static int state_for_exit();
	static int state_for_sg5025();
	static int state_for_sg5026();
	static int state_for_lia();
	static int state_for_lia_init();
	static int state_for_autoupdate();
	static int command_sg5();
	static int command_lia();
	
	// ToString
	static void print_lia_status_flag(int flag, int y, int x);

	// Queue management
	void queue_to_device(int i);
	// connection management
	void *do_connect(void *i);
	void try_connect_to_eq(int i);

	// callbacks for menu items
	static const ITEM_CALLBACK functions[] = {
		state_for_sg5025,
		state_for_sg5026,
		state_for_lia,
		command_sg5,
		state_for_lia_init,
		command_lia,
		state_for_autoupdate,
		not_implemented,
		state_for_exit,
	};

#ifdef __cplusplus
}
#endif

#endif