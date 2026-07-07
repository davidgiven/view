#include "cli.h"
#include <stdio.h>

void cli_putchar(uint8_t c) {
    putchar(c);
}

void cli_putstring(const char *s) {
    fputs(s, stdout);
}

char *cli_readstring(char *buf, size_t size) {
    return fgets(buf, (int)size, stdin);
}
