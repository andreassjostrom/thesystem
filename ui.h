#ifndef UI_H
#define UI_H

#define UI_STATUS_ROW 23

/* Cursor control */
void ui_hide_cursor(void);
void ui_show_cursor(void);

/* Status line helpers (row 23) */
void ui_clear_status_line(void);
void ui_set_status(const char* msg);

#endif
