#ifndef CHAT_H
#define CHAT_H

#include "agents.h"

#define MAX_MESSAGES 20
#define MAX_LENGTH 70
#define MAX_COMMAND_LENGTH 200


extern char chat_history[MAX_MESSAGES][MAX_LENGTH];
extern char current_agent_name[MAX_AGENT_NAME];
extern int message_count;

void handle_chat(int agent_id);
void add_message(const char* msg);
int start_chat_session(int agent_id);

#endif
