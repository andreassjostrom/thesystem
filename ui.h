#ifndef UI_H
#define UI_H

#define UI_STATUS_ROW 23
#define CONSOLE_WIDTH 80

/* Cursor control */
void ui_hide_cursor(void);
void ui_show_cursor(void);

/* Status line helpers (row 23) */
void ui_clear_status_line(void);
void ui_set_status(const char* msg);

/* Input helpers */
int get_user_input(char* buffer, int max_len, int x, int y, int echo);

/* Delay helper */
void delay_ms(unsigned int ms);

/* Error prompt */
void show_error(const char* message);

#endif
