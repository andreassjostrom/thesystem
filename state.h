#ifndef STATE_H
#define STATE_H

#include "agents.h"  /* for Agent struct and MAX constants */

#define SESSION_ID_LENGTH 20
#define SYSTEM_AGENT_ID 0

extern int is_online;
extern int testing_mode;


extern char session_id[SESSION_ID_LENGTH];
extern char current_agent_name[MAX_AGENT_NAME];

#define MAX_AGENTS 50
#define MAX_AGENT_LINE 160

extern char agent_lines[MAX_AGENTS][MAX_AGENT_LINE];


extern Agent agent_list[MAX_AGENTS];
extern int agent_count;


#endif


