#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "io.h"
#include "printing.h"
#include "document.h"
#include "cli.h"

// Forward decls for default printer driver
static void default_print_char(uint8_t a);
static void default_printer_on(void);
static void default_printer_off(void);
static void default_printer_microspace(void);
static void default_printer_getflags(void);
static const struct printer_driver default_printer_driver;

// Printing-only functions
void bad_filename_error(void);
static void c8f29_sub(uint8_t a);
static void process_page_footer(void);
static void print_output_buffer(void);
static uint8_t scan_string_length(uint8_t y_start, addr_t tmp45);
void check_not_continuous_editing(void);
void display_not_enough_memory(void);
static void microspace_word_processor(void);
static void nested_macro_error(void);
void parse_decimal_number(void);
bool parse_optional_filename_from_command(struct scan_state* scan);
static void print_char_x_times(uint8_t a, uint8_t x);
void print_document(struct scan_state* scan);
static void print_loop(void);
static void print_newline(void);
static void print_vertical_space(uint8_t x);
void read_block_from_file(void);
static void render_header_or_footer(uint8_t y);
static void render_new_page(void);
bool scan_input_buffer(struct scan_state* state);
static void start_microspacing_if_active(uint8_t a);
static void emit_microspacing_spaces(uint8_t x);
static void prepare_output_line(void);
static void parse_register_reference(uint8_t a);
static void read_next_output_line(void);
static void compute_lines_remaining_on_page(void);
static void compute_header_left_section(addr_t tmp45);
static void compute_header_middle_section(addr_t tmp45);
static void compute_header_odd_page_section(addr_t tmp45);
static uint8_t get_line_width(addr_t tmp45);
static uint8_t get_right_margin(void);
static void copy_header_footer_text(addr_t tmp23);
static void get_page_parity(void);
static void output_left_margin(void);
static uint8_t add_justification_spaces(uint8_t x);
static void convert_char_for_printing(void);
static void reset_print_registers(void);
static void write_byte_to_memory(uint8_t a);
static void write_cr_to_memory(void);

// Functions from view.c used by printing code

// Forward declarations within printing.c
static void expand_line(void);
static void c950f_impl(uint8_t a);
static void parse_word_flag(void);
static void parse_boolean_from_fmt_cmd(void);
static void page_eject_fmt(void);
static void evaluate_expression_from_fmt_cmd(void);
static void get_current_fmt_cmd_byte(void);
static void get_next_fmt_cmd_byte(uint8_t y);
void lookup_formatting_command(void);
static void store_to_output_buffer(uint8_t a, addr_t tmp23);
static uint8_t process_header_footer_line(uint8_t x, uint8_t y);
void render_register(uint8_t a, uint8_t y);
static void render_number_to_output_buffer(uint16_t value);
static void emit_to_output_buffer_callback(uint8_t digit);
static void render_number_to_callback(uint16_t value, void (*cb)(uint8_t));
static const uint8_t lada6 = 0x40;

static void c950f_impl(uint8_t a)
{
    // c950f_impl
    // c950f:
    //     ldy #3
    uint8_t y;
    uint8_t x;
    y = 3;
    //     tax
    x = a;

    //     beq c951c
    if (x == 0)
        goto c951c;
    //     lda #0x20 ; ' '
    a = 0x20;
    // loop_c9516:
    //     sta (current_format_line_ptr),y
    do
    {
        ram[current_format_line_ptr + y] = a;
        y++;
        x--;
    } while (x != 0);
c951c:
    do
    {
        a = output_buffer[x];
        ram[current_format_line_ptr + y] = a;
        y++;
        x++;
    } while (a != 0x0d);
    //     inc l0030
    l0030++;
    // c9529:
    //     sec
    flags |= FLAG_C;
    // return_36:
    //     rts
    return;
}

static void lj_fmt_cmd(void)
{
    // lj_fmt_cmd
    // Pseudocode: Left-justifies the current format line

    // ;
    // ***************************************************************************************
    // lj_fmt_cmd:
    //     jsr expand_line
    expand_line();
    //     bcc return_36
    if (!(flags & FLAG_C))
        return;
    //     lda #0
    //     beq c950f                                                         ;
    //     ALWAYS branch
    c950f_impl(0);
    return;
}

static void ce_fmt_cmd(void)
{
    // ce_fmt_cmd
    // Pseudocode: Centers the current format line

    // ;
    // ***************************************************************************************
    // ce_fmt_cmd:
    //     jsr expand_line
    expand_line();
    //     bcc return_36
    if (!(flags & FLAG_C))
        return;
    //     txa
    uint8_t a = x;

    //     beq return_36
    if (a == 0)
        return;
    //     lsr
    {
        flags = (flags & ~(FLAG_C | FLAG_Z | FLAG_N)) | ((a & 1) ? FLAG_C : 0);
        a >>= 1;
        flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    }
    //     sta l0084
    l0084 = a;
    //     lda ruler_right_stop
    a = ruler_right_stop;

    //     beq c950f
    if (a == 0)
    {
        c950f_impl(a);
        return;
    }
    //     sec
    flags |= FLAG_C;
    //     sbc ruler_left_stop
    a = sbc(&flags, a, ruler_left_stop); // none live
    //     lsr
    {
        flags = (flags & ~(FLAG_C | FLAG_Z | FLAG_N)) | ((a & 1) ? FLAG_C : 0);
        a >>= 1;
        flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    }
    //     sec
    flags |= FLAG_C;
    //     adc ruler_left_stop
    a = adc(&flags, a, ruler_left_stop); // none live
    //     sec
    flags |= FLAG_C;
    //     sbc l0084
    a = sbc(&flags, a, l0084); // C, V live
    //     bcs c950f
    if (flags & FLAG_C)
    {
        c950f_impl(a);
        return;
    }
    //     lda #0
    //     beq c950f                                                         ;
    //     ALWAYS branch
    c950f_impl(0);
    return;
}

static void rj_fmt_cmd(void)
{
    // rj_fmt_cmd
    // Pseudocode: Right-justifies the current format line

    // ;
    // ***************************************************************************************
    // rj_fmt_cmd:
    //     jsr expand_line
    expand_line();
    //     bcc c9529
    if (!(flags & FLAG_C))
    {
        flags |= FLAG_C;
        return;
    }
    //     txa
    uint8_t a = x;

    //     beq c9529
    if (a == 0)
    {
        flags |= FLAG_C;
        return;
    }
    //     dex
    x--;
    //     dex
    x--;
    //     lda #0
    a = 0;
    //     cpx ruler_right_stop
    //     bcs c950f
    if (x >= ruler_right_stop)
    {
        c950f_impl(a);
        return;
    }
    //     stx l0083
    l0083 = x;
    //     lda ruler_right_stop
    a = ruler_right_stop;
    //     sec
    //     sbc l0083
    a -= l0083;
    // c950f: fall-through to shared routine
    c950f_impl(a);
    return;
}

static void expand_line(void)
{
    // expand_line
    // Pseudocode: Expands a format line into output_buffer, handling register
    // references via |

    // ;
    // ***************************************************************************************
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
    if (flags & FLAG_Z)
        return;
    // c9537:
c9537:
    for (;;)
    {
        //     lda (current_format_line_ptr),y
        a = ram[current_format_line_ptr + y];
        //     iny
        y++;
        //     cmp #0x7c ; '|'
        if (a == 0x7c)
            goto c955e;
        // c953e:
    c953e:
        for (;;)
        {
            //     sta output_buffer,x
            output_buffer[x] = a;
            //     jsr check_for_control_code
            control_code_t cc = check_for_control_code(a);
            //     bne c9548
            if (cc != NO_CONTROL_CODE)
            {
                l0083++;
            }
            //     inx
            x++;
            //     cmp #0x0d
            //     beq c9555
            if (a == 0x0d)
            {
                flags |=
                    FLAG_C; // C = (a >= 0x0d) = 1, feeds c9555's sbc/return
                goto c9555;
            }
            //     cpx #MAX_LINE_LENGTH-1
            //     bcc c9537
            if (x < MAX_LINE_LENGTH - 1)
                break;
            //     lda #0x0d
            a = 0x0d;
            //     bne c953e ; ALWAYS branch
        }
    }
    // c9555:
c9555:
    //     lda print_flags
    a = print_flags;
    if ((int8_t)a >= 0)
        return;
    //     bpl return_37
    //     txa
    a = x;
    //     sbc l0083
    a = sbc(&flags, a, l0083); // C, V live
    //     tax
    x = a;
    // return_37:
    //     rts
    return;

    // c955e:
c955e:
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    //     cmp #0x0d
    if (a == 0x0d)
        goto c953e;
    //     iny
    y++;
    //     jsr render_register
    render_register(a, y);
    // advance x past the digits written by render_number_to_output_buffer
    if (l0082 > x)
        x = l0082;
    //     jmp c9537
    goto c9537;
}

static void store_to_output_buffer(uint8_t a, addr_t tmp23)
{
    // sub_c95b2
    // sub_c95b2:
    //     ldy l0081
    uint8_t y;
    y = l0081;
    //     sta (((uint8_t*)&tmp23)[0]),y
    ram[tmp23 + y] = a;
    //     iny
    y++;
    //     sty l0081
    l0081 = y;
}

static uint8_t process_header_footer_line(uint8_t x, uint8_t y)
{
    uint8_t a;

    // c9575
    //     stx ((uint8_t*)&tmp23)[0]
    addr_t tmp23 = (addr_t)(y) << 8 | x;
    //     lda #0
    //     sta l0081
    l0081 = 0;
    //     sta l007a
    l007a = 0;
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
    // loop_c9589:
    for (;;)
    {
        //     iny
        y++;
        //     sty l0082
        l0082 = y;
        //     lda (current_format_line_ptr),y
        a = ram[current_format_line_ptr + y];
        //     cmp #0x0d
        //     cmp #0x1b
        //     cmp l0083
        // c959c:
        //     ora #0x80
        // c959e:
        //     jsr sub_c95b2
        // (branch restructured: |= 0x80 when a is 0x0d, or equals l0083)
        if (a == 0x0d)
            a |= 0x80;
        else
        {
            if (a < 0x1b)
                a = 0x20;
            if (a == l0083)
                a |= 0x80;
        }
        store_to_output_buffer(a, tmp23);
        //     cmp #0x8d
        if (a == 0x8d)
            goto c95aa;
        //     ldy l0082
        y = l0082;
        //     dex
        x--;
        //     bne loop_c9589
        if (x != 0)
            continue;
        break;
    }
c95aa:
    //     lda #0x80
    //     jsr sub_c95b2
    store_to_output_buffer(0x80, tmp23);
    //     jsr sub_c95b2
    store_to_output_buffer(0x80, tmp23);
    //     (fall through into sub_c95b2)
    store_to_output_buffer(0x80, tmp23);
    return a;
    // MULTIPLE ENTRY POINTS: dh_fmt_cmd, df_fmt_cmd
}

static void df_fmt_cmd(void)
{
    uint8_t x;
    uint8_t y;

    // Pseudocode: Stores footer text (shared code with dh_fmt_cmd)

    // ;
    // ***************************************************************************************
    // df_fmt_cmd:
    //     ldx #<(footer_text_maybe)
    //     ldy #>(footer_text_maybe)
    x = (uintptr_t)footer_text_maybe & 0xff;
    y = (uintptr_t)footer_text_maybe >> 8;
    process_header_footer_line(x, y);
}

static void dh_fmt_cmd(void)
{
    uint8_t y;

    uint8_t x;

    // Pseudocode: Stores header text (shared code with df_fmt_cmd)

    // ;
    // ***************************************************************************************
    // dh_fmt_cmd:
    //     ldx #<(header_text_maybe)
    //     ldy #>(header_text_maybe)
    //     bne c9575                                                         ;
    //     ALWAYS branch
    x = (uintptr_t)header_text_maybe & 0xff;
    y = (uintptr_t)header_text_maybe >> 8;
    process_header_footer_line(x, y);
}

