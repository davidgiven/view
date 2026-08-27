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
static void default_printer_getflags(uint8_t* x, uint8_t* y);
static const struct printer_driver default_printer_driver;

// Printing-only functions
void bad_filename_error(void);
static void set_rw_file_handle(uint8_t a);
static void process_page_footer(void);
static void print_output_buffer(void);
static uint8_t scan_string_length(uint8_t y_start, addr_t tmp45);
void check_not_continuous_editing(void);
void display_not_enough_memory(void);
static void microspace_word_processor(void);
static void nested_macro_error(void);
bool parse_decimal_number(int* value, uint8_t* y);
bool parse_optional_filename_from_command(struct scan_state* scan);
static void print_char_x_times(uint8_t a, uint8_t x);
void print_document(struct scan_state* scan);
static void print_loop(addr_t ptr5);
static void print_newline(void);
static void print_vertical_space(uint8_t x);
read_block_status_t read_block_from_file(addr_t* cursor, addr_t limit);
static void render_header_or_footer(uint16_t yx);
static void render_new_page(void);
bool scan_input_buffer(struct scan_state* state);
static void start_microspacing_if_active(uint8_t a);
static void emit_microspacing_spaces(uint8_t a, uint8_t x);
static addr_t prepare_output_line(addr_t read_limit, addr_t* macro_cursor);
static enum parse_register_result_t parse_register_reference(uint8_t a);
static read_block_status_t read_next_output_line(addr_t limit, addr_t* cursor);
static void compute_lines_remaining_on_page(void);
static addr_t compute_header_left_section(addr_t tmp45);
static addr_t compute_header_middle_section(addr_t tmp45);
static addr_t compute_header_odd_page_section(addr_t tmp45);
static uint8_t get_line_width(addr_t tmp45);
static uint8_t get_right_margin(void);
static void copy_header_footer_text(addr_t tmp23);
static bool get_page_parity(void);
static void output_left_margin(void);
static uint8_t add_justification_spaces(uint8_t x);
static uint8_t convert_char_for_printing(uint8_t a, uint8_t* x, bool* is_tab);
static void reset_print_registers(void);
static void write_byte_to_memory(addr_t* cursor, uint8_t a);
static void write_cr_to_memory(addr_t* cursor);

// Functions from view.c used by printing code

// Forward declarations within printing.c
static uint8_t expand_line(void);
static void write_output_buffer_to_format_line(uint8_t a);
static bool parse_word_flag(addr_t ptr, uint8_t* y, uint8_t* value);
static bool parse_boolean_from_fmt_cmd(uint8_t* y, uint8_t* value);
static void page_eject_fmt(void);
static bool evaluate_expression_from_fmt_cmd(uint16_t* result, uint8_t* y);
static uint8_t get_current_fmt_cmd_byte(uint8_t* y);
static uint8_t get_next_fmt_cmd_byte(uint8_t* y);

enum parse_register_result_t
{
    PARSE_REGISTER_MARKER,
    PARSE_REGISTER_VALUE,
    PARSE_REGISTER_OTHER,
};

enum formatting_command lookup_formatting_command(void);
static void store_to_output_buffer(uint8_t a, addr_t tmp23);
static uint8_t process_header_footer_line(uint8_t x, uint8_t y);
static void write_output_buffer_to_format_line(uint8_t a);
void render_register(uint8_t a, uint8_t x);
static void render_number_to_output_buffer(uint16_t value, uint8_t start_x);
static void emit_to_output_buffer_callback(uint8_t digit);
static void render_number_to_callback(uint16_t value, void (*cb)(uint8_t));

static void write_output_buffer_to_format_line(uint8_t a)
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
    formatted_line_written_flag++;
    // c9529:
    //     sec
    // (every exit of the 6502 routine sets C unconditionally, so it carries
    //  no information; no caller reads it afterwards)
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
    //     bcc return_36 (C=0 ⟺ nothing expanded: expand_line returned 0)
    if (expand_line() == 0)
        return;
    //     lda #0
    //     beq c950f                                                         ;
    //     ALWAYS branch
    write_output_buffer_to_format_line(0);
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
    //     bcc return_36 (C=0 ⟺ nothing expanded)
    uint8_t a = expand_line();
    //     beq return_36
    if (a == 0)
        return;
    //     lsr
    // (flags not used; the following sec and C-comparisons set/read their
    //  own values)
    a >>= 1;
    //     sta l0084
    l0084 = a;
    //     lda ruler_right_stop
    a = ruler_right_stop;

    //     beq c950f
    if (a == 0)
    {
        write_output_buffer_to_format_line(a);
        return;
    }
    //     sec
    //     sbc ruler_left_stop
    //     lsr
    // (flags not used; the following add and C-comparisons set/read their
    //  own values)
    a -= ruler_left_stop;
    a >>= 1;
    //     sec
    //     adc ruler_left_stop
    // (sec makes this a + ruler_left_stop + 1)
    a += ruler_left_stop + 1;
    //     sec
    //     sbc l0084
    //     bcs c950f
    // (sbc with C=1 is a plain subtraction; its C flag (no borrow) selects
    //  the result passed to c950f_impl)
    if (a >= l0084)
    {
        write_output_buffer_to_format_line(a - l0084);
        return;
    }
    //     lda #0
    //     beq c950f ; ALWAYS branch
    write_output_buffer_to_format_line(0);
    return;
}

static void rj_fmt_cmd(void)
{
    uint8_t a;
    uint8_t x;
    // rj_fmt_cmd
    // Pseudocode: Right-justifies the current format line

    // ;
    // ***************************************************************************************
    // rj_fmt_cmd:
    //     jsr expand_line
    //     bcc c9529 (C=0 ⟺ nothing expanded; both bail paths set C)
    x = expand_line();
    if (x == 0)
        return;
    // (the 6502's bail path branched into c9529, setting C; no caller of
    //  rj_fmt_cmd reads C, so it is not reproduced here)
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
        write_output_buffer_to_format_line(a);
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
    write_output_buffer_to_format_line(a);
    return;
}

/**
 * Expand the current format command line into output_buffer, copying from
 * offset 3 onward and expanding |<register> references via render_register.
 * Control codes are counted in l0083 and MAX_LINE_LENGTH is enforced.
 *
 * The 6502 also returned C, but it is redundant: the terminating CR is not
 * a control code, so l0083 <= x-1 at the sbc and every non-empty expansion
 * leaves x >= 1.  A zero return therefore means exactly "nothing expanded".
 *
 * @return the number of characters written to output_buffer (the 6502's X
 *         register); 0 when the byte at offset 3 is nul.
 */
