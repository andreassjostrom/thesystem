#ifndef COMMON_H
#define COMMON_H

#define SUCCESS 0
#define FAILURE -1
#define MAX_RESPONSE_LINES 20

#define MENU_FILE "data/menu.txt"
#define DATA_FOLDER "data/"
#define AGENTS_FILE "data/agents.txt"


void log_message(const char* message);
int launch_external_program(const char* exe_name);

#endif