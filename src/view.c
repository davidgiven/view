/**
 * View - main module.
 * C translation of Acornsoft View word processor.
 */

#include <ctype.h>
#include <fcntl.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "io.h"

#include "globals.h"
#include "macro.h"
#include "printing.h"

jmp_buf env;
#define JMP_CLI 1
#define JMP_EDITOR 2

#define CTRL(c) ((uint8_t)((c) & 0x1f))

#define MAX_COMMAND_LENGTH 68
#define MAX_LINE_LENGTH 132

static void system_init(void);

#include "io.h"

#include "document.h"
#include "cli.h"
#include "editor.h"

uint8_t* ram;

uint8_t current_ruler_buffer[133];

line_t current_line_buffer;

uint8_t* edit_buffer_base; /** Base of edit buffer. */
line_t*
    current_format_line; /** Current formatting line pointer (edit buffer). */
line_t* heap_format_line_ptr;    /** Heap formatting line pointer. */
uint8_t* current_ruler_ptr;      /** Pointer into current ruler buffer. */
uint8_t* current_line_ptr;       /** Walking cursor into document heap. */
uint8_t* top;                    /** End of document heap (first free byte). */
uint8_t* himem;                  /** Top of available memory. */
uint8_t* top_of_screen_line_ptr; /** Document line at top of screen. */
uint8_t*
    print_source_ptr;   /** Source pointer for printing document from memory. */
uint8_t* print_doc_ptr; /** Document pointer used during printing. */
const printer_driver_t* printer_driver_ptr; /** Active printer driver. */
uint8_t* macro_cursor_ptr;              /** Cursor into current macro body. */
uint8_t* ruler_index[RULER_INDEX_SIZE]; /** Ruler index stack. */
uint8_t printing_from_file_flag;
int saved_ruler_index_scroll; /** Index into ruler_index[] */
uint8_t column_position;
uint8_t ruler_buffer_len;
uint8_t file_edit_flags;
uint8_t xpos;
uint8_t input_file_empty_flag;
uint8_t justify_gap_count;
uint8_t cli_output_pos;   /** Output buffer write index for CLI. */
uint8_t cli_header_pos;   /** Header field position for CLI. */
uint8_t cli_header_limit; /** Header loop limit for CLI. */
uint8_t* doc_working_ptr; /** Document working pointer. */
uint8_t print_flags;
uint8_t edit_buffer_dirty_flag;
uint8_t edit_buffer_unpacked_flag;
uint8_t scroll_repeat_count;
int ruler_index_ptr; /** Index into ruler_index[] */
uint8_t hscroll_pos;
uint8_t visual_column;
uint8_t display_start_row;
uint8_t line_counter;
uint8_t flags_need_redrawing_flag;
uint8_t status_line_needs_redrawing_flag;
uint8_t ypos;
uint8_t print_xpos;
uint8_t line_change_pending_flag;
uint8_t search_target_len;
uint8_t cursor_moved_flag;
uint8_t delimiter_char; /** Delimiter character used during parsing. */
uint8_t
    line_format_status; /** Format status byte (marker count and flush tag). */
uint8_t input_buffer_offset;
uint8_t scratch_offset;    /** Generic scratch offset. */
uint8_t scratch_index;     /** Generic scratch index. */
uint8_t screen_row;        /** Generic screen row scratch. */
uint8_t screen_column;     /** Generic screen column scratch. */
uint8_t temp_save;         /** Generic temporary save. */
uint8_t* scratch_line_ptr; /** Generic scratch line pointer. */
ptrdiff_t area_size;       /** Generic area size scratch. */
uint8_t* scratch_scan_ptr; /** Generic scan pointer. */
FILE* file_ptr;

uint8_t top_margin;
uint8_t bottom_margin;
uint8_t macro_executing_flag;
uint8_t highlight_code[2];
uint8_t format_mode_flag;
uint8_t justifying_flag;
uint8_t insert_mode_flag;
uint8_t screen_maxrow;
uint8_t screen_maxcolumn;
uint8_t microspacing_flag;
uint8_t current_tab_key;
uint8_t folding_flag;
uint8_t ruler_right_stop;
uint8_t ruler_left_stop;

pointer_array_t pointer_array;
#define markers_array pointer_array.markers_array
#define area_start_ptr pointer_array.area_start_ptr
#define area_end_ptr pointer_array.area_end_ptr
#define area_insert_ptr pointer_array.area_insert_ptr
#define search_cursor_ptr pointer_array.search_cursor_ptr
#define search_limit_ptr pointer_array.search_limit_ptr