static uint8_t expand_line(void)
{
    uint8_t a;
    uint8_t y;
    uint8_t x;
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
    a = get_current_fmt_cmd_byte(&y);
    //     clc
    //     beq return_37
    if (a == 0)
        return x;
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
            // (the cmp's C=1 fed c9555's sbc in the 6502; expand_line no
            //  longer returns C, so it is not reproduced here)
            if (a == 0x0d)
                goto c9555;
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
    if (!(a & 0x80))
        return x; // C=1 from the cmp #0x0d that reached c9555
    //     bpl return_37
    //     txa
    //     sbc l0083
    // (C=1 carry-in makes this a plain subtraction; the sbc's resulting
    //  carry is provably always set here — CR is not a control code, so
    //  l0083 <= x-1 — which is why the 6502's C return was redundant)
    x -= l0083;
    //     tax
    // return_37:
    //     rts
    return x;

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
    render_register(a, x);
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
    uint8_t a;
    uint8_t y;
    // em_fmt_cmd
    // Pseudocode: Evaluates expression and stores result in a register

    // ;
    // ***************************************************************************************
    // em_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr get_current_fmt_cmd_byte
    a = get_current_fmt_cmd_byte(&y);
    //     beq return_38
    if (a == 0)
        return;
    //     iny
    y++;
    //     jsr get_register_address
    //     bcs return_38
    unsigned int* register_value = get_register_address(a);
    if (register_value == NULL)
        return;
    //     jsr evaluate_expression_from_fmt_cmd
    // (result returned as the 16-bit value; stored into the register)
    uint16_t reg_value;
    evaluate_expression_from_fmt_cmd(&reg_value, &y);
    *register_value = reg_value;
    //     ldy #0
    //     sta (((uint8_t*)&tmp01)[0]),y
    //     lda ((uint8_t*)&tmp89)[1]
    //     sta (((uint8_t*)&tmp01)[0]),y
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
    //     jsr evaluate_expression_from_fmt_cmd
    uint16_t value;
    evaluate_expression_from_fmt_cmd(&value, &y);
    page_length = value;
    //     sta page_length
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
    //     bcs return_39 (C=1 conveyed as a true return)
    uint8_t flag_value;
    if (parse_boolean_from_fmt_cmd(&y, &flag_value))
        return;
    //     sta two_sided_flag
    two_sided_flag = flag_value;
    //     jsr evaluate_expression_from_fmt_cmd
    //     jsr evaluate_expression_from_fmt_cmd
    uint16_t value;
    evaluate_expression_from_fmt_cmd(&value, &y);
    rhs_extra_margin = value;
    //     sta rhs_extra_margin
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
    //     jsr evaluate_expression_from_fmt_cmd
    uint16_t value;
    evaluate_expression_from_fmt_cmd(&value, &y);
    top_margin = value;
    //     sta top_margin
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
    //     jsr evaluate_expression_from_fmt_cmd
    uint16_t value;
    evaluate_expression_from_fmt_cmd(&value, &y);
    bottom_margin = value;
    //     sta bottom_margin
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
    //     jsr evaluate_expression_from_fmt_cmd
    uint16_t value;
    evaluate_expression_from_fmt_cmd(&value, &y);
    header_margin = value;
    //     sta header_margin
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
    //     jsr evaluate_expression_from_fmt_cmd
    uint16_t value;
    evaluate_expression_from_fmt_cmd(&value, &y);
    footer_margin = value;
    //     sta footer_margin
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
    //     jsr evaluate_expression_from_fmt_cmd
    uint16_t value;
    evaluate_expression_from_fmt_cmd(&value, &y);
    left_margin = value;
    //     sta left_margin
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
    //     jsr evaluate_expression_from_fmt_cmd
    uint16_t value;
    evaluate_expression_from_fmt_cmd(&value, &y);
    line_spacing = value;
    //     sta line_spacing
    //     rts
    return;
}

static void pe_fmt_cmd(void)
{
    uint8_t x;
    // pe_fmt_cmd
    // Pseudocode: Forces page eject if remaining lines are less than value

    // ;
    // ***************************************************************************************
    // pe_fmt_cmd:
    //     ldy #3
    y = 3;
    //     jsr evaluate_expression_from_fmt_cmd
    uint16_t value;
    evaluate_expression_from_fmt_cmd(&value, &y);
    x = value;
    //     tax
    //     beq page_eject_fmt
    if (x == 0)
    {
        page_eject_fmt();
        return;
    }
    //     cmp l0021
    //     bcc return_40
    if (a < l0021)
        return;
    // (the 6502 cleared C on this exit; pe_fmt_cmd's caller never reads it)
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

static void eject_two_pages(void)
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
    uint8_t a = (uint8_t)register_value_array['P' - 'A'];
    //     lsr
    // (only the C flag is used: the shifted-out low bit selects the page
    //  parity branch)
    if (!(a & 1))
    {
        page_eject_fmt();
        return;
    }
    a >>= 1;
    //     bcs c9642                                                         ;
    //     ALWAYS branch
    eject_two_pages();
    return;
}

static void ep_fmt_cmd(void)
{
    // ep_fmt_cmd
    // ep_fmt_cmd:
    //     lda register_value_p
    uint8_t a = (uint8_t)register_value_array['P' - 'A'];
    //     lsr
    // (only the C flag is used: the shifted-out low bit selects the page
    //  parity branch)
    if (a & 1)
    {
        page_eject_fmt();
        return;
    }
    a >>= 1;
    // c9642:
    eject_two_pages();
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
    //     bcs return_41 (C=1 conveyed as a true return)
    uint8_t flag_value;
    if (parse_boolean_from_fmt_cmd(&y, &flag_value))
        return;
    //     sta footers_enabled_flag
    footers_enabled_flag = flag_value;
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
    //     bcs return_42 (C=1 conveyed as a true return)
    uint8_t flag_value;
    if (parse_boolean_from_fmt_cmd(&y, &flag_value))
        return;
    //     sta headers_enabled_flag
    headers_enabled_flag = flag_value;
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
    //     bcs return_43 (C=1 conveyed as a true return)
    uint8_t flag_value;
    if (parse_boolean_from_fmt_cmd(&y, &flag_value))
        return;
    //     sta l0038
    l0038 = flag_value;
    // return_43:
    //     rts
    return;
}

static void dm_fmt_cmd(void)
{
    uint8_t a;

    uint8_t y;

    addr_t tmp01;
    struct macro* tmp67;

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
    // (tmp67 keeps the struct macro* of the node being built)
    tmp67 = last_macro_ptr;
    //     ldy #3
    y = 3;
    //     lda (current_format_line_ptr),y
    uint8_t firstchar = ram[current_format_line_ptr + y];
    //     and #0xdf
    firstchar &= 0xdf;
    //     sta l0084
    //     iny                                                               ;
    //     Y=0x04
    y++;
    //     lda (current_format_line_ptr),y
    uint8_t secondchar = ram[current_format_line_ptr + y];
    //     jsr is_uppercase
    //     bcc c968d
    //     lda #0x20 ; ' '
    //     bne c968f                                                         ;
    //     ALWAYS branch
    if (isalpha(secondchar))
        secondchar &= 0xdf;
    else
        secondchar = 0x20;

    // c968d:
    //     and #0xdf
    // c968f:
    //     dey
    //     sta (last_macro_ptr),y
    //     dey
    //     lda l0084
    //     sta (last_macro_ptr),y
    // (the name occupies node offsets 2-3: [1] = second character, [0] = first)
    last_macro_ptr->name[1] = secondchar;
    last_macro_ptr->name[0] = firstchar;
    //     lda #4
    //     clc
    //     adc last_macro_ptr
    //     sta last_macro_ptr
    //     bcc c96a2
    // (the 6502 advances past the 4-byte node header to the body; the C
    //  keeps last_macro_ptr at the node start and reaches the body via
    //  ->body)

    // c96a2: read macro-body lines, skipping any that are not an EM
    // formatting-command line
    for (;;)
    {
        //     lda himem
        //     sec
        //     sbc last_macro_ptr
        //     tax
        //     lda himem+1
        //     sbc last_macro_ptr+1
        //     bne c96b8
        intptr_t diff = (ram + himem) - (uint8_t*)(last_macro_ptr->body);
        if (diff < 0x97)
        {
            display_not_enough_memory();
            return;
        }
        //     lda last_macro_ptr
        //     sta ((uint8_t*)&tmp01)[0]
        //     sta input_buffer_offset+1
        //     sta current_format_line_ptr
        //     lda last_macro_ptr+1
        //     sta ((uint8_t*)&tmp01)[1]
        //     sta l0081
        //     sta current_format_line_ptr+1
        // (16-bit copy: tmp01 = current_format_line_ptr = last_macro_ptr->body)
        tmp01 = (addr_t)(last_macro_ptr->body - ram);
        current_format_line_ptr = (addr_t)(last_macro_ptr->body - ram);
        //     jsr sub_c9241
        if (read_next_output_line((addr_t)(last_macro_ptr->body - ram),
                &tmp01) == READ_BLOCK_DONE)
        //     bcc c96ce
        {
            return;
        }
        //     ldy #0
        y = 0;
        //     lda (last_macro_ptr),y
        a = last_macro_ptr->body[y];
        //     jsr check_for_command_prefix
        //     bne c96f8
        //     jsr lookup_formatting_command
        //     cpx #5
        command_prefix_t cp = check_for_command_prefix(a);
        if (cp != NO_COMMAND_PREFIX &&
            lookup_formatting_command() == FORMATTING_COMMAND_EM)
            break;
        // (c96f8: skip this line and read the next one)
        //     lda tmp0
        //     sta last_macro_ptr
        //     lda tmp1
        //     sta last_macro_ptr+1
        //     bne c96a2
        last_macro_ptr = (struct macro*)&ram[tmp01];
    }
    //     lda #4
    //     ldy #0
    //     sta (last_macro_ptr),y
    // (4 is the macro-body terminator byte, written at the body start)
    last_macro_ptr->body[0] = 4;
    //     inc last_macro_ptr
    //     bne add_macro_to_linked_list
    //     inc last_macro_ptr+1
    // add_macro_to_linked_list:
    //     lda #0
    //     sta (last_macro_ptr),y
    // (the 6502 advances one byte and writes the end-of-list marker at
    //  body+1; the struct write zeroes it directly)
    last_macro_ptr->body[1] = 0;
    //     lda last_macro_ptr
    //     sta (((uint8_t*)&tmp67)[0]),y
    //     iny
    //     lda last_macro_ptr+1
    //     sta (((uint8_t*)&tmp67)[0]),y
    // (16-bit write: the previous macro's next pointer = body + 1)
    tmp67->next = (addr_t)(last_macro_ptr->body + 1 - ram);
    //     rts
    return;
}

