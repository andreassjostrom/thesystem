#ifndef CONNECTION_H
#define CONNECTION_H

#define MSG_FILE "message.txt"
#define RESP_FILE "response.txt"
#define HELPER_APP "helper.exe"
#define AGENTS_FILE "agents2.txt"

void check_connection(void);


/* Writes a command string to message.txt */
int write_message_file(const char* message);

/* Executes helper.exe with message.txt and response.txt as args */
int call_helper(void);

/* Waits for response.txt to appear, with timeout in milliseconds */
int wait_for_response_file(int timeout_ms);

/* Checks if a file exists */
int file_exists(const char* filename);

/* Combines write + call helper, returns 0 on success, negative on error */
int send_message_to_helper(const char* command_line);

/* Reads lines from response.txt into lines[][], skips first if needed */
int read_response_file(char lines[][80], int max_lines, int skip_first_line);

/* Sends a TestConnection command and checks if response is 'Online' */
int check_connection_status(void);  /* returns 1 = online, 0 = offline */

#endif