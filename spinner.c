#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include "spinner.h"

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
}

void spinner_clear(void) {
    gotoxy(SPINNER_COL, SPINNER_ROW);
    cprintf("%-20s", " ");
}
