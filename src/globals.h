#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <setjmp.h>

#define MAX_LINE_LENGTH 132
#define LINE_LENGTH_SPARE 150
#define ARRAY_SIZE(cur_ch) (sizeof(cur_ch) / sizeof((cur_ch)[0]))
#define MAX_COMMAND_LENGTH 68
#define RAM_MAX 0xffff
#define CTRL(c) ((uint8_t)((c) & 0x1f))
#define DEFAULT_RULER_INDEX_SIZE 128

#define JMP_CLI 1
#define JMP_EDITOR 2

#endif
