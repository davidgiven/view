#include "document.h"
#include "io.h"
#include "printing.h"
#include <stdio.h>
#include <ctype.h>

// call_printer_driver moved to printing.c

command_prefix_t check_for_command_prefix(uint8_t ch)
{
    // check_for_command_prefix:
    //     cmp #0x80
    //     beq return_81
    //     cmp #0x81
    //     clc
    // return_81:
    //     rts
    if (ch == COMMAND_BYTE)
        return COMMAND_PREFIX;
    if (ch == RULER_BYTE)
        return RULER_PREFIX;
    return NO_COMMAND_PREFIX;
}

control_code_t check_for_control_code(uint8_t a)
{
    // Pseudocode: Checks if character is a control code (0x1c or 0x1d)
    // Returns HIGHLIGHT1_CODE for 0x1c, HIGHLIGHT2_CODE for 0x1d, or
    // NO_CONTROL_CODE otherwise.

    // check_for_control_code:
    //     cmp #0x1c
    //     beq return_63
    //     cmp #0x1d
    //     clc
    //     rts
    if (a == 0x1c)
        return HIGHLIGHT1_CODE;
    if (a == 0x1d)
        return HIGHLIGHT2_CODE;
    return NO_CONTROL_CODE;
}

int compute_bytes_free(void)
{
    // compute_bytes_free
    // Pseudocode: Computes number of free bytes between top and himem
    // Returns the 16-bit difference himem - top.  (The 6502 returns it in YX;
    // callers used to read the global y/x registers and now use the return
    // value.)

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
    return (int)himem - (int)top;
}

void check_for_at_least_150_bytes_free(void)
{
    // Pseudocode: Checks if at least 150 bytes of memory are available

    // check_for_at_least_150_bytes_free:
    //     jsr compute_bytes_free
    //     tya
    //     bne return_6
    //     cpx #0x96
    //     bcs return_6
    if (compute_bytes_free() >= 0x96)
        return;

    // MULTIPLE ENTRY POINTS: check_for_at_least_150_bytes_free,
    // display_not_enough_memory
    display_not_enough_memory();
}

command_prefix_t deref_and_check_for_command_prefix(uint8_t y, addr_t tmp01)
{
    // deref_and_check_for_command_prefix:
    //     lda (((uint8_t*)&tmp01)[0]),y
    uint8_t a;
    a = ram[tmp01 + y];
    return check_for_command_prefix(a);
}

// Returns COMMAND_PREFIX for ch == 0x80 (format command), RULER_PREFIX for
// ch == 0x81 (ruler line), or NO_COMMAND_PREFIX otherwise.

