#include "../io.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

/**
 * Logs a formatted call to stderr.
 * @param fmt printf-style format string
 * @param ... format arguments
 */
static void log_call(const char* fmt, ...)
{
    fprintf(stderr, "==> ");
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

/**
 * Enters screen mode (logger stub).
 */
void screen_enter(void)
{
    log_call("screen_enter()");
}

/**
 * Leaves screen mode (logger stub).
 */
void screen_leave(void)
{
    log_call("screen_leave()");
}

/**
 * Logs a screen putchar call.
 * @param cur_ch character to display
 */
void screen_putchar(uint8_t cur_ch)
{
    log_call("screen_putchar(%d '%c')",
        cur_ch,
        (cur_ch >= 0x20 && cur_ch < 0x7f) ? (char)cur_ch : '?');
}

/**
 * Logs a screen getchar call.
 * @return 0 always (stub)
 */
uint8_t screen_getchar(void)
{
    log_call("screen_getchar(65535)");

    return 0;
}

/**
 * Logs a setcursor call.
 * @param xpos column
 * @param ypos row
 */
void screen_setcursor(uint8_t xpos, uint8_t ypos)
{
    log_call("screen_setcursor(%d, %d)", xpos, ypos);
}

/**
 * Logs a getcursor call.
 * @return 0 always (stub)
 */
uint16_t screen_getcursor(void)
{
    log_call("screen_getcursor()");

    return 0;
}

/**
 * Logs a setstyle call.
 * @param cur_ch style flag
 */
void screen_setstyle(uint8_t cur_ch)
{
    log_call("screen_setstyle(0x%02x)", cur_ch);
}

/**
 * Logs a getsize call.
 * @return packed size (23 rows, 79 columns)
 */
uint16_t screen_getsize(void)
{
    log_call("screen_getsize() -> (%d,%d)", 23, 79);

    return (uint16_t)(23 << 8) | 79;
}

/**
 * Logs a screen clear call.
 */
void screen_clear(void)
{
    log_call("screen_clear()");
}

/**
 * Logs a scrollup call.
 */
void screen_scrollup(void)
{
    log_call("screen_scrollup()");
}

/**
 * Logs a scrolldown call.
 */
void screen_scrolldown(void)
{
    log_call("screen_scrolldown()");
}

/**
 * Logs an enablecursor call.
 * @param on true to show, false to hide
 */
void screen_enablecursor(bool on)
{
    log_call("screen_enablecursor(%d)", on);
}
