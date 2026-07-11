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
 
#include "cli.h"

// #include "cpm65.inc"
// #include "driver.inc"
// #include "zif.inc"

// ; 6502 CPU register globals
uint8_t a, x, y, sp, flags;

// ; Longjmp buffer for stack unwinding (txs equivalent)
static volatile jmp_buf env;
#define JMP_CLI     1
#define JMP_EDITOR  2

// ; Flag bit masks
#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_V 0x40
#define FLAG_N 0x80

// ; Constants
// buffer_keyboard                             = 0
#define MAX_COMMAND_LENGTH 68
#define MAX_LINE_LENGTH    132

// ; SCREEN driver key codes
#define SCREEN_KEY_UP		0x8b
#define SCREEN_KEY_DOWN		0x8a
#define SCREEN_KEY_LEFT		0x88
#define SCREEN_KEY_RIGHT	0x89

// ; File structure (removed - migrated to stdio)


// Printer driver struct (replaces 6502 jump table with 4 entries at offsets 0,3,6,9)
struct printer_driver {
    void (*print_char)(void);
    void (*printer_on)(void);
    void (*printer_off)(void);
    void (*entry3)(void);
};

// Forward declarations
static void sub_c8c5f(void);
static void to_uppercase(void);
static void is_uppercase(void);
static void control_key_to_ascii(void);
static void draw_prompt_characters(void);
static void read_char(void);
static void tab_highlight_common(void);
static void go_to_marker_n(void);
static void set_marker_common(void);
static void set_marker(void);
static void go_to_marker(void);
static void sub_caec2(void);
static void sub_caef4(void);
static void cab91(void);
static void check_continuous_editing(void);
static void close_input_output_files(void);
static void df_fmt_cmd(void);
static void display_not_enough_memory(void);
static void print_to_screen(void);
static void reset_document_name_after_load(void);
static void return_key(void);
static void sf13_right_key(void);
static void sub_c976c(void);
static void f1_top_of_text_key(void);
static void f2_bottom_of_text_key(void);
static void f4_beginning_of_line_key(void);
static void f5_end_of_line_key(void);
static void f3_delete_to_eol_key(void);
static void cf6_split_line_key(void);
static void sf6_go_to_marker_key(void);
static void go_to_marker_1(void);
static void go_to_marker_2(void);
static void go_to_marker_3(void);
static void go_to_marker_4(void);
static void go_to_marker_5(void);
static void go_to_marker_6(void);
static void cf3_justify_mode_key(void);
static void sf2_release_margins_key(void);
static void sf8_edit_command_key(void);
static void sf9_delete_command_key(void);
static void cf2_format_mode_key(void);
static void cf8_mark_as_ruler_key(void);
static void sf11_copy_key(void);
static void cf5_default_ruler_key(void);
static void sf4_highlight1_key(void);
static void sf5_highlight2_key(void);
static void sf7_set_marker_key(void);
static void f11_copy_key(void);
static void sf0_move_block_key(void);
static void cf0_delete_block_key(void);
static void set_marker_1(void);
static void set_marker_2(void);
static void set_marker_3(void);
static void set_marker_4(void);
static void set_marker_5(void);
static void set_marker_6(void);
static void tab_key(void);
static void delete_key(void);
static void f8_insert_char_key(void);
static void esc_key(void);
static void f15_up_key(void);
static void f12_left_key(void);
static void f13_right_key(void);
static void f14_down_key(void);
static void sf12_left_key(void);
static void sf14_down_key(void);
static void sf15_up_key(void);
static void f9_delete_char_key(void);
static void f7_delete_line_key(void);
static void cf4_insert_mode_key(void);
static void f6_insert_line_key(void);
static void f0_format_block_key(void);
static void sf3_delete_to_char_key(void);
static void cf1_next_match_key(void);
static void cf7_join_lines_key(void);
static void sf1_swap_case_key(void);
static void o_command_key(void);
static void q_command_key(void);
static void k_command_key(void);
static void beep(void);
static void enter_printable_character(void);
static void sub_c9de1(void);
static void sub_c9e22(void);
static void show_memory_full_error(void);
static void editor_loop_impl(void);
static void ca741(void);
static void write_line_back_to_document_safely(void);
static void ca684(void);
static void make_space_for_insertion(void);
static void get_line_length(void);
static void prompt_for_marker(void);
static void lookup_marker(void);
static void set_marker_to_here(void);
static void move_cursor_to_address(void);
static void sub_cae06(void);
static void start_printing(void);
static void compute_bytes_free(void);
static void parse_optional_filename_from_command(void);
static void parse_filename_from_command(void);
static void check_not_continuous_editing(void);
static void get_current_fmt_cmd_byte(void);
static void get_next_fmt_cmd_byte(void);
static void sub_c9e9b(void);
static void sub_caa97(void);

static void sub_cab37(void);
static void sub_ca0af(void);
static void draw_previous_word(void);
static void draw_char(void);
static void sub_cab1a(void);
static void sub_ca44e(void);
static void sub_ca422(void);
static void draw_line(void);
static void sub_caacb(void);
static void draw_ruler(void);
static void draw_status_word(void);
static void unpack_line_into_buffer(void);
static void clear_cmd(void);
static void wipe_buffer(void);
static void caf5c(void);
static void sub_caf5f(void);
static void deref_and_check_for_command_prefix(void);
static void check_for_command_prefix(void);
static void sub_cab6e(void);
static void cab29(void);
static void push_onto_ruler_stack(void);
static void pop_from_ruler_stack(void);
static void sub_cabc4(void);
static void sub_cac41(void);
static void sub_cac50(void);
static void lookup_marker(void);
static void draw_prompt_characters(void);

static void sub_c93b6(void);
static void sub_c8412(void);
static void sub_c8c7c(void);
static void c8b7b(void);
static void sub_c83f0(void);
static void sub_c8a4f(void);
static void sanitise_area(void);
static void parse_marks_from_command(void);
static void sub_c89d3(void);
static void sub_c8361(void);
static void sub_c8371(void);
static void flush_and_read_char(void);
static void write_area_to_file(void);
static void memory_full(void);
static void run_editor(void);
static void enter_editor_mode(void);
static void move_cursor_to_top_of_document(void);
static void check_for_at_least_150_bytes_free(void);
static void cb05a(void);
static void reset_area_to_entire_document(void);
static void read_first_chunk_from_input_file(void);
static void put_byte_to_file(void);
static void c93b8(void);
static void sub_ca071(void);
// Input:  a = document character, y = line offset (for tab stop lookup)
// Output: a = character to render, x = screen width consumed, y preserved, flags.C=0
static void process_document_character(void);
static void read_next_chunk_from_input_file(void);
static void sub_c8da2(void);
static void print_newline(void);
static void print_char(void);
static void print_char_just_to_printer(void);
static void set_cursor_position(void);
static void sub_caed6(void);
static void sub_caedd(void);
static void compute_space_common(void);
static void select_file(void);
static void check_for_control_code(void);

static void clear_screen(void);
static void print_x_words_of_help(void);
static void print_char_via_putchar(void);
static void render_number_to_screen(void);
static void home_cursor(void);
static void cursor_on(void);
static void cursor_off(void);
static void save_cursor_position(void);
static void restore_cursor_position(void);
static void display_document_file_state(void);
static void compute_bytes_free(void);
static void cli_handler_impl(void);
static void return_to_cli_prompt(void);
static void run_cli(void);
static void stop_printing(void);
static void call_printer_driver(void);
static void prepare_printer_driver(void);
static void default_print_char(void);
static void default_printer_on(void);
static void default_printer_off(void);
static void default_printer_entry3(void);
static void redraw_editor(void);
static void sub_c8e33(void);
static void sub_c8c53(void);
static void sub_c8c51(void);
static void write_byte_to_memory(void);
static void write_cr_to_memory(void);
static void get_byte_from_file(void);
static void adjust_pointers(void);
static void cac78(void);
static void parse_mark_from_command(void);
static void sub_c9445(void);
static void c937b(void);
static void print_vertical_space(void);
static void render_header_or_footer(void);
static void write_line_back_to_document(void);
static void sub_c92f0(void);
static void system_init(void);
static void initialise_document(void);
static const struct printer_driver default_printer_driver;
static void render_register(void);
static void render_number_to_callback(void);
static void emit_to_output_buffer_callback(void);
static void sub_ca536(void);
static void readline(void);
static void input_line_not_escaped(void);
static void parse_command(void);
static void cae64(void);
static void c9e94(void);
static void render_char(void);


static void sub_ca4d7(void);
static void render_xchar(void);
static void clear_to_eol(void);
void create_default_ruler(void);
static void set_document_name_to_filename_buffer(void);
static void clear_marks_1_2(void);
static void reset_area_to_marks_1_2(void);
static void sub_ca1cc(void);
static void recalculate_cursor_xpos(void);
// Forward declarations for recently translated functions
static void bad_filename_error(void);
static void read_into_document(void);
static void c9263(void);
static void parse_decimal_number(void);
static void call_through_jumptable(void);
static void close_file(void);
static void compute_required_space_for_insertion(void);
static void evaluate_expression_from_fmt_cmd(void);
static void execute_formatting_command(void);
static void expand_line(void);
static void find_margins_of_current_ruler_buffer(void);
static void get_register_address(void);
static void lookup_formatting_command(void);
static void microspace_word_processor(void);
static void nested_macro_error(void);
static void open_input_file(void);
static void open_output_file(void);
static void page_eject_fmt(void);
static void parse_boolean_from_fmt_cmd(void);
static void parse_integer_from_command(void);
static void print_document(void);
static void read_block_from_file(void);
static void render_new_page(void);
static void sub_c916a(void);
static void sub_c9173(void);
static void sub_c9188(void);
static void sub_c9228(void);
static void sub_c9241(void);
static void sub_c9393(void);
static void sub_c939b(void);
static void sub_c93a1(void);
static void sub_c93be(void);
static void sub_c93c8(void);
static void sub_c93fd(void);
static void sub_c9407(void);
static void sub_c941a(void);
static void sub_c9431(void);
static void sub_c9936(void);
static void sub_c9977(void);
static void sub_c9aa9(void);
static void sub_c9ac1(void);
static void sub_cadf0(void);
static void sub_cb104(void);

#include "screen.h"

//X ram:                              .fill 65536
uint8_t ram[65536];

// ; Memory locations

//X .section .zp, "zax", @nobits

//X ptr1: .fill 2
uint16_t ptr1;
//X current_edit_line_ptr: .fill 2
uint16_t current_edit_line_ptr;
//X current_format_line_ptr: .fill 2
uint16_t current_format_line_ptr;
//X current_ruler_ptr: .fill 2
uint16_t current_ruler_ptr;
//X current_line_ptr: .fill 2
uint16_t current_line_ptr;
//X page: .fill 2
uint16_t page;
//X top: .fill 2
uint16_t top;
//X himem: .fill 2
uint16_t himem;
//X l0011: .fill 1
uint8_t l0011;
//X l0012: .fill 1
uint8_t l0012;
//X ptr6: .fill 2
uint16_t ptr6;
//X ptr5: .fill 2
uint16_t ptr5;
//X printer_driver_ptr: .fill 2 (replaced by struct pointer)
const struct printer_driver *printer_driver_ptr;
//X first_macro_ptr: .fill 2
uint16_t first_macro_ptr;
//X last_macro_ptr: .fill 2
uint16_t last_macro_ptr;
//X ptr3: .fill 2
uint16_t ptr3;
//X oshwm: .fill 2
uint16_t oshwm;
//X l0021: .fill 1
uint8_t l0021;
//X l0030: .fill 1
uint8_t l0030;
//X l0031: .fill 1
uint8_t l0031;
//X printing_from_file_flag: .fill 1
uint8_t printing_from_file_flag;
//X l0033: .fill 1
uint8_t l0033;
//X l0034: .fill 1
uint8_t l0034;
//X l0038: .fill 1
uint8_t l0038;
//X l0039: .fill 1
uint8_t l0039;
//X l003a: .fill 1
uint8_t l003a;
//X l003b: .fill 1
uint8_t l003b;
//X file_edit_flags: .fill 1
uint8_t file_edit_flags;
//X l003d: .fill 1
uint8_t l003d;
//X xpos: .fill 1
uint8_t xpos;
//X input_file_empty_flag: .fill 1
uint8_t input_file_empty_flag;
//X l0042: .fill 1
uint8_t l0042;
//X l0043: .fill 1
uint8_t l0043;
//X l0044: .fill 1
uint8_t l0044;
//X l0045: .fill 1
uint8_t l0045;
//X l0046: .fill 1
uint8_t l0046;
//X l0047: .fill 1
uint8_t l0047;
//X l0048: .fill 1
uint8_t l0048;
//X l0049: .fill 1
uint8_t l0049;
//X l004a: .fill 1
uint8_t l004a;
//X ptr2: .fill 2
uint16_t ptr2;
//X rw_file_handle: .fill 1
uint8_t rw_file_handle;
//X error_handling_mode: .fill 1
uint8_t error_handling_mode;
//X print_flags: .fill 1
uint8_t print_flags;
//X l006d: .fill 1
uint8_t l006d;
//X l006e: .fill 1
uint8_t l006e;
//X l006f: .fill 1
uint8_t l006f;
//X ruler_stack_ptr: .fill 1
uint8_t ruler_stack_ptr;
uint16_t ruler_stack[128];
//X hscroll_pos: .fill 1
uint8_t hscroll_pos;
//X l0072: .fill 1
uint8_t l0072;
//X l0073: .fill 1
uint8_t l0073;
//X l0074: .fill 1
uint8_t l0074;
//X flags_need_redrawing_flag: .fill 1
uint8_t flags_need_redrawing_flag;
//X status_line_needs_redrawing_flag: .fill 1
uint8_t status_line_needs_redrawing_flag;
//X ypos: .fill 1
uint8_t ypos;
//X print_xpos: .fill 1
uint8_t print_xpos;
//X l0079: .fill 1
uint8_t l0079;
//X l007a: .fill 1
uint8_t l007a;
//X cursor_moved_flag: .fill 1
uint8_t cursor_moved_flag;
//X l007e: .fill 1
uint8_t l007e;
//X input_buffer_offset: .fill 2
uint8_t input_buffer_offset;
//X l0081: .fill 1
uint8_t l0081;
//X l0082: .fill 1
uint8_t l0082;
//X l0083: .fill 1
uint8_t l0083;
//X l0084: .fill 1
uint8_t l0084;
//X tmp0: .fill 1
uint8_t tmp0;
//X tmp1: .fill 1
uint8_t tmp1;
//X tmp2: .fill 1
uint8_t tmp2;
//X tmp3: .fill 1
uint8_t tmp3;
//X tmp4: .fill 1
uint8_t tmp4;
//X tmp5: .fill 1
uint8_t tmp5;
//X tmp6: .fill 1
uint8_t tmp6;
//X tmp7: .fill 1
uint8_t tmp7;
//X tmp8: .fill 1
uint8_t tmp8;
//X tmp9: .fill 1
uint8_t tmp9;
//X file_ptr: .fill 2
FILE *file_ptr;

//X .bss

//X top_margin: .fill 1
uint8_t top_margin;
//X bottom_margin: .fill 1
uint8_t bottom_margin;
//X header_margin: .fill 1
uint8_t header_margin;
//X footer_margin: .fill 1
uint8_t footer_margin;
//X page_length: .fill 1
uint8_t page_length;
//X line_spacing: .fill 1
uint8_t line_spacing;
//X footers_enabled_flag: .fill 1
uint8_t footers_enabled_flag;
//X headers_enabled_flag: .fill 1
uint8_t headers_enabled_flag;
//X rhs_extra_margin: .fill 1
uint8_t rhs_extra_margin;
//X macro_executing_flag: .fill 1
uint8_t macro_executing_flag;
//X two_sided_flag: .fill 1
uint8_t two_sided_flag;
//X left_margin: .fill 1
uint8_t left_margin;
//X highlight1_code: .fill 2
uint8_t highlight_code[2];
#define highlight1_code highlight_code[0]
#define highlight2_code highlight_code[1]
//X format_mode_flag: .fill 1
uint8_t format_mode_flag;
//X justifying_flag: .fill 1
uint8_t justifying_flag;
//X insert_mode_flag: .fill 1
uint8_t insert_mode_flag;
//X screen_height: .fill 1
uint8_t screen_height;
//X screen_width: .fill 1
uint8_t screen_width;
//X microspacing_flag: .fill 1
uint8_t microspacing_flag;
//X current_tab_key: .fill 1
uint8_t current_tab_key;
//X folding_flag: .fill 1
uint8_t folding_flag;
//X ruler_right_stop: .fill 1
uint8_t ruler_right_stop;
//X ruler_left_stop: .fill 1
uint8_t ruler_left_stop;

//X __begin_pointer_array:
//X markers_array: .fill 12
//X area_start_ptr: .fill 2
//X area_end_ptr: .fill 2
//X doc_ptr1: .fill 2
//X doc_ptr2: .fill 2
//X doc_ptr3: .fill 2
//X __end_pointer_array:
struct {
    uint16_t markers_array[6];
    uint16_t area_start_ptr;
    uint16_t area_end_ptr;
    uint16_t doc_ptr1;
    uint16_t doc_ptr2;
    uint16_t doc_ptr3;
} pointer_array;
#define markers_array pointer_array.markers_array
#define area_start_ptr pointer_array.area_start_ptr
#define area_end_ptr pointer_array.area_end_ptr
#define doc_ptr1 pointer_array.doc_ptr1
#define doc_ptr2 pointer_array.doc_ptr2
#define doc_ptr3 pointer_array.doc_ptr3

//X printer_driver_block:           .fill 0x100
uint8_t printer_driver_block[0x100];
//X input_buffer:                   .fill 0x45
uint8_t input_buffer[MAX_COMMAND_LENGTH];

//X current_line_buffer:            .fill 135
uint8_t current_line_buffer[MAX_LINE_LENGTH];
//X just_before_current_ruler_buffer: .fill 3 ; ??? something to do with rulers?
uint8_t just_before_current_ruler_buffer[3];
//X current_ruler_buffer:           .fill 133
uint8_t current_ruler_buffer[MAX_LINE_LENGTH];
//X output_buffer:                  .fill 132
uint8_t output_buffer[MAX_LINE_LENGTH];

//X header_text_maybe:              .fill 0x42
uint8_t header_text_maybe[0x42];
//X footer_text_maybe:              .fill 0x42
uint8_t footer_text_maybe[0x42];

//X filename_buffer:                .fill 0x14
uint8_t filename_buffer[MAX_COMMAND_LENGTH];
//X output_filename:                .fill 0x14
uint8_t output_filename[MAX_COMMAND_LENGTH];
//X printer_driver_name:            .fill 0x14
uint8_t printer_driver_name[0x14];

//X register_value_array:           .fill 26*2
uint8_t register_value_array[26*2];
//X register_value_l                = register_value_array + ('L'-'A')*2
#define register_value_l (register_value_array + ('L'-'A')*2)
//X register_value_p                = register_value_array + ('P'-'A')*2
#define register_value_p (register_value_array + ('P'-'A')*2)

#define MAX_LINES 32
//X line_lengths:                   .fill 32
uint8_t line_lengths[MAX_LINES];
//X input_filename:                 .fill 20
uint8_t input_filename[MAX_COMMAND_LENGTH];

//X input_file:                     .fill FS__SIZE
FILE *input_fp;
//X output_file:                    .fill FS__SIZE
FILE *output_fp;
static void main_(void) {
    // Pseudocode: Program entry point with longjmp buffer for stack reset (txs equivalent)

    // .text
    // .global main
    // main:
    //     ldx #0xff
    //     txs
    int val = setjmp(env);
    if (val == JMP_CLI) {
        cli_handler_impl();
        return;
    } else if (val == JMP_EDITOR) {
        editor_loop_impl();
        return;
    }
    // Initial entry (val == 0)
    x = 0xff;
    //     stx error_handling_mode
    error_handling_mode = x;
    //     jsr system_init
    system_init();
    //     jsr initialise_document
    initialise_document();
    run_cli();
}
static void run_cli(void) {
    screen_leave();
    // run_cli:
    //     jsr clear_screen
    clear_screen();
    //     ldx #1
    x = 1;
    //     jsr print_x_words_of_help
    print_x_words_of_help();
    //     jsr print_inline_string
    //     .ascii "\r\rBytes free "
    //     .byte 0
    cli_putstring("\n\nBytes free ");

    //     jsr compute_bytes_free
    compute_bytes_free();
    //     jsr render_number_to_screen
    render_number_to_screen();
    //     jsr bdos_print_newline
    cli_putchar('\n');
    //     jsr display_document_file_state
    display_document_file_state();
    //     bit file_edit_flags
    { uint8_t tmp_ = a & file_edit_flags; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ == 0 ? FLAG_Z : 0) | (file_edit_flags & (FLAG_N|FLAG_V)); }
    //     bvs c816d
    if (flags & FLAG_V) goto c816d;
    //     lda file_edit_flags
    //     ror
    // PROBLEM: lda file_edit_flags; ror A (checking bit 0)
    //     bcc c816d
    if (!(file_edit_flags & 1)) goto c816d;
    //     jsr print_inline_string
    //     .ascii "Input file is "
    //     .byte 0
    cli_putstring("Input file is ");

    //     lda input_file_empty_flag
    a = input_file_empty_flag;
    //     bne c8163
    if (a != 0) goto c8163;
    //     jsr print_inline_string
    //     .ascii "not "
    //     .byte 0
    cli_putstring("not ");

    // c8163:
c8163:
    //     jsr print_inline_string
    //     .ascii "empty\r"
    //     .byte 0
    cli_putstring("empty\n");

    // c816d:
c816d:
    //     lda printer_driver_name
    a = printer_driver_name[0];
    //     beq c81b6
    if (flags & FLAG_Z) goto c81b6;
    //     jsr print_inline_string
    //     .ascii "Printer "
    //     .byte 0
    cli_putstring("Printer ");

    //     ldx #0
    x = 0;
    // loop_c819a:
    //     lda printer_driver_name,x
    //     cmp #0x0d
    //     beq c81a7
    //     jsr bdos_print_char
    //     inx
    //     bne loop_c819a
    do {
        a = printer_driver_name[x];
        if (a == 0x0d) break;
        cli_putchar(a);
        x++;
    } while (x != 0);
    // c81a7:
    //     lda microspacing_flag
    a = microspacing_flag;
    //     beq c81b3
    if (flags & FLAG_Z) goto c81b3;
    //     jsr print_inline_string
    //     .ascii " (m)"
    //     .byte 0
    cli_putstring(" (m)");

    // c81b3:
c81b3:
    //     jsr bdos_print_newline
    cli_putchar('\n');
    // c81b6:
c81b6:
    //     ldx #0
    x = 0;
    //     ldy #0
    y = 0;
    // c81ba:
c81ba:
    //     lda markers_array+1,x
    a = ((uint8_t*)markers_array)[x + 1];
    //     beq c81e7
    if (flags & FLAG_Z) goto c81e7;
    //     tya
    //     bne c81db
    if (y != 0) goto c81db;
    //     stx l0083
    l0083 = x;
    //     jsr print_inline_string
    //     .ascii "Marker(s) set "
    //     .byte 0
    cli_putstring("Marker(s) set ");

    //     ldx l0083
    x = l0083;
    //     ldy #1
    y = 1;
    //     bne c81e0                                                         ; ALWAYS branch
    goto c81e0;

    // c81db:
c81db:
    //     lda #0x2c ; ','
    a = 0x2c;
    //     jsr screen_putchar
    screen_putchar(a);
    // c81e0:
c81e0:
    //     txa
    a = x;
    //     lsr
    a >>= 1;
    //     adc #0x31 ; '1'
    a += 0x31;
    //     jsr screen_putchar
    screen_putchar(a);
    // c81e7:
c81e7:
    //     inx
    x++;
    //     inx
    x++;
    //     cpx #0x0c
    //     bne c81ba
    if (x != 0x0c) goto c81ba;
    //     tya
    //     beq c81f3
    if (y == 0) goto c81f3;
    //     jsr bdos_print_newline
    cli_putchar('\n');
    // c81f3:
c81f3:
    //     jsr bdos_print_newline
    cli_putchar('\n');
    return_to_cli_prompt();
}
static void run_editor(void) {
    // run_editor: Enter editor mode and jump to editor loop.
    enter_editor_mode();
    longjmp(env, JMP_EDITOR);
}
static void cli_handler_impl(void) {
    // cli_handler_impl: Main CLI loop (called after setjmp reset)

    //     jsr stop_printing
    stop_printing();
    //     ldx #0xff
    x = 0xff;
    //     stx error_handling_mode
    error_handling_mode = 0xff;
    //     txs  (handled by setjmp/longjmp in main_)
    //     inx  ; X=0x00
    x = 0;
    //     stx print_flags
    print_flags = 0;
    //     jsr print_inline_string ; .ascii "=>"
    cli_putstring("=>");
    //     jsr readline
    readline();
    //     lda #<input_buffer
    //     sta tmp0
    //     ldx #>input_buffer
    //     stx tmp1
    // (tmp0/tmp1 no longer used as a pointer; parse_command reads input_buffer[] directly)
    //     bcc input_line_not_escaped
    if (!(flags & FLAG_C)) { input_line_not_escaped(); return; }
    //     jmp run_editor
    run_editor();
}
static void return_to_cli_prompt(void) {
    longjmp(env, JMP_CLI);
}
static void esc_key(void) {
    // Pseudocode: Saves edit buffer via write_line_back_to_document_safely and returns to CLI prompt

    // esc_key:
    //     jsr write_line_back_to_document_safely
    //     jmp run_cli
    write_line_back_to_document_safely();
    run_cli();
}
static void input_line_not_escaped(void) {
    // input_line_not_escaped: Parses command input and dispatches through CLI jump table
    uint8_t l0080;

    //     jsr parse_command
    parse_command();
    //     sty input_buffer_offset+1
    l0080 = y;
    //     bcs c8263
    if (flags & FLAG_C) goto c8263;
    //     cpy #(jumptable4_cli_end-jumptable4_cli)/2
    { uint16_t tmp_ = y - 48; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= 48 ? FLAG_C : 0); }
    //     bcc c826e
    if (!(flags & FLAG_C)) goto c826e;
    // c8263:
c8263:
    //     jsr print_inline_string ; .ascii "Mistake\n"
    cli_putstring("Mistake\n");
    // c826e:
c826e:
    //     lda input_buffer_offset+1
    a = l0080;
    //     ldy #2
    y = 2;
    //     jsr call_through_jumptable
    call_through_jumptable();
    //     jmp run_cli
    run_cli();
}
static void bye_cmd(void) {
    // Pseudocode: Exits the program via BDOS exit system call

    // ; ***************************************************************************************
    // zproc bye_cmd
    //     ldy #BDOS_EXIT_PROGRAM
    //     jmp BDOS
    exit(0);
}
static void cmd_err_no_target(void) {
    // c82e7 - shared error handler for CLI commands
    // c82e7:
    //     jsr print_inline_string
    //     .ascii "No target given"
    //     .byte 0xff
    //     rts
    cli_putstring("No target given\n");
    return_to_cli_prompt(); return;
}
static void cmd_err_no_string(void) {
    // c82fa - shared error handler for CLI commands
    // c82fa:
    //     jsr print_inline_string
    //     .ascii "No string found"
    //     .byte 0xff
    //     rts
    cli_putstring("No string found\n");
    return_to_cli_prompt(); return;
}
static void search_cmd(void) {
    // Pseudocode: Searches for target string, reports position if found

    // ; ***************************************************************************************
    // search_cmd:
    //     jsr sub_c8412
    sub_c8412();
    //     beq c82e7
    if (flags & FLAG_Z) { cmd_err_no_target(); return; }
    //     jsr parse_marks_from_command
    parse_marks_from_command();
    //     jsr sanitise_area
    sanitise_area();
    //     beq c82fa
    if (flags & FLAG_Z) { cmd_err_no_string(); return; }
    //     jsr sub_c8c7c
    sub_c8c7c();
    //     jsr c8b7b
    c8b7b();
    //     bne c82fa
    if (!(flags & FLAG_Z)) { cmd_err_no_string(); return; }
    //     jsr move_cursor_to_address
    move_cursor_to_address();
    //     jmp enter_editor_mode
    enter_editor_mode();
    longjmp(env, JMP_EDITOR);
    return;

    // ; ***************************************************************************************
}
static void change_cmd(void) {
    // Pseudocode: Replaces all occurrences of search string in document area, reports change count

    // change_cmd:
    //     jsr sub_c83f0
    sub_c83f0();
    //     bcs c82fa
    if (flags & FLAG_C) { cmd_err_no_string(); return; }
    //     beq c82e7
    if (flags & FLAG_Z) { cmd_err_no_target(); return; }
    //     jsr c8b7b
    c8b7b();
    //     bne c82fa
    if (!(flags & FLAG_Z)) { cmd_err_no_string(); return; }
    //     ldx #0
    x = 0;
    //     stx ptr3
    ptr3 = (ptr3 & 0xff00) | x;
    //     stx ptr3+1
    ptr3 = (ptr3 & 0x00ff) | ((uint16_t)x << 8);
    // loop_c82b3:
loop_c82b3:
    //     inc ptr3
    //     bne c82b9
    //     inc ptr3+1
    // c82b9:
    ptr3++;
    //     jsr move_cursor_to_address
    move_cursor_to_address();
    //     lda #0
    a = 0;
    //     sta print_xpos
    print_xpos = a;
    //     jsr sub_c8a4f
    sub_c8a4f();
    //     bcs c830d
    if (flags & FLAG_C) goto c830d;
    //     jsr c8b7b
    c8b7b();
    //     beq loop_c82b3
    if (flags & FLAG_Z) goto loop_c82b3;
    //     ldx ptr3
    x = (uint8_t)(ptr3 & 0xff);
    //     ldy ptr3+1
    y = (uint8_t)((ptr3 >> 8) & 0xff);
    //     jsr render_number_to_screen
    render_number_to_screen();
    //     jsr print_inline_string
    //     .ascii " string(s) changed"
    //     .byte 0xff
    cli_putstring(" string(s) changed\n"); return_to_cli_prompt(); return;

    // c830d:
c830d:
    //     jmp display_not_enough_memory
    display_not_enough_memory();
}
static void sub_c8310(void) {
    // sub_c8310:
    //     iny
    y++;
    //     lda input_buffer,y
    a = input_buffer[y];
    //     sta l0084
    l0084 = a;
    //     cmp l007e
    { uint16_t tmp_ = a - l007e; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= l007e ? FLAG_C : 0); }
    //     beq return_2
    if (flags & FLAG_Z) return;
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    // return_2:
    //     rts
}
static void replace_cmd(void) {
    // Pseudocode: Interactive search and replace prompting for each match (Y)es/(O)K/(N)o

    // ; ***************************************************************************************
    // replace_cmd:
    //     jsr sub_c83f0
    sub_c83f0();
    //     beq c82e7
    if (flags & FLAG_Z) { cmd_err_no_target(); return; }
    //     jsr c8b7b
    c8b7b();
    //     bne c82fa
    if (!(flags & FLAG_Z)) { cmd_err_no_string(); return; }
    //     jsr move_cursor_to_address
    move_cursor_to_address();
    //     jsr enter_editor_mode
    enter_editor_mode();
    // c832d:
c832d:
    //     jsr sub_c8361
    sub_c8361();
    //     ldx #0x52 ; 'R'
    x = 0x52;
    //     ldy #0x50 ; 'P'
    y = 0x50;
    //     jsr draw_prompt_characters
    draw_prompt_characters();
    //     jsr flush_and_read_char
    flush_and_read_char();
    //     bcs return_2
    if (flags & FLAG_C) return;
    //     and #0xdf
    a &= 0xdf;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     ldx #0
    x = 0;
    //     cmp #0x59 ; 'Y'
    { uint8_t tmp_ = a - 0x59; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | (tmp_ & FLAG_N) | (a >= 0x59 ? FLAG_C : 0); }
    //     beq c8349
    if (flags & FLAG_Z) goto c8349;
    //     dex                                                               ; X=0xff
    x--;
    //     cmp #0x4f ; 'O'
    { uint8_t tmp_ = a - 0x4f; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | (tmp_ & FLAG_N) | (a >= 0x4f ? FLAG_C : 0); }
    //     bne c8356
    if (!(flags & FLAG_Z)) goto c8356;
    // c8349:
c8349:
    //     stx print_xpos
    print_xpos = x;
    //     jsr sub_c8371
    sub_c8371();
    //     jsr sub_c8a4f
    sub_c8a4f();
    //     bcs c836b
    if (flags & FLAG_C) { show_memory_full_error(); esc_key(); return; }
    //     jsr sub_c8361
    sub_c8361();
    // c8356:
c8356:
    //     jsr c8b7b
    c8b7b();
    //     bne return_2
    if (!(flags & FLAG_Z)) return;
    //     jsr move_cursor_to_address
    move_cursor_to_address();
    //     jmp c832d
    goto c832d;
}
static void sub_c8361(void) {
    // sub_c8361:
    //     lda #0
    a = 0;
    //     sta l006e
    l006e = a;
    //     jsr redraw_editor
    redraw_editor();
    //     jmp write_line_back_to_document_safely
    write_line_back_to_document_safely(); return;
    // c836b:
    //     jsr sub_ca94a
    //     jmp esc_key
}
static void sub_c8371(void) {
    // sub_c8371:
    //     lda ptr2
    a = (uint8_t)(ptr2 & 0xff);
    //     sta tmp8
    tmp8 = a;
    //     lda ptr2+1
    a = (uint8_t)(ptr2 >> 8);
    //     sta tmp9
    tmp9 = a;
    //     ldy #0
    y = 0;
    //     ldx #0
    x = 0;
c837d:
    // c837d:
    //     lda tmp9
    a = tmp9;
    //     cmp doc_ptr2+1
    { uint16_t tmp_ = a - (uint8_t)(doc_ptr2 >> 8); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= (uint8_t)(doc_ptr2 >> 8) ? FLAG_C : 0); }
    //     bne c8389
    if (!(flags & FLAG_Z)) goto c8389;
    //     lda tmp8
    a = tmp8;
    //     cmp doc_ptr2+0
    { uint16_t tmp_ = a - (uint8_t)(doc_ptr2 & 0xff); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= (uint8_t)(doc_ptr2 & 0xff) ? FLAG_C : 0); }
    //     beq c8398
    if (flags & FLAG_Z) goto c8398;
c8389:
    // c8389:
    //     lda (tmp8),y
    a = ram[((uint16_t)tmp9 << 8 | tmp8) + y];
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     bne c8390
    if (!(flags & FLAG_Z)) goto c8390;
    //     inx
    x++;
c8390:
    // c8390:
    //     inc tmp8
    tmp8++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (tmp8 == 0 ? FLAG_Z : 0) | (tmp8 & FLAG_N);
    //     bne c837d
    if (!(flags & FLAG_Z)) goto c837d;
    //     inc tmp9
    tmp9++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (tmp9 == 0 ? FLAG_Z : 0) | (tmp9 & FLAG_N);
    //     bne c837d
    if (!(flags & FLAG_Z)) goto c837d;
c8398:
    // c8398:
    //     inc l0074
    l0074++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0074 == 0 ? FLAG_Z : 0) | (l0074 & FLAG_N);
    //     txa
    a = x;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     beq return_3
    if (flags & FLAG_Z) return;
    //     jmp ca741
    ca741(); return;
}
static const uint8_t escaped_char_table[] = { '?', 'T', 'C', 'S', 'L', 'Z', '-', '*', 0xff };
static const uint8_t l83e0_table[] = { 1, 9, 0x0d, 2, 0x0b, 0x1a, 0x1c, 0x1d, 0xff };

static void expand_escaped_string(void) {
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
    if (flags & FLAG_Z) goto c83da;
    //     cmp #0x5e ; '^'
    { uint16_t tmp_ = a - 0x5e; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x5e ? FLAG_C : 0); }
    //     bne c83ca
    if (!(flags & FLAG_Z)) goto c83ca;
    //     jsr sub_c8310
    sub_c8310();
    //     beq c83da
    if (flags & FLAG_Z) goto c83da;
    //     jsr to_uppercase
    to_uppercase();
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
        if (a & 0x80) goto c83c8;
        //     cmp l0082
        { uint16_t tmp_ = a - l0082; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= l0082 ? FLAG_C : 0); }
        //     bne loop_c83b8
        if (!(flags & FLAG_Z)) goto loop_c83b8;
        //     lda l83e0,x
        a = l83e0_table[idx];
        //     bne c83ca
        if (a != 0) goto c83ca;
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
    if (x != 0) goto c83d1;
    //     jsr sub_c8c5f
    sub_c8c5f();
c83d1:
    // c83d1:
    //     ldx l0083
    x = l0083;
    //     sta header_text_maybe,x
    header_text_maybe[x] = a;
    //     inc l0083
    l0083++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0083 == 0 ? FLAG_Z : 0) | (l0083 & FLAG_N);
    //     bne c83a3
    if (!(flags & FLAG_Z)) goto c83a3;
c83da:
    // c83da:
    //     ldx l0083
    x = l0083;
    //     sty input_buffer_offset
    input_buffer_offset = y;
    // return_3:
    //     rts
}
static void sub_c83f0(void) {
    // sub_c83f0:
    //     jsr sub_c8412
    sub_c8412();
    //     beq c8410
    if (flags & FLAG_Z) goto c8410;
    //     jsr sub_c8e33
    sub_c8e33();
    //     beq c8402
    if (flags & FLAG_Z) goto c8402;
    //     ldy input_buffer_offset
    y = input_buffer_offset;
    //     iny
    y++;
    //     jsr expand_escaped_string
    expand_escaped_string();
    //     stx l004a
    l004a = x;
c8402:
    // c8402:
    //     jsr parse_marks_from_command
    parse_marks_from_command();
    //     jsr sanitise_area
    sanitise_area();
    //     sec
    flags |= FLAG_C;
    //     beq return_4
    if (flags & FLAG_Z) goto c8410;
    //     jsr sub_c8c7c
    sub_c8c7c();
    //     lda #1
    a = 1;
c8410:
    // c8410:
    //     clc
    flags &= ~FLAG_C;
    // return_4:
    //     rts
}
static void sub_c8412(void) {
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
    if (flags & FLAG_Z) return;
    //     ldx #0
    x = 0;
    //     jsr expand_escaped_string
    expand_escaped_string();
    //     stx l007a
    l007a = x;
    //     cpx #0
    { uint16_t tmp_ = x - 0; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= 0 ? FLAG_C : 0); }
    // return_5:
    //     rts
}
static void screen_cmd(void) {
    // Pseudocode: Jumps to print_to_screen for on-screen document preview

    // ; ***************************************************************************************
    // screen_cmd:
    //     jmp print_to_screen
    print_to_screen(); return;
}
static void sheets_cmd(void) {
    // Pseudocode: Prints document to printer then displays newline and returns to CLI

    // ; ***************************************************************************************
    // sheets_cmd:
    //     lda #0xc0
    a = 0xc0;
    //     jsr start_printing
    start_printing();
    //     jsr print_document
    print_document();
    //     jsr stop_printing
    stop_printing();
    //     jsr bdos_print_newline
    cli_putchar('\n');
    //     jmp return_to_cli_prompt
    return_to_cli_prompt(); return;
}
static void print_cmd(void) {
    // Pseudocode: Sets print flags and falls through to print_to_screen

    // ; ***************************************************************************************
    // print_cmd:
    //     lda #0x80
    //     jsr start_printing
    // ; ***************************************************************************************
    a = 0x80;
    start_printing();
    // MULTIPLE ENTRY POINTS: print_cmd, print_to_screen
    print_to_screen();
}
static void print_to_screen(void) {
    // print_to_screen: Prints document for screen preview, returns to CLI when done

    //     jsr print_document
    print_document();
    //     jmp return_to_cli_prompt
    return_to_cli_prompt(); return;
}
static void stop_printing(void) {
    // Pseudocode: Stops active printing by clearing print flags and calling printer driver cleanup

    // ; ***************************************************************************************
    // stop_printing:
    //     lda print_flags
    a = print_flags;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bpl c8459
    if (!(flags & FLAG_N)) goto c8459;
    //     rol print_flags
    // PROBLEM: rol print_flags (rotate on memory)
    //     clc
    flags &= ~FLAG_C;
    //     ror print_flags
    // PROBLEM: ror print_flags (rotate on memory)
    //     lda #6
    a = 6;
    //     jsr call_printer_driver
    call_printer_driver();
    // c8459:
c8459:
    //     rts
    return;
}
static void start_printing(void) {
    // Pseudocode: Initializes printer driver and starts printing with given flags

    // start_printing:
    //     jsr print_inline_string
    //     .ascii "Sorry, can't print yet\r"
    //     .byte 0
    cli_putstring("Sorry, can't print yet\n");
    //     jmp return_to_cli_prompt
    return_to_cli_prompt(); return;

    //     sta print_flags
    //     jsr prepare_printer_driver
    //     lda #3
    //     jmp call_printer_driver

    // MULTIPLE ENTRY POINTS: start_printing has stub and real code paths
}
static void edit_cmd(void) {
    check_not_continuous_editing();
    parse_filename_from_command();
    set_document_name_to_filename_buffer();
    open_input_file();
    parse_filename_from_command();
    open_output_file();
    x = 0;
    input_file_empty_flag = x;
    do {
        a = filename_buffer[x];
        if (a == 0) a = 0x0d;
        output_filename[x] = a;
        x++;
    } while (a != 0x0d);
    initialise_document();
    read_first_chunk_from_input_file();
    if (flags & FLAG_Z) {
        close_input_output_files();
        return_to_cli_prompt();
        return;
    }
    file_edit_flags = 1;
}
static void more_cmd(void) {
    // Pseudocode: Appends more text from input file into document at current cursor position

    // ; ***************************************************************************************
    // more_cmd:
    //     jsr check_continuous_editing
    check_continuous_editing();
    //     jsr parse_marks_from_command
    parse_marks_from_command();
    //     lda area_start_ptr
    a = (uint8_t)(area_start_ptr & 0xff);
    //     ldy area_start_ptr+1
    y = (uint8_t)((area_start_ptr >> 8) & 0xff);
    //     jsr move_cursor_to_address
    move_cursor_to_address();

    //     jsr select_file
    x = 1;
    select_file();
    //     jsr write_area_to_file
    write_area_to_file();
    //     bne c84ab
    if (!(flags & FLAG_Z)) { return_to_cli_prompt(); return; }

    //     ldy #0
    y = 0;
    //     ldx l003a
    x = l003a;
    // loop_c84c4:
loop_c84c4:
    //     lda (current_ruler_ptr),y
    a = ram[current_ruler_ptr + y];
    //     sta current_ruler_buffer,y
    current_ruler_buffer[y] = a;
    //     iny
    y++;
    //     dex
    x--;
    //     bne loop_c84c4
    if (x != 0) goto loop_c84c4;
    //     lda #0x0d
    a = 0x0d;
    //     sta current_ruler_buffer,y
    current_ruler_buffer[y] = a;
    //     jsr sub_c89d3
    sub_c89d3();
    //     jsr move_cursor_to_top_of_document
    move_cursor_to_top_of_document();
    //     jsr check_for_at_least_150_bytes_free
    check_for_at_least_150_bytes_free();
    //     lda input_file_empty_flag
    a = input_file_empty_flag;
    //     bne c84e8
    if (a != 0) goto c84e8;
    //     lda top
    a = (uint8_t)(top & 0xff);
    //     ldy top+1
    y = (uint8_t)((top >> 8) & 0xff);
    //     jsr read_next_chunk_from_input_file
    read_next_chunk_from_input_file();
    //     beq c84ab
    if (flags & FLAG_Z) { return_to_cli_prompt(); return; }
    // c84e8:
c84e8:
    //     jmp cb05a
    cb05a();
}
static void finish_cmd(void) {
    // Pseudocode: Writes remaining document content to output file in chunks

    // ; ***************************************************************************************
    // finish_cmd:
    //     jsr check_continuous_editing
    check_continuous_editing();
    // loop_c84ee:
loop_c84ee:
    //     jsr reset_area_to_entire_document
    reset_area_to_entire_document();
    //     jsr sanitise_area
    sanitise_area();

    //     jsr select_file
    x = 1;
    select_file();

    //     jsr write_area_to_file
    write_area_to_file();
    //     bne c84ab
    if (!(flags & FLAG_Z)) { return_to_cli_prompt(); return; }
    //     lda #0
    a = 0;
    //     jsr put_byte_to_file                ; write terminator
    put_byte_to_file();
    //     jsr sub_c89d3
    sub_c89d3();
    //     jsr move_cursor_to_top_of_document
    move_cursor_to_top_of_document();
    //     jsr cb05a
    cb05a();
    //     lda input_file_empty_flag
    a = input_file_empty_flag;
    //     bne close_input_output_files
    if (a != 0) { close_input_output_files(); return; }
    //     jsr read_first_chunk_from_input_file
    read_first_chunk_from_input_file();
    //     beq c84ab
    if (flags & FLAG_Z) { return_to_cli_prompt(); return; }
    //     bne loop_c84ee                                                    ; ALWAYS branch
    goto loop_c84ee;
}
static void quit_cmd(void) {
    // Pseudocode: Checks continuous editing then falls through to close files

    // ; ***************************************************************************************
    // quit_cmd:
    //     jsr check_continuous_editing
    check_continuous_editing();
    // MULTIPLE ENTRY POINTS: quit_cmd, close_input_output_files
    close_input_output_files();
}
static void close_input_output_files(void) {
    // Pseudocode: Closes output file, resets editing flags, returns to CLI

    // close_input_output_files:
    //     lda #0
    a = 0;
    //     sta input_file_empty_flag
    input_file_empty_flag = a;
    //     sta file_edit_flags
    file_edit_flags = a;

    //     jsr select_file
    x = 1;
    select_file();
    //     jsr close_file
    close_file();
    //     jmp return_to_cli_prompt
    return_to_cli_prompt(); return;

    // MULTIPLE ENTRY POINTS: quit_cmd, close_input_output_files
}
static void save_cmd_write_cmd(void) {
    // Pseudocode: Saves document area to output file with optional filename

    // ; ***************************************************************************************
    // save_cmd:
    // write_cmd:
    //     jsr parse_optional_filename_from_command
    parse_optional_filename_from_command();
    //     zif eq
    if (flags & FLAG_Z) {
    //         bit file_edit_flags
        { flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_V)) | ((a & file_edit_flags) == 0 ? FLAG_Z : 0) | (file_edit_flags & (FLAG_N|FLAG_V)); }
    //         zif vc
        if (!(flags & FLAG_V)) {
    //             jmp bad_filename_error
            bad_filename_error(); return;
    //         zendif
        }

    //         ldx #0
        x = 0;
    //         zrepeat
        do {
    //             lda input_filename,x
            a = input_filename[x];
    //             sta filename_buffer,x
            filename_buffer[x] = a;
    //             inx
            x++;
    //             cmp #0x0d
            { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //         zuntil eq
        } while (!(flags & FLAG_Z));
    //     zendif
    }
    //     jsr parse_marks_from_command
    parse_marks_from_command();
    //     jsr sanitise_area
    sanitise_area();
    //     beq return_6
    if (flags & FLAG_Z) return;

    //     jsr open_output_file
    open_output_file();
    //     jsr write_area_to_file
    write_area_to_file();
    //     lda #0
    a = 0;
    //     jsr put_byte_to_file
    put_byte_to_file();

    //     jsr close_file
    close_file();
    //     jmp return_to_cli_prompt
    return_to_cli_prompt(); return;

    // MULTIPLE ENTRY POINTS: save_cmd, write_cmd
}
static void check_for_at_least_150_bytes_free(void) {
    // Pseudocode: Checks if at least 150 bytes of memory are available

    // check_for_at_least_150_bytes_free:
    //     jsr compute_bytes_free
    compute_bytes_free();
    //     tya
    //     bne return_6
    if (y != 0) return;
    //     cpx #0x96
    //     bcs return_6
    if (x >= 0x96) return;

    // MULTIPLE ENTRY POINTS: check_for_at_least_150_bytes_free, display_not_enough_memory
    display_not_enough_memory();
}
static void display_not_enough_memory(void) {
    // Pseudocode: Displays Not enough memory error and stops printing

    // display_not_enough_memory:
    //     jsr stop_printing
    stop_printing();
    //     jsr print_inline_string
    //     .ascii "Not enough memory"
    //     .byte 0xff
    cli_putstring("Not enough memory\n"); return_to_cli_prompt(); return;
// return_6:
return_6:
    //     rts
    return;

    // MULTIPLE ENTRY POINTS: check_for_at_least_150_bytes_free, display_not_enough_memory
}
static void read_into_document(void) {
    // 1: - shared entry point used by both load_cmd and read_cmd

    //     jsr check_for_at_least_150_bytes_free
    check_for_at_least_150_bytes_free();

    //     jsr open_input_file
    open_input_file();

    //     lda area_start_ptr
    a = (uint8_t)(area_start_ptr & 0xff);
    //     ldy area_start_ptr+1
    y = (uint8_t)((area_start_ptr >> 8) & 0xff);
    //     sta tmp4
    tmp4 = a;
    //     sty tmp5
    tmp5 = y;
    //     jsr move_cursor_to_address
    move_cursor_to_address();
    //     lda tmp4
    a = tmp4;
    //     ldy tmp5
    y = tmp5;
    //     jsr compute_required_space_for_insertion
    compute_required_space_for_insertion();
    //     jsr make_space_for_insertion
    make_space_for_insertion();

    //     jsr read_block_from_file
    read_block_from_file();
    //     beq c8584
    //     bcs c8598
    if (flags & FLAG_Z) goto c8584;
    if (flags & FLAG_C) goto c8598;
    // c8584:
c8584:
    //     jsr print_inline_string
    //     .ascii "Not all read in\r"
    //     .byte 0
    cli_putstring("Not all read in\n");

    // c8598:
c8598:
    //     lda tmp0
    a = tmp0;
    //     sta tmp4
    tmp4 = a;
    //     lda tmp1
    a = tmp1;
    //     sta tmp5
    tmp5 = a;
    //     lda ptr5
    a = (uint8_t)(ptr5 & 0xff);
    //     sec
    flags |= FLAG_C;
    //     sbc tmp0
    { uint16_t tmp_ = (uint16_t)a - (uint8_t)(tmp0 & 0xff) - (1 - ((flags & FLAG_C) ? 1 : 0)); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ <= 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     sta tmp6
    tmp6 = a;
    //     lda ptr5+1
    a = (uint8_t)((ptr5 >> 8) & 0xff);
    //     sbc tmp1
    { uint16_t tmp_ = (uint16_t)a - (uint8_t)(tmp1 & 0xff) - (1 - ((flags & FLAG_C) ? 1 : 0)); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ <= 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     sta tmp7
    tmp7 = a;
    //     jsr adjust_pointers
    adjust_pointers();
}
static void load_cmd(void) {
    // Pseudocode: Loads a file, initializes document, clears markers, moves cursor to top

    // ; ***************************************************************************************
    // load_cmd:
    //     jsr check_not_continuous_editing
    check_not_continuous_editing();
    //     jsr parse_filename_from_command
    parse_filename_from_command();
    //     jsr initialise_document
    initialise_document();
    //     jsr reset_area_to_entire_document
    reset_area_to_entire_document();
    //     jsr 1f            ; local forward jump - jumps to "1:" in read_cmd
    read_into_document();
    //     jsr reset_document_name_after_load
    reset_document_name_after_load();
    //     jsr clear_cmd
    clear_cmd();
    //     jmp move_cursor_to_top_of_document
    move_cursor_to_top_of_document();
}
static void read_cmd(void) {
    // Pseudocode: Reads file contents into document at current area_start position

    // read_cmd:
    //     jsr parse_filename_from_command
    parse_filename_from_command();
    //     jsr parse_marks_from_command
    parse_marks_from_command();
    // 1:
    read_into_document();
    //     jmp return_to_cli_prompt
    return_to_cli_prompt(); return;
}
static void mode_cmd(void) {
    // ; ***************************************************************************************
    // mode_cmd:
    //     jsr print_inline_string
    //     .ascii "Bad mode"
    //     .byte 0xff
    cli_putstring("Bad mode\n"); return_to_cli_prompt(); return;
}
static void microspace_cmd(void) {
    // Pseudocode: Configures microspacing by querying printer driver

    // ; ***************************************************************************************
    // microspace_cmd:
    //     jsr prepare_printer_driver
    prepare_printer_driver();
    //     jsr parse_integer_from_command
    parse_integer_from_command();
    //     php
    // PROBLEM: php (push flags)
    //     ldx #0x0a
    x = 0x0a;
    //     plp
    // PROBLEM: plp (pull flags)
    //     beq c8608
    if (flags & FLAG_Z) goto c8608;
    //     ldx tmp8
    x = tmp8;
    //     beq return_7
    if (x == 0) return;
    // c8608:
c8608:
    //     ldy #0
    y = 0;
    //     lda #0x0c
    a = 0x0c;
    //     jsr call_printer_driver
    call_printer_driver();
    //     tya
    a = y;
    //     and #1
    a &= 1; flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq c8617
    if (flags & FLAG_Z) goto c8617;
    //     stx microspacing_flag
    microspacing_flag = x;
    // return_7:
return_7:
    //     rts
    return;

    // c8617:
c8617:
    //     jsr print_inline_string
    //     .ascii "Driver does not support microspacing"
    //     .byte 0xff
    cli_putstring("Driver does not support microspacing\n"); return_to_cli_prompt(); return;
}
static void setup_cmd(void) {
    // Pseudocode: Parses flag letters and sets format_mode_flag, justifying_flag, insert_mode_flag

    // ; ***************************************************************************************
    // setup_cmd:
    //     ldx #1
    x = 1;
    //     stx tmp6
    tmp6 = x;
    //     dex                                                               ; X=0x00
    x--;
    //     stx tmp8
    tmp8 = x;
    //     dex                                                               ; X=0xff
    x--;
    //     stx tmp7
    tmp7 = x;
    // c8649:
c8649:
    //     jsr sub_c8e33
    // PROBLEM: jsr sub_c8e33
    //     beq c8672
    if (flags & FLAG_Z) goto c8672;
    //     and #0xdf
    a &= 0xdf; flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     ldx #0
    x = 0;
    // loop_c8652:
loop_c8652:
    //     cmp c867d,x
    { uint16_t tmp_ = a - ((const uint8_t[]){0x4e, 0x4a, 0x00, 0x49, 0x00})[x]; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= ((const uint8_t[]){0x4e, 0x4a, 0x00, 0x49, 0x00})[x] ? FLAG_C : 0); }
    //     beq c8669
    if (flags & FLAG_Z) goto c8669;
    //     inx
    x++;
    //     ldy c867d,x
    y = ((const uint8_t[]){0x4e, 0x4a, 0x00, 0x49, 0x00})[x];
    //     bne loop_c8652
    if (y != 0) goto loop_c8652;
    //     jsr print_inline_string
    //     .ascii "Bad flag"
    //     .byte 0xff
    cli_putstring("Bad flag\n"); return_to_cli_prompt(); return;

    // c8669:
c8669:
    //     lda c8681,x
    a = ((const uint8_t[]){0x00, 0x00, 0xff})[x];
    //     sta tmp6,x
    if (x == 0) tmp6 = a; else if (x == 1) tmp7 = a; else tmp8 = a;
    //     inc input_buffer_offset
    input_buffer_offset++;
    //     bne c8649
    if (!(flags & FLAG_Z)) goto c8649;
    // c8672:
c8672:
    //     ldx #2
    x = 2;
    // loop_c8674:
loop_c8674:
    //     lda tmp6,x
    if (x == 0) a = tmp6; else if (x == 1) a = tmp7; else a = tmp8;
    //     sta format_mode_flag,x
    if (x == 0) format_mode_flag = a; else if (x == 1) justifying_flag = a; else insert_mode_flag = a;
    //     dex
    x--;
    //     bpl loop_c8674
    if (!(flags & FLAG_N)) goto loop_c8674;
    //     bmi c869b                                                         ; ALWAYS branch
    return_to_cli_prompt(); return;

    // c867d:
    //     lsr l004a
    //     eor #0
    // c8681:
    //     brk

    //     .byte 0, 0xff
}
static void field_cmd(void) {
    // Pseudocode: Sets the tab key field width from parsed integer argument

    // ; ***************************************************************************************
    // field_cmd:
    //     jsr parse_integer_from_command
    parse_integer_from_command();
    //     beq c869b
    if (flags & FLAG_Z) { return_to_cli_prompt(); return; }
    //     lda tmp8
    a = tmp8;
    //     cmp #0x1b
    { uint16_t tmp_ = a - 0x1b; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x1b ? FLAG_C : 0); }
    //     bne c8699
    if (!(flags & FLAG_Z)) goto c8699;
    //     jsr print_inline_string
    //     .ascii "Frump!"
    //     .byte 0xff
    cli_putstring("Frump!\n"); return_to_cli_prompt(); return;

    // c8699:
c8699:
    //     sta current_tab_key
    current_tab_key = a;
    // c869b:
c869b:
    //     jmp return_to_cli_prompt
    return_to_cli_prompt(); return;
}
static void count_cmd(void) {
    // Pseudocode: Counts words in document area handling command prefixes and punctuation
    static const uint8_t l8747_data[] = { 0x52, 0x4a, 'C', 'E', 'L', 'J', 0 };

    // ; ***************************************************************************************
    // count_cmd:
    //     jsr parse_marks_from_command
    parse_marks_from_command();
    //     jsr sanitise_area
    sanitise_area();
    //     beq c869b
    if (flags & FLAG_Z) { return_to_cli_prompt(); return; }
    //     lda area_start_ptr
    a = (uint8_t)(area_start_ptr & 0xff);
    //     sta tmp0
    tmp0 = a;
    //     lda area_start_ptr+1
    a = (uint8_t)((area_start_ptr >> 8) & 0xff);
    //     sta tmp1
    tmp1 = a;
    //     lda #0
    a = 0;
    //     sta tmp8
    tmp8 = a;
    //     sta tmp9
    tmp9 = a;
    //     sta l0083
    l0083 = a;
    //     sta l0082
    l0082 = a;
    // c86b8:
c86b8:
    //     ldy #0
    y = 0;
    //     jsr deref_and_check_for_command_prefix
    deref_and_check_for_command_prefix();
    //     bne c86ea
    if (!(flags & FLAG_Z)) goto c86ea;
    //     ldx #0
    x = 0;
    //     iny
    y++;
    // loop_c86c2:
loop_c86c2:
    //     lda (tmp0),y
    a = ram[((uint16_t)tmp1 << 8 | tmp0) + y];
    //     iny
    y++;
    //     cmp l8747,x
    { uint16_t tmp_ = a - l8747_data[x]; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= l8747_data[x] ? FLAG_C : 0); }
    //     bne c86d1
    if (!(flags & FLAG_Z)) goto c86d1;
    //     lda (tmp0),y
    a = ram[((uint16_t)tmp1 << 8 | tmp0) + y];
    //     cmp l8748,x
    { uint16_t tmp_ = a - l8747_data[x+1]; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= l8747_data[x+1] ? FLAG_C : 0); }
    //     beq c86df
    if (flags & FLAG_Z) goto c86df;
    // c86d1:
c86d1:
    //     lda l8749,x
    a = l8747_data[x+2];
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq c86db
    if (flags & FLAG_Z) goto c86db;
    //     dey
    y--;
    //     inx
    x++;
    //     inx
    x++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     bne loop_c86c2
    if (!(flags & FLAG_Z)) goto loop_c86c2;
    // c86db:
c86db:
    //     lda #0x80
    a = 0x80;
    //     bne c86ff                                                         ; ALWAYS branch
    goto c86ff;

    // c86df:
c86df:
    //     lda tmp0
    a = tmp0;
    //     clc
    flags &= ~FLAG_C;
    //     adc #3
    { uint16_t tmp_ = (uint16_t)a + 3 + ((flags & FLAG_C) ? 1 : 0); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ > 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | ((((a ^ 3) & 0x80) && ((a ^ (uint8_t)tmp_) & 0x80)) ? FLAG_V : 0); }
    //     sta tmp0
    tmp0 = a;
    //     bcs c871d
    if (flags & FLAG_C) goto c871d;
    //     bcc c871f                                                         ; ALWAYS branch
    goto c871f;

    // c86ea:
c86ea:
    //     ldy #0
    y = 0;
    //     jsr draw_char
    draw_char();
    //     and #0x7f
    a &= 0x7f;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     ldx #0
    x = 0;
    //     ldy l0082
    y = l0082;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (y == 0 ? FLAG_Z : 0) | (y & FLAG_N);
    //     bmi c870d
    if (flags & FLAG_N) goto c870d;
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     beq c8703
    if (flags & FLAG_Z) goto c8703;
    //     cmp #0x20 ; ' '
    { uint16_t tmp_ = a - 0x20; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x20 ? FLAG_C : 0); }
    //     beq c8703
    if (flags & FLAG_Z) goto c8703;
    // c86ff:
c86ff:
    //     inc l0083
    l0083++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0083 == 0 ? FLAG_Z : 0) | (l0083 & FLAG_N);
    //     bne c8715
    if (!(flags & FLAG_Z)) goto c8715;
    // c8703:
c8703:
    //     ldy l0083
    y = l0083;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (y == 0 ? FLAG_Z : 0) | (y & FLAG_N);
    //     beq c870d
    if (flags & FLAG_Z) goto c870d;
    //     inc tmp8
    tmp8++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (tmp8 == 0 ? FLAG_Z : 0) | (tmp8 & FLAG_N);
    //     bne c870d
    if (!(flags & FLAG_Z)) goto c870d;
    //     inc tmp9
    tmp9++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (tmp9 == 0 ? FLAG_Z : 0) | (tmp9 & FLAG_N);
    // c870d:
c870d:
    //     stx l0083
    l0083 = x;
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     bne c8715
    if (!(flags & FLAG_Z)) goto c8715;
    //     stx l0082
    l0082 = x;
    // c8715:
c8715:
    //     ora l0082
    a |= l0082;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     sta l0082
    l0082 = a;
    //     inc tmp0
    tmp0++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (tmp0 == 0 ? FLAG_Z : 0) | (tmp0 & FLAG_N);
    //     bne c871f
    if (!(flags & FLAG_Z)) goto c871f;
    // c871d:
c871d:
    //     inc tmp1
    tmp1++;
    // c871f:
c871f:
    //     ldy tmp1
    y = tmp1;
    //     cpy area_end_ptr+1
    { uint16_t tmp_ = y - (uint8_t)(area_end_ptr >> 8); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= (uint8_t)(area_end_ptr >> 8) ? FLAG_C : 0); }
    //     bne c86b8
    if (!(flags & FLAG_Z)) goto c86b8;
    //     ldy tmp0
    y = tmp0;
    //     cpy area_end_ptr
    { uint16_t tmp_ = y - (uint8_t)(area_end_ptr & 0xff); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= (uint8_t)(area_end_ptr & 0xff) ? FLAG_C : 0); }
    //     bne c86b8
    if (!(flags & FLAG_Z)) goto c86b8;
    //     ldx tmp8
    x = tmp8;
    //     ldy tmp9
    y = tmp9;
    //     jsr render_number_to_screen
    render_number_to_screen();
    //     jsr print_inline_string
    //     .ascii " word(s) counted."
    //     .byte 0xff
    cli_putstring(" word(s) counted.\n"); return_to_cli_prompt(); return;

    // l8747:
    //     .byte 0x52
    // l8748:
    //     .byte 0x4a
    // l8749:
    //     .ascii "CELJ"
    //     .byte 0
}
static void format_cmd(void) {
    // Pseudocode: Formats document area by running line-by-line through formatting pipeline

    // ; ***************************************************************************************
    // format_cmd:
    //     jsr parse_marks_from_command
    parse_marks_from_command();
    //     jsr sanitise_area
    sanitise_area();
    //     beq c878b
    if (flags & FLAG_Z) goto c878b;
    //     lda area_start_ptr
    a = (uint8_t)(area_start_ptr & 0xff);
    //     ldy area_start_ptr+1
    y = (uint8_t)((area_start_ptr >> 8) & 0xff);
    //     jsr move_cursor_to_address
    move_cursor_to_address();
    //     jsr sub_caf5f
    sub_caf5f();
    //     lda #0x10
    a = 0x10;
    //     jsr wipe_buffer
    wipe_buffer();
    //     lda current_edit_line_ptr
    a = (uint8_t)(current_edit_line_ptr & 0xff);
    //     sta current_format_line_ptr
    current_format_line_ptr = (current_format_line_ptr & 0xff00) | a;
    //     lda current_edit_line_ptr+1
    a = (uint8_t)((current_edit_line_ptr >> 8) & 0xff);
    //     sta current_format_line_ptr+1
    current_format_line_ptr = (current_format_line_ptr & 0x00ff) | ((uint16_t)a << 8);
    // c876d:
c876d:
    //     jsr sub_c9977
    sub_c9977();
    //     bvs c8791
    if (flags & FLAG_V) goto c8791;
    //     bcs c8787
    if (flags & FLAG_C) goto c8787;
    //     lda #0x2e ; '.'
    a = 0x2e;
    //     jsr bdos_print_char
    cli_putchar(a);
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     ldy current_line_ptr+1
    y = (uint8_t)((current_line_ptr >> 8) & 0xff);
    //     cpy area_end_ptr+1
    { uint16_t tmp_ = y - (uint8_t)(area_end_ptr >> 8); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= (uint8_t)(area_end_ptr >> 8) ? FLAG_C : 0); }
    //     bcc c876d
    if (!(flags & FLAG_C)) goto c876d;
    //     bne c8787
    if (!(flags & FLAG_Z)) goto c8787;
    //     cmp area_end_ptr
    { uint16_t tmp_ = a - (uint8_t)(area_end_ptr & 0xff); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= (uint8_t)(area_end_ptr & 0xff) ? FLAG_C : 0); }
    //     bcc c876d
    if (!(flags & FLAG_C)) goto c876d;
    // c8787:
c8787:
    //     lda #0xff
    a = 0xff;
    //     sta l0012
    l0012 = a;
    // c878b:
c878b:
    //     jsr bdos_print_newline
    cli_putchar('\n');
    //     jmp return_to_cli_prompt
    return_to_cli_prompt(); return;

    // c8791:
c8791:
    //     jsr bdos_print_newline
    cli_putchar('\n');
    //     jmp display_not_enough_memory
    display_not_enough_memory(); return;
}
static void new_cmd(void) {
    // Pseudocode: Creates a new empty document after checking continuous editing state

    // ; ***************************************************************************************
    // new_cmd:
    //     jsr check_not_continuous_editing
    check_not_continuous_editing();
    //     jmp initialise_document
    initialise_document(); return;
}
static void fold_cmd(void) {
    // Pseudocode: Toggles folding on/off and displays current folding status

    // ; ***************************************************************************************
    // fold_cmd:
    //     jsr sub_c8e33
    sub_c8e33();
    //     beq c87b4
    if (flags & FLAG_Z) goto c87b4;
    //     lda #0
    a = 0;
    //     ldx #5
    x = 5;
    //     jsr sub_c976c
    sub_c976c();
    //     bcs c87b4
    if (flags & FLAG_C) goto c87b4;
    //     ldx #0x80
    x = 0x80;
    //     tay
    y = a;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (y == 0 ? FLAG_Z : 0) | (y & FLAG_N);
    //     beq c87b2
    if (flags & FLAG_Z) goto c87b2;
    //     ldx #0
    x = 0;
    // c87b2:
c87b2:
    //     stx folding_flag
    folding_flag = x;
    // c87b4:
c87b4:
    //     jsr print_inline_string
    //     .ascii "Folding "
    //     .byte 0
    cli_putstring("Folding ");

    //     lda folding_flag
    a = folding_flag;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bpl c87cb
    if (!(flags & FLAG_N)) goto c87cb;
    //     jsr print_inline_string
    //     .ascii "off"
    //     .byte 0xff
    cli_putstring("off\n"); return_to_cli_prompt(); return;

    // c87cb:
c87cb:
    //     jsr print_inline_string
    //     .ascii "on"
    //     .byte 0xff
    cli_putstring("on\n"); return_to_cli_prompt(); return;

    // c87d1:
c87d1:
    //     jsr print_inline_string
    //     .ascii "Bad file"
    //     .byte 0xff
    cli_putstring("Bad file\n"); return_to_cli_prompt(); return;
}
static void printer_cmd(void) {
    // Pseudocode: Redirects to print_cmd (printer driver loading code is disabled with #if 0)

    // ; ***************************************************************************************
    // printer_cmd:
    //     jmp print_cmd
    print_cmd(); return;
    // #if 0
    //     // TODO: implement loading printer drivers.
    //     jsr parse_optional_filename_from_command
    //     beq c882f
    //     ;jsr sub_c8849
    //     beq c87d1
    //     lda l050c
    //     ora l050d
    //     bne c87d1
    //     ldy l050b
    //     dey
    //     bmi c8801
    //     bne c87d1
    //     lda l050a
    //     bne c87d1
    // c8801:
    //     lda #<printer_driver_block
    //     sta l0502
    //     lda #>printer_driver_block
    //     sta l0503
    //     lda #osbyte_read_high_order_address
    //     jsr osbyte                                                        ; Read the filing system 'machine high order address'
    //     stx l0504                                                         ; X and Y contain the machine high order address (low, high)
    //     sty l0505
    //     lda #0
    //     sta l0506
    //     lda #0xff
    //     jsr do_osfile_with_buffer
    //     ldx #0
    // loop_c8822:
    //     lda filename_buffer,x
    //     sta printer_driver_name,x
    //     inx
    //     cmp #0x0d
    //     bne loop_c8822
    //     beq c8834                                                         ; ALWAYS branch

    // c882f:
    //     lda #0
    //     sta printer_driver_name
    // c8834:
    //     lda #0
    //     sta microspacing_flag
    // #endif
    // return_8:
    //     rts
}
static void parse_integer_from_command(void) {
    // Pseudocode: Parses a decimal integer from the command input buffer

    // ; ***************************************************************************************
    // parse_integer_from_command:
    //     lda #<(input_buffer)
    a = (uint8_t)((uintptr_t)input_buffer & 0xff);
    //     sta current_format_line_ptr
    current_format_line_ptr = (current_format_line_ptr & 0xff00) | a;
    //     lda #>(input_buffer)
    a = (uint8_t)(((uintptr_t)input_buffer >> 8) & 0xff);
    //     sta current_format_line_ptr+1
    current_format_line_ptr = (current_format_line_ptr & 0x00ff) | ((uint16_t)a << 8);
    //     jsr sub_c8e33
    sub_c8e33();
    //     beq return_8
    if (flags & FLAG_Z) return;
    //     jmp ca6fe
    parse_decimal_number(); return;
}
static void file_not_found_error(void) {
    // Pseudocode: Displays File not found error and returns to CLI

    // ; ***************************************************************************************
    // file_not_found_error:
    //     jsr stop_printing
    stop_printing();
    //     jsr print_inline_string
    //     .ascii "File not found\r"
    //     .byte 0
    cli_putstring("File not found\n");
    //     jmp return_to_cli_prompt
    return_to_cli_prompt(); return;
}
static void name_cmd(void) {
    // Pseudocode: Sets document name from optional filename argument

    // ; ***************************************************************************************
    // name_cmd:
    //     jsr check_not_continuous_editing
    check_not_continuous_editing();
    //     jsr parse_optional_filename_from_command
    parse_optional_filename_from_command();
    //     php
    uint8_t saved_flags = flags;
    //     lda #0
    a = 0;
    //     sta file_edit_flags
    file_edit_flags = a;
    //     plp
    flags = saved_flags;
    //     beq return_9
    if (flags & FLAG_Z) return;

    // MULTIPLE ENTRY POINTS: name_cmd, reset_document_name_after_load
    reset_document_name_after_load();
}
static void reset_document_name_after_load(void) {
    // Pseudocode: Sets file_edit_flags to indicate a document is loaded

    // reset_document_name_after_load:
    //     lda #0x40 ; '@'
    a = 0x40;
    //     sta file_edit_flags
    file_edit_flags = a;
    // fall through to set_document_name_to_filename_buffer
    set_document_name_to_filename_buffer();

    // MULTIPLE ENTRY POINTS: name_cmd, reset_document_name_after_load
}
static void set_document_name_to_filename_buffer(void) {
    // Pseudocode: Copies filename buffer to input filename buffer

    // set_document_name_to_filename_buffer:
    //     ldx #0
    x = 0;
    // loop_c88fa:
loop_c88fa:
    //     lda filename_buffer,x
    a = filename_buffer[x];
    //     sta input_filename,x
    input_filename[x] = a;
    //     inx
    x++;
    //     cmp #0x21
    { uint16_t tmp_ = a - 0x21; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x21 ? FLAG_C : 0); }
    //     bge loop_c88fa
    if (flags & FLAG_C) goto loop_c88fa;
    // return_9:
return_9:
    //     lda #0x0d
    a = 0x0d;
    //     sta input_filename-1, x
    input_filename[x-1] = a;
    //     rts
    return;

    // MULTIPLE ENTRY POINTS: also called directly from edit_cmd
}
static void file_error(void) {
    // Pseudocode: Displays File error and returns to CLI

    // ; ***************************************************************************************
    // zproc file_error
    //     jsr print_inline_string
    //     .ascii "File error"
    //     .byte 0
    cli_putstring("File error");
    //     jmp return_to_cli_prompt
    return_to_cli_prompt(); return;
    // zendproc
}
static void zero_terminate_filename_buffer(void) {
    // zero_terminate_filename_buffer:
    //     ldx #0
    x = 0;
    //     lda #0x0d
    a = 0x0d;
    // zloop:
zloop:
    //     cmp filename_buffer, x
    { uint16_t tmp_ = a - filename_buffer[x]; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= filename_buffer[x] ? FLAG_C : 0); }
    //     zbreakif eq
    if (flags & FLAG_Z) goto zbreak;
    //     inx
    x++;
    //     bne zloop
    goto zloop;
zbreak:
    //     lda #0
    a = 0;
    //     sta filename_buffer, x
    filename_buffer[x] = a;
    //     rts
}
static void sanitise_area(void) {
    uint8_t tmp6, tmp7;
    // sanitise_area:
    //     lda area_start_ptr
    a = (uint8_t)(area_start_ptr & 0xff);
    //     ldx area_start_ptr+1
    x = (uint8_t)(area_start_ptr >> 8);
    //     cpx area_end_ptr+1
    { uint16_t tmp_ = x - (uint8_t)(area_end_ptr >> 8); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= (uint8_t)(area_end_ptr >> 8) ? FLAG_C : 0); }
    //     bcc c8977
    if (!(flags & FLAG_C)) goto c8977;
    //     bne c896b
    if (!(flags & FLAG_Z)) goto c896b;
    //     cmp area_end_ptr
    { uint16_t tmp_ = a - (uint8_t)(area_end_ptr & 0xff); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= (uint8_t)(area_end_ptr & 0xff) ? FLAG_C : 0); }
    //     bcc c8977
    if (!(flags & FLAG_C)) goto c8977;
c896b:
    // c896b:
    //     ldy area_end_ptr
    y = (uint8_t)(area_end_ptr & 0xff);
    //     sty area_start_ptr
    area_start_ptr = (area_start_ptr & 0xff00) | y;
    //     ldy area_end_ptr+1
    y = (uint8_t)(area_end_ptr >> 8);
    //     sty area_start_ptr+1
    area_start_ptr = (area_start_ptr & 0x00ff) | ((uint16_t)y << 8);
    //     stx area_end_ptr+1
    area_end_ptr = (area_end_ptr & 0x00ff) | ((uint16_t)x << 8);
    //     sta area_end_ptr
    area_end_ptr = (area_end_ptr & 0xff00) | a;
c8977:
    // c8977:
    //     lda area_end_ptr
    a = (uint8_t)(area_end_ptr & 0xff);
    //     sec
    flags |= FLAG_C;
    //     sbc area_start_ptr
    { int16_t tmp_ = (int16_t)a - (int16_t)(uint8_t)(area_start_ptr & 0xff); a = (uint8_t)tmp_; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (tmp_ >= 0 ? FLAG_C : 0); }
    //     sta tmp6
    tmp6 = a;
    //     lda area_end_ptr+1
    a = (uint8_t)(area_end_ptr >> 8);
    //     sbc area_start_ptr+1
    { int16_t tmp_ = (int16_t)a - (int16_t)(uint8_t)(area_start_ptr >> 8) - (1 - (flags & FLAG_C)); a = (uint8_t)tmp_; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (tmp_ >= 0 ? FLAG_C : 0); }
    //     sta tmp7
    tmp7 = a;
    //     bne return_10
    if (a != 0) return;
    //     lda tmp6
    a = tmp6;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    // return_10:
    //     rts
}
static void parse_marks_from_command(void) {
    // parse_marks_from_command:
    //     jsr reset_area_to_entire_document
    reset_area_to_entire_document();
    //     jsr parse_mark_from_command
    parse_mark_from_command();
    //     beq return_11
    if (flags & FLAG_Z) return;
    //     sta area_start_ptr
    area_start_ptr = (area_start_ptr & 0xff00) | a;
    //     sty area_start_ptr+1
    area_start_ptr = (area_start_ptr & 0x00ff) | ((uint16_t)y << 8);
    //     jsr parse_mark_from_command
    parse_mark_from_command();
    //     beq return_11
    if (flags & FLAG_Z) return;
    //     sta area_end_ptr
    area_end_ptr = (area_end_ptr & 0xff00) | a;
    //     sty area_end_ptr+1
    area_end_ptr = (area_end_ptr & 0x00ff) | ((uint16_t)y << 8);
    // return_11:
    //     rts
}
static void parse_mark_from_command(void) {
    // parse_mark_from_command:
    //     jsr sub_c8e33
    sub_c8e33();
    //     beq return_12
    if (flags & FLAG_Z) return;
    //     iny
    y++;
    //     sty input_buffer_offset
    input_buffer_offset = y;
    //     jsr lookup_marker
    lookup_marker();
    //     bcs c89b3 / c89b3: jsr print_inline_string ; .ascii "Bad marker" ; .byte 0xff
    if (flags & FLAG_C) { cli_putstring("Bad marker\n"); return_to_cli_prompt(); return; }
    //     beq c89c1 / c89c1: jsr print_inline_string ; .ascii "Marker not set" ; .byte 0xff
    if (flags & FLAG_Z) { cli_putstring("Marker not set\n"); return_to_cli_prompt(); return; }
    //     lda markers_array,x
    a = (uint8_t)(markers_array[x] & 0xff);
    //     ldy markers_array+1,x
    y = (uint8_t)(markers_array[x] >> 8);
    // return_12:
    //     rts
}
static void sub_c89d3(void) {
    // sub_c89d3:
    //     lda area_start_ptr
    a = (uint8_t)(area_start_ptr & 0xff);
    //     sta tmp4
    tmp4 = a;
    //     lda area_start_ptr+1
    a = (uint8_t)(area_start_ptr >> 8);
    //     sta tmp5
    tmp5 = a;
    //     jsr adjust_pointers
    adjust_pointers();
    //     lda tmp4
    a = tmp4;
    //     ldy tmp5
    y = tmp5;
    //     jmp cac78
    cac78(); return;
}
static void display_document_file_state(void) {
    // display_document_file_state:
    //     jsr stop_printing
    stop_printing();
    //     jsr print_inline_string
    //     .ascii "Editing "
    //     .byte 0
    cli_putstring("Editing ");

    //     lda file_edit_flags
    a = file_edit_flags;
    //     beq c8a21
    if (a == 0) goto c8a21;
    //     ldy #0
    y = 0;
loop_c89fa:
    // loop_c89fa:
    //     lda input_filename,y
    a = input_filename[y];
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     beq c8a07
    if (flags & FLAG_Z) goto c8a07;
    //     jsr bdos_print_char
    cli_putchar(a);
    //     iny
    y++;
    //     bne loop_c89fa
    if (y != 0) goto loop_c89fa;
c8a07:
    // c8a07:
    //     bit file_edit_flags
    { uint8_t tmp_ = a & file_edit_flags; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ == 0 ? FLAG_Z : 0) | (file_edit_flags & (FLAG_N|FLAG_V)); }
    //     bvs c8a19
    if (flags & FLAG_V) goto c8a19;
    //     jsr print_inline_string
    //     .ascii " to "
    //     .byte 0
    cli_putstring(" to ");

    //     ldy #0
    y = 0;
loop_c8a15:
    // loop_c8a15:
    //     lda output_filename,y
    a = output_filename[y];
    //     iny
    y++;
c8a19:
    // c8a19:
    //     jsr bdos_print_char
    cli_putchar(a);
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     bne loop_c8a15
    if (!(flags & FLAG_Z)) goto loop_c8a15;
    //     rts
    return;

c8a21:
    // c8a21:
    //     jsr print_inline_string
    //     .ascii "No File\r"
    //     .byte 0
    cli_putstring("No File\n");
    //     rts
}
static void sub_c8a4f(void) {
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
    if (x != 0) goto c8a87;
c8a5b:
    // c8a5b:
    //     lda header_text_maybe,x
    a = header_text_maybe[x];
    //     cmp #1
    { uint16_t tmp_ = a - 1; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 1 ? FLAG_C : 0); }
    //     bne c8a6c
    if (!(flags & FLAG_Z)) goto c8a6c;
    //     lda l0081
    a = l0081;
    //     cmp l0049
    { uint16_t tmp_ = a - l0049; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= l0049 ? FLAG_C : 0); }
    //     bcs c8a86
    if (flags & FLAG_C) goto c8a86;
    //     inc l0081
    l0081++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0081 == 0 ? FLAG_Z : 0) | (l0081 & FLAG_N);
    //     bne c8a84
    if (!(flags & FLAG_Z)) goto c8a84;
c8a6c:
    // c8a6c:
    //     cmp #0x20 ; ' '
    { uint16_t tmp_ = a - 0x20; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x20 ? FLAG_C : 0); }
    //     bne c8a84
    if (!(flags & FLAG_Z)) goto c8a84;
    //     cpy l0048
    { uint16_t tmp_ = y - l0048; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= l0048 ? FLAG_C : 0); }
    //     bcs c8a84
    if (flags & FLAG_C) goto c8a84;
loop_c8a74:
    // loop_c8a74:
    //     lda output_buffer,y
    a = output_buffer[y];
    //     php
    { uint8_t saved_flags_ = flags;
    //     iny
    y++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (y == 0 ? FLAG_Z : 0) | (y & FLAG_N);
    //     plp
    flags = saved_flags_; }
    //     beq c8a86
    if (flags & FLAG_Z) goto c8a86;
    //     inc l0082
    l0082++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0082 == 0 ? FLAG_Z : 0) | (l0082 & FLAG_N);
    //     cpy l0048
    { uint16_t tmp_ = y - l0048; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= l0048 ? FLAG_C : 0); }
    //     bcc loop_c8a74
    if (!(flags & FLAG_C)) goto loop_c8a74;
    //     dec l0082
    l0082--;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0082 == 0 ? FLAG_Z : 0) | (l0082 & FLAG_N);
c8a84:
    // c8a84:
    //     inc l0082
    l0082++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0082 == 0 ? FLAG_Z : 0) | (l0082 & FLAG_N);
c8a86:
    // c8a86:
    //     inx
    x++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
c8a87:
    // c8a87:
    //     cpx l004a
    { uint16_t tmp_ = x - l004a; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= l004a ? FLAG_C : 0); }
    //     bcc c8a5b
    if (!(flags & FLAG_C)) goto c8a5b;
    //     lda doc_ptr2+0
    a = (uint8_t)(doc_ptr2 & 0xff);
    //     sec
    flags |= FLAG_C;
    //     sbc ptr2
    { int16_t tmp_ = (int16_t)a - (int16_t)(uint8_t)(ptr2 & 0xff); a = (uint8_t)tmp_; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (tmp_ >= 0 ? FLAG_C : 0); }
    //     sta input_buffer_offset+1
    //     lda doc_ptr2+1
    a = (uint8_t)(doc_ptr2 >> 8);
    //     sbc ptr2+1
    { int16_t tmp_ = (int16_t)a - (int16_t)(uint8_t)(ptr2 >> 8) - (1 - (flags & FLAG_C)); a = (uint8_t)tmp_; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (tmp_ >= 0 ? FLAG_C : 0); }
    //     sta l0081
    l0081 = a;
    //     ldx l0082
    x = l0082;
    //     tay
    y = a;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (y == 0 ? FLAG_Z : 0) | (y & FLAG_N);
    //     bne c8aa3
    if (!(flags & FLAG_Z)) goto c8aa3;
    //     cpx input_buffer_offset+1
    { uint16_t tmp_ = x - 0; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= 0 ? FLAG_C : 0); }
    //     bcc c8aa3
    if (!(flags & FLAG_C)) goto c8aa3;
    //     ldx input_buffer_offset+1
    x = 0;
c8aa3:
    // c8aa3:
    //     txa
    a = x;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     clc
    flags &= ~FLAG_C;
    //     adc ptr2
    { uint16_t sum = (uint16_t)a + (uint8_t)(ptr2 & 0xff); a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)sum == 0 ? FLAG_Z : 0) | ((uint8_t)sum & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     sta tmp4
    tmp4 = a;
    //     lda ptr2+1
    a = (uint8_t)(ptr2 >> 8);
    //     adc #0
    { uint16_t sum = (uint16_t)a + (flags & FLAG_C); a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)sum == 0 ? FLAG_Z : 0) | ((uint8_t)sum & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     sta tmp5
    tmp5 = a;
    //     lda l0082
    a = l0082;
    //     sec
    flags |= FLAG_C;
    //     sbc input_buffer_offset+1
    { int16_t tmp_ = (int16_t)a - (int16_t)0; a = (uint8_t)tmp_; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (tmp_ >= 0 ? FLAG_C : 0); }
    //     sta tmp6
    tmp6 = a;
    //     lda #0
    a = 0;
    //     sbc l0081
    { int16_t tmp_ = (int16_t)a - (int16_t)l0081 - (1 - (flags & FLAG_C)); a = (uint8_t)tmp_; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (tmp_ >= 0 ? FLAG_C : 0); }
    //     sta tmp7
    tmp7 = a;
    //     bmi c8aca
    if (flags & FLAG_N) goto c8aca;
    //     ora tmp6
    a |= tmp6;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq c8ada
    if (flags & FLAG_Z) goto c8ada;
    //     sta tmp6
    tmp6 = a;
    //     jsr make_space_for_insertion
    make_space_for_insertion();
    //     bcc c8ada
    if (!(flags & FLAG_C)) goto c8ada;
    //     rts
    return;

c8aca:
    // c8aca:
    //     lda #0
    a = 0;
    //     sec
    flags |= FLAG_C;
    //     sbc tmp6
    { int16_t tmp_ = (int16_t)a - (int16_t)tmp6; a = (uint8_t)tmp_; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (tmp_ >= 0 ? FLAG_C : 0); }
    //     sta tmp6
    tmp6 = a;
    //     lda #0
    a = 0;
    //     sbc tmp7
    { int16_t tmp_ = (int16_t)a - (int16_t)tmp7 - (1 - (flags & FLAG_C)); a = (uint8_t)tmp_; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (tmp_ >= 0 ? FLAG_C : 0); }
    //     sta tmp7
    tmp7 = a;
    //     jsr adjust_pointers
    adjust_pointers();
c8ada:
    // c8ada:
    //     ldy #0
    y = 0;
    //     sty l0081
    l0081 = y;
    //     bit print_xpos
    { uint8_t tmp_ = a & print_xpos; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ == 0 ? FLAG_Z : 0) | (print_xpos & (FLAG_N|FLAG_V)); }
    //     bmi c8b11
    if (flags & FLAG_N) goto c8b11;
    //     ldx input_buffer_offset+1
    x = 0;
loop_c8ae4:
    // loop_c8ae4:
    //     lda (ptr2),y
    a = ram[ptr2 + y];
    //     iny
    y++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (y == 0 ? FLAG_Z : 0) | (y & FLAG_N);
    //     jsr is_uppercase
    is_uppercase();
    //     bcc c8af3
    if (!(flags & FLAG_C)) goto c8af3;
    //     ror print_xpos
    { uint8_t old_carry_ = (flags & FLAG_C) ? 0x80 : 0; flags = (flags & ~FLAG_C) | (print_xpos & 1); print_xpos = (print_xpos >> 1) | old_carry_; flags = (flags & ~(FLAG_Z|FLAG_N)) | (print_xpos == 0 ? FLAG_Z : 0) | (print_xpos & FLAG_N); }
    //     dex
    x--;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     bne loop_c8ae4
    if (!(flags & FLAG_Z)) goto loop_c8ae4;
    //     beq c8b11
    goto c8b11;

c8af3:
    // c8af3:
    //     pha
    { uint8_t saved_a_ = a;
    //     lda #0
    a = 0;
    //     sta print_xpos
    print_xpos = a;
    //     pla
    a = saved_a_; }
    //     and #0x20 ; ' '
    a &= 0x20;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bne c8b11
    if (!(flags & FLAG_Z)) goto c8b11;
    //     inc l0081
    l0081++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0081 == 0 ? FLAG_Z : 0) | (l0081 & FLAG_N);
    //     dex
    x--;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     beq c8b0d
    if (flags & FLAG_Z) goto c8b0d;
    //     lda (ptr2),y
    a = ram[ptr2 + y];
    //     jsr is_uppercase
    is_uppercase();
    //     bcs c8b11
    if (flags & FLAG_C) goto c8b11;
    //     and #0x20 ; ' '
    a &= 0x20;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bne c8b11
    if (!(flags & FLAG_Z)) goto c8b11;
c8b0d:
    // c8b0d:
    //     dec l0081
    l0081--;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0081 == 0 ? FLAG_Z : 0) | (l0081 & FLAG_N);
    //     dec l0081
    l0081--;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0081 == 0 ? FLAG_Z : 0) | (l0081 & FLAG_N);
c8b11:
    // c8b11:
    //     ldx #0
    x = 0;
    //     stx l0082
    l0082 = x;
    //     stx l0083
    l0083 = x;
    //     ldx #0x14
    x = 0x14;
    //     stx input_buffer_offset+1
    //     ldx l007a
    x = l007a;
    //     bne c8b6b
    if (x != 0) goto c8b6b;
c8b1f:
    // c8b1f:
    //     lda header_text_maybe,x
    a = header_text_maybe[x];
    //     stx l0084
    l0084 = x;
    //     cmp #0x20 ; ' '
    { uint16_t tmp_ = a - 0x20; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x20 ? FLAG_C : 0); }
    //     bne c8b38
    if (!(flags & FLAG_Z)) goto c8b38;
    //     ldy input_buffer_offset+1
    y = 0;
    //     cpy l0048
    { uint16_t tmp_ = y - l0048; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= l0048 ? FLAG_C : 0); }
    //     bcs c8b47
    if (flags & FLAG_C) goto c8b47;
    //     inc input_buffer_offset+1
    /* high byte inc removed */
    //     lda output_buffer,y
    a = output_buffer[y];
    //     beq c8b6a
    if (a == 0) goto c8b6a;
    //     dex
    x--;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     bcc c8b47
    goto c8b47;

c8b38:
    // c8b38:
    //     cmp #1
    { uint16_t tmp_ = a - 1; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 1 ? FLAG_C : 0); }
    //     bne c8b47
    if (!(flags & FLAG_Z)) goto c8b47;
    //     ldy l0082
    y = l0082;
    //     cpy l0049
    { uint16_t tmp_ = y - l0049; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= l0049 ? FLAG_C : 0); }
    //     bcs c8b6a
    if (flags & FLAG_C) goto c8b6a;
    //     lda output_buffer,y
    a = output_buffer[y];
    //     inc l0082
    l0082++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0082 == 0 ? FLAG_Z : 0) | (l0082 & FLAG_N);
c8b47:
    // c8b47:
    //     cmp #2
    { uint16_t tmp_ = a - 2; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 2 ? FLAG_C : 0); }
    //     bne c8b4d
    if (!(flags & FLAG_Z)) goto c8b4d;
    //     lda #0x20 ; ' '
    a = 0x20;
c8b4d:
    // c8b4d:
    //     bit folding_flag
    { uint8_t tmp_ = a & folding_flag; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ == 0 ? FLAG_Z : 0) | (folding_flag & (FLAG_N|FLAG_V)); }
    //     bmi c8b64
    if (flags & FLAG_N) goto c8b64;
    //     ldy print_xpos
    y = print_xpos;
    //     bne c8b64
    if (y != 0) goto c8b64;
    //     jsr is_uppercase
    is_uppercase();
    //     bcs c8b64
    if (flags & FLAG_C) goto c8b64;
    //     ora #0x20 ; ' '
    a |= 0x20;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     ldy l0081
    y = l0081;
    //     beq c8b64
    if (y == 0) goto c8b64;
    //     dec l0081
    l0081--;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0081 == 0 ? FLAG_Z : 0) | (l0081 & FLAG_N);
    //     and #0xdf
    a &= 0xdf;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
c8b64:
    // c8b64:
    //     ldy l0083
    y = l0083;
    //     sta (ptr2),y
    ram[ptr2 + y] = a;
    //     inc l0083
    l0083++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0083 == 0 ? FLAG_Z : 0) | (l0083 & FLAG_N);
c8b6a:
    // c8b6a:
    //     inx
    x++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
c8b6b:
    // c8b6b:
    //     cpx l004a
    { uint16_t tmp_ = x - l004a; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= l004a ? FLAG_C : 0); }
    //     bcc c8b1f
    if (!(flags & FLAG_C)) goto c8b1f;
    //     lda ptr2
    a = (uint8_t)(ptr2 & 0xff);
    //     ldy ptr2+1
    y = (uint8_t)(ptr2 >> 8);
    //     jsr cac78
    cac78();
    //     clc
    flags &= ~FLAG_C;
    //     rts
}
static void c8b78(void) {
    // c8b78:
    //     lda #0xff
    a = 0xff;
    //     rts
}
static void c8b7b(void) {
    // c8b7b:
    //     lda l007a
    a = l007a;
    //     beq c8b78
    if (a == 0) { c8b78(); return; }
    //     lda #0x14
    a = 0x14;
    //     sta l0048
    l0048 = a;
    //     ldx #0
    x = 0;
    //     stx l0049
    l0049 = x;
    //     stx l0081
    l0081 = x;
    //     lda doc_ptr2+0
    a = (uint8_t)(doc_ptr2 & 0xff);
    //     sta tmp8
    tmp8 = a;
    //     lda doc_ptr2+1
    a = (uint8_t)(doc_ptr2 >> 8);
    //     sta tmp9
    tmp9 = a;
c8b91:
    // c8b91:
    //     lda tmp9
    a = tmp9;
    //     cmp doc_ptr3+1
    { uint16_t tmp_ = a - (uint8_t)(doc_ptr3 >> 8); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= (uint8_t)(doc_ptr3 >> 8) ? FLAG_C : 0); }
    //     bcc c8b9f
    if (!(flags & FLAG_C)) goto c8b9f;
    //     bne c8b78
    if (!(flags & FLAG_Z)) { c8b78(); return; }
    //     lda tmp8
    a = tmp8;
    //     cmp doc_ptr3+0
    { uint16_t tmp_ = a - (uint8_t)(doc_ptr3 & 0xff); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= (uint8_t)(doc_ptr3 & 0xff) ? FLAG_C : 0); }
    //     bcs c8b78
    if (flags & FLAG_C) { c8b78(); return; }
c8b9f:
    // c8b9f:
    //     ldy #0
    y = 0;
    //     lda (tmp8),y
    a = ram[((uint16_t)tmp9 << 8) | tmp8];
    //     jsr check_for_command_prefix
    check_for_command_prefix();
    //     bne c8bb7
    if (!(flags & FLAG_Z)) goto c8bb7;
    //     lda tmp8
    a = tmp8;
    //     clc
    flags &= ~FLAG_C;
    //     adc #3
    { uint16_t sum = (uint16_t)a + 3; a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)sum == 0 ? FLAG_Z : 0) | ((uint8_t)sum & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     sta doc_ptr2+0
    doc_ptr2 = (doc_ptr2 & 0xff00) | a;
    //     lda tmp9
    a = tmp9;
    //     sta doc_ptr2+1
    doc_ptr2 = (doc_ptr2 & 0x00ff) | ((uint16_t)a << 8);
    //     bcc c8b7b
    if (!(flags & FLAG_C)) { c8b7b(); return; }
    //     bcs c8bdf
    goto c8bdf;

c8bb7:
    // c8bb7:
    //     jsr sub_c8c5f
    sub_c8c5f();
    //     sta l0083
    l0083 = a;
c8bbc:
    // c8bbc:
    //     iny
    y++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (y == 0 ? FLAG_Z : 0) | (y & FLAG_N);
    //     lda (tmp8),y
    a = ram[((uint16_t)tmp9 << 8 | tmp8) + y];
    //     beq c8bdb
    if (a == 0) goto c8bdb;
    //     jsr check_for_command_prefix
    check_for_command_prefix();
    //     beq c8bdb
    if (flags & FLAG_Z) goto c8bdb;
    //     lda header_text_maybe,x
    a = header_text_maybe[x];
    //     cmp #0x20 ; ' '
    { uint16_t tmp_ = a - 0x20; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x20 ? FLAG_C : 0); }
    //     beq c8bf7
    if (flags & FLAG_Z) goto c8bf7;
    //     cmp #1
    { uint16_t tmp_ = a - 1; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 1 ? FLAG_C : 0); }
    //     beq c8be3
    if (flags & FLAG_Z) goto c8be3;
    //     cmp #2
    { uint16_t tmp_ = a - 2; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 2 ? FLAG_C : 0); }
    //     bne c8bd7
    if (!(flags & FLAG_Z)) goto c8bd7;
    //     lda #0x20 ; ' '
    a = 0x20;
c8bd7:
    // c8bd7:
    //     cmp l0083
    { uint16_t tmp_ = a - l0083; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= l0083 ? FLAG_C : 0); }
    //     beq c8c33
    if (flags & FLAG_Z) goto c8c33;
c8bdb:
    // c8bdb:
    //     inc doc_ptr2+0
    doc_ptr2 = (doc_ptr2 & 0xff00) | ((uint8_t)(doc_ptr2 & 0xff) + 1);
    //     bne c8b7b
    if ((uint8_t)(doc_ptr2 & 0xff) != 0) { c8b7b(); return; }
c8bdf:
    // c8bdf:
    //     inc doc_ptr2+1
    doc_ptr2 = (doc_ptr2 & 0x00ff) | ((uint16_t)((uint8_t)(doc_ptr2 >> 8) + 1) << 8);
    //     bne c8b7b
    if ((uint8_t)(doc_ptr2 >> 8) != 0) { c8b7b(); return; }
    return;
c8be3:
    // c8be3:
    //     lda l0083
    a = l0083;
    //     stx l0084
    l0084 = x;
    //     ldx l0049
    x = l0049;
    //     cpx #0x14
    { uint16_t tmp_ = x - 0x14; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= 0x14 ? FLAG_C : 0); }
    //     bcs c8bf2
    if (flags & FLAG_C) goto c8bf2;
    //     sta output_buffer,x
    output_buffer[x] = a;
    //     inc l0049
    l0049++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0049 == 0 ? FLAG_Z : 0) | (l0049 & FLAG_N);
c8bf2:
    // c8bf2:
    //     ldx l0084
    x = l0084;
    //     jmp c8c33
    goto c8c33;

c8bf7:
    // c8bf7:
    //     stx l0084
    l0084 = x;
    //     lda l0083
    a = l0083;
    //     cmp #0x20 ; ' '
    { uint16_t tmp_ = a - 0x20; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x20 ? FLAG_C : 0); }
    //     beq c8c23
    if (flags & FLAG_Z) goto c8c23;
    //     cmp #9
    { uint16_t tmp_ = a - 9; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 9 ? FLAG_C : 0); }
    //     beq c8c23
    if (flags & FLAG_Z) goto c8c23;
    //     cmp #0x0b
    { uint16_t tmp_ = a - 0x0b; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0b ? FLAG_C : 0); }
    //     beq c8c23
    if (flags & FLAG_Z) goto c8c23;
    //     cmp #0x1a
    { uint16_t tmp_ = a - 0x1a; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x1a ? FLAG_C : 0); }
    //     beq c8c23
    if (flags & FLAG_Z) goto c8c23;
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     beq c8c23
    if (flags & FLAG_Z) goto c8c23;
    //     lda l0081
    a = l0081;
    //     beq c8bdb
    if (a == 0) goto c8bdb;
    //     jsr sub_c8c51
    sub_c8c51();
    //     lda #0
    a = 0;
    //     sta l0081
    l0081 = a;
    //     ldx l0084
    x = l0084;
    //     inx
    x++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     cpx l007a
    { uint16_t tmp_ = x - l007a; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= l007a ? FLAG_C : 0); }
    //     bcc c8bbc
    if (!(flags & FLAG_C)) goto c8bbc;
    //     bcs c8c3e
    goto c8c3e;

c8c23:
    // c8c23:
    //     jsr sub_c8c53
    sub_c8c53();
    //     ldx l0084
    x = l0084;
    //     sta l0081
    l0081 = a;
loop_c8c2a:
    // loop_c8c2a:
    //     inc tmp8
    tmp8++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (tmp8 == 0 ? FLAG_Z : 0) | (tmp8 & FLAG_N);
    //     bne c8c30
    if (!(flags & FLAG_Z)) goto c8c30;
    //     inc tmp9
    tmp9++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (tmp9 == 0 ? FLAG_Z : 0) | (tmp9 & FLAG_N);
c8c30:
    // c8c30:
    //     jmp c8b91
    goto c8b91;

c8c33:
    // c8c33:
    //     inx
    x++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     cpx l007a
    { uint16_t tmp_ = x - l007a; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= l007a ? FLAG_C : 0); }
    //     bcc loop_c8c2a
    if (!(flags & FLAG_C)) goto loop_c8c2a;
    //     inc tmp8
    tmp8++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (tmp8 == 0 ? FLAG_Z : 0) | (tmp8 & FLAG_N);
    //     bne c8c3e
    if (!(flags & FLAG_Z)) goto c8c3e;
    //     inc tmp9
    tmp9++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (tmp9 == 0 ? FLAG_Z : 0) | (tmp9 & FLAG_N);
c8c3e:
    // c8c3e:
    //     lda doc_ptr2+0
    a = (uint8_t)(doc_ptr2 & 0xff);
    //     ldy doc_ptr2+1
    y = (uint8_t)(doc_ptr2 >> 8);
    //     ldx tmp8
    x = tmp8;
    //     stx doc_ptr2+0
    doc_ptr2 = (doc_ptr2 & 0xff00) | x;
    //     ldx tmp9
    x = tmp9;
    //     stx doc_ptr2+1
    doc_ptr2 = (doc_ptr2 & 0x00ff) | ((uint16_t)x << 8);
    //     sta ptr2
    ptr2 = (ptr2 & 0xff00) | a;
    //     sty ptr2+1
    ptr2 = (ptr2 & 0x00ff) | ((uint16_t)y << 8);
    //     ldx #0
    x = 0;
    //     rts
}
static void sub_c8c53(void) {
    // sub_c8c53:
    //     ldx l0048
    x = l0048;
    //     cpx #MAX_LINE_LENGTH
    //     bcs return_13
    if (x >= MAX_LINE_LENGTH) return;
    //     sta output_buffer,x
    output_buffer[x] = a;
    //     inc l0048
    l0048++;
    // return_13:
    //     rts
}
static void sub_c8c51(void) {
    // sub_c8c51:
    //     lda #0
    a = 0;
    sub_c8c53();
}
static void sub_c8c5f(void) {
    // sub_c8c5f: converts to uppercase only if folding flag is clear
    //     bit folding_flag
    if (folding_flag & FLAG_N) return;
    //     falls through to to_uppercase
    to_uppercase(); return;
}
static void to_uppercase(void) {
    // to_uppercase:
    //     jsr is_uppercase
    is_uppercase();
    //     bcs return_14
    if (flags & FLAG_C) return;
    //     and #0xdf
    a &= 0xdf;
    // return_14:
    //     rts
    return;
}
static void is_uppercase(void) {
    // is_uppercase:
    //     cmp #0x41 ; 'A'
    { uint16_t tmp_ = a - 0x41; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x41 ? FLAG_C : 0); }
    //     bcc c8c7a
    if (!(flags & FLAG_C)) { flags |= FLAG_C; return; }
    //     cmp #0x5b ; '['
    { uint16_t tmp_ = a - 0x5b; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x5b ? FLAG_C : 0); }
    //     bcc return_15
    if (!(flags & FLAG_C)) return;
    //     cmp #0x61 ; 'a'
    { uint16_t tmp_ = a - 0x61; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x61 ? FLAG_C : 0); }
    //     bcc c8c7a
    if (!(flags & FLAG_C)) { flags |= FLAG_C; return; }
    //     cmp #0x7b ; '{'
    { uint16_t tmp_ = a - 0x7b; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x7b ? FLAG_C : 0); }
    // return_15:
    //     rts
    // c8c7a:
    //     sec
    //     rts
}
static void sub_c8c7c(void) {
    // sub_c8c7c:
    //     lda area_start_ptr
    a = (uint8_t)(area_start_ptr & 0xff);
    //     sta doc_ptr2+0
    doc_ptr2 = (doc_ptr2 & 0xff00) | a;
    //     lda area_start_ptr+1
    a = (uint8_t)(area_start_ptr >> 8);
    //     sta doc_ptr2+1
    doc_ptr2 = (doc_ptr2 & 0x00ff) | ((uint16_t)a << 8);
    //     lda area_end_ptr
    a = (uint8_t)(area_end_ptr & 0xff);
    //     sta doc_ptr3+0
    doc_ptr3 = (doc_ptr3 & 0xff00) | a;
    //     lda area_end_ptr+1
    a = (uint8_t)(area_end_ptr >> 8);
    //     sta doc_ptr3+1
    doc_ptr3 = (doc_ptr3 & 0x00ff) | ((uint16_t)a << 8);
    //     rts
}
static void read_block_from_file(void) {
    // read_block_from_file:
    //     lda #0
    a = 0;
    //     sta l0083
    l0083 = a;
    //     sta l0084
    l0084 = a;
c8c95:
    // c8c95:
    //     jsr get_byte_from_file
    get_byte_from_file();
    //     beq c8cf2
    if (flags & FLAG_Z) goto c8cf2;
    //     ldy #0
    y = 0;
    //     cmp #0x7f
    { uint16_t tmp_ = a - 0x7f; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x7f ? FLAG_C : 0); }
    //     bcc c8caf
    if (!(flags & FLAG_C)) goto c8caf;
    //     ldx l0084
    x = l0084;
    //     bne c8c95
    if (x != 0) goto c8c95;
    //     jsr check_for_command_prefix
    check_for_command_prefix();
    //     bne c8c95
    if (!(flags & FLAG_Z)) goto c8c95;
    //     ldx #0xfd
    x = 0xfd;
    //     stx l0083
    l0083 = x;
c8caf:
    // c8caf:
    //     cmp #0x20 ; ' '
    { uint16_t tmp_ = a - 0x20; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x20 ? FLAG_C : 0); }
    //     bcs c8cc8
    if (flags & FLAG_C) goto c8cc8;
    //     jsr check_for_control_code
    check_for_control_code();
    //     beq c8cc8
    if (flags & FLAG_Z) goto c8cc8;
    //     cmp #0x1a
    { uint16_t tmp_ = a - 0x1a; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x1a ? FLAG_C : 0); }
    //     beq c8cc8
    if (flags & FLAG_Z) goto c8cc8;
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     beq c8cc8
    if (flags & FLAG_Z) goto c8cc8;
    //     cmp #0x0b
    { uint16_t tmp_ = a - 0x0b; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0b ? FLAG_C : 0); }
    //     beq c8cc8
    if (flags & FLAG_Z) goto c8cc8;
    //     cmp #9
    { uint16_t tmp_ = a - 9; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 9 ? FLAG_C : 0); }
    //     bne c8c95
    if (!(flags & FLAG_Z)) goto c8c95;
c8cc8:
    // c8cc8:
    //     ldx #1
    x = 1;
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     beq c8cdb
    if (flags & FLAG_Z) goto c8cdb;
    //     dex
    x--;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     ldy l0083
    y = l0083;
    //     cpy #0x84
    { uint16_t tmp_ = y - 0x84; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= 0x84 ? FLAG_C : 0); }
    //     bne c8cdb
    if (!(flags & FLAG_Z)) goto c8cdb;
    //     pha
    { uint8_t saved_a_ = a;
    //     jsr write_cr_to_memory
    write_cr_to_memory();
    //     pla
    a = saved_a_; }
    //     inx
    x++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
c8cdb:
    // c8cdb:
    //     inc l0083
    l0083++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0083 == 0 ? FLAG_Z : 0) | (l0083 & FLAG_N);
    //     jsr write_byte_to_memory
    write_byte_to_memory();
    //     txa
    a = x;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     beq c8c95
    if (flags & FLAG_Z) goto c8c95;
    //     lda tmp1
    a = tmp1;
    //     cmp l0081
    { uint16_t tmp_ = a - l0081; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= l0081 ? FLAG_C : 0); }
    //     bcc c8c95
    if (!(flags & FLAG_C)) goto c8c95;
    //     bne c8cf1
    if (!(flags & FLAG_Z)) goto c8cf1;
    //     lda tmp0
    a = tmp0;
    //     cmp input_buffer_offset+1
    { uint16_t tmp_ = a - l0082; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= l0082 ? FLAG_C : 0); }
    //     bcc c8c95
    if (!(flags & FLAG_C)) goto c8c95;
c8cf1:
    // c8cf1:
    //     clc
    flags &= ~FLAG_C;
c8cf2:
    // c8cf2:
    //     php
    { uint8_t saved_flags_ = flags;
    //     lda l0084
    a = l0084;
    //     beq c8cfa
    if (a == 0) goto c8cfa;
    //     jsr write_cr_to_memory
    write_cr_to_memory();
c8cfa:
    // c8cfa:
    //     plp
    flags = saved_flags_; }
    //     lda l0082
    a = l0082;
    flags = (flags & ~(FLAG_Z | FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     rts
}
static void write_byte_to_memory(void) {
    // write_byte_to_memory:
    //     ldy #0
    y = 0;
    //     sta (tmp0),y
    ram[((uint16_t)tmp1 << 8) | tmp0] = a;
    //     inc tmp0
    tmp0++;
    //     bne c8d0a
    //     inc tmp1
    if (tmp0 == 0) tmp1++;
    // c8d0a:
    //     sta l0084
    l0084 = a;
    //     cmp #0x0d
    //     bne return_16
    if (a != 0x0d) return;
    //     sty l0084
    l0084 = y;
    //     sty l0083
    l0083 = y;
    // return_16:
    //     rts
}
static void write_cr_to_memory(void) {
    // write_cr_to_memory:
    //     lda #0x0d
    a = 0x0d;
    write_byte_to_memory();
}
static void read_next_chunk_from_input_file(void) {
    // read_next_chunk_from_input_file:
    //     jsr sub_c8da2
    sub_c8da2();
    x = 0;
    select_file();
    //     jsr read_block_from_file
    read_block_from_file();
    //     php
    //     beq c8d39
    //     bcc c8d39
    if (!(flags & FLAG_Z) && (flags & FLAG_C)) {
        //     inc input_file_empty_flag
        input_file_empty_flag++;
    }
    // c8d39:
    //     lda #0
    a = 0;
    //     tay                                                               ; Y=0x00
    y = 0;
    //     sta (tmp0),y
    ram[((uint16_t)tmp1 << 8 | tmp0) + y] = a;
    //     lda tmp0
    a = tmp0;
    //     sta top
    top = (top & 0xff00) | a;
    //     lda tmp1
    a = tmp1;
    //     sta top+1
    top = (top & 0x00ff) | ((uint16_t)a << 8);
    //     plp
    //     rts
}
static void read_first_chunk_from_input_file(void) {
    // read_first_chunk_from_input_file:
    //     lda page
    a = (uint8_t)(page & 0xff);
    //     ldy page+1
    y = (uint8_t)((page >> 8) & 0xff);
    //     jmp read_next_chunk_from_input_file
    read_next_chunk_from_input_file();
}
static void write_area_to_file(void) {
    // Pseudocode: Writes document area range to output file byte by byte
    uint8_t tmp8, tmp9;

    // ; Does not include trailing zero!
    // write_area_to_file:
    //     jsr sanitise_area
    sanitise_area();
    //     beq return_17
    if (flags & FLAG_Z) return;

    //     lda area_start_ptr
    a = (uint8_t)(area_start_ptr & 0xff);
    //     sta tmp8
    tmp8 = a;
    //     lda area_start_ptr+1
    a = (uint8_t)(area_start_ptr >> 8);
    //     sta tmp9
    tmp9 = a;

    //     zrepeat
    do {
    //         ldy #0
    y = 0;
    //         lda (tmp8),y
    a = ram[((uint16_t)tmp9 << 8) | tmp8];
    //         jsr put_byte_to_file
    put_byte_to_file();
    //         inc tmp8
    tmp8++;
    //         zif eq
    if (tmp8 == 0) {
    //             inc tmp9
    tmp9++;
    //         zendif
    }

    //         lda tmp9
    a = tmp9;
    //         cmp area_end_ptr+1
    { uint16_t tmp_ = a - (uint8_t)(area_end_ptr >> 8); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= (uint8_t)(area_end_ptr >> 8) ? FLAG_C : 0); }
    //         zif eq
    if (flags & FLAG_Z) {
    //             lda tmp8
    a = tmp8;
    //             cmp area_end_ptr
    { uint16_t tmp_ = a - (uint8_t)(area_end_ptr & 0xff); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= (uint8_t)(area_end_ptr & 0xff) ? FLAG_C : 0); }
    //         zendif
    }
    //     zuntil eq
    } while (!(flags & FLAG_Z));
    // return_17:
    //     rts
}
static void compute_space_common(void) {
    // c8daf:
    //     sta tmp0
    tmp0 = a;
    //     sty tmp1
    tmp1 = y;
    //     jsr compute_bytes_free
    compute_bytes_free();
    //     stx tmp6
    tmp6 = x;
    //     sty tmp7
    tmp7 = y;
    //     lsr tmp9; ror tmp8; lsr tmp9; ror tmp8
    {
        uint16_t t = ((uint16_t)tmp9 << 8) | tmp8;
        t >>= 2;
        tmp9 = (uint8_t)(t >> 8);
        tmp8 = (uint8_t)(t & 0xff);
    }
    //     lda tmp9; cmp #4
    if (tmp9 >= 4) {
        //     lda #4; sta tmp9; sta tmp8
        tmp9 = 4; tmp8 = 4;
        flags |= FLAG_C;
    } else {
        flags &= ~FLAG_C;
    }
    // c8dce:
    //     lda tmp6; sbc tmp8; sta tmp6
    {
        uint16_t r = (uint16_t)tmp6 - (uint16_t)tmp8 - (1 - (flags & FLAG_C));
        tmp6 = (uint8_t)(r & 0xff);
        if (r < 0x100) flags |= FLAG_C; else flags &= ~FLAG_C;
    }
    //     lda tmp7; sbc tmp9; sta tmp7
    {
        uint16_t r = (uint16_t)tmp7 - (uint16_t)tmp9 - (1 - (flags & FLAG_C));
        tmp7 = (uint8_t)(r & 0xff);
        if (r < 0x100) flags |= FLAG_C; else flags &= ~FLAG_C;
    }
    //     lda tmp0; clc; adc tmp6
    flags &= ~FLAG_C;
    {
        uint16_t sum = (uint16_t)tmp0 + (uint16_t)tmp6;
        a = (uint8_t)(sum & 0xff);
        if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C;
    }
    //     sta ptr5
    ptr5 = (ptr5 & 0xff00) | a;
    //     pha
    //     lda tmp1; adc tmp7
    {
        uint16_t sum = (uint16_t)tmp1 + (uint16_t)tmp7 + (flags & FLAG_C ? 1U : 0U);
        a = (uint8_t)(sum & 0xff);
        if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C;
    }
    //     sta ptr5+1
    ptr5 = (ptr5 & 0x00ff) | ((uint16_t)a << 8);
    //     sta l0081
    l0081 = a;
    //     pla
    a = (uint8_t)(ptr5 & 0xff);
    //     sbc #0x8b
    {
        uint16_t r = (uint16_t)a - 0x8bU - (1 - (flags & FLAG_C));
        a = (uint8_t)(r & 0xff);
        if (r < 0x100) flags |= FLAG_C; else flags &= ~FLAG_C;
    }
    //     sta input_buffer_offset+1
    l0082 = a;
    //     bcs return_18
    if (!(flags & FLAG_C)) {
        //     dec l0081
        l0081--;
    }
    // return_18:
    //     rts
}
static void sub_c8da2(void) {
    // sub_c8da2:
    //     pha
    //     tya
    //     pha
    { uint8_t saved_a = a; uint8_t saved_y = y;
    //     jsr compute_bytes_free
    compute_bytes_free();
    //     stx tmp8
    tmp8 = x;
    //     sty tmp9
    tmp9 = y;
    //     pla
    //     tay
    y = saved_y;
    //     pla
    a = saved_a; }
    compute_space_common();
}
static void compute_required_space_for_insertion(void) {
    // compute_required_space_for_insertion:
    //     ldx #0
    x = 0;
    //     stx tmp8
    tmp8 = 0;
    //     stx tmp9
    tmp9 = 0;
    //     beq c8daf                                                         ; ALWAYS branch
    compute_space_common();
}
static void parse_optional_filename_from_command(void) {
    // Pseudocode: Parses optional filename from input buffer into filename_buffer

    // parse_optional_filename_from_command:
    //     jsr sub_c8e33
    sub_c8e33();
    //     beq return_19
    if (flags & FLAG_Z) return;
    //     ldx #0
    x = 0;
    // loop_c8dfb:
loop_c8dfb:
    //     lda input_buffer,y
    a = input_buffer[y];
    //     cmp #0x0d
    if (a == 0x0d) goto c8e25;
    //     iny
    y++;
    //     cmp l007e
    if (a == l007e) goto c8e25;
    //     sta filename_buffer,x
    filename_buffer[x] = a;
    //     inx
    x++;
    //     cpx #MAX_COMMAND_LENGTH-1
    //     bne loop_c8dfb
    if (x != MAX_COMMAND_LENGTH - 1) goto loop_c8dfb;
c8e25:
    //     lda #0x0d
    a = 0x0d;
    //     sta filename_buffer,x
    filename_buffer[x] = a;
    //     sty input_buffer_offset
    input_buffer_offset = y;
    // return_20:
    //     rts
}
static void bad_filename_error(void) {
    // bad_filename_error:
    //     jsr print_inline_string
    //     .ascii "Bad filename\r"
    //     .byte 0
    cli_putstring("Bad filename\n");
    //     jmp return_to_cli_prompt
    return_to_cli_prompt(); return;
}
static void parse_filename_from_command(void) {
    // Pseudocode: Parses mandatory filename, calls bad_filename_error if missing

    // parse_filename_from_command:
    //     jsr parse_optional_filename_from_command
    parse_optional_filename_from_command();
    //     beq bad_filename_error
    if (flags & FLAG_Z) { bad_filename_error(); return; }
    // return_19:
    //     rts
}
static void sub_c8e33(void) {
    // sub_c8e33:
    //     lda l007e
    a = l007e;
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     beq return_20
    if (flags & FLAG_Z) return;
    //     ldy input_buffer_offset
    y = input_buffer_offset;
    // loop_c8e3b:
    while (1) {
        //     lda input_buffer,y
        a = input_buffer[y];
        //     cmp #0x0d
        { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
        //     beq return_20
        if (flags & FLAG_Z) return;
        //     cmp l007e
        { uint16_t tmp_ = a - l007e; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= l007e ? FLAG_C : 0); }
        //     bne return_20
        if (!(flags & FLAG_Z)) return;
        //     iny
        y++;
        //     bne loop_c8e3b
        if (y == 0) break;
    }
    // return_20:
    //     rts
}
static void sub_c8e2d(void) {
    // sub_c8e2d:
    //     lda #0x20 ; ' '
    a = 0x20;
    //     sta l007e
    l007e = a;
    //     sty input_buffer_offset
    input_buffer_offset = y;
    sub_c8e33();
}
static void check_not_continuous_editing(void) {
    // Pseudocode: Verifies not in continuous editing mode, shows file state if editing

    // check_not_continuous_editing:
    //     bit file_edit_flags
    { uint8_t tmp_ = file_edit_flags; flags = (flags & ~(FLAG_N|FLAG_V)) | (tmp_ & FLAG_N) | ((tmp_ << 1) & FLAG_V); }
    //     bvs return_20
    if (flags & FLAG_V) return;
    //     lda file_edit_flags
    a = file_edit_flags;
    //     ror
    { uint8_t old_c = flags & FLAG_C; flags = (flags & ~FLAG_C) | (a & 1); a = (a >> 1) | (old_c << 7); }
    //     bcc return_20
    if (!(flags & FLAG_C)) return;
    //     bcs c8e5d                                                         ; ALWAYS branch
c8e5d:
    display_document_file_state();
}
static void check_continuous_editing(void) {
    // Pseudocode: Verifies continuous editing is active, shows file state if not

    // check_continuous_editing:
    //     bit file_edit_flags
    { uint8_t tmp_ = file_edit_flags; flags = (flags & ~(FLAG_N|FLAG_V)) | (tmp_ & FLAG_N) | ((tmp_ << 1) & FLAG_V); }
    //     bvs c8e5d
    if (flags & FLAG_V) goto c8e5d;
    //     lda file_edit_flags
    a = file_edit_flags;
    //     ror
    { uint8_t old_c = flags & FLAG_C; flags = (flags & ~FLAG_C) | (a & 1); a = (a >> 1) | (old_c << 7); }
    //     bcs return_20
    if (flags & FLAG_C) return;
c8e5d:
    //     jsr display_document_file_state
    display_document_file_state();
}
static void display_no_text(void) {
    // display_no_text:
    //     jsr print_inline_string
    //     .ascii "No text\r"
    //     .byte 0
    cli_putstring("No text\n");
    //     rts
}
static void display_nl_then_no_text(void) {
    // display_nl_then_no_text:
    //     jsr bdos_print_newline
    cli_putchar('\n');
    display_no_text();
}
static void print_document(void) {
    // Pseudocode: Main print/preview loop: formats document with headers, footers, macros, page breaks

    // print_document:
    //     jsr check_not_continuous_editing
    check_not_continuous_editing();
    //     jsr check_for_at_least_150_bytes_free
    check_for_at_least_150_bytes_free();
    //     jsr sub_cb104
    sub_cb104();
    //     lda top
    a = (uint8_t)(top & 0xff);
    //     adc #3
    flags &= ~FLAG_C;
    { uint16_t sum = (uint16_t)a + 3; a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     sta ptr5
    ptr5 = (ptr5 & 0xff00) | a;
    //     tax
    x = a;
    //     lda top+1
    a = (uint8_t)(top >> 8);
    //     adc #0
    { uint16_t sum = (uint16_t)a + 0 + ((flags & FLAG_C) ? 1U : 0U); a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     sta ptr5+1
    ptr5 = (ptr5 & 0x00ff) | ((uint16_t)a << 8);
    //     tay
    y = a;
    //     txa
    a = x;
    //     adc #0x8d
    { uint16_t sum = (uint16_t)a + 0x8d + ((flags & FLAG_C) ? 1U : 0U); a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     bcc c8edb
    if (!(flags & FLAG_C)) goto c8edb;
    //     iny
    y++;
    // c8edb:
c8edb:
    //     sta first_macro_ptr
    first_macro_ptr = (first_macro_ptr & 0xff00) | a;
    //     sta last_macro_ptr
    last_macro_ptr = (last_macro_ptr & 0xff00) | a;
    //     sty first_macro_ptr+1
    first_macro_ptr = (first_macro_ptr & 0x00ff) | ((uint16_t)y << 8);
    //     sty last_macro_ptr+1
    last_macro_ptr = (last_macro_ptr & 0x00ff) | ((uint16_t)y << 8);
    //     lda #0
    a = 0;
    //     sta l0031
    l0031 = a;
    //     sta print_xpos
    print_xpos = a;
    //     sta printing_from_file_flag
    printing_from_file_flag = a;
    //     tay                                                               ; Y=0x00
    y = a;
    //     sta (last_macro_ptr),y
    ram[last_macro_ptr + y] = a;
    //     lda #<(current_ruler_buffer)
    a = (uint8_t)((uintptr_t)current_ruler_buffer & 0xff);
    //     sta current_ruler_ptr
    current_ruler_ptr = (current_ruler_ptr & 0xff00) | a;
    //     lda #>(current_ruler_buffer)
    a = (uint8_t)((uintptr_t)current_ruler_buffer >> 8);
    //     sta current_ruler_ptr+1
    current_ruler_ptr = (current_ruler_ptr & 0x00ff) | ((uint16_t)a << 8);
    //     jsr find_margins_of_current_ruler_buffer
    find_margins_of_current_ruler_buffer();
    //     jsr sub_c8e33
    sub_c8e33();
    //     bne c8f0d
    if (!(flags & FLAG_Z)) goto c8f0d;
    //     inc printing_from_file_flag
    printing_from_file_flag++;
    //     lda page
    a = (uint8_t)(page & 0xff);
    //     sta ptr6
    ptr6 = (ptr6 & 0xff00) | a;
    //     lda page+1
    a = (uint8_t)(page >> 8);
    //     sta ptr6+1
    ptr6 = (ptr6 & 0x00ff) | ((uint16_t)a << 8);
    //     bne c8f30
    if (a != 0) goto c8f30;
    // c8f0a:
c8f0a:
    // c8f0d:
c8f0d:
    //     jsr parse_optional_filename_from_command
    parse_optional_filename_from_command();
    //     bne c8f29
    if (!(flags & FLAG_Z)) goto c8f29;
    //     lda l0031
    a = l0031;
    //     bpl return_23
    if (!(flags & FLAG_N)) return;
    //     jmp c9263
    c9263(); return;

    // return_23:
    //     rts

    // c8f1a:
    //     jsr stop_printing
    //     jsr bdos_print_newline
    //     jmp return_to_cli_prompt

    // c8f29:
c8f29:
    //     #if 0
    //     lda #0x40 ; '@'
    //     jsr open_file
    //     #endif
    //     sta rw_file_handle
    rw_file_handle = a;
    // c8f30:
c8f30:
    //     lda l0031
    a = l0031;
    //     beq c8f3b
    if (a == 0) goto c8f3b;
    //     lda l0021
    a = l0021;
    //     bne c8f3b
    if (a != 0) goto c8f3b;
    //     jsr c9263
    c9263();
    // c8f3b:
c8f3b:
    //     jsr sub_c9188
    sub_c9188();
    //     bcs c8f0a
    if (flags & FLAG_C) goto c8f0a;
    //     jsr sub_c916a
    sub_c916a();
    //     ldy #0
    y = 0;
    //     sty input_buffer_offset+1
    //     jsr deref_and_check_for_command_prefix
    deref_and_check_for_command_prefix();
    //     bne c8fce_thunk
    if (!(flags & FLAG_Z)) goto c8fce_thunk;
    //     ldy #3
    y = 3;
    //     sty input_buffer_offset+1
    //     jsr sub_cab6e
    sub_cab6e();
    //     bne c8f6e
    if (!(flags & FLAG_Z)) goto c8f6e;
    //     ldy #3
    y = 3;
    //     ldx #0
    x = 0;
    // loop_c8f5d:
loop_c8f5d:
    //     lda (tmp0),y
    a = ram[((uint16_t)tmp1 << 8 | tmp0) + y];
    //     sta current_ruler_buffer,x
    current_ruler_buffer[x] = a;
    //     iny
    y++;
    //     inx
    x++;
    //     cmp #0x0d
    //     bne loop_c8f5d
    if (a != 0x0d) goto loop_c8f5d;
    //     jsr find_margins_of_current_ruler_buffer
    find_margins_of_current_ruler_buffer();
    // c8f6b:
c8f6b:
    //     jmp c900e
    goto c900e;

    // c8f6e:
c8f6e:
    //     jsr lookup_formatting_command
    lookup_formatting_command();
    //     bmi c8f7a
    if (flags & FLAG_N) goto c8f7a;
    //     jsr execute_formatting_command
    execute_formatting_command();
    //     beq c8f6b
    if (flags & FLAG_Z) goto c8f6b;
    // c8fce_thunk:
c8fce_thunk:
    //     bne c8fce                                                         ; ALWAYS branch
    goto c8fce;

    // c8f7a:
c8f7a:
    //     lda first_macro_ptr
    a = (uint8_t)(first_macro_ptr & 0xff);
    //     sta tmp6
    tmp6 = a;
    //     lda first_macro_ptr+1
    a = (uint8_t)(first_macro_ptr >> 8);
    //     sta tmp7
    tmp7 = a;
    //     ldy #1
    y = 1;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    //     sta tmp8
    tmp8 = a;
    //     iny                                                               ; Y=0x02
    y++;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    //     jsr is_uppercase
    is_uppercase();
    //     bcc c8f92
    if (!(flags & FLAG_C)) goto c8f92;
    //     lda #0x20 ; ' '
    a = 0x20;
    // c8f92:
c8f92:
    //     sta tmp9
    tmp9 = a;
    // lookup_macro_name:
lookup_macro_name:
    //     ldy #0
    y = 0;
    //     lda (tmp6),y
    a = ram[((uint16_t)tmp7 << 8 | tmp6) + y];
    //     beq c8f6b
    if (a == 0) goto c8f6b;
    //     ldy #2
    y = 2;
    //     lda (tmp6),y
    a = ram[((uint16_t)tmp7 << 8 | tmp6) + y];
    //     cmp tmp8
    { uint16_t tmp_ = a - tmp8; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= tmp8 ? FLAG_C : 0); }
    //     bne get_next_macro_in_linked_list
    if (!(flags & FLAG_Z)) goto get_next_macro_in_linked_list;
    //     iny                                                               ; Y=0x03
    y++;
    //     lda (tmp6),y
    a = ram[((uint16_t)tmp7 << 8 | tmp6) + y];
    //     cmp tmp9
    { uint16_t tmp_ = a - tmp9; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= tmp9 ? FLAG_C : 0); }
    //     beq c8fb9
    if (flags & FLAG_Z) goto c8fb9;
    // get_next_macro_in_linked_list:
get_next_macro_in_linked_list:
    //     ldy #0
    y = 0;
    //     lda (tmp6),y
    a = ram[((uint16_t)tmp7 << 8 | tmp6) + y];
    //     pha
{   uint8_t saved_tmp = a;
    //     iny                                                               ; Y=0x01
    y++;
    //     lda (tmp6),y
    a = ram[((uint16_t)tmp7 << 8 | tmp6) + y];
    //     sta tmp7
    tmp7 = a;
    //     pla
    a = saved_tmp; }
    //     sta tmp6
    tmp6 = a;
    //     jmp lookup_macro_name
    goto lookup_macro_name;

    // c8fb9:
c8fb9:
    //     lda macro_executing_flag
    a = macro_executing_flag;
    //     bne nested_macro_error
    if (a != 0) { nested_macro_error(); return; }
    //     lda tmp6
    a = tmp6;
    //     clc
    flags &= ~FLAG_C;
    //     adc #4
    { uint16_t sum = (uint16_t)a + 4; a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     sta ptr3
    ptr3 = (ptr3 & 0xff00) | a;
    //     lda tmp7
    a = tmp7;
    //     adc #0
    { uint16_t sum = (uint16_t)a + 0 + ((flags & FLAG_C) ? 1U : 0U); a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     sta ptr3+1
    ptr3 = (ptr3 & 0x00ff) | ((uint16_t)a << 8);
    //     sta macro_executing_flag
    macro_executing_flag = a;
    //     bne c900e
    if (a != 0) goto c900e;
    // c8fce:
c8fce:
    //     lda l0031
    a = l0031;
    //     bne c8fd5
    if (a != 0) goto c8fd5;
    //     jsr render_new_page
    render_new_page();
    // c8fd5:
c8fd5:
    //     jsr sub_c9407
    sub_c9407();
    //     lda #0
    a = 0;
    //     sta l0039
    l0039 = a;
    //     ldy input_buffer_offset+1
    y = 0;
    //     lda print_flags
    a = print_flags;
    //     bpl c8fe6
    if (!(flags & FLAG_N)) goto c8fe6;
    //     lda microspacing_flag
    a = microspacing_flag;
    //     bne c9034
    if (a != 0) { microspace_word_processor(); goto c8f30; }
    // c8fe6:
c8fe6:
    //     lda (tmp0),y
    a = ram[((uint16_t)tmp1 << 8 | tmp0) + y];
    //     iny
    y++;
    //     jsr sub_c9431
    sub_c9431();
    //     jsr c9426
    while (x > 0) { print_char(); x--; }
    //     cmp #0x0d
    //     bne c8fe6
    if (a != 0x0d) goto c8fe6;
    //     inc register_value_l
    register_value_l[0]++; flags = (flags & ~(FLAG_Z|FLAG_N)) | (register_value_l[0] == 0 ? FLAG_Z : 0) | (register_value_l[0] & FLAG_N);
    //     bne c8ffb
    if (!(flags & FLAG_Z)) goto c8ffb;
    //     inc register_value_l+1
    register_value_l[1]++; flags = (flags & ~(FLAG_Z|FLAG_N)) | (register_value_l[1] == 0 ? FLAG_Z : 0) | (register_value_l[1] & FLAG_N);
    // c8ffb:
c8ffb:
    //     ldx line_spacing
    x = line_spacing;
    //     lda l0021
    a = l0021;
    //     clc
    flags &= ~FLAG_C;
    //     sbc line_spacing
    { uint16_t r = (uint16_t)a - (uint16_t)line_spacing - (1 - ((flags & FLAG_C) ? 1U : 0U)); a = (uint8_t)(r & 0xff); if (r < 0x100) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     bcs c9009
    if (flags & FLAG_C) goto c9009;
    //     lda #0
    a = 0;
    //     ldx l0021
    x = l0021;
    //     dex
    x--;
    // c9009:
c9009:
    //     sta l0021
    l0021 = a;
    //     jsr print_vertical_space
    print_vertical_space();
    // c900e:
c900e:
    //     jmp c8f30
    goto c8f30;
}
static void nested_macro_error(void) {
    // nested_macro_error:
    //     jsr stop_printing
    stop_printing();
    //     jsr print_inline_string
    //     .ascii "Nested macro call"
    //     .byte 0
    cli_putstring("Nested macro call");
    //     jmp c8f1a
    // c8f1a:
    //     jsr bdos_print_newline
    cli_putchar('\n');
    //     jmp return_to_cli_prompt
    return_to_cli_prompt(); return;
}
static void microspace_word_processor(void) {
    // Pseudocode: Processes words for microspaced justification during printing

    // return_24:
    //     rts

    // c9034:
    //     ldx #0
    x = 0;
    //     stx l0044
    l0044 = x;
    //     stx l0046
    l0046 = x;
    //     stx l0045
    l0045 = x;
    //     stx l0047
    l0047 = x;
    //     stx l0039
    l0039 = x;
    //     stx l0048
    l0048 = x;
    //     stx l0042
    l0042 = x;
    //     stx l0043
    l0043 = x;
    //     stx l0083
    l0083 = x;
    // c9048:
c9048:
    //     txa
    a = x;
    //     pha
{   uint8_t saved_a = a;
    //     lda (tmp0),y
    a = ram[((uint16_t)tmp1 << 8 | tmp0) + y];
    //     jsr sub_c9431
    sub_c9431();
    //     pla
    a = saved_a; }
    //     tax
    x = a;
    //     lda (tmp0),y
    a = ram[((uint16_t)tmp1 << 8 | tmp0) + y];
    //     iny
    y++;
    //     cmp #0x1a
    //     bne c906f
    if (a != 0x1a) goto c906f;
    //     bit l0083
    flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_V)) | ((a & l0083) == 0 ? FLAG_Z : 0) | (l0083 & FLAG_N) | ((l0083 & 0x40) ? FLAG_V : 0);
    //     bpl c9064
    if (!(flags & FLAG_N)) goto c9064;
    //     lda l0048
    a = l0048;
    //     beq c906b
    if (a == 0) goto c906b;
    //     inc l0043
    l0043++; flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0043 == 0 ? FLAG_Z : 0) | (l0043 & FLAG_N);
    //     bne c9048
    if (!(flags & FLAG_Z)) goto c9048;
    // c9064:
c9064:
    //     lda l0039
    a = l0039;
    //     sta l0047
    l0047 = a;
    //     jmp c908c
    goto c908c;

    // c906b:
c906b:
    //     lda #0x20 ; ' '
    a = 0x20;
    //     dec l0042
    l0042--; flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0042 == 0 ? FLAG_Z : 0) | (l0042 & FLAG_N);
    // c906f:
c906f:
    //     cmp #0x20 ; ' '
    //     bcc c9092
    if (a < 0x20) goto c9092;
    //     bne c9090
    if (a != 0x20) goto c9090;
    //     bit l0083
    flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_V)) | ((a & l0083) == 0 ? FLAG_Z : 0) | (l0083 & FLAG_N) | ((l0083 & 0x40) ? FLAG_V : 0);
    //     bpl c9064
    if (!(flags & FLAG_N)) goto c9064;
    //     lda l0042
    a = l0042;
    //     beq c908a
    if (a == 0) goto c908a;
    //     bmi c9087
    if (flags & FLAG_N) goto c9087;
    //     inc l0043
    l0043++; flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0043 == 0 ? FLAG_Z : 0) | (l0043 & FLAG_N);
    //     lda #0
    a = 0;
    //     sta l0042
    l0042 = a;
    //     beq c9048                                                         ; ALWAYS branch
    goto c9048;

    // c9087:
c9087:
    //     clc
    flags &= ~FLAG_C;
    //     ror l0042
    { uint8_t old_c = (flags & FLAG_C) ? 1 : 0; flags = (flags & ~FLAG_C) | (l0042 & 1); l0042 = (l0042 >> 1) | (old_c << 7); flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0042 == 0 ? FLAG_Z : 0) | (l0042 & FLAG_N); }
    // c908a:
c908a:
    //     inc l0048
    l0048++; flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0048 == 0 ? FLAG_Z : 0) | (l0048 & FLAG_N);
    // c908c:
c908c:
    //     lda #0x20 ; ' '
    a = 0x20;
    //     bne c90b6                                                         ; ALWAYS branch
    goto c90b6;

    // c9090:
c9090:
    //     inc l0046
    l0046++; flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0046 == 0 ? FLAG_Z : 0) | (l0046 & FLAG_N);
    // c9092:
c9092:
    //     cmp #9
    //     beq c90a0
    if (a == 9) goto c90a0;
    //     cmp #0x0b
    //     beq c90a0
    if (a == 0x0b) goto c90a0;
    //     sec
    flags |= FLAG_C;
    //     ror l0083
    { uint8_t old_c = (flags & FLAG_C) ? 1 : 0; flags = (flags & ~FLAG_C) | (l0083 & 1); l0083 = (l0083 >> 1) | (old_c << 7); flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0083 == 0 ? FLAG_Z : 0) | (l0083 & FLAG_N); }
    //     jmp c90b6
    goto c90b6;

    // c90a0:
c90a0:
    //     pha
{   uint8_t saved_a2 = a;
    //     lda l0039
    a = l0039;
    //     sta l0047
    l0047 = a;
    //     lda #0
    a = 0;
    //     sta l0083
    l0083 = a;
    //     sta l0046
    l0046 = a;
    //     sta l0048
    l0048 = a;
    //     sta l0042
    l0042 = a;
    //     sta l0043
    l0043 = a;
    //     sta l0044
    l0044 = a;
    //     sta l0045
    l0045 = a;
    //     pla
    a = saved_a2; }
    // c90b6:
c90b6:
    //     sta output_buffer,x
    output_buffer[x] = a;
    //     inx
    x++;
    //     cmp #0x0d
    //     beq c90e2
    if (a == 0x0d) goto c90e2;
    //     cmp #0x20 ; ' '
    //     beq c9048
    if (a == 0x20) goto c9048;
    //     lda l0048
    a = l0048;
    //     beq c9048
    if (a == 0) goto c9048;
    //     clc
    flags &= ~FLAG_C;
    //     adc l0044
    { uint16_t sum = (uint16_t)a + l0044; a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     sta l0044
    l0044 = a;
    //     lda l0046
    a = l0046;
    //     adc l0048
    { uint16_t sum = (uint16_t)a + l0048 + ((flags & FLAG_C) ? 1U : 0U); a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     sta l0046
    l0046 = a;
    //     lda l0045
    a = l0045;
    //     adc l0043
    { uint16_t sum = (uint16_t)a + l0043 + ((flags & FLAG_C) ? 1U : 0U); a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     sta l0045
    l0045 = a;
    //     lda #0
    a = 0;
    //     sta l0048
    l0048 = a;
    //     sta l0042
    l0042 = a;
    //     sta l0043
    l0043 = a;
    //     jmp c9048
    goto c9048;

    // c90e2:
c90e2:
    //     lda l0045
    a = l0045;
    //     beq c90f8
    if (a == 0) goto c90f8;
    //     lda ruler_right_stop
    a = ruler_right_stop;
    //     beq c90f8
    if (a == 0) goto c90f8;
    //     sec
    flags |= FLAG_C;
    //     sbc l0047
    { int16_t r = (int16_t)a - (int16_t)l0047 - (1 - ((flags & FLAG_C) ? 1U : 0U)); a = (uint8_t)r; if (r >= 0) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     bcc c90f8
    if (!(flags & FLAG_C)) goto c90f8;
    //     sbc l0045
    { int16_t r = (int16_t)a - (int16_t)l0045 - (1 - ((flags & FLAG_C) ? 1U : 0U)); a = (uint8_t)r; if (r >= 0) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     adc #0
    { uint16_t sum = (uint16_t)a + 0 + ((flags & FLAG_C) ? 1U : 0U); a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     sec
    flags |= FLAG_C;
    //     sbc l0046
    { int16_t r = (int16_t)a - (int16_t)l0046 - (1 - ((flags & FLAG_C) ? 1U : 0U)); a = (uint8_t)r; if (r >= 0) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     beq c9101
    if (flags & FLAG_Z) goto c9101;
    // c90f8:
c90f8:
    //     lda #0
    a = 0;
    //     sta l0039
    l0039 = a;
    //     ldy input_buffer_offset+1
    y = 0;
    //     jmp c8fe6
    goto c8fe6_inline;

    // c9101:
c9101:
    //     lda #0
    a = 0;
    //     sta tmp9
    tmp9 = a;
    //     ldx #8
    x = 8;
    // loop_c9107:
loop_c9107:
    //     asl
    flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N)) | ((a & 0x80) ? FLAG_C : 0); a <<= 1; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     rol tmp9
    { uint8_t old_c = (flags & FLAG_C) ? 1 : 0; uint8_t new_c = (tmp9 & 0x80) ? FLAG_C : 0; tmp9 = (tmp9 << 1) | old_c; flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N)) | new_c | (tmp9 == 0 ? FLAG_Z : 0) | (tmp9 & FLAG_N); }
    //     asl l0045
    { uint8_t new_c = (l0045 & 0x80) ? FLAG_C : 0; l0045 <<= 1; flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N)) | new_c | (l0045 == 0 ? FLAG_Z : 0) | (l0045 & FLAG_N); }
    //     bcc c9115
    if (!(flags & FLAG_C)) goto c9115;
    //     clc
    flags &= ~FLAG_C;
    //     adc microspacing_flag
    { uint16_t sum = (uint16_t)a + microspacing_flag; a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     bcc c9115
    if (!(flags & FLAG_C)) goto c9115;
    //     inc tmp9
    tmp9++; flags = (flags & ~(FLAG_Z|FLAG_N)) | (tmp9 == 0 ? FLAG_Z : 0) | (tmp9 & FLAG_N);
    // c9115:
c9115:
    //     dex
    x--;
    //     bne loop_c9107
    if (x != 0) goto loop_c9107;
    //     sta tmp8
    tmp8 = a;
    //     lda l0044
    a = l0044;
    //     sta l0046
    l0046 = a;
    //     jsr sub_cadf0
    sub_cadf0();
    //     sta l0045
    l0045 = a;
    //     lda tmp8
    a = tmp8;
    //     sta l0044
    l0044 = a;
    //     ldy #0
    y = 0;
    //     sty l0039
    l0039 = y;
    // c912b:
c912b:
    //     lda output_buffer,y
    a = output_buffer[y];
    //     iny
    y++;
    //     jsr sub_c9431
    sub_c9431();
    //     pha
{   uint8_t saved_a3 = a;
    //     lda l0039
    a = l0039;
    //     cmp l0047
    { uint16_t tmp_ = a - l0047; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= l0047 ? FLAG_C : 0); }
    //     beq c913b
    if (flags & FLAG_Z) goto c913b;
    //     bcs c9142
    if (flags & FLAG_C) goto c9142;
    // c913b:
c913b:
    //     pla
    a = saved_a3;
    //     jsr c9426
    // PROBLEM: jsr c9426 not translated
    //     jmp c9163
    goto c9163;

    // c9142:
c9142:
    //     pla
    a = saved_a3;
    //     cmp #0x20 ; ' '
    //     bne c915b
    if (a != 0x20) goto c915b;
    //     lda microspacing_flag
    a = microspacing_flag;
    //     clc
    flags &= ~FLAG_C;
    //     adc l0044
    { uint16_t sum = (uint16_t)a + l0044; a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     tax
    x = a;
    //     lda l0045
    a = l0045;
    //     beq c9154
    if (a == 0) goto c9154;
    //     inx
    x++;
    //     dec l0045
    l0045--; flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0045 == 0 ? FLAG_Z : 0) | (l0045 & FLAG_N);
    // c9154:
c9154:
    //     jsr sub_c9173
    sub_c9173();
    //     lda #0x20 ; ' '
    a = 0x20;
    //     bne c9160                                                         ; ALWAYS branch
    goto c9160;

    // c915b:
c915b:
    //     ldx microspacing_flag
    x = microspacing_flag;
    //     jsr sub_c9173
    sub_c9173();
    // c9160:
c9160:
    //     jsr print_char
    print_char();
    // c9163:
c9163:
    //     cmp #0x0d
    //     bne c912b
    if (a != 0x0d) goto c912b;
    //     jmp c8ffb
    goto c8ffb_inline;
}

c8fe6_inline:
    //     lda (tmp0),y
    a = ram[((uint16_t)tmp1 << 8 | tmp0) + y];
    //     iny
    y++;
    //     jsr sub_c9431
    sub_c9431();
    //     jsr c9426
    while (x > 0) { print_char(); x--; }
    //     cmp #0x0d
    //     bne c8fe6_inline
    if (a != 0x0d) goto c8fe6_inline;
    //     inc register_value_l
    register_value_l[0]++; flags = (flags & ~(FLAG_Z|FLAG_N)) | (register_value_l[0] == 0 ? FLAG_Z : 0) | (register_value_l[0] & FLAG_N);
    //     bne c8ffb_inline
    if (!(flags & FLAG_Z)) goto c8ffb_inline;
    //     inc register_value_l+1
    register_value_l[1]++; flags = (flags & ~(FLAG_Z|FLAG_N)) | (register_value_l[1] == 0 ? FLAG_Z : 0) | (register_value_l[1] & FLAG_N);
c8ffb_inline:
    //     ldx line_spacing
    x = line_spacing;
    //     lda l0021
    a = l0021;
    //     clc
    flags &= ~FLAG_C;
    //     sbc line_spacing
    { uint16_t r = (uint16_t)a - (uint16_t)line_spacing - (1 - ((flags & FLAG_C) ? 1U : 0U)); a = (uint8_t)(r & 0xff); if (r < 0x100) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     bcs c9009_inline
    if (flags & FLAG_C) goto c9009_inline;
    //     lda #0
    a = 0;
    //     ldx l0021
    x = l0021;
    //     dex
    x--;
    // c9009_inline:
c9009_inline:
    //     sta l0021
    l0021 = a;
    //     jsr print_vertical_space
    print_vertical_space();
    //     rts
    return;
}
static void sub_c916a(void) {
    // Pseudocode: Checks if printer is active and starts microspacing if supported

    // sub_c916a:
    //     ldx print_flags
    x = print_flags;
    //     bpl return_25
    if (!(x & 0x80)) return;
    //     ldx microspacing_flag
    x = microspacing_flag;
    //     bne c9177
    if (x == 0) return;
    // c9177:
    //     jsr sub_c9445
    sub_c9445();
    //     pha
    {   uint8_t saved_a = a;
        //     stx l0043
        l0043 = x;
        //     lda #9
        a = 9;
        //     jsr call_printer_driver
        call_printer_driver();
        //     pla
        a = saved_a;
    }
    // return_25:
    //     rts
    return;
}
static void sub_c9173(void) {
    // Pseudocode: Emits spaces for microspacing by calling printer driver with spacing count

    // sub_c9173:
    //     cpx l0043
    //     beq return_25
    if (x == l0043) return;
    // c9177:
    //     jsr sub_c9445
    sub_c9445();
    //     pha
    {   uint8_t saved_a = a;
        //     stx l0043
        l0043 = x;
        //     lda #9
        a = 9;
        //     jsr call_printer_driver
        call_printer_driver();
        //     pla
        a = saved_a;
    }
    // return_25:
    //     rts
    return;

    // c9184:
    //     lda #0
    //     sta macro_executing_flag
}
static void sub_c9188(void) {
    // Pseudocode: Gets next line of text for printing, handling macro execution

    // sub_c9188:
    //     lda macro_executing_flag
    a = macro_executing_flag;
    //     bne c91a3
    if (a != 0) goto c91a3;
    //     lda ptr5
    a = (uint8_t)(ptr5 & 0xff);
    //     sta input_buffer_offset+1
    //     sta tmp0
    tmp0 = a;
    //     lda ptr5+1
    a = (uint8_t)(ptr5 >> 8);
    //     sta l0081
    l0081 = a;
    //     sta tmp1
    tmp1 = a;
    //     jsr sub_c9241
    sub_c9241();
    //     bcs return_26
    if (flags & FLAG_C) return;
    //     lda ptr5
    a = (uint8_t)(ptr5 & 0xff);
    //     ldy ptr5+1
    y = (uint8_t)(ptr5 >> 8);
    //     bne c91d0
    if (y != 0) goto c91d0;
    // c91a3:
c91a3:
    //     ldy #0
    y = 0;
    //     ldx #0
    x = 0;
    // c91a7:
c91a7:
    //     lda (ptr3),y
    a = ram[ptr3 + y];
    //     cmp #4
    //     beq c9184
    if (a == 4) { macro_executing_flag = 0; return; }
    //     cmp #0x40 ; '@'
    //     beq c91da
    if (a == 0x40) goto c91da;
    //     iny
    y++;
    // loop_c91b2:
loop_c91b2:
    //     sta current_line_buffer,x
    current_line_buffer[x] = a;
    //     inx
    x++;
    //     cmp #0x0d
    //     beq c91c2
    if (a == 0x0d) goto c91c2;
    //     cpx #MAX_LINE_LENGTH-1
    //     bcc c91a7
    if (x < MAX_LINE_LENGTH - 1) goto c91a7;
    //     lda #0x0d
    a = 0x0d;
    //     bne loop_c91b2                                                    ; ALWAYS branch
    goto loop_c91b2;

    // c91c2:
c91c2:
    //     tya
    a = y;
    //     clc
    flags &= ~FLAG_C;
    //     adc ptr3
    { uint16_t sum = (uint16_t)a + (uint8_t)(ptr3 & 0xff); a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     sta ptr3
    ptr3 = (ptr3 & 0xff00) | a;
    //     bcc c91cc
    if (!(flags & FLAG_C)) goto c91cc;
    //     inc ptr3+1
    ptr3 = (ptr3 & 0x00ff) | ((uint16_t)((uint8_t)((ptr3 >> 8) & 0xff) + 1) << 8);
    // c91cc:
c91cc:
    //     lda ptr1
    a = (uint8_t)(ptr1 & 0xff);
    //     ldy ptr1+1
    y = (uint8_t)(ptr1 >> 8);
    // c91d0:
c91d0:
    //     sta tmp0
    tmp0 = a;
    //     sty tmp1
    tmp1 = y;
    //     sta current_format_line_ptr
    current_format_line_ptr = (current_format_line_ptr & 0xff00) | a;
    //     sty current_format_line_ptr+1
    current_format_line_ptr = (current_format_line_ptr & 0x00ff) | ((uint16_t)y << 8);
    //     clc
    flags &= ~FLAG_C;
    // return_26:
    //     rts

    // c91da:
c91da:
    //     iny
    y++;
    //     lda (ptr3),y
    a = ram[ptr3 + y];
    //     sec
    flags |= FLAG_C;
    //     sbc #0x30 ; '0'
    { uint16_t r = (uint16_t)a - 0x30 - (1 - ((flags & FLAG_C) ? 1U : 0U)); a = (uint8_t)(r & 0xff); if (r < 0x100) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     bcc c9225
    if (!(flags & FLAG_C)) goto c9225;
    //     cmp #0x0a
    { uint16_t tmp_ = a - 0x0a; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0a ? FLAG_C : 0); }
    //     bcs c9225
    if (flags & FLAG_C) goto c9225;
    //     iny
    y++;
    //     sty l0084
    l0084 = y;
    //     sta l0083
    l0083 = a;
    //     lda #0
    a = 0;
    //     sta l0082
    l0082 = a;
    //     ldy #2
    y = 2;
    // loop_c91f1:
loop_c91f1:
    //     dec l0083
    l0083--; flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0083 == 0 ? FLAG_Z : 0) | (l0083 & FLAG_N);
    //     bmi c9209
    if (flags & FLAG_N) goto c9209;
    // c91f5:
c91f5:
    //     iny
    y++;
    //     lda (ptr5),y
    a = ram[ptr5 + y];
    //     cmp #0x0d
    //     beq c9223
    if (a == 0x0d) goto c9223;
    //     jsr sub_c9228
    sub_c9228();
    //     beq c91f5
    if (flags & FLAG_Z) goto c91f5;
    //     bvs c91f5
    if (flags & FLAG_V) goto c91f5;
    //     cmp #0x2c ; ','
    //     beq loop_c91f1
    if (a == 0x2c) goto loop_c91f1;
    //     bne c91f5                                                         ; ALWAYS branch
    goto c91f5;

    // c9209:
c9209:
    //     iny
    y++;
    //     lda (ptr5),y
    a = ram[ptr5 + y];
    //     cmp #0x0d
    //     beq c9223
    if (a == 0x0d) goto c9223;
    //     jsr sub_c9228
    sub_c9228();
    //     beq c9209
    if (flags & FLAG_Z) goto c9209;
    //     bvs c921b
    if (flags & FLAG_V) goto c921b;
    //     cmp #0x2c ; ','
    //     beq c9223
    if (a == 0x2c) goto c9223;
    // c921b:
c921b:
    //     sta current_line_buffer,x
    current_line_buffer[x] = a;
    //     inx
    x++;
    //     cpx #MAX_LINE_LENGTH-2
    //     bcc c9209
    if (x < MAX_LINE_LENGTH - 2) goto c9209;
    // c9223:
c9223:
    //     ldy l0084
    y = l0084;
    // c9225:
c9225:
    //     jmp c91a7
    goto c91a7;
}
static void sub_c9228(void) {
    // Pseudocode: Parses register reference markers (<, >, =) in format line

    // sub_c9228:
    //     cmp #0x3e ; '>'
    //     bne c9231
    { uint16_t tmp_ = a - 0x3e; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x3e ? FLAG_C : 0); }
    if (!(flags & FLAG_Z)) goto c9231;
    //     lda #0
    a = 0;
    //     sta l0082
    l0082 = a;
    //     rts
    return;

    // c9231:
c9231:
    //     cmp #0x3c ; '<'
    //     bne c923c
    { uint16_t tmp_ = a - 0x3c; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x3c ? FLAG_C : 0); }
    if (!(flags & FLAG_Z)) goto c923c;
    //     lda #0x40 ; '@'
    a = 0x40;
    //     sta l0082
    l0082 = a;
    //     lda #0
    a = 0;
    //     rts
    return;

    // c923c:
c923c:
    //     bit l0082
    { uint8_t tmp_ = a & l0082; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ == 0 ? FLAG_Z : 0) | (l0082 & (FLAG_N|FLAG_V)); }
    //     ora #0
    a |= 0;
    //     rts
    return;
}
static void sub_c9241(void) {
    // Pseudocode: Reads next line from file buffer or calls read_block_from_file for printing

    // sub_c9241:
    //     lda printing_from_file_flag
    a = printing_from_file_flag;
    //     beq c9260
    if (a == 0) { read_block_from_file(); return; }
    //     ldy #0
    y = 0;
    // loop_c9247:
    loop_c9247:
    //     lda (ptr6),y
    a = ram[ptr6 + y];
    //     sec
    flags |= FLAG_C;
    //     beq return_27
    if (a == 0) return;
    //     sta (tmp0),y
    ram[((uint16_t)tmp1 << 8 | tmp0) + y] = a;
    //     inc ptr6
    ptr6 = (ptr6 & 0xff00) | ((uint8_t)(ptr6 & 0xff) + 1);
    if ((uint8_t)(ptr6 & 0xff) == 0) ptr6 += 0x100;
    //     bne c9254
    if ((uint8_t)(ptr6 & 0xff) != 0) goto c9254;
    //     inc ptr6+1
    ptr6 += 0x100;
    // c9254:
c9254:
    //     inc tmp0
    tmp0++;
    //     bne c925a
    if (tmp0 != 0) goto c925a;
    //     inc tmp1
    tmp1++;
    // c925a:
c925a:
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     bne loop_c9247
    if (!(flags & FLAG_Z)) goto loop_c9247;
    //     clc
    flags &= ~FLAG_C;
    // return_27:
    //     rts
    return;

    // c9260:
    //     jmp read_block_from_file
}
static void c9263(void) {
    // Pseudocode: Handles page footer processing: prints footer, increments page number

    // c9263:
    //     lda l0038
    a = l0038;
    //     beq c9284
    if (a == 0) goto c9284;
    //     ldx l0021                                                         ; X=number of lines
    x = l0021;
    //     jsr print_vertical_space
    print_vertical_space();
    //     ldx footer_margin                                                 ; X=number of lines
    x = footer_margin;
    //     jsr print_vertical_space
    print_vertical_space();
    //     lda footers_enabled_flag
    a = footers_enabled_flag;
    //     beq c927c
    if (a == 0) goto c927c;
    //     ldx #<(footer_text_maybe)
    x = (uint8_t)((uintptr_t)footer_text_maybe & 0xff);
    //     ldy #>(footer_text_maybe)
    y = (uint8_t)((uintptr_t)footer_text_maybe >> 8);
    //     jsr render_header_or_footer
    render_header_or_footer();
    // c927c:
c927c:
    //     jsr print_newline
    print_newline();
    //     ldx bottom_margin                                                 ; X=number of lines
    x = bottom_margin;
    //     jsr print_vertical_space
    print_vertical_space();
    // c9284:
c9284:
    //     inc register_value_p
    register_value_p[0]++; flags = (flags & ~(FLAG_Z|FLAG_N)) | (register_value_p[0] == 0 ? FLAG_Z : 0) | (register_value_p[0] & FLAG_N);
    //     bne c928c
    if (!(flags & FLAG_Z)) goto c928c;
    //     inc register_value_p+1
    register_value_p[1]++; flags = (flags & ~(FLAG_Z|FLAG_N)) | (register_value_p[1] == 0 ? FLAG_Z : 0) | (register_value_p[1] & FLAG_N);
    // c928c:
c928c:
    //     lda #1
    a = 1;
    //     sta register_value_l
    register_value_l[0] = a;
    //     lda #0
    a = 0;
    //     sta register_value_l+1
    register_value_l[1] = a;
    //     sta l0031
    l0031 = a;
    //     rts
    return;
}
static void render_new_page(void) {
    // Pseudocode: Renders a new page with headers, margins, page number prompt

    // ; ***************************************************************************************
    // render_new_page:
    //     lda #0x81
    a = 0x81;
    //     sta l0031
    l0031 = a;
    //     bit print_flags
    flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_V)) | ((a & print_flags) == 0 ? FLAG_Z : 0) | (print_flags & (FLAG_N|FLAG_V));
    //     bvc c92d4
    if (!(flags & FLAG_V)) goto c92d4;
    //     jsr stop_printing
    stop_printing();
    //     jsr print_inline_string
    //     .ascii "\rPage "
    //     .byte 0
    cli_putstring("\nPage ");

    //     ldx register_value_p
    x = register_value_p[0];
    //     ldy register_value_p+1
    y = register_value_p[1];
    //     jsr render_number_to_screen
    render_number_to_screen();
    //     jsr print_inline_string
    //     .ascii ".."
    //     .byte 0
    cli_putstring("..");

    //     jsr flush_and_read_char
    flush_and_read_char();
    //     bcs c92cc
    if (flags & FLAG_C) goto c92cc;
    //     and #0xdf
    a &= 0xdf;
    //     cmp #0x4d ; 'M'
    //     beq c92d4
    if (a == 0x4d) goto c92d4;
    //     cmp #0x51 ; 'Q'
    //     bne c92cf
    if (a != 0x51) goto c92cf;
    // c92cc:
c92cc:
    //     jmp c8f1a
    stop_printing();
    cli_putchar('\n');
    return_to_cli_prompt(); return;

    // c92cf:
c92cf:
    //     lda #0xc0
    a = 0xc0;
    //     jsr start_printing
    start_printing();
    // c92d4:
c92d4:
    //     lda l0038
    a = l0038;
    //     beq c92f0
    if (a == 0) { sub_c92f0(); return; }
    //     ldx top_margin                                                    ; X=number of lines
    x = top_margin;
    //     jsr print_vertical_space
    print_vertical_space();
    //     lda headers_enabled_flag
    a = headers_enabled_flag;
    //     beq c92e8
    if (a == 0) goto c92e8;
    //     ldx #<(header_text_maybe)
    x = (uint8_t)((uintptr_t)header_text_maybe & 0xff);
    //     ldy #>(header_text_maybe)
    y = (uint8_t)((uintptr_t)header_text_maybe >> 8);
    //     jsr render_header_or_footer
    render_header_or_footer();
    // c92e8:
c92e8:
    //     jsr print_newline
    print_newline();
    //     ldx header_margin                                                 ; X=number of lines
    x = header_margin;
    //     jsr print_vertical_space
    print_vertical_space();
    // c92f0: fall-through to shared routine
    sub_c92f0(); return;
}
static void sub_c92f0(void) {
    // Shared code: computes remaining lines on page (c92f0)
    // c92f0:
    //     ldx page_length
    x = page_length;
    //     lda l0038
    a = l0038;
    //     beq c930d
    if (flags & FLAG_Z) goto c930d;
    //     ldx #1
    x = 1;
    //     lda page_length
    a = page_length;
    //     clc
    flags &= ~FLAG_C;
    //     sbc top_margin
    // PROBLEM: sbc top_margin
    //     bcc c930d
    //     sbc header_margin
    // PROBLEM: sbc header_margin
    //     bcc c930d
    //     clc
    //     sbc bottom_margin
    // PROBLEM: sbc bottom_margin
    //     bcc c930d
    //     sbc footer_margin
    // PROBLEM: sbc footer_margin
    //     bcc c930d
    //     tax
    // c930d:
c930d:
    //     stx l0021
    l0021 = x;
    //     rts
    return;
}
static void c937b(void) {
    // c937b:
    //     ldy #0
    y = 0;
    //     ldx l0084
    x = l0084;
    //     beq return_28
    if (x == 0) return;
    // loop_c9381:
loop_c9381:
    //     txa
    a = x;
    //     pha
{   uint8_t saved_x = a;
    //     lda output_buffer,y
    a = output_buffer[y];
    //     jsr sub_c9431
    sub_c9431();
    //     jsr print_char
    print_char();
    //     iny
    y++;
    //     pla
    a = saved_x; }
    //     tax
    x = a;
    //     dex
    x--;
    //     bne loop_c9381
    if (x != 0) goto loop_c9381;
    // return_28:
    //     rts
}
static void render_header_or_footer(void) {
    // Pseudocode: Renders header or footer text with centering and justification

    // ; ***************************************************************************************
    // render_header_or_footer:
    //     stx tmp4
    tmp4 = x;
    //     sty tmp5
    tmp5 = y;
    //     ldy #0
    y = 0;
    //     sty l0082
    l0082 = y;
    //     lda (tmp4),y
    a = ram[((uint16_t)tmp5 << 8 | tmp4) + y];
    //     beq return_28
    if (a == 0) return;
    //     jsr sub_c9407
    sub_c9407();
    //     lda #0
    a = 0;
    //     sta l0039
    l0039 = a;
    //     jsr sub_c9393
    sub_c9393();
    //     jsr sub_c93fd
    sub_c93fd();
    //     bcs c932e
    if (flags & FLAG_C) goto c932e;
    //     jsr sub_c93a1
    sub_c93a1();
    // c932e:
c932e:
    //     jsr sub_c93c8
    sub_c93c8();
    //     jsr c937b
    c937b();
    //     jsr sub_c939b
    sub_c939b();
    //     jsr sub_c93c8
    sub_c93c8();
    //     txa
    a = x;
    //     beq c9355
    if (a == 0) goto c9355;
    //     dex
    x--;
    //     txa
    a = x;
    //     lsr
    flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N)) | ((a & 1) ? FLAG_C : 0); a >>= 1; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     sta l0081
    l0081 = a;
    //     jsr sub_c93be
    sub_c93be();
    //     beq c9355
    if (flags & FLAG_Z) goto c9355;
    //     lsr
    flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N)) | ((a & 1) ? FLAG_C : 0); a >>= 1; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     sec
    flags |= FLAG_C;
    //     sbc l0081
    { int16_t r = (int16_t)a - (int16_t)l0081 - (1 - ((flags & FLAG_C) ? 1U : 0U)); a = (uint8_t)(r & 0xff); if (r >= 0) { flags |= FLAG_C; } else { flags &= ~FLAG_C; } }
    //     bcc c9355
    if (!(flags & FLAG_C)) goto c9355;
    //     sbc l0039
    { int16_t r = (int16_t)a - (int16_t)l0039 - (1 - ((flags & FLAG_C) ? 1U : 0U)); a = (uint8_t)(r & 0xff); if (r >= 0) { flags |= FLAG_C; } else { flags &= ~FLAG_C; } }
    //     bcc c9355
    if (!(flags & FLAG_C)) goto c9355;
    //     tax
    x = a;
    //     jsr sub_c941a
    sub_c941a();
    // c9355:
c9355:
    //     jsr c937b
    c937b();
    //     jsr sub_c93a1
    sub_c93a1();
    //     jsr sub_c93fd
    sub_c93fd();
    //     bcs c9363
    if (flags & FLAG_C) goto c9363;
    //     jsr sub_c9393
    sub_c9393();
    // c9363:
c9363:
    //     jsr sub_c93c8
    sub_c93c8();
    //     jsr sub_c93be
    sub_c93be();
    //     beq c937b
    if (flags & FLAG_Z) { c937b(); return; }
    //     stx l0081
    l0081 = x;
    //     sec
    flags |= FLAG_C;
    //     sbc l0081
    { int16_t r = (int16_t)a - (int16_t)l0081 - (1 - ((flags & FLAG_C) ? 1U : 0U)); a = (uint8_t)(r & 0xff); if (r >= 0) { flags |= FLAG_C; } else { flags &= ~FLAG_C; } }
    //     bcc c937b
    if (!(flags & FLAG_C)) { c937b(); return; }
    //     sbc l0039
    { int16_t r = (int16_t)a - (int16_t)l0039 - (1 - ((flags & FLAG_C) ? 1U : 0U)); a = (uint8_t)(r & 0xff); if (r >= 0) { flags |= FLAG_C; } else { flags &= ~FLAG_C; } }
    //     bcc c937b
    if (!(flags & FLAG_C)) { c937b(); return; }
    //     tax
    x = a;
    //     inx
    x++;
    //     jsr sub_c941a
    sub_c941a();
    c937b();
}
static void sub_c9393(void) {
    // sub_c9393:
    //     jsr sub_c93b6
    sub_c93b6();
    //     lda #0
    a = 0;
    //     jmp c93aa
    {
        // c93aa:
        //     clc
        //     adc tmp4
        uint16_t sum = (uint16_t)a + tmp4;
        tmp2 = (uint8_t)sum;
        //     lda tmp5
        a = tmp5;
        //     adc #0
        a += (uint8_t)(sum >> 8);
        tmp3 = a;
    }
}
static void sub_c939b(void) {
    // sub_c939b:
    //     jsr sub_c93b6
    sub_c93b6();
    //     jmp c93a7
    // c93a7:
    //     iny
    y++;
    //     tya
    a = y;
    //     dey
    y--;
    {
        // c93aa:
        uint16_t sum = (uint16_t)a + tmp4;
        tmp2 = (uint8_t)sum;
        a = tmp5;
        a += (uint8_t)(sum >> 8);
        tmp3 = a;
    }
}
static void sub_c93a1(void) {
    // sub_c93a1:
    //     jsr sub_c93b6
    sub_c93b6();
    //     jsr c93b8
    c93b8();
    // c93a7:
    y++;
    a = y;
    y--;
    // c93aa:
    {
        uint16_t sum = (uint16_t)a + tmp4;
        tmp2 = (uint8_t)sum;
        a = tmp5;
        a += (uint8_t)(sum >> 8);
        tmp3 = a;
    }
}
static void sub_c93b6(void) {
    // sub_c93b6:
    //     ldy #0xff
    y = 0xff;
    c93b8();
}
static void c93b8(void) {
    // c93b8:
    //     iny
    //     lda (tmp4),y
    //     bpl c93b8
    do {
        y++;
        a = ram[((uint16_t)tmp5 << 8 | tmp4) + y];
    } while ((int8_t)a >= 0);
    //     rts
}
static void sub_c93be(void) {
    // Pseudocode: Returns ruler_right_stop or l003a-1 as the line width

    // sub_c93be:
    //     lda ruler_right_stop
    a = ruler_right_stop;
    //     bne return_29
    if (!(flags & FLAG_Z)) goto return_29;
    //     lda l003a
    a = l003a;
    //     sec
    flags |= FLAG_C;
    //     sbc #1
    // PROBLEM: sbc #1 (no flag update)
    a -= 1 + (1 - (flags & FLAG_C));
    // return_29:
return_29:
    ; // fallthrough to rts
}
static void sub_c93c8(void) {
    // Pseudocode: Copies header/footer text to output_buffer, expanding register references

    //     rts
    return;

    // sub_c93c8:
    //     ldx #0
    x = 0;
    //     ldy #0
    y = 0;
    //     sty l0081
    l0081 = y;
    // c93ce:
c93ce:
    //     lda (tmp2),y
    a = ram[(uint16_t)tmp2 + y];
    //     bmi c93e6
    if (flags & FLAG_N) goto c93e6;
    //     jsr check_for_control_code
    check_for_control_code();
    //     bne c93d9
    if (!(flags & FLAG_Z)) goto c93d9;
    //     inc l0081
    l0081++;
    // c93d9:
c93d9:
    //     iny
    y++;
    //     cmp #0x7c ; '|'
    { uint16_t tmp_ = a - 0x7c; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x7c ? FLAG_C : 0); }
    //     beq c93f2
    if (flags & FLAG_Z) goto c93f2;
    //     sta output_buffer,x
    output_buffer[x] = a;
    //     inx
    x++;
    //     cpx #MAX_LINE_LENGTH
    { uint16_t tmp_ = x - MAX_LINE_LENGTH; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= MAX_LINE_LENGTH ? FLAG_C : 0); }
    //     bcc c93ce
    if (!(flags & FLAG_C)) goto c93ce;
    // c93e6:
c93e6:
    //     stx l0084
    l0084 = x;
    //     lda print_flags
    a = print_flags;
    //     bpl return_30
    if (!(flags & FLAG_N)) goto return_30;
    //     txa
    a = x;
    //     sec
    flags |= FLAG_C;
    //     sbc l0081
    // PROBLEM: sbc l0081 (no flag update)
    a -= l0081 + (1 - (flags & FLAG_C));
    //     tax
    x = a;
    // return_30:
return_30:
    //     rts
    return;

    // c93f2:
c93f2:
    //     lda (tmp2),y
    a = ram[(uint16_t)tmp2 + y];
    //     bmi c93e6
    if (flags & FLAG_N) goto c93e6;
    //     iny
    y++;
    //     jsr render_register
    render_register();
    //     jmp c93ce
    goto c93ce;
}
static void sub_c93fd(void) {
    // Pseudocode: Checks two_sided_flag and returns page parity for alternate layout

    // sub_c93fd:
    //     sec
    flags |= FLAG_C;
    //     lda two_sided_flag
    a = two_sided_flag;
    //     beq return_31
    if (flags & FLAG_Z) goto return_31;
    //     lda register_value_p
    a = register_value_p[0];
    //     lsr
    flags = (flags & ~FLAG_C) | (a & 1);
    a >>= 1;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    // return_31:
return_31:
    ; // fallthrough to rts
}
static void sub_c9407(void) {
    // Pseudocode: Outputs left margin spaces, adjusting for two-sided printing

    //     rts
    return;

    // sub_c9407:
    //     jsr sub_c93fd
    sub_c93fd();
    //     lda left_margin
    a = left_margin;
    //     bcc c9415
    if (!(flags & FLAG_C)) goto c9415;
    //     ldx two_sided_flag
    x = two_sided_flag;
    //     beq c9415
    if (flags & FLAG_Z) goto c9415;
    //     clc
    flags &= ~FLAG_C;
    //     adc rhs_extra_margin
    // PROBLEM: adc without full flag update
    a += rhs_extra_margin + (flags & FLAG_C);
    // c9415:
c9415:
    //     tax
    x = a;
    //     lda #0x20 ; ' '
    a = 0x20;
    //     bne c9426                                                         ; ALWAYS branch
    // PROBLEM: cross-function jump to c9426 in print_vertical_space
}
static void sub_c941a(void) {
    // Pseudocode: Adds extra spaces to x position for centering/justification

    // sub_c941a:
    //     txa
    a = x;
    //     clc
    flags &= ~FLAG_C;
    //     adc l0039
    // PROBLEM: adc without full flag update
    a += l0039 + (flags & FLAG_C);
    //     sta l0039
    l0039 = a;
    //     lda #0x20 ; ' '
    a = 0x20;
    //     bne c9426                                                         ; ALWAYS branch
    // PROBLEM: cross-function jump to c9426 in print_vertical_space
}
static void print_vertical_space(void) {
    // Pseudocode: Prints X number of blank lines (newlines)

    // ; ***************************************************************************************
    // ; On Entry:
    // ;     X: number of lines
    // ; ***************************************************************************************
    // print_vertical_space:
    //     lda #0x0d
    a = 0x0d;
    // c9426:
c9426:
    //     inx
    x++;
    //     dex
    x--;
    //     beq return_32
    if (flags & FLAG_Z) goto return_32;
    // loop_c942a:
loop_c942a:
    //     jsr print_char
    print_char();
    //     dex
    x--;
    //     bne loop_c942a
    if (!(flags & FLAG_Z)) goto loop_c942a;
    // return_32:
return_32:
    //     rts
    return;
}
static void sub_c9431(void) {
    // Pseudocode: Converts character for printing, updates x position counter

    // sub_c9431:
    //     jsr sub_ca5ae
    process_document_character();
    //     bit print_flags
    { uint8_t tmp_ = a & print_flags; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ == 0 ? FLAG_Z : 0) | (print_flags & (FLAG_N|FLAG_V)); }
    //     bpl c943c
    if (!(flags & FLAG_N)) goto c943c;
    //     ora #0
    a |= 0;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bmi return_33                                                     ; ALWAYS branch
    if (flags & FLAG_N) goto return_33;

    // c943c:
c943c:
    //     pha
{   uint8_t saved_a = a;
    //     txa
    a = x;
    //     clc
    flags &= ~FLAG_C;
    //     adc l0039
    // PROBLEM: adc without full flag update
    a += l0039 + (flags & FLAG_C);
    //     sta l0039
    l0039 = a;
    //     pla
    a = saved_a; }
// return_33:
return_33:
    //     rts
    return;
}
static void sub_c9445(void) {
    // Pseudocode: Outputs print_xpos number of spaces to align printer

    // sub_c9445:
    //     pha
{   uint8_t saved_a = a;
    //     lda print_xpos
    a = print_xpos;
    //     beq c9453
    if (a == 0) goto c9453;
    //     lda #0x20 ; ' '
    a = 0x20;
    // loop_c944c:
loop_c944c:
    //     jsr print_char_just_to_printer
    print_char_just_to_printer();
    //     dec print_xpos
    print_xpos--;
    //     bne loop_c944c
    if (print_xpos != 0) goto loop_c944c;
    // c9453:
c9453:
    //     pla
    a = saved_a; }
    //     rts
}
static void print_newline(void) {
    // print_newline:
    //     lda #0x0d
    a = 0x0d;
    print_char();
}
static void print_char(void) {
    // print_char:
    //     cmp #0x0d
    //     beq c9462
    if (a == 0x0d) goto c9462;
    //     cmp #0x20 ; ' '
    //     bne c9468
    if (a != 0x20) goto c9468;
    //     inc print_xpos
    print_xpos++;
    //     rts
    return;

c9462:
    //     lda #0
    a = 0;
    //     sta print_xpos
    print_xpos = 0;
    //     lda #0x0d
    a = 0x0d;
c9468:
    //     jsr sub_c9445
    sub_c9445();
    print_char_just_to_printer();
}
static void print_char_just_to_printer(void) {
    // print_char_just_to_printer:
    //     bit print_flags
    //     bpl c9472
    if (!(print_flags & 0x80)) goto c9472;
    //     jmp (printer_driver_ptr)
    printer_driver_ptr->print_char();
    return;

c9472:
    //     jsr check_for_control_code
    check_for_control_code();
    //     bne c9488
    if (!(flags & FLAG_Z)) goto c9488;
    //     pha
    { uint8_t saved_a = a;
    //     lda #0x2d ; '-'
    a = 0x2d; // '-'
    //     bcs c947e
    if (!(flags & FLAG_C)) a = 0x2a; // '*'
    // c947e:
    //     jsr set_inverted_text_if_not_mode_7
    a = STYLE_REVERSE; screen_setstyle(a);
    //     jsr bdos_print_char
    cli_putchar(a);
    //     pla
    a = saved_a; }
    //     jmp set_normal_text_if_not_mode_7
    a = 0; screen_setstyle(a);
    return;

c9488:
    //     jmp bdos_print_char
    cli_putchar(a);
}
static void prepare_printer_driver(void) {
    // Pseudocode: Sets up printer driver pointer from name or default driver

    // ; ***************************************************************************************
    // prepare_printer_driver:
    //     ldx #<printer_driver_block
    //     ldy #>printer_driver_block
    //     lda printer_driver_name
    //     bne c949e
    //     ldx default_printer_driver_ptr
    //     ldy l94b2
    //     lda #0
    //     sta microspacing_flag
    a = 0;
    microspacing_flag = a;
    printer_driver_ptr = &default_printer_driver;
    // c949e:
    //     stx printer_driver_ptr
    //     sty printer_driver_ptr+1
    // return_35:
    //     rts
}
static void call_printer_driver(void) {
    // Pseudocode: Calls a numbered entry point in the printer driver via struct function pointer

    // ; ***************************************************************************************
    // call_printer_driver:
    //     clc
    //     adc printer_driver_ptr          ; A = byte offset into jump table (0,3,6,9)
    //     sta tmp8
    //     lda printer_driver_ptr+1
    //     adc #0
    //     sta tmp9
    //     jmp (tmp8)
    // Replaced with struct dispatch: convert byte offset to entry index
    switch (a) {
        case 0:  printer_driver_ptr->print_char(); break;
        case 3:  printer_driver_ptr->printer_on(); break;
        case 6:  printer_driver_ptr->printer_off(); break;
        case 9:  printer_driver_ptr->entry3(); break;
    }
}
// Default print_char: write character (chars >= 0x80 are filtered)
static void default_print_char(void) {
    // c94c0:
    //     cmp #0x80
    //     bcs return_35
    if (a >= 0x80) return;
    //     jmp bdos_print_char
    cli_putchar(a);
}

// Default printer_on: init / set mode
static void default_printer_on(void) {
    // c94c7:
    //     lda #2
    a = 2;
    //     jmp default_printer_off
    default_printer_off();
}

// Default printer_off: write char with mode byte
static void default_printer_off(void) {
    // c94cb:
    //     lda #3
    a = 3;
    // c94cd:
    //     jmp oswrch
    // PROBLEM: jmp oswrch (BBC Micro OS call - not available)
}

// Default printer driver entry 3: no-op
static void default_printer_entry3(void) {
}

static const struct printer_driver default_printer_driver = {
    .print_char   = default_print_char,
    .printer_on   = default_printer_on,
    .printer_off  = default_printer_off,
    .entry3       = default_printer_entry3,
};
static void c950f_impl(void) {
    // c950f:
    //     ldy #3
    y = 3;
    //     tax
    x = a;
    //     beq c951c
    if (flags & FLAG_Z) goto c951c;
    //     lda #0x20 ; ' '
    a = 0x20;
    // loop_c9516:
    //     sta (current_format_line_ptr),y
    loop_c9516: ram[current_format_line_ptr + y] = a;
    //     iny
    y++;
    //     dex
    x--;
    //     bne loop_c9516
    if (x != 0) goto loop_c9516;
    // c951c:
    c951c:
    //     lda output_buffer,x
    a = output_buffer[x];
    //     sta (current_format_line_ptr),y
    ram[current_format_line_ptr + y] = a;
    //     iny
    y++;
    //     inx
    x++;
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     bne c951c
    if (!(flags & FLAG_Z)) goto c951c;
    //     inc l0030
    l0030++;
    // c9529:
c9529:
    //     sec
    flags |= FLAG_C;
// return_36:
return_36:
    //     rts
    return;
}
static void lj_fmt_cmd(void) {
    // Pseudocode: Left-justifies the current format line

    // ; ***************************************************************************************
    // lj_fmt_cmd:
    //     jsr expand_line
    expand_line();
    //     bcc return_36
    if (!(flags & FLAG_C)) return;
    //     lda #0
    a = 0;
    //     beq c950f                                                         ; ALWAYS branch
    c950f_impl();
}
static void ce_fmt_cmd(void) {
    // Pseudocode: Centers the current format line

    // ; ***************************************************************************************
    // ce_fmt_cmd:
    //     jsr expand_line
    expand_line();
    //     bcc return_36
    if (!(flags & FLAG_C)) return;
    //     txa
    a = x;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq return_36
    if (flags & FLAG_Z) return;
    //     lsr
    { flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N)) | ((a & 1) ? FLAG_C : 0); a >>= 1; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     sta l0084
    l0084 = a;
    //     lda ruler_right_stop
    a = ruler_right_stop;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq c950f
    if (flags & FLAG_Z) { c950f_impl(); return; }
    //     sec
    flags |= FLAG_C;
    //     sbc ruler_left_stop
    { uint16_t tmp_ = (uint16_t)a - ruler_left_stop - (1 - ((flags & FLAG_C) ? 1 : 0)); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ <= 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     lsr
    { flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N)) | ((a & 1) ? FLAG_C : 0); a >>= 1; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     sec
    flags |= FLAG_C;
    //     adc ruler_left_stop
    { uint16_t tmp_ = (uint16_t)a + ruler_left_stop + ((flags & FLAG_C) ? 1 : 0); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ > 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     sec
    flags |= FLAG_C;
    //     sbc l0084
    { uint16_t tmp_ = (uint16_t)a - l0084 - (1 - ((flags & FLAG_C) ? 1 : 0)); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ <= 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     bcs c950f
    if (flags & FLAG_C) { c950f_impl(); return; }
    //     lda #0
    a = 0;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq c950f                                                         ; ALWAYS branch
    c950f_impl();
}
static void rj_fmt_cmd(void) {
    // Pseudocode: Right-justifies the current format line

    // ; ***************************************************************************************
    // rj_fmt_cmd:
    //     jsr expand_line
    expand_line();
    //     bcc c9529
    if (!(flags & FLAG_C)) { flags |= FLAG_C; return; }
    //     txa
    a = x;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq c9529
    if (flags & FLAG_Z) { flags |= FLAG_C; return; }
    //     dex
    x--;
    //     dex
    x--;
    //     lda #0
    a = 0;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     cpx ruler_right_stop
    { uint16_t tmp_ = x - ruler_right_stop; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= ruler_right_stop ? FLAG_C : 0); }
    //     bcs c950f
    if (flags & FLAG_C) { c950f_impl(); return; }
    //     stx l0083
    l0083 = x;
    //     lda ruler_right_stop
    a = ruler_right_stop;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     sec
    flags |= FLAG_C;
    //     sbc l0083
    { uint16_t tmp_ = (uint16_t)a - l0083 - (1 - ((flags & FLAG_C) ? 1 : 0)); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ <= 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    // c950f: fall-through to shared routine
    c950f_impl(); return;
}
static void expand_line(void) {
    // Pseudocode: Expands a format line into output_buffer, handling register references via |

    // ; ***************************************************************************************
    // expand_line:
    //     ldx #0
    x = 0;
    //     stx l0083
    l0083 = x;
    //     ldy #3
    y = 3;
    //     jsr get_current_fmt_cmd_byte
    get_current_fmt_cmd_byte();
    //     clc
    flags &= ~FLAG_C;
    //     beq return_37
    if (flags & FLAG_Z) return;
    // c9537:
c9537:
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     iny
    y++;
    //     cmp #0x7c ; '|'
    { uint16_t tmp_ = a - 0x7c; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x7c ? FLAG_C : 0); }
    //     beq c955e
    if (flags & FLAG_Z) goto c955e;
    // c953e:
c953e:
    //     sta output_buffer,x
    output_buffer[x] = a;
    //     jsr check_for_control_code
    check_for_control_code();
    //     bne c9548
    if (!(flags & FLAG_Z)) goto c9548;
    //     inc l0083
    l0083++;
    // c9548:
c9548:
    //     inx
    x++;
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     beq c9555
    if (flags & FLAG_Z) goto c9555;
    //     cpx #MAX_LINE_LENGTH-1
    { uint16_t tmp_ = x - (MAX_LINE_LENGTH - 1); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= MAX_LINE_LENGTH - 1 ? FLAG_C : 0); }
    //     bcc c9537
    if (!(flags & FLAG_C)) goto c9537;
    //     lda #0x0d
    a = 0x0d;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bne c953e                                                         ; ALWAYS branch
    goto c953e;

    // c9555:
c9555:
    //     lda print_flags
    a = print_flags;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bpl return_37
    if (!(flags & FLAG_N)) return;
    //     txa
    a = x;
    //     sbc l0083
    { uint16_t tmp_ = (uint16_t)a - l0083 - (1 - ((flags & FLAG_C) ? 1 : 0)); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ <= 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     tax
    x = a;
    // return_37:
return_37:
    //     rts
    return;

    // c955e:
c955e:
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     beq c953e
    if (flags & FLAG_Z) goto c953e;
    //     iny
    y++;
    //     jsr render_register
    render_register();
    //     jmp c9537
    goto c9537;
}
static void sub_c95b2(void) {
    // sub_c95b2:
    //     ldy l0081
    y = l0081;
    //     sta (tmp2),y
    ram[((uint16_t)tmp3 << 8) | (uint16_t)(tmp2 + y)] = a;
    //     iny
    y++;
    //     sty l0081
    l0081 = y;
}
static void c9575(void) {
    //     stx tmp2
    tmp2 = x;
    //     sty tmp3
    tmp3 = y;
    //     lda #0
    a = 0;
    //     sta l0081
    l0081 = a;
    //     sta l007a
    l007a = a;
    //     ldy #3
    y = 3;
    //     sty input_buffer_offset+1
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    //     sta l0083
    l0083 = a;
    //     ldx #0x3f ; '?'
    x = 0x3f;
loop_c9589:
    //     iny
    y++;
    //     sty l0082
    l0082 = y;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | (tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     beq c959c
    if (flags & FLAG_Z) goto c959c;
    //     cmp #0x1b
    { uint16_t tmp_ = a - 0x1b; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | (tmp_ & FLAG_N) | (a >= 0x1b ? FLAG_C : 0); }
    //     bcs c9598
    if (flags & FLAG_C) goto c9598;
    //     lda #0x20 ; ' '
    a = 0x20;
c9598:
    //     cmp l0083
    { uint16_t tmp_ = a - l0083; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | (tmp_ & FLAG_N) | (a >= l0083 ? FLAG_C : 0); }
    //     bne c959e
    if (!(flags & FLAG_Z)) goto c959e;
c959c:
    //     ora #0x80
    a |= 0x80;
c959e:
    //     jsr sub_c95b2
    sub_c95b2();
    //     cmp #0x8d
    { uint16_t tmp_ = a - 0x8d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | (tmp_ & FLAG_N) | (a >= 0x8d ? FLAG_C : 0); }
    //     beq c95aa
    if (flags & FLAG_Z) goto c95aa;
    //     ldy l0082
    y = l0082;
    //     dex
    x--;
    //     bne loop_c9589
    if (x != 0) goto loop_c9589;
c95aa:
    //     lda #0x80
    a = 0x80;
    //     jsr sub_c95b2
    sub_c95b2();
    //     jsr sub_c95b2
    sub_c95b2();

    // MULTIPLE ENTRY POINTS: dh_fmt_cmd, df_fmt_cmd
}
static void df_fmt_cmd(void) {
    // Pseudocode: Stores footer text (shared code with dh_fmt_cmd)

    // ; ***************************************************************************************
    // df_fmt_cmd:
    //     ldx #<(footer_text_maybe)
    //     ldy #>(footer_text_maybe)
    x = (uintptr_t)footer_text_maybe & 0xff;
    y = (uintptr_t)footer_text_maybe >> 8;
    c9575();
}
static void dh_fmt_cmd(void) {
    // Pseudocode: Stores header text (shared code with df_fmt_cmd)

    // ; ***************************************************************************************
    // dh_fmt_cmd:
    //     ldx #<(header_text_maybe)
    //     ldy #>(header_text_maybe)
    //     bne c9575                                                         ; ALWAYS branch
    x = (uintptr_t)header_text_maybe & 0xff;
    y = (uintptr_t)header_text_maybe >> 8;
    c9575();
}
static void em_fmt_cmd(void) {
    // Pseudocode: Evaluates expression and stores result in a register

    // ; ***************************************************************************************
    // em_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr get_current_fmt_cmd_byte
    get_current_fmt_cmd_byte();
    //     beq return_38
    if (flags & FLAG_Z) return;
    //     iny
    y++;
    //     jsr get_register_address
    get_register_address();
    //     bcs return_38
    if (flags & FLAG_C) return;
    //     lda tmp6
    a = tmp6;
    //     sta tmp0
    tmp0 = a;
    //     lda tmp7
    a = tmp7;
    //     sta tmp1
    tmp1 = a;
    //     jsr evaluate_expression_from_fmt_cmd
    evaluate_expression_from_fmt_cmd();
    //     ldy #0
    y = 0;
    //     sta (tmp0),y
    ram[((uint16_t)tmp1 << 8) | (uint16_t)(tmp0 + y)] = a;
    //     iny                                                               ; Y=0x01
    y++;
    //     lda tmp9
    a = tmp9;
    //     sta (tmp0),y
    ram[((uint16_t)tmp1 << 8) | (uint16_t)(tmp0 + y)] = a;
    // return_38:
    //     rts
    return;
}
static void pl_fmt_cmd(void) {
    // Pseudocode: Sets page_length from format command expression

    // ; ***************************************************************************************
    // pl_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr evaluate_expression_from_fmt_cmd
    evaluate_expression_from_fmt_cmd();
    //     sta page_length
    page_length = a;
    //     rts
    return;
}
static void ts_fmt_cmd(void) {
    // Pseudocode: Sets two_sided_flag and rhs_extra_margin from format command

    // ; ***************************************************************************************
    // ts_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr parse_boolean_from_fmt_cmd
    parse_boolean_from_fmt_cmd();
    //     bcs return_39
    if (flags & FLAG_C) return;
    //     sta two_sided_flag
    two_sided_flag = a;
    //     jsr evaluate_expression_from_fmt_cmd
    evaluate_expression_from_fmt_cmd();
    //     sta rhs_extra_margin
    rhs_extra_margin = a;
    // return_39:
    //     rts
    return;
}
static void tm_fmt_cmd(void) {
    // Pseudocode: Sets top_margin from format command expression

    // ; ***************************************************************************************
    // tm_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr evaluate_expression_from_fmt_cmd
    evaluate_expression_from_fmt_cmd();
    //     sta top_margin
    top_margin = a;
    //     rts
    return;
}
static void bm_fmt_cmd(void) {
    // Pseudocode: Sets bottom_margin from format command expression

    // ; ***************************************************************************************
    // bm_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr evaluate_expression_from_fmt_cmd
    evaluate_expression_from_fmt_cmd();
    //     sta bottom_margin
    bottom_margin = a;
    //     rts
    return;
}
static void hm_fmt_cmd(void) {
    // Pseudocode: Sets header_margin from format command expression

    // ; ***************************************************************************************
    // hm_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr evaluate_expression_from_fmt_cmd
    evaluate_expression_from_fmt_cmd();
    //     sta header_margin
    header_margin = a;
    //     rts
    return;
}
static void fm_fmt_cmd(void) {
    // Pseudocode: Sets footer_margin from format command expression

    // ; ***************************************************************************************
    // fm_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr evaluate_expression_from_fmt_cmd
    evaluate_expression_from_fmt_cmd();
    //     sta footer_margin
    footer_margin = a;
    //     rts
    return;
}
static void lm_fmt_cmd(void) {
    // Pseudocode: Sets left_margin from format command expression

    // ; ***************************************************************************************
    // lm_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr evaluate_expression_from_fmt_cmd
    evaluate_expression_from_fmt_cmd();
    //     sta left_margin
    left_margin = a;
    //     rts
    return;
}
static void ls_fmt_cmd(void) {
    // Pseudocode: Sets line_spacing from format command expression

    // ; ***************************************************************************************
    // ls_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr evaluate_expression_from_fmt_cmd
    evaluate_expression_from_fmt_cmd();
    //     sta line_spacing
    line_spacing = a;
    //     rts
    return;
}
static void pe_fmt_cmd(void) {
    // Pseudocode: Forces page eject if remaining lines are less than value

    // ; ***************************************************************************************
    // pe_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr evaluate_expression_from_fmt_cmd
    evaluate_expression_from_fmt_cmd();
    //     tax
    x = a;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     beq page_eject_fmt
    if (flags & FLAG_Z) { page_eject_fmt(); return; }
    //     cmp l0021
    { uint16_t tmp_ = a - l0021; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= l0021 ? FLAG_C : 0); }
    //     bcc return_40
    if (!(flags & FLAG_C)) return;
    //     lda l0031
    a = l0031;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bne page_eject_fmt
    if (!(flags & FLAG_Z)) { page_eject_fmt(); return; }
    // return_40:
    //     rts
    return;
}
static void op_fmt_cmd(void) {
    // Pseudocode: Odd page: forces page eject if current page is even

    // ; ***************************************************************************************
    // op_fmt_cmd:
    //     lda register_value_p
    a = *register_value_p;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     lsr
    { flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N)) | ((a & 1) ? FLAG_C : 0); a >>= 1; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     bcc page_eject_fmt
    if (!(flags & FLAG_C)) { page_eject_fmt(); return; }
    //     bcs c9642                                                         ; ALWAYS branch
    return;
}
static void ep_fmt_cmd(void) {
    // Pseudocode: Even page: forces page eject if current page is odd

    // ; ***************************************************************************************
    // ep_fmt_cmd:
    //     lda register_value_p
    a = *register_value_p;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     lsr
    { flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N)) | ((a & 1) ? FLAG_C : 0); a >>= 1; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     bcs page_eject_fmt
    if (flags & FLAG_C) { page_eject_fmt(); return; }
// c9642:
    //     rts
    return;
}
static void page_eject_fmt(void) {
    // Pseudocode: Performs page eject by rendering new page and moving to sheet bottom

    //     jsr page_eject_fmt
    // ; ***************************************************************************************
    // page_eject_fmt:
    //     lda l0031
    a = l0031;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bne c964c
    if (!(flags & FLAG_Z)) goto c964c;
    //     jsr render_new_page
    render_new_page();
    // c964c:
c964c:
    //     jmp c9263
    c9263();
    return;
}
static void fo_fmt_cmd(void) {
    // Pseudocode: Sets footers_enabled_flag from boolean format argument

    // ; ***************************************************************************************
    // fo_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr parse_boolean_from_fmt_cmd
    parse_boolean_from_fmt_cmd();
    //     bcs return_41
    if (flags & FLAG_C) return;
    //     sta footers_enabled_flag
    footers_enabled_flag = a;
    // return_41:
    //     rts
    return;
}
static void he_fmt_cmd(void) {
    // Pseudocode: Sets headers_enabled_flag from boolean format argument

    // ; ***************************************************************************************
    // he_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr parse_boolean_from_fmt_cmd
    parse_boolean_from_fmt_cmd();
    //     bcs return_42
    if (flags & FLAG_C) return;
    //     sta headers_enabled_flag
    headers_enabled_flag = a;
    // return_42:
    //     rts
    return;
}
static void pb_fmt_cmd(void) {
    // Pseudocode: Sets page break flag l0038 from boolean format argument

    // ; ***************************************************************************************
    // pb_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr parse_boolean_from_fmt_cmd
    parse_boolean_from_fmt_cmd();
    //     bcs return_43
    if (flags & FLAG_C) return;
    //     sta l0038
    l0038 = a;
    // return_43:
    //     rts
    return;
}
static void dm_fmt_cmd(void) {
    // Pseudocode: Defines a macro: stores macro name and position in linked list

    // dm_fmt_cmd:
    //     lda macro_executing_flag
    a = macro_executing_flag;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bne return_42
    if (!(flags & FLAG_Z)) return;
    //     lda last_macro_ptr
    a = (uint8_t)(last_macro_ptr & 0xff);
    //     sta tmp6
    tmp6 = a;
    //     lda last_macro_ptr+1
    a = (uint8_t)((last_macro_ptr >> 8) & 0xff);
    //     sta tmp7
    tmp7 = a;
    //     ldy #3
    y = 3;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     and #0xdf
    a &= 0xdf;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     sta l0084
    l0084 = a;
    //     iny                                                               ; Y=0x04
    y++;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     jsr is_uppercase
    is_uppercase();
    //     bcc c968d
    if (!(flags & FLAG_C)) goto c968d;
    //     lda #0x20 ; ' '
    a = 0x20;
    //     bne c968f                                                         ; ALWAYS branch
    goto c968f;

    // c968d:
c968d:
    //     and #0xdf
    a &= 0xdf;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    // c968f:
c968f:
    //     dey
    y--;
    //     sta (last_macro_ptr),y
    ram[last_macro_ptr + y] = a;
    //     dey
    y--;
    //     lda l0084
    a = l0084;
    //     sta (last_macro_ptr),y
    ram[last_macro_ptr + y] = a;
    //     lda #4
    a = 4;
    //     clc
    flags &= ~FLAG_C;
    //     adc last_macro_ptr
    { uint16_t tmp_ = (uint16_t)a + (uint8_t)(last_macro_ptr & 0xff) + ((flags & FLAG_C) ? 1 : 0); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ > 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     sta last_macro_ptr
    last_macro_ptr = (last_macro_ptr & 0xff00) | a;
    //     bcc c96a2
    if (!(flags & FLAG_C)) goto c96a2;
    //     inc last_macro_ptr+1
    last_macro_ptr = (last_macro_ptr & 0x00ff) | ((uint16_t)((last_macro_ptr >> 8) + 1) << 8);
    // c96a2:
c96a2:
    //     lda himem
    a = (uint8_t)(himem & 0xff);
    //     sec
    flags |= FLAG_C;
    //     sbc last_macro_ptr
    { uint16_t tmp_ = (uint16_t)a - (uint8_t)(last_macro_ptr & 0xff) - (1 - ((flags & FLAG_C) ? 1 : 0)); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ <= 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     tax
    x = a;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     lda himem+1
    a = (uint8_t)((himem >> 8) & 0xff);
    //     sbc last_macro_ptr+1
    { uint16_t tmp_ = (uint16_t)a - (uint8_t)((last_macro_ptr >> 8) & 0xff) - (1 - ((flags & FLAG_C) ? 1 : 0)); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ <= 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     bne c96b8
    if (!(flags & FLAG_Z)) goto c96b8;
    //     cpx #0x97
    { uint16_t tmp_ = x - 0x97; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= 0x97 ? FLAG_C : 0); }
    //     bcs c96b8
    if (flags & FLAG_C) goto c96b8;
    //     jmp display_not_enough_memory
    { display_not_enough_memory(); return; }

    // c96b8:
c96b8:
    //     lda last_macro_ptr
    a = (uint8_t)(last_macro_ptr & 0xff);
    //     sta tmp0
    tmp0 = a;
    //     sta input_buffer_offset+1
    //     sta current_format_line_ptr
    current_format_line_ptr = (current_format_line_ptr & 0xff00) | a;
    //     lda last_macro_ptr+1
    a = (uint8_t)((last_macro_ptr >> 8) & 0xff);
    //     sta tmp1
    tmp1 = a;
    //     sta l0081
    l0081 = a;
    //     sta current_format_line_ptr+1
    current_format_line_ptr = (current_format_line_ptr & 0x00ff) | ((uint16_t)a << 8);
    //     jsr sub_c9241
    sub_c9241();
    //     bcc c96ce
    if (!(flags & FLAG_C)) goto c96ce;
    //     rts
    return;

    // c96ce:
c96ce:
    //     ldy #0
    y = 0;
    //     lda (last_macro_ptr),y
    a = ram[last_macro_ptr + y];
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     jsr check_for_command_prefix
    check_for_command_prefix();
    //     bne c96f8
    if (!(flags & FLAG_Z)) { last_macro_ptr = (uint16_t)tmp0 | ((uint16_t)tmp1 << 8); return; }
    //     jsr lookup_formatting_command
    lookup_formatting_command();
    //     cpx #5
    { uint16_t tmp_ = x - 5; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= 5 ? FLAG_C : 0); }
    //     bne c96f8
    if (!(flags & FLAG_Z)) { last_macro_ptr = (uint16_t)tmp0 | ((uint16_t)tmp1 << 8); return; }
    //     lda #4
    a = 4;
    //     ldy #0
    y = 0;
    //     sta (last_macro_ptr),y
    ram[last_macro_ptr + y] = a;
    //     inc last_macro_ptr
    last_macro_ptr = (last_macro_ptr & 0xff00) | ((uint8_t)(last_macro_ptr & 0xff) + 1);
    //     bne add_macro_to_linked_list
    if ((uint8_t)(last_macro_ptr & 0xff) != 0) goto add_macro_to_linked_list;
    //     inc last_macro_ptr+1
    last_macro_ptr = (last_macro_ptr & 0x00ff) | ((uint16_t)((last_macro_ptr >> 8) + 1) << 8);
    // add_macro_to_linked_list:
add_macro_to_linked_list:
    //     lda #0
    a = 0;
    //     sta (last_macro_ptr),y
    ram[last_macro_ptr + y] = a;
    //     lda last_macro_ptr
    a = (uint8_t)(last_macro_ptr & 0xff);
    //     sta (tmp6),y
    ram[((uint16_t)tmp7 << 8) | (uint16_t)(tmp6 + y)] = a;
    //     iny
    y++;
    //     lda last_macro_ptr+1
    a = (uint8_t)((last_macro_ptr >> 8) & 0xff);
    //     sta (tmp6),y
    ram[((uint16_t)tmp7 << 8) | (uint16_t)(tmp6 + y)] = a;
    //     rts
    return;
}
static void ht_fmt_cmd(void) {
    // Pseudocode: Sets highlight codes (highlight1_code, highlight2_code) from format command

    //     bne c96a2
    // ; ***************************************************************************************
    // ht_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr get_current_fmt_cmd_byte
    get_current_fmt_cmd_byte();
    //     beq return_44
    if (flags & FLAG_Z) return;
    //     tax
    x = a;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     lda #0
    a = 0;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     cpx #0x2d ; '-'
    { uint16_t tmp_ = x - 0x2d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= 0x2d ? FLAG_C : 0); }
    //     beq c9716
    if (flags & FLAG_Z) goto c9716;
    //     lda #1
    a = 1;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     cpx #0x2a ; '*'
    { uint16_t tmp_ = x - 0x2a; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= 0x2a ? FLAG_C : 0); }
    //     bne c9719
    if (!(flags & FLAG_Z)) goto c9719;
    // c9716:
c9716:
    //     iny
    y++;
    //     bne c9725
    goto c9725;
    // c9719:
c9719:
    //     jsr evaluate_expression_from_fmt_cmd
    evaluate_expression_from_fmt_cmd();
    //     sec
    flags |= FLAG_C;
    //     sbc #1
    { uint16_t tmp_ = (uint16_t)a - 1 - (1 - ((flags & FLAG_C) ? 1 : 0)); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ <= 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     bcc return_44
    if (!(flags & FLAG_C)) return;
    //     cmp #2
    { uint16_t tmp_ = a - 2; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 2 ? FLAG_C : 0); }
    //     bcs return_44
    if (flags & FLAG_C) return;
    // c9725:
c9725:
    //     pha
    { uint8_t saved_a = a;
    //     jsr evaluate_expression_from_fmt_cmd
    evaluate_expression_from_fmt_cmd();
    //     pla
    a = saved_a; }
    //     tax
    x = a;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     lda tmp8
    a = tmp8;
    //     sta highlight1_code,x
    highlight_code[x] = a;
    // return_44:
return_44:
    //     rts
    return;
}
static const uint8_t commands_table[] = { 'C','E', 'R','J', 'D','F', 'D','H', 'D','M', 'E','M', 'S','R', 'P','E', 'T','M', 'B','M', 'P','L', 'T','S', 'F','O', 'H','E', 'H','T', 'H','M', 'F','M', 'L','M', 'L','S', 'O','P', 'E','P', 'L','J', 'P','B', 0xff };
static void lookup_formatting_command(void) {
    // Pseudocode: Looks up two-letter formatting command in commands_table

    // ; ***************************************************************************************
    // lookup_formatting_command:
    //     ldy #2
    y = 2;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     sta tmp3
    tmp3 = a;
    //     dey                                                               ; Y=0x01
    y--;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     sta tmp2
    tmp2 = a;
    //     dey                                                               ; Y=0x00
    y--;
    //     ldx #0
    x = 0;
    // loop_c973e:
loop_c973e:
    //     lda tmp2
    a = tmp2;
    //     cmp commands_table,y
    { uint16_t tmp_ = a - commands_table[y]; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= commands_table[y] ? FLAG_C : 0); }
    //     bne c974c
    if (!(flags & FLAG_Z)) goto c974c;
    //     lda tmp3
    a = tmp3;
    //     cmp lb2a1,y                                                    ; lb2a1 = commands_table+1
    { uint16_t tmp_ = a - commands_table[y + 1]; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= commands_table[y + 1] ? FLAG_C : 0); }
    //     beq return_45
    if (flags & FLAG_Z) return;
    // c974c:
c974c:
    //     inx
    x++;
    //     iny
    y++;
    //     iny
    y++;
    //     lda commands_table,y
    a = commands_table[y];
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bpl loop_c973e
    if (!(flags & FLAG_N)) goto loop_c973e;
    // return_45:
return_45:
    //     rts
    return;
}
static void execute_formatting_command(void) {
    // Pseudocode: Executes a formatting command by index through the format jump table

    // ; ***************************************************************************************
    // execute_formatting_command:
    //     txa
    a = x;
    //     ldy #0
    y = 0;
    //     ldx #0
    x = 0;
    //     stx l0030
    l0030 = x;
    //     jsr call_through_jumptable
    call_through_jumptable();
    //     ldx l0030
    x = l0030;
    //     rts
    return;
}
static void parse_boolean_from_fmt_cmd(void) {
    // Pseudocode: Parses a boolean (ON/OFF/1/0) from format command argument

    // ; ***************************************************************************************
    // parse_boolean_from_fmt_cmd:
    //     jsr get_current_fmt_cmd_byte
    get_current_fmt_cmd_byte();
    //     sec
    flags |= FLAG_C;
    //     beq return_46
    if (flags & FLAG_Z) return;
    //     lda current_format_line_ptr
    a = current_format_line_ptr;
    //     ldx current_format_line_ptr+1
    x = current_format_line_ptr >> 8;

    // MULTIPLE ENTRY POINTS: parse_boolean_from_fmt_cmd, sub_c976c
    sub_c976c();
}
static const uint8_t l97b0_data[] = { 0x4f, 0x4e, 1, 'O', 'F', 'F', 0, 0xff };

static void sub_c976c(void) {
    // Pseudocode: Parses word-based flag (ON/OFF/YES/NO) from format command

    // sub_c976c:
    //     sta tmp8
    tmp8 = a;
    //     stx tmp9
    tmp9 = x;
    //     lda (tmp8),y
    a = ram[((uint16_t)tmp9 << 8) | (uint16_t)(tmp8 + y)];
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     tax
    x = a;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     lda #1
    a = 1;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     cpx #0x31 ; '1'
    { uint16_t tmp_ = x - 0x31; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= 0x31 ? FLAG_C : 0); }
    //     beq c977f
    if (flags & FLAG_Z) goto c977f;
    //     lda #0
    a = 0;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     cpx #0x30 ; '0'
    { uint16_t tmp_ = x - 0x30; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= 0x30 ? FLAG_C : 0); }
    //     bne c9783
    if (!(flags & FLAG_Z)) goto c9783;
    // c977f:
c977f:
    //     clc
    flags &= ~FLAG_C;
    //     iny
    y++;
    //     bne return_46
    if (y != 0) return;
    // c9783:
c9783:
    //     dey
    y--;
    //     sty l0084
    l0084 = y;
    //     ldx #0xff
    x = 0xff;
    // c9788:
c9788:
    //     iny
    y++;
    //     lda (tmp8),y
    a = ram[((uint16_t)tmp9 << 8) | (uint16_t)(tmp8 + y)];
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     jsr to_uppercase
    to_uppercase();
    //     inx
    x++;
    //     cmp l97b0,x
    { uint16_t tmp_ = a - l97b0_data[x]; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= l97b0_data[x] ? FLAG_C : 0); }
    //     beq c9788
    if (flags & FLAG_Z) goto c9788;
    //     lda l97b0,x
    a = l97b0_data[x];
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bmi c97ae
    if (flags & FLAG_N) goto c97ae;
    //     cmp #0x20 ; ' '
    { uint16_t tmp_ = a - 0x20; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x20 ? FLAG_C : 0); }
    //     bcc return_46
    if (!(flags & FLAG_C)) return;
    // loop_c979d:
loop_c979d:
    //     inx
    x++;
    //     lda l97b0,x
    a = l97b0_data[x];
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bmi c97ae
    if (flags & FLAG_N) goto c97ae;
    //     cmp #0x20 ; ' '
    { uint16_t tmp_ = a - 0x20; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x20 ? FLAG_C : 0); }
    //     bcs loop_c979d
    if (flags & FLAG_C) goto loop_c979d;
    //     ldy l0084
    y = l0084;
    //     lda l97b1,x
    a = l97b0_data[x+1];
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bpl c9788
    if (!(flags & FLAG_N)) goto c9788;
    // c97ae:
c97ae:
    //     sec
    flags |= FLAG_C;
    // return_46:
return_46:
    //     rts
    return;

    // MULTIPLE ENTRY POINTS: parse_boolean_from_fmt_cmd, sub_c976c
}
static void evaluate_expression_from_fmt_cmd(void) {
    // Pseudocode: Evaluates arithmetic expression with +, - and register references

    // l97b0:
    //     .byte 0x4f
    // l97b1:
    //     .byte 0x4e, 1
    //     .ascii "OFF"
    //     .byte 0, 0xff

    // ; ***************************************************************************************
    // evaluate_expression_from_fmt_cmd:
    //     lda #0
    a = 0;
    //     sta tmp8
    tmp8 = a;
    //     sta tmp9
    tmp9 = a;
    //     sta input_buffer_offset+1
    // c97c0:
c97c0:
    //     jsr get_current_fmt_cmd_byte
    get_current_fmt_cmd_byte();
    //     beq c9821
    if (flags & FLAG_Z) goto c9821;
    //     cmp #0x7c ; '|'
    { uint16_t tmp_ = a - 0x7c; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x7c ? FLAG_C : 0); }
    //     bne c97d5
    if (!(flags & FLAG_Z)) goto c97d5;
    //     jsr get_next_fmt_cmd_byte
    get_next_fmt_cmd_byte();
    //     beq c9821
    if (flags & FLAG_Z) goto c9821;
    //     iny
    y++;
    //     jsr render_register
    render_register();
    //     jmp c97dc
    goto c97dc;

    // c97d5:
c97d5:
    //     jsr ca6fe
    parse_decimal_number();
    //     sta tmp8
    tmp8 = a;
    //     stx tmp9
    tmp9 = x;
    // c97dc:
c97dc:
    //     ldx input_buffer_offset+1
    x = 0;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     beq c9804
    if (flags & FLAG_Z) goto c9804;
    //     lda #0
    a = 0;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     sta input_buffer_offset+1
    //     dex
    x--;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     beq c97f7
    if (flags & FLAG_Z) goto c97f7;
    //     lda tmp4
    a = tmp4;
    //     sec
    flags |= FLAG_C;
    //     sbc tmp8
    { uint16_t tmp_ = (uint16_t)a - tmp8 - (1 - ((flags & FLAG_C) ? 1 : 0)); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ <= 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     sta tmp8
    tmp8 = a;
    //     lda tmp5
    a = tmp5;
    //     sbc tmp9
    { uint16_t tmp_ = (uint16_t)a - tmp9 - (1 - ((flags & FLAG_C) ? 1 : 0)); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ <= 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     sta tmp9
    tmp9 = a;
    //     jmp c9804
    goto c9804;

    // c97f7:
c97f7:
    //     lda tmp4
    a = tmp4;
    //     clc
    flags &= ~FLAG_C;
    //     adc tmp8
    { uint16_t tmp_ = (uint16_t)a + tmp8 + ((flags & FLAG_C) ? 1 : 0); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ > 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     sta tmp8
    tmp8 = a;
    //     lda tmp5
    a = tmp5;
    //     adc tmp9
    { uint16_t tmp_ = (uint16_t)a + tmp9 + ((flags & FLAG_C) ? 1 : 0); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ > 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     sta tmp9
    tmp9 = a;
    // c9804:
c9804:
    //     lda tmp8
    a = tmp8;
    //     sta tmp4
    tmp4 = a;
    //     lda tmp9
    a = tmp9;
    //     sta tmp5
    tmp5 = a;
    //     jsr get_current_fmt_cmd_byte
    get_current_fmt_cmd_byte();
    //     beq c9821
    if (flags & FLAG_Z) goto c9821;
    //     ldx #1
    x = 1;
    //     cmp #0x2b ; '+'
    { uint16_t tmp_ = a - 0x2b; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x2b ? FLAG_C : 0); }
    //     beq c981c
    if (flags & FLAG_Z) goto c981c;
    //     inx                                                               ; X=0x02
    x++;
    //     cmp #0x2d ; '-'
    { uint16_t tmp_ = a - 0x2d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x2d ? FLAG_C : 0); }
    //     bne c9821
    if (!(flags & FLAG_Z)) goto c9821;
    // c981c:
c981c:
    //     stx input_buffer_offset+1
    //     iny
    y++;
    //     bne c97c0
    goto c97c0;
    // c9821:
c9821:
    //     lda tmp8
    a = tmp8;
    //     rts
    return;
}
static void get_current_fmt_cmd_byte(void) {
    // get_current_fmt_cmd_byte:
    while (1) {
        //     lda (current_format_line_ptr),y
        a = ram[current_format_line_ptr + y];
        //     cmp #0x0d
        //     beq return_47
        if (a == 0x0d) return;
        //     cmp #0x20 ; ' '
        //     beq get_next_fmt_cmd_byte
        if (a != 0x20) return;
        y++;
    }
    // return_47:
    //     rts
}
static void get_next_fmt_cmd_byte(void) {
    // get_next_fmt_cmd_byte:
    //     iny
    y++;
    get_current_fmt_cmd_byte();
}
static void sub_c9830(void) {
    // Pseudocode: Word-spacing justification: distributes extra spaces between words

    // sub_c9830:
    //     lda justifying_flag
    a = justifying_flag;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bne return_47
    if (!(flags & FLAG_Z)) return;
    //     sta l0046
    l0046 = a;
    //     sta l0039
    l0039 = a;
    //     sta l0042
    l0042 = a;
    //     lda ruler_right_stop
    a = ruler_right_stop;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq return_47
    if (flags & FLAG_Z) return;
    //     jsr get_line_length
    get_line_length();
    //     sty l0043
    l0043 = y;
    //     ldy #0
    y = 0;
    //     beq c9861                                                         ; ALWAYS branch
    goto c9861;

    // c9847:
c9847:
    //     lda l0039
    a = l0039;
    //     sta l0084
    l0084 = a;
    //     iny
    y++;
    //     cpy l0043
    { uint16_t tmp_ = y - l0043; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= l0043 ? FLAG_C : 0); }
    //     beq c9871
    if (flags & FLAG_Z) goto c9871;
    //     clc
    flags &= ~FLAG_C;
    //     jsr sub_c9936
    sub_c9936();
    //     beq c985c
    if (flags & FLAG_Z) goto c985c;
    //     cmp #0x20 ; ' '
    { uint16_t tmp_ = a - 0x20; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x20 ? FLAG_C : 0); }
    //     bne c9847
    if (!(flags & FLAG_Z)) goto c9847;
    //     inc l0046
    l0046++;
    // c985c:
c985c:
    //     iny
    y++;
    //     cpy l0043
    { uint16_t tmp_ = y - l0043; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= l0043 ? FLAG_C : 0); }
    //     beq c986d
    if (flags & FLAG_Z) goto c986d;
    // c9861:
c9861:
    //     sec
    flags |= FLAG_C;
    //     jsr sub_c9936
    sub_c9936();
    //     beq c985c
    if (flags & FLAG_Z) goto c985c;
    //     cmp #0x20 ; ' '
    { uint16_t tmp_ = a - 0x20; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x20 ? FLAG_C : 0); }
    //     bne c9847
    if (!(flags & FLAG_Z)) goto c9847;
    //     beq c985c                                                         ; ALWAYS branch
    goto c985c;

    // c986d:
c986d:
    //     dec l0046
    l0046--;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (l0046 == 0 ? FLAG_Z : 0) | (l0046 & FLAG_N);
    //     bmi return_47
    if (flags & FLAG_N) return;
    // c9871:
c9871:
    //     lda l0046
    a = l0046;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq return_47
    if (flags & FLAG_Z) return;
    //     lda ruler_right_stop
    a = ruler_right_stop;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     sec
    flags |= FLAG_C;
    //     sbc l0084
    { uint16_t tmp_ = (uint16_t)a - l0084 - (1 - ((flags & FLAG_C) ? 1 : 0)); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ <= 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     bcc return_47
    if (!(flags & FLAG_C)) return;
    //     adc #0
    { uint16_t tmp_ = (uint16_t)a + 0 + ((flags & FLAG_C) ? 1 : 0); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ > 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     tax
    x = a;
    //     adc l0043
    { uint16_t tmp_ = (uint16_t)a + l0043 + ((flags & FLAG_C) ? 1 : 0); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ > 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     sec
    flags |= FLAG_C;
    //     sbc #0x84
    { uint16_t tmp_ = (uint16_t)a - 0x84 - (1 - ((flags & FLAG_C) ? 1 : 0)); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ <= 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     bcc c988c
    if (!(flags & FLAG_C)) goto c988c;
    //     sta l0084
    l0084 = a;
    //     txa
    a = x;
    //     sbc l0084
    { uint16_t tmp_ = (uint16_t)a - l0084 - (1 - ((flags & FLAG_C) ? 1 : 0)); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ <= 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     tax
    x = a;
    // c988c:
c988c:
    //     stx l0082
    l0082 = x;
    //     stx tmp8
    tmp8 = x;
    //     lda #0
    a = 0;
    //     sta tmp9
    tmp9 = a;
    //     jsr sub_cadf0
    sub_cadf0();
    //     sta l0045
    l0045 = a;
    //     lda tmp8
    a = tmp8;
    //     sta l0044
    l0044 = a;
    //     ldy #0
    y = 0;
    //     ldx l0046
    x = l0046;
    //     tya                                                               ; A=0x00
    a = y;
    // loop_c98a2:
    //     sta input_buffer,y
loop_c98a2:
    input_buffer[y] = a;
    //     iny
    y++;
    //     dex
    x--;
    //     bne loop_c98a2
    if (x != 0) goto loop_c98a2;
    //     ldy print_xpos
    y = print_xpos;
    //     iny
    y++;
    //     cpy l0046
    { uint16_t tmp_ = y - l0046; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= l0046 ? FLAG_C : 0); }
    //     bcc c98b2
    if (!(flags & FLAG_C)) goto c98b2;
    //     ldy #1
    y = 1;
    // c98b2:
c98b2:
    //     dey
    y--;
    //     ldx l0046
    x = l0046;
    // c98b5:
c98b5:
    //     lda l0045
    a = l0045;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq c98bd
    if (flags & FLAG_Z) goto c98bd;
    //     lda #1
    a = 1;
    //     dec l0045
    l0045--;
    // c98bd:
c98bd:
    //     clc
    flags &= ~FLAG_C;
    //     adc l0044
    { uint16_t tmp_ = (uint16_t)a + l0044 + ((flags & FLAG_C) ? 1 : 0); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ > 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     sta input_buffer,y
    input_buffer[y] = a;
    //     lda l0082
    a = l0082;
    //     sec
    flags |= FLAG_C;
    //     sbc input_buffer,y
    { uint16_t tmp_ = (uint16_t)a - input_buffer[y] - (1 - ((flags & FLAG_C) ? 1 : 0)); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ <= 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     php
    { uint8_t saved_flags = flags;
    //     sta l0082
    l0082 = a;
    //     iny
    y++;
    //     cpy l0046
    { uint16_t tmp_ = y - l0046; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= l0046 ? FLAG_C : 0); }
    //     bcc c98d3
    if (!(flags & FLAG_C)) goto c98d3;
    //     ldy #0
    y = 0;
    // c98d3:
c98d3:
    //     plp
    flags = saved_flags; }
    //     beq c98d9
    if (flags & FLAG_Z) goto c98d9;
    //     dex
    x--;
    //     bne c98b5
    if (x != 0) goto c98b5;
    // c98d9:
c98d9:
    //     sty print_xpos
    print_xpos = y;
    //     ldy #0
    y = 0;
    //     sty l0081
    l0081 = y;
    //     sty l0039
    l0039 = y;
    //     lda #0x1a
    a = 0x1a;
    //     jsr wipe_buffer
    wipe_buffer();
    //     lda l0042
    a = l0042;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq c98f6
    if (flags & FLAG_Z) goto c98f6;
    //     ldy #0
    y = 0;
    // loop_c98ec:
    //     lda output_buffer,y
loop_c98ec:
    a = output_buffer[y];
    //     sta (current_edit_line_ptr),y
    ram[current_edit_line_ptr + y] = a;
    //     iny
    y++;
    //     cpy l0042
    { uint16_t tmp_ = y - l0042; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= l0042 ? FLAG_C : 0); }
    //     bne loop_c98ec
    if (!(flags & FLAG_Z)) goto loop_c98ec;
    // c98f6:
c98f6:
    //     ldy l0042
    y = l0042;
    //     ldx l0042
    x = l0042;
    // c98fa:
c98fa:
    //     lda output_buffer,x
    a = output_buffer[x];
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     cmp #0x20 ; ' '
    { uint16_t tmp_ = a - 0x20; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x20 ? FLAG_C : 0); }
    //     bne c9920
    if (!(flags & FLAG_Z)) goto c9920;
    //     lda l0081
    a = l0081;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq c991c
    if (flags & FLAG_Z) goto c991c;
    //     sty l0084
    l0084 = y;
    //     ldy l0039
    y = l0039;
    //     cpy l0046
    { uint16_t tmp_ = y - l0046; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= l0046 ? FLAG_C : 0); }
    //     lda #0
    a = 0;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bcs c9912
    if (flags & FLAG_C) goto c9912;
    //     lda input_buffer,y
    a = input_buffer[y];
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    // c9912:
c9912:
    //     clc
    flags &= ~FLAG_C;
    //     adc l0084
    { uint16_t tmp_ = (uint16_t)a + l0084 + ((flags & FLAG_C) ? 1 : 0); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ > 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     inc l0039
    l0039++;
    //     tay
    y = a;
    //     lda #0
    a = 0;
    //     sta l0081
    l0081 = a;
    // c991c:
c991c:
    //     lda #0x20 ; ' '
    a = 0x20;
    //     bne c9922                                                         ; ALWAYS branch
    goto c9922;

    // c9920:
c9920:
    //     inc l0081
    l0081++;
    // c9922:
c9922:
    //     sta (current_edit_line_ptr),y
    ram[current_edit_line_ptr + y] = a;
    //     iny
    y++;
    //     inx
    x++;
    //     cpx l0043
    { uint16_t tmp_ = x - l0043; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= l0043 ? FLAG_C : 0); }
    //     bne c98fa
    if (!(flags & FLAG_Z)) goto c98fa;
    //     lda #0x10
    a = 0x10;
    // loop_c992c:
    //     cpy #0x84
loop_c992c:
    { uint16_t tmp_ = y - 0x84; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= 0x84 ? FLAG_C : 0); }
    //     bcs return_48
    if (flags & FLAG_C) return;
    //     sta (current_edit_line_ptr),y
    ram[current_edit_line_ptr + y] = a;
    //     iny
    y++;
    //     bne loop_c992c
    goto loop_c992c;
    // return_48:
return_48:
    //     rts
    return;
}
static void sub_c9936(void) {
    // Pseudocode: Processes a character from the edit line for output, handling tabs and margins

    // sub_c9936:
    //     ror l0083
    { uint8_t tmp_ = (l0083 & 1) ? FLAG_C : 0; l0083 >>= 1; l0083 |= (flags & FLAG_C) ? 0x80 : 0; flags = (flags & ~FLAG_C) | tmp_; }
    //     lda (current_edit_line_ptr),y
    a = ram[current_edit_line_ptr + y];
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     sta output_buffer,y
    output_buffer[y] = a;
    //     cmp #9
    { uint16_t tmp_ = a - 9; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 9 ? FLAG_C : 0); }
    //     bne c994a
    if (!(flags & FLAG_Z)) goto c994a;
    //     jsr sub_ca5ae
    process_document_character();
    //     txa
    a = x;
    //     clc
    flags &= ~FLAG_C;
    //     adc l0039
    { uint16_t tmp_ = (uint16_t)a + l0039 + ((flags & FLAG_C) ? 1 : 0); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ > 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     bne c995c
    if (!(flags & FLAG_Z)) goto c995c;
    // c994a:
c994a:
    //     cmp #0x0b
    { uint16_t tmp_ = a - 0x0b; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0b ? FLAG_C : 0); }
    //     bne c9969
    if (!(flags & FLAG_Z)) goto c9969;
    //     lda ruler_left_stop
    a = ruler_left_stop;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq c9967
    if (flags & FLAG_Z) goto c9967;
    //     ldx l0039
    x = l0039;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     beq c995c
    if (flags & FLAG_Z) goto c995c;
    //     cpx ruler_left_stop
    { uint16_t tmp_ = x - ruler_left_stop; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= ruler_left_stop ? FLAG_C : 0); }
    //     bcc c995c
    if (!(flags & FLAG_C)) goto c995c;
    //     inx
    x++;
    //     txa
    a = x;
    // c995c:
c995c:
    //     sta l0039
    l0039 = a;
    //     sty l0042
    l0042 = y;
    //     inc l0042
    l0042++;
    //     lda #0
    a = 0;
    //     sta l0046
    l0046 = a;
    //     rts
    return;

    // c9967:
c9967:
    //     lda #0x20 ; ' '
    a = 0x20;
    // c9969:
c9969:
    //     cmp #0x1b
    { uint16_t tmp_ = a - 0x1b; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x1b ? FLAG_C : 0); }
    //     bcc c9967
    if (!(flags & FLAG_C)) goto c9967;
    //     cmp #0x20 ; ' '
    { uint16_t tmp_ = a - 0x20; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x20 ? FLAG_C : 0); }
    //     bcc return_49
    if (!(flags & FLAG_C)) return;
    //     inc l0039
    l0039++;
    // return_49:
return_49:
    //     rts
    return;
}
static void sub_c9974(void) {
    // This is an alternate entry point into sub_c9977 at label c9a8d.
    // Calls to sub_c9974 have been translated to goto c9a8d directly.
}
static void sub_c9977(void) {
    // Pseudocode: Main line formatting routine: reads source line, handles margins, tabs, wrapping

    // sub_c9977:
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    //     ldy #4
    y = 4;
    //     sty print_xpos
    print_xpos = y;
    //     ldy #0
    y = 0;
    //     sty input_buffer_offset
    input_buffer_offset = y;
    //     sty l007e
    l007e = y;
    //     lda (current_line_ptr),y
    a = ram[current_line_ptr + y];
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     jsr check_for_command_prefix
    check_for_command_prefix();
    //     beq c9974
    if (flags & FLAG_Z) goto c9a8d;
    // c998a:
c998a:
    //     lda format_mode_flag
    a = format_mode_flag;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     and #0x81
    a &= 0x81;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bne c9974
    if (!(flags & FLAG_Z)) goto c9a8d;
    //     lda ruler_right_stop
    a = ruler_right_stop;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq c9974
    if (flags & FLAG_Z) goto c9a8d;
    //     sec
    flags |= FLAG_C;
    //     sbc ruler_left_stop
    { uint16_t tmp_ = (uint16_t)a - ruler_left_stop - (1 - ((flags & FLAG_C) ? 1 : 0)); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ <= 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     bcc c9974
    if (!(flags & FLAG_C)) goto c9a8d;
    //     adc #1
    { uint16_t tmp_ = (uint16_t)a + 1 + ((flags & FLAG_C) ? 1 : 0); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ > 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     sta input_buffer_offset+1
    //     lda #0x10
    a = 0x10;
    //     jsr wipe_buffer
    wipe_buffer();
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     sta tmp6
    tmp6 = a;
    //     lda current_line_ptr+1
    a = (uint8_t)((current_line_ptr >> 8) & 0xff);
    //     sta tmp7
    tmp7 = a;
    //     ldy #0
    y = 0;
    //     sty l0047
    l0047 = y;
    //     sty l0039
    l0039 = y;
    //     sty l0038
    l0038 = y;
    //     sty l0046
    l0046 = y;
    //     sty bottom_margin
    bottom_margin = y;
    // c99b6:
c99b6:
    //     sty l0048
    l0048 = y;
    //     ldy l0047
    y = l0047;
    // loop_c99ba:
loop_c99ba:
    //     jsr sub_ca536
    sub_ca536();
    //     bne c99c7
    if (!(flags & FLAG_Z)) goto c99c7;
    //     lda #0
    a = 0;
    //     sta markers_array+1,x
    markers_array[1 + x] = a;
    //     inc l007e
    l007e++;
    //     bne loop_c99ba
    if (l007e != 0) goto loop_c99ba;
    // c99c7:
c99c7:
    //     ldy l0047
    y = l0047;
    // c99c9:
c99c9:
    //     lda (current_line_ptr),y
    a = ram[current_line_ptr + y];
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     iny
    y++;
    //     sty l0047
    l0047 = y;
    //     cmp #9
    { uint16_t tmp_ = a - 9; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 9 ? FLAG_C : 0); }
    //     bne c99e0
    if (!(flags & FLAG_Z)) goto c99e0;
    //     jsr sub_ca5ae
    process_document_character();
    //     dex
    x--;
    //     txa
    a = x;
    //     clc
    flags &= ~FLAG_C;
    //     adc l0039
    { uint16_t tmp_ = (uint16_t)a + l0039 + ((flags & FLAG_C) ? 1 : 0); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ > 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     sta l0039
    l0039 = a;
    //     lda #9
    a = 9;
    //     bne c9a21                                                         ; ALWAYS branch
    goto c9a21;

    // c99e0:
c99e0:
    //     cmp #0x1a
    { uint16_t tmp_ = a - 0x1a; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x1a ? FLAG_C : 0); }
    //     bne c99ee
    if (!(flags & FLAG_Z)) goto c99ee;
    // c99e4:
c99e4:
    //     lda l0046
    a = l0046;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bne c99c9
    if (!(flags & FLAG_Z)) goto c99c9;
    //     ldx #0xff
    x = 0xff;
    //     lda #0x20 ; ' '
    a = 0x20;
    //     bne c9a2e                                                         ; ALWAYS branch
    goto c9a2e;

    // c99ee:
c99ee:
    //     cmp #0x0b
    { uint16_t tmp_ = a - 0x0b; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0b ? FLAG_C : 0); }
    //     bne c9a11
    if (!(flags & FLAG_Z)) goto c9a11;
    //     ldx input_buffer_offset
    x = input_buffer_offset;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     bne c99e4
    if (!(flags & FLAG_Z)) goto c99e4;
    //     lda l0038
    a = l0038;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bne c99e4
    if (!(flags & FLAG_Z)) goto c99e4;
    //     inc l0038
    l0038++;
    //     lda ruler_left_stop
    a = ruler_left_stop;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq c99c9
    if (flags & FLAG_Z) goto c99c9;
    //     ldx l0039
    x = l0039;
    //     cpx ruler_left_stop
    { uint16_t tmp_ = x - ruler_left_stop; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= ruler_left_stop ? FLAG_C : 0); }
    //     bcs c9a0a
    if (flags & FLAG_C) goto c9a0a;
    //     sta l0039
    l0039 = a;
    //     dec l0039
    l0039--;
    // c9a0a:
c9a0a:
    //     clc
    flags &= ~FLAG_C;
    //     adc input_buffer_offset+1
    { uint16_t tmp_ = (uint16_t)a + 0 + ((flags & FLAG_C) ? 1 : 0); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ > 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     sta input_buffer_offset+1
    //     lda #0x0b
    a = 0x0b;
    // c9a11:
c9a11:
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     bne c9a21
    if (!(flags & FLAG_Z)) goto c9a21;
    //     dey
    y--;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (y == 0 ? FLAG_Z : 0) | (y & FLAG_N);
    //     beq c9a8d
    if (flags & FLAG_Z) goto c9a8d;
    //     jsr sub_c9ac1
    sub_c9ac1();
    //     bcs c9a87
    if (flags & FLAG_C) goto c9a87;
    //     lda #0x20 ; ' '
    a = 0x20;
    //     sta input_buffer_offset
    input_buffer_offset = a;
    // c9a21:
c9a21:
    //     ldy l0048
    y = l0048;
    //     ldx #0
    x = 0;
    //     cmp #0x20 ; ' '
    { uint16_t tmp_ = a - 0x20; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x20 ? FLAG_C : 0); }
    //     bne c9a2e
    if (!(flags & FLAG_Z)) goto c9a2e;
    //     inx                                                               ; X=0x01
    x++;
    //     bit l0046
    { flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_V)) | ((a & l0046) == 0 ? FLAG_Z : 0) | (l0046 & (FLAG_N|FLAG_V)); }
    //     bmi c9a40
    if (flags & FLAG_N) goto c9a40;
    // c9a2e:
c9a2e:
    //     ldy l0048
    y = l0048;
    //     sta (current_edit_line_ptr),y
    ram[current_edit_line_ptr + y] = a;
    //     cmp #0x20 ; ' '
    { uint16_t tmp_ = a - 0x20; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x20 ? FLAG_C : 0); }
    //     bne c9a38
    if (!(flags & FLAG_Z)) goto c9a38;
    //     ror bottom_margin
    { uint8_t tmp_ = (bottom_margin & 1) ? FLAG_C : 0; bottom_margin >>= 1; bottom_margin |= (flags & FLAG_C) ? 0x80 : 0; flags = (flags & ~FLAG_C) | tmp_; }
    // c9a38:
c9a38:
    //     iny
    y++;
    //     jsr check_for_control_code
    check_for_control_code();
    //     beq c9a40
    if (flags & FLAG_Z) goto c9a40;
    //     inc l0039
    l0039++;
    // c9a40:
c9a40:
    //     bit l0046
    { flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_V)) | ((a & l0046) == 0 ? FLAG_Z : 0) | (l0046 & (FLAG_N|FLAG_V)); }
    //     stx l0046
    l0046 = x;
    //     bmi c9a58
    if (flags & FLAG_N) goto c9a58;
    //     cmp #0x20 ; ' '
    { uint16_t tmp_ = a - 0x20; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x20 ? FLAG_C : 0); }
    //     beq c9a58
    if (flags & FLAG_Z) goto c9a58;
    //     cpy #0x85
    { uint16_t tmp_ = y - 0x85; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= 0x85 ? FLAG_C : 0); }
    //     bcs c9a60
    if (flags & FLAG_C) goto c9a60;
    //     lda bottom_margin
    a = bottom_margin;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq c9a58
    if (flags & FLAG_Z) goto c9a58;
    //     lda l0039
    a = l0039;
    //     cmp input_buffer_offset+1
    { uint16_t tmp_ = a - 0; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0 ? FLAG_C : 0); }
    //     bcs c9a60
    if (flags & FLAG_C) goto c9a60;
    // c9a58:
c9a58:
    //     cpy #0x86
    { uint16_t tmp_ = y - 0x86; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= 0x86 ? FLAG_C : 0); }
    //     bcc c9a5d
    if (!(flags & FLAG_C)) goto c9a5d;
    //     dey
    y--;
    // c9a5d:
c9a5d:
    //     jmp c99b6
    goto c99b6;

    // c9a60:
c9a60:
    //     inc l0047
    l0047++;
    // loop_c9a62:
loop_c9a62:
    //     dec l0047
    l0047--;
    //     dey
    y--;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (y == 0 ? FLAG_Z : 0) | (y & FLAG_N);
    //     beq c9a8d
    if (flags & FLAG_Z) goto c9a8d;
    //     lda (current_edit_line_ptr),y
    a = ram[current_edit_line_ptr + y];
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     pha
    { uint8_t saved_a = a;
    //     lda #0x10
    a = 0x10;
    //     sta (current_edit_line_ptr),y
    ram[current_edit_line_ptr + y] = a;
    //     pla
    a = saved_a; }
    //     cmp #0x20 ; ' '
    { uint16_t tmp_ = a - 0x20; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x20 ? FLAG_C : 0); }
    //     bne loop_c9a62
    if (!(flags & FLAG_Z)) goto loop_c9a62;
    //     sec
    flags |= FLAG_C;
    //     ror input_buffer_offset
    { uint8_t tmp_ = (input_buffer_offset & 0x01) ? FLAG_C : 0; input_buffer_offset = (input_buffer_offset >> 1) | ((flags & FLAG_C) ? 0x80 : 0); flags = (flags & ~FLAG_C) | tmp_; }
    //     jsr sub_caed6
    sub_caed6();
    //     jsr sub_c9830
    sub_c9830();
    //     jsr sub_c9aa9
    sub_c9aa9();
    //     jsr c9a8d
    goto c9a8d;
    //     beq c9aa5
    //     jmp c998a

    // c9a87:
c9a87:
    //     jsr sub_caed6
    sub_caed6();
    //     jsr sub_c9aa9
    sub_c9aa9();
    // c9a8d:
c9a8d:
    //     jsr c9e94
    c9e94();
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     ldy current_line_ptr+1
    y = (uint8_t)((current_line_ptr >> 8) & 0xff);
    //     jsr sub_cab1a
    sub_cab1a();
    //     sec
    flags |= FLAG_C;
    //     beq c9aa5
    if (flags & FLAG_Z) goto c9aa5;
    //     tya
    a = y;
    //     clc
    flags &= ~FLAG_C;
    //     adc tmp0
    { uint16_t tmp_ = (uint16_t)a + tmp0 + ((flags & FLAG_C) ? 1 : 0); flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ > 0xff ? FLAG_C : 0); a = (uint8_t)tmp_; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     sta current_line_ptr
    current_line_ptr = (current_line_ptr & 0xff00) | a;
    //     bcc c9aa4
    if (!(flags & FLAG_C)) goto c9aa4;
    //     inc current_line_ptr+1
    current_line_ptr = (current_line_ptr & 0x00ff) | ((uint16_t)((current_line_ptr >> 8) + 1) << 8);
    // c9aa4:
c9aa4:
    //     clc
    flags &= ~FLAG_C;
    // c9aa5:
c9aa5:
    //     clv
    flags &= ~FLAG_V;
    //     lda l007e
    a = l007e;
    //     rts
    return;
}
static void sub_c9aa9(void) {
    // Pseudocode: Completes line formatting: adjusts pointers updates ruler stack

    // sub_c9aa9:
    //     sec
    flags |= FLAG_C;
    //     rol l007e
    // PROBLEM: rol l007e (rotate left on memory)
    //     ldy l0047
    y = l0047;
    //     dey
    y--; flags = (flags & ~(FLAG_Z|FLAG_N)) | (y == 0 ? FLAG_Z : 0) | (y & FLAG_N);
    //     sty l003b
    l003b = y;
    //     inc l006e
    l006e++; flags = (flags & ~(FLAG_Z|FLAG_N)) | (l006e == 0 ? FLAG_Z : 0) | (l006e & FLAG_N);
    //     jsr write_line_back_to_document
    write_line_back_to_document();
    //     bcc return_50
    if (!(flags & FLAG_C)) return;
    //     pla
    //     pla
    //     lda #0x40 ; '@'
    a = 0x40;
    //     sta l0084
    l0084 = a;
    //     bit l0084
    { uint8_t tmp_ = a & l0084; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ == 0 ? FLAG_Z : 0) | (l0084 & (FLAG_N|FLAG_V)); }
    // return_50:
    //     rts
    return;
}
static void sub_c9ac1(void) {
    // Pseudocode: Finds next word boundary for line wrapping, returns carry if found

    // sub_c9ac1:
    //     tya
    a = y;
    //     sec
    flags |= FLAG_C;
    //     adc current_line_ptr
    { uint16_t sum = (uint16_t)a + (uint16_t)(uint8_t)(current_line_ptr & 0xff); a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)sum == 0 ? FLAG_Z : 0) | ((uint8_t)sum & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     sta tmp8
    tmp8 = a;
    //     sta tmp4
    tmp4 = a;
    //     lda current_line_ptr+1
    a = (uint8_t)(current_line_ptr >> 8);
    //     adc #0
    { uint16_t sum = (uint16_t)a + (flags & FLAG_C); a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)sum == 0 ? FLAG_Z : 0) | ((uint8_t)sum & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     sta tmp9
    tmp9 = a;
    //     sta tmp5
    tmp5 = a;
    //     ldy #0
    y = 0;
    //     sty l0083
    l0083 = y;
    // c9ad5:
c9ad5:
    //     lda (tmp4),y
    a = ram[((uint16_t)tmp5 << 8 | tmp4) + y];
    //     beq c9b2f
    if (a == 0) goto c9b2f;
    //     jsr check_for_command_prefix
    check_for_command_prefix();
    //     beq c9b2f
    if (flags & FLAG_Z) goto c9b2f;
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     beq c9b2f
    if (flags & FLAG_Z) goto c9b2f;
    //     tya
    a = y;
    //     bne c9b06
    if (a != 0) goto c9b06;
    //     sty l0084
    l0084 = y;
    //     beq c9aef                                                         ; ALWAYS branch
    goto c9aef;

    // c9ae9:
c9ae9:
    //     inc tmp8
    tmp8++;
    //     bne c9aef
    if (tmp8 != 0) goto c9aef;
    //     inc tmp9
    tmp9++;
    // c9aef:
c9aef:
    //     lda (tmp8),y
    a = ram[((uint16_t)tmp9 << 8 | tmp8) + y];
    //     beq c9b06
    if (a == 0) goto c9b06;
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     beq c9b06
    if (flags & FLAG_Z) goto c9b06;
    //     cmp #9
    { uint16_t tmp_ = a - 9; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 9 ? FLAG_C : 0); }
    //     beq c9b2f
    if (flags & FLAG_Z) goto c9b2f;
    //     cmp #0x0b
    { uint16_t tmp_ = a - 0x0b; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0b ? FLAG_C : 0); }
    //     bne c9ae9
    if (!(flags & FLAG_Z)) goto c9ae9;
    //     rol l0084
    // PROBLEM: rol l0084 (rotate left on memory)
    //     sec
    flags |= FLAG_C;
    //     ror l0084
    // PROBLEM: ror l0084 (rotate right on memory)
    //     bcs c9ae9
    if (flags & FLAG_C) goto c9ae9;
    // c9b06:
c9b06:
    //     lda (tmp4),y
    a = ram[((uint16_t)tmp5 << 8 | tmp4) + y];
    //     cmp #0x20 ; ' '
    { uint16_t tmp_ = a - 0x20; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x20 ? FLAG_C : 0); }
    //     bne c9b1a
    if (!(flags & FLAG_Z)) goto c9b1a;
    //     ldx ruler_left_stop
    x = ruler_left_stop;
    //     beq c9b2f
    if (x == 0) goto c9b2f;
    //     ldx l0084
    x = l0084;
    //     beq c9b2f
    if (x == 0) goto c9b2f;
    //     ldx l0083
    x = l0083;
    //     bne c9b2f
    if (x != 0) goto c9b2f;
    //     beq c9b20                                                         ; ALWAYS branch
    goto c9b20;

    // c9b1a:
c9b1a:
    //     cmp #0x0b
    { uint16_t tmp_ = a - 0x0b; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0b ? FLAG_C : 0); }
    //     bne c9b23
    if (!(flags & FLAG_Z)) goto c9b23;
    //     sta l0083
    l0083 = a;
    // c9b20:
c9b20:
    //     iny
    y++;
    //     bne c9ad5
    if (y != 0) goto c9ad5;
    // c9b23:
c9b23:
    //     lda ruler_left_stop
    a = ruler_left_stop;
    //     beq c9b31
    if (a == 0) goto c9b31;
    //     lda l0084
    a = l0084;
    //     beq c9b31
    if (a == 0) goto c9b31;
    //     lda l0083
    a = l0083;
    //     bne c9b31
    if (a != 0) goto c9b31;
    // c9b2f:
c9b2f:
    //     sec
    flags |= FLAG_C;
    //     rts
    return;

    // c9b31:
c9b31:
    //     clc
    flags &= ~FLAG_C;
    //     rts
    return;
}
static void editor_loop_impl(void) {
    screen_enter();
    // Pseudocode: Main editor loop: handles cursor positioning, redrawing, key dispatch
editor_loop:
    //     lda format_mode_flag
    //     pha
    {   uint8_t saved_fmt = format_mode_flag;
    //     lda l006e
    a = l006e;
    //     bne c9b44
    if (a != 0) goto c9b44_;
    //     pha
    {   uint8_t saved_a_ = a;
    //     jsr sub_caa97
    sub_caa97();
    //     pla
    a = saved_a_; }
    //     sta l006e
    l006e = a;
c9b44_:
    //     jsr sub_ca608
    recalculate_cursor_xpos();
    //     lda ruler_left_stop
    a = ruler_left_stop;
    //     beq c9b73
    if (a == 0) goto c9b73_;
    //     ldx format_mode_flag
    x = format_mode_flag;
    //     bmi c9b73
    if (x & 0x80) goto c9b73_;
    //     cmp l0072
    { uint16_t tmp_ = a - l0072; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | (tmp_ & FLAG_N) | (a >= l0072 ? FLAG_C : 0); }
    //     bcc c9b73
    if (!(flags & FLAG_C)) goto c9b73_;
    //     beq c9b73
    if (flags & FLAG_Z) goto c9b73_;
    //     ldx cursor_moved_flag
    x = cursor_moved_flag;
    //     bne c9b6a
    if (x != 0) goto c9b6a_;
    //     jsr get_line_length
    get_line_length();
    //     lda format_mode_flag
    a = format_mode_flag;
    //     cpy xpos
    { uint16_t tmp_ = y - xpos; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | (tmp_ & FLAG_N) | (y >= xpos ? FLAG_C : 0); }
    //     bcs c9b84
    if (flags & FLAG_C) goto c9b84_;
    //     bit format_mode_flag
    { uint8_t tmp_ = a & format_mode_flag; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ == 0 ? FLAG_Z : 0) | (format_mode_flag & (FLAG_N|FLAG_V)); }
    //     bvs c9b6a
    if (flags & FLAG_V) goto c9b6a_;
    //     sty xpos
    xpos = y;
    //     bvc c9b84                                                         ; ALWAYS branch
    goto c9b84_;

c9b6a_:
    //     lda ruler_left_stop
    a = ruler_left_stop;
    //     sta l0072
    l0072 = a;
    //     inc l0079
    l0079++;
    //     jsr sub_ca608
    recalculate_cursor_xpos();
    //     lda format_mode_flag
    a = format_mode_flag;
    //     and #0xbf
    a &= 0xbf;
    //     pha
    {   uint8_t saved_mod = a;
    //     jsr sub_caec2
    sub_caec2();
    //     pla
    a = saved_mod; }
    //     bcs c9b86
    if (flags & FLAG_C) goto c9b86_;
    //     cpy xpos
    { uint16_t tmp_ = y - xpos; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | (tmp_ & FLAG_N) | (y >= xpos ? FLAG_C : 0); }
    //     bcc c9b86
    if (!(flags & FLAG_C)) goto c9b86_;
    //     beq c9b86
    if (flags & FLAG_Z) goto c9b86_;
    //     ora #0x40 ; '@'
    a |= 0x40;
    //     sta format_mode_flag
    format_mode_flag = a;
c9b8f_:
c9b84_:
c9b73_:
c9b86_:
    //     pla (was: pop saved_fmt)
    //     cmp format_mode_flag
    //     beq c9b8f
    if (saved_fmt != format_mode_flag || flags_need_redrawing_flag) {
    //     inc flags_need_redrawing_flag
        flags_need_redrawing_flag++;
    //     lda #0
    //     sta cursor_moved_flag
        cursor_moved_flag = 0;
    //     jsr redraw_editor
        redraw_editor();
    }
    }
c9b96:
    //     jsr read_char
    read_char();
    //     cmp current_tab_key
    { uint16_t tmp_ = a - current_tab_key; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | (tmp_ & FLAG_N) | (a >= current_tab_key ? FLAG_C : 0); }
    //     bne c9b9f
    if (!(flags & FLAG_Z)) goto c9b9f;
    //     lda #9
    a = 9;
c9b9f:
    //     sta l0038
    l0038 = a;
    //     tay
    y = a;
    //     bmi c9bbb
    if (a & 0x80) goto editor_loop;
    //     cmp #0x20 ; ' '
    { uint16_t tmp_ = a - 0x20; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | (tmp_ & FLAG_N) | (a >= 0x20 ? FLAG_C : 0); }
    //     bcc enter_nonprintable_character
    if (!(flags & FLAG_C)) goto enter_nonprintable_character;
    //     cmp #0x7f
    { uint16_t tmp_ = a - 0x7f; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | (tmp_ & FLAG_N) | (a >= 0x7f ? FLAG_C : 0); }
    //     bcc enter_printable_character
    if (!(flags & FLAG_C)) { enter_printable_character(); goto editor_loop; }
enter_nonprintable_character:
    switch (a) {
        case '['-'@': esc_key(); goto editor_loop;
        case 'M'-'@': return_key(); goto editor_loop;
        case 0x7f: delete_key(); goto editor_loop;
        case 'I'-'@': tab_key(); goto editor_loop;
        case 'E'-'@': f15_up_key(); goto editor_loop;
        case SCREEN_KEY_UP: f15_up_key(); goto editor_loop;
        case 'S'-'@': f12_left_key(); goto editor_loop;
        case SCREEN_KEY_LEFT: f12_left_key(); goto editor_loop;
        case 'D'-'@': f13_right_key(); goto editor_loop;
        case SCREEN_KEY_RIGHT: f13_right_key(); goto editor_loop;
        case 'X'-'@': f14_down_key(); goto editor_loop;
        case SCREEN_KEY_DOWN: f14_down_key(); goto editor_loop;
        case 'A'-'@': sf12_left_key(); goto editor_loop;
        case 'F'-'@': sf13_right_key(); goto editor_loop;
        case 'C'-'@': sf14_down_key(); goto editor_loop;
        case 'R'-'@': sf15_up_key(); goto editor_loop;
        case 'G'-'@': f9_delete_char_key(); goto editor_loop;
        case 'H'-'@': f8_insert_char_key(); goto editor_loop;
        case 'Y'-'@': f7_delete_line_key(); goto editor_loop;
        case 'V'-'@': cf4_insert_mode_key(); goto editor_loop;
        case 'N'-'@': f6_insert_line_key(); goto editor_loop;
        case 'B'-'@': f0_format_block_key(); goto editor_loop;
        case 'T'-'@': sf3_delete_to_char_key(); goto editor_loop;
        case 'L'-'@': cf1_next_match_key(); goto editor_loop;
        case 'J'-'@': cf7_join_lines_key(); goto editor_loop;
        case 'P'-'@': sf1_swap_case_key(); goto editor_loop;
        case 'O'-'@': o_command_key(); goto editor_loop;
        case 'Q'-'@': q_command_key(); goto editor_loop;
        case 'K'-'@': k_command_key(); goto editor_loop;
        default: goto c9b96;
    }
}
static void jsr_tmp6(void) {
}
static void sub_c9bbb(void) {
    // c9bbb:
    //     jmp editor_loop
    editor_loop_impl();
}
static void sub_c9bca(void) {
    // c9bca:
    //     jsr beep
    // c9bbb:
    //     jmp editor_loop
    beep();
    editor_loop_impl();
}
static void enter_printable_character(void) {
    y = xpos;
    if (y >= MAX_LINE_LENGTH) return;
    l006d++;
    sub_caef4();
    if (flags & FLAG_C) return;
    tmp6 = (uint8_t)(current_edit_line_ptr & 0xff);
    tmp7 = (uint8_t)(current_edit_line_ptr >> 8);
    y = xpos;
    sub_ca536();
    if ((flags & FLAG_Z) && x >= 4) {
        l0074++;
    }
    x = insert_mode_flag;
    if (x != 0) {
        l0074++;
        x = 1;
        sub_cae06();
        if (flags & FLAG_C) { editor_loop_impl(); return; }
    } else {
        a = ram[current_edit_line_ptr + y];
        if (a == 9 || a == 0x0b) {
            l0074++;
            x = 1;
            sub_cae06();
            if (flags & FLAG_C) { editor_loop_impl(); return; }
        }
    }
    a = l0038;
    ram[current_edit_line_ptr + y] = a;
    y = l0074;
    if (y == 0) screen_putchar(a);
    xpos++;
    ca684();
    y = 0;
    l0039 = 0;
    while (1) {
        a = ram[current_edit_line_ptr + y];
        y++;
        if (y > xpos) break;
        if (a == 9) {
            process_document_character();
            a = x;
            a += l0039;
            if (a != 0) { l0039 = a; continue; }
        }
        if (a == 0x0b) {
            if (ruler_left_stop == 0) { a = 0x20; goto c9c4a; }
            if (l0039 != 0 && l0039 < ruler_left_stop) { a = l0039; l0039 = a; continue; }
            x = l0039; x++; a = x;
            l0039 = a;
            continue;
        }
c9c4a:
        if (a < 0x1b) { a = 0x20; goto c9c4a; }
        if (a < 0x20) continue;
        l0039++;
    }
    y = l0039;
    if (y < l003a) {
        a = ram[current_ruler_ptr + y];
        a &= 0xdf;
        if (a == 0x42) beep();
    }
    a = l0038;
    if (a == 0x20) { editor_loop_impl(); return; }
    if (ruler_right_stop == 0) { editor_loop_impl(); return; }
    if (format_mode_flag != 0) { editor_loop_impl(); return; }
    tmp7 = 0;
    if (y == 0) { editor_loop_impl(); return; }
    y--;
    if (y < ruler_right_stop) { editor_loop_impl(); return; }
    // c9c82: word wrap logic
    get_line_length();
    l0083 = y;
    top_margin = 0;
    y = xpos;
    input_buffer_offset = y;
    draw_previous_word();
    recalculate_cursor_xpos();
    if (l0072 < ruler_left_stop) {
        y = input_buffer_offset;
        y--;
        xpos = y;
    }
    {
        uint8_t ibp = input_buffer_offset;
        top_margin = ibp - xpos;
        l0083 = l0083 - xpos;
        y = l0083;
        y++;
        if (ruler_left_stop != 0) { top_margin++; y++; }
        tmp6 = y;
        {
            uint16_t sum = (uint16_t)(uint8_t)(current_line_ptr & 0xff) + l003b;
            tmp4 = (uint8_t)(sum & 0xff);
            tmp5 = (uint8_t)(current_line_ptr >> 8) + (sum >> 8);
        }
        make_space_for_insertion();
        if (flags & FLAG_C) { show_memory_full_error(); longjmp(env, JMP_EDITOR); }
    }
    // c9cd0:
    y = 0;
    if (ruler_left_stop != 0) {
        ram[((uint16_t)tmp5 << 8) | tmp4] = 0x0b;
        y = 1;
    }
    l0081 = y;
    tmp6 = (uint8_t)(current_edit_line_ptr & 0xff);
    tmp7 = (uint8_t)(current_edit_line_ptr >> 8);
    y = xpos;
    y--;
    a = ram[current_edit_line_ptr + y];
    if (a == 0x20) ram[current_edit_line_ptr + y] = 0x10;
    y++;
    l0082 = y;
    // c9cf5:
    while (1) {
        y = l0082;
        l0082++;
        sub_ca536();
        if (!(flags & FLAG_Z)) goto c9d0d;
        {
            uint16_t sum = (uint16_t)l0081 + tmp4;
            ((uint8_t*)markers_array)[x] = (uint8_t)(sum & 0xff);
            ((uint8_t*)markers_array)[x+1] = (uint8_t)(tmp5 + (sum >> 8));
        }
        if (flags & FLAG_C) continue;
c9d0d:
        if (l0083 != 0) goto c9d28;
        a = 0x0d;
        goto c9d30;
c9d28:
        a = ram[current_edit_line_ptr + y];
        { uint8_t saved = a; ram[current_edit_line_ptr + y] = 0x10; a = saved; }
c9d30:
        y = l0081;
        l0081++;
        ram[((uint16_t)tmp5 << 8) | ((uint16_t)tmp4 + y)] = a;
        l0083--;
        if (l0083 >= 0) continue;
        // c9d15:
        break;
    }
    sub_c9830();
    write_line_back_to_document_safely();
    ca741();
    return_key();
    xpos = top_margin;
    editor_loop_impl();
}
// MULTIPLE ENTRY POINTS: sf1_swap_case_key, f13_right_key
static void sf1_swap_case_key(void) {
    // sf1_swap_case_key:
    //     ldy xpos
    y = xpos;
    //     lda (current_edit_line_ptr),y
    a = ram[current_edit_line_ptr + y];
    //     jsr is_uppercase
    is_uppercase();
    //     bcs f13_right_key
    if (flags & FLAG_C) { f13_right_key(); return; }
    //     inc l0074
    l0074++;
    //     eor #0x20 ; ' '
    a ^= 0x20;
    //     sta (current_edit_line_ptr),y
    ram[current_edit_line_ptr + y] = a;
    //     falls through to f13_right_key
    f13_right_key(); return;
}
static void f13_right_key(void) {
    // f13_right_key:
    //     ldy xpos
    y = xpos;
    //     cpy #MAX_LINE_LENGTH
    { uint16_t tmp_ = y - MAX_LINE_LENGTH; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | (tmp_ & FLAG_N) | (y >= MAX_LINE_LENGTH ? FLAG_C : 0); }
    //     bcs return_51
    if (flags & FLAG_C) return;
    //     inc xpos
    xpos++;
    // return_51:
    //     rts
    return;
}
static void f12_left_key(void) {
    // f12_left_key: Moves cursor left by one position

    //     ldy l0072
    y = l0072;
    //     beq return_52
    if (y == 0) return;
    //     dec xpos
    xpos--;
    // return_52:
}
static void f15_up_key(void) {
    // f15_up_key: Moves cursor to previous line, handling ruler stack

    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     ldy current_line_ptr+1
    y = (uint8_t)(current_line_ptr >> 8);
    //     jsr sub_cab37
    sub_cab37();
    //     bcc return_53
    if (!(flags & FLAG_C)) return;
    //     lda tmp0
    a = tmp0;
    //     sta current_line_ptr
    current_line_ptr = (uint16_t)((current_line_ptr & 0xff00) | a);
    //     lda tmp1
    a = tmp1;
    //     sta current_line_ptr+1
    current_line_ptr = (uint16_t)((current_line_ptr & 0x00ff) | ((uint16_t)a << 8));
    //     inc l0079
    l0079++;
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    // return_53:
}
static void f14_down_key(void) {
    // Pseudocode: Moves cursor to next line

    // ; ***************************************************************************************
    // f14_down_key:
    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    //     inc l0079
    l0079++;
    //     bne c9d9b
    if (l0079 != 0) goto c9d9b;

    // MULTIPLE ENTRY POINTS: f14_down_key, return_key
c9d9b:
    return_key();
}
static void return_key(void) {
    // return_key: Carriage return: moves to next line at column 0

    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    //     lda #0
    a = 0;
    //     sta xpos
    xpos = 0;
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     sta tmp0
    tmp0 = a;
    //     lda current_line_ptr+1
    a = (uint8_t)(current_line_ptr >> 8);
    //     sta tmp1
    tmp1 = a;
    //     jsr cab29
    cab29();
    //     bne c9d9b
    if (!(flags & FLAG_Z)) goto c9d9b;
    //     tya
    a = y;
    //     ldy current_line_ptr+1
    y = (uint8_t)(current_line_ptr >> 8);
    //     clc
    flags &= ~FLAG_C;
    //     adc current_line_ptr
    { uint16_t sum = (uint16_t)a + (uint8_t)(current_line_ptr & 0xff); a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     bcc c9d98
    if (!(flags & FLAG_C)) goto c9d98;
    //     iny
    y++;
    // c9d98:
c9d98:
    //     jsr sub_c9de1
    sub_c9de1();
    // c9d9b:
c9d9b:
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     ldy current_line_ptr+1
    y = (uint8_t)(current_line_ptr >> 8);
    //     jsr sub_cab1a
    sub_cab1a();
    //     beq return_54
    if (flags & FLAG_Z) return;
    //     tya
    a = y;
    //     clc
    flags &= ~FLAG_C;
    //     adc current_line_ptr
    { uint16_t sum = (uint16_t)a + (uint8_t)(current_line_ptr & 0xff); a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     sta current_line_ptr
    current_line_ptr = (uint16_t)((current_line_ptr & 0xff00) | a);
    //     bcc return_54
    if (!(flags & FLAG_C)) return;
    //     inc current_line_ptr+1
    current_line_ptr = (uint16_t)(current_line_ptr + 0x100);
    // return_54:
    //     rts
}
static void cf6_split_line_key(void) {
    // cf6_split_line_key: Splits line at cursor position

    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    //     jsr get_line_length
    get_line_length();
    //     cpy xpos
    { uint16_t tmp_ = y - xpos; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= xpos ? FLAG_C : 0); }
    //     bcc c9dbd
    if (!(flags & FLAG_C)) goto c9dbd;
    //     ldy xpos
    y = xpos;
    // c9dbd:
c9dbd:
    //     inc l0079
    l0079++;
    //     tya
    a = y;
    //     tax
    x = a;
    //     ldy #0
    y = 0;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    //     jsr check_for_command_prefix
    check_for_command_prefix();
    //     bne c9dcd
    if (!(flags & FLAG_Z)) goto c9dcd;
    //     inx
    x++;
    //     inx
    x++;
    //     inx
    x++;
    // c9dcd:
c9dcd:
    //     ldy current_line_ptr+1
    y = (uint8_t)(current_line_ptr >> 8);
    //     txa
    a = x;
    //     clc
    flags &= ~FLAG_C;
    //     adc current_line_ptr
    { uint16_t sum = (uint16_t)a + (uint8_t)(current_line_ptr & 0xff); a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     bcc c9de3
    if (!(flags & FLAG_C)) { sub_c9de1(); return; }
    //     iny
    y++;
    //     bne c9de3
    if (y != 0) { sub_c9de1(); return; }
    //     (fall through - y wrapped to 0)
    sub_c9de1(); return;
}

// MULTIPLE ENTRY POINTS: cf6_split_line_key, f6_insert_line_key, sub_c9de1
static void f6_insert_line_key(void) {
    // f6_insert_line_key:
    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    //     lda current_line_ptr
    a = current_line_ptr;
    //     ldy current_line_ptr+1
    y = current_line_ptr >> 8;
    //     inc l0079
    l0079++;
    //     falls through to sub_c9de1
    sub_c9de1(); return;
}

static void sub_c9de1(void) {
    // sub_c9de1:
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    // c9de3:
c9de3:
    //     sta tmp4
    tmp4 = a;
    //     sty tmp5
    tmp5 = y;
    //     lda #1
    a = 1;
    //     sta tmp6
    tmp6 = a;
    //     lda #0
    a = 0;
    //     sta tmp7
    tmp7 = a;
    //     jsr make_space_for_insertion
    make_space_for_insertion();
    //     bcs c9dfd
    if (flags & FLAG_C) goto c9dfd;
    //     lda #0x0d
    a = 0x0d;
    //     ldy #0
    y = 0;
    //     sta (tmp4),y
    ram[((uint16_t)tmp5 << 8 | tmp4) + y] = a;
    //     jmp ca741
    ca741(); return;

c9dfd:
    //     jmp ca941
    memory_full(); return;
}
// MULTIPLE ENTRY POINTS: delete_key, f8_insert_char_key
static void delete_key(void) {
    // delete_key:
    //     lda l0072
    a = l0072;
    //     beq return_55
    if (flags & FLAG_Z) return;
    //     dec xpos
    xpos--;
    //     ldy xpos
    y = xpos;
    //     lda (current_edit_line_ptr),y
    a = ram[current_edit_line_ptr + y];
    //     pha
    { uint8_t saved_a = a;
    //     jsr f9_delete_char_key
    f9_delete_char_key();
    //     pla
    a = saved_a; }
    //     cmp #0x0c
    { uint16_t tmp_ = a - 0x0c; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | (tmp_ & FLAG_N) | (a >= 0x0c ? FLAG_C : 0); }
    //     bcc return_55
    if (!(flags & FLAG_C)) return;
    //     ldx insert_mode_flag
    x = insert_mode_flag;
    //     bne return_55
    if (!(flags & FLAG_Z)) return;
    //     jsr get_line_length
    get_line_length();
    //     cpy xpos
    { uint16_t tmp_ = y - xpos; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | (tmp_ & FLAG_N) | (y >= xpos ? FLAG_C : 0); }
    //     bcc return_55
    if (!(flags & FLAG_C)) return;
    //     beq return_55
    if (flags & FLAG_Z) return;
    //     rts
    return;
}
static void f8_insert_char_key(void) {
    // f8_insert_char_key:
    //     lda #0x20 ; ' '
    a = 0x20;
    //     falls through to sub_c9e22
    sub_c9e22(); return;
}
static void sub_c9e22(void) {
    // sub_c9e22:
    //     pha
    { uint8_t saved_a = a;
    //     ldx #1
    x = 1;
    //     jsr sub_cae06
    sub_cae06();
    //     pla
    a = saved_a; }
    //     bcs return_55
    if (flags & FLAG_C) return;
    //     sta (current_edit_line_ptr),y
    ram[current_edit_line_ptr + y] = a;
    //     inc l0074
    l0074++;
    // return_55:
    //     rts
    return;
}
// MULTIPLE ENTRY POINTS: tab_key, sf4_highlight1_key, sf5_highlight2_key
static void tab_key(void) {
    // tab_key:
    //     lda #9
    a = 9;
    //     jmp c9e3a
    tab_highlight_common(); return;
}
static void sf4_highlight1_key(void) {
    // sf4_highlight1_key:
    //     lda #0x1c
    a = 0x1c;
    //     jmp c9e3a
    tab_highlight_common(); return;
}
static void sf5_highlight2_key(void) {
    // sf5_highlight2_key:
    //     lda #0x1d
    a = 0x1d;
    //     jmp c9e3a
    tab_highlight_common(); return;
}
static void tab_highlight_common(void) {
    // c9e3a:
    //     pha
    //     jsr sub_caef4
    sub_caef4();
    //     pla
    //     bcs return_55
    if (flags & FLAG_C) return;
    //     jsr sub_c9e22
    sub_c9e22();
    //     bcs return_55
    if (flags & FLAG_C) return;
    //     jmp f13_right_key
    f13_right_key(); return;
}
static void f9_delete_char_key(void) {
    // f9_delete_char_key: Deletes character under cursor

    //     ldx #1
    x = 1;
    //     inc l0074
    l0074++;
    //     jmp cae64
    cae64(); return;
}
static void f7_delete_line_key(void) {
    // f7_delete_line_key: Deletes current line and moves cursor up

    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     sta tmp4
    tmp4 = a;
    //     lda current_line_ptr+1
    a = (uint8_t)(current_line_ptr >> 8);
    //     sta tmp5
    tmp5 = a;
    //     ldx l003b
    x = l003b;
    //     inx
    x++;
    //     stx tmp6
    tmp6 = x;
    //     lda #0
    a = 0;
    //     sta tmp7
    tmp7 = a;
    //     jsr adjust_pointers
    adjust_pointers();
    //     jsr cb05a
    cb05a();
    //     ldy #0
    y = 0;
    //     lda (current_line_ptr),y
    a = ram[current_line_ptr + y];
    //     bne c9e81
    if (!(flags & FLAG_Z)) goto c9e81;
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     ldy current_line_ptr+1
    y = (uint8_t)(current_line_ptr >> 8);
    //     jsr sub_cab37
    sub_cab37();
    //     lda tmp0
    a = tmp0;
    //     sta current_line_ptr
    current_line_ptr = (uint16_t)((current_line_ptr & 0xff00) | a);
    //     lda tmp1
    a = tmp1;
    //     sta current_line_ptr+1
    current_line_ptr = (uint16_t)((current_line_ptr & 0x00ff) | ((uint16_t)a << 8));
    // c9e81:
c9e81:
    //     inc l0079
    l0079++;
    //     jmp ca741
    ca741(); return;
}
// MULTIPLE ENTRY POINTS: sf2_release_margins_key, f4_beginning_of_line_key
static void sf2_release_margins_key(void) {
    // sf2_release_margins_key:
    //     bit format_mode_flag
    if (!(format_mode_flag & FLAG_V)) { c9e94(); return; }
    //     jsr sub_caec2
    sub_caec2();
    //     bcs f4_beginning_of_line_key
    if (flags & FLAG_C) { f4_beginning_of_line_key(); return; }
    //     sty xpos
    xpos = y;
    //     rts
    return;
}
static void c9e94(void) {
    // c9e94:
    //     lda #0
    a = 0;
    //     sta xpos
    xpos = a;
    //     rts
}
static void f4_beginning_of_line_key(void) {
    // f4_beginning_of_line_key:
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    //     jmp c9e94
    c9e94(); return;
}
static void sub_c9e9b(void) {
    // Shared code: gets line length and sets xpos
    // c9e9b:
    //     jsr get_line_length
    get_line_length();
    //     sty xpos
    xpos = y;
    //     rts
}
static void f5_end_of_line_key(void) {
    // Pseudocode: Moves cursor to end of current line

    // f5_end_of_line_key:
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    // c9e9b:
    sub_c9e9b();
}
static void cf7_join_lines_key(void) {
    // cf7_join_lines_key: Joins current line with next line

    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     sta tmp0
    tmp0 = a;
    //     lda current_line_ptr+1
    a = (uint8_t)(current_line_ptr >> 8);
    //     sta tmp1
    tmp1 = a;
    //     jsr cab29
    cab29();
    //     beq c9eda
    if (flags & FLAG_Z) { beep(); return; }
    //     jsr check_for_command_prefix
    check_for_command_prefix();
    //     beq c9eda
    if (flags & FLAG_Z) { beep(); return; }
    //     dey
    y--;
    //     tya
    a = y;
    //     clc
    flags &= ~FLAG_C;
    //     adc current_line_ptr
    { uint16_t sum = (uint16_t)a + (uint8_t)(current_line_ptr & 0xff); a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     sta tmp4
    tmp4 = a;
    //     lda current_line_ptr+1
    a = (uint8_t)(current_line_ptr >> 8);
    //     adc #0
    { uint16_t sum = (uint16_t)a + 0 + (flags & FLAG_C ? 1 : 0); a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     sta tmp5
    tmp5 = a;
    //     lda #0
    a = 0;
    //     sta tmp7
    tmp7 = a;
    //     lda #1
    a = 1;
    //     sta tmp6
    tmp6 = a;
    //     jsr adjust_pointers
    adjust_pointers();
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     ldy current_line_ptr+1
    y = (uint8_t)(current_line_ptr >> 8);
    //     jsr cac78
    cac78();
    //     inc l0079
    l0079++;
    //     jmp ca741
    ca741(); return;

    // c9eda:
    //     jmp beep
}
static void f3_delete_to_eol_key(void) {
    // f3_delete_to_eol_key: Deletes from cursor to end of line

    //     lda #MAX_LINE_LENGTH
    a = MAX_LINE_LENGTH;
    //     sec
    flags |= FLAG_C;
    //     sbc xpos
    { uint16_t tmp_ = (uint16_t)a - xpos - (1 - (flags & FLAG_C ? 1 : 0)); a = (uint8_t)tmp_; x = a; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (tmp_ < 0x10000 ? FLAG_C : 0); }
    //     tax
    //     inc l0074
    l0074++;
    //     jmp cae64
    cae64(); return;
}
static void sf8_edit_command_key(void) {
    // sf8_edit_command_key: Allows editing formatting command on current line interactively

    //     jsr c9e94
    c9e94();
    //     jsr redraw_editor
    redraw_editor();
    //     inc l006d
    l006d++;
    //     lda #0
    a = 0;
    //     sta input_buffer_offset+1
    input_buffer_offset = a;
    //     sta l0081
    l0081 = 0;
    // edit_command_loop:
edit_command_loop:
    //     ldx input_buffer_offset+1
    x = input_buffer_offset;
    //     ldy ypos
    y = ypos;
    //     jsr set_cursor_position
    set_cursor_position();
    //     jsr read_char
    read_char();
    //     bcs finished_editing_command
    if (flags & FLAG_C) goto finished_editing_command;
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     beq finished_editing_command
    if (flags & FLAG_Z) goto finished_editing_command;
    //     and #0xdf
    a &= 0xdf;
    //     cmp #0x41 ; 'A'
    { uint16_t tmp_ = a - 0x41; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x41 ? FLAG_C : 0); }
    //     bcc edit_command_loop
    if (!(flags & FLAG_C)) goto edit_command_loop;
    //     cmp #0x5b ; '['
    { uint16_t tmp_ = a - 0x5b; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x5b ? FLAG_C : 0); }
    //     bcs edit_command_loop
    if (flags & FLAG_C) goto edit_command_loop;
    //     sta l0081
    l0081 = a;
    //     jsr screen_putchar
    screen_putchar(a);
    //     ldy input_buffer_offset+1
    y = input_buffer_offset;
    //     iny
    y++;
    //     sty input_buffer_offset+1
    input_buffer_offset = y;
    //     sta (ptr1),y
    ram[(uint16_t)(ptr1 >> 8) << 8 | (ptr1 & 0xff) + y] = a;
    //     cpy #2
    { uint16_t tmp_ = y - 2; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= 2 ? FLAG_C : 0); }
    //     bcc edit_command_loop
    if (!(flags & FLAG_C)) goto edit_command_loop;
    //     lda #0
    a = 0;
    //     sta input_buffer_offset+1
    input_buffer_offset = a;
    //     beq edit_command_loop
    goto edit_command_loop;

    // finished_editing_command:
finished_editing_command:
    //     lda l0081
    a = l0081;
    //     beq return_56
    if (flags & FLAG_Z) return;
    //     lda ptr1
    a = (uint8_t)(ptr1 & 0xff);
    //     sta current_format_line_ptr
    current_format_line_ptr = ptr1;
    //     lda ptr1+1
    a = (uint8_t)(ptr1 >> 8);
    //     sta current_format_line_ptr+1
    //     ldy #0
    y = 0;
    //     lda #0x80
    a = 0x80;
    //     sta (current_format_line_ptr),y
    ram[current_format_line_ptr + y] = a;
    //     jmp caf5c
    caf5c(); return;
}
static void cf8_mark_as_ruler_key(void) {
    // cf8_mark_as_ruler_key: Marks current line as a ruler line with . as default characters

    //     lda ptr1
    a = (uint8_t)(ptr1 & 0xff);
    //     sta current_format_line_ptr
    current_format_line_ptr = ptr1;
    //     lda ptr1+1
    a = (uint8_t)(ptr1 >> 8);
    //     sta current_format_line_ptr+1
    //     ldy #0
    y = 0;
    //     lda #0x81
    a = 0x81;
    //     sta (current_format_line_ptr),y
    ram[current_format_line_ptr + y] = a;
    //     iny
    y++;
    //     lda #0x2e ; '.'
    a = 0x2e;
    //     sta (current_format_line_ptr),y
    ram[current_format_line_ptr + y] = a;
    //     iny
    y++;
    //     lda #0x2e ; '.'
    a = 0x2e;
    //     sta (current_format_line_ptr),y
    ram[current_format_line_ptr + y] = a;
    //     inc l0074
    l0074++;
    //     lda l006e
    a = l006e;
    //     bmi c9f5f
    if (a & 0x80) goto c9f5f;
    //     lda #0x80
    a = 0x80;
    //     sta l006e
    l006e = 0x80;
    //     inc l006d
    l006d++;
    // c9f5f:
c9f5f:
    //     jmp caf5c
    caf5c(); return;
}
static void sf9_delete_command_key(void) {
    // sf9_delete_command_key: Deletes any formatting command prefix from current line

    //     ldy #0
    y = 0;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    //     jsr check_for_command_prefix
    check_for_command_prefix();
    //     bne return_56
    if (!(flags & FLAG_Z)) return;
    //     tya
    a = y;
    //     sta (current_format_line_ptr),y
    ram[current_format_line_ptr + y] = a;
    //     lda current_edit_line_ptr
    a = (uint8_t)(current_edit_line_ptr & 0xff);
    //     sta current_format_line_ptr
    current_format_line_ptr = current_edit_line_ptr;
    //     lda current_edit_line_ptr+1
    a = (uint8_t)(current_edit_line_ptr >> 8);
    //     sta current_format_line_ptr+1
    //     jsr sub_caf5f
    sub_caf5f();
    //     inc l0074
    l0074++;
    //     inc l006d
    l006d++;
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    // return_56:
    //     rts
}
static void sub_c9f80(void) {
    // c9f80:
    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    //     lda current_line_ptr
    a = current_line_ptr;
    //     ldy current_line_ptr+1
    y = current_line_ptr >> 8;
    //     jsr sub_cab37
    sub_cab37();
    //     bcc return_56
    if (!(flags & FLAG_C)) return;
    //     lda tmp0
    a = tmp0;
    //     sta current_line_ptr
    current_line_ptr = (current_line_ptr & 0xff00) | a;
    //     lda tmp1
    a = tmp1;
    //     sta current_line_ptr+1
    current_line_ptr = (current_line_ptr & 0x00ff) | ((uint16_t)a << 8);
    //     jsr sub_caa97
    sub_caa97();
    //     jsr c9e9b
    sub_c9e9b();
    //     dec l006f
    l006f--;
}
static void sf12_left_key(void) {
    // Pseudocode: Moves cursor left by one word

    // ; ***************************************************************************************
    // sf12_left_key:
    //     ldy xpos
    y = xpos;
    //     beq c9f80
    if (y == 0) { sub_c9f80(); return; }
    //     jsr draw_previous_word
    draw_previous_word();
    //     bne return_57
    if (!(flags & FLAG_Z)) return;
    //     cmp #0x20 ; ' '
    { uint16_t tmp_ = a - 0x20; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | (tmp_ & FLAG_N) | (a >= 0x20 ? FLAG_C : 0); }
    //     beq c9f80
    if (flags & FLAG_Z) { sub_c9f80(); return; }
// return_57:
//     rts
}
static void sf13_right_key(void) {
    // Pseudocode: Moves cursor right by one word

    // MULTIPLE ENTRY POINTS: sf12_left_key, sf13_right_key
    sub_c9f80();
}
static void set_marker(void);
static void set_marker_common(void);
// MULTIPLE ENTRY POINTS: sf7_set_marker_key, set_marker, set_marker_1..6
static void sf7_set_marker_key(void) {
    // sf7_set_marker_key:
    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    //     jsr prompt_for_marker
    prompt_for_marker();
    //     bcs return_58
    if (flags & FLAG_C) return;
    // set_marker:
    set_marker(); return;
}
static void set_marker_1(void) {
    // set_marker_1:
    a = '1';
    set_marker_common(); return;
}
static void set_marker_2(void) {
    // set_marker_2:
    a = '2';
    set_marker_common(); return;
}
static void set_marker_3(void) {
    // set_marker_3:
    a = '3';
    set_marker_common(); return;
}
static void set_marker_4(void) {
    // set_marker_4:
    a = '4';
    set_marker_common(); return;
}
static void set_marker_5(void) {
    // set_marker_5:
    a = '5';
    set_marker_common(); return;
}
static void set_marker_6(void) {
    // set_marker_6:
    a = '6';
    set_marker_common(); return;
}
static void set_marker_common(void) {
    //     pha
    uint8_t saved_a = a;
    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    //     pla
    a = saved_a;
    //     jsr lookup_marker
    lookup_marker();
    //     jmp set_marker
    set_marker(); return;
}
static void set_marker(void) {
    // set_marker:
    //     jsr set_marker_to_here
    set_marker_to_here();
    //     jmp ca035
    a = 1;
    l0073 = a;
    ca684(); return;
}
static void go_to_marker(void);
// MULTIPLE ENTRY POINTS: sf6_go_to_marker_key, go_to_marker, go_to_marker_1..6
static void sf6_go_to_marker_key(void) {
    // sf6_go_to_marker_key:
    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    //     jsr prompt_for_marker
    prompt_for_marker();
    //     bcs return_58
    if (flags & FLAG_C) return;
    //     beq return_58
    if (flags & FLAG_Z) return;
    // go_to_marker:
    go_to_marker(); return;
}
static void go_to_marker_1(void) {
    // go_to_marker_1:
    a = '1';
    go_to_marker_n(); return;
}
static void go_to_marker_2(void) {
    // go_to_marker_2:
    a = '2';
    go_to_marker_n(); return;
}
static void go_to_marker_3(void) {
    // go_to_marker_3:
    a = '3';
    go_to_marker_n(); return;
}
static void go_to_marker_4(void) {
    // go_to_marker_4:
    a = '4';
    go_to_marker_n(); return;
}
static void go_to_marker_5(void) {
    // go_to_marker_5:
    a = '5';
    go_to_marker_n(); return;
}
static void go_to_marker_6(void) {
    // go_to_marker_6:
    a = '6';
    go_to_marker_n(); return;
}
static void go_to_marker_n(void) {
    //     jsr lookup_marker
    lookup_marker();
    //     jmp go_to_marker
    go_to_marker(); return;
}
static void go_to_marker(void) {
    // go_to_marker:
    //     lda markers_array,x
    a = ((uint8_t*)markers_array)[x];
    //     ldy markers_array+1,x
    y = ((uint8_t*)markers_array)[x+1];
    //     jsr move_cursor_to_address
    move_cursor_to_address();
    // ca035:
    //     lda #1
    a = 1;
    //     sta l0073
    l0073 = a;
    //     jmp ca684
    ca684(); return;
}
static void f0_format_block_key(void) {
    write_line_back_to_document_safely();
    uint8_t saved_l0073 = l0073;
    uint8_t saved_l003d = l003d;
    ca741();
    sub_c9977();
    if (flags & FLAG_V) { show_memory_full_error(); longjmp(env, JMP_EDITOR); }
    if (flags & FLAG_Z) {
        l003d = saved_l003d;
        l0073 = saved_l0073;
    }
    // If Z=0, values are discarded (equivalent to pla/tax + pla + bcs return)
}
static void f1_top_of_text_key(void) {
    x = 0xff;
    l006f = x;
    sub_ca071();
    sub_caa97();
}
static void sub_ca071(void) {
    // sub_ca071:
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    //     stx input_buffer_offset+1
    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     ldy current_line_ptr+1
    y = (uint8_t)((current_line_ptr >> 8) & 0xff);
    // ca07c:
    while (1) {
        //     sta tmp2
        tmp2 = a;
        //     sty tmp3
        tmp3 = y;
        //     jsr sub_cab37
        sub_cab37();
        //     lda tmp0
        a = tmp0;
        //     ldy tmp1
        y = tmp1;
        //     bcc ca093
        if (!(flags & FLAG_C)) {
            // ca093:
            //     lda tmp2
            a = tmp2;
            //     ldy tmp3
            y = tmp3;
            break;
        }
        //     ldx input_buffer_offset+1
        x = 0;
        //     bmi ca07c
        if ((int8_t)x < 0) continue;
        //     dec input_buffer_offset+1
        x--;
            //     bne ca07c
        if (x != 0) continue;
        break;
    }
    // ca097:
    //     sta current_line_ptr
    current_line_ptr = (current_line_ptr & 0xff00) | a;
    //     sty current_line_ptr+1
    current_line_ptr = (current_line_ptr & 0x00ff) | ((uint16_t)y << 8);
    //     rts
}
static void sf15_up_key(void) {
    // sf15_up_key:
    //     ldx screen_height
    x = screen_height;
    //     inc l0079
    l0079++;
    //     inc l006f
    l006f++;
    sub_ca071();
}
static void f2_bottom_of_text_key(void) {
    // Pseudocode: Moves cursor to bottom of document

    // ; ***************************************************************************************
    // f2_bottom_of_text_key:
    //     ldx #0xff
    x = 0xff;
    //     stx l006f
    l006f = x;
    //     jsr sub_ca0af
    sub_ca0af();
    //     jsr sub_caa97
    sub_caa97();
    //     jmp c9e9b
    sub_c9e9b();
}
static void sub_ca0af(void) {
    // sub_ca0af:
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    //     stx input_buffer_offset+1
    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     ldy current_line_ptr+1
    y = (uint8_t)((current_line_ptr >> 8) & 0xff);
    // ca0ba:
    while (1) {
        //     jsr sub_cab1a
        sub_cab1a();
        //     beq ca0d2
        if (flags & FLAG_Z) {
            // ca0d2:
            //     lda tmp0
            a = tmp0;
            //     ldy tmp1
            y = tmp1;
            break;
        }
        //     tya
        //     ldy tmp1
        //     clc
        //     adc tmp0
        {
            uint16_t sum = (uint16_t)y + tmp0;
            y = tmp1;
            a = (uint8_t)(sum & 0xff);
            if (sum > 0xff) y++;
        }
        // ca0c8:
        //     ldx input_buffer_offset+1
        x = 0;
        //     bmi ca0ba
        if ((int8_t)x < 0) continue;
        //     dec input_buffer_offset+1
        x--;
            //     bne ca0ba
        if (x != 0) continue;
        //     beq ca0d6
        break;
    }
    // ca0d6:
    //     sta current_line_ptr
    current_line_ptr = (current_line_ptr & 0xff00) | a;
    //     sty current_line_ptr+1
    current_line_ptr = (current_line_ptr & 0x00ff) | ((uint16_t)y << 8);
    //     rts
}
static void sf14_down_key(void) {
    // sf14_down_key:
    //     ldx screen_height
    //     inc l0079
    //     inc l006f
    x = screen_height;
    l0079++;
    l006f++;
    sub_ca0af();
}
static void sf11_copy_key(void) {
    f6_insert_line_key();
    redraw_editor();
    x = l003a;
    if (x != 0) {
        y = 0;
        do {
            ram[current_edit_line_ptr + y] = ram[current_ruler_ptr + y];
            y++;
            x--;
        } while (x != 0);
    }
    cf8_mark_as_ruler_key();
}
static void cf5_default_ruler_key(void) {
    f6_insert_line_key();
    redraw_editor();
    cf8_mark_as_ruler_key();
    a = (uint8_t)(current_edit_line_ptr & 0xff);
    y = (uint8_t)(current_edit_line_ptr >> 8);
    create_default_ruler();
}
static void sf3_delete_to_char_key(void) {
    x = 0x43;
    y = 0x48;
    draw_prompt_characters();
    flags_need_redrawing_flag++;
    read_char();
    if (a == 9) goto ca12a;
    if (a != 0xa0) goto ca11a;
    a = 0x1c;
    goto ca12a;
ca11a:
    if (a != 0xa1) goto ca122;
    a = 0x1d;
    goto ca12a;
ca122:
    if (a < 0x20) { beep(); return; }
    if (a >= 0x7f) { beep(); return; }
ca12a:
    {
        uint8_t search_char = a;
        l0074++;
        y = xpos;
        uint8_t start_x = y;
        // loop_ca132: scan forward to find matching char
        while (y < 0x84) {
            a = ram[current_edit_line_ptr + y];
            y++;
            if (a == search_char) goto loop_ca13d;
        }
        beep(); return;
loop_ca13d:
        // loop_ca13d: scan forward to find end of matching sequence
        while (y < 0x84) {
            a = ram[current_edit_line_ptr + y];
            y++;
            if (a != search_char) break;
        }
        y--;
        x = y - start_x;
        cae64();
    }
}
static void cf2_format_mode_key(void) {
    a = format_mode_flag;
    a &= 0xbf;
    if (format_mode_flag & 0x40) a |= 1;
    a ^= 1;
    format_mode_flag = a;
    flags_need_redrawing_flag++;
}
static void cf3_justify_mode_key(void) {
    justifying_flag ^= 0xff;
    flags_need_redrawing_flag++;
}
static void cf4_insert_mode_key(void) {
    insert_mode_flag ^= 0xff;
    flags_need_redrawing_flag++;
}
static void cf0_delete_block_key(void) {
    write_line_back_to_document_safely();
    cursor_moved_flag++;
    reset_area_to_marks_1_2();
    if (flags & FLAG_C) { beep(); return; }
    a = (uint8_t)(area_start_ptr & 0xff);
    y = (uint8_t)(area_start_ptr >> 8);
    move_cursor_to_address();
    ca741();
    sub_c89d3();
    cb05a();
    clear_marks_1_2();
}
static void sf0_move_block_key(void) {
    write_line_back_to_document_safely();
    reset_area_to_marks_1_2();
    if (flags & FLAG_C) { beep(); return; }
    sub_ca1cc();
    x = 0xff;
    l0012 = x;
    l006f = x;
    sub_c89d3();
    cb05a();
    a = (uint8_t)(doc_ptr1 & 0xff);
    y = (uint8_t)(doc_ptr1 >> 8);
    move_cursor_to_address();
    clear_marks_1_2();
}
static void f11_copy_key(void) {
    write_line_back_to_document_safely();
    reset_area_to_marks_1_2();
    if (flags & FLAG_C) { beep(); return; }
    sub_ca1cc();
    a = (uint8_t)(doc_ptr1 & 0xff);
    y = (uint8_t)(doc_ptr1 >> 8);
    move_cursor_to_address();
}
static void sub_ca1cc(void) {
    if (doc_ptr1 >= area_start_ptr && doc_ptr1 < area_end_ptr) {
        beep(); return;
    }
    a = (uint8_t)(area_start_ptr & 0xff);
    y = (uint8_t)(area_start_ptr >> 8);
    move_cursor_to_address();
    {
        uint16_t diff = area_end_ptr - area_start_ptr;
        tmp6 = (uint8_t)(diff & 0xff);
        tmp7 = (uint8_t)(diff >> 8);
    }
    tmp4 = (uint8_t)(doc_ptr1 & 0xff);
    tmp5 = (uint8_t)(doc_ptr1 >> 8);
    make_space_for_insertion();
    if (flags & FLAG_C) { show_memory_full_error(); longjmp(env, JMP_EDITOR); }
    tmp8 = (uint8_t)(area_start_ptr & 0xff);
    tmp9 = (uint8_t)(area_start_ptr >> 8);
    tmp2 = tmp4;
    tmp3 = tmp5;
    // ca219:
    while (1) {
        ram[((uint16_t)tmp3 << 8) | ((uint16_t)tmp2)] = ram[((uint16_t)tmp9 << 8) | ((uint16_t)tmp8)];
        tmp2++;
        if (tmp2 == 0) tmp3++;
        tmp8++;
        if (tmp8 == 0) tmp9++;
        if (((uint16_t)tmp9 << 8 | tmp8) == area_end_ptr) break;
    }
    uint8_t saved_tmp6 = tmp6;
    uint8_t saved_tmp7 = tmp7;
    doc_ptr1 = (uint16_t)tmp5 << 8 | tmp4;
    {
        uint16_t adjusted = ((uint16_t)tmp3 << 8 | tmp2) - 1;
        a = (uint8_t)(adjusted & 0xff);
        y = (uint8_t)(adjusted >> 8);
    }
    cac78();
    a = (uint8_t)(doc_ptr1 & 0xff);
    y = (uint8_t)(doc_ptr1 >> 8);
    cac78();
    tmp7 = saved_tmp7;
    tmp6 = saved_tmp6;
    l0073 = 1;
    cursor_moved_flag = 1;
    flags &= ~FLAG_C;
}
static void cf1_next_match_key(void) {
    write_line_back_to_document_safely();
    c8b7b();
    if (!(flags & FLAG_Z)) { esc_key(); return; }
    move_cursor_to_address();
}
static void redraw_editor(void) {
    // Pseudocode: Main screen update routine: scrolls, redraws lines, updates status and cursor
    uint8_t saved_status_line_needs_redrawing_flag;

    // redraw_editor:
    //     jsr cursor_off
    cursor_off();
    //     lda ruler_stack_ptr
    a = ruler_stack_ptr;
    //     sta l0034
    l0034 = a;
    //     lda status_line_needs_redrawing_flag
    a = status_line_needs_redrawing_flag;
    //     sta input_buffer_offset+1
    saved_status_line_needs_redrawing_flag = a;
    //     lda l006e
    a = l006e;
    //     beq ca28e
    if (a == 0) goto ca28e;
    //     lda l0073
    a = l0073;
    //     ora l006f
    a |= l006f;
    //     beq ca28e
    if (a == 0) goto ca28e;
    //     jmp ca360
    goto ca360;

    // ca28e:
ca28e:
    //     lda current_line_ptr+1
    a = (uint8_t)(current_line_ptr >> 8);
    //     cmp l0012
    { uint16_t tmp_ = a - l0012; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= l0012 ? FLAG_C : 0); }
    //     bcc ca29c
    if (!(flags & FLAG_C)) goto ca29c;
    //     bne ca2dc
    if (!(flags & FLAG_Z)) goto ca2dc;
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     cmp l0011
    { uint16_t tmp_ = a - l0011; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= l0011 ? FLAG_C : 0); }
    //     bcs ca2dc
    if (flags & FLAG_C) goto ca2dc;
    // ca29c:
ca29c:
    //     lda l006f
    a = l006f;
    //     bne ca30d
    if (a != 0) goto ca30d;
    //     lda l0033
    a = l0033;
    //     sta ruler_stack_ptr
    ruler_stack_ptr = a;
    //     ldy l0012
    y = l0012;
    //     lda l0011
    a = l0011;
    //     cpy top+1
    { uint16_t tmp_ = y - (uint8_t)(top >> 8); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= (uint8_t)(top >> 8) ? FLAG_C : 0); }
    //     bcc ca2b2
    if (!(flags & FLAG_C)) goto ca2b2;
    //     bne ca30d
    if (!(flags & FLAG_Z)) goto ca30d;
    //     cmp top
    { uint16_t tmp_ = a - (uint8_t)(top & 0xff); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= (uint8_t)(top & 0xff) ? FLAG_C : 0); }
    //     bcs ca30d
    if (flags & FLAG_C) goto ca30d;
    // ca2b2:
ca2b2:
    //     jsr sub_cab37
    sub_cab37();
    //     ldy tmp1
    y = tmp1;
    //     cpy current_line_ptr+1
    { uint16_t tmp_ = y - (uint8_t)(current_line_ptr >> 8); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= (uint8_t)(current_line_ptr >> 8) ? FLAG_C : 0); }
    //     bne ca30d
    if (!(flags & FLAG_Z)) goto ca30d;
    //     lda tmp0
    a = tmp0;
    //     cmp current_line_ptr
    { uint16_t tmp_ = a - (uint8_t)(current_line_ptr & 0xff); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= (uint8_t)(current_line_ptr & 0xff) ? FLAG_C : 0); }
    //     bne ca30d
    if (!(flags & FLAG_Z)) goto ca30d;
    //     sty l0012
    l0012 = y;
    //     sta l0011
    l0011 = a;
    //     ldx screen_height
    x = screen_height;
    // loop_ca2c7:
loop_ca2c7:
    //     dex
    x--;
    //     lda line_lengths,x
    a = line_lengths[x];
    //     inx
    x++;
    //     sta line_lengths,x
    line_lengths[x] = a;
    //     dex
    x--;
    //     bne loop_ca2c7
    if (x != 0) goto loop_ca2c7;
    //     ldy #SCREEN_SCROLLDOWN
    //     jsr SCREEN
    screen_scrolldown();
    //     jsr home_cursor
    x = 0; y = 1; set_cursor_position();
    //     ldy #1
    y = 1;
    //     jmp ca351
    goto ca351;

    // ca2dc:
ca2dc:
    //     lda l0033
    a = l0033;
    //     sta ruler_stack_ptr
    ruler_stack_ptr = a;
    // ca2e0:
ca2e0:
    //     ldx #0
    x = 0;
    //     lda l0011
    a = l0011;
    //     ldy l0012
    y = l0012;
    // ca2e6:
ca2e6:
    //     inx
    x++;
    //     cpy ptr6+1
    { uint16_t tmp_ = y - (uint8_t)(ptr6 >> 8); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= (uint8_t)(ptr6 >> 8) ? FLAG_C : 0); }
    //     bne ca2f1
    if (!(flags & FLAG_Z)) goto ca2f1;
    //     cmp ptr6
    { uint16_t tmp_ = a - (uint8_t)(ptr6 & 0xff); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= (uint8_t)(ptr6 & 0xff) ? FLAG_C : 0); }
    //     bne ca2f1
    if (!(flags & FLAG_Z)) goto ca2f1;
    //     stx l003d
    l003d = x;
    // ca2f1:
ca2f1:
    //     cpy current_line_ptr+1
    { uint16_t tmp_ = y - (uint8_t)(current_line_ptr >> 8); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= (uint8_t)(current_line_ptr >> 8) ? FLAG_C : 0); }
    //     bne ca2f9
    if (!(flags & FLAG_Z)) goto ca2f9;
    //     cmp current_line_ptr
    { uint16_t tmp_ = a - (uint8_t)(current_line_ptr & 0xff); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= (uint8_t)(current_line_ptr & 0xff) ? FLAG_C : 0); }
    //     beq ca313
    if (flags & FLAG_Z) goto ca313;
    // ca2f9:
ca2f9:
    //     jsr sub_cab1a
    sub_cab1a();
    //     beq ca313
    if (flags & FLAG_Z) goto ca313;
    //     tya
    a = y;
    //     ldy tmp1
    y = tmp1;
    //     clc
    flags &= ~FLAG_C;
    //     adc tmp0
    { uint16_t sum = (uint16_t)a + tmp0; a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     bcc ca307
    if (!(flags & FLAG_C)) goto ca307;
    //     iny
    y++;
    // ca307:
ca307:
    //     cpx screen_height
    { uint16_t tmp_ = x - screen_height; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= screen_height ? FLAG_C : 0); }
    //     beq ca2e6
    if (flags & FLAG_Z) goto ca2e6;
    //     bcc ca2e6
    if (!(flags & FLAG_C)) goto ca2e6;
    // ca30d:
ca30d:
    //     jsr sub_ca44e
    sub_ca44e();
    //     jmp ca2e0
    goto ca2e0;

    // ca313:
ca313:
    //     cpx screen_height
    { uint16_t tmp_ = x - screen_height; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= screen_height ? FLAG_C : 0); }
    //     bcc ca35e
    if (!(flags & FLAG_C)) goto ca35e;
    //     beq ca35e
    if (flags & FLAG_Z) goto ca35e;
    //     lda l006f
    a = l006f;
    //     bne ca30d
    if (a != 0) goto ca30d;
    //     ldx #0
    x = 0;
    // loop_ca31f:
loop_ca31f:
    //     lda line_lengths+1,x
    a = line_lengths[x + 1];
    //     sta line_lengths,x
    line_lengths[x] = a;
    //     inx
    x++;
    //     cpx screen_height
    { uint16_t tmp_ = x - screen_height; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= screen_height ? FLAG_C : 0); }
    //     bne loop_ca31f
    if (!(flags & FLAG_Z)) goto loop_ca31f;
    //     dec l003d
    l003d--;
    //     ldx #0
    x = 0;
    //     lda screen_width
    a = screen_width;
    //     sta line_lengths,x
    line_lengths[x] = a;
    //     lda l0033
    a = l0033;
    //     sta ruler_stack_ptr
    ruler_stack_ptr = a;
    //     ldy l0012
    y = l0012;
    //     lda l0011
    a = l0011;
    //     jsr sub_cab1a
    sub_cab1a();
    //     tya
    a = y;
    //     clc
    flags &= ~FLAG_C;
    //     adc l0011
    { uint16_t sum = (uint16_t)a + l0011; a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     sta l0011
    l0011 = a;
    //     bcc ca348
    if (!(flags & FLAG_C)) goto ca348;
    //     inc l0012
    l0012++;
    // ca348:
ca348:
    //     ldy #SCREEN_SCROLLUP
    //     jsr SCREEN
    screen_scrollup();
    //     ldx #0
    x = 0;
    //     ldy screen_height
    y = screen_height;
    //     jsr set_cursor_position
    set_cursor_position();
    // ca351:
ca351:
    //     lda ruler_stack_ptr
    a = ruler_stack_ptr;
    //     sta l0033
    l0033 = a;
    //     inc input_buffer_offset+1
    saved_status_line_needs_redrawing_flag++;
    //     inc l0074
    l0074++;
    //     tya
    a = y;
    //     tax
    x = a;
    // ca35e:
ca35e:
    //     stx ypos
    ypos = x;
    // ca360:
ca360:
    //     ldy l0034
    y = l0034;
    //     jsr cab91
    cab91();
    //     jsr unpack_line_into_buffer
    unpack_line_into_buffer();
    //     jsr sub_ca608
    recalculate_cursor_xpos();
    //     lda screen_width
    a = screen_width;
    //     lsr
    a >>= 1;
    //     sta l0083
    l0083 = a;
    //     lda l0072
    a = l0072;
    //     cmp hscroll_pos
    { uint16_t tmp_ = a - hscroll_pos; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= hscroll_pos ? FLAG_C : 0); }
    //     bcc ca381
    if (!(flags & FLAG_C)) goto ca381;
    //     lda hscroll_pos
    a = hscroll_pos;
    //     clc
    flags &= ~FLAG_C;
    //     adc screen_width
    { uint16_t sum = (uint16_t)a + screen_width; a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     sbc #3
    { uint16_t tmp_ = (uint16_t)a - 3 - (1 - (flags & FLAG_C ? 1 : 0)); a = (uint8_t)tmp_; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (tmp_ < 0x10000 ? FLAG_C : 0); }
    //     cmp l0072
    { uint16_t tmp_ = a - l0072; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= l0072 ? FLAG_C : 0); }
    //     bcs ca395
    if (flags & FLAG_C) goto ca395;
    // ca381:
ca381:
    //     lda l0072
    a = l0072;
    //     sec
    flags |= FLAG_C;
    //     sbc l0083
    { uint16_t tmp_ = (uint16_t)a - l0083 - (1 - (flags & FLAG_C ? 1 : 0)); a = (uint8_t)tmp_; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (tmp_ < 0x10000 ? FLAG_C : 0); }
    //     bcs ca38a
    if (flags & FLAG_C) goto ca38a;
    //     lda #0
    a = 0;
    // ca38a:
ca38a:
    //     sta hscroll_pos
    hscroll_pos = a;
    //     lda #1
    a = 1;
    //     sta l0073
    l0073 = a;
    //     sta input_buffer_offset+1
    saved_status_line_needs_redrawing_flag = a;
    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    // ca395:
ca395:
    //     lda input_buffer_offset+1
    a = saved_status_line_needs_redrawing_flag;
    //     sta status_line_needs_redrawing_flag
    status_line_needs_redrawing_flag = a;
    //     lda l0073
    a = l0073;
    //     beq ca3e7
    if (a == 0) goto ca3e7;
    //     bpl ca3b2
    if (!((int8_t)a < 0)) goto ca3b2;
    //     lda l003d
    a = l003d;
    //     bmi ca3b2
    if ((int8_t)a < 0) goto ca3b2;
    //     sta l0082
    l0082 = a;
    //     lda screen_height
    a = screen_height;
    //     sec
    flags |= FLAG_C;
    //     sbc l003d
    { uint16_t tmp_ = (uint16_t)a - l003d - (1 - (flags & FLAG_C ? 1 : 0)); a = (uint8_t)tmp_; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (tmp_ < 0x10000 ? FLAG_C : 0); }
    //     tax
    x = a;
    //     inx
    x++;
    //     lda ptr6
    a = (uint8_t)(ptr6 & 0xff);
    //     ldy ptr6+1
    y = (uint8_t)(ptr6 >> 8);
    //     bne ca3c1
    if (y != 0) goto ca3c1;
    // ca3b2:
ca3b2:
    //     ldy l0033
    y = l0033;
    //     jsr cab91
    cab91();
    //     lda #1
    a = 1;
    //     sta l0082
    l0082 = a;
    //     lda l0011
    a = l0011;
    //     ldy l0012
    y = l0012;
    //     ldx screen_height
    x = screen_height;
    // ca3c1:
ca3c1:
    //     stx l0081
    l0081 = x;
    // loop_ca3c3:
loop_ca3c3:
    //     jsr draw_line
    draw_line();
    //     lda tmp0
    a = tmp0;
    //     ldy tmp1
    y = tmp1;
    //     jsr sub_cab1a
    sub_cab1a();
    //     beq ca422
    if (flags & FLAG_Z) { draw_ruler(); sub_ca422(); goto ca3ff; }
    //     tya
    a = y;
    //     ldy tmp1
    y = tmp1;
    //     clc
    flags &= ~FLAG_C;
    //     adc tmp0
    { uint16_t sum = (uint16_t)a + tmp0; a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     bcc ca3d8
    if (!(flags & FLAG_C)) goto ca3d8;
    //     iny
    y++;
    // ca3d8:
ca3d8:
    //     inc l0082
    l0082++;
    //     dec l0081
    l0081--;
    //     bne loop_ca3c3
    if (l0081 != 0) goto loop_ca3c3;
    // ca3de:
ca3de:
    //     lda #0
    a = 0;
    //     sta l0074
    l0074 = a;
    //     ldy l0034
    y = l0034;
    //     jsr cab91
    cab91();
    // ca3e7:
ca3e7:
    //     jsr unpack_line_into_buffer
    unpack_line_into_buffer();
    //     jsr sub_caacb
    sub_caacb();
    //     jsr draw_ruler
    draw_ruler();
    //     lda l0074
    a = l0074;
    //     beq ca3ff
    if (a == 0) goto ca3ff;
    //     lda ypos
    a = ypos;
    //     sta l0082
    l0082 = a;
    //     lda current_format_line_ptr
    a = (uint8_t)(current_format_line_ptr & 0xff);
    //     ldy current_format_line_ptr+1
    y = (uint8_t)(current_format_line_ptr >> 8);
    //     jsr draw_line
    draw_line();
    // ca3ff:
ca3ff:
    //     lda flags_need_redrawing_flag
    a = flags_need_redrawing_flag;
    //     beq ca406
    if (a == 0) goto ca406;
    //     jsr sub_ca651
    draw_status_word();
    // ca406:
ca406:
    //     lda l0072
    a = l0072;
    //     sec
    flags |= FLAG_C;
    //     sbc hscroll_pos
    { uint16_t tmp_ = (uint16_t)a - hscroll_pos - (1 - (flags & FLAG_C ? 1 : 0)); a = (uint8_t)tmp_; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (tmp_ < 0x10000 ? FLAG_C : 0); }
    //     clc
    flags &= ~FLAG_C;
    //     adc #3
    { uint16_t sum = (uint16_t)a + 3; a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     tax
    x = a;
    //     ldy #0
    y = 0;
    //     sty l0073
    l0073 = y;
    //     sty l0074
    l0074 = y;
    //     sty l006f
    l006f = y;
    //     dey                                                               ; Y=0xff
    y--;
    //     sty ptr6+1
    ptr6 = (ptr6 & 0x00ff) | ((uint16_t)y << 8);
    //     ldy ypos
    y = ypos;
    //     jsr set_cursor_position
    set_cursor_position();
    //     jmp cursor_on
    cursor_on(); return;
}
static void sub_ca422(void) {
    l0081--;
    if (l0081 == 0) return;
    x = l0082;
    line_lengths[x + 1] = screen_width;
    l0083 = screen_width;
    do {
        l0082++;
        x = 0;
        y = l0082;
        set_cursor_position();
        clear_to_eol();
        line_lengths[x] = l0083;
        l0083 = 0;
        l0081--;
    } while (l0081 != 0);
}
static void sub_ca44e(void) {
    // sub_ca44e: Computes starting line for display based on screen position

    //     lda l0034
    a = l0034;
    //     sta ruler_stack_ptr
    ruler_stack_ptr = a;
    //     lda screen_height
    a = screen_height;
    //     sta l0073
    l0073 = a;
    //     lsr
    a >>= 1;
    //     tax
    x = a;
    //     inx
    x++;
    //     lda l006f
    a = l006f;
    //     bmi ca461
    if (a & 0x80) goto ca461;
    //     beq ca461
    if (a == 0) goto ca461;
    //     ldx ypos
    x = ypos;
    // ca461:
ca461:
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     ldy current_line_ptr+1
    y = (uint8_t)(current_line_ptr >> 8);
    // loop_ca465:
loop_ca465:
    //     dex
    x--;
    //     beq ca479
    if (x == 0) goto ca479;
    //     sta tmp2
    tmp2 = a;
    //     sty tmp3
    tmp3 = y;
    //     jsr sub_cab37
    sub_cab37();
    //     lda tmp0
    a = tmp0;
    //     ldy tmp1
    y = tmp1;
    //     bcs loop_ca465
    if (flags & FLAG_C) goto loop_ca465;
    //     lda tmp2
    a = tmp2;
    //     ldy tmp3
    y = tmp3;
    // ca479:
ca479:
    //     sta l0011
    l0011 = a;
    //     sty l0012
    l0012 = y;
    //     lda ruler_stack_ptr
    a = ruler_stack_ptr;
    //     sta l0033
    l0033 = a;
    //     lda l0034
    a = l0034;
    //     sta ruler_stack_ptr
    ruler_stack_ptr = a;
    //     rts
}
static void draw_line(void) {
    // draw_line: Renders a single document line to the screen

    //     sta tmp0
    tmp0 = a;
    //     sta tmp6
    tmp6 = a;
    //     sty tmp1
    tmp1 = y;
    //     sty tmp7
    tmp7 = y;
    //     ldx #0
    x = 0;
    //     ldy l0082
    y = l0082;
    //     jsr set_cursor_position
    set_cursor_position();
    //     ldy #0
    y = 0;
    //     sty l0083
    l0083 = 0;
    //     sty input_buffer_offset+1
    input_buffer_offset = 0;
    //     sty l0039
    l0039 = 0;
    //     jsr deref_and_check_for_command_prefix
    deref_and_check_for_command_prefix();
    //     bne ca4b4
    if (!(flags & FLAG_Z)) goto ca4b4;
    //     ldy #3
    y = 3;
    //     lda hscroll_pos
    a = hscroll_pos;
    //     bne ca4b4
    if (!(flags & FLAG_Z)) goto ca4b4;
    //     ldy #1
    y = 1;
    //     jsr sub_ca4d7
    sub_ca4d7();
    //     jsr sub_ca4d7
    sub_ca4d7();
    //     lda #0x20 ; ' '
    a = 0x20;
    //     bne ca4bc
    goto ca4bc;

    // ca4b4:
ca4b4:
    //     lda #0x20 ; ' '
    a = 0x20;
    //     jsr ca4e9
    render_char();
    //     jsr ca4e9
    render_char();
// ca4bc:
ca4bc:
    //     jsr ca4e9
    render_char();
    // loop_ca4bf:
    loop_ca4bf:
    //     jsr draw_char
    draw_char();
    // loop_ca4c2:
loop_ca4c2:
    //     jsr render_xchar
    render_xchar();
    //     dex
    x--;
    //     bne loop_ca4c2
    if (x != 0) goto loop_ca4c2;
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     bne loop_ca4bf
    if (!(flags & FLAG_Z)) goto loop_ca4bf;
    //     lda #0x20 ; ' '
    a = 0x20;
    //     jsr sub_ca597
    clear_to_eol();
    //     lda l0083
    a = l0083;
    //     sta line_lengths,x
    line_lengths[x] = a;
    //     rts
}
static void sub_ca4d7(void) {
    // sub_ca4d7: Draws a character and advances x position

    //     jsr draw_char
    draw_char();
    //     jmp ca4e9
    render_char();
}
static void render_char(void) {
    // ca4e9: Renders character in A to screen with attribute handling

    uint8_t char_to_render = a;
    uint8_t marker_idx = 0;

    //     ldx l0082
    x = l0082;
    //     lda line_lengths,x
    if (line_lengths[x] != 0) {
    //     dec line_lengths,x
        line_lengths[x]--;
    }
    // ca4f4:
    //     ldx l0083
    x = l0083;
    //     cpx screen_width
    if (x >= screen_width) { a = char_to_render; return; }
    //     inc l0083
    l0083++;
    //     tya
    a = y;
    //     beq ca514
    if (a == 0) goto ca514;
    //     dey
    y--;
    //     jsr sub_ca536
    sub_ca536();
    //     iny
    y++;
    //     cpx #4
    if (x >= 4) goto ca514;
    //     tax
    marker_idx = a;
    x = marker_idx;
    //     bmi ca523
    if (x & 0x80) goto ca523;
    //     bne ca514
    if (x != 0) goto ca514;
    // Marker match at index < 4: render inverted
    goto ca50e;
    // ca514:
ca514:
    a = char_to_render;
    //     jsr check_for_control_code
    check_for_control_code();
    if (!(flags & FLAG_Z)) goto ca522;
    if (flags & FLAG_C) {
        a = 0x2d;
    } else {
        a = 0x2a;
    }
    // ca50e:
ca50e:
    a = char_to_render;
    a = STYLE_REVERSE; screen_setstyle(a);
    x = 0;
    goto ca514;
    // ca522:
ca522:
    // ca523:
ca523:
    a = char_to_render;
    //     cmp #0x0d
    if (a == 0x0d || a == 0x00) {
        a = 0x20;
    }
    // ca529:
    //     jsr screen_putchar
    screen_putchar(a);
    //     txa
    a = char_to_render;
    //     bne ca532
    if (a != 0) goto ca532;
    //     jsr set_normal_text_if_not_mode_7
    a = 0; screen_setstyle(a);
    // ca532:
    // ca533:
ca532:
    //     ldx l0084
    x = l0084;
    //     rts
}
static void render_xchar(void) {
    // render_xchar: Renders a character to screen with style/attribute handling

    //     inc l0039
    l0039++;
    //     stx l0084
    l0084 = x;
    //     ldx input_buffer_offset+1
    x = 0;
    //     inc input_buffer_offset+1
    /* high byte inc removed */
    //     cpx hscroll_pos
    { uint16_t tmp_ = x - hscroll_pos; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= hscroll_pos ? FLAG_C : 0); }
    //     bcc ca533
    if (!(flags & FLAG_C)) { x = l0084; return; }
    //     jmp ca4e9
    render_char();
}
static void sub_ca536(void) {
    // Pseudocode: Checks if a position in the edit line corresponds to a marker

    // sub_ca536:
    //     tya
    a = y;
    //     clc
    flags &= ~FLAG_C;
    //     adc tmp6
    { uint16_t tmp_ = (uint16_t)a + tmp6; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C|FLAG_V)) | ((tmp_ & 0xff) == 0 ? FLAG_Z : 0) | ((tmp_ & 0x80) ? FLAG_N : 0) | (tmp_ > 255 ? FLAG_C : 0) | (((~(a ^ tmp6) & (a ^ (uint8_t)tmp_)) >> 1) & FLAG_V); a = (uint8_t)tmp_; }
    //     sta tmp8
    tmp8 = a;
    //     lda tmp7
    a = tmp7;
    //     adc #0
    { uint16_t tmp_ = (uint16_t)a + 0; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C|FLAG_V)) | ((tmp_ & 0xff) == 0 ? FLAG_Z : 0) | ((tmp_ & 0x80) ? FLAG_N : 0) | (tmp_ > 255 ? FLAG_C : 0) | (((~(a ^ 0) & (a ^ (uint8_t)tmp_)) >> 1) & FLAG_V); a = (uint8_t)tmp_; }
    //     sta tmp9
    tmp9 = a;
    //     ldx #0
    x = 0;
    // loop_ca544:
loop_ca544:
    //     lda tmp9
    a = tmp9;
    //     cmp markers_array+1,x
    { uint16_t tmp_ = a - ((uint8_t*)markers_array)[x+1]; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= ((uint8_t*)markers_array)[x+1] ? FLAG_C : 0); }
    //     bne ca550
    if (!(flags & FLAG_Z)) goto ca550;
    //     lda tmp8
    a = tmp8;
    //     cmp markers_array,x
    { uint16_t tmp_ = a - ((uint8_t*)markers_array)[x]; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= ((uint8_t*)markers_array)[x] ? FLAG_C : 0); }
    //     beq ca558
    if (flags & FLAG_Z) goto ca558;
    // ca550:
ca550:
    //     inx
    x++;
    //     inx
    x++;
    //     cpx #0x0c
    { uint16_t tmp_ = x - 0x0c; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= 0x0c ? FLAG_C : 0); }
    //     bne loop_ca544
    if (!(flags & FLAG_Z)) goto loop_ca544;
    //     txa
    a = x;
    //     rts
    return;

    // ca558:
ca558:
    //     lda #0
    a = 0;
    // return_61:
    //     rts
    return;
}
static void clear_to_eol(void) {
    // Pseudocode: Fills remaining space on line with spaces to clear to end

    // sub_ca597:
    //     ldx l0082
    x = l0082;
    //     sta l0084
    l0084 = a;
    //     lda line_lengths,x
    a = line_lengths[x];
    //     beq return_62
    if (flags & FLAG_Z) goto return_62;
    //     lda l0084
    a = l0084;
    // loop_ca5a2:
loop_ca5a2:
    //     jsr screen_putchar
    screen_putchar(a);
    //     dec line_lengths,x
    line_lengths[x]--;
    //     bne loop_ca5a2
    if (!(flags & FLAG_Z)) goto loop_ca5a2;
    // return_62:
return_62:
    //     rts
    return;
}
// Input:  a = document character, y = line offset (for tab stop lookup)
// Output: a = character to render, x = screen width consumed, y preserved, flags.C=0
static void process_document_character(void) {
    // sub_ca5ae:
    //     cmp #9
    if (a == 9) goto ca5e1;
    //     cmp #0x10
    if (a == 0x10) goto ca5d5;
    //     cmp #0x0b
    if (a == 0x0b) goto ca5d9;
    //     cmp #0x1a
    if (a == 0x1a) goto ca5d5;
    //     bcc ca5d1
    if (a < 0x1a) goto ca5d1;
    //     cmp #0x20 ; ' '
    //     bcs ca5d1
    if (a >= 0x20) goto ca5d1;
    //     sty l0084
    l0084 = y;
    //     ldy print_flags
    y = print_flags;
    //     bpl ca5cf
    if (!(y & 0x80)) goto ca5cf;
    //     sbc #0x1b
    a = a - 0x1b - (1 - (flags & FLAG_C));
    if ((uint8_t)(a + 0x1b + (1 - (flags & FLAG_C))) >= a) flags |= FLAG_C; else flags &= ~FLAG_C;
    //     tax
    x = a;
    //     lda highlight1_code,x
    a = highlight_code[x];
ca5cf:
    //     ldy l0084
    y = l0084;
ca5d1:
    //     ldx #1
    x = 1;
    //     clc
    flags &= ~FLAG_C;
    //     rts
    return;

ca5d5:
    //     lda #0x20 ; ' '
    a = 0x20;
    //     bne ca5d1
    goto ca5d1;

ca5d9:
    //     lda ruler_left_stop
    a = ruler_left_stop;
    //     beq ca5d5
    if (a == 0) goto ca5d5;
    //     sty l0084
    l0084 = y;
    //     bne ca5f1
    goto ca5f1;

ca5e1:
    //     sty l0084
    l0084 = y;
    //     ldy l0039
    y = l0039;
loop_ca5e5:
    //     iny
    y++;
    //     cpy l003a
    //     bcs ca5f8
    if (y >= l003a) goto ca5f8;
    //     lda (current_ruler_ptr),y
    a = ram[current_ruler_ptr + y];
    //     cmp #0x2a ; '*'
    //     bne loop_ca5e5
    if (a != 0x2a) goto loop_ca5e5;
    //     tya
    a = y;
ca5f1:
    //     sbc l0039
    {
        uint16_t r = (uint16_t)a - (uint16_t)l0039 - (1 - (flags & FLAG_C));
        a = (uint8_t)r;
        if (r < 0x100) flags |= FLAG_C; else flags &= ~FLAG_C;
    }
    //     tax
    x = a;
    //     beq ca5f8
    if (x == 0) goto ca5f8;
    //     bcs ca5fa
    if (flags & FLAG_C) goto ca5fa;
ca5f8:
    //     ldx #1
    x = 1;
ca5fa:
    //     lda #0x20 ; ' '
    a = 0x20;
    //     ldy l0084
    y = l0084;
    //     sec
    flags |= FLAG_C;
    //     rts
}
static void draw_char(void) {
    // draw_char:
    //     lda (tmp0),y
    a = ram[((uint16_t)tmp1 << 8 | tmp0) + y];
    //     iny
    y++;
    process_document_character();
}
static void check_for_control_code(void) {
    // Pseudocode: Checks if character is a control code (0x1c or 0x1d)

    // check_for_control_code:
    //     cmp #0x1c
    { uint16_t tmp_ = a - 0x1c; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x1c ? FLAG_C : 0); }
    //     beq return_63
    if (flags & FLAG_Z) goto return_63;
    //     cmp #0x1d
    { uint16_t tmp_ = a - 0x1d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x1d ? FLAG_C : 0); }
    //     clc
    flags &= ~FLAG_C;
    // return_63:
return_63:
    //     rts
    return;
}
static void recalculate_cursor_xpos(void) {
    // Pseudocode: Recalculates cursor xpos from visual position accounting for tabs and margins

    // sub_ca608:
    //     lda current_edit_line_ptr
    a = (uint8_t)(current_edit_line_ptr & 0xff);
    //     sta tmp0
    tmp0 = a;
    //     lda current_edit_line_ptr+1
    a = (uint8_t)(current_edit_line_ptr >> 8);
    //     sta tmp1
    tmp1 = a;
    //     lda l0079
    a = l0079;
    //     bne ca624
    if (!(flags & FLAG_Z)) goto ca624;
    //     tay
    y = a;
    // loop_ca615:
loop_ca615:
    //     cpy xpos
    { uint16_t tmp_ = y - xpos; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= xpos ? FLAG_C : 0); }
    //     beq ca63d
    if (flags & FLAG_Z) goto ca63d;
    //     sta l0039
    l0039 = a;
    //     jsr draw_char
    draw_char();
    //     txa
    a = x;
    //     clc
    flags &= ~FLAG_C;
    //     adc l0039
    { uint16_t tmp_ = (uint16_t)a + l0039; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C|FLAG_V)) | ((tmp_ & 0xff) == 0 ? FLAG_Z : 0) | ((tmp_ & 0x80) ? FLAG_N : 0) | (tmp_ > 255 ? FLAG_C : 0) | (((~(a ^ l0039) & (a ^ (uint8_t)tmp_)) >> 1) & FLAG_V); a = (uint8_t)tmp_; }
    //     bcc loop_ca615
    if (!(flags & FLAG_C)) goto loop_ca615;
    // ca624:
ca624:
    //     lda #0
    a = 0;
    //     sta l0079
    l0079 = a;
    //     tay                                                               ; Y=0x00
    y = a;
    // loop_ca629:
loop_ca629:
    //     sta l0039
    l0039 = a;
    //     jsr draw_char
    draw_char();
    //     txa
    a = x;
    //     clc
    flags &= ~FLAG_C;
    //     adc l0039
    { uint16_t tmp_ = (uint16_t)a + l0039; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C|FLAG_V)) | ((tmp_ & 0xff) == 0 ? FLAG_Z : 0) | ((tmp_ & 0x80) ? FLAG_N : 0) | (tmp_ > 255 ? FLAG_C : 0) | (((~(a ^ l0039) & (a ^ (uint8_t)tmp_)) >> 1) & FLAG_V); a = (uint8_t)tmp_; }
    //     cmp l0072
    { uint16_t tmp_ = a - l0072; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= l0072 ? FLAG_C : 0); }
    //     bcc loop_ca629
    if (!(flags & FLAG_C)) goto loop_ca629;
    //     beq ca63b
    if (flags & FLAG_Z) goto ca63b;
    //     lda l0039
    a = l0039;
    //     dey
    y--;
    // ca63b:
ca63b:
    //     sty xpos
    xpos = y;
    // ca63d:
ca63d:
    //     sta l0072
    l0072 = a;
    // return_64:
return_64:
    //     rts
    return;
}
static void draw_ruler(void) {
    // Pseudocode: Displays ruler status word at top of screen if status_line_needs_redrawing_flag is set

    // ; ***************************************************************************************
    // draw_ruler:
    //     lda status_line_needs_redrawing_flag
    a = status_line_needs_redrawing_flag;
    flags = (flags & ~(FLAG_Z | FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq return_64
    if (flags & FLAG_Z) return;
    //     ldy #0
    y = 0;
    //     sty status_line_needs_redrawing_flag
    status_line_needs_redrawing_flag = y;

    //     sty l0082
    l0082 = y;
    //     lda current_ruler_ptr
    a = (uint8_t)(current_ruler_ptr & 0xff);
    //     ldy current_ruler_ptr+1
    y = (uint8_t)(current_ruler_ptr >> 8);
    draw_line();
    //     rts
    return;
}
static void home_cursor(void) {
    // home_cursor:
    // ca681:
    //     ldx #0
    x = 0;
    //     ldy #0
    y = 0;
    //     jmp set_cursor_position
    set_cursor_position(); return;
}
static void draw_status_word(void) {
    // Pseudocode: Redraws status line showing format mode, justify, and insert indicators

    // sub_ca651:
    //     lda #0
    a = 0;
    //     sta flags_need_redrawing_flag
    flags_need_redrawing_flag = a;
    //     jsr home_cursor
    home_cursor();
    //     ldx #0x46 ; 'F'
    x = 0x46;
    //     lda format_mode_flag
    a = format_mode_flag;
    flags = (flags & ~(FLAG_Z | FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq ca666
    if (flags & FLAG_Z) goto ca666;
    //     ldx #0x4d ; 'M'
    x = 0x4d;
    //     and #0xc0
    a &= 0xc0;
    flags = (flags & ~(FLAG_Z | FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bne ca666
    if (!(flags & FLAG_Z)) goto ca666;
    //     ldx #0x20 ; ' '
    x = 0x20;
    // ca666:
ca666:
    //     txa
    a = x;
    //     jsr screen_putchar
    screen_putchar(a);
    //     lda #0x4a ; 'J'
    a = 0x4a;
    //     ldx justifying_flag
    x = justifying_flag;
    flags = (flags & ~(FLAG_Z | FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     beq ca672
    if (flags & FLAG_Z) goto ca672;
    //     lda #0x20 ; ' '
    a = 0x20;
    // ca672:
ca672:
    //     jsr screen_putchar
    screen_putchar(a);
    //     lda #0x49 ; 'I'
    a = 0x49;
    //     ldx insert_mode_flag
    x = insert_mode_flag;
    flags = (flags & ~(FLAG_Z | FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     bne ca681
    if (!(flags & FLAG_Z)) { home_cursor(); return; }
    //     lda #0x20 ; ' '
    a = 0x20;
    //     bne ca681                                                         ; ALWAYS branch
    home_cursor(); return;
}
static void ca684(void) {
    // Pseudocode: Sets line_lengths[ypos] = screen_width after cursor movement

    // ca684:
    //     ldx ypos
    x = ypos;
    //     lda screen_width
    a = screen_width;
    //     sta line_lengths,x
    line_lengths[x] = a;
    //     rts
    return;
}
static void (*number_callback)(void);

static void render_number_to_output_buffer(void) {
    // Pseudocode: Renders a 16-bit number to the output buffer using callback

    // ; ***************************************************************************************
    // ; On Entry:
    // ;     TMP9/TMP8: 16-bit number
    // ; ***************************************************************************************
    // render_number_to_output_buffer:
    //     stx l0082
    l0082 = x;
    //     lda la69a
    // PROBLEM: la69a function pointer data not defined
    //     ldy la69b
    // PROBLEM: la69b function pointer data not defined
    //     jsr render_number_to_callback
    number_callback = emit_to_output_buffer_callback;
    render_number_to_callback();
    //     ldx l0082
    x = l0082;
    //     rts
    return;
}
static void emit_to_output_buffer_callback(void) {
    // Pseudocode: Callback that writes a digit character to the output buffer

    // la69a:
    // la69b = la69a+1
    //     .word emit_to_output_buffer_callback

    // emit_to_output_buffer_callback:
    //     pha
{   uint8_t saved_a = a;
    //     txa
    a = x;
    //     pha
    // PROBLEM: pha (stack save)
    //     tsx
    x = sp;
    //     lda 0x0102,x
    a = ram[(uint16_t)0x0102 + x];
    //     ldx l0082
    x = l0082;
    //     sta output_buffer,x
    output_buffer[x] = a;
    //     cpx #MAX_LINE_LENGTH-2
    { uint16_t tmp_ = x - (MAX_LINE_LENGTH - 2); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= MAX_LINE_LENGTH - 2 ? FLAG_C : 0); }
    //     bcs ca6ae
    if (flags & FLAG_C) goto ca6ae;
    //     inc l0082
    l0082++;
    // ca6ae:
ca6ae:
    //     pla
    // PROBLEM: pla (stack restore)
    //     tax
    x = a;
    //     pla
    a = saved_a; }
    //     rts
    return;
}
static void print_char_via_putchar(void) {
    cli_putchar(a);
}
static void render_number_to_screen(void) {
    // Pseudocode: Renders a 16-bit number to screen via bdos_print_char

    // ; ***************************************************************************************
    // ; On Entry:
    // ;     YX: 16-bit number
    // ; ***************************************************************************************
    // render_number_to_screen:
    //     stx tmp8
    tmp8 = x;
    //     sty tmp9
    tmp9 = y;
    //     lda #<(bdos_print_char)
    a = (uint8_t)((uintptr_t)&print_char_via_putchar & 0xff);
    //     ldy #>(bdos_print_char)
    y = (uint8_t)((uintptr_t)&print_char_via_putchar >> 8);
    number_callback = print_char_via_putchar;
    // Fall through to render_number_to_callback in original 6502
    render_number_to_callback();
}
static void render_number_to_callback(void) {
    // Pseudocode: Render 16-bit number (TMP9:TMP8) as decimal via callback

    tmp6 = a;
    tmp7 = y;
    uint16_t value = (uint16_t)tmp8 | ((uint16_t)tmp9 << 8);
    char buf[6];
    snprintf(buf, sizeof(buf), "%u", (unsigned int)value);
    for (char *p = buf; *p; p++) {
        a = *p - '0';
        a |= 0x30;
        number_callback();
    }
}
static void parse_decimal_number(void) {
    // ca6fe - Parse decimal number from format command line
    // On entry: y = index into current_format_line_ptr
    // On exit:  tmp8:tmp9 = parsed value, a/x = value, y = advanced past digits
    //           flags.Z = 1 if no digits parsed

    uint8_t had_digits = 0;
    tmp8 = 0;
    tmp9 = 0;

    for (;;) {
        a = ram[(uint16_t)current_format_line_ptr + y];
        if (a < '0' || a > '9') break;
        a -= '0';
        y++;
        had_digits = 0xff;

        uint16_t val = ((uint16_t)tmp9 << 8) | tmp8;
        val = val * 10 + a;
        tmp8 = (uint8_t)val;
        tmp9 = (uint8_t)(val >> 8);
    }

    a = tmp8;
    x = tmp9;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (had_digits == 0 ? FLAG_Z : 0) | (had_digits & FLAG_N);
}
static void ca741(void) {
    // ca741: Updates ptr6 to current_line_ptr if ptr6 is ahead, sets refresh flags
    // On entry: current_line_ptr, ptr6
    // On exit:  ptr6 = min(ptr6, current_line_ptr), l0073 = l003d = 0xff
    // Uses: x, y

    //     ldx current_line_ptr
    x = (uint8_t)(current_line_ptr & 0xff);
    //     ldy current_line_ptr+1
    y = (uint8_t)(current_line_ptr >> 8);
    //     cpy ptr6+1
    //     bcc ca74f
    //     bne ca753
    //     cpx ptr6
    //     bcs ca753
    if (y < (uint8_t)(ptr6 >> 8) || (y == (uint8_t)(ptr6 >> 8) && x < (uint8_t)(ptr6 & 0xff))) {
        // ca74f:
        //     stx ptr6
        //     sty ptr6+1
        ptr6 = current_line_ptr;
    }
    // ca753:
    //     ldx #0xff
    x = 0xff;
    //     stx l0073
    l0073 = x;
    //     stx l003d
    l003d = x;
    //     rts
}
static void flush_and_read_char(void) {
    // Pseudocode: Reads a character from keyboard via SCREEN, returning escape flag in carry
flush_and_read_char:
    // read_char:
read_char:
    //     lda #0xff
    a = 0xff;
    //     tax
    x = a;
    //     ldy #SCREEN_GETCHAR
    //     jsr SCREEN
    a = screen_getchar();
    //     bcs read_char
    if (flags & FLAG_C) goto read_char;
    //     cmp #0x1b                                                         ; A=character read
    { uint16_t tmp_ = a - 0x1b; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | (tmp_ & FLAG_N) | (a >= 0x1b ? FLAG_C : 0); }
    //     clc
    flags &= ~FLAG_C;
    //     bne return_65
    if (!(flags & FLAG_Z)) goto return_65;
    //     sec
    flags |= FLAG_C;
    // return_65:
return_65:
    //     rts
    return;
}
static void read_char(void) {
    flush_and_read_char(); // alias - same entry
}
static void clear_screen(void) {
    // Pseudocode: Clears the screen via SCREEN call

    // ; ***************************************************************************************
    // clear_screen:
    //     ldy #SCREEN_CLEAR
    //     jmp SCREEN
    screen_clear(); return;
}
static void draw_prompt_characters(void) {
    // draw_prompt_characters: Draws two inverted prompt characters at top-left
    // On entry: x, y = prompt characters
    // Uses: tmp2, tmp3
    // On exit: cursor position restored

    //     stx tmp2
    tmp2 = x;
    //     sty tmp3
    tmp3 = y;
    //     jsr save_cursor_position
    save_cursor_position();
    //     jsr cursor_off
    cursor_off();
    //     jsr home_cursor
    home_cursor();
    //     jsr set_inverted_text_if_not_mode_7
    a = STYLE_REVERSE; screen_setstyle(a);
    //     lda tmp2
    a = (uint8_t)tmp2;
    //     jsr screen_putchar
    screen_putchar(a);
    //     lda tmp3
    a = (uint8_t)tmp3;
    //     jsr screen_putchar
    screen_putchar(a);
    //     jsr set_normal_text_if_not_mode_7
    a = 0; screen_setstyle(a);
    //     lda #0x20 ; ' '
    a = 0x20;
    //     jsr screen_putchar
    screen_putchar(a);
    //     jsr restore_cursor_position
    restore_cursor_position();
    // cursor_on:
    // cursor_off:
    //     rts
}
static void cursor_on(void) {
    // Pseudocode: Enables cursor display via SCREEN driver

    // cursor_on:
    //     lda #1
    //     jmp SCREEN_SHOWCURSOR
    screen_enablecursor(1);
}
static void cursor_off(void) {
    // Pseudocode: Disables cursor display via SCREEN driver

    // cursor_off:
    //     lda #0
    //     jmp SCREEN_SHOWCURSOR
    screen_enablecursor(0);
}
static void save_cursor_position(void) {
    // Pseudocode: Saves current cursor position via SCREEN call

    // ; ***************************************************************************************
    // save_cursor_position:
    //     ldy #SCREEN_GETCURSOR
    //     jsr SCREEN
    uint16_t cursor_ = screen_getcursor();
    a = (uint8_t)(cursor_ & 0xff);
    x = (uint8_t)(cursor_ >> 8);
    //     sta tmp4
    tmp4 = a;
    //     stx tmp5
    tmp5 = x;
    //     rts
}
static void set_cursor_position(void) {
    // set_cursor_position:
    //     pha
    //     txa
    //     pha
    //     tya
    //     pha
    { uint8_t saved_a = a; uint8_t saved_x = x; uint8_t saved_y = y;

    //     txa
    a = saved_x;
    //     pha
    //     tya
    //     tax
    x = saved_y;
    //     pla
    a = saved_x;
    //     ldy #SCREEN_SETCURSOR
    //     jsr SCREEN
    screen_setcursor((uint16_t)x << 8 | a);

    //     pla
    //     tay
    y = saved_y;
    //     pla
    //     tax
    x = saved_x;
    //     pla
    a = saved_a; }
    // return_34:
    //     rts
}
static void restore_cursor_position(void) {
    // restore_cursor_position:
    //     ldx tmp4
    x = tmp4;
    //     ldy tmp5
    y = tmp5;
    set_cursor_position();
}
static const uint8_t la83d[] = "VIEW\0B3.0 for CP/M-65";

static void print_x_words_of_help(void) {
    // Pseudocode: Prints X words of the help string showing VIEW and version

    // ; ***************************************************************************************
    // print_x_words_of_help:
    //     ldy #0
    y = 0;
    //     beq ca832                                                         ; ALWAYS branch
    goto ca832;

    // ca82e:
ca82e:
    //     jsr bdos_print_char
    cli_putchar(a);
    //     iny
    y++;
    // ca832:
ca832:
    //     lda la83d,y
    a = la83d[y];
    //     bne ca82e
    if (a != 0) goto ca82e;
    //     lda #0x20 ; ' '
    a = 0x20;
    //     dex
    x--;
    //     bpl ca82e
    if ((int8_t)x >= 0) goto ca82e;
    //     rts
    return;
}
extern uint8_t parser_table[];
static void parse_command(void) {
    // Pseudocode: Parses command input against parser_table to identify command number

    // la83d:
    //     .ascii "VIEW"
    //     .byte 0
    //     .ascii "B3.0 for CP/M-65"
    //     .byte 0

    // ; ***************************************************************************************
    // parse_command:
    //     lda #0xff
    a = 0xff;
    //     sta l0082
    l0082 = a;
    //     tax                                                               ; X=0xff
    x = a;
    // ca84c:
ca84c:
    //     ldy input_buffer_offset
    y = input_buffer_offset;
    //     dey
    y--;
    //     inc l0082
    l0082++;
    // loop_ca851:
loop_ca851:
    //     inx
    x++;
    //     iny
    y++;
    //     lda (tmp0),y
    a = input_buffer[y];
    //     and #0xdf
    a &= 0xdf;
    //     sta l0084
    l0084 = a;
    //     lda parser_table,x
    a = parser_table[x];
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq ca890
    if (flags & FLAG_Z) goto ca890;
    //     bmi ca87e
    if (flags & FLAG_N) goto ca87e;
    //     eor #0x5b ; '['
    a ^= 0x5b;
    //     sta l0083
    l0083 = a;
    //     and #0xdf
    a &= 0xdf;
    //     cmp l0084
    { uint16_t tmp_ = a - l0084; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= l0084 ? FLAG_C : 0); }
    //     beq loop_ca851
    if (flags & FLAG_Z) goto loop_ca851;
    // loop_ca86a:
loop_ca86a:
    //     inx
    x++;
    //     lda parser_table,x
    a = parser_table[x];
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq ca890
    if (flags & FLAG_Z) goto ca890;
    //     bpl loop_ca86a
    if (!(flags & FLAG_N)) goto loop_ca86a;
    //     lda l0083
    a = l0083;
    //     and #0x20 ; ' '
    a &= 0x20;
    flags = (flags & ~FLAG_Z) | (a == 0 ? FLAG_Z : 0);
    //     beq ca84c
    if (flags & FLAG_Z) goto ca84c;
    //     lda (tmp0),y
    a = input_buffer[y];
    //     cmp #0x30 ; '0'
    { uint16_t tmp_ = a - 0x30; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x30 ? FLAG_C : 0); }
    //     bcs ca84c
    if (flags & FLAG_C) goto ca84c;
    // ca87e:
ca87e:
    //     lda (tmp0),y
    a = input_buffer[y];
    //     cmp #0x30 ; '0'
    { uint16_t tmp_ = a - 0x30; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x30 ? FLAG_C : 0); }
    //     bcs ca887
    if (flags & FLAG_C) goto ca887;
    //     sta l007e
    l007e = a;
    //     iny
    y++;
    // ca887:
ca887:
    //     sty input_buffer_offset
    input_buffer_offset = y;
    //     ldy l0082
    y = l0082;
    //     lda parser_table,x
    a = parser_table[x];
    //     clc
    flags &= ~FLAG_C;
    //     rts
    return;

    // ca890:
ca890:
    //     sec
    flags |= FLAG_C;
    //     rts
    return;
}
static void call_through_jumptable(void) {
    // call_through_jumptable:
    //     asl
    //     clc
    //     adc jumptable_ptrs,y
    //     sta tmp8
    //     lda #0
    //     adc jumptable_ptrs+1,y
    //     sta tmp9
    //     ldy #0
    //     lda (tmp8),y
    //     sta tmp6
    //     iny
    //     lda (tmp8),y
    //     sta tmp7
    //     jmp (tmp6)

    if (y == 0) {
        switch (a) {
            case 0: ce_fmt_cmd(); break;
            case 1: rj_fmt_cmd(); break;
            case 2: df_fmt_cmd(); break;
            case 3: dh_fmt_cmd(); break;
            case 4: dm_fmt_cmd(); break;
            case 5: return;  // return_34
            case 6: em_fmt_cmd(); break;
            case 7: pe_fmt_cmd(); break;
            case 8: tm_fmt_cmd(); break;
            case 9: bm_fmt_cmd(); break;
            case 10: pl_fmt_cmd(); break;
            case 11: ts_fmt_cmd(); break;
            case 12: fo_fmt_cmd(); break;
            case 13: he_fmt_cmd(); break;
            case 14: ht_fmt_cmd(); break;
            case 15: hm_fmt_cmd(); break;
            case 16: fm_fmt_cmd(); break;
            case 17: lm_fmt_cmd(); break;
            case 18: ls_fmt_cmd(); break;
            case 19: op_fmt_cmd(); break;
            case 20: ep_fmt_cmd(); break;
            case 21: lj_fmt_cmd(); break;
            case 22: pb_fmt_cmd(); break;
        }
    } else {
        switch (a) {
            case 0: quit_cmd(); break;
            case 1: new_cmd(); break;
            case 2: format_cmd(); break;
            case 3: setup_cmd(); break;
            case 4: read_cmd(); break;
            case 5: more_cmd(); break;
            case 6: screen_cmd(); break;
            case 7: sheets_cmd(); break;
            case 8: save_cmd_write_cmd(); break;
            case 9: count_cmd(); break;
            case 10: field_cmd(); break;
            case 11: printer_cmd(); break;
            case 12: search_cmd(); break;
            case 13: clear_cmd(); break;
            case 14: microspace_cmd(); break;
            case 15: fold_cmd(); break;
            case 16: name_cmd(); break;
            case 17: mode_cmd(); break;
            case 18: finish_cmd(); break;
            case 19: print_cmd(); break;
            case 20: change_cmd(); break;
            case 21: save_cmd_write_cmd(); break;
            case 22: edit_cmd(); break;
            case 23: replace_cmd(); break;
            case 24: load_cmd(); break;
            case 25: bye_cmd(); break;
        }
    }
}
static void look_up_address_in_table(void) {
    // Pseudocode: Looks up a key in an address table and returns the associated handler address

    // ; On entry: YX is the address of the table, A is the value
    // ; On exit, tmp6 is the routine; C if error, !C if success
    // zproc look_up_address_in_table
    //     stx tmp8
    tmp8 = x;
    //     sty tmp9
    tmp9 = y;
    //     sta tmp6
    tmp6 = a;
    //     ldy #0
    y = 0;
    //     zloop
    do {
    //         lda (tmp8), y
        a = ram[(uint16_t)tmp8 + y];
    //         zbreakif eq
        if (flags & FLAG_Z) break;
    //         cmp tmp6
        { uint16_t tmp_ = a - tmp6; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= tmp6 ? FLAG_C : 0); }
    //         zif eq
        if (flags & FLAG_Z) {
    //             iny
            y++;
    //             lda (tmp8), y
            a = ram[(uint16_t)tmp8 + y];
    //             sta tmp6
            tmp6 = a;
    //             iny
            y++;
    //             lda (tmp8), y
            a = ram[(uint16_t)tmp8 + y];
    //             sta tmp7
            tmp7 = a;
    //             clc
            flags &= ~FLAG_C;
    //             rts
            return;
    //         zendif
        }
    //         iny
        y++;
    //         iny
        y++;
    //         iny
        y++;
    //     zendloop
    } while (1);
    //     sec
    flags |= FLAG_C;
    //     rts
    return;
    // zendproc
}
static void write_line_back_to_document(void) {
    // Pseudocode: Saves edit line changes back to document memory, updating markers

    // write_line_back_to_document:
    //     lda l006e
    //     beq ca93a
    if (l006e == 0) goto ca93a;
    //     lda current_line_ptr
    //     sta tmp4
    //     lda current_line_ptr+1
    //     sta tmp5
    tmp4 = (uint8_t)(current_line_ptr & 0xff);
    tmp5 = (uint8_t)(current_line_ptr >> 8);
    //     ldy #0
    //     sty tmp7
    y = 0;
    tmp7 = 0;
    //     jsr get_line_length
    get_line_length();
    //     sta l0083
    l0083 = a;
    //     lda l003b
    //     sec
    //     sbc l0083
    //     bcc ca8df
    //     beq ca8ed
    //     sta tmp6
    //     jsr adjust_pointers
    //     jmp ca8ed
    flags |= FLAG_C;
    { uint8_t old_a = l003b; a = l003b - l0083 - (1 - (flags & FLAG_C)); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (old_a >= l0083 ? FLAG_C : 0); }
    if (!(flags & FLAG_C)) goto ca8df;
    if (flags & FLAG_Z) goto ca8ed;
    tmp6 = a;
    adjust_pointers();
    goto ca8ed;

    // ca8df:
ca8df:
    //     sta l0084
    l0084 = a;
    //     lda #0
    //     sec
    //     sbc l0084
    //     sta tmp6
    //     jsr make_space_for_insertion
    //     bcs return_66
    flags |= FLAG_C;
    { uint8_t old_a = 0; a = 0 - l0084 - (1 - (flags & FLAG_C)); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (old_a >= l0084 ? FLAG_C : 0); }
    tmp6 = a;
    make_space_for_insertion();
    if (flags & FLAG_C) return;
    // ca8ed:
ca8ed:
    //     lda l006e
    //     bpl ca8f8
    //     lda l006d
    //     beq ca8f8
    //     jsr ca741
    if (!(l006e & 0x80)) goto ca8f8;
    if (l006d == 0) goto ca8f8;
    ca741();
    // ca8f8:
ca8f8:
    //     ldy #0
    //     sty l006d
    //     sty l006e
    y = 0;
    l006d = 0;
    l006e = 0;
    //     lda current_format_line_ptr
    //     sta tmp6
    //     lda current_format_line_ptr+1
    //     sta tmp7
    tmp6 = (uint8_t)(current_format_line_ptr & 0xff);
    tmp7 = (uint8_t)(current_format_line_ptr >> 8);
    //     ldx l0083
    //     stx l003b
    x = l0083;
    l003b = x;
    // ca90a:
ca90a:
    //     txa
    //     bne ca911
    //     lda #0x0d
    //     bne ca919
    if (x != 0) goto ca911;
    a = 0x0d;
    goto ca919;

    // ca911:
ca911:
    //     lda (current_format_line_ptr),y
    //     cmp #0x10
    //     bne ca919
    //     lda #0x20 ; ' '
    a = ram[current_format_line_ptr + y];
    if (a != 0x10) goto ca919;
    a = 0x20;
    // ca919:
ca919:
    //     pha
    //     txa
    //     pha
    { uint8_t saved_a = a;
      a = x;
    { uint8_t saved_x = a;

    // loop_ca91c:
loop_ca91c:
    //     jsr sub_ca536
    sub_ca536();
    //     bne ca92f
    if (a != 0) goto ca92f;
    //     tya
    a = y;
    //     clc
    flags &= ~FLAG_C;
    //     adc current_line_ptr
    { uint16_t tmp_ = (uint16_t)a + (uint16_t)(current_line_ptr & 0xff); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C|FLAG_V)) | ((tmp_ & 0xff) == 0 ? FLAG_Z : 0) | ((tmp_ & 0x80) ? FLAG_N : 0) | (tmp_ > 255 ? FLAG_C : 0) | (((~(a ^ (uint8_t)(current_line_ptr & 0xff)) & (a ^ (uint8_t)tmp_)) >> 1) & FLAG_V); a = (uint8_t)tmp_; }
    //     sta markers_array,x
    ((uint8_t*)markers_array)[x] = a;
    //     lda current_line_ptr+1
    a = (uint8_t)(current_line_ptr >> 8);
    //     adc #0
    { uint16_t tmp_ = (uint16_t)a + 0 + (flags & FLAG_C ? 1 : 0); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C|FLAG_V)) | ((tmp_ & 0xff) == 0 ? FLAG_Z : 0) | ((tmp_ & 0x80) ? FLAG_N : 0) | (tmp_ > 255 ? FLAG_C : 0) | (((~(a ^ 0) & (a ^ (uint8_t)tmp_)) >> 1) & FLAG_V); a = (uint8_t)tmp_; }
    //     sta markers_array+1,x
    ((uint8_t*)markers_array)[x+1] = a;
    //     bne loop_ca91c
    if (a != 0) goto loop_ca91c;
    // ca92f:
ca92f:
    //     pla
    //     tax
    a = saved_x; }
    x = a;
    //     pla
    a = saved_a; }
    //     sta (current_line_ptr),y
    ram[current_line_ptr + y] = a;
    //     iny
    y++;
    //     dex
    x--;
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     bne ca90a
    if (!(flags & FLAG_Z)) goto ca90a;
    // ca93a:
ca93a:
    //     clc
    flags &= ~FLAG_C;
    // return_66:
    //     rts
    return;
}
// MULTIPLE ENTRY POINTS: write_line_back_to_document_safely (via memory_full)
static void write_line_back_to_document_safely(void) {
    // write_line_back_to_document_safely: Write back edit buffer. If out of memory (C=1), fall through to memory_full.
    //     jsr write_line_back_to_document
    write_line_back_to_document();
    //     bcc return_66
    if (!(flags & FLAG_C)) return;
    //     falls through to memory_full
    memory_full();
}
static void memory_full(void) {
    // run_editor (ca941): Enter editor for memory-full condition.
    // jsr enter_editor_mode  -- NOT called here; entered via ca93c fall-through
    // ca941:
    //     ldx #0xff
    //     txs
    //     jsr sub_ca94a
    show_memory_full_error();
    //     jmp editor_loop
    longjmp(env, JMP_EDITOR);
}
// la995: "Memory full - Press ESCAPE"
static const uint8_t la995_data[] = "Memory full - Press ESCAPE";

static void show_memory_full_error(void) {
    // show_memory_full_error (sub_ca94a): Memory full error handler
    // On entry: (none)
    // On exit:  l006e=0, status_line_needs_redrawing_flag=1, l0073=1, cursor on
    // Uses: a, x, y, line_lengths

    //     jsr cursor_off
    cursor_off();
    //     ldx #3
    x = 3;
    //     ldy #0
    y = 0;
    //     jsr set_cursor_position
    set_cursor_position();
    //     jsr set_inverted_text_if_not_mode_7
    a = STYLE_REVERSE; screen_setstyle(a);
    //     ldy screen_width
    y = screen_width;
    //     sty line_lengths
    line_lengths[0] = y;
    //     dey
    y--;
    //     dey
    y--;
    //     ldx #0
    x = 0;
    //     beq ca965
    goto ca965;

    // loop_ca962:
loop_ca962:
    //     jsr screen_putchar
    screen_putchar(a);
    // ca965:
ca965:;
    //     lda la995,x
    a = la995_data[x];
    //     beq ca96e
    if (a == 0) goto ca96e;
    //     inx
    x++;
    //     dey
    y--;
    //     bne loop_ca962
    if (y != 0) goto loop_ca962;
    // ca96e:
ca96e:
    //     jsr set_normal_text_if_not_mode_7
    a = 0; screen_setstyle(a);
    //     tya
    a = y;
    //     beq ca97c
    if (a == 0) goto ca97c;
    //     lda #0x20 ; ' '
    a = 0x20;
    // loop_ca976:
loop_ca976:
    //     jsr screen_putchar
    screen_putchar(a);
    //     dey
    y--;
    //     bne loop_ca976
    if (y != 0) goto loop_ca976;
    // ca97c:
ca97c:
    //     lda #0
    a = 0;
    //     sta l006e
    l006e = a;
    //     jsr clear_cmd
    clear_cmd();
    // loop_ca983:
loop_ca983:
    //     jsr beep
    beep();
    //     jsr flush_and_read_char
    flush_and_read_char();
    //     bcc loop_ca983
    if (!(flags & FLAG_C)) goto loop_ca983;
    //     jsr cursor_on
    cursor_on();
    //     lda #1
    a = 1;
    //     sta status_line_needs_redrawing_flag
    status_line_needs_redrawing_flag = a;
    //     sta l0073
    l0073 = a;
    //     rts
}
static void adjust_pointers(void) {
    uint8_t tmp2, tmp3, tmp8, tmp9;
    // adjust_pointers:
    //     lda tmp4
    a = tmp4;
    //     sta tmp2
    tmp2 = a;
    //     clc
    flags &= ~FLAG_C;
    //     adc tmp6
    {   uint16_t sum = (uint16_t)a + tmp6;
        a = (uint8_t)sum;
        if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     sta tmp8
    tmp8 = a;
    //     lda tmp5
    a = tmp5;
    //     sta tmp3
    tmp3 = a;
    //     adc tmp7
    {   uint16_t sum = (uint16_t)a + tmp7 + (flags & FLAG_C);
        a = (uint8_t)sum; }
    //     sta tmp9
    tmp9 = a;

    //     ldx #0
    x = 0;
    // ca9c3:
    while (x < 22) {
        //     ldy __begin_pointer_array+1,x
        //     lda __begin_pointer_array+0,x
        uint16_t ptr = ((uint8_t*)&pointer_array)[x] | ((uint16_t)((uint8_t*)&pointer_array)[x + 1] << 8);
        uint16_t src = ((uint16_t)tmp5 << 8) | tmp4;
        uint16_t dst = ((uint16_t)tmp9 << 8) | tmp8;

        if (ptr >= src) {
            if (ptr >= dst) {
                goto ca9e7;
            }
            // ptr in [src, dst)
            if (x < 12) {
                // Zero marker pointing into moved block
                ((uint8_t*)&pointer_array)[x] = 0;
                ((uint8_t*)&pointer_array)[x + 1] = 0;
                goto ca9f1;
            }
ca9e7:
            ptr -= ((uint16_t)tmp7 << 8) | tmp6;
            ((uint8_t*)&pointer_array)[x] = ptr & 0xff;
            ((uint8_t*)&pointer_array)[x + 1] = (ptr >> 8) & 0xff;
        }
ca9f1:
        x += 2;
    }

    // loop_ca9f7:
    {
        uint16_t src_addr = ((uint16_t)tmp9 << 8) | tmp8;
        uint16_t dst_addr = ((uint16_t)tmp3 << 8) | tmp2;
        while (1) {
            //     ldy #0
            y = 0;
            // loop_ca9f9:
            while (1) {
                //     lda (tmp8),y
                a = ram[src_addr + y];
                //     sta (tmp2),y
                ram[dst_addr + y] = a;
                //     beq caa08
                if (a == 0) goto caa08;
                //     iny
                y++;
                //     bne loop_ca9f9
                if (y == 0) break;
            }
            //     inc tmp3
            //     inc tmp9
            dst_addr += 0x100;
            src_addr += 0x100;
        }
    }

caa08:
    //     tya
    a = y;
    //     clc
    //     adc tmp2
    {   uint16_t w = (uint16_t)a + tmp2;
        a = (uint8_t)w;
        if (w > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     sta top
    top = (top & 0xff00) | a;
    //     lda tmp3
    a = tmp3;
    //     adc #0
    {   uint16_t w = (uint16_t)a + (flags & FLAG_C);
        a = (uint8_t)w; }
    //     sta top+1
    top = (top & 0x00ff) | ((uint16_t)a << 8);
    //     rts
}
static void make_space_for_insertion(void) {
    // make_space_for_insertion: Shifts content up to make space for insertion
    // On entry: tmp4:tmp5 = block base, tmp6:tmp7 = size, top = current top
    // On exit:  top += size, pointer_array entries >= base adjusted, block shifted
    // Uses: tmp2, tmp3, tmp8, tmp9

    //     lda top
    a = (uint8_t)(top & 0xff);
    //     sta tmp2
    tmp2 = a;
    //     clc
    flags &= ~FLAG_C;
    //     adc tmp6
    { uint16_t sum = (uint16_t)a + tmp6; a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     sta tmp8
    tmp8 = a;
    //     tax
    x = a;
    //     lda top+1
    a = (uint8_t)(top >> 8);
    //     sta tmp3
    tmp3 = a;
    //     adc tmp7
    { uint16_t sum = (uint16_t)a + tmp7 + (flags & FLAG_C ? 1 : 0); a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     sta tmp9
    tmp9 = a;
    //     tay
    y = a;
    //     cpy himem+1
    //     bcc caa32
    //     bne return_67
    //     cpx himem
    //     bcs return_67
    if (y > (uint8_t)(himem >> 8) || (y == (uint8_t)(himem >> 8) && x >= (uint8_t)(himem & 0xff))) {
        return;
    }
    // caa32:
    //     stx top
    //     sty top+1
    top = (uint16_t)y << 8 | x;
    //     ldx #0
    x = 0;
    // loop_caa38:
loop_caa38:;
    //     ldy __begin_pointer_array+1,x
    y = ((uint8_t *)&pointer_array)[x + 1];
    //     lda __begin_pointer_array+0,x
    a = ((uint8_t *)&pointer_array)[x];
    //     cpy tmp5
    //     bcc caa51
    //     bne caa46
    //     cmp tmp4
    //     bcc caa51
    if (y >= tmp5 && (y > tmp5 || a >= tmp4)) {
        // caa46:
        //     clc
        flags &= ~FLAG_C;
        //     adc tmp6
        { uint16_t sum = (uint16_t)a + tmp6; a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
        //     sta __begin_pointer_array+0,x
        ((uint8_t *)&pointer_array)[x] = a;
        //     lda __begin_pointer_array+1,x
        a = ((uint8_t *)&pointer_array)[x + 1];
        //     adc tmp7
        { uint16_t sum = (uint16_t)a + tmp7 + (flags & FLAG_C ? 1 : 0); a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
        //     sta __begin_pointer_array+1,x
        ((uint8_t *)&pointer_array)[x + 1] = a;
    }
    // caa51:
    //     inx
    x++;
    //     inx
    x++;
    //     cpx #22
    //     bne loop_caa38
    if (x != 22) goto loop_caa38;

    // caa57:
caa57:
    //     lda tmp2
    a = tmp2;
    //     sec
    flags |= FLAG_C;
    //     sbc tmp4
    { uint16_t tmp_ = (uint16_t)a - tmp4; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= tmp4 ? FLAG_C : 0); a = (uint8_t)tmp_; }
    //     tax
    x = a;
    //     lda tmp3
    a = tmp3;
    //     sbc tmp5
    { uint16_t tmp_ = (uint16_t)a - tmp5 - (flags & FLAG_C ? 0 : 1); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= (tmp5 + (flags & FLAG_C ? 0 : 1)) ? FLAG_C : 0); a = (uint8_t)tmp_; }
    //     beq caa65
    if (flags & FLAG_Z) goto caa65;
    //     ldx #0xff
    x = 0xff;
    // caa65:
caa65:
    //     txa
    a = x;
    //     tay
    y = a;
    //     iny
    y++;
    //     lda tmp2
    a = tmp2;
    //     stx tmp2
    tmp2 = x;
    //     sec
    flags |= FLAG_C;
    //     sbc tmp2
    { uint16_t tmp_ = (uint16_t)a - tmp2; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= tmp2 ? FLAG_C : 0); a = (uint8_t)tmp_; }
    //     sta tmp2
    tmp2 = a;
    //     bcs caa75
    if (flags & FLAG_C) goto caa75;
    //     dec tmp3
    tmp3--;
    // caa75:
caa75:
    //     lda tmp8
    a = tmp8;
    //     stx tmp8
    tmp8 = x;
    //     sec
    flags |= FLAG_C;
    //     sbc tmp8
    { uint16_t tmp_ = (uint16_t)a - tmp8; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= tmp8 ? FLAG_C : 0); a = (uint8_t)tmp_; }
    //     sta tmp8
    tmp8 = a;
    //     bcs caa82
    if (flags & FLAG_C) goto caa82;
    //     dec tmp9
    tmp9--;
    // caa82:
caa82:
    //     dey
    y--;
    //     lda (tmp2),y
    a = ram[((uint16_t)tmp3 << 8 | tmp2) + y];
    //     sta (tmp8),y
    ram[((uint16_t)tmp9 << 8 | tmp8) + y] = a;
    //     tya
    a = y;
    //     bne caa82
    if (a != 0) goto caa82;
    //     inx
    x++;
    //     beq caa57
    if (x == 0) goto caa57;
    //     clc
    flags &= ~FLAG_C;
    // return_67:
    //     rts
}
static void sub_caa97(void) {
    // sub_caa97:
    //     lda #0x10
    a = 0x10;
    //     jsr wipe_buffer
    wipe_buffer();
    //     jsr sub_caf5f
    sub_caf5f();
    //     ldy #0
    y = 0;
    //     lda (current_line_ptr),y
    a = ram[current_line_ptr + y];
    //     ldx current_edit_line_ptr
    x = (uint8_t)(current_edit_line_ptr & 0xff);
    //     ldy current_edit_line_ptr+1
    y = (uint8_t)((current_edit_line_ptr >> 8) & 0xff);
    //     jsr check_for_command_prefix
    check_for_command_prefix();
    //     bne caab7
    if (flags & FLAG_Z) {
        // Z=1 (0x80 or 0x81)
        //     bcs caab0
        if (flags & FLAG_C) {
            // 0x80 case: goto caab0
        }
        //     sta l006e (0x81 case or bcc)
        if (!(flags & FLAG_C)) l006e = a;
        // caab0:
        //     jsr caf5c
        caf5c();
        //     ldx ptr1
        x = (uint8_t)(ptr1 & 0xff);
        //     ldy ptr1+1
        y = (uint8_t)((ptr1 >> 8) & 0xff);
    }
    // caab7:
    //     stx current_format_line_ptr
    current_format_line_ptr = (current_format_line_ptr & 0xff00) | x;
    //     sty current_format_line_ptr+1
    current_format_line_ptr = (current_format_line_ptr & 0x00ff) | ((uint16_t)y << 8);
    //     ldy #0
    y = 0;
    // loop_caabd:
    while (1) {
        //     lda (current_line_ptr),y
        a = ram[current_line_ptr + y];
        //     cmp #0x0d
        //     beq caac8
        if (a == 0x0d) break;
        //     sta (current_format_line_ptr),y
        ram[current_format_line_ptr + y] = a;
        //     iny
        y++;
        //     bne loop_caabd
        if (y == 0) break;
    }
    // caac8:
    //     sty l003b
    l003b = y;
    // return_68:
    //     rts
}
static void unpack_line_into_buffer(void) {
    // unpack_line_into_buffer:
    //     lda l006e
    //     bne return_68
    if (l006e != 0) return;
    //     lda #1
    //     sta l006e
    l006e = 1;
    sub_caa97();
}
static void sub_caacb(void) {
    // sub_caacb: Updates marker positions to point into format buffer instead of document buffer

    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     sta tmp6
    tmp6 = a;
    //     lda current_line_ptr+1
    a = (uint8_t)(current_line_ptr >> 8);
    //     sta tmp7
    tmp7 = a;
    //     ldy #0
    y = 0;
    // caad5:
caad5:
    //     jsr sub_ca536
    sub_ca536();
    //     bne caae8
    if (!(flags & FLAG_Z)) goto caae8;
    //     tya
    a = y;
    //     clc
    flags &= ~FLAG_C;
    //     adc current_format_line_ptr
    { uint16_t sum = (uint16_t)a + (uint8_t)(current_format_line_ptr & 0xff); a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     sta __begin_pointer_array,x
    ((uint8_t*)markers_array)[x] = a;
    //     lda current_format_line_ptr+1
    a = (uint8_t)(current_format_line_ptr >> 8);
    //     adc #0
    { uint16_t sum = (uint16_t)a + 0 + (flags & FLAG_C ? 1 : 0); a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     sta markers_array+1,x
    ((uint8_t*)markers_array)[x + 1] = a;
    //     bne caad5
    if (!(flags & FLAG_Z)) goto caad5;
    // caae8:
caae8:
    //     lda (current_line_ptr),y
    a = ram[current_line_ptr + y];
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     beq return_68
    if (flags & FLAG_Z) return;
    //     iny
    y++;
    //     bne caad5
    if (y != 0) goto caad5;
    // return_68:
    //     rts
}
static void get_line_length(void) {
    // Pseudocode: Returns the length of the current edit line

    // ; ***************************************************************************************
    // get_line_length:
    //     ldy #0
    y = 0;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    //     jsr check_for_command_prefix
    check_for_command_prefix();
    //     php
    { uint8_t saved_f = flags;
    //     ldy #0x84
    y = 0x84;
    // loop_caafb:
loop_caafb:
    //     dey
    y--;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (y == 0 ? FLAG_Z : 0) | (y & FLAG_N);
    //     lda (current_edit_line_ptr),y
    a = ram[current_edit_line_ptr + y];
    //     cmp #0x10
    { uint16_t tmp_ = a - 0x10; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x10 ? FLAG_C : 0); }
    //     bne cab06
    if (!(flags & FLAG_Z)) goto cab06;
    //     tya
    a = y;
    //     bne loop_caafb
    if (!(flags & FLAG_Z)) goto loop_caafb;
    //     dey
    y--;
    // cab06:
cab06:
    //     iny
    y++;
    //     tya
    a = y;
    //     plp
    flags = saved_f; }
    //     bne return_69
    if (!(flags & FLAG_Z)) goto return_69;
    //     clc
    flags &= ~FLAG_C;
    //     adc #3
    { uint16_t tmp_ = (uint16_t)a + 3; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C|FLAG_V)) | ((tmp_ & 0xff) == 0 ? FLAG_Z : 0) | ((tmp_ & 0x80) ? FLAG_N : 0) | (tmp_ > 255 ? FLAG_C : 0) | (((~(a ^ 3) & (a ^ (uint8_t)tmp_)) >> 1) & FLAG_V); a = (uint8_t)tmp_; }
    // return_69:
return_69:
    //     rts
    return;
}
static void wipe_buffer(void) {
    // wipe_buffer:
    //     ldy #0
    y = 0;
    //     ldx #0x89
    x = 0x89;
    // loop_cab13:
    //     sta (ptr1),y
    //     iny
    //     dex
    //     bne loop_cab13
    do {
        ram[ptr1 + y] = a;
        y++;
        x--;
    } while (x != 0);
    //     rts
}
static void sub_cab1a(void) {
    // Pseudocode: Finds next line in document, handling command prefix and ruler stack

    // sub_cab1a:
    //     sta tmp0
    //     sty tmp1
    tmp0 = a;
    tmp1 = y;
    //     jsr sub_cab6e
    sub_cab6e();
    //     bne cab29
    if (!(flags & FLAG_Z)) { cab29(); return; }
    //     jsr cab29
    cab29();
    //     bne push_onto_ruler_stack
    if (!(flags & FLAG_Z)) { push_onto_ruler_stack(); return; }
    //     rts
    return;
}
static void cab29(void) {
    // Pseudocode: Skips to next CR or zero terminator in memory

    // cab29:
    //     ldy #0
    y = 0;
    // loop_cab2b:
loop_cab2b:
    //     lda (tmp0),y
    a = ram[((uint16_t)tmp1 << 8 | tmp0) + y];
    flags = (flags & ~(FLAG_Z | FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq return_70
    if (flags & FLAG_Z) goto return_70;
    //     iny
    y++;
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     bne loop_cab2b
    if (!(flags & FLAG_Z)) goto loop_cab2b;
    //     lda (tmp0),y
    a = ram[((uint16_t)tmp1 << 8 | tmp0) + y];
    flags = (flags & ~(FLAG_Z | FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    // return_70:
return_70:
    //     rts
    return;
}
static void sub_cab37(void) {
    // Pseudocode: Finds previous line in document, handling ruler stack

    // sub_cab37:
    //     sec
    flags |= FLAG_C;
    //     sbc #1
    { uint8_t old_a = a; a = a - 1 - (1 - (flags & FLAG_C)); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (old_a >= 1 ? FLAG_C : 0); }
    //     sta tmp0
    tmp0 = a;
    //     bcs cab3f
    if (!(flags & FLAG_C)) y--;
    // cab3f:
cab3f:
    //     sty tmp1
    tmp1 = y;
    //     cpy page+1
    { uint16_t tmp_ = y - (uint8_t)(page >> 8); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= (uint8_t)(page >> 8) ? FLAG_C : 0); }
    //     bcc return_71
    if (!(flags & FLAG_C)) return;
    //     bne cab4b
    if (!(flags & FLAG_Z)) goto cab4b;
    //     cmp page
    { uint16_t tmp_ = a - (uint8_t)(page & 0xff); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= (uint8_t)(page & 0xff) ? FLAG_C : 0); }
    //     bcc return_71
    if (!(flags & FLAG_C)) return;
    // cab4b:
cab4b:
    //     ldy #0
    y = 0;
    // loop_cab4d:
loop_cab4d:
    //     lda tmp0
    //     sec
    //     sbc #1
    //     sta tmp0
    //     bcs cab58
    //     dec tmp1
    flags |= FLAG_C;
    { uint8_t old_a = tmp0; a = tmp0 - 1 - (1 - (flags & FLAG_C)); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (old_a >= 1 ? FLAG_C : 0); }
    tmp0 = a;
    if (!(flags & FLAG_C)) tmp1--;
    // cab58:
cab58:
    //     lda (tmp0),y
    a = ram[((uint16_t)tmp1 << 8 | tmp0) + y];
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     bne loop_cab4d
    if (!(flags & FLAG_Z)) goto loop_cab4d;
    //     inc tmp0
    tmp0++;
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (tmp0 == 0 ? FLAG_Z : 0) | (tmp0 & FLAG_N);
    //     bne cab64
    if (!(flags & FLAG_Z)) goto cab64;
    //     inc tmp1
    tmp1++;
    // cab64:
cab64:
    //     jsr sub_cab6e
    sub_cab6e();
    //     bne cab6c
    if (!(flags & FLAG_Z)) goto cab6c;
    //     jsr pop_from_ruler_stack
    pop_from_ruler_stack();
    // cab6c:
cab6c:
    //     sec
    flags |= FLAG_C;
    // return_71:
    //     rts
    return;
}
static void sub_cab6e(void) {
    // Pseudocode: Checks if byte at tmp0 is a command prefix (0x81)

    // sub_cab6e:
    //     ldy #0
    y = 0;
    //     lda (tmp0),y
    a = ram[((uint16_t)tmp1 << 8 | tmp0) + y];
    //     cmp #0x81
    { uint16_t tmp_ = a - 0x81; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x81 ? FLAG_C : 0); }
    //     rts
    return;
}
static void push_onto_ruler_stack(void) {
    // Pseudocode: Pushes current ruler position onto the ruler stack

    // push_onto_ruler_stack:
    //     tya
    //     pha
    { uint8_t saved_y = y;
    //     inc status_line_needs_redrawing_flag
    status_line_needs_redrawing_flag++;
    //     ldy ruler_stack_ptr
    //     dey
    //     lda tmp0
    //     sta (oshwm),y
    //     dey
    //     lda tmp1
    //     sta (oshwm),y
    y = ruler_stack_ptr;
    y--;
    y--;
    ruler_stack[y / 2] = ((uint16_t)tmp1 << 8) | tmp0;
    //     jsr cab91
    cab91();
    //     pla
    //     tay
    y = saved_y; }
    //     rts
    return;
}
static void pop_from_ruler_stack(void) {
    // Pseudocode: Pops ruler position from the ruler stack

    // pop_from_ruler_stack:
    //     inc status_line_needs_redrawing_flag
    status_line_needs_redrawing_flag++;
    //     ldy ruler_stack_ptr
    y = ruler_stack_ptr;
    //     iny
    y++;
    //     iny
    y++;

    // MULTIPLE ENTRY POINTS: pop_from_ruler_stack, cab91
    cab91();
}
static void cab91(void) {
    // Pseudocode: Sets current_ruler_ptr from stack at ruler_stack_ptr offset

    // cab91:
    //     sty ruler_stack_ptr
    ruler_stack_ptr = y;
    //     iny
    y++;
    //     lda (oshwm),y
    //     clc
    //     adc #3
    //     sta current_ruler_ptr
    flags &= ~FLAG_C;
    {   uint16_t val = ruler_stack[y / 2];
        a = val & 0xff;
        { uint16_t tmp_ = (uint16_t)a + 3; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C|FLAG_V)) | ((tmp_ & 0xff) == 0 ? FLAG_Z : 0) | ((tmp_ & 0x80) ? FLAG_N : 0) | (tmp_ > 255 ? FLAG_C : 0) | (((~(a ^ 3) & (a ^ (uint8_t)tmp_)) >> 1) & FLAG_V); a = (uint8_t)tmp_; }
        current_ruler_ptr = (current_ruler_ptr & 0xff00) | a;
        //     dey
        y--;
        a = (val >> 8) & 0xff;
        { uint16_t tmp_ = (uint16_t)a + 0 + (flags & FLAG_C ? 1 : 0); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C|FLAG_V)) | ((tmp_ & 0xff) == 0 ? FLAG_Z : 0) | ((tmp_ & 0x80) ? FLAG_N : 0) | (tmp_ > 255 ? FLAG_C : 0) | (((~(a ^ 0) & (a ^ (uint8_t)tmp_)) >> 1) & FLAG_V); a = (uint8_t)tmp_; }
        current_ruler_ptr = (current_ruler_ptr & 0x00ff) | ((uint16_t)a << 8);
    }

    // MULTIPLE ENTRY POINTS: pop_from_ruler_stack, cab91
}
static void find_margins_of_current_ruler_buffer(void) {
    // Pseudocode: Finds left (>) and right (<) margin stops in the current ruler buffer

    // ; ***************************************************************************************
    // find_margins_of_current_ruler_buffer:
    //     jsr sub_cabc4
    sub_cabc4();
    // loop_caba5:
loop_caba5:
    //     lda (current_ruler_ptr),y
    a = ram[current_ruler_ptr + y];
    //     cmp #0x3e ; '>'
    { uint16_t tmp_ = a - 0x3e; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x3e ? FLAG_C : 0); }
    //     bne cabad
    if (!(flags & FLAG_Z)) goto cabad;
    //     sty ruler_left_stop
    ruler_left_stop = y;
    // cabad:
cabad:
    //     cmp #0x3c ; '<'
    { uint16_t tmp_ = a - 0x3c; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x3c ? FLAG_C : 0); }
    //     bne cabb3
    if (!(flags & FLAG_Z)) goto cabb3;
    //     sty ruler_right_stop
    ruler_right_stop = y;
    // cabb3:
cabb3:
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     beq cabbc
    if (flags & FLAG_Z) goto cabbc;
    //     iny
    y++;
    //     cpy #0x84
    { uint16_t tmp_ = y - 0x84; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= 0x84 ? FLAG_C : 0); }
    //     bne loop_caba5
    if (!(flags & FLAG_Z)) goto loop_caba5;
    // cabbc:
cabbc:
    //     sty l003a
    l003a = y;
    //     lda ruler_left_stop
    //     cmp ruler_right_stop
    //     bcc return_72
    if (ruler_left_stop < ruler_right_stop) return;
    // fall through to re-zero margins
    ruler_right_stop = 0;
    ruler_left_stop = 0;
}
static void sub_cabc4(void) {
    // Pseudocode: Resets ruler_left_stop and ruler_right_stop to zero

    // sub_cabc4:
    //     ldy #0
    //     sty ruler_right_stop
    //     sty ruler_left_stop
    y = 0;
    ruler_right_stop = 0;
    ruler_left_stop = 0;
    // return_72:
    //     rts
    return;
}
static void move_cursor_to_address(void) {
    // Pseudocode: Moves cursor to a given memory address, handling ruler stack and setting xpos

    // move_cursor_to_address:
    //     sta tmp8
    //     sty tmp9
    tmp8 = a;
    tmp9 = y;
    //     lda current_line_ptr
    //     ldy current_line_ptr+1
    //     cpy tmp9
    //     bcc cabf9
    //     bne cabdf
    //     cmp tmp8
    //     bcc cabf9
    //     beq cac20
    a = (uint8_t)(current_line_ptr & 0xff);
    y = (uint8_t)(current_line_ptr >> 8);
    { uint16_t tmp_ = y - tmp9; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= tmp9 ? FLAG_C : 0); }
    if (!(flags & FLAG_C)) goto cabf9;
    if (!(flags & FLAG_Z)) goto cabdf;
    { uint16_t tmp_ = a - tmp8; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= tmp8 ? FLAG_C : 0); }
    if (!(flags & FLAG_C)) goto cabf9;
    if (flags & FLAG_Z) goto cac20;
    // cabdf:
cabdf:
    //     jsr sub_cab37
    sub_cab37();
    //     lda tmp0
    //     ldy tmp1
    //     bcc cac20
    if (!(flags & FLAG_C)) { a = tmp0; y = tmp1; goto cac20; }
    //     cpy tmp9
    { uint16_t tmp_ = tmp1 - tmp9; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (tmp1 >= tmp9 ? FLAG_C : 0); }
    //     bcc cac20
    if (!(flags & FLAG_C)) { a = tmp0; y = tmp1; goto cac20; }
    //     bne cabdf
    if (!(flags & FLAG_Z)) goto cabdf;
    //     cmp tmp8
    { uint16_t tmp_ = tmp0 - tmp8; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (tmp0 >= tmp8 ? FLAG_C : 0); }
    //     bcc cac20
    if (!(flags & FLAG_C)) { a = tmp0; y = tmp1; goto cac20; }
    //     bne cabdf
    if (!(flags & FLAG_Z)) goto cabdf;
    //     beq cac20
    { a = tmp0; y = tmp1; goto cac20; }

    // cabf6:
cabf6:
    //     jsr sub_cac41
    sub_cac41();
    // cabf9:
cabf9:
    //     sta tmp0
    //     sty tmp1
    tmp0 = a;
    tmp1 = y;
    //     jsr cab29
    cab29();
    //     beq cac17
    if (flags & FLAG_Z) goto cac17;
    //     tya
    a = y;
    //     ldy tmp1
    y = tmp1;
    //     clc
    flags &= ~FLAG_C;
    //     adc tmp0
    { uint16_t tmp_ = (uint16_t)a + tmp0; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C|FLAG_V)) | ((tmp_ & 0xff) == 0 ? FLAG_Z : 0) | ((tmp_ & 0x80) ? FLAG_N : 0) | (tmp_ > 255 ? FLAG_C : 0) | (((~(a ^ tmp0) & (a ^ (uint8_t)tmp_)) >> 1) & FLAG_V); a = (uint8_t)tmp_; }
    //     bcc cac0b
    if (!(flags & FLAG_C)) goto cac0b;
    //     iny
    y++;
    // cac0b:
cac0b:
    //     cpy tmp9
    { uint16_t tmp_ = y - tmp9; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= tmp9 ? FLAG_C : 0); }
    //     bcc cabf6
    if (!(flags & FLAG_C)) goto cabf6;
    //     bne cac17
    if (!(flags & FLAG_Z)) goto cac17;
    //     cmp tmp8
    { uint16_t tmp_ = a - tmp8; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= tmp8 ? FLAG_C : 0); }
    //     bcc cabf6
    if (!(flags & FLAG_C)) goto cabf6;
    //     beq cac1d
    if (flags & FLAG_Z) goto cac1d;
    // cac17:
cac17:
    //     lda tmp0
    //     ldy tmp1
    //     bne cac20
    a = tmp0;
    y = tmp1;
    goto cac20;
    // cac1d:
cac1d:
    //     jsr sub_cac41
    sub_cac41();
    // cac20:
cac20:
    //     sta current_line_ptr
    //     sty current_line_ptr+1
    current_line_ptr = ((uint16_t)y << 8) | a;
    //     lda tmp8
    //     sec
    //     sbc current_line_ptr
    //     tax
    flags |= FLAG_C;
    { uint8_t old_a = tmp8; a = tmp8 - (uint8_t)(current_line_ptr & 0xff) - (1 - (flags & FLAG_C)); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (old_a >= (uint8_t)(current_line_ptr & 0xff) ? FLAG_C : 0); }
    x = a;
    //     ldy #0
    y = 0;
    //     lda (current_line_ptr),y
    a = ram[current_line_ptr + y];
    //     jsr check_for_command_prefix
    check_for_command_prefix();
    //     bne cac3e
    if (!(flags & FLAG_Z)) goto cac3e;
    //     txa
    a = x;
    //     cpx #3
    { uint16_t tmp_ = x - 3; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (x >= 3 ? FLAG_C : 0); }
    //     ldx #0
    x = 0;
    //     bcc cac3e
    if (!(flags & FLAG_C)) goto cac3e;
    //     sec
    flags |= FLAG_C;
    //     sbc #3
    { uint8_t old_a = a; a = a - 3 - (1 - (flags & FLAG_C)); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (old_a >= 3 ? FLAG_C : 0); }
    //     tax
    x = a;
    // cac3e:
cac3e:
    //     stx xpos
    xpos = x;
    //     rts
    return;
}
static void sub_cac41(void) {
    // Pseudocode: Pushes ruler stack before entering a new ruler region

    // sub_cac41:
    //     pha
    //     tya
    //     pha
    { uint8_t saved_a = a; uint8_t saved_y = y;
    //     jsr sub_cab6e
    sub_cab6e();
    //     bne cac4c
    if (!(flags & FLAG_Z)) goto cac4c;
    //     jsr push_onto_ruler_stack
    push_onto_ruler_stack();
    // cac4c:
cac4c:
    //     pla
    //     tay
    //     pla
    y = saved_y;
    a = saved_a; }
    //     rts
    return;
}
static void sub_cac50(void) {
    // Pseudocode: Finds the start of current line by scanning backward for CR

    // sub_cac50:
    //     sec
    flags |= FLAG_C;
    //     sbc #1
    { uint8_t old_a = a; a = a - 1 - (1 - (flags & FLAG_C)); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (old_a >= 1 ? FLAG_C : 0); }
    //     sta tmp8
    tmp8 = a;
    //     bcs cac58
    if (!(flags & FLAG_C)) y--;
    // cac58:
cac58:
    //     sty tmp9
    tmp9 = y;
    //     ldy #0
    y = 0;
    // cac5c:
cac5c:
    //     lda (tmp8),y
    a = ram[((uint16_t)tmp9 << 8 | tmp8) + y];
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     beq cac6f
    if (flags & FLAG_Z) goto cac6f;
    //     lda tmp8
    //     sec
    //     sbc #1
    //     sta tmp8
    //     bcs cac5c
    //     dec tmp9
    //     bne cac5c
    flags |= FLAG_C;
    { uint8_t old_a = tmp8; a = tmp8 - 1 - (1 - (flags & FLAG_C)); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N) | (old_a >= 1 ? FLAG_C : 0); }
    tmp8 = a;
    if (flags & FLAG_C) goto cac5c;
    tmp9--;
    if (tmp9 != 0) goto cac5c;
    // cac6f:
cac6f:
    //     lda tmp8
    //     sta tmp6
    //     lda tmp9
    //     sta tmp7
    tmp6 = tmp8;
    tmp7 = tmp9;
    // return_73:
    //     rts
    return;
}
static void cac78(void) {
    // Pseudocode: Splits a line at the word wrap position, inserting CR for new line

    // cac78:
    //     jsr sub_cac50
    sub_cac50();
    // cac7b:
cac7b:
    //     lda #0
    //     sta l0083
    a = 0;
    l0083 = 0;
    //     ldx #0x85
    x = 0x85;
    //     ldy #1
    y = 1;
    //     lda (tmp8),y
    a = ram[((uint16_t)tmp9 << 8 | tmp8) + y];
    //     jsr check_for_command_prefix
    check_for_command_prefix();
    //     bne cac8d
    if (!(flags & FLAG_Z)) goto cac8d;
    //     inx
    //     inx
    //     inx
    x++;
    x++;
    x++;
    // cac8d:
cac8d:
    //     stx l0084
    l0084 = x;
    // cac8f:
cac8f:
    //     lda (tmp8),y
    a = ram[((uint16_t)tmp9 << 8 | tmp8) + y];
    //     iny
    y++;
    //     cmp #0x20 ; ' '
    { uint16_t tmp_ = a - 0x20; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x20 ? FLAG_C : 0); }
    //     beq cac9a
    if (flags & FLAG_Z) goto cac9a;
    //     cmp #0x1a
    { uint16_t tmp_ = a - 0x1a; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x1a ? FLAG_C : 0); }
    //     bne cac9c
    if (!(flags & FLAG_Z)) goto cac9c;
    // cac9a:
cac9a:
    //     sty l0083
    l0083 = y;
    // cac9c:
cac9c:
    //     cmp #0x0d
    { uint16_t tmp_ = a - 0x0d; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0d ? FLAG_C : 0); }
    //     beq return_73
    if (flags & FLAG_Z) return;
    //     cpy l0084
    { uint16_t tmp_ = y - l0084; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= l0084 ? FLAG_C : 0); }
    //     beq cac8f
    if (flags & FLAG_Z) goto cac8f;
    //     bcc cac8f
    if (!(flags & FLAG_C)) goto cac8f;
    //     lda l0084
    //     ldx l0083
    //     beq cacad
    //     txa
    if (l0083 == 0) { a = l0084; goto cacad; }
    a = l0083;
    // cacad:
cacad:
    //     clc
    flags &= ~FLAG_C;
    //     adc tmp8
    { uint16_t tmp_ = (uint16_t)a + tmp8; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C|FLAG_V)) | ((tmp_ & 0xff) == 0 ? FLAG_Z : 0) | ((tmp_ & 0x80) ? FLAG_N : 0) | (tmp_ > 255 ? FLAG_C : 0) | (((~(a ^ tmp8) & (a ^ (uint8_t)tmp_)) >> 1) & FLAG_V); a = (uint8_t)tmp_; }
    //     sta tmp4
    //     sta tmp8
    tmp4 = a;
    tmp8 = a;
    //     lda tmp9
    //     adc #0
    //     sta tmp5
    //     sta tmp9
    a = tmp9;
    { uint16_t tmp_ = (uint16_t)a + 0 + (flags & FLAG_C ? 1 : 0); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C|FLAG_V)) | ((tmp_ & 0xff) == 0 ? FLAG_Z : 0) | ((tmp_ & 0x80) ? FLAG_N : 0) | (tmp_ > 255 ? FLAG_C : 0) | (((~(a ^ 0) & (a ^ (uint8_t)tmp_)) >> 1) & FLAG_V); a = (uint8_t)tmp_; }
    tmp5 = a;
    tmp9 = a;
    //     lda #1
    //     sta tmp6
    //     lda #0
    //     sta tmp7
    tmp6 = 1;
    tmp7 = 0;
    //     jsr make_space_for_insertion
    make_space_for_insertion();
    //     lda #0x0d
    a = 0x0d;
    //     ldy #0
    y = 0;
    //     sta (tmp4),y
    ram[((uint16_t)tmp5 << 8 | tmp4) + y] = a;
    //     lda tmp4
    //     sta tmp8
    //     lda tmp5
    //     sta tmp9
    tmp8 = tmp4;
    tmp9 = tmp5;
    //     bne cac7b
    if (tmp9 != 0) goto cac7b;
    return;
}
static void prompt_for_marker(void) {
    // Pseudocode: Prompts for a marker character and looks it up

    // prompt_for_marker:
    //     ldx #0x4d ; 'M'
    //     ldy #0x4b ; 'K'
    x = 0x4d;
    y = 0x4b;
    //     jsr draw_prompt_characters
    draw_prompt_characters();
    //     inc flags_need_redrawing_flag
    flags_need_redrawing_flag++;
    //     jsr read_char
    read_char();
    //     jsr lookup_marker
    lookup_marker();
    //     bcc return_74
    if (!(flags & FLAG_C)) return;
    // ; ***************************************************************************************
    // beep:
    // loop_caced:
    //     sec
    // return_74:
    //     rts
    flags |= FLAG_C;
    return;
}
static void beep(void) {
    // beep: Emits a beep (returns with carry set as flag)

    //     lda #7
    a = 7;
    //     jsr oswrch
    cli_putchar(a);
    // loop_caced:
    //     sec
    flags |= FLAG_C;
    // return_74:
    //     rts
}
static void lookup_marker(void) {
    // lookup_marker: Converts marker character '1'-'6' to index

    //     sec
    flags |= FLAG_C;
    //     sbc #0x31 ; '1'
    a = a - 0x31;
    //     bcc loop_caced
    if (!(flags & FLAG_C)) { beep(); return; }
    //     asl
    a <<= 1;
    //     cmp #0x0c
    { uint16_t tmp_ = a - 0x0c; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0c ? FLAG_C : 0); }
    //     bcs return_75
    if (flags & FLAG_C) { /* return_75: */ return; }
    //     tax
    x = a;
    //     lda markers_array+1,x
    a = ((uint8_t*)markers_array)[x + 1];
    // return_75:
    //     rts
}
static void reset_area_to_entire_document(void) {
    // reset_area_to_entire_document: Resets area to entire document (top to page)

    //     lda top
    a = (uint8_t)(top & 0xff);
    //     sta area_start_ptr
    area_start_ptr = top;
    //     lda top+1
    a = (uint8_t)(top >> 8);
    //     sta area_start_ptr+1
    //     lda page
    a = (uint8_t)(page & 0xff);
    //     sta area_end_ptr
    area_end_ptr = page;
    //     lda page+1
    a = (uint8_t)(page >> 8);
    //     sta area_end_ptr+1
    //     rts
}
static void clear_marks_1_2(void) {
    // clear_marks_1_2: Clears markers 1 and 2 (resets to zero)

    //     lda #0
    a = 0;
    //     ldx #3
    x = 3;
    // loop_cad12:
loop_cad12:
    //     sta __begin_pointer_array,x
    ((uint8_t*)markers_array)[x] = a;
    //     dex
    x--;
    //     bpl loop_cad12
    if (!(x & 0x80)) goto loop_cad12;
    //     rts
}
static void reset_area_to_marks_1_2(void) {
    // reset_area_to_marks_1_2: Sets area to markers 1 and 2, then adjusts doc_ptr1

    //     lda #0x31 ; '1'
    a = 0x31;
    //     jsr lookup_marker
    lookup_marker();
    //     bcs return_76
    if (flags & FLAG_C) return;
    //     beq cad45
    if (flags & FLAG_Z) goto cad45;
    //     lda __begin_pointer_array,x
    a = ((uint8_t*)markers_array)[x];
    //     sta area_start_ptr
    area_start_ptr = (uint16_t)((uint8_t*)markers_array)[x + 1] << 8 | a;
    //     lda markers_array+1,x
    //     sta area_start_ptr+1
    //     lda #0x32 ; '2'
    a = 0x32;
    //     jsr lookup_marker
    lookup_marker();
    //     bcs return_76
    if (flags & FLAG_C) return;
    //     beq cad45
    if (flags & FLAG_Z) goto cad45;
    //     lda __begin_pointer_array,x
    a = ((uint8_t*)markers_array)[x];
    //     sta area_end_ptr
    area_end_ptr = (uint16_t)((uint8_t*)markers_array)[x + 1] << 8 | a;
    //     lda markers_array+1,x
    //     sta area_end_ptr+1
    //     ldx #doc_ptr1
    x = ((uint8_t*)&doc_ptr1 - (uint8_t*)markers_array);
    //     jsr set_marker_to_here
    set_marker_to_here();
    //     jsr sanitise_area
    sanitise_area();
    //     clc
    flags &= ~FLAG_C;
    //     bne return_76
    if (!(flags & FLAG_Z)) return;
    // cad45:
cad45:
    //     sec
    flags |= FLAG_C;
    // return_76:
    //     rts
}
static void set_marker_to_here(void) {
    // set_marker_to_here: Sets marker at current cursor position

    //     jsr get_line_length
    get_line_length();
    //     cpy xpos
    { uint16_t tmp_ = y - xpos; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= xpos ? FLAG_C : 0); }
    //     bcc cad5d
    if (!(flags & FLAG_C)) goto cad5d;
    //     ldy #0
    y = 0;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    //     ldy xpos
    y = xpos;
    //     jsr check_for_command_prefix
    check_for_command_prefix();
    //     bne cad5c
    if (!(flags & FLAG_Z)) goto cad5c;
    //     iny
    y++;
    //     iny
    y++;
    //     iny
    y++;
    // cad5c:
cad5c:
    //     tya
    a = y;
    // cad5d:
cad5d:
    //     clc
    flags &= ~FLAG_C;
    //     adc current_line_ptr
    { uint16_t sum = (uint16_t)a + (uint8_t)(current_line_ptr & 0xff); a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     sta 0,x
    ((uint8_t*)markers_array)[x] = a;
    //     lda current_line_ptr+1
    a = (uint8_t)(current_line_ptr >> 8);
    //     adc #0
    { uint16_t sum = (uint16_t)a + 0 + (flags & FLAG_C ? 1 : 0); a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     sta 1,x
    ((uint8_t*)markers_array)[x + 1] = a;
    //     rts
}
static void get_register_address(void) {
    // get_register_address: Gets address of a register value by letter name

    //     jsr is_uppercase
    is_uppercase();
    //     bcs return_77
    if (flags & FLAG_C) { /* return_77: */ return; }
    //     and #0xdf
    a &= 0xdf;
    //     pha
    { uint8_t saved_a = a;
    //     sbc #0x40 ; '@'
    { uint16_t tmp_ = (uint16_t)a - 0x40 - (1 - (flags & FLAG_C ? 1 : 0)); a = (uint8_t)tmp_; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (tmp_ < 0x10000 ? FLAG_C : 0); }
    //     asl
    a <<= 1;
    //     adc #<register_value_array
    { uint16_t sum = (uint16_t)a + (uint8_t)((uintptr_t)register_value_array & 0xff); a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     sta tmp6
    tmp6 = a;
    //     lda #>register_value_array
    a = (uint8_t)((uintptr_t)register_value_array >> 8);
    //     adc #0
    { uint16_t sum = (uint16_t)a + 0 + (flags & FLAG_C ? 1 : 0); a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     sta tmp7
    tmp7 = a;
    //     pla
    a = saved_a; }
    //     clc
    flags &= ~FLAG_C;
    // return_77:
    //     rts
}
static void render_register(void) {
    // render_register: Renders the value of a named register to output buffer

    //     sty l0084
    l0084 = y;
    //     jsr get_register_address
    get_register_address();
    //     ldy #0
    y = 0;
    //     sty tmp8
    tmp8 = 0;
    //     sty tmp9
    tmp9 = 0;
    //     bcs cada2
    if (flags & FLAG_C) goto cada2;
    //     bit lada6
    { flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_V)) | ((a & 0x40) ? FLAG_V : 0); }
    //     cmp #0x44 ; 'D'
    { uint16_t tmp_ = a - 0x44; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x44 ? FLAG_C : 0); }
    //     beq cada3
    if (flags & FLAG_Z) goto cada3;
    //     cmp #0x54 ; 'T'
    { uint16_t tmp_ = a - 0x54; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x54 ? FLAG_C : 0); }
    //     beq cada3
    if (flags & FLAG_Z) goto cada3;
    //     lda (tmp6),y
    a = ram[((uint16_t)tmp7 << 8 | tmp6) + y];
    //     sta tmp8
    tmp8 = a;
    //     iny
    y++;
    //     lda (tmp6),y
    a = ram[((uint16_t)tmp7 << 8 | tmp6) + y];
    //     sta tmp9
    tmp9 = a;
    // cada2:
cada2:
    //     clv
    flags &= ~FLAG_V;
    // cada3:
cada3:
    //     ldy l0084
    y = l0084;
    //     rts
}
static void sub_cadf0(void) {
    // sub_cadf0: Performs 8-bit by 8-bit division for microspacing

    //     ldx #8
    x = 8;
    //     lda tmp9
    a = tmp9;
    // loop_cadf4:
loop_cadf4:
    //     asl tmp8
    { uint8_t old_c = flags & FLAG_C ? 1 : 0; flags = (flags & ~FLAG_C) | ((tmp8 & 0x80) ? FLAG_C : 0); tmp8 <<= 1; }
    //     rol
    { uint8_t old_c = flags & FLAG_C ? 1 : 0; flags = (flags & ~FLAG_C) | ((a & 0x80) ? FLAG_C : 0); a = (a << 1) | old_c; }
    //     cmp l0046
    { uint16_t tmp_ = a - l0046; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= l0046 ? FLAG_C : 0); }
    //     bcc cadff
    if (!(flags & FLAG_C)) goto cadff;
    //     sbc l0046
    { uint16_t tmp_ = (uint16_t)a - l0046 - (1 - (flags & FLAG_C ? 1 : 0)); a = (uint8_t)tmp_; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (tmp_ < 0x10000 ? FLAG_C : 0); }
    //     inc tmp8
    tmp8++;
    // cadff:
cadff:
    //     dex
    x--;
    //     bne loop_cadf4
    if (x != 0) goto loop_cadf4;
    //     rts
}
static void sub_cae03(void) {
    // cae03:
    //     jmp beep
    beep();
}
static void sub_cae06(void) {
    // sub_cae06: Inserts bytes at cursor position, shifting existing content right

    //     lda xpos
    a = xpos;
    //     cmp #MAX_LINE_LENGTH
    { uint16_t tmp_ = a - MAX_LINE_LENGTH; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= MAX_LINE_LENGTH ? FLAG_C : 0); }
    //     bcs cae03
    if (flags & FLAG_C) { sub_cae03(); return; }
    //     stx input_buffer_offset+1
    input_buffer_offset = x;
    //     jsr get_line_length
    get_line_length();
    //     tya
    a = y;
    //     clc
    flags &= ~FLAG_C;
    //     adc input_buffer_offset+1
    { uint16_t sum = (uint16_t)a + input_buffer_offset; a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     bcs cae03
    if (flags & FLAG_C) { sub_cae03(); return; }
    //     cmp #0x85
    { uint16_t tmp_ = a - 0x85; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x85 ? FLAG_C : 0); }
    //     bcs cae03
    if (flags & FLAG_C) { sub_cae03(); return; }
    //     inc l006d
    l006d++;
    //     lda current_edit_line_ptr
    a = (uint8_t)(current_edit_line_ptr & 0xff);
    //     sta tmp6
    tmp6 = a;
    //     lda current_edit_line_ptr+1
    a = (uint8_t)(current_edit_line_ptr >> 8);
    //     sta tmp7
    tmp7 = a;
    //     ldy #0x84
    y = 0x84;
    // cae27:
cae27:
    //     dey
    y--;
    //     ldx #0
    x = 0;
    //     tya
    a = y;
    //     clc
    flags &= ~FLAG_C;
    //     adc input_buffer_offset+1
    { uint16_t sum = (uint16_t)a + input_buffer_offset; a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     bcs cae35
    if (flags & FLAG_C) goto cae35;
    //     cmp #0x84
    { uint16_t tmp_ = a - 0x84; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x84 ? FLAG_C : 0); }
    //     bcs cae35
    if (flags & FLAG_C) goto cae35;
    //     tax
    x = a;
    // cae35:
cae35:
    //     stx l0081
    l0081 = x;
    // loop_cae37:
loop_cae37:
    //     jsr sub_ca536
    sub_ca536();
    //     bne cae52
    if (!(flags & FLAG_Z)) goto cae52;
    //     lda l0081
    a = l0081;
    //     beq cae4b
    if (flags & FLAG_Z) goto cae4b;
    //     clc
    flags &= ~FLAG_C;
    //     adc current_edit_line_ptr
    { uint16_t sum = (uint16_t)a + (uint8_t)(current_edit_line_ptr & 0xff); a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     sta markers_array,x
    ((uint8_t*)markers_array)[x] = a;
    //     lda current_edit_line_ptr+1
    a = (uint8_t)(current_edit_line_ptr >> 8);
    //     adc #0
    { uint16_t sum = (uint16_t)a + 0 + (flags & FLAG_C ? 1 : 0); a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     bne cae4d
    if (!(flags & FLAG_Z)) goto cae4d;
    // cae4b:
cae4b:
    //     sta markers_array,x
    ((uint8_t*)markers_array)[x] = a;
    // cae4d:
cae4d:
    //     sta markers_array+1,x
    ((uint8_t*)markers_array)[x + 1] = a;
    //     jmp loop_cae37
    goto loop_cae37;

    // cae52:
cae52:
    //     lda (current_edit_line_ptr),y
    a = ram[current_edit_line_ptr + y];
    //     sty l0084
    l0084 = y;
    //     ldy l0081
    y = l0081;
    //     beq cae5c
    if (y == 0) goto cae5c;
    //     sta (current_edit_line_ptr),y
    ram[current_edit_line_ptr + y] = a;
    // cae5c:
cae5c:
    //     ldy l0084
    y = l0084;
    //     cpy xpos
    { uint16_t tmp_ = y - xpos; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= xpos ? FLAG_C : 0); }
    //     bne cae27
    if (!(flags & FLAG_Z)) goto cae27;
    //     clc
    flags &= ~FLAG_C;
    //     rts
}
static void cae64(void) {
    // cae64: Deletes N bytes at cursor position, shifting existing content left

    //     stx input_buffer_offset+1
    input_buffer_offset = x;
    //     inc l006d
    l006d++;
    //     lda current_edit_line_ptr
    a = (uint8_t)(current_edit_line_ptr & 0xff);
    //     sta tmp6
    tmp6 = a;
    //     lda current_edit_line_ptr+1
    a = (uint8_t)(current_edit_line_ptr >> 8);
    //     sta tmp7
    tmp7 = a;
    //     ldy xpos
    y = xpos;
    //     tya
    a = y;
    //     clc
    flags &= ~FLAG_C;
    //     adc input_buffer_offset+1
    { uint16_t sum = (uint16_t)a + input_buffer_offset; a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     sta l0084
    l0084 = a;
    // cae78:
cae78:
    //     jsr sub_ca536
    sub_ca536();
    //     bne cae98
    if (!(flags & FLAG_Z)) goto cae98;
    //     lda #0
    a = 0;
    //     cpy l0084
    { uint16_t tmp_ = y - l0084; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= l0084 ? FLAG_C : 0); }
    //     bcc cae91
    if (!(flags & FLAG_C)) goto cae91;
    //     tya
    a = y;
    //     sbc input_buffer_offset+1
    { uint16_t tmp_ = (uint16_t)a - input_buffer_offset - (1 - (flags & FLAG_C ? 1 : 0)); a = (uint8_t)tmp_; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (tmp_ < 0x10000 ? FLAG_C : 0); }
    //     clc
    flags &= ~FLAG_C;
    //     adc current_edit_line_ptr
    { uint16_t sum = (uint16_t)a + (uint8_t)(current_edit_line_ptr & 0xff); a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     sta markers_array,x
    ((uint8_t*)markers_array)[x] = a;
    //     lda current_edit_line_ptr+1
    a = (uint8_t)(current_edit_line_ptr >> 8);
    //     adc #0
    { uint16_t sum = (uint16_t)a + 0 + (flags & FLAG_C ? 1 : 0); a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     bne cae93
    if (!(flags & FLAG_Z)) goto cae93;
    // cae91:
cae91:
    //     sta markers_array,x
    ((uint8_t*)markers_array)[x] = a;
    // cae93:
cae93:
    //     sta markers_array+1,x
    ((uint8_t*)markers_array)[x + 1] = a;
    //     jmp cae78
    goto cae78;

    // cae98:
cae98:
    //     iny
    y++;
    //     cpy #0x85
    { uint16_t tmp_ = y - 0x85; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= 0x85 ? FLAG_C : 0); }
    //     bcc cae78
    if (!(flags & FLAG_C)) goto cae78;
    //     lda xpos
    a = xpos;
    //     cmp #0x84
    { uint16_t tmp_ = a - 0x84; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x84 ? FLAG_C : 0); }
    //     bcs return_78
    if (flags & FLAG_C) { /* return_78: */ return; }
    //     ldy xpos
    y = xpos;
    // loop_caea5:
loop_caea5:
    //     sty l0084
    l0084 = y;
    //     ldx #0x10
    x = 0x10;
    //     tya
    a = y;
    //     clc
    flags &= ~FLAG_C;
    //     adc input_buffer_offset+1
    { uint16_t sum = (uint16_t)a + input_buffer_offset; a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    //     bcs caeb7
    if (flags & FLAG_C) goto caeb7;
    //     tay
    y = a;
    //     cpy #0x84
    { uint16_t tmp_ = y - 0x84; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= 0x84 ? FLAG_C : 0); }
    //     bcs caeb7
    if (flags & FLAG_C) goto caeb7;
    //     lda (current_edit_line_ptr),y
    a = ram[current_edit_line_ptr + y];
    //     tax
    x = a;
    // caeb7:
caeb7:
    //     ldy l0084
    y = l0084;
    //     txa
    a = x;
    //     sta (current_edit_line_ptr),y
    ram[current_edit_line_ptr + y] = a;
    //     iny
    y++;
    //     cpy #0x84
    { uint16_t tmp_ = y - 0x84; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= 0x84 ? FLAG_C : 0); }
    //     bcc loop_caea5
    if (!(flags & FLAG_C)) goto loop_caea5;
    // return_78:
    //     rts
}
static void sub_caec2(void) {
    // sub_caec2: Finds left margin stop (0x0b) in edit line

    //     lda ruler_left_stop
    a = ruler_left_stop;
    //     beq caed4
    if (flags & FLAG_Z) goto caed4;
    //     ldy #0
    y = 0;
    // loop_caec8:
loop_caec8:
    //     lda (current_edit_line_ptr),y
    a = ram[current_edit_line_ptr + y];
    //     iny
    y++;
    //     cmp #0x0b
    { uint16_t tmp_ = a - 0x0b; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x0b ? FLAG_C : 0); }
    //     beq caed4
    if (flags & FLAG_Z) goto caed4;
    //     cpy #0x84
    { uint16_t tmp_ = y - 0x84; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= 0x84 ? FLAG_C : 0); }
    //     bcc loop_caec8
    if (!(flags & FLAG_C)) goto loop_caec8;
    //     rts
    return;

    // caed4:
caed4:
    //     clc
    flags &= ~FLAG_C;
    //     rts
}
static void sub_caedd(void) {
    // sub_caedd:
    //     lda xpos
    a = xpos;
    //     pha
    { uint8_t saved_a = a;
    //     sty xpos
    xpos = y;
    //     ldx #1
    x = 1;
    //     jsr sub_cae06
    sub_cae06();
    //     bcs caef0
    if (!(flags & FLAG_C)) {
        //     ldy xpos
        y = xpos;
        //     lda #0x0b
        a = 0x0b;
        //     sta (current_edit_line_ptr),y
        ram[current_edit_line_ptr + y] = a;
        //     iny
        y++;
    }
    // caef0:
    //     pla
    a = saved_a; }
    //     sta xpos
    xpos = a;
    //     rts
}
static void sub_caed6(void) {
    // sub_caed6:
    //     jsr sub_caec2
    sub_caec2();
    //     bcc caed4
    if (flags & FLAG_C) {
        //     ldy #0
        y = 0;
        sub_caedd();
    }
    // caed4:
    //     rts
}
static void sub_caef4(void) {
    // sub_caef4: Handles margin/folding adjustments when typing at left margin

    //     lda format_mode_flag
    a = format_mode_flag;
    //     and #0x81
    a &= 0x81;
    //     bne caf31
    if (!(flags & FLAG_Z)) goto caf31;
    //     jsr sub_caec2
    sub_caec2();
    //     bcc caf31
    if (flags & FLAG_C) goto caf31;
    //     jsr get_line_length
    get_line_length();
    //     lda xpos
    a = xpos;
    //     sta l0083
    l0083 = a;
    //     sty xpos
    xpos = y;
    //     jsr sub_ca608
    recalculate_cursor_xpos();
    //     lda l0072
    a = l0072;
    //     cmp ruler_left_stop
    { uint16_t tmp_ = a - ruler_left_stop; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= ruler_left_stop ? FLAG_C : 0); }
    //     bcc caf19
    if (!(flags & FLAG_C)) goto caf19;
    //     ldy l0083
    y = l0083;
    //     sty xpos
    xpos = y;
    //     inc xpos
    xpos++;
    //     bcs caf2a
    goto caf2a;

    // caf19:
caf19:
    //     lda l0083
    a = l0083;
    //     ldy xpos
    y = xpos;
    //     cpy l0083
    { uint16_t tmp_ = y - l0083; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= l0083 ? FLAG_C : 0); }
    //     bcs caf28
    if (flags & FLAG_C) goto caf28;
    //     sec
    flags |= FLAG_C;
    //     sbc ruler_left_stop
    { uint16_t tmp_ = (uint16_t)a - ruler_left_stop - (1 - (flags & FLAG_C ? 1 : 0)); a = (uint8_t)tmp_; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (tmp_ < 0x10000 ? FLAG_C : 0); }
    //     bcc caf2a
    if (!(flags & FLAG_C)) goto caf2a;
    //     adc xpos
    { uint16_t sum = (uint16_t)a + xpos; a = (uint8_t)sum; if (sum > 0xff) flags |= FLAG_C; else flags &= ~FLAG_C; }
    // caf28:
caf28:
    //     sta xpos
    xpos = a;
    // caf2a:
caf2a:
    //     jsr sub_caedd
    sub_caedd();
    //     bcs return_79
    if (flags & FLAG_C) { /* return_79: */ return; }
    //     inc l0074
    l0074++;
    // caf31:
caf31:
    //     clc
    flags &= ~FLAG_C;
    // return_79:
    //     rts
}
static void draw_previous_word(void) {
    // draw_previous_word: Moves cursor back to start of previous word

    //     lda current_edit_line_ptr
    a = (uint8_t)(current_edit_line_ptr & 0xff);
    //     sta tmp0
    tmp0 = a;
    //     lda current_edit_line_ptr+1
    a = (uint8_t)(current_edit_line_ptr >> 8);
    //     sta tmp1
    tmp1 = a;
    //     ldy xpos
    y = xpos;
    //     beq caf55
    if (y == 0) goto caf55;
    // loop_caf3f:
loop_caf3f:
    //     dey
    y--;
    //     beq caf55
    if (y == 0) goto caf55;
    //     jsr draw_char
    draw_char();
    //     dey
    y--;
    //     cmp #0x20 ; ' '
    { uint16_t tmp_ = a - 0x20; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x20 ? FLAG_C : 0); }
    //     beq loop_caf3f
    if (flags & FLAG_Z) goto loop_caf3f;
    // loop_caf4a:
loop_caf4a:
    //     dey
    y--;
    //     jsr draw_char
    draw_char();
    //     cmp #0x20 ; ' '
    { uint16_t tmp_ = a - 0x20; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= 0x20 ? FLAG_C : 0); }
    //     beq caf55
    if (flags & FLAG_Z) goto caf55;
    //     dey
    y--;
    //     bne loop_caf4a
    if (y != 0) goto loop_caf4a;
    // caf55:
caf55:
    //     sty xpos
    xpos = y;
    //     jsr draw_char
    draw_char();
    //     dey
    y--;
    //     rts
}
static void caf5c(void) {
    // caf5c:
    uint8_t old = format_mode_flag;
    format_mode_flag |= 0x80;
    if (old != format_mode_flag) {
        flags_need_redrawing_flag++;
    }
}
static void sub_caf5f(void) {
    // sub_caf5f:
    uint8_t old = format_mode_flag;
    format_mode_flag &= ~0x80;
    if (old != format_mode_flag) {
        flags_need_redrawing_flag++;
    }
}
static void deref_and_check_for_command_prefix(void) {
    // deref_and_check_for_command_prefix:
    //     lda (tmp0),y
    a = ram[((uint16_t)tmp1 << 8 | tmp0) + y];
    check_for_command_prefix();
}
static void check_for_command_prefix(void) {
    // check_for_command_prefix:
    //     cmp #0x80
    //     beq return_81
    if (a == 0x80) {
        flags |= FLAG_Z | FLAG_C;
        return;
    }
    //     cmp #0x81
    //     clc
    if (a == 0x81) {
        flags |= FLAG_Z;
        flags &= ~FLAG_C;
        return;
    }
    flags &= ~(FLAG_Z | FLAG_C);
    // return_81:
    //     rts
}
static void system_init(void) {
    himem = 0xffff;
    oshwm = 0;
    uint16_t size_ = screen_getsize();
    screen_width = (uint8_t)(size_ & 0xff);
    screen_height = (uint8_t)(size_ >> 8);
    if (screen_height > MAX_LINES)
        screen_height = MAX_LINES;
}
static void noscreen(void) {
    // Pseudocode: Screen driver not found: displays error and exits

    // noscreen:
    //     jsr print_inline_string
    //     .ascii "No SCREEN\n"
    //     .byte 0
    cli_putstring("No SCREEN\n");
    exit(0);
}
static void compute_bytes_free(void) {
    // Pseudocode: Computes number of free bytes between top and himem

    // ; ***************************************************************************************
    // compute_bytes_free:
    //     lda himem
    a = (uint8_t)(himem & 0xff);
    //     sec
    flags |= FLAG_C;
    //     sbc top
    { int16_t tmp_ = (int16_t)a - (int16_t)(uint8_t)(top & 0xff); a = (uint8_t)tmp_; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (tmp_ >= 0 ? FLAG_C : 0); }
    //     tax
    x = a;
    //     lda himem+1
    a = (uint8_t)(himem >> 8);
    //     sbc top+1
    { int16_t tmp_ = (int16_t)a - (int16_t)(uint8_t)(top >> 8) - (1 - ((flags & FLAG_C) ? 1 : 0)); a = (uint8_t)tmp_; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (tmp_ >= 0 ? FLAG_C : 0); }
    //     tay
    y = a;
    // return_84:
    //     rts
    return;
}
static void initialise_document(void) {
    uint8_t tmp8, tmp9;
    a = 0;
    printer_driver_name[0] = a;
    format_mode_flag = a;
    justifying_flag = a;
    insert_mode_flag = a;
    print_flags = a;
    // page = oshwm + 1 (16-bit)
    page = oshwm + 1;
    y = 0;
    file_edit_flags = y;
    xpos = y;
    a = 0xaa;
    ram[oshwm] = a;
    { uint16_t tmp = page - 1; tmp8 = (uint8_t)(tmp & 0xff); tmp9 = (uint8_t)(tmp >> 8); }
    a = 0x0d;
    ram[((uint16_t)tmp9 << 8) | tmp8] = a;
    current_line_buffer[MAX_LINE_LENGTH - 1] = a;
    top = page;
    ptr1 = (uint16_t)(uintptr_t)current_line_buffer;
    current_edit_line_ptr = ptr1 + 3;
    current_format_line_ptr = current_edit_line_ptr;
    a = (uint8_t)(uintptr_t)current_ruler_buffer;
    y = (uint8_t)((uintptr_t)current_ruler_buffer >> 8);
    current_ruler_ptr = ((uint16_t)y << 8) | a;
    create_default_ruler();
    y++;
    a = 0x0d;
    ram[((uint16_t)tmp1 << 8) | ((uint16_t)tmp0 + y)] = a;
    a = (uint8_t)(uintptr_t)just_before_current_ruler_buffer;
    {   uint8_t low = a;
        y = (uint8_t)((uintptr_t)just_before_current_ruler_buffer >> 8);
        ruler_stack[0xfe / 2] = ((uint16_t)y << 8) | low;
    }
    move_cursor_to_top_of_document();
    clear_cmd();
}
static void cb05a(void) {
    // cb05a: Ensures at least one CR at top of document

    //     ldy page+1
    y = (uint8_t)(page >> 8);
    //     cpy top+1
    { uint16_t tmp_ = y - (uint8_t)(top >> 8); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (y >= (uint8_t)(top >> 8) ? FLAG_C : 0); }
    //     bne return_85
    if (!(flags & FLAG_Z)) return;
    //     lda page
    a = (uint8_t)(page & 0xff);
    //     cmp top
    { uint16_t tmp_ = a - (uint8_t)(top & 0xff); flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= (uint8_t)(top & 0xff) ? FLAG_C : 0); }
    //     bne return_85
    if (!(flags & FLAG_Z)) return;
    //     inc top
    top++;
    //     bne cb06c
    if (top != 0) goto cb06c;
    //     inc top+1
    // (automatically handled by 16-bit top)
    // cb06c:
cb06c:
    //     sta current_line_ptr
    current_line_ptr = (uint16_t)y << 8 | a;
    //     sty current_line_ptr+1
    //     ldy #0
    y = 0;
    //     lda #0x0d
    a = 0x0d;
    //     sta (page),y
    ram[page + y] = a;
    //     tya
    a = y;
    //     sta (top),y
    ram[top + y] = a;
    // return_85:
    //     rts
}
static void move_cursor_to_top_of_document(void) {
    // move_cursor_to_top_of_document: Moves cursor to the top (page) of the document

    //     lda page
    a = (uint8_t)(page & 0xff);
    //     sta current_line_ptr
    current_line_ptr = page;
    //     lda page+1
    a = (uint8_t)(page >> 8);
    //     sta current_line_ptr+1
    //     lda #0
    a = 0;
    //     sta xpos
    xpos = 0;
    //     ldy #0xfe
    y = 0xfe;
    //     sty l0012
    l0012 = 0xfe;
    //     sty ruler_stack_ptr
    ruler_stack_ptr = y;
    //     sty l0033
    l0033 = y;
    //     jmp cab91
    cab91();
}
static void clear_cmd(void) {
    // Pseudocode: Clears all markers (sets to zero)

    // ; ***************************************************************************************
    // clear_cmd:
    //     ldx #0x0b
    x = 0x0b;
    //     lda #0
    a = 0;
    // loop_cb095:
loop_cb095:
    //     sta markers_array,x
    ((uint8_t*)markers_array)[x] = a;
    //     dex
    x--;
    //     bpl loop_cb095
    if (!(x & 0x80)) goto loop_cb095;
    //     rts
    return;
}
static void enter_editor_mode(void) {
    screen_enter();
    // enter_editor_mode: Enters editor mode: clears screen, resets state variables

    //     jsr clear_screen
    clear_screen();
    //     lda #0
    a = 0;
    //     sta l006d
    l006d = 0;
    //     sta l006f
    l006f = 0;
    //     sta l006e
    l006e = 0;
    //     ldx screen_height
    x = screen_height;
    // loop_cb0a8:
loop_cb0a8:
    //     sta line_lengths,x
    line_lengths[x] = a;
    //     dex
    x--;
    //     bpl loop_cb0a8
    if (!(x & 0x80)) goto loop_cb0a8;
    //     ldx #2
    x = 2;
    //     stx l0073
    l0073 = 2;
    //     stx status_line_needs_redrawing_flag
    status_line_needs_redrawing_flag = 2;
    flags_need_redrawing_flag = 1;
    //     rts
}
void create_default_ruler(void) {
    // Pseudocode: Creates a default ruler with tab stops every 6 columns

    // ; ***************************************************************************************
    // create_default_ruler:
    //     sta tmp0
    tmp0 = a;
    //     sty tmp1
    tmp1 = y;

    //     lda #0
    a = 0;
    //     tay                                                               ; Y=0x00
    y = a;
    // loop_cb0e7:
loop_cb0e7:
    //     lda #0x2e ; '.'
    a = 0x2e;
    // loop_cb0e9:
loop_cb0e9:
    //     sta (tmp0),y
    ram[((uint16_t)tmp1 << 8 | tmp0) + y] = a;
    //     iny
    y++;
    //     tya
    a = y;
    //     tax
    x = a;
    //     inx
    x++;
    //     clc
    flags &= ~FLAG_C;
    //     adc #6
    { uint16_t sum = (uint16_t)a + 6; a = (uint8_t)sum; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)sum == 0 ? FLAG_Z : 0) | ((uint8_t)sum & FLAG_N) | (sum > 0xff ? FLAG_C : 0); }
    //     cmp screen_width
    { uint16_t tmp_ = a - screen_width; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | ((uint8_t)tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | (a >= screen_width ? FLAG_C : 0); }
    //     beq cb0ff
    if (flags & FLAG_Z) goto cb0ff;
    //     txa
    a = x;
    //     and #7
    a &= 7;
    //     bne loop_cb0e7
    if (a != 0) goto loop_cb0e7;
    //     lda #0x2a ; '*'
    a = 0x2a;
    //     bne loop_cb0e9                                                    ; ALWAYS branch
    goto loop_cb0e9;

    // cb0ff:
cb0ff:
    //     lda #0x3c ; '<'
    a = 0x3c;
    //     sta (tmp0),y
    ram[((uint16_t)tmp1 << 8 | tmp0) + y] = a;
    //     rts
    return;
}
static void sub_cb104(void) {
    // Pseudocode: Resets formatting registers and default print settings

    // sub_cb104:
    //     lda #0
    a = 0;
    //     ldx #0x33 ; '3'
    x = 0x33;
    // loop_cb108:
loop_cb108:
    //     sta register_value_array,x
    register_value_array[x] = a;
    //     dex
    x--;
    //     bpl loop_cb108
    if (!(x & 0x80)) goto loop_cb108;
    //     sta header_text_maybe
    header_text_maybe[0] = a;
    //     sta footer_text_maybe
    footer_text_maybe[0] = a;
    //     sta two_sided_flag
    two_sided_flag = a;
    //     sta left_margin
    left_margin = a;
    //     sta line_spacing
    line_spacing = a;
    //     sta rhs_extra_margin
    rhs_extra_margin = a;
    //     sta macro_executing_flag
    macro_executing_flag = a;
    //     lda #0x42 ; 'B'
    a = 0x42;
    //     sta page_length
    page_length = a;
    //     lda #1
    a = 1;
    //     sta footers_enabled_flag
    footers_enabled_flag = a;
    //     sta headers_enabled_flag
    headers_enabled_flag = a;
    //     sta l0038
    l0038 = a;
    //     sta register_value_p
    register_value_p[0] = a;
    //     sta register_value_l
    register_value_l[0] = a;
    //     ldy #0x80
    y = 0x80;
    //     sty highlight1_code
    highlight1_code = y;
    //     iny                                                               ; Y=0x81
    y++;
    //     sty highlight2_code
    highlight2_code = y;
    //     lda #4
    a = 4;
    //     sta top_margin
    top_margin = a;
    //     sta bottom_margin
    bottom_margin = a;
    //     sta header_margin
    header_margin = a;
    //     sta footer_margin
    footer_margin = a;
    //     jmp c92f0
    sub_c92f0();
    return;
}
static void control_key_to_ascii(void) {
    // Pseudocode: Converts control key code to ASCII letter by ORing with 0x40
    // zproc control_key_to_ascii
    //     cmp #0x20
    //     zif lt
    //         ora #0x40
    //     zendif
    //     jmp to_uppercase
    // zendproc
    if (a < 0x20) a |= 0x40;
    to_uppercase(); return;
}
static void q_command_key(void) {
    // Pseudocode: Q-command handler: prompts for Q-key, looks up in q_key_table

    // zproc q_command_key
    //     ldx #'^'
    //     ldy #'Q'
    //     jsr draw_prompt_characters
    //     inc flags_need_redrawing_flag
    //     jsr read_char
    //     jsr control_key_to_ascii
    //     ldx #<q_key_table
    //     ldy #>q_key_table
    //     ; lookup dispatch via q_key_table:
    //     ;   { 'R': f1_top_of_text_key, 'C': f2_bottom_of_text_key, ... }
    //     jmp look_up_address_in_table_and_call
    // zendproc
    x = '^';
    y = 'Q';
    draw_prompt_characters();
    flags_need_redrawing_flag++;
    read_char();
    control_key_to_ascii();
    switch (a) {
        case 'R': f1_top_of_text_key(); return;
        case 'C': f2_bottom_of_text_key(); return;
        case 'S': f4_beginning_of_line_key(); return;
        case 'D': f5_end_of_line_key(); return;
        case 'Y': f3_delete_to_eol_key(); return;
        case 'J': cf6_split_line_key(); return;
        case 'M': sf6_go_to_marker_key(); return;
        case '1': go_to_marker_1(); return;
        case '2': go_to_marker_2(); return;
        case '3': go_to_marker_3(); return;
        case '4': go_to_marker_4(); return;
        case '5': go_to_marker_5(); return;
        case '6': go_to_marker_6(); return;
    }
    return;
}
static void o_command_key(void) {
    // Pseudocode: O-command handler: prompts for O-key, looks up in o_key_table

    // zproc o_command_key
    //     ldx #'^'
    //     ldy #'O'
    //     jsr draw_prompt_characters
    //     inc flags_need_redrawing_flag
    //     jsr read_char
    //     jsr control_key_to_ascii
    //     ldx #<o_key_table
    //     ldy #>o_key_table
    //     ; lookup dispatch via o_key_table:
    //     ;   { 'J': cf3_justify_mode_key, 'X': sf2_release_margins_key, ... }
    //     jmp look_up_address_in_table_and_call
    // zendproc
    x = '^';
    y = 'O';
    draw_prompt_characters();
    flags_need_redrawing_flag++;
    read_char();
    control_key_to_ascii();
    switch (a) {
        case 'J': cf3_justify_mode_key(); return;
        case 'X': sf2_release_margins_key(); return;
        case 'C': sf8_edit_command_key(); return;
        case 'D': sf9_delete_command_key(); return;
        case 'F': cf2_format_mode_key(); return;
        case 'M': cf8_mark_as_ruler_key(); return;
        case 'R': sf11_copy_key(); return;
        case 'S': cf5_default_ruler_key(); return;
        case 'U': sf4_highlight1_key(); return;
        case 'B': sf5_highlight2_key(); return;
    }
    return;
}
static void k_command_key(void) {
    // Pseudocode: K-command handler: prompts for K-key, looks up in k_key_table

    // zproc k_command_key
    //     ldx #'^'
    //     ldy #'K'
    //     jsr draw_prompt_characters
    //     inc flags_need_redrawing_flag
    //     jsr read_char
    //     jsr control_key_to_ascii
    //     ldx #<k_key_table
    //     ldy #>k_key_table
    //     ; lookup dispatch via k_key_table:
    //     ;   { 'M': sf7_set_marker_key, 'C': f11_copy_key, ... }
    //     jmp look_up_address_in_table_and_call
    // zendproc
    x = '^';
    y = 'K';
    draw_prompt_characters();
    flags_need_redrawing_flag++;
    read_char();
    control_key_to_ascii();
    switch (a) {
        case 'M': sf7_set_marker_key(); return;
        case 'C': f11_copy_key(); return;
        case 'V': sf0_move_block_key(); return;
        case 'Y': cf0_delete_block_key(); return;
        case '1': set_marker_1(); return;
        case '2': set_marker_2(); return;
        case '3': set_marker_3(); return;
        case '4': set_marker_4(); return;
        case '5': set_marker_5(); return;
        case '6': set_marker_6(); return;
    }
    return;
}
    // decimal_table:
    //     .word 10000, 1000, 100, 10

    // non_function_key_table:
    //     .byte '['-'@' ; escape
    //     .word esc_key
    //     .byte 'M'-'@' ; return
    //     .word return_key
    //     .byte 0x7f ; delete
    //     .word delete_key
    //     .byte 'I'-'@' ; tab
    //     .word tab_key
    //     .byte 'E'-'@' ; up
    //     .word f15_up_key
    //     .byte SCREEN_KEY_UP
    //     .word f15_up_key
    //     .byte 'S'-'@' ; left
    //     .word f12_left_key
    //     .byte SCREEN_KEY_LEFT
    //     .word f12_left_key
    //     .byte 'D'-'@' ; right
    //     .word f13_right_key
    //     .byte SCREEN_KEY_RIGHT
    //     .word f13_right_key
    //     .byte 'X'-'@' ; down
    //     .word f14_down_key
    //     .byte SCREEN_KEY_DOWN
    //     .word f14_down_key
    //     .byte 'A'-'@' ; word left
    //     .word sf12_left_key
    //     .byte 'F'-'@' ; word right
    //     .word sf13_right_key
    //     .byte 'C'-'@' ; page down
    //     .word sf14_down_key
    //     .byte 'R'-'@' ; page up
    //     .word sf15_up_key
    //     .byte 'G'-'@' ; delete char
    //     .word f9_delete_char_key
    //     .byte 'H'-'@' ; insert char
    //     .word f8_insert_char_key
    //     .byte 'Y'-'@' ; delete line
    //     .word f7_delete_line_key
    //     .byte 'V'-'@' ; insert on/off
    //     .word cf4_insert_mode_key
    //     .byte 'N'-'@' ; insert blank line
    //     .word f6_insert_line_key
    //     .byte 'B'-'@' ; format block
    //     .word f0_format_block_key
    //     .byte 'T'-'@' ; delete to char
    //     .word sf3_delete_to_char_key
    //     .byte 'L'-'@' ; next search match
    //     .word cf1_next_match_key
    //     .byte 'J'-'@' ; join lines
    //     .word cf7_join_lines_key
    //     .byte 'P'-'@' ; swap case
    //     .word sf1_swap_case_key
    //     .byte 'O'-'@' ; O-command
    //     .word o_command_key
    //     .byte 'Q'-'@' ; Q-command
    //     .word q_command_key
    //     .byte 'K'-'@' ; K-command
    //     .word k_command_key
    //     .byte 0

    // q_key_table:
    //     .byte 'R'     ; beginning of file
    //     .word f1_top_of_text_key
    //     .byte 'C'     ; end of file
    //     .word f2_bottom_of_text_key
    //     .byte 'S'     ; beginning of line
    //     .word f4_beginning_of_line_key
    //     .byte 'D'     ; end of line
    //     .word f5_end_of_line_key
    //     .byte 'Y'     ; delete to end of line
    //     .word f3_delete_to_eol_key
    //     .byte 'J'     ; split lines
    //     .word cf6_split_line_key
    //     .byte 'M'     ; go to marker
    //     .word sf6_go_to_marker_key
    //     .byte '1'     ; go to marker
    //     .word go_to_marker_1
    //     .byte '2'     ; go to marker
    //     .word go_to_marker_2
    //     .byte '3'     ; go to marker
    //     .word go_to_marker_3
    //     .byte '4'     ; go to marker
    //     .word go_to_marker_4
    //     .byte '5'     ; go to marker
    //     .word go_to_marker_5
    //     .byte '6'     ; go to marker
    //     .word go_to_marker_6
    //     .byte 0

    // o_key_table:
    //     .byte 'J'     ; justification on/off
    //     .word cf3_justify_mode_key
    //     .byte 'X'     ; margin release
    //     .word sf2_release_margins_key
    //     .byte 'C'     ; edit command
    //     .word sf8_edit_command_key
    //     .byte 'D'     ; delete command
    //     .word sf9_delete_command_key
    //     .byte 'F'     ; format mode
    //     .word cf2_format_mode_key
    //     .byte 'M'     ; mark as ruler
    //     .word cf8_mark_as_ruler_key
    //     .byte 'R'     ; copy ruler
    //     .word sf11_copy_key
    //     .byte 'S'     ; standard ruler
    //     .word cf5_default_ruler_key
    //     .byte 'U'     ; highlight 1
    //     .word sf4_highlight1_key
    //     .byte 'B'     ; highlight 2
    //     .word sf5_highlight2_key
    //     .byte 0

    // k_key_table:
    //     .byte 'M'     ; set marker
    //     .word sf7_set_marker_key
    //     .byte 'C'     ; copy block
    //     .word f11_copy_key
    //     .byte 'V'     ; move block
    //     .word sf0_move_block_key
    //     .byte 'Y'     ; delete block
    //     .word cf0_delete_block_key
    //     .byte '1'     ; set marker
    //     .word set_marker_1
    //     .byte '2'     ; set marker
    //     .word set_marker_2
    //     .byte '3'     ; set marker
    //     .word set_marker_3
    //     .byte '4'     ; set marker
    //     .word set_marker_4
    //     .byte '5'     ; set marker
    //     .word set_marker_5
    //     .byte '6'     ; set marker
    //     .word set_marker_6
    //     .byte 0

    // jumptable_ptrs:
    // lb152 = jumptable_ptrs+1
    //     .word jumptable2_format, jumptable4_cli
    // jumptable4_cli:
    //     .word   quit_cmd,   new_cmd,     format_cmd,   setup_cmd
    //     .word   read_cmd,  more_cmd,     screen_cmd,  sheets_cmd
    //     .word   save_cmd, count_cmd,      field_cmd, printer_cmd
    //     .word search_cmd, clear_cmd, microspace_cmd,    fold_cmd
    //     .word   name_cmd,  mode_cmd,     finish_cmd,   print_cmd
    //     .word change_cmd, write_cmd,       edit_cmd, replace_cmd
    //     .word   load_cmd,   bye_cmd
    // jumptable4_cli_end:

    //     .ascii "Mark Colton"
    //     .byte 0
    // ; If a char contains 0x20, it means that a match failure here still counts as a
    // ; positive match --- allowing B to be the abbreviation for BYE, for example.
    // parser_table:
uint8_t parser_table[] = {
    // ; 'QUIT' -> 0, flag=1
    //     .byte 0x0a, 0x0e, 0x12, 0x0f
    0x0a, 0x0e, 0x12, 0x0f,
    //     .byte 0x81
    0x81,
    // ; 'NEW' -> 1, flag=0
    //     .byte 0x15, 0x1e, 0x0c
    0x15, 0x1e, 0x0c,
    //     .byte 0x80
    0x80,
    // ; 'FORMAT' -> 2, flag=1
    //     .byte 0x1d, 0x14, 9, 0x36, 0x3a, 0x2f
    0x1d, 0x14, 9, 0x36, 0x3a, 0x2f,
    //     .byte 0x81
    0x81,
    // ; 'SETUP' -> 3, flag=0
    //     .byte 8, 0x1e, 0x0f, 0x2e, 0x2b
    8, 0x1e, 0x0f, 0x2e, 0x2b,
    //     .byte 0x80
    0x80,
    // ; 'READ' -> 4, flag=1
    //     .byte 9, 0x1e, 0x3a, 0x3f
    9, 0x1e, 0x3a, 0x3f,
    //     .byte 0x81
    0x81,
    // ; 'MORE' -> 5, flag=1
    //     .byte 0x16, 0x14, 0x29, 0x3e
    0x16, 0x14, 0x29, 0x3e,
    //     .byte 0x81
    0x81,
    // ; 'SCREEN' -> 6, flag=1
    //     .byte 8, 0x18, 0x29, 0x3e, 0x3e, 0x35
    8, 0x18, 0x29, 0x3e, 0x3e, 0x35,
    //     .byte 0x81
    0x81,
    // ; 'SHEETS' -> 7, flag=1
    //     .byte 8, 0x13, 0x3e, 0x3e, 0x2f, 0x28
    8, 0x13, 0x3e, 0x3e, 0x2f, 0x28,
    //     .byte 0x81
    0x81,
    // ; 'SAVE' -> 8, flag=1
    //     .byte 8, 0x1a, 0x2d, 0x3e
    8, 0x1a, 0x2d, 0x3e,
    //     .byte 0x81
    0x81,
    // ; 'COUNT' -> 9, flag=1
    //     .byte 0x18, 0x14, 0x2e, 0x35, 0x2f
    0x18, 0x14, 0x2e, 0x35, 0x2f,
    //     .byte 0x81
    0x81,
    // ; 'FIELD' -> 10, flag=1
    //     .byte 0x1d, 0x12, 0x3e, 0x37, 0x3f
    0x1d, 0x12, 0x3e, 0x37, 0x3f,
    //     .byte 0x81
    0x81,
    // ; 'PRINTER' -> 11, flag=1
    //     .byte 0x0b, 9, 0x12, 0x15, 0x0f, 0x1e, 0x29
    0x0b, 9, 0x12, 0x15, 0x0f, 0x1e, 0x29,
    //     .byte 0x81
    0x81,
    // ; 'SEARCH' -> 12, flag=1
    //     .byte 8, 0x3e, 0x3a, 0x29, 0x38, 0x33
    8, 0x3e, 0x3a, 0x29, 0x38, 0x33,
    //     .byte 0x81
    0x81,
    // ; 'CLEAR' -> 13, flag=1
    //     .byte 0x18, 0x17, 0x3e, 0x3a, 0x29
    0x18, 0x17, 0x3e, 0x3a, 0x29,
    //     .byte 0x81
    0x81,
    // ; 'MICROSPACE' -> 14, flag=1
    //     .byte 0x16, 0x12, 0x38, 0x29, 0x34, 0x28, 0x2b, 0x3a, 0x38, 0x3e
    0x16, 0x12, 0x38, 0x29, 0x34, 0x28, 0x2b, 0x3a, 0x38, 0x3e,
    //     .byte 0x81
    0x81,
    // ; 'FOLD' -> 15, flag=1
    //     .byte 0x1d, 0x14, 0x37, 0x3f
    0x1d, 0x14, 0x37, 0x3f,
    //     .byte 0x81
    0x81,
    // ; 'NAME' -> 16, flag=1
    //     .byte 0x15, 0x3a, 0x36, 0x3e
    0x15, 0x3a, 0x36, 0x3e,
    //     .byte 0x81
    0x81,
    // ; 'MODE' -> 17, flag=0
    //     .byte 0x16, 0x34, 0x3f, 0x3e
    0x16, 0x34, 0x3f, 0x3e,
    //     .byte 0x80
    0x80,
    // ; 'FINISH' -> 18, flag=1
    //     .byte 0x1d, 0x32, 0x35, 0x32, 0x28, 0x33
    0x1d, 0x32, 0x35, 0x32, 0x28, 0x33,
    //     .byte 0x81
    0x81,
    // ; 'PRINT' -> 19, flag=1
    //     .byte 0x0b, 0x29, 0x32, 0x35, 0x2f
    0x0b, 0x29, 0x32, 0x35, 0x2f,
    //     .byte 0x81
    0x81,
    // ; 'CHANGE' -> 20, flag=1
    //     .byte 0x18, 0x33, 0x3a, 0x35, 0x3c, 0x3e
    0x18, 0x33, 0x3a, 0x35, 0x3c, 0x3e,
    //     .byte 0x81
    0x81,
    // ; 'WRITE' -> 21, flag=1
    //     .byte 0x0c, 0x29, 0x32, 0x2f, 0x3e
    0x0c, 0x29, 0x32, 0x2f, 0x3e,
    //     .byte 0x81
    0x81,
    // ; 'EDIT' -> 22, flag=0
    //     .byte 0x1e, 0x3f, 0x32, 0x2f
    0x1e, 0x3f, 0x32, 0x2f,
    //     .byte 0x80
    0x80,
    // ; 'REPLACE' -> 23, flag=1
    //     .byte 9, 0x3e, 0x2b, 0x37, 0x3a, 0x38, 0x3e
    9, 0x3e, 0x2b, 0x37, 0x3a, 0x38, 0x3e,
    //     .byte 0x81
    0x81,
    // ; 'LOAD' -> 24, flag=0
    //     .byte 0x17, 0x34, 0x3a, 0x3f
    0x17, 0x34, 0x3a, 0x3f,
    //     .byte 0x80
    0x80,
    // ; 'BYE' -> 25, flag=1
    //     .byte 0x19, 0x22, 0x3e
    0x19, 0x22, 0x3e,
    //     .byte 0x80
    0x80,
    //     .byte 0
    0
};
    // commands_table:
    // lb2a1 = commands_table+1
    //     .ascii "CE"
    //     .ascii "RJ"
    //     .ascii "DF"
    //     .ascii "DH"
    //     .ascii "DM"
    //     .ascii "EM"
    //     .ascii "SR"
    //     .ascii "PE"
    //     .ascii "TM"
    //     .ascii "BM"
    //     .ascii "PL"
    //     .ascii "TS"
    //     .ascii "FO"
    //     .ascii "HE"
    //     .ascii "HT"
    //     .ascii "HM"
    //     .ascii "FM"
    //     .ascii "LM"
    //     .ascii "LS"
    //     .ascii "OP"
    //     .ascii "EP"
    //     .ascii "LJ"
    //     .ascii "PB"
    //     .byte 0xff
    // ; overlapping: cmp lf894,y
    // jumptable2_format:
    //     .word ce_fmt_cmd, rj_fmt_cmd, df_fmt_cmd, dh_fmt_cmd, dm_fmt_cmd
    //     .word  return_34, em_fmt_cmd, pe_fmt_cmd, tm_fmt_cmd, bm_fmt_cmd
    //     .word pl_fmt_cmd, ts_fmt_cmd, fo_fmt_cmd, he_fmt_cmd, ht_fmt_cmd
    //     .word hm_fmt_cmd, fm_fmt_cmd, lm_fmt_cmd, ls_fmt_cmd, op_fmt_cmd
    //     .word ep_fmt_cmd, lj_fmt_cmd, pb_fmt_cmd

static void readline(void) {
    input_buffer_offset = 0;
    if (!cli_readstring((char *)input_buffer, MAX_COMMAND_LENGTH)) {
        flags |= FLAG_C;
        return;
    }
    size_t len = strlen((char *)input_buffer);
    if (len > 0 && input_buffer[len - 1] == '\n')
        len--;
    if (len == 0) {
        input_buffer[0] = 0x0d;
        flags |= FLAG_C;
        return;
    }
    input_buffer[len] = 0x0d;
    flags &= ~FLAG_C;
}
static void select_file(void) {
    file_ptr = x ? output_fp : input_fp;
}
static void open_input_file(void) {
    zero_terminate_filename_buffer();
    input_fp = fopen((char *)filename_buffer, "rb");
    if (!input_fp) { file_not_found_error(); return; }
    file_ptr = input_fp;
}
static void open_output_file(void) {
    zero_terminate_filename_buffer();
    output_fp = fopen((char *)filename_buffer, "wb");
    if (!output_fp) { file_error(); return; }
    file_ptr = output_fp;
}

static void flush_file(void) {
    if (file_ptr) {
        fflush(file_ptr);
    }
}
static void close_file(void) {
    if (file_ptr) {
        fclose(file_ptr);
        file_ptr = NULL;
    }
}
static void put_byte_to_file(void) {
    fputc(a, file_ptr);
}
static void get_byte_from_file(void) {
    int c = fgetc(file_ptr);
    if (c == EOF) {
        a = 0;
        flags |= FLAG_C;
        flags = (flags & ~(FLAG_Z | FLAG_N)) | FLAG_Z;
    } else {
        a = (uint8_t)c;
        flags &= ~FLAG_C;
        flags = (flags & ~(FLAG_Z | FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    }
    y = a;
}

int main(int argc, char* argv[]) {
    main_();
    return 0;
}

