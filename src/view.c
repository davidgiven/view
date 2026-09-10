/* # Note to AI agents
 *
 * This is an in-progress translation from 6502 machine code to C.
 * The original 6502 assembly code is included as comments for reference.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <setjmp.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>

#include "io.h"
#include "globals.h"

// Functions defined in printing.c

// #include "cpm65.inc"
// #include "driver.inc"
// #include "zif.inc"

// ; Longjmp buffer for stack unwinding (txs equivalent)
jmp_buf env;
#define JMP_CLI 1
#define JMP_EDITOR 2

#define CTRL(c) ((uint8_t)((c) & 0x1f))

// ; Constants
// buffer_keyboard                             = 0
#define MAX_COMMAND_LENGTH 68
#define MAX_LINE_LENGTH 132

// ; SCREEN driver key codes

// ; File structure (removed - migrated to stdio)

// Forward declarations
void check_continuous_editing(void);
void parse_filename_from_command(struct scan_state* scan);

command_prefix_t check_for_command_prefix(uint8_t ch);

bool reset_command_parse_state(struct scan_state* scan);
cli_cmd_status_t process_cli_command(struct scan_state* scan);
bool check_area_memory(uint8_t* ptr2);
void redraw_and_write_back(void);
void setup_area_pointers(uint8_t* ptr2);
void write_area_to_file(void);
void run_editor(void);
bool read_first_chunk_from_input_file(void);
// Input:  a = document character, y = line offset (for tab stop lookup)
// Output: a = character to render, x = screen width consumed, y preserved,
// flags.C=0
bool read_next_chunk_from_input_file(uint8_t* ptr);
static uint8_t* compute_space_available(uint8_t* ptr);
static uint8_t* compute_space_common(uint8_t* ptr, ptrdiff_t scan_ptr);
control_code_t check_for_control_code(uint8_t a);

static void system_init(void);

// Forward declarations for recently translated functions
static uint8_t* compute_required_space_for_insertion(uint8_t* ptr);

#include "io.h"

#include "document.h"
#include "cli.h"
#include "editor.h"

// X ram:                              .fill 65536
uint8_t ram[655360];

// ; Memory locations

// X .section .zp, "zax", @nobits

// X ptr1: .fill 2
uint8_t* ptr1;
// X current_edit_line_ptr: .fill 2
// X current_format_line_ptr: .fill 2
uint8_t* current_format_line_ptr;
// X current_ruler_ptr: .fill 2
uint8_t* current_ruler_ptr;
// X current_line_ptr: .fill 2
uint8_t* current_line_ptr;
// X page: .fill 2
uint8_t* page;
// X top: .fill 2
uint8_t* top;
// X himem: .fill 2
uint8_t* himem;
// X l0011: .fill 1
// X l0012: .fill 1
uint8_t* top_of_screen_line_ptr;
// X ptr6: .fill 2
uint8_t* editor_ptr6;
uint8_t* printer_ptr6;
// X ptr5: .fill 2
uint8_t* ptr5;
// X printer_driver_ptr: .fill 2 (replaced by struct pointer)
const struct printer_driver* printer_driver_ptr;
// X first_macro_ptr: .fill 2
struct macro* first_macro_ptr;
// X last_macro_ptr: .fill 2
struct macro* last_macro_ptr;
// X ptr3: .fill 2
uint8_t* ptr3;
// X oshwm: .fill 2
uint8_t* oshwm;
uint8_t* ruler_index[128]; // ruler stack (was oshwm[] trick, now separate)
// X l0021: .fill 1
uint8_t page_lines_remaining;
// X l0030: .fill 1
uint8_t formatted_line_written_flag;
// X l0031: .fill 1
uint8_t page_break_pending_flag;
// X printing_from_file_flag: .fill 1
uint8_t printing_from_file_flag;
// X l0033: .fill 1
uint8_t saved_ruler_index_scroll;
// X l0034: .fill 1
uint8_t saved_ruler_index_redraw;
// X l0038: .fill 1  (6502 zero-page byte; split into two C variables by role)
uint8_t editor_current_key; // current key code in editor input loop (part of
                            // 6502 l0038)
uint8_t page_break_flag; // page-break flag in print path (part of 6502 l0038)
// X l0039: .fill 1
uint8_t column_position;
// X l003a: .fill 1
uint8_t ruler_buffer_len;
// X l003b: .fill 1
uint8_t edit_line_len;
// X file_edit_flags: .fill 1
uint8_t file_edit_flags;
// X l003d: .fill 1
uint8_t ptr6_screen_row;
// X xpos: .fill 1
uint8_t xpos;
// X input_file_empty_flag: .fill 1
uint8_t input_file_empty_flag;
// X l0042: .fill 1
uint8_t justify_overflow_counter; // word-break overflow counter in editor
                                  // justification (part of 6502 l0042, printing
                                  // uses local l0042_1 in microspace)
// X l0043: .fill 1  (6502 zero-page byte; split into two C variables by role)
uint8_t
    print_last_microspacing; // last microspacing value sent to printer (part of
                             // 6502 l0043 in microspace_word_processor)
uint8_t justify_line_length; // line length for justification overflow check
                             // (part of 6502 l0043 in editor justify)
// X l0044: .fill 1  (6502 zero-page byte; split into two C variables by role)
uint8_t print_extra_space_accum;   // inter-word extra-space accumulator in
                                   // microspaced justification printing path
                                   // (part of 6502 l0044)
uint8_t justify_extra_space_accum; // inter-word extra-space accumulator in
                                   // editor justification (part of 6502 l0044)
// X l0045: .fill 1  (6502 zero-page byte; split into two C variables by role)
uint8_t print_running_total_accum; // running-total accumulator for distributing
                                   // extra spacing in microspaced justification
                                   // printing path (part of 6502 l0045)
uint8_t justify_running_total_accum; // running-total accumulator in editor
                                     // justification (part of 6502 l0045)
// X l0046: .fill 1
uint8_t justify_gap_count;
// X l0047: .fill 1
uint8_t format_src_index;
// X l0048: .fill 1  (6502 zero-page byte; split into two C variables by role)
uint8_t cli_output_pos; // output-buffer write index for CLI check_area (part of
                        // 6502 l0048)
uint8_t editor_output_pos; // output-buffer write index / block-advance flag for
                           // editor justification (part of 6502 l0048)
// X l0049: .fill 1  (6502 zero-page byte; split into two C variables by role)
uint8_t cli_header_pos; // header/footer cell/field position for CLI check_area
                        // (part of 6502 l0049)
uint8_t editor_header_pos; // header/footer cell/field position for editor (part
                           // of 6502 l0049)
// X l004a: .fill 1  (6502 zero-page byte; split into two C variables by role)
uint8_t cli_header_limit; // upper-bound loop limit for CLI header/footer (part
                          // of 6502 l004a)
uint8_t editor_header_limit; // upper-bound loop limit for editor (part of 6502
                             // l004a)
// X ptr2: .fill 2
uint8_t* ptr2;
// X rw_file_handle: .fill 1
uint8_t rw_file_handle;
// X print_flags: .fill 1
uint8_t print_flags;
// X l006d: .fill 1
uint8_t edit_buffer_dirty_flag;
// X l006e: .fill 1
uint8_t edit_buffer_unpacked_flag;
// X l006f: .fill 1
uint8_t scroll_repeat_count;
// X ruler_stack_ptr: .fill 1
uint8_t ruler_index_ptr;
// X hscroll_pos: .fill 1
uint8_t hscroll_pos;
// X l0072: .fill 1
uint8_t visual_column;
// X l0073: .fill 1
uint8_t display_start_row;
// X l0074: .fill 1
uint8_t line_counter;
// X flags_need_redrawing_flag: .fill 1
uint8_t flags_need_redrawing_flag;
// X status_line_needs_redrawing_flag: .fill 1
uint8_t status_line_needs_redrawing_flag;
// X l0076: .fill 1
// (removed: unused/reserved byte, previously l0076 - zeroed in init, never
// read) X ypos: .fill 1
uint8_t ypos;
// X print_xpos: .fill 1
uint8_t print_xpos;
// X l0079: .fill 1
uint8_t line_change_pending_flag;
// X l007a: .fill 1
uint8_t search_target_len;
// X cursor_moved_flag: .fill 1
uint8_t cursor_moved_flag;
// X l007e: .fill 1  (6502 zero-page byte; split into two C variables by role)
uint8_t delimiter_char; // delimiter/separator character used during CLI command
                        // parsing and printing scans (part of 6502 l007e)
uint8_t line_format_status; // format status byte (part of 6502 l007e): counts
                            // marker bytes and is tagged when a line is flushed
// X input_buffer_offset: .fill 2
uint8_t input_buffer_offset;
// X l0080: .fill 1
uint8_t
    scratch_offset; // view.py: l0080 (generic multipurpose) - C: scratch_offset
// X l0081: .fill 1
uint8_t
    scratch_index; // view.py: l0081 (generic multipurpose) - C: scratch_index
// X l0082: .fill 1
uint8_t screen_row; // view.py: l0082 (generic multipurpose) - C: screen_row
// X l0083: .fill 1
uint8_t
    screen_column; // view.py: l0083 (generic multipurpose) - C: screen_column
// X l0084: .fill 1
uint8_t temp_save; // view.py: l0084 (generic multipurpose) - C: temp_save
uint8_t* scratch_line_ptr;  // view.py: tmp0/tmp1 (generic multipurpose) - C:
                            // scratch_line_ptr
uint8_t* scratch_block_ptr; // view.py: tmp2/tmp3 (generic multipurpose) - C:
                            // scratch_block_ptr
ptrdiff_t area_size; // view.py: tmp6/tmp7 (generic multipurpose) - C: area_size
                     // (was tmp67)
uint8_t* scratch_scan_ptr; // view.py: tmp8/tmp9 (generic multipurpose) - C:
                           // scratch_scan_ptr
// X file_ptr: .fill 2
FILE* file_ptr;

// X .bss

// X top_margin: .fill 1
uint8_t top_margin;
// X bottom_margin: .fill 1
uint8_t bottom_margin;
// X header_margin: .fill 1
uint8_t header_margin;
// X footer_margin: .fill 1
uint8_t footer_margin;
// X page_length: .fill 1
uint8_t page_length;
// X line_spacing: .fill 1
uint8_t line_spacing;
// X footers_enabled_flag: .fill 1
uint8_t footers_enabled_flag;
// X headers_enabled_flag: .fill 1
uint8_t headers_enabled_flag;
// X rhs_extra_margin: .fill 1
uint8_t rhs_extra_margin;
// X macro_executing_flag: .fill 1
uint8_t macro_executing_flag;
// X two_sided_flag: .fill 1
uint8_t two_sided_flag;
// X left_margin: .fill 1
uint8_t left_margin;
// X highlight1_code: .fill 2
uint8_t highlight_code[2];
#define highlight1_code highlight_code[0]
#define highlight2_code highlight_code[1]
// X format_mode_flag: .fill 1
uint8_t format_mode_flag;
// X justifying_flag: .fill 1
uint8_t justifying_flag;
// X insert_mode_flag: .fill 1
uint8_t insert_mode_flag;
// X screen_height: .fill 1
uint8_t screen_maxrow;
// X screen_width: .fill 1
uint8_t screen_maxcolumn;
// X microspacing_flag: .fill 1
uint8_t microspacing_flag;
// X current_tab_key: .fill 1
uint8_t current_tab_key;
// X folding_flag: .fill 1
uint8_t folding_flag;
// X ruler_right_stop: .fill 1
uint8_t ruler_right_stop;
// X ruler_left_stop: .fill 1
uint8_t ruler_left_stop;

// X __begin_pointer_array:
// X markers_array: .fill 12
// X area_start_ptr: .fill 2
// X area_end_ptr: .fill 2
// X doc_ptr1: .fill 2
// X doc_ptr2: .fill 2
// X doc_ptr3: .fill 2
// X __end_pointer_array:
struct pointer_array_t pointer_array;
#define markers_array pointer_array.markers_array
#define area_start_ptr pointer_array.area_start_ptr
#define area_end_ptr pointer_array.area_end_ptr
#define doc_ptr1 pointer_array.doc_ptr1
#define doc_ptr2 pointer_array.doc_ptr2
#define doc_ptr3 pointer_array.doc_ptr3

// X printer_driver_block:           .fill 0x100
uint8_t printer_driver_block[0x100];
// X input_buffer:                   .fill 0x45
uint8_t input_buffer[MAX_COMMAND_LENGTH];

// X current_line_buffer:            .fill 135
#define RAM_CURRENT_LINE_BUF 0x0545
// X just_before_current_ruler_buffer: .fill 3 ; ??? something to do with
// rulers?
#define RAM_JUST_BEFORE_RULER_BUF 0x05CC
#define just_before_current_ruler_buffer (&ram[RAM_JUST_BEFORE_RULER_BUF])
// X current_ruler_buffer:           .fill 133
//  RAM_CURRENT_RULER_BUF and current_ruler_buffer defined in globals.h
// X output_buffer:                  .fill 132
uint8_t output_buffer[MAX_LINE_LENGTH];

// X header_text_maybe:              .fill 0x42
uint8_t header_text_maybe[0x42];
// X footer_text_maybe:              .fill 0x42
uint8_t footer_text_maybe[0x42];

// X filename_buffer:                .fill 0x14
uint8_t filename_buffer[MAX_COMMAND_LENGTH];
// X output_filename:                .fill 0x14
uint8_t output_filename[MAX_COMMAND_LENGTH];
// X printer_driver_name:            .fill 0x14
uint8_t printer_driver_name[0x14];

// X register_value_array:           .fill 26*2
// (originally stored in emulated 6502 RAM at 0x0798; now a real C array of
//  unsigned int values for registers A-Z)
unsigned int register_value_array[26];

#define MAX_LINES 100
#define MAX_COLUMNS 132
// X line_lengths:                   .fill 32
uint8_t line_lengths[MAX_LINES];
// X input_filename:                 .fill 20
uint8_t input_filename[MAX_COMMAND_LENGTH];

// X input_file:                     .fill FS__SIZE
FILE* input_fp;
// X output_file:                    .fill FS__SIZE
FILE* output_fp;

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    // main
    // Pseudocode: Program entry point with longjmp buffer for stack reset (txs
    // equivalent)
    // .text
    // .global main
    // main:
    //     ldx #0xff
    //     txs
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
    // Initial entry (val == 0)
    //     jsr system_init
    system_init();
    //     jsr initialise_document
    initialise_document();
    run_cli();
    return 0;
}

// run_editor moved to editor.c
// sub_c8310.  *end is set if the byte is the delimiter (delimiter_char) or CR,
// terminating the current token (the 6502's Z flag).
static uint8_t read_next_command_byte(uint8_t* y, bool* end)
{
    // sub_c8310
    // sub_c8310:
    //     iny
    (*y)++;
    //     lda input_buffer,y
    uint8_t a = input_buffer[*y];
    //     sta l0084
    // (the l0084 store is omitted — callers use the returned byte instead)
    //     cmp l007e
    //     beq return_2
    //     cmp #0x0d
    *end = (a == delimiter_char) || (a == 0x0d);
    // return_2:
    //     rts
    return a;
}

void redraw_and_write_back(void)
{
    // sub_c8361
    // sub_c8361:
    //     lda #0
    //     sta l006e
    edit_buffer_unpacked_flag = 0;
    //     jsr redraw_editor
    redraw_editor();
    //     jmp write_line_back_to_document_safely
    write_line_back_to_document_safely();
    return;
    // c836b:
    //     jsr sub_ca94a
    //     jmp esc_key
}

void setup_area_pointers(uint8_t* ptr2)
{
    // sub_c8371
    //  Ptrs:   ptr2
    // sub_c8371:
    //     lda ptr2
    //     sta ((uint8_t*)&tmp89)[0]
    //     lda ptr2+1
    //     sta ((uint8_t*)&tmp89)[1]
    uint8_t* scan_ptr = ptr2;
    //     ldy #0
    //     ldx #0
    uint8_t x = 0;
    if (!(scan_ptr == doc_ptr2))
    {
        // c8389:
        //     lda (((uint8_t*)&tmp89)[0]),y
        uint8_t a = *scan_ptr;
        //     cmp #0x0d
        if (a == 0x0d)
            x++;
        // c8390:
        //     inc ((uint8_t*)&tmp89)[0]
        scan_ptr++;
    }
    //     bne c837d
    //     inc ((uint8_t*)&tmp89)[1]
    //     bne c837d
    // c8398:
    //     inc l0074
    line_counter++;
    //     txa
    //     beq return_3
    if (x == 0)
        return;
    //     jmp ca741
    clamp_ptr6_to_document();
    return;
}

static const uint8_t escaped_char_table[] = {
    '?', 'T', 'C', 'S', 'L', 'Z', '-', '*', 0xff};
static const uint8_t l83e0_table[] = {
    1, 9, 0x0d, 2, 0x0b, 0x1a, 0x1c, 0x1d, 0xff};

static uint8_t expand_escaped_string(uint8_t x, uint8_t y)
{
    // expand_escaped_string
    // expand_escaped_string:
    //     stx l0083
    screen_column = x;
    //     dey
    y--;
    do
    {
        // c83a3:
        //     jsr sub_c8310
        uint8_t a;
        _Bool end;
        a = read_next_command_byte(&y, &end);
        //     beq c83da
        if (end)
            break;
        if (!(a != 0x5e))
        {
            //     bne c83ca
            //     jsr sub_c8310
            uint8_t a_1 = read_next_command_byte(&y, &end);
            //     beq c83da
            if (end)
                break;
            //     jsr to_uppercase
            //     sta l0082
            screen_row = toupper(a_1);
            //     ldx #0xfe
            x = 0xfe;
            // loop_c83b8:
            for (;;)
            {
                //     inx
                //     inx
                x += 2;
                uint8_t idx = x >> 1;
                //     lda escaped_char_table,x
                uint8_t a_3 = escaped_char_table[idx];
                //     bmi c83c8
                if (a_3 & 0x80)
                    break;
                //     cmp l0082
                if (a_3 == screen_row)
                {
                    //     lda l83e0,x
                    a = l83e0_table[idx];
                    //     bne c83ca
                    if (a != 0)
                        goto c83ca;
                }
                //     bne loop_c83b8
            }
            // c83c8:
            //     lda l0084
            a = temp_save;
        }
    c83ca:
        // c83ca:
        //     ldx l007a
        x = search_target_len;
        //     bne c83d1
        if (x == 0)
            a = upper_case_unless_folding(a);
        // c83d1:
        //     ldx l0083
        x = screen_column;
        //     sta header_text_maybe,x
        header_text_maybe[x] = a;
        //     inc l0083
        screen_column++;
    } while (screen_column != 0);
    //     bne c83a3
    // c83da:
    //     ldx l0083
    x = screen_column;
    //     sty input_buffer_offset
    input_buffer_offset = y;
    return x;
    // return_3:
    //     rts
}

cli_cmd_status_t process_cli_command(struct scan_state* scan)
{
    // sub_c83f0
    // sub_c83f0:
    //     jsr sub_c8412
    //     beq c8410
    if (reset_command_parse_state(scan))
        return CLI_CMD_NO_TARGET;
    // c8410: no command
    //     jsr sub_c8e33
    //     beq c8402
    if (!scan_input_buffer(input_buffer, scan))
    {
        // (base index comes from l007a, set by reset_command_parse_state)
        uint8_t x =
            expand_escaped_string(search_target_len, input_buffer_offset + 1);
        cli_header_limit = x;
    }
    // c8402:
    //     jsr parse_marks_from_command
    parse_marks_from_command(scan);
    //     jsr sanitise_area
    //     beq return_4
    if (sanitise_area() == AREA_EMPTY)
    {
        return CLI_CMD_NO_STRING;
        // sec + Z set: no string
    }
    //     jsr sub_c8c7c
    // (inlined: doc_ptr2 = area_start_ptr; doc_ptr3 = area_end_ptr)
    doc_ptr2 = area_start_ptr;
    doc_ptr3 = area_end_ptr;
    //     lda #1
    // c8410:
    //     clc
    // return_4:
    //     rts
    return CLI_CMD_OK;
}

bool reset_command_parse_state(struct scan_state* scan)
{
    // sub_c8412
    // sub_c8412:
    //     ldx #0
    uint8_t x = 0;
    //     stx l007a
    search_target_len = x;
    //     stx l004a
    cli_header_limit = x;
    //     jsr sub_c8e33
    //     beq return_5
    if (scan_input_buffer(input_buffer, scan))
        return true;
    // Z set (no command)
    //     ldx #0
    //     jsr expand_escaped_string
    uint8_t x_1 = expand_escaped_string(0, scan->pos);
    //     stx l007a
    search_target_len = x_1;
    //     cpx #0
    return x_1 == 0;
    // Z = (x == 0)
    // return_5:
    //     rts
}

uint8_t* read_into_document(void)
{
    // read_into_document
    //  Ptrs:   ptr5
    // 1: - shared entry point used by both load_cmd and read_cmd
    //     jsr check_for_at_least_150_bytes_free
    check_for_at_least_150_bytes_free();
    //     ldx #<input_buffer
    //     ldy #>input_buffer
    //     jsr select_file
    // (C translation: select_file does file-pointer setup; commented out
    // because
    //  select_file() in this translation uses inline x,y rather than the 6502's
    //  buffer-address convention, and the actual file selection is already done
    //  by parse_filename_from_command / open_input_file.)
    //     jsr open_input_file
    open_input_file();
    //     lda area_start_ptr
    //     sta ((uint8_t*)&tmp45)[0]
    uint8_t* insert_ptr = area_start_ptr;
    //     jsr move_cursor_to_address
    move_cursor_to_address(area_start_ptr);
    //     lda ((uint8_t*)&tmp45)[0]
    //     ldy ((uint8_t*)&tmp45)[1]
    //     jsr compute_required_space_for_insertion
    uint8_t* space_limit = compute_required_space_for_insertion(insert_ptr);
    //     jsr make_space_for_insertion
    // (the 6502 leaves the clamped free-space size in tmp67; since
    //  space_limit = tmp45 + tmp67 - 0x8b, tmp67 = space_limit - tmp45 + 0x8b)
    make_space_for_insertion(insert_ptr, space_limit - insert_ptr + 0x8b);
    //     jsr read_block_from_file
    // (destination = the insertion point tmp45 explicitly)
    uint8_t* cursor = insert_ptr;
    read_block_status_t status = read_block_from_file(&cursor, space_limit);
    //     beq c8584
    //     bcs c8598
    if (status != READ_BLOCK_DONE)
    {
        //     jsr print_inline_string
        //     .ascii "Not all read in\r"
        //     .byte 0
        cli_putstring("Not all read in\n");
    }
    //     lda ((uint8_t*)&tmp01)[0]
    //     lda ptr5
    //     sec
    //     sbc ((uint8_t*)&tmp01)[0]
    //     sta ((uint8_t*)&tmp67)[0]
    //     lda ptr5+1
    //     sbc ((uint8_t*)&tmp01)[1]
    //     sta ((uint8_t*)&tmp67)[1]
    // (16-bit subtraction: tmp67 = space_limit - tmp01)
    ptrdiff_t size_delta = space_limit - cursor;
    //     jsr adjust_pointers
    scratch_scan_ptr = adjust_pointers(cursor, size_delta);
    // (the 6502 left the post-read cursor in tmp01; load_cmd uses it for top)
    return cursor;
}

/**
 * Check that the area at ptr2 will fit in memory, expanding it if needed
 * (6502 sub_c8a4f).  Also rebuilds ptr2's line into the edit buffer.
 *
 * @return true on memory full (the 6502 left C set after
 *         make_space_for_insertion failed), false otherwise (the 6502's
 *         explicit clc before rts).
 */
