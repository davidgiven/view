#ifndef SCREEN_H
#define SCREEN_H

#include <stdbool.h>

#define STYLE_NORMAL  0
#define STYLE_REVERSE 1

void screen_putchar(uint8_t a);
uint8_t screen_getchar(void);
void screen_setcursor(uint16_t xa);
uint16_t screen_getcursor(void);
void screen_setstyle(uint8_t a);
uint16_t screen_getsize(void);
void screen_clear(void);
void screen_scrollup(void);
void screen_scrolldown(void);
void screen_enablecursor(bool on);

#endif
