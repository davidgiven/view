#include "editor.h"

#include "document.h"

#include "io.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "globals.h"

// Render pipeline state for line drawing.
struct render_state
{
    uint8_t* line_ptr;  // pointer into the current edit line
    uint8_t pos;        // position in the edit line
    uint8_t col;        // current screen column
    uint8_t line;       // screen line number
    uint8_t width;      // character width / render counter
    uint8_t buf_off;    // input buffer offset
    uint8_t char_width; // width accumulator
    uint8_t ch;         // current character
    bool prev_is_tab;   // whether the previous character was a tab expansion
};

// Editor-only functions
uint8_t* adjust_pointers(uint8_t* insert_ptr, ptrdiff_t size_delta);
static bool advance_to_next_doc_line(void);
void beep(void);
bool scan_document_for_next_line(void);
static void insert_line_into_document(uint8_t* target_ptr);
static void update_line_length(void);
void clamp_ptr6_to_document(void);
void clear_screen(void);
static void clear_to_eol(uint8_t a_val, uint8_t line);
static void cursor_off(void);
static void cursor_on(void);
void draw_line(struct render_state* rs, uint8_t* addr);
void draw_prompt_characters(uint8_t x_val, uint8_t y_val);
static void draw_ruler(void);
static void draw_status_word(void);
static uint8_t get_line_length(void);
static void go_to_marker(uint8_t x_val);
static void go_to_marker_n(uint8_t marker);
static void home_cursor(void);
uint8_t justify_edit_buffer(uint8_t* target_ptr);
bool make_space_for_insertion(uint8_t* insert_ptr, ptrdiff_t size_delta);
static void memory_full(void);
uint8_t process_current_document_character(
    uint8_t* target_ptr, uint8_t* x_val, uint8_t* y_val, bool* is_tab);
static void recalculate_cursor_xpos(void);
void redraw_editor(void);
static void render_char(struct render_state* rs);
static void advance_to_next_char(struct render_state* rs);
static void render_xchar(struct render_state* rs);
area_status_t sanitise_area(void);
static void set_marker(uint8_t x_val);
static void set_marker_common(uint8_t a_val);
void show_memory_full_error(void);
void adjust_area_pointers(ptrdiff_t size_delta);
static void append_to_output_buffer(uint8_t a_val);
uint8_t upper_case_unless_folding(uint8_t a_val);
static bool process_char_for_output(
    uint8_t y_val, bool carry_in, uint8_t* x_val, uint8_t* a_val);
format_result_t format_paragraph(void);
static bool find_next_word_boundary(uint8_t y_val);
static bool insert_character_into_edit_buffer(uint8_t a_val);
static void set_xpos_to_line_length(void);
static uint8_t compute_display_start_line(void);
static void advance_to_next_char_and_render(struct render_state* rs);
static uint8_t find_marker_at_position(uint8_t y_val, uint8_t* target_ptr);
static void unpack_line(uint8_t* target_ptr);
static void update_markers_to_format_buffer(void);
void check_for_embedded_ruler(uint8_t* target_ptr);
static uint8_t* find_line_start(uint8_t* target_ptr);
static int find_left_margin_stop(void);
static void insert_at_left_margin(void);
static bool insert_byte_at_xpos(uint8_t y_val);
static void unpack_line_into_buffer(uint8_t* target_ptr);
void wipe_buffer(uint8_t a_val, uint8_t* target_ptr);
static bool write_line_back_to_document(void);
void write_line_back_to_document_safely(void);

void enter_editor_mode(void);
void clear_format_mode_bit7(void);
void set_format_mode_bit7(void);
void draw_previous_word(
    uint8_t* word_boundary, bool* is_start_of_line, uint8_t* char_width);
bool adjust_margins_at_left_margin(void);
bool insert_edit_buffer_bytes_at_xpos(uint8_t x_val);
void set_marker_to_here(uint8_t x_val);
void split_line_at_wrap(uint8_t* target_ptr);

// Editor-internal helper functions

static void advance_current_line_pointer(void);

static void clear_marks_1_2(void);

static uint8_t control_key_to_ascii(uint8_t a_val);

static void delete_edit_buffer_bytes_at_xpos(uint8_t x_val);

static void enter_printable_character(void);

static int prompt_for_marker(void);

static bool reset_area_to_marks_1_2(void);

static void insert_line_at_cursor(uint8_t* target_ptr);

static void move_to_previous_line(void);

static void move_cursor_up(uint8_t x_val);

static void move_cursor_down(uint8_t x_val);

static void check_pointer_in_area(void);

static void tab_highlight_common(uint8_t a_val);

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

/**
 * Returns to the editor main loop.
 * Uses longjmp to re-enter the editor event loop.
 */
void return_to_editor_loop(void)
{
    longjmp(env, JMP_EDITOR);
}

/**
 * Enters editor mode.
 * Initialises editor state and jumps to the editor loop.
 */
void run_editor(void)
{
    enter_editor_mode();
    longjmp(env, JMP_EDITOR);
}

/**
 * Main editor event loop.
 * Handles cursor positioning, screen redraw, and key dispatch for the editor.
 */
void editor_loop_impl(void)
{
    uint8_t a_val3;
    screen_enter();
    for (;;)
    {
    editor_loop:
    {
        uint8_t saved_fmt = format_mode_flag;
        uint8_t a_val = edit_buffer_unpacked_flag;
        if (a_val == 0)
        {
            uint8_t a_val1;
            {
                unpack_line(edit_buffer_base);
                a_val1 = a_val;
            }
            edit_buffer_unpacked_flag = a_val1;
        }
        recalculate_cursor_xpos();
        if (!(ruler_left_stop == 0 || format_mode_flag & 0x80 ||
                ruler_left_stop <= visual_column))
        {
            if (!(cursor_moved_flag != 0))
            {
                uint8_t line_len = get_line_length();
                a_val3 = format_mode_flag;
                if (line_len >= xpos)
                    goto c9b84_;
                if (format_mode_flag & 0x40)
                    goto c9b6a_;
                xpos = line_len;
            }
            else
            c9b6a_:
            {
                visual_column = ruler_left_stop;
                line_change_pending_flag++;
                recalculate_cursor_xpos();
                uint8_t a_val5 = format_mode_flag;
                a_val5 &= 0xbf;
                {
                    int y_val = find_left_margin_stop();
                    a_val3 = a_val5;
                    if (y_val < 0 || y_val <= xpos)
                        goto c9b86_;
                }
                a_val3 |= 0x40;
            }
            c9b84_:
                format_mode_flag = a_val3;
        }
    c9b86_:
        if (saved_fmt != format_mode_flag)
            flags_need_redrawing_flag++;
        cursor_moved_flag = 0;
        redraw_editor();
    }
        uint8_t a_val6 = screen_getchar();
        if (a_val6 == current_tab_key)
            a_val6 = 9;
        editor_current_key = a_val6;
        if (!(a_val6 < 0x20))
        {
            if (a_val6 < 0x7f)
            {
                enter_printable_character();
                goto editor_loop;
            }
        }
        switch (a_val6)
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
        goto editor_loop;
    }
}

/**
 * Deletes the block defined by markers 1 and 2.
 * Resets the area to markers 1 and 2, moves the cursor, and deletes the block.
 */
static void cf0_delete_block_key(void)
{
    write_line_back_to_document_safely();
    cursor_moved_flag++;
    if (reset_area_to_marks_1_2())
    {
        beep();
        return;
    }
    move_cursor_to_address(area_start_ptr);
    clamp_ptr6_to_document();
    adjust_area_pointers(area_size);
    ensure_cr_at_document_top();
    clear_marks_1_2();
}

/**
 * Moves to the next search match.
 * Scans the document for the next occurrence of the search target.
 */
static void cf1_next_match_key(void)
{
    write_line_back_to_document_safely();
    if (!scan_document_for_next_line())
    {
        esc_key();
        return;
    }
    move_cursor_to_address(doc_working_ptr);
}

/**
 * Toggles format mode.
 * Flips the format-mode flag between states and marks the status bar for
 * redraw.
 */
static void cf2_format_mode_key(void)
{
    uint8_t a_val = format_mode_flag;
    a_val &= 0xbf;
    if (format_mode_flag & 0x40)
        a_val |= 1;
    a_val ^= 1;
    format_mode_flag = a_val;
    flags_need_redrawing_flag++;
}

/**
 * Toggles justification mode.
 * Inverts the justification flag and marks the status bar for redraw.
 */
static void cf3_justify_mode_key(void)
{
    justifying_flag ^= 0xff;
    flags_need_redrawing_flag++;
}

/**
 * Toggles insert mode.
 * Inverts the insert-mode flag and marks the status bar for redraw.
 */
static void cf4_insert_mode_key(void)
{
    insert_mode_flag ^= 0xff;
    flags_need_redrawing_flag++;
}

/**
 * Inserts a line with the default ruler.
 * Creates a new line and installs the default ruler definition.
 */
static void cf5_default_ruler_key(void)
{
    f6_insert_line_key();
    redraw_editor();
    cf8_mark_as_ruler_key();
    create_default_ruler(&ram[RAM_EDIT_BUFFER]);
}

/**
 * Splits the current line at the cursor.
 * Writes the buffer back and inserts a new line at the cursor column, handling
 * command prefixes.
 */
static void cf6_split_line_key(void)
{
    write_line_back_to_document_safely();
    uint8_t line_len = get_line_length();
    uint8_t y_val = line_len;
    if (line_len >= xpos)
        y_val = xpos;
    line_change_pending_flag++;
    uint8_t x_val = y_val;
    uint8_t a_val1 = current_format_line_ptr[0];
    command_prefix_t cp = check_for_command_prefix(a_val1);
    if (cp != NO_COMMAND_PREFIX)
    {
        x_val++;
        x_val++;
        x_val++;
    }
    uint32_t sum = (uint32_t)(current_line_ptr - &ram[0]) + x_val;
    if (sum > 0xffff)
    {
        f6_insert_line_key();
        return;
    }
    insert_line_into_document(&ram[(uint16_t)sum]);
    return;
}

/**
 * Joins the current line with the next line.
 * Removes the line break between current and next line if neither is a command.
 */
static void cf7_join_lines_key(void)
{
    write_line_back_to_document_safely();
    uint8_t* line_ptr;
    uint8_t y_val;
    if (find_next_line(current_line_ptr, &line_ptr, &y_val))
    {
        beep();
        return;
    }
    command_prefix_t cp = check_for_command_prefix(line_ptr[y_val]);
    if (cp != NO_COMMAND_PREFIX)
    {
        beep();
        return;
    }
    y_val--;
    uint8_t* insert_ptr = current_line_ptr + y_val;
    scratch_scan_ptr = adjust_pointers(insert_ptr, 1);
    split_line_at_wrap(current_line_ptr);
    line_change_pending_flag++;
    clamp_ptr6_to_document();
    return;
}

/**
 * Marks the current line as a ruler.
 * Writes a ruler marker into the edit buffer and enables format mode.
 */
static void cf8_mark_as_ruler_key(void)
{
    current_format_line_ptr = edit_buffer_base;
    uint8_t y_val = 0;
    current_format_line_ptr[y_val] = 0x81;
    y_val++;
    current_format_line_ptr[y_val] = 0x2e;
    y_val++;
    current_format_line_ptr[y_val] = 0x2e;
    line_counter++;
    if (!(edit_buffer_unpacked_flag & 0x80))
    {
        edit_buffer_unpacked_flag = 0x80;
        edit_buffer_dirty_flag++;
    }
    set_format_mode_bit7();
    return;
}

/**
 * Handles the Delete key.
 * Deletes the character before the cursor and handles insert-mode and
 * auto-insert behavior.
 */
static void delete_key(void)
{
    uint8_t a_val2;
    if (visual_column == 0)
        return;
    xpos--;
    uint8_t a_val1 = ram[RAM_EDIT_BUFFER + xpos];
    {
        f9_delete_char_key();
        a_val2 = a_val1;
    }
    if (a_val2 < 0x0c)
        return;
    if (insert_mode_flag != 0)
        return;
    uint8_t line_len = get_line_length();
    if (line_len < xpos)
        return;
    if (line_len == xpos)
        return;
    f8_insert_char_key();
}

/**
 * Handles the Escape key.
 * Writes the edit buffer back and returns to the CLI.
 */
void esc_key(void)
{
    write_line_back_to_document_safely();
    run_cli();
}

/**
 * Formats the current block.
 * Clamps pointers and invokes the paragraph formatter; shows memory-full error
 * if needed.
 */
static void f0_format_block_key(void)
{
    write_line_back_to_document_safely();
    clamp_ptr6_to_document();
    if (format_paragraph() == FORMAT_MEMORY_FULL)
    {
        show_memory_full_error();
        longjmp(env, JMP_EDITOR);
    }
    if (line_format_status == 0)
    {
        ptr6_screen_row = ptr6_screen_row;
        display_start_row = display_start_row;
    }
}

