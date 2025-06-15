#include <stdio.h>
#include <string.h>
#include "agents.h"
#include "connect.h"
#include "state.h"
#include "common.h"


void initialize_agent_list(void) {
    char buf[64];

    log_message("begin: in initialize_agent_list");
    log_message("Before file_exists check");
    
    log_message("Using AGENTS_FILE:");
		log_message(AGENTS_FILE);

		if (file_exists(AGENTS_FILE) != SUCCESS) {
        log_message("AGENTS_FILE does not exist, calling fetch...");
        call_list_agents_and_save();
    } else {
        log_message("AGENTS_FILE exists, loading from file...");
        
        agent_count = load_agents_from_file();

        if (agent_count == 0) {
            log_message("AGENTS_FILE was empty or invalid. Refetching...");
            call_list_agents_and_save();  /* fallback fetch */
        }
    }

    sprintf(buf, "agent_count = %d", agent_count);
    log_message(buf);
}

int load_agents_from_file(void) {
    FILE* f = fopen(AGENTS_FILE, "r");
    int count = 0;
    char line[MAX_AGENT_LINE];
    char logbuf[160];

    if (!f) return FAILURE;

    log_message("begin: in load_agents_from_file");

    /* Skip session ID line */
    if (!fgets(line, sizeof(line), f)) {
        fclose(f);
        return FAILURE;
    }

    while (fgets(line, sizeof(line), f) && count < MAX_AGENTS) {
        int len = strlen(line);

        /* Trim newline */
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[--len] = '\0';
        }

        strncpy(agent_lines[count], line, MAX_AGENT_LINE - 1);
        agent_lines[count][MAX_AGENT_LINE - 1] = '\0';

        sprintf(logbuf, "Loaded agent line: '%s'", agent_lines[count]);
        log_message(logbuf);

        count++;
    }

    fclose(f);
    return count;
}




void call_list_agents_and_save(void) {
    FILE* in;
    FILE* out;
    char line[128];
    char logbuf[160];

    log_message("begin: in call_list_agents_and_save");

    if (send_message_to_helper("ListAgents") != SUCCESS) {
        log_message("send_message_to_helper failed.");
        show_error("Failed to fetch agent list.");
        return;
    }
    log_message("send_message_to_helper returned SUCCESS");

    log_message("Calling wait_for_response_file...");
    if (wait_for_response_file(5000) != SUCCESS) {
        log_message("wait_for_response_file returned FAILURE");
        show_error("Timeout waiting for ListAgents response.");
        return;
    }
    log_message("wait_for_response_file returned SUCCESS");

    log_message("Opening response.txt for read...");
    in = fopen(RESP_FILE, "r");
    if (in) log_message("Successfully opened response.txt");
    else log_message("Failed to open response.txt");

    log_message("Opening agents file for write...");
    out = fopen(AGENTS_FILE, "w");
    if (out) log_message("Successfully opened agents file");
    else log_message("Failed to open agents file");

    if (!in || !out) {
        if (in) fclose(in);
        if (out) fclose(out);
        show_error("Failed to open files for agent caching.");
        return;
    }

    while (fgets(line, sizeof(line), in)) {
        sprintf(logbuf, "Caching line: '%s'", line);
        log_message(logbuf);
        fputs(line, out);
    }

    fclose(in);
    fclose(out);
    log_message("Finished writing agents file.");
}






