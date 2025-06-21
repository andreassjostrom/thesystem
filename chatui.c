#include <conio.h>
#include <string.h>
#include "chatui.h"
#include "chatlog.h"
#include "ui.h"

void chatui_draw_header(const char* agent_name) {
    gotoxy(2, 1);
    textcolor(LIGHTGREEN);
    textbackground(BLACK);
    cprintf("Chatting with: %s (ESC to end)", agent_name);
}

void chatui_draw_input_prompt(const char* input) {
    int i;

    gotoxy(2, 24);
    textcolor(LIGHTGREEN);
    textbackground(BLACK);

    for (i = 0; i < 80; i++) {
        cprintf(" ");
    }

    gotoxy(2, 24);
    cprintf("> ");
    gotoxy(4, 24);
    cprintf("%s", input);
}

void chatui_draw_chat_window(void) {
    char lines[CHAT_DISPLAY_LINES][CHATLOG_LINE_BYTES];
    int count;
    int i;
    
    /* For char by char printing */
    int col;
    char ch;


    textcolor(LIGHTGREEN);
    textbackground(BLACK);

    count = chatlog_get_visible_lines(lines, CHAT_DISPLAY_LINES);

    /* Print each line from the response */ 
    for (i = 0; i < count; i++) {
        gotoxy(2, 3 + i);
        cprintf("%-78s", lines[i]);
    }
    


    /* Clear any unused lines */
    for (; i < CHAT_DISPLAY_LINES; i++) {
        gotoxy(2, 3 + i);
        cprintf("%-78s", " ");
    }
}

void chatui_refresh_view(const char* agent_name, const char* input) {
    clrscr();
    chatui_draw_header(agent_name);
    chatui_draw_chat_window();
    chatui_draw_input_prompt(input);
}