bool check_area_memory(uint8_t* ptr2)
{
    uint8_t a_3;
    uint8_t a_4;
    // sub_c8a4f
    //  Ptrs:   ptr2
    // sub_c8a4f:
    //     lda #0
    uint8_t a = 0;
    //     sta l0082
    uint8_t block_expansion_len = a;
    //     sta l0081
    scratch_index = a;
    //     ldy #0x14
    uint8_t y = 0x14;
    //     ldx l007a
    uint8_t x = search_target_len;
    if (!(x != 0))
    {
    c8a5b:
        // c8a5b:
        //     lda header_text_maybe,x
        uint8_t a_1 = header_text_maybe[x];
        if (!(a_1 != 1))
        {
            //     bne c8a6c
            //     lda l0081
            a_1 = scratch_index;
            //     cmp l0049
            if (a_1 >= cli_header_pos)
                goto c8a86;
            //     bcs c8a86
            //     inc l0081
            scratch_index++;
            if (scratch_index != 0)
                goto c8a84;
        }
        //     bne c8a84
        // c8a6c:
        //     cmp #0x20 ; ' '
        //     bne c8a84
        //     cpy l0048
        if (!(a_1 != 0x20 || y >= cli_output_pos))
        {
            //     bcs c8a84
            // loop_c8a74:
            while (1)
            {
                //     lda output_buffer,y
                uint8_t a_2 = output_buffer[y];
                //     php; iny; plp
                // (the php/plp preserves Z across iny for the beq below;
                // testing
                //  a == 0 directly is equivalent)
                y++;
                //     beq c8a86
                if (a_2 == 0)
                    goto c8a86;
                //     inc l0082
                block_expansion_len++;
                //     cpy l0048
                if (y >= cli_output_pos)
                    break;
                //     bcc loop_c8a74
            }
            //     dec l0082
            block_expansion_len--;
        }
    c8a84:
        // c8a84:
        //     inc l0082
        block_expansion_len++;
    c8a86:
        // c8a86:
        //     inx
        x++;
    }
    // c8a87:
    //     cpx l004a
    if (x < cli_header_limit)
        goto c8a5b;
    //     lda doc_ptr2+0
    //     sec
    //     sbc ptr2
    //     sta input_buffer_offset+1
    //     lda doc_ptr2+1
    //     sbc ptr2+1
    //     sta l0081
    ptrdiff_t gap = doc_ptr2 - ptr2;
    //     ldx l0082
    uint8_t x_1 = block_expansion_len;
    //     tay
    if (gap < 256 && x_1 >= gap)
        x_1 = gap;
    // c8aa3:
    //     txa
    //     clc; adc ptr2; sta ((uint8_t*)&tmp45)[0]; lda ptr2+1; adc #0; sta
    //     ((uint8_t*)&tmp45)[1]
    uint8_t* insert_ptr = ptr2 + x_1;
    //     lda l0082 / sec; sbc l0080; sta tmp67; lda #0; sbc l0081
    // (tmp67 = l0082 - gap as signed 16-bit) — three-way split:
    // shrink (delta<0), no-change (delta==0), grow (delta>0)
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
    // delta==0 falls through
    // c8ada:
    //     ldy #0
    uint8_t y_1 = 0;
    //     sty l0081
    scratch_index = y_1;
    if (!(print_xpos & 0x80))
    {
        //     ldx input_buffer_offset+1
        uint8_t x_2 = scratch_offset;
        // loop_c8ae4:
        do
        {
            //     lda (ptr2),y
            a_3 = ptr2[y_1];
            //     iny
            y_1++;
            //     jsr is_uppercase
            //     bcc c8af3
            if (isalpha(a_3))
                goto c8af3;
            //     ror print_xpos
            // (carry-in is 1: is_uppercase left C set on this fall-through; the
            //  result C flag is dead — overwritten by jsr is_uppercase on the
            //  loop-back and by cpx/cmp on the exit path)
            print_xpos = (uint8_t)(print_xpos >> 1) | 0x80;
            //     dex
            x_2--;
            //     bne loop_c8ae4
        } while (x_2 != 0);
        //     beq c8b11
        goto c8b11;
    c8af3:
        // c8af3:
        //     pha
        {
            //     lda #0
            //     sta print_xpos
            print_xpos = 0;
            //     pla
            a_4 = a_3;
        }
        //     and #0x20 ; ' '
        a_4 &= 0x20;
        if (a_4 != 0)
            goto c8b11;
        //     bne c8b11
        //     inc l0081
        scratch_index++;
        //     dex
        x_2--;
        if (!(x_2 == 0))
        {
            //     lda (ptr2),y
            uint8_t a_5 = ptr2[y_1];
            //     jsr is_uppercase
            //     bcs c8b11
            if (!isalpha(a_5))
                goto c8b11;
            //     and #0x20 ; ' '
            a_5 &= 0x20;
            if (a_5 != 0)
                goto c8b11;
        }
        //     bne c8b11
        // c8b0d:
        //     dec l0081
        scratch_index--;
        //     dec l0081
        scratch_index--;
    }
c8b11:
    // c8b11:
    //     ldx #0
    //     stx l0082
    uint8_t output_buf_pos = 0;
    //     stx l0083
    uint8_t doc_write_pos = 0;
    //     ldx #0x14
    //     stx input_buffer_offset+1
    scratch_offset = 0x14;
    //     ldx l007a
    uint8_t x_3 = search_target_len;
    //     bne c8b6b
    if (x_3 != 0)
        goto c8b6b;
    do
    {
        //     lda header_text_maybe,x
        uint8_t a_6 = header_text_maybe[x_3];
        //     stx l0084
        temp_save = x_3;
        if (!(a_6 != 0x20))
        {
            //     bne c8b38
            //     ldy input_buffer_offset+1
            uint8_t y_2 = scratch_offset;
            //     cpy l0048
            if (y_2 >= cli_output_pos)
                goto c8b47;
            //     bcs c8b47
            //     inc input_buffer_offset+1
            scratch_offset++;
            //     lda output_buffer,y
            a_6 = output_buffer[y_2];
            //     beq c8b6a
            if (a_6 == 0)
                goto c8b6a;
            //     dex
            x_3--;
            // c8b38:
            //     cmp #1
        }
        else
        {
            if (a_6 != 1)
                goto c8b47;
            //     bne c8b47
            //     ldy l0082
            //     cpy l0049
            if (output_buf_pos >= cli_header_pos)
                goto c8b6a;
            //     bcs c8b6a
            //     lda output_buffer,y
            a_6 = output_buffer[output_buf_pos];
            //     inc l0082
            output_buf_pos++;
        }
    c8b47:
        // c8b47:
        //     cmp #2
        if (a_6 == 2)
            a_6 = 0x20;
        if (!(folding_flag & 0x80 || print_xpos != 0))
        {
            //     jsr is_uppercase
            //     bcs c8b64
            if (isalpha(a_6))
            {
                a_6 |= 0x20;
                if (scratch_index != 0)
                {
                    scratch_index--;
                    a_6 &= 0xdf;
                }
            }
        }
        // c8b64:
        //     ldy l0083
        //     sta (ptr2),y
        ptr2[doc_write_pos] = a_6;
        //     inc l0083
        doc_write_pos++;
    c8b6a:
        // c8b6a:
        //     inx
        x_3++;
    c8b6b:
        // c8b6b:
        //     cpx l004a
    } while (x_3 < cli_header_limit);
    //     lda ptr2
    //     ldy ptr2+1
    //     jsr cac78
    split_line_at_wrap(ptr2); // was scan_ptr (tmp89)
    //     clc
    //     rts
    return false;
}

