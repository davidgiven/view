#include "../io.h"
#include <ncurses.h>
#include <term.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>
static bool ncurses_active;

/**
 * Enters ncurses screen mode.
 * Initialises ncurses and configures input modes.
 */
void screen_enter(void)
{
    if (ncurses_active)
        return;
    initscr();
    raw();
    nonl();
    noecho();
    keypad(stdscr, TRUE);
    set_escdelay(100);
    scrollok(stdscr, FALSE);
    ncurses_active = true;
}

/**
 * Leaves ncurses screen mode.
 * Restores the terminal to normal state.
 */
void screen_leave(void)
{
    if (!ncurses_active)
        return;
    endwin();
    ncurses_active = false;
}

/**
 * Writes a character to the screen.
 * @param cur_ch character to display
 */
void screen_putchar(uint8_t cur_ch)
{
    assert(cur_ch != 0 && "screen_putchar called with NUL");

    if (ncurses_active)
    {
        addch(cur_ch);
#if defined(TEST_HARNESS)
        /*
         * Workaround for pyte bug: DECSC/DECRC (save/restore cursor)
         * produces wrong cursor row when cur_ch DECSTBM (scroll region) is set
         * between them, causing scroll-region RI (\x1bM) to silently
         * move the cursor instead of scrolling.  ncurses emits this exact
         * pattern for scroll-based redraws.  Flushing each character
         * individually avoids batching DECSC+DECSTBM+DECRC in the same
         * VT100 chunk.
         */
        refresh();
#endif
    }
    else
    {
        putchar(cur_ch);
        fflush(stdout);
    }
}

/**
 * Reads a character from the screen input.
 * Maps ncurses keys to internal codes.
 * @return key code
 */
uint8_t screen_getchar(void)
{
    if (ncurses_active)
    {
        int c = getch();

        switch (c)
        {
            case KEY_UP:

                return SCREEN_KEY_UP;

            case KEY_DOWN:

                return SCREEN_KEY_DOWN;

            case KEY_LEFT:

                return SCREEN_KEY_LEFT;

            case KEY_RIGHT:

                return SCREEN_KEY_RIGHT;

            case KEY_BACKSPACE:

                return 0x7f;

            default:
                return (uint8_t)(c & 0xff);
        }
    }
    else
    {
        return (uint8_t)getchar();
    }
}

/**
 * Sets the cursor position.
 * @param xpos column
 * @param ypos row
 */
void screen_setcursor(uint8_t xpos, uint8_t ypos)
{
    if (ncurses_active)
        move(ypos, xpos);
}

/**
 * Gets the current cursor position.
 * @return packed position (row in high byte, column in low byte)
 */
uint16_t screen_getcursor(void)
{
    if (ncurses_active)
    {
        int row, col;

        getyx(stdscr, row, col);

        return (uint16_t)(row << 8) | (uint8_t)col;
    }
    return 0;
}

/**
 * Sets the screen style.
 * @param cur_ch non-zero for reverse video, zero for normal
 */
void screen_setstyle(uint8_t cur_ch)
{
    if (ncurses_active)
    {
        if (cur_ch)
            attron(A_REVERSE);
        else
            attroff(A_REVERSE);
    }
}

/**
 * Gets the screen size.
 * @return packed size (rows in high byte, columns in low byte)
 */
uint16_t screen_getsize(void)
{
    if (ncurses_active)
    {
        int h, w;

        getmaxyx(stdscr, h, w);

        return (uint16_t)((h - 1) << 8) | (uint8_t)(w - 1);
    }
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_row > 0)
        return (uint16_t)((ws.ws_row - 1) << 8) | (uint8_t)(ws.ws_col - 1);
    return (uint16_t)(23 << 8) | 79;
}

/**
 * Clears the screen.
 */
void screen_clear(void)
{
    if (ncurses_active)
    {
        clear();
        refresh();
    }
    else
    {
        static bool term_setup = false;

        if (!term_setup)
        {
            setupterm(NULL, STDOUT_FILENO, NULL);
            term_setup = true;
        }
        putp(tigetstr("clear"));
        fflush(stdout);
    }
}

/**
 * Scrolls the screen up by one line.
 */
void screen_scrollup(void)
{
    if (ncurses_active)
    {
        scrollok(stdscr, TRUE);
        scrl(1);
        scrollok(stdscr, FALSE);
    }
}

/**
 * Scrolls the screen down by one line.
 */
void screen_scrolldown(void)
{
    if (ncurses_active)
    {
        scrollok(stdscr, TRUE);
        scrl(-1);
        scrollok(stdscr, FALSE);
    }
}

/**
 * Enables or disables the cursor.
 * @param on true to show the cursor, false to hide it
 */
void screen_enablecursor(bool on)
{
    if (ncurses_active)
        curs_set(on ? 1 : 0);
}
