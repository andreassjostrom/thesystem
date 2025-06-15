#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "chat.h"
#include "agents.h"
#include "ui.h"
#include "connect.h"
#include "state.h"
#include "common.h"

extern int testing_mode;
extern int is_online;
extern char session_id[20];

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
    char input[60], command[MAX_COMMAND_LENGTH], formatted[MAX_LENGTH];
    char response_lines[MAX_RESPONSE_LINES][MAX_LINE_LENGTH];
    int ch, i, j, response_count, in_chat = 1;
    char logbuf[120];
    int send_result;

    if (start_chat_session(agent_id) != SUCCESS) return;

    message_count = 0;
    clrscr();
    textcolor(LIGHTGREEN);
    textbackground(BLACK);
    gotoxy(2, 1);
    cprintf("Chatting with: %s (ESC to end)", current_agent_name);

    sprintf(logbuf, "Entering chat with agent ID %d", agent_id);
    log_message(logbuf);

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

        memset(input, 0, sizeof(input));
        i = 0;

        while (1) {
            ch = getch();
            if (ch == 13) break;
            if (ch == 27) {
                log_message("User pressed ESC - sending EndChat command.");
                sprintf(command, "EndChat,%s", session_id);
                send_result = send_message_to_helper(command);
                if (send_result != SUCCESS) {
                    sprintf(logbuf, "EndChat send_message_to_helper failed: %d", send_result);
                    log_message(logbuf);
                }

                response_count = read_response_file(response_lines, MAX_RESPONSE_LINES, 1);
                sprintf(logbuf, "EndChat response lines: %d", response_count);
                log_message(logbuf);

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
            sprintf(logbuf, "Sending Chat command: %s", command);
            log_message(logbuf);

            send_result = send_message_to_helper(command);
            if (send_result != SUCCESS) {
                sprintf(logbuf, "send_message_to_helper failed with code %d", send_result);
                log_message(logbuf);
                continue;
            }

            if (wait_for_response_file(5000) != SUCCESS) {
                add_message("SYSTEM: No response (timeout).");
                log_message("SYSTEM: No response (timeout).");
                continue;
            }

            clrscr();
            textcolor(LIGHTGREEN);
            gotoxy(2, 1);
            cprintf("Chatting with: %s (ESC to end)", current_agent_name);
            for (j = 0; j < message_count && j < MAX_MESSAGES; j++) {
                gotoxy(2, 3 + j);
                cprintf("%-78s", chat_history[j]);
            }

            response_count = read_response_file(response_lines, MAX_RESPONSE_LINES, 1);
            sprintf(logbuf, "Chat response lines: %d", response_count);
            log_message(logbuf);

            for (j = 0; j < response_count; j++) {
                sprintf(logbuf, "Line %d: %s", j, response_lines[j]);
                log_message(logbuf);
            }

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
                log_message("SYSTEM: No response (empty or invalid).");
            }
        }
    }
}

int start_chat_session(int agent_id) {
    int result;
    char cmd[40];
    char response[MAX_RESPONSE_LINES][MAX_LINE_LENGTH];
    int count;

    if (testing_mode) {
        strcpy(session_id, "fb4fd402");
        return SUCCESS;
    }

    sprintf(cmd, "StartChat,%d", agent_id);
    result = send_message_to_helper(cmd);

    if (result != SUCCESS) {
        show_error("Failed to start chat session.");
        return FAILURE;
    }

    if (wait_for_response_file(5000) != SUCCESS) {
        show_error("Timeout waiting for session.");
        return FAILURE;
    }

    count = read_response_file(response, MAX_RESPONSE_LINES, 0);
    if (count >= 1) {
        strncpy(session_id, response[0], sizeof(session_id) - 1);
        session_id[sizeof(session_id) - 1] = '\0';
        return SUCCESS;
    }

    show_error("No session ID received.");
    return FAILURE;
}

