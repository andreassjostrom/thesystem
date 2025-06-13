#include <stdio.h>
#include <string.h>
#include "agents.h"

int agent_file_exists(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (f) {
        fclose(f);
        return 1;
    }
    return 0;
}

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

    if (!write_message_file("ListAgents")) {
        show_error("Failed to write ListAgents command.");
        return;
    }

    if (call_helper() != 0) {
        show_error("Helper app failed.");
        return;
    }

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
    if (agent_file_exists(AGENTS_FILE)) {
        agent_count = load_agents_from_file(agent_lines, MAX_AGENTS);
    } else {
        call_list_agents_and_save();
        agent_count = load_agents_from_file(agent_lines, MAX_AGENTS);
    }
}