static void em_fmt_cmd(void)
{
    addr_t tmp01;

    // em_fmt_cmd
    // Pseudocode: Evaluates expression and stores result in a register

    // ;
    // ***************************************************************************************
    // em_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr get_current_fmt_cmd_byte
    get_current_fmt_cmd_byte();
    //     beq return_38
    if (flags & FLAG_Z)
        return;
    //     iny
    y++;
    //     jsr get_register_address
    get_register_address(a);
    //     bcs return_38
    if (flags & FLAG_C)
        return;
    tmp01 = tmp67;
    //     jsr evaluate_expression_from_fmt_cmd
    evaluate_expression_from_fmt_cmd();
    //     ldy #0
    //     sta (((uint8_t*)&tmp01)[0]),y
    ram[tmp01 + 0] = a;
    //     iny                                                               ;
    //     Y=0x01
    //     lda ((uint8_t*)&tmp89)[1]
    a = ((uint8_t*)&tmp89)[1];
    //     sta (((uint8_t*)&tmp01)[0]),y
    ram[tmp01 + 1] = a;
    // return_38:
    //     rts
    return;
}

static void pl_fmt_cmd(void)
{
    // Pseudocode: Sets page_length from format command expression

    // ;
    // ***************************************************************************************
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

static void ts_fmt_cmd(void)
{
    // ts_fmt_cmd
    // Pseudocode: Sets two_sided_flag and rhs_extra_margin from format command

    // ;
    // ***************************************************************************************
    // ts_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr parse_boolean_from_fmt_cmd
    parse_boolean_from_fmt_cmd();
    //     bcs return_39
    if (flags & FLAG_C)
        return;
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

static void tm_fmt_cmd(void)
{
    // Pseudocode: Sets top_margin from format command expression

    // ;
    // ***************************************************************************************
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

static void bm_fmt_cmd(void)
{
    // Pseudocode: Sets bottom_margin from format command expression

    // ;
    // ***************************************************************************************
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

static void hm_fmt_cmd(void)
{
    // Pseudocode: Sets header_margin from format command expression

    // ;
    // ***************************************************************************************
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

static void fm_fmt_cmd(void)
{
    // Pseudocode: Sets footer_margin from format command expression

    // ;
    // ***************************************************************************************
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

static void lm_fmt_cmd(void)
{
    // Pseudocode: Sets left_margin from format command expression

    // ;
    // ***************************************************************************************
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

static void ls_fmt_cmd(void)
{
    // Pseudocode: Sets line_spacing from format command expression

    // ;
    // ***************************************************************************************
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

static void pe_fmt_cmd(void)
{
    // pe_fmt_cmd
    // Pseudocode: Forces page eject if remaining lines are less than value

    // ;
    // ***************************************************************************************
    // pe_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr evaluate_expression_from_fmt_cmd
    evaluate_expression_from_fmt_cmd();
    //     tax
    x = a;

    //     beq page_eject_fmt
    if (x == 0)
    {
        page_eject_fmt();
        return;
    }
    //     cmp l0021
    //     bcc return_40
    if (a < l0021)
    {
        flags &= ~FLAG_C; // C clear (a < l0021)
        return;
    }
    //     lda l0031
    a = l0031;

    //     bne page_eject_fmt
    if (a != 0)
    {
        page_eject_fmt();
        return;
    }
    // return_40:
    //     rts
    return;
}

static void c9642_tail(void)
{
    // c9642:
    //     jsr page_eject_fmt
    page_eject_fmt();
    //     (fall through into page_eject_fmt)
    page_eject_fmt();
}

static void op_fmt_cmd(void)
{
    // op_fmt_cmd
    // op_fmt_cmd:
    //     lda register_value_p
    uint8_t a = ram[RAM_REGISTER_VALUE_P];
    //     lsr
    flags = (flags & ~(FLAG_C | FLAG_Z | FLAG_N)) | ((a & 1) ? FLAG_C : 0);
    a >>= 1;
    flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bcc page_eject_fmt
    if (!(flags & FLAG_C))
    {
        page_eject_fmt();
        return;
    }
    //     bcs c9642                                                         ;
    //     ALWAYS branch
    c9642_tail();
    return;
}

static void ep_fmt_cmd(void)
{
    // ep_fmt_cmd
    // ep_fmt_cmd:
    //     lda register_value_p
    uint8_t a = ram[RAM_REGISTER_VALUE_P];
    //     lsr
    flags = (flags & ~(FLAG_C | FLAG_Z | FLAG_N)) | ((a & 1) ? FLAG_C : 0);
    a >>= 1;
    flags |= (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bcs page_eject_fmt
    if (flags & FLAG_C)
    {
        page_eject_fmt();
        return;
    }
    // c9642:
    c9642_tail();
    return;
}

static void page_eject_fmt(void)
{
    // page_eject_fmt
    // Pseudocode: Performs page eject by rendering new page and moving to sheet
    // bottom

    //     jsr page_eject_fmt
    // ;
    // ***************************************************************************************
    // page_eject_fmt:
    //     lda l0031
    //     bne c964c
    if (l0031 == 0)
    {
        //     jsr render_new_page
        render_new_page();
    }
    //     jmp c9263
    process_page_footer();
    return;
}

static void fo_fmt_cmd(void)
{
    // Pseudocode: Sets footers_enabled_flag from boolean format argument

    // ;
    // ***************************************************************************************
    // fo_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr parse_boolean_from_fmt_cmd
    parse_boolean_from_fmt_cmd();
    //     bcs return_41
    if (flags & FLAG_C)
        return;
    //     sta footers_enabled_flag
    footers_enabled_flag = a;
    // return_41:
    //     rts
    return;
}

static void he_fmt_cmd(void)
{
    // Pseudocode: Sets headers_enabled_flag from boolean format argument

    // ;
    // ***************************************************************************************
    // he_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr parse_boolean_from_fmt_cmd
    parse_boolean_from_fmt_cmd();
    //     bcs return_42
    if (flags & FLAG_C)
        return;
    //     sta headers_enabled_flag
    headers_enabled_flag = a;
    // return_42:
    //     rts
    return;
}

static void pb_fmt_cmd(void)
{
    // Pseudocode: Sets page break flag l0038 from boolean format argument

    // ;
    // ***************************************************************************************
    // pb_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr parse_boolean_from_fmt_cmd
    parse_boolean_from_fmt_cmd();
    //     bcs return_43
    if (flags & FLAG_C)
        return;
    //     sta l0038
    l0038 = a;
    // return_43:
    //     rts
    return;
}

static void dm_fmt_cmd(void)
{
    uint8_t a;

    uint8_t y;

    addr_t tmp67;

    // dm_fmt_cmd
    // Pseudocode: Defines a macro: stores macro name and position in linked
    // list

    // dm_fmt_cmd:
    //     lda macro_executing_flag
    a = macro_executing_flag;
    if (a != 0)
        return;
    //     bne return_42
    //     lda last_macro_ptr
    //     sta ((uint8_t*)&tmp67)[0]
    //     lda last_macro_ptr+1
    //     sta ((uint8_t*)&tmp67)[1]
    tmp67 = last_macro_ptr;
    //     ldy #3
    y = 3;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    //     and #0xdf
    a &= 0xdf;
    //     sta l0084
    l0084 = a;
    //     iny                                                               ;
    //     Y=0x04
    y++;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    //     jsr is_uppercase
    if (isupper(a))
    {
        flags &= ~FLAG_C;
    }
    else
    {
        flags |= FLAG_C;
    }
    //     bcc c968d
    if (!(flags & FLAG_C))
        goto c968d;
    //     lda #0x20 ; ' '
    a = 0x20;
    //     bne c968f                                                         ;
    //     ALWAYS branch
    goto c968f;

    // c968d:
c968d:
    //     and #0xdf
    a &= 0xdf;
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
    a = adc(&flags, a, (uint8_t)(last_macro_ptr & 0xff)); // C, V live
    //     sta last_macro_ptr
    last_macro_ptr = (last_macro_ptr & 0xff00) | a;
    //     bcc c96a2
    if ((flags & FLAG_C))
    {
        last_macro_ptr = (last_macro_ptr & 0x00ff) |
                         ((uint16_t)((last_macro_ptr >> 8) + 1) << 8);
    }
c96a2:
    //     lda himem
    //     sec
    //     sbc last_macro_ptr
    //     tax
    //     lda himem+1
    //     sbc last_macro_ptr+1
    //     bne c96b8
    {
        uint16_t diff = himem - last_macro_ptr;
        x = (uint8_t)diff;
        if (diff < 0x97)
        {
            {
                display_not_enough_memory();
                return;
            }
        }
    }
    //     lda last_macro_ptr
    a = (uint8_t)(last_macro_ptr & 0xff);
    //     sta ((uint8_t*)&tmp01)[0]
    ((uint8_t*)&tmp01)[0] = a;
    //     sta input_buffer_offset+1
    l0080 = a;
    //     sta current_format_line_ptr
    current_format_line_ptr = (current_format_line_ptr & 0xff00) | a;
    //     lda last_macro_ptr+1
    a = (uint8_t)((last_macro_ptr >> 8) & 0xff);
    //     sta ((uint8_t*)&tmp01)[1]
    ((uint8_t*)&tmp01)[1] = a;
    //     sta l0081
    l0081 = a;
    //     sta current_format_line_ptr+1
    current_format_line_ptr =
        (current_format_line_ptr & 0x00ff) | ((uint16_t)a << 8);
    //     jsr sub_c9241
    read_next_output_line();
    //     bcc c96ce
    if ((flags & FLAG_C))
    {
        return;
    }
    //     ldy #0
    y = 0;
    //     lda (last_macro_ptr),y
    a = ram[last_macro_ptr + y];
    //     jsr check_for_command_prefix
    command_prefix_t cp = check_for_command_prefix(a);
    //     bne c96f8
    if (cp == NO_COMMAND_PREFIX)
        goto c96f8;
    //     jsr lookup_formatting_command
    lookup_formatting_command();
    //     cpx #5
    if (x != 5)
        goto c96f8;
    //     lda #4
    //     ldy #0
    y = 0;
    //     sta (last_macro_ptr),y
    ram[last_macro_ptr + y] = 4;
    //     inc last_macro_ptr
    last_macro_ptr++;
    //     bne add_macro_to_linked_list
    //     inc last_macro_ptr+1
    // add_macro_to_linked_list:
    //     lda #0
    //     sta (last_macro_ptr),y
    ram[last_macro_ptr + y] = 0;
    //     lda last_macro_ptr
    a = (uint8_t)(last_macro_ptr & 0xff);
    //     sta (((uint8_t*)&tmp67)[0]),y
    ram[tmp67 + y] = a;
    //     iny
    y++;
    //     lda last_macro_ptr+1
    a = (uint8_t)((last_macro_ptr >> 8) & 0xff);
    //     sta (((uint8_t*)&tmp67)[0]),y
    ram[tmp67 + y] = a;
    //     rts
    return;

    // c96f8:
c96f8:
    last_macro_ptr = tmp01;
    //     bne c96a2                                                         ;
    //     ALWAYS branch
    goto c96a2;
}

static void ht_fmt_cmd(void)
{
    // ht_fmt_cmd
    // Pseudocode: Sets highlight codes (highlight1_code, highlight2_code) from
    // format command

    //     bne c96a2
    // ;
    // ***************************************************************************************
    // ht_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr get_current_fmt_cmd_byte
    get_current_fmt_cmd_byte();
    //     beq return_44
    if (flags & FLAG_Z)
        return;
    //     tax
    x = a;
    //     lda #0
    //     cpx #0x2d ; '-'
    if (x == 0x2d)
        goto c9716;
    //     lda #1
    //     cpx #0x2a ; '*'
    if (x != 0x2a)
        goto c9719;
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
    a = sbc(&flags, a, 1); // C live
    //     bcc return_44
    if (!(flags & FLAG_C))
        return;
    //     cmp #2
    //     bcs return_44
    if (a >= 2)
    {
        flags |= FLAG_C; // C = (a >= 2)
        return;
    }
    // c9725:
c9725:
    //     pha
    {
        uint8_t saved_a = a;
        //     jsr evaluate_expression_from_fmt_cmd
        evaluate_expression_from_fmt_cmd();
        //     pla
        a = saved_a;
    }
    //     tax
    x = a;
    //     lda ((uint8_t*)&tmp89)[0]
    a = ((uint8_t*)&tmp89)[0];
    //     sta highlight1_code,x
    highlight_code[x] = a;
    // return_44:
    //     rts
    return;
}

static const uint8_t commands_table[] = {'C',
    'E',
    'R',
    'J',
    'D',
    'F',
    'D',
    'H',
    'D',
    'M',
    'E',
    'M',
    'S',
    'R',
    'P',
    'E',
    'T',
    'M',
    'B',
    'M',
    'P',
    'L',
    'T',
    'S',
    'F',
    'O',
    'H',
    'E',
    'H',
    'T',
    'H',
    'M',
    'F',
    'M',
    'L',
    'M',
    'L',
    'S',
    'O',
    'P',
    'E',
    'P',
    'L',
    'J',
    'P',
    'B',
    0xff};

void lookup_formatting_command(void)
{
    addr_t tmp23;

    // Pseudocode: Looks up two-letter formatting command in commands_table

    // ;
    // ***************************************************************************************
    // lookup_formatting_command:
    //     ldy #2
    uint8_t y;
    uint8_t a;
    y = 2;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    //     sta ((uint8_t*)&tmp23)[1]
    ((uint8_t*)&tmp23)[1] = a;
    //     dey                                                               ;
    //     Y=0x01
    y--;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    //     sta ((uint8_t*)&tmp23)[0]
    ((uint8_t*)&tmp23)[0] = a;
    //     dey                                                               ;
    //     Y=0x00
    y--;
    //     ldx #0
    x = 0;
    // loop_c973e:
    do
    {
        a = ((uint8_t*)&tmp23)[0];
        if (a == commands_table[y])
        {
            a = ((uint8_t*)&tmp23)[1];
            if (a == commands_table[y + 1])
            {
                set_flags(&flags, 0); // Z set
                return;
            }
        }
        x++;
        y++;
        y++;
        a = commands_table[y];
    } while (!((int8_t)a < 0));
    //     bpl loop_c973e
    set_flags(&flags, a); // N live
    // return_45:
    //     rts
    return;
}

void execute_formatting_command(uint8_t x)
{
    uint8_t a;

    // Pseudocode: Executes a formatting command by index through the format
    // jump table

    // ;
    // ***************************************************************************************
    // execute_formatting_command:
    //     txa
    a = x;
    //     ldy #0
    //     ldx #0
    x = 0;
    //     stx l0030
    l0030 = x;
    //     jsr call_through_jumptable (call_through_jumptable_0, y=0)
    //     asl
    //     clc
    //     adc jumptable_ptrs,y
    //     sta ((uint8_t*)&tmp89)[0]
    //     lda #0
    //     adc jumptable_ptrs+1,y
    //     sta ((uint8_t*)&tmp89)[1]
    //     ldy #0
    //     lda (((uint8_t*)&tmp89)[0]),y
    //     sta ((uint8_t*)&tmp67)[0]
    //     iny
    //     lda (((uint8_t*)&tmp89)[0]),y
    //     sta ((uint8_t*)&tmp67)[1]
    //     jmp (((uint8_t*)&tmp67)[0])
    switch (a)
    {
        case 0:
            ce_fmt_cmd();
            break;
        case 1:
            rj_fmt_cmd();
            break;
        case 2:
            df_fmt_cmd();
            break;
        case 3:
            dh_fmt_cmd();
            break;
        case 4:
            dm_fmt_cmd();
            break;
        case 5:
            break; // return_34 (no-op slot)
        case 6:
            em_fmt_cmd();
            break;
        case 7:
            pe_fmt_cmd();
            break;
        case 8:
            tm_fmt_cmd();
            break;
        case 9:
            bm_fmt_cmd();
            break;
        case 10:
            pl_fmt_cmd();
            break;
        case 11:
            ts_fmt_cmd();
            break;
        case 12:
            fo_fmt_cmd();
            break;
        case 13:
            he_fmt_cmd();
            break;
        case 14:
            ht_fmt_cmd();
            break;
        case 15:
            hm_fmt_cmd();
            break;
        case 16:
            fm_fmt_cmd();
            break;
        case 17:
            lm_fmt_cmd();
            break;
        case 18:
            ls_fmt_cmd();
            break;
        case 19:
            op_fmt_cmd();
            break;
        case 20:
            ep_fmt_cmd();
            break;
        case 21:
            lj_fmt_cmd();
            break;
        case 22:
            pb_fmt_cmd();
            break;
    }
    //     ldx l0030
    set_flags(&flags, l0030); // Z live
    //     rts
    return;
}

static void parse_boolean_from_fmt_cmd(void)
{
    // Pseudocode: Parses a boolean (ON/OFF/1/0) from format command argument

    // ;
    // ***************************************************************************************
    // parse_boolean_from_fmt_cmd:
    //     jsr get_current_fmt_cmd_byte
    get_current_fmt_cmd_byte();
    //     sec
    flags |= FLAG_C;
    //     beq return_46
    if (flags & FLAG_Z)
        return;
    //     lda current_format_line_ptr
    a = current_format_line_ptr;
    //     ldx current_format_line_ptr+1
    x = current_format_line_ptr >> 8;

    // MULTIPLE ENTRY POINTS: parse_boolean_from_fmt_cmd, sub_c976c
    parse_word_flag();
}

static const uint8_t l97b0_data[] = {0x4f, 0x4e, 1, 'O', 'F', 'F', 0, 0xff};

static void parse_word_flag(void)
{
    addr_t tmp89;

    // sub_c976c
    // Pseudocode: Parses word-based flag (ON/OFF/YES/NO) from format command

    // sub_c976c:
    tmp89 = (addr_t)(x) << 8 | a;
    //     lda (((uint8_t*)&tmp89)[0]),y
    a = ram[tmp89 + y];
    //     tax
    x = a;
    //     lda #1
    //     cpx #0x31 ; '1'
    if (x == 0x31)
        goto c977f;
    //     lda #0
    a = 0;
    //     cpx #0x30 ; '0'
    if (x != 0x30)
        goto c9783;
    // c977f:
c977f:
    //     clc
    flags &= ~FLAG_C;
    //     iny
    y++;
    //     bne return_46
    if (y != 0)
        return;
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
    //     lda (((uint8_t*)&tmp89)[0]),y
    a = ram[tmp89 + y];
    //     jsr to_uppercase
    a = toupper(a);
    //     inx
    x++;
    //     cmp l97b0,x
    if (a == l97b0_data[x])
        goto c9788;
    //     lda l97b0,x
    a = l97b0_data[x];
    if ((int8_t)a < 0)
        goto c97ae;
    //     cmp #0x20 ; ' '
    //     bcc return_46
    if (a < 0x20)
    {
        flags &= ~FLAG_C; // C clear (a < 0x20)
        return;
    }
    // loop_c979d:
    for (;;)
    {
        //     inx
        x++;
        //     lda l97b0,x
        a = l97b0_data[x];
        if ((int8_t)a < 0)
            goto c97ae;
        //     cmp #0x20 ; ' '
        if (a >= 0x20)
            continue;
        //     ldy l0084
        //     lda l97b1,x
        a = l97b0_data[x + 1];
        if ((int8_t)a >= 0)
            goto c9788;
        //     bpl c9788
        break;
    }
    // c97ae:
c97ae:
    //     sec
    flags |= FLAG_C;
    // return_46:
    //     rts
    return;

    // MULTIPLE ENTRY POINTS: parse_boolean_from_fmt_cmd, sub_c976c
}

static void evaluate_expression_from_fmt_cmd(void)
{
    addr_t tmp45 = 0;

    // evaluate_expression_from_fmt_cmd
    // Pseudocode: Evaluates arithmetic expression with +, - and register
    // references

    // l97b0:
    //     .byte 0x4f
    // l97b1:
    //     .byte 0x4e, 1
    //     .ascii "OFF"
    //     .byte 0, 0xff

    // ;
    // ***************************************************************************************
    // evaluate_expression_from_fmt_cmd:
    //     lda #0
    a = 0;
    //     sta ((uint8_t*)&tmp89)[0]
    //     sta ((uint8_t*)&tmp89)[1]
    tmp89 = 0;
    //     sta input_buffer_offset+1
    l0080 = a;
    // c97c0:
c97c0:
    //     jsr get_current_fmt_cmd_byte
    get_current_fmt_cmd_byte();
    //     beq c9821
    if (flags & FLAG_Z)
        goto c9821;
    //     cmp #0x7c ; '|'
    if (a != 0x7c)
        goto c97d5;
    //     jsr get_next_fmt_cmd_byte
    get_next_fmt_cmd_byte(y);
    //     beq c9821
    if (flags & FLAG_Z)
        goto c9821;
    //     iny
    y++;
    //     jsr render_register
    render_register(a, y);
    //     jmp c97dc
    goto c97dc;

    // c97d5:
c97d5:
    //     jsr ca6fe
    parse_decimal_number();
    tmp89 = (addr_t)(x) << 8 | a;
    // c97dc:
c97dc:
    //     ldx input_buffer_offset+1
    x = l0080;

    //     beq c9804
    if (x == 0)
        goto c9804;
    //     lda #0
    a = 0;
    //     sta input_buffer_offset+1
    l0080 = a;
    //     dex
    x--;

    //     beq c97f7
    if (x == 0)
        goto c97f7;
    //     lda ((uint8_t*)&tmp45)[0]
    a = ((uint8_t*)&tmp45)[0];
    //     sec
    flags |= FLAG_C;
    //     sbc ((uint8_t*)&tmp89)[0]
    tmp89 = tmp45 - tmp89;
    goto c9804;

c97f7:
    tmp89 += tmp45;

c9804:
    tmp45 = tmp89;
    //     jsr get_current_fmt_cmd_byte
    get_current_fmt_cmd_byte();
    //     beq c9821
    if (flags & FLAG_Z)
        goto c9821;
    //     ldx #1
    x = 1;
    //     cmp #0x2b ; '+'
    //     beq c981c
    if (a == 0x2b)
        goto c981c;
    //     inx                                                               ;
    //     X=0x02
    x++;
    //     cmp #0x2d ; '-'
    //     bne c9821
    if (a != 0x2d)
        goto c9821;
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
    //     lda ((uint8_t*)&tmp89)[0]
    a = ((uint8_t*)&tmp89)[0];
    //     rts
    return;
}

static void get_current_fmt_cmd_byte(void)
{
    // get_current_fmt_cmd_byte:
    while (1)
    {
        a = ram[current_format_line_ptr + y];
        if (a == 0x0d)
        {
            set_flags(&flags, 0); // Z set
            return;
        }
        if (a != 0x20)
        {
            set_flags(&flags, 1); // Z clear
            return;
        }
        y++;
    }
}

static void get_next_fmt_cmd_byte(uint8_t y)
{
    // get_next_fmt_cmd_byte:
    //     iny
    y++;
    get_current_fmt_cmd_byte();
}

void render_register(uint8_t a, uint8_t y)
{
    addr_t tmp89;

    // render_register
    // render_register:
    //     sty l0084
    l0084 = y;
    //     jsr get_register_address
    get_register_address(a);
    //     ldy #0
    y = 0;
    //     sty ((uint8_t*)&tmp89)[0]
    tmp89 = 0;
    //     bcs cada2
    if (!(flags & FLAG_C))
    {
        bit(&flags, a, lada6); // none live
        a = ram[tmp67 + y];
        ((uint8_t*)&tmp89)[0] = a;
        y++;
        a = ram[tmp67 + y];
        ((uint8_t*)&tmp89)[1] = a;
        render_number_to_output_buffer(tmp89);
    }
    //     clv
    flags &= ~FLAG_V;
    //     ldy l0084
    y = l0084;
    //     rts
    return;
}

static void render_number_to_output_buffer(uint16_t value)
{
    // Pseudocode: Renders a 16-bit number to the output buffer using callback

    // ;
    // ***************************************************************************************
    // ; On Entry:
    // ;     TMP9/TMP8: 16-bit number
    // ;
    // ***************************************************************************************
    // render_number_to_output_buffer:
    //     stx l0082
    l0082 = x;
    //     lda la69a
    //     ldy la69b
    //     jsr render_number_to_callback
    render_number_to_callback(value, emit_to_output_buffer_callback);
    //     ldx l0082
    //     rts
    return;
}

static void emit_to_output_buffer_callback(uint8_t digit)
{
    // emit_to_output_buffer_callback
    // Pseudocode: Callback that writes a digit character to the output buffer

    // la69a:
    // la69b = la69a+1
    //     .word emit_to_output_buffer_callback

    //     pha
    // (digit value is in a at entry — set by render_number_to_callback)
    {
        //     txa
        //     pha
        uint8_t saved_x = x;
        //     ldx l0082
        x = l0082;
        //     sta output_buffer,x
        output_buffer[x] = digit;
        //     cpx #MAX_LINE_LENGTH-2
        if (x < MAX_LINE_LENGTH - 2)
        {
            l0082++;
        }
        //     pla
        //     tax
        x = saved_x;
        //     pla (restore a — but we didn't push it; keep the digit value)
        a = digit;
    }
    //     rts
    return;
}

void print_char_via_putchar(uint8_t a)
{
    cli_putchar(a);
}

void render_number_to_screen(uint16_t val)
{
    addr_t tmp89;

    // Pseudocode: Renders a 16-bit number to screen via bdos_print_char

    // ;
    // ***************************************************************************************
    // ; On Entry:
    // ;     YX: 16-bit number
    // ;
    // ***************************************************************************************
    // render_number_to_screen:
    //     stx ((uint8_t*)&tmp89)[0]
    tmp89 = val;
    //     lda #<(bdos_print_char)
    //     ldy #>(bdos_print_char)
    // Fall through to render_number_to_callback in original 6502
    render_number_to_callback(tmp89, print_char_via_putchar);
}

static void render_number_to_callback(uint16_t value, void (*cb)(uint8_t))
{
    // Pseudocode: Render 16-bit number as decimal via callback

    char buf[6];
    snprintf(buf, sizeof(buf), "%u", (unsigned int)value);
    for (char* p = buf; *p; p++)
    {
        a = *p - '0';
        a |= 0x30;
        cb(a);
    }
}

void bad_filename_error(void)
{
    // bad_filename_error:
    //     jsr print_inline_string
    //     .ascii "Bad filename\r"
    //     .byte 0
    cli_putstring("Bad filename\n");
    //     jmp return_to_cli_prompt
    return_to_cli_prompt();
    return;
}

static void c8f29_sub(uint8_t a)
{
    // c8f29:
    //     #if 0
    //     lda #0x40 ; '@'
    //     jsr open_file
    //     #endif
    //     sta rw_file_handle
    rw_file_handle = a;
    //     rts (falls through to c8f30 in original 6502)
}

static void process_page_footer(void)
{
    // c9263
    // Pseudocode: Handles page footer processing: prints footer, increments
    // page number

    // c9263:
    //     lda l0038
    uint8_t a = l0038;
    //     beq c9284
    if (a == 0)
        goto c9284;
    //     ldx l0021                                                         ;
    //     X=number of lines
    x = l0021;
    //     jsr print_vertical_space
    print_vertical_space(x);
    //     ldx footer_margin                                                 ;
    //     X=number of lines
    x = footer_margin;
    //     jsr print_vertical_space
    print_vertical_space(x);
    //     lda footers_enabled_flag
    a = footers_enabled_flag;
    //     beq c927c
    if (a != 0)
    {
        x = (uint8_t)((uintptr_t)footer_text_maybe & 0xff);
        uint8_t y = (uint8_t)((uintptr_t)footer_text_maybe >> 8);
        render_header_or_footer(y);
    }
    //     jsr print_newline
    print_newline();
    //     ldx bottom_margin                                                 ;
    //     X=number of lines
    x = bottom_margin;
    //     jsr print_vertical_space
    print_vertical_space(x);
    // c9284:
c9284:
    //     inc register_value_p
    ram[RAM_REGISTER_VALUE_P]++;
    set_flags(&flags, ram[RAM_REGISTER_VALUE_P]); // Z live
    //     bne c928c
    if ((flags & FLAG_Z))
    {
        ram[RAM_REGISTER_VALUE_P + 1]++;
    }
    //     lda #1
    //     sta register_value_l
    ram[RAM_REGISTER_VALUE_L] = 1;
    //     lda #0
    //     sta register_value_l+1
    ram[RAM_REGISTER_VALUE_L + 1] = 0;
    //     sta l0031
    l0031 = 0;
    //     rts
    return;
}

static void print_output_buffer(void)
{
    uint8_t x;
    uint8_t y;

    // c937b
    // c937b:
    //     ldy #0
    y = 0;
    //     ldx l0084
    x = l0084;
    //     beq return_28
    if (x == 0)
        return;
    // loop_c9381:
    do
    {
        a = x;
        {
            uint8_t saved_x = a;
            a = output_buffer[y];
            convert_char_for_printing();
            print_char(a);
            y++;
            a = saved_x;
        }
        x = a;
        x--;
    } while (x != 0);
    // return_28:
    //     rts
}

static uint8_t scan_string_length(uint8_t y_start, addr_t tmp45)
{
    // c93b8:
    //     iny
    //     lda (((uint8_t*)&tmp45)[0]),y
    //     bpl c93b8
    uint8_t a;
    uint8_t y = y_start;
    do
    {
        y++;
        a = ram[tmp45 + y];
    } while ((int8_t)a >= 0);
    return y;
    //     rts
}

void check_not_continuous_editing(void)
{
    // Pseudocode: Verifies not in continuous editing mode, shows file state if
    // editing

    // check_not_continuous_editing:
    //     bit file_edit_flags
    //     bvs return_20
    if ((file_edit_flags & 0x40))
        return;
    //     lda file_edit_flags
    uint8_t a = file_edit_flags;
    //     ror
    //     bcc return_20
    if (!(a & 1))
        return;
    //     bcs c8e5d                                                         ;
    //     ALWAYS branch
    display_document_file_state();
}

void display_not_enough_memory(void)
{
    // Pseudocode: Displays Not enough memory error and stops printing

    // display_not_enough_memory:
    //     jsr stop_printing
    stop_printing();
    //     jsr print_inline_string
    //     .ascii "Not enough memory"
    //     .byte 0xff
    cli_putstring("Not enough memory\n");
    return_to_cli_prompt();
    return;
    // return_6:
    //     rts
    return;

    // MULTIPLE ENTRY POINTS: check_for_at_least_150_bytes_free,
    // display_not_enough_memory
}

static void microspace_word_processor(void)
{
    addr_t tmp89;

    // microspace_word_processor
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
    {
        uint8_t saved_a = a;
        //     lda (((uint8_t*)&tmp01)[0]),y
        a = ram[tmp01 + y];
        //     jsr sub_c9431
        convert_char_for_printing();
        //     pla
        a = saved_a;
    }
    //     tax
    x = a;
    //     lda (((uint8_t*)&tmp01)[0]),y
    a = ram[tmp01 + y];
    //     iny
    y++;
    //     cmp #0x1a
    if (a != 0x1a)
        goto c906f;
    //     bit l0083
    bit(&flags, a, l0083); // N, V live
    //     bpl c9064
    if (!(flags & FLAG_N))
        goto c9064;
    //     lda l0048
    a = l0048;
    //     beq c906b
    if (a == 0)
        goto c906b;
    //     inc l0043
    l0043++;
    if (l0043 != 0)
        goto c9048;
    //     bne c9048
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
    l0042--;
    // c906f:
c906f:
    //     cmp #0x20 ; ' '
    //     bcc c9092
    if (a < 0x20)
        goto c9092;
    //     bne c9090
    if (a != 0x20)
        goto c9090;
    //     bit l0083
    bit(&flags, a, l0083); // N, V live
    //     bpl c9064
    if (!(flags & FLAG_N))
        goto c9064;
    //     lda l0042
    a = l0042;
    set_flags(&flags, a); // Z, N live
    //     beq c908a
    if (flags & FLAG_Z)
        goto c908a;
    //     bmi c9087
    if (flags & FLAG_N)
        goto c9087;
    //     inc l0043
    l0043++;
    //     lda #0
    a = 0;
    //     sta l0042
    l0042 = a;
    //     beq c9048                                                         ;
    //     ALWAYS branch
    goto c9048;

    // c9087:
c9087:
    //     clc
    flags &= ~FLAG_C;
    //     ror l0042
    l0042 = ror(&flags, l0042); // N live
    // c908a:
c908a:
    //     inc l0048
    l0048++;
    // c908c:
c908c:
    //     lda #0x20 ; ' '
    a = 0x20;
    set_flags(&flags, a); // Z live
    //     bne c90b6                                                         ;
    //     ALWAYS branch
    goto c90b6;

    // c9090:
c9090:
    //     inc l0046
    l0046++;
    // c9092:
c9092:
    //     cmp #9
    if (a == 9)
        goto c90a0;
    //     cmp #0x0b
    if (a == 0x0b)
        goto c90a0;
    //     sec
    flags |= FLAG_C;
    //     ror l0083
    l0083 = ror(&flags, l0083); // none live
    //     jmp c90b6
    goto c90b6;

    // c90a0:
c90a0:
    //     pha
    {
        uint8_t saved_a2 = a;
        //     lda l0039
        a = l0039;
        set_flags(&flags, a); // Z live
        //     sta l0047
        l0047 = a;
        //     lda #0
        //     sta l0083
        l0083 = 0;
        //     sta l0046
        l0046 = 0;
        //     sta l0048
        l0048 = 0;
        //     sta l0042
        l0042 = 0;
        //     sta l0043
        l0043 = 0;
        //     sta l0044
        l0044 = 0;
        //     sta l0045
        l0045 = 0;
        //     pla
        a = saved_a2;
    }
    // c90b6:
c90b6:
    //     sta output_buffer,x
    output_buffer[x] = a;
    //     inx
    x++;
    //     cmp #0x0d
    if (a == 0x0d)
        goto c90e2;
    //     cmp #0x20 ; ' '
    if (a == 0x20)
        goto c9048;
    //     lda l0048
    a = l0048;
    //     beq c9048
    if (a == 0)
        goto c9048;
    //     clc
    flags &= ~FLAG_C;
    //     adc l0044
    a = adc(&flags, a, l0044); // C live
    //     sta l0044
    l0044 = a;
    //     lda l0046
    a = l0046;
    //     adc l0048
    a = adc(&flags, a, l0048); // C live
    //     sta l0046
    l0046 = a;
    //     lda l0045
    a = l0045;
    //     adc l0043
    a = adc(&flags, a, l0043); // none live
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
    if (a == 0)
        goto c90f8;
    //     lda ruler_right_stop
    a = ruler_right_stop;

    //     beq c90f8
    if (a == 0)
        goto c90f8;
    //     sec
    flags |= FLAG_C;
    //     sbc l0047
    a = sbc(&flags, a, l0047); // C live
    //     bcc c90f8
    if (!(flags & FLAG_C))
        goto c90f8;
    //     sbc l0045
    a = sbc(&flags, a, l0045); // C live
    //     adc #0
    a = adc(&flags, a, 0); // none live
    //     sec
    flags |= FLAG_C;
    //     sbc l0046
    a = sbc(&flags, a, l0046); // Z, C live
    //     beq c9101
    if (flags & FLAG_Z)
        goto c9101;
    // c90f8:
c90f8:
    //     lda #0
    a = 0;
    //     sta l0039
    l0039 = a;
    //     ldy input_buffer_offset+1
    //     jmp c8fe6
    goto c8fe6_inline;

    // c9101:
c9101:
    //     lda #0
    a = 0;
    //     sta ((uint8_t*)&tmp89)[1]
    ((uint8_t*)&tmp89)[1] = a;
    //     ldx #8
    x = 8;
    // loop_c9107:
    do
    {
        {
            uint8_t c = (a & 0x80) ? FLAG_C : 0;
            a <<= 1;
            flags = (flags & ~(FLAG_C | FLAG_Z | FLAG_N)) | c;
        }
        ((uint8_t*)&tmp89)[1] = rol(&flags, ((uint8_t*)&tmp89)[1]); // none live
        {
            uint8_t c = (l0045 & 0x80) ? FLAG_C : 0;
            l0045 <<= 1;
            flags = (flags & ~(FLAG_C | FLAG_Z | FLAG_N)) | c;
        }
        if ((flags & FLAG_C))
        {
            flags &= ~FLAG_C;
            a = adc(&flags, a, microspacing_flag); // C live
            if ((flags & FLAG_C))
            {
                ((uint8_t*)&tmp89)[1]++;
            }
        }
        x--;
    } while (x != 0);
    //     sta ((uint8_t*)&tmp89)[0]
    ((uint8_t*)&tmp89)[0] = a;
    //     lda l0044
    a = l0044;
    //     sta l0046
    l0046 = a;
    //     jsr sub_cadf0
    //     sta l0045
    a = tmp89 % l0046;
    tmp89 = tmp89 / l0046;
    l0045 = a;
    //     lda ((uint8_t*)&tmp89)[0]
    a = ((uint8_t*)&tmp89)[0];
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
    convert_char_for_printing();
    //     pha
    {
        uint8_t saved_a3 = a;
        //     lda l0039
        a = l0039;
        //     cmp l0047
        //     beq c913b
        if (a == l0047)
            goto c913b;
        //     bcs c9142
        if (a >= l0047)
            goto c9142;
        // c913b:
    c913b:
        //     pla
        a = saved_a3;
        //     jsr c9426
        print_char_x_times(a, x);
        //     jmp c9163
        goto c9163;

        // c9142:
    c9142:
        //     pla
        a = saved_a3;
        //     cmp #0x20 ; ' '
        if (a != 0x20)
            goto c915b;
        //     lda microspacing_flag
        a = microspacing_flag;
        //     clc
        //     adc l0044
        a += l0044;
        //     tax
        x = a;
        //     lda l0045
        a = l0045;
        //     beq c9154
        if (a != 0)
        {
            x++;
            l0045--;
        }
        //     jsr sub_c9173
        emit_microspacing_spaces(x);
        //     lda #0x20 ; ' '
        a = 0x20;
        //     bne c9160 ; ALWAYS branch
        goto c9160;

        // c915b:
    c915b:
        //     ldx microspacing_flag
        x = microspacing_flag;
        //     jsr sub_c9173
        emit_microspacing_spaces(x);
        // c9160:
    c9160:
        //     jsr print_char
        print_char(a);
        // c9163:
    c9163:
        //     cmp #0x0d
        if (a != 0x0d)
            goto c912b;
        //     jmp c8ffb
        goto c8ffb_inline;
    }

c8fe6_inline:
    do
    {
        a = ram[tmp01 + y];
        y++;
        convert_char_for_printing();
        print_char_x_times(a, x);
    } while (a != 0x0d);
    //     inc register_value_l
    ram[RAM_REGISTER_VALUE_L]++;
    set_flags(&flags, ram[RAM_REGISTER_VALUE_L]); // Z live
    //     bne c8ffb_inline
    if ((flags & FLAG_Z))
    {
        ram[RAM_REGISTER_VALUE_L + 1]++;
    }
c8ffb_inline:
    //     ldx line_spacing
    x = line_spacing;
    //     lda l0021
    a = l0021;
    //     clc
    flags &= ~FLAG_C;
    //     sbc line_spacing
    a = sbc(&flags, a, line_spacing); // C live
    //     bcs c9009_inline
    if (!(flags & FLAG_C))
    {
        a = 0;
        x = l0021;
        x--;
    }
    //     sta l0021
    l0021 = a;
    //     jsr print_vertical_space
    print_vertical_space(x);
    //     rts
    return;
}

static void nested_macro_error(void)
{
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
    return_to_cli_prompt();
    return;
}

void parse_decimal_number(void)
{
    addr_t tmp89;

    // ca6fe - Parse decimal number from format command line
    // On entry: y = index into current_format_line_ptr
    // On exit:  ((uint8_t*)&tmp89)[0]:((uint8_t*)&tmp89)[1] = parsed value, a/x
    // = value, y = advanced past digits
    //           flags.Z = 1 if no digits parsed

    uint8_t had_digits = 0;
    tmp89 = 0;

    for (;;)
    {
        a = ram[(uint16_t)current_format_line_ptr + y];
        if (a < '0' || a > '9')
            break;
        a -= '0';
        y++;
        had_digits = 0xff;

        uint16_t val = tmp89;
        val = val * 10 + a;
        tmp89 = val;
    }

    a = ((uint8_t*)&tmp89)[0];
    x = ((uint8_t*)&tmp89)[1];
    set_flags(&flags, had_digits); // Z live
}

bool parse_optional_filename_from_command(struct scan_state* scan)
{
    // parse_optional_filename_from_command
    // Pseudocode: Parses optional filename from input buffer into
    // filename_buffer

    // parse_optional_filename_from_command:
    //     jsr sub_c8e33
    //     beq return_19
    if (scan_input_buffer(scan))
        return false; // no filename
    //     ldx #0
    uint8_t x = 0;
    while (1)
    {
        scan->ch = input_buffer[scan->pos];
        if (scan->ch == 0x0d)
            break;
        scan->pos++;
        if (scan->ch == l007e)
            break;
        filename_buffer[x] = scan->ch;
        x++;
        if (x == MAX_COMMAND_LENGTH - 1)
        {
            // buffer full → bad_filename_error (does not return)
            bad_filename_error();
            break;
        }
    }
    //     lda #0x0d
    // (A = 0x0d; passed inline to c8f29_sub by print_document)
    //     sta filename_buffer,x
    filename_buffer[x] = 0x0d;
    //     sty input_buffer_offset
    input_buffer_offset = scan->pos;
    // return_20:
    //     rts
    return true;
}

static void print_char_x_times(uint8_t a, uint8_t x)
{
    // c9426: Print character in A, X times. If X==0, return immediately.
    //     beq return_32
    if (x == 0)
        goto return_32;
    // loop_c942a:
    do
    {
        print_char(a);
        x--;
    } while (x != 0);
    //     bne loop_c942a
    // return_32:
return_32:
    //     rts
    return;
}

void print_document(struct scan_state* scan)
{
    // print_document
    // print_document:
    //     jsr check_not_continuous_editing
    check_not_continuous_editing();
    //     jsr check_for_at_least_150_bytes_free
    check_for_at_least_150_bytes_free();
    //     jsr sub_cb104
    reset_print_registers();
    //     lda top
    a = (uint8_t)(top & 0xff);
    //     adc #3
    flags &= ~FLAG_C;
    a = adc(&flags, a, 3); // C live
    //     sta ptr5
    ptr5 = (ptr5 & 0xff00) | a;
    //     tax
    x = a;
    //     lda top+1
    a = (uint8_t)(top >> 8);
    //     adc #0
    a = adc(&flags, a, 0); // C live
    //     sta ptr5+1
    ptr5 = (ptr5 & 0x00ff) | ((uint16_t)a << 8);
    //     tay
    y = a;
    //     txa
    a = x;
    //     adc #0x8d
    a = adc(&flags, a, 0x8d); // C live
    //     bcc c8edb
    if ((flags & FLAG_C))
    {
        y++;
    }
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
    //     tay                                                               ;
    //     Y=0x00
    y = a;
    //     sta (last_macro_ptr),y
    ram[last_macro_ptr + y] = a;
    current_ruler_ptr = RAM_CURRENT_RULER_BUF;
    //     jsr find_margins_of_current_ruler_buffer
    find_margins_of_current_ruler_buffer();
    //     jsr sub_c8e33
    //     bne c8f0d
    if (!scan_input_buffer(scan))
        goto c8f0d;
    //     inc printing_from_file_flag
    printing_from_file_flag++;
    printer_ptr6 = page;
    print_loop();
    goto c8f0d;
    // c8f0a:
    // c8f0d:
c8f0d:
    //     jsr parse_optional_filename_from_command
    //     bne c8f29
    if (parse_optional_filename_from_command(scan))
    {
        // A = 0x0d (set by parse_optional_filename_from_command's lda #&0d)
        c8f29_sub(0x0d);
        print_loop();
        goto c8f0d;
    }
    //     lda l0031
    a = l0031;
    if ((int8_t)a >= 0)
        return;
    //     bpl return_23
    //     jmp c9263
    process_page_footer();
    return;

    // return_23:
    //     rts

    // c8f1a:
    //     jsr stop_printing
    //     jsr bdos_print_newline
    //     jmp return_to_cli_prompt
}

static void print_loop(void)
{
    addr_t tmp67;
    addr_t tmp89;

    // print_loop
    // c8f30:
    while (1)
    {
        //     lda l0031
        a = l0031;

        //     beq c8f3b
        if (a != 0)
        {
            a = l0021;
            if (a == 0)
            {
                process_page_footer();
            }
        }
        //     jsr sub_c9188
        prepare_output_line();
        //     bcs c8f0a
        if (flags & FLAG_C)
            return;
        //     jsr sub_c916a
        start_microspacing_if_active(a);
        //     ldy #0
        y = 0;
        set_flags(&flags, y); // Z live
        //     sty input_buffer_ptr+1
        l0080 = y;
        //     jsr deref_and_check_for_command_prefix
        command_prefix_t cp = deref_and_check_for_command_prefix(y, tmp01);
        //     bne c8fce_thunk
        if (cp == NO_COMMAND_PREFIX)
            goto c8fce_thunk_l;
        //     ldy #3
        y = 3;
        //     sty input_buffer_ptr+1
        l0080 = y;
        //     jsr sub_cab6e
        is_embedded_ruler(tmp01);
        //     bne c8f6e
        if (!(flags & FLAG_Z))
            goto c8f6e_l;
        //     ldy #3
        y = 3;
        //     ldx #0
        x = 0;
        // loop_c8f5d:
        do
        {
            a = ram[tmp01 + y];
            current_ruler_buffer[x] = a;
            y++;
            x++;
        } while (a != 0x0d);
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
        if (flags & FLAG_N)
            goto c8f7a_l;
        //     jsr execute_formatting_command
        execute_formatting_command(x);
        //     beq c8f6b
        if (flags & FLAG_Z)
            goto c8f6b_l;
        // c8fce_thunk:
    c8fce_thunk_l:
        //     bne c8fce ; ALWAYS branch
        goto c8fce_l;

        // c8f7a:
    c8f7a_l:
        //     lda first_macro_ptr
        //     sta ((uint8_t*)&tmp67)[0]
        //     lda first_macro_ptr+1
        //     sta ((uint8_t*)&tmp67)[1]
        tmp67 = first_macro_ptr;
        //     ldy #1
        y = 1;
        //     lda (current_format_line_ptr),y
        a = ram[current_format_line_ptr + y];
        //     sta ((uint8_t*)&tmp89)[0]
        ((uint8_t*)&tmp89)[0] = a;
        //     iny ; Y=0x02
        y++;
        //     lda (current_format_line_ptr),y
        a = ram[current_format_line_ptr + y];
        //     jsr is_uppercase
        // (is_uppercase returns C=0 for A-Z/a-z, C=1 otherwise)
        if (a >= 'A' && a < '[')
        {
            flags &= ~FLAG_C;
        }
        else if (a >= 'a' && a < '{')
        {
            flags &= ~FLAG_C;
        }
        else
        {
            flags |= FLAG_C;
        }
        //     bcc c8f92
        if ((flags & FLAG_C))
        {
            a = 0x20;
        }
        //     sta ((uint8_t*)&tmp89)[1]
        ((uint8_t*)&tmp89)[1] = a;
        // lookup_macro_name:
    lookup_macro_name_l:
        //     ldy #0
        y = 0;
        //     lda (((uint8_t*)&tmp67)[0]),y
        a = ram[tmp67 + y];

        //     beq c8f6b
        if (a == 0)
            goto c8f6b_l;
        //     ldy #2
        y = 2;
        //     lda (((uint8_t*)&tmp67)[0]),y
        a = ram[tmp67 + y];
        //     cmp ((uint8_t*)&tmp89)[0]
        if (a != ((uint8_t*)&tmp89)[0])
            goto get_next_macro_in_linked_list_l;
        //     iny ; Y=0x03
        y++;
        //     lda (((uint8_t*)&tmp67)[0]),y
        a = ram[tmp67 + y];
        //     cmp ((uint8_t*)&tmp89)[1]
        if (a == ((uint8_t*)&tmp89)[1])
            goto c8fb9_l;
        // get_next_macro_in_linked_list:
    get_next_macro_in_linked_list_l:
        //     ldy #0
        y = 0;
        //     lda (((uint8_t*)&tmp67)[0]),y
        a = ram[tmp67 + y];
        //     pha
        {
            uint8_t saved_tmp = a;
            //     iny ; Y=0x01
            y++;
            //     lda (((uint8_t*)&tmp67)[0]),y
            a = ram[tmp67 + y];
            //     sta ((uint8_t*)&tmp67)[1]
            ((uint8_t*)&tmp67)[1] = a;
            //     pla
            a = saved_tmp;
        }
        //     sta ((uint8_t*)&tmp67)[0]
        ((uint8_t*)&tmp67)[0] = a;
        //     jmp lookup_macro_name
        goto lookup_macro_name_l;

        // c8fb9:
    c8fb9_l:
        //     lda macro_executing_flag
        a = macro_executing_flag;

        //     bne nested_macro_error
        if (a != 0)
        {
            nested_macro_error();
            return;
        }
        ptr3 = tmp67 + 4;
        macro_executing_flag = (uint8_t)(ptr3 >> 8);
        //     bne c900e

        if (macro_executing_flag != 0)
            continue;
        // c8fce:
    c8fce_l:
        //     lda l0031
        a = l0031;
        if (a == 0)
        {
            render_new_page();
        }
        //     jsr sub_c9407
        output_left_margin();
        //     lda #0
        a = 0;
        //     sta l0039
        l0039 = a;
        //     ldy input_buffer_ptr+1
        y = l0080;
        //     lda print_flags
        a = print_flags;
        if (((int8_t)a < 0))
        {
            a = microspacing_flag;

            if (a != 0)
            {
                microspace_word_processor();
                continue;
            }
        }
        do
        {
            a = ram[tmp01 + y];
            y++;
            convert_char_for_printing();
            print_char_x_times(a, x);
        } while (a != 0x0d);
        //     inc register_value_l
        ram[RAM_REGISTER_VALUE_L]++;
        set_flags(&flags, ram[RAM_REGISTER_VALUE_L]); // Z live
        //     bne c8ffb
        if ((flags & FLAG_Z))
        {
            ram[RAM_REGISTER_VALUE_L + 1]++;
        }
        //     ldx line_spacing
        x = line_spacing;
        //     lda l0021
        a = l0021;
        //     clc
        flags &= ~FLAG_C;
        //     sbc line_spacing
        a = sbc(&flags, a, line_spacing); // C live
        //     bcs c9009
        if (!(flags & FLAG_C))
        {
            a = 0;
            x = l0021;
            x--;
        }
        //     sta l0021
        l0021 = a;
        //     jsr print_vertical_space
        print_vertical_space(x);
        // c900e:
        //     jmp c8f30
    }
}

static void print_newline(void)
{
    // print_newline:
    //     lda #0x0d
    uint8_t a = 0x0d;
    print_char(a);
}

static void print_vertical_space(uint8_t x)
{
    // Pseudocode: Prints X number of blank lines (newlines)

    // ;
    // ***************************************************************************************
    // ; On Entry:
    // ;     X: number of lines
    // ;
    // ***************************************************************************************
    // print_vertical_space:
    //     lda #0x0d
    a = 0x0d;
    print_char_x_times(a, x);
}

void read_block_from_file(void)
{
    uint8_t a;

    uint8_t y;

    uint8_t x;

    // read_block_from_file
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
    a = get_byte_from_file();
    //     beq c8cf2
    if (flags & FLAG_Z)
        goto c8cf2;
    //     ldy #0
    //     cmp #0x7f
    if (a < 0x7f)
        goto c8caf;
    //     ldx l0084
    x = l0084;
    //     bne c8c95
    if (x != 0)
        goto c8c95;
    //     jsr check_for_command_prefix
    command_prefix_t cp = check_for_command_prefix(a);
    //     bne c8c95
    if (cp == NO_COMMAND_PREFIX)
        goto c8c95;
    //     ldx #0xfd
    x = 0xfd;
    //     stx l0083
    l0083 = x;
c8caf:
    // c8caf:
    //     cmp #0x20 ; ' '
    if (a >= 0x20)
        goto c8cc8;
    //     jsr check_for_control_code
    control_code_t cc = check_for_control_code(a);
    //     beq c8cc8
    if (cc != NO_CONTROL_CODE)
        goto c8cc8;
    //     cmp #0x1a
    if (a == 0x1a)
        goto c8cc8;
    //     cmp #0x0d
    if (a == 0x0d)
        goto c8cc8;
    //     cmp #0x0b
    if (a == 0x0b)
        goto c8cc8;
    //     cmp #9
    if (a != 9)
        goto c8c95;
c8cc8:
    // c8cc8:
    //     ldx #1
    x = 1;
    //     cmp #0x0d
    if (a == 0x0d)
        goto c8cdb;
    //     dex
    x--;
    //     ldy l0083
    y = l0083;
    //     cpy #0x84
    if (y == MAX_LINE_LENGTH)
    {
        {
            uint8_t saved_a_ = a;
            write_cr_to_memory();
            a = saved_a_;
        }
        x++;
    }
c8cdb:
    // c8cdb:
    //     inc l0083
    l0083++;
    //     jsr write_byte_to_memory
    write_byte_to_memory(a);
    //     txa

    //     beq c8c95
    if (x == 0)
        goto c8c95;
    //     lda ((uint8_t*)&tmp01)[1]
    a = ((uint8_t*)&tmp01)[1];
    //     cmp l0081
    //     bcc c8c95
    if (a < l0081)
        goto c8c95;
    //     bne c8cf1
    if (a != l0081)
        goto c8cf1;
    //     lda ((uint8_t*)&tmp01)[0]
    a = ((uint8_t*)&tmp01)[0];
    //     cmp input_buffer_offset+1
    if (a < l0080)
        goto c8c95;
c8cf1:
    // c8cf1:
    //     clc
    flags &= ~FLAG_C;
c8cf2:
    // c8cf2:
    //     php
    {
        uint8_t saved_flags_ = flags;
        //     lda l0084
        a = l0084;
        //     beq c8cfa
        if (a != 0)
        {
            write_cr_to_memory();
        }
        // c8cfa:
        //     plp
        flags = saved_flags_;
    }
    //     lda l0082
    a = l0082;
    flags = (flags & ~(FLAG_Z | FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     rts
}

static void render_header_or_footer(uint8_t y)
{
    // render_header_or_footer
    // Pseudocode: Renders header or footer text with centering and
    // justification

    // ;
    // ***************************************************************************************
    // render_header_or_footer:
    tmp45 = (addr_t)(y) << 8 | x;
    //     ldy #0
    y = 0;
    //     sty l0082
    l0082 = y;
    //     lda (((uint8_t*)&tmp45)[0]),y
    a = ram[tmp45 + y];
    //     beq return_28
    if (a == 0)
        return;
    //     jsr sub_c9407
    output_left_margin();
    //     lda #0
    a = 0;
    //     sta l0039
    l0039 = a;
    //     jsr sub_c9393
    compute_header_left_section(tmp45);
    //     jsr sub_c93fd
    get_page_parity();
    //     bcs c932e
    if (!(flags & FLAG_C))
    {
        compute_header_odd_page_section(tmp45);
    }
    //     jsr sub_c93c8
    copy_header_footer_text(tmp23);
    //     jsr c937b
    print_output_buffer();
    //     jsr sub_c939b
    compute_header_middle_section(tmp45);
    //     jsr sub_c93c8
    copy_header_footer_text(tmp23);
    //     txa
    a = x;
    //     beq c9355
    if (a == 0)
        goto c9355;
    //     dex
    x--;
    //     txa
    a = x;
    //     lsr
    a = asr(&flags, a); // Z live
    //     sta l0081
    l0081 = a;
    //     jsr sub_c93be
    a = get_right_margin();
    //     beq c9355
    if (flags & FLAG_Z)
        goto c9355;
    //     lsr
    a = asr(&flags, a); // none live
    //     sec
    flags |= FLAG_C;
    //     sbc l0081
    a = sbc(&flags, a, l0081); // C live
    //     bcc c9355
    if ((flags & FLAG_C))
    {
        a = sbc(&flags, a, l0039); // C live
        if ((flags & FLAG_C))
        {
            x = a;
            a = add_justification_spaces(x);
        }
    }
c9355:
    //     jsr c937b
    print_output_buffer();
    //     jsr sub_c93a1
    compute_header_odd_page_section(tmp45);
    //     jsr sub_c93fd
    get_page_parity();
    //     bcs c9363
    if (!(flags & FLAG_C))
    {
        compute_header_left_section(tmp45);
    }
    //     jsr sub_c93c8
    copy_header_footer_text(tmp23);
    //     jsr sub_c93be
    a = get_right_margin();
    //     beq c937b
    if (flags & FLAG_Z)
    {
        print_output_buffer();
        return;
    }
    //     stx l0081
    l0081 = x;
    //     sec
    flags |= FLAG_C;
    //     sbc l0081
    a = sbc(&flags, a, l0081); // C live
    //     bcc c937b
    if (!(flags & FLAG_C))
    {
        print_output_buffer();
        return;
    }
    //     sbc l0039
    a = sbc(&flags, a, l0039); // C live
    //     bcc c937b
    if (!(flags & FLAG_C))
    {
        print_output_buffer();
        return;
    }
    //     tax
    x = a;
    //     inx
    x++;
    //     jsr sub_c941a
    a = add_justification_spaces(x);
    print_output_buffer();
}

static void render_new_page(void)
{
    // render_new_page
    // Pseudocode: Renders a new page with headers, margins, page number prompt

    // ;
    // ***************************************************************************************
    // render_new_page:
    //     lda #0x81
    a = 0x81;
    //     sta l0031
    l0031 = a;
    //     bit print_flags
    bit(&flags, a, print_flags); // V live
    //     bvc c92d4
    if (!(flags & FLAG_V))
        goto c92d4;
    //     jsr stop_printing
    stop_printing();
    //     jsr print_inline_string
    //     .ascii "\rPage "
    //     .byte 0
    cli_putstring("\nPage ");

    //     ldx register_value_p
    render_number_to_screen(((addr_t)ram[RAM_REGISTER_VALUE_P + 1] << 8) |
                            ram[RAM_REGISTER_VALUE_P]);
    //     jsr print_inline_string
    //     .ascii ".."
    //     .byte 0
    cli_putstring("..");

    //     jsr flush_and_read_char
    read_char();
    //     bcs c92cc
    if (flags & FLAG_C)
        goto c92cc;
    //     and #0xdf
    a &= 0xdf;
    //     cmp #0x4d ; 'M'
    //     beq c92d4
    if (a == 0x4d)
        goto c92d4;
    //     cmp #0x51 ; 'Q'
    //     bne c92cf
    if (a != 0x51)
        goto c92cf;
    // c92cc:
c92cc:
    //     jmp c8f1a
    stop_printing();
    cli_putchar('\n');
    return_to_cli_prompt();
    return;

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
    if (a == 0)
    {
        compute_lines_remaining_on_page();
        return;
    }
    //     ldx top_margin                                                    ;
    //     X=number of lines
    x = top_margin;
    //     jsr print_vertical_space
    print_vertical_space(x);
    //     lda headers_enabled_flag
    a = headers_enabled_flag;
    //     beq c92e8
    if (a != 0)
    {
        x = (uint8_t)((uintptr_t)header_text_maybe & 0xff);
        uint8_t y = (uint8_t)((uintptr_t)header_text_maybe >> 8);
        render_header_or_footer(y);
    }
    //     jsr print_newline
    print_newline();
    //     ldx header_margin                                                 ;
    //     X=number of lines
    x = header_margin;
    //     jsr print_vertical_space
    print_vertical_space(x);
    // c92f0: fall-through to shared routine
    compute_lines_remaining_on_page();
    return;
}

/**
 * scan_input_buffer: Scans input_buffer from input_buffer_offset looking for
 * the next character that is not the delimiter l007e.
 *
 * Advances the scan position past any run of delimiter characters and stops at
 * the first character that differs from l007e (a "mark"/argument character) or
 * at the end of the command line.
 *
 * @param state On return holds the scan result (see struct scan_state):
 *              state->ch is the character at the scan position (the first
 *              non-delimiter character, or 0x0d if the end of the line was
 *              reached first, or l007e itself when l007e == 0x0d); state->pos
 * is its index into input_buffer (input_buffer_offset advanced past any
 * delimiters).
 * @return true if the Z flag would be set, i.e. no non-delimiter character was
 *         found (no mark); false if a non-delimiter character was found.
 *
 * Note: input_buffer_offset itself is not modified; callers advance it once the
 * mark position is known (e.g. parse_mark_from_command does state->pos++ then
 * input_buffer_offset = state->pos).
 */
bool scan_input_buffer(struct scan_state* state)
{
    // sub_c8e33
    // sub_c8e33:
    //     lda l007e
    //     cmp #0x0d
    //     beq return_20
    state->pos = input_buffer_offset;
    state->ch = l007e;
    if (state->ch == 0x0d)
    {
        return true; // Z set (no mark)
    }
    //     ldy input_buffer_offset
    // loop_c8e3b:
    while (1)
    {
        //     lda input_buffer,y
        state->ch = input_buffer[state->pos];
        //     cmp #0x0d
        //     beq return_20
        if (state->ch == 0x0d)
        {
            return true; // Z set (no mark)
        }
        //     cmp l007e
        //     bne return_20
        if (state->ch != l007e)
        {
            return false; // Z clear (mark found)
        }
        //     iny
        state->pos++;
        //     bne loop_c8e3b
        if (state->pos == 0)
            break;
    }
    //     rts (falls through to check_not_continuous_editing in 6502)
    return true; // Z set (scan position wrapped past the buffer)
}

static void start_microspacing_if_active(uint8_t a)
{
    uint8_t x;

    // Pseudocode: Checks if printer is active and starts microspacing if
    // supported

    // sub_c916a:
    //     ldx print_flags
    x = print_flags;
    //     bpl return_25
    if (!(x & 0x80))
        return;
    //     ldx microspacing_flag
    x = microspacing_flag;
    //     bne c9177
    if (x == 0)
        return;
    // c9177:
    //     jsr sub_c9445
    print_alignment_spaces(a);
    //     pha
    //     stx l0043
    l0043 = x;
    //     lda #9
    printer_driver_ptr->printer_microspace();
    //     pla
    // return_25:
    //     rts
    return;
}

static void emit_microspacing_spaces(uint8_t x)
{
    // Pseudocode: Emits spaces for microspacing by calling printer driver with
    // spacing count

    // sub_c9173:
    //     cpx l0043
    //     beq return_25
    if (x == l0043)
        return;
    // c9177:
    //     jsr sub_c9445
    print_alignment_spaces(a);
    //     pha
    {
        uint8_t saved_a = a;
        //     stx l0043
        l0043 = x;
        //     lda #9
        printer_driver_ptr->printer_microspace();
        //     pla
        a = saved_a;
    }
    // return_25:
    //     rts
    return;
}

static void prepare_output_line(void)
{
    uint8_t a;

    uint8_t x;
    uint8_t y;

    // sub_c9188
    //  Ptrs:   ptr1, ptr3, ptr5
    // c9184:
    //     lda #0
    //     sta macro_executing_flag
    // (handled inline below)

    // sub_c9188:
    //     lda macro_executing_flag
    a = macro_executing_flag;
    //     bne c91a3
    if (a != 0)
        goto c91a3;
c9188_normal_entry:
    //     lda ptr5
    a = (uint8_t)(ptr5 & 0xff);
    //     sta input_buffer_ptr+1
    l0080 = a;
    //     sta ((uint8_t*)&tmp01)[0]
    ((uint8_t*)&tmp01)[0] = a;
    //     lda ptr5+1
    a = (uint8_t)(ptr5 >> 8);
    //     sta l0081
    l0081 = a;
    //     sta ((uint8_t*)&tmp01)[1]
    ((uint8_t*)&tmp01)[1] = a;
    //     jsr sub_c9241
    read_next_output_line();
    //     bcs return_26
    if (flags & FLAG_C)
        return;
    //     lda ptr5
    a = (uint8_t)(ptr5 & 0xff);
    //     ldy ptr5+1
    y = (uint8_t)(ptr5 >> 8);
    //     bne c91d0
    if (y != 0)
        goto c91d0;
    // c91a3:
c91a3:
    //     ldy #0
    y = 0;
    //     ldx #0
    x = 0;
    // c91a7:
c91a7:
    for (;;)
    {
        //     lda (ptr3),y
        a = ram[ptr3 + y];
        //     cmp #4
        //     beq c9184
        if (a == 4)
        {
            macro_executing_flag = 0;
            goto c9188_normal_entry;
        }
        //     cmp #0x40 ; '@'
        //     beq c91da
        if (a == 0x40)
            goto c91da;
        //     iny
        y++;
        // loop_c91b2:
        for (;;)
        {
            //     sta current_line_buffer,x
            ram[RAM_CURRENT_LINE_BUF + x] = a;
            //     inx
            x++;
            //     cmp #0x0d
            //     beq c91c2
            if (a == 0x0d)
                goto c91c2;
            //     cpx #0x83
            //     bcc c91a7
            if (x < 0x83)
                break;
            //     lda #0x0d
            a = 0x0d;
            //     bne loop_c91b2 ; ALWAYS branch
        }
    }

    // c91c2:
c91c2:
    //     tya
    a = y;
    //     clc
    flags &= ~FLAG_C;
    //     adc ptr3
    a = adc(&flags, a, (uint8_t)(ptr3 & 0xff)); // C live
    //     sta ptr3
    ptr3 = (ptr3 & 0xff00) | a;
    //     bcc c91cc
    if ((flags & FLAG_C))
    {
        ptr3 = (ptr3 & 0x00ff) |
               ((uint16_t)((uint8_t)((ptr3 >> 8) & 0xff) + 1) << 8);
    }
    //     lda ptr1
    a = (uint8_t)(ptr1 & 0xff);
    //     ldy ptr1+1
    y = (uint8_t)(ptr1 >> 8);
    // c91d0:
c91d0:
    tmp01 = (addr_t)(y) << 8 | a;
    //     sta current_format_line_ptr
    //     sty current_format_line_ptr+1
    current_format_line_ptr = tmp01;
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
    a = sbc(&flags, a, 0x30); // C, V live
    //     bcc c9225
    if (!(flags & FLAG_C))
        goto c9225;
    //     cmp #0x0a
    //     bcs c9225
    if (a >= 0x0a)
        goto c9225;
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
    l0083--;
    if ((int8_t)l0083 < 0)
        goto c9209;
    // c91f5:
c91f5:
    //     iny
    y++;
    //     lda (ptr5),y
    a = ram[ptr5 + y];
    //     cmp #0x0d
    //     beq c9223
    if (a == 0x0d)
        goto c9223;
    //     jsr sub_c9228
    parse_register_reference(a);
    //     beq c91f5
    if (flags & FLAG_Z)
        goto c91f5;
    //     bvs c91f5
    if (flags & FLAG_V)
        goto c91f5;
    //     cmp #0x2c ; ','
    //     beq loop_c91f1
    if (a == 0x2c)
        goto loop_c91f1;
    //     bne c91f5                                                         ;
    //     ALWAYS branch
    goto c91f5;

    // c9209:
c9209:
    //     iny
    y++;
    //     lda (ptr5),y
    a = ram[ptr5 + y];
    //     cmp #0x0d
    //     beq c9223
    if (a == 0x0d)
        goto c9223;
    //     jsr sub_c9228
    parse_register_reference(a);
    //     beq c9209
    if (flags & FLAG_Z)
        goto c9209;
    //     bvs c921b
    if (flags & FLAG_V)
        goto c921b;
    //     cmp #0x2c ; ','
    //     beq c9223
    if (a == 0x2c)
        goto c9223;
    // c921b:
c921b:
    //     sta current_line_buffer,x
    ram[RAM_CURRENT_LINE_BUF + x] = a;
    //     inx
    x++;
    //     cpx #0x82
    //     bcc c9209
    if (x < 0x82)
        goto c9209;
    // c9223:
c9223:
    //     ldy l0084
    y = l0084;
    // c9225:
c9225:
    //     jmp c91a7
    goto c91a7;
}

static void parse_register_reference(uint8_t a)
{
    // sub_c9228
    // Pseudocode: Parses register reference markers (<, >, =) in format line

    // sub_c9228:
    //     cmp #0x3e ; '>'
    //     bne c9231
    if (a == 0x3e)
    {
        a = 0;
        l0082 = a;
        set_flags(&flags, a); // Z live
        return;
    }
    //     cmp #0x3c ; '<'
    //     bne c923c
    if (a == 0x3c)
    {
        a = 0x40;
        l0082 = a;
        set_flags(&flags, 0); // Z live
        return;
    }
    //     bit l0082
    bit(&flags, a, l0082); // V live
    //     ora #0
    a |= 0;
    set_flags(&flags, a); // Z live
    //     rts
    return;
}

static void read_next_output_line(void)
{
    uint8_t a;
    uint8_t a2;

    // sub_c9241
    //  Ptrs:   ptr6
    // Pseudocode: Reads next line from file buffer or calls
    // read_block_from_file for printing

    // sub_c9241:
    //     lda printing_from_file_flag
    a = printing_from_file_flag;
    //     beq c9260
    if (a == 0)
    {
        read_block_from_file();
        return;
    }
    //     ldy #0
    uint8_t y = 0;
    // loop_c9247:
    do
    {
        a2 = ram[printer_ptr6 + y];
        flags |= FLAG_C;
        if (a2 == 0)
            return;
        ram[tmp01 + y] = a2;
        printer_ptr6++;
        tmp01++;
    } while (a2 != 0x0d);
    //     clc
    flags &= ~FLAG_C;
    // return_27:
    //     rts
    return;

    // c9260:
    //     jmp read_block_from_file
}

static void compute_lines_remaining_on_page(void)
{
    // sub_c92f0
    // sub_c92f0: Computes remaining lines on page = page_length minus margins
    // On exit: l0021 = result (at least 1 if any margin computation underflows)

    // c92f0:
    //     ldx page_length
    uint8_t x;
    uint8_t a;
    x = page_length;
    //     lda l0038
    a = l0038;

    //     beq c930d
    if (a == 0)
        goto c930d;
    //     ldx #1
    x = 1;
    //     lda page_length
    a = page_length;
    //     clc
    flags &= ~FLAG_C;
    //     sbc top_margin
    a = sbc(&flags, a, top_margin); // C live
    //     bcc c930d
    if (!(flags & FLAG_C))
        goto c930d;
    //     sbc header_margin
    a = sbc(&flags, a, header_margin); // C live
    //     bcc c930d
    if (!(flags & FLAG_C))
        goto c930d;
    //     clc
    flags &= ~FLAG_C;
    //     sbc bottom_margin
    a = sbc(&flags, a, bottom_margin); // C live
    //     bcc c930d
    if ((flags & FLAG_C))
    {
        a = sbc(&flags, a, footer_margin); // C live
        if ((flags & FLAG_C))
        {
            x = a;
        }
    }
c930d:
    //     stx l0021
    l0021 = x;
    //     rts
    return;
}

static void compute_header_left_section(addr_t tmp45)
{
    uint8_t a;

    // sub_c9393:
    //     jsr sub_c93b6
    get_line_width(tmp45);
    //     lda #0
    a = 0;
    //     jmp c93aa
    {
        // c93aa:
        //     clc
        //     adc ((uint8_t*)&tmp45)[0]
        //     sta ((uint8_t*)&tmp23)[0]
        //     lda ((uint8_t*)&tmp45)[1]
        //     adc #0
        //     sta ((uint8_t*)&tmp23)[1]
        tmp23 = tmp45 + a;
    }
}

static void compute_header_middle_section(addr_t tmp45)
{
    uint8_t y;

    uint8_t a;

    // sub_c939b:
    //     jsr sub_c93b6
    y = get_line_width(tmp45);
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
        //     clc
        //     adc ((uint8_t*)&tmp45)[0]
        //     sta ((uint8_t*)&tmp23)[0]
        //     lda ((uint8_t*)&tmp45)[1]
        //     adc #0
        //     sta ((uint8_t*)&tmp23)[1]
        tmp23 = tmp45 + a;
    }
}

static void compute_header_odd_page_section(addr_t tmp45)
{
    uint8_t a;

    // sub_c93a1:
    //     jsr sub_c93b6
    //     jsr c93b8
    // c93a7:
    uint8_t y = scan_string_length(get_line_width(tmp45), tmp45);
    y++;
    a = y;
    y--;
    // c93aa:
    {
        //     clc
        //     adc ((uint8_t*)&tmp45)[0]
        //     sta ((uint8_t*)&tmp23)[0]
        //     lda ((uint8_t*)&tmp45)[1]
        //     adc #0
        //     sta ((uint8_t*)&tmp23)[1]
        tmp23 = tmp45 + a;
    }
}

static uint8_t get_line_width(addr_t tmp45)
{
    // sub_c93b6:
    //     ldy #0xff
    return scan_string_length(0xff, tmp45);
}

static uint8_t get_right_margin(void)
{
    // Pseudocode: Returns ruler_right_stop or l003a-1 as the line width

    // sub_c93be:
    //     lda ruler_right_stop
    a = ruler_right_stop;
    if (a != 0)
    {
        set_flags(&flags, 1); // Z clear (margin present)
        return a;
    }
    //     bne return_29
    //     lda l003a
    a = l003a;
    //     sec
    //     sbc #1
    a -= 1;
    set_flags(&flags, a); // Z = (a == 0)
    // return_29:
    return a;
}

static void copy_header_footer_text(addr_t tmp23)
{
    uint8_t a;
    uint8_t y = 0;

    // sub_c93c8
    // Pseudocode: Copies header/footer text to output_buffer, expanding
    // register references

    // sub_c93c8:
    //     ldx #0
    x = 0;
    //     ldy #0
    set_flags(&flags, y); // Z live
    //     sty l0081
    l0081 = y;
    // c93ce:
c93ce:
    //     lda (((uint8_t*)&tmp23)[0]),y
    a = ram[tmp23 + y];
    if ((int8_t)a < 0)
        goto c93e6;
    //     jsr check_for_control_code
    control_code_t cc = check_for_control_code(a);
    //     bne c93d9
    if (cc != NO_CONTROL_CODE)
    {
        l0081++;
    }
    //     iny
    y++;
    //     cmp #0x7c ; '|'
    if (a == 0x7c)
        goto c93f2;
    //     sta output_buffer,x
    output_buffer[x] = a;
    //     inx
    x++;
    //     cpx #MAX_LINE_LENGTH
    if (x < MAX_LINE_LENGTH)
        goto c93ce;
    // c93e6:
c93e6:
    //     stx l0084
    l0084 = x;
    //     lda print_flags
    a = print_flags;
    if (((int8_t)a < 0))
    {
        a = x;
        a -= l0081;
        x = a;
    }
    //     rts
    return;

    // c93f2:
c93f2:
    //     lda (((uint8_t*)&tmp23)[0]),y
    a = ram[tmp23 + y];
    if ((int8_t)a < 0)
        goto c93e6;
    //     iny
    y++;
    //     jsr render_register
    render_register(a, y);
    //     jmp c93ce
    goto c93ce;
}

static void get_page_parity(void)
{
    // Pseudocode: Checks two_sided_flag and returns page parity for alternate
    // layout

    // sub_c93fd:
    //     sec
    uint8_t a;
    flags |= FLAG_C;
    //     lda two_sided_flag
    a = two_sided_flag;

    //     beq return_31
    if (a == 0)
        goto return_31;
    //     lda register_value_p
    a = ram[RAM_REGISTER_VALUE_P];
    //     lsr
    flags = (flags & ~FLAG_C) | (a & 1);
    a >>= 1;
    // return_31:
return_31:; // fallthrough to rts
}

static void output_left_margin(void)
{
    uint8_t a, x;

    // Pseudocode: Outputs left margin spaces, adjusting for two-sided printing

    // sub_c9407:
    //     jsr sub_c93fd
    get_page_parity();
    //     lda left_margin
    a = left_margin;
    //     bcc c9415
    if ((flags & FLAG_C))
    {
        x = two_sided_flag;

        if (x != 0)
        {
            a += rhs_extra_margin;
        }
    }
    //     tax
    x = a;
    //     lda #0x20 ; ' '
    a = 0x20;
    //     bne c9426                                                         ;
    //     ALWAYS branch
    print_char_x_times(a, x);
}

static uint8_t add_justification_spaces(uint8_t x)
{
    // Pseudocode: Adds extra spaces to x position for centering/justification

    // sub_c941a:
    //     txa
    uint8_t a = x;
    //     clc
    //     adc l0039
    a += l0039;
    //     sta l0039
    l0039 = a;
    //     lda #0x20 ; ' '
    a = 0x20;
    //     bne c9426                                                         ;
    //     ALWAYS branch
    print_char_x_times(a, x);
    return a;
}

static void convert_char_for_printing(void)
{
    // Pseudocode: Converts character for printing, updates x position counter

    // sub_c9431:
    //     jsr sub_ca5ae
    process_document_character();
    //     bit print_flags
    if (!(print_flags & 0x80))
        goto c943c;
    //     ora #0
    a |= 0;

    //     bmi return_33                                                     ;
    //     ALWAYS branch
    if (a & 0x80)
        goto return_33;

    // c943c:
c943c:
    //     pha
    {
        uint8_t saved_a = a;
        //     txa
        a = x;
        //     clc
        //     adc l0039
        a += l0039;
        //     sta l0039
        l0039 = a;
        //     pla
        a = saved_a;
    }
// return_33:
return_33:
    //     rts
    return;
}

static void reset_print_registers(void)
{
    uint8_t a;

    // sub_cb104
    // Pseudocode: Resets formatting registers and default print settings

    // sub_cb104:
    //     lda #0
    uint8_t y;
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
    //     iny                                                               ;
    //     Y=0x81
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
    compute_lines_remaining_on_page();
    return;
}

// main is now the function above (inlined from main_)

static void write_byte_to_memory(uint8_t a)
{
    // write_byte_to_memory:
    //     ldy #0
    uint8_t y = 0;
    //     sta (((uint8_t*)&tmp01)[0]),y
    ram[tmp01] = a;
    //     inc ((uint8_t*)&tmp01)[0]
    tmp01++;
    //     bne c8d0a
    //     inc ((uint8_t*)&tmp01)[1]
    // c8d0a:
    //     sta l0084
    l0084 = a;
    //     cmp #0x0d
    //     bne return_16
    if (a != 0x0d)
        return;
    //     sty l0084
    l0084 = y;
    //     sty l0083
    l0083 = y;
    // return_16:
    //     rts
}

static void write_cr_to_memory(void)
{
    // write_cr_to_memory:
    //     lda #0x0d
    write_byte_to_memory(0x0d);
}

// Printer driver setup (called from cli.c)
void stop_printing(void)
{
    uint8_t a;

    // stop_printing
    // Pseudocode: Stops active printing by clearing print flags and calling
    // printer driver cleanup

    // ;
    // ***************************************************************************************
    // stop_printing:
    //     lda print_flags
    a = print_flags;
    if (((int8_t)a < 0))
    {
        // (The 6502 clears bit 7 by rolling left into C, clearing C, and
        //  rolling right again; the input C value is irrelevant.)
        //     rol print_flags
        //     clc
        //     ror print_flags
        print_flags &= 0x7f;
        printer_driver_ptr->printer_off();
    }
    //     rts
    return;
}

void prepare_printer_driver(void)
{
    // Pseudocode: Sets up printer driver pointer from name or default driver

    // ;
    // ***************************************************************************************
    // prepare_printer_driver:
    //     ldx #<printer_driver_block
    //     ldy #>printer_driver_block
    //     lda printer_driver_name
    //     bne c949e
    //     ldx default_printer_driver_ptr
    //     ldy l94b2
    //     lda #0
    //     sta microspacing_flag
    microspacing_flag = 0;
    printer_driver_ptr = &default_printer_driver;
    // c949e:
    //     stx printer_driver_ptr
    //     sty printer_driver_ptr+1
    // return_35:
    //     rts
}

static void default_print_char(uint8_t a)
{
    // c94c0:
    //     cmp #0x80
    //     bcs return_35
    if (a >= 0x80)
        return;
    //     jmp bdos_print_char
    cli_putchar(a);
}

// Default printer_on: init / set mode
static void default_printer_on(void)
{
    // c94c7:
    //     lda #2
    //     jmp default_printer_off
    default_printer_off();
}

// Default printer_off: write char with mode byte
static void default_printer_off(void)
{
    // c94cb:
    //     lda #3
    // c94cd:
    //     jmp oswrch
    // PROBLEM: jmp oswrch (BBC Micro OS call - not available)
}

// Default printer driver entry 3: no-op
static void default_printer_microspace(void) {}

// Default printer_getflags: sets x and y to zero (original view-cpm.S entry)
static void default_printer_getflags(void)
{
    x = 0;
    y = 0;
}

static const struct printer_driver default_printer_driver = {
    .print_char = default_print_char,
    .printer_on = default_printer_on,
    .printer_off = default_printer_off,
    .printer_microspace = default_printer_microspace,
    .printer_getflags = default_printer_getflags,
};