#include "editor.h"

#include "document.h"

#include "io.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "globals.h"

// Render pipeline state.  The line-rendering routines (draw_line, render_char,
// render_xchar, advance_to_next_char_and_render) used to keep their working
// state in the simulated registers; it now lives in this struct, threaded
// through the pipeline.
struct render_state
{
    addr_t line_ptr;    // tmp01: pointer into the current edit line
    uint8_t pos;        // y: position in the edit line
    uint8_t col;        // l0083: current screen column
    uint8_t line;       // l0082: screen line number
    uint8_t width;      // l0084: character width / render counter
    uint8_t buf_off;    // l0080: input buffer offset
    uint8_t char_width; // l0039: width accumulator
    uint8_t ch;         // a: current character
    bool prev_is_tab;   // whether the previous character was a tab expansion
};

// Editor-only functions
addr_t adjust_pointers(addr_t tmp45, addr_t tmp67);
static bool advance_to_next_doc_line(void);
void beep(void);
bool scan_document_for_next_line(void);
static void insert_line_into_document(addr_t ptr);
static void update_line_length(void);
void clamp_ptr6_to_document(void);
void clear_screen(void);
static void clear_to_eol(uint8_t a, uint8_t line);
static void cursor_off(void);
static void cursor_on(void);
void draw_line(struct render_state* rs, uint16_t addr);
void draw_prompt_characters(uint8_t x, uint8_t y);
static void draw_ruler(void);
static void draw_status_word(void);
static uint8_t get_line_length(void);
static void go_to_marker(uint8_t x);
static void go_to_marker_n(uint8_t marker);
static void home_cursor(void);
uint8_t justify_edit_buffer(addr_t ptr1);
bool make_space_for_insertion(addr_t tmp45, addr_t tmp67);
static void memory_full(void);
uint8_t process_current_document_character(
    addr_t tmp01, uint8_t* x, uint8_t* y, bool* is_tab);
static void recalculate_cursor_xpos(void);
void redraw_editor(void);
static void render_char(struct render_state* rs);
static void advance_to_next_char(struct render_state* rs);
static void render_xchar(struct render_state* rs);
static void restore_cursor_position(addr_t tmp45);
area_status_t sanitise_area(void);
static void save_cursor_position(void);
static void set_marker(uint8_t x);
static void set_marker_common(uint8_t a);
void show_memory_full_error(void);
void adjust_area_pointers(addr_t tmp67);
static void append_to_output_buffer(uint8_t a);
uint8_t upper_case_unless_folding(uint8_t a);
static bool process_char_for_output(
    uint8_t y, bool carry_in, uint8_t* x, uint8_t* a);
format_result_t format_paragraph(void);
static bool find_next_word_boundary(uint8_t y);
static bool insert_character_into_edit_buffer(uint8_t a);
static void set_xpos_to_line_length(void);
static uint8_t compute_display_start_line(void);
static void advance_to_next_char_and_render(struct render_state* rs);
static uint8_t find_marker_at_position(uint8_t y, addr_t tmp67);
static void unpack_line(addr_t ptr1);
static void update_markers_to_format_buffer(void);
void check_for_embedded_ruler(addr_t tmp01);
static addr_t find_line_start(addr_t tmp89);
static int find_left_margin_stop(void);
static void insert_at_left_margin(void);
static bool insert_byte_at_xpos(uint8_t y);
static void unpack_line_into_buffer(addr_t ptr1);
void wipe_buffer(uint8_t a, addr_t ptr1);
static bool write_line_back_to_document(void);
void write_line_back_to_document_safely(void);

void enter_editor_mode(void);
void clear_format_mode_bit7(void);
void set_format_mode_bit7(void);
void draw_previous_word(
    uint8_t* word_boundary, bool* is_start_of_line, uint8_t* char_width);
bool adjust_margins_at_left_margin(void);
bool insert_edit_buffer_bytes_at_xpos(uint8_t x);
void set_marker_to_here(uint8_t x);
void split_line_at_wrap(addr_t tmp89);

// Editor-internal helper functions

static void advance_current_line_pointer(void);

static void clear_marks_1_2(void);

static uint8_t control_key_to_ascii(uint8_t a);

static void delete_edit_buffer_bytes_at_xpos(uint8_t x);

static void enter_printable_character(void);

static int prompt_for_marker(void);

static bool reset_area_to_marks_1_2(void);

static void insert_line_at_cursor(addr_t ptr);

static void move_to_previous_line(void);

static void move_cursor_up(uint8_t x);

static void move_cursor_down(uint8_t x);

static void check_pointer_in_area(void);

static void tab_highlight_common(uint8_t a);

// Forward declarations for key handler functions

static void cf0_delete_block_key(void);

static void cf1_next_match_key(void);

static void cf2_format_mode_key(void);

static void cf3_justify_mode_key(void);

static void cf4_insert_mode_key(void);

static void cf5_default_ruler_key(void);

static void cf6_split_line_key(void);

static void cf7_join_lines_key(void);

static void cf8_mark_as_ruler_key(addr_t ptr1);

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

static void sf8_edit_command_key(addr_t ptr1);

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

            uint8_t a = edit_buffer_unpacked_flag;

            //     bne c9b44

            if (a == 0)
            {
                uint8_t a_1;

                {
                    uint8_t saved_a_ = a;
                    unpack_line(ptr1);
                    a_1 = saved_a_;
                }
                edit_buffer_unpacked_flag = a_1;
            }

            //     jsr sub_ca608

            recalculate_cursor_xpos();

            //     lda ruler_left_stop

            uint8_t a_2 = ruler_left_stop;

            //     beq c9b73

            if (a_2 == 0)
                goto c9b73_;

            //     ldx format_mode_flag

            uint8_t x = format_mode_flag;

            //     bmi c9b73

            if (x & 0x80)
                goto c9b73_;

            //     cmp l0072

            if (a_2 <= l0072)
                goto c9b73_;

            //     ldx cursor_moved_flag

            uint8_t x_1 = cursor_moved_flag;

            //     bne c9b6a

            if (x_1 != 0)
                goto c9b6a_;

            //     jsr get_line_length

            uint8_t line_len = get_line_length();

            //     lda format_mode_flag

            uint8_t a_3 = format_mode_flag;

            //     cpy xpos

            if (line_len >= xpos)
                goto c9b84_;

            //     bcs c9b84

            //     bit format_mode_flag

            //     bvs c9b6a
            if (format_mode_flag & 0x40)
                goto c9b6a_;

            //     sty xpos
            // (the 6502's Y holds the line length from get_line_length)
            xpos = line_len;

            //     bvc c9b84 ; ALWAYS branch

            goto c9b84_;

        c9b6a_:

            //     lda ruler_left_stop

            uint8_t a_4 = ruler_left_stop;

            //     sta l0072

            l0072 = a_4;

            //     inc l0079

            l0079++;

            //     jsr sub_ca608

            recalculate_cursor_xpos();

            //     lda format_mode_flag

            uint8_t a_5 = format_mode_flag;

            //     and #0xbf

            a_5 &= 0xbf;

            //     pha

            {

                uint8_t saved_mod = a_5;

                //     jsr find_left_margin_stop

                int y = find_left_margin_stop();

                //     pla

                a_3 = saved_mod;

                //     bcs c9b86

                if (y < 0)
                    goto c9b86_;

                //     cpy xpos

                if (y <= xpos)
                    goto c9b86_;
            }

            //     ora #0x40 ; '@'

            a_3 |= 0x40;

        c9b84_:

            // c9b86:

            //     sta format_mode_flag

            format_mode_flag = a_3;

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

        //     jsr read_char

        uint8_t a_6 = screen_getchar();

        //     cmp current_tab_key

        if (a_6 == current_tab_key)
        {
            a_6 = 9;
        }

        //     sta l0038

        l0038 = a_6;

        //     tay
        // (dead in the C translation: Y is reloaded by the key handlers)

        //     bmi c9bbb  ; omitted to support high-bit control characters

        // if (a & 0x80) goto editor_loop;

        //     cmp #0x20 ; ' '

        if (a_6 < 0x20)
            goto enter_nonprintable_character;

        //     bcc enter_nonprintable_character

        //     cmp #0x7f

        //     bcc enter_printable_character
        if (a_6 < 0x7f)
        {
            enter_printable_character();
            goto editor_loop;
        }

    enter_nonprintable_character:

        switch (a_6)
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

    write_line_back_to_document_safely();

    cursor_moved_flag++;

    if (reset_area_to_marks_1_2())
    {
        beep();
        return;
    }

    move_cursor_to_address(area_start_ptr);

    clamp_ptr6_to_document();

    adjust_area_pointers(tmp67);

    ensure_cr_at_document_top();

    clear_marks_1_2();
}

static void cf1_next_match_key(void)
{
    // cf1_next_match_key

    write_line_back_to_document_safely();

    if (!scan_document_for_next_line())
    {
        esc_key();
        return;
    }

    move_cursor_to_address(ptr2);
}

static void cf2_format_mode_key(void)
{

    uint8_t a = format_mode_flag;

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

    // cf5_default_ruler_key:

    //     jsr f6_insert_line_key

    f6_insert_line_key();

    //     jsr sub_ca276

    redraw_editor();

    //     jsr cf8_mark_as_ruler_key

    cf8_mark_as_ruler_key(ptr1);

    //     lda current_edit_line_ptr

    create_default_ruler(RAM_EDIT_BUFFER);
}

static void cf6_split_line_key(void)
{

    // cf6_split_line_key

    // cf6_split_line_key: Splits line at cursor position

    //     jsr write_line_back_to_document_safely

    write_line_back_to_document_safely();

    //     jsr get_line_length

    uint8_t line_len = get_line_length();

    //     cpy xpos
    // (get_line_length leaves Y = line length; bcc keeps it when Y < xpos)

    uint8_t y = line_len;
    if (line_len >= xpos)
    {
        y = xpos;
    }

    //     inc l0079

    l0079++;

    //     tya

    uint8_t a = y;

    //     tax

    uint8_t x = a;

    //     ldy #0

    uint8_t y_1 = 0;

    //     lda (current_format_line_ptr),y
    // (Z from this lda is clobbered by the following jsr)
    uint8_t a_1 = ram[current_format_line_ptr + y_1];

    //     jsr check_for_command_prefix

    command_prefix_t cp = check_for_command_prefix(a_1);

    //     bne c9dcd

    if (cp != NO_COMMAND_PREFIX)
    {
        x++;
        x++;
        x++;
    }

    //     txa
    //     ldy current_line_ptr+1
    //     txa
    //     clc
    //     adc current_line_ptr
    //     bcc c9de3
    //     iny
    //     bne c9de3
    // (16-bit arithmetic: current_line_ptr + x; if the sum wraps past
    //  0xffff (y == 0 after the carry), fall through to f6_insert_line_key;
    //  the preceding ldy current_line_ptr+1 high-byte load has no C
    //  counterpart — the sum and its wrap are computed as uint32_t below)
    uint32_t sum = (uint32_t)current_line_ptr + x;
    if (sum > 0xffff)
    {
        f6_insert_line_key();
        return;
    }
    insert_line_into_document((uint16_t)sum);
    return;
}

// MULTIPLE ENTRY POINTS: cf6_split_line_key, f6_insert_line_key, sub_c9de1

static void cf7_join_lines_key(void)
{

    // cf7_join_lines_key

    // cf7_join_lines_key: Joins current line with next line

    //     jsr write_line_back_to_document_safely

    write_line_back_to_document_safely();

    //     lda current_line_ptr

    //     sta ((uint8_t*)&tmp01)[0]

    //     lda current_line_ptr+1

    //     sta ((uint8_t*)&tmp01)[1]

    //     jsr sub_cab6e

    addr_t tmp01;
    uint8_t y;
    if (find_next_line(current_line_ptr, &tmp01, &y))
    {
        beep();
        return;
    }

    //     beq c9eda

    //     jsr check_for_command_prefix
    // (find_next_line leaves the byte after the CR at ram[tmp01 + y])
    command_prefix_t cp = check_for_command_prefix(ram[tmp01 + y]);

    //     beq c9eda

    if (cp != NO_COMMAND_PREFIX)
    {
        beep();
        return;
    }

    //     dey

    y--;

    //     tya

    //     clc

    //     adc current_line_ptr

    addr_t tmp45 = current_line_ptr + y;

    //     lda #0

    //     sta ((uint8_t*)&tmp67)[1]

    //     lda #1

    //     sta ((uint8_t*)&tmp67)[0]

    //     jsr adjust_pointers

    tmp89 = adjust_pointers(tmp45, 1);

    //     lda current_line_ptr
    //     ldy current_line_ptr+1
    //     jsr cac78
    split_line_at_wrap(current_line_ptr);
    //     inc l0079

    l0079++;

    //     jmp ca741

    clamp_ptr6_to_document();
    return;

    // c9eda:

    //     jmp beep
}

static void cf8_mark_as_ruler_key(addr_t ptr1)
{
    // cf8_mark_as_ruler_key
    //  Ptrs:   ptr1

    // cf8_mark_as_ruler_key: Marks current line as a ruler line with . as
    // default characters

    //     lda ptr1

    //     sta current_format_line_ptr

    current_format_line_ptr = ptr1;

    //     lda ptr1+1

    //     sta current_format_line_ptr+1

    //     ldy #0

    uint8_t y = 0;

    //     lda #0x81

    uint8_t a = 0x81;

    //     sta (current_format_line_ptr),y

    ram[current_format_line_ptr + y] = a;

    //     iny

    y++;

    //     lda #0x2e ; '.'

    uint8_t a_1 = 0x2e;

    //     sta (current_format_line_ptr),y

    ram[current_format_line_ptr + y] = a_1;

    //     iny

    y++;

    //     lda #0x2e ; '.'

    uint8_t a_2 = 0x2e;

    //     sta (current_format_line_ptr),y

    ram[current_format_line_ptr + y] = a_2;

    //     inc l0074

    l0074++;

    //     lda l006e

    uint8_t a_3 = edit_buffer_unpacked_flag;

    //     bmi c9f5f

    if (!(a_3 & 0x80))
    {
        edit_buffer_unpacked_flag = 0x80;
        edit_buffer_dirty_flag++;
    }

    //     jmp caf5c

    set_format_mode_bit7();
    return;
}

static void delete_key(void)
{
    uint8_t a_2;

    // delete_key

    // delete_key:

    //     lda l0072

    uint8_t a = l0072;

    //     beq return_55

    if (a == 0)
        return;

    //     dec xpos

    xpos--;

    //     ldy xpos

    uint8_t y = xpos;

    //     lda (current_edit_line_ptr),y

    uint8_t a_1 = ram[RAM_EDIT_BUFFER + y];

    //     pha

    {
        uint8_t saved_a = a_1;

        //     jsr f9_delete_char_key

        f9_delete_char_key();

        //     pla

        a_2 = saved_a;
    }

    //     cmp #0x0c

    //     bcc return_55
    if (a_2 < 0x0c)
        return;

    //     ldx insert_mode_flag

    uint8_t x = insert_mode_flag;

    if (x != 0)
        return;

    //     bne return_55

    //     jsr get_line_length

    uint8_t line_len = get_line_length();

    //     cpy xpos

    //     bcc return_55
    if (line_len < xpos)
        return;
    //     beq return_55
    if (line_len == xpos)
        return;

    //     falls through to f8_insert_char_key

    f8_insert_char_key();
}

// MULTIPLE ENTRY POINTS: delete_key, f8_insert_char_key

void esc_key(void)
{

    // esc_key

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

    //     pha

    uint8_t saved_l0073 = l0073;

    //     lda l003d

    //     pha

    uint8_t saved_l003d = l003d;

    //     jsr ca741

    clamp_ptr6_to_document();

    //     jsr sub_c9977
    //     bvs ca05b (V=1 conveyed as FORMAT_MEMORY_FULL)
    if (format_paragraph() == FORMAT_MEMORY_FULL)
    {
        show_memory_full_error();
        longjmp(env, JMP_EDITOR);
    }

    //     sec
    //     bne ca051
    //     bcs return_59
    // (the sec/clc dance just computes C = (line_format_status != 0))
    if (line_format_status == 0)
    {
        l003d = saved_l003d;
        l0073 = saved_l0073;
    }

    //     rts
}

static void f11_copy_key(void)
{
    // f11_copy_key

    write_line_back_to_document_safely();

    if (reset_area_to_marks_1_2())
    {
        beep();
        return;
    }

    check_pointer_in_area();

    move_cursor_to_address(doc_ptr1);
}

static void f12_left_key(void)
{

    // f12_left_key

    // f12_left_key: Moves cursor left by one position

    //     ldy l0072

    uint8_t y = l0072;

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

    // f13_right_key:

    //     ldy xpos

    uint8_t y = xpos;

    //     cpy #MAX_LINE_LENGTH

    //     bcs return_51
    if (y >= MAX_LINE_LENGTH)
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
        advance_current_line_pointer();
        return;
    }

    //     jsr return_key

    return_key();
}

static void f15_up_key(void)
{

    // f15_up_key

    // f15_up_key: Moves cursor to previous line, handling ruler stack

    //     jsr write_line_back_to_document_safely

    write_line_back_to_document_safely();

    //     lda current_line_ptr

    addr_t tmp01;
    if (!find_previous_line(current_line_ptr, &tmp01))
        return;

    //     bcc return_53

    current_line_ptr = tmp01;

    //     inc l0079

    l0079++;

    //     inc cursor_moved_flag

    cursor_moved_flag++;

    // return_53:
}

// c9d9b: Shared line-advance logic used by return_key and f14_down_key

static void f1_top_of_text_key(void)
{

    l006f = 0xff;

    move_cursor_up(0xff);

    unpack_line(ptr1);
}

static void f2_bottom_of_text_key(void)
{
    // f2_bottom_of_text_key

    // Pseudocode: Moves cursor to bottom of document

    // ;
    // ***************************************************************************************

    // f2_bottom_of_text_key:

    //     ldx #0xff

    //     stx l006f

    l006f = 0xff;

    //     jsr sub_ca0af

    move_cursor_down(0xff);

    //     jsr unpack_line

    unpack_line(ptr1);

    //     jmp c9e9b

    set_xpos_to_line_length();
}

static void f3_delete_to_eol_key(void)
{

    // f3_delete_to_eol_key

    // f3_delete_to_eol_key: Deletes from cursor to end of line

    //     lda #MAX_LINE_LENGTH

    uint8_t a = MAX_LINE_LENGTH;

    //     sec
    //     sbc xpos
    // (sbc with C=1 in is a plain subtraction)
    a -= xpos;

    uint8_t x = a;

    //     tax

    //     inc l0074

    l0074++;

    //     jmp delete_edit_buffer_bytes_at_xpos

    delete_edit_buffer_bytes_at_xpos(x);
    return;
}

static void f4_beginning_of_line_key(void)
{

    // f4_beginning_of_line_key:

    //     inc cursor_moved_flag

    cursor_moved_flag++;

    //     jmp c9e94

    xpos = 0;
    return;
}

static void f5_end_of_line_key(void)
{

    // Pseudocode: Moves cursor to end of current line

    // f5_end_of_line_key:

    //     inc cursor_moved_flag

    cursor_moved_flag++;

    // c9e9b:

    set_xpos_to_line_length();
}

static void f6_insert_line_key(void)
{

    // f6_insert_line_key:

    //     jsr write_line_back_to_document_safely

    write_line_back_to_document_safely();

    //     lda current_line_ptr
    //     ldy current_line_ptr+1
    //     inc l0079
    l0079++;
    //     falls through to sub_c9de1
    insert_line_at_cursor(current_line_ptr);
}

static void f7_delete_line_key(void)
{

    // f7_delete_line_key

    // f7_delete_line_key: Deletes current line and moves cursor up

    //     jsr write_line_back_to_document_safely

    write_line_back_to_document_safely();

    //     inc cursor_moved_flag

    cursor_moved_flag++;

    //     lda current_line_ptr

    //     sta ((uint8_t*)&tmp45)[0]

    //     lda current_line_ptr+1

    //     sta ((uint8_t*)&tmp45)[1]

    addr_t tmp45 = current_line_ptr;

    //     ldx l003b

    uint8_t x = l003b;

    //     inx

    x++;

    //     stx ((uint8_t*)&tmp67)[0]

    //     lda #0

    //     sta ((uint8_t*)&tmp67)[1]

    addr_t tmp67 = x;

    //     jsr adjust_pointers

    tmp89 = adjust_pointers(tmp45, tmp67);

    //     jsr cb05a

    ensure_cr_at_document_top();

    //     ldy #0

    uint8_t y = 0;

    //     lda (current_line_ptr),y

    uint8_t a = ram[current_line_ptr + y];

    if (a == 0)
    {

        addr_t tmp01;
        find_previous_line(current_line_ptr, &tmp01);
        current_line_ptr = tmp01;
    }

    //     inc l0079

    l0079++;

    //     jmp ca741

    clamp_ptr6_to_document();
    return;
}

// MULTIPLE ENTRY POINTS: sf2_release_margins_key, f4_beginning_of_line_key

static void f8_insert_char_key(void)
{

    // f8_insert_char_key:

    //     lda #0x20 ; ' '

    //     falls through to sub_c9e22

    insert_character_into_edit_buffer(0x20);
    return;
}

static void f9_delete_char_key(void)
{

    // f9_delete_char_key: Deletes character under cursor

    //     ldx #1

    //     inc l0074

    l0074++;

    //     jmp delete_edit_buffer_bytes_at_xpos

    delete_edit_buffer_bytes_at_xpos(1);
    return;
}