// Returns true if the block read was empty (the 6502's Z flag, restored by
// the php/plp around read_block_from_file).
bool read_next_chunk_from_input_file(uint8_t* ptr)
{
    // read_next_chunk_from_input_file
    // read_next_chunk_from_input_file:
    //     jsr sub_c8da2
    uint8_t* space_limit = compute_space_available(ptr);
    //     jsr select_file
    // (inlined: file_ptr = input_fp)
    file_ptr = input_fp;
    //     jsr read_block_from_file
    uint8_t* cursor = ptr;
    read_block_status_t status = read_block_from_file(&cursor, space_limit);
    //     php
    //     beq c8d39
    //     bcc c8d39
    if (status == READ_BLOCK_DONE)
    {
        //     inc input_file_empty_flag
        input_file_empty_flag++;
    }
    // c8d39:
    //     lda #0
    //     tay                                                               ;
    //     Y=0x00
    //     sta (((uint8_t*)&tmp01)[0]),y
    *cursor = 0;
    top = cursor;
    //     plp
    //     rts
    return status == READ_BLOCK_EMPTY;
}

bool read_first_chunk_from_input_file(void)
{
    // read_first_chunk_from_input_file:
    //     lda page
    //     ldy page+1
    //     jmp read_next_chunk_from_input_file
    return read_next_chunk_from_input_file(page);
}

