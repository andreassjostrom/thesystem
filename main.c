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
#include "splash.h"


int main() {
    char input[4];
    char logbuf[120];

    log_message("********** Start app **********");
    
    /* Show splash screen sequence */
		show_splash_sequence();

    /* Check if there is a connection */
    check_connection();

    /* Load agents either from local file or server */
    load_agents();

    while (1) {
        while (kbhit()) getch();  /* Clear leftover keystrokes */
				render_main_menu();

        /* Prompt for input */
        if (!get_user_input(input, 3, 24, 16, 1)) {
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

                sprintf(logbuf, "Main menu: Set current_agent_name = %s", current_agent_name);
                log_message(logbuf);
                
 								log_message("Main: About to call handle_chat");               
                
								if (handle_chat(SYSTEM_AGENT_ID) != SUCCESS) {
								    log_message("Main: Chat session failed.");
								}


								log_message("Main: Returned from handle_chat");
								
                break;

            case '2':
                if (!is_online) {
                    show_error("You're offline. Cannot chat right now.");
                    break;
                }
                show_agent_selector();
                break;

            case '3':
                handle_settings();
                break;

            case '4':
                handle_settings();
                break;
                
            case '5':
                handle_settings();
                break;                

            case '8':
                test_spinner_file();
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
