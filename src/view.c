/* # Note to AI agents
 *
 * This is an in-progress translation from 6502 machine code to C.
 * The original 6502 assembly code is included as comments for reference.
 * Instructions which have been translated will be prefixed with //X.
 */

#include <stdint.h>

// #include "cpm65.inc"
// #include "driver.inc"
// #include "zif.inc"

// ; 6502 CPU register globals
uint8_t a, x, y, sp, flags;

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

// ; File structure

// FS_FCB = 0
// FS_BUFFERPTR = FS_FCB + FCB__SIZE   // FCB__SIZE = 0x24
// FS_BUFFER = FS_BUFFERPTR + 1
// FS__SIZE = FS_BUFFER + 128
#define FS__SIZE 165

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
static void sub_ca94a(void);
static void editor_loop(void);
static void ca741(void);
static void ca941(void);
static void ca93c(void);
static void ca684(void);
static void make_space_for_insertion(void);
static void get_line_length(void);
static void prompt_for_marker(void);
static void lookup_marker(void);
static void set_marker_to_here(void);
static void move_cursor_to_address(void);
static void sub_cae06(void);
static void SCREEN(void);

// ; SCREEN driver function codes
#define SCREEN_GETCHAR 7

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
//X printer_driver_ptr: .fill 2
uint16_t printer_driver_ptr;
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
//X l0076: .fill 1
uint8_t l0076;
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
//X input_buffer_ptr: .fill 2
uint16_t input_buffer_ptr;
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
uint16_t file_ptr;

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
//X highlight1_code: .fill 1
uint8_t highlight1_code;
//X highlight2_code: .fill 1
uint8_t highlight2_code;
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
uint8_t markers_array[12];
//X area_start_ptr: .fill 2
uint16_t area_start_ptr;
//X area_end_ptr: .fill 2
uint16_t area_end_ptr;
//X doc_ptr1: .fill 2
uint16_t doc_ptr1;
//X doc_ptr2: .fill 2
uint16_t doc_ptr2;
//X doc_ptr3: .fill 2
uint16_t doc_ptr3;
//X __end_pointer_array:

//X printer_driver_block:           .fill 0x100
uint8_t printer_driver_block[0x100];
//X input_buffer:                   .fill 0x45
uint8_t input_buffer[0x45];

//X current_line_buffer:            .fill 135
uint8_t current_line_buffer[135];
//X just_before_current_ruler_buffer: .fill 3 ; ??? something to do with rulers?
uint8_t just_before_current_ruler_buffer[3];
//X current_ruler_buffer:           .fill 133
uint8_t current_ruler_buffer[133];
//X output_buffer:                  .fill 132
uint8_t output_buffer[132];

//X header_text_maybe:              .fill 0x42
uint8_t header_text_maybe[0x42];
//X footer_text_maybe:              .fill 0x42
uint8_t footer_text_maybe[0x42];

//X filename_buffer:                .fill 0x14
uint8_t filename_buffer[0x14];
//X output_filename:                .fill 0x14
uint8_t output_filename[0x14];
//X printer_driver_name:            .fill 0x14
uint8_t printer_driver_name[0x14];

//X register_value_array:           .fill 26*2
uint8_t register_value_array[26*2];
//X register_value_l                = register_value_array + ('L'-'A')*2
#define register_value_l (register_value_array + ('L'-'A')*2)
//X register_value_p                = register_value_array + ('P'-'A')*2
#define register_value_p (register_value_array + ('P'-'A')*2)

//X line_lengths:                   .fill 32
uint8_t line_lengths[32];
//X input_filename:                 .fill 20
uint8_t input_filename[20];

