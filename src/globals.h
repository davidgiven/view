#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <setjmp.h>

// ─── Constants
// ────────────────────────────────────────────────────────────────

#define MAX_LINE_LENGTH 132
#define LINE_LENGTH_SPARE 150
#define ARRAY_SIZE(cur_ch) (sizeof(cur_ch) / sizeof((cur_ch)[0]))
#define MAX_COMMAND_LENGTH 68
#define JMP_CLI 1
#define JMP_EDITOR 2
#define RAM_MAX 0xffff
#define CTRL(c) ((uint8_t)((c) & 0x1f))
#define RULER_INDEX_SIZE 128
#define RAM_CURRENT_LINE_BUF 0x0545
#define RAM_EDIT_BUFFER 0x0548
#define RAM_JUST_BEFORE_RULER_BUF 0x05CC

// ─── Type definitions ───────────────────────────────────────────────────────

typedef enum
{
    NO_COMMAND_PREFIX = 0,
    COMMAND_PREFIX = 0x80, /* format command */
    RULER_PREFIX = 0x81,   /* ruler line */
} command_prefix_t;

typedef enum
{
    NO_CONTROL_CODE = 0,
    HIGHLIGHT1_CODE, /* 0x1c highlight 1 toggle */
    HIGHLIGHT2_CODE, /* 0x1d highlight 2 toggle */
} control_code_t;

typedef enum
{
    CLI_CMD_OK,        /** Command parsed and processed. */
    CLI_CMD_NO_TARGET, /** No command given. */
    CLI_CMD_NO_STRING  /** Area empty or no search string. */
} cli_cmd_status_t;

typedef enum
{
    AREA_NOT_EMPTY,
    AREA_EMPTY
} area_status_t;

typedef enum
{
    READ_BLOCK_EMPTY, /* Nothing was read. */
    READ_BLOCK_DONE,  /* Reached end of file. */
    READ_BLOCK_MORE   /* Block filled to limit, more data remains. */
} read_block_status_t;

typedef enum
{
    FORMAT_OK,         /** Formatting succeeded. */
    FORMAT_AT_END,     /** Reached end of document. */
    FORMAT_MEMORY_FULL /** Document write failed due to insufficient memory. */
} format_result_t;

typedef struct __attribute__((packed, aligned(1))) line
{
    uint8_t prefix_byte;
    uint8_t command[2];
    uint8_t text[MAX_LINE_LENGTH];
    uint8_t extra[3];
} line_t;

typedef struct edit_state
{
    uint8_t pos;
} edit_state_t;

typedef struct printer_driver
{
    void (*print_char)(uint8_t cur_ch);
    void (*printer_on)(void);
    void (*printer_off)(void);
    void (*printer_microspace)(void);
    void (*printer_getflags)(uint8_t* idx, uint8_t* pos);
} printer_driver_t;

typedef struct scan_state
{
    uint8_t ch;  // character found at the scan position
    uint8_t pos; // index of that character into input_buffer
} scan_state_t;

typedef struct pointer_array_t
{
    uint8_t* markers_array[6];
    uint8_t* area_start_ptr;
    uint8_t* area_end_ptr;
    uint8_t* area_insert_ptr;
    uint8_t* search_cursor_ptr;
    uint8_t* search_limit_ptr;
} pointer_array_t;

#endif