uint8_t input_buffer[MAX_COMMAND_LENGTH];

#define RAM_CURRENT_LINE_BUF 0x0545
#define RAM_JUST_BEFORE_RULER_BUF 0x05CC
#define just_before_current_ruler_buffer (&ram[RAM_JUST_BEFORE_RULER_BUF])
uint8_t output_buffer[MAX_LINE_LENGTH];

uint8_t header_text_maybe[0x42];

uint8_t filename_buffer[MAX_COMMAND_LENGTH];
uint8_t output_filename[MAX_COMMAND_LENGTH];
uint8_t printer_driver_name[0x14];

unsigned int register_value_array[26];

#define MAX_LINES 100
#define MAX_COLUMNS 132
uint8_t input_filename[MAX_COMMAND_LENGTH];

FILE* input_fp;
FILE* output_fp;

bool parse_decimal_number(int* value, uint8_t* pos);
bool scan_input_buffer(uint8_t* buffer, scan_state_t* state);
command_prefix_t check_for_command_prefix(uint8_t ch);
control_code_t check_for_control_code(uint8_t cur_ch);
static void emit_to_output_buffer_callback(uint8_t digit);
static void print_char_just_to_screen(uint8_t cur_ch);
static void render_number_to_callback(int value, void (*cb)(uint8_t));
static void render_number_to_output_buffer(uint16_t value, uint8_t start_x);
static void system_init(void);
uint8_t process_document_character(uint8_t cur_ch, uint8_t* idx, bool* is_tab);
uint8_t upper_case_unless_folding(uint8_t ch);
void beep(void);
void display_not_enough_memory(void);
void draw_prompt_characters(uint8_t first_char, uint8_t second_char);
void print_alignment_spaces(uint8_t cur_ch);
void print_char(uint8_t cur_ch);
void render_number_to_screen(int val);
void render_register(uint8_t cur_ch, uint8_t idx);
void return_to_cli_prompt(void);
void run_view(void);
void wipe_buffer(uint8_t fill_value, uint8_t* target_ptr);

/**
 * Run VIEW.
 * Establishes longjmp targets for CLI and editor, initializes system and
 * document, and enters the CLI loop.
 */
void run_view(void)
{
    switch (setjmp(env))
    {
        case JMP_CLI:
            cli_handler_impl();
            return;

        case JMP_EDITOR:
            editor_loop_impl();
            return;

        default:
            system_init();
            initialise_document();
            run_cli();
            return;
    }
}

/**
 * Initialise system memory and screen dimensions.
 * Sets up heap pointers and clamps screen size to compile-time limits.
 */
static void system_init(void)
{
    uint16_t size = screen_getsize();
    screen_maxcolumn = (uint8_t)(size & 0xff);
    screen_maxrow = (uint8_t)(size >> 8);

    if (screen_maxrow > MAX_LINES - 1)
        screen_maxrow = MAX_LINES - 1;

    if (screen_maxcolumn > MAX_COLUMNS - 1)
        screen_maxcolumn = MAX_COLUMNS - 1;
}

/**
 * Emits a beep.
 * Sends a bell character to the output.
 */
void beep(void)
{
    cli_putchar(7);
}

/**
 * Check whether a byte is a command or ruler prefix.
 * @param ch byte to test
 * @return COMMAND_PREFIX for 0x80, RULER_PREFIX for 0x81, NO_COMMAND_PREFIX
 * otherwise
 */
command_prefix_t check_for_command_prefix(uint8_t ch)
{
    if (ch == COMMAND_PREFIX)
        return COMMAND_PREFIX;

    if (ch == RULER_PREFIX)
        return RULER_PREFIX;
    return NO_COMMAND_PREFIX;
}

/**
 * Displays a memory exhaustion error and stops printing.
 */
void display_not_enough_memory(void)
{
    stop_printing();
    cli_putstring("Not enough memory\n");

    return_to_cli_prompt();
}

/**
 * Draws prompt characters.
 * Displays two inverted prompt characters at the top-left and restores the
 * cursor.
 * @param idx first prompt character
 * @param pos second prompt character
 */
void draw_prompt_characters(uint8_t first_char, uint8_t second_char)
{
    uint16_t saved_cursor_pos = screen_getcursor();
    cursor_off();
    home_cursor();
    screen_setstyle(STYLE_REVERSE);
    screen_putchar(first_char);
    screen_putchar(second_char);
    screen_setstyle(0);
    screen_putchar(0x20);
    screen_setcursor(saved_cursor_pos & 0xff, saved_cursor_pos >> 8);
}

