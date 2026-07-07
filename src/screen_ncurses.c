#include "screen.h"
#include <ncurses.h>
#include <term.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

static bool ncurses_active;

void screen_enter(void) {
    if (ncurses_active) return;
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    scrollok(stdscr, TRUE);
    ncurses_active = true;
}

void screen_leave(void) {
    if (!ncurses_active) return;
    endwin();
    ncurses_active = false;
}

void screen_putchar(uint8_t a) {
    if (ncurses_active) {
        addch(a);
        refresh();
    } else {
        putchar(a);
        fflush(stdout);
    }
}

uint8_t screen_getchar(void) {
    if (ncurses_active) {
        int c = getch();
        switch (c) {
            case KEY_UP:    return 0x8b;
            case KEY_DOWN:  return 0x8a;
            case KEY_LEFT:  return 0x88;
            case KEY_RIGHT: return 0x89;
            case KEY_BACKSPACE: return 0x7f;
            case KEY_ENTER: return 0x0d;
            default: return (uint8_t)(c & 0xff);
        }
    } else {
        return (uint8_t)getchar();
    }
}

void screen_setcursor(uint16_t xa) {
    if (ncurses_active) {
        move(xa >> 8, xa & 0xff);
        refresh();
    }
}

uint16_t screen_getcursor(void) {
    if (ncurses_active) {
        int row, col;
        getyx(stdscr, row, col);
        return (uint16_t)(row << 8) | (uint8_t)col;
    }
    return 0;
}

void screen_setstyle(uint8_t a) {
    if (ncurses_active) {
        if (a)
            attron(A_REVERSE);
        else
            attroff(A_REVERSE);
        refresh();
    }
}

uint16_t screen_getsize(void) {
    if (ncurses_active) {
        int h, w;
        getmaxyx(stdscr, h, w);
        return (uint16_t)(h << 8) | (uint8_t)w;
    }
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_row > 0)
        return (uint16_t)(ws.ws_row << 8) | (uint8_t)ws.ws_col;
    return (uint16_t)(24 << 8) | 80;
}

void screen_clear(void) {
    if (ncurses_active) {
        clear();
        refresh();
    } else {
        static bool term_setup = false;
        if (!term_setup) {
            setupterm(NULL, STDOUT_FILENO, NULL);
            term_setup = true;
        }
        putp(tigetstr("clear"));
        fflush(stdout);
    }
}

void screen_scrollup(void) {
    if (ncurses_active) {
        scrl(1);
        refresh();
    }
}

void screen_scrolldown(void) {
    if (ncurses_active) {
        scrl(-1);
        refresh();
    }
}

void screen_enablecursor(bool on) {
    if (ncurses_active) {
        curs_set(on ? 1 : 0);
        refresh();
    }
}