static void ht_fmt_cmd(void)
{
    uint8_t a;
    uint8_t y;
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
    a = get_current_fmt_cmd_byte(&y);
    //     beq return_44
    if (a == 0)
        return;
    //     tax
    uint8_t x = a;
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
    uint16_t value;
    evaluate_expression_from_fmt_cmd(&value, &y);
    a = value;
    //     sec
    //     sbc #1
    //     bcc return_44
    // (sbc with C=1 is a plain subtraction; borrow means a was 0)
    if (a == 0)
        return;
    a -= 1;
    //     cmp #2
    //     bcs return_44
    if (a >= 2)
        return;
    // (the 6502's cmp left C set on this exit; no caller of ht_fmt_cmd
    //  reads it)
    // c9725:
c9725:
    //     pha
    uint8_t saved_a = a;
    //     jsr evaluate_expression_from_fmt_cmd
    addr_t highlight_value;
    evaluate_expression_from_fmt_cmd(&highlight_value, &y);
    //     pla
    a = saved_a;
    //     tax
    x = a;
    //     lda ((uint8_t*)&tmp89)[0]
    a = (uint8_t)highlight_value;
    //     sta highlight1_code,x
    highlight_code[x] = a;
    // return_44:
    //     rts
    return;
}

// Zero-terminated string of the two-letter formatting command codes.  The
// 6502 terminates the table with 0xff (detected via the N flag); the C
// version uses a NUL terminator instead.
static const uint8_t commands_table[] =
    "CERJDFDHDMEMSRPETMBMPLTSFOHEHTHMFMLMLSOPEPLJPB";

// Returns the index of the matched two-letter formatting command in
// commands_table, or NO_FORMATTING_COMMAND if the letters don't match.
enum formatting_command lookup_formatting_command(void)
{
    uint8_t char1;
    uint8_t char2;

    // Pseudocode: Looks up two-letter formatting command in commands_table

    // ;
    // ***************************************************************************************
    // lookup_formatting_command:
    //     ldy #2
    uint8_t y;
    uint8_t a;
    int index;
    y = 2;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    //     sta tmp3                     ; second command letter
    char2 = a;
    //     dey                                                               ;
    //     Y=0x01
    y--;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    //     sta tmp2                     ; first command letter
    char1 = a;
    //     dey                                                               ;
    //     Y=0x00
    y--;
    //     ldx #0
    // (the 6502 uses x for the command index; the C returns it)
    index = 0;
    // loop_c973e:
    do
    {
        a = char1;
        if (a == commands_table[y])
        {
            a = char2;
            if (a == commands_table[y + 1])
            {
                return index;
            }
        }
        index++;
        y++;
        y++;
        a = commands_table[y];
    } while (a != 0);
    //     bpl loop_c973e
    // (the 6502 terminates the table with 0xff and detects it via N; the C
    //  table is NUL-terminated, so the scan ends on a == 0)
    // return_45:
    //     rts
    return NO_FORMATTING_COMMAND;
}

/**
 * Execute one formatting command by index.
 *
 * @param x the format command index (0-22) as returned by
 * lookup_formatting_command().
 *
 * The command is dispatched through the format jump table.  The dispatched
 * *fmt_cmd increments formatted_line_written_flag when it emits a formatted
 * line.
 *
 * @return true iff formatted_line_written_flag == 0 (the 6502's Z flag),
 * i.e. the command did not
 * emit a formatted line.  print_loop uses this to choose between continuing to
 * the next command line (true, c8f6b_l) and outputting the formatted line
 * (false, c8fce_l).  This is the only deliberately-returned flag.
 *
 * C/V are not produced here, and no caller reads them afterwards: the
 * terminal `ldx l0030` only writes Z/N, and both paths back in print_loop
 * (c8f6b_l via continue; c8fce_l through render_new_page /
 * output_left_margin / the character loop) reach the next flag consumer
 * only after prepare_output_line has rewritten C.  Handler exit-flag
 * traffic is therefore not reproduced.  tmp01, ptr1, ptr5 are registers
 * the dispatched command reads/writes, used by the printing pipeline
 * after the call.
 */
bool execute_formatting_command(enum formatting_command x)
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
    //     stx l0030
    formatted_line_written_flag = 0;
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
        case FORMATTING_COMMAND_CE:
            ce_fmt_cmd();
            break;
        case FORMATTING_COMMAND_RJ:
            rj_fmt_cmd();
            break;
        case FORMATTING_COMMAND_DF:
            df_fmt_cmd();
            break;
        case FORMATTING_COMMAND_DH:
            dh_fmt_cmd();
            break;
        case FORMATTING_COMMAND_DM:
            dm_fmt_cmd();
            break;
        case FORMATTING_COMMAND_EM:
            break; // return_34 (no-op slot)
        case FORMATTING_COMMAND_SR:
            em_fmt_cmd();
            break;
        case FORMATTING_COMMAND_PE:
            pe_fmt_cmd();
            break;
        case FORMATTING_COMMAND_TM:
            tm_fmt_cmd();
            break;
        case FORMATTING_COMMAND_BM:
            bm_fmt_cmd();
            break;
        case FORMATTING_COMMAND_PL:
            pl_fmt_cmd();
            break;
        case FORMATTING_COMMAND_TS:
            ts_fmt_cmd();
            break;
        case FORMATTING_COMMAND_FO:
            fo_fmt_cmd();
            break;
        case FORMATTING_COMMAND_HE:
            he_fmt_cmd();
            break;
        case FORMATTING_COMMAND_HT:
            ht_fmt_cmd();
            break;
        case FORMATTING_COMMAND_HM:
            hm_fmt_cmd();
            break;
        case FORMATTING_COMMAND_FM:
            fm_fmt_cmd();
            break;
        case FORMATTING_COMMAND_LM:
            lm_fmt_cmd();
            break;
        case FORMATTING_COMMAND_LS:
            ls_fmt_cmd();
            break;
        case FORMATTING_COMMAND_OP:
            op_fmt_cmd();
            break;
        case FORMATTING_COMMAND_EP:
            ep_fmt_cmd();
            break;
        case FORMATTING_COMMAND_LJ:
            lj_fmt_cmd();
            break;
        case FORMATTING_COMMAND_PB:
            pb_fmt_cmd();
            break;
    }
    //     ldx l0030
    //     rts
    return formatted_line_written_flag == 0;
}