void write_area_to_file(void)
{
    // write_area_to_file
    // Pseudocode: Writes document area range to output file byte by byte
    // ; Does not include trailing zero!
    // write_area_to_file:
    //     jsr sanitise_area
    if (sanitise_area() == AREA_EMPTY)
        return;
    //     lda area_start_ptr
    //     sta ((uint8_t*)&tmp89)[0]
    //     lda area_start_ptr+1
    //     sta ((uint8_t*)&tmp89)[1]
    uint8_t* scan_ptr = area_start_ptr;
    //     zrepeat
    do
    {
        //         ldy #0
        // (y is only set as a side effect of the 6502's indexed dereference;
        //  the C reads *tmp89 directly and no caller reads y afterwards)
        //         lda (((uint8_t*)&tmp89)[0]),y
        uint8_t a = *scan_ptr;
        //         jsr put_byte_to_file
        // (inlined: fputc(a, file_ptr))
        fputc(a, file_ptr);
        scan_ptr++;
    } while (scan_ptr != area_end_ptr);
    // return_17:
    //     rts
}

static uint8_t* compute_space_common(uint8_t* ptr, ptrdiff_t scan_ptr)
{
    // compute_space_common
    // c8daf:
    //     sta ((uint8_t*)&tmp01)[0]
    uint8_t* line_ptr = ptr;
    //     jsr compute_bytes_free
    //     stx ((uint8_t*)&tmp67)[0]
    ptrdiff_t size_delta = compute_bytes_free();
    //     lsr ((uint8_t*)&tmp89)[1]; ror ((uint8_t*)&tmp89)[0]; lsr
    //     ((uint8_t*)&tmp89)[1]; ror ((uint8_t*)&tmp89)[0]
    scan_ptr >>= 2;
    //     lda ((uint8_t*)&tmp89)[1]; cmp #4
    // (16-bit comparison: tmp89 >= 0x0400)
    // c8dce (the clamp's C flag supplies the SBC borrow-in, so the
    //  subtraction is folded into each branch: borrow-in 0 after the clamp
    //  ran, 1 otherwise):
    //     lda ((uint8_t*)&tmp67)[0]; sbc ((uint8_t*)&tmp89)[0]; sta
    //     ((uint8_t*)&tmp67)[0]
    //     lda ((uint8_t*)&tmp67)[1]; sbc ((uint8_t*)&tmp89)[1]; sta
    //     ((uint8_t*)&tmp67)[1]
    if (scan_ptr >= 0x0400)
    {
        //     lda #4; sta ((uint8_t*)&tmp89)[1]; sta ((uint8_t*)&tmp89)[0]
        scan_ptr = 0x0404;
        size_delta -= scan_ptr;
    }
    else
    {
        size_delta -= scan_ptr + 1;
    }
    //     lda ((uint8_t*)&tmp01)[0]; clc; adc ((uint8_t*)&tmp67)[0]; sta ptr5;
    //     pha lda ((uint8_t*)&tmp01)[1]; adc ((uint8_t*)&tmp67)[1]; sta ptr5+1;
    //     sta l0081; pla
    //     sbc #0x8b
    //     bcs return_18
    //     dec l0081
    // (16-bit subtraction: result = tmp01 + tmp67 - 0x8b)
    // return_18:
    //     rts
    return line_ptr + size_delta - 0x8b;
}

