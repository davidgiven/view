#include "../io.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

static void log_call(const char* fmt, ...)
{
    fprintf(stderr, "==> ");
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

void screen_enter(void)
{
    log_call("screen_enter()");
}

void screen_leave(void)
{
    log_call("screen_leave()");
}

void screen_putchar(uint8_t a)
{
    char repr = (a >= 0x20 && a < 0x7f) ? (char)a : '?';
    log_call("screen_putchar(%d '%c')", a, repr);
}

uint8_t screen_getchar(void)
{
    log_call("screen_getchar(65535)");
    return 0;
}

void screen_setcursor(uint8_t xpos, uint8_t ypos)
{
    log_call("screen_setcursor(%d, %d)", xpos, ypos);
}

uint16_t screen_getcursor(void)
{
    log_call("screen_getcursor()");
    return 0;
}

void screen_setstyle(uint8_t a)
{
    log_call("screen_setstyle(0x%02x)", a);
}

uint16_t screen_getsize(void)
{
    log_call("screen_getsize() -> (%d,%d)", 23, 79);
    return (uint16_t)(23 << 8) | 79;
}

void screen_clear(void)
{
    log_call("screen_clear()");
}

void screen_scrollup(void)
{
    log_call("screen_scrollup()");
}

void screen_scrolldown(void)
{
    log_call("screen_scrolldown()");
}

void screen_enablecursor(bool on)
{
    log_call("screen_enablecursor(%d)", on);
}
