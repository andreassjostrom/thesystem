#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "chat.h"
#include "chatlog.h"
#include "chatui.h"
#include "connect.h"
#include "state.h"
#include "common.h"
#include "ui.h"

extern int is_online;
extern char session_id[20];

int handle_chat_INTERNAL_DEBUG(int agent_id) {
    int result;

    result = start_session(agent_id);
    if (result != SUCCESS) {
        log_message("handle_chat_INTERNAL_DEBUG: start_session failed");
        return FAILURE;
    }

    result = run_chat_loop();
    if (result != SUCCESS) {
        log_message("handle_chat_INTERNAL_DEBUG: run_chat_loop failed");
        return FAILURE;
    }

    log_message("handle_chat_INTERNAL_DEBUG: completed successfully");
    return SUCCESS;
}


int start_session(int agent_id) {
    log_message("start_session: initializing");

    if (start_chat_session(agent_id) != SUCCESS) {
        show_error("Failed to start chat session.");
        return FAILURE;
    }

    chatlog_clear();
    chatui_refresh_view(current_agent_name, "");

    log_message("start_session: completed successfully");
    return SUCCESS;
}

int run_chat_loop(void) {
    char input[CHATLOG_MAX_INPUT_LENGTH + 1];
    char command[MAX_COMMAND_LENGTH];
    char formatted[CHATLOG_LINE_BYTES];
    char logbuf[120];
    int ch, i, j, response_count;
    int in_chat = 1;

    /* Dynamically allocate response buffer to avoid _BSS overflow */
    char (*response)[RESPONSE_LINE_MAX];
    response = malloc(MAX_RESPONSE_LINES * RESPONSE_LINE_MAX);
    if (!response) {
        show_error("Memory allocation failed.");
        return FAILURE;
    }
    memset(response, 0, MAX_RESPONSE_LINES * RESPONSE_LINE_MAX);

    while (in_chat) {
        memset(input, 0, sizeof(input));
        i = 0;

        while (1) {
            ch = getch();
            if (ch == 13) break;
            if (ch == 27) {
                sprintf(command, "EndChat,%s", session_id);
                if (call_helper(command) != SUCCESS) {
                    chatlog_add("SYSTEM: EndChat failed.");
                    free(response);
                    return FAILURE;
                }
                response_count = read_response_file(response, MAX_RESPONSE_LINES, 1);
                for (j = 0; j < response_count; j++) {
                    if (j == 0) {
                        sprintf(formatted, "%s:", current_agent_name);
                        chatlog_add(formatted);
                    }
                    chatlog_add(response[j]);
                }
                free(response);
                return SUCCESS;
            }

            if (ch == 8 && i > 0) i--;
            else if (i < CHATLOG_MAX_INPUT_LENGTH - 1 && ch >= 32 && ch <= 126) input[i++] = ch;

            input[i] = '\0';
            chatui_draw_input_prompt(input);
        }

        if (i > 0) {
            sprintf(formatted, "YOU: %s", input);
            chatlog_add(formatted);
            chatui_refresh_view(current_agent_name, "");

            sprintf(command, "Chat,%s,%s", session_id, input);
            if (call_helper(command) != SUCCESS) continue;

            response_count = read_response_file(response, MAX_RESPONSE_LINES, 1);
            if (response_count > 0) {
                if (spinner_enabled) spinner_wait(3000);
                for (j = 0; j < response_count; j++) {
                    if (j == 0) {
                        sprintf(formatted, "%s:", current_agent_name);
                        chatlog_add(formatted);
                    }
                    chatlog_add(response[j]);
                }
            } else {
                chatlog_add("SYSTEM: No response (empty or invalid).");
            }

            chatui_refresh_view(current_agent_name, "");
        }

        if (chatlog_blocked()) {
            show_error("Chat log full. Restart required.");
            free(response);
            return FAILURE;
        }
    }

    free(response);
    return SUCCESS;
}
