#include "document.h"
#include "io.h"
#include "printing.h"
#include <stdio.h>
#include <ctype.h>

// call_printer_driver moved to printing.c

uint8_t check_for_command_prefix(uint8_t ch)
{
    // check_for_command_prefix:
    //     cmp #0x80
    //     beq return_81
    //     cmp #0x81
    //     clc
    // return_81:
    //     rts
    if (ch == 0x80)
        return FLAG_Z | FLAG_C;
    if (ch == 0x81)
        return FLAG_Z;
    return 0;
}

void check_for_control_code(uint8_t a)
{
    // Pseudocode: Checks if character is a control code (0x1c or 0x1d)

    // check_for_control_code:
    //     cmp #0x1c
    cmp(&flags, a, 0x1c);
    //     beq return_63
    if (!(flags & FLAG_Z))
    {
        cmp(&flags, a, 0x1d);
        flags &= ~FLAG_C;
    }
    //     rts
    return;
}

void compute_bytes_free(void)
{
    // compute_bytes_free
    // Pseudocode: Computes number of free bytes between top and himem

    // ;
    // ***************************************************************************************
    // compute_bytes_free:
    //     lda himem
    uint8_t a;
    a = (uint8_t)(himem & 0xff);
    //     sec
    flags |= FLAG_C;
    //     sbc top
    flags |= FLAG_C;
    a = sbc(&flags, a, (uint8_t)(top & 0xff));
    //     tax
    x = a;
    //     lda himem+1
    a = (uint8_t)(himem >> 8);
    //     sbc top+1
    a = sbc(&flags, a, (uint8_t)(top >> 8));
    //     tay
    y = a;
    // return_84:
    //     rts
    return;
}

void check_for_at_least_150_bytes_free(void)
{
    // Pseudocode: Checks if at least 150 bytes of memory are available

    // check_for_at_least_150_bytes_free:
    //     jsr compute_bytes_free
    compute_bytes_free();
    //     tya
    //     bne return_6
    if (y != 0)
        return;
    //     cpx #0x96
    //     bcs return_6
    if (x >= 0x96)
        return;

    // MULTIPLE ENTRY POINTS: check_for_at_least_150_bytes_free,
    // display_not_enough_memory
    display_not_enough_memory();
}

uint8_t deref_and_check_for_command_prefix(uint8_t y)
{
    // deref_and_check_for_command_prefix:
    //     lda (((uint8_t*)&tmp01)[0]),y
    uint8_t a;
    a = ram[tmp01 + y];
    return check_for_command_prefix(a);
}
// Returns flags value: if ch is 0x80 (format command) → FLAG_Z|FLAG_C;
// if ch is 0x81 (ruler line) → FLAG_Z; otherwise → 0.