/**
 * Parse a boolean (ON/OFF/1/0) from the format command argument at *y
 * (6502 parse_boolean_from_fmt_cmd).
 *
 * The 6502 returned error status in C (set = bad/missing argument); here it
 * is the return value.  On success *y is advanced past the parsed token and
 * *value holds the parsed byte (exactly what the 6502 left in A).
 *
 * @return true on parse error, false on success.
 */
static bool parse_boolean_from_fmt_cmd(uint8_t* y, uint8_t* value)
{
    uint8_t a;
    // Pseudocode: Parses a boolean (ON/OFF/1/0) from format command argument

    // ;
    // ***************************************************************************************
    // parse_boolean_from_fmt_cmd:
    //     jsr get_current_fmt_cmd_byte
    a = get_current_fmt_cmd_byte(y);
    //     sec
    // (C=1 pre-set for the empty-argument return; conveyed as true)
    *value = a;
    if (a == 0)
        return true;
    //     lda current_format_line_ptr
    //     ldx current_format_line_ptr+1
    // (the 6502 passes the pointer in XA; the C passes it as an argument)

    // MULTIPLE ENTRY POINTS: parse_boolean_from_fmt_cmd, sub_c976c
    return parse_word_flag(current_format_line_ptr, y, value);
}

static const uint8_t l97b0_data[] = {0x4f, 0x4e, 1, 'O', 'F', 'F', 0, 0xff};

/**
 * Parse a word-based flag (ON/OFF/YES/NO, or the digits 1/0) from ptr at
 * *y (6502 sub_c976c).  On success *y is advanced past the token and
 * *value holds the parsed byte (exactly what the 6502 left in A).
 *
 * @return true on parse error (the 6502's C set), false on success.
 */
static bool parse_word_flag(addr_t ptr, uint8_t* y, uint8_t* value)
{
    uint8_t a;
    uint8_t x;

    // sub_c976c
    // Pseudocode: Parses word-based flag (ON/OFF/YES/NO) from format command
    // On entry: ptr = the format-command line (the 6502 passes it in XA),
    //           *y = cursor position into the line (advanced as the word is
    //           consumed).
    // x is a scratch index into the word table.

    // sub_c976c:
    //     lda (((uint8_t*)&tmp89)[0]),y
    a = ram[ptr + *y];
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
    // (C=0 conveyed as a false return)
    *value = a;
    (*y)++;
    if (*y != 0)
        return false;
    // c9783:
c9783:
    //     dey
    (*y)--;
    //     sty l0084
    l0084 = *y;
    //     ldx #0xff
    x = 0xff;
    // c9788:
c9788:
    //     iny
    (*y)++;
    //     lda (((uint8_t*)&tmp89)[0]),y
    a = ram[ptr + *y];
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
        *value = a;
        return false; // C clear (a < 0x20): word matched
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
        *y = l0084;
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
    // (C=1 conveyed as a true return = parse error)
    *value = a;
    return true;

    // MULTIPLE ENTRY POINTS: parse_boolean_from_fmt_cmd, sub_c976c
}

// Evaluates an arithmetic expression with +, - and register references.
// *y = cursor position into the format command line (advanced as the
// expression is consumed).
//
// The 6502 returned the 16-bit result in tmp89 and left C meaningless (no
// caller tested it); here the result is written through result on every
// path, and the return value is true when a term was parsed, false when
// the argument was empty (the line byte at *y was nul, leaving the value
// at its initialised 0).
static bool evaluate_expression_from_fmt_cmd(uint16_t* result, uint8_t* y)
{
    uint8_t count;
    addr_t tmp45 = 0;
    addr_t tmp89;

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
    a = get_current_fmt_cmd_byte(y);
    //     beq c9821
    if (a == 0)
    {
        // empty argument: nothing parsed (the 6502 just returned tmp89=0)
        *result = tmp89;
        return false;
    }
    //     cmp #0x7c ; '|'
    if (a != 0x7c)
        goto c97d5;
    //     jsr get_next_fmt_cmd_byte
    a = get_next_fmt_cmd_byte(y);
    //     beq c9821
    if (a == 0)
        goto c9821;
    //     iny
    (*y)++;
    //     jsr render_register
    render_register(a, x);
    //     jmp c97dc
    goto c97dc;

    // c97d5:
c97d5:
    //     jsr ca6fe
    int parsed;
    parse_decimal_number(&parsed, y);
    tmp89 = (addr_t)parsed;
    // c97dc:
c97dc:
    //     ldx input_buffer_offset+1
    count = l0080;

    //     beq c9804
    if (count == 0)
        goto c9804;
    //     lda #0
    a = 0;
    //     sta input_buffer_offset+1
    l0080 = a;
    //     dex
    count--;

    //     beq c97f7
    if (count == 0)
        goto c97f7;
    //     lda ((uint8_t*)&tmp45)[0]
    a = ((uint8_t*)&tmp45)[0];
    //     sec
    //     sbc ((uint8_t*)&tmp89)[0]
    // (the sec makes this a plain subtraction; its carry is dead — no
    //  reader exists between here and the next flag write)
    tmp89 = tmp45 - tmp89;
    goto c9804;

c97f7:
    tmp89 += tmp45;

c9804:
    tmp45 = tmp89;
    //     jsr get_current_fmt_cmd_byte
    a = get_current_fmt_cmd_byte(y);
    //     beq c9821
    if (a == 0)
        goto c9821;
    //     ldx #1
    count = 1;
    //     cmp #0x2b ; '+'
    //     beq c981c
    if (a == 0x2b)
        goto c981c;
    //     inx                                                               ;
    //     X=0x02
    count++;
    //     cmp #0x2d ; '-'
    //     bne c9821
    if (a != 0x2d)
        goto c9821;
    // c981c:
c981c:
    //     stx input_buffer_offset+1
    l0080 = count;
    //     iny
    (*y)++;
    //     bne c97c0
    goto c97c0;
    // c9821:
c9821:
    //     lda ((uint8_t*)&tmp89)[0]
    //     rts
    *result = tmp89;
    return true;
}

// Reads the next non-space byte of the current format command line, advancing
// the cursor position *y.  Returns 0 (CR/end) or the byte.
static uint8_t get_current_fmt_cmd_byte(uint8_t* y)
{
    // get_current_fmt_cmd_byte:
    while (1)
    {
        uint8_t val = ram[current_format_line_ptr + *y];
        if (val == 0x0d)
            return 0; // Z set
        if (val != 0x20)
            return val; // Z clear
        (*y)++;
    }
}

static uint8_t get_next_fmt_cmd_byte(uint8_t* y)
{
    // get_next_fmt_cmd_byte:
    //     iny
    (*y)++;
    return get_current_fmt_cmd_byte(y);
}

void render_register(uint8_t a, uint8_t x)
{
    addr_t tmp89;

    // render_register
    // render_register:
    //     jsr get_register_address
    //     bcs cada2
    unsigned int* register_value = get_register_address(a);
    //     sty ((uint8_t*)&tmp89)[0]
    tmp89 = 0;
    if (register_value != NULL)
    {
        //     bit lada6
        // (the bit's Z/N/V flags are all dead: clv clears V and the y save
        //  through l0084 is gone, so the instruction is a no-op)
        //     lda (tmp6),y
        //     sta tmp8
        //     iny ; Y=&01
        //     lda (tmp6),y
        //     sta tmp9
        tmp89 = *register_value;
        render_number_to_output_buffer(tmp89, x);
    }
    //     clv
    // (the 6502 clears V here, but no caller of render_register reads it)
    //     rts
    return;
}

