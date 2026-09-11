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

jmp_buf env;
#define JMP_CLI 1
#define JMP_EDITOR 2

#define CTRL(c) ((uint8_t)((c) & 0x1f))

#define MAX_COMMAND_LENGTH 68
#define MAX_LINE_LENGTH 132

void check_continuous_editing(void);
void parse_filename_from_command(struct scan_state* scan);
command_prefix_t check_for_command_prefix(uint8_t ch);
bool reset_command_parse_state(struct scan_state* scan);
cli_cmd_status_t process_cli_command(struct scan_state* scan);
bool check_area_memory(uint8_t* doc_line_ptr);
void redraw_and_write_back(void);
void setup_area_pointers(uint8_t* doc_line_ptr);
void write_area_to_file(void);
void run_editor(void);
bool read_first_chunk_from_input_file(void);
bool read_next_chunk_from_input_file(uint8_t* target_ptr);
static uint8_t* compute_space_available(uint8_t* target_ptr);
static uint8_t* compute_space_common(uint8_t* target_ptr, ptrdiff_t scan_ptr);
control_code_t check_for_control_code(uint8_t cur_ch);
static void system_init(void);
static uint8_t* compute_required_space_for_insertion(uint8_t* target_ptr);

#include "io.h"

#include "document.h"
#include "cli.h"
#include "editor.h"

uint8_t ram[655360];

uint8_t* edit_buffer_base;        /** Base of edit buffer. */
uint8_t* current_format_line_ptr; /** Current formatting line pointer. */
uint8_t* current_ruler_ptr;       /** Pointer into current ruler buffer. */
uint8_t* current_line_ptr;        /** Walking cursor into document heap. */
uint8_t* page;                    /** Start of document heap. */
uint8_t* top;                     /** End of document heap (first free byte). */
uint8_t* himem;                   /** Top of available memory. */
uint8_t* top_of_screen_line_ptr;  /** Document line at top of screen. */
uint8_t* editor_ptr6;             /** Editor working pointer. */
uint8_t* printer_ptr6;            /** Printer working pointer. */
uint8_t* print_doc_ptr;           /** Document pointer used during printing. */
const struct printer_driver* printer_driver_ptr; /** Active printer driver. */
struct macro* first_macro_ptr; /** Head of macro linked list. */
struct macro* last_macro_ptr;  /** Tail of macro linked list. */
uint8_t* macro_cursor_ptr;     /** Cursor into current macro body. */
uint8_t* oshwm;                /** Ruler stack base. */
uint8_t* ruler_index[128];     /** Ruler index stack. */
uint8_t page_lines_remaining;
uint8_t formatted_line_written_flag;
uint8_t page_break_pending_flag;
uint8_t printing_from_file_flag;
uint8_t saved_ruler_index_scroll;
uint8_t saved_ruler_index_redraw;
uint8_t editor_current_key; /** Current key code in editor input loop. */
uint8_t page_break_flag;    /** Page-break flag in print path. */
uint8_t column_position;
uint8_t ruler_buffer_len;
uint8_t edit_line_len;
uint8_t file_edit_flags;
uint8_t ptr6_screen_row;
uint8_t xpos;
uint8_t input_file_empty_flag;
uint8_t justify_overflow_counter; /** Word-break overflow counter for
                                     justification. */
uint8_t print_last_microspacing; /** Last microspacing value sent to printer. */
uint8_t
    justify_line_length; /** Line length for justification overflow check. */
uint8_t print_extra_space_accum;     /** Extra-space accumulator for microspaced
                                        printing. */
uint8_t justify_extra_space_accum;   /** Extra-space accumulator for editor
                                        justification. */
uint8_t print_running_total_accum;   /** Running total for distributing extra
                                        spacing (printing). */
uint8_t justify_running_total_accum; /** Running total for distributing extra
                                        spacing (editor). */
