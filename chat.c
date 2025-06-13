#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "chat.h"
#include "agents.h"
#include "ui.h"

extern int testing_mode;
extern int is_online;
extern char session_id[20];

extern int send_message_to_helper(const char* command_line);
extern int read_response_file(char lines[][MAX_LINE_LENGTH], int max_lines, int skip_first_line);
extern int start_chat_session(int agent_id);

/* Shared state */
char chat_history[MAX_MESSAGES][MAX_LENGTH];
int message_count = 0;

void add_message(const char* msg) {
    if (message_count < MAX_MESSAGES) {
        strncpy(chat_history[message_count], msg, MAX_LENGTH - 1);
        chat_history[message_count][MAX_LENGTH - 1] = '\0';
        message_count++;
    } else {
        int i;
        for (i = 1; i < MAX_MESSAGES; i++) {
            strcpy(chat_history[i - 1], chat_history[i]);
        }
        strncpy(chat_history[MAX_MESSAGES - 1], msg, MAX_LENGTH - 1);
        chat_history[MAX_MESSAGES - 1][MAX_LENGTH - 1] = '\0';
    }
}

void handle_chat(int agent_id) {
    char input[60];
    char command[MAX_COMMAND_LENGTH];
    char formatted[MAX_LENGTH];
    char response_lines[MAX_RESPONSE_LINES][MAX_LINE_LENGTH];
    int ch, i, j;
    int response_count;
    int in_chat = 1;
    int steps = 0;
    int pos = 0;
    int dir = 1;
    char line[81];
    FILE* f;

    if (!start_chat_session(agent_id)) return;

    message_count = 0;
    clrscr();
    textcolor(LIGHTGREEN);
    textbackground(BLACK);
    gotoxy(2, 1);
    cprintf("Chatting with: %s (ESC to end)", current_agent_name);

    while (in_chat) {
        for (i = 0; i < message_count && i < MAX_MESSAGES; i++) {
            gotoxy(2, 3 + i);
            cprintf("%-78s", chat_history[i]);
        }

        gotoxy(2, 24);
        for (i = 0; i < 80; i++) cprintf(" ");
        gotoxy(2, 24);
        cprintf("> ");
        gotoxy(4, 24);

        for (i = 0; i < 60; i++) input[i] = '\0';
        i = 0;

        while (1) {
            ch = getch();
            if (ch == 13) break;
            if (ch == 27) {
                sprintf(command, "EndChat,%s", session_id);
                send_message_to_helper(command);
                response_count = read_response_file(response_lines, MAX_RESPONSE_LINES, 1);
                for (j = 0; j < response_count; j++) {
                    if (j == 0) {
                        sprintf(formatted, "%s:", current_agent_name);
                        add_message(formatted);
                    }
                    add_message(response_lines[j]);
                }
                in_chat = 0;
                break;
            }
            if (ch == 8 && i > 0) {
                i--;
                input[i] = '\0';
                gotoxy(4 + i, 24);
                cprintf(" ");
                gotoxy(4 + i, 24);
            } else if (i < 58 && ch >= 32 && ch <= 126) {
                input[i++] = ch;
                input[i] = '\0';
                cprintf("%c", ch);
            }
        }

        if (i > 0 && in_chat) {
            sprintf(formatted, "YOU: %s", input);
            add_message(formatted);

            clrscr();
            textcolor(LIGHTGREEN);
            gotoxy(2, 1);
            cprintf("Chatting with: %s (ESC to end)", current_agent_name);
            for (j = 0; j < message_count && j < MAX_MESSAGES; j++) {
                gotoxy(2, 3 + j);
                cprintf("%-78s", chat_history[j]);
            }

            sprintf(command, "Chat,%s,%s", session_id, input);
            if (send_message_to_helper(command)) {
                steps = 0;
                pos = 0;
                dir = 1;
                ui_hide_cursor();

                while (steps < 100) {
                    int k;
                    for (k = 0; k < 20; k++) line[k] = ' ';
                    line[20] = '\0';

                    line[pos] = 'O';
                    if (pos - dir >= 0 && pos - dir < 20) line[pos - dir] = 'o';
                    if (pos - 2 * dir >= 0 && pos - 2 * dir < 20) line[pos - 2 * dir] = '.';

                    gotoxy(30, 22);
                    cprintf("%s", line);
                    fflush(stdout);

                    f = fopen(RESP_FILE, "r");
                    if (f) {
                        fclose(f);
                        break;
                    }

                    delay(50);
                    pos += dir;
                    if (pos == 19 || pos == 0) dir = -dir;
                    steps++;
                }

                gotoxy(30, 22);
                cprintf("                    ");
                ui_show_cursor();

                clrscr();
                textcolor(LIGHTGREEN);
                gotoxy(2, 1);
                cprintf("Chatting with: %s (ESC to end)", current_agent_name);
                for (j = 0; j < message_count && j < MAX_MESSAGES; j++) {
                    gotoxy(2, 3 + j);
                    cprintf("%-78s", chat_history[j]);
                }

                if (steps < 100) {
                    response_count = read_response_file(response_lines, MAX_RESPONSE_LINES, 1);
                    if (response_count > 0) {
                        for (j = 0; j < response_count; j++) {
                            if (j == 0) {
                                sprintf(formatted, "%s:", current_agent_name);
                                add_message(formatted);
                            }
                            add_message(response_lines[j]);
                        }
                    } else {
                        add_message("SYSTEM: No response (empty or invalid).");
                    }
                } else {
                    add_message("SYSTEM: No response (timeout).");
                }
            }
        }
    }
}
