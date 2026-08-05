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

// Functions defined in printing.c

// #include "cpm65.inc"
// #include "driver.inc"
// #include "zif.inc"

typedef uint16_t addr_t;

// ; 6502 CPU register globals
uint8_t a, x, y, sp, flags;

// ; Longjmp buffer for stack unwinding (txs equivalent)
jmp_buf env;
#define JMP_CLI 1
#define JMP_EDITOR 2

// ; Flag bit masks
#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_V 0x40
#define FLAG_N 0x80

#define CTRL(c) ((uint8_t)((c) & 0x1f))

// ; Constants
// buffer_keyboard                             = 0
#define MAX_COMMAND_LENGTH 68
#define MAX_LINE_LENGTH 132

// ; SCREEN driver key codes

// ; File structure (removed - migrated to stdio)

// Forward declarations
void check_continuous_editing(void);
void parse_filename_from_command(void);

uint8_t check_for_command_prefix(uint8_t ch);

void sub_c8412(void);
void sub_c8c7c(void);
void sub_c83f0(void);
void sub_c8a4f(addr_t ptr2);
void sub_c8361(addr_t ptr6);
void sub_c8371(addr_t ptr2, addr_t ptr6);
void write_area_to_file(void);
void run_editor(void);
void read_first_chunk_from_input_file(void);
// Input:  a = document character, y = line offset (for tab stop lookup)
// Output: a = character to render, x = screen width consumed, y preserved,
// flags.C=0
void read_next_chunk_from_input_file(void);
static void sub_c8da2(uint8_t a, uint8_t y);
static void compute_space_common(void);
uint8_t check_for_control_code(uint8_t a);

static void system_init(void);

// Forward declarations for recently translated functions
static void compute_required_space_for_insertion(void);

#include "io.h"

#include "document.h"
#include "cli.h"
#include "editor.h"

// X ram:                              .fill 65536
uint8_t ram[65536];

// ; Memory locations

// X .section .zp, "zax", @nobits

// X ptr1: .fill 2
addr_t ptr1; // PROVISIONAL: working copy of current_format_line_ptr, used for
             // editing/unpacking lines
// X current_edit_line_ptr: .fill 2
// X current_format_line_ptr: .fill 2
addr_t current_format_line_ptr; // PROVISIONAL: points to line being
                                // formatted/printed; aliased to
                                // current_edit_line_ptr during editing
// X current_ruler_ptr: .fill 2
addr_t current_ruler_ptr; // PROVISIONAL: points to current ruler definition
                          // (tab stops, margins) in ruler buffer
// X current_line_ptr: .fill 2
addr_t current_line_ptr; // PROVISIONAL: cursor/position pointer that walks
                         // through document memory
// X page: .fill 2
addr_t
    page; // PROVISIONAL: start (lowest address) of document text area in memory
// X top: .fill 2
addr_t top; // PROVISIONAL: dynamic end-of-document pointer (top of free memory)
// X himem: .fill 2
addr_t himem; // PROVISIONAL: absolute upper bound of available RAM
// X l0011: .fill 1
// X l0012: .fill 1
addr_t top_of_screen_line_ptr; // PROVISIONAL: document address of the first
                               // visible line on screen
// X ptr6: .fill 2
addr_t ptr6; // PROVISIONAL: file/print read pointer — points to next byte to
             // read during printing/formatting
// X ptr5: .fill 2
addr_t ptr5; // PROVISIONAL: print-engine source pointer — next line/file-byte
             // to print
// X printer_driver_ptr: .fill 2 (replaced by struct pointer)
const struct printer_driver*
    printer_driver_ptr; // PROVISIONAL: function-pointer struct for printer
                        // backend dispatch
// X first_macro_ptr: .fill 2
addr_t first_macro_ptr; // PROVISIONAL: start of macro-definition linked list in
                        // document memory
// X last_macro_ptr: .fill 2
addr_t last_macro_ptr; // PROVISIONAL: end of macro-definition area where new
                       // macros are appended
// X ptr3: .fill 2
addr_t ptr3; // PROVISIONAL: macro text pointer — walks through
             // currently-executing macro body
// X oshwm: .fill 2
addr_t oshwm; // PROVISIONAL: OS high-water mark, base address for ruler stack
              // and document area
// X l0021: .fill 1
uint8_t l0021; // PROVISIONAL: remaining-lines counter on current page during
               // printing
// X l0030: .fill 1
uint8_t l0030; // PROVISIONAL: multi-line format-command line counter
// X l0031: .fill 1
uint8_t l0031; // PROVISIONAL: page-break-requested flag for the print engine
// X printing_from_file_flag: .fill 1
uint8_t printing_from_file_flag; // PROVISIONAL: selects between file-buffer and
                                 // in-memory reading during printing
// X l0033: .fill 1
uint8_t l0033; // PROVISIONAL: saved ruler_index_ptr during editor scroll-up
               // operations
// X l0034: .fill 1
uint8_t l0034; // PROVISIONAL: saved ruler_index_ptr during editor redraw
// X l0038: .fill 1
uint8_t l0038; // PROVISIONAL: page-break flag in print path; stores current key
               // in editor input
// X l0039: .fill 1
uint8_t l0039; // PROVISIONAL: column-position counter used in formatting,
               // tab-handling, and cursor recalculation
// X l003a: .fill 1
uint8_t l003a; // PROVISIONAL: ruler-buffer width / ruler byte count
// X l003b: .fill 1
uint8_t l003b; // PROVISIONAL: byte count (length) of the current document line
// X file_edit_flags: .fill 1
uint8_t file_edit_flags; // PROVISIONAL: bitfield tracking file-editing state
                         // (bit0=input open, bit6=continuous, bit7=output open)
