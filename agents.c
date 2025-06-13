#include <stdio.h>
#include <string.h>
#include "agents.h"
#include "connect.h"
#include "state.h"


int load_agents_from_file(char lines[][MAX_AGENT_LINE], int max_lines) {
    FILE* f = fopen(AGENTS_FILE, "r");
    int count = 0;
    int len;

    if (!f) return 0;

    while (fgets(lines[count], MAX_AGENT_LINE, f) && count < max_lines) {
        len = strlen(lines[count]);
        while (len > 0 && (lines[count][len - 1] == '\n' || lines[count][len - 1] == '\r')) {
            lines[count][--len] = '\0';
        }
        count++;
    }

    fclose(f);
    return count;
}

void call_list_agents_and_save(void) {
    FILE* in;
    FILE* cache;
    char buffer[MAX_AGENT_LINE];

    /* Step 1: Send ListAgents command */
    int result = send_message_to_helper("ListAgents");
    if (result == -1) {
        show_error("Failed to write ListAgents command.");
        return;
    } else if (result == -2) {
        show_error("Helper app failed.");
        return;
    }

    /* Step 2: Wait for response */
    if (!wait_for_response_file(5000)) {
        show_error("Timeout waiting for ListAgents response.");
        return;
    }

    /* Step 3: Copy response.txt to agents.txt */
    in = fopen(RESP_FILE, "r");
    cache = fopen(AGENTS_FILE, "w");

    if (in && cache) {
        while (fgets(buffer, sizeof(buffer), in)) {
            fputs(buffer, cache);
        }
    }

    if (in) fclose(in);
    if (cache) fclose(cache);
}


void initialize_agent_list(void) {
    if (file_exists(AGENTS_FILE)) {
        agent_count = load_agents_from_file(agent_lines, MAX_AGENTS);
    } else {
        call_list_agents_and_save();
        agent_count = load_agents_from_file(agent_lines, MAX_AGENTS);
    }
}
