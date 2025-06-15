#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "agents.h"
#include "chat.h"
#include "ui.h"
#include "connect.h"
#include "menu.h"
#include "state.h"
#include "common.h"

extern int agent_count;
extern int is_online;
extern int testing_mode;
extern char session_id[20];
extern char current_agent_name[MAX_AGENT_NAME];
extern Agent agent_list[MAX_AGENTS];

void draw_status_bar() {
    gotoxy(2, 25);
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

        gotoxy(30, 12);
        cprintf("%s", line);

        delay(50);
        pos += dir;
        if (pos == 19 || pos == 0) dir = -dir;
        steps++;
    }

    gotoxy(30, 12);
    cprintf("                    ");
}

void handle_chat_with_agent() {
    char input[5];
    int i, selected_id = -1;
    int redraw = 1;

refresh_menu:
    if (redraw) {
        clrscr();
        gotoxy(2, 2);
        cprintf("Select an AI Agent to chat with:");

        for (i = 1; i < agent_count; i++) {
            gotoxy(2, 4 + i);
            cprintf("ID: %d | %s - %s", agent_list[i].id,
                    agent_list[i].name,
                    agent_list[i].desc);
        }

        redraw = 0;
    }

    gotoxy(2, 5 + agent_count);
    cprintf("Enter ID: ");
    gotoxy(13, 5 + agent_count);

    if (!get_user_input(input, sizeof(input), 13, 5 + agent_count, 1)) {
        return;
    }

    selected_id = atoi(input);

    for (i = 0; i < agent_count; i++) {
        if (agent_list[i].id == selected_id) {
            strncpy(current_agent_name, agent_list[i].name, MAX_AGENT_NAME - 1);
            current_agent_name[MAX_AGENT_NAME - 1] = '\0';

            if (testing_mode) {
                strcpy(session_id, "fb4fd402");
                handle_chat(selected_id);
            } else {
                if (start_chat_session(selected_id) == SUCCESS) {
                    handle_chat(selected_id);
                } else {
                    show_error("Failed to start session with agent.");
                }
            }
            return;
        }
    }

    show_error("Invalid ID.");
    goto refresh_menu;
}
