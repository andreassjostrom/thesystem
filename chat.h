#ifndef CHAT_H
#define CHAT_H

#define MAX_MESSAGES 20
#define MAX_LENGTH 70
#define MAX_LINE_LENGTH 80
#define MAX_RESPONSE_LINES 20
#define MAX_COMMAND_LENGTH 200

extern char chat_history[MAX_MESSAGES][MAX_LENGTH];
extern int message_count;

void handle_chat(int agent_id);
void add_message(const char* msg);

#endif