uint8_t justify_gap_count;
uint8_t format_src_index;
uint8_t cli_output_pos;      /** Output buffer write index for CLI. */
uint8_t editor_output_pos;   /** Output buffer write index for editor. */
uint8_t cli_header_pos;      /** Header field position for CLI. */
uint8_t editor_header_pos;   /** Header field position for editor. */
uint8_t cli_header_limit;    /** Header loop limit for CLI. */
uint8_t editor_header_limit; /** Header loop limit for editor. */
uint8_t* doc_working_ptr;    /** Document working pointer. */
uint8_t rw_file_handle;
uint8_t print_flags;
uint8_t edit_buffer_dirty_flag;
uint8_t edit_buffer_unpacked_flag;
uint8_t scroll_repeat_count;
uint8_t ruler_index_ptr;
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
uint8_t scratch_offset;     /** Generic scratch offset. */
uint8_t scratch_index;      /** Generic scratch index. */
uint8_t screen_row;         /** Generic screen row scratch. */
uint8_t screen_column;      /** Generic screen column scratch. */
uint8_t temp_save;          /** Generic temporary save. */
uint8_t* scratch_line_ptr;  /** Generic scratch line pointer. */
uint8_t* scratch_block_ptr; /** Generic scratch block pointer. */
ptrdiff_t area_size;        /** Generic area size scratch. */
uint8_t* scratch_scan_ptr;  /** Generic scan pointer. */
FILE* file_ptr;

uint8_t top_margin;
uint8_t bottom_margin;
uint8_t header_margin;
uint8_t footer_margin;
uint8_t page_length;
uint8_t line_spacing;
uint8_t footers_enabled_flag;
uint8_t headers_enabled_flag;
uint8_t rhs_extra_margin;
uint8_t macro_executing_flag;
uint8_t two_sided_flag;
uint8_t left_margin;
uint8_t highlight_code[2];
#define highlight1_code highlight_code[0]
#define highlight2_code highlight_code[1]
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

struct pointer_array_t pointer_array;
#define markers_array pointer_array.markers_array
#define area_start_ptr pointer_array.area_start_ptr
#define area_end_ptr pointer_array.area_end_ptr
#define doc_ptr1 pointer_array.doc_ptr1
#define doc_ptr2 pointer_array.doc_ptr2
#define doc_ptr3 pointer_array.doc_ptr3

uint8_t printer_driver_block[0x100];
uint8_t input_buffer[MAX_COMMAND_LENGTH];

#define RAM_CURRENT_LINE_BUF 0x0545
#define RAM_JUST_BEFORE_RULER_BUF 0x05CC
#define just_before_current_ruler_buffer (&ram[RAM_JUST_BEFORE_RULER_BUF])
uint8_t output_buffer[MAX_LINE_LENGTH];

uint8_t header_text_maybe[0x42];
uint8_t footer_text_maybe[0x42];

uint8_t filename_buffer[MAX_COMMAND_LENGTH];
uint8_t output_filename[MAX_COMMAND_LENGTH];
uint8_t printer_driver_name[0x14];

unsigned int register_value_array[26];

#define MAX_LINES 100
#define MAX_COLUMNS 132
uint8_t line_lengths[MAX_LINES];
uint8_t input_filename[MAX_COMMAND_LENGTH];

FILE* input_fp;
FILE* output_fp;

/**
 * Program entry point.
 * Establishes longjmp targets for CLI and editor, initializes system and
 * document, and enters the CLI loop.
 * @param argc argument count (unused)
 * @param argv argument vector (unused)
 * @return exit status
 */
int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    int val = setjmp(env);
    if (val == JMP_CLI)
    {
        cli_handler_impl();
        return 0;
    }
    else if (val == JMP_EDITOR)
    {
        editor_loop_impl();
        return 0;
    }
    system_init();
    initialise_document();
    run_cli();
    return 0;
}

/**
 * Read the next byte from the input buffer.
 * Advances the position and reports whether the byte terminates the current
 * token.
 * @param pos pointer to current buffer index, incremented on entry
 * @param end output flag set when byte equals delimiter or carriage return
 * @return the byte at the new position
 */
static uint8_t read_next_command_byte(uint8_t* pos, bool* end)
{
    (*pos)++;
    uint8_t cur_ch = input_buffer[*pos];
    *end = (cur_ch == delimiter_char) || (cur_ch == 0x0d);
    return cur_ch;
}

/**
 * Clear the unpacked flag, redraw the editor, and write the edit buffer back.
 */
void redraw_and_write_back(void)
{
    edit_buffer_unpacked_flag = 0;
    redraw_editor();
    write_line_back_to_document_safely();
    return;
}

/**
 * Set up area pointers for the line at the working pointer.
 * Increments the line counter and clamps the visible pointer when the line
 * contains a carriage return.
 * @param doc_working_ptr pointer to the document line to inspect
 */
