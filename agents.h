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


int load_agents_from_file(void);


void call_list_agents_and_save(void);
void initialize_agent_list(void);

#endif
