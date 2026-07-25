#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "io.h"
#include "printing.h"
#include "document.h"

extern void start_printing(void);

// Printing-only functions
void bad_filename_error(void);
static void c8f29_sub(void);
static void c9263(void);
static void c937b(void);
static void c93b8(void);
void check_not_continuous_editing(void);
void display_not_enough_memory(void);
static void microspace_word_processor(void);
static void nested_macro_error(void);
void parse_decimal_number(void);
void parse_optional_filename_from_command(void);
static void print_char_x_times(void);
void print_document(void);
static void print_loop(void);
static void print_newline(void);
static void print_vertical_space(void);
void read_block_from_file(void);
static void render_header_or_footer(void);
static void render_new_page(void);
void sub_c8e33(void);
static void sub_c916a(void);
static void sub_c9173(void);
static void sub_c9188(void);
static void sub_c9228(void);
static void sub_c9241(void);
static void sub_c92f0(void);
static void sub_c9393(void);
static void sub_c939b(void);
static void sub_c93a1(void);
static void sub_c93b6(void);
static void sub_c93be(void);
static void sub_c93c8(void);
static void sub_c93fd(void);
static void sub_c9407(void);
static void sub_c941a(void);
static void sub_c9431(void);
static void sub_cb104(void);
static void write_byte_to_memory(void);
static void write_cr_to_memory(void);



// Functions from view.c used by printing code
extern void cli_putstring(const char *s);
extern void return_to_cli_prompt(void);
extern void display_not_enough_memory(void);
extern void get_register_address(void);
extern void write_cr_to_memory(void);
extern void adjust_pointers(void);
extern void make_space_for_insertion(void);
extern void compute_required_space_for_insertion(void);
extern void move_cursor_to_address(void);
extern void parse_filename_from_command(void);
extern void c8b7b(void);
extern void c8b78(void);
extern uint8_t check_for_command_prefix(uint8_t ch);
extern void check_for_control_code(void);
extern void render_new_page(void);
extern void c9263(void);
extern void sub_c9241(void);
extern void parse_decimal_number(void);

// Forward declarations within printing.c
static void expand_line(void);
static void c950f_impl(void);
static void sub_c976c(void);
static void parse_boolean_from_fmt_cmd(void);
static void page_eject_fmt(void);
static void evaluate_expression_from_fmt_cmd(void);
static void get_current_fmt_cmd_byte(void);
static void get_next_fmt_cmd_byte(void);
void lookup_formatting_command(void);
static void sub_c95b2(void);
static void c9575(void);
void render_register(void);
static void render_number_to_output_buffer(void);
static void emit_to_output_buffer_callback(void);
static void render_number_to_callback(void);
static const uint8_t lada6 = 0x40;
static void c950f_impl(void) {
    // c950f:
    //     ldy #3
    y = 3;
    //     tax
    x = a;
    set_flags(x);
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
    cmp(a, 0x0d);
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
    set_flags(a);
    //     beq return_36
    if (flags & FLAG_Z) return;
    //     lsr
    { flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N)) | ((a & 1) ? FLAG_C : 0); a >>= 1; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     sta l0084
    l0084 = a;
    //     lda ruler_right_stop
    a = ruler_right_stop;
    set_flags(a);
    //     beq c950f
    if (flags & FLAG_Z) { c950f_impl(); return; }
    //     sec
    flags |= FLAG_C;
    //     sbc ruler_left_stop
    sbc(ruler_left_stop);
    //     lsr
    { flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N)) | ((a & 1) ? FLAG_C : 0); a >>= 1; flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N); }
    //     sec
    flags |= FLAG_C;
    //     adc ruler_left_stop
    adc(ruler_left_stop);
    //     sec
    flags |= FLAG_C;
    //     sbc l0084
    sbc(l0084);
    //     bcs c950f
    if (flags & FLAG_C) { c950f_impl(); return; }
    //     lda #0
    a = 0;
    set_flags(a);
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
    set_flags(a);
    //     beq c9529
    if (flags & FLAG_Z) { flags |= FLAG_C; return; }
    //     dex
    x--;
    //     dex
    x--;
    //     lda #0
    a = 0;
    set_flags(a);
    //     cpx ruler_right_stop
    cmp(x, ruler_right_stop);
    //     bcs c950f
    if (flags & FLAG_C) { c950f_impl(); return; }
    //     stx l0083
    l0083 = x;
    //     lda ruler_right_stop
    a = ruler_right_stop;
    set_flags(a);
    //     sec
    flags |= FLAG_C;
    //     sbc l0083
    sbc(l0083);
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
    set_flags(a);
    //     iny
    y++;
    //     cmp #0x7c ; '|'
    cmp(a, 0x7c);
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
    cmp(a, 0x0d);
    //     beq c9555
    if (flags & FLAG_Z) goto c9555;
    //     cpx #MAX_LINE_LENGTH-1
    cmp(x, MAX_LINE_LENGTH - 1);
    //     bcc c9537
    if (!(flags & FLAG_C)) goto c9537;
    //     lda #0x0d
    a = 0x0d;
    set_flags(a);
    //     bne c953e                                                         ; ALWAYS branch
    goto c953e;

    // c9555:
c9555:
    //     lda print_flags
    a = print_flags;
    set_flags(a);
    //     bpl return_37
    if (!(flags & FLAG_N)) return;
    //     txa
    a = x;
    //     sbc l0083
    sbc(l0083);
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
    set_flags(a);
    //     cmp #0x0d
    cmp(a, 0x0d);
    //     beq c953e
    if (flags & FLAG_Z) goto c953e;
    //     iny
    y++;
    //     jsr render_register
    render_register();
    // advance x past the digits written by render_number_to_output_buffer
    if (l0082 > x) x = l0082;
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
    l0080 = y;
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
    cmp(a, 0x0d);
    //     beq c959c
    if (flags & FLAG_Z) goto c959c;
    //     cmp #0x1b
    cmp(a, 0x1b);
    //     bcs c9598
    if (flags & FLAG_C) goto c9598;
    //     lda #0x20 ; ' '
    a = 0x20;
