#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <dos.h>
#include "splash.h"
#include "common.h"
#include "ui.h"
#include "spinner.h"
#include "state.h"


int render_template_file(const char* filename) {
    FILE* f;
    char line[81], logbuf[100];
    int y = 1;
    int len;

    f = fopen(filename, "r");
    if (!f) {
        sprintf(logbuf, "render_template_file: failed to open %s", filename);
        log_message(logbuf);
        return FAILURE;
    }

    clrscr();
    textcolor(LIGHTGREEN);
    textbackground(BLACK);

    while (fgets(line, sizeof(line), f) && y <= 24) {
        len = strlen(line);
        if (line[len - 1] == '\n' || line[len - 1] == '\r') line[--len] = '\0';
        while (len < 80) line[len++] = ' ';
        line[80] = '\0';

        gotoxy(1, y++);
        cprintf("%s", line);
    }

    fclose(f);
    return SUCCESS;
}


int show_splash_sequence(void) {
    char filename[20];
    int index = 1;
    int delay_ms = 1500;
    char logbuf[64];

    log_message("show_splash_sequence: starting");

    while (1) {
    	  sprintf(filename, "%ssplash%d.txt", DATA_FOLDER, index);
        sprintf(logbuf, "show_splash_sequence: checking %s", filename);
        log_message(logbuf);

        if (render_template_file(filename) != SUCCESS) break;

        delay(delay_ms);
        index++;
    }

    log_message("show_splash_sequence: ended");

    return SUCCESS;
}

int show_agent_splash_screen(int agent_id) {
    char filename[40], logbuf[80];

    sprintf(filename, "%sagent%d.txt", DATA_FOLDER, agent_id);
    sprintf(logbuf, "show_agent_splash_screen: checking %s", filename);
    log_message(logbuf);

    if (file_exists(filename) != SUCCESS) {
        log_message("show_agent_splash_screen: file does not exist");
        return FAILURE;
    }

    ui_hide_cursor();

    if (render_template_file(filename) == SUCCESS) {
        log_message("show_agent_splash_screen: splash rendered successfully");
        if (spinner_enabled) spinner_wait(3000);
        ui_show_cursor();
        return SUCCESS;
    }

    log_message("show_agent_splash_screen: render failed");
    ui_show_cursor();
    return FAILURE;
}

int show_credits_sequence(void) {
    char filename[20], logbuf[64];
    int index = 1;
    int key;

    ui_hide_cursor();
    log_message("show_credits_sequence: starting");

    while (1) {
    	  sprintf(filename, "%scredits%d.txt", DATA_FOLDER, index);
        sprintf(logbuf, "show_credits_sequence: checking %s", filename);
        log_message(logbuf);

        if (render_template_file(filename) != SUCCESS) {
            break; /* no more files */
        }

        gotoxy(26, 25); /* near bottom center */
				textcolor(LIGHTGREEN);
				textbackground(BLACK);
        cprintf("[Press any key to continue...]");

        key = getch();
        if (key == 27) {
            log_message("show_credits_sequence: ESC pressed, exiting early");
            break;
        }
        
        index++;
    }
    


    log_message("show_credits_sequence: ended");
    ui_show_cursor();
    return SUCCESS;
}

