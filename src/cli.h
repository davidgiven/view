#ifndef CLI_H
#define CLI_H

#include <stdint.h>
#include <stddef.h>

void cli_putchar(uint8_t c);
void cli_putstring(const char *s);
char *cli_readstring(char *buf, size_t size);

#endif
