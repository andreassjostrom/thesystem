#ifndef CONNECT_H
#define CONNECT_H

#include "ui.h"

#define MSG_FILE "message.txt"
#define RESP_FILE "response.txt"
#define HELPER_APP "helper.exe"

#define RESPONSE_LINE_MAX 512  /* Reduced from 2048 to stay within DOS limits */

int file_exists(const char* filename);
int call_helper(const char* message);
int read_response_file(char lines[][RESPONSE_LINE_MAX], int max_lines, int skip_first_line);
int start_chat_session(int agent_id);

#endif