void display_document_file_state(void)
{
    // display_document_file_state
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
    if (a == 0)
        goto c8a21;
    //     ldy #0
    y = 0;
loop_c89fa:
    // loop_c89fa:
    //     lda input_filename,y
    a = input_filename[y];
    //     cmp #0x0d
    if (a == 0x0d)
        goto c8a07;
    //     jsr bdos_print_char
    cli_putchar(a);
    //     iny
    y++;
    //     bne loop_c89fa
    if (y != 0)
        goto loop_c89fa;
c8a07:
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
    cmp(&flags, a, 0x0d);
    //     bne loop_c8a15
    if (!(flags & FLAG_Z))
        goto loop_c8a15;
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
loop_caba5:
    //     lda (current_ruler_ptr),y
    a = ram[current_ruler_ptr + y];
    //     cmp #0x3e ; '>'
    if (!(a != 0x3e))
    {
        ruler_left_stop = y;
    }
    //     cmp #0x3c ; '<'
    if (!(a != 0x3c))
    {
        ruler_right_stop = y;
    }
    //     cmp #0x0d
    if (a == 0x0d)
        goto cabbc;
    //     iny
    y++;
    //     cpy #0x84
    if (y != MAX_LINE_LENGTH)
        goto loop_caba5;
    // cabbc:
cabbc:
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

void print_char(void)
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
    a = 0;
    //     sta print_xpos
    print_xpos = 0;
    //     lda #0x0d
    a = 0x0d;
c9468:
    //     jsr sub_c9445
    sub_c9445();
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
        printer_driver_ptr->print_char();
        return;
    }
    //     jsr check_for_control_code
    check_for_control_code(a);
    //     bne c9488
    if (!(flags & FLAG_Z))
        goto c9488;
    //     pha
    {
        uint8_t saved_a = a;
        //     lda #0x2d ; '-'
        a = 0x2d; // '-'
        //     bcs c947e
        if (!(flags & FLAG_C))
            a = 0x2a; // '*'
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

void process_document_character(void)
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
    cmp(&flags, a, 0x1a);
    //     beq ca5d5
    if (flags & FLAG_Z)
        goto ca5d5;
    //     bcc ca5d1
    if (!(flags & FLAG_C))
        goto ca5d1;
    //     cmp #0x20 ; ' '
    cmp(&flags, a, 0x20);
    //     bcs ca5d1
    if (flags & FLAG_C)
        goto ca5d1;
    //     sty l0084
    l0084 = y;
    //     ldy print_flags
    y = print_flags;
    //     bpl ca5cf
    if ((y & 0x80))
    {
        a = sbc(&flags, a, 0x1b);
        x = a;
        a = highlight_code[x];
    }
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
    set_flags(&flags, a);
    //     beq ca5d5
    if (flags & FLAG_Z)
        goto ca5d5;
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
    cmp(&flags, y, l003a);
    //     bcs ca5f8
    if (flags & FLAG_C)
        goto ca5f8;
    //     lda (current_ruler_ptr),y
    a = ram[current_ruler_ptr + y];
    //     cmp #0x2a ; '*'
    cmp(&flags, a, 0x2a);
    //     bne loop_ca5e5
    if (!(flags & FLAG_Z))
        goto loop_ca5e5;
    //     tya
    a = y;
ca5f1:
    //     sbc l0039
    a = sbc(&flags, a, l0039);
    //     tax
    x = a;
    //     beq ca5f8
    if (x == 0)
        goto ca5f8;
    //     bcs ca5fa
    if (flags & FLAG_C)
        goto ca5fa;
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
    return;
}

void read_char(void)
{
// read_char
// Pseudocode: Reads a character from keyboard via SCREEN, returning escape
// flag in carry flush_and_read_char / read_char (same entry point)
read_char:
    //     lda #0xff
    a = 0xff;
    //     tax
    x = a;
    //     ldy #SCREEN_GETCHAR
    //     jsr SCREEN
    flags &= ~FLAG_C;
    a = screen_getchar();
    //     bcs read_char
    if (flags & FLAG_C)
        goto read_char;

    //     cmp #0x1b                                                         ;
    //     A=character read
    cmp(&flags, a, 0x1b);
    //     clc
    flags &= ~FLAG_C;
    //     bne return_65
    if ((flags & FLAG_Z))
    {
        flags |= FLAG_C;
    }
    //     rts
    return;
}

void return_to_cli_prompt(void)
{
    longjmp(env, JMP_CLI);
}

void sub_c9445(void)
{
    // sub_c9445
    // Pseudocode: Outputs print_xpos number of spaces to align printer

    // sub_c9445:
    //     pha
    {
        uint8_t saved_a = a;
        //     lda print_xpos
        a = print_xpos;
        //     beq c9453
        if (a == 0)
            goto c9453;
        //     lda #0x20 ; ' '
        a = 0x20;
        // loop_c944c:
    loop_c944c:
        //     jsr print_char_just_to_printer
        print_char_just_to_screen(a);
        //     dec print_xpos
        print_xpos--;
        //     bne loop_c944c
        if (print_xpos != 0)
            goto loop_c944c;
        // c9453:
    c9453:
        //     pla
        a = saved_a;
    }
    //     rts
}