// X l003d: .fill 1
uint8_t l003d; // PROVISIONAL: screen-row counter for partial redraw; 0xff means
               // redraw all rows
// X xpos: .fill 1
uint8_t xpos; // PROVISIONAL: cursor horizontal column position (0-based) on the
              // current edit line
// X input_file_empty_flag: .fill 1
uint8_t input_file_empty_flag; // PROVISIONAL: flag indicating whether the input
                               // file was found to be empty
// X l0042: .fill 1
uint8_t l0042; // PROVISIONAL: word-break overflow counter in
               // microspace_word_processor
// X l0043: .fill 1
uint8_t l0043; // PROVISIONAL: soft-hyphen / break-position counter in
               // microspace_word_processor
// X l0044: .fill 1
uint8_t l0044; // PROVISIONAL: inter-word extra-space accumulator in microspaced
               // justification
// X l0045: .fill 1
uint8_t l0045; // PROVISIONAL: running-total accumulator for distributing extra
               // spacing in microspaced justification
// X l0046: .fill 1
uint8_t l0046; // PROVISIONAL: non-space character counter on current line
               // during microspaced word processing
// X l0047: .fill 1
uint8_t l0047; // PROVISIONAL: saved word-break position (column) for
               // line-breaking in justification
// X l0048: .fill 1
uint8_t l0048; // PROVISIONAL: output-buffer write index in header/footer
               // rendering; block-advance flag in justification
// X l0049: .fill 1
uint8_t l0049; // PROVISIONAL: output-buffer cell/field position counter in
               // header/footer register-substitution rendering
// X l004a: .fill 1
uint8_t l004a; // PROVISIONAL: upper-bound loop limit in header/footer rendering
               // (total expanded-text length)
// X ptr2: .fill 2
addr_t ptr2; // PROVISIONAL: working pointer into document body — used as
             // source/dest in search/replace/convert
// X rw_file_handle: .fill 1
uint8_t
    rw_file_handle; // PROVISIONAL: raw OS file handle returned by open_file()
// X error_handling_mode: .fill 1
uint8_t error_handling_mode; // PROVISIONAL: 0xff = CLI-style errors; 0 = return
                             // to editor on error
// X print_flags: .fill 1
uint8_t print_flags; // PROVISIONAL: controls printer output routing and state
                     // (bit7 selects printer-driver vs screen output)
// X l006d: .fill 1
uint8_t
    edit_buffer_dirty_flag; // PROVISIONAL: non-zero when edit buffer differs
                            // from packed document and must be written back
// X l006e: .fill 1
uint8_t
    edit_buffer_unpacked_flag; // PROVISIONAL: tracks whether edit line has been
                               // unpacked; bit7 set when needs repacking
// X l006f: .fill 1
uint8_t l006f; // PROVISIONAL: line-counter/index used in document body scanning
               // and vertical-scroll calculations
// X ruler_stack_ptr: .fill 1
uint8_t ruler_index_ptr; // PROVISIONAL: index pointer into ruler-index stored
                         // in high RAM at oshwm
// X hscroll_pos: .fill 1
uint8_t
    hscroll_pos; // PROVISIONAL: horizontal scroll offset of the editor viewport
// X l0072: .fill 1
uint8_t l0072; // PROVISIONAL: right margin (right stop) of the current ruler,
               // used as wrap column limit
// X l0073: .fill 1
uint8_t l0073; // PROVISIONAL: multi-purpose flag/counter tracking rendered
               // display lines or redraw phase
// X l0074: .fill 1
uint8_t l0074; // PROVISIONAL: character/line counter incremented for each
               // CR-terminated line during document scanning
// X flags_need_redrawing_flag: .fill 1
uint8_t flags_need_redrawing_flag; // PROVISIONAL: non-zero triggers redrawing
                                   // of status-area flags/indicators
// X status_line_needs_redrawing_flag: .fill 1
uint8_t status_line_needs_redrawing_flag; // PROVISIONAL: non-zero triggers
                                          // redrawing of ruler status line
// X l0076: .fill 1
uint8_t
    l0076; // PROVISIONAL: unused/reserved variable (zeroed in init, never read)
// X ypos: .fill 1
uint8_t ypos; // PROVISIONAL: current Y (row) position on screen for cursor
              // addressing
// X print_xpos: .fill 1
uint8_t print_xpos; // PROVISIONAL: printer's current horizontal column position
// X l0079: .fill 1
uint8_t l0079; // PROVISIONAL: flag controlling early-exit in
               // character-rendering loop (non-zero skips first draw)
// X l007a: .fill 1
uint8_t l007a; // PROVISIONAL: character-count limit used as loop bound in
               // print-formatter output buffer processing
// X cursor_moved_flag: .fill 1
uint8_t cursor_moved_flag; // PROVISIONAL: incremented when cursor position
                           // changes; triggers row recalculation in display
// X l007e: .fill 1
uint8_t l007e; // PROVISIONAL: delimiter/separator character (default space)
               // used during CLI command parsing
// X input_buffer_offset: .fill 2
uint8_t input_buffer_offset; // PROVISIONAL: current read index into
                             // input_buffer during command/filename parsing
// X l0080: .fill 1
uint8_t l0080; // PROVISIONAL: low byte paired with input_buffer_offset for
               // 16-bit pointer arithmetic
// X l0081: .fill 1
uint8_t l0081; // PROVISIONAL: general-purpose counter (output-buffer position
               // index in print formatter)
// X l0082: .fill 1
uint8_t l0082; // PROVISIONAL: current screen line (row) number during document
               // rendering
