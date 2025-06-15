#ifndef AGENTS_H
#define AGENTS_H

#define MAX_AGENTS 20
#define MAX_AGENT_NAME 40
#define MAX_AGENT_DESC 80
#define MAX_AGENT_LINE 80

typedef struct {
    int id;
    char name[MAX_AGENT_NAME];
    char desc[MAX_AGENT_DESC];
} Agent;

extern Agent agent_list[MAX_AGENTS];
extern int agent_count;

int parse_agent_line(const char* line, Agent* agent);
int load_agents(void);

#endif