void setup_area_pointers(uint8_t* doc_working_ptr)
{
    uint8_t* scan_ptr = doc_working_ptr;
    uint8_t idx = 0;
    if (!(scan_ptr == doc_ptr2))
    {
        uint8_t cur_ch = *scan_ptr;
        if (cur_ch == 0x0d)
            idx++;
        scan_ptr++;
    }
    line_counter++;
    if (idx == 0)
        return;
    clamp_ptr6_to_document();
    return;
}

static const uint8_t escaped_char_table[] = {
    '?', 'T', 'C', 'S', 'L', 'Z', '-', '*', 0xff};
static const uint8_t l83e0_table[] = {
    1, 9, 0x0d, 2, 0x0b, 0x1a, 0x1c, 0x1d, 0xff};

/**
 * Expand an escaped string from the input buffer into the header text buffer.
 * Handles caret escapes and optional case folding.
 * @param idx starting index in the header text buffer
 * @param pos starting position in the input buffer
 * @return updated header text length
 */
static uint8_t expand_escaped_string(uint8_t idx, uint8_t pos)
{
    screen_column = idx;
    pos--;
    do
    {
        uint8_t cur_ch;
        _Bool end;
        cur_ch = read_next_command_byte(&pos, &end);
        if (end)
            break;
        if (!(cur_ch != 0x5e))
        {
            uint8_t next_ch = read_next_command_byte(&pos, &end);
            if (end)
                break;
            screen_row = toupper(next_ch);
            idx = 0xfe;
            for (;;)
            {
                idx += 2;
                uint8_t table_idx = idx >> 1;
                uint8_t tmp_ch3 = escaped_char_table[table_idx];
                if (tmp_ch3 & 0x80)
                    break;
                if (tmp_ch3 == screen_row)
                {
                    cur_ch = l83e0_table[table_idx];
                    if (cur_ch != 0)
                        goto c83ca;
                }
            }
            cur_ch = temp_save;
        }
    c83ca:
        idx = search_target_len;
        if (idx == 0)
            cur_ch = upper_case_unless_folding(cur_ch);
        idx = screen_column;
        header_text_maybe[idx] = cur_ch;
        screen_column++;
    } while (screen_column != 0);
    idx = screen_column;
    input_buffer_offset = pos;
    return idx;
}

/**
 * Process a CLI command from the input buffer.
 * Parses the search string and marks, sanitises the area, and copies area
 * pointers to the working pointers.
 * @param scan scan state containing current parse position
 * @return CLI_CMD_NO_TARGET if no command, CLI_CMD_NO_STRING if area empty,
 * CLI_CMD_OK otherwise
 */
cli_cmd_status_t process_cli_command(struct scan_state* scan)
{
    if (reset_command_parse_state(scan))
        return CLI_CMD_NO_TARGET;
    if (!scan_input_buffer(input_buffer, scan))
    {
        uint8_t idx =
            expand_escaped_string(search_target_len, input_buffer_offset + 1);
        cli_header_limit = idx;
    }
    parse_marks_from_command(scan);
    if (sanitise_area() == AREA_EMPTY)
        return CLI_CMD_NO_STRING;
    doc_ptr2 = area_start_ptr;
    doc_ptr3 = area_end_ptr;
    return CLI_CMD_OK;
}

/**
 * Reset command parse state and extract the search target length.
 * Scans the input buffer and expands any escaped search string.
 * @param scan scan state to initialise
 * @return true if no search string was found, false otherwise
 */
bool reset_command_parse_state(struct scan_state* scan)
{
    uint8_t idx = 0;
    search_target_len = idx;
    cli_header_limit = idx;
    if (scan_input_buffer(input_buffer, scan))
        return true;
    uint8_t idx2 = expand_escaped_string(0, scan->pos);
    search_target_len = idx2;
    return idx2 == 0;
}

/**
 * Read the input file into the document at the current area start.
 * Ensures free space, computes insertion limits, reads a block, and adjusts
 * document pointers.
 * @return pointer to the byte after the inserted data
 */
uint8_t* read_into_document(void)
{
    check_for_at_least_150_bytes_free();
    open_input_file();
    uint8_t* insert_ptr = area_start_ptr;
    move_cursor_to_address(area_start_ptr);
    uint8_t* space_limit = compute_required_space_for_insertion(insert_ptr);
    make_space_for_insertion(insert_ptr, space_limit - insert_ptr + 0x8b);
    uint8_t* cursor = insert_ptr;
    read_block_status_t status = read_block_from_file(&cursor, space_limit);
    if (status != READ_BLOCK_DONE)
        cli_putstring("Not all read in\n");
    ptrdiff_t size_delta = space_limit - cursor;
    scratch_scan_ptr = adjust_pointers(cursor, size_delta);
    return cursor;
}

