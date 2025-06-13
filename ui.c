#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include "ui.h"

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