// X l0083: .fill 1
uint8_t l0083; // PROVISIONAL: document line length from get_line_length; also
               // screen column during character rendering
// X l0084: .fill 1
uint8_t l0084; // PROVISIONAL: temporary column-position save/restore slot used
               // during character rendering
addr_t tmp01;  // PROVISIONAL: combined 16-bit temporary (was tmp0:tmp1)
addr_t tmp23;  // PROVISIONAL: combined 16-bit temporary (was tmp2:tmp3)
addr_t tmp45;  // PROVISIONAL: combined 16-bit temporary (was tmp4:tmp5)
addr_t tmp67;  // PROVISIONAL: combined 16-bit temporary (was tmp6:tmp7)
addr_t tmp89;  // PROVISIONAL: combined 16-bit temporary (was tmp8:tmp9)
// X file_ptr: .fill 2
FILE* file_ptr; // PROVISIONAL: currently selected FILE* for file I/O (set to
                // input_fp or output_fp)

// X .bss

// X top_margin: .fill 1
uint8_t top_margin; // PROVISIONAL: blank lines at page top before header (TM
                    // format command)
// X bottom_margin: .fill 1
uint8_t bottom_margin; // PROVISIONAL: blank lines at page bottom after footer
                       // (BM format command)
// X header_margin: .fill 1
uint8_t header_margin; // PROVISIONAL: blank lines between header text and body
                       // (HM format command)
// X footer_margin: .fill 1
uint8_t footer_margin; // PROVISIONAL: blank lines between body text and footer
                       // (FM format command)
// X page_length: .fill 1
uint8_t page_length; // PROVISIONAL: total lines per page (PL format command,
                     // default 66)
// X line_spacing: .fill 1
uint8_t
    line_spacing; // PROVISIONAL: line spacing (1 or 2) set by LS format command
// X footers_enabled_flag: .fill 1
uint8_t footers_enabled_flag; // PROVISIONAL: boolean flag controlling footer
                              // printing (FO format command)
// X headers_enabled_flag: .fill 1
uint8_t headers_enabled_flag; // PROVISIONAL: boolean flag controlling header
                              // printing (HE format command)
// X rhs_extra_margin: .fill 1
uint8_t rhs_extra_margin; // PROVISIONAL: extra right-hand margin for even pages
                          // in two-sided printing (TS)
// X macro_executing_flag: .fill 1
uint8_t macro_executing_flag; // PROVISIONAL: non-zero when a macro is currently
                              // executing
// X two_sided_flag: .fill 1
uint8_t two_sided_flag; // PROVISIONAL: enables two-sided printing (TS format
                        // command)
// X left_margin: .fill 1
uint8_t left_margin; // PROVISIONAL: left margin width in columns (LM format
                     // command)
// X highlight1_code: .fill 2
uint8_t highlight_code[2]; // PROVISIONAL: highlight control codes for text
                           // attributes (HT format command)
#define highlight1_code highlight_code[0]
#define highlight2_code highlight_code[1]
// X format_mode_flag: .fill 1
uint8_t format_mode_flag; // PROVISIONAL: bitfield controlling format mode
                          // (bit0=on, bit6=margin-release, bit7=command-line)
// X justifying_flag: .fill 1
uint8_t justifying_flag; // PROVISIONAL: enables/disables word-spacing
                         // justification during printing
// X insert_mode_flag: .fill 1
uint8_t insert_mode_flag; // PROVISIONAL: toggle insert vs overwrite mode for
                          // typed characters
// X screen_height: .fill 1
uint8_t screen_maxrow; // PROVISIONAL: maximum row index (height-1) of
                       // terminal/screen
// X screen_width: .fill 1
uint8_t screen_maxcolumn; // PROVISIONAL: maximum column index (width-1) of
                          // terminal/screen
// X microspacing_flag: .fill 1
uint8_t microspacing_flag; // PROVISIONAL: non-zero if printer driver supports
                           // microspacing
// X current_tab_key: .fill 1
uint8_t current_tab_key; // PROVISIONAL: user-configured tab key code (remapped
                         // to ASCII 9)
// X folding_flag: .fill 1
uint8_t folding_flag; // PROVISIONAL: when set uppercase folded to lowercase
                      // during printing
// X ruler_right_stop: .fill 1
uint8_t ruler_right_stop; // PROVISIONAL: right margin column from current ruler
                          // (< character)
// X ruler_left_stop: .fill 1
uint8_t ruler_left_stop; // PROVISIONAL: left margin column from current ruler
                         // (> character)

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
uint8_t printer_driver_block[0x100]; // PROVISIONAL: 256-byte workspace holding
                                     // loaded printer driver binary
// X input_buffer:                   .fill 0x45
uint8_t input_buffer[MAX_COMMAND_LENGTH]; // PROVISIONAL: CLI command-line input
                                          // buffer (69 bytes)

// X current_line_buffer:            .fill 135
#define RAM_CURRENT_LINE_BUF 0x0545
// X just_before_current_ruler_buffer: .fill 3 ; ??? something to do with
// rulers?
#define RAM_JUST_BEFORE_RULER_BUF 0x05CC
#define just_before_current_ruler_buffer (&ram[RAM_JUST_BEFORE_RULER_BUF])
// X current_ruler_buffer:           .fill 133
//  RAM_CURRENT_RULER_BUF and current_ruler_buffer defined in globals.h
// X output_buffer:                  .fill 132
uint8_t
    output_buffer[MAX_LINE_LENGTH]; // PROVISIONAL: general-purpose output
                                    // buffer for formatted line construction