static void render_number_to_output_buffer(uint16_t value, uint8_t start_x)
{
    // Pseudocode: Renders a 16-bit number to the output buffer using callback

    // ;
    // ***************************************************************************************
    // ; On Entry:
    // ;     TMP9/TMP8: 16-bit number
    // ;     X: position in the output buffer
    // ;
    // ***************************************************************************************
    // render_number_to_output_buffer:
    //     stx l0082
    l0082 = start_x;
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
        //     ldx l0082
        //     pla
        //     tax
        // (the work uses a local x, so the caller's x register is
        //  preserved without an explicit save/restore)
        uint8_t x = l0082;
        //     sta output_buffer,x
        output_buffer[x] = digit;
        //     cpx #MAX_LINE_LENGTH-2
        if (x < MAX_LINE_LENGTH - 2)
        {
            l0082++;
        }
        //     pla (restore a — but we didn't push it; keep the digit value)
        a = digit;
    }
    //     rts
    return;
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
    render_number_to_callback(tmp89, cli_putchar);
}

static void render_number_to_callback(uint16_t value, void (*cb)(uint8_t))
{
    uint8_t a;
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

static void set_rw_file_handle(uint8_t a)
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
        // (address in YX registers: x = low byte, y = high byte;
        //  passed explicitly as the yx parameter)
        x = (uint8_t)((uintptr_t)footer_text_maybe & 0xff);
        uint8_t y = (uint8_t)((uintptr_t)footer_text_maybe >> 8);
        render_header_or_footer((uint16_t)(y) << 8 | x);
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
    //     bne c928c
    //     inc register_value_p+1
    register_value_array['P' - 'A']++;
    //     lda #1
    //     sta register_value_l
    register_value_array['L' - 'A'] = 1;
    //     lda #0
    //     sta register_value_l+1
    //     sta l0031
    l0031 = 0;
    //     rts
    return;
}