//X input_file:                     .fill FS__SIZE
uint8_t input_file[FS__SIZE];
//X output_file:                    .fill FS__SIZE
uint8_t output_file[FS__SIZE];
static void main_(void) {
    // Pseudocode: Program entry point, initializes stack pointer and error handling mode

    // .text
    // .global main
    // main:
    //     ldx #0xff
    //     txs
    //     stx error_handling_mode
    //     jsr system_init
    //     jsr initialise_document
}
static void run_cli(void) {
    // Pseudocode: Clears screen, prints welcome with bytes free and document state, then drops into CLI

    // run_cli:
    //     jsr clear_screen
    //     ldx #1
    //     jsr print_x_words_of_help
    //     jsr print_inline_string
    //     .ascii "\r\rBytes free "
    //     .byte 0

    //     jsr compute_bytes_free
    //     jsr render_number_to_screen
    //     jsr bdos_print_newline
    //     jsr display_document_file_state
    //     bit file_edit_flags
    //     bvs c816d
    //     lda file_edit_flags
    //     ror
    //     bcc c816d
    //     jsr print_inline_string
    //     .ascii "Input file is "
    //     .byte 0

    //     lda input_file_empty_flag
    //     bne c8163
    //     jsr print_inline_string
    //     .ascii "not "
    //     .byte 0

    // c8163:
    //     jsr print_inline_string
    //     .ascii "empty\r"
    //     .byte 0

    // c816d:
    //     lda printer_driver_name
    //     beq c81b6
    //     jsr print_inline_string
    //     .ascii "Printer "
    //     .byte 0

    //     ldx #0
    // loop_c819a:
    //     lda printer_driver_name,x
    //     cmp #0x0d
    //     beq c81a7
    //     jsr bdos_print_char                                                        ; Write character
    //     inx
    //     bne loop_c819a
    // c81a7:
    //     lda microspacing_flag
    //     beq c81b3
    //     jsr print_inline_string
    //     .ascii " (m)"
    //     .byte 0

    // c81b3:
    //     jsr bdos_print_newline
    // c81b6:
    //     ldx #0
    //     ldy #0
    // c81ba:
    //     lda markers_array+1,x
    //     beq c81e7
    //     tya
    //     bne c81db
    //     stx l0083
    //     jsr print_inline_string
    //     .ascii "Marker(s) set "
    //     .byte 0

    //     ldx l0083
    //     ldy #1
    //     bne c81e0                                                         ; ALWAYS branch

    // c81db:
    //     lda #0x2c ; ','
    //     jsr screen_putchar
    // c81e0:
    //     txa
    //     lsr
    //     adc #0x31 ; '1'
    //     jsr screen_putchar
    // c81e7:
    //     inx
    //     inx
    //     cpx #0x0c
    //     bne c81ba
    //     tya
    //     beq c81f3
    //     jsr bdos_print_newline
    // c81f3:
    //     jsr bdos_print_newline
    // ; ***************************************************************************************
}
static void cli_loop(void) {
    // Pseudocode: Main CLI loop: prints => prompt, reads command, dispatches via jump table

    // cli_loop:
    //     jsr stop_printing
    //     ldx #0xff
    //     stx error_handling_mode
    //     txs
    //     inx                                                               ; X=0x00
    //     stx print_flags
    //     jsr print_inline_string
    //     .ascii "=>"
    //     .byte 0

    //     jsr readline
    //     lda #<input_buffer
    //     sta tmp0                    ; tmp0/1 used for parsing
    //     ldx #>input_buffer
    //     stx tmp1

    //     bcc input_line_not_escaped
    //     jmp run_editor

    // ; ***************************************************************************************
}
static void esc_key(void) {
    // Pseudocode: Saves edit buffer via ca93c and returns to CLI prompt

    // esc_key:
    //     jsr ca93c
    //     jmp run_cli
}
static void input_line_not_escaped(void) {
    // Pseudocode: Parses command input and dispatches through CLI jump table

    // input_line_not_escaped:
    //     jsr parse_command
    //     sty input_buffer_ptr+1
    //     bcs c8263
    //     cpy #(jumptable4_cli_end-jumptable4_cli)/2
    //     bcc c826e
    // c8263:
    //     jsr print_inline_string
    //     .ascii "Mistake"
    //     .byte 0xff

    // c826e:
    //     lda input_buffer_ptr+1
    //     ldy #2
    //     jsr call_through_jumptable
    //     jmp run_cli
}
static void bye_cmd(void) {
    // Pseudocode: Exits the program via BDOS exit system call

    // ; ***************************************************************************************
    // zproc bye_cmd
    //     ldy #BDOS_EXIT_PROGRAM
    //     jmp BDOS
    // zendproc
}
static void search_cmd(void) {
    // Pseudocode: Parses search string and marks area, searches for first match, enters editor

    // ; ***************************************************************************************
    // search_cmd:
    //     jsr sub_c8412
    //     beq c82e7
    //     jsr parse_marks_from_command
    //     jsr sanitise_area
    //     beq c82fa
    //     jsr sub_c8c7c
    //     jsr c8b7b
    //     bne c82fa
    //     jsr move_cursor_to_address
    //     jmp run_editor

    // ; ***************************************************************************************
}
static void change_cmd(void) {
    // Pseudocode: Replaces all occurrences of search string in document area, reports change count

    // change_cmd:
    //     jsr sub_c83f0
    //     bcs c82fa
    //     beq c82e7
    //     jsr c8b7b
    //     bne c82fa
    //     ldx #0
    //     stx ptr3
    //     stx ptr3+1
    // loop_c82b3:
    //     inc ptr3
    //     bne c82b9
    //     inc ptr3+1
    // c82b9:
    //     jsr move_cursor_to_address
    //     lda #0
    //     sta print_xpos
    //     jsr sub_c8a4f
    //     bcs c830d
    //     jsr c8b7b
    //     beq loop_c82b3
    //     ldx ptr3
    //     ldy ptr3+1
    //     jsr render_number_to_screen
    //     jsr print_inline_string
    //     .ascii " string(s) changed"
    //     .byte 0xff

    // c82e7:
    //     jsr print_inline_string
    //     .ascii "No target given"
    //     .byte 0xff

    // c82fa:
    //     jsr print_inline_string
    //     .ascii "No string found"
    //     .byte 0xff

    // c830d:
    //     jmp display_not_enough_memory
}
static void sub_c8310(void) {
    // Pseudocode: Helper that reads next input buffer char and compares against delimiter l007e

    // sub_c8310:
    //     iny
    //     lda input_buffer,y
    //     sta l0084
    //     cmp l007e
    //     beq return_2
    //     cmp #0x0d
    // return_2:
    //     rts
}
static void replace_cmd(void) {
    // Pseudocode: Interactive search and replace prompting for each match (Y)es/(O)K/(N)o

    // ; ***************************************************************************************
    // replace_cmd:
    //     jsr sub_c83f0
    //     beq c82e7
    //     jsr c8b7b
    //     bne c82fa
    //     jsr move_cursor_to_address
    //     jsr enter_editor_mode
    // c832d:
    //     jsr sub_c8361
    //     ldx #0x52 ; 'R'
    //     ldy #0x50 ; 'P'
    //     jsr draw_prompt_characters
    //     jsr flush_and_read_char
    //     bcs return_2
    //     and #0xdf
    //     ldx #0
    //     cmp #0x59 ; 'Y'
    //     beq c8349
    //     dex                                                               ; X=0xff
    //     cmp #0x4f ; 'O'
    //     bne c8356
    // c8349:
    //     stx print_xpos
    //     jsr sub_c8371
    //     jsr sub_c8a4f
    //     bcs c836b
    //     jsr sub_c8361
    // c8356:
    //     jsr c8b7b
    //     bne return_2
    //     jsr move_cursor_to_address
    //     jmp c832d
}
static void sub_c8361(void) {
    // Pseudocode: Saves edit state by clearing l006e, updating screen, and saving edit buffer

    // sub_c8361:
    //     lda #0
    //     sta l006e
    //     jsr sub_ca276
    //     jmp ca93c

    // c836b:
    //     jsr sub_ca94a
    //     jmp esc_key
}
static void sub_c8371(void) {
    // Pseudocode: Counts carriage returns between ptr2 and doc_ptr2 for line tracking during replace

    // sub_c8371:
    //     lda ptr2
    //     sta tmp8
    //     lda ptr2+1
    //     sta tmp9
    //     ldy #0
    //     ldx #0
    // c837d:
    //     lda tmp9
    //     cmp doc_ptr2+1
    //     bne c8389
    //     lda tmp8
    //     cmp doc_ptr2+0
    //     beq c8398
    // c8389:
    //     lda (tmp8),y
    //     cmp #0x0d
    //     bne c8390
    //     inx
    // c8390:
    //     inc tmp8
    //     bne c837d
    //     inc tmp9
    //     bne c837d
    // c8398:
    //     inc l0074
    //     txa
    //     beq return_3
    //     jmp ca741
}
static void expand_escaped_string(void) {
    // Pseudocode: Expands ^X escape sequences (^T=tab, ^C=CR, ^S=stop, etc.) in strings

    // ; ***************************************************************************************
    // expand_escaped_string:
    //     stx l0083
    //     dey
    // c83a3:
    //     jsr sub_c8310
    //     beq c83da
    //     cmp #0x5e ; '^'
    //     bne c83ca
    //     jsr sub_c8310
    //     beq c83da
    //     jsr to_uppercase
    //     sta l0082
    //     ldx #0xfe
    // loop_c83b8:
    //     inx
    //     inx
    //     lda escaped_char_table,x
    //     bmi c83c8
    //     cmp l0082
    //     bne loop_c83b8
    //     lda l83e0,x
    //     bne c83ca
    // c83c8:
    //     lda l0084
    // c83ca:
    //     ldx l007a
    //     bne c83d1
    //     jsr sub_c8c5f
    // c83d1:
    //     ldx l0083
    //     sta header_text_maybe,x
    //     inc l0083
    //     bne c83a3
    // c83da:
    //     ldx l0083
    //     sty input_buffer_ptr
    // return_3:
    //     rts
}
static void sub_c83f0(void) {
    // Pseudocode: Parses search and replace arguments: expands strings and parses marker range

    // escaped_char_table:
    //     .ascii "?"
    // l83e0:
    //     .byte 1
    //     .ascii "T"
    //     .byte 9
    //     .ascii "C"
    //     .byte 0x0d
    //     .ascii "S"
    //     .byte 2
    //     .ascii "L"
    //     .byte 0x0b
    //     .ascii "Z"
    //     .byte 0x1a
    //     .ascii "-"
    //     .byte 0x1c
    //     .ascii "*"
    //     .byte 0x1d
    //     .byte 0xff

    // sub_c83f0:
    //     jsr sub_c8412
    //     beq c8410
    //     jsr sub_c8e33
    //     beq c8402
    //     ldy input_buffer_ptr
    //     iny
    //     jsr expand_escaped_string
    //     stx l004a
    // c8402:
    //     jsr parse_marks_from_command
    //     jsr sanitise_area
    //     sec
    //     beq return_4
    //     jsr sub_c8c7c
    //     lda #1
    // c8410:
    //     clc
    // return_4:
    //     rts
}
static void sub_c8412(void) {
    // Pseudocode: Parses initial search string from command input with escape expansion

    // sub_c8412:
    //     ldx #0
    //     stx l007a
    //     stx l004a
    //     jsr sub_c8e33
    //     beq return_5
    //     ldx #0
    //     jsr expand_escaped_string
    //     stx l007a
    //     cpx #0
    // return_5:
    //     rts
}
static void screen_cmd(void) {
    // Pseudocode: Jumps to print_to_screen for on-screen document preview

    // ; ***************************************************************************************
    // screen_cmd:
    //     jmp print_to_screen
}
static void sheets_cmd(void) {
    // Pseudocode: Prints document to printer then displays newline and returns to CLI

    // ; ***************************************************************************************
    // sheets_cmd:
    //     lda #0xc0
    //     jsr start_printing
    //     jsr print_document
    //     jsr stop_printing
    //     jsr bdos_print_newline
    //     jmp cli_loop
}
static void print_cmd(void) {
    // Pseudocode: Sets print flags and falls through to print_to_screen

    // ; ***************************************************************************************
    // print_cmd:
    //     lda #0x80
    //     jsr start_printing
    // ; ***************************************************************************************

    // MULTIPLE ENTRY POINTS: print_cmd, print_to_screen
    print_to_screen();
}
static void print_to_screen(void) {
    // Pseudocode: Prints document for screen preview, returns to CLI when done

    // print_to_screen:
    //     jsr print_document
    //     jmp cli_loop

    // MULTIPLE ENTRY POINTS: print_cmd, print_to_screen
}
static void stop_printing(void) {
    // Pseudocode: Stops active printing by clearing print flags and calling printer driver cleanup

    // ; ***************************************************************************************
    // stop_printing:
    //     lda print_flags
    //     bpl c8459
    //     rol print_flags
    //     clc
    //     ror print_flags
    //     lda #6
    //     jsr call_printer_driver
    // c8459:
    //     rts
}
static void start_printing(void) {
    // Pseudocode: Initializes printer driver and starts printing with given flags

    // start_printing:
    //     jsr print_inline_string
    //     .ascii "Sorry, can't print yet\r"
    //     .byte 0
    //     jmp cli_loop

    //     sta print_flags
    //     jsr prepare_printer_driver
    //     lda #3
    //     jmp call_printer_driver

    // MULTIPLE ENTRY POINTS: start_printing has stub and real code paths
}
static void edit_cmd(void) {
    // Pseudocode: Opens input and output files, initializes document, reads first input chunk

    // ; ***************************************************************************************
    // edit_cmd:
    //     jsr check_not_continuous_editing

    //     jsr parse_filename_from_command
    //     jsr set_document_name_to_filename_buffer
    //     ldx #<input_file
    //     ldy #>input_file
    //     jsr select_file
    //     jsr open_input_file

    //     jsr parse_filename_from_command
    //     ldx #<output_file
    //     ldy #>output_file
    //     jsr select_file
    //     jsr open_output_file

    //     ldx #0
    //     stx input_file_empty_flag
    // loop_c8490:
    //     lda filename_buffer,x
    //     zif eq
    //         lda #0x0d
    //     zendif
    //     sta output_filename,x
    //     inx
    //     cmp #0x0d
    //     bne loop_c8490
    //     jsr initialise_document
    //     jsr read_first_chunk_from_input_file
    //     beq c84a8
    //     lda #1
    //     sta file_edit_flags
    //     rts

    // c84a8:
    //     jsr close_input_output_files
    // c84ab:
    //     jmp cli_loop
}
static void more_cmd(void) {
    // Pseudocode: Appends more text from input file into document at current cursor position

    // ; ***************************************************************************************
    // more_cmd:
    //     jsr check_continuous_editing
    //     jsr parse_marks_from_command
    //     lda area_start_ptr
    //     ldy area_start_ptr+1
    //     jsr move_cursor_to_address

    //     ldx #<output_file
    //     ldy #>output_file
    //     jsr select_file
    //     jsr write_area_to_file
    //     bne c84ab

    //     ldy #0
    //     ldx l003a
    // loop_c84c4:
    //     lda (current_ruler_ptr),y
    //     sta current_ruler_buffer,y
    //     iny
    //     dex
    //     bne loop_c84c4
    //     lda #0x0d
    //     sta current_ruler_buffer,y
    //     jsr sub_c89d3
    //     jsr move_cursor_to_top_of_document
    //     jsr check_for_at_least_150_bytes_free
    //     lda input_file_empty_flag
    //     bne c84e8
    //     lda top
    //     ldy top+1
    //     jsr read_next_chunk_from_input_file
    //     beq c84ab
    // c84e8:
    //     jmp cb05a
}
static void finish_cmd(void) {
    // Pseudocode: Writes remaining document content to output file in chunks

    // ; ***************************************************************************************
    // finish_cmd:
    //     jsr check_continuous_editing
    // loop_c84ee:
    //     jsr reset_area_to_entire_document
    //     jsr sanitise_area

    //     ldx #<output_file
    //     ldy #>output_file
    //     jsr select_file

    //     jsr write_area_to_file
    //     bne c84ab
    //     lda #0
    //     jsr put_byte_to_file                ; write terminator
    //     jsr sub_c89d3
    //     jsr move_cursor_to_top_of_document
    //     jsr cb05a
    //     lda input_file_empty_flag
    //     bne close_input_output_files
    //     jsr read_first_chunk_from_input_file
    //     beq c84ab
    //     bne loop_c84ee                                                    ; ALWAYS branch
}
static void quit_cmd(void) {
    // Pseudocode: Checks continuous editing then falls through to close files

    // ; ***************************************************************************************
    // quit_cmd:
    //     jsr check_continuous_editing

    // MULTIPLE ENTRY POINTS: quit_cmd, close_input_output_files
    close_input_output_files();
}
static void close_input_output_files(void) {
    // Pseudocode: Closes output file, resets editing flags, returns to CLI

    // close_input_output_files:
    //     lda #0
    //     sta input_file_empty_flag
    //     sta file_edit_flags

    //     ldx #<output_file
    //     ldy #>output_file
    //     jsr select_file
    //     jsr close_file
    //     jmp cli_loop

    // MULTIPLE ENTRY POINTS: quit_cmd, close_input_output_files
}
static void save_cmd_write_cmd(void) {
    // Pseudocode: Saves document area to output file with optional filename

    // ; ***************************************************************************************
    // save_cmd:
    // write_cmd:
    //     jsr parse_optional_filename_from_command
    //     zif eq
    //         bit file_edit_flags
    //         zif vc
    //             jmp bad_filename_error
    //         zendif

    //         ldx #0
    //         zrepeat
    //             lda input_filename,x
    //             sta filename_buffer,x
    //             inx
    //             cmp #0x0d
    //         zuntil eq
    //     zendif
    //     jsr parse_marks_from_command
    //     jsr sanitise_area
    //     beq return_6

    //     ldx #<input_buffer
    //     ldy #>input_buffer
    //     jsr select_file

    //     jsr open_output_file
    //     jsr write_area_to_file
    //     lda #0
    //     jsr put_byte_to_file

    //     jsr close_file
    //     jmp cli_loop

    // MULTIPLE ENTRY POINTS: save_cmd, write_cmd
}
static void check_for_at_least_150_bytes_free(void) {
    // Pseudocode: Checks if at least 150 bytes of memory are available

    // check_for_at_least_150_bytes_free:
    //     jsr compute_bytes_free
    //     tya
    //     bne return_6
    //     cpx #0x96
    //     bcs return_6

    // MULTIPLE ENTRY POINTS: check_for_at_least_150_bytes_free, display_not_enough_memory
    display_not_enough_memory();
}
static void display_not_enough_memory(void) {
    // Pseudocode: Displays Not enough memory error and stops printing

    // display_not_enough_memory:
    //     jsr stop_printing
    //     jsr print_inline_string
    //     .ascii "Not enough memory"
    //     .byte 0xff

    // return_6:
    //     rts

    // MULTIPLE ENTRY POINTS: check_for_at_least_150_bytes_free, display_not_enough_memory
}
static void load_cmd(void) {
    // Pseudocode: Loads a file, initializes document, clears markers, moves cursor to top

    // ; ***************************************************************************************
    // load_cmd:
    //     jsr check_not_continuous_editing
    //     jsr parse_filename_from_command
    //     jsr initialise_document
    //     jsr reset_area_to_entire_document
    //     jsr 1f
    //     jsr reset_document_name_after_load
    //     jsr clear_cmd
    //     jmp move_cursor_to_top_of_document

    // MULTIPLE ENTRY POINTS: load_cmd jumps into read_cmd at label 1:
}
static void read_cmd(void) {
    // Pseudocode: Reads file contents into document at current area_start position

    // read_cmd:
    //     jsr parse_filename_from_command
    //     jsr parse_marks_from_command
    // 1:
    //     jsr check_for_at_least_150_bytes_free

    //     ldx #<input_buffer
    //     ldy #>input_buffer
    //     jsr select_file

    //     jsr open_input_file

    //     lda area_start_ptr
    //     ldy area_start_ptr+1
    //     sta tmp4
    //     sty tmp5
    //     jsr move_cursor_to_address
    //     lda tmp4
    //     ldy tmp5
    //     jsr compute_required_space_for_insertion
    //     jsr make_space_for_insertion

    //     jsr read_block_from_file
    //     beq c8584
    //     bcs c8598
    // c8584:
    //     jsr print_inline_string
    //     .ascii "Not all read in\r"
    //     .byte 0

    // c8598:
    //     lda tmp0
    //     sta tmp4
    //     lda tmp1
    //     sta tmp5
    //     lda ptr5
    //     sec
    //     sbc tmp0
    //     sta tmp6
    //     lda ptr5+1
    //     sbc tmp1
    //     sta tmp7
    //     jsr adjust_pointers
    //     jmp cli_loop

    // MULTIPLE ENTRY POINTS: load_cmd jumps to label 1: within read_cmd
}
static void mode_cmd(void) {
    // Pseudocode: Displays Bad mode error message and returns to CLI

    // ; ***************************************************************************************
    // mode_cmd:
    //     jsr print_inline_string
    //     .ascii "Bad mode"
    //     .byte 0xff
}
static void microspace_cmd(void) {
    // Pseudocode: Configures microspacing by querying printer driver

    // ; ***************************************************************************************
    // microspace_cmd:
    //     jsr prepare_printer_driver
    //     jsr parse_integer_from_command
    //     php
    //     ldx #0x0a
    //     plp
    //     beq c8608
    //     ldx tmp8
    //     beq return_7
    // c8608:
    //     ldy #0
    //     lda #0x0c
    //     jsr call_printer_driver
    //     tya
    //     and #1
    //     beq c8617
    //     stx microspacing_flag
    // return_7:
    //     rts

    // c8617:
    //     jsr print_inline_string
    //     .ascii "Driver does not support microspacing"
    //     .byte 0xff
}
static void setup_cmd(void) {
    // Pseudocode: Parses flag letters and sets format_mode_flag, justifying_flag, insert_mode_flag

    // ; ***************************************************************************************
    // setup_cmd:
    //     ldx #1
    //     stx tmp6
    //     dex                                                               ; X=0x00
    //     stx tmp8
    //     dex                                                               ; X=0xff
    //     stx tmp7
    // c8649:
    //     jsr sub_c8e33
    //     beq c8672
    //     and #0xdf
    //     ldx #0
    // loop_c8652:
    //     cmp c867d,x
    //     beq c8669
    //     inx
    //     ldy c867d,x
    //     bne loop_c8652
    //     jsr print_inline_string
    //     .ascii "Bad flag"
    //     .byte 0xff

    // c8669:
    //     lda c8681,x
    //     sta tmp6,x
    //     inc input_buffer_ptr
    //     bne c8649
    // c8672:
    //     ldx #2
    // loop_c8674:
    //     lda tmp6,x
    //     sta format_mode_flag,x
    //     dex
    //     bpl loop_c8674
    //     bmi c869b                                                         ; ALWAYS branch

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
    //     beq c869b
    //     lda tmp8
    //     cmp #0x1b
    //     bne c8699
    //     jsr print_inline_string
    //     .ascii "Frump!"
    //     .byte 0xff

    // c8699:
    //     sta current_tab_key
    // c869b:
    //     jmp cli_loop
}
static void count_cmd(void) {
    // Pseudocode: Counts words in document area handling command prefixes and punctuation

    // ; ***************************************************************************************
    // count_cmd:
    //     jsr parse_marks_from_command
    //     jsr sanitise_area
    //     beq c869b
    //     lda area_start_ptr
    //     sta tmp0
    //     lda area_start_ptr+1
    //     sta tmp1
    //     lda #0
    //     sta tmp8
    //     sta tmp9
    //     sta l0083
    //     sta l0082
    // c86b8:
    //     ldy #0
    //     jsr deref_and_check_for_command_prefix
    //     bne c86ea
    //     ldx #0
    //     iny
    // loop_c86c2:
    //     lda (tmp0),y
    //     iny
    //     cmp l8747,x
    //     bne c86d1
    //     lda (tmp0),y
    //     cmp l8748,x
    //     beq c86df
    // c86d1:
    //     lda l8749,x
    //     beq c86db
    //     dey
    //     inx
    //     inx
    //     bne loop_c86c2
    // c86db:
    //     lda #0x80
    //     bne c86ff                                                         ; ALWAYS branch

    // c86df:
    //     lda tmp0
    //     clc
    //     adc #3
    //     sta tmp0
    //     bcs c871d
    //     bcc c871f                                                         ; ALWAYS branch

    // c86ea:
    //     ldy #0
    //     jsr draw_char
    //     and #0x7f
    //     ldx #0
    //     ldy l0082
    //     bmi c870d
    //     cmp #0x0d
    //     beq c8703
    //     cmp #0x20 ; ' '
    //     beq c8703
    // c86ff:
    //     inc l0083
    //     bne c8715
    // c8703:
    //     ldy l0083
    //     beq c870d
    //     inc tmp8
    //     bne c870d
    //     inc tmp9
    // c870d:
    //     stx l0083
    //     cmp #0x0d
    //     bne c8715
    //     stx l0082
    // c8715:
    //     ora l0082
    //     sta l0082
    //     inc tmp0
    //     bne c871f
    // c871d:
    //     inc tmp1
    // c871f:
    //     ldy tmp1
    //     cpy area_end_ptr+1
    //     bne c86b8
    //     ldy tmp0
    //     cpy area_end_ptr
    //     bne c86b8
    //     ldx tmp8
    //     ldy tmp9
    //     jsr render_number_to_screen
    //     jsr print_inline_string
    //     .ascii " word(s) counted."
    //     .byte 0xff
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
    //     jsr sanitise_area
    //     beq c878b
    //     lda area_start_ptr
    //     ldy area_start_ptr+1
    //     jsr move_cursor_to_address
    //     jsr sub_caf5f
    //     lda #0x10
    //     jsr wipe_buffer
    //     lda current_edit_line_ptr
    //     sta current_format_line_ptr
    //     lda current_edit_line_ptr+1
    //     sta current_format_line_ptr+1
    // c876d:
    //     jsr sub_c9977
    //     bvs c8791
    //     bcs c8787
    //     lda #0x2e ; '.'
    //     jsr bdos_print_char
    //     lda current_line_ptr
    //     ldy current_line_ptr+1
    //     cpy area_end_ptr+1
    //     bcc c876d
    //     bne c8787
    //     cmp area_end_ptr
    //     bcc c876d
    // c8787:
    //     lda #0xff
    //     sta l0012
    // c878b:
    //     jsr bdos_print_newline
    //     jmp cli_loop

    // c8791:
    //     jsr bdos_print_newline
    //     jmp display_not_enough_memory
}
static void new_cmd(void) {
    // Pseudocode: Creates a new empty document after checking continuous editing state

    // ; ***************************************************************************************
    // new_cmd:
    //     jsr check_not_continuous_editing
    //     jmp initialise_document
}
static void fold_cmd(void) {
    // Pseudocode: Toggles folding on/off and displays current folding status

    // ; ***************************************************************************************
    // fold_cmd:
    //     jsr sub_c8e33
    //     beq c87b4
    //     lda #0
    //     ldx #5
    //     jsr sub_c976c
    //     bcs c87b4
    //     ldx #0x80
    //     tay
    //     beq c87b2
    //     ldx #0
    // c87b2:
    //     stx folding_flag
    // c87b4:
    //     jsr print_inline_string
    //     .ascii "Folding "
    //     .byte 0

    //     lda folding_flag
    //     bpl c87cb
    //     jsr print_inline_string
    //     .ascii "off"
    //     .byte 0xff

    // c87cb:
    //     jsr print_inline_string
    //     .ascii "on"
    //     .byte 0xff

    // c87d1:
    //     jsr print_inline_string
    //     .ascii "Bad file"
    //     .byte 0xff
}
static void printer_cmd(void) {
    // Pseudocode: Redirects to print_cmd (printer driver loading code is disabled with #if 0)

    // ; ***************************************************************************************
    // printer_cmd:
    //     jmp print_cmd
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
    //     sta current_format_line_ptr
    //     lda #>(input_buffer)
    //     sta current_format_line_ptr+1
    //     jsr sub_c8e33
    //     beq return_8
    //     jmp ca6fe
}
static void file_not_found_error(void) {
    // Pseudocode: Displays File not found error and returns to CLI

    // ; ***************************************************************************************
    // file_not_found_error:
    //     jsr stop_printing
    //     jsr print_inline_string
    //     .ascii "File not found\r"
    //     .byte 0
    //     jmp cli_loop
}
static void name_cmd(void) {
    // Pseudocode: Sets document name from optional filename argument

    // ; ***************************************************************************************
    // name_cmd:
    //     jsr check_not_continuous_editing
    //     jsr parse_optional_filename_from_command
    //     php
    //     lda #0
    //     sta file_edit_flags
    //     plp
    //     beq return_9

    // MULTIPLE ENTRY POINTS: name_cmd, reset_document_name_after_load
    reset_document_name_after_load();
}
static void reset_document_name_after_load(void) {
    // Pseudocode: Sets file_edit_flags to indicate a document is loaded

    // reset_document_name_after_load:
    //     lda #0x40 ; '@'
    //     sta file_edit_flags

    // MULTIPLE ENTRY POINTS: name_cmd, reset_document_name_after_load
}
static void set_document_name_to_filename_buffer(void) {
    // Pseudocode: Copies filename buffer to input filename buffer

    // set_document_name_to_filename_buffer:
    //     ldx #0
    // loop_c88fa:
    //     lda filename_buffer,x
    //     sta input_filename,x
    //     inx
    //     cmp #0x21
    //     bge loop_c88fa
    // return_9:
    //     lda #0x0d
    //     sta input_filename-1, x
    //     rts

    // MULTIPLE ENTRY POINTS: also called directly from edit_cmd
}
static void file_error(void) {
    // Pseudocode: Displays File error and returns to CLI

    // ; ***************************************************************************************
    // zproc file_error
    //     jsr print_inline_string
    //     .ascii "File error"
    //     .byte 0
    //     jmp cli_loop
    // zendproc
}
static void zero_terminate_filename_buffer(void) {
    // Pseudocode: Replaces CR at end of filename with null (0) byte

    // zproc zero_terminate_filename_buffer
    //     ldx #0
    //     lda #0x0d
    //     zloop
    //         cmp filename_buffer, x
    //         zbreakif eq
    //         inx
    //     zendloop

    //     lda #0
    //     sta filename_buffer, x
    //     rts
    // zendproc
}
static void sanitise_area(void) {
    // Pseudocode: Ensures area_start <= area_end by swapping if needed, returns area size

    // ; ***************************************************************************************
    // sanitise_area:
    //     lda area_start_ptr
    //     ldx area_start_ptr+1
    //     cpx area_end_ptr+1
    //     bcc c8977
    //     bne c896b
    //     cmp area_end_ptr
    //     bcc c8977
    // c896b:
    //     ldy area_end_ptr
    //     sty area_start_ptr
    //     ldy area_end_ptr+1
    //     sty area_start_ptr+1
    //     stx area_end_ptr+1
    //     sta area_end_ptr
    // c8977:
    //     lda area_end_ptr
    //     sec
    //     sbc area_start_ptr
    //     sta tmp6
    //     lda area_end_ptr+1
    //     sbc area_start_ptr+1
    //     sta tmp7
    //     bne return_10
    //     lda tmp6
    // return_10:
    //     rts
}
static void parse_marks_from_command(void) {
    // Pseudocode: Parses up to two markers from command to set area_start and area_end

    // ; ***************************************************************************************
    // parse_marks_from_command:
    //     jsr reset_area_to_entire_document
    //     jsr parse_mark_from_command
    //     beq return_11
    //     sta area_start_ptr
    //     sty area_start_ptr+1
    //     jsr parse_mark_from_command
    //     beq return_11
    //     sta area_end_ptr
    //     sty area_end_ptr+1
    // return_11:
    //     rts
}
static void parse_mark_from_command(void) {
    // Pseudocode: Parses a single marker number and looks up its stored address

    // ; ***************************************************************************************
    // parse_mark_from_command:
    //     jsr sub_c8e33
    //     beq return_12
    //     iny
    //     sty input_buffer_ptr
    //     jsr lookup_marker
    //     bcs c89b3
    //     beq c89c1
    //     lda markers_array,x
    //     ldy markers_array+1,x
    // return_12:
    //     rts

    // c89b3:
    //     jsr print_inline_string
    //     .ascii "Bad marker"
    //     .byte 0xff

    // c89c1:
    //     jsr print_inline_string
    //     .ascii "Marker not set"
    //     .byte 0xff
}
static void sub_c89d3(void) {
    // Pseudocode: Adjusts document pointers using area_start and calls cac78

    // sub_c89d3:
    //     lda area_start_ptr
    //     sta tmp4
    //     lda area_start_ptr+1
    //     sta tmp5
    //     jsr adjust_pointers
    //     lda tmp4
    //     ldy tmp5
    //     jmp cac78
}
static void display_document_file_state(void) {
    // Pseudocode: Displays editing state showing filenames or No File

    // display_document_file_state:
    //     jsr stop_printing
    //     jsr print_inline_string
    //     .ascii "Editing "
    //     .byte 0

    //     lda file_edit_flags
    //     beq c8a21
    //     ldy #0
    // loop_c89fa:
    //     lda input_filename,y
    //     cmp #0x0d
    //     beq c8a07
    //     jsr bdos_print_char                                                        ; Write character
    //     iny
    //     bne loop_c89fa
    // c8a07:
    //     bit file_edit_flags
    //     bvs c8a19
    //     jsr print_inline_string
    //     .ascii " to "
    //     .byte 0

    //     ldy #0
    // loop_c8a15:
    //     lda output_filename,y
    //     iny
    // c8a19:
    //     jsr bdos_print_char                                                        ; Write character
    //     cmp #0x0d
    //     bne loop_c8a15
    //     rts

    // c8a21:
    //     jsr print_inline_string
    //     .ascii "No File\r"
    //     .byte 0

    //     rts
}
static void sub_c8a4f(void) {
    // Pseudocode: Performs case-aware string replacement in document at current position

    // sub_c8a4f:
    //     lda #0
    //     sta l0082
    //     sta l0081
    //     ldy #0x14
    //     ldx l007a
    //     bne c8a87
    // c8a5b:
    //     lda header_text_maybe,x
    //     cmp #1
    //     bne c8a6c
    //     lda l0081
    //     cmp l0049
    //     bcs c8a86
    //     inc l0081
    //     bne c8a84
    // c8a6c:
    //     cmp #0x20 ; ' '
    //     bne c8a84
    //     cpy l0048
    //     bcs c8a84
    // loop_c8a74:
    //     lda output_buffer,y
    //     php
    //     iny
    //     plp
    //     beq c8a86
    //     inc l0082
    //     cpy l0048
    //     bcc loop_c8a74
    //     dec l0082
    // c8a84:
    //     inc l0082
    // c8a86:
    //     inx
    // c8a87:
    //     cpx l004a
    //     bcc c8a5b
    //     lda doc_ptr2+0
    //     sec
    //     sbc ptr2
    //     sta input_buffer_ptr+1
    //     lda doc_ptr2+1
    //     sbc ptr2+1
    //     sta l0081
    //     ldx l0082
    //     tay
    //     bne c8aa3
    //     cpx input_buffer_ptr+1
    //     bcc c8aa3
    //     ldx input_buffer_ptr+1
    // c8aa3:
    //     txa
    //     clc
    //     adc ptr2
    //     sta tmp4
    //     lda ptr2+1
    //     adc #0
    //     sta tmp5
    //     lda l0082
    //     sec
    //     sbc input_buffer_ptr+1
    //     sta tmp6
    //     lda #0
    //     sbc l0081
    //     sta tmp7
    //     bmi c8aca
    //     ora tmp6
    //     beq c8ada
    //     sta tmp6
    //     jsr make_space_for_insertion
    //     bcc c8ada
    //     rts

    // c8aca:
    //     lda #0
    //     sec
    //     sbc tmp6
    //     sta tmp6
    //     lda #0
    //     sbc tmp7
    //     sta tmp7
    //     jsr adjust_pointers
    // c8ada:
    //     ldy #0
    //     sty l0081
    //     bit print_xpos
    //     bmi c8b11
    //     ldx input_buffer_ptr+1
    // loop_c8ae4:
    //     lda (ptr2),y
    //     iny
    //     jsr is_uppercase
    //     bcc c8af3
    //     ror print_xpos
    //     dex
    //     bne loop_c8ae4
    //     beq c8b11                                                         ; ALWAYS branch

    // c8af3:
    //     pha
    //     lda #0
    //     sta print_xpos
    //     pla
    //     and #0x20 ; ' '
    //     bne c8b11
    //     inc l0081
    //     dex
    //     beq c8b0d
    //     lda (ptr2),y
    //     jsr is_uppercase
    //     bcs c8b11
    //     and #0x20 ; ' '
    //     bne c8b11
    // c8b0d:
    //     dec l0081
    //     dec l0081
    // c8b11:
    //     ldx #0
    //     stx l0082
    //     stx l0083
    //     ldx #0x14
    //     stx input_buffer_ptr+1
    //     ldx l007a
    //     bne c8b6b
    // c8b1f:
    //     lda header_text_maybe,x
    //     stx l0084
    //     cmp #0x20 ; ' '
    //     bne c8b38
    //     ldy input_buffer_ptr+1
    //     cpy l0048
    //     bcs c8b47
    //     inc input_buffer_ptr+1
    //     lda output_buffer,y
    //     beq c8b6a
    //     dex
    //     bcc c8b47                                                         ; ALWAYS branch

    // c8b38:
    //     cmp #1
    //     bne c8b47
    //     ldy l0082
    //     cpy l0049
    //     bcs c8b6a
    //     lda output_buffer,y
    //     inc l0082
    // c8b47:
    //     cmp #2
    //     bne c8b4d
    //     lda #0x20 ; ' '
    // c8b4d:
    //     bit folding_flag
    //     bmi c8b64
    //     ldy print_xpos
    //     bne c8b64
    //     jsr is_uppercase
    //     bcs c8b64
    //     ora #0x20 ; ' '
    //     ldy l0081
    //     beq c8b64
    //     dec l0081
    //     and #0xdf
    // c8b64:
    //     ldy l0083
    //     sta (ptr2),y
    //     inc l0083
    // c8b6a:
    //     inx
    // c8b6b:
    //     cpx l004a
    //     bcc c8b1f
    //     lda ptr2
    //     ldy ptr2+1
    //     jsr cac78
    //     clc
    //     rts
}
static void c8b78(void) {
    // Pseudocode: Returns 0xff as a failure indicator

    // c8b78:
    //     lda #0xff
    //     rts
}
static void c8b7b(void) {
    // Pseudocode: Searches for next pattern match in document, skipping command prefixes

    // c8b7b:
    //     lda l007a
    //     beq c8b78
    //     lda #0x14
    //     sta l0048
    //     ldx #0
    //     stx l0049
    //     stx l0081
    //     lda doc_ptr2+0
    //     sta tmp8
    //     lda doc_ptr2+1
    //     sta tmp9
    // c8b91:
    //     lda tmp9
    //     cmp doc_ptr3+1
    //     bcc c8b9f
    //     bne c8b78
    //     lda tmp8
    //     cmp doc_ptr3+0
    //     bcs c8b78
    // c8b9f:
    //     ldy #0
    //     lda (tmp8),y
    //     jsr check_for_command_prefix
    //     bne c8bb7
    //     lda tmp8
    //     clc
    //     adc #3
    //     sta doc_ptr2+0
    //     lda tmp9
    //     sta doc_ptr2+1
    //     bcc c8b7b
    //     bcs c8bdf                                                         ; ALWAYS branch

    // c8bb7:
    //     jsr sub_c8c5f
    //     sta l0083
    // c8bbc:
    //     iny
    //     lda (tmp8),y
    //     beq c8bdb
    //     jsr check_for_command_prefix
    //     beq c8bdb
    //     lda header_text_maybe,x
    //     cmp #0x20 ; ' '
    //     beq c8bf7
    //     cmp #1
    //     beq c8be3
    //     cmp #2
    //     bne c8bd7
    //     lda #0x20 ; ' '
    // c8bd7:
    //     cmp l0083
    //     beq c8c33
    // c8bdb:
    //     inc doc_ptr2+0
    //     bne c8b7b
    // c8bdf:
    //     inc doc_ptr2+1
    //     bne c8b7b
    // c8be3:
    //     lda l0083
    //     stx l0084
    //     ldx l0049
    //     cpx #0x14
    //     bcs c8bf2
    //     sta output_buffer,x
    //     inc l0049
    // c8bf2:
    //     ldx l0084
    //     jmp c8c33

    // c8bf7:
    //     stx l0084
    //     lda l0083
    //     cmp #0x20 ; ' '
    //     beq c8c23
    //     cmp #9
    //     beq c8c23
    //     cmp #0x0b
    //     beq c8c23
    //     cmp #0x1a
    //     beq c8c23
    //     cmp #0x0d
    //     beq c8c23
    //     lda l0081
    //     beq c8bdb
    //     jsr sub_c8c51
    //     lda #0
    //     sta l0081
    //     ldx l0084
    //     inx
    //     cpx l007a
    //     bcc c8bbc
    //     bcs c8c3e                                                         ; ALWAYS branch

    // c8c23:
    //     jsr sub_c8c53
    //     ldx l0084
    //     sta l0081
    // loop_c8c2a:
    //     inc tmp8
    //     bne c8c30
    //     inc tmp9
    // c8c30:
    //     jmp c8b91

    // c8c33:
    //     inx
    //     cpx l007a
    //     bcc loop_c8c2a
    //     inc tmp8
    //     bne c8c3e
    //     inc tmp9
    // c8c3e:
    //     lda doc_ptr2+0
    //     ldy doc_ptr2+1
    //     ldx tmp8
    //     stx doc_ptr2+0
    //     ldx tmp9
    //     stx doc_ptr2+1
    //     sta ptr2
    //     sty ptr2+1
    //     ldx #0
    //     rts
}
static void sub_c8c51_sub_c8c53(void) {
    // Pseudocode: Writes a byte (or zero) to output_buffer at current position

    // sub_c8c51:
    //     lda #0
    // sub_c8c53:
    //     ldx l0048
    //     cpx #0x84
    //     bcs return_13
    //     sta output_buffer,x
    //     inc l0048
    // return_13:
    //     rts

    // MULTIPLE ENTRY POINTS: sub_c8c51, sub_c8c53
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
    // Pseudocode: Checks if character is uppercase A-Z, returns carry if not

    // ; ***************************************************************************************
    // is_uppercase:
    //     cmp #0x41 ; 'A'
    //     bcc c8c7a
    //     cmp #0x5b ; '['
    //     bcc return_15
    //     cmp #0x61 ; 'a'
    //     bcc c8c7a
    //     cmp #0x7b ; '{'
    // return_15:
    //     rts

    // c8c7a:
    //     sec
    //     rts
}
static void sub_c8c7c(void) {
    // Pseudocode: Copies area_start to doc_ptr2 and area_end to doc_ptr3

    // sub_c8c7c:
    //     lda area_start_ptr
    //     sta doc_ptr2+0
    //     lda area_start_ptr+1
    //     sta doc_ptr2+1
    //     lda area_end_ptr
    //     sta doc_ptr3+0
    //     lda area_end_ptr+1
    //     sta doc_ptr3+1
    //     rts
}
static void read_block_from_file(void) {
    // Pseudocode: Reads bytes from input file into memory filtering out invalid control codes

    // read_block_from_file:
    //     lda #0
    //     sta l0083
    //     sta l0084
    // c8c95:
    //     jsr get_byte_from_file
    //     beq c8cf2
    //     ldy #0
    //     cmp #0x7f
    //     bcc c8caf
    //     ldx l0084
    //     bne c8c95
    //     jsr check_for_command_prefix
    //     bne c8c95
    //     ldx #0xfd
    //     stx l0083
    // c8caf:
    //     cmp #0x20 ; ' '
    //     bcs c8cc8
    //     jsr check_for_control_code
    //     beq c8cc8
    //     cmp #0x1a
    //     beq c8cc8
    //     cmp #0x0d
    //     beq c8cc8
    //     cmp #0x0b
    //     beq c8cc8
    //     cmp #9
    //     bne c8c95
    // c8cc8:
    //     ldx #1
    //     cmp #0x0d
    //     beq c8cdb
    //     dex                                                               ; X=0x00
    //     ldy l0083
    //     cpy #0x84
    //     bne c8cdb
    //     pha
    //     jsr write_cr_to_memory
    //     pla
    //     inx
    // c8cdb:
    //     inc l0083
    //     jsr write_byte_to_memory
    //     txa
    //     beq c8c95
    //     lda tmp1
    //     cmp l0081
    //     bcc c8c95
    //     bne c8cf1
    //     lda tmp0
    //     cmp input_buffer_ptr+1
    //     bcc c8c95
    // c8cf1:
    //     clc
    // c8cf2:
    //     php
    //     lda l0084
    //     beq c8cfa
    //     jsr write_cr_to_memory
    // c8cfa:
    //     plp
    //     lda l0082
    //     rts
}
static void write_cr_to_memory_write_byte_to_memory(void) {
    // Pseudocode: Writes a byte to memory at tmp0, increments pointer, tracks CR state

    // write_cr_to_memory:
    //     lda #0x0d
    // write_byte_to_memory:
    //     ldy #0
    //     sta (tmp0),y
    //     inc tmp0
    //     bne c8d0a
    //     inc tmp1
    // c8d0a:
    //     sta l0084
    //     cmp #0x0d
    //     bne return_16
    //     sty l0084
    //     sty l0083
    // return_16:
    //     rts

    // MULTIPLE ENTRY POINTS: write_cr_to_memory, write_byte_to_memory
}
static void read_first_chunk_from_input_file(void) {
    // Pseudocode: Reads first chunk of data from input file into memory top

    // ; ***************************************************************************************
    // read_first_chunk_from_input_file:
    //     lda page
    //     ldy page+1
    // read_next_chunk_from_input_file:
    //     jsr sub_c8da2

    //     ldx #<input_file
    //     ldy #>input_file
    //     jsr select_file
    //     jsr read_block_from_file

    //     php
    //     beq c8d39
    //     bcc c8d39
    //     inc input_file_empty_flag
    // c8d39:
    //     lda #0
    //     tay                                                               ; Y=0x00
    //     sta (tmp0),y
    //     lda tmp0
    //     sta top
    //     lda tmp1
    //     sta top+1
    //     plp
    //     rts

    // MULTIPLE ENTRY POINTS: read_first_chunk_from_input_file, read_next_chunk_from_input_file
}
static void write_area_to_file(void) {
    // Pseudocode: Writes document area range to output file byte by byte

    // ; Does not include trailing zero!
    // write_area_to_file:
    //     jsr sanitise_area
    //     beq return_17

    //     lda area_start_ptr
    //     sta tmp8
    //     lda area_start_ptr+1
    //     sta tmp9

    //     zrepeat
    //         ldy #0
    //         lda (tmp8),y
    //         jsr put_byte_to_file
    //         inc tmp8
    //         zif eq
    //             inc tmp9
    //         zendif

    //         lda tmp9
    //         cmp area_end_ptr+1
    //         zif eq
    //             lda tmp8
    //             cmp area_end_ptr
    //         zendif
    //     zuntil eq
    // return_17:
    //     rts
}
static void compute_required_space_for_insertion(void) {
    // Pseudocode: Computes available memory space for insertion, capping at 4 pages

    // compute_required_space_for_insertion:
    //     ldx #0
    //     stx tmp8
    //     stx tmp9
    //     beq c8daf                                                         ; ALWAYS branch

    // sub_c8da2:
    //     pha
    //     tya
    //     pha
    //     jsr compute_bytes_free
    //     stx tmp8
    //     sty tmp9
    //     pla
    //     tay
    //     pla
    // c8daf:
    //     sta tmp0
    //     sty tmp1
    //     jsr compute_bytes_free
    //     stx tmp6
    //     sty tmp7
    //     lsr tmp9
    //     ror tmp8
    //     lsr tmp9
    //     ror tmp8
    //     lda tmp9
    //     cmp #4
    //     bcc c8dce
    //     lda #4
    //     sta tmp9
    //     sta tmp8
    // c8dce:
    //     lda tmp6
    //     sbc tmp8
    //     sta tmp6
    //     lda tmp7
    //     sbc tmp9
    //     sta tmp7
    //     lda tmp0
    //     clc
    //     adc tmp6
    //     sta ptr5
    //     pha
    //     lda tmp1
    //     adc tmp7
    //     sta ptr5+1
    //     sta l0081
    //     pla
    //     sbc #0x8b
    //     sta input_buffer_ptr+1
    //     bcs return_18
    //     dec l0081
    // return_18:
    //     rts

    // MULTIPLE ENTRY POINTS: compute_required_space_for_insertion, sub_c8da2
}
static void parse_optional_filename_from_command(void) {
    // Pseudocode: Parses optional filename from input buffer into filename_buffer

    // parse_optional_filename_from_command:
    //     jsr sub_c8e33
    //     beq return_19
    //     ldx #0
    // loop_c8dfb:
    //     lda input_buffer,y
    //     cmp #0x0d
    //     beq c8e25
    //     iny
    //     cmp l007e
    //     beq c8e25
    //     sta filename_buffer,x
    //     inx
    //     cpx #0x14
    //     bne loop_c8dfb
}
static void bad_filename_error(void) {
    // Pseudocode: Displays Bad filename error and jumps to CLI

    // bad_filename_error:
    //     jsr print_inline_string
    //     .ascii "Bad filename\r"
    //     .byte 0
    //     jmp cli_loop
}
static void parse_filename_from_command(void) {
    // Pseudocode: Parses mandatory filename, calls bad_filename_error if missing

    // parse_filename_from_command:
    //     jsr parse_optional_filename_from_command
    //     beq bad_filename_error
    // return_19:
    //     rts

    // c8e25:
    //     lda #0x0d
    //     sta filename_buffer,x
    //     sty input_buffer_ptr
    // return_20:
    //     rts
}
static void sub_c8e2d_sub_c8e33(void) {
    // Pseudocode: Skips delimiter/space characters in input buffer

    // sub_c8e2d:
    //     lda #0x20 ; ' '
    //     sta l007e
    //     sty input_buffer_ptr
    // sub_c8e33:
    //     lda l007e
    //     cmp #0x0d
    //     beq return_20
    //     ldy input_buffer_ptr
    // loop_c8e3b:
    //     lda input_buffer,y
    //     cmp #0x0d
    //     beq return_20
    //     cmp l007e
    //     bne return_20
    //     iny
    //     bne loop_c8e3b

    // MULTIPLE ENTRY POINTS: sub_c8e2d, sub_c8e33
}
static void check_not_continuous_editing(void) {
    // Pseudocode: Verifies not in continuous editing mode, shows file state if editing

    // check_not_continuous_editing:
    //     bit file_edit_flags
    //     bvs return_20
    //     lda file_edit_flags
    //     ror
    //     bcc return_20
    //     bcs c8e5d                                                         ; ALWAYS branch

    // MULTIPLE ENTRY POINTS: check_not_continuous_editing, check_continuous_editing
    check_continuous_editing();
}
static void check_continuous_editing(void) {
    // Pseudocode: Verifies continuous editing is active, shows file state if not

    // check_continuous_editing:
    //     bit file_edit_flags
    //     bvs c8e5d
    //     lda file_edit_flags
    //     ror
    //     bcs return_20
    // c8e5d:
    //     jsr display_document_file_state
    //     jmp cli_loop

    // MULTIPLE ENTRY POINTS: check_not_continuous_editing, check_continuous_editing
}
static void display_nl_then_no_text(void) {
    // Pseudocode: Displays newline then No text message

    // display_nl_then_no_text:
    //     jsr bdos_print_newline
    // display_no_text:
    //     jsr print_inline_string
    //     .ascii "No text\r"
    //     .byte 0

    //     rts

    // MULTIPLE ENTRY POINTS: display_nl_then_no_text, display_no_text
}
static void print_document(void) {
    // Pseudocode: Main print/preview loop: formats document with headers, footers, macros, page breaks

    // print_document:
    //     jsr check_not_continuous_editing
    //     jsr check_for_at_least_150_bytes_free
    //     jsr sub_cb104
    //     lda top
    //     adc #3
    //     sta ptr5
    //     tax
    //     lda top+1
    //     adc #0
    //     sta ptr5+1
    //     tay
    //     txa
    //     adc #0x8d
    //     bcc c8edb
    //     iny
    // c8edb:
    //     sta first_macro_ptr
    //     sta last_macro_ptr
    //     sty first_macro_ptr+1
    //     sty last_macro_ptr+1
    //     lda #0
    //     sta l0031
    //     sta print_xpos
    //     sta printing_from_file_flag
    //     tay                                                               ; Y=0x00
    //     sta (last_macro_ptr),y
    //     lda #<(current_ruler_buffer)
    //     sta current_ruler_ptr
    //     lda #>(current_ruler_buffer)
    //     sta current_ruler_ptr+1
    //     jsr find_margins_of_current_ruler_buffer
    //     jsr sub_c8e33
    //     bne c8f0d
    //     inc printing_from_file_flag
    //     lda page
    //     sta ptr6
    //     lda page+1
    //     sta ptr6+1
    //     bne c8f30
    // c8f0a:
    // c8f0d:
    //     jsr parse_optional_filename_from_command
    //     bne c8f29
    //     lda l0031
    //     bpl return_23
    //     jmp c9263

    // return_23:
    //     rts

    // c8f1a:
    //     jsr stop_printing
    //     jsr bdos_print_newline
    //     jmp cli_loop

    // c8f29:
    //     #if 0
    //     lda #0x40 ; '@'
    //     jsr open_file
    //     #endif
    //     sta rw_file_handle
    // c8f30:
    //     lda l0031
    //     beq c8f3b
    //     lda l0021
    //     bne c8f3b
    //     jsr c9263
    // c8f3b:
    //     jsr sub_c9188
    //     bcs c8f0a
    //     jsr sub_c916a
    //     ldy #0
    //     sty input_buffer_ptr+1
    //     jsr deref_and_check_for_command_prefix
    //     bne c8fce_thunk
    //     ldy #3
    //     sty input_buffer_ptr+1
    //     jsr sub_cab6e
    //     bne c8f6e
    //     ldy #3
    //     ldx #0
    // loop_c8f5d:
    //     lda (tmp0),y
    //     sta current_ruler_buffer,x
    //     iny
    //     inx
    //     cmp #0x0d
    //     bne loop_c8f5d
    //     jsr find_margins_of_current_ruler_buffer
    // c8f6b:
    //     jmp c900e

    // c8f6e:
    //     jsr lookup_formatting_command
    //     bmi c8f7a
    //     jsr execute_formatting_command
    //     beq c8f6b
    // c8fce_thunk:
    //     bne c8fce                                                         ; ALWAYS branch

    // c8f7a:
    //     lda first_macro_ptr
    //     sta tmp6
    //     lda first_macro_ptr+1
    //     sta tmp7
    //     ldy #1
    //     lda (current_format_line_ptr),y
    //     sta tmp8
    //     iny                                                               ; Y=0x02
    //     lda (current_format_line_ptr),y
    //     jsr is_uppercase
    //     bcc c8f92
    //     lda #0x20 ; ' '
    // c8f92:
    //     sta tmp9
    // lookup_macro_name:
    //     ldy #0
    //     lda (tmp6),y
    //     beq c8f6b
    //     ldy #2
    //     lda (tmp6),y
    //     cmp tmp8
    //     bne get_next_macro_in_linked_list
    //     iny                                                               ; Y=0x03
    //     lda (tmp6),y
    //     cmp tmp9
    //     beq c8fb9
    // get_next_macro_in_linked_list:
    //     ldy #0
    //     lda (tmp6),y
    //     pha
    //     iny                                                               ; Y=0x01
    //     lda (tmp6),y
    //     sta tmp7
    //     pla
    //     sta tmp6
    //     jmp lookup_macro_name

    // c8fb9:
    //     lda macro_executing_flag
    //     bne nested_macro_error
    //     lda tmp6
    //     clc
    //     adc #4
    //     sta ptr3
    //     lda tmp7
    //     adc #0
    //     sta ptr3+1
    //     sta macro_executing_flag
    //     bne c900e
    // c8fce:
    //     lda l0031
    //     bne c8fd5
    //     jsr render_new_page
    // c8fd5:
    //     jsr sub_c9407
    //     lda #0
    //     sta l0039
    //     ldy input_buffer_ptr+1
    //     lda print_flags
    //     bpl c8fe6
    //     lda microspacing_flag
    //     bne c9034
    // c8fe6:
    //     lda (tmp0),y
    //     iny
    //     jsr sub_c9431
    //     jsr c9426
    //     cmp #0x0d
    //     bne c8fe6
    //     inc register_value_l
    //     bne c8ffb
    //     inc register_value_l+1
    // c8ffb:
    //     ldx line_spacing
    //     lda l0021
    //     clc
    //     sbc line_spacing
    //     bcs c9009
    //     lda #0
    //     ldx l0021
    //     dex
    // c9009:
    //     sta l0021
    //     jsr print_vertical_space
    // c900e:
    //     jmp c8f30
}
static void nested_macro_error(void) {
    // Pseudocode: Displays Nested macro call error and stops printing

    // ; ***************************************************************************************
    // nested_macro_error:
    //     jsr stop_printing
    //     jsr print_inline_string
    //     .ascii "Nested macro call"
    //     .byte 0

    //     jmp c8f1a
}
static void microspace_word_processor(void) {
    // Pseudocode: Processes words for microspaced justification during printing

    // return_24:
    //     rts

    // c9034:
    //     ldx #0
    //     stx l0044
    //     stx l0046
    //     stx l0045
    //     stx l0047
    //     stx l0039
    //     stx l0048
    //     stx l0042
    //     stx l0043
    //     stx l0083
    // c9048:
    //     txa
    //     pha
    //     lda (tmp0),y
    //     jsr sub_c9431
    //     pla
    //     tax
    //     lda (tmp0),y
    //     iny
    //     cmp #0x1a
    //     bne c906f
    //     bit l0083
    //     bpl c9064
    //     lda l0048
    //     beq c906b
    //     inc l0043
    //     bne c9048
    // c9064:
    //     lda l0039
    //     sta l0047
    //     jmp c908c

    // c906b:
    //     lda #0x20 ; ' '
    //     dec l0042
    // c906f:
    //     cmp #0x20 ; ' '
    //     bcc c9092
    //     bne c9090
    //     bit l0083
    //     bpl c9064
    //     lda l0042
    //     beq c908a
    //     bmi c9087
    //     inc l0043
    //     lda #0
    //     sta l0042
    //     beq c9048                                                         ; ALWAYS branch

    // c9087:
    //     clc
    //     ror l0042
    // c908a:
    //     inc l0048
    // c908c:
    //     lda #0x20 ; ' '
    //     bne c90b6                                                         ; ALWAYS branch

    // c9090:
    //     inc l0046
    // c9092:
    //     cmp #9
    //     beq c90a0
    //     cmp #0x0b
    //     beq c90a0
    //     sec
    //     ror l0083
    //     jmp c90b6

    // c90a0:
    //     pha
    //     lda l0039
    //     sta l0047
    //     lda #0
    //     sta l0083
    //     sta l0046
    //     sta l0048
    //     sta l0042
    //     sta l0043
    //     sta l0044
    //     sta l0045
    //     pla
    // c90b6:
    //     sta output_buffer,x
    //     inx
    //     cmp #0x0d
    //     beq c90e2
    //     cmp #0x20 ; ' '
    //     beq c9048
    //     lda l0048
    //     beq c9048
    //     clc
    //     adc l0044
    //     sta l0044
    //     lda l0046
    //     adc l0048
    //     sta l0046
    //     lda l0045
    //     adc l0043
    //     sta l0045
    //     lda #0
    //     sta l0048
    //     sta l0042
    //     sta l0043
    //     jmp c9048

    // c90e2:
    //     lda l0045
    //     beq c90f8
    //     lda ruler_right_stop
    //     beq c90f8
    //     sec
    //     sbc l0047
    //     bcc c90f8
    //     sbc l0045
    //     adc #0
    //     sec
    //     sbc l0046
    //     beq c9101
    // c90f8:
    //     lda #0
    //     sta l0039
    //     ldy input_buffer_ptr+1
    //     jmp c8fe6

    // c9101:
    //     lda #0
    //     sta tmp9
    //     ldx #8
    // loop_c9107:
    //     asl
    //     rol tmp9
    //     asl l0045
    //     bcc c9115
    //     clc
    //     adc microspacing_flag
    //     bcc c9115
    //     inc tmp9
    // c9115:
    //     dex
    //     bne loop_c9107
    //     sta tmp8
    //     lda l0044
    //     sta l0046
    //     jsr sub_cadf0
    //     sta l0045
    //     lda tmp8
    //     sta l0044
    //     ldy #0
    //     sty l0039
    // c912b:
    //     lda output_buffer,y
    //     iny
    //     jsr sub_c9431
    //     pha
    //     lda l0039
    //     cmp l0047
    //     beq c913b
    //     bcs c9142
    // c913b:
    //     pla
    //     jsr c9426
    //     jmp c9163

    // c9142:
    //     pla
    //     cmp #0x20 ; ' '
    //     bne c915b
    //     lda microspacing_flag
    //     clc
    //     adc l0044
    //     tax
    //     lda l0045
    //     beq c9154
    //     inx
    //     dec l0045
    // c9154:
    //     jsr sub_c9173
    //     lda #0x20 ; ' '
    //     bne c9160                                                         ; ALWAYS branch

    // c915b:
    //     ldx microspacing_flag
    //     jsr sub_c9173
    // c9160:
    //     jsr print_char
    // c9163:
    //     cmp #0x0d
    //     bne c912b
    //     jmp c8ffb
}
static void sub_c916a(void) {
    // Pseudocode: Checks if printer is active and starts microspacing if supported

    // sub_c916a:
    //     ldx print_flags
    //     bpl return_25
    //     ldx microspacing_flag
    //     bne c9177
    //     rts
}
static void sub_c9173(void) {
    // Pseudocode: Emits spaces for microspacing by calling printer driver with spacing count

    // sub_c9173:
    //     cpx l0043
    //     beq return_25
    // c9177:
    //     jsr sub_c9445
    //     pha
    //     stx l0043
    //     lda #9
    //     jsr call_printer_driver
    //     pla
    // return_25:
    //     rts

    // c9184:
    //     lda #0
    //     sta macro_executing_flag
}
static void sub_c9188(void) {
    // Pseudocode: Gets next line of text for printing, handling macro execution

    // sub_c9188:
    //     lda macro_executing_flag
    //     bne c91a3
    //     lda ptr5
    //     sta input_buffer_ptr+1
    //     sta tmp0
    //     lda ptr5+1
    //     sta l0081
    //     sta tmp1
    //     jsr sub_c9241
    //     bcs return_26
    //     lda ptr5
    //     ldy ptr5+1
    //     bne c91d0
    // c91a3:
    //     ldy #0
    //     ldx #0
    // c91a7:
    //     lda (ptr3),y
    //     cmp #4
    //     beq c9184
    //     cmp #0x40 ; '@'
    //     beq c91da
    //     iny
    // loop_c91b2:
    //     sta current_line_buffer,x
    //     inx
    //     cmp #0x0d
    //     beq c91c2
    //     cpx #0x83
    //     bcc c91a7
    //     lda #0x0d
    //     bne loop_c91b2                                                    ; ALWAYS branch

    // c91c2:
    //     tya
    //     clc
    //     adc ptr3
    //     sta ptr3
    //     bcc c91cc
    //     inc ptr3+1
    // c91cc:
    //     lda ptr1
    //     ldy ptr1+1
    // c91d0:
    //     sta tmp0
    //     sty tmp1
    //     sta current_format_line_ptr
    //     sty current_format_line_ptr+1
    //     clc
    // return_26:
    //     rts

    // c91da:
    //     iny
    //     lda (ptr3),y
    //     sec
    //     sbc #0x30 ; '0'
    //     bcc c9225
    //     cmp #0x0a
    //     bcs c9225
    //     iny
    //     sty l0084
    //     sta l0083
    //     lda #0
    //     sta l0082
    //     ldy #2
    // loop_c91f1:
    //     dec l0083
    //     bmi c9209
    // c91f5:
    //     iny
    //     lda (ptr5),y
    //     cmp #0x0d
    //     beq c9223
    //     jsr sub_c9228
    //     beq c91f5
    //     bvs c91f5
    //     cmp #0x2c ; ','
    //     beq loop_c91f1
    //     bne c91f5                                                         ; ALWAYS branch

    // c9209:
    //     iny
    //     lda (ptr5),y
    //     cmp #0x0d
    //     beq c9223
    //     jsr sub_c9228
    //     beq c9209
    //     bvs c921b
    //     cmp #0x2c ; ','
    //     beq c9223
    // c921b:
    //     sta current_line_buffer,x
    //     inx
    //     cpx #0x82
    //     bcc c9209
    // c9223:
    //     ldy l0084
    // c9225:
    //     jmp c91a7
}
static void sub_c9228(void) {
    // Pseudocode: Parses register reference markers (<, >, =) in format line

    // sub_c9228:
    //     cmp #0x3e ; '>'
    //     bne c9231
    //     lda #0
    //     sta l0082
    //     rts

    // c9231:
    //     cmp #0x3c ; '<'
    //     bne c923c
    //     lda #0x40 ; '@'
    //     sta l0082
    //     lda #0
    //     rts

    // c923c:
    //     bit l0082
    //     ora #0
    //     rts
}
static void sub_c9241(void) {
    // Pseudocode: Reads next line from file buffer or calls read_block_from_file for printing

    // sub_c9241:
    //     lda printing_from_file_flag
    //     beq c9260
    //     ldy #0
    // loop_c9247:
    //     lda (ptr6),y
    //     sec
    //     beq return_27
    //     sta (tmp0),y
    //     inc ptr6
    //     bne c9254
    //     inc ptr6+1
    // c9254:
    //     inc tmp0
    //     bne c925a
    //     inc tmp1
    // c925a:
    //     cmp #0x0d
    //     bne loop_c9247
    //     clc
    // return_27:
    //     rts

    // c9260:
    //     jmp read_block_from_file
}
static void c9263(void) {
    // Pseudocode: Handles page footer processing: prints footer, increments page number

    // c9263:
    //     lda l0038
    //     beq c9284
    //     ldx l0021                                                         ; X=number of lines
    //     jsr print_vertical_space
    //     ldx footer_margin                                                 ; X=number of lines
    //     jsr print_vertical_space
    //     lda footers_enabled_flag
    //     beq c927c
    //     ldx #<(footer_text_maybe)
    //     ldy #>(footer_text_maybe)
    //     jsr render_header_or_footer
    // c927c:
    //     jsr print_newline
    //     ldx bottom_margin                                                 ; X=number of lines
    //     jsr print_vertical_space
    // c9284:
    //     inc register_value_p
    //     bne c928c
    //     inc register_value_p+1
    // c928c:
    //     lda #1
    //     sta register_value_l
    //     lda #0
    //     sta register_value_l+1
    //     sta l0031
    //     rts
}
static void render_new_page(void) {
    // Pseudocode: Renders a new page with headers, margins, page number prompt

    // ; ***************************************************************************************
    // render_new_page:
    //     lda #0x81
    //     sta l0031
    //     bit print_flags
    //     bvc c92d4
    //     jsr stop_printing
    //     jsr print_inline_string
    //     .ascii "\rPage "
    //     .byte 0

    //     ldx register_value_p
    //     ldy register_value_p+1
    //     jsr render_number_to_screen
    //     jsr print_inline_string
    //     .ascii ".."
    //     .byte 0

    //     jsr flush_and_read_char
    //     bcs c92cc
    //     and #0xdf
    //     cmp #0x4d ; 'M'
    //     beq c92d4
    //     cmp #0x51 ; 'Q'
    //     bne c92cf
    // c92cc:
    //     jmp c8f1a

    // c92cf:
    //     lda #0xc0
    //     jsr start_printing
    // c92d4:
    //     lda l0038
    //     beq c92f0
    //     ldx top_margin                                                    ; X=number of lines
    //     jsr print_vertical_space
    //     lda headers_enabled_flag
    //     beq c92e8
    //     ldx #<(header_text_maybe)
    //     ldy #>(header_text_maybe)
    //     jsr render_header_or_footer
    // c92e8:
    //     jsr print_newline
    //     ldx header_margin                                                 ; X=number of lines
    //     jsr print_vertical_space
    // c92f0:
    //     ldx page_length
    //     lda l0038
    //     beq c930d
    //     ldx #1
    //     lda page_length
    //     clc
    //     sbc top_margin
    //     bcc c930d
    //     sbc header_margin
    //     bcc c930d
    //     clc
    //     sbc bottom_margin
    //     bcc c930d
    //     sbc footer_margin
    //     bcc c930d
    //     tax
    // c930d:
    //     stx l0021
    //     rts
}
static void render_header_or_footer(void) {
    // Pseudocode: Renders header or footer text with centering and justification

    // ; ***************************************************************************************
    // render_header_or_footer:
    //     stx tmp4
    //     sty tmp5
    //     ldy #0
    //     sty l0082
    //     lda (tmp4),y
    //     beq return_28
    //     jsr sub_c9407
    //     lda #0
    //     sta l0039
    //     jsr sub_c9393
    //     jsr sub_c93fd
    //     bcs c932e
    //     jsr sub_c93a1
    // c932e:
    //     jsr sub_c93c8
    //     jsr c937b
    //     jsr sub_c939b
    //     jsr sub_c93c8
    //     txa
    //     beq c9355
    //     dex
    //     txa
    //     lsr
    //     sta l0081
    //     jsr sub_c93be
    //     beq c9355
    //     lsr
    //     sec
    //     sbc l0081
    //     bcc c9355
    //     sbc l0039
    //     bcc c9355
    //     tax
    //     jsr sub_c941a
    // c9355:
    //     jsr c937b
    //     jsr sub_c93a1
    //     jsr sub_c93fd
    //     bcs c9363
    //     jsr sub_c9393
    // c9363:
    //     jsr sub_c93c8
    //     jsr sub_c93be
    //     beq c937b
    //     stx l0081
    //     sec
    //     sbc l0081
    //     bcc c937b
    //     sbc l0039
    //     bcc c937b
    //     tax
    //     inx
    //     jsr sub_c941a
    // c937b:
    //     ldy #0
    //     ldx l0084
    //     beq return_28
    // loop_c9381:
    //     txa
    //     pha
    //     lda output_buffer,y
    //     jsr sub_c9431
    //     jsr print_char
    //     iny
    //     pla
    //     tax
    //     dex
    //     bne loop_c9381
    // return_28:
    //     rts
}
static void sub_c9393_sub_c939b_sub_c93a1(void) {
    // Pseudocode: Advances through header/footer text finding string delimiters

    // sub_c9393:
    //     jsr sub_c93b6
    //     lda #0
    //     jmp c93aa

    // sub_c939b:
    //     jsr sub_c93b6
    //     jmp c93a7

    // sub_c93a1:
    //     jsr sub_c93b6
    //     jsr c93b8
    // c93a7:
    //     iny
    //     tya
    //     dey
    // c93aa:
    //     clc
    //     adc tmp4
    //     sta tmp2
    //     lda tmp5
    //     adc #0
    //     sta tmp3
    //     rts

    // MULTIPLE ENTRY POINTS: sub_c9393, sub_c939b, sub_c93a1
}
static void sub_c93b6(void) {
    // Pseudocode: Finds length of header/footer string by scanning for high-bit-set terminator

    // sub_c93b6:
    //     ldy #0xff
    // c93b8:
    //     iny
    //     lda (tmp4),y
    //     bpl c93b8
    //     rts
}
static void sub_c93be(void) {
    // Pseudocode: Returns ruler_right_stop or l003a-1 as the line width

    // sub_c93be:
    //     lda ruler_right_stop
    //     bne return_29
    //     lda l003a
    //     sec
    //     sbc #1
    // return_29:
}
static void sub_c93c8(void) {
    // Pseudocode: Copies header/footer text to output_buffer, expanding register references

    //     rts

    // sub_c93c8:
    //     ldx #0
    //     ldy #0
    //     sty l0081
    // c93ce:
    //     lda (tmp2),y
    //     bmi c93e6
    //     jsr check_for_control_code
    //     bne c93d9
    //     inc l0081
    // c93d9:
    //     iny
    //     cmp #0x7c ; '|'
    //     beq c93f2
    //     sta output_buffer,x
    //     inx
    //     cpx #0x84
    //     bcc c93ce
    // c93e6:
    //     stx l0084
    //     lda print_flags
    //     bpl return_30
    //     txa
    //     sec
    //     sbc l0081
    //     tax
    // return_30:
    //     rts

    // c93f2:
    //     lda (tmp2),y
    //     bmi c93e6
    //     iny
    //     jsr render_register
    //     jmp c93ce
}
static void sub_c93fd(void) {
    // Pseudocode: Checks two_sided_flag and returns page parity for alternate layout

    // sub_c93fd:
    //     sec
    //     lda two_sided_flag
    //     beq return_31
    //     lda register_value_p
    //     lsr
    // return_31:
}
static void sub_c9407(void) {
    // Pseudocode: Outputs left margin spaces, adjusting for two-sided printing

    //     rts

    // sub_c9407:
    //     jsr sub_c93fd
    //     lda left_margin
    //     bcc c9415
    //     ldx two_sided_flag
    //     beq c9415
    //     clc
    //     adc rhs_extra_margin
    // c9415:
    //     tax
    //     lda #0x20 ; ' '
    //     bne c9426                                                         ; ALWAYS branch
}
static void sub_c941a(void) {
    // Pseudocode: Adds extra spaces to x position for centering/justification

    // sub_c941a:
    //     txa
    //     clc
    //     adc l0039
    //     sta l0039
    //     lda #0x20 ; ' '
    //     bne c9426                                                         ; ALWAYS branch
}
static void print_vertical_space(void) {
    // Pseudocode: Prints X number of blank lines (newlines)

    // ; ***************************************************************************************
    // ; On Entry:
    // ;     X: number of lines
    // ; ***************************************************************************************
    // print_vertical_space:
    //     lda #0x0d
    // c9426:
    //     inx
    //     dex
    //     beq return_32
    // loop_c942a:
    //     jsr print_char
    //     dex
    //     bne loop_c942a
    // return_32:
    //     rts
}
static void sub_c9431(void) {
    // Pseudocode: Converts character for printing, updates x position counter

    // sub_c9431:
    //     jsr sub_ca5ae
    //     bit print_flags
    //     bpl c943c
    //     ora #0
    //     bmi return_33                                                     ; ALWAYS branch

    // c943c:
    //     pha
    //     txa
    //     clc
    //     adc l0039
    //     sta l0039
    //     pla
    // return_33:
    //     rts
}
static void sub_c9445(void) {
    // Pseudocode: Outputs print_xpos number of spaces to align printer

    // sub_c9445:
    //     pha
    //     lda print_xpos
    //     beq c9453
    //     lda #0x20 ; ' '
    // loop_c944c:
    //     jsr print_char_just_to_printer
    //     dec print_xpos
    //     bne loop_c944c
    // c9453:
    //     pla
    //     rts
}
static void print_newline_print_char(void) {
    // Pseudocode: Prints character to screen or printer with control code handling

    // ; ***************************************************************************************
    // print_newline:
    //     lda #0x0d
    // ; ***************************************************************************************
    // print_char:
    //     cmp #0x0d
    //     beq c9462
    //     cmp #0x20 ; ' '
    //     bne c9468
    //     inc print_xpos
    //     rts

    // c9462:
    //     lda #0
    //     sta print_xpos
    //     lda #0x0d
    // c9468:
    //     jsr sub_c9445
    // ; ***************************************************************************************
    // print_char_just_to_printer:
    //     bit print_flags
    //     bpl c9472
    //     jmp (printer_driver_ptr)

    // c9472:
    //     jsr check_for_control_code
    //     bne c9488
    //     pha
    //     lda #0x2d ; '-'
    //     bcs c947e
    //     lda #0x2a ; '*'
    // c947e:
    //     jsr set_inverted_text_if_not_mode_7
    //     jsr bdos_print_char                                                        ; Write character
    //     pla
    //     jmp set_normal_text_if_not_mode_7

    // c9488:
    //     jmp bdos_print_char                                                        ; Write character

    // MULTIPLE ENTRY POINTS: print_newline, print_char, print_char_just_to_printer
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
    // c949e:
    //     stx printer_driver_ptr
    //     sty printer_driver_ptr+1
    // return_35:
    //     rts
}
static void call_printer_driver(void) {
    // Pseudocode: Calls a numbered entry point in the printer driver via jump table

    // ; ***************************************************************************************
    // call_printer_driver:
    //     clc
    //     adc printer_driver_ptr
    //     sta tmp8
    //     lda printer_driver_ptr+1
    //     adc #0
    //     sta tmp9
    //     jmp (tmp8)
}
static void default_printer_driver(void) {
    // Pseudocode: Default printer driver stubs (all currently replaced with BRK)

    // default_printer_driver_ptr:
    // l94b2 = default_printer_driver_ptr+1
    //     .word default_printer_driver

    // ; ***************************************************************************************
    // default_printer_driver:
    //     jmp c94c0

    //     jmp c94c7

    //     jmp c94cb

    //     jmp return_34

    //     rts

    // c94c0:
    //     brk
    // #if 0
    //     cmp #0x80
    //     bcs return_35
    //     jmp bdos_print_char                                                        ; Write character
    // #endif

    // c94c7:
    //     brk
    // #if 0
    //     lda #2
    //     bne c94cd                                                         ; ALWAYS branch
    // #endif

    // c94cb:
    //     brk
    // #if 0
    //     lda #3
    // c94cd:
    //     jmp oswrch                                                        ; Write character 3
    // #endif
}
static void lj_fmt_cmd(void) {
    // Pseudocode: Left-justifies the current format line

    // ; ***************************************************************************************
    // lj_fmt_cmd:
    //     jsr expand_line
    //     bcc return_36
    //     lda #0
    //     beq c950f                                                         ; ALWAYS branch
}
static void ce_fmt_cmd(void) {
    // Pseudocode: Centers the current format line

    // ; ***************************************************************************************
    // ce_fmt_cmd:
    //     jsr expand_line
    //     bcc return_36
    //     txa
    //     beq return_36
    //     lsr
    //     sta l0084
    //     lda ruler_right_stop
    //     beq c950f
    //     sec
    //     sbc ruler_left_stop
    //     lsr
    //     sec
    //     adc ruler_left_stop
    //     sec
    //     sbc l0084
    //     bcs c950f
    //     lda #0
    //     beq c950f                                                         ; ALWAYS branch
}
static void rj_fmt_cmd(void) {
    // Pseudocode: Right-justifies the current format line

    // ; ***************************************************************************************
    // rj_fmt_cmd:
    //     jsr expand_line
    //     bcc c9529
    //     txa
    //     beq c9529
    //     dex
    //     dex
    //     lda #0
    //     cpx ruler_right_stop
    //     bcs c950f
    //     stx l0083
    //     lda ruler_right_stop
    //     sec
    //     sbc l0083
    // c950f:
    //     ldy #3
    //     tax
    //     beq c951c
    //     lda #0x20 ; ' '
    // loop_c9516:
    //     sta (current_format_line_ptr),y
    //     iny
    //     dex
    //     bne loop_c9516
    // c951c:
    //     lda output_buffer,x
    //     sta (current_format_line_ptr),y
    //     iny
    //     inx
    //     cmp #0x0d
    //     bne c951c
    //     inc l0030
    // c9529:
    //     sec
    // return_36:
    //     rts
}
static void expand_line(void) {
    // Pseudocode: Expands a format line into output_buffer, handling register references via |

    // ; ***************************************************************************************
    // expand_line:
    //     ldx #0
    //     stx l0083
    //     ldy #3
    //     jsr get_current_fmt_cmd_byte
    //     clc
    //     beq return_37
    // c9537:
    //     lda (current_format_line_ptr),y
    //     iny
    //     cmp #0x7c ; '|'
    //     beq c955e
    // c953e:
    //     sta output_buffer,x
    //     jsr check_for_control_code
    //     bne c9548
    //     inc l0083
    // c9548:
    //     inx
    //     cmp #0x0d
    //     beq c9555
    //     cpx #0x83
    //     bcc c9537
    //     lda #0x0d
    //     bne c953e                                                         ; ALWAYS branch

    // c9555:
    //     lda print_flags
    //     bpl return_37
    //     txa
    //     sbc l0083
    //     tax
    // return_37:
    //     rts

    // c955e:
    //     lda (current_format_line_ptr),y
    //     cmp #0x0d
    //     beq c953e
    //     iny
    //     jsr render_register
    //     jmp c9537
}
static void dh_fmt_cmd(void) {
    // Pseudocode: Stores header text (shared code with df_fmt_cmd)

    // ; ***************************************************************************************
    // dh_fmt_cmd:
    //     ldx #<(header_text_maybe)
    //     ldy #>(header_text_maybe)
    //     bne c9575                                                         ; ALWAYS branch

    // MULTIPLE ENTRY POINTS: dh_fmt_cmd, df_fmt_cmd
    df_fmt_cmd();
}
static void df_fmt_cmd(void) {
    // Pseudocode: Stores footer text (shared code with dh_fmt_cmd)

    // ; ***************************************************************************************
    // df_fmt_cmd:
    //     ldx #<(footer_text_maybe)
    //     ldy #>(footer_text_maybe)
    // c9575:
    //     stx tmp2
    //     sty tmp3
    //     lda #0
    //     sta l0081
    //     sta l007a
    //     ldy #3
    //     sty input_buffer_ptr+1
    //     lda (current_format_line_ptr),y
    //     sta l0083
    //     ldx #0x3f ; '?'
    // loop_c9589:
    //     iny
    //     sty l0082
    //     lda (current_format_line_ptr),y
    //     cmp #0x0d
    //     beq c959c
    //     cmp #0x1b
    //     bcs c9598
    //     lda #0x20 ; ' '
    // c9598:
    //     cmp l0083
    //     bne c959e
    // c959c:
    //     ora #0x80
    // c959e:
    //     jsr sub_c95b2
    //     cmp #0x8d
    //     beq c95aa
    //     ldy l0082
    //     dex
    //     bne loop_c9589
    // c95aa:
    //     lda #0x80
    //     jsr sub_c95b2
    //     jsr sub_c95b2
    // sub_c95b2:
    //     ldy l0081
    //     sta (tmp2),y
    //     iny
    //     sty l0081
    //     rts

    // MULTIPLE ENTRY POINTS: dh_fmt_cmd, df_fmt_cmd
}
static void em_fmt_cmd(void) {
    // Pseudocode: Evaluates expression and stores result in a register

    // ; ***************************************************************************************
    // em_fmt_cmd:
    //     ldy #3
    //     jsr get_current_fmt_cmd_byte
    //     beq return_38
    //     iny
    //     jsr get_register_address
    //     bcs return_38
    //     lda tmp6
    //     sta tmp0
    //     lda tmp7
    //     sta tmp1
    //     jsr evaluate_expression_from_fmt_cmd
    //     ldy #0
    //     sta (tmp0),y
    //     iny                                                               ; Y=0x01
    //     lda tmp9
    //     sta (tmp0),y
    // return_38:
    //     rts
}
static void pl_fmt_cmd(void) {
    // Pseudocode: Sets page_length from format command expression

    // ; ***************************************************************************************
    // pl_fmt_cmd:
    //     ldy #3
    //     jsr evaluate_expression_from_fmt_cmd
    //     sta page_length
    //     rts
}
static void ts_fmt_cmd(void) {
    // Pseudocode: Sets two_sided_flag and rhs_extra_margin from format command

    // ; ***************************************************************************************
    // ts_fmt_cmd:
    //     ldy #3
    //     jsr parse_boolean_from_fmt_cmd
    //     bcs return_39
    //     sta two_sided_flag
    //     jsr evaluate_expression_from_fmt_cmd
    //     sta rhs_extra_margin
    // return_39:
    //     rts
}
static void tm_fmt_cmd(void) {
    // Pseudocode: Sets top_margin from format command expression

    // ; ***************************************************************************************
    // tm_fmt_cmd:
    //     ldy #3
    //     jsr evaluate_expression_from_fmt_cmd
    //     sta top_margin
    //     rts
}
static void bm_fmt_cmd(void) {
    // Pseudocode: Sets bottom_margin from format command expression

    // ; ***************************************************************************************
    // bm_fmt_cmd:
    //     ldy #3
    //     jsr evaluate_expression_from_fmt_cmd
    //     sta bottom_margin
    //     rts
}
static void hm_fmt_cmd(void) {
    // Pseudocode: Sets header_margin from format command expression

    // ; ***************************************************************************************
    // hm_fmt_cmd:
    //     ldy #3
    //     jsr evaluate_expression_from_fmt_cmd
    //     sta header_margin
    //     rts
}
static void fm_fmt_cmd(void) {
    // Pseudocode: Sets footer_margin from format command expression

    // ; ***************************************************************************************
    // fm_fmt_cmd:
    //     ldy #3
    //     jsr evaluate_expression_from_fmt_cmd
    //     sta footer_margin
    //     rts
}
static void lm_fmt_cmd(void) {
    // Pseudocode: Sets left_margin from format command expression

    // ; ***************************************************************************************
    // lm_fmt_cmd:
    //     ldy #3
    //     jsr evaluate_expression_from_fmt_cmd
    //     sta left_margin
    //     rts
}
static void ls_fmt_cmd(void) {
    // Pseudocode: Sets line_spacing from format command expression

    // ; ***************************************************************************************
    // ls_fmt_cmd:
    //     ldy #3
    //     jsr evaluate_expression_from_fmt_cmd
    //     sta line_spacing
    //     rts
}
static void pe_fmt_cmd(void) {
    // Pseudocode: Forces page eject if remaining lines are less than value

    // ; ***************************************************************************************
    // pe_fmt_cmd:
    //     ldy #3
    //     jsr evaluate_expression_from_fmt_cmd
    //     tax
    //     beq page_eject_fmt
    //     cmp l0021
    //     bcc return_40
    //     lda l0031
    //     bne page_eject_fmt
    // return_40:
    //     rts
}
static void op_fmt_cmd(void) {
    // Pseudocode: Odd page: forces page eject if current page is even

    // ; ***************************************************************************************
    // op_fmt_cmd:
    //     lda register_value_p
    //     lsr
    //     bcc page_eject_fmt
    //     bcs c9642                                                         ; ALWAYS branch
}
static void ep_fmt_cmd(void) {
    // Pseudocode: Even page: forces page eject if current page is odd

    // ; ***************************************************************************************
    // ep_fmt_cmd:
    //     lda register_value_p
    //     lsr
    //     bcs page_eject_fmt
    // c9642:
}
static void page_eject_fmt(void) {
    // Pseudocode: Performs page eject by rendering new page and moving to sheet bottom

    //     jsr page_eject_fmt
    // ; ***************************************************************************************
    // page_eject_fmt:
    //     lda l0031
    //     bne c964c
    //     jsr render_new_page
    // c964c:
    //     jmp c9263
}
static void fo_fmt_cmd(void) {
    // Pseudocode: Sets footers_enabled_flag from boolean format argument

    // ; ***************************************************************************************
    // fo_fmt_cmd:
    //     ldy #3
    //     jsr parse_boolean_from_fmt_cmd
    //     bcs return_41
    //     sta footers_enabled_flag
    // return_41:
    //     rts
}
static void he_fmt_cmd(void) {
    // Pseudocode: Sets headers_enabled_flag from boolean format argument

    // ; ***************************************************************************************
    // he_fmt_cmd:
    //     ldy #3
    //     jsr parse_boolean_from_fmt_cmd
    //     bcs return_42
    //     sta headers_enabled_flag
    // return_42:
    //     rts
}
static void pb_fmt_cmd(void) {
    // Pseudocode: Sets page break flag l0038 from boolean format argument

    // ; ***************************************************************************************
    // pb_fmt_cmd:
    //     ldy #3
    //     jsr parse_boolean_from_fmt_cmd
    //     bcs return_43
    //     sta l0038
    // return_43:
    //     rts
}
static void add_macro_to_linked_list(void) {
    // Pseudocode: Links a new macro entry at the end of the macro linked list

    // ; ***************************************************************************************
    // dm_fmt_cmd:
    //     lda macro_executing_flag
    //     bne return_42
    //     lda last_macro_ptr
    //     sta tmp6
    //     lda last_macro_ptr+1
    //     sta tmp7
    //     ldy #3
    //     lda (current_format_line_ptr),y
    //     and #0xdf
    //     sta l0084
    //     iny                                                               ; Y=0x04
    //     lda (current_format_line_ptr),y
    //     jsr is_uppercase
    //     bcc c968d
    //     lda #0x20 ; ' '
    //     bne c968f                                                         ; ALWAYS branch

    // c968d:
    //     and #0xdf
    // c968f:
    //     dey
    //     sta (last_macro_ptr),y
    //     dey
    //     lda l0084
    //     sta (last_macro_ptr),y
    //     lda #4
    //     clc
    //     adc last_macro_ptr
    //     sta last_macro_ptr
    //     bcc c96a2
    //     inc last_macro_ptr+1
    // c96a2:
    //     lda himem
    //     sec
    //     sbc last_macro_ptr
    //     tax
    //     lda himem+1
    //     sbc last_macro_ptr+1
    //     bne c96b8
    //     cpx #0x97
    //     bcs c96b8
    //     jmp display_not_enough_memory

    // c96b8:
    //     lda last_macro_ptr
    //     sta tmp0
    //     sta input_buffer_ptr+1
    //     sta current_format_line_ptr
    //     lda last_macro_ptr+1
    //     sta tmp1
    //     sta l0081
    //     sta current_format_line_ptr+1
    //     jsr sub_c9241
    //     bcc c96ce
    //     rts

    // c96ce:
    //     ldy #0
    //     lda (last_macro_ptr),y
    //     jsr check_for_command_prefix
    //     bne c96f8
    //     jsr lookup_formatting_command
    //     cpx #5
    //     bne c96f8
    //     lda #4
    //     ldy #0
    //     sta (last_macro_ptr),y
    //     inc last_macro_ptr
    //     bne add_macro_to_linked_list
    //     inc last_macro_ptr+1
    // add_macro_to_linked_list:
    //     lda #0
    //     sta (last_macro_ptr),y
    //     lda last_macro_ptr
    //     sta (tmp6),y
    //     iny
    //     lda last_macro_ptr+1
    //     sta (tmp6),y
    //     rts
}
static void dm_fmt_cmd(void) {
    // Pseudocode: Defines a macro: stores macro name and position in linked list

    // c96f8:
    //     lda tmp0
    //     sta last_macro_ptr
    //     lda tmp1
    //     sta last_macro_ptr+1
}
static void ht_fmt_cmd(void) {
    // Pseudocode: Sets highlight codes (highlight1_code, highlight2_code) from format command

    //     bne c96a2
    // ; ***************************************************************************************
    // ht_fmt_cmd:
    //     ldy #3
    //     jsr get_current_fmt_cmd_byte
    //     beq return_44
    //     tax
    //     lda #0
    //     cpx #0x2d ; '-'
    //     beq c9716
    //     lda #1
    //     cpx #0x2a ; '*'
    //     bne c9719
    // c9716:
    //     iny
    //     bne c9725
    // c9719:
    //     jsr evaluate_expression_from_fmt_cmd
    //     sec
    //     sbc #1
    //     bcc return_44
    //     cmp #2
    //     bcs return_44
    // c9725:
    //     pha
    //     jsr evaluate_expression_from_fmt_cmd
    //     pla
    //     tax
    //     lda tmp8
    //     sta highlight1_code,x
    // return_44:
    //     rts
}
static void lookup_formatting_command(void) {
    // Pseudocode: Looks up two-letter formatting command in commands_table

    // ; ***************************************************************************************
    // lookup_formatting_command:
    //     ldy #2
    //     lda (current_format_line_ptr),y
    //     sta tmp3
    //     dey                                                               ; Y=0x01
    //     lda (current_format_line_ptr),y
    //     sta tmp2
    //     dey                                                               ; Y=0x00
    //     ldx #0
    // loop_c973e:
    //     lda tmp2
    //     cmp commands_table,y
    //     bne c974c
    //     lda tmp3
    //     cmp lb2a1,y
    //     beq return_45
    // c974c:
    //     inx
    //     iny
    //     iny
    //     lda commands_table,y
    //     bpl loop_c973e
    // return_45:
    //     rts
}
static void execute_formatting_command(void) {
    // Pseudocode: Executes a formatting command by index through the format jump table

    // ; ***************************************************************************************
    // execute_formatting_command:
    //     txa
    //     ldy #0
    //     ldx #0
    //     stx l0030
    //     jsr call_through_jumptable
    //     ldx l0030
    //     rts
}
static void parse_boolean_from_fmt_cmd(void) {
    // Pseudocode: Parses a boolean (ON/OFF/1/0) from format command argument

    // ; ***************************************************************************************
    // parse_boolean_from_fmt_cmd:
    //     jsr get_current_fmt_cmd_byte
    //     sec
    //     beq return_46
    //     lda current_format_line_ptr
    //     ldx current_format_line_ptr+1

    // MULTIPLE ENTRY POINTS: parse_boolean_from_fmt_cmd, sub_c976c
    sub_c976c();
}
static void sub_c976c(void) {
    // Pseudocode: Parses word-based flag (ON/OFF/YES/NO) from format command

    // sub_c976c:
    //     sta tmp8
    //     stx tmp9
    //     lda (tmp8),y
    //     tax
    //     lda #1
    //     cpx #0x31 ; '1'
    //     beq c977f
    //     lda #0
    //     cpx #0x30 ; '0'
    //     bne c9783
    // c977f:
    //     clc
    //     iny
    //     bne return_46
    // c9783:
    //     dey
    //     sty l0084
    //     ldx #0xff
    // c9788:
    //     iny
    //     lda (tmp8),y
    //     jsr to_uppercase
    //     inx
    //     cmp l97b0,x
    //     beq c9788
    //     lda l97b0,x
    //     bmi c97ae
    //     cmp #0x20 ; ' '
    //     bcc return_46
    // loop_c979d:
    //     inx
    //     lda l97b0,x
    //     bmi c97ae
    //     cmp #0x20 ; ' '
    //     bcs loop_c979d
    //     ldy l0084
    //     lda l97b1,x
    //     bpl c9788
    // c97ae:
    //     sec
    // return_46:
    //     rts

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
    //     sta tmp8
    //     sta tmp9
    //     sta input_buffer_ptr+1
    // c97c0:
    //     jsr get_current_fmt_cmd_byte
    //     beq c9821
    //     cmp #0x7c ; '|'
    //     bne c97d5
    //     jsr get_next_fmt_cmd_byte
    //     beq c9821
    //     iny
    //     jsr render_register
    //     jmp c97dc

    // c97d5:
    //     jsr ca6fe
    //     sta tmp8
    //     stx tmp9
    // c97dc:
    //     ldx input_buffer_ptr+1
    //     beq c9804
    //     lda #0
    //     sta input_buffer_ptr+1
    //     dex
    //     beq c97f7
    //     lda tmp4
    //     sec
    //     sbc tmp8
    //     sta tmp8
    //     lda tmp5
    //     sbc tmp9
    //     sta tmp9
    //     jmp c9804

    // c97f7:
    //     lda tmp4
    //     clc
    //     adc tmp8
    //     sta tmp8
    //     lda tmp5
    //     adc tmp9
    //     sta tmp9
    // c9804:
    //     lda tmp8
    //     sta tmp4
    //     lda tmp9
    //     sta tmp5
    //     jsr get_current_fmt_cmd_byte
    //     beq c9821
    //     ldx #1
    //     cmp #0x2b ; '+'
    //     beq c981c
    //     inx                                                               ; X=0x02
    //     cmp #0x2d ; '-'
    //     bne c9821
    // c981c:
    //     stx input_buffer_ptr+1
    //     iny
    //     bne c97c0
    // c9821:
    //     lda tmp8
    //     rts
}
static void get_next_fmt_cmd_byte(void) {
    // Pseudocode: Gets next non-space byte from format command line

    // ; ***************************************************************************************
    // get_next_fmt_cmd_byte:
    //     iny
    // ; ***************************************************************************************
    // get_current_fmt_cmd_byte:
    //     lda (current_format_line_ptr),y
    //     cmp #0x0d
    //     beq return_47
    //     cmp #0x20 ; ' '
    //     beq get_next_fmt_cmd_byte
    // return_47:
    //     rts

    // MULTIPLE ENTRY POINTS: get_next_fmt_cmd_byte, get_current_fmt_cmd_byte
}
static void sub_c9830(void) {
    // Pseudocode: Word-spacing justification: distributes extra spaces between words

    // sub_c9830:
    //     lda justifying_flag
    //     bne return_47
    //     sta l0046
    //     sta l0039
    //     sta l0042
    //     lda ruler_right_stop
    //     beq return_47
    //     jsr get_line_length
    //     sty l0043
    //     ldy #0
    //     beq c9861                                                         ; ALWAYS branch

    // c9847:
    //     lda l0039
    //     sta l0084
    //     iny
    //     cpy l0043
    //     beq c9871
    //     clc
    //     jsr sub_c9936
    //     beq c985c
    //     cmp #0x20 ; ' '
    //     bne c9847
    //     inc l0046
    // c985c:
    //     iny
    //     cpy l0043
    //     beq c986d
    // c9861:
    //     sec
    //     jsr sub_c9936
    //     beq c985c
    //     cmp #0x20 ; ' '
    //     bne c9847
    //     beq c985c                                                         ; ALWAYS branch

    // c986d:
    //     dec l0046
    //     bmi return_47
    // c9871:
    //     lda l0046
    //     beq return_47
    //     lda ruler_right_stop
    //     sec
    //     sbc l0084
    //     bcc return_47
    //     adc #0
    //     tax
    //     adc l0043
    //     sec
    //     sbc #0x84
    //     bcc c988c
    //     sta l0084
    //     txa
    //     sbc l0084
    //     tax
    // c988c:
    //     stx l0082
    //     stx tmp8
    //     lda #0
    //     sta tmp9
    //     jsr sub_cadf0
    //     sta l0045
    //     lda tmp8
    //     sta l0044
    //     ldy #0
    //     ldx l0046
    //     tya                                                               ; A=0x00
    // loop_c98a2:
    //     sta input_buffer,y
    //     iny
    //     dex
    //     bne loop_c98a2
    //     ldy print_xpos
    //     iny
    //     cpy l0046
    //     bcc c98b2
    //     ldy #1
    // c98b2:
    //     dey
    //     ldx l0046
    // c98b5:
    //     lda l0045
    //     beq c98bd
    //     lda #1
    //     dec l0045
    // c98bd:
    //     clc
    //     adc l0044
    //     sta input_buffer,y
    //     lda l0082
    //     sec
    //     sbc input_buffer,y
    //     php
    //     sta l0082
    //     iny
    //     cpy l0046
    //     bcc c98d3
    //     ldy #0
    // c98d3:
    //     plp
    //     beq c98d9
    //     dex
    //     bne c98b5
    // c98d9:
    //     sty print_xpos
    //     ldy #0
    //     sty l0081
    //     sty l0039
    //     lda #0x1a
    //     jsr wipe_buffer
    //     lda l0042
    //     beq c98f6
    //     ldy #0
    // loop_c98ec:
    //     lda output_buffer,y
    //     sta (current_edit_line_ptr),y
    //     iny
    //     cpy l0042
    //     bne loop_c98ec
    // c98f6:
    //     ldy l0042
    //     ldx l0042
    // c98fa:
    //     lda output_buffer,x
    //     cmp #0x20 ; ' '
    //     bne c9920
    //     lda l0081
    //     beq c991c
    //     sty l0084
    //     ldy l0039
    //     cpy l0046
    //     lda #0
    //     bcs c9912
    //     lda input_buffer,y
    // c9912:
    //     clc
    //     adc l0084
    //     inc l0039
    //     tay
    //     lda #0
    //     sta l0081
    // c991c:
    //     lda #0x20 ; ' '
    //     bne c9922                                                         ; ALWAYS branch

    // c9920:
    //     inc l0081
    // c9922:
    //     sta (current_edit_line_ptr),y
    //     iny
    //     inx
    //     cpx l0043
    //     bne c98fa
    //     lda #0x10
    // loop_c992c:
    //     cpy #0x84
    //     bcs return_48
    //     sta (current_edit_line_ptr),y
    //     iny
    //     bne loop_c992c
    // return_48:
    //     rts
}
static void sub_c9936(void) {
    // Pseudocode: Processes a character from the edit line for output, handling tabs and margins

    // sub_c9936:
    //     ror l0083
    //     lda (current_edit_line_ptr),y
    //     sta output_buffer,y
    //     cmp #9
    //     bne c994a
    //     jsr sub_ca5ae
    //     txa
    //     clc
    //     adc l0039
    //     bne c995c
    // c994a:
    //     cmp #0x0b
    //     bne c9969
    //     lda ruler_left_stop
    //     beq c9967
    //     ldx l0039
    //     beq c995c
    //     cpx ruler_left_stop
    //     bcc c995c
    //     inx
    //     txa
    // c995c:
    //     sta l0039
    //     sty l0042
    //     inc l0042
    //     lda #0
    //     sta l0046
    //     rts

    // c9967:
    //     lda #0x20 ; ' '
    // c9969:
    //     cmp #0x1b
    //     bcc c9967
    //     cmp #0x20 ; ' '
    //     bcc return_49
    //     inc l0039
    // return_49:
    //     rts
}
static void sub_c9977(void) {
    // Pseudocode: Main line formatting routine: reads source line, handles margins, tabs, wrapping

    // c9974:
    //     jmp c9a8d

    // sub_c9977:
    //     inc cursor_moved_flag
    //     ldy #4
    //     sty print_xpos
    //     ldy #0
    //     sty input_buffer_ptr
    //     sty l007e
    //     lda (current_line_ptr),y
    //     jsr check_for_command_prefix
    //     beq c9974
    // c998a:
    //     lda format_mode_flag
    //     and #0x81
    //     bne c9974
    //     lda ruler_right_stop
    //     beq c9974
    //     sec
    //     sbc ruler_left_stop
    //     bcc c9974
    //     adc #1
    //     sta input_buffer_ptr+1
    //     lda #0x10
    //     jsr wipe_buffer
    //     lda current_line_ptr
    //     sta tmp6
    //     lda current_line_ptr+1
    //     sta tmp7
    //     ldy #0
    //     sty l0047
    //     sty l0039
    //     sty l0038
    //     sty l0046
    //     sty bottom_margin
    // c99b6:
    //     sty l0048
    //     ldy l0047
    // loop_c99ba:
    //     jsr sub_ca536
    //     bne c99c7
    //     lda #0
    //     sta markers_array+1,x
    //     inc l007e
    //     bne loop_c99ba
    // c99c7:
    //     ldy l0047
    // c99c9:
    //     lda (current_line_ptr),y
    //     iny
    //     sty l0047
    //     cmp #9
    //     bne c99e0
    //     jsr sub_ca5ae
    //     dex
    //     txa
    //     clc
    //     adc l0039
    //     sta l0039
    //     lda #9
    //     bne c9a21                                                         ; ALWAYS branch

    // c99e0:
    //     cmp #0x1a
    //     bne c99ee
    // c99e4:
    //     lda l0046
    //     bne c99c9
    //     ldx #0xff
    //     lda #0x20 ; ' '
    //     bne c9a2e                                                         ; ALWAYS branch

    // c99ee:
    //     cmp #0x0b
    //     bne c9a11
    //     ldx input_buffer_ptr
    //     bne c99e4
    //     lda l0038
    //     bne c99e4
    //     inc l0038
    //     lda ruler_left_stop
    //     beq c99c9
    //     ldx l0039
    //     cpx ruler_left_stop
    //     bcs c9a0a
    //     sta l0039
    //     dec l0039
    // c9a0a:
    //     clc
    //     adc input_buffer_ptr+1
    //     sta input_buffer_ptr+1
    //     lda #0x0b
    // c9a11:
    //     cmp #0x0d
    //     bne c9a21
    //     dey
    //     beq c9a8d
    //     jsr sub_c9ac1
    //     bcs c9a87
    //     lda #0x20 ; ' '
    //     sta input_buffer_ptr
    // c9a21:
    //     ldy l0048
    //     ldx #0
    //     cmp #0x20 ; ' '
    //     bne c9a2e
    //     inx                                                               ; X=0x01
    //     bit l0046
    //     bmi c9a40
    // c9a2e:
    //     ldy l0048
    //     sta (current_edit_line_ptr),y
    //     cmp #0x20 ; ' '
    //     bne c9a38
    //     ror bottom_margin
    // c9a38:
    //     iny
    //     jsr check_for_control_code
    //     beq c9a40
    //     inc l0039
    // c9a40:
    //     bit l0046
    //     stx l0046
    //     bmi c9a58
    //     cmp #0x20 ; ' '
    //     beq c9a58
    //     cpy #0x85
    //     bcs c9a60
    //     lda bottom_margin
    //     beq c9a58
    //     lda l0039
    //     cmp input_buffer_ptr+1
    //     bcs c9a60
    // c9a58:
    //     cpy #0x86
    //     bcc c9a5d
    //     dey
    // c9a5d:
    //     jmp c99b6

    // c9a60:
    //     inc l0047
    // loop_c9a62:
    //     dec l0047
    //     dey
    //     beq c9a8d
    //     lda (current_edit_line_ptr),y
    //     pha
    //     lda #0x10
    //     sta (current_edit_line_ptr),y
    //     pla
    //     cmp #0x20 ; ' '
    //     bne loop_c9a62
    //     sec
    //     ror input_buffer_ptr
    //     jsr sub_caed6
    //     jsr sub_c9830
    //     jsr sub_c9aa9
    //     jsr c9a8d
    //     beq c9aa5
    //     jmp c998a

    // c9a87:
    //     jsr sub_caed6
    //     jsr sub_c9aa9
    // c9a8d:
    //     jsr c9e94
    //     lda current_line_ptr
    //     ldy current_line_ptr+1
    //     jsr sub_cab1a
    //     sec
    //     beq c9aa5
    //     tya
    //     clc
    //     adc tmp0
    //     sta current_line_ptr
    //     bcc c9aa4
    //     inc current_line_ptr+1
    // c9aa4:
    //     clc
    // c9aa5:
    //     clv
    //     lda l007e
    //     rts
}
static void sub_c9aa9(void) {
    // Pseudocode: Completes line formatting: adjusts pointers updates ruler stack

    // sub_c9aa9:
    //     sec
    //     rol l007e
    //     ldy l0047
    //     dey
    //     sty l003b
    //     inc l006e
    //     jsr sub_ca8b9
    //     bcc return_50
    //     pla
    //     pla
    //     lda #0x40 ; '@'
    //     sta l0084
    //     bit l0084
    // return_50:
    //     rts
}
static void sub_c9ac1(void) {
    // Pseudocode: Finds next word boundary for line wrapping, returns carry if found

    // sub_c9ac1:
    //     tya
    //     sec
    //     adc current_line_ptr
    //     sta tmp8
    //     sta tmp4
    //     lda current_line_ptr+1
    //     adc #0
    //     sta tmp9
    //     sta tmp5
    //     ldy #0
    //     sty l0083
    // c9ad5:
    //     lda (tmp4),y
    //     beq c9b2f
    //     jsr check_for_command_prefix
    //     beq c9b2f
    //     cmp #0x0d
    //     beq c9b2f
    //     tya
    //     bne c9b06
    //     sty l0084
    //     beq c9aef                                                         ; ALWAYS branch

    // c9ae9:
    //     inc tmp8
    //     bne c9aef
    //     inc tmp9
    // c9aef:
    //     lda (tmp8),y
    //     beq c9b06
    //     cmp #0x0d
    //     beq c9b06
    //     cmp #9
    //     beq c9b2f
    //     cmp #0x0b
    //     bne c9ae9
    //     rol l0084
    //     sec
    //     ror l0084
    //     bcs c9ae9
    // c9b06:
    //     lda (tmp4),y
    //     cmp #0x20 ; ' '
    //     bne c9b1a
    //     ldx ruler_left_stop
    //     beq c9b2f
    //     ldx l0084
    //     beq c9b2f
    //     ldx l0083
    //     bne c9b2f
    //     beq c9b20                                                         ; ALWAYS branch

    // c9b1a:
    //     cmp #0x0b
    //     bne c9b23
    //     sta l0083
    // c9b20:
    //     iny
    //     bne c9ad5
    // c9b23:
    //     lda ruler_left_stop
    //     beq c9b31
    //     lda l0084
    //     beq c9b31
    //     lda l0083
    //     bne c9b31
    // c9b2f:
    //     sec
    //     rts

    // c9b31:
    //     clc
    //     rts
}
static void run_editor(void) {
    // Pseudocode: Enters editor mode and falls through to editor_loop

    // run_editor:
    //     jsr enter_editor_mode
}
static void editor_loop(void) {
    // Pseudocode: Main editor loop: handles cursor positioning, redrawing, key dispatch
editor_loop:
    //     lda format_mode_flag
    //     pha
    //     lda l006e
    //     bne c9b44
    //     pha
    //     jsr sub_caa97
    //     pla
    //     sta l006e
    // c9b44:
    //     jsr sub_ca608
    //     lda ruler_left_stop
    //     beq c9b73
    //     ldx format_mode_flag
    //     bmi c9b73
    //     cmp l0072
    //     bcc c9b73
    //     beq c9b73
    //     ldx cursor_moved_flag
    //     bne c9b6a
    //     jsr get_line_length
    //     lda format_mode_flag
    //     cpy xpos
    //     bcs c9b84
    //     bit format_mode_flag
    //     bvs c9b6a
    //     sty xpos
    //     bvc c9b84                                                         ; ALWAYS branch

    //     lda ruler_left_stop
    //     sta l0072
    //     inc l0079
    //     jsr sub_ca608
    //     lda format_mode_flag
    //     and #0xbf
    //     pha
    //     jsr sub_caec2
    //     pla
    //     bcs c9b86
    //     cpy xpos
    //     bcc c9b86
    //     beq c9b86
    //     ora #0x40 ; '@'
    //     sta format_mode_flag
    //     pla
    //     cmp format_mode_flag
    //     beq c9b8f
    //     inc flags_need_redrawing_flag
    //     lda #0
    //     sta cursor_moved_flag
    //     jsr sub_ca276
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
    // Pseudocode: Indirect jump through tmp6 for dispatching key handlers

    // jsr_tmp6:
    //     jmp (tmp6)
}
static void enter_printable_character(void) {
    // Pseudocode: Inserts a printable character at cursor, handling insert mode

    // c9bca:
    //     jsr beep
    // c9bbb:
    //     jmp editor_loop

    // enter_printable_character:
    //     ldy xpos
    //     cpy #0x84
    //     bcs c9bca
    //     inc l006d
    //     jsr sub_caef4
    //     bcs c9bca
    //     lda current_edit_line_ptr
    //     sta tmp6
    //     lda current_edit_line_ptr+1
    //     sta tmp7
    //     ldy xpos
    //     jsr sub_ca536
    //     bne c9bf2
    //     cpx #4
    //     bcs c9bf2
    //     inc l0074
    // c9bf2:
    //     ldx insert_mode_flag
    //     bne c9c00
    //     lda (current_edit_line_ptr),y
    //     cmp #9
    //     beq c9c00
    //     cmp #0x0b
    //     bne c9c09
    // c9c00:
    //     inc l0074
    //     ldx #1
    //     jsr sub_cae06
    //     bcs c9c7f
    // c9c09:
    //     lda l0038
    //     sta (current_edit_line_ptr),y
    //     ldy l0074
    //     bne c9c14
    //     jsr screen_putchar
    // c9c14:
    //     inc xpos
    //     jsr ca684
    //     ldy #0
    //     sty l0039
    // c9c1d:
    //     lda (current_edit_line_ptr),y
    //     iny
    //     cpy xpos
    //     bcs c9c56
    //     cmp #9
    //     bne c9c31
    //     jsr sub_ca5ae
    //     txa
    //     clc
    //     adc l0039
    //     bne c9c43
    // c9c31:
    //     cmp #0x0b
    //     bne c9c4a
    //     lda ruler_left_stop
    //     beq c9c48
    //     ldx l0039
    //     beq c9c43
    //     cpx ruler_left_stop
    //     bcc c9c43
    //     inx
    //     txa
    // c9c43:
    //     sta l0039
    //     jmp c9c1d

    // c9c48:
    //     lda #0x20 ; ' '
    // c9c4a:
    //     cmp #0x1b
    //     bcc c9c48
    //     cmp #0x20 ; ' '
    //     bcc c9c1d
    //     inc l0039
    //     bne c9c1d
    // c9c56:
    //     ldy l0039
    //     cpy l003a
    //     bcs c9c67
    //     lda (current_ruler_ptr),y
    //     and #0xdf
    //     cmp #0x42 ; 'B'
    //     bne c9c67
    //     jsr beep
    // c9c67:
    //     lda l0038
    //     cmp #0x20 ; ' '
    //     beq c9c7f
    //     lda ruler_right_stop
    //     beq c9c7f
    //     lda format_mode_flag
    //     bne c9c7f
    //     sta tmp7
    //     tya
    //     beq c9c7f
    //     dey
    //     cpy ruler_right_stop
    //     bcs c9c82
    // c9c7f:
    //     jmp editor_loop

    // c9c82:
    //     jsr get_line_length
    //     sty l0083
    //     lda #0
    //     sta top_margin
    //     ldy xpos
    //     sty input_buffer_ptr+1
    //     jsr draw_previous_word
    //     jsr sub_ca608
    //     lda l0072
    //     cmp ruler_left_stop
    //     beq c9c9d
    //     bcs c9ca2
    // c9c9d:
    //     ldy input_buffer_ptr+1
    //     dey
    //     sty xpos
    // c9ca2:
    //     lda input_buffer_ptr+1
    //     sec
    //     sbc xpos
    //     sta top_margin
    //     lda l0083
    //     sec
    //     sbc xpos
    //     sta l0083
    //     tay
    //     iny
    //     lda ruler_left_stop
    //     beq c9cb9
    //     inc top_margin
    //     iny
    // c9cb9:
    //     sty tmp6
    //     lda current_line_ptr
    //     sec
    //     adc l003b
    //     sta tmp4
    //     lda current_line_ptr+1
    //     adc #0
    //     sta tmp5
    //     jsr make_space_for_insertion
    //     bcc c9cd0
    //     jmp ca941

    // c9cd0:
    //     ldy #0
    //     lda ruler_left_stop
    //     beq c9cdb
    //     lda #0x0b
    //     sta (tmp4),y
    //     iny                                                               ; Y=0x01
    // c9cdb:
    //     sty l0081
    //     lda current_edit_line_ptr
    //     sta tmp6
    //     lda current_edit_line_ptr+1
    //     sta tmp7
    //     ldy xpos
    //     dey
    //     lda (current_edit_line_ptr),y
    //     cmp #0x20 ; ' '
    //     bne c9cf2
    //     lda #0x10
    //     sta (current_edit_line_ptr),y
    // c9cf2:
    //     iny
    //     sty l0082
    // c9cf5:
    //     ldy l0082
    //     inc l0082
    // loop_c9cf9:
    //     jsr sub_ca536
    //     bne c9d0d
    //     lda l0081
    //     clc
    //     adc tmp4
    //     sta markers_array,x
    //     lda tmp5
    //     adc #0
    //     sta markers_array+1,x
    //     bcc loop_c9cf9
    // c9d0d:
    //     lda l0083
    //     bne c9d28
    //     lda #0x0d
    //     bne c9d30                                                         ; ALWAYS branch

    // c9d15:
    //     jsr sub_c9830
    //     jsr ca93c
    //     jsr ca741
    //     jsr return_key
    //     lda top_margin
    //     sta xpos
    //     jmp editor_loop

    // c9d28:
    //     lda (current_edit_line_ptr),y
    //     pha
    //     lda #0x10
    //     sta (current_edit_line_ptr),y
    //     pla
    // c9d30:
    //     ldy l0081
    //     inc l0081
    //     sta (tmp4),y
    //     dec l0083
    //     bpl c9cf5
    //     bmi c9d15                                                         ; ALWAYS branch
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
    //     cpy #0x84
    { uint16_t tmp_ = y - 0x84; flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | (tmp_ & FLAG_N) | (y >= 0x84 ? FLAG_C : 0); }
    //     bcs return_51
    if (flags & FLAG_C) return;
    //     inc xpos
    xpos++;
    // return_51:
    //     rts
    return;
}
static void f12_left_key(void) {
    // Pseudocode: Moves cursor left by one position

    // ; ***************************************************************************************
    // f12_left_key:
    //     ldy l0072
    //     beq return_52
    //     dec xpos
    // return_52:
}
static void f15_up_key(void) {
    // Pseudocode: Moves cursor to previous line, handling ruler stack

    //     rts

    // ; ***************************************************************************************
    // f15_up_key:
    //     jsr ca93c
    //     lda current_line_ptr
    //     ldy current_line_ptr+1
    //     jsr sub_cab37
    //     bcc return_53
    //     lda tmp0
    //     sta current_line_ptr
    //     lda tmp1
    //     sta current_line_ptr+1
    //     inc l0079
    //     inc cursor_moved_flag
    // return_53:
    //     rts
}
static void f14_down_key(void) {
    // Pseudocode: Moves cursor to next line

    // ; ***************************************************************************************
    // f14_down_key:
    //     jsr ca93c
    //     inc l0079
    //     bne c9d9b

    // MULTIPLE ENTRY POINTS: f14_down_key, return_key
    return_key();
}
static void return_key(void) {
    // Pseudocode: Carriage return: moves to next line at column 0

    // ; ***************************************************************************************
    // return_key:
    //     jsr ca93c
    //     lda #0
    //     sta xpos
    //     lda current_line_ptr
    //     sta tmp0
    //     lda current_line_ptr+1
    //     sta tmp1
    //     jsr cab29
    //     bne c9d9b
    //     tya
    //     ldy current_line_ptr+1
    //     clc
    //     adc current_line_ptr
    //     bcc c9d98
    //     iny
    // c9d98:
    //     jsr sub_c9de1
    // c9d9b:
    //     inc cursor_moved_flag
    //     lda current_line_ptr
    //     ldy current_line_ptr+1
    //     jsr sub_cab1a
    //     beq return_54
    //     tya
    //     clc
    //     adc current_line_ptr
    //     sta current_line_ptr
    //     bcc return_54
    //     inc current_line_ptr+1
    // return_54:
    //     rts

    // MULTIPLE ENTRY POINTS: f14_down_key, return_key
}
static void cf6_split_line_key(void) {
    // Pseudocode: Splits line at cursor position

    // ; ***************************************************************************************
    // cf6_split_line_key:
    //     jsr ca93c
    //     jsr get_line_length
    //     cpy xpos
    //     bcc c9dbd
    //     ldy xpos
    // c9dbd:
    //     inc l0079
    //     tya
    //     tax
    //     ldy #0
    //     lda (current_format_line_ptr),y
    //     jsr check_for_command_prefix
    //     bne c9dcd
    //     inx
    //     inx
    //     inx
    // c9dcd:
    //     ldy current_line_ptr+1
    //     txa
    //     clc
    //     adc current_line_ptr
    //     bcc c9de3
    //     iny
    //     bne c9de3
    // ; ***************************************************************************************

    // MULTIPLE ENTRY POINTS: cf6_split_line_key, f6_insert_line_key, sub_c9de1
}

