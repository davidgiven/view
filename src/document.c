#include "document.h"
#include "io.h"
#include "io.h"

void call_printer_driver(void) {
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

uint8_t check_for_command_prefix(uint8_t ch) {
    // check_for_command_prefix:
    //     cmp #0x80
    //     beq return_81
    //     cmp #0x81
    //     clc
    // return_81:
    //     rts
    if (ch == 0x80) return FLAG_Z | FLAG_C;
    if (ch == 0x81) return FLAG_Z;
    return 0;
}

void check_for_control_code(void) {
    // Pseudocode: Checks if character is a control code (0x1c or 0x1d)

    // check_for_control_code:
    //     cmp #0x1c
    cmp(a, 0x1c);
    //     beq return_63
    if (flags & FLAG_Z) goto return_63;
    //     cmp #0x1d
    cmp(a, 0x1d);
    //     clc
    flags &= ~FLAG_C;
    // return_63:
return_63:
    //     rts
    return;
}

void compute_bytes_free(void) {
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

void check_for_at_least_150_bytes_free(void) {
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

uint8_t deref_and_check_for_command_prefix(void) {
    // deref_and_check_for_command_prefix:
    //     lda (tmp0),y
    a = ram[((uint16_t)tmp1 << 8 | tmp0) + y];
    return check_for_command_prefix(a);
}
// Returns flags value: if ch is 0x80 (format command) → FLAG_Z|FLAG_C;
// if ch is 0x81 (ruler line) → FLAG_Z; otherwise → 0.

void display_document_file_state(void) {
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
    cmp(a, 0x0d);
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
    bit(file_edit_flags);
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
    cmp(a, 0x0d);
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

void find_margins_of_current_ruler_buffer(void) {
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
    cmp(a, 0x3e);
    //     bne cabad
    if (!(flags & FLAG_Z)) goto cabad;
    //     sty ruler_left_stop
    ruler_left_stop = y;
    // cabad:
cabad:
    //     cmp #0x3c ; '<'
    cmp(a, 0x3c);
    //     bne cabb3
    if (!(flags & FLAG_Z)) goto cabb3;
    //     sty ruler_right_stop
    ruler_right_stop = y;
    // cabb3:
cabb3:
    //     cmp #0x0d
    cmp(a, 0x0d);
    //     beq cabbc
    if (flags & FLAG_Z) goto cabbc;
    //     iny
    y++;
    //     cpy #0x84
    cmp(y, MAX_LINE_LENGTH);
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

void print_char(void) {
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
    print_char_just_to_screen();
}

void print_char_just_to_screen(void) {
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
    screen_setstyle(STYLE_REVERSE);
    //     jsr bdos_print_char
    cli_putchar(a);
    //     pla
    a = saved_a; }
    //     jmp set_normal_text_if_not_mode_7
    screen_setstyle(0);
    return;

c9488:
    //     jmp bdos_print_char
    if (a == 0x0d) { cli_putchar('\n'); return; }
    cli_putchar(a);
}

void process_document_character(void) {
    // sub_ca5ae:
    //     cmp #9
    cmp(a, 9);
    //     beq ca5e1
    if (flags & FLAG_Z) goto ca5e1;
    //     cmp #0x10
    cmp(a, 0x10);
    //     beq ca5d5
    if (flags & FLAG_Z) goto ca5d5;
    //     cmp #0x0b
    cmp(a, 0x0b);
    //     beq ca5d9
    if (flags & FLAG_Z) goto ca5d9;
    //     cmp #0x1a
    cmp(a, 0x1a);
    //     beq ca5d5
    if (flags & FLAG_Z) goto ca5d5;
    //     bcc ca5d1
    if (!(flags & FLAG_C)) goto ca5d1;
    //     cmp #0x20 ; ' '
    cmp(a, 0x20);
    //     bcs ca5d1
    if (flags & FLAG_C) goto ca5d1;
    //     sty l0084
    l0084 = y;
    //     ldy print_flags
    y = print_flags;
    //     bpl ca5cf
    if (!(y & 0x80)) goto ca5cf;
    //     sbc #0x1b
    sbc(0x1b);
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
    set_flags(a);
    //     beq ca5d5
    if (flags & FLAG_Z) goto ca5d5;
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
    cmp(y, l003a);
    //     bcs ca5f8
    if (flags & FLAG_C) goto ca5f8;
    //     lda (current_ruler_ptr),y
    a = ram[current_ruler_ptr + y];
    //     cmp #0x2a ; '*'
    cmp(a, 0x2a);
    //     bne loop_ca5e5
    if (!(flags & FLAG_Z)) goto loop_ca5e5;
    //     tya
    a = y;
ca5f1:
    //     sbc l0039
    sbc(l0039);
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
    return;
}

void read_char(void) {
    // Pseudocode: Reads a character from keyboard via SCREEN, returning escape flag in carry
    // flush_and_read_char / read_char (same entry point)
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
    if (flags & FLAG_C) goto read_char;

    //     cmp #0x1b                                                         ; A=character read
    cmp(a, 0x1b);
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

void return_to_cli_prompt(void) {
    longjmp(env, JMP_CLI);
}

void stop_printing(void) {
    // Pseudocode: Stops active printing by clearing print flags and calling printer driver cleanup

    // ; ***************************************************************************************
    // stop_printing:
    //     lda print_flags
    a = print_flags;
    set_flags(a);
    //     bpl c8459
    if (!(flags & FLAG_N)) goto c8459;
    //     rol print_flags
    a = rol(print_flags);
    print_flags = a;
    //     clc
    flags &= ~FLAG_C;
    //     ror print_flags
    a = ror(print_flags);
    print_flags = a;
    //     lda #6
    a = 6;
    //     jsr call_printer_driver
    call_printer_driver();
    // c8459:
c8459:
    //     rts
    return;
}

void sub_c9445(void) {
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
    print_char_just_to_screen();
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

void sub_cab6e(void) {
    // Pseudocode: Checks if byte at tmp0 is a command prefix (0x81)

    // sub_cab6e:
    //     ldy #0
    y = 0;
    //     lda (tmp0),y
    a = ram[((uint16_t)tmp1 << 8 | tmp0) + y];
    //     cmp #0x81
    cmp(a, 0x81);
    //     rts
    return;
}

void sub_cabc4(void) {
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

void sub_cadf0(void) {
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
    cmp(a, l0046);
    //     bcc cadff
    if (!(flags & FLAG_C)) goto cadff;
    //     sbc l0046
    sbc(l0046);
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

