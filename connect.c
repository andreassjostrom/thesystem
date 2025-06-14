#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include "connect.h"
#include "common.h"
#include "spinner.h"
#include "ui.h"



void check_connection(void) {
    if (check_connection_status() == SUCCESS) {
        is_online = 1;
    } else {
        is_online = 0;
    }
}



int check_connection_status(void) {
    char lines[1][80];
    int count;

		if (write_message_file("TestConnection") != SUCCESS) return FAILURE;
		if (call_helper() != SUCCESS) return FAILURE;
		if (wait_for_response_file(5000) != SUCCESS) return FAILURE;

    count = read_response_file(lines, 1, 1);
    if (count >= 1) {
        if (strcmp(lines[0], "Online") == 0) {
            return SUCCESS;
        } else {
            return FAILURE;
        }
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
    remove(RESP_FILE);
    sprintf(cmd, "%s %s %s", HELPER_APP, MSG_FILE, RESP_FILE);
    if (system(cmd) == 0) {
        return SUCCESS;
    } else {
        return FAILURE;
    }
}

int wait_for_response_file(int timeout_ms) {
    int pos = 0, dir = 1, steps = 0;
    int max_steps = timeout_ms / 100;
    FILE* f;

    ui_hide_cursor();
    log_message("wait_for_response_file");

    while (steps < max_steps) {
        log_message("spinner_tick() called.");
        spinner_tick(pos, dir);

        f = fopen(RESP_FILE, "r");
        if (f) {
            fclose(f);
            break;
        }

        delay(100);
        pos += dir;
        if (pos == 19 || pos == 0) dir = -dir;
        steps++;
    }

    spinner_clear();
    log_message("spinner cleared called.");
    ui_show_cursor();

    if (file_exists(RESP_FILE)) {
        return SUCCESS;
    } else {
        return FAILURE;
    }
}

int file_exists(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (f) {
        fclose(f);
        return SUCCESS;
    }
    return FAILURE;
}

int read_response_file(char lines[][80], int max_lines, int skip_first_line) {
    FILE* f;
    int count = 0;
    int len;

    f = fopen(RESP_FILE, "r");
    if (!f) return 0;

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