// X header_text_maybe:              .fill 0x42
uint8_t header_text_maybe[0x42]; // PROVISIONAL: stores user-defined page header
                                 // text string
// X footer_text_maybe:              .fill 0x42
uint8_t footer_text_maybe[0x42]; // PROVISIONAL: stores user-defined page footer
                                 // text string

// X filename_buffer:                .fill 0x14
uint8_t filename_buffer[MAX_COMMAND_LENGTH]; // PROVISIONAL: primary filename
                                             // buffer for current file
// X output_filename:                .fill 0x14
uint8_t output_filename[MAX_COMMAND_LENGTH]; // PROVISIONAL: target filename for
                                             // save/print operations
// X printer_driver_name:            .fill 0x14
uint8_t printer_driver_name[0x14]; // PROVISIONAL: filename of loaded printer
                                   // driver (e.g. "P.DOTMATRIX")

// X register_value_array:           .fill 26*2
#define RAM_REGISTER_VALUE_ARRAY 0x0798
#define register_value_array \
    (&ram[RAM_REGISTER_VALUE_ARRAY]) // PROVISIONAL: 52-byte array of 16-bit
                                     // values for registers A-Z
// X register_value_l                = register_value_array + ('L'-'A')*2
#define RAM_REGISTER_VALUE_L (RAM_REGISTER_VALUE_ARRAY + ('L' - 'A') * 2)
// X register_value_p                = register_value_array + ('P'-'A')*2
#define RAM_REGISTER_VALUE_P (RAM_REGISTER_VALUE_ARRAY + ('P' - 'A') * 2)

#define MAX_LINES 100
#define MAX_COLUMNS 132
// X line_lengths:                   .fill 32
uint8_t line_lengths[MAX_LINES]; // PROVISIONAL: table of displayed line widths
                                 // indexed by screen row
// X input_filename:                 .fill 20
uint8_t input_filename[MAX_COMMAND_LENGTH]; // PROVISIONAL: source filename of
                                            // currently loaded document

// X input_file:                     .fill FS__SIZE
FILE* input_fp; // PROVISIONAL: FILE* handle for currently open input/read file
// X output_file:                    .fill FS__SIZE
FILE*
    output_fp; // PROVISIONAL: FILE* handle for currently open output/write file

int main(int argc, char* argv[])
{
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
    //     stx error_handling_mode
    error_handling_mode = 0xff;
    //     jsr system_init
    system_init();
    //     jsr initialise_document
    a = initialise_document();
    run_cli();
    return 0;
}

// run_editor moved to editor.c
static void sub_c8310(void)
{
    // sub_c8310
    // sub_c8310:
    //     iny
    y++;
    //     lda input_buffer,y
    a = input_buffer[y];
    //     sta l0084
    l0084 = a;
    //     cmp l007e
    cmp(&flags, a, l007e); // Z live
    //     beq return_2
    if (flags & FLAG_Z)
        return;
    //     cmp #0x0d
    cmp(&flags, a, 0x0d); // Z live
    // return_2:
    //     rts
    return;
}

void sub_c8361(addr_t ptr6)
{
    // sub_c8361
    // sub_c8361:
    //     lda #0
    //     sta l006e
    edit_buffer_unpacked_flag = 0;
    //     jsr redraw_editor
    redraw_editor(ptr6);
    //     jmp write_line_back_to_document_safely
    write_line_back_to_document_safely();
    return;
    // c836b:
    //     jsr sub_ca94a
    //     jmp esc_key
}

void sub_c8371(addr_t ptr2, addr_t ptr6)
{
    // sub_c8371
    //  Ptrs:   ptr2
    // sub_c8371:
    //     lda ptr2
    addr_t tmp89;
    uint8_t x;
    uint8_t a;
    //     sta ((uint8_t*)&tmp89)[0]
    //     lda ptr2+1
    //     sta ((uint8_t*)&tmp89)[1]
    tmp89 = ptr2;
    //     ldy #0
    uint8_t y = 0;
    //     ldx #0
    x = 0;
    // c837d:
    //     lda ((uint8_t*)&tmp89)[1]
    a = ((uint8_t*)&tmp89)[1];
    //     cmp doc_ptr2+1
    if (a != (uint8_t)(doc_ptr2 >> 8))
        goto c8389;
    //     lda ((uint8_t*)&tmp89)[0]
    a = ((uint8_t*)&tmp89)[0];
    //     cmp doc_ptr2+0
    if (a == (uint8_t)(doc_ptr2 & 0xff))
        goto c8398;
c8389:
    // c8389:
    //     lda (((uint8_t*)&tmp89)[0]),y
    a = ram[tmp89 + y];
    //     cmp #0x0d
    if (!(a != 0x0d))
    {
        x++;
    }
    // c8390:
    //     inc ((uint8_t*)&tmp89)[0]
    tmp89++;
    //     bne c837d
    //     inc ((uint8_t*)&tmp89)[1]
    //     bne c837d
c8398:
    // c8398:
    //     inc l0074
    l0074++;
    //     txa
    a = x;
    set_flags(&flags, x); // Z live
    //     beq return_3
    if (flags & FLAG_Z)
        return;
    //     jmp ca741
    ca741(ptr6);
    return;
}

static const uint8_t escaped_char_table[] = {
    '?', 'T', 'C', 'S', 'L', 'Z', '-', '*', 0xff};
static const uint8_t l83e0_table[] = {
    1, 9, 0x0d, 2, 0x0b, 0x1a, 0x1c, 0x1d, 0xff};

