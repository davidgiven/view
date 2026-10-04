/* CLI input via GNU Readline.  Pressing ESCAPE enters the editor. */

#include "../io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

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
 * @param s string terminated by '\0' or '\r'
 */
void cli_putstring(const char* s)
{
    for (; *s != '\0' && *s != '\r'; s++)
        putchar(*s);
    fflush(stdout);
}

static int escape_pressed;

/**
 * Readline handler for the Escape key.
 * Marks Escape as pressed and aborts the current readline.
 * @param count unused repeat count
 * @param key unused key code
 * @return 0 always
 */
static int escape_handler(int count, int key)
{
    (void)count;
    (void)key;
    escape_pressed = 1;
    rl_point = 0;
    rl_end = 0;
    rl_done = 1;

    return 0;
}

/**
 * Reads a line from CLI input via readline.
 * Handles Escape to enter the editor and CR termination.
 * @param buf destination buffer
 * @param size size of the destination buffer
 * @return true if Escape was pressed, false otherwise
 */
bool cli_readstring(char* buf, size_t size)
{
    escape_pressed = 0;

    rl_variable_bind("keyseq-timeout", "100");
    rl_unbind_key(0x1b);
    rl_bind_key(0x1b, escape_handler);
    rl_set_keyboard_input_timeout(100);
    char* line = readline(NULL);

    if (!line)
        return false;

    if (escape_pressed)
    {
        free(line);

        return true;
    }
    size_t len = strlen(line);

    if (len >= size)
        len = size - 1;
    memcpy(buf, line, len);
    buf[len] = 0x0d;
    free(line);

    return false;
}
