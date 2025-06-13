#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include "connection.h"

int write_message_file(const char* message) {
    FILE* f = fopen(MSG_FILE, "w");
    if (!f) return -1;
    fprintf(f, "%s\n", message);
    fclose(f);
    return 0;
}

int call_helper(void) {
    char command[100];
    sprintf(command, "%s %s %s", HELPER_APP, MSG_FILE, RESP_FILE);
    return system(command);  /* Returns 0 on success */
}

int wait_for_response_file(int timeout_ms) {
    int attempts = 0;
    int max_attempts = timeout_ms / 500;
    while (!file_exists(RESP_FILE) && attempts < max_attempts) {
        delay(500);
        attempts++;
    }
    return file_exists(RESP_FILE);
}

int file_exists(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (f) {
        fclose(f);
        return 1;
    }
    return 0;
}

int send_message_to_helper(const char* command_line) {
    if (write_message_file(command_line) != 0) return -1;
    if (call_helper() != 0) return -2;
    return 0;
}

int read_response_file(char lines[][80], int max_lines, int skip_first_line) {
    FILE* f;
    int count = 0;
    int len;

    f = fopen(RESP_FILE, "r");
    if (!f) return 0;

    if (skip_first_line) {
        char temp[80];
        fgets(temp, sizeof(temp), f);  /* Skip first line */
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

int check_connection_status(void) {
    FILE* f;
    char buffer1[40], buffer2[40];
    int attempts = 0;
    int len;

    remove(RESP_FILE);

    if (write_message_file("TestConnection") != 0) return 0;
    if (call_helper() != 0) return 0;

    while (!file_exists(RESP_FILE) && attempts < 10) {
        delay(500);
        attempts++;
    }

    if (!file_exists(RESP_FILE)) return 0;

    f = fopen(RESP_FILE, "r");
    if (!f) return 0;

    buffer1[0] = '\0';
    buffer2[0] = '\0';

    fgets(buffer1, sizeof(buffer1), f);  /* Session ID */
    fgets(buffer2, sizeof(buffer2), f);  /* Status Line */
    fclose(f);

    len = strlen(buffer2);
    while (len > 0 && (buffer2[len - 1] == '\n' || buffer2[len - 1] == '\r')) {
        buffer2[--len] = '\0';
    }

    return (strcmp(buffer2, "Online") == 0);
}
