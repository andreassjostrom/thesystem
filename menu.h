#ifndef MENU_H
#define MENU_H

void draw_menu(void);
void draw_status_bar(void);
void handle_settings(void);
void handle_exit(void);
void test_spinner(void);
void test_spinner_file(void);
void draw_agent_entry(const Agent* agent, int* y);
void show_agent_selector(void);
void render_main_menu(void);


/* Screen handlers */
int handle_about();
int handle_credits();

#endif
