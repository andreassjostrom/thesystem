#ifndef SPINNER_H
#define SPINNER_H

#define SPINNER_WIDTH 20
#define SPINNER_ROW 23
#define SPINNER_COL 30

void spinner_tick(int pos, int dir);
void spinner_clear(void);
void spinner_wait(int milliseconds);


#endif