#ifndef IO_H
#define IO_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// ─── CLI ──────────────────────────────────────────────────────
extern void cli_putchar(uint8_t c);
extern void cli_putstring(const char *s);
extern bool cli_readstring(char *buf, size_t size);
// ─── Screen ───────────────────────────────────────────────────
#define STYLE_NORMAL  0
#define STYLE_REVERSE 1

#define SCREEN_KEY_UP    0x8b
#define SCREEN_KEY_DOWN  0x8a
#define SCREEN_KEY_LEFT  0x88
#define SCREEN_KEY_RIGHT 0x89

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