static void k_command_key(void)
{

    // k_command_key

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

    draw_prompt_characters('^', 'K');

    flags_need_redrawing_flag++;

    uint8_t a = screen_getchar();

    uint8_t a_1 = control_key_to_ascii(a);

    switch (a_1)
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
            set_marker_common('1');
            return;

        case '2':
            set_marker_common('2');
            return;

        case '3':
            set_marker_common('3');
            return;

        case '4':
            set_marker_common('4');
            return;

        case '5':
            set_marker_common('5');
            return;

        case '6':
            set_marker_common('6');
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

    draw_prompt_characters('^', 'O');

    flags_need_redrawing_flag++;

    uint8_t a = screen_getchar();

    uint8_t a_1 = control_key_to_ascii(a);

    switch (a_1)
    {

        case 'J':
            cf3_justify_mode_key();
            return;

        case 'X':
            sf2_release_margins_key();
            return;

        case 'C':
            sf8_edit_command_key(ptr1);
            return;

        case 'D':
            sf9_delete_command_key();
            return;

        case 'F':
            cf2_format_mode_key();
            return;

        case 'M':
            cf8_mark_as_ruler_key(ptr1);
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

    draw_prompt_characters('^', 'Q');

    flags_need_redrawing_flag++;

    uint8_t a = screen_getchar();

    uint8_t a_1 = control_key_to_ascii(a);

    switch (a_1)
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
            go_to_marker_n('1');
            return;

        case '2':
            go_to_marker_n('2');
            return;

        case '3':
            go_to_marker_n('3');
            return;

        case '4':
            go_to_marker_n('4');
            return;

        case '5':
            go_to_marker_n('5');
            return;

        case '6':
            go_to_marker_n('6');
            return;
    }

    return;
}

void return_key(void)
{

    // return_key

    // return_key: Carriage return: moves to next line at column 0

    //     jsr write_line_back_to_document_safely

    write_line_back_to_document_safely();

    //     lda #0

    //     sta xpos

    xpos = 0;

    //     lda current_line_ptr

    //     sta ((uint8_t*)&tmp01)[0]

    //     lda current_line_ptr+1

    //     sta ((uint8_t*)&tmp01)[1]

    addr_t tmp01;
    uint8_t y;
    if (!find_next_line(current_line_ptr, &tmp01, &y))
    {
        advance_current_line_pointer();
        return;
    }
    //     bne c9d9b

    //     tya
    //     ldy current_line_ptr+1
    //     clc
    //     adc current_line_ptr
    //     bcc c9d98
    //     iny
    // (16-bit arithmetic: current_line_ptr + y (the offset from
    //  find_next_line))
    uint16_t sum = current_line_ptr + y;

    //     jsr sub_c9de1

    insert_line_at_cursor(sum);

    //     // falls through to c9d9b

    advance_current_line_pointer();
    return;
}

// c9de3: Insert-line entry point used by cf6_split_line_key.

// Skips cursor_moved_flag++ that sub_c9de1 would do.

// Expects A = low byte, Y = high byte of insertion address.

static void sf0_move_block_key(void)
{
    // sf0_move_block_key

    write_line_back_to_document_safely();

    if (reset_area_to_marks_1_2())
    {
        beep();
        return;
    }

    check_pointer_in_area();

    uint8_t x = 0xff;

    top_of_screen_line_ptr =
        (top_of_screen_line_ptr & 0x00ff) | ((addr_t)x << 8);

    l006f = x;

    adjust_area_pointers(tmp67);

    ensure_cr_at_document_top();

    move_cursor_to_address(doc_ptr1);

    clear_marks_1_2();
}

static void sf11_copy_key(void)
{
    // sf11_copy_key

    // sf11_copy_key:

    //     jsr f6_insert_line_key

    f6_insert_line_key();

    //     jsr sub_ca276

    redraw_editor();

    //     ldx l003a

    uint8_t x = l003a;

    //     beq ca0ef

    if (x == 0)
        goto ca0ef;

    //     ldy #0

    uint8_t y = 0;

    // loop_ca0e7:

    do
    {
        uint8_t a = ram[current_ruler_ptr + y];
        ram[RAM_EDIT_BUFFER + y] = a;
        y++;
        x--;
    } while (x != 0);

    // ca0ef:

ca0ef:

    //     jmp cf8_mark_as_ruler_key

    cf8_mark_as_ruler_key(ptr1);
}

static void sf12_left_key(void)
{
    // sf12_left_key

    // Pseudocode: Moves cursor left by one word

    // ;
    // ***************************************************************************************

    // sf12_left_key:

    //     ldy xpos
    uint8_t y = xpos;

    //     beq c9f80

    if (y == 0)
    {
        move_to_previous_line();
        return;
    }

    //     jsr draw_previous_word

    uint8_t word_boundary;
    _Bool is_start_of_line;
    uint8_t x;
    draw_previous_word(&word_boundary, &is_start_of_line, &x);

    //     bne return_57

    //     cmp #0x20 ; ' '

    //     beq c9f80
    if (!is_start_of_line)
        return;
    if (word_boundary == 0x20)
    {
        move_to_previous_line();
        return;
    }

    // return_57:

    //     rts
}

static void sf13_right_key(void)
{

    // sf13_right_key

    // sf13_right_key: Moves cursor right by one word

entry:

    //     lda current_edit_line_ptr

    //     sta ((uint8_t*)&tmp01)[0]

    //     lda current_edit_line_ptr+1

    //     sta ((uint8_t*)&tmp01)[1]

    addr_t tmp01 = RAM_EDIT_BUFFER;

    //     jsr get_line_length

    uint8_t line_len = get_line_length();

    //     sty input_buffer_ptr+1

    //     cpy xpos

    //     bcc c9fab

    if (line_len < xpos)
        goto c9fab;

    //     beq c9fab

    if (line_len == xpos)
        goto c9fab;

    //     ldy xpos

    uint8_t y = xpos;

    // loop_c9ff8:

    goto loop_c9ff8;

c9fab:

    // c9fab:  (when cursor is at or past end of line, move to next line)

    //     sty xpos

    uint8_t y_1;
    xpos = y_1;

    //     jsr ca93c

    write_line_back_to_document_safely();

    //     lda current_line_ptr

    //     ldy current_line_ptr+1

    //     jsr sub_cab1a

    if (advance_to_next_line(current_line_ptr, &tmp01, &y_1))
        return;

    //     beq return_58

    //     tya

    uint8_t a = y_1;
    //     clc
    //     adc current_line_ptr
    //     sta current_line_ptr
    //     bcc c9fc3
    // (16-bit arithmetic: current_line_ptr += y)
    current_line_ptr += a;

    //     jsr unpack_line

    unpack_line(ptr1);

    //     dec l006f

    l006f--;

    //     jsr c9e94

    xpos = 0;

    //     jsr get_line_length

    if (get_line_length() == xpos)
        return; // xpos == 0, empty line

    //     lda current_edit_line_ptr

    //     sta ((uint8_t*)&tmp01)[0]

    addr_t tmp01_1 = RAM_EDIT_BUFFER;

    //     ldy xpos

    y_1 = 0;

    //     jsr process_current_document_character
    bool is_tab = false;

    uint8_t a_1;
    uint8_t x;
    a_1 = process_current_document_character(tmp01_1, &x, &y_1, &is_tab);

    //     cmp #0x20 ; ' '

    //     bne return_58
    if (a_1 != 0x20)
        return;

    //     (fall through — line starts with space, scan forward as usual)

    goto entry;

// loop_c9ff8:
loop_c9ff8:
    for (;;)
    {
        //     cpy input_buffer_ptr+1
        //     bcs ca00f
        if (y >= line_len)
            goto ca00f;
        //     jsr process_current_document_character
        uint8_t x;
        uint8_t a_2 =
            process_current_document_character(tmp01, &x, &y, &is_tab);
        //     cmp #0x20 ; ' '
        if (a_2 != 0x20)
            continue;
        //     bne loop_c9ff8
        break;
    }
    //     loop_ca003:
    for (;;)
    {
        //     cpy input_buffer_ptr+1
        //     bcs ca00f
        if (y >= line_len)
            goto ca00f;
        //     jsr process_current_document_character
        uint8_t x;
        uint8_t a_3 =
            process_current_document_character(tmp01, &x, &y, &is_tab);
        //     cmp #0x20 ; ' '
        if (a_3 == 0x20)
            continue;
        //     beq loop_ca003
        break;
    }
    //     dey

    y--;

ca00f:

    //     sty xpos

    xpos = y;

    //     rts

    return;
}

static void set_marker(uint8_t x);

static void set_marker_common(uint8_t a);

// MULTIPLE ENTRY POINTS: sf7_set_marker_key, set_marker, set_marker_1..6

static void sf14_down_key(void)
{

    // sf14_down_key:

    //     ldx screen_height

    //     inc l0079

    //     inc l006f

    uint8_t x = screen_maxrow;

    l0079++;

    l006f++;

    move_cursor_down(x);
}

static void sf15_up_key(void)
{

    // sf15_up_key:

    //     ldx screen_height

    uint8_t x = screen_maxrow;

    //     inc l0079

    l0079++;

    //     inc l006f

    l006f++;

    move_cursor_up(x);
}

static void sf1_swap_case_key(void)
{

    // sf1_swap_case_key

    // sf1_swap_case_key:

    //     ldy xpos

    uint8_t y = xpos;

    //     lda (current_edit_line_ptr),y

    uint8_t a = ram[RAM_EDIT_BUFFER + y];

    //     jsr is_uppercase
    //     bcs f13_right_key
    if (!isalpha(a))
    {
        f13_right_key();
        return;
    }

    //     inc l0074

    l0074++;

    //     eor #0x20 ; ' '

    a ^= 0x20;

    //     sta (current_edit_line_ptr),y

    ram[RAM_EDIT_BUFFER + y] = a;

    //     falls through to f13_right_key

    f13_right_key();
    return;
}

static void sf2_release_margins_key(void)
{

    // sf2_release_margins_key:

    //     bit format_mode_flag

    if (!(format_mode_flag & 0x40))
    {
        xpos = 0;
        return;
    }

    //     jsr find_left_margin_stop

    int y = find_left_margin_stop();

    //     bcs f4_beginning_of_line_key

    if (y < 0)
    {
        f4_beginning_of_line_key();
        return;
    }

    //     sty xpos

    xpos = (uint8_t)y;

    //     rts

    return;
}

static void sf3_delete_to_char_key(void)
{

    // sf3_delete_to_char_key

    draw_prompt_characters('C', 'H');

    flags_need_redrawing_flag++;

    uint8_t a = screen_getchar();

    // (branch restructured: 0xa0 -> 0x1c, 0xa1 -> 0x1d, control/high -> beep)
    if (a == 9 || a == 0xa0 || a == 0xa1)
    {
        if (a == 0xa0)
            a = 0x1c;
        else if (a == 0xa1)
            a = 0x1d;
    }
    else
    {
        if (a < 0x20 || a >= 0x7f)
        {
            beep();
            return;
        }
    }

    {

        uint8_t search_char = a;

        l0074++;

        uint8_t y = xpos;

        uint8_t start_x = y;

        // loop_ca132: scan forward to find matching char
        bool found_match = false;
        while (y < MAX_LINE_LENGTH)
        {
            uint8_t a_1 = ram[RAM_EDIT_BUFFER + y];
            y++;
            if (a_1 == search_char)
            {
                found_match = true;
                break;
            }
        }
        if (!found_match)
        {
            beep();
            return;
        }
        // loop_ca13d: scan forward to find end of matching sequence
        while (y < MAX_LINE_LENGTH)
        {
            uint8_t a_2 = ram[RAM_EDIT_BUFFER + y];
            y++;
            if (a_2 != search_char)
                break;
        }

        y--;

        uint8_t x = y - start_x;

        delete_edit_buffer_bytes_at_xpos(x);
    }
}

static void sf4_highlight1_key(void)
{

    // sf4_highlight1_key:

    //     lda #0x1c

    uint8_t a = 0x1c;

    //     jmp c9e3a

    tab_highlight_common(a);
    return;
}

static void sf5_highlight2_key(void)
{

    // sf5_highlight2_key:

    //     lda #0x1d

    uint8_t a = 0x1d;

    //     jmp c9e3a

    tab_highlight_common(a);
    return;
}

static void sf6_go_to_marker_key(void)
{

    // sf6_go_to_marker_key:

    //     jsr write_line_back_to_document_safely

    write_line_back_to_document_safely();

    //     jsr prompt_for_marker

    int marker = prompt_for_marker();

    //     bcs return_58

    if (marker == MARKER_INVALID)
        return;

    //     beq return_58

    if (markers_array[marker] == 0)
        return;

    // go_to_marker:

    go_to_marker(marker);
    return;
}

static void sf7_set_marker_key(void)
{

    // sf7_set_marker_key:

    //     jsr write_line_back_to_document_safely

    write_line_back_to_document_safely();

    //     jsr prompt_for_marker

    int marker = prompt_for_marker();

    //     bcs return_58

    if (marker == MARKER_INVALID)
        return;

    // set_marker:

    set_marker(marker);
    return;
}

static void sf8_edit_command_key(addr_t ptr1)
{

    // sf8_edit_command_key
    //  Ptrs:   ptr1

    // sf8_edit_command_key: Allows editing formatting command on current line
    // interactively

    //     jsr c9e94

    xpos = 0;

    //     jsr redraw_editor

    redraw_editor();

    //     inc l006d

    edit_buffer_dirty_flag++;

    //     lda #0

    uint8_t a = 0;

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

    uint8_t a_1 = screen_getchar();

    //     cmp #0x0d

    if (a_1 == 0x0d)
        goto finished_editing_command;

    //     beq finished_editing_command

    //     and #0xdf

    a_1 &= 0xdf;

    //     cmp #0x41 ; 'A'

    if (a_1 < 0x41)
        goto edit_command_loop;

    //     bcc edit_command_loop

    //     cmp #0x5b ; '['

    if (a_1 >= 0x5b)
        goto edit_command_loop;

    //     bcs edit_command_loop

    //     sta l0081

    l0081 = a_1;

    //     jsr screen_putchar

    screen_putchar(a_1);

    //     ldy input_buffer_offset+1

    uint8_t y = l0080;

    //     iny

    y++;

    //     sty input_buffer_offset+1

    l0080 = y;

    //     sta (ptr1),y
    // (16-bit arithmetic: ram[ptr1 + y], with the carry across the low-byte
    //  boundary handled by the plain 16-bit index)
    ram[ptr1 + y] = a_1;

    //     cpy #2

    if (y < 2)
        goto edit_command_loop;

    //     bcc edit_command_loop

    //     lda #0

    uint8_t a_2 = 0;

    //     sta input_buffer_offset+1

    l0080 = a_2;

    //     beq edit_command_loop

    goto edit_command_loop;

    // finished_editing_command:

finished_editing_command:

    //     lda l0081

    uint8_t a_3 = l0081;

    //     beq return_56

    if (a_3 == 0)
        return;

    //     lda ptr1

    //     sta current_format_line_ptr

    current_format_line_ptr = ptr1;

    //     lda ptr1+1

    //     sta current_format_line_ptr+1

    //     ldy #0

    //     lda #0x80

    //     sta (current_format_line_ptr),y

    ram[current_format_line_ptr + 0] = 0x80;

    //     jmp caf5c

    set_format_mode_bit7();
    return;
}

static void sf9_delete_command_key(void)
{

    // sf9_delete_command_key

    // sf9_delete_command_key: Deletes any formatting command prefix from
    // current line

    //     ldy #0

    uint8_t y = 0;

    //     lda (current_format_line_ptr),y

    uint8_t a = ram[current_format_line_ptr + y];

    //     jsr check_for_command_prefix

    command_prefix_t cp = check_for_command_prefix(a);

    //     bne return_56

    if (cp == NO_COMMAND_PREFIX)
        return;

    //     tya

    uint8_t a_1 = y;

    //     sta (current_format_line_ptr),y

    ram[current_format_line_ptr + y] = a_1;

    //     lda current_edit_line_ptr

    //     sta current_format_line_ptr

    current_format_line_ptr = RAM_EDIT_BUFFER;

    //     lda current_edit_line_ptr+1

    //     sta current_format_line_ptr+1

    //     jsr sub_caf5f

    clear_format_mode_bit7();

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

    uint8_t a = 9;

    //     jmp c9e3a

    tab_highlight_common(a);
    return;
}

static void advance_current_line_pointer(void)
{

    //     inc cursor_moved_flag
    cursor_moved_flag++;
    //     lda current_line_ptr
    //     ldy current_line_ptr+1
    //     jsr sub_cab1a

    addr_t tmp01;
    uint8_t y;
    if (advance_to_next_line(current_line_ptr, &tmp01, &y))
        return;
    //     beq return_54
    //     tya
    //     clc
    //     adc current_line_ptr
    //     sta current_line_ptr
    //     bcc return_54
    //     inc current_line_ptr+1
    // (sub_cab1a leaves y = offset of the CR terminator)
    current_line_ptr += y;
    // return_54:
    //     rts
}

static void clear_marks_1_2(void)
{
    // clear_marks_1_2: Clears markers 1 and 2 (resets to zero)

    //     lda #0
    //     ldx #3
    // loop_cad12:
    //     sta markers_array,x
    //     dex
    //     bpl loop_cad12
    markers_array[0] = 0;
    markers_array[1] = 0;
    //     rts
}

static uint8_t control_key_to_ascii(uint8_t a)
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
    return toupper(a);
}

static void delete_edit_buffer_bytes_at_xpos(uint8_t x)
{

    // delete_edit_buffer_bytes_at_xpos
    // delete_edit_buffer_bytes_at_xpos: Deletes N bytes at cursor position,
    // shifting existing content left

    //     stx input_buffer_offset+1
    l0080 = x;
    //     inc l006d
    edit_buffer_dirty_flag++;
    //     lda current_edit_line_ptr
    //     sta ((uint8_t*)&tmp67)[0]
    //     lda current_edit_line_ptr+1
    //     sta ((uint8_t*)&tmp67)[1]
    addr_t tmp67 = RAM_EDIT_BUFFER;
    //     ldy xpos

    uint8_t y = xpos;
    //     tya

    uint8_t a = y;
    //     clc
    //     adc input_buffer_offset+1
    a += l0080;
    //     sta l0084
    // cae78:
cae78:
    //     jsr sub_ca536
    //     bne cae98
    x = find_marker_at_position(y, tmp67);
    if (x == 0x0c)
        goto cae98;
    //     lda #0
    //     cpy l0084
    //     bcc cae91
    //     tya
    //     sbc input_buffer_offset+1
    //     clc
    //     adc current_edit_line_ptr
    //     sta markers_array,x
    //     lda current_edit_line_ptr+1
    //     adc #0
    //     bne cae93
    //     (fall through to cae91 if the high byte is 0, which never happens
    //      because RAM_EDIT_BUFFER's high byte is 0x05)
    // cae91:
    //     sta markers_array,x
    // cae93:
    //     sta markers_array+1,x
    // (16-bit store: markers_array[x] = (y >= l0084)
    //  ? RAM_EDIT_BUFFER + (y - l0080) : 0)
    uint16_t marker_val = (y >= l0084) ? RAM_EDIT_BUFFER + (y - l0080) : 0;
    markers_array[x / 2] = marker_val;
    //     jmp cae78
    goto cae78;

    // cae98:
cae98:
    //     iny
    y++;
    //     cpy #0x85
    if (y < MAX_LINE_LENGTH + 1)
        goto cae78;
    //     lda xpos
    uint8_t a_1 = xpos;
    //     cmp #0x84
    //     bcs return_78
    if (a_1 >= MAX_LINE_LENGTH)
    { /* return_78: */
        return;
    }
    //     ldy xpos
    // loop_caea5:
    //     sty l0084
    //     ldx #0x10
    //     tya
    //     clc
    //     adc input_buffer_ptr+1
    //     bcs caeb7
    //     tay
    //     cpy #0x84
    //     bcs caeb7
    //     lda (current_edit_line_ptr),y
    //     tax
    // caeb7:
    //     ldy l0084
    //     txa
    //     sta (current_edit_line_ptr),y
    //     iny
    //     cpy #0x84
    //     bcc loop_caea5
    // return_78:
    //     rts
    int copy_len = MAX_LINE_LENGTH - (int)xpos - (int)l0080;
    if (copy_len > 0)
    {
        memmove(&ram[RAM_EDIT_BUFFER + xpos],
            &ram[RAM_EDIT_BUFFER + xpos + l0080],
            (size_t)copy_len);
        memset(&ram[RAM_EDIT_BUFFER + xpos + copy_len],
            0x10,
            MAX_LINE_LENGTH - xpos - copy_len);
    }
    else
    {
        memset(&ram[RAM_EDIT_BUFFER + xpos], 0x10, MAX_LINE_LENGTH - xpos);
    }
    return;
}

