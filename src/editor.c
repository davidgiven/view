#include "editor.h"

#include "document.h"

#include "io.h"

#include <ctype.h>

#include "globals.h"

// Editor-only functions
void adjust_pointers(void);
static void advance_to_next_line(void);
void beep(void);
static void c8b78(void);
void c8b7b(void);
static void c9de3_insert_line(void);
static void c9e94(void);
static void ca684(void);
void ca741(void);
void clear_screen(void);
static void clear_to_eol(void);
static void cursor_off(void);
static void cursor_on(void);
void draw_line(uint16_t addr);
void draw_prompt_characters(void);
static void draw_ruler(void);
static void draw_status_word(void);
static void get_line_length(void);
static void go_to_marker(void);
static void go_to_marker_n(void);
static void home_cursor(void);
void justify_edit_buffer(void);
void make_space_for_insertion(void);
static void memory_full(void);
void process_current_document_character(void);
static void recalculate_cursor_xpos(void);
void redraw_editor(void);
static void render_char(void);
static void render_xchar(void);
static void restore_cursor_position(void);
void sanitise_area(void);
static void save_cursor_position(void);
static void set_marker(void);
static void set_marker_common(void);
void show_memory_full_error(void);
void sub_c89d3(void);
static void sub_c8c51(void);
static void sub_c8c53(void);
void sub_c8c5f(void);
static void sub_c9936(void);
void sub_c9977(void);
static void sub_c9ac1(void);
static void sub_c9e22(void);
static void sub_c9e9b(void);
static void sub_ca44e(void);
static void sub_ca4d7(void);
static void sub_ca536(void);
static void sub_caa97(void);
static void sub_caacb(void);
void sub_cac41(void);
static void sub_cac50(void);
static void sub_cae03(void);
static void sub_caec2(void);
static void sub_caed6(void);
static void sub_caedd(void);
static void unpack_line_into_buffer(void);
void wipe_buffer(void);
static void write_line_back_to_document(void);
void write_line_back_to_document_safely(void);

void enter_editor_mode(void);
void sub_caf5f(void);
void caf5c(void);
void draw_previous_word(void);
void sub_caef4(void);
void insert_edit_buffer_bytes_at_xpos(void);
void set_marker_to_here(void);
void cac78(void);

// Editor-internal helper functions

static void c9d9b_advance_ptr(void);

static void clear_marks_1_2(void);

static void control_key_to_ascii(void);

static void delete_edit_buffer_bytes_at_xpos(void);

static void enter_printable_character(void);

static void go_to_marker_1(void);

static void go_to_marker_2(void);

static void go_to_marker_3(void);

static void go_to_marker_4(void);

static void go_to_marker_5(void);

static void go_to_marker_6(void);

static void prompt_for_marker(void);

static void reset_area_to_marks_1_2(void);

static void set_marker_1(void);

static void set_marker_2(void);

static void set_marker_3(void);

static void set_marker_4(void);

static void set_marker_5(void);

static void set_marker_6(void);

static void sub_c9de1(void);

static void sub_c9f80(void);

static void sub_ca071(void);

static void sub_ca0af(void);

static void sub_ca1cc(void);

static void tab_highlight_common(void);

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

void return_to_editor_loop(void)
{

    longjmp(env, JMP_EDITOR);
}

void run_editor(void)
{
    enter_editor_mode();
    longjmp(env, JMP_EDITOR);
}