static uint8_t* compute_space_available(uint8_t* ptr)
{
    // sub_c8da2
    // sub_c8da2:
    //     pha
    //     tya
    //     pha
    //     jsr compute_bytes_free
    //     stx ((uint8_t*)&tmp89)[0]
    //     pla
    //     tay
    //     pla
    return compute_space_common(ptr, compute_bytes_free());
}

static uint8_t* compute_required_space_for_insertion(uint8_t* ptr)
{
    // compute_required_space_for_insertion:
    //     ldx #0
    //     stx ((uint8_t*)&tmp89)[0]
    //     beq c8daf                                                         ;
    //     ALWAYS branch
    return compute_space_common(ptr, 0);
}

void parse_filename_from_command(struct scan_state* scan)
{
    // Pseudocode: Parses mandatory filename, calls bad_filename_error if
    // missing
    // parse_filename_from_command:
    //     jsr parse_optional_filename_from_command
    //     beq bad_filename_error  ; Z=1 → no filename
    if (!parse_optional_filename_from_command(scan))
    {
        bad_filename_error();
        return;
    }
    // return_19:
    //     rts
}

void check_continuous_editing(void)
{
    // check_continuous_editing
    // Pseudocode: Verifies continuous editing is active, shows file state if
    // not
    // check_continuous_editing:
    //     bit file_edit_flags
    if (!((file_edit_flags & 0x40)))
    {
        if (file_edit_flags & 1)
            return;
    }
    //     jsr display_document_file_state
    display_document_file_state();
}

// prepare_printer_driver, default_printer_* moved to printing.c
// parse_command moved to cli.c
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
