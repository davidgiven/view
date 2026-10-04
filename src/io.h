#ifndef IO_H
#define IO_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Writes a single character to the CLI output.
 * @param c character to output
 */
extern void cli_putchar(uint8_t c);

/**
 * Writes a string to the CLI output.
 * @param s null-terminated string to output
 */
extern void cli_putstring(const char* s);

/**
 * Reads a line from CLI input.
 * Handles newline stripping and CR termination; Escape is treated as empty.
 * @param buf destination buffer
 * @param size size of the destination buffer
 * @return true if Escape was pressed or the line was empty, false otherwise
 */
extern bool cli_readstring(char* buf, size_t size);

#define STYLE_NORMAL 0
#define STYLE_REVERSE 1

#define SCREEN_KEY_UP 0x8b
#define SCREEN_KEY_DOWN 0x8a
#define SCREEN_KEY_LEFT 0x88
#define SCREEN_KEY_RIGHT 0x89

/**
 * Writes a character to the screen.
 * @param cur_ch character to display
 */
extern void screen_putchar(uint8_t cur_ch);

/**
 * Reads a character from the screen input.
 * Maps special keys to internal codes (e.g. arrow keys to SCREEN_KEY_*).
 * @return key code
 */
extern uint8_t screen_getchar(void);

/**
 * Sets the cursor position.
 * @param xpos column
 * @param ypos row
 */
extern void screen_setcursor(uint8_t xpos, uint8_t ypos);

/**
 * Gets the current cursor position.
 * @return packed position (row in high byte, column in low byte)
 */
extern uint16_t screen_getcursor(void);

/**
 * Sets the screen style.
 * @param cur_ch non-zero for reverse video, zero for normal
 */
extern void screen_setstyle(uint8_t cur_ch);

/**
 * Gets the screen size.
 * @return packed size (rows in high byte, columns in low byte)
 */
extern uint16_t screen_getsize(void);

/**
 * Enters screen mode.
 * Initialises the screen backend and configures input modes.
 */
extern void screen_enter(void);

/**
 * Leaves screen mode.
 * Restores the terminal to its normal state.
 */
extern void screen_leave(void);

/**
 * Clears the screen.
 */
extern void screen_clear(void);

/**
 * Scrolls the screen up by one line.
 */
extern void screen_scrollup(void);

/**
 * Scrolls the screen down by one line.
 */
extern void screen_scrolldown(void);

/**
 * Enables or disables the cursor.
 * @param on true to show the cursor, false to hide it
 */
extern void screen_enablecursor(bool on);

#endif