void sub_cab6e(void)
{
    // Pseudocode: Checks if byte at ((uint8_t*)&tmp01)[0] is a command prefix
    // (0x81)

    // sub_cab6e:
    //     ldy #0
    uint8_t a;
    uint8_t y = 0;
    //     lda (((uint8_t*)&tmp01)[0]),y
    a = ram[tmp01 + y];
    //     cmp #0x81
    cmp(&flags, a, 0x81);
    //     rts
    return;
}

void sub_cadf0(void)
{
    // sub_cadf0
    // sub_cadf0: Performs 8-bit by 8-bit division for microspacing

    //     ldx #8
    uint8_t x;
    x = 8;
    //     lda ((uint8_t*)&tmp89)[1]
    a = ((uint8_t*)&tmp89)[1];
    // loop_cadf4:
loop_cadf4:
    //     asl ((uint8_t*)&tmp89)[0]
    ((uint8_t*)&tmp89)[0] = rol(&flags, ((uint8_t*)&tmp89)[0]);
    //     rol
    a = rol(&flags, a);
    //     cmp l0046
    cmp(&flags, a, l0046);
    //     bcc cadff
    if ((flags & FLAG_C))
    {
        a = sbc(&flags, a, l0046);
        ((uint8_t*)&tmp89)[0]++;
    }
    //     dex
    x--;
    //     bne loop_cadf4
    if (x != 0)
        goto loop_cadf4;
    //     rts
}

void cab91(void)
{
    // cab91

    // Pseudocode: Sets current_ruler_ptr from stack at ruler_index_ptr offset

    // cab91:
    //     sty ruler_stack_ptr
    ruler_index_ptr = y;
    //     iny
    y++;
    //     lda (oshwm),y
    //     clc
    //     adc #3
    //     sta current_ruler_ptr
    flags &= ~FLAG_C;
    //     lda (oshwm),y
    a = ram[oshwm + y];
    //     adc #3
    a = adc(&flags, a, 3);
    //     sta current_ruler_ptr
    current_ruler_ptr = (current_ruler_ptr & 0xff00) | a;
    //     dey
    y--;
    //     lda (oshwm),y
    a = ram[oshwm + y];
    //     adc #0
    a = adc(&flags, a, 0);
    //     sta current_ruler_ptr+1
    current_ruler_ptr = (current_ruler_ptr & 0x00ff) | ((uint16_t)a << 8);
    // MULTIPLE ENTRY POINTS: pop_from_ruler_index, cab91
    //     (falls through to find_margins_of_current_ruler_buffer)
    find_margins_of_current_ruler_buffer();
}

