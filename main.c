#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <stdlib.h>
#include "agents.h"  /* Handles loading, caching, and parsing agent list from agents.txt */
#include "chat.h"
#include "ui.h"
#include "connection.h"





#define CONSOLE_WIDTH 80



char command[100];

typedef struct {
    int id;
    char name[MAX_AGENT_NAME];
    char desc[MAX_AGENT_DESC];
} Agent;

Agent agent_list[MAX_AGENTS];
int agent_count = 0;

char current_agent_name[MAX_AGENT_NAME] = "THE SYSTEM";
#define SYSTEM_AGENT_ID 0


/* For Chat UI Code */
#define MAX_MESSAGES 20
#define MAX_LENGTH 70

/* For File exchange */
#define MAX_RESPONSE_LINES 20
#define MAX_LINE_LENGTH 80

#define MAX_COMMAND_LENGTH 200

char agent_lines[MAX_AGENTS][MAX_AGENT_LINE];  /* Global cache */


int pos;
char ch;

int is_online = 0;  /* 0 = offline, 1 = online */


char session_id[20] = "fb4fd402";  /* Will be overwritten once StartChat is implemented */

int testing_mode = 0;  /* Set to 1 to keep response.txt, 0 to delete after reading */



/* Function declarations */
void draw_menu();
void handle_list_agents();
void handle_settings();
void handle_exit();
void show_error(const char* message);
void display_response(char lines[][MAX_LINE_LENGTH], int count);


void delay_ms(unsigned int ms) {
    delay(ms);  /* Turbo C delay in milliseconds */
}


/**
 * Gets input from user with live editing (ESC/backspace supported).
 * Returns 1 if ENTER pressed, 0 if ESC pressed.
 */
int get_user_input(char* buffer, int max_len, int x, int y, int echo) {
    int pos = 0;
    int ch;

    buffer[0] = '\0';
    gotoxy(x, y);

    while (1) {
        ch = getch();

        if (ch == 13) {  /* ENTER */
            buffer[pos] = '\0';
            return 1;
        }

        if (ch == 27) {  /* ESC */
            buffer[0] = '\0';
            return 0;
        }

        if (ch == 8 && pos > 0) {  /* BACKSPACE */
            pos--;
            buffer[pos] = '\0';
            gotoxy(x + pos, y);
            cprintf(" ");
            gotoxy(x + pos, y);
        }
        else if (ch >= 32 && ch <= 126 && pos < max_len - 1) {  /* Printable */
            buffer[pos++] = ch;
            buffer[pos] = '\0';
            if (echo) cprintf("%c", ch);
        }
    }
}






void show_error(const char* message) {
    gotoxy(20, 13);
    cprintf("%s", message);
    getch();
}



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

    gotoxy(30, 2);
    cprintf("THE SYSTEM");

    gotoxy(26, 4);
    cprintf("Main Menu");
    gotoxy(20, 6);  cprintf("1. Chat with The System");
		gotoxy(20, 7);  cprintf("2. Chat with an AI Agent");
		gotoxy(20, 8);  cprintf("3. Settings");
		gotoxy(20, 9);  cprintf("8. Test code");
		gotoxy(20, 11);  cprintf("9. Exit");

    gotoxy(20, 11); cprintf("Selection number: ");
    gotoxy(39, 11); /* Cursor position for input */


    
    
}



void wait_for_response_and_display() {
    char response[MAX_RESPONSE_LINES][MAX_LINE_LENGTH];
    int line_count;

    if (!wait_for_response_file(5000)) {
        gotoxy(2, 6);
        cprintf("Timeout waiting for response.");
        return;
    }

    line_count = read_response_file(response, MAX_RESPONSE_LINES, 1);
    if (line_count > 0) {
        display_response(response, line_count);
    } else {
        gotoxy(2, 6);
        cprintf("Received empty response.");
    }
}



void display_response(char lines[][MAX_LINE_LENGTH], int count) {
	  int i;
	
    clrscr();
    textcolor(LIGHTGREEN);
    gotoxy(2, 2);
    cprintf("RESPONSE FROM THE SYSTEM:");

    for (i = 0; i < count && i < MAX_RESPONSE_LINES; i++) {
        gotoxy(2, 4 + i);
        cprintf("%s", lines[i]);
    }

    gotoxy(2, UI_STATUS_ROW);
    cprintf("Press any key to continue...");
    getch();
}


int start_chat_session(int agent_id) {
    char cmd[40];
    char response[MAX_RESPONSE_LINES][MAX_LINE_LENGTH];
    int count;

    if (testing_mode) {
        strcpy(session_id, "fb4fd402");  /* fixed ID for test mode */
        return 1;
    }

		sprintf(cmd, "StartChat,%d", agent_id);
		int result = send_message_to_helper(cmd);
		
		if (result == -1) {
		    show_error("Failed to write message.txt");
		    return 0;
		} else if (result == -2) {
		    show_error("Helper app failed.");
		    return 0;
		}
		
		if (!wait_for_response_file(5000)) {
		    show_error("Timeout waiting for session.");
		    return 0;
		}



    count = read_response_file(response, MAX_RESPONSE_LINES, 0);  /* do not skip session id ... that is the 0 */  
    
    if (count >= 1) {
        strncpy(session_id, response[0], sizeof(session_id) - 1);
        session_id[sizeof(session_id) - 1] = '\0';
        return 1;
    }

    show_error("No session ID received.");
    return 0;
}