/**
 * Copies the defined block to the cursor.
 * Validates the area and copies it to the current position.
 */
static void f11_copy_key(void)
{
    write_line_back_to_document_safely();
    if (reset_area_to_marks_1_2())
    {
        beep();
        return;
    }
    check_pointer_in_area();
    move_cursor_to_address(doc_ptr1);
}

/**
 * Moves the cursor left one character.
 * Decrements the cursor column if not at column zero.
 */
static void f12_left_key(void)
{
    if (visual_column == 0)
        return;
    xpos--;
}

/**
 * Moves the cursor right one character.
 * Increments the cursor column up to the maximum line length.
 * void
 */
void f13_right_key(void)
{
    if (xpos >= MAX_LINE_LENGTH)
        return;
    xpos++;
    return;
}

/**
 * Moves the cursor down one line.
 * Writes the buffer back and advances to the next line.
 */
static void f14_down_key(void)
{
    write_line_back_to_document_safely();
    line_change_pending_flag++;
    if (line_change_pending_flag != 0)
    {
        advance_current_line_pointer();
        return;
    }
    return_key();
}

/**
 * Moves the cursor up one line.
 * Writes the buffer back and moves to the previous line.
 */
static void f15_up_key(void)
{
    write_line_back_to_document_safely();
    uint8_t* line_ptr;
    if (!find_previous_line(current_line_ptr, &line_ptr))
        return;
    current_line_ptr = line_ptr;
    line_change_pending_flag++;
    cursor_moved_flag++;
}

/**
 * Moves the cursor to the top of the document.
 * Scrolls to the first line and unpacks it into the edit buffer.
 */
static void f1_top_of_text_key(void)
{
    scroll_repeat_count = 0xff;
    move_cursor_up(0xff);
    unpack_line(edit_buffer_base);
}

/**
 * Moves the cursor to the bottom of the document.
 * Scrolls to the last line, unpacks it, and moves to line end.
 */
static void f2_bottom_of_text_key(void)
{
    scroll_repeat_count = 0xff;
    move_cursor_down(0xff);
    unpack_line(edit_buffer_base);
    set_xpos_to_line_length();
}

/**
 * Deletes from the cursor to the end of the line.
 * Removes all characters from the cursor to the maximum line length.
 */
static void f3_delete_to_eol_key(void)
{
    uint8_t a_val = MAX_LINE_LENGTH;
    a_val -= xpos;
    line_counter++;
    delete_edit_buffer_bytes_at_xpos(a_val);
    return;
}

/**
 * Moves the cursor to the beginning of the line.
 * Sets the cursor column to zero and marks cursor movement.
 */
static void f4_beginning_of_line_key(void)
{
    cursor_moved_flag++;
    xpos = 0;
    return;
}

/**
 * Moves the cursor to the end of the line.
 * Sets the cursor column to the current line length.
 */
static void f5_end_of_line_key(void)
{
    cursor_moved_flag++;
    set_xpos_to_line_length();
}

/**
 * Inserts a new empty line at the cursor.
 * Writes the buffer back and inserts a line break at the current line.
 */
static void f6_insert_line_key(void)
{
    write_line_back_to_document_safely();
    line_change_pending_flag++;
    insert_line_at_cursor(current_line_ptr);
}

/**
 * Deletes the current line.
 * Removes the current line from the document and moves the cursor.
 */
static void f7_delete_line_key(void)
{
    write_line_back_to_document_safely();
    cursor_moved_flag++;
    uint8_t* insert_ptr = current_line_ptr;
    uint8_t x_val = edit_line_len;
    x_val++;
    ptrdiff_t size_delta = x_val;
    scratch_scan_ptr = adjust_pointers(insert_ptr, size_delta);
    ensure_cr_at_document_top();
    uint8_t a_val = current_line_ptr[0];
    if (a_val == 0)
    {
        uint8_t* line_ptr;
        find_previous_line(current_line_ptr, &line_ptr);
        current_line_ptr = line_ptr;
    }
    line_change_pending_flag++;
    clamp_ptr6_to_document();
    return;
}

/**
 * Inserts a space at the cursor.
 * Inserts a single space character at the current position.
 */
static void f8_insert_char_key(void)
{
    insert_character_into_edit_buffer(0x20);
    return;
}

/**
 * Deletes the character under the cursor.
 * Removes one character at the cursor position.
 */
static void f9_delete_char_key(void)
{
    line_counter++;
    delete_edit_buffer_bytes_at_xpos(1);
    return;
}

/**
 * Handles the K-command prefix.
 * Prompts for a follow-up key and dispatches to marker or block operations.
 */
