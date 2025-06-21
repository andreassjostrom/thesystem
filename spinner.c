#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include "spinner.h"
#include "common.h"
#include "ui.h"

#define SPINNER_WIDTH 20
#define SPINNER_ROW 23
#define SPINNER_COL 30

void spinner_tick(int pos, int dir) {
    char line[SPINNER_WIDTH + 1];
    int i;
    for (i = 0; i < SPINNER_WIDTH; i++) line[i] = ' ';
    line[SPINNER_WIDTH] = '\0';

    line[pos] = 'O';
    if (pos - dir >= 0 && pos - dir < SPINNER_WIDTH) line[pos - dir] = 'o';
    if (pos - 2 * dir >= 0 && pos - 2 * dir < SPINNER_WIDTH) line[pos - 2 * dir] = '.';

    gotoxy(SPINNER_COL, SPINNER_ROW);
    cprintf("%s", line);

    log_message("spinner_tick updated line.");
}

void spinner_wait(int milliseconds) {
    int pos = 0, dir = 1;
    int steps = milliseconds / 100;

		ui_hide_cursor();
    while (steps-- > 0) {
        spinner_tick(pos, dir);
        delay(50);
        pos += dir;
        if (pos == SPINNER_WIDTH - 1 || pos == 0) dir = -dir;
    }
		ui_show_cursor();

    spinner_clear();
}

void spinner_clear(void) {
    gotoxy(SPINNER_COL, SPINNER_ROW);
    cprintf("%-20s", " ");
    log_message("spinner_clear executed.");
}