/**
 * Check that the area at the working pointer fits in memory and rebuild the
 * line. Expands or shrinks the document gap as needed and reformats the line
 * content with case handling.
 * @param doc_working_ptr pointer to the document line to check
 * @return true if memory allocation failed, false otherwise
 */
bool check_area_memory(uint8_t* doc_working_ptr)
{
    uint8_t tmp_ch3;
    uint8_t tmp_ch4;
    uint8_t cur_ch = 0;
    uint8_t block_expansion_len = cur_ch;
    scratch_index = cur_ch;
    uint8_t pos = 0x14;
    uint8_t idx = search_target_len;
    if (!(idx != 0))
    {
    c8a5b:
        uint8_t next_ch = header_text_maybe[idx];
        if (!(next_ch != 1))
        {
            next_ch = scratch_index;
            if (next_ch >= cli_header_pos)
                goto c8a86;
            scratch_index++;
            if (scratch_index != 0)
                goto c8a84;
        }
        if (!(next_ch != 0x20 || pos >= cli_output_pos))
        {
            while (1)
            {
                uint8_t tmp_ch2 = output_buffer[pos];
                pos++;
                if (tmp_ch2 == 0)
                    goto c8a86;
                block_expansion_len++;
                if (pos >= cli_output_pos)
                    break;
            }
            block_expansion_len--;
        }
    c8a84:
        block_expansion_len++;
    c8a86:
        idx++;
    }
    if (idx < cli_header_limit)
        goto c8a5b;
    ptrdiff_t gap = doc_ptr2 - doc_working_ptr;
    uint8_t idx2 = block_expansion_len;
    if (gap < 256 && idx2 >= gap)
        idx2 = gap;
    uint8_t* insert_ptr = doc_working_ptr + idx2;
    ptrdiff_t delta = (ptrdiff_t)block_expansion_len - gap;
    if (delta < 0)
    {
        ptrdiff_t size_delta = -delta;
        scratch_scan_ptr = adjust_pointers(insert_ptr, size_delta);
    }
    else if (delta > 0)
    {
        if (!make_space_for_insertion(insert_ptr, delta))
            return true;
    }
    uint8_t pos2 = 0;
    scratch_index = pos2;
    if (!(print_xpos & 0x80))
    {
        uint8_t idx3 = scratch_offset;
        do
        {
            tmp_ch3 = doc_working_ptr[pos2];
            pos2++;
            if (isalpha(tmp_ch3))
                goto c8af3;
            print_xpos = (uint8_t)(print_xpos >> 1) | 0x80;
            idx3--;
        } while (idx3 != 0);
        goto c8b11;
    c8af3:
    {
        print_xpos = 0;
        tmp_ch4 = tmp_ch3;
    }
        tmp_ch4 &= 0x20;
        if (tmp_ch4 != 0)
            goto c8b11;
        scratch_index++;
        idx3--;
        if (!(idx3 == 0))
        {
            uint8_t tmp_ch5 = doc_working_ptr[pos2];
            if (!isalpha(tmp_ch5))
                goto c8b11;
            tmp_ch5 &= 0x20;
            if (tmp_ch5 != 0)
                goto c8b11;
        }
        scratch_index--;
        scratch_index--;
    }
c8b11:
    uint8_t output_buf_pos = 0;
    uint8_t doc_write_pos = 0;
    scratch_offset = 0x14;
    uint8_t idx4 = search_target_len;
    if (idx4 != 0)
        goto c8b6b;
    do
    {
        uint8_t tmp_ch6 = header_text_maybe[idx4];
        temp_save = idx4;
        if (!(tmp_ch6 != 0x20))
        {
            uint8_t pos3 = scratch_offset;
            if (pos3 >= cli_output_pos)
                goto c8b47;
            scratch_offset++;
            tmp_ch6 = output_buffer[pos3];
            if (tmp_ch6 == 0)
                goto c8b6a;
            idx4--;
        }
        else
        {
            if (tmp_ch6 != 1)
                goto c8b47;
            if (output_buf_pos >= cli_header_pos)
                goto c8b6a;
            tmp_ch6 = output_buffer[output_buf_pos];
            output_buf_pos++;
        }
    c8b47:
        if (tmp_ch6 == 2)
            tmp_ch6 = 0x20;
        if (!(folding_flag & 0x80 || print_xpos != 0))
        {
            if (isalpha(tmp_ch6))
            {
                tmp_ch6 |= 0x20;
                if (scratch_index != 0)
                {
                    scratch_index--;
                    tmp_ch6 &= 0xdf;
                }
            }
        }
        doc_working_ptr[doc_write_pos] = tmp_ch6;
        doc_write_pos++;
    c8b6a:
        idx4++;
    c8b6b:;
    } while (idx4 < cli_header_limit);
    split_line_at_wrap(doc_working_ptr);
    return false;
}

