#include "editor.h"
#include "document.h"
#include "io.h"
#include <ctype.h>
#include "globals.h"

// Forward declarations for key handler functions
static void cf0_delete_block_key(void);
static void cf1_next_match_key(void);
static void cf2_format_mode_key(void);
static void cf3_justify_mode_key(void);
static void cf4_insert_mode_key(void);
static void cf5_default_ruler_key(void);
static void cf6_split_line_key(void);
static void cf7_join_lines_key(void);
static void cf8_mark_as_ruler_key(void);
static void delete_key(void);
static void f0_format_block_key(void);
static void f11_copy_key(void);
static void f12_left_key(void);
static void f14_down_key(void);
static void f15_up_key(void);
static void f1_top_of_text_key(void);
static void f2_bottom_of_text_key(void);
static void f3_delete_to_eol_key(void);
static void f4_beginning_of_line_key(void);
static void f5_end_of_line_key(void);
static void f6_insert_line_key(void);
static void f7_delete_line_key(void);
static void f8_insert_char_key(void);
static void f9_delete_char_key(void);
static void k_command_key(void);
static void o_command_key(void);
static void q_command_key(void);
static void sf0_move_block_key(void);
static void sf11_copy_key(void);
static void sf12_left_key(void);
static void sf13_right_key(void);
static void sf14_down_key(void);
static void sf15_up_key(void);
static void sf1_swap_case_key(void);
static void sf2_release_margins_key(void);
static void sf3_delete_to_char_key(void);
static void sf4_highlight1_key(void);
static void sf5_highlight2_key(void);
static void sf6_go_to_marker_key(void);
static void sf7_set_marker_key(void);
static void sf8_edit_command_key(void);
static void sf9_delete_command_key(void);
static void tab_key(void);

void return_to_editor_loop(void) {
    longjmp(env, JMP_EDITOR);
}

