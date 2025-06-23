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
#include "splash.h"

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
    gotoxy(20, 6); cprintf("1. The System - The All-Knowing");
    gotoxy(20, 7); cprintf("2. Totally 1980s AI Chat");
    gotoxy(20, 8); cprintf("3. Settings");
    /* gotoxy(20, 9); cprintf("8. Test code");  */
    gotoxy(20, 14); cprintf("9. Exit");
    gotoxy(20, 20); cprintf("Selection number: ");
    gotoxy(39, 20);
}


void render_main_menu(void) {
    if (render_template_file(MENU_FILE) != SUCCESS) {
        clrscr();
        gotoxy(30, 10);
        cprintf("Main menu not found.");
        getch();
        return;
    }

    gotoxy(24, 16);  /* Position for input after menu draws */
}

void handle_settings() {
    clrscr();
    gotoxy(20, 10);
    cprintf("Opening settings...");

    getch();  /* Pause briefly before returning to menu */
}


int handle_credits() {
    log_message("handle_credits: called");
    if (show_credits_sequence() != SUCCESS) {
        log_message("handle_credits: show_credits_sequence() failed");
        return FAILURE;
    }
    return SUCCESS;
}


int handle_about() {
    int i = 1;
    char filename[20];
    char logbuf[100];
    int key;

    log_message("handle_about: start");
    ui_hide_cursor();

    while (1) {
        sprintf(filename, "%sabout%d.txt", DATA_FOLDER, i);
        sprintf(logbuf, "handle_about: checking %s", filename);
        log_message(logbuf);

        if (render_template_file(filename) != SUCCESS) {
            log_message("handle_about: no more about screens");
            break;
        }

        gotoxy(26, 25);  /* Bottom center */
        textcolor(LIGHTGREEN);
        textbackground(BLACK);
        cprintf("[Press any key to continue...]");

        key = getch();
        if (key == 27) {  /* ESC to exit early */
            log_message("handle_about: ESC pressed, exiting early");
            break;
        }

        i++;
    }

    ui_show_cursor();
    log_message("handle_about: end");
    return SUCCESS;
}


void handle_exit() {
    clrscr();
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

void show_agent_selector() {
    char input[5];
    int i, selected_id = -1;
    int redraw = 1;
    int y;

refresh_menu:
    if (redraw) {
        clrscr();
        gotoxy(2, 2);
        cprintf("Select an AI bot or agent:");

        y = 4;
        for (i = 1; i < agent_count; i++) {
            draw_agent_entry(&agent_list[i], &y);
            if (y >= 24) break;
        }

        redraw = 0;
    }

    gotoxy(2, y);
    cprintf("Selection number:");
    gotoxy(32, y);

    if (!get_user_input(input, sizeof(input), 19, y, 1)) {
        return;
    }

    selected_id = atoi(input);
    for (i = 0; i < agent_count; i++) {
        if (agent_list[i].id == selected_id) {
            strncpy(current_agent_name, agent_list[i].name, MAX_AGENT_NAME - 1);
            current_agent_name[MAX_AGENT_NAME - 1] = '\0';
            handle_chat(selected_id);
            return;
        }
    }

    show_error("Invalid ID.");
    goto refresh_menu;
}

void draw_agent_entry(const Agent* agent, int* y) {
    int len = strlen(agent->desc);
    const char* desc = agent->desc;

    if (*y >= 24) return;  /* Prevent overflow */

    gotoxy(2, (*y)++);
    cprintf("%d. %s", agent->id, agent->name);

    while (len > 0 && *y < 24) {
        gotoxy(5, (*y)++);
        cprintf("%.70s", desc);
        desc += 70;
        len -= 70;
    }

    if (*y < 24) (*y)++;  /* Blank line after agent */
}