#include <stdio.h>
#include <dos.h>
#include "common.h"

#define LOGFILE "log.txt"


void log_message(const char* message) {
    FILE* f;
    struct time t;
    struct date d;

    f = fopen(LOGFILE, "a");
    if (!f) return;

    gettime(&t);
    getdate(&d);

    fprintf(f, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
            d.da_year, d.da_mon, d.da_day,
            t.ti_hour, t.ti_min, t.ti_sec,
            message);

		fflush(f); 
    fclose(f);
}