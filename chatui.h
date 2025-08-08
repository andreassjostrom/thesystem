#ifndef CHATUI_H
#define CHATUI_H

#include "chatlog.h"

void chatui_draw_header(const char* agent_name);   /* Line 1 */
void chatui_draw_input_prompt(const char* input);  /* Line 24 */
void chatui_draw_chat_window(void);                /* Lines 3..22 */
void chatui_refresh_view(const char* agent_name, const char* input);  /* Full repaint */
void chatui_update_input_line(const char *input);

void chatui_force_redraw(void);
void chatui_update_input_line_fast(const char *input);

#endif