static void print_output_buffer(void)
{
    uint8_t a;
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
    bool is_tab = false;
    do
    {
        a = x;
        {
            uint8_t saved_x = a;
            a = output_buffer[y];
            a = convert_char_for_printing(a, &x, &is_tab);
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
    uint8_t a;
    uint8_t x;
    addr_t tmp89;
    uint32_t accum = 0;
    // l0042/l0043/l0047/l0048 are only used in this function (within
    // printing.c) and in justify_edit_buffer (editor.c) — make them local
    // to avoid global reuse confusion; l0039/l0083 are used elsewhere.
    uint8_t l0042 = 0, l0043 = 0, l0047 = 0, l0048 = 0;

    // microspace_word_processor
    // Pseudocode: Processes words for microspaced justification during printing

    // return_24:
    //     rts

    // c9034:
    //     ldx #0
    x = 0;
    //     stx l0044 / stx l0046 / stx l0045
    accum = 0;
    //     stx l0047
    l0047 = 0;
    //     stx l0039
    l0039 = 0;
    //     stx l0048
    l0048 = 0;
    //     stx l0042
    l0042 = 0;
    //     stx l0043
    l0043 = 0;
    //     stx l0083
    l0083 = 0;
    bool is_tab = false;
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
        convert_char_for_printing(a, &x, &is_tab);
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
    //     bpl c9064
    // (bit test: N = l0083 & 0x80)
    if (!(l0083 & 0x80))
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
    //     bpl c9064
    // (bit test: N = l0083 & 0x80)
    if (!(l0083 & 0x80))
        goto c9064;
    //     lda l0042
    a = l0042;
    //     beq c908a
    if (a == 0)
        goto c908a;
    //     bmi c9087
    if (a & 0x80)
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
    //     ror l0042
    // (the ror result is never read before l0042 is overwritten, and the
    //  result flags are dead too — N is clobbered by the following
    //  inc l0048, and C is clobbered by cmp #0x0d at c90b6 — so the
    //  statement is a no-op)
    // c908a:
c908a:
    //     inc l0048
    l0048++;
    // c908c:
c908c:
    //     lda #0x20 ; ' '
    // (Z set here is never read: bne is an ALWAYS branch and sta/inx/cmp
    //  clobber flags before the next Z consumer)
    a = 0x20;
    //     bne c90b6                                                         ;
    //     ALWAYS branch
    goto c90b6;

    // c9090:
c9090:
    //     inc l0046
    accum += 0x100;
    // c9092:
c9092:
    //     cmp #9
    if (a == 9)
        goto c90a0;
    //     cmp #0x0b
    if (a == 0x0b)
        goto c90a0;
    //     sec
    //     ror l0083
    // (the ror result is never read before l0083 is overwritten, and the
    //  result flags are dead too — the statement is a no-op)
    //     jmp c90b6
    goto c90b6;

    // c90a0:
c90a0:
    //     pha
    {
        uint8_t saved_a2 = a;
        //     lda l0039
        // (Z from this lda is clobbered by the following lda #0)
        a = l0039;
        //     sta l0047
        l0047 = a;
        //     lda #0
        //     sta l0083
        l0083 = 0;
        //     sta l0046 / sta l0044 / sta l0045
        accum = 0;
        //     sta l0048
        l0048 = 0;
        //     sta l0042
        l0042 = 0;
        //     sta l0043
        l0043 = 0;
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
    //     adc l0044
    //     sta l0044
    //     lda l0046
    //     adc l0048
    //     sta l0046
    //     lda l0045
    //     adc l0043
    //     sta l0045
    // (24-bit addition: acc is incremented by l0048 in its two low
    //  bytes and l0043 in its high byte)
    accum += ((uint32_t)l0043 << 16) | ((uint32_t)l0048 << 8) | l0048;
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
    a = accum >> 16;

    //     beq c90f8
    if (a == 0)
        goto c90f8;
    //     lda ruler_right_stop
    a = ruler_right_stop;

    //     beq c90f8
    if (a == 0)
        goto c90f8;
    //     sec
    //     sbc l0047
    //     bcc c90f8
    //     sbc l0045
    //     adc #0
    //     sec
    //     sbc l0046
    //     beq c9101
    // (check whether the remaining width matches the microspacing
    //  accumulator; the 6502 does d = ruler_right_stop - l0047, then
    //  sbc l0045 / adc #0 to recover the borrow)
    int d = ruler_right_stop - l0047;
    if (d < 0)
        goto c90f8;
    if ((uint8_t)(d - (accum >> 16) + (d >= (int)(accum >> 16))) ==
        (uint8_t)((accum >> 8) & 0xFF))
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
    //     sta ((uint8_t*)&tmp89)[1]
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
    // (shift-add multiply: tmp89 = l0045 * microspacing_flag)
    tmp89 = (uint16_t)l0045 * microspacing_flag;
    //     lda l0044
    //     sta l0046
    l0046 = l0044;
    //     jsr sub_cadf0
    //     sta l0045
    //     lda tmp8
    //     sta l0044
    // (16-bit division by 8-bit: l0044 = tmp89 / l0046,
    //  l0045 = tmp89 % l0046)
    l0045 = tmp89 % l0046;
    tmp89 = tmp89 / l0046;
    l0044 = (uint8_t)tmp89;
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
    a = convert_char_for_printing(a, &x, &is_tab);
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
        emit_microspacing_spaces(a, x);
        //     lda #0x20 ; ' '
        a = 0x20;
        //     bne c9160 ; ALWAYS branch
        goto c9160;

        // c915b:
    c915b:
        //     ldx microspacing_flag
        x = microspacing_flag;
        //     jsr sub_c9173
        emit_microspacing_spaces(a, x);
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
        a = convert_char_for_printing(a, &x, &is_tab);
        print_char_x_times(a, x);
    } while (a != 0x0d);
    //     inc register_value_l
    //     bne c8ffb_inline
    //     inc register_value_l+1
    register_value_array['L' - 'A']++;
c8ffb_inline:
    //     ldx line_spacing
    x = line_spacing;
    //     lda l0021
    //     clc
    //     sbc line_spacing
    //     bcs c9009_inline
    // (clc forces C=0, so the sbc subtracts line_spacing + 1; the borrow
    //  branch is taken when l0021 <= line_spacing)
    a = l0021 - line_spacing - 1;
    if (l0021 <= line_spacing)
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

bool parse_decimal_number(int* value, uint8_t* y)
{
    // ca6fe - Parse decimal number from format command line
    // On entry: *y = index into current_format_line_ptr
    // On exit:  *value = parsed value, *y = advanced past digits
    // (no leading whitespace is guaranteed, and strtoul parses the value as
    //  unsigned, so no leading-sign/whitespace handling is needed: a leading
    //  non-digit yields end == start and value 0)
    const char* start;
    if (current_format_line_ptr == (addr_t)(uintptr_t)input_buffer)
        start = (const char*)&input_buffer[*y];
    else
        start = (const char*)&ram[current_format_line_ptr + *y];
    char* end;
    *value = (int)strtoul(start, &end, 10);
    bool parsed = (end != start);
    *y += (uint8_t)(end - start);
    return parsed;
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
        if (scan->ch == delimiter_char)
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
    uint8_t a;
    addr_t ptr5;
    // print_document
    // print_document:
    //     jsr check_not_continuous_editing
    check_not_continuous_editing();
    //     jsr check_for_at_least_150_bytes_free
    check_for_at_least_150_bytes_free();
    //     jsr sub_cb104
    reset_print_registers();
    //     lda top
    //     adc #3
    //     sta ptr5
    //     tax
    //     lda top+1
    //     adc #0
    //     sta ptr5+1
    //     tay
    // (16-bit arithmetic: ptr5 = top + 3)
    ptr5 = top + 3;
    //     txa
    //     adc #0x8d
    //     bcc c8edb
    //     iny
    //     sta first_macro_ptr
    //     sta last_macro_ptr
    //     sty first_macro_ptr+1
    //     sty last_macro_ptr+1
    // (16-bit arithmetic: first_macro_ptr = last_macro_ptr = ptr5 + 0x8d)
    first_macro_ptr = (struct macro*)&ram[ptr5 + 0x8d];
    last_macro_ptr = first_macro_ptr;
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
    //     sta (last_macro_ptr),y
    // (initialise the empty macro list: the first node's next pointer = 0)
    last_macro_ptr->next = 0;
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
    print_loop(ptr5);
    goto c8f0d;
    // c8f0a:
    // c8f0d:
c8f0d:
    //     jsr parse_optional_filename_from_command
    //     bne c8f29
    if (parse_optional_filename_from_command(scan))
    {
        // A = 0x0d (set by parse_optional_filename_from_command's lda #&0d)
        set_rw_file_handle(0x0d);
        print_loop(ptr5);
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

static void print_loop(addr_t ptr5)
{
    uint8_t a;
    uint8_t x;
    addr_t ptr3 = 0; // set before first use (macro start); 0 placates GCC's
                     // cross-function uninitialised analysis
    struct macro* macro;
    uint8_t firstchar, secondchar;
    addr_t cursor; // current format-line address (was global tmp01)

    // print_loop
    // c8f30:
    bool is_tab = false;
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
        //     bcs c8f0a (C=1 conveyed as a true return)
        cursor = prepare_output_line(ptr5, &ptr3);
        if (cursor == 0)
            return;
        //     jsr sub_c916a
        start_microspacing_if_active(a);
        //     ldy #0
        // (Z from ldy #0 is clobbered by the following jsr)
        y = 0;
        //     sty input_buffer_ptr+1
        l0080 = y;
        //     jsr deref_and_check_for_command_prefix
        command_prefix_t cp = deref_and_check_for_command_prefix(y, cursor);
        //     bne c8fce_thunk
        if (cp == NO_COMMAND_PREFIX)
            goto c8fce_thunk_l;
        //     ldy #3
        y = 3;
        //     sty input_buffer_ptr+1
        l0080 = y;
        //     jsr sub_cab6e
        //     bne c8f6e
        // (inlined: Z = (ram[tmp01] == RULER_BYTE))
        if (ram[cursor] != RULER_BYTE)
            goto c8f6e_l;
        //     ldy #3
        y = 3;
        //     ldx #0
        x = 0;
        // loop_c8f5d:
        do
        {
            a = ram[cursor + y];
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
        enum formatting_command fmt_cmd_index = lookup_formatting_command();
        //     bmi c8f7a
        if (fmt_cmd_index == NO_FORMATTING_COMMAND)
            goto c8f7a_l;
        //     jsr execute_formatting_command
        if (execute_formatting_command(fmt_cmd_index))
            goto c8f6b_l;
        //     beq c8f6b
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
        // (macro walks the macro linked list; first_macro_ptr is the head)
        macro = first_macro_ptr;
        //     ldy #1
        y = 1;
        //     lda (current_format_line_ptr),y
        a = ram[current_format_line_ptr + y];
        //     sta tmp8
        firstchar = a;
        //     iny ; Y=0x02
        y++;
        //     lda (current_format_line_ptr),y
        a = ram[current_format_line_ptr + y];
        //     jsr is_uppercase
        // (the 6502 is_uppercase returns C=0 for A-Z/a-z, i.e. what
        //  isalpha() tests; C=1 otherwise)
        //     bcc c8f92
        if (!isalpha(a))
        {
            a = 0x20;
        }
        //     sta tmp9
        secondchar = a;
        // lookup_macro_name:
    lookup_macro_name_l:
        //     ldy #0
        //     lda (((uint8_t*)&tmp67)[0]),y
        //     beq c8f6b
        // (the 6502 tests only the low byte of the next pointer for the
        //  end-of-list marker)
        if ((uint8_t)macro->next == 0)
            goto c8f6b_l;
        //     ldy #2
        //     lda (((uint8_t*)&tmp67)[0]),y
        //     cmp tmp8
        if (macro->name[0] != firstchar)
            goto get_next_macro_in_linked_list_l;
        //     iny ; Y=0x03
        //     lda (((uint8_t*)&tmp67)[0]),y
        //     cmp tmp9
        if (macro->name[1] == secondchar)
            goto c8fb9_l;
        // get_next_macro_in_linked_list:
    get_next_macro_in_linked_list_l:
        //     ldy #0
        //     lda (((uint8_t*)&tmp67)[0]),y
        //     pha
        //     iny ; Y=0x01
        //     lda (((uint8_t*)&tmp67)[0]),y
        //     sta ((uint8_t*)&tmp67)[1]
        //     pla
        //     sta ((uint8_t*)&tmp67)[0]
        //     jmp lookup_macro_name
        // (macro = the next-macro pointer stored at *macro)
        macro = (struct macro*)&ram[macro->next];
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
        // (the macro body cursor persists across prepare_output_line calls,
        //  so it lives here and is passed by reference; the body follows the
        //  macro header via body[])
        ptr3 = (addr_t)(macro->body - ram);
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
            a = ram[cursor + y];
            y++;
            a = convert_char_for_printing(a, &x, &is_tab);
            print_char_x_times(a, x);
        } while (a != 0x0d);
        //     inc register_value_l
        //     bne c8ffb
        //     inc register_value_l+1
        register_value_array['L' - 'A']++;
        //     ldx line_spacing
        x = line_spacing;
        //     lda l0021
        //     clc
        //     sbc line_spacing
        //     bcs c9009
        // (clc forces C=0, so the sbc subtracts line_spacing + 1; the borrow
        //  branch is taken when l0021 <= line_spacing)
        a = l0021 - line_spacing - 1;
        if (l0021 <= line_spacing)
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
    uint8_t a = 0x0d;
    print_char_x_times(a, x);
}

read_block_status_t read_block_from_file(addr_t* cursor, addr_t limit)
{
    uint8_t a;

    uint8_t y;

    uint8_t x;

    // Set when the read ended on a NUL/EOF byte (as opposed to the destination
    // block filling up); distinguishes READ_BLOCK_DONE from READ_BLOCK_MORE.
    bool eof = false;

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
    if (a == 0)
    {
        eof = true;
        goto c8cf2;
    }
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
            write_cr_to_memory(&tmp01);
            a = saved_a_;
        }
        x++;
    }
c8cdb:
    // c8cdb:
    //     inc l0083
    l0083++;
    //     jsr write_byte_to_memory
    write_byte_to_memory(cursor, a);
    //     txa

    //     beq c8c95
    if (x == 0)
        goto c8c95;
    //     lda ((uint8_t*)&tmp01)[1]
    //     cmp l0081
    //     bcc c8c95
    //     bne c8cf1
    //     lda ((uint8_t*)&tmp01)[0]
    //     cmp input_buffer_offset+1
    // (16-bit comparison: tmp01 < limit)
    if (*cursor < limit)
        goto c8c95;
    // c8cf1:
    //     clc
    eof = false;
c8cf2:
    // c8cf2:
    //     lda l0084
    a = l0084;
    //     beq c8cfa
    if (a != 0)
    {
        write_cr_to_memory(cursor);
    }
    // c8cfa:
    //     lda l0082
    a = l0082;
    // (return: EMPTY if l0082 == 0, else eof selects DONE vs block-full MORE)
    if (a == 0)
        return READ_BLOCK_EMPTY;
    if (eof)
        return READ_BLOCK_DONE;
    return READ_BLOCK_MORE;
    //     rts
}

static void render_header_or_footer(uint16_t yx)
{
    uint8_t a;
    // render_header_or_footer
    // Pseudocode: Renders header or footer text with centering and
    // justification

    // ;
    // ***************************************************************************************
    // render_header_or_footer:
    // (address passed in YX: high byte in y, low byte in x)
    tmp45 = yx;
    //     ldy #0
    //     sty l0082
    //     lda (((uint8_t*)&tmp45)[0]),y
    // (y is 0 throughout, so the first header/footer text byte is read
    // directly)
    a = ram[tmp45];
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
    addr_t section_start = compute_header_left_section(tmp45);
    //     jsr sub_c93fd
    bool parity = get_page_parity();
    //     bcs c932e
    if (!parity)
    {
        section_start = compute_header_odd_page_section(tmp45);
    }
    //     jsr sub_c93c8
    copy_header_footer_text(section_start);
    //     jsr c937b
    print_output_buffer();
    //     jsr sub_c939b
    section_start = compute_header_middle_section(tmp45);
    //     jsr sub_c93c8
    copy_header_footer_text(section_start);
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
    // (flags not used: get_right_margin sets them fresh; plain shift)
    a >>= 1;
    //     sta l0081
    l0081 = a;
    //     jsr sub_c93be
    a = get_right_margin();
    //     beq c9355
    if (a == 0)
        goto c9355;
    //     lsr
    // (flags not used; plain shift)
    a >>= 1;
    //     sec
    //     sbc l0081
    //     bcc c9355
    //     sbc l0039
    //     bcc c9355
    // (the two sbc subtractions are equivalent to a C comparison:
    //  reach add_justification_spaces iff a >= l0081 + l0039, with
    //  the count a - l0081 - l0039)
    if (a >= l0081 + l0039)
    {
        x = a - l0081 - l0039;
        a = add_justification_spaces(x);
    }
c9355:
    //     jsr c937b
    print_output_buffer();
    //     jsr sub_c93a1
    section_start = compute_header_odd_page_section(tmp45);
    //     jsr sub_c93fd
    bool parity2 = get_page_parity();
    //     bcs c9363
    if (!parity2)
    {
        section_start = compute_header_left_section(tmp45);
    }
    //     jsr sub_c93c8
    copy_header_footer_text(section_start);
    //     jsr sub_c93be
    a = get_right_margin();
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
    // (only reach add_justification_spaces when the right margin is
    //  non-zero and both sbc subtractions succeed; otherwise fall through
    //  to the shared end, c937b)
    if (a != 0)
    {
        //     stx l0081
        l0081 = x;
        //     sec
        //     sbc l0081
        //     bcc c937b
        //     sbc l0039
        //     bcc c937b
        //     tax
        //     inx
        //     jsr sub_c941a
        // (the two sbc subtractions are equivalent to a C comparison:
        //  reach add_justification_spaces iff a >= l0081 + l0039, with
        //  the count a - l0081 - l0039)
        if (a >= l0081 + l0039)
        {
            x = a - l0081 - l0039;
            x++;
            a = add_justification_spaces(x);
        }
    }
    // c937b:
    //     jsr print_output_buffer
    //     rts
    print_output_buffer();
    return;
}

static void render_new_page(void)
{
    uint8_t a;
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
    //     bvc c92d4
    // (bit test: V = print_flags & 0x40)
    if (!(print_flags & 0x40))
        goto c92d4;
    //     jsr stop_printing
    stop_printing();
    //     jsr print_inline_string
    //     .ascii "\rPage "
    //     .byte 0
    cli_putstring("\nPage ");

    //     ldx register_value_p
    render_number_to_screen(register_value_array['P' - 'A']);
    //     jsr print_inline_string
    //     .ascii ".."
    //     .byte 0
    cli_putstring("..");

    //     jsr flush_and_read_char
    a = screen_getchar();
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
    // c92cc: (Q pressed — stop printing)
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
        // (address in YX registers: x = low byte, y = high byte;
        //  passed explicitly as the yx parameter)
        x = (uint8_t)((uintptr_t)header_text_maybe & 0xff);
        uint8_t y = (uint8_t)((uintptr_t)header_text_maybe >> 8);
        render_header_or_footer((uint16_t)(y) << 8 | x);
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
 * the next character that is not the delimiter delimiter_char.
 *
 * Advances the scan position past any run of delimiter characters and stops at
 * the first character that differs from delimiter_char (a "mark"/argument
 * character) or
 * at the end of the command line.
 *
 * @param state On return holds the scan result (see struct scan_state):
 *              state->ch is the character at the scan position (the first
 *              non-delimiter character, or 0x0d if the end of the line was
 *              reached first, or delimiter_char itself when delimiter_char ==
 *              0x0d); state->pos
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
    state->ch = delimiter_char;
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
        if (state->ch != delimiter_char)
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

/**
 * Emits microspacing spaces while preserving the character being printed.
 *
 * @param a character to preserve across the printer callback
 * @param x requested microspacing amount
 */
static void emit_microspacing_spaces(uint8_t a, uint8_t x)
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
    //     stx l0043
    l0043 = x;
    //     lda #9
    printer_driver_ptr->printer_microspace();
    //     pla
    // return_25:
    //     rts
    return;
}

/**
 * Prepare the next line for printing (6502 sub_c9188): fetches the next
 * output line via read_next_output_line (handling macro execution) and
 * points current_format_line_ptr at it.
 *
 * The 6502 kept the macro-body cursor in the global ptr3, persisting it
 * across successive calls; here the caller owns that cursor and passes it
 * in by reference.
 *
 * @return the address of the prepared line, or NULL (0) when no more
 *         output remains (the 6502 returned C set; print_loop's bcs
 *         c8f0a).
 */
addr_t prepare_output_line(addr_t read_limit, addr_t* macro_cursor)
{
    addr_t cursor;
    uint8_t a;

    uint8_t x;
    uint8_t y;

    // sub_c9188
    //  Ptrs:   ptr1, (*macro_cursor), read_limit
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
    //     sta input_buffer_ptr+1
    //     lda ptr5+1
    //     sta l0081
    // (16-bit copy: *cursor = read_limit.  The 6502 also stored the low/high
    //  bytes into input_buffer_ptr+1 (l0080) and l0081; every reachable
    //  reader of those bytes re-initialises them first, so the stores are
    //  omitted.)
    cursor = read_limit;
    //     jsr sub_c9241
    if (read_next_output_line(read_limit, &cursor) == READ_BLOCK_DONE)
    {
        //     bcs return_26 (C=1 conveyed as a true return)
        return 0;
    }
    //     lda ptr5 / ldy ptr5+1 / bne c91d0
    // (high byte of read_limit nonzero: no macro to execute; the result is
    //  just read_limit itself)
    if (read_limit != 0)
    {
        cursor = read_limit;
        current_format_line_ptr = cursor;
        return cursor;
    }
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
        //     lda ((*macro_cursor)),y
        a = ram[(*macro_cursor) + y];
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
    //     adc (*macro_cursor)
    //     sta (*macro_cursor)
    //     bcc c91cc
    // (16-bit arithmetic: (*macro_cursor) += y)
    (*macro_cursor) += a;
    //     lda ptr1 / ldy ptr1+1 (folded via c91d0)
    // c91d0:
    cursor = ptr1;
    current_format_line_ptr = cursor;
    //     clc
    // return_26:
    //     rts
    return cursor;

    // c91da:
c91da:
    //     iny
    y++;
    //     lda ((*macro_cursor)),y
    a = ram[(*macro_cursor) + y];
    //     sec
    //     sbc #0x30 ; '0'
    //     bcc c9225
    // (sbc with C=1 is a plain subtraction; borrow means non-digit)
    if (a < 0x30)
        goto c9225;
    a -= 0x30;
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
    a = ram[read_limit + y];
    //     cmp #0x0d
    //     beq c9223
    if (a == 0x0d)
        goto c9223;
    //     jsr sub_c9228
    {
        enum parse_register_result_t r = parse_register_reference(a);
        //     beq c91f5
        if (r == PARSE_REGISTER_MARKER)
            goto c91f5;
        //     bvs c91f5
        if (r == PARSE_REGISTER_VALUE)
            goto c91f5;
    }
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
    a = ram[read_limit + y];
    //     cmp #0x0d
    //     beq c9223
    if (a == 0x0d)
        goto c9223;
    //     jsr sub_c9228
    {
        enum parse_register_result_t r = parse_register_reference(a);
        //     beq c9209
        if (r == PARSE_REGISTER_MARKER)
            goto c9209;
        //     bvs c921b
        if (r == PARSE_REGISTER_VALUE)
            goto c921b;
    }
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

static enum parse_register_result_t parse_register_reference(uint8_t a)
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
        return PARSE_REGISTER_MARKER; // Z live
    }
    //     cmp #0x3c ; '<'
    //     bne c923c
    if (a == 0x3c)
    {
        a = 0x40;
        l0082 = a;
        return PARSE_REGISTER_MARKER; // Z live
    }
    //     bit l0082
    // (bit sets Z and V; Z is killed by the following set_flags, so V
    //  is the only surviving flag on this path)
    if (a & l0082)
        return PARSE_REGISTER_VALUE; // V live
    //     ora #0
    //     rts
    return PARSE_REGISTER_OTHER;
}

static read_block_status_t read_next_output_line(addr_t limit, addr_t* cursor)
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
        return read_block_from_file(cursor, limit);
    } //     ldy #0
    uint8_t y = 0;
    // loop_c9247:
    do
    {
        a2 = ram[printer_ptr6 + y];
        if (a2 == 0)
            return READ_BLOCK_DONE;
        ram[(*cursor) + y] = a2;
        printer_ptr6++;
        (*cursor)++;
    } while (a2 != 0x0d);
    //     clc
    // return_27:
    //     rts
    return READ_BLOCK_MORE;

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
    //     sbc top_margin
    //     bcc c930d
    if (a < top_margin)
        goto c930d;
    a -= top_margin;
    //     sbc header_margin
    //     bcc c930d
    if (a < header_margin)
        goto c930d;
    a -= header_margin;
    //     clc
    //     sbc bottom_margin
    //     bcc c930d
    if (a < bottom_margin)
        goto c930d;
    a -= bottom_margin;
    //     sbc footer_margin
    //     bcc c930d
    if (a < footer_margin)
        goto c930d;
    a -= footer_margin;
    x = a;
c930d:
    //     stx l0021
    l0021 = x;
    //     rts
    return;
}

