#ifndef CLI_H
#define CLI_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Write a single character to the CLI output. */
extern void cli_putchar(uint8_t c);

/* Write a null-terminated string to the CLI output. */
extern void cli_putstring(const char *s);

/* Read a line from the CLI input into *buf (up to size bytes).
 * Strips the trailing newline and terminates with 0x0d.
 * Returns true if an empty line was entered (user pressed Enter
 * at the prompt), indicating the caller should enter the editor.
 * Returns false on EOF/error or when non-empty input was read. */
extern bool cli_readstring(char *buf, size_t size);

#endif
