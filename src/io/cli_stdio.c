#include "../io.h"
#include <stdio.h>
#include <string.h>

/**
 * Writes a single character to the CLI output.
 * @param c character to output
 */
void cli_putchar(uint8_t c)
{
    putchar(c);
    fflush(stdout);
}

/**
 * Writes a string to the CLI output.
 * @param s null-terminated string to output
 */
void cli_putstring(const char* s)
{
    fputs(s, stdout);
    fflush(stdout);
}

/**
 * Reads a line from CLI input.
 * Handles newline stripping and CR termination.
 * @param buf destination buffer
 * @param size size of the destination buffer
 * @return true if the line was empty (Escape/empty), false otherwise
 */
bool cli_readstring(char* buf, size_t size)
{
    if (!fgets(buf, (int)size, stdin))
        return false;
    size_t len = strlen(buf);

    if (len > 0 && buf[len - 1] == '\n')
        len--;

    if (len == 0)
    {
        buf[0] = 0x0d;

        return true;
    }
    buf[len] = 0x0d;

    return false;
}