/**
 * Parses a decimal number from the current format line.
 *
 * @param value output for the parsed integer
 * @param pos cursor into the line, advanced past digits
 * @return true if digits were parsed, false otherwise
 */
bool parse_decimal_number(int* value, uint8_t* pos)
{
    const char* start;

    if ((uint8_t*)heap_format_line_ptr == input_buffer ||
        (uint8_t*)current_format_line == input_buffer)
        start = (const char*)&input_buffer[*pos];
    else
        start = (const char*)&heap_format_line_ptr->text[*pos];
    char* end;
    *value = (int)strtoul(start, &end, 10);
    *pos += (uint8_t)(end - start);

    return (end != start);
}

/**
 * Process one document character, performing tab, ruler and highlight-code
 * expansion.
 * @param cur_ch character from the current edit line to process
 * @param idx on return, holds 1 on ordinary paths or the tab offset (index of
 * the first '*' ruler stop beyond column_position) on the tab path
 * @param is_tab on entry, the previous tab-expansion state for this walk; on
 * return, true if tab expansion was performed for cur_ch, false otherwise
 * @return processed character, normally 0x20 (space); tabs and characters below
 * 0x1a map to space, and characters in [0x1a, 0x20) map to highlight codes when
 * printer output is enabled
 */
uint8_t process_document_character(uint8_t cur_ch, uint8_t* idx, bool* is_tab)
{
    if (cur_ch != 9)
    {
        if ((cur_ch == 0x10) || cur_ch == 0x1a)
            goto ca5d5;

        if (cur_ch == 0x0b)
            goto ca5d9;

        if (cur_ch > 0x1a)
        {
            if (cur_ch < 0x20)
            {
                if ((print_flags & 0x80))
                {
                    cur_ch = (uint8_t)(cur_ch - 0x1b - 1);
                    *idx = cur_ch;
                    cur_ch = highlight_code[*idx];
                }
            }
        }
    ca5d1:
        *idx = 1;
        *is_tab = false;

        return cur_ch;

    ca5d5:
        do
        {
            cur_ch = 0x20;

            goto ca5d1;

        ca5d9:
            cur_ch = ruler_left_stop;
        } while (cur_ch == 0);
        cur_ch--;
    }
    else
    {
        uint8_t tab_pos = column_position;

        do
        {
            tab_pos++;

            if (tab_pos >= ruler_buffer_len)
                goto ca5f8;
            cur_ch = current_ruler_ptr[tab_pos];
        } while (cur_ch != 0x2a);
        cur_ch = tab_pos;
    }
    {
        bool no_borrow = (cur_ch >= column_position);

        cur_ch -= column_position;
        *idx = cur_ch;

        if (*idx == 0)
            goto ca5f8;

        if (no_borrow)
            goto ca5fa;
    }
ca5f8:
    *idx = 1;

ca5fa:
    cur_ch = 0x20;
    *is_tab = true;

    return cur_ch;
}

/**
 * Callback that writes a digit character into the output buffer.
 *
 * @param digit character to emit
 */
static void emit_to_output_buffer_callback(uint8_t digit)
{
    {
        output_buffer[screen_row] = digit;

        if (screen_row < MAX_LINE_LENGTH - 2)
            screen_row++;
    }
}

/**
 * Renders a number to the screen via the console output.
 *
 * @param val value to display
 */
void render_number_to_screen(int val)
{
    render_number_to_callback(val, cli_putchar);
}

/**
 * Expands a register reference into the output buffer.
 *
 * @param cur_ch register name character
 * @param idx position in the output buffer where expansion starts
 */
void render_register(uint8_t cur_ch, uint8_t idx)
{
    unsigned int* register_value = get_register_address(cur_ch);

    if (register_value != NULL)
        render_number_to_output_buffer(*register_value, idx);
}

/**
 * Renders a 16-bit number into the output buffer.
 *
 * @param value number to render
 * @param start_x starting offset in the output buffer
 */
static void render_number_to_output_buffer(uint16_t value, uint8_t start_x)
{
    screen_row = start_x;
    render_number_to_callback(value, emit_to_output_buffer_callback);
}

/**
 * Renders a number as decimal by invoking a callback for each digit.
 *
 * @param value number to render
 * @param cb callback invoked for each digit character
 */
