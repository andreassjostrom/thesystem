#include <stdio.h>
#include <string.h>
#include "common.h"
#include "agents.h"
#include "connect.h"
#include "ui.h"


Agent agent_list[MAX_AGENTS];
int agent_count = 0;

int load_agents(void) {
    FILE *f, *in, *out;
    char line[MAX_AGENT_LINE];
    agent_count = 0;

    if (file_exists(AGENTS_FILE) != SUCCESS) {
        if (write_message_file("ListAgents") != SUCCESS) {
            log_message("Failed to write ListAgents command.");
            show_error("Failed to write ListAgents command.");
            return FAILURE;
        }

        if (call_helper() != SUCCESS) {
            log_message("Helper app failed.");
            show_error("Helper app failed.");
            return FAILURE;
        }

        in = fopen(RESP_FILE, "r");
        out = fopen(AGENTS_FILE, "w");
        if (in && out) {
            while (fgets(line, sizeof(line), in)) {
                fputs(line, out);
            }
        }
        if (in) fclose(in);
        if (out) fclose(out);

        delay(100);  /* Let DOS file system settle */
        log_message("Saved agents file");
    }

    f = fopen(AGENTS_FILE, "r");
    if (!f) {
        log_message("Failed to open agents.txt for reading.");
        return FAILURE;
    }

    while (fgets(line, sizeof(line), f)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
            line[--len] = '\0';

        if (len == 0) continue;
        if (agent_count >= MAX_AGENTS) break;

        if (parse_agent_line(line, &agent_list[agent_count]) == SUCCESS) {
            char logbuf[120];
            sprintf(logbuf, "Loaded agent: %s [%d]", agent_list[agent_count].name, agent_list[agent_count].id);
            log_message(logbuf);
            agent_count++;
        }
    }

    fclose(f);
    return SUCCESS;
}


int parse_agent_line(const char* line, Agent* agent) {
    char temp[MAX_AGENT_LINE];
    char *id_part, *name_part, *desc_part;
    char *name_end;
    int name_len;

    strncpy(temp, line, MAX_AGENT_LINE - 1);
    temp[MAX_AGENT_LINE - 1] = '\0';

    id_part = strstr(temp, "ID: ");
    name_part = strstr(temp, "Name: ");
    desc_part = strstr(temp, "Desc: ");

    if (!id_part || !name_part || !desc_part)
        return FAILURE;

    agent->id = atoi(id_part + 4);

    name_end = strchr(name_part, '|');
    if (!name_end) return FAILURE;

    name_len = name_end - (name_part + 6);
    if (name_len <= 0 || name_len >= MAX_AGENT_NAME) return FAILURE;

    strncpy(agent->name, name_part + 6, name_len);
    agent->name[name_len] = '\0';

    strncpy(agent->desc, desc_part + 6, MAX_AGENT_DESC - 1);
    agent->desc[MAX_AGENT_DESC - 1] = '\0';

    return SUCCESS;
}