/**
 * Read the next chunk of the input file into the document.
 * Computes available space, reads a block, and updates the document top.
 * @param target_ptr destination address in the document heap
 * @return true if the block read was empty, false otherwise
 */
bool read_next_chunk_from_input_file(uint8_t* target_ptr)
{
    uint8_t* space_limit = compute_space_available(target_ptr);
    file_ptr = input_fp;
    uint8_t* cursor = target_ptr;
    read_block_status_t status = read_block_from_file(&cursor, space_limit);
    if (status == READ_BLOCK_DONE)
        input_file_empty_flag++;
    *cursor = 0;
    top = cursor;
    return status == READ_BLOCK_EMPTY;
}

/**
 * Read the first chunk of the input file starting at the document page.
 * @return true if the block read was empty, false otherwise
 */
bool read_first_chunk_from_input_file(void)
{
    return read_next_chunk_from_input_file(page);
}

/**
 * Write the sanitised document area to the output file.
 * Iterates from area start to area end and writes each byte.
 */
void write_area_to_file(void)
{
    if (sanitise_area() == AREA_EMPTY)
        return;
    uint8_t* scan_ptr = area_start_ptr;
    do
    {
        uint8_t cur_ch = *scan_ptr;
        fputc(cur_ch, file_ptr);
        scan_ptr++;
    } while (scan_ptr != area_end_ptr);
}

/**
 * Compute the free-space limit for an insertion at the target pointer.
 * Applies the common clamping logic for available versus required space.
 * @param target_ptr insertion point in the document heap
 * @param scan_ptr size hint (quarter-scaled free count or zero)
 * @return pointer to the computed limit
 */
static uint8_t* compute_space_common(uint8_t* target_ptr, ptrdiff_t scan_ptr)
{
    uint8_t* line_ptr = target_ptr;
    ptrdiff_t size_delta = compute_bytes_free();
    scan_ptr >>= 2;
    if (scan_ptr >= 0x0400)
    {
        scan_ptr = 0x0404;
        size_delta -= scan_ptr;
    }
    else
    {
        size_delta -= scan_ptr + 1;
    }
    return line_ptr + size_delta - 0x8b;
}

/**
 * Compute available space for insertion at the target pointer.
 * @param target_ptr insertion point
 * @return limit pointer for the insertion
 */
static uint8_t* compute_space_available(uint8_t* target_ptr)
{
    return compute_space_common(target_ptr, compute_bytes_free());
}

/**
 * Compute required space for a fresh insertion at the target pointer.
 * @param target_ptr insertion point
 * @return limit pointer for the insertion
 */
static uint8_t* compute_required_space_for_insertion(uint8_t* target_ptr)
{
    return compute_space_common(target_ptr, 0);
}

/**
 * Parse a mandatory filename from the command line.
 * Reports an error if no filename is present.
 * @param scan scan state pointing into the command buffer
 */
void parse_filename_from_command(struct scan_state* scan)
{
    if (!parse_optional_filename_from_command(scan))
    {
        bad_filename_error();
        return;
    }
}

/**
 * Verify that continuous editing is active.
 * Displays the document file state when continuous editing is not enabled.
 */
void check_continuous_editing(void)
{
    if (!((file_edit_flags & 0x40)))
    {
        if (file_edit_flags & 1)
            return;
    }
    display_document_file_state();
}

/**
 * Initialise system memory and screen dimensions.
 * Sets up heap pointers and clamps screen size to compile-time limits.
 */
static void system_init(void)
{
    himem = ram + sizeof(ram) - 1;
    oshwm = &ram[0x0800];
    uint16_t size = screen_getsize();
    screen_maxcolumn = (uint8_t)(size & 0xff);
    screen_maxrow = (uint8_t)(size >> 8);
    if (screen_maxrow > MAX_LINES - 1)
        screen_maxrow = MAX_LINES - 1;
    if (screen_maxcolumn > MAX_COLUMNS - 1)
        screen_maxcolumn = MAX_COLUMNS - 1;
}