static uint8_t expand_escaped_string(void)
{
    // expand_escaped_string
    // expand_escaped_string:
    //     stx l0083
    l0083 = x;
    //     dey
    y--;
c83a3:
    // c83a3:
    //     jsr sub_c8310
    sub_c8310();
    //     beq c83da
    if (flags & FLAG_Z)
        goto c83da;
    //     cmp #0x5e ; '^'
    if (a != 0x5e)
        goto c83ca;
    //     bne c83ca
    //     jsr sub_c8310
    sub_c8310();
    //     beq c83da
    if (flags & FLAG_Z)
        goto c83da;
    //     jsr to_uppercase
    a = toupper(a);
    //     sta l0082
    l0082 = a;
    //     ldx #0xfe
    x = 0xfe;
loop_c83b8:
    // loop_c83b8:
    //     inx
    //     inx
    x += 2;
    {
        uint8_t idx = x >> 1;
        //     lda escaped_char_table,x
        a = escaped_char_table[idx];
        //     bmi c83c8
        if (a & 0x80)
            goto c83c8;
        //     cmp l0082
        if (a != l0082)
            goto loop_c83b8;
        //     bne loop_c83b8
        //     lda l83e0,x
        a = l83e0_table[idx];
        //     bne c83ca
        if (a != 0)
            goto c83ca;
    }
c83c8:
    // c83c8:
    //     lda l0084
    a = l0084;
c83ca:
    // c83ca:
    //     ldx l007a
    x = l007a;
    //     bne c83d1
    if (!(x != 0))
    {
        a = sub_c8c5f();
    }
    // c83d1:
    //     ldx l0083
    x = l0083;
    //     sta header_text_maybe,x
    header_text_maybe[x] = a;
    //     inc l0083
    l0083++;
    if (l0083 != 0)
        goto c83a3;
    //     bne c83a3
c83da:
    // c83da:
    //     ldx l0083
    x = l0083;
    //     sty input_buffer_offset
    input_buffer_offset = y;
    return x;
    // return_3:
    //     rts
}

void sub_c83f0(void)
{
    // sub_c83f0
    // sub_c83f0:
    //     jsr sub_c8412
    sub_c8412();
    //     beq c8410
    if (flags & FLAG_Z)
        goto c8410;
    //     jsr sub_c8e33
    sub_c8e33();
    //     beq c8402
    if (!(flags & FLAG_Z))
    {
        y = input_buffer_offset;
        y++;
        x = expand_escaped_string();
        l004a = x;
    }
    // c8402:
    //     jsr parse_marks_from_command
    parse_marks_from_command();
    //     jsr sanitise_area
    area_status_t status = sanitise_area();
    //     sec
    flags |= FLAG_C;
    //     beq return_4
    if (status == AREA_NOT_EMPTY)
    {
        sub_c8c7c();
        a = 1;
    }
c8410:
    // c8410:
    //     clc
    flags &= ~FLAG_C;
    // return_4:
    //     rts
}

void sub_c8412(void)
{
    // sub_c8412
    // sub_c8412:
    //     ldx #0
    x = 0;
    //     stx l007a
    l007a = x;
    //     stx l004a
    l004a = x;
    //     jsr sub_c8e33
    sub_c8e33();
    //     beq return_5
    if (flags & FLAG_Z)
        return;
    //     ldx #0
    x = 0;
    //     jsr expand_escaped_string
    x = expand_escaped_string();
    //     stx l007a
    l007a = x;
    //     cpx #0
    cmp(&flags, x, 0); // Z live
    // return_5:
    //     rts
}

uint8_t read_into_document(void)
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
    tmp45 = area_start_ptr;
    //     jsr move_cursor_to_address
    move_cursor_to_address(area_start_ptr);
    //     lda ((uint8_t*)&tmp45)[0]
    a = ((uint8_t*)&tmp45)[0];
    //     ldy ((uint8_t*)&tmp45)[1]
    y = ((uint8_t*)&tmp45)[1];
    //     jsr compute_required_space_for_insertion
    compute_required_space_for_insertion();
    //     jsr make_space_for_insertion
    make_space_for_insertion();

    //     jsr read_block_from_file
    read_block_from_file();
    //     beq c8584
    //     bcs c8598
    if (flags & FLAG_Z)
        goto c8584;
    if (flags & FLAG_C)
        goto c8598;
    // c8584:
c8584:
    //     jsr print_inline_string
    //     .ascii "Not all read in\r"
    //     .byte 0
    cli_putstring("Not all read in\n");
    // c8598:
c8598:
    //     lda ((uint8_t*)&tmp01)[0]
    tmp45 = tmp01;
    //     lda ptr5
    a = (uint8_t)(ptr5 & 0xff);
    //     sec
    flags |= FLAG_C;
    //     sbc ((uint8_t*)&tmp01)[0]
    a = sbc(&flags, a, (uint8_t)(((uint8_t*)&tmp01)[0] & 0xff)); // C live
    //     sta ((uint8_t*)&tmp67)[0]
    ((uint8_t*)&tmp67)[0] = a;
    //     lda ptr5+1
    a = (uint8_t)((ptr5 >> 8) & 0xff);
    //     sbc ((uint8_t*)&tmp01)[1]
    a = sbc(&flags, a, (uint8_t)(((uint8_t*)&tmp01)[1] & 0xff)); // none live
    //     sta ((uint8_t*)&tmp67)[1]
    ((uint8_t*)&tmp67)[1] = a;
    //     jsr adjust_pointers
    adjust_pointers(tmp45, tmp67);
    return a;
}

