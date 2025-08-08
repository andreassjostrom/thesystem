#include <string.h>
#include "chatlog.h"
#include "common.h"
#include "ui.h"

static char chatlog[CHATLOG_MAX_LINES][CHATLOG_LINE_BYTES];
static int chatlog_count = 0;
static int scroll_index = 0;
static int blocked = 0;

void chatlog_init(void) {
    chatlog_count = 0;
    scroll_index = 0;
    blocked = 0;
}

void chatlog_clear(void) {
    chatlog_init();
}

int chatlog_blocked(void) {
    return blocked;
}

int chatlog_line_count(void) {
    return chatlog_count;
}

int chatlog_scroll_index(void) {
    return scroll_index;
}

void chatlog_scroll(int direction) {
    int max_scroll = (chatlog_count > CHAT_DISPLAY_LINES) ? (chatlog_count - CHAT_DISPLAY_LINES) : 0;
    scroll_index += direction;
    if (scroll_index < 0) scroll_index = 0;
    if (scroll_index > max_scroll) scroll_index = max_scroll;
}

void chatlog_scroll_reset_to_bottom(void) {
    if (chatlog_count > CHAT_DISPLAY_LINES) {
        scroll_index = chatlog_count - CHAT_DISPLAY_LINES;
    } else {
        scroll_index = 0;
    }
}



int chatlog_get_visible_lines(char lines[][CHATLOG_LINE_BYTES], int max_lines) {
    int count = 0;
    int i, max;
    char logbuf[80];

    /* Determine how many lines to return */
    max = scroll_index + max_lines;
    if (max > chatlog_count) {
        max = chatlog_count;
    }

    for (i = scroll_index; i < max; i++, count++) {
        strncpy(lines[count], chatlog[i], CHATLOG_LINE_BYTES - 1);
        lines[count][CHATLOG_LINE_BYTES - 1] = '\0';
    }

    /* Log how many lines are being returned and from where */
    sprintf(logbuf, "chatlog_get_visible_lines: scroll=%d count=%d", scroll_index, count);
    log_message(logbuf);

    return count;
}





int wrap_text_into_lines(const char* input, char lines[][CHATLOG_LINE_BYTES], int max_lines, int max_width) {
    int count = 0;
    int len = strlen(input);
    int start = 0;
    int end, i;

    while (start < len && count < max_lines) {
        end = start + max_width;

        if (end >= len) {
            strncpy(lines[count], input + start, len - start);
            lines[count][len - start] = '\0';
            count++;
            break;
        }

        /* Look backwards for a space */
        i = end;
        while (i > start && input[i] != ' ') {
            i--;
        }

        if (i == start) {
            /* No space found — force break at max_width */
            strncpy(lines[count], input + start, max_width);
            lines[count][max_width] = '\0';
            start += max_width;
        } else {
            /* Break at last space */
            strncpy(lines[count], input + start, i - start);
            lines[count][i - start] = '\0';
            start = i + 1;
        }

        /* Skip leading spaces for next line */
        while (input[start] == ' ') {
            start++;
        }

        count++;
    }

    return count;
}


void chatlog_add(const char* raw_line) {
    int i;
    char wrapped[CHATLOG_WRAP_TEMP_LINES][CHATLOG_LINE_BYTES];
    int count;

    log_message("chatlog_add: START");

    if (blocked || chatlog_count >= CHATLOG_MAX_LINES - CHATLOG_WRAP_TEMP_LINES) {
        if (!blocked) {
            strncpy(chatlog[chatlog_count++], "SYSTEM: Chat limit reached. Restart required.", CHATLOG_LINE_BYTES - 1);
            chatlog[chatlog_count - 1][CHATLOG_LINE_BYTES - 1] = '\0';
            blocked = 1;
        }
        log_message("chatlog_add: BLOCKED");
        return;
    }

    count = wrap_text_into_lines(raw_line, wrapped, CHATLOG_WRAP_TEMP_LINES, CHATLOG_LINE_WIDTH);

    for (i = 0; i < count && chatlog_count < CHATLOG_MAX_LINES; i++) {
        char logbuf[128];
        strncpy(chatlog[chatlog_count++], wrapped[i], CHATLOG_LINE_BYTES - 1);
        chatlog[chatlog_count - 1][CHATLOG_LINE_BYTES - 1] = '\0';

        sprintf(logbuf, "chatlog_add: line %d: %s", chatlog_count - 1, wrapped[i]);
        log_message(logbuf);
    }

    chatlog_scroll_reset_to_bottom();
    log_message("chatlog_add: DONE");
}