static void enter_printable_character(void)
{

    // enter_printable_character
    // enter_printable_character:
    //     ldy xpos
    uint8_t y = xpos;
    //     cpy #0x84
    if (y >= MAX_LINE_LENGTH)
        return;
    //     inc l006d
    edit_buffer_dirty_flag++;
    //     jsr sub_caef4
    if (adjust_margins_at_left_margin())
        return;
    //     bcs c9bca
    //     lda current_edit_line_ptr

    addr_t tmp67 = RAM_EDIT_BUFFER;
    //     ldy xpos
    uint8_t y_1 = xpos;
    //     jsr sub_ca536
    //     bne c9bf2
    uint8_t idx = find_marker_at_position(y_1, tmp67);
    if (idx != 0x0c)
    {
        if (idx < 4)
        {
            l0074++;
        }
    }
    //     ldx insert_mode_flag

    uint8_t x = insert_mode_flag;
    //     bne c9c00
    if (x != 0)
        goto c9c00;
    //     lda (current_edit_line_ptr),y
    uint8_t a = ram[RAM_EDIT_BUFFER + y_1];
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
    //     jsr insert_edit_buffer_bytes_at_xpos
    if (!insert_edit_buffer_bytes_at_xpos(1))
    {
        return; // bcs c9c7f
    }
    // c9c09:
c9c09:
    //     lda l0038
    uint8_t a_1 = l0038;
    //     sta (current_edit_line_ptr),y
    // (the 6502 left y = the insert position, i.e. xpos)
    ram[RAM_EDIT_BUFFER + xpos] = a_1;
    //     ldy l0074
    uint8_t y_2 = l0074;
    //     bne c9c14
    if (y_2 == 0)
    {
        screen_putchar(a_1);
    }
    //     inc xpos
    xpos++;
    //     jsr ca684
    update_line_length();
    //     ldy #0
    uint8_t y_3 = 0;
    //     sty l0039
    l0039 = 0;
    // c9c1d:
c9c1d:
    //     lda (current_edit_line_ptr),y
    uint8_t a_2 = ram[RAM_EDIT_BUFFER + y_3];
    //     iny
    y_3++;
    //     cpy xpos
    //     bcs c9c56
    if (y_3 > xpos)
        goto c9c56;
    //     cmp #9
    //     bne c9c31
    if (a_2 != 9)
        goto c9c31;
    //     jsr sub_ca5ae
    {
        bool is_tab = false;
        (void)process_document_character(a_2, &x, &is_tab);
    }
    //     txa
    a_2 = x;
    //     clc
    //     adc l0039
    a_2 += l0039;
    //     bne c9c43
    if (a_2 != 0)
        goto c9c43;
    // c9c31:
c9c31:
    //     cmp #0x0b
    //     bne c9c4a
    if (a_2 != 0x0b)
        goto c9c4a;
    //     lda ruler_left_stop
    a_2 = ruler_left_stop;

    //     beq c9c48
    if (a_2 == 0)
        goto c9c48;
    //     ldx l0039
    x = l0039;
    //     beq c9c43
    if (x != 0)
    {
        if (x >= ruler_left_stop)
        {
            x++;
            a_2 = x;
        }
    }
c9c43:
    //     sta l0039
    l0039 = a_2;
    //     jmp c9c1d
    goto c9c1d;
    // c9c48:
c9c48:
    //     lda #0x20 ; ' '
    a_2 = 0x20;
    // c9c4a:
c9c4a:
    //     cmp #0x1b
    //     bcc c9c48
    if (a_2 < 0x1b)
        goto c9c48;
    //     cmp #0x20
    //     bcc c9c1d
    if (a_2 < 0x20)
        goto c9c1d;
    //     inc l0039
    l0039++;
    //     bne c9c1d
    goto c9c1d;
    // c9c56:
c9c56:
    //     ldy l0039
    uint8_t y_4 = l0039;
    //     cpy l003a
    if (y_4 < l003a)
    {
        //     lda (current_ruler_ptr),y
        uint8_t a_4 = ram[current_ruler_ptr + y_4];
        //     and #0xdf
        a_4 &= 0xdf;
        //     cmp #0x42 ; 'B'
        if (a_4 == 0x42)
            beep();
    }
    //     lda l0038
    uint8_t a_5 = l0038;
    //     cmp #0x20 ; ' '
    //     beq c9c7f
    if (a_5 == 0x20)
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
    //     sta ((uint8_t*)&tmp67)[1]
    addr_t tmp67_1;
    ((uint8_t*)&tmp67_1)[1] = 0;
    //     tya
    //     beq c9c7f
    if (y_4 == 0)
        return;
    //     dey
    y_4--;
    //     cpy ruler_right_stop
    //     bcs c9c82
    if (y_4 < ruler_right_stop)
        return;
    // c9c82: (4202)
    //     jsr get_line_length (4203)
    l0083 = get_line_length();
    //     lda #0 (4205)
    //     sta top_margin (4206)
    top_margin = 0;
    //     ldy xpos (4207)
    uint8_t y_5 = xpos;
    //     sty input_buffer_ptr+1 (4208)
    input_buffer_offset = y_5;
    //     jsr draw_previous_word (4209)

    uint8_t word_boundary;
    _Bool is_start_of_line;
    draw_previous_word(&word_boundary, &is_start_of_line, &x);
    //     jsr sub_ca608 (4210)
    recalculate_cursor_xpos();
    //     lda l0072 (4211)
    uint8_t a_6 = l0072;
    //     cmp ruler_left_stop (4212)
    //     beq c9c9d (4213) bcs c9ca2 (4214)
    // c9c9d: (4215)
    //     ldy input_buffer_ptr+1, dey, sty xpos (4216-4218)
    if (a_6 == ruler_left_stop)
    {
        uint8_t y_6 = input_buffer_offset;
        y_6--;
        xpos = y_6;
        goto c9ca2;
    }
    if (a_6 < ruler_left_stop)
    {
        {
            uint8_t y_7 = input_buffer_offset;
            y_7--;
            xpos = y_7;
        }
    }
c9ca2:
    //     lda input_buffer_ptr+1 (4220)
    uint8_t a_7 = input_buffer_offset;
    //     sec (4221)
    //     sbc xpos (4222)
    //     sta top_margin (4223)
    a_7 -= xpos;
    top_margin = a_7;
    //     lda l0083 (4224)
    uint8_t a_8 = l0083;
    //     sec (4225)
    //     sbc xpos (4226)
    //     sta l0083 (4227)
    a_8 -= xpos;
    l0083 = a_8;
    //     tay (4228)
    uint8_t y_8 = a_8;
    //     iny (4229)
    y_8++;
    //     lda ruler_left_stop (4230)
    uint8_t a_9 = ruler_left_stop;

    //     beq c9cb9 (4231)
    if (a_9 != 0)
    {
        top_margin++;
        y_8++;
    }
    //     sty ((uint8_t*)&tmp67)[0] (4235)
    //     lda current_line_ptr (4236)
    //     sec (4237)
    //     adc l003b (4238)
    //     sta ((uint8_t*)&tmp45)[0] (4239)
    //     lda current_line_ptr+1 (4240)
    //     adc #0 (4241)
    //     sta ((uint8_t*)&tmp45)[1] (4242)

    addr_t tmp45 = current_line_ptr + l003b + 1;
    //     jsr make_space_for_insertion (4243)
    if (make_space_for_insertion(tmp45, y_8))
        goto c9cd0; // bcc c9cd0
    //     jmp ca941 (4245)
    show_memory_full_error();
    longjmp(env, JMP_EDITOR);
    // c9cd0: (4247)
c9cd0:
    //     ldy #0 (4248)
    uint8_t y_9 = 0;
    //     lda ruler_left_stop
    //     beq c9cdb
    if (ruler_left_stop != 0)
    {
        //     lda #0x0b
        //     sta (((uint8_t*)&tmp45)[0]),y
        ram[tmp45] = 0x0b;
        //     iny ; Y=0x01
        y_9 = 1;
    }
    // c9cdb:
    //     sty l0081
    l0081 = y_9;
    //     lda current_edit_line_ptr
    addr_t tmp67_3 = RAM_EDIT_BUFFER;
    //     ldy xpos

    uint8_t y_10 = xpos;
    //     dey
    y_10--;
    //     lda (current_edit_line_ptr),y

    uint8_t a_10 = ram[RAM_EDIT_BUFFER + y_10];
    //     cmp #0x20 ; ' '
    //     bne c9cf2
    if (a_10 == 0x20)
    {
        //     lda #0x10
        //     sta (current_edit_line_ptr),y
        ram[RAM_EDIT_BUFFER + y_10] = 0x10;
    }
    // c9cf2:
    //     iny
    y_10++;
    //     sty l0082
    l0082 = y_10;
    // c9cf5:
c9cf5:
    //     ldy l0082

    uint8_t y_11 = l0082;
    //     inc l0082
    l0082++;
    // loop_c9cf9:
    for (;;)
    {
        //     jsr sub_ca536
        //     bne c9d0d
        uint8_t marker_index = find_marker_at_position(y_11, tmp67_3);
        if (marker_index == 0x0c)
            break;
        //     lda l0081
        //     clc
        //     adc ((uint8_t*)&tmp45)[0]
        //     sta markers_array,x
        //     lda ((uint8_t*)&tmp45)[1]
        //     adc #0
        //     sta markers_array+1,x
        // (16-bit arithmetic: markers_array[marker_index] = tmp45 + l0081;
        //  the bcc loop_c9cf9 continues while the address fits in 16 bits)
        {
            uint16_t val = tmp45 + l0081;
            markers_array[marker_index / 2] = val;
            if (tmp45 + (uint16_t)l0081 >= 0x10000)
                break;
        }
    }
    // c9d0d:
    //     lda l0083

    uint8_t a_11 = l0083;
    //     bne c9d28
    if (a_11 != 0)
        goto c9d28;
    //     lda #0x0d

    uint8_t a_12 = 0x0d;
    //     bne c9d30                                                         ;
    //     ALWAYS branch
    goto c9d30;
    // c9d28:
c9d28:
    //     lda (current_edit_line_ptr),y

    uint8_t a_13 = ram[RAM_EDIT_BUFFER + y_11];
    //     pha
    {
        uint8_t saved = a_13;
        //     lda #0x10
        //     sta (current_edit_line_ptr),y
        ram[RAM_EDIT_BUFFER + y_11] = 0x10;
        //     pla
        a_12 = saved;
    }
    // c9d30:
c9d30:
    //     ldy l0081

    uint8_t y_12 = l0081;
    //     inc l0081
    l0081++;
    //     sta (((uint8_t*)&tmp45)[0]),y
    ram[tmp45 + y_12] = a_12;
    //     dec l0083
    l0083--;
    //     bpl c9cf5
    if (!(l0083 & 0x80))
        goto c9cf5;
    //     bmi c9d15                                                         ;
    //     ALWAYS branch
    // c9d15:
    //     jsr justify_edit_buffer
    justify_edit_buffer(ptr1);
    //     jsr ca93c
    write_line_back_to_document_safely();
    //     jsr ca741
    clamp_ptr6_to_document();
    //     jsr return_key
    return_key();
    //     lda top_margin
    //     sta xpos
    xpos = top_margin;
    //     jmp editor_loop
    return;
}

// MULTIPLE ENTRY POINTS: sf1_swap_case_key, f13_right_key

// Prompts for a marker character and looks it up.  Returns the marker index
// 0-5, or MARKER_INVALID if the character read is not a valid marker.
static int prompt_for_marker(void)
{

    // Pseudocode: Prompts for a marker character and looks it up

    // prompt_for_marker:
    //     ldx #0x4d ; 'M'
    //     ldy #0x4b ; 'K'
    //     jsr draw_prompt_characters
    draw_prompt_characters('M', 'K');
    //     inc flags_need_redrawing_flag
    flags_need_redrawing_flag++;
    //     jsr read_char

    uint8_t a = screen_getchar();
    //     jsr lookup_marker
    return lookup_marker(a);
}

// Sets the area to markers 1 and 2, then adjusts doc_ptr1.  Returns true if
// the area could not be defined (an invalid or unset marker, or an empty
// area) — the 6502's carry flag.
static bool reset_area_to_marks_1_2(void)
{

    // reset_area_to_marks_1_2
    // reset_area_to_marks_1_2: Sets area to markers 1 and 2, then adjusts
    // doc_ptr1

    //     lda #0x31 ; '1'
    //     jsr lookup_marker
    int idx1 = lookup_marker(0x31);
    //     bcs return_76
    if (idx1 == MARKER_INVALID)
        return true;
    //     beq cad45
    if (markers_array[idx1] == 0)
        goto cad45;
    //     lda __begin_pointer_array,x
    //     sta area_start_ptr
    //     lda markers_array+1,x
    //     sta area_start_ptr+1
    area_start_ptr = markers_array[idx1];
    //     lda #0x32 ; '2'
    //     jsr lookup_marker
    int idx2 = lookup_marker(0x32);
    //     bcs return_76
    if (idx2 == MARKER_INVALID)
        return true;
    //     beq cad45
    if (markers_array[idx2] != 0)
    {
        //     lda __begin_pointer_array,x
        //     sta area_end_ptr
        //     lda markers_array+1,x
        //     sta area_end_ptr+1
        area_end_ptr = markers_array[idx2];
        // (doc_ptr1 aliases markers_array[8]; set_marker_to_here now takes the
        //  element index, so divide the byte offset by the element size)

        uint8_t x =
            ((uint8_t*)&doc_ptr1 - (uint8_t*)markers_array) / sizeof(addr_t);
        set_marker_to_here(x);
        area_status_t status = sanitise_area();
        if (status == AREA_NOT_EMPTY)
            return false;
    }
cad45:
    //     sec
    // return_76:
    //     rts
    return true;
}

static void insert_line_at_cursor(addr_t ptr)
{
    // sub_c9de1:
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    //     falls through to c9de3
    insert_line_into_document(ptr);
}

static void move_to_previous_line(void)
{

    // c9f80:
    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    //     lda current_line_ptr

    addr_t tmp01;
    if (!find_previous_line(current_line_ptr, &tmp01))
        return;
    //     bcc return_56
    current_line_ptr = tmp01;
    //     jsr unpack_line
    unpack_line(ptr1);
    //     jsr c9e9b
    set_xpos_to_line_length();
    //     dec l006f
    l006f--;
}