void sub_c8a4f(addr_t ptr2)
{
    uint8_t y;

    uint8_t x;

    // sub_c8a4f
    //  Ptrs:   ptr2
    // sub_c8a4f:
    //     lda #0
    a = 0;
    //     sta l0082
    l0082 = a;
    //     sta l0081
    l0081 = a;
    //     ldy #0x14
    y = 0x14;
    //     ldx l007a
    x = l007a;
    //     bne c8a87
    if (x != 0)
        goto c8a87;
c8a5b:
    // c8a5b:
    //     lda header_text_maybe,x
    a = header_text_maybe[x];
    //     cmp #1
    if (a != 1)
        goto c8a6c;
    //     bne c8a6c
    //     lda l0081
    a = l0081;
    //     cmp l0049
    if (a >= l0049)
        goto c8a86;
    //     bcs c8a86
    //     inc l0081
    l0081++;
    if (l0081 != 0)
        goto c8a84;
    //     bne c8a84
c8a6c:
    // c8a6c:
    //     cmp #0x20 ; ' '
    if (a != 0x20)
        goto c8a84;
    //     bne c8a84
    //     cpy l0048
    if (y >= l0048)
        goto c8a84;
    //     bcs c8a84
loop_c8a74:
    // loop_c8a74:
    //     lda output_buffer,y
    a = output_buffer[y];
    //     php
    {
        uint8_t saved_flags_ = flags;
        //     iny
        y++;
        //     plp
        flags = saved_flags_;
    }
    //     beq c8a86
    if (flags & FLAG_Z)
        goto c8a86;
    //     inc l0082
    l0082++;
    //     cpy l0048
    if (y < l0048)
        goto loop_c8a74;
    //     bcc loop_c8a74
    //     dec l0082
    l0082--;
c8a84:
    // c8a84:
    //     inc l0082
    l0082++;
c8a86:
    // c8a86:
    //     inx
    x++;
c8a87:
    // c8a87:
    //     cpx l004a
    if (x < l004a)
        goto c8a5b;
    //     lda doc_ptr2+0
    a = (uint8_t)(doc_ptr2 & 0xff);
    //     sec
    flags |= FLAG_C;
    //     sbc ptr2
    flags |= FLAG_C;
    a = sbc(&flags, a, (uint8_t)(ptr2 & 0xff)); // C live
    //     sta input_buffer_offset+1
    l0080 = a;
    //     lda doc_ptr2+1
    a = (uint8_t)(doc_ptr2 >> 8);
    //     sbc ptr2+1
    a = sbc(&flags, a, (uint8_t)(ptr2 >> 8)); // none live
    //     sta l0081
    l0081 = a;
    //     ldx l0082
    x = l0082;
    //     tay
    y = a;
    if (!(y != 0))
    {
        if (!(x < l0080))
        {
            x = l0080;
        }
    }
    // c8aa3:
    //     txa
    //     clc; adc ptr2; sta ((uint8_t*)&tmp45)[0]; lda ptr2+1; adc #0; sta
    //     ((uint8_t*)&tmp45)[1]
    tmp45 = ptr2 + x;
    //     lda l0082
    a = l0082;
    //     sec; sbc input_buffer_offset+1; sta ((uint8_t*)&tmp67)[0]; lda #0;
    //     sbc l0081; sta ((uint8_t*)&tmp67)[1]
    {
        uint16_t sub = (uint16_t)l0082 - ((uint16_t)l0081 << 8 | l0080);
        tmp67 = sub;
        if ((uint16_t)l0082 >= ((uint16_t)l0081 << 8 | l0080))
            flags |= FLAG_C;
        else
            flags &= ~FLAG_C;
        if (sub & 0x8000)
            flags |= FLAG_N;
        else
            flags &= ~FLAG_N;
        if (sub == 0)
            flags |= FLAG_Z;
        else
            flags &= ~FLAG_Z;
    }
    //     bmi c8aca
    if (flags & FLAG_N)
        goto c8aca;
    //     ora ((uint8_t*)&tmp67)[0]
    a |= ((uint8_t*)&tmp67)[0];
    set_flags(&flags, a); // Z live
    //     beq c8ada
    if (flags & FLAG_Z)
        goto c8ada;
    //     sta ((uint8_t*)&tmp67)[0]
    ((uint8_t*)&tmp67)[0] = a;
    //     jsr make_space_for_insertion
    make_space_for_insertion();
    //     bcc c8ada
    if (!(flags & FLAG_C))
        goto c8ada;
    //     rts
    return;

c8aca:
    // c8aca:
    //     lda #0
    a = 0;
    //     sec; sbc ((uint8_t*)&tmp67)[0]; lda #0; sbc ((uint8_t*)&tmp67)[1]
    //     (negate tmp67)
    tmp67 = -tmp67;
    //     jsr adjust_pointers
    adjust_pointers(tmp45, tmp67);
c8ada:
    // c8ada:
    //     ldy #0
    y = 0;
    //     sty l0081
    l0081 = y;
    //     bit print_xpos
    bit(&flags, a, print_xpos); // N, V live
    //     bmi c8b11
    if (flags & FLAG_N)
        goto c8b11;
    //     ldx input_buffer_offset+1
    x = l0080;
loop_c8ae4:
    // loop_c8ae4:
    //     lda (ptr2),y
    a = ram[ptr2 + y];
    //     iny
    y++;
    //     jsr is_uppercase
    if (isupper(a))
    {
        flags &= ~FLAG_C;
    }
    else
    {
        flags |= FLAG_C;
    }
    //     bcc c8af3
    if (!(flags & FLAG_C))
        goto c8af3;
    //     ror print_xpos
    print_xpos = ror(&flags, print_xpos); // C live
    //     dex
    x--;
    if (x != 0)
        goto loop_c8ae4;
    //     bne loop_c8ae4
    //     beq c8b11
    goto c8b11;

c8af3:
    // c8af3:
    //     pha
    {
        uint8_t saved_a_ = a;
        //     lda #0
        //     sta print_xpos
        print_xpos = 0;
        //     pla
        a = saved_a_;
    }
    //     and #0x20 ; ' '
    a &= 0x20;
    if (a != 0)
        goto c8b11;
    //     bne c8b11
    //     inc l0081
    l0081++;
    //     dex
    x--;
    set_flags(&flags, x); // Z live
    //     beq c8b0d
    if (flags & FLAG_Z)
        goto c8b0d;
    //     lda (ptr2),y
    a = ram[ptr2 + y];
    //     jsr is_uppercase
    if (isupper(a))
    {
        flags &= ~FLAG_C;
    }
    else
    {
        flags |= FLAG_C;
    }
    //     bcs c8b11
    if (flags & FLAG_C)
        goto c8b11;
    //     and #0x20 ; ' '
    a &= 0x20;
    if (a != 0)
        goto c8b11;
    //     bne c8b11
c8b0d:
    // c8b0d:
    //     dec l0081
    l0081--;
    //     dec l0081
    l0081--;
c8b11:
    // c8b11:
    //     ldx #0
    //     stx l0082
    l0082 = 0;
    //     stx l0083
    l0083 = 0;
    //     ldx #0x14
    //     stx input_buffer_offset+1
    l0080 = 0x14;
    //     ldx l007a
    x = l007a;
    //     bne c8b6b
    if (x != 0)
        goto c8b6b;
c8b1f:
    // c8b1f:
    //     lda header_text_maybe,x
    a = header_text_maybe[x];
    //     stx l0084
    l0084 = x;
    //     cmp #0x20 ; ' '
    if (a != 0x20)
        goto c8b38;
    //     bne c8b38
    //     ldy input_buffer_offset+1
    y = l0080;
    //     cpy l0048
    if (y >= l0048)
        goto c8b47;
    //     bcs c8b47
    //     inc input_buffer_offset+1
    l0080++;
    //     lda output_buffer,y
    a = output_buffer[y];
    //     beq c8b6a
    if (a == 0)
        goto c8b6a;
    //     dex
    x--;
    //     bcc c8b47
    goto c8b47;

c8b38:
    // c8b38:
    //     cmp #1
    if (a != 1)
        goto c8b47;
    //     bne c8b47
    //     ldy l0082
    y = l0082;
    //     cpy l0049
    if (y >= l0049)
        goto c8b6a;
    //     bcs c8b6a
    //     lda output_buffer,y
    a = output_buffer[y];
    //     inc l0082
    l0082++;
c8b47:
    // c8b47:
    //     cmp #2
    if (!(a != 2))
    {
        a = 0x20;
    }
    // c8b4d:
    //     bit folding_flag
    bit(&flags, a, folding_flag); // N, V live
    //     bmi c8b64
    if (flags & FLAG_N)
        goto c8b64;
    //     ldy print_xpos
    y = print_xpos;
    //     bne c8b64
    if (y != 0)
        goto c8b64;
    //     jsr is_uppercase
    if (isupper(a))
    {
        flags &= ~FLAG_C;
    }
    else
    {
        flags |= FLAG_C;
    }
    //     bcs c8b64
    if (!(flags & FLAG_C))
    {
        a |= 0x20;
        y = l0081;
        if (!(y == 0))
        {
            l0081--;
            a &= 0xdf;
        }
    }
c8b64:
    // c8b64:
    //     ldy l0083
    y = l0083;
    //     sta (ptr2),y
    ram[ptr2 + y] = a;
    //     inc l0083
    l0083++;
c8b6a:
    // c8b6a:
    //     inx
    x++;
c8b6b:
    // c8b6b:
    //     cpx l004a
    if (x < l004a)
        goto c8b1f;
    //     bcc c8b1f
    //     lda ptr2
    //     ldy ptr2+1
    //     jsr cac78
    cac78(tmp89);
    //     clc
    flags &= ~FLAG_C;
    //     rts
}