static void render_number_to_callback(int value, void (*cb)(uint8_t))
{
    char buf[12];

    snprintf(buf, sizeof(buf), "%d", value);

    for (char* p = buf; *p; p++)
    {
        uint8_t cur_ch = (uint8_t)*p;

        if (cur_ch >= '0' && cur_ch <= '9')
        {
            cur_ch -= '0';
            cur_ch |= 0x30;
        }
        cb(cur_ch);
    }
}

/**
 * Return control to the CLI prompt via longjmp.
 */
void return_to_cli_prompt(void)
{
    longjmp(env, JMP_CLI);
}

/**
 * Scans the input buffer for the next non-delimiter character.
 *
 * @param buffer text to scan
 * @param state scan state holding the position and result character
 * @return true if no non-delimiter character was found, false otherwise
 */
bool scan_input_buffer(uint8_t* buffer, scan_state_t* state)
{
    state->pos = input_buffer_offset;
    state->ch = delimiter_char;

    if (state->ch == 0x0d)
        return true;

    while (1)
    {
        state->ch = buffer[state->pos];

        if (state->ch == 0x0d)
            return true;

        if (state->ch != delimiter_char)
            return false;
        state->pos++;

        if (state->pos == 0)
            break;
    }
    return true;
}

/**
 * Converts to uppercase unless folding is enabled.
 * Returns the character uppercased when folding is off.
 * @param acc input character
 * @return possibly uppercased character
 */
uint8_t upper_case_unless_folding(uint8_t ch)
{
    if (folding_flag & 0x80)
        return ch;
    return toupper(ch);
}

/**
 * Fills a buffer with a constant.
 * Writes the given value across the buffer length.
 * @param acc fill value
 * @param target_ptr destination
 */
void wipe_buffer(uint8_t fill_value, uint8_t* target_ptr)
{
    uint8_t idx = 0;
    uint8_t remaining = 0x89;

    do
    {
        target_ptr[idx] = fill_value;
        idx++;
        remaining--;
    } while (remaining != 0);
}

/**
 * Check whether a character is a highlight control code.
 * @param cur_ch character to test
 * @return HIGHLIGHT1_CODE for 0x1c, HIGHLIGHT2_CODE for 0x1d, NO_CONTROL_CODE
 * otherwise
 */
control_code_t check_for_control_code(uint8_t cur_ch)
{
    if (cur_ch == 0x1c)
        return HIGHLIGHT1_CODE;

    if (cur_ch == 0x1d)
        return HIGHLIGHT2_CODE;
    return NO_CONTROL_CODE;
}

/**
 * Print a character with alignment handling.
 * Spaces increment the pending alignment count; carriage returns reset it.
 * Other characters are flushed via alignment and then rendered.
 * @param cur_ch character to print
 */
void print_char(uint8_t cur_ch)
{
    if (cur_ch == 0x20)
    {
        print_xpos++;

        return;
    }
    if (cur_ch == 0x0d)
        print_xpos = 0;
    print_alignment_spaces(cur_ch);
    print_char_just_to_screen(cur_ch);
}

/**
 * Flush pending alignment spaces to the output.
 * Prints print_xpos spaces and resets the counter.
 * @param cur_ch unused, retained for call-site compatibility
 */
void print_alignment_spaces(uint8_t cur_ch)
{
    cur_ch = print_xpos;

    if (cur_ch == 0)
        return;

    do
    {
        print_char_just_to_screen(' ');
        print_xpos--;
    } while (print_xpos != 0);
}

/**
 * Render a character directly to screen or printer.
 * If printer output is enabled, delegates to the printer driver.
 * Otherwise handles highlight codes by rendering '-' or '*' in reverse
 * video and translates carriage return to newline.
 * @param cur_ch character to render
 */
static void print_char_just_to_screen(uint8_t cur_ch)
{
    if ((print_flags & 0x80))
    {
        printer_driver_ptr->print_char(cur_ch);

        return;
    }
    control_code_t cc = check_for_control_code(cur_ch);

    if (cc != NO_CONTROL_CODE)
    {
        {
            uint8_t saved_a = cur_ch;

            cur_ch = (cc == HIGHLIGHT1_CODE) ? 0x2d : 0x2a;
            screen_setstyle(STYLE_REVERSE);
            cli_putchar(cur_ch);
            cur_ch = saved_a;
        }
        screen_setstyle(0);

        return;
    }
    if (cur_ch == 0x0d)
    {
        cli_putchar('\n');

        return;
    }
    cli_putchar(cur_ch);
}