void handle_chat_with_agent() {
    char input[5];
    int i;
    int selected_id = -1;
    int len;
    int id;
    int redraw = 1;

refresh_menu:

    if (redraw) {
        /* Rebuild agent_list[] from agent_lines[] */
        agent_count = 1;  /* THE SYSTEM already in slot 0 */
        for (i = 0; i < MAX_AGENTS && agent_lines[i][0] != '\0'; i++) {
            char name[MAX_AGENT_NAME];
            char desc[MAX_AGENT_DESC];

            if (sscanf(agent_lines[i], "ID: %d | Name: %[^|]| Desc: %[^\n]", &id, name, desc) == 3) {
                if (id == SYSTEM_AGENT_ID) continue;

                len = strlen(name);
                while (len > 0 && name[len - 1] == ' ') {
                    name[--len] = '\0';
                }

                agent_list[agent_count].id = id;
                strncpy(agent_list[agent_count].name, name, MAX_AGENT_NAME - 1);
                agent_list[agent_count].name[MAX_AGENT_NAME - 1] = '\0';
                strncpy(agent_list[agent_count].desc, desc, MAX_AGENT_DESC - 1);
                agent_list[agent_count].desc[MAX_AGENT_DESC - 1] = '\0';
                agent_count++;
            }
        }

        /* Redraw menu */
        clrscr();
        gotoxy(2, 2);
        cprintf("Select an AI Agent to chat with:");

        gotoxy(2, 4);
        cprintf("R) Refresh agent list");

        for (i = 1; i < agent_count; i++) {
            gotoxy(2, 5 + i);
            cprintf("ID: %d | %s - %s", agent_list[i].id, agent_list[i].name, agent_list[i].desc);
        }

        redraw = 0;
    }

    /* Prompt for input */
    gotoxy(2, 6 + agent_count);
    cprintf("Enter ID or command: ");
    gotoxy(25, 6 + agent_count);

    if (!get_user_input(input, sizeof(input), 25, 6 + agent_count, 1)) {
        return;  /* ESC pressed back to menu */
    }

    /* Handle command */
    if (strcmpi(input, "R") == 0) {
        call_list_agents_and_save();
        load_agents_from_file(agent_lines, MAX_AGENTS);
        redraw = 1;
        goto refresh_menu;
    }

    /* Try parse agent ID */
    selected_id = atoi(input);

    for (i = 0; i < agent_count; i++) {
        if (agent_list[i].id == selected_id) {
            strncpy(current_agent_name, agent_list[i].name, MAX_AGENT_NAME - 1);
            current_agent_name[MAX_AGENT_NAME - 1] = '\0';

            if (testing_mode) {
                strcpy(session_id, "fb4fd402");
                handle_chat(selected_id);
            } else {
                if (start_chat_session(selected_id)) {
                    handle_chat(selected_id);
                } else {
                    show_error("Failed to start session with agent.");
                }
            }
            return;
        }
    }

    show_error("Invalid ID or command.");
    goto refresh_menu;
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





int main() {
    char input[4];

    /* Ensure agent 0 (The System) exists */
    agent_list[0].id = SYSTEM_AGENT_ID;
    strncpy(agent_list[0].name, "THE SYSTEM", MAX_AGENT_NAME - 1);
    agent_list[0].name[MAX_AGENT_NAME - 1] = '\0';
    agent_count = 1;

    gotoxy(2, 2);
    cprintf("Checking connection...");
    check_connection_status();

    gotoxy(2, 3);
    if (is_online) {
        cprintf("Status: ONLINE");
    } else {
        cprintf("Status: OFFLINE");
    }

    initialize_agent_list();

    while (1) {
        while (kbhit()) getch();  /* Clear leftover keystrokes */
        draw_menu();

        /* Prompt for input */
        if (!get_user_input(input, 3, 39, 11, 1)) {
            continue;  /* ESC pressed skip and redraw */
        }

        switch (input[0]) {
            case '1':
                if (!is_online) {
                    show_error("You're offline. Cannot chat right now.");
                    break;
                }

                strncpy(current_agent_name, agent_list[SYSTEM_AGENT_ID].name, MAX_AGENT_NAME - 1);
                current_agent_name[MAX_AGENT_NAME - 1] = '\0';

                if (testing_mode) {
                    strcpy(session_id, "fb4fd402");
                    handle_chat(SYSTEM_AGENT_ID);
                } else {
                    if (start_chat_session(SYSTEM_AGENT_ID)) {
                        handle_chat(SYSTEM_AGENT_ID);
                    } else {
                        show_error("Failed to start session with THE SYSTEM.");
                    }
                }
                break;

            case '2':
                if (!is_online) {
                    show_error("You're offline. Cannot chat right now.");
                    break;
                }
                handle_chat_with_agent();
                break;

            case '3':
                handle_settings();
                break;

            case '8':
                test_spinner();
                return 0;



            case '9':
                handle_exit();
                return 0;

            default:
                gotoxy(20, 13);
                cprintf("Invalid selection. Press any key...");
                getch();
                break;
        }
    }

    return 0;  /* Unreachable, but included for completeness */
}


