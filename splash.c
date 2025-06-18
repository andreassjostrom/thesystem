#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <dos.h>
#include "splash.h"
#include "common.h"
#include "ui.h"

int render_template_file(const char* filename) {
    FILE* f;
    char line[81];
    int y = 1;
    int len;

    f = fopen(filename, "r");
    if (!f) return FAILURE;

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

    while (1) {
        sprintf(filename, "splash%d.txt", index);
        if (render_template_file(filename) != SUCCESS) break;
        delay(delay_ms);
        index++;
    }

    return SUCCESS;
}