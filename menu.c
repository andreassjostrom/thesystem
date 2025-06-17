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
#include "spinner.h"

extern int agent_count;
extern int is_online;
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

void test_spinner(void) {
    int steps = 0, pos = 0, dir = 1;

    clrscr();
    gotoxy(30, 10);
    cprintf("SPINNER TEST (ESC to exit)");

    ui_hide_cursor();

    while (steps < 1000) {
        spinner_tick(pos, dir);
        delay(70);
        pos += dir;
        if (pos == SPINNER_WIDTH - 1 || pos == 0) dir = -dir;

        if (kbhit() && getch() == 27) break;
        steps++;
    }

    spinner_clear();
    ui_show_cursor(); 
}

void test_spinner_file(void) {
    int pos = 0, dir = 1, steps = 0;
    const char* filename = "testflag.txt";

    clrscr();
    gotoxy(30, 10);
    cprintf("Waiting for 'testflag.txt'...");

    while (steps < 200) {  /* max 20 seconds */
        if (file_exists(filename) == SUCCESS) {
            gotoxy(30, 12);
            cprintf("File detected. Spinner stopped.");
            break;
        }

        spinner_tick(pos, dir);
        delay(100);
        pos += dir;
        if (pos == 19 || pos == 0) dir = -dir;
        steps++;
    }

    spinner_clear();
    gotoxy(30, 14);
    cprintf("Press any key to exit.");
    getch();
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
		
		        handle_chat_INTERNAL_DEBUG(selected_id);
		        return;
		    }
		}

    show_error("Invalid ID.");
    goto refresh_menu;
}