void sub_c8c7c(void)
{
    // sub_c8c7c:
    doc_ptr2 = area_start_ptr;
    doc_ptr3 = area_end_ptr;
    //     rts
}

void read_next_chunk_from_input_file(void)
{

    // read_next_chunk_from_input_file
    // read_next_chunk_from_input_file:
    //     jsr sub_c8da2
    sub_c8da2(a, y);
    select_file(0);
    //     jsr read_block_from_file
    read_block_from_file();
    //     php
    //     beq c8d39
    //     bcc c8d39
    if (!(flags & FLAG_Z) && (flags & FLAG_C))
    {
        //     inc input_file_empty_flag
        input_file_empty_flag++;
    }
    // c8d39:
    //     lda #0
    //     tay                                                               ;
    //     Y=0x00
    //     sta (((uint8_t*)&tmp01)[0]),y
    ram[tmp01 + 0] = 0;
    top = tmp01;
    //     plp
    //     rts
}

void read_first_chunk_from_input_file(void)
{
    // read_first_chunk_from_input_file:
    //     lda page
    a = (uint8_t)(page & 0xff);
    //     ldy page+1
    y = (uint8_t)((page >> 8) & 0xff);
    //     jmp read_next_chunk_from_input_file
    read_next_chunk_from_input_file();
}

void write_area_to_file(void)
{
    uint8_t a;

    addr_t tmp89;

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
    tmp89 = area_start_ptr;

    //     zrepeat
    do
    {
        //         ldy #0
        y = 0;
        //         lda (((uint8_t*)&tmp89)[0]),y
        a = ram[tmp89];
        //         jsr put_byte_to_file
        put_byte_to_file(a);
        tmp89++;

    } while (tmp89 != area_end_ptr);
    // return_17:
    //     rts
}

