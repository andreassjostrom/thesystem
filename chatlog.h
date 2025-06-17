#ifndef CHATLOG_H
#define CHATLOG_H

#define CHATLOG_MAX_LINES 300                 /* Total wrapped lines retained */
#define CHATLOG_LINE_WIDTH 77                 /* Max characters per line (display width) */
#define CHATLOG_LINE_BYTES (CHATLOG_LINE_WIDTH + 1)  /* Include null terminator */
#define CHATLOG_MAX_INPUT_LENGTH 240          /* Max input length from user prompt */
#define CHATLOG_WRAP_TEMP_LINES 5             /* Temp space to hold wrapped fragments */
#define CHAT_DISPLAY_LINES 20                 /* Number of visible lines on screen */

void chatlog_init(void);
void chatlog_clear(void);
void chatlog_add(const char* raw_line);
int  chatlog_blocked(void);
int  chatlog_line_count(void);
int  chatlog_scroll_index(void);
void chatlog_scroll(int direction);           /* -1 = up, 1 = down, -20 = PGUP, +20 = PGDN */
void chatlog_scroll_reset_to_bottom(void);
int  chatlog_get_visible_lines(char lines[][CHATLOG_LINE_BYTES], int max_lines);

                         
int wrap_text_into_lines(const char* input,
                         char lines[][CHATLOG_LINE_BYTES],
                         int max_lines,
                         int max_width);

#endif


