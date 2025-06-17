#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include "connect.h"
#include "common.h"
#include "spinner.h"
#include "ui.h"
#include "state.h"

void check_connection(void) {
    if (check_connection_status() == SUCCESS) {
        is_online = 1;
        log_message("Connection status: ONLINE");
    } else {
        is_online = 0;
        log_message("Connection status: OFFLINE");
    }
}

int check_connection_status(void) {
    char lines[1][RESPONSE_LINE_MAX];
    int count;

    log_message("check_connection_status: starting");

    if (call_helper("TestConnection") != SUCCESS) {
        log_message("check_connection_status: call_helper failed");
        return FAILURE;
    }

    count = read_response_file(lines, 1, 1);
    if (count >= 1 && strcmp(lines[0], "Online") == 0) {
        return SUCCESS;
    }

    log_message("check_connection_status: unexpected or missing response");
    return FAILURE;
}


int call_helper(const char* message) {
    FILE* f;
    char cmd[100];
    int result;

    log_message("call_helper: starting");

    /* Write message to file */
    f = fopen(MSG_FILE, "w");
    if (!f) {
        log_message("call_helper: failed to open msg.txt for writing.");
        return FAILURE;
    }
    fprintf(f, "%s\n", message);
    fclose(f);

    log_message("call_helper: wrote message");
    
		/* Remove old response */
    remove(RESP_FILE);
    log_message("call_helper: removed old response");

    /* Build and run command */
    sprintf(cmd, "%s %s %s", HELPER_APP, MSG_FILE, RESP_FILE);
    log_message("call_helper: invoking helper");
    log_message(cmd);

		/* THIS IS WHERE WE CALL HELPER */
    result = system(cmd);
    
    if (result == 0) {
        log_message("call_helper: SUCCESS response.txt should now exist");
        return SUCCESS;
    } else {
        log_message("call_helper: FAILURE");
        return FAILURE;
    }
}


int file_exists(const char* filename) {
    char logbuf[100];
    FILE* f;
    long size;

    sprintf(logbuf, "Checking file_exists: %s", filename);
    log_message(logbuf);

    f = fopen(filename, "r");
    if (f) {
        log_message("File pointer is not NULL");

        fseek(f, 0, SEEK_END);
        size = ftell(f);
        fclose(f);

        sprintf(logbuf, "Opened '%s', size = %ld", filename, size);
        log_message(logbuf);

        if (size > 0) {
            return SUCCESS;
        } else {
            log_message("File size is 0  treating as FAILURE");
            return FAILURE;
        }
    }

    log_message("File pointer is NULL  fopen failed");
    return FAILURE;
}

int start_chat_session(int agent_id) {
    char cmd[40];
    char logbuf[120];
    int count;

    /* Dynamically allocate to avoid BSS overflow */
    char (*response)[RESPONSE_LINE_MAX];
    response = malloc(MAX_RESPONSE_LINES * RESPONSE_LINE_MAX);
    if (!response) {
        show_error("Memory allocation failed.");
        return FAILURE;
    }
    memset(response, 0, MAX_RESPONSE_LINES * RESPONSE_LINE_MAX);

    sprintf(logbuf, "start_chat_session chatc: agent_id = %d", agent_id);
    log_message(logbuf);

    sprintf(cmd, "StartChat,%d", agent_id);
    log_message("start_chat_session: command constructed");
    log_message(cmd);

    if (call_helper(cmd) != SUCCESS) {
        log_message("start_chat_session: call_helper FAILED");
        free(response);
        return FAILURE;
    }

    log_message("start_chat_session: call_helper returned SUCCESS");

    count = read_response_file(response, MAX_RESPONSE_LINES, 0);
    sprintf(logbuf, "start_chat_session: response line count = %d", count);
    log_message(logbuf);

    if (count >= 1) {
        strncpy(session_id, response[0], sizeof(session_id) - 1);
        session_id[sizeof(session_id) - 1] = '\0';
        sprintf(logbuf, "start_chat_session: session_id = %s", session_id);
        log_message(logbuf);
        log_message("start_chat_session: returning SUCCESS");
        free(response);
        return SUCCESS;
    }

    log_message("start_chat_session: No session ID received.");
    free(response);
    return FAILURE;
}



int read_response_file(char lines[][RESPONSE_LINE_MAX], int max_lines, int skip_first_line) {
    FILE* f;
    int count;
    int len;

    char buf[RESPONSE_LINE_MAX];

    count = 0;
    f = fopen(RESP_FILE, "r");
    if (!f) return FAILURE;

    if (skip_first_line) {
        fgets(buf, sizeof(buf), f);  /* discard first line */
    }

    while (count < max_lines && fgets(lines[count], RESPONSE_LINE_MAX, f)) {
        len = strlen(lines[count]);
        while (len > 0 && (lines[count][len - 1] == '\n' || lines[count][len - 1] == '\r')) {
            lines[count][--len] = '\0';
        }
        count++;
    }

    fclose(f);
    return count;
}
