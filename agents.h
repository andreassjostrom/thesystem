#ifndef _AGENTS_H
#define _AGENTS_H

#define AGENTS_FILE "agents.txt"
#define MAX_AGENTS 20
#define MAX_AGENT_NAME 40
#define MAX_AGENT_DESC 80
#define MAX_AGENT_LINE 80

/* External variables shared with main.c */
extern int agent_count;
extern char agent_lines[MAX_AGENTS][MAX_AGENT_LINE];

/* Agent handling functions */
int agent_file_exists(const char* filename);
int load_agents_from_file(char lines[][MAX_AGENT_LINE], int max_lines);
void call_list_agents_and_save(void);
void initialize_agent_list(void);

/* External dependencies needed by agents.c */
int write_message_file(const char* message);
int call_helper(void);
void show_error(const char* message);

#define RESP_FILE "response.txt"

#endif