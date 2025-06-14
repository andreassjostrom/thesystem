#include <stdio.h>
#include "common.h"

#define LOGFILE "log.txt"

void log_message(const char* message) {
    FILE* f = fopen(LOGFILE, "a");
    if (!f) return;

    fprintf(f, "%s\n", message);
    fclose(f);
}
