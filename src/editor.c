#include "editor.h"
#include "document.h"
#include "io.h"
#include "globals.h"

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