static void k_command_key(void)
{
    draw_prompt_characters('^', 'K');
    flags_need_redrawing_flag++;
    uint8_t a_val = screen_getchar();
    uint8_t a_val1 = control_key_to_ascii(a_val);
    switch (a_val1)
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

/** Command table for CLI parsing. Encodes command names and flags. */
uint8_t parser_table[] = {

    0x0a,
    0x0e,
    0x12,
    0x0f,

    0x81,

    0x15,
    0x1e,
    0x0c,

    0x80,

    0x1d,
    0x14,
    9,
    0x36,
    0x3a,
    0x2f,

    0x81,

    8,
    0x1e,
    0x0f,
    0x2e,
    0x2b,

    0x80,

    9,
    0x1e,
    0x3a,
    0x3f,

    0x81,

    0x16,
    0x14,
    0x29,
    0x3e,

    0x81,

    8,
    0x18,
    0x29,
    0x3e,
    0x3e,
    0x35,

    0x81,

    8,
    0x13,
    0x3e,
    0x3e,
    0x2f,
    0x28,

    0x81,

    8,
    0x1a,
    0x2d,
    0x3e,

    0x81,

    0x18,
    0x14,
    0x2e,
    0x35,
    0x2f,

    0x81,

    0x1d,
    0x12,
    0x3e,
    0x37,
    0x3f,

    0x81,

    0x0b,
    9,
    0x12,
    0x15,
    0x0f,
    0x1e,
    0x29,

    0x81,

    8,
    0x3e,
    0x3a,
    0x29,
    0x38,
    0x33,

    0x81,

    0x18,
    0x17,
    0x3e,
    0x3a,
    0x29,

    0x81,

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

    0x81,

    0x1d,
    0x14,
    0x37,
    0x3f,

    0x81,

    0x15,
    0x3a,
    0x36,
    0x3e,

    0x81,

    0x16,
    0x34,
    0x3f,
    0x3e,

    0x80,

    0x1d,
    0x32,
    0x35,
    0x32,
    0x28,
    0x33,

    0x81,

    0x0b,
    0x29,
    0x32,
    0x35,
    0x2f,

    0x81,

    0x18,
    0x33,
    0x3a,
    0x35,
    0x3c,
    0x3e,

    0x81,

    0x0c,
    0x29,
    0x32,
    0x2f,
    0x3e,

    0x81,

    0x1e,
    0x3f,
    0x32,
    0x2f,

    0x80,

    9,
    0x3e,
    0x2b,
    0x37,
    0x3a,
    0x38,
    0x3e,

    0x81,

    0x17,
    0x34,
    0x3a,
    0x3f,

    0x80,

    0x19,
    0x22,
    0x3e,

    0x80,

    0

};

/**
 * Handles the O-command prefix.
 * Prompts for a follow-up key and dispatches to formatting and ruler
 * operations.
 */
static void o_command_key(void)
{
    draw_prompt_characters('^', 'O');
    flags_need_redrawing_flag++;
    uint8_t a_val = screen_getchar();
    uint8_t a_val1 = control_key_to_ascii(a_val);
    switch (a_val1)
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

/**
 * Handles the Q-command prefix.
 * Prompts for a follow-up key and dispatches to navigation and editing
 * operations.
 */
static void q_command_key(void)
{
    draw_prompt_characters('^', 'Q');
    flags_need_redrawing_flag++;
    uint8_t a_val = screen_getchar();
    uint8_t a_val1 = control_key_to_ascii(a_val);
    switch (a_val1)
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

/**
 * Handles the Return key.
 * Writes the buffer back and moves the cursor to the next line at column zero.
 */
void return_key(void)
{
    write_line_back_to_document_safely();
    xpos = 0;
    uint8_t* line_ptr;
    uint8_t y_val;
    if (!find_next_line(current_line_ptr, &line_ptr, &y_val))
    {
        advance_current_line_pointer();
        return;
    }
    uint16_t sum = (current_line_ptr - &ram[0]) + y_val;
    insert_line_at_cursor(&ram[sum]);
    advance_current_line_pointer();
    return;
}

/**
 * Moves the defined block to the cursor.
 * Validates the area, deletes the source, and inserts it at the cursor.
 */
static void sf0_move_block_key(void)
{
    write_line_back_to_document_safely();
    if (reset_area_to_marks_1_2())
    {
        beep();
        return;
    }
    check_pointer_in_area();
    top_of_screen_line_ptr = &ram[RAM_MAX];
    scroll_repeat_count = 0xff;
    adjust_area_pointers(area_size);
    ensure_cr_at_document_top();
    move_cursor_to_address(doc_ptr1);
    clear_marks_1_2();
}

/**
 * Copies the current ruler to a new line.
 * Inserts a new line and copies the ruler definition into it.
 */
static void sf11_copy_key(void)
{
    f6_insert_line_key();
    redraw_editor();
    uint8_t x_val = ruler_buffer_len;
    if (!(x_val == 0))
    {
        uint8_t y_val = 0;
        do
        {
            uint8_t a_val = current_ruler_ptr[y_val];
            ram[RAM_EDIT_BUFFER + y_val] = a_val;
            y_val++;
            x_val--;
        } while (x_val != 0);
    }
    cf8_mark_as_ruler_key();
}

/**
 * Moves the cursor left by one word.
 * Scans backward to the start of the previous word, crossing line boundaries if
 * needed.
 */
static void sf12_left_key(void)
{
    if (xpos == 0)
    {
        move_to_previous_line();
        return;
    }
    uint8_t word_boundary;
    _Bool is_start_of_line;
    uint8_t x_val;
    draw_previous_word(&word_boundary, &is_start_of_line, &x_val);
    if (!is_start_of_line)
        return;
    if (word_boundary == 0x20)
    {
        move_to_previous_line();
        return;
    }
}

/**
 * Moves the cursor right by one word.
 * Scans forward across words and wraps to the next line if needed.
 */
static void sf13_right_key(void)
{
    uint8_t y_val;
    _Bool is_tab;
entry:
    uint8_t* line_ptr = &ram[RAM_EDIT_BUFFER];
    uint8_t line_len = get_line_length();
    if (!(line_len < xpos || line_len == xpos))
    {
        y_val = xpos;
    }
    else
    {
        uint8_t y_val1;
        xpos = y_val1;
        write_line_back_to_document_safely();
        if (advance_to_next_line(current_line_ptr, &line_ptr, &y_val1))
            return;
        current_line_ptr += y_val1;
        unpack_line(edit_buffer_base);
        scroll_repeat_count--;
        xpos = 0;
        if (get_line_length() == xpos)
            return;
        y_val1 = 0;
        is_tab = false;
        uint8_t a_val1;
        uint8_t x_val;
        a_val1 = process_current_document_character(
            &ram[RAM_EDIT_BUFFER], &x_val, &y_val1, &is_tab);
        if (a_val1 != 0x20)
            return;
        goto entry;
    }
    for (;;)
    {
        if (y_val >= line_len)
            goto ca00f;
        uint8_t x_val;
        uint8_t a_val2 = process_current_document_character(
            line_ptr, &x_val, &y_val, &is_tab);
        if (a_val2 != 0x20)
            continue;
        break;
    }
    for (;;)
    {
        if (y_val >= line_len)
            goto ca00f;
        uint8_t x_val;
        uint8_t a_val3 = process_current_document_character(
            line_ptr, &x_val, &y_val, &is_tab);
        if (a_val3 == 0x20)
            continue;
        break;
    }
    y_val--;
ca00f:
    xpos = y_val;
    return;
}

static void set_marker(uint8_t x_val);

static void set_marker_common(uint8_t a_val);

/**
 * Moves the cursor down by a page.
 * Advances the cursor by screen height lines.
 */
static void sf14_down_key(void)
{
    line_change_pending_flag++;
    scroll_repeat_count++;
    move_cursor_down(screen_maxrow);
}

/**
 * Moves the cursor up by a page.
 * Moves the cursor up by screen height lines.
 */
static void sf15_up_key(void)
{
    line_change_pending_flag++;
    scroll_repeat_count++;
    move_cursor_up(screen_maxrow);
}

/**
 * Swaps the case of the character under the cursor.
 * Toggles alphabetic case and advances the cursor.
 */
static void sf1_swap_case_key(void)
{
    uint8_t a_val = ram[RAM_EDIT_BUFFER + xpos];
    if (!isalpha(a_val))
    {
        f13_right_key();
        return;
    }
    line_counter++;
    a_val ^= 0x20;
    ram[RAM_EDIT_BUFFER + xpos] = a_val;
    f13_right_key();
    return;
}

/**
 * Releases the left margin.
 * Moves the cursor to the left-margin stop or to column zero if not in format
 * mode.
 */
static void sf2_release_margins_key(void)
{
    if (!(format_mode_flag & 0x40))
    {
        xpos = 0;
        return;
    }
    int y_val = find_left_margin_stop();
    if (y_val < 0)
    {
        f4_beginning_of_line_key();
        return;
    }
    xpos = (uint8_t)y_val;
    return;
}

/**
 * Deletes from the cursor to a specified character.
 * Prompts for a character and deletes up to and including its next occurrence.
 */
static void sf3_delete_to_char_key(void)
{
    draw_prompt_characters('C', 'H');
    flags_need_redrawing_flag++;
    uint8_t a_val = screen_getchar();
    if (a_val == 9 || a_val == 0xa0 || a_val == 0xa1)
    {
        if (a_val == 0xa0)
            a_val = 0x1c;
        else if (a_val == 0xa1)
            a_val = 0x1d;
    }
    else if (a_val < 0x20 || a_val >= 0x7f)
    {
        beep();
        return;
    }
    {
        line_counter++;
        uint8_t y_val = xpos;
        uint8_t start_x = y_val;
        bool found_match = false;
        while (y_val < MAX_LINE_LENGTH)
        {
            uint8_t a_val1 = ram[RAM_EDIT_BUFFER + y_val];
            y_val++;
            if (a_val1 == a_val)
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
        while (y_val < MAX_LINE_LENGTH)
        {
            uint8_t a_val2 = ram[RAM_EDIT_BUFFER + y_val];
            y_val++;
            if (a_val2 != a_val)
                break;
        }
        y_val--;
        uint8_t x_val = y_val - start_x;
        delete_edit_buffer_bytes_at_xpos(x_val);
    }
}

/**
 * Inserts highlight code 1.
 * Inserts the first highlight control character at the cursor.
 */
static void sf4_highlight1_key(void)
{
    tab_highlight_common(0x1c);
    return;
}

/**
 * Inserts highlight code 2.
 * Inserts the second highlight control character at the cursor.
 */
static void sf5_highlight2_key(void)
{
    tab_highlight_common(0x1d);
    return;
}

/**
 * Jumps to a marker.
 * Prompts for a marker and moves the cursor to its position.
 */
static void sf6_go_to_marker_key(void)
{
    write_line_back_to_document_safely();
    int marker = prompt_for_marker();
    if (marker == MARKER_INVALID)
        return;
    if (markers_array[marker] == 0)
        return;
    go_to_marker(marker);
    return;
}

/**
 * Sets a marker at the cursor.
 * Prompts for a marker and stores the current position.
 */
static void sf7_set_marker_key(void)
{
    write_line_back_to_document_safely();
    int marker = prompt_for_marker();
    if (marker == MARKER_INVALID)
        return;
    set_marker(marker);
    return;
}

/**
 * Edits the formatting command on the current line.
 * Allows interactive editing of the command prefix characters.
 */
static void sf8_edit_command_key(void)
{
    uint8_t a_val1;
    xpos = 0;
    redraw_editor();
    edit_buffer_dirty_flag++;
    scratch_offset = 0;
    scratch_index = 0;
edit_command_loop:
    do
    {
        screen_setcursor(scratch_offset, ypos);
        a_val1 = screen_getchar();
        if (a_val1 == 0x0d)
            goto finished_editing_command;
        a_val1 &= 0xdf;
    } while (a_val1 < 0x41 || a_val1 >= 0x5b);
    scratch_index = a_val1;
    screen_putchar(a_val1);
    uint8_t y_val = scratch_offset;
    y_val++;
    scratch_offset = y_val;
    edit_buffer_base[y_val] = a_val1;
    if (y_val < 2)
        goto edit_command_loop;
    scratch_offset = 0;
    goto edit_command_loop;
finished_editing_command:
    if (scratch_index == 0)
        return;
    current_format_line_ptr = edit_buffer_base;
    current_format_line_ptr[0] = 0x80;
    set_format_mode_bit7();
    return;
}

/**
 * Deletes the formatting command prefix.
 * Removes the command prefix from the current line if present.
 */
static void sf9_delete_command_key(void)
{
    uint8_t y_val = 0;
    uint8_t a_val = current_format_line_ptr[y_val];
    command_prefix_t cp = check_for_command_prefix(a_val);
    if (cp == NO_COMMAND_PREFIX)
        return;
    current_format_line_ptr[y_val] = y_val;
    current_format_line_ptr = &ram[RAM_EDIT_BUFFER];
    clear_format_mode_bit7();
    line_counter++;
    edit_buffer_dirty_flag++;
    cursor_moved_flag++;
}

/**
 * Handles the Tab key.
 * Inserts a tab control character.
 */
static void tab_key(void)
{
    tab_highlight_common(9);
    return;
}

/**
 * Advances to the next line.
 * Moves the current line pointer to the following line and marks cursor
 * movement.
 */
static void advance_current_line_pointer(void)
{
    cursor_moved_flag++;
    uint8_t* line_ptr;
    uint8_t y_val;
    if (advance_to_next_line(current_line_ptr, &line_ptr, &y_val))
        return;
    current_line_ptr += y_val;
}

/**
 * Clears markers 1 and 2.
 * Resets marker array entries for markers 1 and 2.
 */
static void clear_marks_1_2(void)
{
    markers_array[0] = 0;
    markers_array[1] = 0;
}

/**
 * Converts a control key code to ASCII.
 * Maps control codes below 0x20 to letters and folds to uppercase.
 * @param a_val key code
 * @return converted ASCII code
 */
static uint8_t control_key_to_ascii(uint8_t a_val)
{
    if (a_val < 0x20)
        a_val |= 0x40;
    return toupper(a_val);
}

/**
 * Deletes bytes at the cursor in the edit buffer.
 * Removes a given number of bytes at the cursor, shifts content left, and
 * updates markers.
 * @param x_val number of bytes to delete
 */
static void delete_edit_buffer_bytes_at_xpos(uint8_t x_val)
{
    scratch_offset = x_val;
    edit_buffer_dirty_flag++;
    uint8_t* size_delta = &ram[RAM_EDIT_BUFFER];
    uint8_t y_val = xpos;
    uint8_t a_val = y_val;
    a_val += scratch_offset;
    do
    {
        x_val = find_marker_at_position(y_val, size_delta);
        if (!(x_val == 0x0c))
        {
            uint16_t marker_val =
                (y_val >= temp_save)
                    ? RAM_EDIT_BUFFER + (y_val - scratch_offset)
                    : 0;
            markers_array[x_val / 2] = marker_val ? &ram[marker_val] : NULL;
            continue;
        }
        y_val++;
    } while (y_val < MAX_LINE_LENGTH + 1);
    if (xpos >= MAX_LINE_LENGTH)
    {
        /* return_78: */
        return;
    }
    int copy_len = MAX_LINE_LENGTH - (int)xpos - (int)scratch_offset;
    if (copy_len > 0)
    {
        memmove(&ram[RAM_EDIT_BUFFER + xpos],
            &ram[RAM_EDIT_BUFFER + xpos + scratch_offset],
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

/**
 * Inserts a printable character.
 * Handles insertion, margin checks, wrapping, and justification for a typed
 * character.
 */
static void enter_printable_character(void)
{
    uint8_t a_val2;
    uint8_t a_val12;
    uint8_t y_val = xpos;
    if (y_val >= MAX_LINE_LENGTH)
        return;
    edit_buffer_dirty_flag++;
    if (adjust_margins_at_left_margin())
        return;
    uint8_t* size_delta = &ram[RAM_EDIT_BUFFER];
    uint8_t y_val1 = xpos;
    uint8_t idx = find_marker_at_position(y_val1, size_delta);
    if (idx != 0x0c)
    {
        if (idx < 4)
            line_counter++;
    }
    uint8_t x_val = insert_mode_flag;
    if (!(x_val != 0))
    {
        uint8_t a_val = ram[RAM_EDIT_BUFFER + y_val1];
        if (a_val == 9)
            goto c9c00;
        if (a_val != 0x0b)
            goto c9c09;
    }
c9c00:
    line_counter++;
    if (!insert_edit_buffer_bytes_at_xpos(1))
        return;
c9c09:
    uint8_t a_val1 = editor_current_key;
    ram[RAM_EDIT_BUFFER + xpos] = a_val1;
    uint8_t y_val2 = line_counter;
    if (y_val2 == 0)
        screen_putchar(a_val1);
    xpos++;
    update_line_length();
    uint8_t y_val3 = 0;
    column_position = 0;
c9c1d:
    do
    {
        a_val2 = ram[RAM_EDIT_BUFFER + y_val3];
        y_val3++;
        if (y_val3 > xpos)
            goto c9c56;
        if (!(a_val2 != 9))
        {
            {
                bool is_tab = false;
                (void)process_document_character(a_val2, &x_val, &is_tab);
            }
            a_val2 = x_val;
            a_val2 += column_position;
            if (a_val2 != 0)
                goto c9c43;
        }
        if (!(a_val2 != 0x0b))
        {
            a_val2 = ruler_left_stop;
            if (!(a_val2 == 0))
            {
                x_val = column_position;
                if (x_val != 0)
                {
                    if (x_val >= ruler_left_stop)
                    {
                        x_val++;
                        a_val2 = x_val;
                    }
                }
            c9c43:
                column_position = a_val2;
                continue;
            }
        c9c48:
            a_val2 = 0x20;
        }
        if (a_val2 < 0x1b)
            goto c9c48;
    } while (a_val2 < 0x20);
    column_position++;
    goto c9c1d;
c9c56:
    uint8_t y_val4 = column_position;
    if (y_val4 < ruler_buffer_len)
    {
        uint8_t a_val4 = current_ruler_ptr[y_val4];
        a_val4 &= 0xdf;
        if (a_val4 == 0x42)
            beep();
    }
    uint8_t a_val5 = editor_current_key;
    if (a_val5 == 0x20)
        return;
    if (ruler_right_stop == 0)
    {
        line_counter = 0;
        return_to_editor_loop();
    }
    if (format_mode_flag != 0)
        return;
    if (y_val4 == 0)
        return;
    y_val4--;
    if (y_val4 < ruler_right_stop)
        return;
    screen_column = get_line_length();
    top_margin = 0;
    uint8_t y_val5 = xpos;
    input_buffer_offset = y_val5;
    uint8_t word_boundary;
    _Bool is_start_of_line;
    draw_previous_word(&word_boundary, &is_start_of_line, &x_val);
    recalculate_cursor_xpos();
    uint8_t a_val6 = visual_column;
    if (a_val6 == ruler_left_stop)
    {
        uint8_t y_val6 = input_buffer_offset;
        y_val6--;
        xpos = y_val6;
        goto c9ca2;
    }
    if (a_val6 < ruler_left_stop)
    {
        {
            uint8_t y_val7 = input_buffer_offset;
            y_val7--;
            xpos = y_val7;
        }
    }
c9ca2:
    uint8_t a_val7 = input_buffer_offset;
    a_val7 -= xpos;
    top_margin = a_val7;
    uint8_t a_val8 = screen_column;
    a_val8 -= xpos;
    screen_column = a_val8;
    uint8_t y_val8 = a_val8;
    y_val8++;
    uint8_t a_val9 = ruler_left_stop;
    if (a_val9 != 0)
    {
        top_margin++;
        y_val8++;
    }
    uint8_t* insert_ptr = current_line_ptr + edit_line_len + 1;
    if (!(make_space_for_insertion(insert_ptr, y_val8)))
    {
        show_memory_full_error();
        longjmp(env, JMP_EDITOR);
    }
    uint8_t y_val9 = 0;
    if (ruler_left_stop != 0)
    {
        *insert_ptr = 0x0b;
        y_val9 = 1;
    }
    scratch_index = y_val9;
    uint8_t* marker_base_ptr = &ram[RAM_EDIT_BUFFER];
    uint8_t y_val10 = xpos;
    y_val10--;
    uint8_t a_val10 = ram[RAM_EDIT_BUFFER + y_val10];
    if (a_val10 == 0x20)
        ram[RAM_EDIT_BUFFER + y_val10] = 0x10;
    y_val10++;
    screen_row = y_val10;
    do
    {
        uint8_t y_val11 = screen_row;
        screen_row++;
        for (;;)
        {
            uint8_t marker_index =
                find_marker_at_position(y_val11, marker_base_ptr);
            if (marker_index == 0x0c)
                break;
            {
                uint8_t* val_ptr = insert_ptr + scratch_index;
                markers_array[marker_index / 2] = val_ptr;
                if (insert_ptr - &ram[0] + scratch_index >= 0x10000)
                    break;
            }
        }
        uint8_t a_val11 = screen_column;
        if (!(a_val11 != 0))
        {
            a_val12 = 0x0d;
        }
        else
        {
            uint8_t a_val13 = ram[RAM_EDIT_BUFFER + y_val11];
            {
                uint8_t saved = a_val13;
                ram[RAM_EDIT_BUFFER + y_val11] = 0x10;
                a_val12 = saved;
            }
        }
        uint8_t y_val12 = scratch_index;
        scratch_index++;
        insert_ptr[y_val12] = a_val12;
        screen_column--;
    } while (!(screen_column & 0x80));
    justify_edit_buffer(edit_buffer_base);
    write_line_back_to_document_safely();
    clamp_ptr6_to_document();
    return_key();
    xpos = top_margin;
    return;
}

/**
 * Prompts for a marker key.
 * Displays the marker prompt and reads a key, returning its marker index.
 * @return marker index 0-5 or MARKER_INVALID
 */
static int prompt_for_marker(void)
{
    draw_prompt_characters('M', 'K');
    flags_need_redrawing_flag++;
    uint8_t a_val = screen_getchar();
    return lookup_marker(a_val);
}

/**
 * Sets the area to markers 1 and 2.
 * Looks up markers 1 and 2, sets area pointers, and validates the area.
 * @return true if area invalid or empty, false on success
 */
static bool reset_area_to_marks_1_2(void)
{
    int idx1 = lookup_marker(0x31);
    if (idx1 == MARKER_INVALID)
        return true;
    if (!(markers_array[idx1] == 0))
    {
        area_start_ptr = markers_array[idx1];
        int idx2 = lookup_marker(0x32);
        if (idx2 == MARKER_INVALID)
            return true;
        if (markers_array[idx2] != 0)
        {
            area_end_ptr = markers_array[idx2];
            uint8_t x_val = ((uint8_t*)&doc_ptr1 - (uint8_t*)markers_array) / 2;
            set_marker_to_here(x_val);
            area_status_t status = sanitise_area();
            if (status == AREA_NOT_EMPTY)
                return false;
        }
    }
    return true;
}

/**
 * Inserts a line at the cursor.
 * Increments cursor-moved flag and inserts a line break at the given pointer.
 * @param target_ptr insertion point in document heap
 */
static void insert_line_at_cursor(uint8_t* target_ptr)
{
    cursor_moved_flag++;
    insert_line_into_document(target_ptr);
}

/**
 * Moves the cursor to the previous line.
 * Writes the buffer back, moves to the previous line, and sets the cursor to
 * its end.
 */
static void move_to_previous_line(void)
{
    write_line_back_to_document_safely();
    uint8_t* line_ptr;
    if (!find_previous_line(current_line_ptr, &line_ptr))
        return;
    current_line_ptr = line_ptr;
    unpack_line(edit_buffer_base);
    set_xpos_to_line_length();
    scroll_repeat_count--;
}

/**
 * Moves the cursor up.
 * Writes the buffer back and moves up by the specified number of lines.
 * @param x_val number of lines to move
 */
static void move_cursor_up(uint8_t x_val)
{
    cursor_moved_flag++;
    scratch_offset = x_val;
    write_line_back_to_document_safely();
    uint8_t* line = current_line_ptr;
    do
    {
        uint8_t* copy_ptr = line;
        uint8_t* line_ptr;
        if (!find_previous_line(line, &line_ptr))
        {
            line = copy_ptr;
            break;
        }
        line = line_ptr;
        x_val = scratch_offset;
        if ((int8_t)x_val < 0)
            continue;
        scratch_offset--;
    } while (scratch_offset != 0);
    current_line_ptr = line;
}

/**
 * Moves the cursor down.
 * Writes the buffer back and moves down by the specified number of lines.
 * @param x_val number of lines to move
 */
static void move_cursor_down(uint8_t x_val)
{
    cursor_moved_flag++;
    scratch_offset = x_val;
    write_line_back_to_document_safely();
    uint8_t* line = current_line_ptr;
    while (1)
    {
        uint8_t* line_ptr;
        uint8_t y_val;
        if (advance_to_next_line(line, &line_ptr, &y_val))
        {
            line = line_ptr;
            break;
        }
        line = line + y_val;
        x_val = scratch_offset;
        if ((int8_t)x_val < 0)
            continue;
        scratch_offset--;
        if (scratch_offset != 0)
            continue;
        break;
    }
    current_line_ptr = line;
}

/**
 * Ensures the copy pointer is outside the current area.
 * If inside, beeps; otherwise copies the area to the cursor.
 */
static void check_pointer_in_area(void)
{
    if (doc_ptr1 >= area_start_ptr && doc_ptr1 <= area_end_ptr)
    {
        beep();
        return;
    }
    move_cursor_to_address(area_start_ptr);
    area_size = area_end_ptr - area_start_ptr;
    uint8_t* insert_ptr = doc_ptr1;
    if (!make_space_for_insertion(insert_ptr, area_size))
    {
        show_memory_full_error();
        longjmp(env, JMP_EDITOR);
    }
    uint8_t* scan_ptr = area_start_ptr;
    uint8_t* copy_ptr = doc_ptr1;
    while (1)
    {
        *copy_ptr = *scan_ptr;
        copy_ptr++;
        scan_ptr++;
        if (scan_ptr == area_end_ptr)
            break;
    }
    doc_ptr1 = insert_ptr;
    uint8_t* adjusted = copy_ptr - 1;
    split_line_at_wrap(adjusted);
    split_line_at_wrap(doc_ptr1);
    display_start_row = 1;
    cursor_moved_flag = 1;
}

/**
 * Common handler for tab and highlight insertion.
 * Checks margins and inserts the given control character, then advances the
 * cursor.
 * @param a_val control character to insert
 */
static void tab_highlight_common(uint8_t a_val)
{
    if (adjust_margins_at_left_margin())
        return;
    if (!insert_character_into_edit_buffer(a_val))
        return;
    f13_right_key();
    return;
}

/**
 * Initialises the editor mode.
 * Clears the screen and resets editor state variables.
 */
void enter_editor_mode(void)
{
    screen_enter();
    clear_screen();
    edit_buffer_dirty_flag = 0;
    scroll_repeat_count = 0;
    edit_buffer_unpacked_flag = 0;
    uint8_t x_val = screen_maxrow;
    do
    {
        line_lengths[x_val] = 0;
        x_val--;
    } while (!(x_val & 0x80));
    status_line_needs_redrawing_flag = 2;
    flags_need_redrawing_flag = 1;
}

/**
 * Clears format-mode bit 7.
 * Clears the high bit of the format mode flag and marks redraw if changed.
 */
void clear_format_mode_bit7(void)
{
    uint8_t old = format_mode_flag;
    format_mode_flag &= ~0x80;
    if (old != format_mode_flag)
        flags_need_redrawing_flag++;
}

/**
 * Sets format-mode bit 7.
 * Sets the high bit of the format mode flag and marks redraw if changed.
 */
void set_format_mode_bit7(void)
{
    uint8_t old = format_mode_flag;
    format_mode_flag |= 0x80;
    if (old != format_mode_flag)
        flags_need_redrawing_flag++;
}

void draw_previous_word(
    uint8_t* word_boundary, bool* is_start_of_line, uint8_t* char_width)
{
    uint8_t pos = xpos;
    bool is_tab = false;
    if (!(pos == 0))
    {
        for (;;)
        {
            pos--;
            if (pos == 0)
                goto caf55;
            uint8_t ch;
            ch = process_current_document_character(
                &ram[RAM_EDIT_BUFFER], char_width, &pos, &is_tab);
            pos--;
            if (ch == 0x20)
                continue;
            break;
        }
        do
        {
            pos--;
            uint8_t ch_1;
            ch_1 = process_current_document_character(
                &ram[RAM_EDIT_BUFFER], char_width, &pos, &is_tab);
            if (ch_1 == 0x20)
                break;
            pos--;
        } while (pos != 0);
    }
caf55:
    xpos = pos;
    uint8_t ch_2;
    ch_2 = process_current_document_character(
        &ram[RAM_EDIT_BUFFER], char_width, &pos, &is_tab);
    pos--;
    *word_boundary = ch_2;
    *is_start_of_line = (pos == 0);
}

/**
 * Handles margin adjustment at the left margin.
 * Adjusts cursor and inserts margin tab when typing at the left edge.
 * @return true if insertion failed
 */
bool adjust_margins_at_left_margin(void)
{
    uint8_t y_val;
    uint8_t a_val = format_mode_flag;
    a_val &= 0x81;
    if (!(a_val != 0 || find_left_margin_stop() >= 0))
    {
        uint8_t line_len = get_line_length();
        screen_column = xpos;
        xpos = line_len;
        recalculate_cursor_xpos();
        if (!(visual_column < ruler_left_stop))
        {
            y_val = screen_column;
            xpos = y_val;
            xpos++;
        }
        else
        {
            uint8_t a_val3 = screen_column;
            y_val = xpos;
            if (!(y_val >= screen_column))
            {
                if (screen_column < ruler_left_stop)
                    goto caf2a;
                a_val3 = screen_column - ruler_left_stop + xpos + 1;
            }
            xpos = a_val3;
        }
    caf2a:
        if (!insert_byte_at_xpos(y_val))
            return true;
        line_counter++;
    }
    return false;
}

/**
 * Inserts bytes at the cursor in the edit buffer.
 * Makes room at the cursor by shifting existing content right.
 * @param x_val number of bytes to insert
 * @return true on success, false if overflow or out of space
 */
bool insert_edit_buffer_bytes_at_xpos(uint8_t x_val)
{
    if (xpos >= MAX_LINE_LENGTH)
    {
        beep();
        return false;
    }
    scratch_offset = x_val;
    uint8_t a_val1 = get_line_length();
    a_val1 += scratch_offset;
    if (a_val1 < scratch_offset)
    {
        beep();
        return false;
    }
    if (a_val1 >= MAX_LINE_LENGTH + 1)
    {
        beep();
        return false;
    }
    edit_buffer_dirty_flag++;
    uint8_t* size_delta = &ram[RAM_EDIT_BUFFER];
    uint8_t y_val = MAX_LINE_LENGTH;
cae27:
    y_val--;
    x_val = 0;
    uint8_t a_val2 = y_val;
    a_val2 += scratch_offset;
    if (a_val2 >= scratch_offset)
    {
        if (a_val2 < MAX_LINE_LENGTH)
            x_val = a_val2;
    }
    scratch_index = x_val;
    for (;;)
    {
        uint8_t idx = find_marker_at_position(y_val, size_delta);
        if (idx == 0x0c)
            goto cae52;
        uint16_t marker_val =
            scratch_index ? RAM_EDIT_BUFFER + scratch_index : 0;
        markers_array[idx / 2] = marker_val ? &ram[marker_val] : NULL;
    }
cae52:
    if (y_val != xpos)
        goto cae27;
    int copy_len = MAX_LINE_LENGTH - (int)xpos - (int)scratch_offset;
    if (copy_len > 0)
    {
        memmove(&ram[RAM_EDIT_BUFFER + xpos + scratch_offset],
            &ram[RAM_EDIT_BUFFER + xpos],
            (size_t)copy_len);
    }
    return true;
}

/**
 * Sets a marker to the current position.
 * Computes the document address for the cursor and stores it in the marker
 * array.
 * @param x_val marker index
 */
void set_marker_to_here(uint8_t x_val)
{
    uint8_t y_val;
    (void)y_val;
    uint8_t len = get_line_length();
    if (len >= xpos)
    {
        uint8_t a_val = current_format_line_ptr[0];
        command_prefix_t cp = check_for_command_prefix(a_val);
        len = xpos;
        if (cp != NO_COMMAND_PREFIX)
            len += 3;
    }
    uint16_t marker_addr = (current_line_ptr - &ram[0]) + len;
    markers_array[x_val] = &ram[marker_addr];
}

/**
 * Splits a line at wrap position.
 * Inserts carriage returns at word boundaries to enforce line length limits.
 * @param target_ptr pointer within the line to split
 */
void split_line_at_wrap(uint8_t* target_ptr)
{
    uint8_t a_val3;
    uint8_t* scan_ptr = find_line_start(target_ptr);
    do
    {
        screen_column = 0;
        uint8_t x_val = MAX_LINE_LENGTH + 1;
        uint8_t y_val = 1;
        uint8_t a_val1 = scan_ptr[y_val];
        command_prefix_t cp = check_for_command_prefix(a_val1);
        if (cp != NO_COMMAND_PREFIX)
        {
            x_val++;
            x_val++;
            x_val++;
        }
        temp_save = x_val;
        do
        {
            uint8_t a_val2 = scan_ptr[y_val];
            y_val++;
            if (!(a_val2 == 0x20))
            {
                if (a_val2 != 0x1a)
                    goto cac9c;
            }
            screen_column = y_val;
        cac9c:
            if (a_val2 == 0x0d)
                return;
        } while (y_val == temp_save || y_val < temp_save);
        if (screen_column == 0)
        {
            a_val3 = temp_save;
            goto cacad;
        }
        a_val3 = screen_column;
    cacad:
        uint8_t* insert_ptr = scan_ptr + a_val3;
        scan_ptr = insert_ptr;
        make_space_for_insertion(insert_ptr, 1);
        insert_ptr[0] = 0x0d;
        scan_ptr = insert_ptr;
    } while (((uint8_t*)&scan_ptr)[1] != 0);
    return;
}

/**
 * Adjusts pointers after a document size change.
 * Updates all heap pointers for an insertion or deletion and moves the heap
 * content.
 * @param insert_ptr base of changed region
 * @param size_delta signed size change (negative for deletion)
 * @return pointer to the end of the moved region
 */
uint8_t* adjust_pointers(uint8_t* insert_ptr, ptrdiff_t size_delta)
{
    uint8_t* copy_ptr = insert_ptr;
    uint8_t* local_tmp89 = insert_ptr + size_delta;
    uint8_t x_val = 0;
    do
    {
        {
            uint8_t* pa_val = ((uint8_t**)&pointer_array)[x_val];
            if (pa_val < insert_ptr)
                goto ca9f1;
            if (pa_val < local_tmp89)
                goto ca9db;
            goto ca9e7;
        }
    ca9db:
        if (!(x_val >= ARRAY_SIZE(markers_array)))
            ((uint8_t**)&pointer_array)[x_val] = NULL;
        else
        ca9e7:
        {
            ((uint8_t**)&pointer_array)[x_val] -= size_delta;
        }
        ca9f1:
            x_val++;
    } while (x_val != sizeof(pointer_array) / sizeof(uint8_t*));
    {
        size_t copy_len = strlen((char*)local_tmp89) + 1;
        memmove(copy_ptr, local_tmp89, copy_len);
        top = copy_ptr + copy_len - 1;
    }
    return local_tmp89;
}

/**
 * Advances to the next document line.
 * Zeroes xpos and moves current_line_ptr past the current CR.
 * @return true if at end of document
 */
static bool advance_to_next_doc_line(void)
{
    xpos = 0;
    uint8_t* line_ptr;
    uint8_t y_val;
    bool end_of_document = find_next_line(current_line_ptr, &line_ptr, &y_val);
    if (!end_of_document)
        current_line_ptr = line_ptr + y_val;
    return end_of_document;
}

[[nodiscard]] static bool flush_formatted_line(void)
{
    line_format_status = (uint8_t)(line_format_status << 1) | 1;
    edit_line_len = format_src_index - 1;
    edit_buffer_unpacked_flag++;
    if (!write_line_back_to_document())
        return false;
    return true;
}

/**
 * Emits a beep.
 * Sends a bell character to the output.
 */
void beep(void)
{
    cli_putchar(7);
}

/**
 * Scans for the next line matching the search.
 * Searches the document between doc_ptr2 and doc_ptr3 for the next match.
 * @return true if a match was found
 */
bool scan_document_for_next_line(void)
{
    if (search_target_len == 0)
        return false;
    editor_output_pos = 0x14;
    uint8_t x_val = 0;
    editor_header_pos = 0;
    scratch_index = 0;
    uint8_t* scan_ptr = doc_ptr2;
c8b91:
    if (scan_ptr >= doc_ptr3)
        return false;
    uint8_t y_val = 0;
    uint8_t a_val2 = *scan_ptr;
    command_prefix_t cp = check_for_command_prefix(a_val2);
    if (cp != NO_COMMAND_PREFIX)
    {
        doc_ptr2 = scan_ptr + 3;
        return scan_document_for_next_line();
    }
    screen_column = upper_case_unless_folding(a_val2);
    do
    {
        y_val++;
        uint8_t a_val4 = scan_ptr[y_val];
        if (a_val4 == 0)
            goto c8bdb;
        command_prefix_t cp2 = check_for_command_prefix(a_val4);
        if (cp2 != NO_COMMAND_PREFIX)
            goto c8bdb;
        uint8_t a_val5 = header_text_maybe[x_val];
        if (a_val5 == 0x20)
            goto c8bf7;
        if (a_val5 == 1)
            goto c8be3;
        if (a_val5 == 2)
            a_val5 = 0x20;
        if (a_val5 == screen_column)
            goto c8c33;
    c8bdb:
        do
        {
            doc_ptr2++;
            return scan_document_for_next_line();
        c8be3:
            temp_save = x_val;
            if (editor_header_pos < 0x14)
            {
                output_buffer[editor_header_pos] = screen_column;
                editor_header_pos++;
            }
            x_val = temp_save;
            goto c8c33;
        c8bf7:
            temp_save = x_val;
            if (screen_column == 0x20 || screen_column == 9 ||
                screen_column == 0x0b || screen_column == 0x1a ||
                screen_column == 0x0d)
            {
                goto c8c23;
            }
        } while (scratch_index == 0);
        append_to_output_buffer(0);
        scratch_index = 0;
        x_val = temp_save;
        x_val++;
    } while (x_val < search_target_len);
    goto c8c3e;
c8c23:
    append_to_output_buffer(screen_column);
    x_val = temp_save;
    scratch_index = screen_column;
    do
    {
        scan_ptr++;
        goto c8b91;
    c8c33:
        x_val++;
    } while (x_val < search_target_len);
    scan_ptr++;
c8c3e:
    doc_working_ptr = doc_ptr2;
    doc_ptr2 = scan_ptr;
    return true;
}

/**
 * Inserts an empty line into the document.
 * Creates space and inserts a CR at the target address.
 * @param target_ptr insertion point
 */
static void insert_line_into_document(uint8_t* target_ptr)
{
    uint8_t* insert_ptr = target_ptr;
    if (make_space_for_insertion(insert_ptr, 1))
    {
        *insert_ptr = 0x0d;
        clamp_ptr6_to_document();
        return;
    }
    memory_full();
    return;
}

/**
 * Updates the stored line length.
 * Copies the current screen width into line_lengths for the current row.
 */
static void update_line_length(void)
{
    line_lengths[ypos] = screen_maxcolumn;
    return;
}

/**
 * Clamps the editor pointer to the current line.
 * Ensures editor_ptr6 does not advance beyond current_line_ptr and resets
 * display flags.
 */
void clamp_ptr6_to_document(void)
{
    if (current_line_ptr < editor_ptr6)
        editor_ptr6 = current_line_ptr;
    display_start_row = 0xff;
    ptr6_screen_row = 0xff;
}

/**
 * Clears the screen.
 * Invokes the screen driver to clear the display.
 */
void clear_screen(void)
{
    screen_clear();
    return;
}

/**
 * Clears to the end of the line.
 * Fills the remainder of the screen line with the given character.
 * @param a_val fill character
 * @param line screen line index
 */
static void clear_to_eol(uint8_t a_val, uint8_t line)
{
    uint8_t line_len = line_lengths[line];
    if (!(line_len == 0))
    {
        do
        {
            screen_putchar(a_val);
            line_lengths[line]--;
        } while (line_lengths[line] != 0);
    }
    return;
}

/**
 * Disables the cursor.
 * Hides the cursor via the screen driver.
 */
static void cursor_off(void)
{
    screen_enablecursor(0);
}

/**
 * Enables the cursor.
 * Shows the cursor via the screen driver.
 */
static void cursor_on(void)
{
    screen_enablecursor(1);
}

/**
 * Renders a document line to the screen.
 * Formats and draws a line with correct highlighting and horizontal scrolling.
 * @param rs render state
 * @param addr address of the document line
 */
void draw_line(struct render_state* rs, uint8_t* addr)
{
    rs->line_ptr = addr;
    scratch_line_ptr = addr;
    screen_setcursor(0, rs->line);
    rs->pos = 0;
    rs->col = 0;
    rs->buf_off = 0;
    rs->char_width = 0;
    command_prefix_t f =
        deref_and_check_for_command_prefix(0, scratch_line_ptr);
    if (f == NO_COMMAND_PREFIX || hscroll_pos != 0)
    {
        rs->ch = 0x20;
        render_char(rs);
        render_char(rs);
    }
    else
    {
        rs->pos = 1;
        advance_to_next_char_and_render(rs);
        advance_to_next_char_and_render(rs);
        rs->ch = 0x20;
    }
    render_char(rs);
    do
    {
        advance_to_next_char(rs);
        do
        {
            render_xchar(rs);
            rs->width--;
        } while (rs->width != 0);
    } while (rs->ch != 0x0d);
    clear_to_eol(0x20, rs->line);
    clear_to_eol(0x20, rs->line);
    line_lengths[rs->line] = rs->col;
}

/**
 * Draws prompt characters.
 * Displays two inverted prompt characters at the top-left and restores the
 * cursor.
 * @param x_val first prompt character
 * @param y_val second prompt character
 */
void draw_prompt_characters(uint8_t x_val, uint8_t y_val)
{
    uint16_t saved_cursor_pos = screen_getcursor();
    cursor_off();
    home_cursor();
    screen_setstyle(STYLE_REVERSE);
    screen_putchar(x_val);
    screen_putchar(y_val);
    screen_setstyle(0);
    screen_putchar(0x20);
    screen_setcursor(saved_cursor_pos & 0xff, saved_cursor_pos >> 8);
}

/**
 * Draws the ruler line.
 * Renders the current ruler at the top row if a redraw is needed.
 */
static void draw_ruler(void)
{
    if (status_line_needs_redrawing_flag == 0)
        return;
    status_line_needs_redrawing_flag = 0;
    struct render_state rs = {.line = 0};
    draw_line(&rs, current_ruler_ptr);
    flags_need_redrawing_flag = 1;
    return;
}

/**
 * Draws the status bar.
 * Shows format, justify, and insert mode indicators.
 */
static void draw_status_word(void)
{
    flags_need_redrawing_flag = 0;
    home_cursor();
    uint8_t x_val = 0x46;
    uint8_t a_val = format_mode_flag;
    if (a_val != 0)
    {
        x_val = 0x4d;
        a_val &= 0xc0;
        if (a_val == 0)
            x_val = 0x20;
    }
    screen_putchar(x_val);
    uint8_t a_val2 = 0x4a;
    if (justifying_flag != 0)
        a_val2 = 0x20;
    screen_putchar(a_val2);
    if (insert_mode_flag != 0)
    {
        home_cursor();
        return;
    }
    home_cursor();
    return;
}

/**
 * Returns the length of the current edit line.
 * Scans the edit buffer for the last non-fill byte, adjusting for command
 * prefixes.
 * @return line length in characters
 */
static uint8_t get_line_length(void)
{
    uint8_t a_val = *current_format_line_ptr;
    command_prefix_t cp = check_for_command_prefix(a_val);
    uint8_t y_val = MAX_LINE_LENGTH;
    do
    {
        y_val--;
        if (ram[RAM_EDIT_BUFFER + y_val] != 0x10)
            goto cab06;
    } while (y_val != 0);
    y_val--;
cab06:
    y_val++;
    if (cp != NO_COMMAND_PREFIX)
        y_val += 3;
    return y_val;
}

/**
 * Jumps to a marker.
 * Moves the cursor to the address stored in the marker.
 * @param x_val marker index
 */
static void go_to_marker(uint8_t x_val)
{
    move_cursor_to_address(markers_array[x_val]);
    display_start_row = 1;
    update_line_length();
    return;
}

/**
 * Jumps to a numbered marker.
 * Writes the buffer back and jumps to the marker identified by the given
 * character.
 * @param marker marker character '1'-'6'
 */
static void go_to_marker_n(uint8_t marker)
{
    write_line_back_to_document_safely();
    int idx = lookup_marker(marker);
    if (markers_array[idx] != 0)
        go_to_marker(idx);
    return;
}

/**
 * Moves the cursor to the home position.
 * Sets the cursor to row 0, column 0.
 */
static void home_cursor(void)
{
    screen_setcursor(0, 0);
    return;
}

/**
 * Justifies the edit buffer line.
 * Distributes extra spaces between words to align to the right margin.
 * @param target_ptr buffer to justify
 * @return last processed character
 */
uint8_t justify_edit_buffer(uint8_t* target_ptr)
{
    _Bool is_zero_1;
    uint8_t x_val = 0;
    uint8_t a_val = justifying_flag;
    if (a_val != 0)
        return x_val;
    justify_gap_count = a_val;
    column_position = a_val;
    justify_overflow_counter = a_val;
    uint8_t a_val1 = ruler_right_stop;
    if (a_val1 == 0)
        return x_val;
    justify_line_length = get_line_length();
    uint8_t y_val = 0;
    goto c9861;
c9847:
    do
    {
        a_val1 = column_position;
        temp_save = a_val1;
        y_val++;
        if (y_val == justify_line_length)
            goto c9871;
        bool is_zero = process_char_for_output(y_val, false, &x_val, &a_val1);
        if (is_zero)
            goto c985c;
    } while (a_val1 != 0x20);
    justify_gap_count++;
c985c:
    do
    {
        y_val++;
        if (y_val == justify_line_length)
            goto c986d;
    c9861:
        is_zero_1 = process_char_for_output(y_val, true, &x_val, &a_val1);
    } while (is_zero_1);
    if (a_val1 != 0x20)
        goto c9847;
    goto c985c;
c986d:
    justify_gap_count--;
    if (justify_gap_count & 0x80)
        return x_val;
c9871:
    uint8_t a_val2 = justify_gap_count;
    if (a_val2 == 0)
        return x_val;
    uint8_t a_val3 = ruler_right_stop;
    if (a_val3 < temp_save)
        return x_val;
    a_val3 -= temp_save;
    uint8_t x_val1 = (uint8_t)(a_val3 + 1);
    uint8_t extra = (uint8_t)(a_val3 + 1 + justify_line_length);
    if (extra >= MAX_LINE_LENGTH)
    {
        temp_save = (uint8_t)(extra - MAX_LINE_LENGTH);
        x_val1 = (uint8_t)(x_val1 - temp_save);
    }
    screen_row = x_val1;
    uint16_t scan_ptr = x_val1;
    uint8_t a_val4 = scan_ptr % justify_gap_count;
    uint16_t quotient_tmp = scan_ptr / justify_gap_count;
    justify_running_total_accum = a_val4;
    uint8_t a_val5 = quotient_tmp & 0xff;
    justify_extra_space_accum = a_val5;
    uint8_t y_val1 = 0;
    uint8_t x_val2 = justify_gap_count;
    uint8_t a_val6 = y_val1;
    do
    {
        input_buffer[y_val1] = a_val6;
        y_val1++;
        x_val2--;
    } while (x_val2 != 0);
    uint8_t y_val2 = print_xpos;
    y_val2++;
    if (y_val2 >= justify_gap_count)
        y_val2 = 1;
    y_val2--;
    uint8_t x_val3 = justify_gap_count;
    do
    {
        uint8_t a_val7 = justify_running_total_accum;
        if (a_val7 != 0)
        {
            a_val7 = 1;
            justify_running_total_accum--;
        }
        a_val7 += justify_extra_space_accum;
        input_buffer[y_val2] = a_val7;
        uint8_t a_val8 = screen_row;
        uint8_t a_val9 = a_val8 - input_buffer[y_val2];
        screen_row = a_val9;
        y_val2++;
        if (y_val2 >= justify_gap_count)
            y_val2 = 0;
        if (a_val9 == 0)
            break;
        x_val3--;
    } while (x_val3 != 0);
    print_xpos = y_val2;
    uint8_t y_val3 = 0;
    scratch_index = y_val3;
    column_position = y_val3;
    wipe_buffer(0x1a, target_ptr);
    uint8_t a_val10 = justify_overflow_counter;
    if (!(a_val10 == 0))
    {
        uint8_t y_val4 = 0;
        do
        {
            uint8_t a_val11 = output_buffer[y_val4];
            ram[RAM_EDIT_BUFFER + y_val4] = a_val11;
            y_val4++;
        } while (y_val4 != justify_overflow_counter);
    }
    uint8_t y_val5 = justify_overflow_counter;
    uint8_t x_val4 = justify_overflow_counter;
    do
    {
        uint8_t a_val12 = output_buffer[x_val4];
        if (!(a_val12 != 0x20))
        {
            uint8_t a_val13 = scratch_index;
            if (!(a_val13 == 0))
            {
                temp_save = y_val5;
                uint8_t y_val6 = column_position;
                if (y_val6 < justify_gap_count)
                    a_val13 = input_buffer[y_val6];
                a_val13 += temp_save;
                column_position++;
                y_val5 = a_val13;
                scratch_index = 0;
            }
            a_val12 = 0x20;
        }
        else
        {
            scratch_index++;
        }
        ram[RAM_EDIT_BUFFER + y_val5] = a_val12;
        y_val5++;
        x_val4++;
    } while (x_val4 != justify_line_length);
    while (1)
    {
        if (y_val5 >= MAX_LINE_LENGTH)
            return x_val4;
        ram[RAM_EDIT_BUFFER + y_val5] = 0x10;
        y_val5++;
    }
    return x_val4;
}

/**
 * Makes space for an insertion in the heap.
 * Shifts heap content and adjusts pointers; checks against himem.
 * @param insert_ptr insertion point
 * @param size_delta bytes to create
 * @return true if space was made
 */
bool make_space_for_insertion(uint8_t* insert_ptr, ptrdiff_t size_delta)
{
    uint8_t* copy_ptr = top;
    uint8_t* scan_ptr = top + size_delta;
    if (scan_ptr >= himem)
        return false;
    top = scan_ptr;
    uint8_t x_val = 0;
    do
    {
        if (!(((uint8_t**)&pointer_array)[x_val] < insert_ptr))
            ((uint8_t**)&pointer_array)[x_val] += size_delta;
        x_val++;
    } while (x_val != sizeof(pointer_array) / sizeof(uint8_t*));
    size_t copy_len = (size_t)(copy_ptr - insert_ptr) + 1;
    memmove(insert_ptr + size_delta, insert_ptr, copy_len);
    return true;
}

/**
 * Handles memory-full condition.
 * Displays an error and returns to the editor loop.
 */
static void memory_full(void)
{
    show_memory_full_error();
    longjmp(env, JMP_EDITOR);
}

static const uint8_t la995_data[] = "Memory full - Press ESCAPE";

uint8_t process_current_document_character(
    uint8_t* target_ptr, uint8_t* x_val, uint8_t* y_val, bool* is_tab)
{
    uint8_t a_val = target_ptr[*y_val];
    (*y_val)++;
    uint8_t a_val1 = process_document_character(a_val, x_val, is_tab);
    return a_val1;
}

/**
 * Recalculates the cursor column.
 * Walks the edit line to map visual column to buffer position, handling tabs.
 */
static void recalculate_cursor_xpos(void)
{
    uint8_t x_val;
    uint8_t* line_ptr = &ram[RAM_EDIT_BUFFER];
    uint8_t a_val = line_change_pending_flag;
    bool is_tab = false;
    if (!(a_val != 0))
    {
        uint8_t y_val = a_val;
        do
        {
            if (y_val == xpos)
                goto ca63d;
            column_position = a_val;
            (void)process_current_document_character(
                line_ptr, &x_val, &y_val, &is_tab);
            a_val = x_val;
            a_val += column_position;
        } while (a_val >= column_position);
    }
    a_val = 0;
    line_change_pending_flag = a_val;
    uint8_t y_val1 = a_val;
    do
    {
        column_position = a_val;
        (void)process_current_document_character(
            line_ptr, &x_val, &y_val1, &is_tab);
        a_val = x_val;
        a_val += column_position;
    } while (a_val < visual_column);
    if (a_val != visual_column)
    {
        a_val = column_position;
        y_val1--;
    }
    xpos = y_val1;
ca63d:
    visual_column = a_val;
    return;
}

/**
 * Redraws the entire editor display.
 * Handles scrolling, line rendering, status, and cursor placement.
 */
void redraw_editor(void)
{
    uint8_t y_val1;
    uint8_t x_val5;
    uint8_t* draw;
    cursor_off();
    uint8_t a_val = ruler_index_ptr;
    saved_ruler_index_redraw = a_val;
    uint8_t a_val1 = status_line_needs_redrawing_flag;
    uint8_t saved_status_line_needs_redrawing_flag = a_val1;
    if (!(edit_buffer_unpacked_flag == 0))
    {
        uint8_t a_val3 = display_start_row;
        a_val3 |= scroll_repeat_count;
        if (a_val3 != 0)
            goto ca28e;
    }
    else
    ca28e:
    {
        if (current_line_ptr < top_of_screen_line_ptr)
            goto ca29c;
        else
            goto ca2dc;
    ca29c:
        if (scroll_repeat_count != 0)
            goto ca30d;
        ruler_index_ptr = saved_ruler_index_scroll;
        if (top_of_screen_line_ptr >= top)
            goto ca30d;
        {
            uint8_t* nav_ptr;
            find_previous_line(top_of_screen_line_ptr, &nav_ptr);
            scratch_line_ptr = nav_ptr;
        }
        if (scratch_line_ptr != current_line_ptr)
            goto ca30d;
        top_of_screen_line_ptr = scratch_line_ptr;
        uint8_t x_val = screen_maxrow;
        do
        {
            x_val--;
            uint8_t a_val6 = line_lengths[x_val];
            x_val++;
            line_lengths[x_val] = a_val6;
            x_val--;
        } while (x_val != 0);
        screen_scrolldown();
        screen_setcursor(0, 1);
        uint8_t y_val = 1;
        goto ca351;
    ca2dc:
        ruler_index_ptr = saved_ruler_index_scroll;
    ca2e0:
        uint8_t x_val1 = 0;
        uint8_t* walk = top_of_screen_line_ptr;
        do
        {
            x_val1++;
            if (walk == editor_ptr6)
                ptr6_screen_row = x_val1;
            if (walk == current_line_ptr)
                goto ca313;
            {
                uint8_t* nav_ptr;
                if (advance_to_next_line(walk, &nav_ptr, &y_val1))
                    goto ca313;
                scratch_line_ptr = nav_ptr;
            }
            scratch_line_ptr += y_val1;
            walk = scratch_line_ptr;
        } while (x_val1 <= screen_maxrow);
    ca30d:
        do
        {
            (void)compute_display_start_line();
            goto ca2e0;
        ca313:
            if (x_val1 <= screen_maxrow)
                goto ca35e;
        } while (scroll_repeat_count != 0);
        uint8_t x_val3 = 0;
        do
        {
            uint8_t a_val9 = line_lengths[x_val3 + 1];
            line_lengths[x_val3] = a_val9;
            x_val3++;
        } while (x_val3 != screen_maxrow);
        ptr6_screen_row--;
        line_lengths[0] = screen_maxcolumn;
        ruler_index_ptr = saved_ruler_index_scroll;
        {
            uint8_t* nav_ptr;
            advance_to_next_line(top_of_screen_line_ptr, &nav_ptr, &y_val1);
            scratch_line_ptr = nav_ptr;
        }
        top_of_screen_line_ptr += y_val1;
        screen_scrollup();
        screen_setcursor(0, screen_maxrow);
        y_val = screen_maxrow;
    ca351:
        saved_ruler_index_scroll = ruler_index_ptr;
        saved_status_line_needs_redrawing_flag++;
        line_counter++;
        x_val1 = y_val;
    ca35e:
        ypos = x_val1;
    }
        load_current_ruler(saved_ruler_index_redraw);
    unpack_line_into_buffer(edit_buffer_base);
    recalculate_cursor_xpos();
    uint8_t a_14 = screen_maxcolumn;
    a_14 >>= 1;
    screen_column = a_14;
    if (!(visual_column < hscroll_pos))
    {
        if ((int)visual_column <= (int)hscroll_pos + (int)screen_maxcolumn - 4)
            goto ca395;
    }
    uint8_t a_16 = visual_column;
    if (a_16 < screen_column)
        a_16 = 0;
    else
        a_16 -= screen_column;
    hscroll_pos = a_16;
    uint8_t a_17 = 1;
    display_start_row = a_17;
    saved_status_line_needs_redrawing_flag = a_17;
    write_line_back_to_document_safely();
ca395:
    uint8_t a_18 = saved_status_line_needs_redrawing_flag;
    status_line_needs_redrawing_flag = a_18;
    if (display_start_row == 0)
        goto ca3e7;
    if (!((int8_t)display_start_row >= 0 || (int8_t)ptr6_screen_row < 0))
    {
        screen_row = ptr6_screen_row;
        uint8_t a_21 = screen_maxrow;
        a_21 -= ptr6_screen_row;
        x_val5 = a_21;
        x_val5++;
        draw = editor_ptr6;
        if (editor_ptr6 != NULL)
            goto ca3c1;
    }
    load_current_ruler(saved_ruler_index_scroll);
    screen_row = 1;
    draw = top_of_screen_line_ptr;
    x_val5 = screen_maxrow;
ca3c1:
    scratch_index = x_val5;
    do
    {
        struct render_state rs = {.line = screen_row};
        draw_line(&rs, draw);
        uint8_t y_val2 = 0;
        {
            uint8_t* nav_ptr = scratch_line_ptr;
            if (advance_to_next_line(nav_ptr, &nav_ptr, &y_val2))
                goto ca422;
            scratch_line_ptr = nav_ptr;
        }
        scratch_line_ptr += y_val2;
        draw = scratch_line_ptr;
        screen_row++;
        scratch_index--;
    } while (scratch_index != 0);
ca3de:
    do
    {
        line_counter = 0;
        load_current_ruler(saved_ruler_index_redraw);
    ca3e7:
        unpack_line_into_buffer(edit_buffer_base);
        update_markers_to_format_buffer();
        draw_ruler();
        if (line_counter != 0)
        {
            screen_row = ypos;
            struct render_state rs_1 = {.line = screen_row};
            draw_line(&rs_1, current_format_line_ptr);
        }
        if (flags_need_redrawing_flag != 0)
            draw_status_word();
        uint8_t a_27 = visual_column;
        a_27 -= hscroll_pos;
        a_27 += 3;
        display_start_row = 0;
        line_counter = 0;
        scroll_repeat_count = 0;
        editor_ptr6 = &ram[RAM_MAX];
        screen_setcursor(a_27, ypos);
        cursor_on();
        return;
    ca422:
        scratch_index--;
    } while (scratch_index == 0);
    uint8_t a_28 = screen_maxcolumn + 1;
    line_lengths[screen_row + 1] = a_28;
    screen_column = a_28;
    uint8_t a_29 = 0x2a;
    do
    {
        screen_row++;
        screen_setcursor(0, screen_row);
        clear_to_eol(a_29, screen_row);
        line_lengths[screen_row] = screen_column;
        screen_column = 0;
        a_29 = 0x20;
        scratch_index--;
    } while (scratch_index != 0);
    goto ca3de;
}

/**
 * Renders a character with attributes.
 * Handles highlighting, control codes, and clipping to screen width.
 * @param rs render state
 */
static void render_char(struct render_state* rs)
{
    uint8_t char_to_render = rs->ch;
    uint8_t x_val = rs->line;
    if (line_lengths[x_val] != 0)
        line_lengths[x_val]--;
    uint8_t x_val1 = rs->col;
    if (x_val1 >= screen_maxcolumn)
    {
        rs->ch = char_to_render;
        return;
    }
    rs->col++;
    uint8_t a_val = rs->pos;
    if (!(a_val == 0))
    {
        uint8_t* size_delta = rs->line_ptr;
        x_val1 = find_marker_at_position(rs->pos - 1, size_delta);
        if (x_val1 >= 4)
            goto ca514;
        x_val1 = 0;
        if (x_val1 & 0x80)
        {
            rs->ch = char_to_render;
            goto ca523;
        }
        if (x_val1 != 0)
            goto ca514;
        screen_setstyle(STYLE_REVERSE);
    }
ca514:
    a_val = char_to_render;
    control_code_t f = check_for_control_code(a_val);
    if (f != NO_CONTROL_CODE)
    {
        if (f == HIGHLIGHT1_CODE)
            a_val = 0x2d;
        else
            a_val = 0x2a;
    }
ca523:
    if (a_val == 0x0d || a_val == 0x00)
        a_val = 0x20;
    screen_putchar(a_val);
    if (x_val1 == 0)
        screen_setstyle(0);
    rs->ch = char_to_render;
}

/**
 * Renders a character with horizontal scroll.
 * Accounts for hscroll before delegating to render_char.
 * @param rs render state
 */
static void render_xchar(struct render_state* rs)
{
    rs->char_width++;
    uint8_t x_val = rs->buf_off;
    rs->buf_off++;
    if (x_val < hscroll_pos)
        return;
    render_char(rs);
}

/**
 * Sanitises the defined area.
 * Ensures area pointers are ordered and checks for emptiness.
 * @return AREA_NOT_EMPTY or AREA_EMPTY
 */
area_status_t sanitise_area(void)
{
    if (area_start_ptr >= area_end_ptr)
    {
        uint8_t* tmp = area_start_ptr;
        area_start_ptr = area_end_ptr;
        area_end_ptr = tmp;
    }
    ptrdiff_t size_delta = area_end_ptr - area_start_ptr;
    if (size_delta != 0)
        return AREA_NOT_EMPTY;
    return AREA_EMPTY;
}

/**
 * Sets a marker.
 * Stores the current position in the given marker and marks display for update.
 * @param x_val marker index
 */
static void set_marker(uint8_t x_val)
{
    set_marker_to_here(x_val);
    display_start_row = 1;
    update_line_length();
    return;
}

void go_to_marker(uint8_t x_val);

/**
 * Common handler for setting numbered markers.
 * Writes the buffer, looks up the marker index, and sets it.
 * @param a_val marker character
 */
static void set_marker_common(uint8_t a_val)
{
    write_line_back_to_document_safely();
    int idx = lookup_marker(a_val);
    set_marker(idx);
    return;
}

/**
 * Displays a memory-full error message.
 * Shows an inverted message and waits for Escape.
 */
void show_memory_full_error(void)
{
    uint8_t a_val5;
    cursor_off();
    screen_setcursor(3, 0);
    screen_setstyle(STYLE_REVERSE);
    uint8_t y_val = screen_maxcolumn;
    line_lengths[0] = y_val;
    y_val--;
    y_val--;
    uint8_t x_val = 0;
    for (;;)
    {
        uint8_t a_val1 = la995_data[x_val];
        if (a_val1 == 0)
            break;
        x_val++;
        y_val--;
        if (y_val == 0)
            break;
        screen_putchar(a_val1);
    }
    screen_setstyle(0);
    if (!(y_val == 0))
    {
        do
        {
            screen_putchar(0x20);
            y_val--;
        } while (y_val != 0);
    }
    edit_buffer_unpacked_flag = 0;
    clear_cmd();
    do
    {
        beep();
        a_val5 = screen_getchar();
    } while (a_val5 != 0x1b);
    cursor_on();
    status_line_needs_redrawing_flag = 1;
    display_start_row = 1;
}

/**
 * Adjusts area pointers after an edit.
 * Applies heap adjustment and re-wraps lines at the area start.
 * @param size_delta size change
 */
void adjust_area_pointers(ptrdiff_t size_delta)
{
    uint8_t* insert_ptr = area_start_ptr;
    scratch_scan_ptr = adjust_pointers(insert_ptr, size_delta);
    split_line_at_wrap(insert_ptr);
    return;
}

/**
 * Appends a byte to the output buffer.
 * Writes the byte if space remains in the editor output buffer.
 * @param a_val byte to append
 */
static void append_to_output_buffer(uint8_t a_val)
{
    if (editor_output_pos >= MAX_LINE_LENGTH)
        return;
    output_buffer[editor_output_pos] = a_val;
    editor_output_pos++;
}

/**
 * Converts to uppercase unless folding is enabled.
 * Returns the character uppercased when folding is off.
 * @param a_val input character
 * @return possibly uppercased character
 */
uint8_t upper_case_unless_folding(uint8_t a_val)
{
    if (folding_flag & 0x80)
        return a_val;
    return toupper(a_val);
}

static bool process_char_for_output(
    uint8_t y_val, bool carry_in, uint8_t* x_val, uint8_t* a_val)
{
    screen_column = (screen_column >> 1) | (carry_in ? 0x80 : 0);
    (*a_val) = ram[RAM_EDIT_BUFFER + y_val];
    output_buffer[y_val] = (*a_val);
    if (!((*a_val) != 9))
    {
        {
            bool is_tab = false;
            (*a_val) = process_document_character((*a_val), x_val, &is_tab);
        }
        (*a_val) = *x_val;
        (*a_val) += column_position;
        if ((*a_val) != 0)
            goto c995c;
    }
    if (!((*a_val) != 0x0b))
    {
        (*a_val) = ruler_left_stop;
        if (!((*a_val) == 0))
        {
            *x_val = column_position;
            if (*x_val != 0)
            {
                if (*x_val >= ruler_left_stop)
                {
                    (*x_val)++;
                    (*a_val) = *x_val;
                }
            }
        c995c:
            column_position = (*a_val);
            justify_overflow_counter = y_val;
            justify_overflow_counter++;
            (*a_val) = 0;
            justify_gap_count = (*a_val);
            return true;
        }
    c9967:
        (*a_val) = 0x20;
    }
    if ((*a_val) < 0x1b)
        goto c9967;
    if ((*a_val) < 0x20)
        return false;
    column_position++;
    return column_position == 0;
}

/**
 * Formats the current paragraph.
 * Processes the line at current_line_ptr, handling margins, tabs, and word
 * wrap.
 * @return FORMAT_OK, FORMAT_AT_END, or FORMAT_MEMORY_FULL
 */
format_result_t format_paragraph(void)
{
    uint8_t a_val5;
    uint8_t a_val10;
    uint8_t a_15;
    uint8_t x_val;
    cursor_moved_flag++;
    print_xpos = 4;
    uint8_t y_val = 0;
    input_buffer_offset = y_val;
    line_format_status = y_val;
    uint8_t a_val = current_line_ptr[y_val];
    command_prefix_t cp = check_for_command_prefix(a_val);
    if (cp != NO_COMMAND_PREFIX)
        return advance_to_next_doc_line() ? FORMAT_AT_END : FORMAT_OK;
c998a:
    uint8_t a_val1 = format_mode_flag;
    a_val1 &= 0x81;
    if (a_val1 != 0)
        return advance_to_next_doc_line() ? FORMAT_AT_END : FORMAT_OK;
    if (ruler_right_stop == 0)
        return advance_to_next_doc_line() ? FORMAT_AT_END : FORMAT_OK;
    if (ruler_right_stop < ruler_left_stop)
        return advance_to_next_doc_line() ? FORMAT_AT_END : FORMAT_OK;
    uint8_t a_val3 = ruler_right_stop - ruler_left_stop + 2;
    scratch_offset = a_val3;
    wipe_buffer(0x10, edit_buffer_base);
    uint8_t* size_delta = current_line_ptr;
    uint8_t y_val1 = 0;
    uint8_t soft_hyphen_flag = 0;
    format_src_index = y_val1;
    column_position = y_val1;
    soft_hyphen_flag = y_val1;
    justify_gap_count = y_val1;
    bottom_margin = y_val1;
c99b6:
    editor_output_pos = y_val1;
    uint8_t y_val2 = format_src_index;
    do
    {
        uint8_t idx = find_marker_at_position(y_val2, size_delta);
        if (idx == 0x0c)
            break;
        markers_array[idx / 2] = 0;
        line_format_status++;
    } while (line_format_status != 0);
c99c9:
    do
    {
        a_val5 = current_line_ptr[y_val2];
        y_val2++;
        format_src_index = y_val2;
        if (!(a_val5 != 9))
        {
            {
                bool is_tab = false;
                (void)process_document_character(a_val5, &x_val, &is_tab);
            }
            x_val--;
            uint8_t a_val7 = x_val;
            a_val7 += column_position;
            column_position = a_val7;
            a_val5 = 9;
            goto c9a21;
        }
        if (a_val5 != 0x1a)
            goto c99ee;
        do
        {
            if (justify_gap_count != 0)
                goto c99c9;
            a_val5 = 0x20;
            goto c9a2e;
        c99ee:
            if (a_val5 != 0x0b)
                goto c9a11;
            x_val = input_buffer_offset;
        } while (x_val != 0 || soft_hyphen_flag != 0);
        soft_hyphen_flag++;
        a_val10 = ruler_left_stop;
    } while (a_val10 == 0);
    if (column_position < ruler_left_stop)
    {
        column_position = a_val10;
        column_position--;
    }
    a_val10 += scratch_offset;
    scratch_offset = a_val10;
    a_val5 = 0x0b;
c9a11:
    if (!(a_val5 != 0x0d))
    {
        y_val2--;
        if (y_val2 == 0)
            return advance_to_next_doc_line() ? FORMAT_AT_END : FORMAT_OK;
        if (find_next_word_boundary(y_val2))
            goto c9a87;
        a_val5 = 0x20;
        input_buffer_offset = a_val5;
    }
c9a21:
    y_val1 = editor_output_pos;
    x_val = 0;
    if (!(a_val5 != 0x20))
    {
        x_val++;
        if ((justify_gap_count & 0x80))
            goto c9a40;
    }
c9a2e:
    y_val1 = editor_output_pos;
    ram[RAM_EDIT_BUFFER + y_val1] = a_val5;
    if (a_val5 == 0x20)
        bottom_margin = (uint8_t)(bottom_margin >> 1) | 0x80;
    y_val1++;
    control_code_t cc = check_for_control_code(a_val5);
    if (cc == NO_CONTROL_CODE)
        column_position++;
c9a40:
    bool old_l0046_high = (justify_gap_count & 0x80) != 0;
    justify_gap_count = x_val;
    if (!(old_l0046_high || a_val5 == 0x20))
    {
        if (y_val1 >= MAX_LINE_LENGTH + 1)
            goto c9a60;
        if (bottom_margin == 0)
            goto c9a58;
        if (column_position >= scratch_offset)
            goto c9a60;
    }
c9a58:
    if (y_val1 >= 0x86)
        y_val1--;
    goto c99b6;
c9a60:
    format_src_index++;
    do
    {
        format_src_index--;
        y_val1--;
        if (y_val1 == 0)
            return advance_to_next_doc_line() ? FORMAT_AT_END : FORMAT_OK;
        uint8_t a_val13 = ram[RAM_EDIT_BUFFER + y_val1];
        {
            ram[RAM_EDIT_BUFFER + y_val1] = 0x10;
            a_15 = a_val13;
        }
    } while (a_15 != 0x20);
    insert_at_left_margin();
    justify_edit_buffer(edit_buffer_base);
    if (flush_formatted_line())
        return FORMAT_MEMORY_FULL;
    _Bool at_end_1;
    at_end_1 = advance_to_next_doc_line();
    if (!(line_format_status == 0))
    {
        goto c998a;
    c9a87:
        insert_at_left_margin();
        if (flush_formatted_line())
            return FORMAT_MEMORY_FULL;
        at_end_1 = advance_to_next_doc_line();
        goto c9aa5;
    }
c9aa5:
    return at_end_1 ? FORMAT_AT_END : FORMAT_OK;
}

/**
 * Finds the next word boundary for wrapping.
 * Scans forward to locate a suitable wrap point.
 * @param y_val current source index
 * @return true if wrap is needed
 */
static bool find_next_word_boundary(uint8_t y_val)
{
    uint8_t a_val = y_val;
    uint8_t* scan_ptr = current_line_ptr + a_val + 1;
    uint8_t* insert_ptr = scan_ptr;
    y_val = 0;
    screen_column = y_val;
    do
    {
        uint8_t a_val1 = insert_ptr[y_val];
        if (a_val1 == 0)
            goto c9b2f;
        command_prefix_t cp = check_for_command_prefix(a_val1);
        if (cp != NO_COMMAND_PREFIX || a_val1 == 0x0d)
            goto c9b2f;
        if (!(y_val != 0))
        {
            temp_save = y_val;
            goto c9aef;
        c9ae9:
            scan_ptr++;
        c9aef:
            uint8_t a_val3 = scan_ptr[y_val];
            if (a_val3 == 0 || a_val3 == 0x0d)
                goto c9b06;
            if (a_val3 == 9)
                goto c9b2f;
            if (a_val3 == 0x0b)
            {
                temp_save |= 0x80;
                if (a_val3 >= 0x0b)
                    goto c9ae9;
            }
        }
    c9b06:
        uint8_t a_val4 = insert_ptr[y_val];
        if (!(a_val4 != 0x20))
        {
            if (ruler_left_stop == 0 || temp_save == 0 || screen_column != 0)
                goto c9b2f;
        }
        else
        {
            if (a_val4 != 0x0b)
                break;
            screen_column = a_val4;
        }
        y_val++;
    } while (y_val != 0);
    if (!(ruler_left_stop == 0 || temp_save == 0 || screen_column != 0))
    {
    c9b2f:
        return true;
    }
    return false;
}

/**
 * Inserts a character into the edit buffer.
 * Makes room and stores the character at the cursor.
 * @param a_val character to insert
 * @return true on success
 */
static bool insert_character_into_edit_buffer(uint8_t a_val)
{
    _Bool ok;
    {
        ok = insert_edit_buffer_bytes_at_xpos(1);
        a_val = a_val;
    }
    if (!ok)
        return false;
    ram[RAM_EDIT_BUFFER + xpos] = a_val;
    line_counter++;
    return true;
}

/**
 * Sets the cursor to the end of the line.
 * Updates xpos to the current line length.
 */
static void set_xpos_to_line_length(void)
{
    xpos = get_line_length();
}

/**
 * Computes the top display line.
 * Walks backward from current_line_ptr to find the top-of-screen line.
 * @return display start adjustment
 */
static uint8_t compute_display_start_line(void)
{
    ruler_index_ptr = saved_ruler_index_redraw;
    uint8_t a_val1 = screen_maxrow;
    display_start_row = a_val1;
    a_val1 >>= 1;
    uint8_t x_val = a_val1;
    x_val++;
    if (!(scroll_repeat_count & 0x80))
    {
        if (scroll_repeat_count != 0)
            x_val = ypos;
    }
    uint8_t* line = current_line_ptr;
    for (;;)
    {
        x_val--;
        if (x_val == 0)
            break;
        uint8_t* line_ptr;
        if (!find_previous_line(line, &line_ptr))
            break;
        line = line_ptr;
        continue;
    }
    top_of_screen_line_ptr = line;
    saved_ruler_index_scroll = ruler_index_ptr;
    ruler_index_ptr = saved_ruler_index_redraw;
    return x_val;
}

/**
 * Advances the render state to the next character.
 * Reads and processes the next document character into the render state.
 * @param rs render state
 */
static void advance_to_next_char(struct render_state* rs)
{
    uint8_t y_val = rs->pos;
    column_position = rs->char_width;
    uint8_t a_val;
    uint8_t x_val;
    a_val = process_current_document_character(
        rs->line_ptr, &x_val, &y_val, &rs->prev_is_tab);
    rs->ch = a_val;
    rs->pos = y_val;
    rs->width = x_val;
    rs->char_width = column_position;
}

/**
 * Advances and renders the next character.
 * Combines character advance with rendering.
 * @param rs render state
 */
static void advance_to_next_char_and_render(struct render_state* rs)
{
    advance_to_next_char(rs);
    render_char(rs);
}

/**
 * Finds a marker at a buffer position.
 * Checks if any marker points at the given edit-buffer offset.
 * @param y_val buffer position
 * @param target_ptr base pointer
 * @return marker index or 0x0c if none
 */
static uint8_t find_marker_at_position(uint8_t y_val, uint8_t* target_ptr)
{
    uint8_t* scan_ptr = target_ptr + y_val;
    uint8_t x_val = 0;
    do
    {
        if (!(scan_ptr != markers_array[x_val / 2]))
            goto ca558;
        x_val++;
        x_val++;
    } while (x_val != 0x0c);
    return 0x0c;
ca558:
    return x_val;
}

/**
 * Unpacks a document line into the edit buffer.
 * Fills the buffer, handles command prefixes, and copies line content.
 * @param target_ptr destination buffer
 */
static void unpack_line(uint8_t* target_ptr)
{
    wipe_buffer(0x10, target_ptr);
    clear_format_mode_bit7();
    uint8_t a_val = *current_line_ptr;
    command_prefix_t cp = check_for_command_prefix(a_val);
    if (cp != NO_COMMAND_PREFIX)
    {
        if (cp == RULER_PREFIX)
            edit_buffer_unpacked_flag = a_val;
        set_format_mode_bit7();
    }
    current_format_line_ptr =
        (cp != NO_COMMAND_PREFIX) ? target_ptr : &ram[RAM_EDIT_BUFFER];
    uint8_t y_val1 = 0;
    do
    {
        uint8_t a2 = current_line_ptr[y_val1];
        if (a2 == 0x0d)
            break;
        current_format_line_ptr[y_val1] = a2;
        y_val1++;
    } while (y_val1 != 0);
    edit_line_len = y_val1;
}

/**
 * Updates markers to point into the format buffer.
 * Retargets markers from document heap into the current format line.
 */
static void update_markers_to_format_buffer(void)
{
    uint8_t* size_delta = current_line_ptr;
    uint8_t y_val = 0;
    do
    {
        uint8_t idx = find_marker_at_position(y_val, size_delta);
        if (!(idx == 0x0c))
        {
            {
                uint16_t val = (current_format_line_ptr - &ram[0]) + y_val;
                markers_array[idx / 2] = &ram[val];
                if (val != 0)
                    continue;
            }
        }
        uint8_t a_val = current_line_ptr[y_val];
        if (a_val == 0x0d)
            return;
        y_val++;
    } while (y_val != 0);
}

/**
 * Checks for an embedded ruler.
 * Pushes the ruler stack if the line starts with a ruler byte.
 * @param target_ptr line pointer
 */
void check_for_embedded_ruler(uint8_t* target_ptr)
{
    if (*target_ptr == RULER_BYTE)
        push_onto_ruler_index(target_ptr);
    return;
}

/**
 * Finds the start of the current line.
 * Scans backward for the preceding CR.
 * @param target_ptr pointer within line
 * @return pointer to line start
 */
static uint8_t* find_line_start(uint8_t* target_ptr)
{
    while (1)
    {
        target_ptr--;
        uint8_t a_val = target_ptr[0];
        if (a_val == 0x0d)
            break;
    }
    return target_ptr;
}

/**
 * find_left_margin_stop: Finds the left margin stop (0x0b) in the edit line.
 *
 * @return the position after the 0x0b in the edit line, or -1 if no margin
 *         stop was found.
 */
static int find_left_margin_stop(void)
{
    uint8_t y_val = 0;
    if (ruler_left_stop != 0)
    {
        do
        {
            uint8_t a_val1 = ram[RAM_EDIT_BUFFER + y_val];
            y_val++;
            if (a_val1 == 0x0b)
                return y_val;
        } while (y_val < MAX_LINE_LENGTH);
        return -1;
    }
    return y_val;
}

/**
 * Inserts at the left margin if needed.
 * Ensures a margin tab exists by inserting one at column zero.
 */
static void insert_at_left_margin(void)
{
    if (find_left_margin_stop() < 0)
        insert_byte_at_xpos(0);
    return;
}

/**
 * Inserts a margin byte at a position.
 * Inserts a single margin tab at the given position.
 * @param y_val position
 * @return true on success
 */
static bool insert_byte_at_xpos(uint8_t y_val)
{
    _Bool ok;
    uint8_t a_val = xpos;
    xpos = y_val;
    ok = insert_edit_buffer_bytes_at_xpos(1);
    if (ok)
        ram[RAM_EDIT_BUFFER + xpos] = 0x0b;
    xpos = a_val;
    return ok;
}

/**
 * Unpacks the current line into a buffer if needed.
 * Checks the unpacked flag before delegating to unpack_line.
 * @param target_ptr destination buffer
 */
static void unpack_line_into_buffer(uint8_t* target_ptr)
{
    if (edit_buffer_unpacked_flag != 0)
        return;
    edit_buffer_unpacked_flag = 1;
    unpack_line(target_ptr);
}

/**
 * Fills a buffer with a constant.
 * Writes the given value across the buffer length.
 * @param a_val fill value
 * @param target_ptr destination
 */
void wipe_buffer(uint8_t a_val, uint8_t* target_ptr)
{
    uint8_t y_val = 0;
    uint8_t x_val = 0x89;
    do
    {
        target_ptr[y_val] = a_val;
        y_val++;
        x_val--;
    } while (x_val != 0);
}

/**
 * Writes the edit buffer back to the document.
 * Computes size delta, adjusts heap, and copies the line including marker
 * updates.
 * @return true if write failed due to memory
 */
static bool write_line_back_to_document(void)
{
    uint8_t a_val6;
    uint8_t a_val9;
    if (!(edit_buffer_unpacked_flag == 0))
    {
        uint8_t* insert_ptr = current_line_ptr;
        area_size = 0;
        screen_column = get_line_length();
        uint8_t a_val1 = edit_line_len;
        {
            uint8_t minuend = a_val1;
            a_val1 -= screen_column;
            if (minuend < screen_column)
                goto ca8df;
            if (a_val1 == 0)
                goto ca8ed;
        }
        area_size = a_val1;
        scratch_scan_ptr = adjust_pointers(insert_ptr, area_size);
        goto ca8ed;
    ca8df:
        temp_save = a_val1;
        uint8_t a_val2 = 0;
        a_val2 -= temp_save;
        area_size = a_val2;
        if (!make_space_for_insertion(insert_ptr, area_size))
            return true;
    ca8ed:
        if (((int8_t)edit_buffer_unpacked_flag < 0))
        {
            if (edit_buffer_dirty_flag != 0)
                clamp_ptr6_to_document();
        }
        uint8_t y_val1 = 0;
        edit_buffer_dirty_flag = y_val1;
        edit_buffer_unpacked_flag = y_val1;
        area_size = current_format_line_ptr - &ram[0];
        uint8_t x_val = screen_column;
        edit_line_len = x_val;
        do
        {
            if (!(x_val != 0))
            {
                a_val6 = 0x0d;
            }
            else
            {
                a_val6 = current_format_line_ptr[y_val1];
                if (a_val6 == 0x10)
                    a_val6 = 0x20;
            }
            {
                uint16_t val;
                do
                {
                    uint8_t idx =
                        find_marker_at_position(y_val1, &ram[area_size]);
                    if (idx == 0x0c)
                        break;
                    val = (current_line_ptr - &ram[0]) + y_val1;
                    markers_array[idx / 2] = &ram[val];
                } while (val != 0);
                a_val9 = a_val6;
            }
            current_line_ptr[y_val1] = a_val9;
            y_val1++;
            x_val--;
        } while (a_val9 != 0x0d);
    }
    return false;
}

/**
 * Safely writes the edit buffer back.
 * Writes the buffer and invokes memory-full handling on failure.
 */
void write_line_back_to_document_safely(void)
{
    if (!write_line_back_to_document())
        return;
    memory_full();
}