static addr_t compute_header_left_section(addr_t tmp45)
{
    uint8_t a;
    addr_t tmp23;

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
    return tmp23;
}

static addr_t compute_header_middle_section(addr_t tmp45)
{
    uint8_t y;

    addr_t tmp23;

    // sub_c939b:
    //     jsr sub_c93b6
    y = get_line_width(tmp45);
    //     jmp c93a7
    // c93a7:
    //     iny
    //     tya
    //     dey
    // c93aa:
    //     clc
    //     adc ((uint8_t*)&tmp45)[0]
    //     sta ((uint8_t*)&tmp23)[0]
    //     lda ((uint8_t*)&tmp45)[1]
    //     adc #0
    //     sta ((uint8_t*)&tmp23)[1]
    tmp23 = tmp45 + y + 1;
    return tmp23;
}

static addr_t compute_header_odd_page_section(addr_t tmp45)
{
    uint8_t a;

    // sub_c93a1:
    //     jsr sub_c93b6
    //     jsr c93b8
    // c93a7:
    uint8_t y = scan_string_length(get_line_width(tmp45), tmp45);
    addr_t tmp23;
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
    return tmp23;
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
    //     bne return_29
    if (ruler_right_stop != 0)
        return ruler_right_stop;
    //     lda l003a
    //     sec
    //     sbc #1
    // return_29:
    return l003a - 1;
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
    // (Z from ldy #0 is clobbered by the following lda (tmp23),y)
    y = 0;
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
    render_register(a, x);
    //     jmp c93ce
    goto c93ce;
}

