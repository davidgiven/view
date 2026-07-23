#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "cli.h"
#include "printing.h"


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
