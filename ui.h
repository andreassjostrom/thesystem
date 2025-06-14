#ifndef UI_H
#define UI_H

#define UI_STATUS_ROW 23
#define CONSOLE_WIDTH 80

#define MAX_RESPONSE_LINES 20
#define MAX_LINE_LENGTH 80

/* Cursor control */
void ui_hide_cursor(void);
void ui_show_cursor(void);

/* Status line helpers (row 23) */
void ui_clear_status_line(void);
void ui_set_status(const char* msg);

void ui_wait_and_display_response(const char* title);
void display_response(char lines[][MAX_LINE_LENGTH], int count);

#endif