c9598:
    //     cmp l0083
    cmp(a, l0083);
    //     bne c959e
    if (!(flags & FLAG_Z)) goto c959e;
c959c:
    //     ora #0x80
    a |= 0x80;
c959e:
    //     jsr sub_c95b2
    sub_c95b2();
    //     cmp #0x8d
    cmp(a, 0x8d);
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
    //     (fall through into sub_c95b2)
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
    set_flags(x);
    //     beq page_eject_fmt
    if (flags & FLAG_Z) { page_eject_fmt(); return; }
    //     cmp l0021
    cmp(a, l0021);
    //     bcc return_40
    if (!(flags & FLAG_C)) return;
    //     lda l0031
    a = l0031;
    set_flags(a);
    //     bne page_eject_fmt
    if (!(flags & FLAG_Z)) { page_eject_fmt(); return; }
    // return_40:
    //     rts
    return;
}
static void c9642_tail(void) {
    // c9642:
    //     jsr page_eject_fmt
    page_eject_fmt();
    //     (fall through into page_eject_fmt)
    page_eject_fmt();
}

static void op_fmt_cmd(void) {
    // op_fmt_cmd:
    //     lda register_value_p
    a = ram[RAM_REGISTER_VALUE_P];
    //     lsr
    flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N)) | ((a & 1) ? FLAG_C : 0);
    a >>= 1;
    flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bcc page_eject_fmt
    if (!(flags & FLAG_C)) { page_eject_fmt(); return; }
    //     bcs c9642                                                         ; ALWAYS branch
    c9642_tail(); return;
}
static void ep_fmt_cmd(void) {
    // ep_fmt_cmd:
    //     lda register_value_p
    a = ram[RAM_REGISTER_VALUE_P];
    //     lsr
    flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N)) | ((a & 1) ? FLAG_C : 0);
    a >>= 1;
    flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bcs page_eject_fmt
    if (flags & FLAG_C) { page_eject_fmt(); return; }
    // c9642:
    c9642_tail(); return;
}
static void page_eject_fmt(void) {
    // Pseudocode: Performs page eject by rendering new page and moving to sheet bottom

    //     jsr page_eject_fmt
    // ; ***************************************************************************************
    // page_eject_fmt:
    //     lda l0031
    a = l0031;
    set_flags(a);
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
    set_flags(a);
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
    set_flags(a);
    //     and #0xdf
    a &= 0xdf;
    set_flags(a);
    //     sta l0084
    l0084 = a;
    //     iny                                                               ; Y=0x04
    y++;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    set_flags(a);
    //     jsr is_uppercase
    if (isupper(a)) { flags &= ~FLAG_C; } else { flags |= FLAG_C; }
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
    set_flags(a);
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
    adc((uint8_t)(last_macro_ptr & 0xff));
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
    sbc((uint8_t)(last_macro_ptr & 0xff));
    //     tax
    x = a;
    set_flags(x);
    //     lda himem+1
    a = (uint8_t)((himem >> 8) & 0xff);
    //     sbc last_macro_ptr+1
    sbc((uint8_t)((last_macro_ptr >> 8) & 0xff));
    //     bne c96b8
    if (!(flags & FLAG_Z)) goto c96b8;
    //     cpx #0x97
    cmp(x, 0x97);
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
    l0080 = a;
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
    set_flags(a);
    //     jsr check_for_command_prefix
    flags = check_for_command_prefix(a);
    //     bne c96f8
    if (!(flags & FLAG_Z)) goto c96f8;
    //     jsr lookup_formatting_command
    lookup_formatting_command();
    //     cpx #5
    cmp(x, 5);
    //     bne c96f8
    if (!(flags & FLAG_Z)) goto c96f8;
    //     lda #4
    a = 4;
    //     ldy #0
    y = 0;
    //     sta (last_macro_ptr),y
    ram[last_macro_ptr + y] = a;
    //     inc last_macro_ptr
    last_macro_ptr++;
    //     bne add_macro_to_linked_list
    //     inc last_macro_ptr+1
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

    // c96f8:
c96f8:
    //     lda tmp0
    a = tmp0;
    //     sta last_macro_ptr
    last_macro_ptr = (last_macro_ptr & 0xff00) | a;
    //     lda tmp1
    a = tmp1;
    //     sta last_macro_ptr+1
    last_macro_ptr = (last_macro_ptr & 0x00ff) | ((uint16_t)a << 8);
    //     bne c96a2                                                         ; ALWAYS branch
    goto c96a2;
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
    set_flags(x);
    //     lda #0
    a = 0;
    set_flags(a);
    //     cpx #0x2d ; '-'
    cmp(x, 0x2d);
    //     beq c9716
    if (flags & FLAG_Z) goto c9716;
    //     lda #1
    a = 1;
    set_flags(a);
    //     cpx #0x2a ; '*'
    cmp(x, 0x2a);
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
    sbc(1);
    //     bcc return_44
    if (!(flags & FLAG_C)) return;
    //     cmp #2
    cmp(a, 2);
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
    set_flags(x);
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
void lookup_formatting_command(void) {
    // Pseudocode: Looks up two-letter formatting command in commands_table

    // ; ***************************************************************************************
    // lookup_formatting_command:
    //     ldy #2
    y = 2;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    set_flags(a);
    //     sta tmp3
    tmp3 = a;
    //     dey                                                               ; Y=0x01
    y--;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    set_flags(a);
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
    cmp(a, commands_table[y]);
    //     bne c974c
    if (!(flags & FLAG_Z)) goto c974c;
    //     lda tmp3
    a = tmp3;
    //     cmp lb2a1,y                                                    ; lb2a1 = commands_table+1
    cmp(a, commands_table[y + 1]);
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
    set_flags(a);
    //     bpl loop_c973e
    if (!(flags & FLAG_N)) goto loop_c973e;
    set_flags(a);
    // return_45:
return_45:
    //     rts
    return;
}
void execute_formatting_command(void) {
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
    //     jsr call_through_jumptable (call_through_jumptable_0, y=0)
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
    switch (a) {
        case 0: ce_fmt_cmd(); break;
        case 1: rj_fmt_cmd(); break;
        case 2: df_fmt_cmd(); break;
        case 3: dh_fmt_cmd(); break;
        case 4: dm_fmt_cmd(); break;
        case 5: break;  // return_34 (no-op slot)
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
    //     ldx l0030
    x = l0030;
    set_flags(x);
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
    set_flags(a);
    //     tax
    x = a;
    set_flags(x);
    //     lda #1
    a = 1;
    set_flags(a);
    //     cpx #0x31 ; '1'
    cmp(x, 0x31);
    //     beq c977f
    if (flags & FLAG_Z) goto c977f;
    //     lda #0
    a = 0;
    set_flags(a);
    //     cpx #0x30 ; '0'
    cmp(x, 0x30);
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
    set_flags(a);
    //     jsr to_uppercase
    a = toupper(a);
    //     inx
    x++;
    //     cmp l97b0,x
    cmp(a, l97b0_data[x]);
    //     beq c9788
    if (flags & FLAG_Z) goto c9788;
    //     lda l97b0,x
    a = l97b0_data[x];
    set_flags(a);
    //     bmi c97ae
    if (flags & FLAG_N) goto c97ae;
    //     cmp #0x20 ; ' '
    cmp(a, 0x20);
    //     bcc return_46
    if (!(flags & FLAG_C)) return;
    // loop_c979d:
loop_c979d:
    //     inx
    x++;
    //     lda l97b0,x
    a = l97b0_data[x];
    set_flags(a);
    //     bmi c97ae
    if (flags & FLAG_N) goto c97ae;
    //     cmp #0x20 ; ' '
    cmp(a, 0x20);
    //     bcs loop_c979d
    if (flags & FLAG_C) goto loop_c979d;
    //     ldy l0084
    y = l0084;
    //     lda l97b1,x
    a = l97b0_data[x+1];
    set_flags(a);
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
    l0080 = a;
    // c97c0:
c97c0:
    //     jsr get_current_fmt_cmd_byte
    get_current_fmt_cmd_byte();
    //     beq c9821
    if (flags & FLAG_Z) goto c9821;
    //     cmp #0x7c ; '|'
    cmp(a, 0x7c);
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
    x = l0080;
    set_flags(x);
    //     beq c9804
    if (flags & FLAG_Z) goto c9804;
    //     lda #0
    a = 0;
    set_flags(a);
    //     sta input_buffer_offset+1
    l0080 = a;
    //     dex
    x--;
    set_flags(x);
    //     beq c97f7
    if (flags & FLAG_Z) goto c97f7;
    //     lda tmp4
    a = tmp4;
    //     sec
    flags |= FLAG_C;
    //     sbc tmp8
    sbc(tmp8);
    //     sta tmp8
    tmp8 = a;
    //     lda tmp5
    a = tmp5;
    //     sbc tmp9
    sbc(tmp9);
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
    adc(tmp8);
    //     sta tmp8
    tmp8 = a;
    //     lda tmp5
    a = tmp5;
    //     adc tmp9
    adc(tmp9);
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
    cmp(a, 0x2b);
    //     beq c981c
    if (flags & FLAG_Z) goto c981c;
    //     inx                                                               ; X=0x02
    x++;
    //     cmp #0x2d ; '-'
    cmp(a, 0x2d);
    //     bne c9821
    if (!(flags & FLAG_Z)) goto c9821;
    // c981c:
c981c:
    //     stx input_buffer_offset+1
    l0080 = x;
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
loop:
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    //     cmp #0x0d
    cmp(a, 0x0d);
    //     beq return_47
    if (flags & FLAG_Z) return;
    //     cmp #0x20 ; ' '
    cmp(a, 0x20);
    //     beq get_next_fmt_cmd_byte
    if (!(flags & FLAG_Z)) return;
    // get_next_fmt_cmd_byte:
    //     iny
    y++;
    //     jmp get_current_fmt_cmd_byte
    goto loop;
}
static void get_next_fmt_cmd_byte(void) {
    // get_next_fmt_cmd_byte:
    //     iny
    y++;
    get_current_fmt_cmd_byte();
}

void render_register(void) {
    // render_register:
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
    bit_val(lada6);
    //     lda (tmp6),y
    a = ram[((uint16_t)tmp7 << 8 | tmp6) + y];
    //     sta tmp8
    tmp8 = a;
    //     iny                                                               ; Y=0x01
    y++;
    //     lda (tmp6),y
    a = ram[((uint16_t)tmp7 << 8 | tmp6) + y];
    //     sta tmp9
    tmp9 = a;
    //     jsr render_number_to_output_buffer
    render_number_to_output_buffer();
    // cada2:
cada2:
    //     clv
    flags &= ~FLAG_V;
    //     ldy l0084
    y = l0084;
    //     rts
    return;
}

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
    //     ldy la69b
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
    // (digit value is in a at entry — set by render_number_to_callback)
{   uint8_t digit = a;
    //     txa
    //     pha
    uint8_t saved_x = x;
    //     ldx l0082
    x = l0082;
    //     sta output_buffer,x
    output_buffer[x] = digit;
    //     cpx #MAX_LINE_LENGTH-2
    cmp(x, MAX_LINE_LENGTH - 2);
    //     bcs ca6ae
    if (flags & FLAG_C) goto ca6ae;
    //     inc l0082
    l0082++;
    // ca6ae:
ca6ae:
    //     pla
    //     tax
    x = saved_x;
    //     pla (restore a — but we didn't push it; keep the digit value)
    a = digit; }
    //     rts
    return;
}
void print_char_via_putchar(void) {
    cli_putchar(a);
}
void render_number_to_screen(void) {
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
void bad_filename_error(void) {
    // bad_filename_error:
    //     jsr print_inline_string
    //     .ascii "Bad filename\r"
    //     .byte 0
    cli_putstring("Bad filename\n");
    //     jmp return_to_cli_prompt
    return_to_cli_prompt(); return;
}

static void c8f29_sub(void) {
    // c8f29:
    //     #if 0
    //     lda #0x40 ; '@'
    //     jsr open_file
    //     #endif
    //     sta rw_file_handle
    rw_file_handle = a;
    //     rts (falls through to c8f30 in original 6502)
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
    ram[RAM_REGISTER_VALUE_P]++; set_flags(ram[RAM_REGISTER_VALUE_P]);
    //     bne c928c
    if (!(flags & FLAG_Z)) goto c928c;
    //     inc register_value_p+1
    ram[RAM_REGISTER_VALUE_P+1]++; set_flags(ram[RAM_REGISTER_VALUE_P+1]);
    // c928c:
c928c:
    //     lda #1
    a = 1;
    //     sta register_value_l
    ram[RAM_REGISTER_VALUE_L] = a;
    //     lda #0
    a = 0;
    //     sta register_value_l+1
    ram[RAM_REGISTER_VALUE_L+1] = a;
    //     sta l0031
    l0031 = a;
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

void check_not_continuous_editing(void) {
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

void display_not_enough_memory(void) {
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
    cmp(a, 0x1a);
    //     bne c906f
    if (!(flags & FLAG_Z)) goto c906f;
    //     bit l0083
    bit(l0083);
    //     bpl c9064
    if (!(flags & FLAG_N)) goto c9064;
    //     lda l0048
    a = l0048;
    //     beq c906b
    if (a == 0) goto c906b;
    //     inc l0043
    l0043++; set_flags(l0043);
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
    l0042--; set_flags(l0042);
    // c906f:
c906f:
    //     cmp #0x20 ; ' '
    cmp(a, 0x20);
    //     bcc c9092
    if (!(flags & FLAG_C)) goto c9092;
    //     bne c9090
    if (!(flags & FLAG_Z)) goto c9090;
    //     bit l0083
    bit(l0083);
    //     bpl c9064
    if (!(flags & FLAG_N)) goto c9064;
    //     lda l0042
    a = l0042;
    set_flags(a);
    //     beq c908a
    if (flags & FLAG_Z) goto c908a;
    //     bmi c9087
    if (flags & FLAG_N) goto c9087;
    //     inc l0043
    l0043++; set_flags(l0043);
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
    l0042 = ror(l0042);
    // c908a:
c908a:
    //     inc l0048
    l0048++; set_flags(l0048);
    // c908c:
c908c:
    //     lda #0x20 ; ' '
    a = 0x20;
    //     bne c90b6                                                         ; ALWAYS branch
    goto c90b6;

    // c9090:
c9090:
    //     inc l0046
    l0046++; set_flags(l0046);
    // c9092:
c9092:
    //     cmp #9
    cmp(a, 9);
    //     beq c90a0
    if (flags & FLAG_Z) goto c90a0;
    //     cmp #0x0b
    cmp(a, 0x0b);
    //     beq c90a0
    if (flags & FLAG_Z) goto c90a0;
    //     sec
    flags |= FLAG_C;
    //     ror l0083
    l0083 = ror(l0083);
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
    cmp(a, 0x0d);
    //     beq c90e2
    if (flags & FLAG_Z) goto c90e2;
    //     cmp #0x20 ; ' '
    cmp(a, 0x20);
    //     beq c9048
    if (flags & FLAG_Z) goto c9048;
    //     lda l0048
    a = l0048;
    //     beq c9048
    if (a == 0) goto c9048;
    //     clc
    flags &= ~FLAG_C;
    //     adc l0044
    adc(l0044);
    //     sta l0044
    l0044 = a;
    //     lda l0046
    a = l0046;
    //     adc l0048
    adc(l0048);
    //     sta l0046
    l0046 = a;
    //     lda l0045
    a = l0045;
    //     adc l0043
    adc(l0043);
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
    set_flags(a);
    //     beq c90f8
    if (flags & FLAG_Z) goto c90f8;
    //     lda ruler_right_stop
    a = ruler_right_stop;
    set_flags(a);
    //     beq c90f8
    if (flags & FLAG_Z) goto c90f8;
    //     sec
    flags |= FLAG_C;
    //     sbc l0047
    sbc(l0047);
    //     bcc c90f8
    if (!(flags & FLAG_C)) goto c90f8;
    //     sbc l0045
    sbc(l0045);
    //     adc #0
    adc(0);
    //     sec
    flags |= FLAG_C;
    //     sbc l0046
    sbc(l0046);
    //     beq c9101
    if (flags & FLAG_Z) goto c9101;
    // c90f8:
c90f8:
    //     lda #0
    a = 0;
    //     sta l0039
    l0039 = a;
    //     ldy input_buffer_offset+1
    y = l0080;
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
    { uint8_t c = (a & 0x80) ? FLAG_C : 0; a <<= 1; flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N)) | c; set_flags(a); }
    //     rol tmp9
    tmp9 = rol(tmp9);
    //     asl l0045
    { uint8_t c = (l0045 & 0x80) ? FLAG_C : 0; l0045 <<= 1; flags = (flags & ~(FLAG_C|FLAG_Z|FLAG_N)) | c; set_flags(l0045); }
    //     bcc c9115
    if (!(flags & FLAG_C)) goto c9115;
    //     clc
    flags &= ~FLAG_C;
    //     adc microspacing_flag
    adc(microspacing_flag);
    //     bcc c9115
    if (!(flags & FLAG_C)) goto c9115;
    //     inc tmp9
    tmp9++; set_flags(tmp9);
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
    cmp(a, l0047);
    //     beq c913b
    if (flags & FLAG_Z) goto c913b;
    //     bcs c9142
    if (flags & FLAG_C) goto c9142;
    // c913b:
c913b:
    //     pla
    a = saved_a3;
    //     jsr c9426
    print_char_x_times();
    //     jmp c9163
    goto c9163;

    // c9142:
c9142:
    //     pla
    a = saved_a3;
    //     cmp #0x20 ; ' '
    cmp(a, 0x20);
    //     bne c915b
    if (!(flags & FLAG_Z)) goto c915b;
    //     lda microspacing_flag
    a = microspacing_flag;
    //     clc
    flags &= ~FLAG_C;
    //     adc l0044
    adc(l0044);
    //     tax
    x = a;
    //     lda l0045
    a = l0045;
    //     beq c9154
    if (a == 0) goto c9154;
    //     inx
    x++;
    //     dec l0045
    l0045--; set_flags(l0045);
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
    cmp(a, 0x0d);
    //     bne c912b
    if (!(flags & FLAG_Z)) goto c912b;
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
    print_char_x_times();
    //     cmp #0x0d
    cmp(a, 0x0d);
    //     bne c8fe6_inline
    if (!(flags & FLAG_Z)) goto c8fe6_inline;
    //     inc register_value_l
    ram[RAM_REGISTER_VALUE_L]++; set_flags(ram[RAM_REGISTER_VALUE_L]);
    //     bne c8ffb_inline
    if (!(flags & FLAG_Z)) goto c8ffb_inline;
    //     inc register_value_l+1
    ram[RAM_REGISTER_VALUE_L+1]++; set_flags(ram[RAM_REGISTER_VALUE_L+1]);
c8ffb_inline:
    //     ldx line_spacing
    x = line_spacing;
    //     lda l0021
    a = l0021;
    //     clc
    flags &= ~FLAG_C;
    //     sbc line_spacing
    sbc(line_spacing);
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

void parse_decimal_number(void) {
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
    set_flags(had_digits);
}

void parse_optional_filename_from_command(void) {
    // Pseudocode: Parses optional filename from input buffer into filename_buffer

    // parse_optional_filename_from_command:
    //     jsr sub_c8e33
    sub_c8e33();
    //     beq return_19
    if (flags & FLAG_Z) return;  // returns Z=1 → no filename
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
    // buffer full → bad_filename_error (does not return)
    bad_filename_error();
    // c8e25:
c8e25:
    //     lda #0x0d
    a = 0x0d;
    set_flags(a);
    //     sta filename_buffer,x
    filename_buffer[x] = a;
    //     sty input_buffer_offset
    input_buffer_offset = y;
    // return_20:
    //     rts
}

static void print_char_x_times(void) {
    // c9426: Print character in A, X times. If X==0, return immediately.
    //     inx
    x++;
    //     dex
    x--;
    set_flags(x);
    //     beq return_32
    if (flags & FLAG_Z) goto return_32;
    // loop_c942a:
loop_c942a:
    //     jsr print_char
    print_char();
    //     dex
    x--;
    set_flags(x);
    //     bne loop_c942a
    if (!(flags & FLAG_Z)) goto loop_c942a;
    // return_32:
return_32:
    //     rts
    return;
}

void print_document(void) {
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
    adc(3);
    //     sta ptr5
    ptr5 = (ptr5 & 0xff00) | a;
    //     tax
    x = a;
    //     lda top+1
    a = (uint8_t)(top >> 8);
    //     adc #0
    adc(0);
    //     sta ptr5+1
    ptr5 = (ptr5 & 0x00ff) | ((uint16_t)a << 8);
    //     tay
    y = a;
    //     txa
    a = x;
    //     adc #0x8d
    adc(0x8d);
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
    a = (uint8_t)(RAM_CURRENT_RULER_BUF & 0xff);
    //     sta current_ruler_ptr
    current_ruler_ptr = (current_ruler_ptr & 0xff00) | a;
    //     lda #>(current_ruler_buffer)
    a = (uint8_t)(RAM_CURRENT_RULER_BUF >> 8);
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
    if (a != 0) { print_loop(); goto c8f0d; }
    // c8f0a:
c8f0a:
    // c8f0d:
c8f0d:
    //     jsr parse_optional_filename_from_command
    parse_optional_filename_from_command();
    //     bne c8f29
    if (!(flags & FLAG_Z)) { c8f29_sub(); print_loop(); goto c8f0d; }
    //     lda l0031
    a = l0031;
    set_flags(a);
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
}

static void print_loop(void) {
    // c8f30:
    while (1) {
        //     lda l0031
        a = l0031;
        set_flags(a);
        //     beq c8f3b
        if (flags & FLAG_Z) goto c8f3b_l;
        //     lda l0021
        a = l0021;
        set_flags(a);
        //     bne c8f3b
        if (!(flags & FLAG_Z)) goto c8f3b_l;
        //     jsr c9263
        c9263();
        // c8f3b:
c8f3b_l:
        //     jsr sub_c9188
        sub_c9188();
        //     bcs c8f0a
        if (flags & FLAG_C) return;
        //     jsr sub_c916a
        sub_c916a();
        //     ldy #0
        y = 0;
        //     sty input_buffer_ptr+1
        l0080 = y;
        //     jsr deref_and_check_for_command_prefix
        flags = deref_and_check_for_command_prefix();
        //     bne c8fce_thunk
        if (!(flags & FLAG_Z)) goto c8fce_thunk_l;
        //     ldy #3
        y = 3;
        //     sty input_buffer_ptr+1
        l0080 = y;
        //     jsr sub_cab6e
        sub_cab6e();
        //     bne c8f6e
        if (!(flags & FLAG_Z)) goto c8f6e_l;
        //     ldy #3
        y = 3;
        //     ldx #0
        x = 0;
        // loop_c8f5d:
loop_c8f5d_l:
        //     lda (tmp0),y
        a = ram[((uint16_t)tmp1 << 8 | tmp0) + y];
        //     sta current_ruler_buffer,x
        current_ruler_buffer[x] = a;
        //     iny
        y++;
        //     inx
        x++;
        //     cmp #0x0d
        cmp(a, 0x0d);
        //     bne loop_c8f5d
        if (!(flags & FLAG_Z)) goto loop_c8f5d_l;
        //     jsr find_margins_of_current_ruler_buffer
        find_margins_of_current_ruler_buffer();
        // c8f6b:
c8f6b_l:
        //     jmp c900e
        continue;

        // c8f6e:
c8f6e_l:
        //     jsr lookup_formatting_command
        lookup_formatting_command();
        //     bmi c8f7a
        if (flags & FLAG_N) goto c8f7a_l;
        //     jsr execute_formatting_command
        execute_formatting_command();
        //     beq c8f6b
        if (flags & FLAG_Z) goto c8f6b_l;
        // c8fce_thunk:
c8fce_thunk_l:
        //     bne c8fce                                                         ; ALWAYS branch
        goto c8fce_l;

        // c8f7a:
c8f7a_l:
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
        // (is_uppercase returns C=0 for A-Z/a-z, C=1 otherwise)
        if (a >= 'A' && a < '[') { flags &= ~FLAG_C; }
        else if (a >= 'a' && a < '{') { flags &= ~FLAG_C; }
        else { flags |= FLAG_C; }
        //     bcc c8f92
        if (!(flags & FLAG_C)) goto c8f92_l;
        //     lda #0x20 ; ' '
        a = 0x20;
        // c8f92:
c8f92_l:
        //     sta tmp9
        tmp9 = a;
        // lookup_macro_name:
lookup_macro_name_l:
        //     ldy #0
        y = 0;
        //     lda (tmp6),y
        a = ram[((uint16_t)tmp7 << 8 | tmp6) + y];
        set_flags(a);
        //     beq c8f6b
        if (flags & FLAG_Z) goto c8f6b_l;
        //     ldy #2
        y = 2;
        //     lda (tmp6),y
        a = ram[((uint16_t)tmp7 << 8 | tmp6) + y];
        //     cmp tmp8
        cmp(a, tmp8);
        //     bne get_next_macro_in_linked_list
        if (!(flags & FLAG_Z)) goto get_next_macro_in_linked_list_l;
        //     iny                                                               ; Y=0x03
        y++;
        //     lda (tmp6),y
        a = ram[((uint16_t)tmp7 << 8 | tmp6) + y];
        //     cmp tmp9
        cmp(a, tmp9);
        //     beq c8fb9
        if (flags & FLAG_Z) goto c8fb9_l;
        // get_next_macro_in_linked_list:
get_next_macro_in_linked_list_l:
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
        goto lookup_macro_name_l;

        // c8fb9:
c8fb9_l:
        //     lda macro_executing_flag
        a = macro_executing_flag;
        set_flags(a);
        //     bne nested_macro_error
        if (!(flags & FLAG_Z)) { nested_macro_error(); return; }
        //     lda tmp6
        a = tmp6;
        //     clc
        flags &= ~FLAG_C;
        //     adc #4
        adc(4);
        //     sta ptr3
        ptr3 = (ptr3 & 0xff00) | a;
        //     lda tmp7
        a = tmp7;
        //     adc #0
        adc(0);
        //     sta ptr3+1
        ptr3 = (ptr3 & 0x00ff) | ((uint16_t)a << 8);
        //     sta macro_executing_flag
        macro_executing_flag = a;
        //     bne c900e
        set_flags(macro_executing_flag);
        if (!(flags & FLAG_Z)) continue;
        // c8fce:
c8fce_l:
        //     lda l0031
        a = l0031;
        set_flags(a);
        //     bne c8fd5
        if (!(flags & FLAG_Z)) goto c8fd5_l;
        //     jsr render_new_page
        render_new_page();
        // c8fd5:
c8fd5_l:
        //     jsr sub_c9407
        sub_c9407();
        //     lda #0
        a = 0;
        //     sta l0039
        l0039 = a;
        //     ldy input_buffer_ptr+1
        y = l0080;
        //     lda print_flags
        a = print_flags;
        set_flags(a);
        //     bpl c8fe6
        if (!(flags & FLAG_N)) goto c8fe6_l;
        //     lda microspacing_flag
        a = microspacing_flag;
        set_flags(a);
        //     bne c9034
        if (!(flags & FLAG_Z)) { microspace_word_processor(); continue; }
        // c8fe6:
c8fe6_l:
        //     lda (tmp0),y
        a = ram[((uint16_t)tmp1 << 8 | tmp0) + y];
        //     iny
        y++;
        //     jsr sub_c9431
        sub_c9431();
        //     jsr c9426
        print_char_x_times();
        //     cmp #0x0d
        cmp(a, 0x0d);
        //     bne c8fe6
        if (!(flags & FLAG_Z)) goto c8fe6_l;
        //     inc register_value_l
        ram[RAM_REGISTER_VALUE_L]++; set_flags(ram[RAM_REGISTER_VALUE_L]);
        //     bne c8ffb
        if (!(flags & FLAG_Z)) goto c8ffb_l;
        //     inc register_value_l+1
        ram[RAM_REGISTER_VALUE_L+1]++; set_flags(ram[RAM_REGISTER_VALUE_L+1]);
        // c8ffb:
c8ffb_l:
        //     ldx line_spacing
        x = line_spacing;
        //     lda l0021
        a = l0021;
        //     clc
        flags &= ~FLAG_C;
        //     sbc line_spacing
        sbc(line_spacing);
        //     bcs c9009
        if (flags & FLAG_C) goto c9009_l;
        //     lda #0
        a = 0;
        //     ldx l0021
        x = l0021;
        //     dex
        x--;
        // c9009:
c9009_l:
        //     sta l0021
        l0021 = a;
        //     jsr print_vertical_space
        print_vertical_space();
        // c900e:
        //     jmp c8f30
    }
}

static void print_newline(void) {
    // print_newline:
    //     lda #0x0d
    a = 0x0d;
    print_char();
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
    print_char_x_times();
}

void read_block_from_file(void) {
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
    cmp(a, 0x7f);
    //     bcc c8caf
    if (!(flags & FLAG_C)) goto c8caf;
    //     ldx l0084
    x = l0084;
    //     bne c8c95
    if (x != 0) goto c8c95;
    //     jsr check_for_command_prefix
    flags = check_for_command_prefix(a);
    //     bne c8c95
    if (!(flags & FLAG_Z)) goto c8c95;
    //     ldx #0xfd
    x = 0xfd;
    //     stx l0083
    l0083 = x;
c8caf:
    // c8caf:
    //     cmp #0x20 ; ' '
    cmp(a, 0x20);
    //     bcs c8cc8
    if (flags & FLAG_C) goto c8cc8;
    //     jsr check_for_control_code
    check_for_control_code();
    //     beq c8cc8
    if (flags & FLAG_Z) goto c8cc8;
    //     cmp #0x1a
    cmp(a, 0x1a);
    //     beq c8cc8
    if (flags & FLAG_Z) goto c8cc8;
    //     cmp #0x0d
    cmp(a, 0x0d);
    //     beq c8cc8
    if (flags & FLAG_Z) goto c8cc8;
    //     cmp #0x0b
    cmp(a, 0x0b);
    //     beq c8cc8
    if (flags & FLAG_Z) goto c8cc8;
    //     cmp #9
    cmp(a, 9);
    //     bne c8c95
    if (!(flags & FLAG_Z)) goto c8c95;
c8cc8:
    // c8cc8:
    //     ldx #1
    x = 1;
    //     cmp #0x0d
    cmp(a, 0x0d);
    //     beq c8cdb
    if (flags & FLAG_Z) goto c8cdb;
    //     dex
    x--;
    set_flags(x);
    //     ldy l0083
    y = l0083;
    //     cpy #0x84
    cmp(y, MAX_LINE_LENGTH);
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
    set_flags(x);
c8cdb:
    // c8cdb:
    //     inc l0083
    l0083++;
    set_flags(l0083);
    //     jsr write_byte_to_memory
    write_byte_to_memory();
    //     txa
    a = x;
    set_flags(x);
    //     beq c8c95
    if (flags & FLAG_Z) goto c8c95;
    //     lda tmp1
    a = tmp1;
    //     cmp l0081
    cmp(a, l0081);
    //     bcc c8c95
    if (!(flags & FLAG_C)) goto c8c95;
    //     bne c8cf1
    if (!(flags & FLAG_Z)) goto c8cf1;
    //     lda tmp0
    a = tmp0;
    //     cmp input_buffer_offset+1
    cmp(a, l0080);
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

static void render_new_page(void) {
    // Pseudocode: Renders a new page with headers, margins, page number prompt

    // ; ***************************************************************************************
    // render_new_page:
    //     lda #0x81
    a = 0x81;
    //     sta l0031
    l0031 = a;
    //     bit print_flags
    bit(print_flags);
    //     bvc c92d4
    if (!(flags & FLAG_V)) goto c92d4;
    //     jsr stop_printing
    stop_printing();
    //     jsr print_inline_string
    //     .ascii "\rPage "
    //     .byte 0
    cli_putstring("\nPage ");

    //     ldx register_value_p
    x = ram[RAM_REGISTER_VALUE_P];
    //     ldy register_value_p+1
    y = ram[RAM_REGISTER_VALUE_P+1];
    //     jsr render_number_to_screen
    render_number_to_screen();
    //     jsr print_inline_string
    //     .ascii ".."
    //     .byte 0
    cli_putstring("..");

    //     jsr flush_and_read_char
    read_char();
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

void sub_c8e33(void) {
    // sub_c8e33:
    //     lda l007e
    a = l007e;
    //     cmp #0x0d
    cmp(a, 0x0d);
    //     beq return_20
    if (flags & FLAG_Z) return;
    //     ldy input_buffer_offset
    y = input_buffer_offset;
    // loop_c8e3b:
    while (1) {
        //     lda input_buffer,y
        a = input_buffer[y];
        //     cmp #0x0d
        cmp(a, 0x0d);
        //     beq return_20
        if (flags & FLAG_Z) return;
        //     cmp l007e
        cmp(a, l007e);
        //     bne return_20
        if (!(flags & FLAG_Z)) return;
        //     iny
        y++;
        //     bne loop_c8e3b
        if (y == 0) break;
    }
    set_flags(y);
    //     rts (falls through to check_not_continuous_editing in 6502)
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
}

static void sub_c9188(void) {
    // c9184:
    //     lda #0
    //     sta macro_executing_flag
    // (handled inline below)

    // sub_c9188:
    //     lda macro_executing_flag
    a = macro_executing_flag;
    //     bne c91a3
    if (a != 0) goto c91a3;
c9188_normal_entry:
    //     lda ptr5
    a = (uint8_t)(ptr5 & 0xff);
    //     sta input_buffer_ptr+1
    l0080 = a;
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
    if (a == 4) { macro_executing_flag = 0; goto c9188_normal_entry; }
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
    //     cpx #0x83
    //     bcc c91a7
    if (x < 0x83) goto c91a7;
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
    adc((uint8_t)(ptr3 & 0xff));
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
    return;

    // c91da:
c91da:
    //     iny
    y++;
    //     lda (ptr3),y
    a = ram[ptr3 + y];
    //     sec
    flags |= FLAG_C;
    //     sbc #0x30 ; '0'
    sbc(0x30);
    //     bcc c9225
    if (!(flags & FLAG_C)) goto c9225;
    //     cmp #0x0a
    cmp(a, 0x0a);
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
    l0083--; set_flags(l0083);
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
    //     cpx #0x82
    //     bcc c9209
    if (x < 0x82) goto c9209;
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
    cmp(a, 0x3e);
    if (!(flags & FLAG_Z)) goto c9231;
    //     lda #0
    a = 0;
    //     sta l0082
    l0082 = a;
    set_flags(a);
    //     rts
    return;

    // c9231:
c9231:
    //     cmp #0x3c ; '<'
    //     bne c923c
    cmp(a, 0x3c);
    if (!(flags & FLAG_Z)) goto c923c;
    //     lda #0x40 ; '@'
    a = 0x40;
    //     sta l0082
    l0082 = a;
    //     lda #0
    a = 0;
    set_flags(a);
    //     rts
    return;

    // c923c:
c923c:
    //     bit l0082
    bit(l0082);
    //     ora #0
    a |= 0;
    set_flags(a);
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
    ptr6++;
    //     bne c9254
    //     inc ptr6+1
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
    cmp(a, 0x0d);
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

static void sub_c92f0(void) {
    // sub_c92f0: Computes remaining lines on page = page_length minus margins
    // On exit: l0021 = result (at least 1 if any margin computation underflows)

    // c92f0:
    //     ldx page_length
    x = page_length;
    //     lda l0038
    a = l0038;
    set_flags(a);
    //     beq c930d
    if (flags & FLAG_Z) goto c930d;
    //     ldx #1
    x = 1;
    //     lda page_length
    a = page_length;
    //     clc
    flags &= ~FLAG_C;
    //     sbc top_margin
    sbc(top_margin);
    //     bcc c930d
    if (!(flags & FLAG_C)) goto c930d;
    //     sbc header_margin
    sbc(header_margin);
    //     bcc c930d
    if (!(flags & FLAG_C)) goto c930d;
    //     clc
    flags &= ~FLAG_C;
    //     sbc bottom_margin
    sbc(bottom_margin);
    //     bcc c930d
    if (!(flags & FLAG_C)) goto c930d;
    //     sbc footer_margin
    sbc(footer_margin);
    //     bcc c930d
    if (!(flags & FLAG_C)) goto c930d;
    //     tax
    x = a;
    // c930d:
c930d:
    //     stx l0021
    l0021 = x;
    //     rts
    return;
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

static void sub_c93be(void) {
    // Pseudocode: Returns ruler_right_stop or l003a-1 as the line width

    // sub_c93be:
    //     lda ruler_right_stop
    a = ruler_right_stop;
    set_flags(a);
    //     bne return_29
    if (!(flags & FLAG_Z)) goto return_29;
    //     lda l003a
    a = l003a;
    //     sec
    flags |= FLAG_C;
    //     sbc #1
    sbc(1);
    // return_29:
return_29:
    ; // fallthrough to rts
}

static void sub_c93c8(void) {
    // Pseudocode: Copies header/footer text to output_buffer, expanding register references

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
    a = ram[((uint16_t)tmp3 << 8 | tmp2) + y];
    set_flags(a);
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
    cmp(a, 0x7c);
    //     beq c93f2
    if (flags & FLAG_Z) goto c93f2;
    //     sta output_buffer,x
    output_buffer[x] = a;
    //     inx
    x++;
    //     cpx #MAX_LINE_LENGTH
    cmp(x, MAX_LINE_LENGTH);
    //     bcc c93ce
    if (!(flags & FLAG_C)) goto c93ce;
    // c93e6:
c93e6:
    //     stx l0084
    l0084 = x;
    //     lda print_flags
    a = print_flags;
    set_flags(a);
    //     bpl return_30
    if (!(flags & FLAG_N)) goto return_30;
    //     txa
    a = x;
    //     sec
    flags |= FLAG_C;
    //     sbc l0081
    sbc(l0081);
    //     tax
    x = a;
    // return_30:
return_30:
    //     rts
    return;

    // c93f2:
c93f2:
    //     lda (tmp2),y
    a = ram[((uint16_t)tmp3 << 8 | tmp2) + y];
    set_flags(a);
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
    set_flags(a);
    //     beq return_31
    if (flags & FLAG_Z) goto return_31;
    //     lda register_value_p
    a = ram[RAM_REGISTER_VALUE_P];
    //     lsr
    flags = (flags & ~FLAG_C) | (a & 1);
    a >>= 1;
    set_flags(a);
    // return_31:
return_31:
    ; // fallthrough to rts
}

static void sub_c9407(void) {
    // Pseudocode: Outputs left margin spaces, adjusting for two-sided printing

    // sub_c9407:
    //     jsr sub_c93fd
    sub_c93fd();
    //     lda left_margin
    a = left_margin;
    //     bcc c9415
    if (!(flags & FLAG_C)) goto c9415;
    //     ldx two_sided_flag
    x = two_sided_flag;
    set_flags(x);
    //     beq c9415
    if (flags & FLAG_Z) goto c9415;
    //     clc
    flags &= ~FLAG_C;
    //     adc rhs_extra_margin
    adc(rhs_extra_margin);
    // c9415:
c9415:
    //     tax
    x = a;
    //     lda #0x20 ; ' '
    a = 0x20;
    //     bne c9426                                                         ; ALWAYS branch
    print_char_x_times();
}

static void sub_c941a(void) {
    // Pseudocode: Adds extra spaces to x position for centering/justification

    // sub_c941a:
    //     txa
    a = x;
    //     clc
    flags &= ~FLAG_C;
    //     adc l0039
    adc(l0039);
    //     sta l0039
    l0039 = a;
    //     lda #0x20 ; ' '
    a = 0x20;
    //     bne c9426                                                         ; ALWAYS branch
    print_char_x_times();
}

static void sub_c9431(void) {
    // Pseudocode: Converts character for printing, updates x position counter

    // sub_c9431:
    //     jsr sub_ca5ae
    process_document_character();
    //     bit print_flags
    bit(print_flags);
    //     bpl c943c
    if (!(flags & FLAG_N)) goto c943c;
    //     ora #0
    a |= 0;
    set_flags(a);
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
    adc(l0039);
    //     sta l0039
    l0039 = a;
    //     pla
    a = saved_a; }
// return_33:
return_33:
    //     rts
    return;
}

static void sub_cb104(void) {
    // Pseudocode: Resets formatting registers and default print settings

    // sub_cb104:
    //     lda #0
    a = 0;
    //     ldx #0x33 ; '3'
    // loop_cb108:
    //     sta register_value_array,x
    memset(register_value_array, 0, 26 * 2);
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
    ram[RAM_REGISTER_VALUE_P] = a;
    //     sta register_value_l
    ram[RAM_REGISTER_VALUE_L] = a;
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
// main is now the function above (inlined from main_)


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

