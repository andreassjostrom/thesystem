#ifndef STATE_H
#define STATE_H

#include "agents.h"  /* for Agent struct and MAX constants */

#define SESSION_ID_LENGTH 20


extern int is_online;
extern int is_logging_enabled;
extern char session_id[SESSION_ID_LENGTH];
extern char current_agent_name[MAX_AGENT_NAME];

extern int spinner_enabled; /* If spinner should be shown or not when waiting for response files. Only in chats. */

#endif