static bool get_page_parity(void)
{
    // Pseudocode: Checks two_sided_flag and returns page parity for alternate
    // layout

    // sub_c93fd:
    //     sec
    //     lda two_sided_flag
    //     beq return_31
    //     lda register_value_p
    //     lsr
    // return_31:
    // (return value: C flag = 1 if two_sided_flag is 0, otherwise
    //  the parity of register P; true means "even page / left-hand")
    if (two_sided_flag == 0)
        return true;
    return (register_value_array['P' - 'A'] & 1) != 0;
}

static void output_left_margin(void)
{
    uint8_t a, x;

    // Pseudocode: Outputs left margin spaces, adjusting for two-sided printing

    // sub_c9407:
    //     jsr sub_c93fd
    bool parity = get_page_parity();
    //     lda left_margin
    a = left_margin;
    //     bcc c9415
    if (parity)
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

/**
 * Converts a character for printing and updates its display width.
 *
 * @param a character to convert
 * @param[out] x converted character width
 * @param is_tab tab-state carried between characters
 * @return converted character
 */
static uint8_t convert_char_for_printing(uint8_t a, uint8_t* x, bool* is_tab)
{
    // Pseudocode: Converts character for printing, updates x position counter

    // sub_c9431:
    //     jsr sub_ca5ae
    a = process_document_character(a, x, is_tab);
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
        a = *x;
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
    return a;
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
    memset(register_value_array, 0, sizeof(register_value_array));
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
    //     sta register_value_l
    register_value_array['P' - 'A'] = a;
    register_value_array['L' - 'A'] = a;
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

static void write_byte_to_memory(addr_t* cursor, uint8_t a)
{
    // write_byte_to_memory:
    //     ldy #0
    //     sta (((uint8_t*)&tmp01)[0]),y
    ram[*cursor] = a;
    //     inc ((uint8_t*)&tmp01)[0]
    (*cursor)++;
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
    l0084 = 0;
    //     sty l0083
    l0083 = 0;
    // return_16:
    //     rts
}

static void write_cr_to_memory(addr_t* cursor)
{
    // write_cr_to_memory:
    //     lda #0x0d
    write_byte_to_memory(cursor, 0x0d);
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
static void default_printer_getflags(uint8_t* x, uint8_t* y)
{
    *x = 0;
    *y = 0;
}

static const struct printer_driver default_printer_driver = {
    .print_char = default_print_char,
    .printer_on = default_printer_on,
    .printer_off = default_printer_off,
    .printer_microspace = default_printer_microspace,
    .printer_getflags = default_printer_getflags,
};
