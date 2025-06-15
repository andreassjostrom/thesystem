#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <stdlib.h>
#include "agents.h"
#include "chat.h"
#include "ui.h"
#include "connect.h"
#include "state.h"
#include "common.h"



int main() {
    char input[4];


		log_message("********** Start app **********");

    /* Check if there is a connection */
		check_connection();
		
		/* Load agents either from local file or server */
    load_agents();

    while (1) {
        while (kbhit()) getch();  /* Clear leftover keystrokes */
        draw_menu();

        /* Prompt for input */
        if (!get_user_input(input, 3, 39, 11, 1)) {
            continue;  /* ESC pressed skip and redraw */
        }

        switch (input[0]) {
            case '1':
                if (!is_online) {
                    show_error("You're offline. Cannot chat right now.");
                    break;
                }

                strncpy(current_agent_name, agent_list[SYSTEM_AGENT_ID].name, MAX_AGENT_NAME - 1);
                current_agent_name[MAX_AGENT_NAME - 1] = '\0';

                if (testing_mode) {
                    strcpy(session_id, "fb4fd402");
                    handle_chat(SYSTEM_AGENT_ID);
                } else {
                    if (start_chat_session(SYSTEM_AGENT_ID) == SUCCESS) {
                        handle_chat(SYSTEM_AGENT_ID);
                    } else {
                        show_error("Failed to start session with THE SYSTEM.");
                    }
                }
                break;

            case '2':
                if (!is_online) {
                    show_error("You're offline. Cannot chat right now.");
                    break;
                }
                handle_chat_with_agent();
                break;

            case '3':
                handle_settings();
                break;

            case '8':
                test_spinner();
								return SUCCESS;


            case '9':
                handle_exit();
								return SUCCESS;

            default:
                gotoxy(20, 13);
                cprintf("Invalid selection. Press any key...");
                getch();
                break;
        }
    }

	return SUCCESS;  /* Unreachable, but included for completeness */
}


