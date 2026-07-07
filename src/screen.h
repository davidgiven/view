#ifndef SCREEN_H
#define SCREEN_H

#include <stdbool.h>

void screen_putchar(void);
void screen_getchar(void);
void screen_setcursor(void);
void screen_getcursor(void);
void screen_setstyle(void);
void screen_getsize(void);
void screen_clear(void);
void screen_scrollup(void);
void screen_scrolldown(void);
void screen_enablecursor(bool on);

#endif