void editor_loop_impl(void) {
    screen_enter();
    // Pseudocode: Main editor loop: handles cursor positioning, redrawing, key dispatch
    for (;;) {
editor_loop:
    //     lda format_mode_flag
    //     pha
    {   uint8_t saved_fmt = format_mode_flag;
    //     lda l006e
    a = edit_buffer_unpacked_flag;
    //     bne c9b44
    if (a != 0) goto c9b44_;
    //     pha
    {   uint8_t saved_a_ = a;
    //     jsr sub_caa97
    sub_caa97();
    //     pla
    a = saved_a_; }
    //     sta l006e
    edit_buffer_unpacked_flag = a;
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
    cmp(a, l0072);
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
    cmp(y, xpos);
    //     bcs c9b84
    if (flags & FLAG_C) goto c9b84_;
    //     bit format_mode_flag
    bit(format_mode_flag);
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
    cmp(y, xpos);
    //     bcc c9b86
    if (!(flags & FLAG_C)) goto c9b86_;
    //     beq c9b86
    if (flags & FLAG_Z) goto c9b86_;
    //     ora #0x40 ; '@'
    a |= 0x40;
c9b84_:
    // c9b86:
    //     sta format_mode_flag
    format_mode_flag = a;
c9b8f_:
c9b73_:
c9b86_:
    //     pla (was: pop saved_fmt)
    //     cmp format_mode_flag
    //     beq c9b8f
    if (saved_fmt != format_mode_flag) {
    //     inc flags_need_redrawing_flag
        flags_need_redrawing_flag++;
    }
    // c9b8f:
    //     lda #0
    //     sta cursor_moved_flag
    cursor_moved_flag = 0;
    //     jsr redraw_editor
    redraw_editor();
    }
c9b96:
    //     jsr read_char
    read_char();
    //     cmp current_tab_key
    cmp(a, current_tab_key);
    //     bne c9b9f
    if (!(flags & FLAG_Z)) goto c9b9f;
    //     lda #9
    a = 9;
c9b9f:
    //     sta l0038
    l0038 = a;
    //     tay
    y = a;
    //     bmi c9bbb  ; omitted to support high-bit control characters
    // if (a & 0x80) goto editor_loop;
    //     cmp #0x20 ; ' '
    cmp(a, 0x20);
    //     bcc enter_nonprintable_character
    if (!(flags & FLAG_C)) goto enter_nonprintable_character;
    //     cmp #0x7f
    cmp(a, 0x7f);
    //     bcc enter_printable_character
    if (!(flags & FLAG_C)) { enter_printable_character(); goto editor_loop; }
enter_nonprintable_character:
    switch (a) {
        case CTRL('['): esc_key(); goto editor_loop;
        case CTRL('M'): return_key(); goto editor_loop;
        case 0x7f: delete_key(); goto editor_loop;
        case CTRL('I'): tab_key(); goto editor_loop;
        case CTRL('E'): f15_up_key(); goto editor_loop;
        case SCREEN_KEY_UP: f15_up_key(); goto editor_loop;
        case CTRL('S'): f12_left_key(); goto editor_loop;
        case SCREEN_KEY_LEFT: f12_left_key(); goto editor_loop;
        case CTRL('D'): f13_right_key(); goto editor_loop;
        case SCREEN_KEY_RIGHT: f13_right_key(); goto editor_loop;
        case CTRL('X'): f14_down_key(); goto editor_loop;
        case SCREEN_KEY_DOWN: f14_down_key(); goto editor_loop;
        case CTRL('A'): sf12_left_key(); goto editor_loop;
        case CTRL('F'): sf13_right_key(); goto editor_loop;
        case CTRL('C'): sf14_down_key(); goto editor_loop;
        case CTRL('R'): sf15_up_key(); goto editor_loop;
        case CTRL('G'): f9_delete_char_key(); goto editor_loop;
        case CTRL('H'): f8_insert_char_key(); goto editor_loop;
        case CTRL('Y'): f7_delete_line_key(); goto editor_loop;
        case CTRL('V'): cf4_insert_mode_key(); goto editor_loop;
        case CTRL('N'): f6_insert_line_key(); goto editor_loop;
        case CTRL('B'): f0_format_block_key(); goto editor_loop;
        case CTRL('T'): sf3_delete_to_char_key(); goto editor_loop;
        case CTRL('L'): cf1_next_match_key(); goto editor_loop;
        case CTRL('J'): cf7_join_lines_key(); goto editor_loop;
        case CTRL('P'): sf1_swap_case_key(); goto editor_loop;
        case CTRL('O'): o_command_key(); goto editor_loop;
        case CTRL('Q'): q_command_key(); goto editor_loop;
        case CTRL('K'): k_command_key(); goto editor_loop;
    }
    //     jmp editor_loop
    goto editor_loop;
    }
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

static void cf1_next_match_key(void) {
    write_line_back_to_document_safely();
    c8b7b();
    if (!(flags & FLAG_Z)) { esc_key(); return; }
    move_cursor_to_address();
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

static void cf5_default_ruler_key(void) {
    // cf5_default_ruler_key:
    //     jsr f6_insert_line_key
    f6_insert_line_key();
    //     jsr sub_ca276
    redraw_editor();
    //     jsr cf8_mark_as_ruler_key
    cf8_mark_as_ruler_key();
    //     lda current_edit_line_ptr
    a = (uint8_t)(current_edit_line_ptr & 0xff);
    //     ldy current_edit_line_ptr+1
    y = (uint8_t)(current_edit_line_ptr >> 8);
    //     jmp create_default_ruler
    create_default_ruler();
}

static void cf6_split_line_key(void) {
    // cf6_split_line_key: Splits line at cursor position

    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    //     jsr get_line_length
    get_line_length();
    //     cpy xpos
    cmp(y, xpos);
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
    flags = check_for_command_prefix(a);
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
    if (!(flags & FLAG_C)) { c9de3_insert_line(); return; }
    //     iny
    y++;
    //     bne c9de3
    if (y != 0) { c9de3_insert_line(); return; }
    //     (fall through - y wrapped to 0 → f6_insert_line_key)
    f6_insert_line_key();
}

// MULTIPLE ENTRY POINTS: cf6_split_line_key, f6_insert_line_key, sub_c9de1

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
    move_tmp01_to_next_line();
    //     beq c9eda
    if (flags & FLAG_Z) { beep(); return; }
    //     jsr check_for_command_prefix
    flags = check_for_command_prefix(a);
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
    a = edit_buffer_unpacked_flag;
    //     bmi c9f5f
    if (a & 0x80) goto c9f5f;
    //     lda #0x80
    a = 0x80;
    //     sta l006e
    edit_buffer_unpacked_flag = 0x80;
    //     inc l006d
    edit_buffer_dirty_flag++;
    // c9f5f:
c9f5f:
    //     jmp caf5c
    caf5c(); return;
}

static void delete_key(void) {
    // delete_key:
    //     lda l0072
    a = l0072;
    set_flags(a);
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
    cmp(a, 0x0c);
    //     bcc return_55
    if (!(flags & FLAG_C)) return;
    //     ldx insert_mode_flag
    x = insert_mode_flag;
    set_flags(x);
    //     bne return_55
    if (!(flags & FLAG_Z)) return;
    //     jsr get_line_length
    get_line_length();
    //     cpy xpos
    cmp(y, xpos);
    //     bcc return_55
    if (!(flags & FLAG_C)) return;
    //     beq return_55
    if (flags & FLAG_Z) return;
    //     falls through to f8_insert_char_key
    f8_insert_char_key();
}
// MULTIPLE ENTRY POINTS: delete_key, f8_insert_char_key

void esc_key(void) {
    // Pseudocode: Saves edit buffer via write_line_back_to_document_safely and returns to CLI prompt

    // esc_key:
    //     jsr write_line_back_to_document_safely
    //     jmp run_cli
    write_line_back_to_document_safely();
    run_cli();
}

static void f0_format_block_key(void) {
    // f0_format_block_key:
    //     jsr ca93c
    write_line_back_to_document_safely();
    //     lda l0073
    a = l0073;
    //     pha
    uint8_t saved_l0073 = a;
    //     lda l003d
    a = l003d;
    //     pha
    uint8_t saved_l003d = a;
    //     jsr ca741
    ca741();
    //     jsr sub_c9977
    sub_c9977();
    //     bvs ca05b
    if (flags & FLAG_V) { show_memory_full_error(); longjmp(env, JMP_EDITOR); }
    //     sec
    flags |= FLAG_C;
    //     bne ca051
    if (!(flags & FLAG_Z)) goto ca051;
    //     clc
    flags &= ~FLAG_C;
    // ca051:
ca051:
    //     pla
    a = saved_l003d;
    //     tax
    x = a;
    //     pla
    a = saved_l0073;
    //     bcs return_59
    if (flags & FLAG_C) goto return_59;
    //     stx l003d
    l003d = x;
    //     sta l0073
    l0073 = a;
    // return_59:
return_59:
    //     rts
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

void f13_right_key(void) {
    // f13_right_key:
    //     ldy xpos
    y = xpos;
    //     cpy #MAX_LINE_LENGTH
    cmp(y, MAX_LINE_LENGTH);
    //     bcs return_51
    if (flags & FLAG_C) return;
    //     inc xpos
    xpos++;
    // return_51:
    //     rts
    return;
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
    if (l0079 != 0) { c9d9b_advance_ptr(); return; }

    //     jsr return_key
    return_key();
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
    move_tmp01_to_previous_line();
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
// c9d9b: Shared line-advance logic used by return_key and f14_down_key

static void f1_top_of_text_key(void) {
    x = 0xff;
    l006f = x;
    sub_ca071();
    sub_caa97();
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

static void f3_delete_to_eol_key(void) {
    // f3_delete_to_eol_key: Deletes from cursor to end of line

    //     lda #MAX_LINE_LENGTH
    a = MAX_LINE_LENGTH;
    //     sec
    flags |= FLAG_C;
    //     sbc xpos
    sbc(xpos); x = a;
    //     tax
    //     inc l0074
    l0074++;
    //     jmp delete_edit_buffer_bytes_at_xpos
    delete_edit_buffer_bytes_at_xpos(); return;
}

static void f4_beginning_of_line_key(void) {
    // f4_beginning_of_line_key:
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    //     jmp c9e94
    c9e94(); return;
}

static void f5_end_of_line_key(void) {
    // Pseudocode: Moves cursor to end of current line

    // f5_end_of_line_key:
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    // c9e9b:
    sub_c9e9b();
}

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
    sub_c9de1();
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
    set_flags(a);
    //     bne c9e81
    if (!(flags & FLAG_Z)) goto c9e81;
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     ldy current_line_ptr+1
    y = (uint8_t)(current_line_ptr >> 8);
    //     jsr sub_cab37
    move_tmp01_to_previous_line();
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

static void f8_insert_char_key(void) {
    // f8_insert_char_key:
    //     lda #0x20 ; ' '
    a = 0x20;
    //     falls through to sub_c9e22
    sub_c9e22(); return;
}

static void f9_delete_char_key(void) {
    // f9_delete_char_key: Deletes character under cursor

    //     ldx #1
    x = 1;
    //     inc l0074
    l0074++;
    //     jmp delete_edit_buffer_bytes_at_xpos
    delete_edit_buffer_bytes_at_xpos(); return;
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

void return_key(void) {
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
    move_tmp01_to_next_line();
    //     bne c9d9b
    if (!(flags & FLAG_Z)) { c9d9b_advance_ptr(); return; }
    //     tya
    a = y;
    //     ldy current_line_ptr+1
    y = (uint8_t)(current_line_ptr >> 8);
    //     clc
    flags &= ~FLAG_C;
    //     adc current_line_ptr
    adc((uint8_t)(current_line_ptr & 0xff));
    //     bcc c9d98
    if (!(flags & FLAG_C)) goto c9d98;
    //     iny
    y++;
    // c9d98:
c9d98:
    //     jsr sub_c9de1
    sub_c9de1();
    //     // falls through to c9d9b
    c9d9b_advance_ptr();
}
// c9de3: Insert-line entry point used by cf6_split_line_key.
// Skips cursor_moved_flag++ that sub_c9de1 would do.
// Expects A = low byte, Y = high byte of insertion address.

static void sf0_move_block_key(void) {
    write_line_back_to_document_safely();
    reset_area_to_marks_1_2();
    if (flags & FLAG_C) { beep(); return; }
    sub_ca1cc();
    x = 0xff;
    top_of_screen_line_ptr = (top_of_screen_line_ptr & 0x00ff) | ((addr_t)x << 8);
    l006f = x;
    sub_c89d3();
    cb05a();
    a = (uint8_t)(doc_ptr1 & 0xff);
    y = (uint8_t)(doc_ptr1 >> 8);
    move_cursor_to_address();
    clear_marks_1_2();
}

static void sf11_copy_key(void) {
    // sf11_copy_key:
    //     jsr f6_insert_line_key
    f6_insert_line_key();
    //     jsr sub_ca276
    redraw_editor();
    //     ldx l003a
    x = l003a;
    //     beq ca0ef
    if (x == 0) goto ca0ef;
    //     ldy #0
    y = 0;
    // loop_ca0e7:
loop_ca0e7:
    //     lda (current_ruler_ptr),y
    a = ram[current_ruler_ptr + y];
    //     sta (current_edit_line_ptr),y
    ram[current_edit_line_ptr + y] = a;
    //     iny
    y++;
    //     dex
    x--;
    //     bne loop_ca0e7
    if (x != 0) goto loop_ca0e7;
    // ca0ef:
ca0ef:
    //     jmp cf8_mark_as_ruler_key
    cf8_mark_as_ruler_key();
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
    cmp(a, 0x20);
    //     beq c9f80
    if (flags & FLAG_Z) { sub_c9f80(); return; }
// return_57:
//     rts
}

static void sf13_right_key(void) {
    // sf13_right_key: Moves cursor right by one word
    uint8_t line_len;

entry:
    //     lda current_edit_line_ptr
    a = (uint8_t)(current_edit_line_ptr & 0xff);
    //     sta tmp0
    tmp0 = a;
    //     lda current_edit_line_ptr+1
    a = (uint8_t)(current_edit_line_ptr >> 8);
    //     sta tmp1
    tmp1 = a;
    //     jsr get_line_length
    get_line_length();
    //     sty input_buffer_ptr+1
    line_len = y;
    //     cpy xpos
    //     bcc c9fab
    if (y < xpos) goto c9fab;
    //     beq c9fab
    if (y == xpos) goto c9fab;
    //     ldy xpos
    y = xpos;
    // loop_c9ff8:
    goto loop_c9ff8;

c9fab:
    // c9fab:  (when cursor is at or past end of line, move to next line)
    //     sty xpos
    xpos = y;
    //     jsr ca93c
    write_line_back_to_document_safely();
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     ldy current_line_ptr+1
    y = (uint8_t)(current_line_ptr >> 8);
    //     jsr sub_cab1a
    sub_cab1a();
    //     beq return_58
    if (flags & FLAG_Z) return;
    //     tya
    a = y;
    //     clc
    flags &= ~FLAG_C;
    //     adc current_line_ptr
    adc((uint8_t)(current_line_ptr & 0xff));
    //     sta current_line_ptr
    current_line_ptr = (uint16_t)((current_line_ptr & 0xff00) | a);
    //     bcc c9fc3
    if (!(flags & FLAG_C)) goto c9fc3;
    //     inc current_line_ptr+1
    current_line_ptr = (uint16_t)(current_line_ptr + 0x100);
    // c9fc3:
c9fc3:
    //     jsr sub_caa97
    sub_caa97();
    //     dec l006f
    l006f--;
    //     jsr c9e94
    c9e94();
    //     jsr get_line_length
    get_line_length();
    //     cpy xpos
    if (y == xpos) return;          // xpos == 0, empty line
    //     lda current_edit_line_ptr
    a = (uint8_t)(current_edit_line_ptr & 0xff);
    //     sta tmp0
    tmp0 = a;
    //     lda current_edit_line_ptr+1
    a = (uint8_t)(current_edit_line_ptr >> 8);
    //     sta tmp1
    tmp1 = a;
    //     ldy xpos
    y = 0;
    //     jsr process_current_document_character
    process_current_document_character();
    //     cmp #0x20 ; ' '
    cmp(a, 0x20);
    //     bne return_58
    if (!(flags & FLAG_Z)) return;
    //     (fall through — line starts with space, scan forward as usual)
    goto entry;

loop_c9ff8:
    //     cpy input_buffer_ptr+1
    //     bcs ca00f
    if (y >= line_len) goto ca00f;
    //     jsr process_current_document_character
    process_current_document_character();
    //     cmp #0x20 ; ' '
    cmp(a, 0x20);
    //     bne loop_c9ff8
    if (!(flags & FLAG_Z)) goto loop_c9ff8;

loop_ca003:
    //     cpy input_buffer_ptr+1
    //     bcs ca00f
    if (y >= line_len) goto ca00f;
    //     jsr process_current_document_character
    process_current_document_character();
    //     cmp #0x20 ; ' '
    cmp(a, 0x20);
    //     beq loop_ca003
    if (flags & FLAG_Z) goto loop_ca003;
    //     dey
    y--;

ca00f:
    //     sty xpos
    xpos = y;
return_58:
    //     rts
    return;
}
static void set_marker(void);
static void set_marker_common(void);
// MULTIPLE ENTRY POINTS: sf7_set_marker_key, set_marker, set_marker_1..6

static void sf14_down_key(void) {
    // sf14_down_key:
    //     ldx screen_height
    //     inc l0079
    //     inc l006f
    x = screen_maxrow;
    l0079++;
    l006f++;
    sub_ca0af();
}

static void sf15_up_key(void) {
    // sf15_up_key:
    //     ldx screen_height
    x = screen_maxrow;
    //     inc l0079
    l0079++;
    //     inc l006f
    l006f++;
    sub_ca071();
}

static void sf1_swap_case_key(void) {
    // sf1_swap_case_key:
    //     ldy xpos
    y = xpos;
    //     lda (current_edit_line_ptr),y
    a = ram[current_edit_line_ptr + y];
    //     jsr is_uppercase
    if (isupper(a)) { flags &= ~FLAG_C; } else { flags |= FLAG_C; }
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
        while (y < MAX_LINE_LENGTH) {
            a = ram[current_edit_line_ptr + y];
            y++;
            if (a == search_char) goto loop_ca13d;
        }
        beep(); return;
loop_ca13d:
        // loop_ca13d: scan forward to find end of matching sequence
        while (y < MAX_LINE_LENGTH) {
            a = ram[current_edit_line_ptr + y];
            y++;
            if (a != search_char) break;
        }
        y--;
        x = y - start_x;
        delete_edit_buffer_bytes_at_xpos();
    }
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

static void sf8_edit_command_key(void) {
    // sf8_edit_command_key: Allows editing formatting command on current line interactively

    //     jsr c9e94
    c9e94();
    //     jsr redraw_editor
    redraw_editor();
    //     inc l006d
    edit_buffer_dirty_flag++;
    //     lda #0
    a = 0;
    //     sta input_buffer_offset+1
    l0080 = a;
    //     sta l0081
    l0081 = 0;
    // edit_command_loop:
edit_command_loop:
    //     ldx input_buffer_offset+1
    //     ldy ypos
    screen_setcursor(l0080, ypos);
    //     jsr read_char
    read_char();
    //     bcs finished_editing_command
    if (flags & FLAG_C) goto finished_editing_command;
    //     cmp #0x0d
    cmp(a, 0x0d);
    //     beq finished_editing_command
    if (flags & FLAG_Z) goto finished_editing_command;
    //     and #0xdf
    a &= 0xdf;
    //     cmp #0x41 ; 'A'
    cmp(a, 0x41);
    //     bcc edit_command_loop
    if (!(flags & FLAG_C)) goto edit_command_loop;
    //     cmp #0x5b ; '['
    cmp(a, 0x5b);
    //     bcs edit_command_loop
    if (flags & FLAG_C) goto edit_command_loop;
    //     sta l0081
    l0081 = a;
    //     jsr screen_putchar
    screen_putchar(a);
    //     ldy input_buffer_offset+1
    y = l0080;
    //     iny
    y++;
    //     sty input_buffer_offset+1
    l0080 = y;
    //     sta (ptr1),y
    ram[(uint16_t)(ptr1 >> 8) << 8 | (ptr1 & 0xff) + y] = a;
    //     cpy #2
    cmp(y, 2);
    //     bcc edit_command_loop
    if (!(flags & FLAG_C)) goto edit_command_loop;
    //     lda #0
    a = 0;
    //     sta input_buffer_offset+1
    l0080 = a;
    //     beq edit_command_loop
    goto edit_command_loop;

    // finished_editing_command:
finished_editing_command:
    //     lda l0081
    a = l0081;
    set_flags(a);
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

static void sf9_delete_command_key(void) {
    // sf9_delete_command_key: Deletes any formatting command prefix from current line

    //     ldy #0
    y = 0;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    //     jsr check_for_command_prefix
    flags = check_for_command_prefix(a);
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
    edit_buffer_dirty_flag++;
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    // return_56:
    //     rts
}

static void tab_key(void) {
    // tab_key:
    //     lda #9
    a = 9;
    //     jmp c9e3a
    tab_highlight_common(); return;
}

