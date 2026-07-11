#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#include <stdbool.h>

#define STYLE_NORMAL  0
#define STYLE_REVERSE 1

extern void screen_putchar(uint8_t a);
extern uint8_t screen_getchar(void);
extern void screen_setcursor(uint8_t xpos, uint8_t ypos);
extern uint16_t screen_getcursor(void);
extern void screen_setstyle(uint8_t a);
extern uint16_t screen_getsize(void);
extern void screen_enter(void);
extern void screen_leave(void);
extern void screen_clear(void);
extern void screen_scrollup(void);
extern void screen_scrolldown(void);
extern void screen_enablecursor(bool on);

#endif