static void move_cursor_up(uint8_t x)
{

    // sub_ca071
    // (the 6502 held the candidate line in the YA register pair; use a real
    //  16-bit variable instead)
    // sub_ca071:
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    //     stx input_buffer_offset+1
    l0080 = x;
    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    //     lda current_line_ptr / ldy current_line_ptr+1
    addr_t line = current_line_ptr;
    // ca07c:
    while (1)
    {
        //     sta ((uint8_t*)&tmp23)[0]

        addr_t tmp23 = line;

        addr_t tmp01;
        if (!find_previous_line(line, &tmp01))
        {
            // ca093:
            line = tmp23;
            break;
        }
        line = tmp01;
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
    current_line_ptr = line;
    //     sty current_line_ptr+1
    //     rts
}

static void move_cursor_down(uint8_t x)
{

    // sub_ca0af
    // (the 6502 held the candidate line in the YA register pair; use a real
    //  16-bit variable instead)
    // sub_ca0af:
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    //     stx input_buffer_offset+1
    l0080 = x;
    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    //     lda current_line_ptr / ldy current_line_ptr+1
    addr_t line = current_line_ptr;
    // ca0ba:
    while (1)
    {
        //     jsr sub_cab1a

        addr_t tmp01;
        uint8_t y;
        if (advance_to_next_line(line, &tmp01, &y))
        {
            // ca0d2:
            line = tmp01;
            break;
        }
        //     tya / ldy tmp01[1] / clc / adc tmp01[0]
        // (advance_to_next_line -> find_next_line leaves the offset past the
        //  CR in the global y register; the next line starts there, so the
        //  16-bit addition is simply line += y)
        line = line + y;
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
    current_line_ptr = line;
    //     sty current_line_ptr+1
    //     rts
}

static void check_pointer_in_area(void)
{

    // sub_ca1cc
    //     lda doc_ptr1 / ldy doc_ptr1+1
    //     cpy area_start_ptr+1 / bcc ca1ea / bne ca1da
    //     cmp area_start_ptr / bcc ca1ea
    //     ; ca1da: cpy area_end_ptr+1 / bcc ca1c9 / bne ca1e6
    //     ;        cmp area_end_ptr / bcc ca1c9 / beq ca1c9
    // (beep when area_start_ptr <= doc_ptr1 <= area_end_ptr)
    if (doc_ptr1 >= area_start_ptr && doc_ptr1 <= area_end_ptr)
    {
        beep();
        return;
    }
    move_cursor_to_address(area_start_ptr);
    {
        uint16_t diff = area_end_ptr - area_start_ptr;
        tmp67 = diff;
    }

    addr_t tmp45 = doc_ptr1;
    if (!make_space_for_insertion(tmp45, tmp67))
    {
        show_memory_full_error();
        longjmp(env, JMP_EDITOR);
    }

    addr_t tmp89 = area_start_ptr;

    addr_t tmp23 = tmp45;
    // ca219:
    while (1)
    {
        ram[tmp23] = ram[tmp89];
        tmp23++;
        tmp89++;
        if ((tmp89) == area_end_ptr)
            break;
    }
    addr_t saved_tmp67 = tmp67;
    doc_ptr1 = tmp45;
    //     lda tmp2 / ldy tmp3 / sec / sbc #1 / bcs ca24d / dey
    uint16_t adjusted = (tmp23)-1;
    //     jsr split_line_at_wrap
    split_line_at_wrap(adjusted);
    //     lda doc_ptr1 / ldy doc_ptr1+1 / jsr split_line_at_wrap
    split_line_at_wrap(doc_ptr1);
    tmp67 = saved_tmp67;
    l0073 = 1;
    cursor_moved_flag = 1;
}

static void tab_highlight_common(uint8_t a)
{
    // c9e3a:
    //     pha
    //     jsr sub_caef4
    if (adjust_margins_at_left_margin())
        return;
    //     pla
    //     bcs return_55
    //     jsr sub_c9e22
    if (!insert_character_into_edit_buffer(a))
        return;
    //     bcs return_55
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
    //     sta l006d
    edit_buffer_dirty_flag = 0;
    //     sta l006f
    l006f = 0;
    //     sta l006e
    edit_buffer_unpacked_flag = 0;
    //     ldx screen_height

    uint8_t x = screen_maxrow;
    // loop_cb0a8:
    do
    {
        line_lengths[x] = 0;
        x--;
    } while (!(x & 0x80));
    //     ldx #2
    //     stx l0073
    //     stx status_line_needs_redrawing_flag
    status_line_needs_redrawing_flag = 2;
    flags_need_redrawing_flag = 1;
    //     rts
}

void clear_format_mode_bit7(void)
{
    // sub_caf5f:
    uint8_t old = format_mode_flag;
    format_mode_flag &= ~0x80;
    if (old != format_mode_flag)
    {
        flags_need_redrawing_flag++;
    }
}

void set_format_mode_bit7(void)
{
    // caf5c:
    uint8_t old = format_mode_flag;
    format_mode_flag |= 0x80;
    if (old != format_mode_flag)
    {
        flags_need_redrawing_flag++;
    }
}

/**
 * Move the cursor back to the start of the previous word.
 *
 * Scans backward from the current cursor position through the edit buffer,
 * skipping whitespace, until the start of the previous word is found, and
 * sets xpos to that position.
 *
 * @param[out] word_boundary the processed character at the previous word
 * boundary (the 6502's A register; callers test it, e.g. for a space).
 * @param[out] is_start_of_line true if the cursor landed at the start of the
 * line (y == 0, the 6502's Z flag); false if a word boundary was found.
 */
void draw_previous_word(
    uint8_t* word_boundary, bool* is_start_of_line, uint8_t* char_width)
{

    // draw_previous_word
    // draw_previous_word: Moves cursor back to start of previous word

    //     lda current_edit_line_ptr
    //     sta ((uint8_t*)&tmp01)[0]
    //     lda current_edit_line_ptr+1
    //     sta ((uint8_t*)&tmp01)[1]

    addr_t line_base = RAM_EDIT_BUFFER;
    //     ldy xpos

    uint8_t pos = xpos;
    bool is_tab = false;
    //     beq caf55
    if (pos == 0)
        goto caf55;
    // loop_caf3f:
    // loop_caf3f:
    for (;;)
    {
        //     dey
        pos--;
        //     beq caf55
        if (pos == 0)
            goto caf55;
        //     jsr process_current_document_character
        uint8_t ch;
        ch = process_current_document_character(
            line_base, char_width, &pos, &is_tab);
        //     dey
        pos--;
        //     cmp #0x20 ; ' '
        if (ch == 0x20)
            continue;
        //     beq loop_caf3f
        break;
    }
    // loop_caf4a:
    for (;;)
    {
        //     dey
        pos--;
        //     jsr process_current_document_character
        uint8_t ch_1;
        ch_1 = process_current_document_character(
            line_base, char_width, &pos, &is_tab);
        //     cmp #0x20 ; ' '
        if (ch_1 == 0x20)
            break;
        //     beq caf55
        //     dey
        pos--;
        //     bne loop_caf4a
        if (pos != 0)
            continue;
        break;
    }
    // caf55:
caf55:
    //     sty xpos
    xpos = pos;
    //     jsr process_current_document_character
    uint8_t ch_2;
    ch_2 = process_current_document_character(
        line_base, char_width, &pos, &is_tab);
    //     dey
    pos--;
    // (the 6502 returns the boundary character in A and Z = (y == 0))
    *word_boundary = ch_2;
    *is_start_of_line = (pos == 0);
}

bool adjust_margins_at_left_margin(void)
{

    // sub_caef4
    // sub_caef4: Handles margin/folding adjustments when typing at left margin

    //     lda format_mode_flag

    uint8_t a = format_mode_flag;
    //     and #0x81
    a &= 0x81;
    if (a != 0)
        goto caf31;
    //     bne caf31
    //     jsr find_left_margin_stop
    //     bcc caf31
    if (find_left_margin_stop() >= 0)
        goto caf31;
    //     jsr get_line_length
    uint8_t line_len = get_line_length();
    //     lda xpos
    uint8_t a_1 = xpos;
    //     sta l0083
    l0083 = a_1;
    //     sty xpos
    xpos = line_len;
    //     jsr sub_ca608
    recalculate_cursor_xpos();
    //     lda l0072
    uint8_t a_2 = l0072;
    //     cmp ruler_left_stop
    if (a_2 < ruler_left_stop)
        goto caf19;
    //     bcc caf19
    //     ldy l0083

    uint8_t y = l0083;
    //     sty xpos
    xpos = y;
    //     inc xpos
    xpos++;
    //     bcs caf2a
    goto caf2a;

    // caf19:
caf19:
    //     lda l0083
    uint8_t a_3 = l0083;
    //     ldy xpos
    y = xpos;
    //     cpy l0083
    if (y >= l0083)
        goto caf28;
    //     sec
    //     sbc ruler_left_stop
    //     bcc caf2a
    // (sbc with C=1 in is a plain subtraction; if it borrows, skip the store.
    //  Otherwise the carry into the following adc is 1: a += xpos + 1)
    if (l0083 < ruler_left_stop)
        goto caf2a;
    a_3 = l0083 - ruler_left_stop + xpos + 1;
    //     adc xpos
    // caf28:
caf28:
    //     sta xpos
    xpos = a_3;
    // caf2a:
caf2a:
    //     jsr sub_caedd
    if (!insert_byte_at_xpos(y))
        return true; // bcs return_79 — insertion failed
    //     inc l0074
    l0074++;
    // caf31:
caf31:
    //     clc
    // return_79:
    //     rts
    return false;
}

bool insert_edit_buffer_bytes_at_xpos(uint8_t x)
{

    // insert_edit_buffer_bytes_at_xpos
    // insert_edit_buffer_bytes_at_xpos: Inserts bytes at cursor position,
    // shifting existing content right

    //     lda xpos

    uint8_t a = xpos;
    //     cmp #MAX_LINE_LENGTH
    //     bcs cae03
    if (a >= MAX_LINE_LENGTH)
    {
        beep();
        return false;
    }
    //     stx input_buffer_offset+1
    l0080 = x;
    //     jsr get_line_length
    uint8_t a_1 = get_line_length();
    //     clc
    //     adc input_buffer_offset+1
    //     bcs cae03
    // (adc with C=0 is a plain addition; carry means overflow)
    a_1 += l0080;
    if (a_1 < l0080)
    {
        beep();
        return false;
    }
    //     cmp #0x85
    //     bcs cae03
    if (a_1 >= MAX_LINE_LENGTH + 1)
    {
        beep();
        return false;
    }
    //     inc l006d
    edit_buffer_dirty_flag++;
    //     lda current_edit_line_ptr
    //     sta ((uint8_t*)&tmp67)[0]
    //     lda current_edit_line_ptr+1
    //     sta ((uint8_t*)&tmp67)[1]
    addr_t tmp67 = RAM_EDIT_BUFFER;
    //     ldy #0x84

    uint8_t y = MAX_LINE_LENGTH;
    // cae27:
cae27:
    //     dey
    y--;
    //     ldx #0
    x = 0;
    //     tya
    uint8_t a_2 = y;
    //     clc
    //     adc input_buffer_offset+1
    //     bcs cae35
    // (adc with C=0 is a plain addition; carry means overflow)
    a_2 += l0080;
    if (a_2 >= l0080)
    {
        if (a_2 < MAX_LINE_LENGTH)
        {
            x = a_2;
        }
    }
    //     stx l0081
    l0081 = x;
    // loop_cae37:
    for (;;)
    {
        //     jsr sub_ca536
        //     bne cae52
        uint8_t idx = find_marker_at_position(y, tmp67);
        if (idx == 0x0c)
            goto cae52;
        //     lda l0081
        //     beq cae4b
        //     clc
        //     adc current_edit_line_ptr
        //     sta markers_array,x
        //     lda current_edit_line_ptr+1
        //     adc #0
        //     bne cae4d
        //     (fall through to cae4b if the high byte is 0, which never
        //      happens because RAM_EDIT_BUFFER's high byte is 0x05)
        // cae4b:
        //     sta markers_array,x
        // cae4d:
        //     sta markers_array+1,x
        // (16-bit store: markers_array[idx] = l0081
        //  ? RAM_EDIT_BUFFER + l0081 : 0)
        uint16_t marker_val = l0081 ? RAM_EDIT_BUFFER + l0081 : 0;
        markers_array[idx / 2] = marker_val;
        //     jmp loop_cae37
    }

    // cae52:
    //     lda (current_edit_line_ptr),y
    //     sty l0084
    //     ldy l0081
    //     beq cae5c
    //     sta (current_edit_line_ptr),y
    // cae5c:
    //     ldy l0084
    //     cpy xpos
    //     bne cae27
    // (byte shift consolidated into a single memmove)
cae52:
    if (y != xpos)
        goto cae27;
    int copy_len = MAX_LINE_LENGTH - (int)xpos - (int)l0080;
    if (copy_len > 0)
        memmove(&ram[RAM_EDIT_BUFFER + xpos + l0080],
            &ram[RAM_EDIT_BUFFER + xpos],
            (size_t)copy_len);
    //     clc
    //     rts
    return true;
}

void set_marker_to_here(uint8_t x)
{

    uint8_t y;
    (void)y;
    // set_marker_to_here
    // set_marker_to_here: Sets marker at current cursor position

    //     jsr get_line_length
    uint8_t len = get_line_length();
    if (len >= xpos)
    {
        //     bcc cad5d
        //     ldy #0
        //     lda (current_format_line_ptr),y

        uint8_t a = ram[current_format_line_ptr + 0];
        //     ldy xpos
        // (Z from ldy xpos is clobbered by the following jsr)
        //     jsr check_for_command_prefix
        command_prefix_t cp = check_for_command_prefix(a);

        //     bne cad5c

        len = xpos;
        if (cp != NO_COMMAND_PREFIX)
            len += 3;
        //     tya
        // cad5d:
    }
    //     clc
    //     adc current_line_ptr
    //     sta 0,x
    //     lda current_line_ptr+1
    //     adc #0
    //     sta 1,x
    uint16_t marker_addr = current_line_ptr + len;
    markers_array[x] = marker_addr;
    //     rts
}

void split_line_at_wrap(addr_t tmp89)
{
    uint8_t a_3;

    // cac78
    // Pseudocode: Splits a line at the word wrap position, inserting CR for new
    // line

    // cac78:
    //     jsr sub_cac50
    tmp89 = find_line_start(tmp89);
    // cac7b:
cac7b:
    //     lda #0
    //     sta l0083

    l0083 = 0;
    //     ldx #0x85

    uint8_t x = MAX_LINE_LENGTH + 1;
    //     ldy #1

    uint8_t y = 1;
    //     lda (((uint8_t*)&tmp89)[0]),y
    uint8_t a_1 = ram[tmp89 + y];
    //     jsr check_for_command_prefix
    command_prefix_t cp = check_for_command_prefix(a_1);

    //     bne cac8d

    if (cp != NO_COMMAND_PREFIX)
    {
        x++;
        x++;
        x++;
    }
    //     stx l0084
    l0084 = x;
    // cac8f:
cac8f:
    //     lda (((uint8_t*)&tmp89)[0]),y
    uint8_t a_2 = ram[tmp89 + y];
    //     iny
    y++;
    //     cmp #0x20 ; ' '
    if (a_2 == 0x20)
        goto cac9a;
    //     beq cac9a
    //     cmp #0x1a
    if (a_2 != 0x1a)
        goto cac9c;
    //     bne cac9c
    // cac9a:
cac9a:
    //     sty l0083
    l0083 = y;
    // cac9c:
cac9c:
    //     cmp #0x0d
    //     beq return_73
    if (a_2 == 0x0d)
        return;
    //     cpy l0084
    //     beq cac8f
    if (y == l0084)
        goto cac8f;
    //     bcc cac8f
    if (y < l0084)
        goto cac8f;
    //     lda l0084
    //     ldx l0083
    //     beq cacad
    //     txa
    if (l0083 == 0)
    {
        a_3 = l0084;
        goto cacad;
    }
    a_3 = l0083;
    // cacad:
cacad:
    //     clc
    //     adc ((uint8_t*)&tmp89)[0]
    //     sta ((uint8_t*)&tmp45)[0]
    //     sta ((uint8_t*)&tmp89)[0]
    //     lda ((uint8_t*)&tmp89)[1]
    //     adc #0
    //     sta ((uint8_t*)&tmp45)[1]
    //     sta ((uint8_t*)&tmp89)[1]

    addr_t tmp45 = tmp89 + a_3;
    tmp89 = tmp45;
    //     lda #1
    //     sta ((uint8_t*)&tmp67)[0]
    //     lda #0
    //     sta ((uint8_t*)&tmp67)[1]

    addr_t tmp67 = 1;
    //     jsr make_space_for_insertion
    make_space_for_insertion(tmp45, tmp67);
    //     lda #0x0d
    uint8_t a_4 = 0x0d;
    //     ldy #0
    uint8_t y_1 = 0;
    //     sta (((uint8_t*)&tmp45)[0]),y
    ram[tmp45 + y_1] = a_4;
    tmp89 = tmp45;
    //     bne cac7b
    if (((uint8_t*)&tmp89)[1] != 0)
        goto cac7b;
    return;
}

addr_t adjust_pointers(addr_t tmp45, addr_t tmp67)
{

    // adjust_pointers

    // adjust_pointers: (6372)

    addr_t tmp23 = tmp45;
    addr_t local_tmp89 = tmp45 + tmp67;
    //     ldx #0 (6382)

    uint8_t x = 0;
    // ca9c3: (6383)
ca9c3:
    //     ldy __begin_pointer_array+1,x (6384)
    //     lda __begin_pointer_array+0,x (6385)
    //     cpy ((uint8_t*)&tmp45)[1] (6386)
    //     bcc ca9f1 (6387)
    //     bne ca9d1 (6388)
    //     cmp ((uint8_t*)&tmp45)[0] (6389)
    //     bcc ca9f1 (6390)
    //     ca9d1: (6391)
    //     cpy ((uint8_t*)&tmp89)[1] (6392)
    //     bcc ca9db (6393)
    //     bne ca9e7 (6394)
    //     cmp ((uint8_t*)&tmp89)[0] (6395)
    //     bcs ca9e7 (6396)
    // (16-bit comparisons: pointer_array[x] < tmp45 → ca9f1 (unchanged),
    //  pointer_array[x] < tmp89 → ca9db (zero if marker, else subtract),
    //  else → ca9e7 (subtract))
    {
        addr_t pa_val = ((addr_t*)&pointer_array)[x];
        if (pa_val < tmp45)
            goto ca9f1;
        if (pa_val < local_tmp89)
            goto ca9db;
        goto ca9e7;
    }
    // ca9db: (6397)
ca9db:
    //     cpx #12 (6398)
    if (x >= ARRAY_SIZE(markers_array))
        goto ca9e7;
    //     bcs ca9e7 (6399)
    //     lda #0 (6400)
    //     sta __begin_pointer_array+0,x (6401)
    //     sta __begin_pointer_array+1,x (6402)
    ((addr_t*)&pointer_array)[x] = 0;
    //     beq ca9f1 (6403) ALWAYS branch
    goto ca9f1;

    // ca9e7: (6405)
ca9e7:
    //     sbc ((uint8_t*)&tmp67)[0] (6406)
    //     sta __begin_pointer_array+0,x (6407)
    //     lda __begin_pointer_array+1,x (6408)
    //     sbc ((uint8_t*)&tmp67)[1] (6409)
    //     sta __begin_pointer_array+1,x (6410)
    // (carry-in to the first sbc is always 1 (all three paths to ca9e7 leave
    //  C=1), so this is a plain 16-bit subtraction: pointer_array[x] -= tmp67)
    ((addr_t*)&pointer_array)[x] -= tmp67;
    // ca9f1: (6411)
ca9f1:
    //     inx (6412)
    //     inx (6413)
    //     cpx #22 (6414)
    // (the 6502 steps x by two bytes per entry; x counts elements here)
    x++;
    if (x != sizeof(pointer_array) / sizeof(addr_t))
        goto ca9c3;
    // loop_ca9f7: (6416)
    //     ldy #0 (6417)
    // loop_ca9f9: (6418)
    //     lda (((uint8_t*)&tmp89)[0]),y (6419)
    //     sta (((uint8_t*)&tmp23)[0]),y (6420)
    //     beq caa08 (6421)
    //     iny (6422)
    //     bne loop_ca9f9 (6423)
    //     inc ((uint8_t*)&tmp23)[1] (6424)
    //     inc ((uint8_t*)&tmp89)[1] (6425)
    //     bne loop_ca9f7 (6426)
    // (6502 (ptr),y loop consolidated into a single memmove: copies the
    //  NUL-terminated region at tmp89 down to tmp23, including the 0x00
    //  terminator; the page wrap in the asm is a plain contiguous copy)
    {
        size_t copy_len = strlen((char*)&ram[local_tmp89]) + 1;
        memmove(&ram[tmp23], &ram[local_tmp89], copy_len);
        // caa08: (6427)
        //     tya (6428)
        //     clc (6429)
        //     adc ((uint8_t*)&tmp23)[0] (6430)
        //     sta top (6431)
        //     lda ((uint8_t*)&tmp23)[1] (6432)
        //     adc #0 (6433)
        //     sta top+1 (6434)
        // (16-bit arithmetic: top = tmp23 + y = tmp23 + strlen)
        top = tmp23 + copy_len - 1;
    }
    //     rts (6435)
    return local_tmp89;
}

/**
 * Advance the document line cursor to the start of the next line.
 *
 * Corresponds to 6502 sub_ca8d (c9a8d).  Advances current_line_ptr past the
 * current line's CR terminator and zeroes xpos.  The 6502 also returned
 * processor flags (V cleared, Z/N from l007e, C set at end of document); no
 * caller consumes Z any more (they test line_format_status directly), but C
 * survives to format_paragraph's callers, so it is returned explicitly here.
 *
 * @note Outputs (via globals):
 *  - current_line_ptr: start of the next line; left unchanged at end of
 *    document (when find_next_line reports end-of-document).
 *  - xpos: set to 0 (the jsr sub_c9e94).
 *
 * @return true when at the end of the document (6502 C=1), false when
 *         current_line_ptr was advanced to a following line.
 */
static bool advance_to_next_doc_line(void)
{

    // advance_to_next_line
    // c9a8d: Advance to next line in document

    //     jsr c9e94
    xpos = 0;
    //     lda current_line_ptr
    //     ldy current_line_ptr+1
    //     jsr sub_cab1a

    addr_t tmp01;
    uint8_t y;
    bool end_of_document = find_next_line(current_line_ptr, &tmp01, &y);
    //     sec
    //     beq c9aa5
    if (!end_of_document)
    {
        //     tya
        //     clc
        //     adc tmp0
        //     sta current_line_ptr
        //     bcc c9aa4
        //     inc current_line_ptr+1
        // (sub_cab1a leaves y = offset of the CR terminator and tmp01 = the
        //  line address, so the next line starts at tmp01 + y)
        current_line_ptr = tmp01 + y;
        // c9aa4:
        //     clc
    }
    //     clv
    //     lda l007e
    return end_of_document;
}

[[nodiscard]] static bool flush_formatted_line(void)
{
    // Pseudocode: Completes line formatting: adjusts pointers updates ruler
    // stack Returns: true if write failed (V=1, caller should return
    // immediately)

    // sub_c9aa9:
    //     sec
    // (the sec exists only as the rol carry-in; nothing consumes C here)
    //     rol l007e
    line_format_status = (uint8_t)(line_format_status << 1) | 1;
    //     ldy l0047
    //     dey
    //     sty l003b
    // (y is used only to compute l003b; write_line_back_to_document overwrites
    //  y before reading it, so the register writes are dead)
    l003b = l0047 - 1;
    //     inc l006e
    edit_buffer_unpacked_flag++;
    //     jsr write_line_back_to_document
    if (!write_line_back_to_document())
        return false;
    //     pla (pop sub_c9aa9's return address)
    //     pla (pop sub_c9977's return address — stack unwind)
    //     lda #0x40 ; '@'
    //     sta l0084
    //     bit l0084   ; sets V
    // (the 6502 returns V=1 via the bit; the C return value conveys it)
    //     rts (return to sub_c9977's caller with V=1, bypassing sub_c9977's
    //     clv)
    return true;
}

void beep(void)
{
    // beep: Emits a beep.  The 6502 leaves carry set; callers that rely on
    // that set C themselves, so the C translation does not.

    //     lda #7
    uint8_t a = 7;
    //     jsr oswrch
    cli_putchar(a);
    // loop_caced:
    //     sec
    // return_74:
    //     rts
}

// Scans the document for the next format-command line, updating doc_ptr2 and
// ptr2.  Returns true if such a line was found (the 6502's Z flag set by the
// final ldx #0).
bool scan_document_for_next_line(void)
{

    // c8b7b
    // c8b7b:
    //     lda l007a

    uint8_t a = l007a;
    //     beq c8b78
    // c8b78:
    //     lda #0xff
    if (a == 0)
        return false;
    //     lda #0x14
    uint8_t a_1 = 0x14;
    //     sta l0048
    l0048 = a_1;
    //     ldx #0
    // (the 6502's ldx #0 overwrites Z before any branch reads it)

    uint8_t x = 0;
    //     stx l0049
    l0049 = x;
    //     stx l0081
    l0081 = x;
    //     lda doc_ptr2+0
    //     sta ((uint8_t*)&tmp89)[0]
    //     lda doc_ptr2+1
    //     sta ((uint8_t*)&tmp89)[1]

    addr_t tmp89 = doc_ptr2;
c8b91:
    // c8b91:
    //     lda ((uint8_t*)&tmp89)[1]
    //     cmp doc_ptr3+1
    //     bcc c8b9f
    //     bne signal_no_more_document
    //     lda ((uint8_t*)&tmp89)[0]
    //     cmp doc_ptr3
    //     bcs signal_no_more_document
    // (16-bit comparison: tmp89 < doc_ptr3)
    if (tmp89 < doc_ptr3)
        goto c8b9f;
    // c8b78:
    //     lda #0xff
    return false;
c8b9f:
    // c8b9f:
    //     ldy #0

    uint8_t y = 0;
    //     lda (((uint8_t*)&tmp89)[0]),y
    // (Z from this lda is clobbered by the following jsr)
    uint8_t a_2 = ram[tmp89];
    //     jsr check_for_command_prefix
    command_prefix_t cp = check_for_command_prefix(a_2);
    //     bne c8bb7
    if (cp == NO_COMMAND_PREFIX)
        goto c8bb7;
    //     lda ((uint8_t*)&tmp89)[0]
    //     clc
    //     adc #3
    //     sta doc_ptr2+0
    //     lda ((uint8_t*)&tmp89)[1]
    //     sta doc_ptr2+1
    //     bcc c8b7b
    //     bcs c8bdf
    // (16-bit arithmetic: doc_ptr2 = tmp89 + 3)
    doc_ptr2 = tmp89 + 3;
    return scan_document_for_next_line();

c8bb7:
    // c8bb7:
    //     jsr sub_c8c5f
    uint8_t a_3 = upper_case_unless_folding(a_2);
    //     sta l0083
    l0083 = a_3;
c8bbc:
    // c8bbc:
    //     iny
    y++;
    //     lda (((uint8_t*)&tmp89)[0]),y
    uint8_t a_4 = ram[tmp89 + y];
    //     beq c8bdb
    if (a_4 == 0)
        goto c8bdb;
    //     jsr check_for_command_prefix
    command_prefix_t cp2 = check_for_command_prefix(a_4);
    //     beq c8bdb
    if (cp2 != NO_COMMAND_PREFIX)
        goto c8bdb;
    //     lda header_text_maybe,x
    // (Z from this lda is clobbered by the following cmp #0x20)
    uint8_t a_5 = header_text_maybe[x];
    //     cmp #0x20 ; ' '
    if (a_5 == 0x20)
        goto c8bf7;
    //     beq c8bf7
    //     cmp #1
    if (a_5 == 1)
        goto c8be3;
    //     beq c8be3
    //     cmp #2
    if (a_5 == 2)
    {
        a_5 = 0x20;
    }
    // c8bd7:
    //     cmp l0083
    if (a_5 == l0083)
        goto c8c33;
    //     beq c8c33
c8bdb:
    // c8bdb:
    //     inc doc_ptr2+0
    //     bne c8b7b
    // c8bdf:
    //     inc doc_ptr2+1
    //     bne c8b7b
    // (16-bit arithmetic: doc_ptr2++)
    doc_ptr2++;
    return scan_document_for_next_line();
c8be3:
    // c8be3:
    //     lda l0083
    uint8_t a_6 = l0083;
    //     stx l0084
    l0084 = x;
    //     ldx l0049
    uint8_t x_1 = l0049;
    //     cpx #0x14
    if (x_1 < 0x14)
    {
        output_buffer[x_1] = a_6;
        l0049++;
    }
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
    uint8_t a_7 = l0083;
    //     cmp #0x20 ; ' '
    if (a_7 == 0x20)
        goto c8c23;
    //     beq c8c23
    //     cmp #9
    if (a_7 == 9)
        goto c8c23;
    //     beq c8c23
    //     cmp #0x0b
    if (a_7 == 0x0b)
        goto c8c23;
    //     beq c8c23
    //     cmp #0x1a
    if (a_7 == 0x1a)
        goto c8c23;
    //     beq c8c23
    //     cmp #0x0d
    if (a_7 == 0x0d)
        goto c8c23;
    //     beq c8c23
    //     lda l0081
    uint8_t a_8 = l0081;
    //     beq c8bdb
    if (a_8 == 0)
        goto c8bdb;
    //     jsr sub_c8c51
    // (inlined: append_to_output_buffer(0))
    append_to_output_buffer(0);
    //     lda #0
    // (Z from this lda is clobbered by the following ldx l0084)
    uint8_t a_9 = 0;
    //     sta l0081
    l0081 = a_9;
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
    append_to_output_buffer(a_7);
    //     ldx l0084
    x = l0084;
    //     sta l0081
    l0081 = a_7;
loop_c8c2a:
    // loop_c8c2a:
    //     inc ((uint8_t*)&tmp89)[0]
    tmp89++;
    //     bne c8c30
    //     inc ((uint8_t*)&tmp89)[1]
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
    //     inc ((uint8_t*)&tmp89)[0]
    tmp89++;
    //     bne c8c3e
    //     inc ((uint8_t*)&tmp89)[1]
c8c3e:
    // c8c3e:
    //     lda doc_ptr2+0
    //     ldy doc_ptr2+1
    //     ldx ((uint8_t*)&tmp89)[0]
    //     stx doc_ptr2+0
    //     ldx ((uint8_t*)&tmp89)[1]
    //     stx doc_ptr2+1
    //     sta ptr2
    //     sty ptr2+1
    // (16-bit copy: doc_ptr2 = tmp89; ptr2 = the previous doc_ptr2.  The
    //  6502 also returned the old address in YA for its callers, who now
    //  read ptr2 instead)
    ptr2 = doc_ptr2;
    doc_ptr2 = tmp89;
    //     ldx #0
    return true;
}

static void insert_line_into_document(addr_t ptr)
{

    //     sta ((uint8_t*)&tmp45)[0]

    addr_t tmp45 = ptr;
    //     lda #1
    //     sta ((uint8_t*)&tmp67)[0]
    //     lda #0
    //     sta ((uint8_t*)&tmp67)[1]

    addr_t tmp67 = 1;
    //     jsr make_space_for_insertion
    if (make_space_for_insertion(tmp45, tmp67))
    {
        ram[tmp45] = 0x0d;
        clamp_ptr6_to_document();
        return;
    }
    //     jmp ca941
    memory_full();
    return;
}

static void update_line_length(void)
{
    // Pseudocode: Sets line_lengths[ypos] = screen_width after cursor movement

    // ca684:
    //     ldx ypos

    uint8_t x = ypos;
    //     lda screen_width

    uint8_t a = screen_maxcolumn;
    //     sta line_lengths,x
    line_lengths[x] = a;
    //     rts
    return;
}

void clamp_ptr6_to_document(void)
{
    // ca741: Updates editor_ptr6 to current_line_ptr if editor_ptr6 is ahead,
    // sets refresh flags On entry: current_line_ptr, editor_ptr6 On exit:
    // editor_ptr6 = min(editor_ptr6, current_line_ptr), l0073 = l003d = 0xff

    //     ldx current_line_ptr
    //     ldy current_line_ptr+1
    //     cpy ptr6+1
    //     bcc ca74f
    //     bne ca753
    //     cpx ptr6
    //     bcs ca753
    if (current_line_ptr < editor_ptr6)
    {
        // ca74f:
        //     stx ptr6
        //     sty ptr6+1
        editor_ptr6 = current_line_ptr;
    }
    // ca753:
    //     ldx #0xff
    l0073 = 0xff;
    //     stx l0073
    //     stx l003d
    l003d = 0xff;
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

static void clear_to_eol(uint8_t a, uint8_t line)
{
    // Pseudocode: Fills remaining space on line with spaces to clear to end

    // sub_ca597:
    //     ldx l0082
    //     sta l0084
    //     lda line_lengths,x
    uint8_t line_len = line_lengths[line];

    //     beq return_62
    if (line_len == 0)
        goto return_62;
    //     lda l0084
    // loop_ca5a2:
    do
    {
        screen_putchar(a);
        line_lengths[line]--;
    } while (line_lengths[line] != 0);
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

void draw_line(struct render_state* rs, uint16_t addr)
{
    // draw_line
    // draw_line: Renders a single document line to the screen
    // On entry:  rs->line = screen line number, addr = address of the
    // document line (also stored in rs->line_ptr)

    //     sta ((uint8_t*)&tmp01)[0]
    rs->line_ptr = addr;
    tmp01 = rs->line_ptr;
    //     ldx #0
    //     ldy l0082
    screen_setcursor(0, rs->line);
    //     ldy #0
    rs->pos = 0;
    //     sty l0083
    rs->col = 0;
    //     sty input_buffer_offset+1
    rs->buf_off = 0;
    //     sty l0039
    rs->char_width = 0;
    //     jsr deref_and_check_for_command_prefix
    command_prefix_t f = deref_and_check_for_command_prefix(0, tmp01);
    //     bne ca4b4
    if (f == NO_COMMAND_PREFIX)
        goto ca4b4;
    //     ldy #3
    //     lda hscroll_pos
    if (hscroll_pos != 0)
        goto ca4b4;
    //     bne ca4b4
    //     ldy #1
    rs->pos = 1;
    //     jsr sub_ca4d7
    advance_to_next_char_and_render(rs);
    //     jsr sub_ca4d7
    advance_to_next_char_and_render(rs);
    //     lda #0x20 ; ' '
    rs->ch = 0x20;
    //     bne ca4bc
    goto ca4bc;

    // ca4b4:
ca4b4:
    //     lda #0x20 ; ' '
    rs->ch = 0x20;
    //     jsr ca4e9
    render_char(rs);
    //     jsr ca4e9
    render_char(rs);
// ca4bc:
ca4bc:
    //     jsr ca4e9
    render_char(rs);
    // loop_ca4bf:
    //     jsr process_current_document_character
    // loop_ca4c2:
    //     cmp #0x0d
    //     bne loop_ca4bf
    //     lda #0x20 ; ' '
    //     jsr sub_ca597
    // (loop restructured)
    while (1)
    {
        advance_to_next_char(rs);
        do
        {
            render_xchar(rs);
            rs->width--;
        } while (rs->width != 0);
        if (rs->ch == 0x0d)
            break;
    }
    clear_to_eol(0x20, rs->line);
    clear_to_eol(0x20, rs->line);
    //     lda l0083
    //     sta line_lengths,x
    line_lengths[rs->line] = rs->col;

    //     rts
}

void draw_prompt_characters(uint8_t x, uint8_t y)
{

    // draw_prompt_characters: Draws two inverted prompt characters at top-left
    // On entry: x, y = prompt characters
    // Uses: ((uint8_t*)&tmp23)[0], ((uint8_t*)&tmp23)[1]
    // On exit: cursor position restored

    //     stx ((uint8_t*)&tmp23)[0]

    //     jsr save_cursor_position
    save_cursor_position();
    //     jsr cursor_off
    cursor_off();
    //     jsr home_cursor
    home_cursor();
    //     jsr set_inverted_text_if_not_mode_7

    uint8_t a = STYLE_REVERSE;
    screen_setstyle(a);
    //     lda ((uint8_t*)&tmp23)[0]
    //     jsr screen_putchar
    screen_putchar(x);
    //     lda ((uint8_t*)&tmp23)[1]
    //     jsr screen_putchar
    screen_putchar(y);
    //     jsr set_normal_text_if_not_mode_7
    screen_setstyle(0);
    //     lda #0x20 ; ' '
    //     jsr screen_putchar
    screen_putchar(0x20);
    //     jsr restore_cursor_position
    restore_cursor_position(tmp45);
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
    //     lda status_line_needs_redrawing_flag
    //     beq return_64
    if (status_line_needs_redrawing_flag == 0)
        return;
    //     ldy #0
    //     sty status_line_needs_redrawing_flag
    status_line_needs_redrawing_flag = 0;

    //     sty l0082
    struct render_state rs = {.line = 0};
    draw_line(&rs, current_ruler_ptr);
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
    // Pseudocode: Redraws status line showing format mode, justify, and insert
    // indicators

    // sub_ca651:
    //     lda #0

    //     sta flags_need_redrawing_flag
    flags_need_redrawing_flag = 0;
    //     jsr home_cursor
    home_cursor();
    //     ldx #0x46 ; 'F'

    uint8_t x = 0x46;
    //     lda format_mode_flag

    uint8_t a = format_mode_flag;
    //     beq ca666
    if (a != 0)
    {
        x = 0x4d;
        a &= 0xc0;
        if (a == 0)
        {
            x = 0x20;
        }
    }
    //     txa
    uint8_t a_1 = x;
    //     jsr screen_putchar
    screen_putchar(a_1);
    //     lda #0x4a ; 'J'
    uint8_t a_2 = 0x4a;
    //     ldx justifying_flag
    uint8_t x_1 = justifying_flag;
    //     beq ca672
    if (x_1 != 0)
    {
        a_2 = 0x20;
    }
    //     jsr screen_putchar
    screen_putchar(a_2);
    //     lda #0x49 ; 'I'
    //     ldx insert_mode_flag
    uint8_t x_2 = insert_mode_flag;
    //     bne ca681
    if (x_2 != 0)
    {
        home_cursor();
        return;
    }
    //     lda #0x20 ; ' '
    //     bne ca681                                                         ;
    //     ALWAYS branch
    home_cursor();
    return;
}

static uint8_t get_line_length(void)
{
    uint8_t a_3;
    command_prefix_t cp_1;

    // get_line_length
    // Pseudocode: Returns the length of the current edit line

    // ;
    // ***************************************************************************************
    // get_line_length:
    //     ldy #0
    //     lda (current_format_line_ptr),y

    uint8_t a = ram[current_format_line_ptr + 0];
    //     jsr check_for_command_prefix
    command_prefix_t cp = check_for_command_prefix(a);
    //     php
    {
        command_prefix_t saved_cp = cp;
        //     ldy #0x84

        uint8_t y = MAX_LINE_LENGTH;
        // loop_caafb:
        for (;;)
        {
            //     dey
            y--;
            //     lda (current_edit_line_ptr),y
            uint8_t a_1 = ram[RAM_EDIT_BUFFER + y];
            //     cmp #0x10
            if (a_1 != 0x10)
                goto cab06;
            //     bne cab06
            //     tya
            uint8_t a_2 = y;
            //     bne loop_caafb
            if (a_2 != 0)
                continue;
            break;
        }
        //     dey
        y--;
        // cab06:
    cab06:
        //     iny
        y++;
        //     tya
        a_3 = y; //     plp
        cp_1 = saved_cp;
    }
    //     bne return_69
    if (cp_1 != NO_COMMAND_PREFIX)
    {
        a_3 += 3;
    }
    //     rts
    return a_3;
}

static void go_to_marker(uint8_t x)
{
    // go_to_marker:
    //     lda markers_array,x
    //     ldy markers_array+1,x
    //     jsr move_cursor_to_address
    move_cursor_to_address(markers_array[x]);
    // ca035:
    //     lda #1
    //     sta l0073
    l0073 = 1;
    //     jmp ca684
    update_line_length();
    return;
}

static void go_to_marker_n(uint8_t marker)
{
    //     pha
    //     jsr ca93c
    write_line_back_to_document_safely();
    //     pla
    //     jsr lookup_marker
    int idx = lookup_marker(marker);
    //     jmp go_to_marker
    // (skip the jump if the marker is unset, mirroring sf6_go_to_marker_key's
    //  Z-flag guard — otherwise an unset marker's null address is treated as a
    //  document position)
    if (markers_array[idx] != 0)
        go_to_marker(idx);
    return;
}

static void home_cursor(void)
{
    // home_cursor
    // home_cursor:
    // ca681:
    //     ldx #0
    //     ldy #0
    //     jmp set_cursor_position
    screen_setcursor(0, 0);
    return;
}

uint8_t justify_edit_buffer(addr_t ptr1)
{

    uint8_t x = 0;

    // Pseudocode: Word-spacing justification: distributes extra spaces between
    // words

    // justify_edit_buffer:
    //     lda justifying_flag

    uint8_t a = justifying_flag;
    if (a != 0)
        return x;
    //     bne return_47
    //     sta l0046
    l0046 = a;
    //     sta l0039
    l0039 = a;
    //     sta l0042
    l0042 = a;
    //     lda ruler_right_stop
    uint8_t a_1 = ruler_right_stop;

    //     beq return_47
    if (a_1 == 0)
        return x;
    //     jsr get_line_length
    l0043 = get_line_length();
    //     ldy #0

    uint8_t y = 0;
    //     beq c9861                                                         ;
    //     ALWAYS branch
    goto c9861;

    // c9847:
c9847:
    //     lda l0039
    a_1 = l0039;
    //     sta l0084
    l0084 = a_1;
    //     iny
    y++;
    //     cpy l0043
    if (y == l0043)
        goto c9871;
    //     clc
    //     jsr sub_c9936
    bool is_zero = process_char_for_output(y, false, &x, &a_1);
    //     beq c985c
    if (is_zero)
        goto c985c;
    //     cmp #0x20 ; ' '
    if (a_1 != 0x20)
        goto c9847;
    //     bne c9847
    //     inc l0046
    l0046++;
    // c985c:
c985c:
    //     iny
    y++;
    //     cpy l0043
    if (y == l0043)
        goto c986d;
    // c9861:
c9861:
    //     sec
    //     jsr sub_c9936
    _Bool is_zero_1;
    is_zero_1 = process_char_for_output(y, true, &x, &a_1);
    //     beq c985c
    if (is_zero_1)
        goto c985c;
    //     cmp #0x20 ; ' '
    if (a_1 != 0x20)
        goto c9847;
    //     beq c985c                                                         ;
    //     ALWAYS branch
    goto c985c;

    // c986d:
c986d:
    //     dec l0046
    l0046--;

    //     bmi return_47
    if (l0046 & 0x80)
        return x;
    // c9871:
c9871:
    //     lda l0046
    uint8_t a_2 = l0046;

    //     beq return_47
    if (a_2 == 0)
        return x;
    //     lda ruler_right_stop
    uint8_t a_3 = ruler_right_stop;
    //     sec
    //     sbc l0084
    //     bcc return_47
    // (C=1: plain subtraction; if it borrows, abort)
    if (a_3 < l0084)
        return x;
    a_3 -= l0084;
    //     adc #0
    //     tax
    //     adc l0043
    //     sec
    //     sbc #MAX_LINE_LENGTH
    //     bcc c988c
    // (the 6502 chains adc/sbc only to stay within 8 bits; in C this is one
    //  value: if the line already reaches the margin, the slack to distribute
    //  is MAX_LINE_LENGTH - l0043)
    uint8_t x_1 = (uint8_t)(a_3 + 1);
    uint8_t extra = (uint8_t)(a_3 + 1 + l0043);
    if (extra >= MAX_LINE_LENGTH)
    {
        l0084 = (uint8_t)(extra - MAX_LINE_LENGTH);
        x_1 = (uint8_t)(x_1 - l0084);
    }
    //     stx l0082
    l0082 = x_1;
    //     stx ((uint8_t*)&tmp89)[0]
    //     lda #0
    //     sta ((uint8_t*)&tmp89)[1]

    addr_t tmp89 = x_1;
    //     jsr sub_cadf0
    //     sta l0045
    uint8_t a_4 = tmp89 % l0046;
    addr_t tmp89_1 = tmp89 / l0046;
    l0045 = a_4;
    //     lda ((uint8_t*)&tmp89)[0]
    uint8_t a_5 = ((uint8_t*)&tmp89_1)[0];
    //     sta l0044
    l0044 = a_5;
    //     ldy #0
    uint8_t y_1 = 0;
    //     ldx l0046
    uint8_t x_2 = l0046;
    //     tya                                                               ;
    //     A=0x00
    uint8_t a_6 = y_1;
    // loop_c98a2:
    //     sta input_buffer,y
    do
    {
        input_buffer[y_1] = a_6;
        y_1++;
        x_2--;
    } while (x_2 != 0);
    //     ldy print_xpos
    uint8_t y_2 = print_xpos;
    //     iny
    y_2++;
    //     cpy l0046
    if (y_2 >= l0046)
    {
        y_2 = 1;
    }
    //     dey
    y_2--;
    //     ldx l0046
    uint8_t x_3 = l0046;
    // c98b5:
c98b5:
    //     lda l0045
    uint8_t a_7 = l0045;

    //     beq c98bd
    if (a_7 != 0)
    {
        a_7 = 1;
        l0045--;
    }
    //     clc
    //     adc l0044
    // (plain addition: a += l0044)
    a_7 += l0044;
    //     sta input_buffer,y
    input_buffer[y_2] = a_7;
    //     lda l0082
    uint8_t a_8 = l0082;
    //     sec
    //     sbc input_buffer,y
    // (sec makes this a plain 8-bit subtraction; the result's Z flag is tested
    //  by the following beq, so compare directly instead of via php/plp)
    uint8_t a_9 = a_8 - input_buffer[y_2];
    //     sta l0082
    l0082 = a_9;
    //     iny
    y_2++;
    //     cpy l0046
    //     bcc c98d3
    if (y_2 >= l0046)
    {
        y_2 = 0;
    }
    //     beq c98d9
    if (a_9 == 0)
        goto c98d9;
    //     dex
    x_3--;
    //     bne c98b5
    if (x_3 != 0)
        goto c98b5;
    // c98d9:
c98d9:
    //     sty print_xpos
    print_xpos = y_2;
    //     ldy #0
    uint8_t y_3 = 0;
    //     sty l0081
    l0081 = y_3;
    //     sty l0039
    l0039 = y_3;
    //     lda #0x1a
    //     jsr wipe_buffer
    wipe_buffer(0x1a, ptr1);
    //     lda l0042

    uint8_t a_10 = l0042;

    //     beq c98f6
    if (a_10 == 0)
        goto c98f6;
    //     ldy #0
    uint8_t y_4 = 0;
    // loop_c98ec:
    //     lda output_buffer,y
    do
    {

        uint8_t a_11 = output_buffer[y_4];
        ram[RAM_EDIT_BUFFER + y_4] = a_11;
        y_4++;
    } while (y_4 != l0042);
    //     bne loop_c98ec
    // c98f6:
c98f6:
    //     ldy l0042
    uint8_t y_5 = l0042;
    //     ldx l0042
    uint8_t x_4 = l0042;
    // c98fa:
c98fa:
    //     lda output_buffer,x

    uint8_t a_12 = output_buffer[x_4];
    //     cmp #0x20 ; ' '
    if (a_12 != 0x20)
        goto c9920;
    //     bne c9920
    //     lda l0081

    uint8_t a_13 = l0081;

    //     beq c991c
    if (a_13 == 0)
        goto c991c;
    //     sty l0084
    l0084 = y_5;
    //     ldy l0039
    uint8_t y_6 = l0039;
    //     cpy l0046
    //     lda #0
    //     bcs c9912
    if (y_6 < l0046)
    {
        a_13 = input_buffer[y_6];
    }
    //     clc
    //     adc l0084
    a_13 += l0084;
    //     inc l0039
    l0039++;
    //     tay
    y_5 = a_13;
    //     lda #0

    uint8_t a_14 = 0;
    //     sta l0081
    l0081 = a_14;
    // c991c:
c991c:
    //     lda #0x20 ; ' '
    a_12 = 0x20;
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
    ram[RAM_EDIT_BUFFER + y_5] = a_12;
    //     iny
    y_5++;
    //     inx
    x_4++;
    //     cpx l0043
    if (x_4 != l0043)
        goto c98fa;
    //     bne c98fa
    //     lda #0x10

    uint8_t a_15 = 0x10;
    // loop_c992c:
    //     cpy #0x84
    while (1)
    {
        if (y_5 >= MAX_LINE_LENGTH)
            return x_4;
        ram[RAM_EDIT_BUFFER + y_5] = a_15;
        y_5++;
    }
    // return_48:
    //     rts
    return x_4;
}

bool make_space_for_insertion(addr_t tmp45, addr_t tmp67)
{

    // make_space_for_insertion: Shifts content up to make space for insertion
    // (6437) On entry: ((uint8_t*)&tmp45)[0]:((uint8_t*)&tmp45)[1] = block
    // base, ((uint8_t*)&tmp67)[0]:((uint8_t*)&tmp67)[1] = size, top = current
    // top On exit:  top += size, pointer_array entries >= base adjusted, block
    // shifted Uses: ((uint8_t*)&tmp23)[0], ((uint8_t*)&tmp23)[1],
    // ((uint8_t*)&tmp89)[0], ((uint8_t*)&tmp89)[1]

    //     lda top (6438)
    //     sta ((uint8_t*)&tmp23)[0] (6439)
    //     clc (6440)
    //     adc ((uint8_t*)&tmp67)[0] (6441)
    //     sta ((uint8_t*)&tmp89)[0] (6442)
    //     tax (6443)
    //     lda top+1 (6444)
    //     sta ((uint8_t*)&tmp23)[1] (6445)
    //     adc ((uint8_t*)&tmp67)[1] (6446)
    //     sta ((uint8_t*)&tmp89)[1] (6447)
    //     tay (6448)
    //     cpy himem+1 (6449)
    //     caa32: (6454)
    //     stx top (6455) sty top+1 (6456)
    // (16-bit arithmetic: tmp23 = top, then tmp89 = top + tmp67;
    //  if that exceeds himem there is not enough space)

    addr_t tmp23 = top;

    addr_t tmp89 = top + tmp67;
    if (tmp89 >= himem)
        return false;
    top = tmp89;
    //     ldx #0 (6457)

    uint8_t x = 0;
    // loop_caa38: (6458)
    for (;;)
    {
        //     ldy __begin_pointer_array+1,x (6459)
        //     lda __begin_pointer_array+0,x (6460)
        //     cpy ((uint8_t*)&tmp45)[1] (6461)
        //     bcc caa51 (6462)
        //     bne caa46 (6463)
        //     cmp ((uint8_t*)&tmp45)[0] (6464)
        //     bcc caa51 (6465)
        // (16-bit comparison: pointer_array[x] < tmp45 → caa51, else caa46)
        if (((addr_t*)&pointer_array)[x] < tmp45)
            goto caa51;
        // caa46: (6466)
        //     clc (6467)
        //     adc ((uint8_t*)&tmp67)[0] (6468)
        //     sta __begin_pointer_array+0,x (6469)
        //     lda __begin_pointer_array+1,x (6470)
        //     adc ((uint8_t*)&tmp67)[1] (6471)
        //     sta __begin_pointer_array+1,x (6472)
        // (16-bit addition: pointer_array[x] += tmp67)
        ((addr_t*)&pointer_array)[x] += tmp67;
        // caa51: (6473)
    caa51:
        //     inx (6474)
        //     inx (6475)
        //     cpx #22 (6476)
        // (the 6502 steps x by two bytes per entry; x counts elements here)
        x++;
        if (x != sizeof(pointer_array) / sizeof(addr_t))
            continue;
        break;
    }
    // caa57: (6478)
    //     lda ((uint8_t*)&tmp23)[0] (6479)
    //     sec (6480)
    //     sbc ((uint8_t*)&tmp45)[0] (6481)
    //     tax (6482)
    //     lda ((uint8_t*)&tmp23)[1] (6483)
    //     sbc ((uint8_t*)&tmp45)[1] (6484)
    //     beq caa65 (6485)
    //     ldx #0xff (6487)
    // caa65: (6488)
    //     txa (6489)
    //     tay (6490)
    //     iny (6491)
    //     lda ((uint8_t*)&tmp23)[0] (6492)
    //     stx ((uint8_t*)&tmp23)[0] (6493)
    //     sec (6494)
    //     sbc ((uint8_t*)&tmp23)[0] (6495)
    //     sta ((uint8_t*)&tmp23)[0] (6496)
    //     bcs caa75 (6497)
    //     dec ((uint8_t*)&tmp23)[1] (6498)
    // caa75: (6499)
    //     lda ((uint8_t*)&tmp89)[0] (6500)
    //     stx ((uint8_t*)&tmp89)[0] (6501)
    //     sec (6502)
    //     sbc ((uint8_t*)&tmp89)[0] (6503)
    //     sta ((uint8_t*)&tmp89)[0] (6504)
    //     bcs caa82 (6505)
    //     dec ((uint8_t*)&tmp89)[1] (6506)
    // caa82: (6507)
    //     dey (6508)
    //     lda (tmp23),y (6509)
    //     sta (tmp89),y (6510)
    //     tya (6511)
    //     bne caa82 (6512)
    //     inx (6513)
    //     beq caa57 (6514)
    // (byte shift consolidated into a single memmove: copies [tmp45, tmp23]
    //  inclusive, i.e. (top - base) + 1 bytes, to [tmp45 + tmp67, tmp89])
    addr_t old_top = tmp23;
    size_t copy_len = (size_t)(old_top - tmp45) + 1;
    memmove(&ram[tmp45 + tmp67], &ram[tmp45], copy_len);
    //     clc (6515)
    // return_67: (6516)
    //     rts (6517)
    return true;
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

uint8_t process_current_document_character(
    addr_t tmp01, uint8_t* x, uint8_t* y, bool* is_tab)
{
    // draw_char:
    //     lda (((uint8_t*)&tmp01)[0]),y
    uint8_t a = ram[tmp01 + *y];
    //     iny
    (*y)++;
    uint8_t a_1 = process_document_character(a, x, is_tab);
    return a_1;
}

static void recalculate_cursor_xpos(void)
{
    uint8_t x;

    // recalculate_cursor_xpos
    // Pseudocode: Recalculates cursor xpos from visual position accounting for
    // tabs and margins

    // sub_ca608:
    //     lda current_edit_line_ptr
    //     sta ((uint8_t*)&tmp01)[0]
    //     lda current_edit_line_ptr+1
    //     sta ((uint8_t*)&tmp01)[1]

    addr_t tmp01 = RAM_EDIT_BUFFER;
    //     lda l0079

    uint8_t a = l0079;
    // (The SBC at ca5f1 in process_document_character uses the previous
    //  character's tab status as its borrow-in; propagate it across the walk.)
    bool is_tab = false;
    if (a != 0)
        goto ca624;
    //     bne ca624
    //     tay

    uint8_t y = a;
    // loop_ca615:
    for (;;)
    {
        //     cpy xpos
        if (y == xpos)
            goto ca63d;
        //     beq ca63d
        //     sta l0039
        l0039 = a;
        //     jsr process_current_document_character
        (void)process_current_document_character(tmp01, &x, &y, &is_tab);
        //     txa
        a = x;
        //     clc
        //     adc l0039
        //     bcc loop_ca615
        // (adc with C=0 is a plain addition; carry means overflow)
        a += l0039;
        if (a >= l0039)
            continue;
        break;
    }
    // ca624:
ca624:
    //     lda #0
    a = 0;
    //     sta l0079
    l0079 = a;
    //     tay                                                               ;
    //     Y=0x00
    uint8_t y_1 = a;
    // loop_ca629:
    do
    {
        l0039 = a;
        (void)process_current_document_character(tmp01, &x, &y_1, &is_tab);
        a = x;
        a += l0039;
    } while (a < l0072);
    //     beq ca63b
    if (a != l0072)
    {
        a = l0039;
        y_1--;
    }
    //     sty xpos
    xpos = y_1;
    // ca63d:
ca63d:
    //     sta l0072
    l0072 = a;
    // return_64:
    //     rts
    return;
}

void redraw_editor(void)
{
    // redraw_editor
    //  Ptrs:   ptr6
    // Pseudocode: Main screen update routine: scrolls, redraws lines, updates
    // status and cursor

    // (the 6502 held the line being drawn in the YA register pair; use a real
    //  16-bit variable instead)

    // redraw_editor:                                                    (5206)
    //     jsr cursor_off                                                (5207)
    cursor_off();
    //     lda ruler_stack_ptr                                           (5208)

    uint8_t a = ruler_index_ptr;
    //     sta l0034                                                     (5209)
    l0034 = a;
    //     lda l0076                                                     (5210)
    uint8_t a_1 = status_line_needs_redrawing_flag;
    //     sta input_buffer_ptr+1                                        (5211)

    uint8_t saved_status_line_needs_redrawing_flag = a_1;
    //     lda l006e                                                     (5212)
    uint8_t a_2 = edit_buffer_unpacked_flag;
    //     beq ca28e                                                     (5213)
    if (a_2 == 0)
        goto ca28e;
    //     lda l0073                                                     (5214)
    uint8_t a_3 = l0073;
    //     ora l006f                                                     (5215)
    a_3 |= l006f;
    //     bne ca28e                                                     (5216)
    if (a_3 != 0)
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
    uint8_t a_4 = l006f;
    //     bne ca30d (5229)
    if (a_4 != 0)
        goto ca30d;
    //     lda l0033 (5230)
    uint8_t a_5 = l0033;
    //     sta ruler_stack_ptr (5231)
    ruler_index_ptr = a_5;
    //     ldy l0012 (5232) lda l0011 (5233) cpy top+1 (5234) bcc ca2b2 (5235)
    //     bne ca30d (5236) cmp top (5237) bcs ca30d (5238)
    if (top_of_screen_line_ptr >= top)
        goto ca30d;
    // ca2b2: (5239)
    //     jsr sub_cab37 (5240)
    find_previous_line(top_of_screen_line_ptr, &tmp01);
    //     ldy ((uint8_t*)&tmp01)[1] (5241)
    //     cpy current_line_ptr+1 (5242)
    //     lda ((uint8_t*)&tmp01)[0] (5244)
    //     cmp current_line_ptr (5245)
    // (16-bit equality consolidated)
    if (tmp01 != current_line_ptr)
        goto ca30d;
    //     sty l0012 (5247) sta l0011 (5248)
    top_of_screen_line_ptr = tmp01;
    //     ldx screen_height (5249)

    uint8_t x = screen_maxrow;
    // loop_ca2c7: (5250)
    do
    {
        x--;
        uint8_t a_6 = line_lengths[x];
        x++;
        line_lengths[x] = a_6;
        x--;
    } while (x != 0);
    //     ldy #SCREEN_SCROLLDOWN (5257) jsr SCREEN (5258)
    screen_scrolldown();
    //     jsr home_cursor (5259)
    screen_setcursor(0, 1);
    //     ldy #1 (5260)

    uint8_t y = 1;
    //     jmp ca351 (5261)
    goto ca351;

    // ca2dc: (5263)
ca2dc:
    //     lda l0033 (5264)
    uint8_t a_7 = l0033;
    //     sta ruler_stack_ptr (5265)
    ruler_index_ptr = a_7;
    // ca2e0: (5266)
ca2e0:
    //     ldx #0 (5267)
    uint8_t x_1 = 0;
    //     lda l0011 / ldy l0012  (folded into the walk variable)
    addr_t walk = top_of_screen_line_ptr;
    // ca2e6: (5270)
ca2e6:
    //     inx (5271)
    x_1++;
    //     cpy ptr6+1 (5272)
    //     cmp ptr6 (5274)
    // (16-bit comparison: walk == editor_ptr6)
    if (walk == editor_ptr6)
    {
        l003d = x_1;
    }
    //     cpy current_line_ptr+1 (5278)
    //     cmp current_line_ptr (5280)
    // (16-bit comparison: walk == current_line_ptr)
    if (walk == current_line_ptr)
        goto ca313;
    // ca2f9: (5282)
    //     jsr sub_cab1a (5283)
    uint8_t y_1;
    if (advance_to_next_line(walk, &tmp01, &y_1))
        goto ca313;
    //     beq ca313 (5284)
    //     tya / ldy tmp01[1] / clc / adc tmp01[0]
    // (tmp01 += y; y holds the offset past the CR left by find_next_line)
    tmp01 += y_1;
    walk = tmp01;
    //     cpx screen_height (5292)
    //     beq ca2e6 (5293)
    if (x_1 <= screen_maxrow)
        goto ca2e6;
    // ca30d: (5295)
ca30d:
    //     jsr sub_ca44e (5296)
    (void)compute_display_start_line();
    //     jmp ca2e0 (5297)
    goto ca2e0;

    // ca313: (5299)
ca313:
    //     cpx screen_height (5300)
    if (x_1 <= screen_maxrow)
        goto ca35e;
    //     lda l006f (5303)
    uint8_t a_8 = l006f;
    //     bne ca30d (5304)
    if (a_8 != 0)
        goto ca30d;
    //     ldx #0 (5305)
    uint8_t x_3 = 0;
    // loop_ca31f: (5306)
    do
    {
        uint8_t a_9 = line_lengths[x_3 + 1];
        line_lengths[x_3] = a_9;
        x_3++;
    } while (x_3 != screen_maxrow);
    //     bne loop_ca31f (5311)
    //     dec l003d (5312)
    l003d--;
    //     ldx #0 (5313)
    uint8_t x_4 = 0;
    //     lda screen_width (5314)

    uint8_t a_10 = screen_maxcolumn;
    //     sta line_lengths,x (5315)
    line_lengths[x_4] = a_10;
    //     lda l0033 (5316)

    uint8_t a_11 = l0033;
    //     sta ruler_stack_ptr (5317)
    ruler_index_ptr = a_11;
    //     ldy l0012 / lda l0011  (these loaded bytes are clobbered by
    //     advance_to_next_line's scan; y is left holding the offset past the
    //     CR, which is what the following addition needs)
    //     jsr sub_cab1a (5320)
    advance_to_next_line(top_of_screen_line_ptr, &tmp01, &y_1);
    //     tya (5321)
    //     clc (5322)
    //     adc l0011 (5323)
    //     sta l0011 (5324)
    //     bcc ca348 (5325)
    // (16-bit arithmetic: top_of_screen_line_ptr += y)
    top_of_screen_line_ptr += y_1;
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

    uint8_t a_12 = ruler_index_ptr;
    //     sta l0033 (5335)
    l0033 = a_12;
    //     inc input_buffer_ptr+1 (5336)
    saved_status_line_needs_redrawing_flag++;
    //     inc l0074 (5337)
    l0074++;
    //     tya (5338)

    uint8_t a_13 = y;
    //     tax (5339)
    x_1 = a_13;
    // ca35e: (5340)
ca35e:
    //     stx ypos (5341)
    ypos = x_1;
    // ca360: (5342)
ca360:
    //     ldy l0034 (5343)
    //     jsr cab91 (5344)
    load_current_ruler(l0034);
    //     jsr unpack_line_into_buffer (5345)
    unpack_line_into_buffer(ptr1);
    //     jsr sub_ca608 (5346)
    recalculate_cursor_xpos();
    //     lda screen_width (5347)

    uint8_t a_14 = screen_maxcolumn;
    //     lsr (5348)
    a_14 >>= 1;
    //     sta l0083 (5349)
    l0083 = a_14;
    //     lda l0072 (5350)

    uint8_t a_15 = l0072;
    //     cmp hscroll_pos (5351)
    if (a_15 < hscroll_pos)
        goto ca381;
    //     lda hscroll_pos / clc / adc screen_width / sbc #3 / cmp l0072 (5357)
    // (the 8-bit sequence just tests whether the cursor x (l0072) is still
    //  within the visible right edge (hscroll_pos + screen_width, less a
    //  4-column margin for the borrow); do the comparison directly)
    if ((int)l0072 <= (int)hscroll_pos + (int)screen_maxcolumn - 4)
        goto ca395;
    //     bcs ca395 (5358)
    // ca381: (5359)
ca381:
    //     lda l0072 (5360)

    uint8_t a_16 = l0072;
    //     sec (5361)
    //     sbc l0083 (5362)
    //     bcs ca38a (5363)
    // (sbc with C=1 is a plain subtraction; the borrow selects 0)
    if (a_16 < l0083)
    {
        a_16 = 0;
    }
    else
    {
        a_16 -= l0083;
    }
    //     sta hscroll_pos (5366)
    hscroll_pos = a_16;
    //     lda #1 (5367)

    uint8_t a_17 = 1;
    //     sta l0073 (5368)
    l0073 = a_17;
    //     sta input_buffer_ptr+1 (5369)
    saved_status_line_needs_redrawing_flag = a_17;
    //     jsr ca93c (5370)
    write_line_back_to_document_safely();
    // ca395: (5371)
ca395:
    //     lda input_buffer_ptr+1 (5372)

    uint8_t a_18 = saved_status_line_needs_redrawing_flag;
    //     sta l0076 (5373)
    status_line_needs_redrawing_flag = a_18;
    //     lda l0073 (5374)

    uint8_t a_19 = l0073;
    //     beq ca3e7 (5375)
    if (a_19 == 0)
        goto ca3e7;
    //     bpl ca3b2 (5376)
    if ((int8_t)a_19 >= 0)
        goto ca3b2;
    //     lda l003d (5377)

    uint8_t a_20 = l003d;
    //     bmi ca3b2 (5378)
    if ((int8_t)a_20 < 0)
        goto ca3b2;
    //     sta l0082 (5379)
    l0082 = a_20;
    //     lda screen_height (5380)

    uint8_t a_21 = screen_maxrow;
    //     sec (5381)
    //     sbc l003d (5382)
    a_21 -= l003d;
    //     tax (5383)
    uint8_t x_5 = a_21;
    //     inx (5384)
    x_5++;
    //     lda ptr6 / ldy ptr6+1 / bne ca3c1

    addr_t draw = editor_ptr6;
    if (editor_ptr6 != 0)
        goto ca3c1;
    // ca3b2: (5388)
ca3b2:
    //     ldy l0033 (5389)
    //     jsr cab91 (5390)
    load_current_ruler(l0033);
    //     lda #1 (5391)
    //     sta l0082 (5392)
    l0082 = 1;
    //     lda l0011 / ldy l0012
    draw = top_of_screen_line_ptr;
    //     ldx screen_height (5395)
    x_5 = screen_maxrow;
    // ca3c1: (5396)
ca3c1:
    //     stx l0081 (5397)
    l0081 = x_5;
    // loop_ca3c3: (5398)
    for (;;)
    {
        //     jsr sub_ca486 (5399)
        struct render_state rs = {.line = l0082};
        draw_line(&rs, draw);
        //     lda ((uint8_t*)&tmp01)[0] (5400)

        //     ldy ((uint8_t*)&tmp01)[1] (5401)
        uint8_t y_2 = 0;
        //     jsr sub_cab1a (5402)
        if (advance_to_next_line(tmp01, &tmp01, &y_2))
            goto ca422;
        //     beq ca422 (5403)
        //     tya (5404)
        //     ldy ((uint8_t*)&tmp01)[1] (5405)
        //     clc (5406)
        //     adc ((uint8_t*)&tmp01)[0] (5407)
        //     bcc ca3d8 (5408)
        // (16-bit arithmetic: tmp01 += y; result kept in a/y for the loop)
        tmp01 += y_2;
        draw = tmp01;
        //     inc l0082 (5411)
        l0082++;
        //     dec l0081 (5412)
        l0081--;
        //     bne loop_ca3c3 (5413)
        if (l0081 != 0)
            continue;
        break;
    }
    // ca3de: (5414)
ca3de:
    //     lda #0 (5415)

    uint8_t a_23 = 0;
    //     sta l0074 (5416)
    l0074 = a_23;
    //     ldy l0034 (5417)
    //     jsr cab91 (5418)
    load_current_ruler(l0034);
    // ca3e7: (5419)
ca3e7:
    //     jsr unpack_line_into_buffer (5420)
    unpack_line_into_buffer(ptr1);
    //     jsr sub_caacb (5421)
    update_markers_to_format_buffer();
    //     jsr draw_ruler (5422)
    draw_ruler();
    //     lda l0074 (5423)

    uint8_t a_24 = l0074;
    //     beq ca3ff (5424)
    if (a_24 != 0)
    {

        uint8_t a_25 = ypos;
        l0082 = a_25;
        struct render_state rs_1 = {.line = l0082};
        draw_line(&rs_1, current_format_line_ptr);
    }
    //     lda flags_need_redrawing_flag (5431)

    uint8_t a_26 = flags_need_redrawing_flag;
    //     beq ca406 (5432)
    if (a_26 != 0)
    {
        draw_status_word();
    }
    //     lda l0072 (5435)

    uint8_t a_27 = l0072;
    //     sec (5436)
    //     sbc hscroll_pos (5437)
    //     clc (5438)
    //     adc #3 (5439)
    a_27 -= hscroll_pos;
    a_27 += 3;
    //     tax (5440)
    uint8_t x_6 = a_27;
    //     ldy #0 (5441)
    //     sty l0073 (5442)
    l0073 = 0;
    //     sty l0074 (5443)
    l0074 = 0;
    //     sty l006f (5444)
    l006f = 0;
    //     dey (5445)
    uint8_t y_3 = -1;
    //     sty ptr6+1 (5446)
    editor_ptr6 = (editor_ptr6 & 0x00ff) | ((uint16_t)y_3 << 8);
    //     ldy ypos (5447) jsr set_cursor_position (5448)
    screen_setcursor(x_6, ypos);
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
    uint8_t x_7 = l0082;
    //     lda screen_width (5455)

    uint8_t a_28 = screen_maxcolumn + 1;
    //     sta line_lengths+1,x (5456)
    line_lengths[x_7 + 1] = a_28;
    //     sta l0083 (5457)
    l0083 = a_28;
    //     lda #0x2a ; '*' (5458)

    uint8_t a_29 = 0x2a;
    // loop_ca431: (5459)
    do
    {
        l0082++;
        screen_setcursor(0, l0082);
        clear_to_eol(a_29, l0082);

        uint8_t a_30 = l0083;
        line_lengths[l0082] = a_30;

        uint8_t a_31 = 0;
        l0083 = a_31;
        a_29 = 0x20;
        l0081--;
    } while (l0081 != 0);
    //     beq ca3de (5472)
    goto ca3de;
}

static void render_char(struct render_state* rs)
{

    // render_char
    // ca4e9: Renders character to screen with attribute handling.
    //
    // Marker handling:
    //   Highlight toggles 0x1c/0x1d are replaced with '-'/'*' and
    //   displayed inverted.  Markers at index 0 (match via sub_ca536)
    //   enable REVERSE style for the character.  After output, style
    //   is reset to NORMAL if x (the marker index / screen column, as
    //   left by the flow above) is zero.  CR and NUL are replaced with
    //   space.

    uint8_t char_to_render = rs->ch;

    //     ldx l0082

    uint8_t x = rs->line;
    //     lda line_lengths,x
    if (line_lengths[x] != 0)
    {
        //     dec line_lengths,x
        line_lengths[x]--;
    }
    // ca4f4:
    //     ldx l0083
    uint8_t x_1 = rs->col;
    //     cpx screen_width
    if (x_1 >= screen_maxcolumn)
    {
        rs->ch = char_to_render;
        return;
    }
    //     inc l0083
    rs->col++;
    //     tya

    uint8_t a = rs->pos;
    //     beq ca514
    if (a == 0)
        goto ca514;
    //     dey
    //     jsr sub_ca536

    addr_t tmp67 = rs->line_ptr;
    x_1 = find_marker_at_position(rs->pos - 1, tmp67);
    //     iny
    //     cpx #4
    if (x_1 >= 4)
        goto ca514;
    //     tax
    x_1 = 0;
    //     bmi ca523
    if (x_1 & 0x80)
    {
        rs->ch = char_to_render;
        goto ca523;
    }
    //     bne ca514
    if (x_1 != 0)
        goto ca514;
    //     pla
    uint8_t a_1 = STYLE_REVERSE;
    screen_setstyle(a_1);
ca514:
    a = char_to_render;
    //     jsr check_for_control_code
    control_code_t f = check_for_control_code(a);
    if (f != NO_CONTROL_CODE)
    {
        if (f == HIGHLIGHT1_CODE)
        {
            a = 0x2d;
        }
        else
        {
            a = 0x2a;
        }
    }
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
    //     txa  (use x for the style-reset decision)
    //     bne ca532
    if (x_1 == 0)
    {
        screen_setstyle(0);
    }
    rs->ch = char_to_render;
    //     rts
}

static void render_xchar(struct render_state* rs)
{

    // render_xchar: Renders a character to screen with style/attribute handling

    //     inc l0039
    rs->char_width++;
    //     stx l0084
    //     ldx input_buffer_offset+1

    uint8_t x = rs->buf_off;
    //     inc input_buffer_offset+1
    rs->buf_off++;
    //     cpx hscroll_pos
    //     bcc ca533
    if (x < hscroll_pos)
        return;
    //     jmp ca4e9
    render_char(rs);
}

static void restore_cursor_position(addr_t tmp45)
{
    // restore_cursor_position:
    //     ldx ((uint8_t*)&tmp45)[0]
    //     ldy ((uint8_t*)&tmp45)[1]
    screen_setcursor(((uint8_t*)&tmp45)[0], ((uint8_t*)&tmp45)[1]);
}

area_status_t sanitise_area(void)
{

    // sanitise_area
    // sanitise_area:
    //     lda area_start_ptr
    //     ldx area_start_ptr+1
    //     cpx area_end_ptr+1
    //     bcc c8977
    //     bne c896b
    //     cmp area_end_ptr
    //     bcc c8977
    // (16-bit comparison consolidated: swap if area_start_ptr >= area_end_ptr)
    if (area_start_ptr >= area_end_ptr)
    {
        // c896b:
        //     ldy area_end_ptr
        //     sty area_start_ptr
        //     ldy area_end_ptr+1
        //     sty area_start_ptr+1
        //     stx area_end_ptr+1
        //     sta area_end_ptr
        addr_t tmp = area_start_ptr;
        area_start_ptr = area_end_ptr;
        area_end_ptr = tmp;
    }
    // c8977:
    //     lda area_end_ptr
    //     sec
    //     sbc area_start_ptr
    //     sta ((uint8_t*)&tmp67)[0]
    //     lda area_end_ptr+1
    //     sbc area_start_ptr+1
    //     sta ((uint8_t*)&tmp67)[1]

    addr_t tmp67 = area_end_ptr - area_start_ptr;
    //     bne return_10
    if (tmp67 != 0)
    {
        return AREA_NOT_EMPTY;
    }
    // return_10:
    //     rts
    return AREA_EMPTY;
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
    //     sta ((uint8_t*)&tmp45)[0]
    tmp45 = cursor_;
    //     rts
}

static void set_marker(uint8_t x)
{
    // set_marker:
    //     jsr set_marker_to_here
    set_marker_to_here(x);
    //     jmp ca035
    l0073 = 1;
    update_line_length();
    return;
}

void go_to_marker(uint8_t x);

// MULTIPLE ENTRY POINTS: sf6_go_to_marker_key, go_to_marker, go_to_marker_1..6

static void set_marker_common(uint8_t a)
{
    //     pha
    //     jsr write_line_back_to_document_safely
    write_line_back_to_document_safely();
    //     pla
    //     jsr lookup_marker
    int idx = lookup_marker(a);
    //     jmp set_marker
    set_marker(idx);
    return;
}

void show_memory_full_error(void)
{
    uint8_t a_5;

    // show_memory_full_error
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

    uint8_t a = STYLE_REVERSE;
    screen_setstyle(a);
    //     ldy screen_width

    uint8_t y = screen_maxcolumn;
    //     sty line_lengths
    line_lengths[0] = y;
    //     dey
    y--;
    //     dey
    y--;
    //     ldx #0
    //     beq ca965
    // loop_ca962:
    //     jsr screen_putchar
    // ca965:
    //     lda la995,x
    //     beq ca96e
    //     inx
    //     dey
    //     bne loop_ca962
    // (loop restructured)

    uint8_t x = 0;
    for (;;)
    {
        uint8_t a_1 = la995_data[x];
        if (a_1 == 0)
            break;
        x++;
        y--;
        if (y == 0)
            break;
        screen_putchar(a_1);
    }
    // ca96e:
    //     jsr set_normal_text_if_not_mode_7
    screen_setstyle(0);
    //     tya
    uint8_t a_2 = y;
    //     beq ca97c
    if (a_2 == 0)
        goto ca97c;
    //     lda #0x20 ; ' '
    uint8_t a_3 = 0x20;
    // loop_ca976:
    do
    {
        screen_putchar(a_3);
        y--;
    } while (y != 0);
    // ca97c:
ca97c:
    //     lda #0
    uint8_t a_4 = 0;
    //     sta l006e
    edit_buffer_unpacked_flag = a_4;
    //     jsr clear_cmd
    clear_cmd();
    // loop_ca983:
    do
    {
        beep();
        a_5 = screen_getchar();
    } while (a_5 != 0x1b);
    //     jsr cursor_on
    cursor_on();
    //     lda #1
    //     sta status_line_needs_redrawing_flag
    status_line_needs_redrawing_flag = 1;
    //     sta l0073
    l0073 = 1;
    //     rts
}

void adjust_area_pointers(addr_t tmp67)
{

    // sub_c89d3:
    //     lda area_start_ptr
    //     sta ((uint8_t*)&tmp45)[0]
    //     lda area_start_ptr+1
    //     sta ((uint8_t*)&tmp45)[1]

    addr_t tmp45 = area_start_ptr;
    //     jsr adjust_pointers
    tmp89 = adjust_pointers(tmp45, tmp67);
    //     lda ((uint8_t*)&tmp45)[0]
    //     ldy ((uint8_t*)&tmp45)[1]
    //     jmp cac78
    split_line_at_wrap(tmp45);
    return;
}

static void append_to_output_buffer(uint8_t a)
{
    // sub_c8c53:
    //     ldx l0048

    uint8_t x = l0048;
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

uint8_t upper_case_unless_folding(uint8_t a)
{
    // sub_c8c5f: converts to uppercase only if folding flag is clear
    //     bit folding_flag
    if (folding_flag & 0x80)
        return a;
    //     falls through to to_uppercase
    return toupper(a);
}

// Returns true if the resulting width accumulator is zero (the 6502's Z flag).
static bool process_char_for_output(
    uint8_t y, bool carry_in, uint8_t* x, uint8_t* a)
{
    // sub_c9936
    // Pseudocode: Processes a character from the edit line for output, handling
    // tabs and margins

    // sub_c9936:
    //     ror l0083
    l0083 = (l0083 >> 1) | (carry_in ? 0x80 : 0);
    //     lda (current_edit_line_ptr),y
    (*a) = ram[RAM_EDIT_BUFFER + y];
    //     sta output_buffer,y
    output_buffer[y] = (*a);
    //     cmp #9
    if ((*a) != 9)
        goto c994a;
    //     bne c994a
    //     jsr sub_ca5ae
    {
        bool is_tab = false;
        (*a) = process_document_character((*a), x, &is_tab);
    }
    //     txa
    (*a) = *x;
    //     clc
    //     adc l0039
    //     bne c995c
    // (adc with C=0 is a plain addition; Z means the result is zero)
    (*a) += l0039;
    if ((*a) != 0)
        goto c995c;
    // c994a:
c994a:
    //     cmp #0x0b
    if ((*a) != 0x0b)
        goto c9969;
    //     bne c9969
    //     lda ruler_left_stop
    (*a) = ruler_left_stop;

    //     beq c9967
    if ((*a) == 0)
        goto c9967;
    //     ldx l0039
    *x = l0039;

    //     beq c995c
    if (*x != 0)
    {
        if (*x >= ruler_left_stop)
        {
            (*x)++;
            (*a) = *x;
        }
    }
c995c:
    //     sta l0039
    l0039 = (*a);
    //     sty l0042
    l0042 = y;
    //     inc l0042
    l0042++;
    //     lda #0
    (*a) = 0;
    //     sta l0046
    l0046 = (*a);
    //     rts
    return true;

    // c9967:
c9967:
    //     lda #0x20 ; ' '
    (*a) = 0x20;
    // c9969:
c9969:
    //     cmp #0x1b
    if ((*a) < 0x1b)
        goto c9967;
    //     bcc c9967
    //     cmp #0x20 ; ' '
    //     bcc return_49
    if ((*a) < 0x20)
        return false;
    //     inc l0039
    l0039++;
    // return_49:
    //     rts
    return l0039 == 0;
}

/**
 * Format the line at current_line_ptr (6502 sub_c9977).
 *
 * Reads the source line, handles margins, tabs and word wrapping.  Called
 * from f0_format_block_key (Ctrl+B) and fold_cmd.  Processes one line, or
 * skips command/ruler lines.
 *
 * On return the caller tests line_format_status == 0 directly (the 6502 set
 * Z from l007e at c9aa5).  The 6502's outgoing C (end of document, from
 * c9a8d) is returned as FORMAT_AT_END.
 *
 * @return FORMAT_MEMORY_FULL if the document write failed (6502 V=1),
 *         FORMAT_AT_END if the final advance_to_next_line reached the end of
 *         the document (6502 C=1), otherwise FORMAT_OK.
 */
format_result_t format_paragraph(void)
{
    uint8_t a_15;
    uint8_t x;

    // sub_c9977
    // PROVISIONAL: Main line formatting routine — reads source line, handles
    // margins, tabs, wrapping. PROVISIONAL: Called from f0_format_block_key
    // (Ctrl+B) and fold_cmd. PROVISIONAL: Processes one line (or skips
    // command/ruler lines).  On return the caller tests line_format_status
    // == 0 directly (the 6502 set Z from l007e at c9aa5).

    // sub_c9977:
    // PROVISIONAL: Mark cursor moved, init print_xpos=4, zero
    // input_buffer_offset and line_format_status.
    //     inc cursor_moved_flag
    cursor_moved_flag++;
    //     ldy #4
    //     sty print_xpos
    print_xpos = 4;
    //     ldy #0
    // (Z from ldy #0 is clobbered by the following lda (current_line_ptr),y)

    uint8_t y = 0;
    //     sty input_buffer_offset
    input_buffer_offset = y;
    //     sty l007e
    line_format_status = y;
    // PROVISIONAL: Check if first byte of current line is a command prefix
    // (0x80/0x81). PROVISIONAL: If so, skip this line and return (paragraph
    // boundary reached).
    //      lda (current_line_ptr),y

    uint8_t a = ram[current_line_ptr + y];
    //     jsr check_for_command_prefix
    command_prefix_t cp = check_for_command_prefix(a);
    //     beq c9974
    if (cp != NO_COMMAND_PREFIX)
    {
        return advance_to_next_doc_line() ? FORMAT_AT_END : FORMAT_OK;
    }
    // PROVISIONAL: Main formatting loop entry. Check format mode — if bit 7 or
    // bit 0 is set, PROVISIONAL: skip this line (paragraph boundary). Also skip
    // if ruler_right_stop == 0 PROVISIONAL: or if right_stop <= left_stop.
    //  c998a:
c998a:
    //     lda format_mode_flag
    uint8_t a_1 = format_mode_flag;
    //     and #0x81
    a_1 &= 0x81;

    //     bne c9974
    if (a_1 != 0)
    {
        return advance_to_next_doc_line() ? FORMAT_AT_END : FORMAT_OK;
    }
    //     lda ruler_right_stop
    uint8_t a_2 = ruler_right_stop;

    //     beq c9974
    if (a_2 == 0)
    {
        return advance_to_next_doc_line() ? FORMAT_AT_END : FORMAT_OK;
    }
    //     sec
    //     sbc ruler_left_stop
    //     bcc c9974
    // (sbc with C=1 in is a plain subtraction; if it borrows, bail out.
    //  Otherwise the carry into the following adc is 1: adc #1 adds 2)
    if (a_2 < ruler_left_stop)
    {
        return advance_to_next_doc_line() ? FORMAT_AT_END : FORMAT_OK;
    }
    // PROVISIONAL: Compute line width = right_stop - left_stop + 1, store in
    // l0080.
    //      adc #1
    // (carry is 1, so this is a + 2)
    uint8_t a_3 = a_2 - ruler_left_stop + 2;
    //     sta input_buffer_offset+1
    l0080 = a_3;
    // PROVISIONAL: Wipe the edit buffer with 0x10 (soft spaces) and set up
    // ((uint8_t*)&tmp67)[0]/((uint8_t*)&tmp67)[1] = current_line_ptr.
    //      lda #0x10
    //     jsr wipe_buffer
    wipe_buffer(0x10, ptr1);
    //     lda current_line_ptr
    //     sta ((uint8_t*)&tmp67)[0]
    //     lda current_line_ptr+1
    //     sta ((uint8_t*)&tmp67)[1]

    addr_t tmp67 = current_line_ptr;
    // PROVISIONAL: Zero working variables: l0047 (character index), l0039
    // (column counter), PROVISIONAL: l0038 (soft-hyphen/break flag), l0046
    // (word-start flag), bottom_margin.
    //      ldy #0
    uint8_t y_1 = 0;
    //     sty l0047
    l0047 = y_1;
    //     sty l0039
    l0039 = y_1;
    //     sty l0038
    l0038 = y_1;
    //     sty l0046
    l0046 = y_1;
    //     sty bottom_margin
    bottom_margin = y_1;
    // PROVISIONAL: Save current buffer index in l0048. Then scan forward
    // through the source line, PROVISIONAL: processing any marker-prefix bytes
    // detected by sub_ca536. Increments line_format_status for PROVISIONAL:
    // each marker processed (line_format_status counts marker bytes).
    //  c99b6:
c99b6:
    //     sty l0048
    l0048 = y_1;
    //     ldy l0047
    uint8_t y_2 = l0047;
    // loop_c99ba:
    for (;;)
    {
        //     jsr sub_ca536
        //     bne c99c7
        uint8_t idx = find_marker_at_position(y_2, tmp67);
        if (idx == 0x0c)
            break;
        //     lda #0
        //     sta markers_array+1,x
        markers_array[idx / 2] &= 0x00ff;
        //     inc l007e
        line_format_status++;
        //     bne loop_c99ba
        if (line_format_status != 0)
            continue;
        break;
    }
    // c99c7:
    // PROVISIONAL: Character processing loop. Reads one byte from the source
    // document line. PROVISIONAL: l0047 tracks the read position, l0048 tracks
    // the write position. PROVISIONAL: Handles tabs (0x09), soft hyphens /
    // break markers (0x1a), soft spaces (0x0b), CR (0x0d), PROVISIONAL: and
    // ordinary characters.
    //  c99c9:
c99c9:
    //     lda (current_line_ptr),y
    uint8_t a_5 = ram[current_line_ptr + y_2];
    //     iny
    y_2++;
    //     sty l0047
    l0047 = y_2;
    // PROVISIONAL: Tab (0x09): process via process_document_character to
    // compute tab width, PROVISIONAL: subtract 1 (x--) and add to column
    // counter l0039.
    //      cmp #9
    if (a_5 != 9)
        goto c99e0;
    //     bne c99e0
    //     jsr sub_ca5ae
    {
        bool is_tab = false;
        (void)process_document_character(a_5, &x, &is_tab);
    }
    //     dex
    x--;
    //     txa
    uint8_t a_7 = x;
    //     clc
    //     adc l0039
    a_7 += l0039;
    //     sta l0039
    l0039 = a_7;
    //     lda #9
    a_5 = 9;
    //     bne c9a21                                                         ;
    //     ALWAYS branch
    goto c9a21;

    // c99e0:
c99e0:
    //     cmp #0x1a
    if (a_5 != 0x1a)
        goto c99ee;
    //     bne c99ee
    // PROVISIONAL: Soft hyphen / break marker (0x1a): if l0046 (word-start
    // flag) is non-zero, PROVISIONAL: skip the marker and continue reading.
    // Otherwise, treat it as a word-break: PROVISIONAL: set l0046 (word-break
    // state), output a space, and go to c9a2e.
    //  c99e4:
c99e4:
    //     lda l0046
    uint8_t a_8 = l0046;
    if (a_8 != 0)
        goto c99c9;
    //     bne c99c9
    //     ldx #0xff
    //     lda #0x20 ; ' '
    a_5 = 0x20;
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
    if (a_5 != 0x0b)
        goto c9a11;
    //     bne c9a11
    //     ldx input_buffer_offset
    x = input_buffer_offset;
    if (x != 0)
        goto c99e4;
    //     bne c99e4
    //     lda l0038
    uint8_t a_9 = l0038;
    if (a_9 != 0)
        goto c99e4;
    //     bne c99e4
    //     inc l0038
    l0038++;
    //     lda ruler_left_stop

    uint8_t a_10 = ruler_left_stop;

    //     beq c99c9
    if (a_10 == 0)
        goto c99c9;
    //     ldx l0039
    uint8_t x_1 = l0039;
    //     cpx ruler_left_stop
    if (x_1 < ruler_left_stop)
    {
        l0039 = a_10;
        l0039--;
    }
    //     clc
    //     adc input_buffer_offset+1
    a_10 += l0080;
    //     sta input_buffer_offset+1
    l0080 = a_10;
    //     lda #0x0b
    a_5 = 0x0b;
    // c9a11:
    // PROVISIONAL: CR (0x0d) — end of source line. If y == 0 after decrement,
    // the buffer is PROVISIONAL: empty; advance to next document line.
    // Otherwise, call sub_c9ac1 to find the PROVISIONAL: word-wrap boundary. If
    // C set (wrap needed), go to c9a87 to flush the current PROVISIONAL: line
    // and advance. Otherwise, insert a space at the break.
c9a11:
    //     cmp #0x0d
    if (a_5 != 0x0d)
        goto c9a21;
    //     bne c9a21
    //     dey
    y_2--;

    //     beq c9a8d
    if (y_2 == 0)
    {
        return advance_to_next_doc_line() ? FORMAT_AT_END : FORMAT_OK;
    }
    //     jsr sub_c9ac1
    if (find_next_word_boundary(y_2))
        goto c9a87; // bcs c9a87
    //     lda #0x20 ; ' '
    a_5 = 0x20;
    //     sta input_buffer_offset
    input_buffer_offset = a_5;
    // c9a21:
    // PROVISIONAL: Track word-start state. If the current character is a space
    // (0x20), set x=1 PROVISIONAL: (word-start flag). If l0046 already
    // indicates word-start (N bit set), skip PROVISIONAL: straight to the write
    // (c9a40) — this is a multi-space gap, keep only the first.
c9a21:
    //     ldy l0048
    y_1 = l0048;
    //     ldx #0
    x = 0;
    //     cmp #0x20 ; ' '
    if (a_5 != 0x20)
        goto c9a2e;
    //     bne c9a2e
    //     inx                                                               ;
    //     X=0x01
    x++;
    //     bit l0046
    if ((l0046 & 0x80))
        goto c9a40;
    // PROVISIONAL: Write character to edit buffer at write position (l0048). If
    // it's a space, PROVISIONAL: rotate bottom_margin (tracks word-boundary
    // state for justification).
    //  c9a2e:
c9a2e:
    //     ldy l0048
    y_1 = l0048;
    //     sta (current_edit_line_ptr),y
    ram[RAM_EDIT_BUFFER + y_1] = a_5;
    //     cmp #0x20 ; ' '
    //     bne c9a38
    if (a_5 == 0x20)
    {
        //     ror bottom_margin
        // (carry-in is 1 from the cmp #0x20 with a == 0x20; the result
        //  flags are dead)
        bottom_margin = (uint8_t)(bottom_margin >> 1) | 0x80;
    }
    //     iny
    y_1++;
    //     jsr check_for_control_code
    control_code_t cc = check_for_control_code(a_5);
    //     beq c9a40
    if (cc == NO_CONTROL_CODE)
    {
        l0039++;
    }
c9a40:
    //     bit l0046
    //     stx l0046
    // (bit test: N reflects the old l0046 before it is overwritten)
    bool old_l0046_high = (l0046 & 0x80) != 0;
    l0046 = x;
    //     bmi c9a58
    if (old_l0046_high)
        goto c9a58;
    //     cmp #0x20 ; ' '
    if (a_5 == 0x20)
        goto c9a58;
    //     beq c9a58
    //     cpy #0x85
    if (y_1 >= MAX_LINE_LENGTH + 1)
        goto c9a60;
    //     lda bottom_margin

    uint8_t a_11 = bottom_margin;

    //     beq c9a58
    if (a_11 == 0)
        goto c9a58;
    //     lda l0039

    uint8_t a_12 = l0039;
    //     cmp input_buffer_offset+1
    if (a_12 >= l0080)
        goto c9a60;
    //     bcs c9a60
    // PROVISIONAL: Clamp buffer index to max 0x85 (133). Loop back to process
    // next character.
    //  c9a58:
c9a58:
    //     cpy #0x86
    if (y_1 >= 0x86)
    {
        y_1--;
    }
    //     jmp c99b6
    goto c99b6;

    // c9a60:
    // PROVISIONAL: Line-width exceeded — flush the current formatted line.
    // Increment the source PROVISIONAL: index (l0047), then scan backward
    // through the edit buffer replacing spaces (0x20) PROVISIONAL: with 0x10
    // (justification markers). This marks word boundaries for
    // justify_edit_buffer. PROVISIONAL: On finding a space, call sub_caed6,
    // justify_edit_buffer, then flush_formatted_line to write the line.
    // Advance to the next document line; if non-empty, loop back to c998a.
c9a60:
    //     inc l0047
    l0047++;
    // loop_c9a62:
    do
    {
        l0047--;
        y_1--;

        if (y_1 == 0)
        {
            return advance_to_next_doc_line() ? FORMAT_AT_END : FORMAT_OK;
        }

        uint8_t a_13 = ram[RAM_EDIT_BUFFER + y_1];
        {
            uint8_t saved_a = a_13;

            uint8_t a_14 = 0x10;
            ram[RAM_EDIT_BUFFER + y_1] = a_14;
            a_15 = saved_a;
        }
    } while (a_15 != 0x20);
    //     sec
    //     ror input_buffer_offset
    // (dead: the rotated value is never read again, and the ror's Z/N flags
    //  are dead: the `beq c9aa5` that followed now tests
    //  line_format_status == 0 directly)
    //     jsr sub_caed6
    insert_at_left_margin();
    //     jsr justify_edit_buffer
    justify_edit_buffer(ptr1);
    //     jsr sub_c9aa9
    if (flush_formatted_line())
        return FORMAT_MEMORY_FULL;
    //     jsr c9a8d
    _Bool at_end_1;
    at_end_1 = advance_to_next_doc_line();
    //     beq c9aa5
    if (line_format_status == 0)
        goto c9aa5;
    //     jmp c998a
    goto c998a;

    // c9a87:
    // PROVISIONAL: Word-wrap path — line needs wrapping at a word boundary.
    // Flush the current PROVISIONAL: buffer via sub_caed6 +
    // flush_formatted_line, advance to the next document line, PROVISIONAL:
    // then fall through to c9a8d (returns to sub_c9977's caller).
c9a87:
    //     jsr sub_caed6
    insert_at_left_margin();
    //     jsr sub_c9aa9
    if (flush_formatted_line())
        return FORMAT_MEMORY_FULL;
    //     (fall through to c9a8d in 6502 — no jsr)
    at_end_1 = advance_to_next_doc_line();
    //     (c9a8d/c9aa5 merged into advance_to_next_line; return directly to
    //     caller)
    goto c9aa5;
    // c9aa5:
    // PROVISIONAL: Cleanup — the 6502 cleared V and set Z from l007e here;
    // callers now test line_format_status == 0 directly.
c9aa5:
    //     clv
    //     lda l007e
    // (the 6502 sets Z from line_format_status here for its caller; callers
    //  now test line_format_status == 0 directly)
    //     rts
    return at_end_1 ? FORMAT_AT_END : FORMAT_OK;
}

static bool find_next_word_boundary(uint8_t y)
{

    // sub_c9ac1
    // Pseudocode: Finds next word boundary for line wrapping, returns carry if
    // found

    // sub_c9ac1:
    //     tya

    uint8_t a = y;
    //     sec
    //     adc current_line_ptr
    //     sta ((uint8_t*)&tmp89)[0]
    //     sta ((uint8_t*)&tmp45)[0]
    //     lda current_line_ptr+1
    //     adc #0
    //     sta ((uint8_t*)&tmp89)[1]
    //     sta ((uint8_t*)&tmp45)[1]
    // (16-bit arithmetic: the sec before the adc adds 1, so
    //  tmp89 = tmp45 = current_line_ptr + y + 1)

    addr_t tmp89 = current_line_ptr + a + 1;

    addr_t tmp45 = tmp89;
    //     ldy #0
    y = 0;
    //     sty l0083
    l0083 = y;
    // c9ad5:
c9ad5:
    //     lda (((uint8_t*)&tmp45)[0]),y
    uint8_t a_1 = ram[tmp45 + y];
    //     beq c9b2f
    if (a_1 == 0)
        goto c9b2f;
    //     jsr check_for_command_prefix
    command_prefix_t cp = check_for_command_prefix(a_1);
    //     beq c9b2f
    if (cp != NO_COMMAND_PREFIX)
        goto c9b2f;
    //     cmp #0x0d
    if (a_1 == 0x0d)
        goto c9b2f;
    //     beq c9b2f
    //     tya
    uint8_t a_2 = y;
    //     bne c9b06
    if (a_2 != 0)
        goto c9b06;
    //     sty l0084
    l0084 = y;
    //     beq c9aef                                                         ;
    //     ALWAYS branch
    goto c9aef;

    // c9ae9:
c9ae9:
    //     inc ((uint8_t*)&tmp89)[0]
    tmp89++;
    //     bne c9aef
    //     inc ((uint8_t*)&tmp89)[1]
    // c9aef:
c9aef:
    //     lda (((uint8_t*)&tmp89)[0]),y
    uint8_t a_3 = ram[tmp89 + y];
    //     beq c9b06
    if (a_3 == 0)
        goto c9b06;
    //     cmp #0x0d
    if (a_3 == 0x0d)
        goto c9b06;
    //     beq c9b06
    //     cmp #9
    if (a_3 == 9)
        goto c9b2f;
    //     beq c9b2f
    //     cmp #0x0b
    //     bne c9ae9
    if (a_3 == 0x0b)
    {
        //     rol l0084
        //     sec
        //     ror l0084
        // (The 6502 sets bit 7 by rolling left, setting C, and rolling right
        //  again; the carry is preserved from the preceding cmp.)
        l0084 |= 0x80;
        //     bcs c9ae9
        // (a >= 0x0b holds on this path, so the bcs is always taken)
        if (a_3 >= 0x0b)
            goto c9ae9;
    }
    // c9b06:
c9b06:
    //     lda (((uint8_t*)&tmp45)[0]),y
    uint8_t a_4 = ram[tmp45 + y];
    //     cmp #0x20 ; ' '
    if (a_4 != 0x20)
        goto c9b1a;
    //     bne c9b1a
    //     ldx ruler_left_stop

    uint8_t x = ruler_left_stop;
    //     beq c9b2f
    if (x == 0)
        goto c9b2f;
    //     ldx l0084
    uint8_t x_1 = l0084;
    //     beq c9b2f
    if (x_1 == 0)
        goto c9b2f;
    //     ldx l0083
    uint8_t x_2 = l0083;
    //     bne c9b2f
    if (x_2 != 0)
        goto c9b2f;
    //     beq c9b20                                                         ;
    //     ALWAYS branch
    goto c9b20;

    // c9b1a:
c9b1a:
    //     cmp #0x0b
    //     bne c9b23
    if (a_4 != 0x0b)
        goto c9b23;
    //     sta l0083
    l0083 = a_4;
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
    uint8_t a_5 = ruler_left_stop;
    //     beq c9b31
    if (a_5 == 0)
        goto c9b31;
    //     lda l0084
    uint8_t a_6 = l0084;
    //     beq c9b31
    if (a_6 == 0)
        goto c9b31;
    //     lda l0083
    uint8_t a_7 = l0083;
    //     bne c9b31
    if (a_7 != 0)
        goto c9b31;
    // c9b2f:
c9b2f:
    //     sec
    //     rts
    return true;

    // c9b31:
c9b31:
    //     clc
    //     rts
    return false;
}

// Returns false if the insert failed (out of memory).
static bool insert_character_into_edit_buffer(uint8_t a)
{
    _Bool ok;

    // sub_c9e22:
    //     pha
    {
        uint8_t saved_a = a;
        //     ldx #1
        //     jsr insert_edit_buffer_bytes_at_xpos
        ok = insert_edit_buffer_bytes_at_xpos(1);
        //     pla
        a = saved_a;
    }
    //     bcs return_55
    if (!ok)
        return false;
    //     sta (current_edit_line_ptr),y
    // (the 6502 left y = the insert position, i.e. xpos)
    ram[RAM_EDIT_BUFFER + xpos] = a;
    //     inc l0074
    l0074++;
    // return_55:
    //     rts
    return true;
}

// MULTIPLE ENTRY POINTS: tab_key, sf4_highlight1_key, sf5_highlight2_key

static void set_xpos_to_line_length(void)
{
    // Shared code: gets line length and sets xpos
    // c9e9b:
    //     jsr get_line_length
    xpos = get_line_length();
    //     rts
}

static uint8_t compute_display_start_line(void)
{

    // sub_ca44e
    // sub_ca44e: Computes starting line for display based on screen position

    //     lda l0034

    uint8_t a = l0034;
    //     sta ruler_stack_ptr
    ruler_index_ptr = a;
    //     lda screen_height
    uint8_t a_1 = screen_maxrow;
    //     sta l0073
    l0073 = a_1;
    //     lsr
    a_1 >>= 1;
    //     tax

    uint8_t x = a_1;
    //     inx
    x++;
    //     lda l006f
    uint8_t a_2 = l006f;
    //     bmi ca461
    if (!(a_2 & 0x80))
    {
        if (a_2 != 0)
        {
            x = ypos;
        }
    }
    //     lda current_line_ptr / ldy current_line_ptr+1
    // (the 6502 carried the candidate line in the YA register pair.  Here
    //  `line` holds it: after a successful find_previous_line it is advanced
    //  to the previous line (left in tmp01), and on failure/early-exit it is
    //  the starting line.  In all cases the top-of-screen line is the final
    //  value of `line` — i.e. tmp01 after a success, or the starting line on
    //  failure/early-exit.)
    addr_t line = current_line_ptr;
    // loop_ca465:
    for (;;)
    {
        //     dex
        x--;
        //     beq ca479
        if (x == 0)
            break;
        //     sta tmp2 / sty tmp3 / jsr sub_cab37 (find_previous_line)

        addr_t tmp01;
        if (!find_previous_line(line, &tmp01))
            break;
        //     lda tmp0 / ldy tmp1 / bcs loop_ca465
        line = tmp01;
        continue;
    }
    // ca479:
    //     sta top_of_screen_line_ptr / sty top_of_screen_line_ptr+1
    top_of_screen_line_ptr = line;
    //     lda ruler_stack_ptr
    uint8_t a_3 = ruler_index_ptr;
    //     sta l0033
    l0033 = a_3;
    //     lda l0034
    uint8_t a_4 = l0034;
    //     sta ruler_stack_ptr
    ruler_index_ptr = a_4;
    return x;
    //     rts
}

static void advance_to_next_char(struct render_state* rs)
{

    // Advance to the next document character.  The shared
    // process_current_document_character() reads/writes the simulated
    // registers, so synchronise the render state around the call.
    //     jsr process_current_document_character

    uint8_t y = rs->pos;
    l0039 = rs->char_width;

    uint8_t a;
    uint8_t x;
    a = process_current_document_character(
        rs->line_ptr, &x, &y, &rs->prev_is_tab);
    rs->ch = a;
    rs->pos = y;
    rs->width = x;
    rs->char_width = l0039;
}

static void advance_to_next_char_and_render(struct render_state* rs)
{
    // advance_to_next_char_and_render: Draws a character and advances x
    // position

    //     jsr process_current_document_character
    advance_to_next_char(rs);
    //     jmp ca4e9
    render_char(rs);
}

static uint8_t find_marker_at_position(uint8_t y, addr_t tmp67)
{

    // sub_ca536
    // Pseudocode: Checks if a position in the edit line corresponds to a marker
    // On exit:  return value is the marker array index (0,2,...,10) if the
    // position matches a marker, or 0x0c if it does not

    // sub_ca536:

    addr_t tmp89 = tmp67 + y;
    //     ldx #0

    uint8_t x = 0;
    // loop_ca544:
    for (;;)
    {
        //     lda ((uint8_t*)&tmp89)[1]
        //     cmp markers_array+1,x
        //     bne ca550
        //     lda ((uint8_t*)&tmp89)[0]
        //     cmp markers_array,x
        //     beq ca558
        // ca550:
        if (tmp89 != markers_array[x / 2])
            goto ca550;
        goto ca558;
        // ca550:
    ca550:
        //     inx
        x++;
        //     inx
        x++;
        //     cpx #0x0c
        if (x != 0x0c)
            continue;
        break;
    }
    //     txa
    //     rts
    return 0x0c;

    // ca558:
ca558:
    //     lda #0
    //     rts
    return x;
}

static void unpack_line(addr_t ptr1)
{

    // unpack_line
    // unpack_line:
    //     lda #0x10
    //     jsr wipe_buffer
    wipe_buffer(0x10, ptr1);
    //     jsr sub_caf5f
    clear_format_mode_bit7();
    //     ldy #0

    //     lda (current_line_ptr),y

    uint8_t a = ram[current_line_ptr];
    //     ldx current_edit_line_ptr
    //     ldy current_edit_line_ptr+1
    //     jsr check_for_command_prefix
    command_prefix_t cp = check_for_command_prefix(a);
    //     bne caab7
    if (cp != NO_COMMAND_PREFIX)
    {
        if (cp == RULER_PREFIX)
            edit_buffer_unpacked_flag = a;
        set_format_mode_bit7();
    }
    //     stx current_format_line_ptr
    //     sty current_format_line_ptr+1
    current_format_line_ptr =
        (cp != NO_COMMAND_PREFIX) ? ptr1 : RAM_EDIT_BUFFER;
    //     ldy #0
    uint8_t y_1 = 0;
    // loop_caabd:
    for (;;)
    {
        //     lda (current_line_ptr),y

        uint8_t a2 = ram[current_line_ptr + y_1];
        //     cmp #0x0d
        if (a2 == 0x0d)
            break;
        //     beq caac8
        //     sta (current_format_line_ptr),y
        ram[current_format_line_ptr + y_1] = a2;
        //     iny
        y_1++;
        //     bne loop_caabd
        if (y_1 != 0)
            continue;
        break;
    }
    // caac8:
    //     sty l003b
    l003b = y_1;
    // return_68:
    //     rts
}

static void update_markers_to_format_buffer(void)
{

    // sub_caacb: Updates marker positions to point into format buffer instead
    // of document buffer

    //     lda current_line_ptr
    //     sta ((uint8_t*)&tmp67)[0]
    //     lda current_line_ptr+1
    //     sta ((uint8_t*)&tmp67)[1]

    addr_t tmp67 = current_line_ptr;
    //     ldy #0

    uint8_t y = 0;
    // caad5:
caad5:
    //     jsr sub_ca536
    //     bne caae8
    uint8_t idx = find_marker_at_position(y, tmp67);
    if (idx == 0x0c)
        goto caae8;
    //     tya
    //     clc
    //     adc current_format_line_ptr
    //     sta __begin_pointer_array,x
    //     lda current_format_line_ptr+1
    //     adc #0
    //     sta markers_array+1,x
    // (16-bit arithmetic: markers_array[idx] = current_format_line_ptr + y;
    //  the bne caad5 loops while the high byte of the result is non-zero)
    {
        uint16_t val = current_format_line_ptr + y;
        markers_array[idx / 2] = val;
        if (val != 0)
            goto caad5;
    }
    // caae8:
caae8:
    //     lda (current_line_ptr),y

    uint8_t a = ram[current_line_ptr + y];
    //     cmp #0x0d
    //     beq return_68
    if (a == 0x0d)
        return;
    //     iny
    y++;
    //     bne caad5
    if (y != 0)
        goto caad5;
    // return_68:
    //     rts
}

void check_for_embedded_ruler(addr_t tmp01)
{
    // Pseudocode: Pushes ruler stack before entering a new ruler region

    // sub_cac41:
    //     pha
    //     tya
    //     pha
    //     jsr sub_cab6e
    //     bne cac4c
    // (inlined: Z = (ram[tmp01] == RULER_BYTE))
    if (ram[tmp01] == RULER_BYTE)
        push_onto_ruler_index(tmp01);
    //     pla
    //     tay
    //     pla
    //     rts
    return;
}

static addr_t find_line_start(addr_t tmp89)
{

    // sub_cac50
    // Pseudocode: Finds the start of current line by scanning backward for CR

    //     ldy #0

    uint8_t y = 0;
    // cac5c:
    //     lda (((uint8_t*)&tmp89)[0]),y
    //     cmp #0x0d
    //     bne cac5c
    // (loop restructured)
    while (1)
    {
        tmp89--;
        uint8_t a = ram[tmp89 + y];
        if (a == 0x0d)
            break;
    }
    // cac6f:
    // return_73:
    //     rts
    return tmp89;
}

/**
 * find_left_margin_stop: Finds the left margin stop (0x0b) in the edit line.
 *
 * @return the position after the 0x0b in the edit line, or -1 if no margin
 *         stop was found.
 */
static int find_left_margin_stop(void)
{
    //     lda ruler_left_stop

    uint8_t y = 0;

    uint8_t a = ruler_left_stop;
    //     beq caed4
    if (a != 0)
    {
        //     ldy #0
        // loop_caec8:
        do
        {
            //     lda (current_edit_line_ptr),y
            uint8_t a_1 = ram[RAM_EDIT_BUFFER + y];
            //     iny
            y++;
            //     cmp #0x0b
            //     beq caed4
            if (a_1 == 0x0b)
                return y;
            //     cpy #0x84
            //     bcc loop_caec8
        } while (y < MAX_LINE_LENGTH);
        //     cpy #0x84 set carry when Y >= MAX_LINE_LENGTH
        return -1;

        // caed4:
    }
    //     clc
    return y;
}

static void insert_at_left_margin(void)
{
    // sub_caed6:
    //     jsr find_left_margin_stop
    //     bcc caed4
    if (find_left_margin_stop() < 0)
    {
        //     ldy #0
        insert_byte_at_xpos(0);
    }
    // caed4:
    //     rts
    return;
}

static bool insert_byte_at_xpos(uint8_t y)
{
    _Bool ok;

    // sub_caedd:
    //     lda xpos

    uint8_t a = xpos;
    //     pha
    //     sty xpos
    xpos = y;
    //     ldx #1
    //     jsr insert_edit_buffer_bytes_at_xpos
    ok = insert_edit_buffer_bytes_at_xpos(1);
    //     bcs caef0
    if (ok)
    {
        //     ldy xpos
        //     lda #0x0b
        //     sta (current_edit_line_ptr),y
        // (the 6502 left y = the insert position, i.e. xpos)
        ram[RAM_EDIT_BUFFER + xpos] = 0x0b;
        //     iny
    }
    // caef0:
    //     pla
    //     sta xpos
    xpos = a;
    //     rts
    return ok;
}

static void unpack_line_into_buffer(addr_t ptr1)
{
    // unpack_line_into_buffer:
    //     lda l006e
    //     bne return_68
    if (edit_buffer_unpacked_flag != 0)
        return;
    //     lda #1
    //     sta l006e
    edit_buffer_unpacked_flag = 1;
    unpack_line(ptr1);
}

void wipe_buffer(uint8_t a, addr_t ptr1)
{
    // wipe_buffer:
    //     ldy #0
    uint8_t y = 0;
    //     ldx #0x89
    uint8_t x = 0x89;
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

static bool write_line_back_to_document(void)
{
    uint8_t a_9;

    // write_line_back_to_document
    // sub_ca8b9:
    // write_line_back_to_document:
    //     lda l006e
    //     beq ca93a

    uint8_t a = edit_buffer_unpacked_flag;

    if (a == 0)
        goto ca93a;
    //     lda current_line_ptr
    //     sta ((uint8_t*)&tmp45)[0]

    addr_t tmp45 = current_line_ptr;
    //     ldy #0

    uint8_t y = 0;
    //     sty ((uint8_t*)&tmp67)[1]
    ((uint8_t*)&tmp67)[1] = y;
    //     jsr get_line_length
    l0083 = get_line_length();
    //     lda l003b
    uint8_t a_1 = l003b;
    //     sec
    //     sbc l0083
    //     bcc ca8df
    //     beq ca8ed
    // (sbc with C=1 is a plain subtraction; a holds a - l0083 on all
    //  paths, and the borrow/zero flags select the branch)
    {
        uint8_t minuend = a_1;
        a_1 -= l0083;
        if (minuend < l0083)
            goto ca8df; // borrow: a < l0083
        if (a_1 == 0)
            goto ca8ed;
    }
    //     sta ((uint8_t*)&tmp67)[0]
    ((uint8_t*)&tmp67)[0] = a_1;
    //     jsr adjust_pointers
    tmp89 = adjust_pointers(tmp45, tmp67);
    //     jmp ca8ed
    goto ca8ed;

    // ca8df:
ca8df:
    //     sta l0084
    l0084 = a_1;
    //     lda #0
    // (Z from this lda is clobbered by the following sbc l0084)
    uint8_t a_2 = 0;
    //     sec
    //     sbc l0084
    a_2 -= l0084;
    //     sta ((uint8_t*)&tmp67)[0]
    ((uint8_t*)&tmp67)[0] = a_2;
    //     jsr make_space_for_insertion
    if (!make_space_for_insertion(tmp45, tmp67))
        return true; // bcs return_66 — out of memory, write failed

    // ca8ed:
ca8ed:
    //     lda l006e
    uint8_t a_3 = edit_buffer_unpacked_flag;
    if (((int8_t)a_3 < 0))
    {
        uint8_t a_4 = edit_buffer_dirty_flag;

        if (a_4 != 0)
        {
            clamp_ptr6_to_document();
        }
    }
    //     ldy #0
    uint8_t y_1 = 0;
    //     sty l006d
    edit_buffer_dirty_flag = y_1;
    //     sty l006e
    edit_buffer_unpacked_flag = y_1;
    //     lda current_format_line_ptr
    //     sta ((uint8_t*)&tmp67)[0]
    //     lda current_format_line_ptr+1
    //     sta ((uint8_t*)&tmp67)[1]
    tmp67 = current_format_line_ptr;
    //     ldx l0083
    uint8_t x = l0083;
    //     stx l003b
    l003b = x;

    // ca90a:
ca90a:
    //     txa
    uint8_t a_5 = x;
    if (a_5 != 0)
        goto ca911;
    //     bne ca911
    //     lda #0x0d
    uint8_t a_6 = 0x0d;
    //     bne ca919
    goto ca919;

    // ca911:
ca911:
    //     lda (current_format_line_ptr),y
    a_6 = ram[current_format_line_ptr + y_1];
    //     cmp #0x10
    if (a_6 == 0x10)
    {
        a_6 = 0x20;
    }
ca919:
    //     pha
    {
        uint8_t a_8;

        uint8_t saved_a = a_6;
        //     txa
        uint8_t a_7 = x;
        //     pha
        {
            uint8_t saved_x = a_7;

            // loop_ca91c:
            // loop_ca91c:
            for (;;)
            {
                //     jsr sub_ca536
                //     bne ca92f
                uint8_t idx = find_marker_at_position(y_1, tmp67);
                if (idx == 0x0c)
                    break;
                //     tya
                //     clc
                //     adc current_line_ptr
                //     sta markers_array,x
                //     lda current_line_ptr+1
                //     adc #0
                //     sta markers_array+1,x
                // (16-bit arithmetic: val = current_line_ptr + y)
                uint16_t val = current_line_ptr + y_1;
                markers_array[idx / 2] = val;
                //     bne loop_ca91c
                if (val != 0)
                    continue;
                break;
            }
            // ca92f:
            //     pla
            a_8 = saved_x;
        }
        //     tax
        x = a_8;
        //     pla
        a_9 = saved_a;
    }
    //     sta (current_line_ptr),y
    ram[current_line_ptr + y_1] = a_9;
    //     iny
    y_1++;
    //     dex
    x--;
    //     cmp #0x0d
    if (a_9 != 0x0d)
        goto ca90a;

    // ca93a:
ca93a:
    //     clc
    // return_66:
    //     rts
    return false;
}

// MULTIPLE ENTRY POINTS: write_line_back_to_document_safely (via memory_full)

void write_line_back_to_document_safely(void)
{
    // write_line_back_to_document_safely: Write back edit buffer. If out of
    // memory, fall through to memory_full.
    //     jsr write_line_back_to_document
    //     bcc return_66
    if (!write_line_back_to_document())
        return;
    //     falls through to memory_full
    memory_full();
}
