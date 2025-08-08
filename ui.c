#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include "ui.h"
#include "connect.h"
#include "state.h"
#include "common.h"
 
#define ATTR_NORMAL 0x07         /* Light gray on black */


unsigned int ui_get_vmem_segment(void) {
    /* Detect video mode: 0x07 = monochrome, else assume color */
    return (*(unsigned char far *)MK_FP(0x40, 0x49) == 0x07) ? 0xB000 : 0xB800;
}


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
    regs.h.ch = 0x0D;  /* Start line - near bottom */
    regs.h.cl = 0x0F;  /* End line - bottom */
    int86(0x10, &regs, &regs);
}

void ui_clear_status_line(void) {
    gotoxy(1, UI_STATUS_ROW);
    cprintf("%-80s", " ");
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
    clrscr();
    textcolor(LIGHTRED);
    textbackground(BLACK);
    
    gotoxy(20, 13);
    cprintf("%s", message);
    
    gotoxy(20, 15);
    cprintf("Press any key to continue...");
    
    getch();
    
    /* Clear screen after error */
    clrscr();
}



void ui_draw_line_fast(int row, const char* text) {
	unsigned int segment = ui_get_vmem_segment();
	unsigned char far* screen = (unsigned char far*)MK_FP(segment, 0);
  int i, offset = row * SCREEN_COLS * 2;

    for (i = 0; i < SCREEN_COLS; i++) {
        screen[offset++] = (text[i] && text[i] != '\0') ? text[i] : ' ';
        screen[offset++] = ATTR_NORMAL;
    }
}


void ui_clear_screen_fast() {
    unsigned int segment = ui_get_vmem_segment();
    unsigned char far* screen = (unsigned char far*)MK_FP(segment, 0);
    int offset = 0;
    int total_chars = SCREEN_COLS * SCREEN_ROWS;
    char buf[40];

    sprintf(buf, "ui_clear_screen_fast: segment = 0x%X", segment);
    log_message(buf);

    log_message("in ui_clear_screen_fast");

    while (total_chars--) {
        screen[offset++] = ' ';
        screen[offset++] = ATTR_NORMAL;
    }
}


void ui_set_status(const char* msg) {
    gotoxy(2, UI_STATUS_ROW);
    textcolor(LIGHTGREEN);
    textbackground(BLACK);
    cprintf("%-78s", msg);
}


void ui_reset_fullscreen(void) {
    window(1, 1, SCREEN_COLS, SCREEN_ROWS);
    clrscr();
    textcolor(LIGHTGREEN);
    textbackground(BLACK);
}
