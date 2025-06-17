#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include "ui.h"
#include "connect.h"
#include "state.h"

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
