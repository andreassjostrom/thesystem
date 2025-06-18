#ifndef SPLASH_H
#define SPLASH_H

#define SPLASH_MAX_WIDTH 80
#define SPLASH_MAX_HEIGHT 24
#define SPLASH_BASE_NAME "splash"
#define SPLASH_FILE_EXT ".txt"
#define SPLASH_DELAY_MS 1500

int show_splash_sequence(void);                        /* Cycles splash1.txt, splash2.txt, etc. */
int render_template_file(const char* filename);        /* Generic 80x24 screen renderer */

#endif