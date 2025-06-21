#ifndef CHAT_H
#define CHAT_H

#define MAX_COMMAND_LENGTH 200

int handle_chat(int agent_id);
int start_session(int agent_id);
int run_chat_loop(void);

#endif