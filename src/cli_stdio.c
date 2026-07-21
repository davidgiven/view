#include "cli.h"
#include <stdio.h>
#include <string.h>

void cli_putchar(uint8_t c) {
    putchar(c);
    fflush(stdout);
}

void cli_putstring(const char *s) {
    fputs(s, stdout);
    fflush(stdout);
}

bool cli_readstring(char *buf, size_t size) {
    if (!fgets(buf, (int)size, stdin)) {
        return false;
    }
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
        len--;
    if (len == 0) {
        buf[0] = 0x0d;
        return true;
    }
    buf[len] = 0x0d;
    return false;
}
