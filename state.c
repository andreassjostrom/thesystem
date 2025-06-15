#include "state.h"
#include "common.h"

/* Connection status */
int is_online = 0;

/* Test mode flag */
int testing_mode = 0;

/* Current session identifier */
char session_id[SESSION_ID_LENGTH] = "fb4fd402";  /* Default/test value */

/* Currently selected agent's name */
char current_agent_name[MAX_AGENT_NAME] = "THE SYSTEM";

/* List of loaded agents */
Agent agent_list[MAX_AGENTS];
int agent_count = 0;

char agent_lines[MAX_AGENTS][MAX_AGENT_LINE];