void editor_loop_impl(void)
{
    // editor_loop_impl
    //  Inputs: y
    //  Outputs: a, x

    screen_enter();

    // Pseudocode: Main editor loop: handles cursor positioning, redrawing, key
    // dispatch

    for (;;)
    {

    editor_loop:

        //     lda format_mode_flag

        //     pha

        {
            uint8_t saved_fmt = format_mode_flag;

            //     lda l006e

            a = edit_buffer_unpacked_flag;

            //     bne c9b44

            if (a != 0)
                goto c9b44_;

            //     pha

            {
                uint8_t saved_a_ = a;

                //     jsr sub_caa97

                sub_caa97();

                //     pla

                a = saved_a_;
            }

            //     sta l006e

            edit_buffer_unpacked_flag = a;

        c9b44_:

            //     jsr sub_ca608

            recalculate_cursor_xpos();

            //     lda ruler_left_stop

            a = ruler_left_stop;

            //     beq c9b73

            if (a == 0)
                goto c9b73_;

            //     ldx format_mode_flag

            x = format_mode_flag;

            //     bmi c9b73

            if (x & 0x80)
                goto c9b73_;

            //     cmp l0072

            cmp(&flags, a, l0072);

            //     bcc c9b73

            if (!(flags & FLAG_C))
                goto c9b73_;

            //     beq c9b73

            if (flags & FLAG_Z)
                goto c9b73_;

            //     ldx cursor_moved_flag

            x = cursor_moved_flag;

            //     bne c9b6a

            if (x != 0)
                goto c9b6a_;

            //     jsr get_line_length

            get_line_length();

            //     lda format_mode_flag

            a = format_mode_flag;

            //     cpy xpos

            if (y >= xpos)
                goto c9b84_;

            //     bcs c9b84

            //     bit format_mode_flag

            bit(&flags, a, format_mode_flag);

            //     bvs c9b6a

            if (flags & FLAG_V)
                goto c9b6a_;

            //     sty xpos

            xpos = y;

            //     bvc c9b84 ; ALWAYS branch

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

            {
                uint8_t saved_mod = a;

                //     jsr sub_caec2

                sub_caec2();

                //     pla

                a = saved_mod;
            }

            //     bcs c9b86

            if (flags & FLAG_C)
                goto c9b86_;

            //     cpy xpos

            cmp(&flags, y, xpos);

            //     bcc c9b86

            if (!(flags & FLAG_C))
                goto c9b86_;

            //     beq c9b86

            if (flags & FLAG_Z)
                goto c9b86_;

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

            if (saved_fmt != format_mode_flag)
            {

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

        if (a != current_tab_key)
            goto c9b9f;

        //     bne c9b9f

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

        if (a < 0x20)
            goto enter_nonprintable_character;

        //     bcc enter_nonprintable_character

        //     cmp #0x7f

        cmp(&flags, a, 0x7f);

        //     bcc enter_printable_character

        if (!(flags & FLAG_C))
        {
            enter_printable_character();
            goto editor_loop;
        }

    enter_nonprintable_character:

        switch (a)
        {

            case CTRL('['):
                esc_key();
                goto editor_loop;

            case CTRL('M'):
                return_key();
                goto editor_loop;

            case 0x7f:
                delete_key();
                goto editor_loop;

            case CTRL('I'):
                tab_key();
                goto editor_loop;

            case CTRL('E'):
                f15_up_key();
                goto editor_loop;

            case SCREEN_KEY_UP:
                f15_up_key();
                goto editor_loop;

            case CTRL('S'):
                f12_left_key();
                goto editor_loop;

            case SCREEN_KEY_LEFT:
                f12_left_key();
                goto editor_loop;

            case CTRL('D'):
                f13_right_key();
                goto editor_loop;

            case SCREEN_KEY_RIGHT:
                f13_right_key();
                goto editor_loop;

            case CTRL('X'):
                f14_down_key();
                goto editor_loop;

            case SCREEN_KEY_DOWN:
                f14_down_key();
                goto editor_loop;

            case CTRL('A'):
                sf12_left_key();
                goto editor_loop;

            case CTRL('F'):
                sf13_right_key();
                goto editor_loop;

            case CTRL('C'):
                sf14_down_key();
                goto editor_loop;

            case CTRL('R'):
                sf15_up_key();
                goto editor_loop;

            case CTRL('G'):
                f9_delete_char_key();
                goto editor_loop;

            case CTRL('H'):
                f8_insert_char_key();
                goto editor_loop;

            case CTRL('Y'):
                f7_delete_line_key();
                goto editor_loop;

            case CTRL('V'):
                cf4_insert_mode_key();
                goto editor_loop;

            case CTRL('N'):
                f6_insert_line_key();
                goto editor_loop;

            case CTRL('B'):
                f0_format_block_key();
                goto editor_loop;

            case CTRL('T'):
                sf3_delete_to_char_key();
                goto editor_loop;

            case CTRL('L'):
                cf1_next_match_key();
                goto editor_loop;

            case CTRL('J'):
                cf7_join_lines_key();
                goto editor_loop;

            case CTRL('P'):
                sf1_swap_case_key();
                goto editor_loop;

            case CTRL('O'):
                o_command_key();
                goto editor_loop;

            case CTRL('Q'):
                q_command_key();
                goto editor_loop;

            case CTRL('K'):
                k_command_key();
                goto editor_loop;
        }

        //     jmp editor_loop

        goto editor_loop;
    }
}

static void cf0_delete_block_key(void)
{
    // cf0_delete_block_key
    //  Inputs: flags:C
    //  Outputs: a, y

    write_line_back_to_document_safely();

    cursor_moved_flag++;

    reset_area_to_marks_1_2();

    if (flags & FLAG_C)
    {
        beep();
        return;
    }

    a = (uint8_t)(area_start_ptr & 0xff);

    y = (uint8_t)(area_start_ptr >> 8);

    move_cursor_to_address();

    ca741();

    sub_c89d3();

    cb05a();

    clear_marks_1_2();
}

static void cf1_next_match_key(void)
{
    // cf1_next_match_key
    //  Inputs: flags:Z
    //  Outputs: a

    write_line_back_to_document_safely();

    c8b7b();

    if (!(flags & FLAG_Z))
    {
        esc_key();
        return;
    }

    move_cursor_to_address();
}

static void cf2_format_mode_key(void)
{

    a = format_mode_flag;

    a &= 0xbf;

    if (format_mode_flag & 0x40)
        a |= 1;

    a ^= 1;

    format_mode_flag = a;

    flags_need_redrawing_flag++;
}

static void cf3_justify_mode_key(void)
{

    justifying_flag ^= 0xff;

    flags_need_redrawing_flag++;
}

static void cf4_insert_mode_key(void)
{

    insert_mode_flag ^= 0xff;

    flags_need_redrawing_flag++;
}

static void cf5_default_ruler_key(void)
{
    // cf5_default_ruler_key
    //  Outputs: a, y

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

static void cf6_split_line_key(void)
{
    // cf6_split_line_key
    //  Inputs: y
    //  Outputs: a, x

    // cf6_split_line_key: Splits line at cursor position

    //     jsr write_line_back_to_document_safely

    write_line_back_to_document_safely();

    //     jsr get_line_length

    get_line_length();

    //     cpy xpos

    if (y < xpos)
        goto c9dbd;

    //     bcc c9dbd

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

    if (!(flags & FLAG_Z))
        goto c9dcd;

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

    flags &= ~FLAG_C;
    a = adc(&flags, a, (uint8_t)(current_line_ptr & 0xff));

    //     bcc c9de3

    if (!(flags & FLAG_C))
    {
        c9de3_insert_line();
        return;
    }

    //     iny

    y++;

    //     bne c9de3

    if (y != 0)
    {
        c9de3_insert_line();
        return;
    }

    //     (fall through - y wrapped to 0 → f6_insert_line_key)

    f6_insert_line_key();
}

// MULTIPLE ENTRY POINTS: cf6_split_line_key, f6_insert_line_key, sub_c9de1

static void cf7_join_lines_key(void)
{
    // cf7_join_lines_key
    //  Inputs: y
    //  Outputs: a; tmp01, tmp45, tmp67

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

    if (flags & FLAG_Z)
    {
        beep();
        return;
    }

    //     jsr check_for_command_prefix

    flags = check_for_command_prefix(a);

    //     beq c9eda

    if (flags & FLAG_Z)
    {
        beep();
        return;
    }

    //     dey

    y--;

    //     tya

    a = y;

    //     clc

    flags &= ~FLAG_C;

    //     adc current_line_ptr

    flags &= ~FLAG_C;
    a = adc(&flags, a, (uint8_t)(current_line_ptr & 0xff));

    //     sta tmp4

    tmp4 = a;

    //     lda current_line_ptr+1

    a = (uint8_t)(current_line_ptr >> 8);

    //     adc #0

    a = adc(&flags, a, 0);

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

    ca741();
    return;

    // c9eda:

    //     jmp beep
}

static void cf8_mark_as_ruler_key(void)
{
    // cf8_mark_as_ruler_key
    //  Ptrs:   ptr1
    //  Outputs: a, y

    // cf8_mark_as_ruler_key: Marks current line as a ruler line with . as
    // default characters

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

    if (a & 0x80)
        goto c9f5f;

    //     lda #0x80

    a = 0x80;

    //     sta l006e

    edit_buffer_unpacked_flag = 0x80;

    //     inc l006d

    edit_buffer_dirty_flag++;

    // c9f5f:

c9f5f:

    //     jmp caf5c

    caf5c();
    return;
}

static void delete_key(void)
{
    // delete_key
    //  Inputs: -
    //  Outputs: a, x, y

    // delete_key:

    //     lda l0072

    a = l0072;

    set_flags(&flags, a);

    //     beq return_55

    if (flags & FLAG_Z)
        return;

    //     dec xpos

    xpos--;

    //     ldy xpos

    y = xpos;

    //     lda (current_edit_line_ptr),y

    a = ram[current_edit_line_ptr + y];

    //     pha

    {
        uint8_t saved_a = a;

        //     jsr f9_delete_char_key

        f9_delete_char_key();

        //     pla

        a = saved_a;
    }

    //     cmp #0x0c

    cmp(&flags, a, 0x0c);

    //     bcc return_55

    if (!(flags & FLAG_C))
        return;

    //     ldx insert_mode_flag

    x = insert_mode_flag;

    if (x != 0)
        return;

    //     bne return_55

    //     jsr get_line_length

    get_line_length();

    //     cpy xpos

    cmp(&flags, y, xpos);

    //     bcc return_55

    if (!(flags & FLAG_C))
        return;

    //     beq return_55

    if (flags & FLAG_Z)
        return;

    //     falls through to f8_insert_char_key

    f8_insert_char_key();
}

// MULTIPLE ENTRY POINTS: delete_key, f8_insert_char_key

void esc_key(void)
{
    // esc_key
    //  Inputs: -
    //  Outputs: a, x

    // Pseudocode: Saves edit buffer via write_line_back_to_document_safely and
    // returns to CLI prompt

    // esc_key:

    //     jsr write_line_back_to_document_safely

    //     jmp run_cli

    write_line_back_to_document_safely();

    run_cli();
}

static void f0_format_block_key(void)
{

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

    if (flags & FLAG_V)
    {
        show_memory_full_error();
        longjmp(env, JMP_EDITOR);
    }

    //     sec

    flags |= FLAG_C;

    //     bne ca051

    if (!(flags & FLAG_Z))
        goto ca051;

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

    if (flags & FLAG_C)
        goto return_59;

    //     stx l003d

    l003d = x;

    //     sta l0073

    l0073 = a;

    // return_59:

return_59:

    //     rts
}

static void f11_copy_key(void)
{
    // f11_copy_key
    //  Inputs: flags:C

    write_line_back_to_document_safely();

    reset_area_to_marks_1_2();

    if (flags & FLAG_C)
    {
        beep();
        return;
    }

    sub_ca1cc();

    a = (uint8_t)(doc_ptr1 & 0xff);

    y = (uint8_t)(doc_ptr1 >> 8);

    move_cursor_to_address();
}

static void f12_left_key(void)
{
    // f12_left_key
    //  Outputs: y

    // f12_left_key: Moves cursor left by one position

    //     ldy l0072

    y = l0072;

    //     beq return_52

    if (y == 0)
        return;

    //     dec xpos

    xpos--;

    // return_52:
}

void f13_right_key(void)
{
    // f13_right_key
    //  Inputs: -
    //  Outputs: y

    // f13_right_key:

    //     ldy xpos

    y = xpos;

    //     cpy #MAX_LINE_LENGTH

    cmp(&flags, y, MAX_LINE_LENGTH);

    //     bcs return_51

    if (flags & FLAG_C)
        return;

    //     inc xpos

    xpos++;

    // return_51:

    //     rts

    return;
}

static void f14_down_key(void)
{

    // Pseudocode: Moves cursor to next line

    // ;
    // ***************************************************************************************

    // f14_down_key:

    //     jsr write_line_back_to_document_safely

    write_line_back_to_document_safely();

    //     inc l0079

    l0079++;

    //     bne c9d9b

    if (l0079 != 0)
    {
        c9d9b_advance_ptr();
        return;
    }

    //     jsr return_key

    return_key();
}

static void f15_up_key(void)
{
    // f15_up_key
    //  Inputs: flags:C
    //  Temps:  tmp01
    //  Outputs: a, y

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

    if (!(flags & FLAG_C))
        return;

    //     lda tmp0

    a = tmp0;

    //     sta current_line_ptr

    current_line_ptr = (uint16_t)((current_line_ptr & 0xff00) | a);

    //     lda tmp1

    a = tmp1;

    //     sta current_line_ptr+1

    current_line_ptr =
        (uint16_t)((current_line_ptr & 0x00ff) | ((uint16_t)a << 8));

    //     inc l0079

    l0079++;

    //     inc cursor_moved_flag

    cursor_moved_flag++;

    // return_53:
}

// c9d9b: Shared line-advance logic used by return_key and f14_down_key

static void f1_top_of_text_key(void)
{

    x = 0xff;

    l006f = x;

    sub_ca071();

    sub_caa97();
}

static void f2_bottom_of_text_key(void)
{
    // f2_bottom_of_text_key
    //  Outputs: x

    // Pseudocode: Moves cursor to bottom of document

    // ;
    // ***************************************************************************************

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

static void f3_delete_to_eol_key(void)
{
    // f3_delete_to_eol_key
    //  Inputs: -
    //  Outputs: a, flags:C

    // f3_delete_to_eol_key: Deletes from cursor to end of line

    //     lda #MAX_LINE_LENGTH

    a = MAX_LINE_LENGTH;

    //     sec

    flags |= FLAG_C;

    //     sbc xpos

    a -= xpos;
    x = a;

    //     tax

    //     inc l0074

    l0074++;

    //     jmp delete_edit_buffer_bytes_at_xpos

    delete_edit_buffer_bytes_at_xpos();
    return;
}

static void f4_beginning_of_line_key(void)
{

    // f4_beginning_of_line_key:

    //     inc cursor_moved_flag

    cursor_moved_flag++;

    //     jmp c9e94

    c9e94();
    return;
}

static void f5_end_of_line_key(void)
{

    // Pseudocode: Moves cursor to end of current line

    // f5_end_of_line_key:

    //     inc cursor_moved_flag

    cursor_moved_flag++;

    // c9e9b:

    sub_c9e9b();
}

static void f6_insert_line_key(void)
{

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

static void f7_delete_line_key(void)
{
    // f7_delete_line_key
    //  Inputs: -
    //  Temps:  tmp01
    //  Outputs: a, x, y; tmp45, tmp67

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

    if (a != 0)
        goto c9e81;

    //     bne c9e81

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

    current_line_ptr =
        (uint16_t)((current_line_ptr & 0x00ff) | ((uint16_t)a << 8));

    // c9e81:

c9e81:

    //     inc l0079

    l0079++;

    //     jmp ca741

    ca741();
    return;
}

// MULTIPLE ENTRY POINTS: sf2_release_margins_key, f4_beginning_of_line_key

static void f8_insert_char_key(void)
{

    // f8_insert_char_key:

    //     lda #0x20 ; ' '

    a = 0x20;

    //     falls through to sub_c9e22

    sub_c9e22();
    return;
}

static void f9_delete_char_key(void)
{

    // f9_delete_char_key: Deletes character under cursor

    //     ldx #1

    x = 1;

    //     inc l0074

    l0074++;

    //     jmp delete_edit_buffer_bytes_at_xpos

    delete_edit_buffer_bytes_at_xpos();
    return;
}

static void k_command_key(void)
{
    // k_command_key
    //  Inputs: a
    //  Outputs: x, y

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

    switch (a)
    {

        case 'M':
            sf7_set_marker_key();
            return;

        case 'C':
            f11_copy_key();
            return;

        case 'V':
            sf0_move_block_key();
            return;

        case 'Y':
            cf0_delete_block_key();
            return;

        case '1':
            set_marker_1();
            return;

        case '2':
            set_marker_2();
            return;

        case '3':
            set_marker_3();
            return;

        case '4':
            set_marker_4();
            return;

        case '5':
            set_marker_5();
            return;

        case '6':
            set_marker_6();
            return;
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

// ; If a char contains 0x20, it means that a match failure here still counts as
// a

// ; positive match --- allowing B to be the abbreviation for BYE, for example.

// parser_table:

uint8_t parser_table[] = {

    // ; 'QUIT' -> 0, flag=1

    //     .byte 0x0a, 0x0e, 0x12, 0x0f

    0x0a,
    0x0e,
    0x12,
    0x0f,

    //     .byte 0x81

    0x81,

    // ; 'NEW' -> 1, flag=0

    //     .byte 0x15, 0x1e, 0x0c

    0x15,
    0x1e,
    0x0c,

    //     .byte 0x80

    0x80,

    // ; 'FORMAT' -> 2, flag=1

    //     .byte 0x1d, 0x14, 9, 0x36, 0x3a, 0x2f

    0x1d,
    0x14,
    9,
    0x36,
    0x3a,
    0x2f,

    //     .byte 0x81

    0x81,

    // ; 'SETUP' -> 3, flag=0

    //     .byte 8, 0x1e, 0x0f, 0x2e, 0x2b

    8,
    0x1e,
    0x0f,
    0x2e,
    0x2b,

    //     .byte 0x80

    0x80,

    // ; 'READ' -> 4, flag=1

    //     .byte 9, 0x1e, 0x3a, 0x3f

    9,
    0x1e,
    0x3a,
    0x3f,

    //     .byte 0x81

    0x81,

    // ; 'MORE' -> 5, flag=1

    //     .byte 0x16, 0x14, 0x29, 0x3e

    0x16,
    0x14,
    0x29,
    0x3e,

    //     .byte 0x81

    0x81,

    // ; 'SCREEN' -> 6, flag=1

    //     .byte 8, 0x18, 0x29, 0x3e, 0x3e, 0x35

    8,
    0x18,
    0x29,
    0x3e,
    0x3e,
    0x35,

    //     .byte 0x81

    0x81,

    // ; 'SHEETS' -> 7, flag=1

    //     .byte 8, 0x13, 0x3e, 0x3e, 0x2f, 0x28

    8,
    0x13,
    0x3e,
    0x3e,
    0x2f,
    0x28,

    //     .byte 0x81

    0x81,

    // ; 'SAVE' -> 8, flag=1

    //     .byte 8, 0x1a, 0x2d, 0x3e

    8,
    0x1a,
    0x2d,
    0x3e,

    //     .byte 0x81

    0x81,

    // ; 'COUNT' -> 9, flag=1

    //     .byte 0x18, 0x14, 0x2e, 0x35, 0x2f

    0x18,
    0x14,
    0x2e,
    0x35,
    0x2f,

    //     .byte 0x81

    0x81,

    // ; 'FIELD' -> 10, flag=1

    //     .byte 0x1d, 0x12, 0x3e, 0x37, 0x3f

    0x1d,
    0x12,
    0x3e,
    0x37,
    0x3f,

    //     .byte 0x81

    0x81,

    // ; 'PRINTER' -> 11, flag=1

    //     .byte 0x0b, 9, 0x12, 0x15, 0x0f, 0x1e, 0x29

    0x0b,
    9,
    0x12,
    0x15,
    0x0f,
    0x1e,
    0x29,

    //     .byte 0x81

    0x81,

    // ; 'SEARCH' -> 12, flag=1

    //     .byte 8, 0x3e, 0x3a, 0x29, 0x38, 0x33

    8,
    0x3e,
    0x3a,
    0x29,
    0x38,
    0x33,

    //     .byte 0x81

    0x81,

    // ; 'CLEAR' -> 13, flag=1

    //     .byte 0x18, 0x17, 0x3e, 0x3a, 0x29

    0x18,
    0x17,
    0x3e,
    0x3a,
    0x29,

    //     .byte 0x81

    0x81,

    // ; 'MICROSPACE' -> 14, flag=1

    //     .byte 0x16, 0x12, 0x38, 0x29, 0x34, 0x28, 0x2b, 0x3a, 0x38, 0x3e

    0x16,
    0x12,
    0x38,
    0x29,
    0x34,
    0x28,
    0x2b,
    0x3a,
    0x38,
    0x3e,

    //     .byte 0x81

    0x81,

    // ; 'FOLD' -> 15, flag=1

    //     .byte 0x1d, 0x14, 0x37, 0x3f

    0x1d,
    0x14,
    0x37,
    0x3f,

    //     .byte 0x81

    0x81,

    // ; 'NAME' -> 16, flag=1

    //     .byte 0x15, 0x3a, 0x36, 0x3e

    0x15,
    0x3a,
    0x36,
    0x3e,

    //     .byte 0x81

    0x81,

    // ; 'MODE' -> 17, flag=0

    //     .byte 0x16, 0x34, 0x3f, 0x3e

    0x16,
    0x34,
    0x3f,
    0x3e,

    //     .byte 0x80

    0x80,

    // ; 'FINISH' -> 18, flag=1

    //     .byte 0x1d, 0x32, 0x35, 0x32, 0x28, 0x33

    0x1d,
    0x32,
    0x35,
    0x32,
    0x28,
    0x33,

    //     .byte 0x81

    0x81,

    // ; 'PRINT' -> 19, flag=1

    //     .byte 0x0b, 0x29, 0x32, 0x35, 0x2f

    0x0b,
    0x29,
    0x32,
    0x35,
    0x2f,

    //     .byte 0x81

    0x81,

    // ; 'CHANGE' -> 20, flag=1

    //     .byte 0x18, 0x33, 0x3a, 0x35, 0x3c, 0x3e

    0x18,
    0x33,
    0x3a,
    0x35,
    0x3c,
    0x3e,

    //     .byte 0x81

    0x81,

    // ; 'WRITE' -> 21, flag=1

    //     .byte 0x0c, 0x29, 0x32, 0x2f, 0x3e

    0x0c,
    0x29,
    0x32,
    0x2f,
    0x3e,

    //     .byte 0x81

    0x81,

    // ; 'EDIT' -> 22, flag=0

    //     .byte 0x1e, 0x3f, 0x32, 0x2f

    0x1e,
    0x3f,
    0x32,
    0x2f,

    //     .byte 0x80

    0x80,

    // ; 'REPLACE' -> 23, flag=1

    //     .byte 9, 0x3e, 0x2b, 0x37, 0x3a, 0x38, 0x3e

    9,
    0x3e,
    0x2b,
    0x37,
    0x3a,
    0x38,
    0x3e,

    //     .byte 0x81

    0x81,

    // ; 'LOAD' -> 24, flag=0

    //     .byte 0x17, 0x34, 0x3a, 0x3f

    0x17,
    0x34,
    0x3a,
    0x3f,

    //     .byte 0x80

    0x80,

    // ; 'BYE' -> 25, flag=1

    //     .byte 0x19, 0x22, 0x3e

    0x19,
    0x22,
    0x3e,

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

static void o_command_key(void)
{
    // o_command_key
    //  Inputs: a
    //  Outputs: x, y

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

    switch (a)
    {

        case 'J':
            cf3_justify_mode_key();
            return;

        case 'X':
            sf2_release_margins_key();
            return;

        case 'C':
            sf8_edit_command_key();
            return;

        case 'D':
            sf9_delete_command_key();
            return;

        case 'F':
            cf2_format_mode_key();
            return;

        case 'M':
            cf8_mark_as_ruler_key();
            return;

        case 'R':
            sf11_copy_key();
            return;

        case 'S':
            cf5_default_ruler_key();
            return;

        case 'U':
            sf4_highlight1_key();
            return;

        case 'B':
            sf5_highlight2_key();
            return;
    }

    return;
}

static void q_command_key(void)
{
    // q_command_key
    //  Inputs: a
    //  Outputs: x, y

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

    switch (a)
    {

        case 'R':
            f1_top_of_text_key();
            return;

        case 'C':
            f2_bottom_of_text_key();
            return;

        case 'S':
            f4_beginning_of_line_key();
            return;

        case 'D':
            f5_end_of_line_key();
            return;

        case 'Y':
            f3_delete_to_eol_key();
            return;

        case 'J':
            cf6_split_line_key();
            return;

        case 'M':
            sf6_go_to_marker_key();
            return;

        case '1':
            go_to_marker_1();
            return;

        case '2':
            go_to_marker_2();
            return;

        case '3':
            go_to_marker_3();
            return;

        case '4':
            go_to_marker_4();
            return;

        case '5':
            go_to_marker_5();
            return;

        case '6':
            go_to_marker_6();
            return;
    }

    return;
}

void return_key(void)
{
    // return_key
    //  Inputs: y
    //  Outputs: a; tmp01

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

    if (!(flags & FLAG_Z))
    {
        c9d9b_advance_ptr();
        return;
    }

    //     tya

    a = y;

    //     ldy current_line_ptr+1

    y = (uint8_t)(current_line_ptr >> 8);

    //     clc

    flags &= ~FLAG_C;

    //     adc current_line_ptr

    a = adc(&flags, a, (uint8_t)(current_line_ptr & 0xff));

    //     bcc c9d98

    if (!(flags & FLAG_C))
        goto c9d98;

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

static void sf0_move_block_key(void)
{
    // sf0_move_block_key
    //  Inputs: flags:C

    write_line_back_to_document_safely();

    reset_area_to_marks_1_2();

    if (flags & FLAG_C)
    {
        beep();
        return;
    }

    sub_ca1cc();

    x = 0xff;

    top_of_screen_line_ptr =
        (top_of_screen_line_ptr & 0x00ff) | ((addr_t)x << 8);

    l006f = x;

    sub_c89d3();

    cb05a();

    a = (uint8_t)(doc_ptr1 & 0xff);

    y = (uint8_t)(doc_ptr1 >> 8);

    move_cursor_to_address();

    clear_marks_1_2();
}

static void sf11_copy_key(void)
{
    // sf11_copy_key
    //  Outputs: a, x, y

    // sf11_copy_key:

    //     jsr f6_insert_line_key

    f6_insert_line_key();

    //     jsr sub_ca276

    redraw_editor();

    //     ldx l003a

    x = l003a;

    //     beq ca0ef

    if (x == 0)
        goto ca0ef;

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

    if (x != 0)
        goto loop_ca0e7;

    // ca0ef:

ca0ef:

    //     jmp cf8_mark_as_ruler_key

    cf8_mark_as_ruler_key();
}

static void sf12_left_key(void)
{
    // sf12_left_key
    //  Outputs: y

    // Pseudocode: Moves cursor left by one word

    // ;
    // ***************************************************************************************

    // sf12_left_key:

    //     ldy xpos

    y = xpos;

    //     beq c9f80

    if (y == 0)
    {
        sub_c9f80();
        return;
    }

    //     jsr draw_previous_word

    draw_previous_word();

    //     bne return_57

    if (!(flags & FLAG_Z))
        return;

    //     cmp #0x20 ; ' '

    cmp(&flags, a, 0x20);

    //     beq c9f80

    if (flags & FLAG_Z)
    {
        sub_c9f80();
        return;
    }

    // return_57:

    //     rts
}

static void sf13_right_key(void)
{
    // sf13_right_key
    //  Inputs: y
    //  Outputs: a; tmp01

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

    if (y < xpos)
        goto c9fab;

    //     beq c9fab

    if (y == xpos)
        goto c9fab;

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

    if (flags & FLAG_Z)
        return;

    //     tya

    a = y;

    //     clc

    flags &= ~FLAG_C;

    //     adc current_line_ptr

    a = adc(&flags, a, (uint8_t)(current_line_ptr & 0xff));

    //     sta current_line_ptr

    current_line_ptr = (uint16_t)((current_line_ptr & 0xff00) | a);

    //     bcc c9fc3

    if (!(flags & FLAG_C))
        goto c9fc3;

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

    if (y == xpos)
        return; // xpos == 0, empty line

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

    cmp(&flags, a, 0x20);

    //     bne return_58

    if (!(flags & FLAG_Z))
        return;

    //     (fall through — line starts with space, scan forward as usual)

    goto entry;

loop_c9ff8:

    //     cpy input_buffer_ptr+1

    //     bcs ca00f

    if (y >= line_len)
        goto ca00f;

    //     jsr process_current_document_character

    process_current_document_character();

    //     cmp #0x20 ; ' '

    if (a != 0x20)
        goto loop_c9ff8;

    //     bne loop_c9ff8

loop_ca003:

    //     cpy input_buffer_ptr+1

    //     bcs ca00f

    if (y >= line_len)
        goto ca00f;

    //     jsr process_current_document_character

    process_current_document_character();

    //     cmp #0x20 ; ' '

    if (a == 0x20)
        goto loop_ca003;

    //     beq loop_ca003

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

static void sf14_down_key(void)
{

    // sf14_down_key:

    //     ldx screen_height

    //     inc l0079

    //     inc l006f

    x = screen_maxrow;

    l0079++;

    l006f++;

    sub_ca0af();
}

static void sf15_up_key(void)
{

    // sf15_up_key:

    //     ldx screen_height

    x = screen_maxrow;

    //     inc l0079

    l0079++;

    //     inc l006f

    l006f++;

    sub_ca071();
}

static void sf1_swap_case_key(void)
{
    // sf1_swap_case_key
    //  Inputs: -
    //  Outputs: a, y

    // sf1_swap_case_key:

    //     ldy xpos

    y = xpos;

    //     lda (current_edit_line_ptr),y

    a = ram[current_edit_line_ptr + y];

    //     jsr is_uppercase

    if (isupper(a))
    {
        flags &= ~FLAG_C;
    }
    else
    {
        flags |= FLAG_C;
    }

    //     bcs f13_right_key

    if (flags & FLAG_C)
    {
        f13_right_key();
        return;
    }

    //     inc l0074

    l0074++;

    //     eor #0x20 ; ' '

    a ^= 0x20;

    //     sta (current_edit_line_ptr),y

    ram[current_edit_line_ptr + y] = a;

    //     falls through to f13_right_key

    f13_right_key();
    return;
}

static void sf2_release_margins_key(void)
{

    // sf2_release_margins_key:

    //     bit format_mode_flag

    if (!(format_mode_flag & FLAG_V))
    {
        c9e94();
        return;
    }

    //     jsr sub_caec2

    sub_caec2();

    //     bcs f4_beginning_of_line_key

    if (flags & FLAG_C)
    {
        f4_beginning_of_line_key();
        return;
    }

    //     sty xpos

    xpos = y;

    //     rts

    return;
}

static void sf3_delete_to_char_key(void)
{
    // sf3_delete_to_char_key
    //  Outputs: a, x, y

    x = 0x43;

    y = 0x48;

    draw_prompt_characters();

    flags_need_redrawing_flag++;

    read_char();

    if (a == 9)
        goto ca12a;

    if (a != 0xa0)
        goto ca11a;

    a = 0x1c;

    goto ca12a;

ca11a:

    if (a != 0xa1)
        goto ca122;

    a = 0x1d;

    goto ca12a;

ca122:

    if (a < 0x20)
    {
        beep();
        return;
    }

    if (a >= 0x7f)
    {
        beep();
        return;
    }

ca12a:

{

    uint8_t search_char = a;

    l0074++;

    y = xpos;

    uint8_t start_x = y;

    // loop_ca132: scan forward to find matching char

    while (y < MAX_LINE_LENGTH)
    {

        a = ram[current_edit_line_ptr + y];

        y++;

        if (a == search_char)
            goto loop_ca13d;
    }

    beep();
    return;

loop_ca13d:

    // loop_ca13d: scan forward to find end of matching sequence

    while (y < MAX_LINE_LENGTH)
    {

        a = ram[current_edit_line_ptr + y];

        y++;

        if (a != search_char)
            break;
    }

    y--;

    x = y - start_x;

    delete_edit_buffer_bytes_at_xpos();
}
}

static void sf4_highlight1_key(void)
{

    // sf4_highlight1_key:

    //     lda #0x1c

    a = 0x1c;

    //     jmp c9e3a

    tab_highlight_common();
    return;
}

static void sf5_highlight2_key(void)
{

    // sf5_highlight2_key:

    //     lda #0x1d

    a = 0x1d;

    //     jmp c9e3a

    tab_highlight_common();
    return;
}

static void sf6_go_to_marker_key(void)
{

    // sf6_go_to_marker_key:

    //     jsr write_line_back_to_document_safely

    write_line_back_to_document_safely();

    //     jsr prompt_for_marker

    prompt_for_marker();

    //     bcs return_58

    if (flags & FLAG_C)
        return;

    //     beq return_58

    if (flags & FLAG_Z)
        return;

    // go_to_marker:

    go_to_marker();
    return;
}

static void sf7_set_marker_key(void)
{

    // sf7_set_marker_key:

    //     jsr write_line_back_to_document_safely

    write_line_back_to_document_safely();

    //     jsr prompt_for_marker

    prompt_for_marker();

    //     bcs return_58

    if (flags & FLAG_C)
        return;

    // set_marker:

    set_marker();
    return;
}

static void sf8_edit_command_key(void)
{
    // sf8_edit_command_key
    //  Inputs: flags:C
    //  Ptrs:   ptr1
    //  Outputs: a, y

    // sf8_edit_command_key: Allows editing formatting command on current line
    // interactively

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

    if (flags & FLAG_C)
        goto finished_editing_command;

    //     cmp #0x0d

    if (a == 0x0d)
        goto finished_editing_command;

    //     beq finished_editing_command

    //     and #0xdf

    a &= 0xdf;

    //     cmp #0x41 ; 'A'

    if (a < 0x41)
        goto edit_command_loop;

    //     bcc edit_command_loop

    //     cmp #0x5b ; '['

    if (a >= 0x5b)
        goto edit_command_loop;

    //     bcs edit_command_loop

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

    if (y < 2)
        goto edit_command_loop;

    //     bcc edit_command_loop

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

    set_flags(&flags, a);

    //     beq return_56

    if (flags & FLAG_Z)
        return;

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

    caf5c();
    return;
}

static void sf9_delete_command_key(void)
{
    // sf9_delete_command_key
    //  Outputs: a, y

    // sf9_delete_command_key: Deletes any formatting command prefix from
    // current line

    //     ldy #0

    y = 0;

    //     lda (current_format_line_ptr),y

    a = ram[current_format_line_ptr + y];

    //     jsr check_for_command_prefix

    flags = check_for_command_prefix(a);

    //     bne return_56

    if (!(flags & FLAG_Z))
        return;

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

static void tab_key(void)
{

    // tab_key:

    //     lda #9

    a = 9;

    //     jmp c9e3a

    tab_highlight_common();
    return;
}

static void c9d9b_advance_ptr(void)
{
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    //     lda current_line_ptr
    //     ldy current_line_ptr+1
    //     jsr sub_cab1a
    a = (uint8_t)(current_line_ptr & 0xff);
    y = (uint8_t)(current_line_ptr >> 8);
    sub_cab1a();
    //     beq return_54
    if (flags & FLAG_Z)
        return;
    //     tya
    a = y;
    //     clc
    flags &= ~FLAG_C;
    //     adc current_line_ptr
    a = adc(&flags, a, (uint8_t)(current_line_ptr & 0xff));
    //     sta current_line_ptr
    current_line_ptr = (uint16_t)((current_line_ptr & 0xff00) | a);
    //     bcc return_54
    if (!(flags & FLAG_C))
        return;
    //     inc current_line_ptr+1
    current_line_ptr = (uint16_t)(current_line_ptr + 0x100);
    // return_54:
    //     rts
}

static void clear_marks_1_2(void)
{
    // clear_marks_1_2: Clears markers 1 and 2 (resets to zero)

    //     lda #0
    a = 0;
    //     ldx #3
    x = 3;
    // loop_cad12:
loop_cad12:
    //     sta __begin_pointer_array,x
    ((uint8_t*)markers_array)[x] = a;
    //     dex
    x--;
    //     bpl loop_cad12
    if (!(x & 0x80))
        goto loop_cad12;
    //     rts
}

static void control_key_to_ascii(void)
{
    // Pseudocode: Converts control key code to ASCII letter by ORing with 0x40
    // zproc control_key_to_ascii
    //     cmp #0x20
    //     zif lt
    //         ora #0x40
    //     zendif
    //     jmp to_uppercase
    // zendproc
    if (a < 0x20)
        a |= 0x40;
    a = toupper(a);
    return;
}

static void delete_edit_buffer_bytes_at_xpos(void)
{
    // delete_edit_buffer_bytes_at_xpos
    //  Inputs: x
    //  Outputs: a, y, flags:C, flags:Z; tmp67
    // delete_edit_buffer_bytes_at_xpos: Deletes N bytes at cursor position,
    // shifting existing content left

    //     stx input_buffer_offset+1
    l0080 = x;
    //     inc l006d
    edit_buffer_dirty_flag++;
    //     lda current_edit_line_ptr
    a = (uint8_t)(current_edit_line_ptr & 0xff);
    //     sta tmp6
    tmp6 = a;
    //     lda current_edit_line_ptr+1
    a = (uint8_t)(current_edit_line_ptr >> 8);
    //     sta tmp7
    tmp7 = a;
    //     ldy xpos
    y = xpos;
    //     tya
    a = y;
    //     clc
    //     adc input_buffer_offset+1
    a += l0080;
    //     sta l0084
    // cae78:
cae78:
    //     jsr sub_ca536
    sub_ca536();
    //     bne cae98
    if (!(flags & FLAG_Z))
        goto cae98;
    //     lda #0
    a = 0;
    //     cpy l0084
    cmp(&flags, y, l0084);
    //     bcc cae91
    if (!(flags & FLAG_C))
        goto cae91;
    //     tya
    a = y;
    //     sbc input_buffer_offset+1
    a = sbc(&flags, a, l0080);
    //     clc
    flags &= ~FLAG_C;
    //     adc current_edit_line_ptr
    a = adc(&flags, a, (uint8_t)(current_edit_line_ptr & 0xff));
    //     sta markers_array,x
    ((uint8_t*)markers_array)[x] = a;
    //     lda current_edit_line_ptr+1
    a = (uint8_t)(current_edit_line_ptr >> 8);
    //     adc #0
    a = adc(&flags, a, 0);
    //     bne cae93
    if (!(flags & FLAG_Z))
        goto cae93;
    // cae91:
cae91:
    //     sta markers_array,x
    ((uint8_t*)markers_array)[x] = a;
    // cae93:
cae93:
    //     sta markers_array+1,x
    ((uint8_t*)markers_array)[x + 1] = a;
    //     jmp cae78
    goto cae78;

    // cae98:
cae98:
    //     iny
    y++;
    //     cpy #0x85
    cmp(&flags, y, MAX_LINE_LENGTH + 1);
    //     bcc cae78
    if (!(flags & FLAG_C))
        goto cae78;
    //     lda xpos
    a = xpos;
    //     cmp #0x84
    cmp(&flags, a, MAX_LINE_LENGTH);
    //     bcs return_78
    if (flags & FLAG_C)
    { /* return_78: */
        return;
    }
    //     ldy xpos
    y = xpos;
    // loop_caea5:
loop_caea5:
    //     sty l0084
    l0084 = y;
    //     ldx #0x10
    x = 0x10;
    //     tya
    a = y;
    //     clc
    flags &= ~FLAG_C;
    //     adc input_buffer_offset+1
    a = adc(&flags, a, l0080);
    //     bcs caeb7
    if (flags & FLAG_C)
        goto caeb7;
    //     tay
    y = a;
    //     cpy #0x84
    if (y >= MAX_LINE_LENGTH)
        goto caeb7;
    //     bcs caeb7
    //     lda (current_edit_line_ptr),y
    a = ram[current_edit_line_ptr + y];
    //     tax
    x = a;
    // caeb7:
caeb7:
    //     ldy l0084
    y = l0084;
    //     txa
    a = x;
    //     sta (current_edit_line_ptr),y
    ram[current_edit_line_ptr + y] = a;
    //     iny
    y++;
    //     cpy #0x84
    if (y < MAX_LINE_LENGTH)
        goto loop_caea5;
    //     bcc loop_caea5
    // return_78:
    //     rts
}

static void enter_printable_character(void)
{
    // enter_printable_character
    //  Inputs: x
    //  Outputs: a, y; tmp45, tmp67
    // enter_printable_character:
    //     ldy xpos
    y = xpos;
    //     cpy #0x84
    if (y >= MAX_LINE_LENGTH)
        return;
    //     inc l006d
    edit_buffer_dirty_flag++;
    //     jsr sub_caef4
    sub_caef4();
    //     bcs c9bca
    if (flags & FLAG_C)
        return;
    //     lda current_edit_line_ptr
    tmp67 = current_edit_line_ptr;
    //     ldy xpos
    y = xpos;
    //     jsr sub_ca536
    sub_ca536();
    //     bne c9bf2
    if (!(flags & FLAG_Z))
        goto c9bf2;
    //     cpx #4
    //     bcs c9bf2
    if (x >= 4)
        goto c9bf2;
    //     inc l0074
    l0074++;
    // c9bf2:
c9bf2:
    //     ldx insert_mode_flag
    x = insert_mode_flag;
    //     bne c9c00
    if (x != 0)
        goto c9c00;
    //     lda (current_edit_line_ptr),y
    a = ram[current_edit_line_ptr + y];
    //     cmp #9
    //     beq c9c00
    if (a == 9)
        goto c9c00;
    //     cmp #0x0b
    //     bne c9c09
    if (a != 0x0b)
        goto c9c09;
    // c9c00:
c9c00:
    //     inc l0074
    l0074++;
    //     ldx #1
    x = 1;
    //     jsr insert_edit_buffer_bytes_at_xpos
    insert_edit_buffer_bytes_at_xpos();
    //     bcs c9c7f
    if (flags & FLAG_C)
    {
        return;
    }
    // c9c09:
c9c09:
    //     lda l0038
    a = l0038;
    //     sta (current_edit_line_ptr),y
    ram[current_edit_line_ptr + y] = a;
    //     ldy l0074
    y = l0074;
    //     bne c9c14
    if (y != 0)
        goto c9c14;
    //     jsr screen_putchar
    screen_putchar(a);
    // c9c14:
c9c14:
    //     inc xpos
    xpos++;
    //     jsr ca684
    ca684();
    //     ldy #0
    y = 0;
    //     sty l0039
    l0039 = 0;
    // c9c1d:
c9c1d:
    //     lda (current_edit_line_ptr),y
    a = ram[current_edit_line_ptr + y];
    //     iny
    y++;
    //     cpy xpos
    //     bcs c9c56
    if (y > xpos)
        goto c9c56;
    //     cmp #9
    //     bne c9c31
    if (a != 9)
        goto c9c31;
    //     jsr sub_ca5ae
    process_document_character();
    //     txa
    a = x;
    //     clc
    //     adc l0039
    a += l0039;
    //     bne c9c43
    if (a != 0)
        goto c9c43;
    // c9c31:
c9c31:
    //     cmp #0x0b
    //     bne c9c4a
    if (a != 0x0b)
        goto c9c4a;
    //     lda ruler_left_stop
    a = ruler_left_stop;
    set_flags(&flags, a);
    //     beq c9c48
    if (flags & FLAG_Z)
        goto c9c48;
    //     ldx l0039
    x = l0039;
    //     beq c9c43
    if (x == 0)
        goto c9c43;
    //     cpx ruler_left_stop
    if (x < ruler_left_stop)
        goto c9c43;
    //     bcc c9c43
    //     inx
    x++;
    //     txa
    a = x;
    // c9c43:
c9c43:
    //     sta l0039
    l0039 = a;
    //     jmp c9c1d
    goto c9c1d;
    // c9c48:
c9c48:
    //     lda #0x20 ; ' '
    a = 0x20;
    // c9c4a:
c9c4a:
    //     cmp #0x1b
    //     bcc c9c48
    if (a < 0x1b)
        goto c9c48;
    //     cmp #0x20
    //     bcc c9c1d
    if (a < 0x20)
        goto c9c1d;
    //     inc l0039
    l0039++;
    //     bne c9c1d
    goto c9c1d;
    // c9c56:
c9c56:
    //     ldy l0039
    y = l0039;
    //     cpy l003a
    if (y < l003a)
    {
        //     lda (current_ruler_ptr),y
        a = ram[current_ruler_ptr + y];
        //     and #0xdf
        a &= 0xdf;
        //     cmp #0x42 ; 'B'
        if (a == 0x42)
            beep();
    }
    //     lda l0038
    a = l0038;
    //     cmp #0x20 ; ' '
    //     beq c9c7f
    if (a == 0x20)
        return;
    //     lda ruler_right_stop
    //     beq c9c7f
    if (ruler_right_stop == 0)
    {
        l0074 = 0;
        return_to_editor_loop();
    }
    //     lda format_mode_flag
    //     bne c9c7f
    if (format_mode_flag != 0)
        return;
    //     lda #0
    //     sta tmp7
    tmp7 = 0;
    //     tya
    //     beq c9c7f
    if (y == 0)
        return;
    //     dey
    y--;
    //     cpy ruler_right_stop
    //     bcs c9c82
    if (y < ruler_right_stop)
        return;
    // c9c82: (4202)
    //     jsr get_line_length (4203)
    get_line_length();
    //     sty l0083 (4204)
    l0083 = y;
    //     lda #0 (4205)
    a = 0;
    //     sta top_margin (4206)
    top_margin = a;
    //     ldy xpos (4207)
    y = xpos;
    //     sty input_buffer_ptr+1 (4208)
    input_buffer_offset = y;
    //     jsr draw_previous_word (4209)
    draw_previous_word();
    //     jsr sub_ca608 (4210)
    recalculate_cursor_xpos();
    //     lda l0072 (4211)
    a = l0072;
    //     cmp ruler_left_stop (4212)
    cmp(&flags, a, ruler_left_stop);
    //     beq c9c9d (4213) bcs c9ca2 (4214)
    // c9c9d: (4215)
    //     ldy input_buffer_ptr+1, dey, sty xpos (4216-4218)
    if (flags & FLAG_Z)
    {
        y = input_buffer_offset;
        y--;
        xpos = y;
        goto c9ca2;
    }
    if (flags & FLAG_C)
        goto c9ca2;
    {
        y = input_buffer_offset;
        y--;
        xpos = y;
    }
    // c9ca2: (4219)
c9ca2:
    //     lda input_buffer_ptr+1 (4220)
    a = input_buffer_offset;
    //     sec (4221)
    flags |= FLAG_C;
    //     sbc xpos (4222)
    a = sbc(&flags, a, xpos);
    //     sta top_margin (4223)
    top_margin = a;
    //     lda l0083 (4224)
    a = l0083;
    //     sec (4225)
    flags |= FLAG_C;
    //     sbc xpos (4226)
    a = sbc(&flags, a, xpos);
    //     sta l0083 (4227)
    l0083 = a;
    //     tay (4228)
    y = a;
    //     iny (4229)
    y++;
    //     lda ruler_left_stop (4230)
    a = ruler_left_stop;
    set_flags(&flags, a);
    //     beq c9cb9 (4231)
    if (flags & FLAG_Z)
        goto c9cb9;
    //     inc top_margin (4232)
    top_margin++;
    //     iny (4233)
    y++;
    // c9cb9: (4234)
c9cb9:
    //     sty tmp6 (4235)
    tmp6 = y;
    //     lda current_line_ptr (4236)
    a = (uint8_t)(current_line_ptr & 0xff);
    //     sec (4237)
    flags |= FLAG_C;
    //     adc l003b (4238)
    a = adc(&flags, a, l003b);
    //     sta tmp4 (4239)
    tmp4 = a;
    //     lda current_line_ptr+1 (4240)
    a = (uint8_t)(current_line_ptr >> 8);
    //     adc #0 (4241)
    a = adc(&flags, a, 0);
    //     sta tmp5 (4242)
    tmp5 = a;
    //     jsr make_space_for_insertion (4243)
    make_space_for_insertion();
    //     bcc c9cd0 (4244)
    if (!(flags & FLAG_C))
        goto c9cd0;
    //     jmp ca941 (4245)
    show_memory_full_error();
    longjmp(env, JMP_EDITOR);
    // c9cd0: (4247)
c9cd0:
    //     ldy #0 (4248)
    y = 0;
    //     lda ruler_left_stop
    //     beq c9cdb
    if (ruler_left_stop != 0)
    {
        //     lda #0x0b
        //     sta (tmp4),y
        ram[tmp45] = 0x0b;
        //     iny ; Y=0x01
        y = 1;
    }
    // c9cdb:
c9cdb:
    //     sty l0081
    l0081 = y;
    //     lda current_edit_line_ptr
    tmp67 = current_edit_line_ptr;
    //     ldy xpos
    y = xpos;
    //     dey
    y--;
    //     lda (current_edit_line_ptr),y
    a = ram[current_edit_line_ptr + y];
    //     cmp #0x20 ; ' '
    //     bne c9cf2
    if (a == 0x20)
    {
        //     lda #0x10
        //     sta (current_edit_line_ptr),y
        ram[current_edit_line_ptr + y] = 0x10;
    }
    // c9cf2:
c9cf2:
    //     iny
    y++;
    //     sty l0082
    l0082 = y;
    // c9cf5:
c9cf5:
    //     ldy l0082
    y = l0082;
    //     inc l0082
    l0082++;
    // loop_c9cf9:
loop_c9cf9:
    //     jsr sub_ca536
    sub_ca536();
    //     bne c9d0d
    if (!(flags & FLAG_Z))
        goto c9d0d;
    //     lda l0081
    a = l0081;
    //     clc
    flags &= ~FLAG_C;
    //     adc tmp4
    a = adc(&flags, a, tmp4);
    //     sta markers_array,x
    ((uint8_t*)markers_array)[x] = a;
    //     lda tmp5
    a = tmp5;
    //     adc #0
    a = adc(&flags, a, 0);
    //     sta markers_array+1,x
    ((uint8_t*)markers_array)[x + 1] = a;
    //     bcc loop_c9cf9
    if (!(flags & FLAG_C))
        goto loop_c9cf9;
    // c9d0d:
c9d0d:
    //     lda l0083
    a = l0083;
    //     bne c9d28
    if (a != 0)
        goto c9d28;
    //     lda #0x0d
    a = 0x0d;
    //     bne c9d30                                                         ;
    //     ALWAYS branch
    goto c9d30;
    // c9d28:
c9d28:
    //     lda (current_edit_line_ptr),y
    a = ram[current_edit_line_ptr + y];
    //     pha
    {
        uint8_t saved = a;
        //     lda #0x10
        //     sta (current_edit_line_ptr),y
        ram[current_edit_line_ptr + y] = 0x10;
        //     pla
        a = saved;
    }
    // c9d30:
c9d30:
    //     ldy l0081
    y = l0081;
    //     inc l0081
    l0081++;
    //     sta (tmp4),y
    ram[tmp45 + y] = a;
    //     dec l0083
    l0083--;
    //     bpl c9cf5
    if (!(l0083 & 0x80))
        goto c9cf5;
    //     bmi c9d15                                                         ;
    //     ALWAYS branch
    // c9d15:
c9d15:
    //     jsr justify_edit_buffer
    justify_edit_buffer();
    //     jsr ca93c
    write_line_back_to_document_safely();
    //     jsr ca741
    ca741();
    //     jsr return_key
    return_key();
    //     lda top_margin
    //     sta xpos
    xpos = top_margin;
    //     jmp editor_loop
    return;
}
// MULTIPLE ENTRY POINTS: sf1_swap_case_key, f13_right_key

static void go_to_marker_1(void)
{
    // go_to_marker_1:
    a = '1';
    go_to_marker_n();
    return;
}

static void go_to_marker_2(void)
{
    // go_to_marker_2:
    a = '2';
    go_to_marker_n();
    return;
}

static void go_to_marker_3(void)
{
    // go_to_marker_3:
    a = '3';
    go_to_marker_n();
    return;
}

static void go_to_marker_4(void)
{
    // go_to_marker_4:
    a = '4';
    go_to_marker_n();
    return;
}

static void go_to_marker_5(void)
{
    // go_to_marker_5:
    a = '5';
    go_to_marker_n();
    return;
}

static void go_to_marker_6(void)
{
    // go_to_marker_6
    //  Outputs: a
    // go_to_marker_6:
    a = '6';
    go_to_marker_n();
    return;
}

static void prompt_for_marker(void)
{
    // Pseudocode: Prompts for a marker character and looks it up

    // prompt_for_marker:
    //     ldx #0x4d ; 'M'
    //     ldy #0x4b ; 'K'
    x = 0x4d;
    y = 0x4b;
    //     jsr draw_prompt_characters
    draw_prompt_characters();
    //     inc flags_need_redrawing_flag
    flags_need_redrawing_flag++;
    //     jsr read_char
    read_char();
    //     jsr lookup_marker
    lookup_marker();
    //     bcc return_74
    if (!(flags & FLAG_C))
        return;
    // ;
    // ***************************************************************************************
    // beep:
    // loop_caced:
    //     sec
    // return_74:
    //     rts
    flags |= FLAG_C;
    return;
}

static void reset_area_to_marks_1_2(void)
{
    // reset_area_to_marks_1_2
    //  Inputs: x, flags:Z
    //  Outputs: a
    // reset_area_to_marks_1_2: Sets area to markers 1 and 2, then adjusts
    // doc_ptr1

    //     lda #0x31 ; '1'
    a = 0x31;
    //     jsr lookup_marker
    lookup_marker();
    //     bcs return_76
    if (flags & FLAG_C)
        return;
    //     beq cad45
    if (flags & FLAG_Z)
        goto cad45;
    //     lda __begin_pointer_array,x
    a = ((uint8_t*)markers_array)[x];
    //     sta area_start_ptr
    area_start_ptr = (uint16_t)((uint8_t*)markers_array)[x + 1] << 8 | a;
    //     lda markers_array+1,x
    //     sta area_start_ptr+1
    //     lda #0x32 ; '2'
    a = 0x32;
    //     jsr lookup_marker
    lookup_marker();
    //     bcs return_76
    if (flags & FLAG_C)
        return;
    //     beq cad45
    if (flags & FLAG_Z)
        goto cad45;
    //     lda __begin_pointer_array,x
    a = ((uint8_t*)markers_array)[x];
    //     sta area_end_ptr
    area_end_ptr = (uint16_t)((uint8_t*)markers_array)[x + 1] << 8 | a;
    //     lda markers_array+1,x
    //     sta area_end_ptr+1
    //     ldx #doc_ptr1
    x = ((uint8_t*)&doc_ptr1 - (uint8_t*)markers_array);
    //     jsr set_marker_to_here
    set_marker_to_here();
    //     jsr sanitise_area
    sanitise_area();
    //     clc
    flags &= ~FLAG_C;
    //     bne return_76
    if (!(flags & FLAG_Z))
        return;
    // cad45:
cad45:
    //     sec
    flags |= FLAG_C;
    // return_76:
    //     rts
}

static void set_marker_1(void)
{
    // set_marker_1:
    a = '1';
    set_marker_common();
    return;
}

static void set_marker_2(void)
{
    // set_marker_2:
    a = '2';
    set_marker_common();
    return;
}

static void set_marker_3(void)
{
    // set_marker_3:
    a = '3';
    set_marker_common();
    return;
}

static void set_marker_4(void)
{
    // set_marker_4:
    a = '4';
    set_marker_common();
    return;
}

static void set_marker_5(void)
{
    // set_marker_5:
    a = '5';
    set_marker_common();
    return;
}

static void set_marker_6(void)
{
    // set_marker_6:
    a = '6';
    set_marker_common();
    return;
}

static void sub_c9de1(void)
{
    // sub_c9de1:
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    //     falls through to c9de3
    c9de3_insert_line();
}

static void sub_c9f80(void)
{
    // c9f80:
    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    //     lda current_line_ptr
    a = current_line_ptr;
    //     ldy current_line_ptr+1
    y = current_line_ptr >> 8;
    //     jsr sub_cab37
    move_tmp01_to_previous_line();
    //     bcc return_56
    if (!(flags & FLAG_C))
        return;
    //     lda tmp0
    a = tmp0;
    //     sta current_line_ptr
    current_line_ptr = (current_line_ptr & 0xff00) | a;
    //     lda tmp1
    a = tmp1;
    //     sta current_line_ptr+1
    current_line_ptr = (current_line_ptr & 0x00ff) | ((uint16_t)a << 8);
    //     jsr sub_caa97
    sub_caa97();
    //     jsr c9e9b
    sub_c9e9b();
    //     dec l006f
    l006f--;
}

static void sub_ca071(void)
{
    // sub_ca071
    //  Inputs: x, flags:C
    //  Temps:  tmp01
    //  Outputs: a, y; tmp23
    // sub_ca071:
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    //     stx input_buffer_offset+1
    l0080 = x;
    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     ldy current_line_ptr+1
    y = (uint8_t)((current_line_ptr >> 8) & 0xff);
    // ca07c:
    while (1)
    {
        //     sta tmp2
        tmp23 = (addr_t)(y) << 8 | a;
        //     jsr sub_cab37
        move_tmp01_to_previous_line();
        //     lda tmp0
        a = tmp0;
        //     ldy tmp1
        y = tmp1;
        //     bcc ca093
        if (!(flags & FLAG_C))
        {
            // ca093:
            //     lda tmp2
            a = tmp2;
            //     ldy tmp3
            y = tmp3;
            break;
        }
        //     ldx input_buffer_offset+1
        x = l0080;
        //     bmi ca07c
        if ((int8_t)x < 0)
            continue;
        //     dec input_buffer_offset+1
        l0080--;
        //     bne ca07c
        if (l0080 != 0)
            continue;
        break;
    }
    // ca097:
    //     sta current_line_ptr
    current_line_ptr = (current_line_ptr & 0xff00) | a;
    //     sty current_line_ptr+1
    current_line_ptr = (current_line_ptr & 0x00ff) | ((uint16_t)y << 8);
    //     rts
}

static void sub_ca0af(void)
{
    // sub_ca0af
    //  Inputs: x, flags:Z
    //  Temps:  tmp01
    //  Outputs: a, y
    // sub_ca0af:
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    //     stx input_buffer_offset+1
    l0080 = x;
    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     ldy current_line_ptr+1
    y = (uint8_t)((current_line_ptr >> 8) & 0xff);
    // ca0ba:
    while (1)
    {
        //     jsr sub_cab1a
        sub_cab1a();
        //     beq ca0d2
        if (flags & FLAG_Z)
        {
            // ca0d2:
            //     lda tmp0
            a = tmp0;
            //     ldy tmp1
            y = tmp1;
            break;
        }
        //     tya
        //     ldy tmp1
        //     clc
        //     adc tmp0
        {
            uint16_t sum = (uint16_t)y + tmp0;
            y = tmp1;
            a = (uint8_t)(sum & 0xff);
            if (sum > 0xff)
                y++;
        }
        // ca0c8:
        //     ldx input_buffer_offset+1
        x = l0080;
        //     bmi ca0ba
        if ((int8_t)x < 0)
            continue;
        //     dec input_buffer_offset+1
        l0080--;
        //     bne ca0ba
        if (l0080 != 0)
            continue;
        //     beq ca0d6
        break;
    }
    // ca0d6:
    //     sta current_line_ptr
    current_line_ptr = (current_line_ptr & 0xff00) | a;
    //     sty current_line_ptr+1
    current_line_ptr = (current_line_ptr & 0x00ff) | ((uint16_t)y << 8);
    //     rts
}

static void sub_ca1cc(void)
{
    // sub_ca1cc
    //  Inputs: -
    //  Outputs: a, y; tmp45, tmp67
    if (doc_ptr1 >= area_start_ptr && doc_ptr1 < area_end_ptr)
    {
        beep();
        return;
    }
    a = (uint8_t)(area_start_ptr & 0xff);
    y = (uint8_t)(area_start_ptr >> 8);
    move_cursor_to_address();
    {
        uint16_t diff = area_end_ptr - area_start_ptr;
        tmp67 = diff;
    }
    tmp45 = doc_ptr1;
    make_space_for_insertion();
    if (flags & FLAG_C)
    {
        show_memory_full_error();
        longjmp(env, JMP_EDITOR);
    }
    tmp89 = area_start_ptr;
    tmp23 = tmp45;
    // ca219:
    while (1)
    {
        ram[tmp23] = ram[tmp89];
        tmp2++;
        if (tmp2 == 0)
            tmp3++;
        tmp8++;
        if (tmp8 == 0)
            tmp9++;
        if ((tmp89) == area_end_ptr)
            break;
    }
    uint8_t saved_tmp6 = tmp6;
    uint8_t saved_tmp7 = tmp7;
    doc_ptr1 = tmp45;
    {
        uint16_t adjusted = (tmp23)-1;
        a = (uint8_t)(adjusted & 0xff);
        y = (uint8_t)(adjusted >> 8);
    }
    cac78();
    a = (uint8_t)(doc_ptr1 & 0xff);
    y = (uint8_t)(doc_ptr1 >> 8);
    cac78();
    tmp7 = saved_tmp7;
    tmp6 = saved_tmp6;
    l0073 = 1;
    cursor_moved_flag = 1;
    flags &= ~FLAG_C;
}

static void tab_highlight_common(void)
{
    // c9e3a:
    //     pha
    {
        uint8_t saved_a = a;
        //     jsr sub_caef4
        sub_caef4();
        //     pla
        a = saved_a;
    }
    //     bcs return_55
    if (flags & FLAG_C)
        return;
    //     jsr sub_c9e22
    sub_c9e22();
    //     bcs return_55
    if (flags & FLAG_C)
        return;
    //     jmp f13_right_key
    f13_right_key();
    return;
}
void enter_editor_mode(void)
{
    screen_enter();
    // enter_editor_mode: Enters editor mode: clears screen, resets state
    // variables

    //     jsr clear_screen
    clear_screen();
    //     lda #0
    a = 0;
    //     sta l006d
    edit_buffer_dirty_flag = 0;
    //     sta l006f
    l006f = 0;
    //     sta l006e
    edit_buffer_unpacked_flag = 0;
    //     ldx screen_height
    x = screen_maxrow;
    // loop_cb0a8:
loop_cb0a8:
    //     sta line_lengths,x
    line_lengths[x] = a;
    //     dex
    x--;
    //     bpl loop_cb0a8
    if (!(x & 0x80))
        goto loop_cb0a8;
    //     ldx #2
    x = 2;
    //     stx l0073
    l0073 = 2;
    //     stx status_line_needs_redrawing_flag
    status_line_needs_redrawing_flag = 2;
    flags_need_redrawing_flag = 1;
    //     rts
}

void sub_caf5f(void)
{
    // sub_caf5f:
    uint8_t old = format_mode_flag;
    format_mode_flag &= ~0x80;
    if (old != format_mode_flag)
    {
        flags_need_redrawing_flag++;
    }
}

void caf5c(void)
{
    // caf5c:
    uint8_t old = format_mode_flag;
    format_mode_flag |= 0x80;
    if (old != format_mode_flag)
    {
        flags_need_redrawing_flag++;
    }
}

void draw_previous_word(void)
{
    // draw_previous_word
    //  Outputs: a, y; tmp01
    // draw_previous_word: Moves cursor back to start of previous word

    //     lda current_edit_line_ptr
    a = (uint8_t)(current_edit_line_ptr & 0xff);
    //     sta tmp0
    tmp0 = a;
    //     lda current_edit_line_ptr+1
    a = (uint8_t)(current_edit_line_ptr >> 8);
    //     sta tmp1
    tmp1 = a;
    //     ldy xpos
    y = xpos;
    //     beq caf55
    if (y == 0)
        goto caf55;
    // loop_caf3f:
loop_caf3f:
    //     dey
    y--;
    //     beq caf55
    if (y == 0)
        goto caf55;
    //     jsr process_current_document_character
    process_current_document_character();
    //     dey
    y--;
    //     cmp #0x20 ; ' '
    if (a == 0x20)
        goto loop_caf3f;
    //     beq loop_caf3f
    // loop_caf4a:
loop_caf4a:
    //     dey
    y--;
    //     jsr process_current_document_character
    process_current_document_character();
    //     cmp #0x20 ; ' '
    if (a == 0x20)
        goto caf55;
    //     beq caf55
    //     dey
    y--;
    //     bne loop_caf4a
    if (y != 0)
        goto loop_caf4a;
    // caf55:
caf55:
    //     sty xpos
    xpos = y;
    //     jsr process_current_document_character
    process_current_document_character();
    //     dey
    y--;
    set_flags(&flags, y);
    //     rts
}

void sub_caef4(void)
{
    // sub_caef4
    //  Inputs: y
    //  Outputs: a, flags:C
    // sub_caef4: Handles margin/folding adjustments when typing at left margin

    //     lda format_mode_flag
    a = format_mode_flag;
    //     and #0x81
    a &= 0x81;
    if (a != 0)
        goto caf31;
    //     bne caf31
    //     jsr sub_caec2
    sub_caec2();
    //     bcc caf31
    if (!(flags & FLAG_C))
        goto caf31;
    //     jsr get_line_length
    get_line_length();
    //     lda xpos
    a = xpos;
    //     sta l0083
    l0083 = a;
    //     sty xpos
    xpos = y;
    //     jsr sub_ca608
    recalculate_cursor_xpos();
    //     lda l0072
    a = l0072;
    //     cmp ruler_left_stop
    if (a < ruler_left_stop)
        goto caf19;
    //     bcc caf19
    //     ldy l0083
    y = l0083;
    //     sty xpos
    xpos = y;
    //     inc xpos
    xpos++;
    //     bcs caf2a
    goto caf2a;

    // caf19:
caf19:
    //     lda l0083
    a = l0083;
    //     ldy xpos
    y = xpos;
    //     cpy l0083
    cmp(&flags, y, l0083);
    //     bcs caf28
    if (flags & FLAG_C)
        goto caf28;
    //     sec
    flags |= FLAG_C;
    //     sbc ruler_left_stop
    a = sbc(&flags, a, ruler_left_stop);
    //     bcc caf2a
    if (!(flags & FLAG_C))
        goto caf2a;
    //     adc xpos
    a = adc(&flags, a, xpos);
    // caf28:
caf28:
    //     sta xpos
    xpos = a;
    // caf2a:
caf2a:
    //     jsr sub_caedd
    sub_caedd();
    //     bcs return_79
    if (flags & FLAG_C)
    { /* return_79: */
        return;
    }
    //     inc l0074
    l0074++;
    // caf31:
caf31:
    //     clc
    flags &= ~FLAG_C;
    // return_79:
    //     rts
}

void insert_edit_buffer_bytes_at_xpos(void)
{
    // insert_edit_buffer_bytes_at_xpos
    //  Inputs: x, y
    //  Outputs: a, flags:C, flags:Z; tmp67
    // insert_edit_buffer_bytes_at_xpos: Inserts bytes at cursor position,
    // shifting existing content right

    //     lda xpos
    a = xpos;
    //     cmp #MAX_LINE_LENGTH
    cmp(&flags, a, MAX_LINE_LENGTH);
    //     bcs cae03
    if (flags & FLAG_C)
    {
        sub_cae03();
        return;
    }
    //     stx input_buffer_offset+1
    l0080 = x;
    //     jsr get_line_length
    get_line_length();
    //     tya
    a = y;
    //     clc
    flags &= ~FLAG_C;
    //     adc input_buffer_offset+1
    a = adc(&flags, a, l0080);
    //     bcs cae03
    if (flags & FLAG_C)
    {
        sub_cae03();
        return;
    }
    //     cmp #0x85
    cmp(&flags, a, MAX_LINE_LENGTH + 1);
    //     bcs cae03
    if (flags & FLAG_C)
    {
        sub_cae03();
        return;
    }
    //     inc l006d
    edit_buffer_dirty_flag++;
    //     lda current_edit_line_ptr
    a = (uint8_t)(current_edit_line_ptr & 0xff);
    //     sta tmp6
    tmp6 = a;
    //     lda current_edit_line_ptr+1
    a = (uint8_t)(current_edit_line_ptr >> 8);
    //     sta tmp7
    tmp7 = a;
    //     ldy #0x84
    y = MAX_LINE_LENGTH;
    // cae27:
cae27:
    //     dey
    y--;
    //     ldx #0
    x = 0;
    //     tya
    a = y;
    //     clc
    flags &= ~FLAG_C;
    //     adc input_buffer_offset+1
    a = adc(&flags, a, l0080);
    //     bcs cae35
    if (flags & FLAG_C)
        goto cae35;
    //     cmp #0x84
    if (a >= MAX_LINE_LENGTH)
        goto cae35;
    //     bcs cae35
    //     tax
    x = a;
    // cae35:
cae35:
    //     stx l0081
    l0081 = x;
    // loop_cae37:
loop_cae37:
    //     jsr sub_ca536
    sub_ca536();
    //     bne cae52
    if (!(flags & FLAG_Z))
        goto cae52;
    //     lda l0081
    a = l0081;
    set_flags(&flags, a);
    //     beq cae4b
    if (flags & FLAG_Z)
        goto cae4b;
    //     clc
    flags &= ~FLAG_C;
    //     adc current_edit_line_ptr
    a = adc(&flags, a, (uint8_t)(current_edit_line_ptr & 0xff));
    //     sta markers_array,x
    ((uint8_t*)markers_array)[x] = a;
    //     lda current_edit_line_ptr+1
    a = (uint8_t)(current_edit_line_ptr >> 8);
    //     adc #0
    a = adc(&flags, a, 0);
    //     bne cae4d
    if (!(flags & FLAG_Z))
        goto cae4d;
    // cae4b:
cae4b:
    //     sta markers_array,x
    ((uint8_t*)markers_array)[x] = a;
    // cae4d:
cae4d:
    //     sta markers_array+1,x
    ((uint8_t*)markers_array)[x + 1] = a;
    //     jmp loop_cae37
    goto loop_cae37;

    // cae52:
cae52:
    //     lda (current_edit_line_ptr),y
    a = ram[current_edit_line_ptr + y];
    //     sty l0084
    l0084 = y;
    //     ldy l0081
    y = l0081;
    //     beq cae5c
    if (y == 0)
        goto cae5c;
    //     sta (current_edit_line_ptr),y
    ram[current_edit_line_ptr + y] = a;
    // cae5c:
cae5c:
    //     ldy l0084
    y = l0084;
    //     cpy xpos
    cmp(&flags, y, xpos);
    //     bne cae27
    if (!(flags & FLAG_Z))
        goto cae27;
    //     clc
    flags &= ~FLAG_C;
    //     rts
}

void set_marker_to_here(void)
{
    // set_marker_to_here
    //  Inputs: y
    // set_marker_to_here: Sets marker at current cursor position

    //     jsr get_line_length
    get_line_length();
    //     cpy xpos
    if (y < xpos)
        goto cad5d;
    //     bcc cad5d
    //     ldy #0
    y = 0;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    //     ldy xpos
    y = xpos;
    //     jsr check_for_command_prefix
    flags = check_for_command_prefix(a);
    //     bne cad5c
    if (!(flags & FLAG_Z))
        goto cad5c;
    //     iny
    y++;
    //     iny
    y++;
    //     iny
    y++;
    // cad5c:
cad5c:
    //     tya
    a = y;
    // cad5d:
cad5d:
    //     clc
    flags &= ~FLAG_C;
    //     adc current_line_ptr
    flags &= ~FLAG_C;
    a = adc(&flags, a, (uint8_t)(current_line_ptr & 0xff));
    //     sta 0,x
    ((uint8_t*)markers_array)[x] = a;
    //     lda current_line_ptr+1
    a = (uint8_t)(current_line_ptr >> 8);
    //     adc #0
    a = adc(&flags, a, 0);
    //     sta 1,x
    ((uint8_t*)markers_array)[x + 1] = a;
    //     rts
}

void cac78(void)
{
    // cac78
    //  Temps:  tmp89
    //  Outputs: a, x, y, flags:C, flags:Z
    // Pseudocode: Splits a line at the word wrap position, inserting CR for new
    // line

    // cac78:
    //     jsr sub_cac50
    sub_cac50();
    // cac7b:
cac7b:
    //     lda #0
    //     sta l0083
    a = 0;
    l0083 = 0;
    //     ldx #0x85
    x = MAX_LINE_LENGTH + 1;
    //     ldy #1
    y = 1;
    //     lda (tmp8),y
    a = ram[tmp89 + y];
    //     jsr check_for_command_prefix
    flags = check_for_command_prefix(a);
    //     bne cac8d
    if (!(flags & FLAG_Z))
        goto cac8d;
    //     inx
    //     inx
    //     inx
    x++;
    x++;
    x++;
    // cac8d:
cac8d:
    //     stx l0084
    l0084 = x;
    // cac8f:
cac8f:
    //     lda (tmp8),y
    a = ram[tmp89 + y];
    //     iny
    y++;
    //     cmp #0x20 ; ' '
    if (a == 0x20)
        goto cac9a;
    //     beq cac9a
    //     cmp #0x1a
    if (a != 0x1a)
        goto cac9c;
    //     bne cac9c
    // cac9a:
cac9a:
    //     sty l0083
    l0083 = y;
    // cac9c:
cac9c:
    //     cmp #0x0d
    cmp(&flags, a, 0x0d);
    //     beq return_73
    if (flags & FLAG_Z)
        return;
    //     cpy l0084
    cmp(&flags, y, l0084);
    //     beq cac8f
    if (flags & FLAG_Z)
        goto cac8f;
    //     bcc cac8f
    if (!(flags & FLAG_C))
        goto cac8f;
    //     lda l0084
    //     ldx l0083
    //     beq cacad
    //     txa
    if (l0083 == 0)
    {
        a = l0084;
        goto cacad;
    }
    a = l0083;
    // cacad:
cacad:
    //     clc
    flags &= ~FLAG_C;
    //     adc tmp8
    a = adc(&flags, a, tmp8);
    //     sta tmp4
    //     sta tmp8
    tmp4 = a;
    tmp8 = a;
    //     lda tmp9
    //     adc #0
    //     sta tmp5
    //     sta tmp9
    a = tmp9;
    a = adc(&flags, a, 0);
    tmp5 = a;
    tmp9 = a;
    //     lda #1
    //     sta tmp6
    //     lda #0
    //     sta tmp7
    tmp6 = 1;
    tmp7 = 0;
    //     jsr make_space_for_insertion
    make_space_for_insertion();
    //     lda #0x0d
    a = 0x0d;
    //     ldy #0
    y = 0;
    //     sta (tmp4),y
    ram[tmp45 + y] = a;
    //     lda tmp4
    //     sta tmp8
    //     lda tmp5
    //     sta tmp9
    tmp8 = tmp4;
    tmp9 = tmp5;
    //     bne cac7b
    if (tmp9 != 0)
        goto cac7b;
    return;
}

void adjust_pointers(void)
{
    // adjust_pointers
    //  Inputs: -
    //  Temps:  tmp23, tmp45, tmp67, tmp89
    //  Outputs: a, x, y
    uint8_t tmp2, tmp3, tmp8, tmp9;
    // adjust_pointers: (6372)
    //     lda tmp4 (6373)
    a = tmp4;
    //     sta tmp2 (6374)
    tmp2 = a;
    //     clc (6375)
    flags &= ~FLAG_C;
    //     adc tmp6 (6376)
    a = adc(&flags, a, tmp6);
    //     sta tmp8 (6377)
    tmp8 = a;
    //     lda tmp5 (6378)
    a = tmp5;
    //     sta tmp3 (6379)
    tmp3 = a;
    //     adc tmp7 (6380)
    a = adc(&flags, a, tmp7);
    //     sta tmp9 (6381)
    tmp9 = a;
    //     ldx #0 (6382)
    x = 0;
    // ca9c3: (6383)
ca9c3:
    //     ldy __begin_pointer_array+1,x (6384)
    y = ((uint8_t*)&pointer_array)[x + 1];
    //     lda __begin_pointer_array+0,x (6385)
    a = ((uint8_t*)&pointer_array)[x];
    //     cpy tmp5 (6386)
    cmp(&flags, y, tmp5);
    //     bcc ca9f1 (6387)
    if (!(flags & FLAG_C))
        goto ca9f1;
    //     bne ca9d1 (6388)
    if (!(flags & FLAG_Z))
        goto ca9d1;
    //     cmp tmp4 (6389)
    if (a < tmp4)
        goto ca9f1;
    //     bcc ca9f1 (6390)
    // ca9d1: (6391)
ca9d1:
    //     cpy tmp9 (6392)
    cmp(&flags, y, tmp9);
    //     bcc ca9db (6393)
    if (!(flags & FLAG_C))
        goto ca9db;
    //     bne ca9e7 (6394)
    if (!(flags & FLAG_Z))
        goto ca9e7;
    //     cmp tmp8 (6395)
    if (a >= tmp8)
        goto ca9e7;
    //     bcs ca9e7 (6396)
    // ca9db: (6397)
ca9db:
    //     cpx #12 (6398)
    if (x >= 12)
        goto ca9e7;
    //     bcs ca9e7 (6399)
    //     lda #0 (6400)
    a = 0;
    //     sta __begin_pointer_array+0,x (6401)
    ((uint8_t*)&pointer_array)[x] = a;
    //     sta __begin_pointer_array+1,x (6402)
    ((uint8_t*)&pointer_array)[x + 1] = a;
    //     beq ca9f1 (6403) ALWAYS branch
    goto ca9f1;

    // ca9e7: (6405)
ca9e7:
    //     sbc tmp6 (6406)
    a = sbc(&flags, a, tmp6);
    //     sta __begin_pointer_array+0,x (6407)
    ((uint8_t*)&pointer_array)[x] = a;
    //     lda __begin_pointer_array+1,x (6408)
    a = ((uint8_t*)&pointer_array)[x + 1];
    //     sbc tmp7 (6409)
    a = sbc(&flags, a, tmp7);
    //     sta __begin_pointer_array+1,x (6410)
    ((uint8_t*)&pointer_array)[x + 1] = a;
    // ca9f1: (6411)
ca9f1:
    //     inx (6412)
    x++;
    //     inx (6413)
    x++;
    //     cpx #22 (6414)
    cmp(&flags, x, sizeof(pointer_array));
    //     bne ca9c3 (6415)
    if (!(flags & FLAG_Z))
        goto ca9c3;
    // loop_ca9f7: (6416)
loop_ca9f7:
    //     ldy #0 (6417)
    y = 0;
    // loop_ca9f9: (6418)
loop_ca9f9:
    //     lda (tmp8),y (6419)
    a = ram[tmp89 + y];
    //     sta (tmp2),y (6420)
    ram[tmp23 + y] = a;
    //     beq caa08 (6421)
    if (a == 0)
        goto caa08;
    //     iny (6422)
    y++;
    //     bne loop_ca9f9 (6423)
    if (y != 0)
        goto loop_ca9f9;
    //     inc tmp3 (6424)
    tmp3++;
    //     inc tmp9 (6425)
    tmp9++;
    //     bne loop_ca9f7 (6426)
    if (tmp9 != 0)
        goto loop_ca9f7;
    // caa08: (6427)
caa08:
    //     tya (6428)
    a = y;
    //     clc (6429)
    flags &= ~FLAG_C;
    //     adc tmp2 (6430)
    a = adc(&flags, a, tmp2);
    //     sta top (6431)
    top = (top & 0xff00) | a;
    //     lda tmp3 (6432)
    a = tmp3;
    //     adc #0 (6433)
    a = adc(&flags, a, 0);
    //     sta top+1 (6434)
    top = (top & 0x00ff) | ((uint16_t)a << 8);
    //     rts (6435)
}

static void advance_to_next_line(void)
{
    // advance_to_next_line
    //  Inputs: -
    //  Temps:  tmp01
    //  Outputs: a, y
    // c9a8d: Advance to next line in document
    // Sets Z from l007e on return (like c9aa5 does)

    //     jsr c9e94
    c9e94();
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     ldy current_line_ptr+1
    y = (uint8_t)((current_line_ptr >> 8) & 0xff);
    //     jsr sub_cab1a
    sub_cab1a();
    //     sec
    flags |= FLAG_C;
    //     beq c9aa5
    if (flags & FLAG_Z)
        goto c9aa5_;
    //     tya
    a = y;
    //     clc
    flags &= ~FLAG_C;
    //     adc tmp0
    a = adc(&flags, a, tmp0);
    //     sta current_line_ptr
    current_line_ptr = (current_line_ptr & 0xff00) | a;
    //     bcc c9aa4
    if (!(flags & FLAG_C))
        goto c9aa4_;
    //     inc current_line_ptr+1
    current_line_ptr = (current_line_ptr & 0x00ff) |
                       ((uint16_t)((current_line_ptr >> 8) + 1) << 8);
    // c9aa4:
c9aa4_:
    //     clc
    flags &= ~FLAG_C;
    // c9aa5:
c9aa5_:
    //     clv
    flags &= ~FLAG_V;
    //     lda l007e
    a = l007e;
    set_flags(&flags, a);
}

[[nodiscard]] static bool sub_c9aa9(void)
{
    // Pseudocode: Completes line formatting: adjusts pointers updates ruler
    // stack Returns: true if write failed (V=1, caller should return
    // immediately)

    // sub_c9aa9:
    //     sec
    flags |= FLAG_C;
    //     rol l007e
    l007e = rol(&flags, l007e);
    //     ldy l0047
    y = l0047;
    //     dey
    y--;
    //     sty l003b
    l003b = y;
    //     inc l006e
    edit_buffer_unpacked_flag++;
    //     jsr write_line_back_to_document
    write_line_back_to_document();
    //     bcc return_50
    if (!(flags & FLAG_C))
        return false;
    //     pla (pop sub_c9aa9's return address)
    //     pla (pop sub_c9977's return address — stack unwind)
    //     lda #0x40 ; '@'
    a = 0x40;
    //     sta l0084
    l0084 = a;
    //     bit l0084   ; sets V
    bit(&flags, a, l0084);
    //     rts (return to sub_c9977's caller with V=1, bypassing sub_c9977's
    //     clv)
    return true;
}

void beep(void)
{
    // beep: Emits a beep (returns with carry set as flag)

    //     lda #7
    a = 7;
    //     jsr oswrch
    cli_putchar(a);
    // loop_caced:
    //     sec
    flags |= FLAG_C;
    // return_74:
    //     rts
}

static void c8b78(void)
{
    // c8b78:
    //     lda #0xff
    a = 0xff;
    set_flags(&flags, a);
    //     rts
}

void c8b7b(void)
{
    // c8b7b
    //  Inputs: -
    //  Outputs: a, x, y; tmp89
    // c8b7b:
    //     lda l007a
    a = l007a;
    //     beq c8b78
    if (a == 0)
    {
        c8b78();
        return;
    }
    //     lda #0x14
    a = 0x14;
    //     sta l0048
    l0048 = a;
    //     ldx #0
    x = 0;
    //     stx l0049
    l0049 = x;
    //     stx l0081
    l0081 = x;
    //     lda doc_ptr2+0
    a = (uint8_t)(doc_ptr2 & 0xff);
    //     sta tmp8
    tmp8 = a;
    //     lda doc_ptr2+1
    a = (uint8_t)(doc_ptr2 >> 8);
    //     sta tmp9
    tmp9 = a;
c8b91:
    // c8b91:
    //     lda tmp9
    a = tmp9;
    //     cmp doc_ptr3+1
    cmp(&flags, a, (uint8_t)(doc_ptr3 >> 8));
    //     bcc c8b9f
    if (!(flags & FLAG_C))
        goto c8b9f;
    //     bne c8b78
    if (!(flags & FLAG_Z))
    {
        c8b78();
        return;
    }
    //     lda tmp8
    a = tmp8;
    //     cmp doc_ptr3+0
    cmp(&flags, a, (uint8_t)(doc_ptr3 & 0xff));
    //     bcs c8b78
    if (flags & FLAG_C)
    {
        c8b78();
        return;
    }
c8b9f:
    // c8b9f:
    //     ldy #0
    y = 0;
    //     lda (tmp8),y
    a = ram[tmp89];
    //     jsr check_for_command_prefix
    flags = check_for_command_prefix(a);
    //     bne c8bb7
    if (!(flags & FLAG_Z))
        goto c8bb7;
    //     lda tmp8
    a = tmp8;
    //     clc
    flags &= ~FLAG_C;
    //     adc #3
    flags &= ~FLAG_C;
    a = adc(&flags, a, 3);
    //     sta doc_ptr2+0
    doc_ptr2 = (doc_ptr2 & 0xff00) | a;
    //     lda tmp9
    a = tmp9;
    //     sta doc_ptr2+1
    doc_ptr2 = (doc_ptr2 & 0x00ff) | ((uint16_t)a << 8);
    //     bcc c8b7b
    if (!(flags & FLAG_C))
    {
        c8b7b();
        return;
    }
    //     bcs c8bdf
    goto c8bdf;

c8bb7:
    // c8bb7:
    //     jsr sub_c8c5f
    sub_c8c5f();
    //     sta l0083
    l0083 = a;
c8bbc:
    // c8bbc:
    //     iny
    y++;
    //     lda (tmp8),y
    a = ram[tmp89 + y];
    //     beq c8bdb
    if (a == 0)
        goto c8bdb;
    //     jsr check_for_command_prefix
    flags = check_for_command_prefix(a);
    //     beq c8bdb
    if (flags & FLAG_Z)
        goto c8bdb;
    //     lda header_text_maybe,x
    a = header_text_maybe[x];
    //     cmp #0x20 ; ' '
    if (a == 0x20)
        goto c8bf7;
    //     beq c8bf7
    //     cmp #1
    if (a == 1)
        goto c8be3;
    //     beq c8be3
    //     cmp #2
    if (a != 2)
        goto c8bd7;
    //     bne c8bd7
    //     lda #0x20 ; ' '
    a = 0x20;
c8bd7:
    // c8bd7:
    //     cmp l0083
    if (a == l0083)
        goto c8c33;
    //     beq c8c33
c8bdb:
    // c8bdb:
    //     inc doc_ptr2+0
    doc_ptr2++;
    //     bne c8b7b
    if ((uint8_t)(doc_ptr2 & 0xff) != 0)
    {
        c8b7b();
        return;
    }
c8bdf:
    // c8bdf:
    //     inc doc_ptr2+1
    //     bne c8b7b
    if (doc_ptr2 != 0)
    {
        c8b7b();
        return;
    }
    a = 0xff;
    set_flags(&flags, a);
    return;
c8be3:
    // c8be3:
    //     lda l0083
    a = l0083;
    //     stx l0084
    l0084 = x;
    //     ldx l0049
    x = l0049;
    //     cpx #0x14
    if (x >= 0x14)
        goto c8bf2;
    //     bcs c8bf2
    //     sta output_buffer,x
    output_buffer[x] = a;
    //     inc l0049
    l0049++;
c8bf2:
    // c8bf2:
    //     ldx l0084
    x = l0084;
    //     jmp c8c33
    goto c8c33;

c8bf7:
    // c8bf7:
    //     stx l0084
    l0084 = x;
    //     lda l0083
    a = l0083;
    //     cmp #0x20 ; ' '
    if (a == 0x20)
        goto c8c23;
    //     beq c8c23
    //     cmp #9
    if (a == 9)
        goto c8c23;
    //     beq c8c23
    //     cmp #0x0b
    if (a == 0x0b)
        goto c8c23;
    //     beq c8c23
    //     cmp #0x1a
    if (a == 0x1a)
        goto c8c23;
    //     beq c8c23
    //     cmp #0x0d
    if (a == 0x0d)
        goto c8c23;
    //     beq c8c23
    //     lda l0081
    a = l0081;
    //     beq c8bdb
    if (a == 0)
        goto c8bdb;
    //     jsr sub_c8c51
    sub_c8c51();
    //     lda #0
    a = 0;
    //     sta l0081
    l0081 = a;
    //     ldx l0084
    x = l0084;
    //     inx
    x++;
    //     cpx l007a
    if (x < l007a)
        goto c8bbc;
    //     bcc c8bbc
    //     bcs c8c3e
    goto c8c3e;

c8c23:
    // c8c23:
    //     jsr sub_c8c53
    sub_c8c53();
    //     ldx l0084
    x = l0084;
    //     sta l0081
    l0081 = a;
loop_c8c2a:
    // loop_c8c2a:
    //     inc tmp8
    tmp8++;
    if (tmp8 != 0)
        goto c8c30;
    //     bne c8c30
    //     inc tmp9
    tmp9++;
c8c30:
    // c8c30:
    //     jmp c8b91
    goto c8b91;

c8c33:
    // c8c33:
    //     inx
    x++;
    //     cpx l007a
    if (x < l007a)
        goto loop_c8c2a;
    //     bcc loop_c8c2a
    //     inc tmp8
    tmp8++;
    if (tmp8 != 0)
        goto c8c3e;
    //     bne c8c3e
    //     inc tmp9
    tmp9++;
c8c3e:
    // c8c3e:
    //     lda doc_ptr2+0
    a = (uint8_t)(doc_ptr2 & 0xff);
    //     ldy doc_ptr2+1
    y = (uint8_t)(doc_ptr2 >> 8);
    //     ldx tmp8
    x = tmp8;
    //     stx doc_ptr2+0
    doc_ptr2 = (doc_ptr2 & 0xff00) | x;
    //     ldx tmp9
    x = tmp9;
    //     stx doc_ptr2+1
    doc_ptr2 = (doc_ptr2 & 0x00ff) | ((uint16_t)x << 8);
    //     sta ptr2
    ptr2 = (ptr2 & 0xff00) | a;
    //     sty ptr2+1
    ptr2 = (ptr2 & 0x00ff) | ((uint16_t)y << 8);
    //     ldx #0
    x = 0;
    set_flags(&flags, 0);
    //     rts
}

static void c9de3_insert_line(void)
{
    //     sta tmp4
    tmp45 = (addr_t)(y) << 8 | a;
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
    if (flags & FLAG_C)
        goto c9dfd;
    //     lda #0x0d
    a = 0x0d;
    //     ldy #0
    y = 0;
    //     sta (tmp4),y
    ram[tmp45 + y] = a;
    //     jmp ca741
    ca741();
    return;

c9dfd:
    //     jmp ca941
    memory_full();
    return;
}

static void c9e94(void)
{
    // c9e94:
    //     lda #0
    a = 0;
    //     sta xpos
    xpos = a;
    //     rts
}

static void ca684(void)
{
    // Pseudocode: Sets line_lengths[ypos] = screen_width after cursor movement

    // ca684:
    //     ldx ypos
    x = ypos;
    //     lda screen_width
    a = screen_maxcolumn;
    //     sta line_lengths,x
    line_lengths[x] = a;
    //     rts
    return;
}
void (*number_callback)(void);

void ca741(void)
{
    // ca741: Updates ptr6 to current_line_ptr if ptr6 is ahead, sets refresh
    // flags On entry: current_line_ptr, ptr6 On exit:  ptr6 = min(ptr6,
    // current_line_ptr), l0073 = l003d = 0xff Uses: x, y

    //     ldx current_line_ptr
    x = (uint8_t)(current_line_ptr & 0xff);
    //     ldy current_line_ptr+1
    y = (uint8_t)(current_line_ptr >> 8);
    //     cpy ptr6+1
    //     bcc ca74f
    //     bne ca753
    //     cpx ptr6
    //     bcs ca753
    if (y < (uint8_t)(ptr6 >> 8) ||
        (y == (uint8_t)(ptr6 >> 8) && x < (uint8_t)(ptr6 & 0xff)))
    {
        // ca74f:
        //     stx ptr6
        //     sty ptr6+1
        ptr6 = current_line_ptr;
    }
    // ca753:
    //     ldx #0xff
    x = 0xff;
    //     stx l0073
    l0073 = x;
    //     stx l003d
    l003d = x;
    //     rts
}

void clear_screen(void)
{
    // Pseudocode: Clears the screen via SCREEN call

    // ;
    // ***************************************************************************************
    // clear_screen:
    //     ldy #SCREEN_CLEAR
    //     jmp SCREEN
    screen_clear();
    return;
}

static void clear_to_eol(void)
{
    // Pseudocode: Fills remaining space on line with spaces to clear to end

    // sub_ca597:
    //     ldx l0082
    x = l0082;
    //     sta l0084
    l0084 = a;
    //     lda line_lengths,x
    a = line_lengths[x];
    set_flags(&flags, a);
    //     beq return_62
    if (flags & FLAG_Z)
        goto return_62;
    //     lda l0084
    a = l0084;
    // loop_ca5a2:
loop_ca5a2:
    //     jsr screen_putchar
    screen_putchar(a);
    //     dec line_lengths,x
    line_lengths[x]--;
    set_flags(&flags, line_lengths[x]);
    //     bne loop_ca5a2
    if (!(flags & FLAG_Z))
        goto loop_ca5a2;
    // return_62:
return_62:
    //     rts
    return;
}
// Input:  a = document character, y = line offset (for tab stop lookup)
// Output: a = character to render, x = screen width consumed, y preserved,
// flags.C=0

static void cursor_off(void)
{
    // Pseudocode: Disables cursor display via SCREEN driver

    // cursor_off:
    //     lda #0
    //     jmp SCREEN_SHOWCURSOR
    screen_enablecursor(0);
}

static void cursor_on(void)
{
    // Pseudocode: Enables cursor display via SCREEN driver

    // cursor_on:
    //     lda #1
    //     jmp SCREEN_SHOWCURSOR
    screen_enablecursor(1);
}

void draw_line(uint16_t addr)
{
    // draw_line
    //  Outputs: a, y; tmp01, tmp67
    // draw_line: Renders a single document line to the screen

    //     sta tmp0
    tmp01 = addr;
    tmp67 = addr;
    //     ldx #0
    //     ldy l0082
    screen_setcursor(0, l0082);
    //     ldy #0
    y = 0;
    //     sty l0083
    l0083 = 0;
    //     sty input_buffer_offset+1
    l0080 = 0;
    //     sty l0039
    l0039 = 0;
    //     jsr deref_and_check_for_command_prefix
    flags = deref_and_check_for_command_prefix();
    //     bne ca4b4
    if (!(flags & FLAG_Z))
        goto ca4b4;
    //     ldy #3
    y = 3;
    //     lda hscroll_pos
    a = hscroll_pos;
    if (a != 0)
        goto ca4b4;
    //     bne ca4b4
    //     ldy #1
    y = 1;
    //     jsr sub_ca4d7
    sub_ca4d7();
    //     jsr sub_ca4d7
    sub_ca4d7();
    //     lda #0x20 ; ' '
    a = 0x20;
    //     bne ca4bc
    goto ca4bc;

    // ca4b4:
ca4b4:
    //     lda #0x20 ; ' '
    a = 0x20;
    //     jsr ca4e9
    render_char();
    //     jsr ca4e9
    render_char();
// ca4bc:
ca4bc:
    //     jsr ca4e9
    render_char();
// loop_ca4bf:
loop_ca4bf:
    //     jsr process_current_document_character
    process_current_document_character();
    // loop_ca4c2:
loop_ca4c2:
    //     jsr render_xchar
    render_xchar();
    //     dex
    x--;
    //     bne loop_ca4c2
    if (x != 0)
        goto loop_ca4c2;
    //     cmp #0x0d
    if (a != 0x0d)
        goto loop_ca4bf;
    //     bne loop_ca4bf
    //     lda #0x20 ; ' '
    a = 0x20;
    //     jsr sub_ca597
    clear_to_eol();
    //     lda l0083
    a = l0083;
    //     sta line_lengths,x
    line_lengths[x] = a;
    //     rts
}

void draw_prompt_characters(void)
{
    // draw_prompt_characters: Draws two inverted prompt characters at top-left
    // On entry: x, y = prompt characters
    // Uses: tmp2, tmp3
    // On exit: cursor position restored

    //     stx tmp2
    tmp23 = (addr_t)(y) << 8 | x;
    //     jsr save_cursor_position
    save_cursor_position();
    //     jsr cursor_off
    cursor_off();
    //     jsr home_cursor
    home_cursor();
    //     jsr set_inverted_text_if_not_mode_7
    a = STYLE_REVERSE;
    screen_setstyle(a);
    //     lda tmp2
    a = (uint8_t)tmp2;
    //     jsr screen_putchar
    screen_putchar(a);
    //     lda tmp3
    a = (uint8_t)tmp3;
    //     jsr screen_putchar
    screen_putchar(a);
    //     jsr set_normal_text_if_not_mode_7
    a = 0;
    screen_setstyle(a);
    //     lda #0x20 ; ' '
    a = 0x20;
    //     jsr screen_putchar
    screen_putchar(a);
    //     jsr restore_cursor_position
    restore_cursor_position();
    // cursor_on:
    // cursor_off:
    //     rts
}

static void draw_ruler(void)
{
    // Pseudocode: Displays ruler status word at top of screen if
    // status_line_needs_redrawing_flag is set

    // ;
    // ***************************************************************************************
    // draw_ruler:
    a = status_line_needs_redrawing_flag;
    flags = (flags & ~(FLAG_Z | FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq return_64
    if (flags & FLAG_Z)
        return;
    //     ldy #0
    y = 0;
    //     sty status_line_needs_redrawing_flag
    status_line_needs_redrawing_flag = y;

    //     sty l0082
    l0082 = y;
    draw_line(current_ruler_ptr);
    // The 6502 never sets flags_need_redrawing_flag in the scroll/redraw path,
    // so draw_status_word is not called after draw_ruler.  This would leave
    // three spaces at columns 0-2 (draw_line writes a 3-byte prefix for
    // non-0x80 lines).  Re-enable the status-word refresh here.
    flags_need_redrawing_flag = 1;
    //     rts
    return;
}

static void draw_status_word(void)
{
    // draw_status_word
    //  Outputs: a, x
    // Pseudocode: Redraws status line showing format mode, justify, and insert
    // indicators

    // sub_ca651:
    //     lda #0
    a = 0;
    //     sta flags_need_redrawing_flag
    flags_need_redrawing_flag = a;
    //     jsr home_cursor
    home_cursor();
    //     ldx #0x46 ; 'F'
    x = 0x46;
    //     lda format_mode_flag
    a = format_mode_flag;
    flags = (flags & ~(FLAG_Z | FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     beq ca666
    if (flags & FLAG_Z)
        goto ca666;
    //     ldx #0x4d ; 'M'
    x = 0x4d;
    //     and #0xc0
    a &= 0xc0;
    flags = (flags & ~(FLAG_Z | FLAG_N)) | (a == 0 ? FLAG_Z : 0) | (a & FLAG_N);
    //     bne ca666
    if (!(flags & FLAG_Z))
        goto ca666;
    //     ldx #0x20 ; ' '
    x = 0x20;
    // ca666:
ca666:
    //     txa
    a = x;
    //     jsr screen_putchar
    screen_putchar(a);
    //     lda #0x4a ; 'J'
    a = 0x4a;
    //     ldx justifying_flag
    x = justifying_flag;
    flags = (flags & ~(FLAG_Z | FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     beq ca672
    if (flags & FLAG_Z)
        goto ca672;
    //     lda #0x20 ; ' '
    a = 0x20;
    // ca672:
ca672:
    //     jsr screen_putchar
    screen_putchar(a);
    //     lda #0x49 ; 'I'
    a = 0x49;
    //     ldx insert_mode_flag
    x = insert_mode_flag;
    flags = (flags & ~(FLAG_Z | FLAG_N)) | (x == 0 ? FLAG_Z : 0) | (x & FLAG_N);
    //     bne ca681
    if (!(flags & FLAG_Z))
    {
        home_cursor();
        return;
    }
    //     lda #0x20 ; ' '
    a = 0x20;
    //     bne ca681                                                         ;
    //     ALWAYS branch
    home_cursor();
    return;
}

static void get_line_length(void)
{
    // get_line_length
    //  Outputs: a, y
    // Pseudocode: Returns the length of the current edit line

    // ;
    // ***************************************************************************************
    // get_line_length:
    //     ldy #0
    y = 0;
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    //     jsr check_for_command_prefix
    flags = check_for_command_prefix(a);
    //     php
    {
        uint8_t saved_f = flags;
        //     ldy #0x84
        y = MAX_LINE_LENGTH;
        // loop_caafb:
    loop_caafb:
        //     dey
        y--;
        //     lda (current_edit_line_ptr),y
        a = ram[current_edit_line_ptr + y];
        //     cmp #0x10
        if (a != 0x10)
            goto cab06;
        //     bne cab06
        //     tya
        a = y;
        if (a != 0)
            goto loop_caafb;
        //     bne loop_caafb
        //     dey
        y--;
        // cab06:
    cab06:
        //     iny
        y++;
        //     tya
        a = y;
        //     plp
        flags = saved_f;
    }
    //     bne return_69
    if (!(flags & FLAG_Z))
        goto return_69;
    //     clc
    //     adc #3
    a += 3;
    // return_69:
return_69:
    //     rts
    return;
}

static void go_to_marker(void)
{
    // go_to_marker:
    //     lda markers_array,x
    a = ((uint8_t*)markers_array)[x];
    //     ldy markers_array+1,x
    y = ((uint8_t*)markers_array)[x + 1];
    //     jsr move_cursor_to_address
    move_cursor_to_address();
    // ca035:
    //     lda #1
    a = 1;
    //     sta l0073
    l0073 = a;
    //     jmp ca684
    ca684();
    return;
}

static void go_to_marker_n(void)
{
    //     pha
    uint8_t saved_a = a;
    //     jsr ca93c
    write_line_back_to_document_safely();
    //     pla
    a = saved_a;
    //     jsr lookup_marker
    lookup_marker();
    //     jmp go_to_marker
    go_to_marker();
    return;
}

static void home_cursor(void)
{
    // home_cursor
    //  Inputs: y
    //  Temps:  tmp23, tmp45, tmp67
    //  Outputs: a, x; tmp89
    // home_cursor:
    // ca681:
    //     ldx #0
    //     ldy #0
    //     jmp set_cursor_position
    screen_setcursor(0, 0);
    return;
}

void justify_edit_buffer(void)
{
    // Pseudocode: Word-spacing justification: distributes extra spaces between
    // words

    // justify_edit_buffer:
    //     lda justifying_flag
    a = justifying_flag;
    if (a != 0)
        return;
    //     bne return_47
    //     sta l0046
    l0046 = a;
    //     sta l0039
    l0039 = a;
    //     sta l0042
    l0042 = a;
    //     lda ruler_right_stop
    a = ruler_right_stop;
    set_flags(&flags, a);
    //     beq return_47
    if (flags & FLAG_Z)
        return;
    //     jsr get_line_length
    get_line_length();
    //     sty l0043
    l0043 = y;
    //     ldy #0
    y = 0;
    //     beq c9861                                                         ;
    //     ALWAYS branch
    goto c9861;

    // c9847:
c9847:
    //     lda l0039
    a = l0039;
    //     sta l0084
    l0084 = a;
    //     iny
    y++;
    //     cpy l0043
    cmp(&flags, y, l0043);
    //     beq c9871
    if (flags & FLAG_Z)
        goto c9871;
    //     clc
    flags &= ~FLAG_C;
    //     jsr sub_c9936
    sub_c9936();
    //     beq c985c
    if (flags & FLAG_Z)
        goto c985c;
    //     cmp #0x20 ; ' '
    if (a != 0x20)
        goto c9847;
    //     bne c9847
    //     inc l0046
    l0046++;
    // c985c:
c985c:
    //     iny
    y++;
    //     cpy l0043
    cmp(&flags, y, l0043);
    //     beq c986d
    if (flags & FLAG_Z)
        goto c986d;
    // c9861:
c9861:
    //     sec
    flags |= FLAG_C;
    //     jsr sub_c9936
    sub_c9936();
    //     beq c985c
    if (flags & FLAG_Z)
        goto c985c;
    //     cmp #0x20 ; ' '
    cmp(&flags, a, 0x20);
    //     bne c9847
    if (!(flags & FLAG_Z))
        goto c9847;
    //     beq c985c                                                         ;
    //     ALWAYS branch
    goto c985c;

    // c986d:
c986d:
    //     dec l0046
    l0046--;
    set_flags(&flags, l0046);
    //     bmi return_47
    if (flags & FLAG_N)
        return;
    // c9871:
c9871:
    //     lda l0046
    a = l0046;
    set_flags(&flags, a);
    //     beq return_47
    if (flags & FLAG_Z)
        return;
    //     lda ruler_right_stop
    a = ruler_right_stop;
    //     sec
    flags |= FLAG_C;
    //     sbc l0084
    a = sbc(&flags, a, l0084);
    //     bcc return_47
    if (!(flags & FLAG_C))
        return;
    //     adc #0
    a = adc(&flags, a, 0);
    //     tax
    x = a;
    //     adc l0043
    a = adc(&flags, a, l0043);
    //     sec
    flags |= FLAG_C;
    //     sbc #0x84
    a = sbc(&flags, a, MAX_LINE_LENGTH);
    //     bcc c988c
    if (!(flags & FLAG_C))
        goto c988c;
    //     sta l0084
    l0084 = a;
    //     txa
    a = x;
    //     sbc l0084
    a = sbc(&flags, a, l0084);
    //     tax
    x = a;
    // c988c:
c988c:
    //     stx l0082
    l0082 = x;
    //     stx tmp8
    tmp8 = x;
    //     lda #0
    a = 0;
    //     sta tmp9
    tmp9 = a;
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
    //     ldx l0046
    x = l0046;
    //     tya                                                               ;
    //     A=0x00
    a = y;
    // loop_c98a2:
    //     sta input_buffer,y
loop_c98a2:
    input_buffer[y] = a;
    //     iny
    y++;
    //     dex
    x--;
    //     bne loop_c98a2
    if (x != 0)
        goto loop_c98a2;
    //     ldy print_xpos
    y = print_xpos;
    //     iny
    y++;
    //     cpy l0046
    if (y < l0046)
        goto c98b2;
    //     bcc c98b2
    //     ldy #1
    y = 1;
    // c98b2:
c98b2:
    //     dey
    y--;
    //     ldx l0046
    x = l0046;
    // c98b5:
c98b5:
    //     lda l0045
    a = l0045;
    set_flags(&flags, a);
    //     beq c98bd
    if (flags & FLAG_Z)
        goto c98bd;
    //     lda #1
    a = 1;
    //     dec l0045
    l0045--;
    // c98bd:
c98bd:
    //     clc
    flags &= ~FLAG_C;
    //     adc l0044
    a = adc(&flags, a, l0044);
    //     sta input_buffer,y
    input_buffer[y] = a;
    //     lda l0082
    a = l0082;
    //     sec
    flags |= FLAG_C;
    //     sbc input_buffer,y
    a = sbc(&flags, a, input_buffer[y]);
    //     php
    {
        uint8_t saved_flags = flags;
        //     sta l0082
        l0082 = a;
        //     iny
        y++;
        //     cpy l0046
        cmp(&flags, y, l0046);
        //     bcc c98d3
        if (!(flags & FLAG_C))
            goto c98d3;
        //     ldy #0
        y = 0;
        // c98d3:
    c98d3:
        //     plp
        flags = saved_flags;
    }
    //     beq c98d9
    if (flags & FLAG_Z)
        goto c98d9;
    //     dex
    x--;
    //     bne c98b5
    if (x != 0)
        goto c98b5;
    // c98d9:
c98d9:
    //     sty print_xpos
    print_xpos = y;
    //     ldy #0
    y = 0;
    //     sty l0081
    l0081 = y;
    //     sty l0039
    l0039 = y;
    //     lda #0x1a
    a = 0x1a;
    //     jsr wipe_buffer
    wipe_buffer();
    //     lda l0042
    a = l0042;
    set_flags(&flags, a);
    //     beq c98f6
    if (flags & FLAG_Z)
        goto c98f6;
    //     ldy #0
    y = 0;
    // loop_c98ec:
    //     lda output_buffer,y
loop_c98ec:
    a = output_buffer[y];
    //     sta (current_edit_line_ptr),y
    ram[current_edit_line_ptr + y] = a;
    //     iny
    y++;
    //     cpy l0042
    if (y != l0042)
        goto loop_c98ec;
    //     bne loop_c98ec
    // c98f6:
c98f6:
    //     ldy l0042
    y = l0042;
    //     ldx l0042
    x = l0042;
    // c98fa:
c98fa:
    //     lda output_buffer,x
    a = output_buffer[x];
    //     cmp #0x20 ; ' '
    if (a != 0x20)
        goto c9920;
    //     bne c9920
    //     lda l0081
    a = l0081;
    set_flags(&flags, a);
    //     beq c991c
    if (flags & FLAG_Z)
        goto c991c;
    //     sty l0084
    l0084 = y;
    //     ldy l0039
    y = l0039;
    //     cpy l0046
    cmp(&flags, y, l0046);
    //     lda #0
    a = 0;
    //     bcs c9912
    if (flags & FLAG_C)
        goto c9912;
    //     lda input_buffer,y
    a = input_buffer[y];
    // c9912:
c9912:
    //     clc
    //     adc l0084
    a += l0084;
    //     inc l0039
    l0039++;
    //     tay
    y = a;
    //     lda #0
    a = 0;
    //     sta l0081
    l0081 = a;
    // c991c:
c991c:
    //     lda #0x20 ; ' '
    a = 0x20;
    //     bne c9922                                                         ;
    //     ALWAYS branch
    goto c9922;

    // c9920:
c9920:
    //     inc l0081
    l0081++;
    // c9922:
c9922:
    //     sta (current_edit_line_ptr),y
    ram[current_edit_line_ptr + y] = a;
    //     iny
    y++;
    //     inx
    x++;
    //     cpx l0043
    if (x != l0043)
        goto c98fa;
    //     bne c98fa
    //     lda #0x10
    a = 0x10;
    // loop_c992c:
    //     cpy #0x84
loop_c992c:
    cmp(&flags, y, MAX_LINE_LENGTH);
    //     bcs return_48
    if (flags & FLAG_C)
        return;
    //     sta (current_edit_line_ptr),y
    ram[current_edit_line_ptr + y] = a;
    //     iny
    y++;
    //     bne loop_c992c
    goto loop_c992c;
    // return_48:
return_48:
    //     rts
    return;
}

void make_space_for_insertion(void)
{
    uint8_t tmp2, tmp3, tmp8, tmp9;
    // make_space_for_insertion: Shifts content up to make space for insertion
    // (6437) On entry: tmp4:tmp5 = block base, tmp6:tmp7 = size, top = current
    // top On exit:  top += size, pointer_array entries >= base adjusted, block
    // shifted Uses: tmp2, tmp3, tmp8, tmp9

    //     lda top (6438)
    a = (uint8_t)(top & 0xff);
    //     sta tmp2 (6439)
    tmp2 = a;
    //     clc (6440)
    flags &= ~FLAG_C;
    //     adc tmp6 (6441)
    a = adc(&flags, a, tmp6);
    //     sta tmp8 (6442)
    tmp8 = a;
    //     tax (6443)
    x = a;
    //     lda top+1 (6444)
    a = (uint8_t)(top >> 8);
    //     sta tmp3 (6445)
    tmp3 = a;
    //     adc tmp7 (6446)
    a = adc(&flags, a, tmp7);
    //     sta tmp9 (6447)
    tmp9 = a;
    //     tay (6448)
    y = a;
    //     cpy himem+1 (6449)
    cmp(&flags, y, (uint8_t)(himem >> 8));
    //     bcc caa32 (6450)
    if (!(flags & FLAG_C))
        goto caa32;
    //     bne return_67 (6451)
    if (!(flags & FLAG_Z))
        goto return_67;
    //     cpx himem (6452)
    cmp(&flags, x, (uint8_t)(himem & 0xff));
    //     bcs return_67 (6453)
    if (flags & FLAG_C)
        goto return_67;
    // caa32: (6454)
caa32:
    //     stx top (6455) sty top+1 (6456)
    top = (uint16_t)y << 8 | x;
    //     ldx #0 (6457)
    x = 0;
    // loop_caa38: (6458)
loop_caa38:
    //     ldy __begin_pointer_array+1,x (6459)
    y = ((uint8_t*)&pointer_array)[x + 1];
    //     lda __begin_pointer_array+0,x (6460)
    a = ((uint8_t*)&pointer_array)[x];
    //     cpy tmp5 (6461)
    cmp(&flags, y, tmp5);
    //     bcc caa51 (6462)
    if (!(flags & FLAG_C))
        goto caa51;
    //     bne caa46 (6463)
    if (!(flags & FLAG_Z))
        goto caa46;
    //     cmp tmp4 (6464)
    if (a < tmp4)
        goto caa51;
    //     bcc caa51 (6465)
    // caa46: (6466)
caa46:
    //     clc (6467)
    flags &= ~FLAG_C;
    //     adc tmp6 (6468)
    a = adc(&flags, a, tmp6);
    //     sta __begin_pointer_array+0,x (6469)
    ((uint8_t*)&pointer_array)[x] = a;
    //     lda __begin_pointer_array+1,x (6470)
    a = ((uint8_t*)&pointer_array)[x + 1];
    //     adc tmp7 (6471)
    a = adc(&flags, a, tmp7);
    //     sta __begin_pointer_array+1,x (6472)
    ((uint8_t*)&pointer_array)[x + 1] = a;
    // caa51: (6473)
caa51:
    //     inx (6474)
    x++;
    //     inx (6475)
    x++;
    //     cpx #22 (6476)
    cmp(&flags, x, sizeof(pointer_array));
    //     bne loop_caa38 (6477)
    if (!(flags & FLAG_Z))
        goto loop_caa38;
    // caa57: (6478)
caa57:
    //     lda tmp2 (6479)
    a = tmp2;
    //     sec (6480)
    flags |= FLAG_C;
    //     sbc tmp4 (6481)
    a = sbc(&flags, a, tmp4);
    //     tax (6482)
    x = a;
    //     lda tmp3 (6483)
    a = tmp3;
    //     sbc tmp5 (6484)
    a = sbc(&flags, a, tmp5);
    //     beq caa65 (6485)
    if (flags & FLAG_Z)
        goto caa65;
    //     ldx #0xff (6486)
    x = 0xff;
    // caa65: (6487)
caa65:
    //     txa (6488)
    a = x;
    //     tay (6489)
    y = a;
    //     iny (6490)
    y++;
    //     lda tmp2 (6491)
    a = tmp2;
    //     stx tmp2 (6492)
    tmp2 = x;
    //     sec (6493)
    flags |= FLAG_C;
    //     sbc tmp2 (6494)
    a = sbc(&flags, a, tmp2);
    //     sta tmp2 (6495)
    tmp2 = a;
    //     bcs caa75 (6496)
    if (flags & FLAG_C)
        goto caa75;
    //     dec tmp3 (6497)
    tmp3--;
    // caa75: (6498)
caa75:
    //     lda tmp8 (6499)
    a = tmp8;
    //     stx tmp8 (6500)
    tmp8 = x;
    //     sec (6501)
    flags |= FLAG_C;
    //     sbc tmp8 (6502)
    a = sbc(&flags, a, tmp8);
    //     sta tmp8 (6503)
    tmp8 = a;
    //     bcs caa82 (6504)
    if (flags & FLAG_C)
        goto caa82;
    //     dec tmp9 (6505)
    tmp9--;
    // caa82: (6506)
caa82:
    //     dey (6507)
    y--;
    //     lda (tmp2),y (6508)
    a = ram[tmp23 + y];
    //     sta (tmp8),y (6509)
    ram[tmp89 + y] = a;
    //     tya (6510)
    a = y;
    //     bne caa82 (6511)
    if (a != 0)
        goto caa82;
    //     inx (6512)
    x++;
    //     beq caa57 (6513)
    if (x == 0)
        goto caa57;
    //     clc (6514)
    flags &= ~FLAG_C;
    // return_67: (6515)
return_67:
    //     rts (6516)
}

static void memory_full(void)
{
    // run_editor (ca941): Enter editor for memory-full condition.
    // jsr enter_editor_mode  -- NOT called here; entered via ca93c fall-through
    // ca941:
    //     ldx #0xff
    //     txs
    //     jsr sub_ca94a
    show_memory_full_error();
    //     jmp editor_loop
    longjmp(env, JMP_EDITOR);
}
// la995: "Memory full - Press ESCAPE"
static const uint8_t la995_data[] = "Memory full - Press ESCAPE";

void process_current_document_character(void)
{
    // draw_char:
    //     lda (tmp0),y
    a = ram[tmp01 + y];
    //     iny
    y++;
    process_document_character();
}

static void recalculate_cursor_xpos(void)
{
    // recalculate_cursor_xpos
    //  Inputs: x
    //  Outputs: a, y, flags:C, flags:Z; tmp01
    // Pseudocode: Recalculates cursor xpos from visual position accounting for
    // tabs and margins

    // sub_ca608:
    //     lda current_edit_line_ptr
    a = (uint8_t)(current_edit_line_ptr & 0xff);
    //     sta tmp0
    tmp0 = a;
    //     lda current_edit_line_ptr+1
    a = (uint8_t)(current_edit_line_ptr >> 8);
    //     sta tmp1
    tmp1 = a;
    //     lda l0079
    a = l0079;
    if (a != 0)
        goto ca624;
    //     bne ca624
    //     tay
    y = a;
    // loop_ca615:
loop_ca615:
    //     cpy xpos
    if (y == xpos)
        goto ca63d;
    //     beq ca63d
    //     sta l0039
    l0039 = a;
    //     jsr process_current_document_character
    process_current_document_character();
    //     txa
    a = x;
    //     clc
    flags &= ~FLAG_C;
    //     adc l0039
    a = adc(&flags, a, l0039);
    //     bcc loop_ca615
    if (!(flags & FLAG_C))
        goto loop_ca615;
    // ca624:
ca624:
    //     lda #0
    a = 0;
    //     sta l0079
    l0079 = a;
    //     tay                                                               ;
    //     Y=0x00
    y = a;
    // loop_ca629:
loop_ca629:
    //     sta l0039
    l0039 = a;
    //     jsr process_current_document_character
    process_current_document_character();
    //     txa
    a = x;
    //     clc
    //     adc l0039
    a += l0039;
    //     cmp l0072
    cmp(&flags, a, l0072);
    //     bcc loop_ca629
    if (!(flags & FLAG_C))
        goto loop_ca629;
    //     beq ca63b
    if (flags & FLAG_Z)
        goto ca63b;
    //     lda l0039
    a = l0039;
    //     dey
    y--;
    // ca63b:
ca63b:
    //     sty xpos
    xpos = y;
    // ca63d:
ca63d:
    //     sta l0072
    l0072 = a;
    // return_64:
return_64:
    //     rts
    return;
}

void redraw_editor(void)
{
    // redraw_editor
    //  Inputs: -
    //  Temps:  tmp01
    //  Ptrs:   ptr6
    //  Outputs: a, x, y, flags:C, flags:Z
    // Pseudocode: Main screen update routine: scrolls, redraws lines, updates
    // status and cursor
    uint8_t saved_status_line_needs_redrawing_flag;

    // redraw_editor:                                                    (5206)
    //     jsr cursor_off                                                (5207)
    cursor_off();
    //     lda ruler_stack_ptr                                           (5208)
    a = ruler_index_ptr;
    //     sta l0034                                                     (5209)
    l0034 = a;
    //     lda l0076                                                     (5210)
    a = status_line_needs_redrawing_flag;
    //     sta input_buffer_ptr+1                                        (5211)
    saved_status_line_needs_redrawing_flag = a;
    //     lda l006e                                                     (5212)
    a = edit_buffer_unpacked_flag;
    //     beq ca28e                                                     (5213)
    if (a == 0)
        goto ca28e;
    //     lda l0073                                                     (5214)
    a = l0073;
    //     ora l006f                                                     (5215)
    a |= l006f;
    //     bne ca28e                                                     (5216)
    if (a != 0)
        goto ca28e;
    //     jmp ca360                                                     (5217)
    goto ca360;

    // ca28e: (5219)
ca28e:
    //     lda current_line_ptr+1 (5220) cmp l0012 (5221) bcc ca29c (5222) bne
    //     ca2dc                                                       (5223)
    //     lda current_line_ptr (5224) cmp l0011 (5225) bcs ca2dc (5226)
    if (current_line_ptr < top_of_screen_line_ptr)
        goto ca29c;
    else
        goto ca2dc;
    // ca29c: (5227)
ca29c:
    //     lda l006f (5228)
    a = l006f;
    //     bne ca30d (5229)
    if (a != 0)
        goto ca30d;
    //     lda l0033 (5230)
    a = l0033;
    //     sta ruler_stack_ptr (5231)
    ruler_index_ptr = a;
    //     ldy l0012 (5232) lda l0011 (5233) cpy top+1 (5234) bcc ca2b2 (5235)
    //     bne ca30d (5236) cmp top (5237) bcs ca30d (5238)
    if (top_of_screen_line_ptr >= top)
        goto ca30d;
    // ca2b2: (5239)
ca2b2:
    //     jsr sub_cab37 (5240)
    move_tmp01_to_previous_line();
    //     ldy tmp1 (5241)
    y = tmp1;
    //     cpy current_line_ptr+1 (5242)
    cmp(&flags, y, (uint8_t)(current_line_ptr >> 8));
    //     bne ca30d (5243)
    if (!(flags & FLAG_Z))
        goto ca30d;
    //     lda tmp0 (5244)
    a = tmp0;
    //     cmp current_line_ptr (5245)
    cmp(&flags, a, (uint8_t)(current_line_ptr & 0xff));
    //     bne ca30d (5246)
    if (!(flags & FLAG_Z))
        goto ca30d;
    //     sty l0012 (5247) sta l0011 (5248)
    top_of_screen_line_ptr = ((addr_t)y << 8) | a;
    //     ldx screen_height (5249)
    x = screen_maxrow;
    // loop_ca2c7: (5250)
loop_ca2c7:
    //     dex (5251)
    x--;
    //     lda line_lengths,x (5252)
    a = line_lengths[x];
    //     inx (5253)
    x++;
    //     sta line_lengths,x (5254)
    line_lengths[x] = a;
    //     dex (5255)
    x--;
    //     bne loop_ca2c7 (5256)
    if (x != 0)
        goto loop_ca2c7;
    //     ldy #SCREEN_SCROLLDOWN (5257) jsr SCREEN (5258)
    screen_scrolldown();
    //     jsr home_cursor (5259)
    screen_setcursor(0, 1);
    //     ldy #1 (5260)
    y = 1;
    //     jmp ca351 (5261)
    goto ca351;

    // ca2dc: (5263)
ca2dc:
    //     lda l0033 (5264)
    a = l0033;
    //     sta ruler_stack_ptr (5265)
    ruler_index_ptr = a;
    // ca2e0: (5266)
ca2e0:
    //     ldx #0 (5267)
    x = 0;
    //     lda l0011 (5268)
    a = (uint8_t)(top_of_screen_line_ptr & 0xff);
    //     ldy l0012 (5269)
    y = (uint8_t)(top_of_screen_line_ptr >> 8);
    // ca2e6: (5270)
ca2e6:
    //     inx (5271)
    x++;
    //     cpy ptr6+1 (5272)
    cmp(&flags, y, (uint8_t)(ptr6 >> 8));
    //     bne ca2f1 (5273)
    if (!(flags & FLAG_Z))
        goto ca2f1;
    //     cmp ptr6 (5274)
    cmp(&flags, a, (uint8_t)(ptr6 & 0xff));
    //     bne ca2f1 (5275)
    if (!(flags & FLAG_Z))
        goto ca2f1;
    //     stx l003d (5276)
    l003d = x;
    // ca2f1: (5277)
ca2f1:
    //     cpy current_line_ptr+1 (5278)
    cmp(&flags, y, (uint8_t)(current_line_ptr >> 8));
    //     bne ca2f9 (5279)
    if (!(flags & FLAG_Z))
        goto ca2f9;
    //     cmp current_line_ptr (5280)
    cmp(&flags, a, (uint8_t)(current_line_ptr & 0xff));
    //     beq ca313 (5281)
    if (flags & FLAG_Z)
        goto ca313;
    // ca2f9: (5282)
ca2f9:
    //     jsr sub_cab1a (5283)
    sub_cab1a();
    //     beq ca313 (5284)
    if (flags & FLAG_Z)
        goto ca313;
    //     tya (5285)
    a = y;
    //     ldy tmp1 (5286)
    y = tmp1;
    //     clc (5287)
    flags &= ~FLAG_C;
    //     adc tmp0 (5288)
    a = adc(&flags, a, tmp0);
    //     bcc ca307 (5289)
    if (!(flags & FLAG_C))
        goto ca307;
    //     iny (5290)
    y++;
    // ca307: (5291)
ca307:
    //     cpx screen_height (5292)
    cmp(&flags, x, screen_maxrow);
    //     beq ca2e6 (5293)
    if (flags & FLAG_Z)
        goto ca2e6;
    //     bcc ca2e6 (5294)
    if (!(flags & FLAG_C))
        goto ca2e6;
    // ca30d: (5295)
ca30d:
    //     jsr sub_ca44e (5296)
    sub_ca44e();
    //     jmp ca2e0 (5297)
    goto ca2e0;

    // ca313: (5299)
ca313:
    //     cpx screen_height (5300)
    cmp(&flags, x, screen_maxrow);
    //     bcc ca35e (5301)
    if (!(flags & FLAG_C))
        goto ca35e;
    //     beq ca35e (5302)
    if (flags & FLAG_Z)
        goto ca35e;
    //     lda l006f (5303)
    a = l006f;
    //     bne ca30d (5304)
    if (a != 0)
        goto ca30d;
    //     ldx #0 (5305)
    x = 0;
    // loop_ca31f: (5306)
loop_ca31f:
    //     lda line_lengths+1,x (5307)
    a = line_lengths[x + 1];
    //     sta line_lengths,x (5308)
    line_lengths[x] = a;
    //     inx (5309)
    x++;
    //     cpx screen_height (5310)
    if (x != screen_maxrow)
        goto loop_ca31f;
    //     bne loop_ca31f (5311)
    //     dec l003d (5312)
    l003d--;
    //     ldx #0 (5313)
    x = 0;
    //     lda screen_width (5314)
    a = screen_maxcolumn;
    //     sta line_lengths,x (5315)
    line_lengths[x] = a;
    //     lda l0033 (5316)
    a = l0033;
    //     sta ruler_stack_ptr (5317)
    ruler_index_ptr = a;
    //     ldy l0012 (5318)
    y = (uint8_t)(top_of_screen_line_ptr >> 8);
    //     lda l0011 (5319)
    a = (uint8_t)(top_of_screen_line_ptr & 0xff);
    //     jsr sub_cab1a (5320)
    sub_cab1a();
    //     tya (5321)
    a = y;
    //     clc (5322)
    flags &= ~FLAG_C;
    //     adc l0011 (5323)
    a = adc(&flags, a, (uint8_t)(top_of_screen_line_ptr & 0xff));
    //     sta l0011 (5324)
    top_of_screen_line_ptr = (top_of_screen_line_ptr & 0xff00) | a;
    //     bcc ca348 (5325)
    if (!(flags & FLAG_C))
        goto ca348;
    //     inc l0012 (5326)
    top_of_screen_line_ptr += 0x0100;
    // ca348: (5327)
ca348:
    //     ldy #SCREEN_SCROLLUP (5328) jsr SCREEN (5329)
    screen_scrollup();
    //     ldx #0 (5330) ldy screen_height (5331) jsr set_cursor_position (5332)
    screen_setcursor(0, screen_maxrow);
    // After the jsr, Y still holds screen_height (set by ldy above).
    // The C equivalent must restore it explicitly since screen_setcursor
    // receives the value as a parameter rather than via the global y.
    y = screen_maxrow;
    // ca351: (5333)
ca351:
    //     lda ruler_stack_ptr (5334)
    a = ruler_index_ptr;
    //     sta l0033 (5335)
    l0033 = a;
    //     inc input_buffer_ptr+1 (5336)
    saved_status_line_needs_redrawing_flag++;
    //     inc l0074 (5337)
    l0074++;
    //     tya (5338)
    a = y;
    //     tax (5339)
    x = a;
    // ca35e: (5340)
ca35e:
    //     stx ypos (5341)
    ypos = x;
    // ca360: (5342)
ca360:
    //     ldy l0034 (5343)
    y = l0034;
    //     jsr cab91 (5344)
    cab91();
    //     jsr unpack_line_into_buffer (5345)
    unpack_line_into_buffer();
    //     jsr sub_ca608 (5346)
    recalculate_cursor_xpos();
    //     lda screen_width (5347)
    a = screen_maxcolumn;
    //     lsr (5348)
    a >>= 1;
    //     sta l0083 (5349)
    l0083 = a;
    //     lda l0072 (5350)
    a = l0072;
    //     cmp hscroll_pos (5351)
    cmp(&flags, a, hscroll_pos);
    //     bcc ca381 (5352)
    if (!(flags & FLAG_C))
        goto ca381;
    //     lda hscroll_pos (5353)
    a = hscroll_pos;
    //     clc (5354)
    flags &= ~FLAG_C;
    //     adc screen_width (5355)
    a = adc(&flags, a, screen_maxcolumn);
    //     sbc #3 (5356)
    a = sbc(&flags, a, 3);
    //     cmp l0072 (5357)
    if (a >= l0072)
        goto ca395;
    //     bcs ca395 (5358)
    // ca381: (5359)
ca381:
    //     lda l0072 (5360)
    a = l0072;
    //     sec (5361)
    flags |= FLAG_C;
    //     sbc l0083 (5362)
    a = sbc(&flags, a, l0083);
    //     bcs ca38a (5363)
    if (flags & FLAG_C)
        goto ca38a;
    //     lda #0 (5364)
    a = 0;
    // ca38a: (5365)
ca38a:
    //     sta hscroll_pos (5366)
    hscroll_pos = a;
    //     lda #1 (5367)
    a = 1;
    //     sta l0073 (5368)
    l0073 = a;
    //     sta input_buffer_ptr+1 (5369)
    saved_status_line_needs_redrawing_flag = a;
    //     jsr ca93c (5370)
    write_line_back_to_document_safely();
    // ca395: (5371)
ca395:
    //     lda input_buffer_ptr+1 (5372)
    a = saved_status_line_needs_redrawing_flag;
    //     sta l0076 (5373)
    status_line_needs_redrawing_flag = a;
    //     lda l0073 (5374)
    a = l0073;
    //     beq ca3e7 (5375)
    if (a == 0)
        goto ca3e7;
    //     bpl ca3b2 (5376)
    if (!((int8_t)a < 0))
        goto ca3b2;
    //     lda l003d (5377)
    a = l003d;
    //     bmi ca3b2 (5378)
    if ((int8_t)a < 0)
        goto ca3b2;
    //     sta l0082 (5379)
    l0082 = a;
    //     lda screen_height (5380)
    a = screen_maxrow;
    //     sec (5381)
    //     sbc l003d (5382)
    a -= l003d;
    //     tax (5383)
    x = a;
    //     inx (5384)
    x++;
    //     lda ptr6 (5385)
    a = (uint8_t)(ptr6 & 0xff);
    //     ldy ptr6+1 (5386)
    y = (uint8_t)(ptr6 >> 8);
    //     bne ca3c1 (5387)
    if (y != 0)
        goto ca3c1;
    // ca3b2: (5388)
ca3b2:
    //     ldy l0033 (5389)
    y = l0033;
    //     jsr cab91 (5390)
    cab91();
    //     lda #1 (5391)
    a = 1;
    //     sta l0082 (5392)
    l0082 = a;
    //     lda l0011 (5393)
    a = (uint8_t)(top_of_screen_line_ptr & 0xff);
    //     ldy l0012 (5394)
    y = (uint8_t)(top_of_screen_line_ptr >> 8);
    //     ldx screen_height (5395)
    x = screen_maxrow;
    // ca3c1: (5396)
ca3c1:
    //     stx l0081 (5397)
    l0081 = x;
    // loop_ca3c3: (5398)
loop_ca3c3:
    //     jsr sub_ca486 (5399)
    draw_line(((uint16_t)y << 8) | a);
    //     lda tmp0 (5400)
    a = tmp0;
    //     ldy tmp1 (5401)
    y = tmp1;
    //     jsr sub_cab1a (5402)
    sub_cab1a();
    //     beq ca422 (5403)
    if (flags & FLAG_Z)
        goto ca422;
    //     tya (5404)
    a = y;
    //     ldy tmp1 (5405)
    y = tmp1;
    //     clc (5406)
    flags &= ~FLAG_C;
    //     adc tmp0 (5407)
    a = adc(&flags, a, tmp0);
    //     bcc ca3d8 (5408)
    if (!(flags & FLAG_C))
        goto ca3d8;
    //     iny (5409)
    y++;
    // ca3d8: (5410)
ca3d8:
    //     inc l0082 (5411)
    l0082++;
    //     dec l0081 (5412)
    l0081--;
    //     bne loop_ca3c3 (5413)
    if (l0081 != 0)
        goto loop_ca3c3;
    //     bne loop_ca3c3 fall-through → ca3de (5413→5414)
    // ca3de: (5414)
ca3de:
    //     lda #0 (5415)
    a = 0;
    //     sta l0074 (5416)
    l0074 = a;
    //     ldy l0034 (5417)
    y = l0034;
    //     jsr cab91 (5418)
    cab91();
    // ca3e7: (5419)
ca3e7:
    //     jsr unpack_line_into_buffer (5420)
    unpack_line_into_buffer();
    //     jsr sub_caacb (5421)
    sub_caacb();
    //     jsr draw_ruler (5422)
    draw_ruler();
    //     lda l0074 (5423)
    a = l0074;
    //     beq ca3ff (5424)
    if (a == 0)
        goto ca3ff;
    //     lda ypos (5425)
    a = ypos;
    //     sta l0082 (5426)
    l0082 = a;
    //     lda current_format_line_ptr (5427) ldy current_format_line_ptr+1
    //     (5428) jsr sub_ca486 (5429)
    draw_line(current_format_line_ptr);
    // ca3ff: (5430)
ca3ff:
    //     lda flags_need_redrawing_flag (5431)
    a = flags_need_redrawing_flag;
    //     beq ca406 (5432)
    if (a == 0)
        goto ca406;
    //     jsr sub_ca651 (5433)
    draw_status_word();
    // ca406: (5434)
ca406:
    //     lda l0072 (5435)
    a = l0072;
    //     sec (5436)
    flags |= FLAG_C;
    //     sbc hscroll_pos (5437)
    a = sbc(&flags, a, hscroll_pos);
    //     clc (5438)
    //     adc #3 (5439)
    a += 3;
    //     tax (5440)
    x = a;
    //     ldy #0 (5441)
    y = 0;
    //     sty l0073 (5442)
    l0073 = y;
    //     sty l0074 (5443)
    l0074 = y;
    //     sty l006f (5444)
    l006f = y;
    //     dey (5445)
    y--;
    //     sty ptr6+1 (5446)
    ptr6 = (ptr6 & 0x00ff) | ((uint16_t)y << 8);
    //     ldy ypos (5447) jsr set_cursor_position (5448)
    screen_setcursor(x, ypos);
    //     jmp cursor_on (5449)
    cursor_on();
    return;

    // ca422: (5451)
ca422:
    //     dec l0081 (5452)
    l0081--;
    //     beq ca3de (5453)
    if (l0081 == 0)
        goto ca3de;
    //     ldx l0082 (5454)
    x = l0082;
    //     lda screen_width (5455)
    a = screen_maxcolumn + 1;
    //     sta line_lengths+1,x (5456)
    line_lengths[x + 1] = a;
    //     sta l0083 (5457)
    l0083 = a;
    //     lda #0x2a ; '*' (5458)
    a = 0x2a;
    // loop_ca431: (5459)
loop_ca431:
    //     inc l0082 (5460)
    l0082++;
    //     ldx #0 (5461) ldy l0082 (5462) jsr set_cursor_position (5463)
    screen_setcursor(0, l0082);
    //     jsr sub_ca597 (5464)
    clear_to_eol();
    //     lda l0083 (5465)
    a = l0083;
    //     sta line_lengths,x (5466)
    line_lengths[x] = a;
    //     lda #0 (5467)
    a = 0;
    //     sta l0083 (5468)
    l0083 = a;
    //     lda #0x20 ; ' ' (5469)
    a = 0x20;
    //     dec l0081 (5470)
    l0081--;
    //     bne loop_ca431 (5471)
    if (l0081 != 0)
        goto loop_ca431;
    //     beq ca3de (5472)
    goto ca3de;
}

static void render_char(void)
{
    // render_char
    //  Inputs: a, y, flags:C, flags:Z
    //  Outputs: x
    // ca4e9: Renders character to screen with attribute handling.
    //
    // Input:
    //   a     = character to render
    //   y     = position in edit buffer (for marker check)
    //   x     = l0083 (screen column), l0082 (line number)
    //
    // Output:
    //   a     = char_to_render (for caller's CR line-terminator detection)
    //   x     = l0084 (restored by caller), y unchanged
    //
    // Marker handling:
    //   Highlight toggles 0x1c/0x1d are replaced with '-'/'*' and
    //   displayed inverted.  Markers at index 0 (match via sub_ca536)
    //   enable REVERSE style for the character.  After output, style
    //   is reset to NORMAL if the current position matched a marker
    //   (marker_idx == 0).  CR and NUL are replaced with space.
    uint8_t char_to_render = a;
    uint8_t marker_idx = 0;

    //     ldx l0082
    x = l0082;
    //     lda line_lengths,x
    if (line_lengths[x] != 0)
    {
        //     dec line_lengths,x
        line_lengths[x]--;
    }
    // ca4f4:
    //     ldx l0083
    x = l0083;
    //     cpx screen_width
    if (x >= screen_maxcolumn)
    {
        a = char_to_render;
        x = l0084;
        return;
    }
    //     inc l0083
    l0083++;
    //     tya
    a = y;
    //     beq ca514
    if (a == 0)
        goto ca514;
    //     dey
    y--;
    //     jsr sub_ca536
    sub_ca536();
    //     iny
    y++;
    //     cpx #4
    if (x >= 4)
        goto ca514;
    //     tax
    marker_idx = a;
    x = marker_idx;
    //     bmi ca523
    if (x & 0x80)
    {
        a = char_to_render;
        goto ca523;
    }
    //     bne ca514
    if (x != 0)
        goto ca514;
    // Marker match at index < 4: render inverted
    // ca50e:
ca50e:
    a = STYLE_REVERSE;
    screen_setstyle(a);
    // ca514:
ca514:
    a = char_to_render;
    //     jsr check_for_control_code
    check_for_control_code();
    if (!(flags & FLAG_Z))
        goto ca522;
    if (flags & FLAG_C)
    {
        a = 0x2d;
    }
    else
    {
        a = 0x2a;
    }
    // ca522:
ca522:
    // ca523:
ca523:
    //     cmp #0x0d
    if (a == 0x0d || a == 0x00)
    {
        a = 0x20;
    }
    // ca529:
    //     jsr screen_putchar
    screen_putchar(a);
    //     txa  (use x directly for the style-reset decision)
    //     bne ca532
    if (x != 0)
        goto ca532;
    a = 0;
    screen_setstyle(a);
    // ca532:
    // ca533:
ca532:
    a = char_to_render;
    //     ldx l0084
    x = l0084;
    //     rts
}

static void render_xchar(void)
{
    // render_xchar: Renders a character to screen with style/attribute handling

    //     inc l0039
    l0039++;
    //     stx l0084
    l0084 = x;
    //     ldx input_buffer_offset+1
    x = l0080;
    //     inc input_buffer_offset+1
    l0080++;
    //     cpx hscroll_pos
    cmp(&flags, x, hscroll_pos);
    //     bcc ca533
    if (!(flags & FLAG_C))
    {
        x = l0084;
        return;
    }
    //     jmp ca4e9
    render_char();
}

static void restore_cursor_position(void)
{
    // restore_cursor_position:
    //     ldx tmp4
    //     ldy tmp5
    screen_setcursor(tmp4, tmp5);
}
extern uint8_t parser_table[];

void sanitise_area(void)
{
    // sanitise_area
    //  Inputs: -
    //  Temps:  tmp67
    //  Outputs: a, x, flags:Z
    uint8_t tmp6, tmp7;
    // sanitise_area:
    //     lda area_start_ptr
    a = (uint8_t)(area_start_ptr & 0xff);
    //     ldx area_start_ptr+1
    x = (uint8_t)(area_start_ptr >> 8);
    //     cpx area_end_ptr+1
    cmp(&flags, x, (uint8_t)(area_end_ptr >> 8));
    //     bcc c8977
    if (!(flags & FLAG_C))
        goto c8977;
    //     bne c896b
    if (!(flags & FLAG_Z))
        goto c896b;
    //     cmp area_end_ptr
    cmp(&flags, a, (uint8_t)(area_end_ptr & 0xff));
    //     bcc c8977
    if (!(flags & FLAG_C))
        goto c8977;
c896b:
    // c896b:
    //     ldy area_end_ptr
    y = (uint8_t)(area_end_ptr & 0xff);
    //     sty area_start_ptr
    area_start_ptr = (area_start_ptr & 0xff00) | y;
    //     ldy area_end_ptr+1
    y = (uint8_t)(area_end_ptr >> 8);
    //     sty area_start_ptr+1
    area_start_ptr = (area_start_ptr & 0x00ff) | ((uint16_t)y << 8);
    //     stx area_end_ptr+1
    area_end_ptr = (area_end_ptr & 0x00ff) | ((uint16_t)x << 8);
    //     sta area_end_ptr
    area_end_ptr = (area_end_ptr & 0xff00) | a;
c8977:
    // c8977:
    //     lda area_end_ptr
    a = (uint8_t)(area_end_ptr & 0xff);
    //     sec
    flags |= FLAG_C;
    //     sbc area_start_ptr
    flags |= FLAG_C;
    a = sbc(&flags, a, (uint8_t)(area_start_ptr & 0xff));
    //     sta tmp6
    tmp6 = a;
    //     lda area_end_ptr+1
    a = (uint8_t)(area_end_ptr >> 8);
    //     sbc area_start_ptr+1
    a = sbc(&flags, a, (uint8_t)(area_start_ptr >> 8));
    //     sta tmp7
    tmp7 = a;
    //     bne return_10
    if (a != 0)
    {
        set_flags(&flags, a);
        return;
    }
    //     lda tmp6
    a = tmp6;
    set_flags(&flags, a);
    // return_10:
    //     rts
}

static void save_cursor_position(void)
{
    // Pseudocode: Saves current cursor position via SCREEN call

    // ;
    // ***************************************************************************************
    // save_cursor_position:
    //     ldy #SCREEN_GETCURSOR
    //     jsr SCREEN
    uint16_t cursor_ = screen_getcursor();
    a = (uint8_t)(cursor_ & 0xff);
    x = (uint8_t)(cursor_ >> 8);
    //     sta tmp4
    tmp45 = (addr_t)(x) << 8 | a;
    //     rts
}

static void set_marker(void)
{
    // set_marker:
    //     jsr set_marker_to_here
    set_marker_to_here();
    //     jmp ca035
    a = 1;
    l0073 = a;
    ca684();
    return;
}
void go_to_marker(void);
// MULTIPLE ENTRY POINTS: sf6_go_to_marker_key, go_to_marker, go_to_marker_1..6

static void set_marker_common(void)
{
    //     pha
    uint8_t saved_a = a;
    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    //     pla
    a = saved_a;
    //     jsr lookup_marker
    lookup_marker();
    //     jmp set_marker
    set_marker();
    return;
}

void show_memory_full_error(void)
{
    // show_memory_full_error
    //  Outputs: a, x, y
    // show_memory_full_error (sub_ca94a): Memory full error handler
    // On entry: (none)
    // On exit:  l006e=0, status_line_needs_redrawing_flag=1, l0073=1, cursor on
    // Uses: a, x, y, line_lengths

    //     jsr cursor_off
    cursor_off();
    //     ldx #3
    //     ldy #0
    screen_setcursor(3, 0);
    //     jsr set_inverted_text_if_not_mode_7
    a = STYLE_REVERSE;
    screen_setstyle(a);
    //     ldy screen_width
    y = screen_maxcolumn;
    //     sty line_lengths
    line_lengths[0] = y;
    //     dey
    y--;
    //     dey
    y--;
    //     ldx #0
    x = 0;
    //     beq ca965
    goto ca965;

    // loop_ca962:
loop_ca962:
    //     jsr screen_putchar
    screen_putchar(a);
    // ca965:
ca965:;
    //     lda la995,x
    a = la995_data[x];
    //     beq ca96e
    if (a == 0)
        goto ca96e;
    //     inx
    x++;
    //     dey
    y--;
    //     bne loop_ca962
    if (y != 0)
        goto loop_ca962;
    // ca96e:
ca96e:
    //     jsr set_normal_text_if_not_mode_7
    a = 0;
    screen_setstyle(a);
    //     tya
    a = y;
    //     beq ca97c
    if (a == 0)
        goto ca97c;
    //     lda #0x20 ; ' '
    a = 0x20;
    // loop_ca976:
loop_ca976:
    //     jsr screen_putchar
    screen_putchar(a);
    //     dey
    y--;
    //     bne loop_ca976
    if (y != 0)
        goto loop_ca976;
    // ca97c:
ca97c:
    //     lda #0
    a = 0;
    //     sta l006e
    edit_buffer_unpacked_flag = a;
    //     jsr clear_cmd
    clear_cmd();
    // loop_ca983:
loop_ca983:
    //     jsr beep
    beep();
    //     jsr flush_and_read_char
    read_char();
    //     bcc loop_ca983
    if (!(flags & FLAG_C))
        goto loop_ca983;
    //     jsr cursor_on
    cursor_on();
    //     lda #1
    a = 1;
    //     sta status_line_needs_redrawing_flag
    status_line_needs_redrawing_flag = a;
    //     sta l0073
    l0073 = a;
    //     rts
}

void sub_c89d3(void)
{
    // sub_c89d3:
    //     lda area_start_ptr
    a = (uint8_t)(area_start_ptr & 0xff);
    //     sta tmp4
    tmp4 = a;
    //     lda area_start_ptr+1
    a = (uint8_t)(area_start_ptr >> 8);
    //     sta tmp5
    tmp5 = a;
    //     jsr adjust_pointers
    adjust_pointers();
    //     lda tmp4
    a = tmp4;
    //     ldy tmp5
    y = tmp5;
    //     jmp cac78
    cac78();
    return;
}

static void sub_c8c51(void)
{
    // sub_c8c51:
    //     lda #0
    a = 0;
    sub_c8c53();
}

static void sub_c8c53(void)
{
    // sub_c8c53:
    //     ldx l0048
    x = l0048;
    //     cpx #MAX_LINE_LENGTH
    //     bcs return_13
    if (x >= MAX_LINE_LENGTH)
        return;
    //     sta output_buffer,x
    output_buffer[x] = a;
    //     inc l0048
    l0048++;
    // return_13:
    //     rts
}

void sub_c8c5f(void)
{
    // sub_c8c5f: converts to uppercase only if folding flag is clear
    //     bit folding_flag
    if (folding_flag & FLAG_N)
        return;
    //     falls through to to_uppercase
    a = toupper(a);
    return;
}

static void sub_c9936(void)
{
    // sub_c9936
    //  Inputs: x, y
    //  Outputs: a, flags:Z
    // Pseudocode: Processes a character from the edit line for output, handling
    // tabs and margins

    // sub_c9936:
    //     ror l0083
    l0083 = ror(&flags, l0083);
    //     lda (current_edit_line_ptr),y
    a = ram[current_edit_line_ptr + y];
    //     sta output_buffer,y
    output_buffer[y] = a;
    //     cmp #9
    if (a != 9)
        goto c994a;
    //     bne c994a
    //     jsr sub_ca5ae
    process_document_character();
    //     txa
    a = x;
    //     clc
    //     adc l0039
    a += l0039;
    //     bne c995c
    if (!(flags & FLAG_Z))
        goto c995c;
    // c994a:
c994a:
    //     cmp #0x0b
    if (a != 0x0b)
        goto c9969;
    //     bne c9969
    //     lda ruler_left_stop
    a = ruler_left_stop;
    set_flags(&flags, a);
    //     beq c9967
    if (flags & FLAG_Z)
        goto c9967;
    //     ldx l0039
    x = l0039;
    set_flags(&flags, x);
    //     beq c995c
    if (flags & FLAG_Z)
        goto c995c;
    //     cpx ruler_left_stop
    if (x < ruler_left_stop)
        goto c995c;
    //     bcc c995c
    //     inx
    x++;
    //     txa
    a = x;
    // c995c:
c995c:
    //     sta l0039
    l0039 = a;
    //     sty l0042
    l0042 = y;
    //     inc l0042
    l0042++;
    //     lda #0
    a = 0;
    //     sta l0046
    l0046 = a;
    set_flags(&flags, 0);
    //     rts
    return;

    // c9967:
c9967:
    //     lda #0x20 ; ' '
    a = 0x20;
    // c9969:
c9969:
    //     cmp #0x1b
    if (a < 0x1b)
        goto c9967;
    //     bcc c9967
    //     cmp #0x20 ; ' '
    cmp(&flags, a, 0x20);
    //     bcc return_49
    if (!(flags & FLAG_C))
        return;
    //     inc l0039
    l0039++;
    set_flags(&flags, l0039);
    // return_49:
return_49:
    //     rts
    return;
}

void sub_c9977(void)
{
    // sub_c9977
    //  Inputs: x
    //  Outputs: a, y; tmp67
    // PROVISIONAL: Main line formatting routine — reads source line, handles
    // margins, tabs, wrapping. PROVISIONAL: Called from f0_format_block_key
    // (Ctrl+B) and fold_cmd. PROVISIONAL: Processes one line (or skips
    // command/ruler lines), returns with Z from l007e.

    // sub_c9977:
    // PROVISIONAL: Mark cursor moved, init print_xpos=4, zero
    // input_buffer_offset and l007e.
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    //     ldy #4
    y = 4;
    //     sty print_xpos
    print_xpos = y;
    //     ldy #0
    y = 0;
    //     sty input_buffer_offset
    input_buffer_offset = y;
    //     sty l007e
    l007e = y;
    // PROVISIONAL: Check if first byte of current line is a command prefix
    // (0x80/0x81). PROVISIONAL: If so, skip this line and return (paragraph
    // boundary reached).
    //      lda (current_line_ptr),y
    a = ram[current_line_ptr + y];
    //     jsr check_for_command_prefix
    flags = check_for_command_prefix(a);
    //     beq c9974
    if (flags & FLAG_Z)
    {
        advance_to_next_line();
        return;
    }
    // PROVISIONAL: Main formatting loop entry. Check format mode — if bit 7 or
    // bit 0 is set, PROVISIONAL: skip this line (paragraph boundary). Also skip
    // if ruler_right_stop == 0 PROVISIONAL: or if right_stop <= left_stop.
    //  c998a:
c998a:
    //     lda format_mode_flag
    a = format_mode_flag;
    //     and #0x81
    a &= 0x81;
    set_flags(&flags, a);
    //     bne c9974
    if (!(flags & FLAG_Z))
    {
        advance_to_next_line();
        return;
    }
    //     lda ruler_right_stop
    a = ruler_right_stop;
    set_flags(&flags, a);
    //     beq c9974
    if (flags & FLAG_Z)
    {
        advance_to_next_line();
        return;
    }
    //     sec
    flags |= FLAG_C;
    //     sbc ruler_left_stop
    a = sbc(&flags, a, ruler_left_stop);
    //     bcc c9974
    if (!(flags & FLAG_C))
    {
        advance_to_next_line();
        return;
    }
    // PROVISIONAL: Compute line width = right_stop - left_stop + 1, store in
    // l0080.
    //      adc #1
    a = adc(&flags, a, 1);
    //     sta input_buffer_offset+1
    l0080 = a;
    // PROVISIONAL: Wipe the edit buffer with 0x10 (soft spaces) and set up
    // tmp6/tmp7 = current_line_ptr.
    //      lda #0x10
    a = 0x10;
    //     jsr wipe_buffer
    wipe_buffer();
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     sta tmp6
    tmp6 = a;
    //     lda current_line_ptr+1
    a = (uint8_t)((current_line_ptr >> 8) & 0xff);
    //     sta tmp7
    tmp7 = a;
    // PROVISIONAL: Zero working variables: l0047 (character index), l0039
    // (column counter), PROVISIONAL: l0038 (soft-hyphen/break flag), l0046
    // (word-start flag), bottom_margin.
    //      ldy #0
    y = 0;
    //     sty l0047
    l0047 = y;
    //     sty l0039
    l0039 = y;
    //     sty l0038
    l0038 = y;
    //     sty l0046
    l0046 = y;
    //     sty bottom_margin
    bottom_margin = y;
    // PROVISIONAL: Save current buffer index in l0048. Then scan forward
    // through the source line, PROVISIONAL: processing any marker-prefix bytes
    // detected by sub_ca536. Increments l007e for PROVISIONAL: each marker
    // processed (l007e counts marker bytes).
    //  c99b6:
c99b6:
    //     sty l0048
    l0048 = y;
    //     ldy l0047
    y = l0047;
    // loop_c99ba:
loop_c99ba:
    //     jsr sub_ca536
    sub_ca536();
    //     bne c99c7
    if (!(flags & FLAG_Z))
        goto c99c7;
    //     lda #0
    a = 0;
    //     sta markers_array+1,x
    markers_array[1 + x] = a;
    //     inc l007e
    l007e++;
    //     bne loop_c99ba
    if (l007e != 0)
        goto loop_c99ba;
    // c99c7:
c99c7:
    // PROVISIONAL: Character processing loop. Reads one byte from the source
    // document line. PROVISIONAL: l0047 tracks the read position, l0048 tracks
    // the write position. PROVISIONAL: Handles tabs (0x09), soft hyphens /
    // break markers (0x1a), soft spaces (0x0b), CR (0x0d), PROVISIONAL: and
    // ordinary characters.
    //  c99c9:
c99c9:
    //     lda (current_line_ptr),y
    a = ram[current_line_ptr + y];
    //     iny
    y++;
    //     sty l0047
    l0047 = y;
    // PROVISIONAL: Tab (0x09): process via process_document_character to
    // compute tab width, PROVISIONAL: subtract 1 (x--) and add to column
    // counter l0039.
    //      cmp #9
    if (a != 9)
        goto c99e0;
    //     bne c99e0
    //     jsr sub_ca5ae
    process_document_character();
    //     dex
    x--;
    //     txa
    a = x;
    //     clc
    //     adc l0039
    a += l0039;
    //     sta l0039
    l0039 = a;
    //     lda #9
    a = 9;
    //     bne c9a21                                                         ;
    //     ALWAYS branch
    goto c9a21;

    // c99e0:
c99e0:
    //     cmp #0x1a
    if (a != 0x1a)
        goto c99ee;
    //     bne c99ee
    // PROVISIONAL: Soft hyphen / break marker (0x1a): if l0046 (word-start
    // flag) is non-zero, PROVISIONAL: skip the marker and continue reading.
    // Otherwise, treat it as a word-break: PROVISIONAL: set l0046 (word-break
    // state), output a space, and go to c9a2e.
    //  c99e4:
c99e4:
    //     lda l0046
    a = l0046;
    if (a != 0)
        goto c99c9;
    //     bne c99c9
    //     ldx #0xff
    x = 0xff;
    //     lda #0x20 ; ' '
    a = 0x20;
    //     bne c9a2e                                                         ;
    //     ALWAYS branch
    goto c9a2e;

    // c99ee:
    // PROVISIONAL: Soft space (0x0b, margin tab). If input_buffer_offset is
    // non-zero (already PROVISIONAL: in a word-break), treat as break marker
    // (c99e4). If l0038 (soft-hyphen flag) PROVISIONAL: is set, also treat as
    // break. Otherwise set l0038 and advance the line width PROVISIONAL:
    // (l0080) by ruler_left_stop, or snap column to ruler_left_stop if below
    // it.
c99ee:
    //     cmp #0x0b
    if (a != 0x0b)
        goto c9a11;
    //     bne c9a11
    //     ldx input_buffer_offset
    x = input_buffer_offset;
    if (x != 0)
        goto c99e4;
    //     bne c99e4
    //     lda l0038
    a = l0038;
    if (a != 0)
        goto c99e4;
    //     bne c99e4
    //     inc l0038
    l0038++;
    //     lda ruler_left_stop
    a = ruler_left_stop;
    set_flags(&flags, a);
    //     beq c99c9
    if (flags & FLAG_Z)
        goto c99c9;
    //     ldx l0039
    x = l0039;
    //     cpx ruler_left_stop
    if (x >= ruler_left_stop)
        goto c9a0a;
    //     bcs c9a0a
    //     sta l0039
    l0039 = a;
    //     dec l0039
    l0039--;
    // c9a0a:
c9a0a:
    //     clc
    //     adc input_buffer_offset+1
    a += l0080;
    //     sta input_buffer_offset+1
    l0080 = a;
    //     lda #0x0b
    a = 0x0b;
    // c9a11:
    // PROVISIONAL: CR (0x0d) — end of source line. If y == 0 after decrement,
    // the buffer is PROVISIONAL: empty; advance to next document line.
    // Otherwise, call sub_c9ac1 to find the PROVISIONAL: word-wrap boundary. If
    // C set (wrap needed), go to c9a87 to flush the current PROVISIONAL: line
    // and advance. Otherwise, insert a space at the break.
c9a11:
    //     cmp #0x0d
    if (a != 0x0d)
        goto c9a21;
    //     bne c9a21
    //     dey
    y--;
    set_flags(&flags, y);
    //     beq c9a8d
    if (flags & FLAG_Z)
    {
        advance_to_next_line();
        return;
    }
    //     jsr sub_c9ac1
    sub_c9ac1();
    //     bcs c9a87
    if (flags & FLAG_C)
        goto c9a87;
    //     lda #0x20 ; ' '
    a = 0x20;
    //     sta input_buffer_offset
    input_buffer_offset = a;
    // c9a21:
    // PROVISIONAL: Track word-start state. If the current character is a space
    // (0x20), set x=1 PROVISIONAL: (word-start flag). If l0046 already
    // indicates word-start (N bit set), skip PROVISIONAL: straight to the write
    // (c9a40) — this is a multi-space gap, keep only the first.
c9a21:
    //     ldy l0048
    y = l0048;
    //     ldx #0
    x = 0;
    //     cmp #0x20 ; ' '
    if (a != 0x20)
        goto c9a2e;
    //     bne c9a2e
    //     inx                                                               ;
    //     X=0x01
    x++;
    //     bit l0046
    bit(&flags, a, l0046);
    //     bmi c9a40
    if (flags & FLAG_N)
        goto c9a40;
    // PROVISIONAL: Write character to edit buffer at write position (l0048). If
    // it's a space, PROVISIONAL: rotate bottom_margin (tracks word-boundary
    // state for justification).
    //  c9a2e:
c9a2e:
    //     ldy l0048
    y = l0048;
    //     sta (current_edit_line_ptr),y
    ram[current_edit_line_ptr + y] = a;
    //     cmp #0x20 ; ' '
    cmp(&flags, a, 0x20);
    //     bne c9a38
    if (!(flags & FLAG_Z))
        goto c9a38;
    //     ror bottom_margin
    bottom_margin = ror(&flags, bottom_margin);
    // PROVISIONAL: Advance write position, check for control codes. If not a
    // control code, PROVISIONAL: increment column counter l0039.
    //  c9a38:
c9a38:
    //     iny
    y++;
    //     jsr check_for_control_code
    check_for_control_code();
    //     beq c9a40
    if (flags & FLAG_Z)
        goto c9a40;
    //     inc l0039
    l0039++;
    // PROVISIONAL: Update l0046 (word-start state = x). If N bit of l0046 was
    // set (word-start), PROVISIONAL: or character is space, or buffer index >=
    // 0x85, or bottom_margin is zero, PROVISIONAL: or column >= line width
    // (l0080), skip to c9a58 (clamp and loop back to c99b6).
    //  c9a40:
c9a40:
    //     bit l0046
    bit(&flags, a, l0046);
    //     stx l0046
    l0046 = x;
    //     bmi c9a58
    if (flags & FLAG_N)
        goto c9a58;
    //     cmp #0x20 ; ' '
    if (a == 0x20)
        goto c9a58;
    //     beq c9a58
    //     cpy #0x85
    cmp(&flags, y, MAX_LINE_LENGTH + 1);
    //     bcs c9a60
    if (flags & FLAG_C)
        goto c9a60;
    //     lda bottom_margin
    a = bottom_margin;
    set_flags(&flags, a);
    //     beq c9a58
    if (flags & FLAG_Z)
        goto c9a58;
    //     lda l0039
    a = l0039;
    //     cmp input_buffer_offset+1
    if (a >= l0080)
        goto c9a60;
    //     bcs c9a60
    // PROVISIONAL: Clamp buffer index to max 0x85 (133). Loop back to process
    // next character.
    //  c9a58:
c9a58:
    //     cpy #0x86
    if (y < 0x86)
        goto c9a5d;
    //     bcc c9a5d
    //     dey
    y--;
    // c9a5d:
c9a5d:
    //     jmp c99b6
    goto c99b6;

    // c9a60:
    // PROVISIONAL: Line-width exceeded — flush the current formatted line.
    // Increment the source PROVISIONAL: index (l0047), then scan backward
    // through the edit buffer replacing spaces (0x20) PROVISIONAL: with 0x10
    // (justification markers). This marks word boundaries for
    // justify_edit_buffer. PROVISIONAL: On finding a space, call sub_caed6,
    // justify_edit_buffer, then sub_c9aa9 to write PROVISIONAL: the line.
    // Advance to the next document line; if non-empty, loop back to c998a.
c9a60:
    //     inc l0047
    l0047++;
    // loop_c9a62:
loop_c9a62:
    //     dec l0047
    l0047--;
    //     dey
    y--;
    set_flags(&flags, y);
    //     beq c9a8d
    if (flags & FLAG_Z)
    {
        advance_to_next_line();
        return;
    }
    //     lda (current_edit_line_ptr),y
    a = ram[current_edit_line_ptr + y];
    set_flags(&flags, a);
    //     pha
    {
        uint8_t saved_a = a;
        //     lda #0x10
        a = 0x10;
        //     sta (current_edit_line_ptr),y
        ram[current_edit_line_ptr + y] = a;
        //     pla
        a = saved_a;
    }
    //     cmp #0x20 ; ' '
    cmp(&flags, a, 0x20);
    //     bne loop_c9a62
    if (!(flags & FLAG_Z))
        goto loop_c9a62;
    //     sec
    flags |= FLAG_C;
    //     ror input_buffer_offset
    input_buffer_offset = ror(&flags, input_buffer_offset);
    //     jsr sub_caed6
    sub_caed6();
    //     jsr justify_edit_buffer
    justify_edit_buffer();
    //     jsr sub_c9aa9
    if (sub_c9aa9())
        return;
    //     jsr c9a8d
    advance_to_next_line();
    //     beq c9aa5
    if (flags & FLAG_Z)
        goto c9aa5;
    //     jmp c998a
    goto c998a;

    // c9a87:
    // PROVISIONAL: Word-wrap path — line needs wrapping at a word boundary.
    // Flush the current PROVISIONAL: buffer via sub_caed6 + sub_c9aa9, advance
    // to the next document line, PROVISIONAL: then fall through to c9a8d
    // (returns to sub_c9977's caller).
c9a87:
    //     jsr sub_caed6
    sub_caed6();
    //     jsr sub_c9aa9
    if (sub_c9aa9())
        return;
    //     (fall through to c9a8d in 6502 — no jsr)
    advance_to_next_line();
    //     (c9a8d/c9aa5 merged into advance_to_next_line; return directly to
    //     caller)
    goto c9aa5;
    // c9aa5:
    // PROVISIONAL: Cleanup — clear overflow flag, load l007e into A (sets Z for
    // caller).
c9aa5:
    //     clv
    flags &= ~FLAG_V;
    //     lda l007e
    a = l007e;
    set_flags(&flags, a);
    //     rts
    return;
}

static void sub_c9ac1(void)
{
    // sub_c9ac1
    //  Inputs: y
    //  Outputs: a, x, flags:C; tmp45, tmp89
    // Pseudocode: Finds next word boundary for line wrapping, returns carry if
    // found

    // sub_c9ac1:
    //     tya
    a = y;
    //     sec
    flags |= FLAG_C;
    //     adc current_line_ptr
    a = adc(&flags, a, (uint16_t)(uint8_t)(current_line_ptr & 0xff));
    //     sta tmp8
    tmp8 = a;
    //     sta tmp4
    tmp4 = a;
    //     lda current_line_ptr+1
    a = (uint8_t)(current_line_ptr >> 8);
    //     adc #0
    a = adc(&flags, a, 0);
    //     sta tmp9
    tmp9 = a;
    //     sta tmp5
    tmp5 = a;
    //     ldy #0
    y = 0;
    //     sty l0083
    l0083 = y;
    // c9ad5:
c9ad5:
    //     lda (tmp4),y
    a = ram[tmp45 + y];
    //     beq c9b2f
    if (a == 0)
        goto c9b2f;
    //     jsr check_for_command_prefix
    flags = check_for_command_prefix(a);
    //     beq c9b2f
    if (flags & FLAG_Z)
        goto c9b2f;
    //     cmp #0x0d
    if (a == 0x0d)
        goto c9b2f;
    //     beq c9b2f
    //     tya
    a = y;
    //     bne c9b06
    if (a != 0)
        goto c9b06;
    //     sty l0084
    l0084 = y;
    //     beq c9aef                                                         ;
    //     ALWAYS branch
    goto c9aef;

    // c9ae9:
c9ae9:
    //     inc tmp8
    tmp8++;
    //     bne c9aef
    if (tmp8 != 0)
        goto c9aef;
    //     inc tmp9
    tmp9++;
    // c9aef:
c9aef:
    //     lda (tmp8),y
    a = ram[tmp89 + y];
    //     beq c9b06
    if (a == 0)
        goto c9b06;
    //     cmp #0x0d
    if (a == 0x0d)
        goto c9b06;
    //     beq c9b06
    //     cmp #9
    if (a == 9)
        goto c9b2f;
    //     beq c9b2f
    //     cmp #0x0b
    cmp(&flags, a, 0x0b);
    //     bne c9ae9
    if (!(flags & FLAG_Z))
        goto c9ae9;
    //     rol l0084
    l0084 = rol(&flags, l0084);
    //     sec
    flags |= FLAG_C;
    //     ror l0084
    l0084 = ror(&flags, l0084);
    //     bcs c9ae9
    if (flags & FLAG_C)
        goto c9ae9;
    // c9b06:
c9b06:
    //     lda (tmp4),y
    a = ram[tmp45 + y];
    //     cmp #0x20 ; ' '
    if (a != 0x20)
        goto c9b1a;
    //     bne c9b1a
    //     ldx ruler_left_stop
    x = ruler_left_stop;
    //     beq c9b2f
    if (x == 0)
        goto c9b2f;
    //     ldx l0084
    x = l0084;
    //     beq c9b2f
    if (x == 0)
        goto c9b2f;
    //     ldx l0083
    x = l0083;
    //     bne c9b2f
    if (x != 0)
        goto c9b2f;
    //     beq c9b20                                                         ;
    //     ALWAYS branch
    goto c9b20;

    // c9b1a:
c9b1a:
    //     cmp #0x0b
    cmp(&flags, a, 0x0b);
    //     bne c9b23
    if (!(flags & FLAG_Z))
        goto c9b23;
    //     sta l0083
    l0083 = a;
    // c9b20:
c9b20:
    //     iny
    y++;
    //     bne c9ad5
    if (y != 0)
        goto c9ad5;
    // c9b23:
c9b23:
    //     lda ruler_left_stop
    a = ruler_left_stop;
    //     beq c9b31
    if (a == 0)
        goto c9b31;
    //     lda l0084
    a = l0084;
    //     beq c9b31
    if (a == 0)
        goto c9b31;
    //     lda l0083
    a = l0083;
    //     bne c9b31
    if (a != 0)
        goto c9b31;
    // c9b2f:
c9b2f:
    //     sec
    flags |= FLAG_C;
    //     rts
    return;

    // c9b31:
c9b31:
    //     clc
    flags &= ~FLAG_C;
    //     rts
    return;
}

static void sub_c9e22(void)
{
    // sub_c9e22:
    //     pha
    {
        uint8_t saved_a = a;
        //     ldx #1
        x = 1;
        //     jsr insert_edit_buffer_bytes_at_xpos
        insert_edit_buffer_bytes_at_xpos();
        //     pla
        a = saved_a;
    }
    //     bcs return_55
    if (flags & FLAG_C)
        return;
    //     sta (current_edit_line_ptr),y
    ram[current_edit_line_ptr + y] = a;
    //     inc l0074
    l0074++;
    // return_55:
    //     rts
    return;
}
// MULTIPLE ENTRY POINTS: tab_key, sf4_highlight1_key, sf5_highlight2_key

static void sub_c9e9b(void)
{
    // Shared code: gets line length and sets xpos
    // c9e9b:
    //     jsr get_line_length
    get_line_length();
    //     sty xpos
    xpos = y;
    //     rts
}

static void sub_ca44e(void)
{
    // sub_ca44e
    //  Outputs: a, x
    // sub_ca44e: Computes starting line for display based on screen position

    //     lda l0034
    a = l0034;
    //     sta ruler_stack_ptr
    ruler_index_ptr = a;
    //     lda screen_height
    a = screen_maxrow;
    //     sta l0073
    l0073 = a;
    //     lsr
    a >>= 1;
    //     tax
    x = a;
    //     inx
    x++;
    //     lda l006f
    a = l006f;
    //     bmi ca461
    if (a & 0x80)
        goto ca461;
    //     beq ca461
    if (a == 0)
        goto ca461;
    //     ldx ypos
    x = ypos;
    // ca461:
ca461:
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     ldy current_line_ptr+1
    y = (uint8_t)(current_line_ptr >> 8);
    // loop_ca465:
loop_ca465:
    //     dex
    x--;
    //     beq ca479
    if (x == 0)
        goto ca479;
    //     sta tmp2
    tmp23 = (addr_t)(y) << 8 | a;
    //     jsr sub_cab37
    move_tmp01_to_previous_line();
    //     lda tmp0
    a = tmp0;
    //     ldy tmp1
    y = tmp1;
    //     bcs loop_ca465
    if (flags & FLAG_C)
        goto loop_ca465;
    //     lda tmp2
    a = tmp2;
    //     ldy tmp3
    y = tmp3;
    // ca479:
ca479:
    //     sta l0011
    //     sty l0012
    top_of_screen_line_ptr = ((addr_t)y << 8) | a;
    //     lda ruler_stack_ptr
    a = ruler_index_ptr;
    //     sta l0033
    l0033 = a;
    //     lda l0034
    a = l0034;
    //     sta ruler_stack_ptr
    ruler_index_ptr = a;
    //     rts
}

static void sub_ca4d7(void)
{
    // sub_ca4d7: Draws a character and advances x position

    //     jsr process_current_document_character
    process_current_document_character();
    //     jmp ca4e9
    render_char();
}

static void sub_ca536(void)
{
    // sub_ca536
    //  Inputs: y
    //  Outputs: a, flags:Z
    // Pseudocode: Checks if a position in the edit line corresponds to a marker

    // sub_ca536:
    tmp89 = tmp67 + y;
    //     ldx #0
    x = 0;
    // loop_ca544:
loop_ca544:
    //     lda tmp9
    a = tmp9;
    //     cmp markers_array+1,x
    cmp(&flags, a, ((uint8_t*)markers_array)[x + 1]);
    //     bne ca550
    if (!(flags & FLAG_Z))
        goto ca550;
    //     lda tmp8
    a = tmp8;
    //     cmp markers_array,x
    cmp(&flags, a, ((uint8_t*)markers_array)[x]);
    //     beq ca558
    if (flags & FLAG_Z)
        goto ca558;
    // ca550:
ca550:
    //     inx
    x++;
    //     inx
    x++;
    //     cpx #0x0c
    if (x != 0x0c)
        goto loop_ca544;
    //     bne loop_ca544
    //     txa
    a = x;
    set_flags(&flags, a);
    //     rts
    return;

    // ca558:
ca558:
    //     lda #0
    a = 0;
    set_flags(&flags, 0);
    // return_61:
    //     rts
    return;
}

static void sub_caa97(void)
{
    // sub_caa97
    //  Outputs: a, y
    // sub_caa97:
    //     lda #0x10
    a = 0x10;
    //     jsr wipe_buffer
    wipe_buffer();
    //     jsr sub_caf5f
    sub_caf5f();
    //     ldy #0
    y = 0;
    //     lda (current_line_ptr),y
    a = ram[current_line_ptr + y];
    //     ldx current_edit_line_ptr
    x = (uint8_t)(current_edit_line_ptr & 0xff);
    //     ldy current_edit_line_ptr+1
    y = (uint8_t)((current_edit_line_ptr >> 8) & 0xff);
    //     jsr check_for_command_prefix
    flags = check_for_command_prefix(a);
    //     bne caab7
    if (!(flags & FLAG_Z))
        goto caab7;
    //     bcs caab0
    if (flags & FLAG_C)
        goto caab0;
    //     sta l006e
    edit_buffer_unpacked_flag = a;
    // caab0:
caab0:
    //     jsr caf5c
    caf5c();
    //     ldx ptr1
    x = (uint8_t)(ptr1 & 0xff);
    //     ldy ptr1+1
    y = (uint8_t)((ptr1 >> 8) & 0xff);
    // caab7:
caab7:
    //     stx current_format_line_ptr
    current_format_line_ptr = (current_format_line_ptr & 0xff00) | x;
    //     sty current_format_line_ptr+1
    current_format_line_ptr =
        (current_format_line_ptr & 0x00ff) | ((uint16_t)y << 8);
    //     ldy #0
    y = 0;
    // loop_caabd:
loop_caabd:
    //     lda (current_line_ptr),y
    a = ram[current_line_ptr + y];
    //     cmp #0x0d
    if (a == 0x0d)
        goto caac8;
    //     beq caac8
    //     sta (current_format_line_ptr),y
    ram[current_format_line_ptr + y] = a;
    //     iny
    y++;
    //     bne loop_caabd
    if (y != 0)
        goto loop_caabd;
    // caac8:
caac8:
    //     sty l003b
    l003b = y;
    // return_68:
    //     rts
}

static void sub_caacb(void)
{
    // sub_caacb: Updates marker positions to point into format buffer instead
    // of document buffer

    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     sta tmp6
    tmp6 = a;
    //     lda current_line_ptr+1
    a = (uint8_t)(current_line_ptr >> 8);
    //     sta tmp7
    tmp7 = a;
    //     ldy #0
    y = 0;
    // caad5:
caad5:
    //     jsr sub_ca536
    sub_ca536();
    //     bne caae8
    if (!(flags & FLAG_Z))
        goto caae8;
    //     tya
    a = y;
    //     clc
    flags &= ~FLAG_C;
    //     adc current_format_line_ptr
    a = adc(&flags, a, (uint8_t)(current_format_line_ptr & 0xff));
    //     sta __begin_pointer_array,x
    ((uint8_t*)markers_array)[x] = a;
    //     lda current_format_line_ptr+1
    a = (uint8_t)(current_format_line_ptr >> 8);
    //     adc #0
    a = adc(&flags, a, 0);
    //     sta markers_array+1,x
    ((uint8_t*)markers_array)[x + 1] = a;
    //     bne caad5
    if (!(flags & FLAG_Z))
        goto caad5;
    // caae8:
caae8:
    //     lda (current_line_ptr),y
    a = ram[current_line_ptr + y];
    //     cmp #0x0d
    cmp(&flags, a, 0x0d);
    //     beq return_68
    if (flags & FLAG_Z)
        return;
    //     iny
    y++;
    //     bne caad5
    if (y != 0)
        goto caad5;
    // return_68:
    //     rts
}

void sub_cac41(void)
{
    // Pseudocode: Pushes ruler stack before entering a new ruler region

    // sub_cac41:
    //     pha
    //     tya
    //     pha
    {
        uint8_t saved_a = a;
        uint8_t saved_y = y;
        //     jsr sub_cab6e
        sub_cab6e();
        //     bne cac4c
        if (!(flags & FLAG_Z))
            goto cac4c;
        //     jsr push_onto_ruler_stack
        push_onto_ruler_index();
        // cac4c:
    cac4c:
        //     pla
        //     tay
        //     pla
        y = saved_y;
        a = saved_a;
    }
    //     rts
    return;
}

static void sub_cac50(void)
{
    // sub_cac50
    //  Inputs: -
    // Pseudocode: Finds the start of current line by scanning backward for CR

    // sub_cac50:
    //     sec
    flags |= FLAG_C;
    //     sbc #1
    {
        uint8_t old_a = a;
        a = a - 1 - (1 - (flags & FLAG_C));
        flags = (flags & ~(FLAG_Z | FLAG_N | FLAG_C)) | (a == 0 ? FLAG_Z : 0) |
                (a & FLAG_N) | (old_a >= 1 ? FLAG_C : 0);
    }
    //     sta tmp8
    tmp8 = a;
    //     bcs cac58
    if (!(flags & FLAG_C))
        y--;
    // cac58:
cac58:
    //     sty tmp9
    tmp9 = y;
    //     ldy #0
    y = 0;
    // cac5c:
cac5c:
    //     lda (tmp8),y
    a = ram[tmp89 + y];
    //     cmp #0x0d
    cmp(&flags, a, 0x0d);
    //     beq cac6f
    if (flags & FLAG_Z)
        goto cac6f;
    //     lda tmp8
    //     sec
    //     sbc #1
    //     sta tmp8
    //     bcs cac5c
    //     dec tmp9
    //     bne cac5c
    flags |= FLAG_C;
    {
        uint8_t old_a = tmp8;
        a = tmp8 - 1 - (1 - (flags & FLAG_C));
        flags = (flags & ~(FLAG_Z | FLAG_N | FLAG_C)) | (a == 0 ? FLAG_Z : 0) |
                (a & FLAG_N) | (old_a >= 1 ? FLAG_C : 0);
    }
    tmp8 = a;
    if (flags & FLAG_C)
        goto cac5c;
    tmp9--;
    if (tmp9 != 0)
        goto cac5c;
    // cac6f:
cac6f:
    //     lda tmp8
    //     sta tmp6
    //     lda tmp9
    //     sta tmp7
    tmp6 = tmp8;
    tmp7 = tmp9;
    // return_73:
    //     rts
    return;
}

static void sub_cae03(void)
{
    // cae03:
    //     jmp beep
    beep();
}

static void sub_caec2(void)
{
    // sub_caec2: Finds left margin stop (0x0b) in edit line

    //     lda ruler_left_stop
    a = ruler_left_stop;
    set_flags(&flags, a);
    //     beq caed4
    if (flags & FLAG_Z)
        goto caed4;
    //     ldy #0
    y = 0;
    // loop_caec8:
loop_caec8:
    //     lda (current_edit_line_ptr),y
    a = ram[current_edit_line_ptr + y];
    //     iny
    y++;
    //     cmp #0x0b
    if (a == 0x0b)
        goto caed4;
    //     beq caed4
    //     cpy #0x84
    if (y < MAX_LINE_LENGTH)
        goto loop_caec8;
    //     bcc loop_caec8
    //     rts
    return;

    // caed4:
caed4:
    //     clc
    flags &= ~FLAG_C;
    //     rts
}

static void sub_caed6(void)
{
    // sub_caed6:
    //     jsr sub_caec2
    sub_caec2();
    //     bcc caed4
    if (flags & FLAG_C)
    {
        //     ldy #0
        y = 0;
        sub_caedd();
    }
    // caed4:
    //     rts
}

static void sub_caedd(void)
{
    // sub_caedd:
    //     lda xpos
    a = xpos;
    //     pha
    {
        uint8_t saved_a = a;
        //     sty xpos
        xpos = y;
        //     ldx #1
        x = 1;
        //     jsr insert_edit_buffer_bytes_at_xpos
        insert_edit_buffer_bytes_at_xpos();
        //     bcs caef0
        if (!(flags & FLAG_C))
        {
            //     ldy xpos
            y = xpos;
            //     lda #0x0b
            a = 0x0b;
            //     sta (current_edit_line_ptr),y
            ram[current_edit_line_ptr + y] = a;
            //     iny
            y++;
        }
        // caef0:
        //     pla
        a = saved_a;
    }
    //     sta xpos
    xpos = a;
    //     rts
}

static void unpack_line_into_buffer(void)
{
    // unpack_line_into_buffer:
    //     lda l006e
    //     bne return_68
    if (edit_buffer_unpacked_flag != 0)
        return;
    //     lda #1
    //     sta l006e
    edit_buffer_unpacked_flag = 1;
    sub_caa97();
}

void wipe_buffer(void)
{
    // wipe_buffer:
    //     ldy #0
    y = 0;
    //     ldx #0x89
    x = 0x89;
    // loop_cab13:
    //     sta (ptr1),y
    //     iny
    //     dex
    //     bne loop_cab13
    do
    {
        ram[ptr1 + y] = a;
        y++;
        x--;
    } while (x != 0);
    //     rts
}

static void write_line_back_to_document(void)
{
    // write_line_back_to_document
    //  Inputs: -
    //  Outputs: a, x, y, flags:C; tmp45, tmp67
    // sub_ca8b9:
    // write_line_back_to_document:
    //     lda l006e
    //     beq ca93a
    a = edit_buffer_unpacked_flag;
    set_flags(&flags, a);
    if (flags & FLAG_Z)
        goto ca93a;
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     sta tmp4
    tmp4 = a;
    //     lda current_line_ptr+1
    a = (uint8_t)(current_line_ptr >> 8);
    //     sta tmp5
    tmp5 = a;
    //     ldy #0
    y = 0;
    //     sty tmp7
    tmp7 = y;
    //     jsr get_line_length
    get_line_length();
    //     sta l0083
    l0083 = a;
    //     lda l003b
    a = l003b;
    //     sec
    flags |= FLAG_C;
    //     sbc l0083
    a = sbc(&flags, a, l0083);
    //     bcc ca8df
    if (!(flags & FLAG_C))
        goto ca8df;
    //     beq ca8ed
    if (flags & FLAG_Z)
        goto ca8ed;
    //     sta tmp6
    tmp6 = a;
    //     jsr adjust_pointers
    adjust_pointers();
    //     jmp ca8ed
    goto ca8ed;

    // ca8df:
ca8df:
    //     sta l0084
    l0084 = a;
    //     lda #0
    a = 0;
    //     sec
    //     sbc l0084
    a -= l0084;
    //     sta tmp6
    tmp6 = a;
    //     jsr make_space_for_insertion
    make_space_for_insertion();
    //     bcs return_66
    if (flags & FLAG_C)
        return;

    // ca8ed:
ca8ed:
    //     lda l006e
    a = edit_buffer_unpacked_flag;
    if (!((int8_t)a < 0))
        goto ca8f8;
    //     bpl ca8f8
    //     lda l006d
    a = edit_buffer_dirty_flag;
    set_flags(&flags, a);
    //     beq ca8f8
    if (flags & FLAG_Z)
        goto ca8f8;
    //     jsr ca741
    ca741();

    // ca8f8:
ca8f8:
    //     ldy #0
    y = 0;
    //     sty l006d
    edit_buffer_dirty_flag = y;
    //     sty l006e
    edit_buffer_unpacked_flag = y;
    //     lda current_format_line_ptr
    a = (uint8_t)(current_format_line_ptr & 0xff);
    //     sta tmp6
    tmp6 = a;
    //     lda current_format_line_ptr+1
    a = (uint8_t)(current_format_line_ptr >> 8);
    //     sta tmp7
    tmp7 = a;
    //     ldx l0083
    x = l0083;
    //     stx l003b
    l003b = x;

    // ca90a:
ca90a:
    //     txa
    a = x;
    if (a != 0)
        goto ca911;
    //     bne ca911
    //     lda #0x0d
    a = 0x0d;
    //     bne ca919
    goto ca919;

    // ca911:
ca911:
    //     lda (current_format_line_ptr),y
    a = ram[current_format_line_ptr + y];
    //     cmp #0x10
    if (a != 0x10)
        goto ca919;
    //     bne ca919
    //     lda #0x20 ; ' '
    a = 0x20;

    // ca919:
ca919:
    //     pha
    {
        uint8_t saved_a = a;
        //     txa
        a = x;
        //     pha
        {
            uint8_t saved_x = a;

            // loop_ca91c:
        loop_ca91c:
            //     jsr sub_ca536
            sub_ca536();
            //     bne ca92f
            if (a != 0)
                goto ca92f;
            //     tya
            a = y;
            //     clc
            flags &= ~FLAG_C;
            //     adc current_line_ptr
            a = adc(&flags, a, (uint8_t)(current_line_ptr & 0xff));
            //     sta markers_array,x
            ((uint8_t*)markers_array)[x] = a;
            //     lda current_line_ptr+1
            a = (uint8_t)(current_line_ptr >> 8);
            //     adc #0
            a = adc(&flags, a, 0);
            //     sta markers_array+1,x
            ((uint8_t*)markers_array)[x + 1] = a;
            //     bne loop_ca91c
            if (a != 0)
                goto loop_ca91c;

            // ca92f:
        ca92f:
            //     pla
            a = saved_x;
        }
        //     tax
        x = a;
        //     pla
        a = saved_a;
    }
    //     sta (current_line_ptr),y
    ram[current_line_ptr + y] = a;
    //     iny
    y++;
    //     dex
    x--;
    //     cmp #0x0d
    cmp(&flags, a, 0x0d);
    //     bne ca90a
    if (!(flags & FLAG_Z))
        goto ca90a;

    // ca93a:
ca93a:
    //     clc
    flags &= ~FLAG_C;
    // return_66:
    //     rts
    return;
}
// MULTIPLE ENTRY POINTS: write_line_back_to_document_safely (via memory_full)

void write_line_back_to_document_safely(void)
{
    // write_line_back_to_document_safely: Write back edit buffer. If out of
    // memory (C=1), fall through to memory_full.
    //     jsr write_line_back_to_document
    write_line_back_to_document();
    //     bcc return_66
    if (!(flags & FLAG_C))
        return;
    //     falls through to memory_full
    memory_full();
}