static void compute_space_common(void)
{
    // compute_space_common
    // c8daf:
    //     sta ((uint8_t*)&tmp01)[0]
    addr_t ptr5;
    addr_t tmp67;
    tmp01 = (addr_t)(y) << 8 | a;
    //     jsr compute_bytes_free
    compute_bytes_free();
    //     stx ((uint8_t*)&tmp67)[0]
    tmp67 = (addr_t)(y) << 8 | x;
    //     lsr ((uint8_t*)&tmp89)[1]; ror ((uint8_t*)&tmp89)[0]; lsr
    //     ((uint8_t*)&tmp89)[1]; ror ((uint8_t*)&tmp89)[0]
    {
        uint16_t t = tmp89;
        t >>= 2;
        tmp89 = t;
    }
    //     lda ((uint8_t*)&tmp89)[1]; cmp #4
    if (((uint8_t*)&tmp89)[1] >= 4)
    {
        //     lda #4; sta ((uint8_t*)&tmp89)[1]; sta ((uint8_t*)&tmp89)[0]
        ((uint8_t*)&tmp89)[1] = 4;
        ((uint8_t*)&tmp89)[0] = 4;
        flags |= FLAG_C;
    }
    else
    {
        flags &= ~FLAG_C;
    }
    // c8dce:
    //     lda ((uint8_t*)&tmp67)[0]; sbc ((uint8_t*)&tmp89)[0]; sta
    //     ((uint8_t*)&tmp67)[0]
    a = ((uint8_t*)&tmp67)[0];
    a = sbc(&flags, a, ((uint8_t*)&tmp89)[0]); // C live
    ((uint8_t*)&tmp67)[0] = a;
    //     lda ((uint8_t*)&tmp67)[1]; sbc ((uint8_t*)&tmp89)[1]; sta
    //     ((uint8_t*)&tmp67)[1]
    a = ((uint8_t*)&tmp67)[1];
    a = sbc(&flags, a, ((uint8_t*)&tmp89)[1]); // C live
    ((uint8_t*)&tmp67)[1] = a;
    //     lda ((uint8_t*)&tmp01)[0]; clc; adc ((uint8_t*)&tmp67)[0]; sta ptr5;
    //     pha lda ((uint8_t*)&tmp01)[1]; adc ((uint8_t*)&tmp67)[1]; sta ptr5+1;
    //     sta l0081; pla
    ptr5 = tmp01 + tmp67;
    l0081 = (uint8_t)(ptr5 >> 8);
    a = (uint8_t)ptr5;
    //     sbc #0x8b
    a = sbc(&flags, a, 0x8b); // C live
    //     sta input_buffer_offset+1
    l0080 = a;
    //     bcs return_18
    if (!(flags & FLAG_C))
    {
        //     dec l0081
        l0081--;
    }
    // return_18:
    //     rts
}

static void sub_c8da2(uint8_t a, uint8_t y)
{
    // sub_c8da2
    // sub_c8da2:
    //     pha
    //     tya
    //     pha
    {
        uint8_t saved_a = a;
        uint8_t saved_y = y;
        //     jsr compute_bytes_free
        compute_bytes_free();
        //     stx ((uint8_t*)&tmp89)[0]
        tmp89 = (addr_t)(y) << 8 | x;
        //     pla
        //     tay
        y = saved_y;
        //     pla
        a = saved_a;
    }
    compute_space_common();
}

static void compute_required_space_for_insertion(void)
{
    // compute_required_space_for_insertion:
    //     ldx #0
    //     stx ((uint8_t*)&tmp89)[0]
    tmp89 = 0;
    //     beq c8daf                                                         ;
    //     ALWAYS branch
    compute_space_common();
}

void parse_filename_from_command(void)
{
    // Pseudocode: Parses mandatory filename, calls bad_filename_error if
    // missing

    // parse_filename_from_command:
    //     jsr parse_optional_filename_from_command
    parse_optional_filename_from_command();
    //     beq bad_filename_error  ; Z=1 → no filename (but Z is never 1 from
    //     rts)
    // return_19:
    //     rts
}

static void sub_c8e2d(uint8_t y)
{
    // sub_c8e2d:
    //     lda #0x20 ; ' '
    //     sta l007e
    l007e = 0x20;
    //     sty input_buffer_offset
    input_buffer_offset = y;
    sub_c8e33();
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
        a = file_edit_flags;
        if (a & 1)
            return;
    }
    //     jsr display_document_file_state
    display_document_file_state();
}

static void display_no_text(void)
{
    // display_no_text:
    //     jsr print_inline_string
    //     .ascii "No text\r"
    //     .byte 0
    cli_putstring("No text\n");
    //     rts
}

static void display_nl_then_no_text(void)
{
    // display_nl_then_no_text:
    //     jsr bdos_print_newline
    cli_putchar('\n');
    display_no_text();
}

// prepare_printer_driver, default_printer_* moved to printing.c
// parse_command moved to cli.c
static void system_init(void)
{
    himem = 0xffff;
    oshwm = 0x0800;
    uint16_t size_ = screen_getsize();
    screen_maxcolumn = (uint8_t)(size_ & 0xff);
    screen_maxrow = (uint8_t)(size_ >> 8);
    if (screen_maxrow > MAX_LINES - 1)
        screen_maxrow = MAX_LINES - 1;
    if (screen_maxcolumn > MAX_COLUMNS - 1)
        screen_maxcolumn = MAX_COLUMNS - 1;
}
