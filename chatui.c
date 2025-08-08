#include <conio.h>
#include <string.h>
#include "chatui.h"
#include "chatlog.h"
#include "ui.h"
#include "common.h"
#include "agents.h"

/* Static variables to track what was last drawn */
static char last_drawn_lines[CHAT_DISPLAY_LINES][CHATLOG_LINE_BYTES];
static int last_line_count = 0;
static int last_scroll_index = -1;
static int force_full_redraw = 0;
static char last_input[CHATLOG_MAX_INPUT_LENGTH + 1] = "";

void chatui_draw_header(const char* agent_name) {
    gotoxy(2, 1);
    textcolor(LIGHTGREEN);
    textbackground(BLACK);
    cprintf("Chatting with: %s (ESC to end)", agent_name);
}

void chatui_draw_input_prompt(const char* input) {
    gotoxy(1, 24);
    textcolor(LIGHTGREEN);
    textbackground(BLACK);
    cprintf("%-80s", "");  /* Clear the line first */
    
    gotoxy(2, 24);
    cprintf("> %s", input);
}

void chatui_update_input_line(const char *input) {
    textcolor(LIGHTGREEN);
    textbackground(BLACK);

    gotoxy(2, 24);
    cprintf("> %-76s", input);   /* overwrite input line fully */
    gotoxy(4 + strlen(input), 24); /* move cursor after input */
}

/* Call this to force a full redraw (e.g., when entering chat) */
void chatui_force_redraw(void) {
    force_full_redraw = 1;
    last_scroll_index = -1;
    last_line_count = 0;
}

/* Ultra-fast input updates - only changes what's different */
void chatui_update_input_line_fast(const char *input) {
    int old_len = strlen(last_input);
    int new_len = strlen(input);
    int i;
    
    textcolor(LIGHTGREEN);
    textbackground(BLACK);

    /* If this is the first character, always redraw the prompt */
    if (old_len == 0 && new_len > 0) {
        gotoxy(2, 24);
        cprintf("> %s", input);
        gotoxy(4 + new_len, 24);
        strcpy(last_input, input);
        return;
    }

    /* Only update the characters that changed */
    for (i = 0; i < new_len || i < old_len; i++) {
        char old_char = (i < old_len) ? last_input[i] : '\0';
        char new_char = (i < new_len) ? input[i] : '\0';
        
        if (old_char != new_char) {
            gotoxy(4 + i, 24);  /* Position after "> " */
            if (new_char == '\0') {
                cprintf(" ");  /* Clear old character */
            } else {
                cprintf("%c", new_char);
            }
        }
    }
    
    /* Clear any remaining old characters */
    if (old_len > new_len) {
        gotoxy(4 + new_len, 24);
        for (i = new_len; i < old_len; i++) {
            cprintf(" ");
        }
    }
    
    /* Position cursor at end of input */
    gotoxy(4 + new_len, 24);
    
    strcpy(last_input, input);
}

/* Optimized chat window that only draws changed lines */
void chatui_draw_chat_window(void) {
    char lines[CHAT_DISPLAY_LINES][CHATLOG_LINE_BYTES];
    int count;
    int i;
    int current_scroll = chatlog_scroll_index();
    int need_full_redraw = 0;

    textcolor(LIGHTGREEN);
    textbackground(BLACK);

    /* Check if we need a full redraw */
    if (force_full_redraw || current_scroll != last_scroll_index) {
        need_full_redraw = 1;
        force_full_redraw = 0;
    }

    count = chatlog_get_visible_lines(lines, CHAT_DISPLAY_LINES);

    if (need_full_redraw) {
        /* Full redraw - draw all lines */
        for (i = 0; i < count; i++) {
            gotoxy(2, 3 + i);
            cprintf("%-78s", lines[i]);
            strcpy(last_drawn_lines[i], lines[i]);
        }

        /* Clear any unused lines */
        for (; i < CHAT_DISPLAY_LINES; i++) {
            gotoxy(2, 3 + i);
            cprintf("%-78s", "");
            last_drawn_lines[i][0] = '\0';
        }
        
        last_line_count = count;
        last_scroll_index = current_scroll;
    } else {
        /* Incremental update - only draw changed lines */
        for (i = 0; i < count; i++) {
            if (i >= last_line_count || strcmp(lines[i], last_drawn_lines[i]) != 0) {
                gotoxy(2, 3 + i);
                cprintf("%-78s", lines[i]);
                strcpy(last_drawn_lines[i], lines[i]);
            }
        }

        /* Clear any lines that are no longer used */
        for (; i < last_line_count; i++) {
            gotoxy(2, 3 + i);
            cprintf("%-78s", "");
            last_drawn_lines[i][0] = '\0';
        }
        
        last_line_count = count;
    }
}

/* Updated refresh function that's smarter about when to clear screen */
void chatui_refresh_view(const char* agent_name, const char* input) {
    static char last_agent_name[MAX_AGENT_NAME] = "";
    static int first_time = 1;
    
    /* Only clear screen if agent changed or first time */
    if (first_time || strcmp(agent_name, last_agent_name) != 0) {
        clrscr();
        window(1, 1, SCREEN_COLS, SCREEN_ROWS);
        textcolor(LIGHTGREEN);
        textbackground(BLACK);
        
        chatui_draw_header(agent_name);
        chatui_force_redraw();  /* Force full chat window redraw */
        
        strcpy(last_agent_name, agent_name);
        first_time = 0;
    }

    /* Always update chat window (but it will be smart about what to draw) */
    chatui_draw_chat_window();
    
    /* Always update input prompt (it's just one line) */
    chatui_draw_input_prompt(input);
    
    log_message("chatui_refresh_view: optimized refresh complete");
}