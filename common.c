#include <stdio.h>
#include <dos.h>
#include "common.h"
#include "state.h"

#define LOGFILE "log.txt"


void log_message(const char* message) {
    FILE* f;
    struct time t;
    struct date d;

    if (!is_logging_enabled) return;

    gettime(&t);
    getdate(&d);

    f = fopen("log.txt", "a");
    if (!f) return;

    fprintf(f, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
            d.da_year, d.da_mon, d.da_day,
            t.ti_hour, t.ti_min, t.ti_sec,
            message);

    fflush(f);
    fclose(f);
}

int launch_external_program(const char* exe_name) {
    int result;

    ui_hide_cursor();
    clrscr();
    gotoxy(20, 12);
    cprintf("Launching %s...", exe_name);
    delay_ms(500);

    result = system(exe_name);

    clrscr();
    while (kbhit()) getch();

    if (result == -1) {
        gotoxy(20, 13);
        cprintf("Failed to launch: %s", exe_name);
        delay_ms(1500);
        draw_menu();
        ui_show_cursor();
        return FAILURE;
    }

    draw_menu();
    ui_show_cursor();
    return SUCCESS;
}