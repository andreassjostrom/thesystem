#include "state.h"
#include "common.h"

/* Connection status */
int is_online = 0;

/* Current session identifier */
char session_id[SESSION_ID_LENGTH] = "";

/* Currently selected agent's name */
char current_agent_name[MAX_AGENT_NAME];

/* Start with spinner on */
int spinner_enabled = 1;