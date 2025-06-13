#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include "ui.h"
#include "connection.h"

void ui_hide_cursor(void) {
    union REGS regs;
    regs.h.ah = 0x01;
    regs.h.ch = 0x20;
    regs.h.cl = 0;
    int86(0x10, &regs, &regs);
}

void ui_show_cursor(void) {
    union REGS regs;
    regs.h.ah = 0x01;
    regs.h.ch = 0x06;
    regs.h.cl = 0x07;
    int86(0x10, &regs, &regs);
}

void ui_clear_status_line(void) {
    gotoxy(1, UI_STATUS_ROW);
    cprintf("%-80s", " ");
}

void ui_set_status(const char* msg) {
    ui_clear_status_line();
    gotoxy(2, UI_STATUS_ROW);
    cprintf("%s", msg);
}


void ui_wait_and_display_response(const char* title) {
    char response[MAX_RESPONSE_LINES][MAX_LINE_LENGTH];
    int line_count, i;

    if (!wait_for_response_file(5000)) {
        ui_set_status("Timeout waiting for response.");
        return;
    }

    line_count = read_response_file(response, MAX_RESPONSE_LINES, 1);
    clrscr();
    textcolor(LIGHTGREEN);

    gotoxy(2, 2);
    cprintf("%s", title ? title : "Response:");

    for (i = 0; i < line_count && i < MAX_RESPONSE_LINES; i++) {
        gotoxy(2, 4 + i);
        cprintf("%s", response[i]);
    }

    gotoxy(2, UI_STATUS_ROW);
    cprintf("Press any key to continue...");
    getch();
}



/**
 * Gets input from user with live editing (ESC/backspace supported).
 * Returns 1 if ENTER pressed, 0 if ESC pressed.
 */
int get_user_input(char* buffer, int max_len, int x, int y, int echo) {
    int pos = 0;
    int ch;

    buffer[0] = '\0';
    gotoxy(x, y);

    while (1) {
        ch = getch();

        if (ch == 13) {  /* ENTER */
            buffer[pos] = '\0';
            return 1;
        }

        if (ch == 27) {  /* ESC */
            buffer[0] = '\0';
            return 0;
        }

        if (ch == 8 && pos > 0) {  /* BACKSPACE */
            pos--;
            buffer[pos] = '\0';
            gotoxy(x + pos, y);
            cprintf(" ");
            gotoxy(x + pos, y);
        }
        else if (ch >= 32 && ch <= 126 && pos < max_len - 1) {  /* Printable */
            buffer[pos++] = ch;
            buffer[pos] = '\0';
            if (echo) cprintf("%c", ch);
        }
    }
}


void delay_ms(unsigned int ms) {
    delay(ms);  /* Turbo C delay in milliseconds */
}





void show_error(const char* message) {
    gotoxy(20, 13);
    cprintf("%s", message);
    getch();
}
