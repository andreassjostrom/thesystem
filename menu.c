#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "agents.h"
#include "chat.h"
#include "ui.h"
#include "connection.h"
#include "menu.h"

extern int agent_count;
extern int is_online;
extern int testing_mode;
extern char session_id[20];
extern char current_agent_name[MAX_AGENT_NAME];
extern Agent agent_list[MAX_AGENTS];
extern char agent_lines[MAX_AGENTS][MAX_AGENT_LINE];

void draw_status_bar() {
    gotoxy(2, 25);  /* Bottom row */
    textcolor(LIGHTGREEN);
    textbackground(BLACK);

    if (is_online) {
        cprintf("[ * ONLINE ]   SYSTEM LINK STABLE");
    } else {
        cprintf("[ - OFFLINE]   CHECK YOUR MODEM CABLE ");
    }
}

void draw_menu() {
    clrscr();
    textcolor(LIGHTGREEN);
    textbackground(BLACK);
    
    draw_status_bar();

    gotoxy(30, 2); cprintf("THE SYSTEM");
    gotoxy(26, 4); cprintf("Main Menu");
    gotoxy(20, 6); cprintf("1. Chat with The System");
    gotoxy(20, 7); cprintf("2. Chat with an AI Agent");
    gotoxy(20, 8); cprintf("3. Settings");
    gotoxy(20, 9); cprintf("8. Test code");
    gotoxy(20, 11); cprintf("9. Exit");
    gotoxy(20, 11); cprintf("Selection number: ");
    gotoxy(39, 11);
}

void handle_settings() {
    clrscr();
    gotoxy(20, 10);
    cprintf("Opening settings menu...");
    getch();
}

void handle_exit() {
    clrscr();
    gotoxy(20, 10);
    cprintf("Exiting THE SYSTEM...");
    getch();
}

void test_spinner() {
    int steps = 0, pos = 0, dir = 1;
    char line[81];
    int k;

    while (steps < 100) {
        for (k = 0; k < 20; k++) line[k] = ' ';
        line[20] = '\0';

        line[pos] = 'O';
        if (pos - dir >= 0 && pos - dir < 20) line[pos - dir] = 'o';
        if (pos - 2 * dir >= 0 && pos - 2 * dir < 20) line[pos - 2 * dir] = '.';

        gotoxy(30, 12);  /* draw near center */
        cprintf("%s", line);

        delay(50);
        pos += dir;
        if (pos == 19 || pos == 0) dir = -dir;
        steps++;
    }

    gotoxy(30, 12);
    cprintf("                    ");  /* clear */
}