void cb05a(void)
{
    // cb05a
    // cb05a: Ensures at least one CR at top of document

    //     ldy page+1
    uint8_t y;
    uint8_t a;
    y = (uint8_t)(page >> 8);
    //     cpy top+1
    cmp(&flags, y, (uint8_t)(top >> 8));
    //     bne return_85
    if (!(flags & FLAG_Z))
        return;
    //     lda page
    a = (uint8_t)(page & 0xff);
    //     cmp top
    cmp(&flags, a, (uint8_t)(top & 0xff));
    //     bne return_85
    if (!(flags & FLAG_Z))
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

void close_file(void)
{
    if (file_ptr)
    {
        fclose(file_ptr);
        file_ptr = NULL;
    }
}

void create_default_ruler(uint16_t ruler_addr)
{
    a = (uint8_t)(ruler_addr & 0xff);
    y = (uint8_t)(ruler_addr >> 8);
    // create_default_ruler
    // Pseudocode: Creates a default ruler with tab stops every 6 columns

    // ;
    // ***************************************************************************************
    // create_default_ruler:
    //     sta ((uint8_t*)&tmp01)[0]
    uint8_t x;
    tmp01 = (addr_t)(y) << 8 | a;

    //     lda #0
    a = 0;
    //     tay                                                               ;
    //     Y=0x00
    y = a;
    // loop_cb0e7:
loop_cb0e7:
    //     lda #0x2e ; '.'
    a = 0x2e;
    // loop_cb0e9:
loop_cb0e9:
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
        goto loop_cb0e7;
    //     lda #0x2a ; '*'
    a = 0x2a;
    //     bne loop_cb0e9                                                    ;
    //     ALWAYS branch
    goto loop_cb0e9;

    // cb0ff:
cb0ff:
    //     lda #0x3c ; '<'
    a = 0x3c;
    //     sta (((uint8_t*)&tmp01)[0]),y
    ram[tmp01 + y] = a;
    //     rts
    return;
}

void get_byte_from_file(void)
{
    // get_byte_from_file
    int c = fgetc(file_ptr);
    if (c == EOF || c == 0)
    {
        a = 0;
        flags |= FLAG_C;
        flags = (flags & ~(FLAG_Z | FLAG_N)) | FLAG_Z;
    }
    else
    {
        a = (uint8_t)c;
        flags &= ~FLAG_C;
        flags =
            (flags & ~(FLAG_Z | FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    }
    y = a;
}

void get_register_address(uint8_t a)
{
    // get_register_address
    // get_register_address: Gets address of a register value by letter name

    //     jsr is_uppercase
    if (isupper(a))
    {
        flags &= ~FLAG_C;
    }
    else
    {
        flags |= FLAG_C;
    }
    //     bcs return_77
    if (flags & FLAG_C)
    { /* return_77: */
        return;
    }
    //     and #0xdf
    a &= 0xdf;
    //     pha
    {
        uint8_t saved_a = a;
        //     sbc #0x40 ; '@'
        a = sbc(&flags, a, 0x40);
        //     asl
        a <<= 1;
        //     adc #<register_value_array
        flags &= ~FLAG_C;
        a = adc(&flags, a, (uint8_t)(RAM_REGISTER_VALUE_ARRAY & 0xff));
        //     sta ((uint8_t*)&tmp67)[0]
        ((uint8_t*)&tmp67)[0] = a;
        //     lda #>register_value_array
        a = (uint8_t)(RAM_REGISTER_VALUE_ARRAY >> 8);
        //     adc #0
        a = adc(&flags, a, 0);
        //     sta ((uint8_t*)&tmp67)[1]
        ((uint8_t*)&tmp67)[1] = a;
        //     pla
        a = saved_a;
    }
    //     clc
    flags &= ~FLAG_C;
    // return_77:
    //     rts
}

void initialise_document(void)
{
    // initialise_document
    // initialise_document:
    //     lda #0
    a = 0;
    //     sta printer_driver_name
    printer_driver_name[0] = a;
    //     sta format_mode_flag
    format_mode_flag = a;
    //     sta justifying_flag
    justifying_flag = a;
    //     sta insert_mode_flag
    insert_mode_flag = a;
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
    l007e = 0;
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
    a = 0xaa;
    //     sta (oshwm),y
    ram[oshwm + y] = a;
    //     lda page
    //     sec
    //     sbc #1
    //     sta ((uint8_t*)&tmp89)[0]
    //     lda page+1
    //     sbc #0
    //     sta ((uint8_t*)&tmp89)[1]
    tmp89 = page - 1;
    //     lda #0x0d
    a = 0x0d;
    //     sta (((uint8_t*)&tmp89)[0]),y
    ram[tmp89] = a;
    //     sta current_line_buffer + 0x89
    current_line_buffer[MAX_LINE_LENGTH - 1] = a;
    //     lda page / sta top / lda page+1 / sta top+1
    top = page;
    //     lda #<(current_line_buffer)
    a = (uint8_t)(RAM_CURRENT_LINE_BUF & 0xff);
    //     sta ptr1
    ptr1 = (ptr1 & 0xff00) | a;
    //     clc
    flags &= ~FLAG_C;
    //     adc #3
    a = adc(&flags, a, 3);
    //     sta current_edit_line_ptr
    //     sta current_format_line_ptr
    current_edit_line_ptr = (current_edit_line_ptr & 0xff00) | a;
    current_format_line_ptr = current_edit_line_ptr;
    //     lda #>(current_line_buffer)
    a = (uint8_t)(RAM_CURRENT_LINE_BUF >> 8);
    //     sta ptr1+1
    ptr1 = (ptr1 & 0x00ff) | ((uint16_t)a << 8);
    //     adc #0
    a = adc(&flags, a, 0);
    //     sta current_edit_line_ptr+1
    //     sta current_format_line_ptr+1
    current_edit_line_ptr =
        (current_edit_line_ptr & 0x00ff) | ((uint16_t)a << 8);
    current_format_line_ptr = current_edit_line_ptr;
    //     lda #<(current_ruler_buffer)
    create_default_ruler(RAM_CURRENT_RULER_BUF);
    //     iny
    y++;
    //     lda #0x0d
    a = 0x0d;
    //     sta (((uint8_t*)&tmp01)[0]),y
    ram[tmp01 + y] = a;
    //     ldy #0xff
    y = 0xff;
    //     lda #<(just_before_current_ruler_buffer)
    a = (uint8_t)(RAM_JUST_BEFORE_RULER_BUF & 0xff);
    //     sta (oshwm),y
    ram[oshwm + y] = a;
    //     dey                                                               ;
    //     Y=0xfe
    y--;
    //     lda #>(just_before_current_ruler_buffer)
    a = (uint8_t)(RAM_JUST_BEFORE_RULER_BUF >> 8);
    //     sta (oshwm),y
    ram[oshwm + y] = a;
    //     jsr move_cursor_to_top_of_document
    move_cursor_to_top_of_document();
    //     jsr clear_cmd
    clear_cmd();
    //     (falls through to cb05a)
    cb05a();
}

void lookup_marker(void)
{
    // lookup_marker
    // lookup_marker: Converts marker character '1'-'6' to index

    //     sec
    uint8_t x;
    flags |= FLAG_C;
    //     sbc #0x31 ; '1'
    a = a - 0x31;
    //     bcc loop_caced
    if (!(flags & FLAG_C))
    {
        beep();
        return;
    }
    //     asl
    a <<= 1;
    //     cmp #0x0c
    cmp(&flags, a, 0x0c);
    //     bcs return_75
    if (flags & FLAG_C)
    { /* return_75: */
        return;
    }
    //     tax
    x = a;
    //     lda markers_array+1,x
    a = ((uint8_t*)markers_array)[x + 1];
    // return_75:
    //     rts
}

void move_cursor_to_address(void)
{
    // move_cursor_to_address
    // move_cursor_to_address:
    //     sta ((uint8_t*)&tmp89)[0]
    tmp89 = (addr_t)(y) << 8 | a;
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     ldy current_line_ptr+1
    y = (uint8_t)(current_line_ptr >> 8);
    //     cpy ((uint8_t*)&tmp89)[1]
    cmp(&flags, y, ((uint8_t*)&tmp89)[1]);
    //     bcc cabf9
    if (!(flags & FLAG_C))
        goto cabf9;
    //     bne cabdf
    if (!(flags & FLAG_Z))
        goto cabdf;
    //     cmp ((uint8_t*)&tmp89)[0]
    cmp(&flags, a, ((uint8_t*)&tmp89)[0]);
    //     bcc cabf9
    if (!(flags & FLAG_C))
        goto cabf9;
    //     beq cac20
    if (flags & FLAG_Z)
        goto cac20;
    // cabdf:
cabdf:
    //     jsr sub_cab37
    move_tmp01_to_previous_line(current_line_ptr);
    //     lda ((uint8_t*)&tmp01)[0]
    a = ((uint8_t*)&tmp01)[0];
    //     ldy ((uint8_t*)&tmp01)[1]
    y = ((uint8_t*)&tmp01)[1];
    //     bcc cac20
    if (!(flags & FLAG_C))
        goto cac20;
    //     cpy ((uint8_t*)&tmp89)[1]
    cmp(&flags, y, ((uint8_t*)&tmp89)[1]);
    //     bcc cac20
    if (!(flags & FLAG_C))
        goto cac20;
    //     bne cabdf
    if (!(flags & FLAG_Z))
        goto cabdf;
    //     cmp ((uint8_t*)&tmp89)[0]
    cmp(&flags, a, ((uint8_t*)&tmp89)[0]);
    //     bcc cac20
    if (!(flags & FLAG_C))
        goto cac20;
    //     bne cabdf
    if (!(flags & FLAG_Z))
        goto cabdf;
    //     beq cac20                                                         ;
    //     ALWAYS branch
    goto cac20;

    // cabf6:
cabf6:
    //     jsr sub_cac41
    sub_cac41();
    // cabf9:
cabf9:
    //     sta ((uint8_t*)&tmp01)[0]
    move_tmp01_to_next_line((addr_t)(y) << 8 | a);
    //     beq cac17
    if (flags & FLAG_Z)
        goto cac17;
    //     tya
    a = y;
    //     ldy ((uint8_t*)&tmp01)[1]
    y = ((uint8_t*)&tmp01)[1];
    //     clc
    flags &= ~FLAG_C;
    //     adc ((uint8_t*)&tmp01)[0]
    a = adc(&flags, a, ((uint8_t*)&tmp01)[0]);
    //     bcc cac0b
    if ((flags & FLAG_C))
    {
        y++;
    }
    //     cpy ((uint8_t*)&tmp89)[1]
    cmp(&flags, y, ((uint8_t*)&tmp89)[1]);
    //     bcc cabf6
    if (!(flags & FLAG_C))
        goto cabf6;
    //     bne cac17
    if (!(flags & FLAG_Z))
        goto cac17;
    //     cmp ((uint8_t*)&tmp89)[0]
    cmp(&flags, a, ((uint8_t*)&tmp89)[0]);
    //     bcc cabf6
    if (!(flags & FLAG_C))
        goto cabf6;
    //     beq cac1d
    if (flags & FLAG_Z)
        goto cac1d;
    // cac17:
cac17:
    //     lda ((uint8_t*)&tmp01)[0]
    a = ((uint8_t*)&tmp01)[0];
    //     ldy ((uint8_t*)&tmp01)[1]
    y = ((uint8_t*)&tmp01)[1];
    //     bne cac20
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
    //     lda ((uint8_t*)&tmp89)[0]
    a = ((uint8_t*)&tmp89)[0];
    //     sec
    flags |= FLAG_C;
    //     sbc current_line_ptr
    a = sbc(&flags, a, (uint8_t)(current_line_ptr & 0xff));
    //     tax
    x = a;
    //     ldy #0
    y = 0;
    //     lda (current_line_ptr),y
    a = ram[current_line_ptr + y];
    //     jsr check_for_command_prefix
    flags = check_for_command_prefix(a);
    //     bne cac3e
    if (!(flags & FLAG_Z))
        goto cac3e;
    //     txa
    a = x;
    //     cpx #3
    cmp(&flags, x, 3);
    //     ldx #0
    x = 0;
    //     bcc cac3e
    if ((flags & FLAG_C))
    {
        a -= 3;
        x = a;
    }
cac3e:
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
    top_of_screen_line_ptr = (addr_t)0xfe << 8;
    //     sty ruler_stack_ptr
    ruler_index_ptr = y;
    //     sty l0033
    l0033 = y;
    //     jmp cab91
    cab91();
}

void move_tmp01_to_next_line(uint16_t start)
{
    tmp01 = start;
    // move_tmp01_to_next_line
    // Pseudocode: Skips to next CR or zero terminator in memory

    // cab29:
    //     ldy #0
    uint8_t a;
    y = 0;
    // loop_cab2b:
loop_cab2b:
    //     lda (((uint8_t*)&tmp01)[0]),y
    a = ram[tmp01 + y];
    set_flags(&flags, a);
    //     beq return_70
    if (flags & FLAG_Z)
        goto return_70;
    //     iny
    y++;
    //     cmp #0x0d
    if (a != 0x0d)
        goto loop_cab2b;
    //     lda (((uint8_t*)&tmp01)[0]),y
    a = ram[tmp01 + y];
    set_flags(&flags, a);
    // return_70:
return_70:
    //     rts
    return;
}

void move_tmp01_to_previous_line(uint16_t val)
{
    a = (uint8_t)(val & 0xff);
    y = (uint8_t)(val >> 8);
    // move_tmp01_to_previous_line
    // sub_cab37:
    //     sec
    flags |= FLAG_C;
    //     sbc #1
    a = sbc(&flags, a, 1);
    //     sta ((uint8_t*)&tmp01)[0]
    ((uint8_t*)&tmp01)[0] = a;
    //     bcs cab3f
    if (!(flags & FLAG_C))
    {
        y--;
    }
    //     sty ((uint8_t*)&tmp01)[1]
    ((uint8_t*)&tmp01)[1] = y;
    //     cpy page+1
    cmp(&flags, y, (uint8_t)(page >> 8));
    //     bcc return_71
    if (!(flags & FLAG_C))
        return;
    //     bne cab4b
    if ((flags & FLAG_Z))
    {
        cmp(&flags, a, (uint8_t)(page & 0xff));
        if (!(flags & FLAG_C))
            return;
    }
    // loop_cab4d:
loop_cab4d:
    tmp01--;
    a = ram[tmp01];
    if (a != 0x0d)
        goto loop_cab4d;
    tmp01++;
    //     jsr sub_cab6e
    sub_cab6e();
    //     bne cab6c
    if ((flags & FLAG_Z))
    {
        pop_from_ruler_index();
    }
    //     sec
    flags |= FLAG_C;
    // return_71:
    //     rts
    return;
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
    cab91();
}

void push_onto_ruler_index(void)
{
    // push_onto_ruler_index
    // Pseudocode: Pushes current ruler position onto the ruler index

    // push_onto_ruler_stack:
    //     tya
    //     pha
    {
        uint8_t saved_y = y;
        //     inc status_line_needs_redrawing_flag
        status_line_needs_redrawing_flag++;
        //     ldy ruler_stack_ptr
        y = ruler_index_ptr;
        //     dey
        y--;
        //     lda ((uint8_t*)&tmp01)[0]
        a = ((uint8_t*)&tmp01)[0];
        //     sta (oshwm),y
        ram[oshwm + y] = a;
        //     dey
        y--;
        //     lda ((uint8_t*)&tmp01)[1]
        a = ((uint8_t*)&tmp01)[1];
        //     sta (oshwm),y
        ram[oshwm + y] = a;
        //     jsr cab91
        cab91();
        //     pla
        //     tay
        y = saved_y;
        set_flags(&flags, y);
    }
    //     rts
    return;
}

void put_byte_to_file(uint8_t a)
{
    fputc(a, file_ptr);
}

void reset_area_to_entire_document(void)
{
    // reset_area_to_entire_document: Resets area to entire document (top to
    // page)

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

void select_file(uint8_t x)
{
    file_ptr = x ? output_fp : input_fp;
}

void sub_cab1a(void)
{
    // Pseudocode: Finds next line in document, handling command prefix and
    // ruler stack

    // sub_cab1a:
    //     sta ((uint8_t*)&tmp01)[0]
    tmp01 = (addr_t)(y) << 8 | a;
    //     jsr sub_cab6e
    sub_cab6e();
    //     bne cab29
    if (!(flags & FLAG_Z))
    {
        move_tmp01_to_next_line(tmp01);
        return;
    }
    //     jsr cab29
    move_tmp01_to_next_line(tmp01);
    //     bne push_onto_ruler_stack
    if (!(flags & FLAG_Z))
    {
        push_onto_ruler_index();
        return;
    }
    //     rts
    return;
}
