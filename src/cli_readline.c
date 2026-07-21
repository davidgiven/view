/* CLI input via GNU Readline.  Pressing ESCAPE enters the editor. */

#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

void cli_putchar(uint8_t c) {
    putchar(c);
    fflush(stdout);
}

void cli_putstring(const char *s) {
    fputs(s, stdout);
    fflush(stdout);
}

static int escape_pressed;

static int escape_handler(int count, int key) {
    (void)count; (void)key;
    escape_pressed = 1;
    rl_point = 0;
    rl_end = 0;
    rl_done = 1;
    return 0;
}

bool cli_readstring(char *buf, size_t size) {
    escape_pressed = 0;

    rl_variable_bind("keyseq-timeout", "100");
    rl_unbind_key(0x1b);
    rl_bind_key(0x1b, escape_handler);
    rl_set_keyboard_input_timeout(100);
    char *line = readline(NULL);
    if (!line) return false;
    if (escape_pressed) {
        free(line);
        return true;
    }
    size_t len = strlen(line);
    if (len >= size) len = size - 1;
    memcpy(buf, line, len);
    buf[len] = 0x0d;
    free(line);
    return false;
}