void display_document_file_state(void)
{
    uint8_t y;

    // display_document_file_state
    // display_document_file_state:
    //     jsr stop_printing
    stop_printing();
    //     jsr print_inline_string
    //     .ascii "Editing "
    //     .byte 0
    cli_putstring("Editing ");

    //     lda file_edit_flags
    uint8_t a = file_edit_flags;
    //     beq c8a21
    if (a == 0)
        goto c8a21;
    //     ldy #0
    y = 0;
    // loop_c89fa:
    //     lda input_filename,y
    //     cmp #0x0d
    //     jsr bdos_print_char
    //     iny
    //     bne loop_c89fa
    // (loop restructured)
    for (;;)
    {
        a = input_filename[y];
        if (a == 0x0d)
            break;
        cli_putchar(a);
        y++;
    }
    // c8a07:
    //     bit file_edit_flags
    if ((file_edit_flags & 0x40))
        goto c8a19;
    //     jsr print_inline_string
    //     .ascii " to "
    //     .byte 0
    cli_putstring(" to ");

    //     ldy #0
    y = 0;
    // loop_c8a15:
    //     lda output_filename,y
    //     iny
    //     jsr bdos_print_char
    //     cmp #0x0d
    //     bne loop_c8a15
    // (loop restructured)
    for (;;)
    {
        a = output_filename[y];
        y++;
    c8a19:
        // c8a19:
        //     jsr bdos_print_char
        cli_putchar(a);
        if (a == 0x0d)
            break;
    }
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

void find_margins_of_current_ruler_buffer(void)
{
    // find_margins_of_current_ruler_buffer
    // Pseudocode: Finds left (>) and right (<) margin stops in the current
    // ruler buffer

    // ;
    // ***************************************************************************************
    // find_margins_of_current_ruler_buffer:
    // sub_cabc4 (inlined):
    //     ldy #0
    //     sty ruler_right_stop
    //     sty ruler_left_stop
    uint8_t a;
    uint8_t y = 0;
    ruler_right_stop = 0;
    ruler_left_stop = 0;
    // loop_caba5:
    do
    {
        //     lda (current_ruler_ptr),y
        a = ram[current_ruler_ptr + y];
        //     cmp #0x3e ; '>'
        if (a == 0x3e)
        {
            ruler_left_stop = y;
        }
        //     cmp #0x3c ; '<'
        if (a == 0x3c)
        {
            ruler_right_stop = y;
        }
        //     cmp #0x0d
        if (a == 0x0d)
            break;
        //     iny
        y++;
        //     cpy #0x84
    } while (y != MAX_LINE_LENGTH);
    // cabbc:
    //     sty l003a
    l003a = y;
    //     lda ruler_left_stop
    //     cmp ruler_right_stop
    //     bcc return_72
    if (ruler_left_stop < ruler_right_stop)
        return;
    // fall through to re-zero margins
    ruler_right_stop = 0;
    ruler_left_stop = 0;
}

void print_char(uint8_t a)
{
    // print_char
    // print_char:
    //     cmp #0x0d
    //     beq c9462
    if (a == 0x0d)
        goto c9462;
    //     cmp #0x20 ; ' '
    //     bne c9468
    if (a != 0x20)
        goto c9468;
    //     inc print_xpos
    print_xpos++;
    //     rts
    return;

c9462:
    //     lda #0
    //     sta print_xpos
    print_xpos = 0;
    //     lda #0x0d
c9468:
    //     jsr sub_c9445
    print_alignment_spaces(a);
    print_char_just_to_screen(a);
}

void print_char_just_to_screen(uint8_t a)
{
    // print_char_just_to_screen
    // print_char_just_to_printer:
    //     bit print_flags
    //     bpl c9472
    if ((print_flags & 0x80))
    {
        printer_driver_ptr->print_char(a);
        return;
    }
    //     jsr check_for_control_code
    control_code_t cc = check_for_control_code(a);
    //     bne c9488
    if (cc == NO_CONTROL_CODE)
        goto c9488;
    //     pha
    {
        uint8_t saved_a = a;
        //     lda #0x2d ; '-'
        a = (cc == HIGHLIGHT1_CODE) ? 0x2d : 0x2a;
        //     bcs c947e
        // c947e:
        //     jsr set_inverted_text_if_not_mode_7
        screen_setstyle(STYLE_REVERSE);
        //     jsr bdos_print_char
        cli_putchar(a);
        //     pla
        a = saved_a;
    }
    //     jmp set_normal_text_if_not_mode_7
    screen_setstyle(0);
    return;

c9488:
    //     jmp bdos_print_char
    if (a == 0x0d)
    {
        cli_putchar('\n');
        return;
    }
    cli_putchar(a);
}

/**
 * Process one document character, performing tab, ruler and highlight-code
 * expansion.
 *
 * @param a the character to process (the byte from the current edit line).
 * @param[out] x on return, holds 1 on the ordinary paths (ca5d1 / ca5f8), or
 * the tab offset (index of the first `*` ruler stop beyond l0039) on the tab
 * path.
 * @param[in,out] is_tab on entry, the previous character in the same walk's
 * tab-expansion status (emulated from the 6502's SEC/CLC carry flag, but
 * passed via the bool since sub_ca5ae starts with cmp #9 which clobbers the
 * 6502 carry);
 * the tab path (ca5e1) uses true as carry-in for ca5f1, the indent path
 * (ca5d9) always uses false (reached via beq with C set).  On return, true
 * if a tab expansion was performed (the C flag of the
 * 6502 sub_ca5ae), false otherwise.
 *
 * Reads the globals current_ruler_ptr, ruler_left_stop, l0039, l003a,
 * print_flags and highlight_code.
 *
 * @return the processed character, normally `0x20` (space): tabs and
 * characters below `0x1a` map to space, and characters in `[0x1a, 0x20)`
 * map to a highlight code when `print_flags & 0x80`.
 */
uint8_t process_document_character(uint8_t a, uint8_t* x, bool* is_tab)
{

    // process_document_character
    // sub_ca5ae:
    //     cmp #9
    if (a == 9)
        goto ca5e1;
    //     cmp #0x10
    if (a == 0x10)
        goto ca5d5;
    //     cmp #0x0b
    if (a == 0x0b)
        goto ca5d9;
    //     cmp #0x1a
    //     beq ca5d5
    if (a == 0x1a)
        goto ca5d5;
    //     bcc ca5d1
    if (a < 0x1a)
        goto ca5d1;
    //     cmp #0x20 ; ' '
    //     bcs ca5d1
    // (The 6502 saves/restores y via l0084 around this block; print_flags is
    //  read directly, so the register is never clobbered.)
    if (a < 0x20)
    {
        if ((print_flags & 0x80))
        {
            //     sbc #0x1b
            // (carry is clear from the cmp #0x20, so this is a -= 0x1b + 1)
            a = (uint8_t)(a - 0x1b - 1);
            //     tax
            //     lda highlight1_code,x
            *x = a;
            a = highlight_code[*x];
        }
    }
ca5d1:
    //     ldx #1
    *x = 1;
    //     clc
    *is_tab = false;
    //     rts
    return a;

ca5d5:
    //     lda #0x20 ; ' '
    a = 0x20;
    //     bne ca5d1
    goto ca5d1;

ca5d9:
    //     lda ruler_left_stop
    a = ruler_left_stop;

    //     beq ca5d5
    if (a == 0)
        goto ca5d5;
    //     sty l0084
    //     bne ca5f1
    // (ca5d9 is reached via beq from cmp #0x0b with a==0x0b, so the 6502
    //  carry is always set; the indent ruler path enters ca5f1 with
    //  a decremented by 1 to account for the borrow)
    a--;
    goto ca5f1;

ca5e1:
    //     sty l0084
    //     ldy l0039
    // (The 6502 uses y as the tab counter with y saved in l0084; the C uses a
    //  local so y is never touched.)
    {
        uint8_t tab_pos = l0039;
        // loop_ca5e5:
        for (;;)
        {
            //     iny
            tab_pos++;
            //     cpy l003a
            //     bcs ca5f8
            if (tab_pos >= l003a)
                goto ca5f8;
            //     lda (current_ruler_ptr),y
            a = ram[current_ruler_ptr + tab_pos];
            //     cmp #0x2a ; '*'
            //     bne loop_ca5e5
            if (a != 0x2a)
                continue;
            break;
        }
        //     tya
        a = tab_pos;
    }
ca5f1:
    //     sbc l0039
    {
        bool no_borrow = (a >= l0039);
        a -= l0039;
        //     tax
        *x = a;
        //     beq ca5f8
        if (*x == 0)
            goto ca5f8;
        //     bcs ca5fa
        if (no_borrow)
            goto ca5fa;
    }
ca5f8:
    //     ldx #1
    *x = 1;
ca5fa:
    //     lda #0x20 ; ' '
    a = 0x20;
    //     ldy l0084
    //     sec
    *is_tab = true;
    //     rts
    return a;
}

void return_to_cli_prompt(void)
{
    longjmp(env, JMP_CLI);
}

void print_alignment_spaces(uint8_t a)
{
    // sub_c9445
    // Pseudocode: Outputs print_xpos number of spaces to align printer

    // sub_c9445:
    //     pha
    //     lda print_xpos
    a = print_xpos;
    //     beq c9453
    if (a == 0)
        goto c9453;
    //     lda #0x20 ; ' '
    a = 0x20;
    // loop_c944c:
    do
    {
        print_char_just_to_screen(a);
        print_xpos--;
    } while (print_xpos != 0);
    // c9453:
c9453:
    //     pla
    //     rts
    return;
}

void load_current_ruler(uint8_t y)
{
    // cab91

    // Pseudocode: Sets current_ruler_ptr from stack at ruler_index_ptr offset

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
    // (16-bit arithmetic: the two stacked bytes form the stored ruler
    //  pointer, high byte first; current_ruler_ptr = stored + 3)
    ruler_index_ptr = y;
    current_ruler_ptr = ruler_index[y >> 1] + 3;
    // MULTIPLE ENTRY POINTS: pop_from_ruler_index, cab91
    //     (falls through to find_margins_of_current_ruler_buffer)
    find_margins_of_current_ruler_buffer();
}

void ensure_cr_at_document_top(void)
{
    // cb05a
    // cb05a: Ensures at least one CR at top of document

    if (page != top)
        return;
    //     inc top
    top++;
    //     bne cb06c
    if (top != 0)
        goto cb06c;
    //     inc top+1
    // (automatically handled by 16-bit top)
    // cb06c:
cb06c:
    //     sta current_line_ptr
    //     sty current_line_ptr+1
    current_line_ptr = page;
    //     ldy #0
    uint8_t y = 0;
    //     lda #0x0d
    uint8_t a = 0x0d;
    //     sta (page),y
    ram[page + y] = a;
    //     tya
    a = y;
    //     sta (top),y
    ram[top + y] = a;
    // return_85:
    //     rts
}

void close_file(void)
{
    if (file_ptr)
    {
        fclose(file_ptr);
        file_ptr = NULL;
    }
}

uint8_t create_default_ruler(addr_t ruler_addr)
{
    // create_default_ruler
    // Pseudocode: Creates a default ruler with tab stops every 6 columns

    // ;
    // ***************************************************************************************
    // create_default_ruler:
    //     sta ((uint8_t*)&tmp01)[0]
    uint8_t x;
    addr_t tmp01 = ruler_addr;

    //     lda #0
    //     tay                                                               ;
    //     Y=0x00
    uint8_t a;
    y = 0;
    // loop_cb0e7:
    for (;;)
    {
        //     lda #0x2e ; '.'
        a = 0x2e;
        // loop_cb0e9:
        for (;;)
        {
            //     sta (((uint8_t*)&tmp01)[0]),y
            ram[tmp01 + y] = a;
            //     iny
            y++;
            //     tya
            a = y;
            //     tax
            x = a;
            //     inx
            x++;
            //     clc
            //     adc #6
            a += 6;
            //     cmp screen_width
            if (a == screen_maxcolumn)
                goto cb0ff;
            //     txa
            a = x;
            //     and #7
            a &= 7;
            //     bne loop_cb0e7
            if (a != 0)
                break;
            //     lda #0x2a ; '*'
            a = 0x2a;
            //     bne loop_cb0e9 ; ALWAYS branch
        }
    }

    // cb0ff:
cb0ff:
    //     lda #0x3c ; '<'
    a = 0x3c;
    //     sta (((uint8_t*)&tmp01)[0]),y
    ram[tmp01 + y] = a;
    //     rts
    return y;
}

uint8_t get_byte_from_file(void)
{
    // get_byte_from_file
    int c = fgetc(file_ptr);
    if (c == EOF || c == 0)
        return 0;
    return (uint8_t)c;
}

unsigned int* get_register_address(uint8_t a)
{
    // get_register_address
    // get_register_address: Gets a pointer to the register value by letter
    // name.  Returns NULL if the letter is not a letter A-Z/a-z (invalid).

    //     jsr is_uppercase
    //     bcs return_77
    if (!isalpha(a))
        return NULL;
    //     and #0xdf
    a &= 0xdf;
    //     sbc #0x40 ; '@'
    //     asl
    //     adc #<register_value_array
    //     sta ((uint8_t*)&tmp67)[0]
    //     lda #>register_value_array
    //     adc #0
    //     sta ((uint8_t*)&tmp67)[1]
    // (16-bit arithmetic: pointer = register_value_array + (a - 'A') * 2)
    return &register_value_array[a - 0x41];
}

uint8_t initialise_document(void)
{
    uint8_t y;

    // initialise_document
    // initialise_document:
    //     lda #0
    addr_t tmp89;
    //     sta printer_driver_name
    printer_driver_name[0] = 0;
    //     sta format_mode_flag
    format_mode_flag = 0;
    //     sta justifying_flag
    justifying_flag = 0;
    //     sta insert_mode_flag
    insert_mode_flag = 0;
    //     ldx #(input_buffer_ptr+2 - print_flags)
    // loop_cafe9:
    //     sta print_flags,x
    //     dex
    //     bpl loop_cafe9
    print_flags = 0;
    edit_buffer_dirty_flag = 0;
    edit_buffer_unpacked_flag = 0;
    l006f = 0;
    ruler_index_ptr = 0;
    hscroll_pos = 0;
    l0072 = 0;
    l0073 = 0;
    l0074 = 0;
    flags_need_redrawing_flag = 0;
    l0076 = 0;
    ypos = 0;
    print_xpos = 0;
    l0079 = 0;
    l007a = 0;
    cursor_moved_flag = 0;
    delimiter_char = 0;
    line_format_status = 0;
    input_buffer_offset = 0;
    // cafee:
    //     ldx oshwm
    //     ldy oshwm+1
    //     iny
    //     inx
    //     stx page
    //     bne caffe
    //     iny
    // caffe:
    //     sty page+1
    page = oshwm + 0x101;
    //     ldy #0
    y = 0;
    //     sty file_edit_flags
    file_edit_flags = y;
    //     sty xpos
    xpos = y;
    //     lda #0xaa
    //     sta (oshwm),y
    ram[oshwm + y] = 0xaa;
    //     lda page
    //     sec
    //     sbc #1
    //     sta ((uint8_t*)&tmp89)[0]
    //     lda page+1
    //     sbc #0
    //     sta ((uint8_t*)&tmp89)[1]
    tmp89 = page - 1;
    //     lda #0x0d
    //     sta (((uint8_t*)&tmp89)[0]),y
    ram[tmp89] = 0x0d;
    //     sta current_line_buffer + 0x89
    ram[RAM_CURRENT_LINE_BUF + MAX_LINE_LENGTH - 1] = 0x0d;
    //     lda page / sta top / lda page+1 / sta top+1
    top = page;
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
    ptr1 = RAM_CURRENT_LINE_BUF;
    current_format_line_ptr = RAM_EDIT_BUFFER;
    //     lda #<(current_ruler_buffer)
    y = create_default_ruler(RAM_CURRENT_RULER_BUF);
    //     iny
    y++;
    //     lda #0x0d
    //     sta (((uint8_t*)&tmp01)[0]),y
    ram[RAM_CURRENT_RULER_BUF + y] = 0x0d;
    //     ldy #0xff
    //     lda #<(just_before_current_ruler_buffer)
    //     sta (oshwm),y
    //     dey
    //     lda #>(just_before_current_ruler_buffer)
    //     sta (oshwm),y
    ruler_index[0x7f] = RAM_JUST_BEFORE_RULER_BUF;
    //     jsr move_cursor_to_top_of_document
    move_cursor_to_top_of_document();
    //     jsr clear_cmd
    clear_cmd();
    //     (falls through to cb05a)
    ensure_cr_at_document_top();
    return a;
}

// Returns the marker index 0-5, or MARKER_INVALID if the character is not a
// valid marker ('1'-'6').  Beeps on a non-digit marker (the 6502 branches to
// the beep entry point); out-of-range digits return MARKER_INVALID silently.
int lookup_marker(uint8_t a)
{
    // lookup_marker
    // lookup_marker: Converts marker character '1'-'6' to index

    //     sec
    //     sbc #0x31 ; '1'
    //     bcc loop_caced
    // (sbc with C=1 is a plain subtraction; borrow means invalid marker.
    //  loop_caced is beep.)
    if (a < 0x31)
    {
        beep();
        return MARKER_INVALID;
    }
    a -= 0x31;
    //     cmp #6
    //     bcs return_75
    if (a >= 6)
        return MARKER_INVALID;
    // return_75:
    //     rts
    return a;
}

void move_cursor_to_address(uint16_t addr)
{
    addr_t tmp01;
    uint8_t a;
    uint8_t x;
    uint8_t yy;

    // move_cursor_to_address
    // move_cursor_to_address:
    //     sta ((uint8_t*)&tmp89)[0]
    addr_t tmp89 = addr;
    uint16_t cur = current_line_ptr;
    //     cpy ((uint8_t*)&tmp89)[1]
    //     bcc cabf9
    //     bne cabdf
    //     cmp ((uint8_t*)&tmp89)[0]
    //     bcc cabf9
    //     beq cac20
    if (cur == addr)
        goto cac20;
    if (cur > addr)
    {
        // cabdf:
        for (;;)
        {
            //     jsr sub_cab37
            if (!find_previous_line(cur, &tmp01))
                goto cac20;
            cur = tmp01;
            //     bcc cac20
            //     cpy ((uint8_t*)&tmp89)[1]
            //     bcc cac20
            //     bne cabdf
            //     cmp ((uint8_t*)&tmp89)[0]
            //     bcc cac20
            //     bne cabdf
            //     beq cac20
            if (cur <= addr)
                goto cac20;
            //     ALWAYS branch
        }
    }

    // cabf6:
    // cabf9:
    do
    {
        //     sta ((uint8_t*)&tmp01)[0]
        if (find_next_line(cur, &tmp01))
            goto cac17;
        //     beq cac17
        //     tya
        //     ldy ((uint8_t*)&tmp01)[1]
        //     clc
        //     adc ((uint8_t*)&tmp01)[0]
        //     bcc cac0b
        cur = tmp01 + y;
        //     cpy ((uint8_t*)&tmp89)[1]
        //     bcc cabf6
        //     bne cac17
        //     cmp ((uint8_t*)&tmp89)[0]
        //     bcc cabf6
        //     beq cac1d
        if (cur >= addr)
        {
            if (cur == addr)
                goto cac1d;
            goto cac17;
        }
        //     cabf6:
        //     jsr sub_cac41
        check_for_embedded_ruler(tmp01);
    } while (1);
    // cac17:
cac17:
    //     lda ((uint8_t*)&tmp01)[0]
    //     ldy ((uint8_t*)&tmp01)[1]
    //     bne cac20
    cur = tmp01;
    goto cac20;
    // cac1d:
cac1d:
    //     jsr sub_cac41
    check_for_embedded_ruler(tmp01);
    // cac20:
cac20:
    //     sta current_line_ptr
    //     sty current_line_ptr+1
    current_line_ptr = cur;
    //     lda ((uint8_t*)&tmp89)[0]
    //     sec
    //     sbc current_line_ptr
    //     tax
    // (sbc with C=1 is a plain subtraction; tax overwrites the flags)
    x = (uint8_t)(tmp89 - current_line_ptr);
    //     ldy #0
    yy = 0;
    //     lda (current_line_ptr),y
    a = ram[current_line_ptr + yy];
    //     jsr check_for_command_prefix
    command_prefix_t cp = check_for_command_prefix(a);
    //     bne cac3e
    if (cp != NO_COMMAND_PREFIX)
    {
        a = x;
        x = 0;
        if (a >= 3)
        {
            a -= 3;
            x = a;
        }
    }
    //     stx xpos
    xpos = x;
    //     rts
    return;
}

void move_cursor_to_top_of_document(void)
{
    // move_cursor_to_top_of_document
    // move_cursor_to_top_of_document: Moves cursor to the top (page) of the
    // document

    //     lda page
    //     sta current_line_ptr
    current_line_ptr = page;
    //     lda page+1
    //     sta current_line_ptr+1
    //     lda #0
    //     sta xpos
    xpos = 0;
    //     ldy #0xfe
    //     sty l0012
    top_of_screen_line_ptr = (addr_t)0xfe << 8;
    //     sty ruler_stack_ptr
    ruler_index_ptr = 0xfe;
    //     sty l0033
    l0033 = 0xfe;
    //     jmp cab91
    load_current_ruler(0xfe);
}

// Skips to the next CR or zero terminator.  Sets the global y to the offset of
// the byte past the CR (or of the NUL), and returns true if that byte is a NUL
// (end of document) — the 6502's Z flag.
bool find_next_line(addr_t start, addr_t* tmp01)
{
    *tmp01 = start;
    // find_next_line
    // Pseudocode: Skips to next CR or zero terminator in memory

    // cab29:
    //     ldy #0
    uint8_t a;
    y = 0;
    // loop_cab2b:
    //     lda (((uint8_t*)&tmp01)[0]),y
    //     beq return_70
    //     iny
    //     cmp #0x0d
    //     bne loop_cab2b
    //     lda (((uint8_t*)&tmp01)[0]),y
    for (;;)
    {
        a = ram[(*tmp01) + y];
        if (a == 0)
            return true;
        y++;
        if (a == 0x0d)
            break;
    }
    a = ram[(*tmp01) + y];
    // return_70:
    //     rts
    return a == 0;
}

// Returns false if tmp01 is already at the start of the document (no previous
// line); true if it was moved back to the start of the previous line.
bool find_previous_line(addr_t val, addr_t* tmp01)
{
    uint8_t a;
    // find_previous_line
    // sub_cab37:
    //     sec
    //     sbc #1
    //     sta ((uint8_t*)&tmp01)[0]
    //     bcs cab3f
    //     sty ((uint8_t*)&tmp01)[1]
    *tmp01 = val - 1;
    //     cpy page+1
    //     bcc return_71
    //     bne cab4b
    //     cmp page
    //     bcc return_71
    if (*tmp01 < page)
        return false;
    // loop_cab4d:
    do
    {
        (*tmp01)--;
        a = ram[(*tmp01)];
    } while (a != 0x0d);
    (*tmp01)++;
    //     jsr sub_cab6e
    //     bne cab6c
    // (inlined: Z = (ram[tmp01] == RULER_BYTE))
    if (ram[(*tmp01)] == RULER_BYTE)
    {
        pop_from_ruler_index();
    }
    //     sec
    // return_71:
    //     rts
    return true;
}

void open_input_file(void)
{
    zero_terminate_filename_buffer();
    input_fp = fopen((char*)filename_buffer, "rb");
    if (!input_fp)
    {
        file_not_found_error();
        return;
    }
    file_ptr = input_fp;
}

void open_output_file(void)
{
    zero_terminate_filename_buffer();
    output_fp = fopen((char*)filename_buffer, "wb");
    if (!output_fp)
    {
        file_error();
        return;
    }
    file_ptr = output_fp;
}

void pop_from_ruler_index(void)
{
    uint8_t y;

    // Pseudocode: Pops ruler position from the ruler index

    // pop_from_ruler_stack:
    //     inc status_line_needs_redrawing_flag
    status_line_needs_redrawing_flag++;
    //     ldy ruler_stack_ptr
    y = ruler_index_ptr;
    //     iny
    y++;
    //     iny
    y++;

    // MULTIPLE ENTRY POINTS: pop_from_ruler_index, cab91
    load_current_ruler(y);
}

void push_onto_ruler_index(addr_t tmp01)
{
    // push_onto_ruler_index
    // Pseudocode: Pushes current ruler position onto the ruler index

    // push_onto_ruler_stack:
    //     tya
    //     pha
    {
        uint8_t stack_index;
        //     inc status_line_needs_redrawing_flag
        status_line_needs_redrawing_flag++;
        //     ldy ruler_stack_ptr
        stack_index = ruler_index_ptr - 2;
        //     sta (oshwm),y / sta (oshwm),y+1
        ruler_index[stack_index >> 1] = tmp01;
        //     jsr cab91
        load_current_ruler(stack_index);
    }
    //     rts
    return;
}

void reset_area_to_entire_document(void)
{
    // reset_area_to_entire_document: Resets area to entire document (top to
    // page)

    //     lda top
    //     sta area_start_ptr
    area_start_ptr = top;
    //     lda top+1
    //     sta area_start_ptr+1
    //     lda page
    //     sta area_end_ptr
    area_end_ptr = page;
    //     lda page+1
    //     sta area_end_ptr+1
    //     rts
}

// Finds the next line, handling a command/ruler prefix and pushing onto the
// ruler index.  Returns true if the next line is the end of the document (the
// 6502's Z flag, as left by find_next_line).
bool advance_to_next_line(addr_t line, addr_t* tmp01)
{
    // Pseudocode: Finds next line in document, handling command prefix and
    // ruler stack

    // sub_cab1a:
    //     sta ((uint8_t*)&tmp01)[0]
    //     jsr sub_cab6e
    //     bne cab29
    // (inlined: Z = (ram[tmp01] == RULER_BYTE))
    if (ram[line] != RULER_BYTE)
    {
        return find_next_line(line, tmp01);
    }
    //     jsr cab29
    bool end = find_next_line(line, tmp01);
    //     bne push_onto_ruler_stack
    if (!end)
    {
        push_onto_ruler_index(line);
    }
    //     rts
    return end;
}
