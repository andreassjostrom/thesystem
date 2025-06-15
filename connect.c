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
    char lines[1][80];
    int count;

    log_message("ccs in");

    if (write_message_file("TestConnection") != SUCCESS) return FAILURE;
    log_message("ccs after write");

    if (call_helper() != SUCCESS) return FAILURE;
    log_message("ccs after call helper");

    if (wait_for_response_file(5000) != SUCCESS) return FAILURE;
    log_message("ccs after wait for response");

    count = read_response_file(lines, 1, 1);
    if (count >= 1 && strcmp(lines[0], "Online") == 0) {
        return SUCCESS;
    }

    return FAILURE;
}

int send_message_to_helper(const char* command_line) {
    if (write_message_file(command_line) != SUCCESS) return FAILURE;
    if (call_helper() != SUCCESS) return FAILURE;
    return SUCCESS;
}

int write_message_file(const char* message) {
    FILE* f = fopen(MSG_FILE, "w");
    if (!f) return FAILURE;
    fprintf(f, "%s\n", message);
    fclose(f);
    return SUCCESS;
}

int call_helper(void) {
    char cmd[100];
    log_message("In call helper");

    remove(RESP_FILE);
    log_message("Removed response.txt");

    sprintf(cmd, "%s %s %s", HELPER_APP, MSG_FILE, RESP_FILE);
    log_message("helper app called");

    return (system(cmd) == 0) ? SUCCESS : FAILURE;
}

int wait_for_response_file(int timeout_ms) {
    int pos = 0, dir = 1, steps = 0;
    int max_steps = timeout_ms / 100;
    FILE* f;
    long size;

    ui_hide_cursor();
    log_message("wait_for_response_file");

    while (steps < max_steps) {
        log_message("spinner_tick() called.");
        spinner_tick(pos, dir);

        f = fopen(RESP_FILE, "r");
        if (f) {
            log_message("Opened RESP_FILE");

            fseek(f, 0, SEEK_END);
            size = ftell(f);
            fclose(f);

            if (size > 0) {
                log_message("RESP_FILE has content");
                spinner_clear();
                log_message("spinner cleared called.");
                ui_show_cursor();
                log_message("Returning SUCCESS from wait_for_response_file");
                return SUCCESS;
            } else {
                log_message("RESP_FILE is empty");
            }
        }

        delay(100);
        pos += dir;
        if (pos == 19 || pos == 0) dir = -dir;
        steps++;
    }

    spinner_clear();
    log_message("spinner cleared called.");
    ui_show_cursor();

    if (file_exists(RESP_FILE) == SUCCESS) {
        log_message("Final file_exists check: SUCCESS");
        return SUCCESS;
    } else {
        log_message("Final file_exists check: FAILURE");
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


int read_response_file(char lines[][80], int max_lines, int skip_first_line) {
    FILE* f = fopen(RESP_FILE, "r");
    int count = 0;
    int len;

		if (!f) return FAILURE;

    if (skip_first_line) {
        char temp[80];
        fgets(temp, sizeof(temp), f);
    }

    while (fgets(lines[count], 80, f) && count < max_lines) {
        len = strlen(lines[count]);
        while (len > 0 && (lines[count][len - 1] == '\n' || lines[count][len - 1] == '\r')) {
            lines[count][--len] = '\0';
        }
        count++;
    }

    fclose(f);
    remove(RESP_FILE);
    return count;
}