// MULTIPLE ENTRY POINTS: cf6_split_line_key, f6_insert_line_key, sub_c9de1
static void f6_insert_line_key(void) {
    // f6_insert_line_key:
    //     jsr ca93c
    ca93c();
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
    ram[tmp4 + y] = a;
    //     jmp ca741
    ca741(); return;

c9dfd:
    //     jmp ca941
    ca941(); return;
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
    // Pseudocode: Deletes character under cursor

    // f9_delete_char_key:
    //     ldx #1
    //     inc l0074
    //     jmp cae64
}
static void f7_delete_line_key(void) {
    // Pseudocode: Deletes current line and moves cursor to previous line

    // ; ***************************************************************************************
    // f7_delete_line_key:
    //     jsr ca93c
    //     inc cursor_moved_flag
    //     lda current_line_ptr
    //     sta tmp4
    //     lda current_line_ptr+1
    //     sta tmp5
    //     ldx l003b
    //     inx
    //     stx tmp6
    //     lda #0
    //     sta tmp7
    //     jsr adjust_pointers
    //     jsr cb05a
    //     ldy #0
    //     lda (current_line_ptr),y
    //     bne c9e81
    //     lda current_line_ptr
    //     ldy current_line_ptr+1
    //     jsr sub_cab37
    //     lda tmp0
    //     sta current_line_ptr
    //     lda tmp1
    //     sta current_line_ptr+1
    // c9e81:
    //     inc l0079
    //     jmp ca741

    // ; ***************************************************************************************
}
// MULTIPLE ENTRY POINTS: sf2_release_margins_key, f4_beginning_of_line_key
static void sf2_release_margins_key(void) {
    // sf2_release_margins_key:
    //     bit format_mode_flag
    if (!(format_mode_flag & FLAG_V)) goto c9e94;
    //     jsr sub_caec2
    sub_caec2();
    //     bcs f4_beginning_of_line_key
    if (flags & FLAG_C) { f4_beginning_of_line_key(); return; }
    //     sty xpos
    xpos = y;
    //     rts
    return;
c9e94:
    //     lda #0
    a = 0;
    //     sta xpos
    xpos = a;
    //     rts
    return;
}
static void f4_beginning_of_line_key(void) {
    // f4_beginning_of_line_key:
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    //     jmp c9e94
    a = 0;
    xpos = a;
    return;
}
static void f5_end_of_line_key(void) {
    // Pseudocode: Moves cursor to end of current line

    // f5_end_of_line_key:
    //     inc cursor_moved_flag
    // c9e9b:
    //     jsr get_line_length
    //     sty xpos
    //     rts
}
static void cf7_join_lines_key(void) {
    // Pseudocode: Joins current line with next line

    // ; ***************************************************************************************
    // cf7_join_lines_key:
    //     jsr ca93c
    //     lda current_line_ptr
    //     sta tmp0
    //     lda current_line_ptr+1
    //     sta tmp1
    //     jsr cab29
    //     beq c9eda
    //     jsr check_for_command_prefix
    //     beq c9eda
    //     dey
    //     tya
    //     clc
    //     adc current_line_ptr
    //     sta tmp4
    //     lda current_line_ptr+1
    //     adc #0
    //     sta tmp5
    //     lda #0
    //     sta tmp7
    //     lda #1
    //     sta tmp6
    //     jsr adjust_pointers
    //     lda current_line_ptr
    //     ldy current_line_ptr+1
    //     jsr cac78
    //     inc l0079
    //     jmp ca741

    // c9eda:
    //     jmp beep

    // ; ***************************************************************************************
}
static void f3_delete_to_eol_key(void) {
    // Pseudocode: Deletes from cursor to end of line

    // f3_delete_to_eol_key:
    //     lda #0x84
    //     sec
    //     sbc xpos
    //     tax
    //     inc l0074
    //     jmp cae64
}
static void sf8_edit_command_key(void) {
    // Pseudocode: Allows editing formatting command on current line interactively

    // ; ***************************************************************************************
    // sf8_edit_command_key:
    //     jsr c9e94
    //     jsr sub_ca276
    //     inc l006d
    //     lda #0
    //     sta input_buffer_ptr+1
    //     sta l0081
    // edit_command_loop:
    //     ldx input_buffer_ptr+1
    //     ldy ypos
    //     jsr set_cursor_position
    //     jsr read_char
    //     bcs finished_editing_command
    //     cmp #0x0d
    //     beq finished_editing_command
    //     and #0xdf
    //     cmp #0x41 ; 'A'
    //     bcc edit_command_loop
    //     cmp #0x5b ; '['
    //     bcs edit_command_loop
    //     sta l0081
    //     jsr screen_putchar
    //     ldy input_buffer_ptr+1
    //     iny
    //     sty input_buffer_ptr+1
    //     sta (ptr1),y
    //     cpy #2
    //     bcc edit_command_loop
    //     lda #0
    //     sta input_buffer_ptr+1
    //     beq edit_command_loop                                             ; ALWAYS branch

    // finished_editing_command:
    //     lda l0081
    //     beq return_56
    //     lda ptr1
    //     sta current_format_line_ptr
    //     lda ptr1+1
    //     sta current_format_line_ptr+1
    //     ldy #0
    //     lda #0x80
    //     sta (current_format_line_ptr),y
    //     jmp caf5c
}
static void cf8_mark_as_ruler_key(void) {
    // Pseudocode: Marks current line as a ruler line with . as default characters

    // ; ***************************************************************************************
    // cf8_mark_as_ruler_key:
    //     lda ptr1
    //     sta current_format_line_ptr
    //     lda ptr1+1
    //     sta current_format_line_ptr+1
    //     ldy #0
    //     lda #0x81
    //     sta (current_format_line_ptr),y
    //     iny                                                               ; Y=0x01
    //     lda #0x2e ; '.'
    //     sta (current_format_line_ptr),y
    //     iny                                                               ; Y=0x02
    //     lda #0x2e ; '.'
    //     sta (current_format_line_ptr),y
    //     inc l0074
    //     lda l006e
    //     bmi c9f5f
    //     lda #0x80
    //     sta l006e
    //     inc l006d
    // c9f5f:
    //     jmp caf5c
}
static void sf9_delete_command_key(void) {
    // Pseudocode: Deletes any formatting command prefix from current line

    // ; ***************************************************************************************
    // sf9_delete_command_key:
    //     ldy #0
    //     lda (current_format_line_ptr),y
    //     jsr check_for_command_prefix
    //     bne return_56
    //     tya
    //     sta (current_format_line_ptr),y
    //     lda current_edit_line_ptr
    //     sta current_format_line_ptr
    //     lda current_edit_line_ptr+1
    //     sta current_format_line_ptr+1
    //     jsr sub_caf5f
    //     inc l0074
    //     inc l006d
    //     inc cursor_moved_flag
    // return_56:
    //     rts
}
static void sf12_left_key(void) {
    // Pseudocode: Moves cursor left by one word

    // c9f80:
    //     jsr ca93c
    //     lda current_line_ptr
    //     ldy current_line_ptr+1
    //     jsr sub_cab37
    //     bcc return_56
    //     lda tmp0
    //     sta current_line_ptr
    //     lda tmp1
    //     sta current_line_ptr+1
    //     jsr sub_caa97
    //     jsr c9e9b
    //     dec l006f
    //     rts

    // ; ***************************************************************************************
    // sf12_left_key:
    //     ldy xpos
    //     beq c9f80
    //     jsr draw_previous_word
    //     bne return_57
    //     cmp #0x20 ; ' '
    //     beq c9f80
    // return_57:
    //     rts

    // c9fab:
    //     sty xpos
    //     jsr ca93c
    //     lda current_line_ptr
    //     ldy current_line_ptr+1
    //     jsr sub_cab1a
    //     beq return_58
    //     tya
    //     clc
    //     adc current_line_ptr
    //     sta current_line_ptr
    //     bcc c9fc3
    //     inc current_line_ptr+1
    // c9fc3:
    //     jsr sub_caa97
    //     dec l006f
    //     jsr c9e94
    //     jsr get_line_length
    //     cpy xpos
    //     beq return_58
    //     lda current_edit_line_ptr
    //     sta tmp0
    //     lda current_edit_line_ptr+1
    //     sta tmp1
    //     ldy xpos
    //     jsr draw_char
    //     cmp #0x20 ; ' '
    //     bne return_58
    // ; ***************************************************************************************
    // sf13_right_key:
    //     lda current_edit_line_ptr
    //     sta tmp0
    //     lda current_edit_line_ptr+1
    //     sta tmp1
    //     jsr get_line_length
    //     sty input_buffer_ptr+1
    //     cpy xpos
    //     bcc c9fab
    //     beq c9fab
    //     ldy xpos
    // loop_c9ff8:
    //     cpy input_buffer_ptr+1
    //     bcs ca00f
    //     jsr draw_char
    //     cmp #0x20 ; ' '
    //     bne loop_c9ff8
    // loop_ca003:
    //     cpy input_buffer_ptr+1
    //     bcs ca00f
    //     jsr draw_char
    //     cmp #0x20 ; ' '
    //     beq loop_ca003
    //     dey
    // ca00f:
    //     sty xpos
    // return_58:

    // MULTIPLE ENTRY POINTS: sf12_left_key, sf13_right_key
    sf13_right_key();
}
static void sf13_right_key(void) {
    // Pseudocode: Moves cursor right by one word

    // MULTIPLE ENTRY POINTS: sf12_left_key, sf13_right_key
}
static void set_marker(void);
static void set_marker_common(void);
// MULTIPLE ENTRY POINTS: sf7_set_marker_key, set_marker, set_marker_1..6
static void sf7_set_marker_key(void) {
    // sf7_set_marker_key:
    //     jsr ca93c
    ca93c();
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
    //     jsr ca93c
    ca93c();
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
    //     jsr ca93c
    ca93c();
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
    a = markers_array[x];
    //     ldy markers_array+1,x
    y = markers_array[x+1];
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
    // Pseudocode: Formats the text block from current line to end of area

    // ; ***************************************************************************************
    // f0_format_block_key:
    //     jsr ca93c
    //     lda l0073
    //     pha
    //     lda l003d
    //     pha
    //     jsr ca741
    //     jsr sub_c9977
    //     bvs ca05b
    //     sec
    //     bne ca051
    //     clc
    // ca051:
    //     pla
    //     tax
    //     pla
    //     bcs return_59
    //     stx l003d
    //     sta l0073
    // return_59:
    //     rts

    // ca05b:
    //     jmp ca941
}
static void f1_top_of_text_key(void) {
    // Pseudocode: Moves cursor to top of document (page area)

    // ; ***************************************************************************************
    // f1_top_of_text_key:
    //     ldx #0xff
    //     stx l006f
    //     jsr sub_ca071
    //     jsr sub_caa97
    //     jmp c9e94
}
static void sf15_up_key(void) {
    // Pseudocode: Scrolls up one screenful or to top of document

    // ; ***************************************************************************************
    // sf15_up_key:
    //     ldx screen_height
    //     inc l0079
    //     inc l006f
    // sub_ca071:
    //     inc cursor_moved_flag
    //     stx input_buffer_ptr+1
    //     jsr ca93c
    //     lda current_line_ptr
    //     ldy current_line_ptr+1
    // ca07c:
    //     sta tmp2
    //     sty tmp3
    //     jsr sub_cab37
    //     lda tmp0
    //     ldy tmp1
    //     bcc ca093
    //     ldx input_buffer_ptr+1
    //     bmi ca07c
    //     dec input_buffer_ptr+1
    //     bne ca07c
    //     beq ca097                                                         ; ALWAYS branch

    // ca093:
    //     lda tmp2
    //     ldy tmp3
    // ca097:
    //     sta current_line_ptr
    //     sty current_line_ptr+1
    //     rts

    // MULTIPLE ENTRY POINTS: sf15_up_key, sub_ca071
}
static void f2_bottom_of_text_key(void) {
    // Pseudocode: Moves cursor to bottom of document

    // ; ***************************************************************************************
    // f2_bottom_of_text_key:
    //     ldx #0xff
    //     stx l006f
    //     jsr sub_ca0af
    //     jsr sub_caa97
    //     jmp c9e9b
}
static void sf14_down_key(void) {
    // Pseudocode: Scrolls down one screenful or to bottom of document

    // ; ***************************************************************************************
    // sf14_down_key:
    //     ldx screen_height
    //     inc l0079
    //     inc l006f
    // sub_ca0af:
    //     inc cursor_moved_flag
    //     stx input_buffer_ptr+1
    //     jsr ca93c
    //     lda current_line_ptr
    //     ldy current_line_ptr+1
    // ca0ba:
    //     jsr sub_cab1a
    //     beq ca0d2
    //     tya
    //     ldy tmp1
    //     clc
    //     adc tmp0
    //     bcc ca0c8
    //     iny
    // ca0c8:
    //     ldx input_buffer_ptr+1
    //     bmi ca0ba
    //     dec input_buffer_ptr+1
    //     bne ca0ba
    //     beq ca0d6                                                         ; ALWAYS branch

    // ca0d2:
    //     lda tmp0
    //     ldy tmp1
    // ca0d6:
    //     sta current_line_ptr
    //     sty current_line_ptr+1
    //     rts

    // MULTIPLE ENTRY POINTS: sf14_down_key, sub_ca0af
}
static void sf11_copy_key(void) {
    // Pseudocode: Copies ruler from previous line to current line

    // ; ***************************************************************************************
    // sf11_copy_key:
    //     jsr f6_insert_line_key
    //     jsr sub_ca276
    //     ldx l003a
    //     beq ca0ef
    //     ldy #0
    // loop_ca0e7:
    //     lda (current_ruler_ptr),y
    //     sta (current_edit_line_ptr),y
    //     iny
    //     dex
    //     bne loop_ca0e7
    // ca0ef:
    //     jmp cf8_mark_as_ruler_key
}
static void cf5_default_ruler_key(void) {
    // Pseudocode: Creates a default ruler on current line

    // ; ***************************************************************************************
    // cf5_default_ruler_key:
    //     jsr f6_insert_line_key
    //     jsr sub_ca276
    //     jsr cf8_mark_as_ruler_key
    //     lda current_edit_line_ptr
    //     ldy current_edit_line_ptr+1
    //     jmp create_default_ruler
}
static void sf3_delete_to_char_key(void) {
    // Pseudocode: Deletes text from cursor to specified character

    // ; ***************************************************************************************
    // sf3_delete_to_char_key:
    //     ldx #0x43 ; 'C'
    //     ldy #0x48 ; 'H'
    //     jsr draw_prompt_characters
    //     inc flags_need_redrawing_flag
    //     jsr read_char
    //     cmp #9
    //     beq ca12a
    //     cmp #0xa0
    //     bne ca11a
    //     lda #0x1c
    //     bne ca12a                                                         ; ALWAYS branch

    // ca11a:
    //     cmp #0xa1
    //     bne ca122
    //     lda #0x1d
    //     bne ca12a                                                         ; ALWAYS branch

    // ca122:
    //     cmp #0x20 ; ' '
    //     bcc ca151
    //     cmp #0x7f
    //     bcs ca151
    // ca12a:
    //     sta input_buffer_ptr+1
    //     inc l0074
    //     ldy xpos
    //     sty l0081
    // loop_ca132:
    //     cpy #0x84
    //     bcs ca151
    //     lda (current_edit_line_ptr),y
    //     iny
    //     cmp input_buffer_ptr+1
    //     bne loop_ca132
    // loop_ca13d:
    //     cpy #0x84
    //     bcs ca151
    //     lda (current_edit_line_ptr),y
    //     iny
    //     cmp input_buffer_ptr+1
    //     beq loop_ca13d
    //     dey
    //     tya
    //     sec
    //     sbc l0081
    //     tax
    //     jmp cae64

    // ca151:
    //     jmp beep

    // ; ***************************************************************************************
}
static void cf2_format_mode_key(void) {
    // Pseudocode: Toggles format mode on/off

    // cf2_format_mode_key:
    //     lda format_mode_flag
    //     and #0xbf
    //     bit format_mode_flag
    //     bvc ca15e
    //     ora #1
    // ca15e:
    //     eor #1
    //     sta format_mode_flag
    //     inc flags_need_redrawing_flag
    //     rts

    // ; ***************************************************************************************
}
static void cf3_justify_mode_key(void) {
    // Pseudocode: Toggles justify mode on/off

    // cf3_justify_mode_key:
    //     lda justifying_flag
    //     eor #0xff
    //     sta justifying_flag
    //     inc flags_need_redrawing_flag
    //     rts

    // ; ***************************************************************************************
}
static void cf4_insert_mode_key(void) {
    // Pseudocode: Toggles insert mode on/off

    // cf4_insert_mode_key:
    //     lda insert_mode_flag
    //     eor #0xff
    //     sta insert_mode_flag
    //     inc flags_need_redrawing_flag
    // return_60:
    //     rts
}
static void cf0_delete_block_key(void) {
    // Pseudocode: Deletes marked block from document

    // ; ***************************************************************************************
    // cf0_delete_block_key:
    //     jsr ca93c
    //     inc cursor_moved_flag
    //     jsr reset_area_to_marks_1_2
    //     bcs ca1c9
    //     lda area_start_ptr
    //     ldy area_start_ptr+1
    //     jsr move_cursor_to_address
    //     jsr ca741
    //     jsr sub_c89d3
    //     jsr cb05a
    //     jmp clear_marks_1_2
}
static void sf0_move_block_key(void) {
    // Pseudocode: Moves marked block to current cursor position

    // ; ***************************************************************************************
    // sf0_move_block_key:
    //     jsr ca93c
    //     jsr reset_area_to_marks_1_2
    //     bcs ca1c9
    //     jsr sub_ca1cc
    //     bcs return_60
    //     ldx #0xff
    //     stx l0012
    //     stx l006f
    //     jsr sub_c89d3
    //     jsr cb05a
    //     lda doc_ptr1+0
    //     ldy doc_ptr1+1
    //     jsr move_cursor_to_address
    //     jmp clear_marks_1_2
}
static void f11_copy_key(void) {
    // Pseudocode: Copies marked block to current cursor position

    // ; ***************************************************************************************
    // f11_copy_key:
    //     jsr ca93c
    //     jsr reset_area_to_marks_1_2
    //     bcs ca1c9
    //     jsr sub_ca1cc
    //     lda doc_ptr1+0
    //     ldy doc_ptr1+1
    //     jmp move_cursor_to_address

    // ca1c9:
    //     jmp beep
}
static void sub_ca1cc(void) {
    // Pseudocode: Block copy/move: inserts block content at cursor, optionally deleting source

    // sub_ca1cc:
    //     lda doc_ptr1+0
    //     ldy doc_ptr1+1
    //     cpy area_start_ptr+1
    //     bcc ca1ea
    //     bne ca1da
    //     cmp area_start_ptr
    //     bcc ca1ea
    // ca1da:
    //     cpy area_end_ptr+1
    //     bcc ca1c9
    //     bne ca1e6
    //     cmp area_end_ptr
    //     bcc ca1c9
    //     beq ca1c9
    // ca1e6:
    //     lda area_start_ptr
    //     ldy area_start_ptr+1
    // ca1ea:
    //     jsr move_cursor_to_address
    //     lda area_end_ptr
    //     sec
    //     sbc area_start_ptr
    //     sta tmp6
    //     lda area_end_ptr+1
    //     sbc area_start_ptr+1
    //     sta tmp7
    //     lda doc_ptr1+0
    //     sta tmp4
    //     lda doc_ptr1+1
    //     sta tmp5
    //     jsr make_space_for_insertion
    //     bcs ca265
    //     lda area_start_ptr
    //     sta tmp8
    //     lda area_start_ptr+1
    //     sta tmp9
    //     lda tmp4
    //     sta tmp2
    //     lda tmp5
    //     sta tmp3
    //     ldy #0
    // ca219:
    //     lda (tmp8),y
    //     sta (tmp2),y
    //     inc tmp2
    //     bne ca223
    //     inc tmp3
    // ca223:
    //     inc tmp8
    //     bne ca229
    //     inc tmp9
    // ca229:
    //     lda tmp8
    //     ldx tmp9
    //     cpx area_end_ptr+1
    //     bne ca219
    //     cmp area_end_ptr
    //     bne ca219
    //     lda tmp6
    //     pha
    //     lda tmp7
    //     pha
    //     lda tmp4
    //     sta doc_ptr1+0
    //     lda tmp5
    //     sta doc_ptr1+1
    //     lda tmp2
    //     ldy tmp3
    //     sec
    //     sbc #1
    //     bcs ca24d
    //     dey
    // ca24d:
    //     jsr cac78
    //     lda doc_ptr1+0
    //     ldy doc_ptr1+1
    //     jsr cac78
    //     pla
    //     sta tmp7
    //     pla
    //     sta tmp6
    //     lda #1
    //     sta l0073
    //     sta cursor_moved_flag
    //     clc
    //     rts

    // ca265:
    //     jmp ca941
}
static void cf1_next_match_key(void) {
    // Pseudocode: Finds and jumps to next occurrence of search pattern

    // ; ***************************************************************************************
    // cf1_next_match_key:
    //     jsr ca93c
    //     jsr c8b7b
    //     bne ca273
    //     jmp move_cursor_to_address

    // ca273:
    //     jmp esc_key
}
static void sub_ca276(void) {
    // Pseudocode: Main screen update routine: scrolls, redraws lines, updates status and cursor

    // sub_ca276:
    //     jsr cursor_off
    //     lda ruler_stack_ptr
    //     sta l0034
    //     lda l0076
    //     sta input_buffer_ptr+1
    //     lda l006e
    //     beq ca28e
    //     lda l0073
    //     ora l006f
    //     bne ca28e
    //     jmp ca360

    // ca28e:
    //     lda current_line_ptr+1
    //     cmp l0012
    //     bcc ca29c
    //     bne ca2dc
    //     lda current_line_ptr
    //     cmp l0011
    //     bcs ca2dc
    // ca29c:
    //     lda l006f
    //     bne ca30d
    //     lda l0033
    //     sta ruler_stack_ptr
    //     ldy l0012
    //     lda l0011
    //     cpy top+1
    //     bcc ca2b2
    //     bne ca30d
    //     cmp top
    //     bcs ca30d
    // ca2b2:
    //     jsr sub_cab37
    //     ldy tmp1
    //     cpy current_line_ptr+1
    //     bne ca30d
    //     lda tmp0
    //     cmp current_line_ptr
    //     bne ca30d
    //     sty l0012
    //     sta l0011
    //     ldx screen_height
    // loop_ca2c7:
    //     dex
    //     lda line_lengths,x
    //     inx
    //     sta line_lengths,x
    //     dex
    //     bne loop_ca2c7
    //     ldy #SCREEN_SCROLLDOWN
    //     jsr SCREEN
    //     jsr home_cursor
    //     ldy #1
    //     jmp ca351

    // ca2dc:
    //     lda l0033
    //     sta ruler_stack_ptr
    // ca2e0:
    //     ldx #0
    //     lda l0011
    //     ldy l0012
    // ca2e6:
    //     inx
    //     cpy ptr6+1
    //     bne ca2f1
    //     cmp ptr6
    //     bne ca2f1
    //     stx l003d
    // ca2f1:
    //     cpy current_line_ptr+1
    //     bne ca2f9
    //     cmp current_line_ptr
    //     beq ca313
    // ca2f9:
    //     jsr sub_cab1a
    //     beq ca313
    //     tya
    //     ldy tmp1
    //     clc
    //     adc tmp0
    //     bcc ca307
    //     iny
    // ca307:
    //     cpx screen_height
    //     beq ca2e6
    //     bcc ca2e6
    // ca30d:
    //     jsr sub_ca44e
    //     jmp ca2e0

    // ca313:
    //     cpx screen_height
    //     bcc ca35e
    //     beq ca35e
    //     lda l006f
    //     bne ca30d
    //     ldx #0
    // loop_ca31f:
    //     lda line_lengths+1,x
    //     sta line_lengths,x
    //     inx
    //     cpx screen_height
    //     bne loop_ca31f
    //     dec l003d
    //     ldx #0
    //     lda screen_width
    //     sta line_lengths,x
    //     lda l0033
    //     sta ruler_stack_ptr
    //     ldy l0012
    //     lda l0011
    //     jsr sub_cab1a
    //     tya
    //     clc
    //     adc l0011
    //     sta l0011
    //     bcc ca348
    //     inc l0012
    // ca348:
    //     ldy #SCREEN_SCROLLUP
    //     jsr SCREEN
    //     ldx #0
    //     ldy screen_height
    //     jsr set_cursor_position
    // ca351:
    //     lda ruler_stack_ptr
    //     sta l0033
    //     inc input_buffer_ptr+1
    //     inc l0074
    //     tya
    //     tax
    // ca35e:
    //     stx ypos
    // ca360:
    //     ldy l0034
    //     jsr cab91
    //     jsr unpack_line_into_buffer
    //     jsr sub_ca608
    //     lda screen_width
    //     lsr
    //     sta l0083
    //     lda l0072
    //     cmp hscroll_pos
    //     bcc ca381
    //     lda hscroll_pos
    //     clc
    //     adc screen_width
    //     sbc #3
    //     cmp l0072
    //     bcs ca395
    // ca381:
    //     lda l0072
    //     sec
    //     sbc l0083
    //     bcs ca38a
    //     lda #0
    // ca38a:
    //     sta hscroll_pos
    //     lda #1
    //     sta l0073
    //     sta input_buffer_ptr+1
    //     jsr ca93c
    // ca395:
    //     lda input_buffer_ptr+1
    //     sta l0076
    //     lda l0073
    //     beq ca3e7
    //     bpl ca3b2
    //     lda l003d
    //     bmi ca3b2
    //     sta l0082
    //     lda screen_height
    //     sec
    //     sbc l003d
    //     tax
    //     inx
    //     lda ptr6
    //     ldy ptr6+1
    //     bne ca3c1
    // ca3b2:
    //     ldy l0033
    //     jsr cab91
    //     lda #1
    //     sta l0082
    //     lda l0011
    //     ldy l0012
    //     ldx screen_height
    // ca3c1:
    //     stx l0081
    // loop_ca3c3:
    //     jsr sub_ca486
    //     lda tmp0
    //     ldy tmp1
    //     jsr sub_cab1a
    //     beq ca422
    //     tya
    //     ldy tmp1
    //     clc
    //     adc tmp0
    //     bcc ca3d8
    //     iny
    // ca3d8:
    //     inc l0082
    //     dec l0081
    //     bne loop_ca3c3
    // ca3de:
    //     lda #0
    //     sta l0074
    //     ldy l0034
    //     jsr cab91
    // ca3e7:
    //     jsr unpack_line_into_buffer
    //     jsr sub_caacb
    //     jsr display_status_word
    //     lda l0074
    //     beq ca3ff
    //     lda ypos
    //     sta l0082
    //     lda current_format_line_ptr
    //     ldy current_format_line_ptr+1
    //     jsr sub_ca486
    // ca3ff:
    //     lda flags_need_redrawing_flag
    //     beq ca406
    //     jsr sub_ca651
    // ca406:
    //     lda l0072
    //     sec
    //     sbc hscroll_pos
    //     clc
    //     adc #3
    //     tax
    //     ldy #0
    //     sty l0073
    //     sty l0074
    //     sty l006f
    //     dey                                                               ; Y=0xff
    //     sty ptr6+1
    //     ldy ypos
    //     jsr set_cursor_position
    //     jmp cursor_on
}
static void sub_ca44e(void) {
    // Pseudocode: Computes starting line for display based on screen position

    // ca422:
    //     dec l0081
    //     beq ca3de
    //     ldx l0082
    //     lda screen_width
    //     sta line_lengths+1,x
    //     sta l0083
    //     lda #0x2a ; '*'
    // loop_ca431:
    //     inc l0082
    //     ldx #0
    //     ldy l0082
    //     jsr set_cursor_position
    //     jsr sub_ca597
    //     lda l0083
    //     sta line_lengths,x
    //     lda #0
    //     sta l0083
    //     lda #0x20 ; ' '
    //     dec l0081
    //     bne loop_ca431
    //     beq ca3de                                                         ; ALWAYS branch

    // sub_ca44e:
    //     lda l0034
    //     sta ruler_stack_ptr
    //     lda screen_height
    //     sta l0073
    //     lsr
    //     tax
    //     inx
    //     lda l006f
    //     bmi ca461
    //     beq ca461
    //     ldx ypos
    // ca461:
    //     lda current_line_ptr
    //     ldy current_line_ptr+1
    // loop_ca465:
    //     dex
    //     beq ca479
    //     sta tmp2
    //     sty tmp3
    //     jsr sub_cab37
    //     lda tmp0
    //     ldy tmp1
    //     bcs loop_ca465
    //     lda tmp2
    //     ldy tmp3
    // ca479:
    //     sta l0011
    //     sty l0012
    //     lda ruler_stack_ptr
    //     sta l0033
    //     lda l0034
    //     sta ruler_stack_ptr
    //     rts
}
static void sub_ca486(void) {
    // Pseudocode: Renders a single document line to the screen at given y position

    // sub_ca486:
    //     sta tmp0
    //     sta tmp6
    //     sty tmp1
    //     sty tmp7
    //     ldx #0
    //     ldy l0082
    //     jsr set_cursor_position
    //     ldy #0
    //     sty l0083
    //     sty input_buffer_ptr+1
    //     sty l0039
    //     jsr deref_and_check_for_command_prefix
    //     bne ca4b4
    //     ldy #3
    //     lda hscroll_pos
    //     bne ca4b4
    //     ldy #1
    //     jsr sub_ca4d7
    //     jsr sub_ca4d7
    //     lda #0x20 ; ' '
    //     bne ca4bc                                                         ; ALWAYS branch

    // ca4b4:
    //     lda #0x20 ; ' '
    //     jsr ca4e9
    //     jsr ca4e9
    // ca4bc:
    //     jsr ca4e9
    // loop_ca4bf:
    //     jsr draw_char
    // loop_ca4c2:
    //     jsr render_xchar
    //     dex
    //     bne loop_ca4c2
    //     cmp #0x0d
    //     bne loop_ca4bf
    //     lda #0x20 ; ' '
    //     jsr sub_ca597
    //     lda l0083
    //     sta line_lengths,x
    //     rts
}
static void sub_ca4d7(void) {
    // Pseudocode: Draws a character and advances x position

    // sub_ca4d7:
    //     jsr draw_char
    //     jmp ca4e9
}
static void render_xchar(void) {
    // Pseudocode: Renders a character to screen with style/attribute handling

    // render_xchar:
    //     inc l0039
    //     stx l0084
    //     ldx input_buffer_ptr+1
    //     inc input_buffer_ptr+1
    //     cpx hscroll_pos
    //     bcc ca533
    // ca4e9:
    //     pha
    //     ldx l0082
    //     lda line_lengths,x
    //     beq ca4f4
    //     dec line_lengths,x
    // ca4f4:
    //     ldx l0083
    //     cpx screen_width
    //     bcs ca532
    //     inc l0083
    //     tya
    //     beq ca514
    //     dey
    //     jsr sub_ca536
    //     iny
    //     cpx #4
    //     bcs ca514
    //     tax
    //     bmi ca523
    //     bne ca514
    //     pla
    // ca50e:
    //     pha
    //     jsr set_inverted_text_if_not_mode_7
    //     ldx #0
    // ca514:
    //     pla
    //     jsr check_for_control_code
    //     bne ca522
    //     lda #0x2d ; '-'
    //     bcs ca50e
    //     lda #0x2a ; '*'
    //     bcc ca50e                                                         ; ALWAYS branch

    // ca522:
    //     pha
    // ca523:
    //     cmp #0x0d
    //     bne ca529
    //     lda #0x20 ; ' '
    // ca529:
    //     jsr screen_putchar
    //     txa
    //     bne ca532
    //     jsr set_normal_text_if_not_mode_7
    // ca532:
    //     pla
    // ca533:
    //     ldx l0084
    //     rts
}
static void sub_ca536(void) {
    // Pseudocode: Checks if a position in the edit line corresponds to a marker

    // sub_ca536:
    //     tya
    //     clc
    //     adc tmp6
    //     sta tmp8
    //     lda tmp7
    //     adc #0
    //     sta tmp9
    //     ldx #0
    // loop_ca544:
    //     lda tmp9
    //     cmp markers_array+1,x
    //     bne ca550
    //     lda tmp8
    //     cmp markers_array,x
    //     beq ca558
    // ca550:
    //     inx
    //     inx
    //     cpx #0x0c
    //     bne loop_ca544
    //     txa
    //     rts

    // ca558:
    //     lda #0
    // return_61:
    //     rts
}
static void set_normal_text_if_not_mode_7(void) {
    // Pseudocode: Sets normal text style via SCREEN call (no-op if mode 7)

    // ; ***************************************************************************************
    // set_normal_text_if_not_mode_7:
    //     pha
    //     txa
    //     pha
    //     tya
    //     pha
    //     ldy #SCREEN_SETSTYLE
    //     lda #0
    //     jsr SCREEN
    //     jmp 1f

    // ; ***************************************************************************************
    // set_inverted_text_if_not_mode_7:
    //     pha
    //     txa
    //     pha
    //     tya
    //     pha
    //     ldy #SCREEN_SETSTYLE
    //     lda #STYLE_REVERSE
    //     jsr SCREEN
    // 1:
    //     pla
    //     tay
    //     pla
    //     tax
    //     pla
    //     rts
}
static void set_inverted_text_if_not_mode_7(void) {
    // Pseudocode: Sets inverted/reverse text style via SCREEN call
}
static void sub_ca597(void) {
    // Pseudocode: Fills remaining space on line with spaces to clear to end

    // sub_ca597:
    //     ldx l0082
    //     sta l0084
    //     lda line_lengths,x
    //     beq return_62
    //     lda l0084
    // loop_ca5a2:
    //     jsr screen_putchar
    //     dec line_lengths,x
    //     bne loop_ca5a2
    // return_62:
    //     rts
}
static void draw_char(void) {
    // Pseudocode: Fetches and processes character: handles tabs, control codes, highlights

    // draw_char:
    //     lda (tmp0),y
    //     iny
    // sub_ca5ae:
    //     cmp #9
    //     beq ca5e1
    //     cmp #0x10
    //     beq ca5d5
    //     cmp #0x0b
    //     beq ca5d9
    //     cmp #0x1a
    //     beq ca5d5
    //     bcc ca5d1
    //     cmp #0x20 ; ' '
    //     bcs ca5d1
    //     sty l0084
    //     ldy print_flags
    //     bpl ca5cf
    //     sbc #0x1b
    //     tax
    //     lda highlight1_code,x
    // ca5cf:
    //     ldy l0084
    // ca5d1:
    //     ldx #1
    //     clc
    //     rts

    // ca5d5:
    //     lda #0x20 ; ' '
    //     bne ca5d1                                                         ; ALWAYS branch

    // ca5d9:
    //     lda ruler_left_stop
    //     beq ca5d5
    //     sty l0084
    //     bne ca5f1                                                         ; ALWAYS branch

    // ca5e1:
    //     sty l0084
    //     ldy l0039
    // loop_ca5e5:
    //     iny
    //     cpy l003a
    //     bcs ca5f8
    //     lda (current_ruler_ptr),y
    //     cmp #0x2a ; '*'
    //     bne loop_ca5e5
    //     tya
    // ca5f1:
    //     sbc l0039
    //     tax
    //     beq ca5f8
    //     bcs ca5fa
    // ca5f8:
    //     ldx #1
    // ca5fa:
    //     lda #0x20 ; ' '
    //     ldy l0084
    //     sec
    //     rts

    // MULTIPLE ENTRY POINTS: draw_char, sub_ca5ae
}
static void check_for_control_code(void) {
    // Pseudocode: Checks if character is a control code (0x1c or 0x1d)

    // check_for_control_code:
    //     cmp #0x1c
    //     beq return_63
    //     cmp #0x1d
    //     clc
    // return_63:
    //     rts
}
static void sub_ca608(void) {
    // Pseudocode: Recalculates cursor xpos from visual position accounting for tabs and margins

    // sub_ca608:
    //     lda current_edit_line_ptr
    //     sta tmp0
    //     lda current_edit_line_ptr+1
    //     sta tmp1
    //     lda l0079
    //     bne ca624
    //     tay
    // loop_ca615:
    //     cpy xpos
    //     beq ca63d
    //     sta l0039
    //     jsr draw_char
    //     txa
    //     clc
    //     adc l0039
    //     bcc loop_ca615
    // ca624:
    //     lda #0
    //     sta l0079
    //     tay                                                               ; Y=0x00
    // loop_ca629:
    //     sta l0039
    //     jsr draw_char
    //     txa
    //     clc
    //     adc l0039
    //     cmp l0072
    //     bcc loop_ca629
    //     beq ca63b
    //     lda l0039
    //     dey
    // ca63b:
    //     sty xpos
    // ca63d:
    //     sta l0072
    // return_64:
    //     rts
}
static void display_status_word(void) {
    // Pseudocode: Displays ruler status word at top of screen if l0076 is set

    // ; ***************************************************************************************
    // display_status_word:
    //     lda l0076
    //     beq return_64
    //     ldy #0
    //     sty l0076
    //     sty l0082
    //     lda current_ruler_ptr
    //     ldy current_ruler_ptr+1
    //     jsr sub_ca486
}
static void home_cursor(void) {
    // Pseudocode: Moves cursor to home position (0,0)

    // sub_ca651:
    //     lda #0
    //     sta flags_need_redrawing_flag
    //     jsr home_cursor
    //     ldx #0x46 ; 'F'
    //     lda format_mode_flag
    //     beq ca666
    //     ldx #0x4d ; 'M'
    //     and #0xc0
    //     bne ca666
    //     ldx #0x20 ; ' '
    // ca666:
    //     txa
    //     jsr screen_putchar
    //     lda #0x4a ; 'J'
    //     ldx justifying_flag
    //     beq ca672
    //     lda #0x20 ; ' '
    // ca672:
    //     jsr screen_putchar
    //     lda #0x49 ; 'I'
    //     ldx insert_mode_flag
    //     bne ca681
    //     lda #0x20 ; ' '
    //     bne ca681                                                         ; ALWAYS branch

    // ; ***************************************************************************************
    // home_cursor:
    //     ldx #0
    //     ldy #0
    //     jmp set_cursor_position
}
static void sub_ca651(void) {
    // Pseudocode: Redraws status line showing format mode, justify, and insert indicators

    // ca681:
}
static void ca684(void) {
    // Pseudocode: Sets line_lengths[ypos] = screen_width after cursor movement

    //     jmp screen_putchar

    // ca684:
    //     ldx ypos
    //     lda screen_width
    //     sta line_lengths,x
    //     rts
}
static void render_number_to_output_buffer(void) {
    // Pseudocode: Renders a 16-bit number to the output buffer using callback

    // ; ***************************************************************************************
    // ; On Entry:
    // ;     TMP9/TMP8: 16-bit number
    // ; ***************************************************************************************
    // render_number_to_output_buffer:
    //     stx l0082
    //     lda la69a
    //     ldy la69b
    //     jsr render_number_to_callback
    //     ldx l0082
    //     rts
}
static void emit_to_output_buffer_callback(void) {
    // Pseudocode: Callback that writes a digit character to the output buffer

    // la69a:
    // la69b = la69a+1
    //     .word emit_to_output_buffer_callback

    // emit_to_output_buffer_callback:
    //     pha
    //     txa
    //     pha
    //     tsx
    //     lda 0x0102,x
    //     ldx l0082
    //     sta output_buffer,x
    //     cpx #0x82
    //     bcs ca6ae
    //     inc l0082
    // ca6ae:
    //     pla
    //     tax
    //     pla
    //     rts
}
static void render_number_to_screen(void) {
    // Pseudocode: Renders a 16-bit number to screen via bdos_print_char

    // ; ***************************************************************************************
    // ; On Entry:
    // ;     YX: 16-bit number
    // ; ***************************************************************************************
    // render_number_to_screen:
    //     stx tmp8
    //     sty tmp9
    //     lda #<(bdos_print_char)
    //     ldy #>(bdos_print_char)
}
static void render_number_to_callback(void) {
    // Pseudocode: Generic 16-bit number renderer using configurable callback

    // ; ***************************************************************************************
    // ; On Entry:
    // ;     TMP8/TMP9: 16-bit number
    // ;     YA: callback
    // ; ***************************************************************************************
    // render_number_to_callback:
    //     sta tmp6
    //     sty tmp7
    //     ldy #0
    //     sty l0083
    // ca6c2:
    //     ldx #0
    // loop_ca6c4:
    //     lda tmp9
    //     cmp decimal_table+1,y
    //     bcc ca6e5
    //     bne ca6d4
    //     lda tmp8
    //     cmp decimal_table,y
    //     bcc ca6e5
    // ca6d4:
    //     lda tmp8
    //     sbc decimal_table,y
    //     sta tmp8
    //     lda tmp9
    //     sbc decimal_table+1,y
    //     sta tmp9
    //     inx
    //     bne loop_ca6c4
    // ca6e5:
    //     txa
    //     bne ca6ec
    //     ldx l0083
    //     beq ca6f1
    // ca6ec:
    //     jsr sub_ca6f9
    //     inc l0083
    // ca6f1:
    //     iny
    //     iny
    //     cpy #8
    //     bcc ca6c2
    //     lda tmp8
    // sub_ca6f9:
    //     ora #0x30 ; '0'
    //     jmp (tmp6)
}
static void sub_ca6f9(void) {
    // Pseudocode: Outputs a single digit character via callback
}
static void ca6fe(void) {
    // Pseudocode: Parses a decimal number from the format command line into tmp8/tmp9

    // ca6fe:
    //     lda #0
    //     tax                                                               ; X=0x00
    //     sta tmp8
    //     sta tmp9
    // ca705:
    //     lda (current_format_line_ptr),y
    //     sec
    //     sbc #0x30 ; '0'
    //     bcc ca739
    //     cmp #0x0a
    //     bcs ca739
    //     iny
    //     sta l0084
    //     asl tmp8
    //     rol tmp9
    //     ldx tmp9
    //     lda tmp8
    //     pha
    //     asl tmp8
    //     rol tmp9
    //     asl tmp8
    //     rol tmp9
    //     pla
    //     clc
    //     adc tmp8
    //     bcc ca72b
    //     inx
    // ca72b:
    //     clc
    //     adc l0084
    //     sta tmp8
    //     txa
    //     adc tmp9
    //     sta tmp9
    //     ldx #0xff
    //     bne ca705                                                         ; ALWAYS branch

    // ca739:
    //     txa
    //     php
    //     lda tmp8
    //     ldx tmp9
    //     plp
    //     rts
}
static void ca741(void) {
    // Pseudocode: Updates ptr6 to current_line_ptr and sets refresh flags

    // ca741:
    //     ldx current_line_ptr
    //     ldy current_line_ptr+1
    //     cpy ptr6+1
    //     bcc ca74f
    //     bne ca753
    //     cpx ptr6
    //     bcs ca753
    // ca74f:
    //     stx ptr6
    //     sty ptr6+1
    // ca753:
    //     ldx #0xff
    //     stx l0073
    //     stx l003d
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
    y = SCREEN_GETCHAR;
    //     jsr SCREEN
    SCREEN();
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
}
static void draw_prompt_characters(void) {
    // Pseudocode: Draws two inverted prompt characters at top-left of screen

    // ; ***************************************************************************************
    // draw_prompt_characters:
    //     stx tmp2
    //     sty tmp3
    //     jsr save_cursor_position
    //     jsr cursor_off
    //     jsr home_cursor
    //     jsr set_inverted_text_if_not_mode_7
    //     lda tmp2
    //     jsr screen_putchar
    //     lda tmp3
    //     jsr screen_putchar
    //     jsr set_normal_text_if_not_mode_7
    //     lda #0x20 ; ' '
    //     jsr screen_putchar
    //     jsr restore_cursor_position
    // cursor_on:
    // cursor_off:
    //     rts
}
static void cursor_on(void) {
    // Pseudocode: Placeholder for cursor on (currently does nothing)
}
static void cursor_off(void) {
    // Pseudocode: Placeholder for cursor off (currently does nothing)
}
static void save_cursor_position(void) {
    // Pseudocode: Saves current cursor position via SCREEN call

    // ; ***************************************************************************************
    // save_cursor_position:
    //     ldy #SCREEN_GETCURSOR
    //     jsr SCREEN
    //     sta tmp4
    //     stx tmp5
    //     rts
}
static void restore_cursor_position(void) {
    // Pseudocode: Restores cursor position and sets cursor coordinates

    // ; ***************************************************************************************
    // restore_cursor_position:
    //     ldx tmp4
    //     ldy tmp5
    // ; ***************************************************************************************
    // ; On Entry:
    // ;     X: X position
    // ;     Y: Y position
    // ; ***************************************************************************************
    // set_cursor_position:
    //     pha
    //     txa
    //     pha
    //     tya
    //     pha

    //     txa
    //     pha
    //     tya
    //     tax
    //     pla
    //     ldy #SCREEN_SETCURSOR
    //     jsr SCREEN

    //     pla
    //     tay
    //     pla
    //     tax
    //     pla
    // return_34:
    //     rts

    // MULTIPLE ENTRY POINTS: restore_cursor_position, set_cursor_position
}
static void print_inline_string(void) {
    // Pseudocode: Prints an inline string (embedded after JSR) with optional newline return to CLI

    // ; ***************************************************************************************
    // print_inline_string:
    //     sty l0084
    //     pla
    //     clc
    //     adc #1
    //     sta tmp2
    //     pla
    //     adc #0
    //     sta tmp3
    //     ldy #0
    //     beq ca80f                                                         ; ALWAYS branch

    // loop_ca80b:
    //     jsr bdos_print_char                                                        ; Write character
    //     iny
    // ca80f:
    //     lda (tmp2),y
    //     bmi ca824
    //     bne loop_ca80b
    //     tya
    //     sec
    //     adc tmp2
    //     sta tmp2
    //     bcc ca81f
    //     inc tmp3
    // ca81f:
    //     ldy l0084
    //     jmp (tmp2)

    // ca824:
    //     jsr bdos_print_newline
    //     jmp cli_loop
}
static void print_x_words_of_help(void) {
    // Pseudocode: Prints X words of the help string showing VIEW and version

    // ; ***************************************************************************************
    // print_x_words_of_help:
    //     ldy #0
    //     beq ca832                                                         ; ALWAYS branch

    // ca82e:
    //     jsr bdos_print_char
    //     iny
    // ca832:
    //     lda la83d,y
    //     bne ca82e
    //     lda #0x20 ; ' '
    //     dex
    //     bpl ca82e
    //     rts
}
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
    //     sta l0082
    //     tax                                                               ; X=0xff
    // ca84c:
    //     ldy input_buffer_ptr
    //     dey
    //     inc l0082
    // loop_ca851:
    //     inx
    //     iny
    //     lda (tmp0),y
    //     and #0xdf
    //     sta l0084
    //     lda parser_table,x
    //     beq ca890
    //     bmi ca87e
    //     eor #0x5b ; '['
    //     sta l0083
    //     and #0xdf
    //     cmp l0084
    //     beq loop_ca851
    // loop_ca86a:
    //     inx
    //     lda parser_table,x
    //     beq ca890
    //     bpl loop_ca86a
    //     lda l0083
    //     and #0x20 ; ' '
    //     beq ca84c
    //     lda (tmp0),y
    //     cmp #0x30 ; '0'
    //     bcs ca84c
    // ca87e:
    //     lda (tmp0),y
    //     cmp #0x30 ; '0'
    //     bcs ca887
    //     sta l007e
    //     iny
    // ca887:
    //     sty input_buffer_ptr
    //     ldy l0082
    //     lda parser_table,x
    //     clc
    //     rts

    // ca890:
    //     sec
    //     rts
}
static void call_through_jumptable(void) {
    // Pseudocode: Calls a routine through a jump table by index

    // ; ***************************************************************************************
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
    //     iny                                                               ; Y=0x01
    //     lda (tmp8),y
    //     sta tmp7
    //     jmp (tmp6)
}
static void look_up_address_in_table(void) {
    // Pseudocode: Looks up a key in an address table and returns the associated handler address

    // ; On entry: YX is the address of the table, A is the value
    // ; On exit, tmp6 is the routine; C if error, !C if success
    // zproc look_up_address_in_table
    //     stx tmp8
    //     sty tmp9
    //     sta tmp6
    //     ldy #0
    //     zloop
    //         lda (tmp8), y
    //         zbreakif eq
    //         cmp tmp6
    //         zif eq
    //             iny
    //             lda (tmp8), y
    //             sta tmp6
    //             iny
    //             lda (tmp8), y
    //             sta tmp7
    //             clc
    //             rts
    //         zendif
    //         iny
    //         iny
    //         iny
    //     zendloop
    //     sec
    //     rts
    // zendproc
}
static void sub_ca8b9(void) {
    // Pseudocode: Saves edit line changes back to document memory, updating markers

    // sub_ca8b9:
    //     lda l006e
    //     beq ca93a
    //     lda current_line_ptr
    //     sta tmp4
    //     lda current_line_ptr+1
    //     sta tmp5
    //     ldy #0
    //     sty tmp7
    //     jsr get_line_length
    //     sta l0083
    //     lda l003b
    //     sec
    //     sbc l0083
    //     bcc ca8df
    //     beq ca8ed
    //     sta tmp6
    //     jsr adjust_pointers
    //     jmp ca8ed

    // ca8df:
    //     sta l0084
    //     lda #0
    //     sec
    //     sbc l0084
    //     sta tmp6
    //     jsr make_space_for_insertion
    //     bcs return_66
    // ca8ed:
    //     lda l006e
    //     bpl ca8f8
    //     lda l006d
    //     beq ca8f8
    //     jsr ca741
    // ca8f8:
    //     ldy #0
    //     sty l006d
    //     sty l006e
    //     lda current_format_line_ptr
    //     sta tmp6
    //     lda current_format_line_ptr+1
    //     sta tmp7
    //     ldx l0083
    //     stx l003b
    // ca90a:
    //     txa
    //     bne ca911
    //     lda #0x0d
    //     bne ca919                                                         ; ALWAYS branch

    // ca911:
    //     lda (current_format_line_ptr),y
    //     cmp #0x10
    //     bne ca919
    //     lda #0x20 ; ' '
    // ca919:
    //     pha
    //     txa
    //     pha
    // loop_ca91c:
    //     jsr sub_ca536
    //     bne ca92f
    //     tya
    //     clc
    //     adc current_line_ptr
    //     sta markers_array,x
    //     lda current_line_ptr+1
    //     adc #0
    //     sta markers_array+1,x
    //     bne loop_ca91c
    // ca92f:
    //     pla
    //     tax
    //     pla
    //     sta (current_line_ptr),y
    //     iny
    //     dex
    //     cmp #0x0d
    //     bne ca90a
    // ca93a:
    //     clc
    // return_66:
    //     rts
}
// MULTIPLE ENTRY POINTS: ca93c, ca941
static void ca93c(void) {
    // ca93c:
    //     jsr sub_ca8b9
    sub_ca8b9();
    //     bcc return_66
    if (!(flags & FLAG_C)) return;
    //     falls through to ca941
    ca941(); return;
}
static void ca941(void) {
    // ca941:
    //     ldx #0xff
    x = 0xff;
    //     txs
    // PROBLEM: txs
    //     jsr sub_ca94a
    sub_ca94a();
    //     jmp editor_loop
    editor_loop(); return;
}
static void sub_ca94a(void) {
    // Pseudocode: Memory full error handler: displays message, waits for ESCAPE, clears state

    // sub_ca94a:
    //     jsr cursor_off
    //     ldx #3
    //     ldy #0
    //     jsr set_cursor_position
    //     jsr set_inverted_text_if_not_mode_7
    //     ldy screen_width
    //     sty line_lengths
    //     dey
    //     dey
    //     ldx #0
    //     beq ca965                                                         ; ALWAYS branch

    // loop_ca962:
    //     jsr screen_putchar
    // ca965:
    //     lda la995,x
    //     beq ca96e
    //     inx
    //     dey
    //     bne loop_ca962
    // ca96e:
    //     jsr set_normal_text_if_not_mode_7
    //     tya
    //     beq ca97c
    //     lda #0x20 ; ' '
    // loop_ca976:
    //     jsr screen_putchar
    //     dey
    //     bne loop_ca976
    // ca97c:
    //     lda #0
    //     sta l006e
    //     jsr clear_cmd
    // loop_ca983:
    //     jsr beep
    //     jsr flush_and_read_char
    //     bcc loop_ca983
    //     jsr cursor_on
    //     lda #1
    //     sta l0076
    //     sta l0073
    //     rts
}
static void adjust_pointers(void) {
    // Pseudocode: Adjusts all pointer array entries by delta (tmp6/tmp7) and moves memory block

    // la995:
    //     .ascii "Memory full - Press ESCAPE"
    //     .byte 0

    // adjust_pointers:
    //     lda tmp4
    //     sta tmp2
    //     clc
    //     adc tmp6
    //     sta tmp8
    //     lda tmp5
    //     sta tmp3
    //     adc tmp7
    //     sta tmp9
    //     ldx #0
    // ca9c3:
    //     ldy __begin_pointer_array+1,x
    //     lda __begin_pointer_array+0,x
    //     cpy tmp5
    //     bcc ca9f1
    //     bne ca9d1
    //     cmp tmp4
    //     bcc ca9f1
    // ca9d1:
    //     cpy tmp9
    //     bcc ca9db
    //     bne ca9e7
    //     cmp tmp8
    //     bcs ca9e7
    // ca9db:
    //     cpx #12
    //     bcs ca9e7
    //     lda #0
    //     sta __begin_pointer_array+0,x
    //     sta __begin_pointer_array+1,x
    //     beq ca9f1                                                         ; ALWAYS branch

    // ca9e7:
    //     sbc tmp6
    //     sta __begin_pointer_array+0,x
    //     lda __begin_pointer_array+1,x
    //     sbc tmp7
    //     sta __begin_pointer_array+1,x
    // ca9f1:
    //     inx
    //     inx
    //     cpx #22
    //     bne ca9c3
    // loop_ca9f7:
    //     ldy #0
    // loop_ca9f9:
    //     lda (tmp8),y
    //     sta (tmp2),y
    //     beq caa08
    //     iny
    //     bne loop_ca9f9
    //     inc tmp3
    //     inc tmp9
    //     bne loop_ca9f7
    // caa08:
    //     tya
    //     clc
    //     adc tmp2
    //     sta top
    //     lda tmp3
    //     adc #0
    //     sta top+1
    //     rts
}
static void make_space_for_insertion(void) {
    // Pseudocode: Makes space in document memory by shifting content up

    // make_space_for_insertion:
    //     lda top
    //     sta tmp2
    //     clc
    //     adc tmp6
    //     sta tmp8
    //     tax
    //     lda top+1
    //     sta tmp3
    //     adc tmp7
    //     sta tmp9
    //     tay
    //     cpy himem+1
    //     bcc caa32
    //     bne return_67
    //     cpx himem
    //     bcs return_67
    // caa32:
    //     stx top
    //     sty top+1
    //     ldx #0
    // loop_caa38:
    //     ldy __begin_pointer_array+1,x
    //     lda __begin_pointer_array+0,x
    //     cpy tmp5
    //     bcc caa51
    //     bne caa46
    //     cmp tmp4
    //     bcc caa51
    // caa46:
    //     clc
    //     adc tmp6
    //     sta __begin_pointer_array+0,x
    //     lda __begin_pointer_array+1,x
    //     adc tmp7
    //     sta __begin_pointer_array+1,x
    // caa51:
    //     inx
    //     inx
    //     cpx #22
    //     bne loop_caa38
    // caa57:
    //     lda tmp2
    //     sec
    //     sbc tmp4
    //     tax
    //     lda tmp3
    //     sbc tmp5
    //     beq caa65
    //     ldx #0xff
    // caa65:
    //     txa
    //     tay
    //     iny
    //     lda tmp2
    //     stx tmp2
    //     sec
    //     sbc tmp2
    //     sta tmp2
    //     bcs caa75
    //     dec tmp3
    // caa75:
    //     lda tmp8
    //     stx tmp8
    //     sec
    //     sbc tmp8
    //     sta tmp8
    //     bcs caa82
    //     dec tmp9
    // caa82:
    //     dey
    //     lda (tmp2),y
    //     sta (tmp8),y
    //     tya
    //     bne caa82
    //     inx
    //     beq caa57
    //     clc
    // return_67:
    //     rts
}
static void unpack_line_into_buffer(void) {
    // Pseudocode: Unpacks current line into edit buffer for editing

    // unpack_line_into_buffer:
    //     lda l006e
    //     bne return_68
    //     lda #1
    //     sta l006e
    // sub_caa97:
    //     lda #0x10
    //     jsr wipe_buffer
    //     jsr sub_caf5f
    //     ldy #0
    //     lda (current_line_ptr),y
    //     ldx current_edit_line_ptr
    //     ldy current_edit_line_ptr+1
    //     jsr check_for_command_prefix
    //     bne caab7
    //     bcs caab0
    //     sta l006e
    // caab0:
    //     jsr caf5c
    //     ldx ptr1
    //     ldy ptr1+1
    // caab7:
    //     stx current_format_line_ptr
    //     sty current_format_line_ptr+1
    //     ldy #0
    // loop_caabd:
    //     lda (current_line_ptr),y
    //     cmp #0x0d
    //     beq caac8
    //     sta (current_format_line_ptr),y
    //     iny
    //     bne loop_caabd
    // caac8:
    //     sty l003b
    // return_68:
    //     rts

    // MULTIPLE ENTRY POINTS: unpack_line_into_buffer, sub_caa97
}
static void sub_caacb(void) {
    // Pseudocode: Updates marker positions to point into format buffer instead of document buffer

    // sub_caacb:
    //     lda current_line_ptr
    //     sta tmp6
    //     lda current_line_ptr+1
    //     sta tmp7
    //     ldy #0
    // caad5:
    //     jsr sub_ca536
    //     bne caae8
    //     tya
    //     clc
    //     adc current_format_line_ptr
    //     sta markers_array,x
    //     lda current_format_line_ptr+1
    //     adc #0
    //     sta markers_array+1,x
    //     bne caad5
    // caae8:
    //     lda (current_line_ptr),y
    //     cmp #0x0d
    //     beq return_68
    //     iny
    //     bne caad5
}
static void get_line_length(void) {
    // Pseudocode: Returns the length of the current edit line

    // ; ***************************************************************************************
    // get_line_length:
    //     ldy #0
    //     lda (current_format_line_ptr),y
    //     jsr check_for_command_prefix
    //     php
    //     ldy #0x84
    // loop_caafb:
    //     dey
    //     lda (current_edit_line_ptr),y
    //     cmp #0x10
    //     bne cab06
    //     tya
    //     bne loop_caafb
    //     dey
    // cab06:
    //     iny
    //     tya
    //     plp
    //     bne return_69
    //     clc
    //     adc #3
    // return_69:
    //     rts
}
static void wipe_buffer(void) {
    // Pseudocode: Fills ptr1 buffer with a given byte value for 0x89 bytes

    // wipe_buffer:
    //     ldy #0
    //     ldx #0x89
    // loop_cab13:
    //     sta (ptr1),y
    //     iny
    //     dex
    //     bne loop_cab13
    //     rts
}
static void sub_cab1a(void) {
    // Pseudocode: Finds next line in document, handling command prefix and ruler stack

    // sub_cab1a:
    //     sta tmp0
    //     sty tmp1
    //     jsr sub_cab6e
    //     bne cab29
    //     jsr cab29
    //     bne push_onto_ruler_stack
    //     rts
}
static void cab29(void) {
    // Pseudocode: Skips to next CR or zero terminator in memory

    // cab29:
    //     ldy #0
    // loop_cab2b:
    //     lda (tmp0),y
    //     beq return_70
    //     iny
    //     cmp #0x0d
    //     bne loop_cab2b
    //     lda (tmp0),y
    // return_70:
    //     rts
}
static void sub_cab37(void) {
    // Pseudocode: Finds previous line in document, handling ruler stack

    // sub_cab37:
    //     sec
    //     sbc #1
    //     sta tmp0
    //     bcs cab3f
    //     dey
    // cab3f:
    //     sty tmp1
    //     cpy page+1
    //     bcc return_71
    //     bne cab4b
    //     cmp page
    //     bcc return_71
    // cab4b:
    //     ldy #0
    // loop_cab4d:
    //     lda tmp0
    //     sec
    //     sbc #1
    //     sta tmp0
    //     bcs cab58
    //     dec tmp1
    // cab58:
    //     lda (tmp0),y
    //     cmp #0x0d
    //     bne loop_cab4d
    //     inc tmp0
    //     bne cab64
    //     inc tmp1
    // cab64:
    //     jsr sub_cab6e
    //     bne cab6c
    //     jsr pop_from_ruler_stack
    // cab6c:
    //     sec
    // return_71:
    //     rts
}
static void sub_cab6e(void) {
    // Pseudocode: Checks if byte at tmp0 is a command prefix (0x81)

    // sub_cab6e:
    //     ldy #0
    //     lda (tmp0),y
    //     cmp #0x81
    //     rts
}
static void push_onto_ruler_stack(void) {
    // Pseudocode: Pushes current ruler position onto the ruler stack

    // push_onto_ruler_stack:
    //     tya
    //     pha
    //     inc l0076
    //     ldy ruler_stack_ptr
    //     dey
    //     lda tmp0
    //     sta (oshwm),y
    //     dey
    //     lda tmp1
    //     sta (oshwm),y
    //     jsr cab91
    //     pla
    //     tay
    //     rts
}
static void pop_from_ruler_stack(void) {
    // Pseudocode: Pops ruler position from the ruler stack

    // pop_from_ruler_stack:
    //     inc l0076
    //     ldy ruler_stack_ptr
    //     iny
    //     iny

    // MULTIPLE ENTRY POINTS: pop_from_ruler_stack, cab91
    cab91();
}
static void cab91(void) {
    // Pseudocode: Sets current_ruler_ptr from stack at ruler_stack_ptr offset

    // cab91:
    //     sty ruler_stack_ptr
    //     iny
    //     lda (oshwm),y
    //     clc
    //     adc #3
    //     sta current_ruler_ptr
    //     dey
    //     lda (oshwm),y
    //     adc #0
    //     sta current_ruler_ptr+1

    // MULTIPLE ENTRY POINTS: pop_from_ruler_stack, cab91
}
static void find_margins_of_current_ruler_buffer(void) {
    // Pseudocode: Finds left (>) and right (<) margin stops in the current ruler buffer

    // ; ***************************************************************************************
    // find_margins_of_current_ruler_buffer:
    //     jsr sub_cabc4
    // loop_caba5:
    //     lda (current_ruler_ptr),y
    //     cmp #0x3e ; '>'
    //     bne cabad
    //     sty ruler_left_stop
    // cabad:
    //     cmp #0x3c ; '<'
    //     bne cabb3
    //     sty ruler_right_stop
    // cabb3:
    //     cmp #0x0d
    //     beq cabbc
    //     iny
    //     cpy #0x84
    //     bne loop_caba5
    // cabbc:
    //     sty l003a
    //     lda ruler_left_stop
    //     cmp ruler_right_stop
    //     bcc return_72
}
static void sub_cabc4(void) {
    // Pseudocode: Resets ruler_left_stop and ruler_right_stop to zero

    // sub_cabc4:
    //     ldy #0
    //     sty ruler_right_stop
    //     sty ruler_left_stop
    // return_72:
    //     rts
}
static void move_cursor_to_address(void) {
    // Pseudocode: Moves cursor to a given memory address, handling ruler stack and setting xpos

    // move_cursor_to_address:
    //     sta tmp8
    //     sty tmp9
    //     lda current_line_ptr
    //     ldy current_line_ptr+1
    //     cpy tmp9
    //     bcc cabf9
    //     bne cabdf
    //     cmp tmp8
    //     bcc cabf9
    //     beq cac20
    // cabdf:
    //     jsr sub_cab37
    //     lda tmp0
    //     ldy tmp1
    //     bcc cac20
    //     cpy tmp9
    //     bcc cac20
    //     bne cabdf
    //     cmp tmp8
    //     bcc cac20
    //     bne cabdf
    //     beq cac20                                                         ; ALWAYS branch

    // cabf6:
    //     jsr sub_cac41
    // cabf9:
    //     sta tmp0
    //     sty tmp1
    //     jsr cab29
    //     beq cac17
    //     tya
    //     ldy tmp1
    //     clc
    //     adc tmp0
    //     bcc cac0b
    //     iny
    // cac0b:
    //     cpy tmp9
    //     bcc cabf6
    //     bne cac17
    //     cmp tmp8
    //     bcc cabf6
    //     beq cac1d
    // cac17:
    //     lda tmp0
    //     ldy tmp1
    //     bne cac20
    // cac1d:
    //     jsr sub_cac41
    // cac20:
    //     sta current_line_ptr
    //     sty current_line_ptr+1
    //     lda tmp8
    //     sec
    //     sbc current_line_ptr
    //     tax
    //     ldy #0
    //     lda (current_line_ptr),y
    //     jsr check_for_command_prefix
    //     bne cac3e
    //     txa
    //     cpx #3
    //     ldx #0
    //     bcc cac3e
    //     sec
    //     sbc #3
    //     tax
    // cac3e:
    //     stx xpos
    //     rts
}
static void sub_cac41(void) {
    // Pseudocode: Pushes ruler stack before entering a new ruler region

    // sub_cac41:
    //     pha
    //     tya
    //     pha
    //     jsr sub_cab6e
    //     bne cac4c
    //     jsr push_onto_ruler_stack
    // cac4c:
    //     pla
    //     tay
    //     pla
    //     rts
}
static void sub_cac50(void) {
    // Pseudocode: Finds the start of current line by scanning backward for CR

    // sub_cac50:
    //     sec
    //     sbc #1
    //     sta tmp8
    //     bcs cac58
    //     dey
    // cac58:
    //     sty tmp9
    //     ldy #0
    // cac5c:
    //     lda (tmp8),y
    //     cmp #0x0d
    //     beq cac6f
    //     lda tmp8
    //     sec
    //     sbc #1
    //     sta tmp8
    //     bcs cac5c
    //     dec tmp9
    //     bne cac5c
    // cac6f:
    //     lda tmp8
    //     sta tmp6
    //     lda tmp9
    //     sta tmp7
    // return_73:
    //     rts
}
static void cac78(void) {
    // Pseudocode: Splits a line at the word wrap position, inserting CR for new line

    // cac78:
    //     jsr sub_cac50
    // cac7b:
    //     lda #0
    //     sta l0083
    //     ldx #0x85
    //     ldy #1
    //     lda (tmp8),y
    //     jsr check_for_command_prefix
    //     bne cac8d
    //     inx
    //     inx
    //     inx
    // cac8d:
    //     stx l0084
    // cac8f:
    //     lda (tmp8),y
    //     iny
    //     cmp #0x20 ; ' '
    //     beq cac9a
    //     cmp #0x1a
    //     bne cac9c
    // cac9a:
    //     sty l0083
    // cac9c:
    //     cmp #0x0d
    //     beq return_73
    //     cpy l0084
    //     beq cac8f
    //     bcc cac8f
    //     lda l0084
    //     ldx l0083
    //     beq cacad
    //     txa
    // cacad:
    //     clc
    //     adc tmp8
    //     sta tmp4
    //     sta tmp8
    //     lda tmp9
    //     adc #0
    //     sta tmp5
    //     sta tmp9
    //     lda #1
    //     sta tmp6
    //     lda #0
    //     sta tmp7
    //     jsr make_space_for_insertion
    //     lda #0x0d
    //     ldy #0
    //     sta (tmp4),y
    //     lda tmp4
    //     sta tmp8
    //     lda tmp5
    //     sta tmp9
    //     bne cac7b
}
static void prompt_for_marker(void) {
    // Pseudocode: Prompts for a marker character and looks it up

    // prompt_for_marker:
    //     ldx #0x4d ; 'M'
    //     ldy #0x4b ; 'K'
    //     jsr draw_prompt_characters
    //     inc flags_need_redrawing_flag
    //     jsr read_char
    //     jsr lookup_marker
    //     bcc return_74
    // ; ***************************************************************************************
    // beep:
    // loop_caced:
    //     sec
    // return_74:
    //     rts
}
static void beep(void) {
    // Pseudocode: Emits a beep (returns with carry set as flag)
}
static void lookup_marker(void) {
    // Pseudocode: Converts marker character '1'-'6' to index and returns pointer

    // ; ***************************************************************************************
    // lookup_marker:
    //     sec
    //     sbc #0x31 ; '1'
    //     bcc loop_caced
    //     asl
    //     cmp #0x0c
    //     bcs return_75
    //     tax
    //     lda markers_array+1,x
    // return_75:
    //     rts
}
static void reset_area_to_entire_document(void) {
    // Pseudocode: Resets area to entire document (top to page)

    // ; ***************************************************************************************
    // reset_area_to_entire_document:
    //     lda top
    //     sta area_start_ptr
    //     lda top+1
    //     sta area_start_ptr+1
    //     lda page
    //     sta area_end_ptr
    //     lda page+1
    //     sta area_end_ptr+1
    //     rts
}
static void clear_marks_1_2(void) {
    // Pseudocode: Clears markers 1 and 2 (resets to zero)

    // ; ***************************************************************************************
    // clear_marks_1_2:
    //     lda #0
    //     ldx #3
    // loop_cad12:
    //     sta markers_array,x
    //     dex
    //     bpl loop_cad12
    //     rts
}
static void reset_area_to_marks_1_2(void) {
    // Pseudocode: Sets area to markers 1 and 2, then adjusts doc_ptr1

    // ; ***************************************************************************************
    // reset_area_to_marks_1_2:
    //     lda #0x31 ; '1'
    //     jsr lookup_marker
    //     bcs return_76
    //     beq cad45
    //     lda markers_array,x
    //     sta area_start_ptr
    //     lda markers_array+1,x
    //     sta area_start_ptr+1
    //     lda #0x32 ; '2'
    //     jsr lookup_marker
    //     bcs return_76
    //     beq cad45
    //     lda markers_array,x
    //     sta area_end_ptr
    //     lda markers_array+1,x
    //     sta area_end_ptr+1
    //     ldx #(doc_ptr1-__begin_pointer_array)/2
    //     jsr set_marker_to_here
    //     jsr sanitise_area
    //     clc
    //     bne return_76
    // cad45:
    //     sec
    // return_76:
    //     rts
}
static void set_marker_to_here(void) {
    // Pseudocode: Sets marker at current cursor position

    // set_marker_to_here:
    //     jsr get_line_length
    //     cpy xpos
    //     bcc cad5d
    //     ldy #0
    //     lda (current_format_line_ptr),y
    //     ldy xpos
    //     jsr check_for_command_prefix
    //     bne cad5c
    //     iny
    //     iny
    //     iny
    // cad5c:
    //     tya
    // cad5d:
    //     clc
    //     adc current_line_ptr
    //     sta markers_array+0,x
    //     lda current_line_ptr+1
    //     adc #0
    //     sta markers_array+1,x
    //     rts
}
static void get_register_address(void) {
    // Pseudocode: Gets address of a register value by letter name

    // ; ***************************************************************************************
    // ; On Entry:
    // ;     A: register name
    // ; ***************************************************************************************
    // get_register_address:
    //     jsr is_uppercase
    //     bcs return_77
    //     and #0xdf
    //     pha
    //     sbc #0x40 ; '@'
    //     asl
    //     adc #<register_value_array
    //     sta tmp6
    //     lda #>register_value_array
    //     adc #0
    //     sta tmp7
    //     pla
    //     clc
    // return_77:
    //     rts
}
static void render_register(void) {
    // Pseudocode: Renders the value of a named register to output buffer

    // ; ***************************************************************************************
    // ; On Entry:
    // ;     TMP7/TMP6: pointer to register
    // ;     A: register name
    // ; ***************************************************************************************
    // render_register:
    //     sty l0084
    //     jsr get_register_address
    //     ldy #0
    //     sty tmp8
    //     sty tmp9
    //     zif cc
    //         bit lada6
    //         lda (tmp6),y
    //         sta tmp8
    //         iny                                                               ; Y=0x01
    //         lda (tmp6),y
    //         sta tmp9
    //         jsr render_number_to_output_buffer
    //     zendif
    //     clv
    //     ldy l0084
    //     rts

    // lada6:
    //     .byte 0x40
}
static void sub_cadf0(void) {
    // Pseudocode: Performs 8-bit by 8-bit division for microspacing

    // sub_cadf0:
    //     ldx #8
    //     lda tmp9
    // loop_cadf4:
    //     asl tmp8
    //     rol
    //     cmp l0046
    //     bcc cadff
    //     sbc l0046
    //     inc tmp8
    // cadff:
    //     dex
    //     bne loop_cadf4
    //     rts
}
static void sub_cae06(void) {
    // Pseudocode: Inserts bytes at cursor position, shifting existing content right

    // cae03:
    //     jmp beep

    // sub_cae06:
    //     lda xpos
    //     cmp #0x84
    //     bcs cae03
    //     stx input_buffer_ptr+1
    //     jsr get_line_length
    //     tya
    //     clc
    //     adc input_buffer_ptr+1
    //     bcs cae03
    //     cmp #0x85
    //     bcs cae03
    //     inc l006d
    //     lda current_edit_line_ptr
    //     sta tmp6
    //     lda current_edit_line_ptr+1
    //     sta tmp7
    //     ldy #0x84
    // cae27:
    //     dey
    //     ldx #0
    //     tya
    //     clc
    //     adc input_buffer_ptr+1
    //     bcs cae35
    //     cmp #0x84
    //     bcs cae35
    //     tax
    // cae35:
    //     stx l0081
    // loop_cae37:
    //     jsr sub_ca536
    //     bne cae52
    //     lda l0081
    //     beq cae4b
    //     clc
    //     adc current_edit_line_ptr
    //     sta markers_array,x
    //     lda current_edit_line_ptr+1
    //     adc #0
    //     bne cae4d
    // cae4b:
    //     sta markers_array,x
    // cae4d:
    //     sta markers_array+1,x
    //     jmp loop_cae37

    // cae52:
    //     lda (current_edit_line_ptr),y
    //     sty l0084
    //     ldy l0081
    //     beq cae5c
    //     sta (current_edit_line_ptr),y
    // cae5c:
    //     ldy l0084
    //     cpy xpos
    //     bne cae27
    //     clc
    //     rts
}
static void cae64(void) {
    // Pseudocode: Deletes N bytes at cursor position, shifting existing content left

    // cae64:
    //     stx input_buffer_ptr+1
    //     inc l006d
    //     lda current_edit_line_ptr
    //     sta tmp6
    //     lda current_edit_line_ptr+1
    //     sta tmp7
    //     ldy xpos
    //     tya
    //     clc
    //     adc input_buffer_ptr+1
    //     sta l0084
    // cae78:
    //     jsr sub_ca536
    //     bne cae98
    //     lda #0
    //     cpy l0084
    //     bcc cae91
    //     tya
    //     sbc input_buffer_ptr+1
    //     clc
    //     adc current_edit_line_ptr
    //     sta markers_array,x
    //     lda current_edit_line_ptr+1
    //     adc #0
    //     bne cae93
    // cae91:
    //     sta markers_array,x
    // cae93:
    //     sta markers_array+1,x
    //     jmp cae78

    // cae98:
    //     iny
    //     cpy #0x85
    //     bcc cae78
    //     lda xpos
    //     cmp #0x84
    //     bcs return_78
    //     ldy xpos
    // loop_caea5:
    //     sty l0084
    //     ldx #0x10
    //     tya
    //     clc
    //     adc input_buffer_ptr+1
    //     bcs caeb7
    //     tay
    //     cpy #0x84
    //     bcs caeb7
    //     lda (current_edit_line_ptr),y
    //     tax
    // caeb7:
    //     ldy l0084
    //     txa
    //     sta (current_edit_line_ptr),y
    //     iny
    //     cpy #0x84
    //     bcc loop_caea5
    // return_78:
    //     rts
}
static void sub_caec2(void) {
    // Pseudocode: Finds left margin stop (0x0b) in edit line

    // sub_caec2:
    //     lda ruler_left_stop
    //     beq caed4
    //     ldy #0
    // loop_caec8:
    //     lda (current_edit_line_ptr),y
    //     iny
    //     cmp #0x0b
    //     beq caed4
    //     cpy #0x84
    //     bcc loop_caec8
    //     rts

    // caed4:
    //     clc
    //     rts
}
static void sub_caed6_sub_caedd(void) {
    // Pseudocode: Inserts a left margin stop (0x0b) in edit line

    // sub_caed6:
    //     jsr sub_caec2
    //     bcc caed4
    //     ldy #0
    // sub_caedd:
    //     lda xpos
    //     pha
    //     sty xpos
    //     ldx #1
    //     jsr sub_cae06
    //     bcs caef0
    //     ldy xpos
    //     lda #0x0b
    //     sta (current_edit_line_ptr),y
    //     iny
    // caef0:
    //     pla
    //     sta xpos
    //     rts

    // MULTIPLE ENTRY POINTS: sub_caed6, sub_caedd
}
static void sub_caef4(void) {
    // Pseudocode: Handles margin/folding adjustments when typing at left margin

    // sub_caef4:
    //     lda format_mode_flag
    //     and #0x81
    //     bne caf31
    //     jsr sub_caec2
    //     bcc caf31
    //     jsr get_line_length
    //     lda xpos
    //     sta l0083
    //     sty xpos
    //     jsr sub_ca608
    //     lda l0072
    //     cmp ruler_left_stop
    //     bcc caf19
    //     ldy l0083
    //     sty xpos
    //     inc xpos
    //     bcs caf2a                                                         ; ALWAYS branch

    // caf19:
    //     lda l0083
    //     ldy xpos
    //     cpy l0083
    //     bcs caf28
    //     sec
    //     sbc ruler_left_stop
    //     bcc caf2a
    //     adc xpos
    // caf28:
    //     sta xpos
    // caf2a:
    //     jsr sub_caedd
    //     bcs return_79
    //     inc l0074
    // caf31:
    //     clc
    // return_79:
    //     rts
}
static void draw_previous_word(void) {
    // Pseudocode: Moves cursor back to start of previous word

    // draw_previous_word:
    //     lda current_edit_line_ptr
    //     sta tmp0
    //     lda current_edit_line_ptr+1
    //     sta tmp1
    //     ldy xpos
    //     beq caf55
    // loop_caf3f:
    //     dey
    //     beq caf55
    //     jsr draw_char
    //     dey
    //     cmp #0x20 ; ' '
    //     beq loop_caf3f
    // loop_caf4a:
    //     dey
    //     jsr draw_char
    //     cmp #0x20 ; ' '
    //     beq caf55
    //     dey
    //     bne loop_caf4a
    // caf55:
    //     sty xpos
    //     jsr draw_char
    //     dey
    //     rts
}
static void caf5c_sub_caf5f(void) {
    // Pseudocode: Sets or clears the format_mode_flag bit 7 and updates redraw flag

    // caf5c:
    //     sec
    //     bcs caf60                                                         ; ALWAYS branch

    // sub_caf5f:
    //     clc
    // caf60:
    //     php
    //     lda format_mode_flag
    //     rol format_mode_flag
    //     plp
    //     ror format_mode_flag
    //     cmp format_mode_flag
    //     beq return_80
    //     inc flags_need_redrawing_flag
    // return_80:
    //     rts

    // MULTIPLE ENTRY POINTS: caf5c, sub_caf5f
}
static void deref_and_check_for_command_prefix(void) {
    // Pseudocode: Dereferences pointer and checks for command prefix byte (0x80/0x81)

    // deref_and_check_for_command_prefix:
    //     lda (tmp0),y
    // check_for_command_prefix:
    //     cmp #0x80
    //     beq return_81
    //     cmp #0x81
    //     clc
    // return_81:
    //     rts

    // MULTIPLE ENTRY POINTS: deref_and_check_for_command_prefix, check_for_command_prefix
}
static void system_init(void) {
    // Pseudocode: Initializes system: gets TPA, finds screen driver, gets screen size

    // system_init:
    //     ldy #BIOS_GETTPA
    //     lda #BANK_MAIN
    //     jsr BIOS
    //     stx himem+1
    //     lda #0
    //     sta himem+0

    //     lda #<cpm_ram
    //     sta oshwm+0
    //     lda #>cpm_ram
    //     sta oshwm+1

    //     ldy #BIOS_FINDDRV
    //     lda #DRVID_SCREEN
    //     ldx #0
    //     jsr BIOS
    //     bcs noscreen
    //     sta SCREEN+1
    //     stx SCREEN+2

    //     ldy #SCREEN_GETSIZE
    //     jsr SCREEN
    //     sta screen_width
    //     stx screen_height
    //     rts
}
static void noscreen(void) {
    // Pseudocode: Screen driver not found: displays error and exits

    // noscreen:
    //     jsr print_inline_string
    //     .ascii "No SCREEN\n"
    //     .byte 0
    //     ldy #BDOS_EXIT_PROGRAM
    //     jmp BDOS
}
static void SCREEN(void) {
    // Pseudocode: Trampoline to screen driver (patched at runtime)

    // SCREEN:
    //     jmp 0x1234
}
static void compute_bytes_free(void) {
    // Pseudocode: Computes number of free bytes between top and himem

    // ; ***************************************************************************************
    // compute_bytes_free:
    //     lda himem
    //     sec
    //     sbc top
    //     tax
    //     lda himem+1
    //     sbc top+1
    //     tay
    // return_84:
    //     rts
}
static void initialise_document(void) {
    // Pseudocode: Initializes document state: sets up page, top, rulers, clears markers

    // ; ***************************************************************************************
    // initialise_document:
    //     lda #0
    //     sta printer_driver_name
    //     sta format_mode_flag
    //     sta justifying_flag
    //     sta insert_mode_flag
    //     ldx #(input_buffer_ptr+2 - print_flags)
    // loop_cafe9:
    //     sta print_flags,x
    //     dex
    //     bpl loop_cafe9
    // cafee:
    //     ldx oshwm                                                         ; X and Y contain the address of OSHWM (low, high)
    //     ldy oshwm+1
    //     iny
    //     inx
    //     stx page
    //     bne caffe
    //     iny
    // caffe:
    //     sty page+1
    //     ldy #0
    //     sty file_edit_flags
    //     sty xpos
    //     lda #0xaa
    //     sta (oshwm),y
    //     lda page
    //     sec
    //     sbc #1
    //     sta tmp8
    //     lda page+1
    //     sbc #0
    //     sta tmp9
    //     lda #0x0d
    //     sta (tmp8),y
    //     sta current_line_buffer + 0x89
    //     lda page
    //     sta top
    //     lda page+1
    //     sta top+1
    //     lda #<(current_line_buffer)
    //     sta ptr1
    //     clc
    //     adc #3
    //     sta current_edit_line_ptr
    //     sta current_format_line_ptr
    //     lda #>(current_line_buffer)
    //     sta ptr1+1
    //     adc #0
    //     sta current_edit_line_ptr+1
    //     sta current_format_line_ptr+1
    //     lda #<(current_ruler_buffer)
    //     ldy #>(current_ruler_buffer)
    //     jsr create_default_ruler
    //     iny
    //     lda #0x0d
    //     sta (tmp0),y
    //     ldy #0xff
    //     lda #<(just_before_current_ruler_buffer)
    //     sta (oshwm),y
    //     dey                                                               ; Y=0xfe
    //     lda #>(just_before_current_ruler_buffer)
    //     sta (oshwm),y
    //     jsr move_cursor_to_top_of_document
    //     jsr clear_cmd
}
static void cb05a(void) {
    // Pseudocode: Ensures at least one CR at top of document

    // cb05a:
    //     ldy page+1
    //     cpy top+1
    //     bne return_85
    //     lda page
    //     cmp top
    //     bne return_85
    //     inc top
    //     bne cb06c
    //     inc top+1
    // cb06c:
    //     sta current_line_ptr
    //     sty current_line_ptr+1
    //     ldy #0
    //     lda #0x0d
    //     sta (page),y
    //     tya                                                               ; A=0x00
    //     sta (top),y
    // return_85:
    //     rts
}
static void move_cursor_to_top_of_document(void) {
    // Pseudocode: Moves cursor to the top (page) of the document

    // move_cursor_to_top_of_document:
    //     lda page
    //     sta current_line_ptr
    //     lda page+1
    //     sta current_line_ptr+1
    //     lda #0
    //     sta xpos
    //     ldy #0xfe
    //     sty l0012
    //     sty ruler_stack_ptr
    //     sty l0033
    //     jmp cab91
}
static void clear_cmd(void) {
    // Pseudocode: Clears all markers (sets to zero)

    // ; ***************************************************************************************
    // clear_cmd:
    //     ldx #0x0b
    //     lda #0
    // loop_cb095:
    //     sta markers_array,x
    //     dex
    //     bpl loop_cb095
    //     rts
}
static void enter_editor_mode(void) {
    // Pseudocode: Enters editor mode: clears screen, resets state variables

    // ; ***************************************************************************************
    // enter_editor_mode:
    //     jsr clear_screen
    //     lda #0
    //     sta l006d
    //     sta l006f
    //     sta l006e
    //     ldx screen_height
    // loop_cb0a8:
    //     sta line_lengths,x
    //     dex
    //     bpl loop_cb0a8
    //     ldx #2
    //     stx l0073
    //     stx l0076
    //     rts
}
static void create_default_ruler(void) {
    // Pseudocode: Creates a default ruler with tab stops every 6 columns

    // ; ***************************************************************************************
    // create_default_ruler:
    //     sta tmp0
    //     sty tmp1
    //     lda #0
    //     tay                                                               ; Y=0x00
    // loop_cb0e7:
    //     lda #0x2e ; '.'
    // loop_cb0e9:
    //     sta (tmp0),y
    //     iny
    //     tya
    //     tax
    //     inx
    //     clc
    //     adc #6
    //     cmp screen_width
    //     beq cb0ff
    //     txa
    //     and #7
    //     bne loop_cb0e7
    //     lda #0x2a ; '*'
    //     bne loop_cb0e9                                                    ; ALWAYS branch

    // cb0ff:
    //     lda #0x3c ; '<'
    //     sta (tmp0),y
    //     rts
}
static void sub_cb104(void) {
    // Pseudocode: Resets formatting registers and default print settings

    // sub_cb104:
    //     lda #0
    //     ldx #0x33 ; '3'
    // loop_cb108:
    //     sta register_value_array,x
    //     dex
    //     bpl loop_cb108
    //     sta header_text_maybe
    //     sta footer_text_maybe
    //     sta two_sided_flag
    //     sta left_margin
    //     sta line_spacing
    //     sta rhs_extra_margin
    //     sta macro_executing_flag
    //     lda #0x42 ; 'B'
    //     sta page_length
    //     lda #1
    //     sta footers_enabled_flag
    //     sta headers_enabled_flag
    //     sta l0038
    //     sta register_value_p
    //     sta register_value_l
    //     ldy #0x80
    //     sty highlight1_code
    //     iny                                                               ; Y=0x81
    //     sty highlight2_code
    //     lda #4
    //     sta top_margin
    //     sta bottom_margin
    //     sta header_margin
    //     sta footer_margin
    //     jmp c92f0
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
static void bdos_print_char(void) {
    // Pseudocode: Prints a character to BDOS console output, expanding CR to CR+LF

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
    // ; 'QUIT' -> 0, flag=1
    //     .byte 0x0a, 0x0e, 0x12, 0x0f
    //     .byte 0x81
    // ; 'NEW' -> 1, flag=0
    //     .byte 0x15, 0x1e, 0x0c
    //     .byte 0x80
    // ; 'FORMAT' -> 2, flag=1
    //     .byte 0x1d, 0x14, 9, 0x36, 0x3a, 0x2f
    //     .byte 0x81
    // ; 'SETUP' -> 3, flag=0
    //     .byte 8, 0x1e, 0x0f, 0x2e, 0x2b
    //     .byte 0x80
    // ; 'READ' -> 4, flag=1
    //     .byte 9, 0x1e, 0x3a, 0x3f
    //     .byte 0x81
    // ; 'MORE' -> 5, flag=1
    //     .byte 0x16, 0x14, 0x29, 0x3e
    //     .byte 0x81
    // ; 'SCREEN' -> 6, flag=1
    //     .byte 8, 0x18, 0x29, 0x3e, 0x3e, 0x35
    //     .byte 0x81
    // ; 'SHEETS' -> 7, flag=1
    //     .byte 8, 0x13, 0x3e, 0x3e, 0x2f, 0x28
    //     .byte 0x81
    // ; 'SAVE' -> 8, flag=1
    //     .byte 8, 0x1a, 0x2d, 0x3e
    //     .byte 0x81
    // ; 'COUNT' -> 9, flag=1
    //     .byte 0x18, 0x14, 0x2e, 0x35, 0x2f
    //     .byte 0x81
    // ; 'FIELD' -> 10, flag=1
    //     .byte 0x1d, 0x12, 0x3e, 0x37, 0x3f
    //     .byte 0x81
    // ; 'PRINTER' -> 11, flag=1
    //     .byte 0x0b, 9, 0x12, 0x15, 0x0f, 0x1e, 0x29
    //     .byte 0x81
    // ; 'SEARCH' -> 12, flag=1
    //     .byte 8, 0x3e, 0x3a, 0x29, 0x38, 0x33
    //     .byte 0x81
    // ; 'CLEAR' -> 13, flag=1
    //     .byte 0x18, 0x17, 0x3e, 0x3a, 0x29
    //     .byte 0x81
    // ; 'MICROSPACE' -> 14, flag=1
    //     .byte 0x16, 0x12, 0x38, 0x29, 0x34, 0x28, 0x2b, 0x3a, 0x38, 0x3e
    //     .byte 0x81
    // ; 'FOLD' -> 15, flag=1
    //     .byte 0x1d, 0x14, 0x37, 0x3f
    //     .byte 0x81
    // ; 'NAME' -> 16, flag=1
    //     .byte 0x15, 0x3a, 0x36, 0x3e
    //     .byte 0x81
    // ; 'MODE' -> 17, flag=0
    //     .byte 0x16, 0x34, 0x3f, 0x3e
    //     .byte 0x80
    // ; 'FINISH' -> 18, flag=1
    //     .byte 0x1d, 0x32, 0x35, 0x32, 0x28, 0x33
    //     .byte 0x81
    // ; 'PRINT' -> 19, flag=1
    //     .byte 0x0b, 0x29, 0x32, 0x35, 0x2f
    //     .byte 0x81
    // ; 'CHANGE' -> 20, flag=1
    //     .byte 0x18, 0x33, 0x3a, 0x35, 0x3c, 0x3e
    //     .byte 0x81
    // ; 'WRITE' -> 21, flag=1
    //     .byte 0x0c, 0x29, 0x32, 0x2f, 0x3e
    //     .byte 0x81
    // ; 'EDIT' -> 22, flag=0
    //     .byte 0x1e, 0x3f, 0x32, 0x2f
    //     .byte 0x80
    // ; 'REPLACE' -> 23, flag=1
    //     .byte 9, 0x3e, 0x2b, 0x37, 0x3a, 0x38, 0x3e
    //     .byte 0x81
    // ; 'LOAD' -> 24, flag=0
    //     .byte 0x17, 0x34, 0x3a, 0x3f
    //     .byte 0x80
    // ; 'BYE' -> 25, flag=1
    //     .byte 0x19, 0x22, 0x3e
    //     .byte 0x80
    //     .byte 0
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

    // bdos_print_char:
    //     pha
    //     txa
    //     pha
    //     tya
    //     pha
    //     tsx
    //     lda 0x0103,x
    //     ldy #BDOS_CONSOLE_OUTPUT
    //     jsr BDOS
    //     pla
    //     tay
    //     pla
    //     tax
    //     pla
    //     cmp #13
    //     bne 1f
    //     pha
    //     lda #10
    //     jsr bdos_print_char
    //     pla
    // 1:
    //     rts
}
static void bdos_print_newline(void) {
    // Pseudocode: Prints a newline (CR) to BDOS console output

    // bdos_print_newline:
    //     pha
    //     lda #13
    //     jsr bdos_print_char
    //     pla
    //     rts
}
static void screen_putchar(void) {
    // Pseudocode: Outputs a character to the screen via SCREEN putchar call

    // zproc screen_putchar
    //     pha
    //     txa
    //     pha
    //     tya
    //     pha

    //     tsx
    //     lda 0x103, x
    //     ldy #SCREEN_PUTCHAR
    //     jsr SCREEN

    //     pla
    //     tay
    //     pla
    //     tax
    //     pla
    //     rts
    // zendproc
}
static void readline(void) {
    // Pseudocode: Reads a line from keyboard with editing support (backspace, delete line)

    // ; Read a line from the keyboard input input_buffer.
    // ; This is a hacked and customised copy of the BDOS routine.
    // zproc readline
    //     lda #0
    //     sta input_buffer_ptr

    //     zloop
    //         ; Read a key without echo.

    //         ldy #BDOS_DIRECT_IO
    //         ldx #0xfd
    //         jsr BDOS
    //         tax

    //         ; Delete?

    //         cmp #8
    //         zif eq
    //             lda #127
    //         zendif
    //         cmp #127
    //         zif eq
    //             ldy input_buffer_ptr
    //             zif ne
    //                 dec input_buffer_ptr
    //                 jsr bdos_print_char
    //             zendif
    //             zcontinue
    //         zendif

    //         ; Delete line?

    //         cmp #21
    //         zif eq
    //             ldy input_buffer_ptr
    //             zif ne
    //                 zrepeat
    //                     lda #127
    //                     jsr bdos_print_char
    //                     dey
    //                 zuntil eq
    //                 sty input_buffer_ptr
    //             zendif
    //             zcontinue
    //         zendif

    //         ; Finished?

    //         cmp #13
    //         zbreakif eq
    //         cmp #10
    //         zbreakif eq
    //         cmp #27
    //         zif eq
    //             lda #0
    //             sta input_buffer_ptr
    //             sec
    //             rts
    //         zendif

    //         ; Graphic character?

    //         cmp #32
    //         zif ge
    //             ldy #MAX_COMMAND_LENGTH
    //             cpy input_buffer_ptr
    //             zif cs
    //                 ldy input_buffer_ptr
    //                 sta input_buffer, y
    //                 jsr bdos_print_char
    //                 inc input_buffer_ptr
    //             zendif
    //         zendif
    //     zendloop

    //     jsr bdos_print_newline
    //     ldx input_buffer_ptr
    //     lda #0x0d
    //     sta input_buffer, x
    //     ldx #0
    //     stx input_buffer_ptr
    //     clc
    //     rts
    // zendproc
}
static void select_file(void) {
    // Pseudocode: Sets the file pointer for subsequent file operations

    // zproc select_file
    //     stx file_ptr+0
    //     sty file_ptr+1
    //     rts
    // zendproc
}
static void prepare_to_open_file(void) {
    // Pseudocode: Prepares file structure: zeros FCB, parses filename via BDOS

    // zproc prepare_to_open_file
    //     jsr zero_terminate_filename_buffer

    //     ; Wipe the FCB.

    //     ldy #FCB__SIZE-1
    //     lda #0
    //     zrepeat
    //         sta (file_ptr+0), y
    //         dey
    //     zuntil mi

    //     ; Parse the filename.

    //     ldy #BDOS_SET_DMA_ADDRESS
    //     lda file_ptr+0
    //     ldx file_ptr+1
    //     jsr BDOS

    //     ldy #BDOS_PARSEFILENAME
    //     lda #<filename_buffer
    //     ldx #>filename_buffer
    //     jsr BDOS
    //     zif cs
    //         jmp bad_filename_error
    //     zendif
    //     rts
    // zendproc
}
static void open_input_file(void) {
    // Pseudocode: Opens a file for reading via BDOS open file call

    // ; YX = pointer to file structure
    // ; Input filename is in filename_buffer
    // ; Uses file_ptr+0/file_ptr+1.
    // zproc open_input_file
    //     jsr prepare_to_open_file

    //     ; Delete any existing file.

    //     ldy #BDOS_OPEN_FILE
    //     lda file_ptr+0
    //     ldx file_ptr+1
    //     jsr BDOS
    //     zif cs
    //         jmp file_not_found_error
    //     zendif

    //     ldy #FS_BUFFERPTR
    //     lda #FS_BUFFER+128
    //     sta (file_ptr+0), y
    //     rts
    // zendproc
}
static void open_output_file(void) {
    // Pseudocode: Creates and opens a file for writing via BDOS calls

    // ; YX = pointer to file structure
    // ; Input filename is in filename_buffer
    // ; Uses file_ptr+0/file_ptr+1.
    // zproc open_output_file
    //     jsr prepare_to_open_file

    //     ; Delete any existing file.

    //     ldy #BDOS_DELETE_FILE
    //     lda file_ptr+0
    //     ldx file_ptr+1
    //     jsr BDOS

    //     ; Create a new file.

    //     ldy #FCB_T3+1
    //     lda #0
    //     zrepeat
    //         sta (file_ptr+0), y
    //         iny
    //         cpy #FCB__SIZE
    //     zuntil eq

    //     ldy #BDOS_CREATE_FILE
    //     lda file_ptr+0
    //     ldx file_ptr+1
    //     jsr bdos_and_file_error

    //     ldy #FS_BUFFERPTR
    //     lda #FS_BUFFER
    //     sta (file_ptr+0), y
    //     rts
    // zendproc
}
static void bdos_and_file_error(void) {
    // Pseudocode: Calls BDOS and jumps to file_error if carry is set

    // zproc bdos_and_file_error
    //     jsr BDOS
    //     zif cs
    //         jmp file_not_found_error
    //     zendif
    //     rts
    // zendproc
}
static void set_dma_to_buffer_address_of_file(void) {
    // Pseudocode: Sets BDOS DMA address to file buffer

    // zproc set_dma_to_buffer_address_of_file
    //     clc
    //     lda file_ptr+0
    //     adc #FS_BUFFER
    //     pha
    //     lda file_ptr+1
    //     adc #0
    //     tax
    //     pla
    //     ldy #BDOS_SET_DMA_ADDRESS
    //     jmp BDOS
    // zendproc
}
static void flush_file(void) {
    // Pseudocode: Flushes file buffer to disk by writing sequential block

    // ; FS pointer in file_ptr+0/file_ptr+1.
    // zproc flush_file
    //     jsr set_dma_to_buffer_address_of_file

    //     ldy #BDOS_WRITE_SEQUENTIAL
    //     lda file_ptr+0
    //     ldx file_ptr+1
    //     jsr bdos_and_file_error

    //     ldy #FS_BUFFER
    //     lda #0
    //     zrepeat
    //         sta (file_ptr+0), y
    //         iny
    //         cpy #FS_BUFFER+128
    //     zuntil eq
    //     rts
    // zendproc
}
static void close_file(void) {
    // Pseudocode: Flushes if needed and closes file via BDOS

    // zproc close_file
    //     ldy #FS_BUFFERPTR
    //     lda (file_ptr+0), y
    //     cmp #FS_BUFFER
    //     zif ne
    //         jsr flush_file
    //     zendif

    //     ldy #BDOS_CLOSE_FILE
    //     lda file_ptr+0
    //     ldx file_ptr+1
    //     jmp bdos_and_file_error
    // zendproc
}
static void put_byte_to_file(void) {
    // Pseudocode: Writes a byte to file buffer, flushing when buffer is full

    // zproc put_byte_to_file
    //     pha
    //     ldy #FS_BUFFERPTR
    //     lda (file_ptr+0), y
    //     tay
    //     pla
    //     sta (file_ptr+0), y

    //     iny
    //     cpy #FS_BUFFER + 128
    //     zif eq
    //         jsr flush_file
    //         ldy #FS_BUFFER
    //     zendif
    //     tya
    //     ldy #FS_BUFFERPTR
    //     sta (file_ptr+0), y
    //     rts
    // zendproc
}
static void get_byte_from_file(void) {
    // Pseudocode: Reads a byte from file, refilling buffer when empty, sets carry on EOF

    // ; If the value is 0, returns with C set to signal end of file.
    // zproc get_byte_from_file
    //     ldy #FS_BUFFERPTR
    //     lda (file_ptr+0), y
    //     cmp #FS_BUFFER+128
    //     zif eq
    //         jsr set_dma_to_buffer_address_of_file
    //         ldy #BDOS_READ_SEQUENTIAL
    //         lda file_ptr+0
    //         ldx file_ptr+1
    //         jsr bdos_and_file_error

    //         lda #FS_BUFFER
    //     zendif

    //     tay
    //     lda (file_ptr+0), y
    //     pha
    //     iny
    //     tya
    //     ldy #FS_BUFFERPTR
    //     sta (file_ptr+0), y
    //     pla
    //     clc
    //     tay                     ; set flags
    //     zif eq
    //         sec
    //     zendif
    //     rts
    // zendproc
}

int main(int argc, char* argv[]) {
    // Placeholder
    return 0;
}

