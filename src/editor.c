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
static void clear_to_eol(uint8_t fill_char, uint8_t line);
static void cursor_off(void);
static void cursor_on(void);
void draw_line(struct render_state* rs, uint8_t* addr);
void draw_prompt_characters(uint8_t first_char, uint8_t second_char);
static void draw_ruler(void);
static void draw_status_word(void);
static uint8_t get_line_length(void);
static void go_to_marker(uint8_t marker_idx);
static void go_to_marker_n(uint8_t marker);
static void home_cursor(void);
uint8_t justify_edit_buffer(uint8_t* target_ptr);
bool make_space_for_insertion(uint8_t* insert_ptr, ptrdiff_t size_delta);
static void memory_full(void);
uint8_t process_current_document_character(uint8_t* target_ptr,
    uint8_t* char_width_out,
    uint8_t* pos_inout,
    bool* is_tab);
static void recalculate_cursor_xpos(void);
void redraw_editor(void);
static void render_char(struct render_state* rs);
static void advance_to_next_char(struct render_state* rs);
static void render_xchar(struct render_state* rs);
area_status_t sanitise_area(void);
static void set_marker(uint8_t marker_idx);
static void set_marker_common(uint8_t marker_char);
void show_memory_full_error(void);
void adjust_area_pointers(ptrdiff_t size_delta);
static void append_to_output_buffer(uint8_t byte_to_append);
uint8_t upper_case_unless_folding(uint8_t ch);
static bool process_char_for_output(
    uint8_t buf_idx, bool carry_in, uint8_t* char_width_out, uint8_t* out_char);
format_result_t format_paragraph(void);
static bool find_next_word_boundary(uint8_t src_idx);
static bool insert_character_into_edit_buffer(uint8_t ch);
static void set_xpos_to_line_length(void);
static uint8_t compute_display_start_line(void);
static void advance_to_next_char_and_render(struct render_state* rs);
static uint8_t find_marker_at_position(uint8_t buf_offset, uint8_t* target_ptr);
static void unpack_line(uint8_t* target_ptr);
static void update_markers_to_format_buffer(void);
void check_for_embedded_ruler(uint8_t* target_ptr);
static uint8_t* find_line_start(uint8_t* target_ptr);
static int find_left_margin_stop(void);
static void insert_at_left_margin(void);
static bool insert_byte_at_xpos(uint8_t insert_pos);
static void unpack_line_into_buffer(uint8_t* target_ptr);
void wipe_buffer(uint8_t fill_value, uint8_t* target_ptr);
static bool write_line_back_to_document(void);
void write_line_back_to_document_safely(void);

void enter_editor_mode(void);
void clear_format_mode_bit7(void);
void set_format_mode_bit7(void);
void draw_previous_word(
    uint8_t* word_boundary, bool* is_start_of_line, uint8_t* char_width);
bool adjust_margins_at_left_margin(void);
bool insert_edit_buffer_bytes_at_xpos(uint8_t count);
void set_marker_to_here(uint8_t marker_idx);
void split_line_at_wrap(uint8_t* target_ptr);

// Editor-internal helper functions

static void advance_current_line_pointer(void);

static void clear_marks_1_2(void);

static uint8_t control_key_to_ascii(uint8_t key_code);

static void delete_edit_buffer_bytes_at_xpos(uint8_t delete_count);

static void enter_printable_character(void);

static int prompt_for_marker(void);

static bool reset_area_to_marks_1_2(void);

static void insert_line_at_cursor(uint8_t* target_ptr);

static void move_to_previous_line(void);

static void move_cursor_up(uint8_t lines_to_move);

static void move_cursor_down(uint8_t lines_to_move);

static void check_pointer_in_area(void);

static void tab_highlight_common(uint8_t char_to_insert);

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
    uint8_t tmp_flags;

    screen_enter();

    for (;;)
    {
    editor_loop:
    {
        uint8_t saved_fmt = format_mode_flag;
        uint8_t unpacked_flag = edit_buffer_unpacked_flag;

        if (unpacked_flag == 0)
        {
            uint8_t unpacked_flag_copy;
            {
                unpack_line(edit_buffer_base);
                unpacked_flag_copy = unpacked_flag;
            }
            edit_buffer_unpacked_flag = unpacked_flag_copy;
        }
        recalculate_cursor_xpos();

        if (!(ruler_left_stop == 0 || format_mode_flag & 0x80 ||
                ruler_left_stop <= visual_column))
        {
            if (!(cursor_moved_flag != 0))
            {
                uint8_t line_len = get_line_length();

                tmp_flags = format_mode_flag;

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
                uint8_t masked_flags = format_mode_flag;

                masked_flags &= 0xbf;
                {
                    int margin_pos = find_left_margin_stop();

                    tmp_flags = masked_flags;

                    if (margin_pos < 0 || margin_pos <= xpos)
                        goto c9b86_;
                }
                tmp_flags |= 0x40;
            }
            c9b84_:
                format_mode_flag = tmp_flags;
        }
    c9b86_:
        if (saved_fmt != format_mode_flag)
            flags_need_redrawing_flag++;
        cursor_moved_flag = 0;
        redraw_editor();
    }
        uint8_t key_code = screen_getchar();

        if (key_code == current_tab_key)
            key_code = 9;
        editor_current_key = key_code;

        if (!(key_code < 0x20))
        {
            if (key_code < 0x7f)
            {
                enter_printable_character();

                goto editor_loop;
            }
        }
        switch (key_code)
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
    uint8_t new_flags = format_mode_flag;

    new_flags &= 0xbf;

    if (format_mode_flag & 0x40)
        new_flags |= 1;
    new_flags ^= 1;
    format_mode_flag = new_flags;
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
    uint8_t split_pos = line_len;

    if (line_len >= xpos)
        split_pos = xpos;
    line_change_pending_flag++;
    uint8_t insert_offset = split_pos;
    uint8_t first_char = current_format_line_ptr[0];

    command_prefix_t cp = check_for_command_prefix(first_char);

    if (cp != NO_COMMAND_PREFIX)
    {
        insert_offset++;
        insert_offset++;
        insert_offset++;
    }
    uint32_t sum = (uint32_t)(current_line_ptr - &ram[0]) + insert_offset;

    if (sum > 0xffff)
    {
        f6_insert_line_key();

        return;
    }
    insert_line_into_document(&ram[(uint16_t)sum]);
}

/**
 * Joins the current line with the next line.
 * Removes the line break between current and next line if neither is a command.
 */
static void cf7_join_lines_key(void)
{
    write_line_back_to_document_safely();
    uint8_t* line_ptr;
    uint8_t next_line_len;

    if (find_next_line(current_line_ptr, &line_ptr, &next_line_len))
    {
        beep();

        return;
    }
    command_prefix_t cp = check_for_command_prefix(line_ptr[next_line_len]);

    if (cp != NO_COMMAND_PREFIX)
    {
        beep();

        return;
    }
    next_line_len--;
    uint8_t* insert_ptr = current_line_ptr + next_line_len;

    scratch_scan_ptr = adjust_pointers(insert_ptr, 1);
    split_line_at_wrap(current_line_ptr);
    line_change_pending_flag++;
    clamp_ptr6_to_document();
}

/**
 * Marks the current line as a ruler.
 * Writes a ruler marker into the edit buffer and enables format mode.
 */
static void cf8_mark_as_ruler_key(void)
{
    current_format_line_ptr = edit_buffer_base;
    uint8_t buf_idx = 0;

    current_format_line_ptr[buf_idx] = 0x81;
    buf_idx++;
    current_format_line_ptr[buf_idx] = 0x2e;
    buf_idx++;
    current_format_line_ptr[buf_idx] = 0x2e;
    line_counter++;

    if (!(edit_buffer_unpacked_flag & 0x80))
    {
        edit_buffer_unpacked_flag = 0x80;
        edit_buffer_dirty_flag++;
    }
    set_format_mode_bit7();
}

/**
 * Handles the Delete key.
 * Deletes the character before the cursor and handles insert-mode and
 * auto-insert behavior.
 */
static void delete_key(void)
{
    uint8_t marker_check;

    if (visual_column == 0)
        return;
    xpos--;
    uint8_t char_at_cursor = ram[RAM_EDIT_BUFFER + xpos];
    {
        f9_delete_char_key();
        marker_check = char_at_cursor;
    }
    if (marker_check < 0x0c)
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
    uint8_t bytes_to_delete = MAX_LINE_LENGTH;

    bytes_to_delete -= xpos;
    line_counter++;
    delete_edit_buffer_bytes_at_xpos(bytes_to_delete);
}

/**
 * Moves the cursor to the beginning of the line.
 * Sets the cursor column to zero and marks cursor movement.
 */
static void f4_beginning_of_line_key(void)
{
    cursor_moved_flag++;
    xpos = 0;
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
    uint8_t bytes_to_remove = edit_line_len;

    bytes_to_remove++;
    ptrdiff_t size_delta = bytes_to_remove;

    scratch_scan_ptr = adjust_pointers(insert_ptr, size_delta);
    ensure_cr_at_document_top();
    uint8_t first_byte = current_line_ptr[0];

    if (first_byte == 0)
    {
        uint8_t* line_ptr;

        find_previous_line(current_line_ptr, &line_ptr);
        current_line_ptr = line_ptr;
    }
    line_change_pending_flag++;
    clamp_ptr6_to_document();
}

/**
 * Inserts a space at the cursor.
 * Inserts a single space character at the current position.
 */
static void f8_insert_char_key(void)
{
    insert_character_into_edit_buffer(0x20);
}

/**
 * Deletes the character under the cursor.
 * Removes one character at the cursor position.
 */
static void f9_delete_char_key(void)
{
    line_counter++;
    delete_edit_buffer_bytes_at_xpos(1);
}

/**
 * Handles the K-command prefix.
 * Prompts for a follow-up key and dispatches to marker or block operations.
 */
static void k_command_key(void)
{
    draw_prompt_characters('^', 'K');
    flags_need_redrawing_flag++;
    uint8_t key_code = screen_getchar();
    uint8_t ascii_code = control_key_to_ascii(key_code);

    switch (ascii_code)
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

    0};

/**
 * Handles the O-command prefix.
 * Prompts for a follow-up key and dispatches to formatting and ruler
 * operations.
 */
static void o_command_key(void)
{
    draw_prompt_characters('^', 'O');
    flags_need_redrawing_flag++;
    uint8_t key_code = screen_getchar();
    uint8_t ascii_code = control_key_to_ascii(key_code);

    switch (ascii_code)
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
    uint8_t key_code = screen_getchar();
    uint8_t ascii_code = control_key_to_ascii(key_code);

    switch (ascii_code)
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
    uint8_t next_line_len;

    if (!find_next_line(current_line_ptr, &line_ptr, &next_line_len))
    {
        advance_current_line_pointer();

        return;
    }
    uint16_t sum = (current_line_ptr - &ram[0]) + next_line_len;
    insert_line_at_cursor(&ram[sum]);
    advance_current_line_pointer();
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
    uint8_t remaining = ruler_buffer_len;

    if (!(remaining == 0))
    {
        uint8_t copy_idx = 0;

        do
        {
            uint8_t ruler_byte = current_ruler_ptr[copy_idx];

            ram[RAM_EDIT_BUFFER + copy_idx] = ruler_byte;
            copy_idx++;
            remaining--;
        } while (remaining != 0);
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
    uint8_t char_width;

    draw_previous_word(&word_boundary, &is_start_of_line, &char_width);

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
    uint8_t buf_pos;
    _Bool is_tab;

entry:
    uint8_t* line_ptr = &ram[RAM_EDIT_BUFFER];
    uint8_t line_len = get_line_length();

    if (!(line_len < xpos || line_len == xpos))
    {
        buf_pos = xpos;
    }
    else
    {
        uint8_t next_line_len;

        xpos = next_line_len;
        write_line_back_to_document_safely();

        if (advance_to_next_line(current_line_ptr, &line_ptr, &next_line_len))
            return;
        current_line_ptr += next_line_len;
        unpack_line(edit_buffer_base);
        scroll_repeat_count--;
        xpos = 0;

        if (get_line_length() == xpos)
            return;
        next_line_len = 0;
        is_tab = false;
        uint8_t cur_char;
        uint8_t char_width;

        cur_char = process_current_document_character(
            &ram[RAM_EDIT_BUFFER], &char_width, &next_line_len, &is_tab);

        if (cur_char != 0x20)
            return;

        goto entry;
    }
    for (;;)
    {
        if (buf_pos >= line_len)
            goto ca00f;
        uint8_t char_width;
        uint8_t cur_char2 = process_current_document_character(
            line_ptr, &char_width, &buf_pos, &is_tab);

        if (cur_char2 != 0x20)
            continue;
        break;
    }
    for (;;)
    {
        if (buf_pos >= line_len)
            goto ca00f;
        uint8_t char_width;
        uint8_t cur_char3 = process_current_document_character(
            line_ptr, &char_width, &buf_pos, &is_tab);

        if (cur_char3 == 0x20)
            continue;
        break;
    }
    buf_pos--;

ca00f:
    xpos = buf_pos;
}

static void set_marker(uint8_t marker_idx);

static void set_marker_common(uint8_t marker_char);

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
    uint8_t acc = ram[RAM_EDIT_BUFFER + xpos];

    if (!isalpha(acc))
    {
        f13_right_key();

        return;
    }
    line_counter++;
    acc ^= 0x20;
    ram[RAM_EDIT_BUFFER + xpos] = acc;
    f13_right_key();
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
    int margin_pos = find_left_margin_stop();

    if (margin_pos < 0)
    {
        f4_beginning_of_line_key();

        return;
    }
    xpos = (uint8_t)margin_pos;
}

/**
 * Deletes from the cursor to a specified character.
 * Prompts for a character and deletes up to and including its next occurrence.
 */
static void sf3_delete_to_char_key(void)
{
    draw_prompt_characters('C', 'H');
    flags_need_redrawing_flag++;
    uint8_t target_char = screen_getchar();

    if (target_char == 9 || target_char == 0xa0 || target_char == 0xa1)
    {
        if (target_char == 0xa0)
            target_char = 0x1c;
        else if (target_char == 0xa1)
            target_char = 0x1d;
    }
    else if (target_char < 0x20 || target_char >= 0x7f)
    {
        beep();

        return;
    }
    {
        line_counter++;
        uint8_t scan_pos = xpos;
        uint8_t start_x = scan_pos;
        bool found_match = false;

        while (scan_pos < MAX_LINE_LENGTH)
        {
            uint8_t scanned_char = ram[RAM_EDIT_BUFFER + scan_pos];

            scan_pos++;

            if (scanned_char == target_char)
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
        while (scan_pos < MAX_LINE_LENGTH)
        {
            uint8_t scanned_char2 = ram[RAM_EDIT_BUFFER + scan_pos];

            scan_pos++;

            if (scanned_char2 != target_char)
                break;
        }
        scan_pos--;
        uint8_t chars_to_delete = scan_pos - start_x;

        delete_edit_buffer_bytes_at_xpos(chars_to_delete);
    }
}

/**
 * Inserts highlight code 1.
 * Inserts the first highlight control character at the cursor.
 */
static void sf4_highlight1_key(void)
{
    tab_highlight_common(0x1c);
}

/**
 * Inserts highlight code 2.
 * Inserts the second highlight control character at the cursor.
 */
static void sf5_highlight2_key(void)
{
    tab_highlight_common(0x1d);
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
}

/**
 * Edits the formatting command on the current line.
 * Allows interactive editing of the command prefix characters.
 */
static void sf8_edit_command_key(void)
{
    uint8_t key_code;

    xpos = 0;
    redraw_editor();
    edit_buffer_dirty_flag++;
    scratch_offset = 0;
    scratch_index = 0;

edit_command_loop:
    do
    {
        screen_setcursor(scratch_offset, ypos);
        key_code = screen_getchar();

        if (key_code == 0x0d)
            goto finished_editing_command;
        key_code &= 0xdf;
    } while (key_code < 0x41 || key_code >= 0x5b);
    scratch_index = key_code;
    screen_putchar(key_code);
    uint8_t edit_pos = scratch_offset;

    edit_pos++;
    scratch_offset = edit_pos;
    edit_buffer_base[edit_pos] = key_code;

    if (edit_pos < 2)
        goto edit_command_loop;
    scratch_offset = 0;

    goto edit_command_loop;

finished_editing_command:
    if (scratch_index == 0)
        return;
    current_format_line_ptr = edit_buffer_base;
    current_format_line_ptr[0] = 0x80;
    set_format_mode_bit7();
}

/**
 * Deletes the formatting command prefix.
 * Removes the command prefix from the current line if present.
 */
static void sf9_delete_command_key(void)
{
    uint8_t pos = 0;
    uint8_t first_char = current_format_line_ptr[pos];

    command_prefix_t cp = check_for_command_prefix(first_char);

    if (cp == NO_COMMAND_PREFIX)
        return;
    current_format_line_ptr[pos] = pos;
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
    uint8_t line_len;

    if (advance_to_next_line(current_line_ptr, &line_ptr, &line_len))
        return;
    current_line_ptr += line_len;
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
 * @param acc key code
 * @return converted ASCII code
 */
static uint8_t control_key_to_ascii(uint8_t key_code)
{
    if (key_code < 0x20)
        key_code |= 0x40;

    return toupper(key_code);
}

/**
 * Deletes bytes at the cursor in the edit buffer.
 * Removes a given number of bytes at the cursor, shifts content left, and
 * updates markers.
 * @param idx number of bytes to delete
 */
static void delete_edit_buffer_bytes_at_xpos(uint8_t delete_count)
{
    scratch_offset = delete_count;
    edit_buffer_dirty_flag++;
    uint8_t* size_delta = &ram[RAM_EDIT_BUFFER];
    uint8_t scan_pos = xpos;
    uint8_t end_pos = scan_pos;

    end_pos += scratch_offset;

    do
    {
        delete_count = find_marker_at_position(scan_pos, size_delta);

        if (!(delete_count == 0x0c))
        {
            uint16_t marker_addr =
                (scan_pos >= temp_save)
                    ? RAM_EDIT_BUFFER + (scan_pos - scratch_offset)
                    : 0;
            markers_array[delete_count / 2] =
                marker_addr ? &ram[marker_addr] : NULL;
            continue;
        }
        scan_pos++;
    } while (scan_pos < MAX_LINE_LENGTH + 1);

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
}

/**
 * Inserts a printable character.
 * Handles insertion, margin checks, wrapping, and justification for a typed
 * character.
 */
static void enter_printable_character(void)
{
    uint8_t scanned_char;
    uint8_t filler;
    uint8_t cursor_pos = xpos;

    if (cursor_pos >= MAX_LINE_LENGTH)
        return;
    edit_buffer_dirty_flag++;

    if (adjust_margins_at_left_margin())
        return;
    uint8_t* size_delta = &ram[RAM_EDIT_BUFFER];
    uint8_t check_pos = xpos;
    uint8_t idx = find_marker_at_position(check_pos, size_delta);

    if (idx != 0x0c)
    {
        if (idx < 4)
            line_counter++;
    }
    uint8_t mode_flag = insert_mode_flag;

    if (!(mode_flag != 0))
    {
        uint8_t char_at_pos = ram[RAM_EDIT_BUFFER + check_pos];

        if (char_at_pos == 9)
            goto c9c00;

        if (char_at_pos != 0x0b)
            goto c9c09;
    }
c9c00:
    line_counter++;

    if (!insert_edit_buffer_bytes_at_xpos(1))
        return;

c9c09:
    uint8_t typed_char = editor_current_key;

    ram[RAM_EDIT_BUFFER + xpos] = typed_char;
    uint8_t copied_flag = line_counter;

    if (copied_flag == 0)
        screen_putchar(typed_char);
    xpos++;
    update_line_length();
    uint8_t scan_idx = 0;

    column_position = 0;

c9c1d:
    do
    {
        scanned_char = ram[RAM_EDIT_BUFFER + scan_idx];
        scan_idx++;

        if (scan_idx > xpos)
            goto c9c56;

        if (!(scanned_char != 9))
        {
            {
                bool is_tab = false;

                (void)process_document_character(
                    scanned_char, &mode_flag, &is_tab);
            }
            scanned_char = mode_flag;
            scanned_char += column_position;

            if (scanned_char != 0)
                goto c9c43;
        }
        if (!(scanned_char != 0x0b))
        {
            scanned_char = ruler_left_stop;

            if (!(scanned_char == 0))
            {
                mode_flag = column_position;

                if (mode_flag != 0)
                {
                    if (mode_flag >= ruler_left_stop)
                    {
                        mode_flag++;
                        scanned_char = mode_flag;
                    }
                }
            c9c43:
                column_position = scanned_char;
                continue;
            }
        c9c48:
            scanned_char = 0x20;
        }
        if (scanned_char < 0x1b)
            goto c9c48;
    } while (scanned_char < 0x20);
    column_position++;

    goto c9c1d;

c9c56:
    uint8_t col_pos = column_position;

    if (col_pos < ruler_buffer_len)
    {
        uint8_t ruler_char = current_ruler_ptr[col_pos];

        ruler_char &= 0xdf;

        if (ruler_char == 0x42)
            beep();
    }
    uint8_t typed_char2 = editor_current_key;

    if (typed_char2 == 0x20)
        return;

    if (ruler_right_stop == 0)
    {
        line_counter = 0;

        return_to_editor_loop();
    }
    if (format_mode_flag != 0)
        return;

    if (col_pos == 0)
        return;
    col_pos--;

    if (col_pos < ruler_right_stop)
        return;
    screen_column = get_line_length();
    top_margin = 0;
    uint8_t saved_xpos = xpos;

    input_buffer_offset = saved_xpos;
    uint8_t word_boundary;
    _Bool is_start_of_line;

    draw_previous_word(&word_boundary, &is_start_of_line, &mode_flag);
    recalculate_cursor_xpos();
    uint8_t vis_col = visual_column;

    if (vis_col == ruler_left_stop)
    {
        uint8_t tmp_pos = input_buffer_offset;

        tmp_pos--;
        xpos = tmp_pos;

        goto c9ca2;
    }
    if (vis_col < ruler_left_stop)
    {
        {
            uint8_t tmp_pos2 = input_buffer_offset;

            tmp_pos2--;
            xpos = tmp_pos2;
        }
    }
c9ca2:
    uint8_t wrap_len = input_buffer_offset;

    wrap_len -= xpos;
    top_margin = wrap_len;
    uint8_t col_remaining = screen_column;

    col_remaining -= xpos;
    screen_column = col_remaining;
    uint8_t remaining_len = col_remaining;

    remaining_len++;
    uint8_t left_stop = ruler_left_stop;

    if (left_stop != 0)
    {
        top_margin++;
        remaining_len++;
    }
    uint8_t* insert_ptr = current_line_ptr + edit_line_len + 1;

    if (!(make_space_for_insertion(insert_ptr, remaining_len)))
    {
        show_memory_full_error();
        longjmp(env, JMP_EDITOR);
    }
    uint8_t dst_idx = 0;

    if (ruler_left_stop != 0)
    {
        *insert_ptr = 0x0b;
        dst_idx = 1;
    }
    scratch_index = dst_idx;
    uint8_t* marker_base_ptr = &ram[RAM_EDIT_BUFFER];
    uint8_t prev_pos = xpos;

    prev_pos--;
    uint8_t prev_char = ram[RAM_EDIT_BUFFER + prev_pos];

    if (prev_char == 0x20)
        ram[RAM_EDIT_BUFFER + prev_pos] = 0x10;
    prev_pos++;
    screen_row = prev_pos;

    do
    {
        uint8_t src_pos = screen_row;

        screen_row++;

        for (;;)
        {
            uint8_t marker_index =
                find_marker_at_position(src_pos, marker_base_ptr);

            if (marker_index == 0x0c)
                break;
            {
                uint8_t* val_ptr = insert_ptr + scratch_index;

                markers_array[marker_index / 2] = val_ptr;

                if (insert_ptr - &ram[0] + scratch_index >= 0x10000)
                    break;
            }
        }
        uint8_t src_char = screen_column;

        if (!(src_char != 0))
        {
            filler = 0x0d;
        }
        else
        {
            uint8_t acc13 = ram[RAM_EDIT_BUFFER + src_pos];
            {
                uint8_t saved = acc13;

                ram[RAM_EDIT_BUFFER + src_pos] = 0x10;
                filler = saved;
            }
        }
        uint8_t dst_pos = scratch_index;

        scratch_index++;
        insert_ptr[dst_pos] = filler;
        screen_column--;
    } while (!(screen_column & 0x80));
    justify_edit_buffer(edit_buffer_base);
    write_line_back_to_document_safely();
    clamp_ptr6_to_document();

    return_key();
    xpos = top_margin;
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
    uint8_t key_code = screen_getchar();

    return lookup_marker(key_code);
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
            uint8_t marker_array_idx =
                ((uint8_t*)&doc_ptr1 - (uint8_t*)markers_array) / 2;
            set_marker_to_here(marker_array_idx);
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
 * @param idx number of lines to move
 */
static void move_cursor_up(uint8_t lines_to_move)
{
    cursor_moved_flag++;
    scratch_offset = lines_to_move;
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
        lines_to_move = scratch_offset;

        if ((int8_t)lines_to_move < 0)
            continue;
        scratch_offset--;
    } while (scratch_offset != 0);
    current_line_ptr = line;
}

/**
 * Moves the cursor down.
 * Writes the buffer back and moves down by the specified number of lines.
 * @param idx number of lines to move
 */
static void move_cursor_down(uint8_t lines_to_move)
{
    cursor_moved_flag++;
    scratch_offset = lines_to_move;
    write_line_back_to_document_safely();
    uint8_t* line = current_line_ptr;

    while (1)
    {
        uint8_t* line_ptr;
        uint8_t line_len;

        if (advance_to_next_line(line, &line_ptr, &line_len))
        {
            line = line_ptr;
            break;
        }
        line = line + line_len;
        lines_to_move = scratch_offset;

        if ((int8_t)lines_to_move < 0)
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
 * @param acc control character to insert
 */
static void tab_highlight_common(uint8_t char_to_insert)
{
    if (adjust_margins_at_left_margin())
        return;

    if (!insert_character_into_edit_buffer(char_to_insert))
        return;
    f13_right_key();
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
    uint8_t remaining_rows = screen_maxrow;

    do
    {
        line_lengths[remaining_rows] = 0;
        remaining_rows--;
    } while (!(remaining_rows & 0x80));
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
    uint8_t insert_pos;
    uint8_t flags_tmp = format_mode_flag;

    flags_tmp &= 0x81;

    if (!(flags_tmp != 0 || find_left_margin_stop() >= 0))
    {
        uint8_t line_len = get_line_length();

        screen_column = xpos;
        xpos = line_len;
        recalculate_cursor_xpos();

        if (!(visual_column < ruler_left_stop))
        {
            insert_pos = screen_column;
            xpos = insert_pos;
            xpos++;
        }
        else
        {
            uint8_t acc3 = screen_column;

            insert_pos = xpos;

            if (!(insert_pos >= screen_column))
            {
                if (screen_column < ruler_left_stop)
                    goto caf2a;
                acc3 = screen_column - ruler_left_stop + xpos + 1;
            }
            xpos = acc3;
        }
    caf2a:
        if (!insert_byte_at_xpos(insert_pos))
            return true;
        line_counter++;
    }
    return false;
}

/**
 * Inserts bytes at the cursor in the edit buffer.
 * Makes room at the cursor by shifting existing content right.
 * @param idx number of bytes to insert
 * @return true on success, false if overflow or out of space
 */
bool insert_edit_buffer_bytes_at_xpos(uint8_t count)
{
    if (xpos >= MAX_LINE_LENGTH)
    {
        beep();

        return false;
    }
    scratch_offset = count;
    uint8_t acc1 = get_line_length();

    acc1 += scratch_offset;

    if (acc1 < scratch_offset)
    {
        beep();

        return false;
    }
    if (acc1 >= MAX_LINE_LENGTH + 1)
    {
        beep();

        return false;
    }
    edit_buffer_dirty_flag++;
    uint8_t* size_delta = &ram[RAM_EDIT_BUFFER];
    uint8_t scan_pos = MAX_LINE_LENGTH;

cae27:
    scan_pos--;
    count = 0;
    uint8_t tail_len = scan_pos;

    tail_len += scratch_offset;

    if (tail_len >= scratch_offset)
    {
        if (tail_len < MAX_LINE_LENGTH)
            count = tail_len;
    }
    scratch_index = count;

    for (;;)
    {
        uint8_t idx = find_marker_at_position(scan_pos, size_delta);

        if (idx == 0x0c)
            goto cae52;
        uint16_t marker_addr =
            scratch_index ? RAM_EDIT_BUFFER + scratch_index : 0;
        markers_array[idx / 2] = marker_addr ? &ram[marker_addr] : NULL;
    }
cae52:
    if (scan_pos != xpos)
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
 * @param idx marker index
 */
void set_marker_to_here(uint8_t marker_idx)
{
    uint8_t tmp_pos;

    (void)tmp_pos;
    uint8_t len = get_line_length();

    if (len >= xpos)
    {
        uint8_t first_char = current_format_line_ptr[0];

        command_prefix_t cp = check_for_command_prefix(first_char);
        len = xpos;

        if (cp != NO_COMMAND_PREFIX)
            len += 3;
    }
    uint16_t marker_addr = (current_line_ptr - &ram[0]) + len;
    markers_array[marker_idx] = &ram[marker_addr];
}

/**
 * Splits a line at wrap position.
 * Inserts carriage returns at word boundaries to enforce line length limits.
 * @param target_ptr pointer within the line to split
 */
void split_line_at_wrap(uint8_t* target_ptr)
{
    uint8_t acc3;
    uint8_t* scan_ptr = find_line_start(target_ptr);

    do
    {
        screen_column = 0;
        uint8_t copy_len = MAX_LINE_LENGTH + 1;
        uint8_t scan_pos = 1;
        uint8_t scan_char = scan_ptr[scan_pos];

        command_prefix_t cp = check_for_command_prefix(scan_char);

        if (cp != NO_COMMAND_PREFIX)
        {
            copy_len++;
            copy_len++;
            copy_len++;
        }
        temp_save = copy_len;

        do
        {
            uint8_t next_char = scan_ptr[scan_pos];

            scan_pos++;

            if (!(next_char == 0x20))
            {
                if (next_char != 0x1a)
                    goto cac9c;
            }
            screen_column = scan_pos;

        cac9c:
            if (next_char == 0x0d)
                return;
        } while (scan_pos == temp_save || scan_pos < temp_save);

        if (screen_column == 0)
        {
            acc3 = temp_save;

            goto cacad;
        }
        acc3 = screen_column;

    cacad:
        uint8_t* insert_ptr = scan_ptr + acc3;

        scan_ptr = insert_ptr;
        make_space_for_insertion(insert_ptr, 1);
        insert_ptr[0] = 0x0d;
        scan_ptr = insert_ptr;
    } while (((uint8_t*)&scan_ptr)[1] != 0);
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
    uint8_t slot_idx = 0;

    do
    {
        {
            uint8_t* slot_ptr = ((uint8_t**)&pointer_array)[slot_idx];

            if (slot_ptr < insert_ptr)
                goto ca9f1;

            if (slot_ptr < local_tmp89)
                goto ca9db;

            goto ca9e7;
        }
    ca9db:
        if (!(slot_idx >= ARRAY_SIZE(markers_array)))
            ((uint8_t**)&pointer_array)[slot_idx] = NULL;
        else

        ca9e7:
        {
            ((uint8_t**)&pointer_array)[slot_idx] -= size_delta;
        }
        ca9f1:
            slot_idx++;
    } while (slot_idx != sizeof(pointer_array) / sizeof(uint8_t*));
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
    uint8_t line_len;
    bool end_of_document =
        find_next_line(current_line_ptr, &line_ptr, &line_len);

    if (!end_of_document)
        current_line_ptr = line_ptr + line_len;

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
    uint8_t idx = 0;

    editor_header_pos = 0;
    scratch_index = 0;
    uint8_t* scan_ptr = doc_ptr2;

c8b91:
    if (scan_ptr >= doc_ptr3)
        return false;
    uint8_t pos = 0;
    uint8_t acc2 = *scan_ptr;

    command_prefix_t cp = check_for_command_prefix(acc2);

    if (cp != NO_COMMAND_PREFIX)
    {
        doc_ptr2 = scan_ptr + 3;

        return scan_document_for_next_line();
    }
    screen_column = upper_case_unless_folding(acc2);

    do
    {
        pos++;
        uint8_t acc4 = scan_ptr[pos];

        if (acc4 == 0)
            goto c8bdb;
        command_prefix_t cp2 = check_for_command_prefix(acc4);

        if (cp2 != NO_COMMAND_PREFIX)
            goto c8bdb;
        uint8_t acc5 = header_text_maybe[idx];

        if (acc5 == 0x20)
            goto c8bf7;

        if (acc5 == 1)
            goto c8be3;

        if (acc5 == 2)
            acc5 = 0x20;

        if (acc5 == screen_column)
            goto c8c33;

    c8bdb:
        do
        {
            doc_ptr2++;

            return scan_document_for_next_line();

        c8be3:
            temp_save = idx;

            if (editor_header_pos < 0x14)
            {
                output_buffer[editor_header_pos] = screen_column;
                editor_header_pos++;
            }
            idx = temp_save;

            goto c8c33;

        c8bf7:
            temp_save = idx;

            if (screen_column == 0x20 || screen_column == 9 ||
                screen_column == 0x0b || screen_column == 0x1a ||
                screen_column == 0x0d)
            {
                goto c8c23;
            }
        } while (scratch_index == 0);
        append_to_output_buffer(0);
        scratch_index = 0;
        idx = temp_save;
        idx++;
    } while (idx < search_target_len);

    goto c8c3e;

c8c23:
    append_to_output_buffer(screen_column);
    idx = temp_save;
    scratch_index = screen_column;

    do
    {
        scan_ptr++;

        goto c8b91;

    c8c33:
        idx++;
    } while (idx < search_target_len);
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
}

/**
 * Updates the stored line length.
 * Copies the current screen width into line_lengths for the current row.
 */
static void update_line_length(void)
{
    line_lengths[ypos] = screen_maxcolumn;
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
}

/**
 * Clears to the end of the line.
 * Fills the remainder of the screen line with the given character.
 * @param acc fill character
 * @param line screen line index
 */
static void clear_to_eol(uint8_t fill_char, uint8_t line)
{
    uint8_t line_len = line_lengths[line];

    if (!(line_len == 0))
    {
        do
        {
            screen_putchar(fill_char);
            line_lengths[line]--;
        } while (line_lengths[line] != 0);
    }
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
 * @param idx first prompt character
 * @param pos second prompt character
 */
void draw_prompt_characters(uint8_t first_char, uint8_t second_char)
{
    uint16_t saved_cursor_pos = screen_getcursor();
    cursor_off();
    home_cursor();
    screen_setstyle(STYLE_REVERSE);
    screen_putchar(first_char);
    screen_putchar(second_char);
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
}

/**
 * Draws the status bar.
 * Shows format, justify, and insert mode indicators.
 */
static void draw_status_word(void)
{
    flags_need_redrawing_flag = 0;
    home_cursor();
    uint8_t status_char = 0x46;
    uint8_t fmt_flags = format_mode_flag;

    if (fmt_flags != 0)
    {
        status_char = 0x4d;
        fmt_flags &= 0xc0;

        if (fmt_flags == 0)
            status_char = 0x20;
    }
    screen_putchar(status_char);
    uint8_t acc2 = 0x4a;

    if (justifying_flag != 0)
        acc2 = 0x20;
    screen_putchar(acc2);

    if (insert_mode_flag != 0)
    {
        home_cursor();

        return;
    }
    home_cursor();
}

/**
 * Returns the length of the current edit line.
 * Scans the edit buffer for the last non-fill byte, adjusting for command
 * prefixes.
 * @return line length in characters
 */
static uint8_t get_line_length(void)
{
    uint8_t first_char = *current_format_line_ptr;

    command_prefix_t cp = check_for_command_prefix(first_char);
    uint8_t scan_pos = MAX_LINE_LENGTH;

    do
    {
        scan_pos--;

        if (ram[RAM_EDIT_BUFFER + scan_pos] != 0x10)
            goto cab06;
    } while (scan_pos != 0);
    scan_pos--;

cab06:
    scan_pos++;

    if (cp != NO_COMMAND_PREFIX)
        scan_pos += 3;

    return scan_pos;
}

/**
 * Jumps to a marker.
 * Moves the cursor to the address stored in the marker.
 * @param idx marker index
 */
static void go_to_marker(uint8_t marker_idx)
{
    move_cursor_to_address(markers_array[marker_idx]);
    display_start_row = 1;
    update_line_length();
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
}

/**
 * Moves the cursor to the home position.
 * Sets the cursor to row 0, column 0.
 */
static void home_cursor(void)
{
    screen_setcursor(0, 0);
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
    uint8_t gap_idx = 0;
    uint8_t acc = justifying_flag;

    if (acc != 0)
        return gap_idx;
    justify_gap_count = acc;
    column_position = acc;
    justify_overflow_counter = acc;
    uint8_t cur_char = ruler_right_stop;

    if (cur_char == 0)
        return gap_idx;
    justify_line_length = get_line_length();
    uint8_t scan_pos = 0;

    goto c9861;

c9847:
    do
    {
        cur_char = column_position;
        temp_save = cur_char;
        scan_pos++;

        if (scan_pos == justify_line_length)
            goto c9871;
        bool is_zero =
            process_char_for_output(scan_pos, false, &gap_idx, &cur_char);

        if (is_zero)
            goto c985c;
    } while (cur_char != 0x20);
    justify_gap_count++;

c985c:
    do
    {
        scan_pos++;

        if (scan_pos == justify_line_length)
            goto c986d;

    c9861:
        is_zero_1 =
            process_char_for_output(scan_pos, true, &gap_idx, &cur_char);
    } while (is_zero_1);

    if (cur_char != 0x20)
        goto c9847;

    goto c985c;

c986d:
    justify_gap_count--;

    if (justify_gap_count & 0x80)
        return gap_idx;

c9871:
    uint8_t acc2 = justify_gap_count;

    if (acc2 == 0)
        return gap_idx;
    uint8_t acc3 = ruler_right_stop;

    if (acc3 < temp_save)
        return gap_idx;
    acc3 -= temp_save;
    uint8_t idx1 = (uint8_t)(acc3 + 1);
    uint8_t extra = (uint8_t)(acc3 + 1 + justify_line_length);

    if (extra >= MAX_LINE_LENGTH)
    {
        temp_save = (uint8_t)(extra - MAX_LINE_LENGTH);
        idx1 = (uint8_t)(idx1 - temp_save);
    }
    screen_row = idx1;
    uint16_t scan_ptr = idx1;
    uint8_t acc4 = scan_ptr % justify_gap_count;

    uint16_t quotient_tmp = scan_ptr / justify_gap_count;
    justify_running_total_accum = acc4;
    uint8_t acc5 = quotient_tmp & 0xff;

    justify_extra_space_accum = acc5;
    uint8_t pos1 = 0;
    uint8_t idx2 = justify_gap_count;
    uint8_t acc6 = pos1;

    do
    {
        input_buffer[pos1] = acc6;
        pos1++;
        idx2--;
    } while (idx2 != 0);
    uint8_t pos2 = print_xpos;

    pos2++;

    if (pos2 >= justify_gap_count)
        pos2 = 1;
    pos2--;
    uint8_t idx3 = justify_gap_count;

    do
    {
        uint8_t acc7 = justify_running_total_accum;

        if (acc7 != 0)
        {
            acc7 = 1;
            justify_running_total_accum--;
        }
        acc7 += justify_extra_space_accum;
        input_buffer[pos2] = acc7;
        uint8_t acc8 = screen_row;
        uint8_t acc9 = acc8 - input_buffer[pos2];

        screen_row = acc9;
        pos2++;

        if (pos2 >= justify_gap_count)
            pos2 = 0;

        if (acc9 == 0)
            break;
        idx3--;
    } while (idx3 != 0);
    print_xpos = pos2;
    uint8_t pos3 = 0;

    scratch_index = pos3;
    column_position = pos3;
    wipe_buffer(0x1a, target_ptr);
    uint8_t acc10 = justify_overflow_counter;

    if (!(acc10 == 0))
    {
        uint8_t pos4 = 0;

        do
        {
            uint8_t acc11 = output_buffer[pos4];

            ram[RAM_EDIT_BUFFER + pos4] = acc11;
            pos4++;
        } while (pos4 != justify_overflow_counter);
    }
    uint8_t pos5 = justify_overflow_counter;
    uint8_t idx4 = justify_overflow_counter;

    do
    {
        uint8_t acc12 = output_buffer[idx4];

        if (!(acc12 != 0x20))
        {
            uint8_t acc13 = scratch_index;

            if (!(acc13 == 0))
            {
                temp_save = pos5;
                uint8_t pos6 = column_position;

                if (pos6 < justify_gap_count)
                    acc13 = input_buffer[pos6];
                acc13 += temp_save;
                column_position++;
                pos5 = acc13;
                scratch_index = 0;
            }
            acc12 = 0x20;
        }
        else
        {
            scratch_index++;
        }
        ram[RAM_EDIT_BUFFER + pos5] = acc12;
        pos5++;
        idx4++;
    } while (idx4 != justify_line_length);

    while (1)
    {
        if (pos5 >= MAX_LINE_LENGTH)
            return idx4;
        ram[RAM_EDIT_BUFFER + pos5] = 0x10;
        pos5++;
    }
    return idx4;
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
    uint8_t slot_idx = 0;

    do
    {
        if (!(((uint8_t**)&pointer_array)[slot_idx] < insert_ptr))
            ((uint8_t**)&pointer_array)[slot_idx] += size_delta;
        slot_idx++;
    } while (slot_idx != sizeof(pointer_array) / sizeof(uint8_t*));
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

static const uint8_t memory_full_message[] = "Memory full - Press ESCAPE";

uint8_t process_current_document_character(uint8_t* target_ptr,
    uint8_t* char_width_out,
    uint8_t* pos_inout,
    bool* is_tab)
{
    uint8_t cur_byte = target_ptr[*pos_inout];
    (*pos_inout)++;
    uint8_t processed_char =
        process_document_character(cur_byte, char_width_out, is_tab);
    return processed_char;
}

/**
 * Recalculates the cursor column.
 * Walks the edit line to map visual column to buffer position, handling tabs.
 */
static void recalculate_cursor_xpos(void)
{
    uint8_t char_width;
    uint8_t* line_ptr = &ram[RAM_EDIT_BUFFER];
    uint8_t acc_width = line_change_pending_flag;
    bool is_tab = false;

    if (!(acc_width != 0))
    {
        uint8_t scan_pos = acc_width;

        do
        {
            if (scan_pos == xpos)
                goto ca63d;
            column_position = acc_width;
            (void)process_current_document_character(
                line_ptr, &char_width, &scan_pos, &is_tab);
            acc_width = char_width;
            acc_width += column_position;
        } while (acc_width >= column_position);
    }
    acc_width = 0;
    line_change_pending_flag = acc_width;
    uint8_t buf_pos = acc_width;

    do
    {
        column_position = acc_width;
        (void)process_current_document_character(
            line_ptr, &char_width, &buf_pos, &is_tab);
        acc_width = char_width;
        acc_width += column_position;
    } while (acc_width < visual_column);

    if (acc_width != visual_column)
    {
        acc_width = column_position;
        buf_pos--;
    }
    xpos = buf_pos;

ca63d:
    visual_column = acc_width;
}

/**
 * Redraws the entire editor display.
 * Handles scrolling, line rendering, status, and cursor placement.
 */
void redraw_editor(void)
{
    uint8_t next_line_len;
    uint8_t rows_remaining;
    uint8_t* draw;

    cursor_off();
    int ruler_idx = ruler_index_ptr;

    saved_ruler_index_redraw = ruler_idx;
    uint8_t status_needs_redraw = status_line_needs_redrawing_flag;
    uint8_t saved_status_line_needs_redrawing_flag = status_needs_redraw;

    if (!(edit_buffer_unpacked_flag == 0))
    {
        uint8_t display_flag = display_start_row;

        display_flag |= scroll_repeat_count;

        if (display_flag != 0)
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
        uint8_t tmp_idx = screen_maxrow;

        do
        {
            uint8_t line_len = line_lengths[tmp_idx - 1];

            line_lengths[tmp_idx] = line_len;
            tmp_idx--;
        } while (tmp_idx != 0);
        screen_scrolldown();
        screen_setcursor(0, 1);
        uint8_t row = 1;

        goto ca351;

    ca2dc:
        ruler_index_ptr = saved_ruler_index_scroll;

    ca2e0:
        uint8_t walk_idx = 0;
        uint8_t* walk = top_of_screen_line_ptr;

        do
        {
            walk_idx++;

            if (walk == editor_ptr6)
                ptr6_screen_row = walk_idx;

            if (walk == current_line_ptr)
                goto ca313;
            {
                uint8_t* nav_ptr;

                if (advance_to_next_line(walk, &nav_ptr, &next_line_len))
                    goto ca313;
                scratch_line_ptr = nav_ptr;
            }
            scratch_line_ptr += next_line_len;
            walk = scratch_line_ptr;
        } while (walk_idx <= screen_maxrow);

    ca30d:
        do
        {
            (void)compute_display_start_line();

            goto ca2e0;

        ca313:
            if (walk_idx <= screen_maxrow)
                goto ca35e;
        } while (scroll_repeat_count != 0);
        uint8_t shift_idx = 0;

        do
        {
            uint8_t next_len2 = line_lengths[shift_idx + 1];

            line_lengths[shift_idx] = next_len2;
            shift_idx++;
        } while (shift_idx != screen_maxrow);
        ptr6_screen_row--;
        line_lengths[0] = screen_maxcolumn;
        ruler_index_ptr = saved_ruler_index_scroll;
        {
            uint8_t* nav_ptr;

            advance_to_next_line(
                top_of_screen_line_ptr, &nav_ptr, &next_line_len);
            scratch_line_ptr = nav_ptr;
        }
        top_of_screen_line_ptr += next_line_len;
        screen_scrollup();
        screen_setcursor(0, screen_maxrow);
        row = screen_maxrow;

    ca351:
        saved_ruler_index_scroll = ruler_index_ptr;
        saved_status_line_needs_redrawing_flag++;
        line_counter++;
        walk_idx = row;

    ca35e:
        ypos = walk_idx;
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
        rows_remaining = a_21;
        rows_remaining++;
        draw = editor_ptr6;

        if (editor_ptr6 != NULL)
            goto ca3c1;
    }
    load_current_ruler(saved_ruler_index_scroll);
    screen_row = 1;
    draw = top_of_screen_line_ptr;
    rows_remaining = screen_maxrow;

ca3c1:
    scratch_index = rows_remaining;

    do
    {
        struct render_state rs = {.line = screen_row};

        draw_line(&rs, draw);
        uint8_t next_len = 0;
        {
            uint8_t* nav_ptr = scratch_line_ptr;

            if (advance_to_next_line(nav_ptr, &nav_ptr, &next_len))
                goto ca422;
            scratch_line_ptr = nav_ptr;
        }
        scratch_line_ptr += next_len;
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
    uint8_t line_idx = rs->line;

    if (line_lengths[line_idx] != 0)
        line_lengths[line_idx]--;
    uint8_t col = rs->col;

    if (col >= screen_maxcolumn)
    {
        rs->ch = char_to_render;

        return;
    }
    rs->col++;
    uint8_t pos = rs->pos;

    if (!(pos == 0))
    {
        uint8_t* size_delta = rs->line_ptr;

        col = find_marker_at_position(rs->pos - 1, size_delta);

        if (col >= 4)
            goto ca514;
        col = 0;

        if (col & 0x80)
        {
            rs->ch = char_to_render;

            goto ca523;
        }
        if (col != 0)
            goto ca514;
        screen_setstyle(STYLE_REVERSE);
    }
ca514:
    pos = char_to_render;
    control_code_t f = check_for_control_code(pos);

    if (f != NO_CONTROL_CODE)
    {
        if (f == HIGHLIGHT1_CODE)
            pos = 0x2d;
        else
            pos = 0x2a;
    }
ca523:
    if (pos == 0x0d || pos == 0x00)
        pos = 0x20;
    screen_putchar(pos);

    if (col == 0)
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
    uint8_t buf_off = rs->buf_off;

    rs->buf_off++;

    if (buf_off < hscroll_pos)
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
 * @param idx marker index
 */
static void set_marker(uint8_t marker_idx)
{
    set_marker_to_here(marker_idx);
    display_start_row = 1;
    update_line_length();
}

void go_to_marker(uint8_t marker_idx);

/**
 * Common handler for setting numbered markers.
 * Writes the buffer, looks up the marker index, and sets it.
 * @param acc marker character
 */
static void set_marker_common(uint8_t marker_char)
{
    write_line_back_to_document_safely();
    int idx = lookup_marker(marker_char);

    set_marker(idx);
}

/**
 * Displays a memory-full error message.
 * Shows an inverted message and waits for Escape.
 */
void show_memory_full_error(void)
{
    uint8_t key_code;

    cursor_off();
    screen_setcursor(3, 0);
    screen_setstyle(STYLE_REVERSE);
    uint8_t remaining_cols = screen_maxcolumn;

    line_lengths[0] = remaining_cols;
    remaining_cols--;
    remaining_cols--;
    uint8_t msg_idx = 0;

    for (;;)
    {
        uint8_t msg_char = memory_full_message[msg_idx];

        if (msg_char == 0)
            break;
        msg_idx++;
        remaining_cols--;

        if (remaining_cols == 0)
            break;
        screen_putchar(msg_char);
    }
    screen_setstyle(0);

    if (!(remaining_cols == 0))
    {
        do
        {
            screen_putchar(0x20);
            remaining_cols--;
        } while (remaining_cols != 0);
    }
    edit_buffer_unpacked_flag = 0;
    clear_cmd();

    do
    {
        beep();
        key_code = screen_getchar();
    } while (key_code != 0x1b);
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
}

/**
 * Appends a byte to the output buffer.
 * Writes the byte if space remains in the editor output buffer.
 * @param acc byte to append
 */
static void append_to_output_buffer(uint8_t byte_to_append)
{
    if (editor_output_pos >= MAX_LINE_LENGTH)
        return;
    output_buffer[editor_output_pos] = byte_to_append;
    editor_output_pos++;
}

/**
 * Converts to uppercase unless folding is enabled.
 * Returns the character uppercased when folding is off.
 * @param acc input character
 * @return possibly uppercased character
 */
uint8_t upper_case_unless_folding(uint8_t ch)
{
    if (folding_flag & 0x80)
        return ch;
    return toupper(ch);
}

static bool process_char_for_output(
    uint8_t buf_idx, bool carry_in, uint8_t* char_width_out, uint8_t* out_char)
{
    screen_column = (screen_column >> 1) | (carry_in ? 0x80 : 0);
    (*out_char) = ram[RAM_EDIT_BUFFER + buf_idx];
    output_buffer[buf_idx] = (*out_char);
    if (!((*out_char) != 9))
    {
        {
            bool is_tab = false;

            (*out_char) = process_document_character(
                (*out_char), char_width_out, &is_tab);
        }
        (*out_char) = *char_width_out;
        (*out_char) += column_position;

        if ((*out_char) != 0)
            goto c995c;
    }
    if (!((*out_char) != 0x0b))
    {
        (*out_char) = ruler_left_stop;

        if (!((*out_char) == 0))
        {
            *char_width_out = column_position;

            if (*char_width_out != 0)
            {
                if (*char_width_out >= ruler_left_stop)
                {
                    (*char_width_out)++;
                    (*out_char) = *char_width_out;
                }
            }
        c995c:
            column_position = (*out_char);
            justify_overflow_counter = buf_idx;
            justify_overflow_counter++;
            (*out_char) = 0;
            justify_gap_count = (*out_char);

            return true;
        }
    c9967:
        (*out_char) = 0x20;
    }
    if ((*out_char) < 0x1b)
        goto c9967;

    if ((*out_char) < 0x20)
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
    uint8_t cur_byte;
    uint8_t left_stop_tmp;
    uint8_t a_15;
    uint8_t is_space;

    cursor_moved_flag++;
    print_xpos = 4;
    uint8_t out_pos = 0;

    input_buffer_offset = out_pos;
    line_format_status = out_pos;
    uint8_t first_char = current_line_ptr[out_pos];

    command_prefix_t cp = check_for_command_prefix(first_char);

    if (cp != NO_COMMAND_PREFIX)
        return advance_to_next_doc_line() ? FORMAT_AT_END : FORMAT_OK;

c998a:
    uint8_t fmt_flags = format_mode_flag;

    fmt_flags &= 0x81;

    if (fmt_flags != 0)
        return advance_to_next_doc_line() ? FORMAT_AT_END : FORMAT_OK;

    if (ruler_right_stop == 0)
        return advance_to_next_doc_line() ? FORMAT_AT_END : FORMAT_OK;

    if (ruler_right_stop < ruler_left_stop)
        return advance_to_next_doc_line() ? FORMAT_AT_END : FORMAT_OK;
    uint8_t available_width = ruler_right_stop - ruler_left_stop + 2;

    scratch_offset = available_width;
    wipe_buffer(0x10, edit_buffer_base);
    uint8_t* size_delta = current_line_ptr;
    uint8_t tmp_pos = 0;
    uint8_t soft_hyphen_flag = 0;

    format_src_index = tmp_pos;
    column_position = tmp_pos;
    soft_hyphen_flag = tmp_pos;
    justify_gap_count = tmp_pos;
    bottom_margin = tmp_pos;

c99b6:
    editor_output_pos = tmp_pos;
    uint8_t src_pos = format_src_index;

    do
    {
        uint8_t idx = find_marker_at_position(src_pos, size_delta);

        if (idx == 0x0c)
            break;
        markers_array[idx / 2] = 0;
        line_format_status++;
    } while (line_format_status != 0);

c99c9:
    do
    {
        cur_byte = current_line_ptr[src_pos];
        src_pos++;
        format_src_index = src_pos;

        if (!(cur_byte != 9))
        {
            {
                bool is_tab = false;

                (void)process_document_character(cur_byte, &is_space, &is_tab);
            }
            is_space--;
            uint8_t col_tmp = is_space;

            col_tmp += column_position;
            column_position = col_tmp;
            cur_byte = 9;

            goto c9a21;
        }
        if (cur_byte != 0x1a)
            goto c99ee;

        do
        {
            if (justify_gap_count != 0)
                goto c99c9;
            cur_byte = 0x20;

            goto c9a2e;

        c99ee:
            if (cur_byte != 0x0b)
                goto c9a11;
            is_space = input_buffer_offset;
        } while (is_space != 0 || soft_hyphen_flag != 0);
        soft_hyphen_flag++;
        left_stop_tmp = ruler_left_stop;
    } while (left_stop_tmp == 0);

    if (column_position < ruler_left_stop)
    {
        column_position = left_stop_tmp;
        column_position--;
    }
    left_stop_tmp += scratch_offset;
    scratch_offset = left_stop_tmp;
    cur_byte = 0x0b;

c9a11:
    if (!(cur_byte != 0x0d))
    {
        src_pos--;

        if (src_pos == 0)
            return advance_to_next_doc_line() ? FORMAT_AT_END : FORMAT_OK;

        if (find_next_word_boundary(src_pos))
            goto c9a87;
        cur_byte = 0x20;
        input_buffer_offset = cur_byte;
    }
c9a21:
    tmp_pos = editor_output_pos;
    is_space = 0;

    if (!(cur_byte != 0x20))
    {
        is_space++;

        if ((justify_gap_count & 0x80))
            goto c9a40;
    }
c9a2e:
    tmp_pos = editor_output_pos;
    ram[RAM_EDIT_BUFFER + tmp_pos] = cur_byte;

    if (cur_byte == 0x20)
        bottom_margin = (uint8_t)(bottom_margin >> 1) | 0x80;
    tmp_pos++;
    control_code_t cc = check_for_control_code(cur_byte);

    if (cc == NO_CONTROL_CODE)
        column_position++;

c9a40:
    bool old_l0046_high = (justify_gap_count & 0x80) != 0;

    justify_gap_count = is_space;

    if (!(old_l0046_high || cur_byte == 0x20))
    {
        if (tmp_pos >= MAX_LINE_LENGTH + 1)
            goto c9a60;

        if (bottom_margin == 0)
            goto c9a58;

        if (column_position >= scratch_offset)
            goto c9a60;
    }
c9a58:
    if (tmp_pos >= 0x86)
        tmp_pos--;

    goto c99b6;

c9a60:
    format_src_index++;

    do
    {
        format_src_index--;
        tmp_pos--;

        if (tmp_pos == 0)
            return advance_to_next_doc_line() ? FORMAT_AT_END : FORMAT_OK;
        uint8_t saved_byte = ram[RAM_EDIT_BUFFER + tmp_pos];
        {
            ram[RAM_EDIT_BUFFER + tmp_pos] = 0x10;
            a_15 = saved_byte;
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
 * @param pos current source index
 * @return true if wrap is needed
 */
static bool find_next_word_boundary(uint8_t src_idx)
{
    uint8_t base_off = src_idx;
    uint8_t* scan_ptr = current_line_ptr + base_off + 1;
    uint8_t* insert_ptr = scan_ptr;

    src_idx = 0;
    screen_column = src_idx;

    do
    {
        uint8_t next_char = insert_ptr[src_idx];

        if (next_char == 0)
            goto c9b2f;
        command_prefix_t cp = check_for_command_prefix(next_char);

        if (cp != NO_COMMAND_PREFIX || next_char == 0x0d)
            goto c9b2f;

        if (!(src_idx != 0))
        {
            temp_save = src_idx;

            goto c9aef;

        c9ae9:
            scan_ptr++;

        c9aef:
            uint8_t scan_char = scan_ptr[src_idx];

            if (scan_char == 0 || scan_char == 0x0d)
                goto c9b06;

            if (scan_char == 9)
                goto c9b2f;

            if (scan_char == 0x0b)
            {
                temp_save |= 0x80;

                if (scan_char >= 0x0b)
                    goto c9ae9;
            }
        }
    c9b06:
        uint8_t check_char = insert_ptr[src_idx];

        if (!(check_char != 0x20))
        {
            if (ruler_left_stop == 0 || temp_save == 0 || screen_column != 0)
                goto c9b2f;
        }
        else
        {
            if (check_char != 0x0b)
                break;
            screen_column = check_char;
        }
        src_idx++;
    } while (src_idx != 0);

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
 * @param acc character to insert
 * @return true on success
 */
static bool insert_character_into_edit_buffer(uint8_t ch)
{
    _Bool ok;
    {
        ok = insert_edit_buffer_bytes_at_xpos(1);
        ch = ch;
    }
    if (!ok)
        return false;
    ram[RAM_EDIT_BUFFER + xpos] = ch;
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
    uint8_t half_rows = screen_maxrow;

    display_start_row = half_rows;
    half_rows >>= 1;
    uint8_t rows_needed = half_rows;

    rows_needed++;

    if (!(scroll_repeat_count & 0x80))
    {
        if (scroll_repeat_count != 0)
            rows_needed = ypos;
    }
    uint8_t* line = current_line_ptr;

    for (;;)
    {
        rows_needed--;

        if (rows_needed == 0)
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

    return rows_needed;
}

/**
 * Advances the render state to the next character.
 * Reads and processes the next document character into the render state.
 * @param rs render state
 */
static void advance_to_next_char(struct render_state* rs)
{
    uint8_t pos = rs->pos;

    column_position = rs->char_width;
    uint8_t cur_byte;
    uint8_t char_width;

    cur_byte = process_current_document_character(
        rs->line_ptr, &char_width, &pos, &rs->prev_is_tab);
    rs->ch = cur_byte;
    rs->pos = pos;
    rs->width = char_width;
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
 * @param pos buffer position
 * @param target_ptr base pointer
 * @return marker index or 0x0c if none
 */
static uint8_t find_marker_at_position(uint8_t buf_offset, uint8_t* target_ptr)
{
    uint8_t* scan_ptr = target_ptr + buf_offset;
    uint8_t slot_idx = 0;

    do
    {
        if (!(scan_ptr != markers_array[slot_idx / 2]))
            goto ca558;
        slot_idx++;
        slot_idx++;
    } while (slot_idx != 0x0c);

    return 0x0c;

ca558:
    return slot_idx;
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
    uint8_t first_byte = *current_line_ptr;

    command_prefix_t cp = check_for_command_prefix(first_byte);

    if (cp != NO_COMMAND_PREFIX)
    {
        if (cp == RULER_PREFIX)
            edit_buffer_unpacked_flag = first_byte;
        set_format_mode_bit7();
    }
    current_format_line_ptr =
        (cp != NO_COMMAND_PREFIX) ? target_ptr : &ram[RAM_EDIT_BUFFER];
    uint8_t copy_idx = 0;

    do
    {
        uint8_t a2 = current_line_ptr[copy_idx];

        if (a2 == 0x0d)
            break;
        current_format_line_ptr[copy_idx] = a2;
        copy_idx++;
    } while (copy_idx != 0);
    edit_line_len = copy_idx;
    target_ptr[MAX_LINE_LENGTH + 3] = 0x0d;
}

/**
 * Updates markers to point into the format buffer.
 * Retargets markers from document heap into the current format line.
 */
static void update_markers_to_format_buffer(void)
{
    uint8_t* size_delta = current_line_ptr;
    uint8_t offset = 0;

    do
    {
        uint8_t idx = find_marker_at_position(offset, size_delta);

        if (!(idx == 0x0c))
        {
            {
                uint16_t val = (current_format_line_ptr - &ram[0]) + offset;
                markers_array[idx / 2] = &ram[val];

                if (val != 0)
                    continue;
            }
        }
        uint8_t acc = current_line_ptr[offset];

        if (acc == 0x0d)
            return;
        offset++;
    } while (offset != 0);
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
        uint8_t acc = target_ptr[0];

        if (acc == 0x0d)
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
    uint8_t scan_pos = 0;

    if (ruler_left_stop != 0)
    {
        do
        {
            uint8_t scanned_char = ram[RAM_EDIT_BUFFER + scan_pos];

            scan_pos++;

            if (scanned_char == 0x0b)
                return scan_pos;
        } while (scan_pos < MAX_LINE_LENGTH);

        return -1;
    }
    return scan_pos;
}

/**
 * Inserts at the left margin if needed.
 * Ensures a margin tab exists by inserting one at column zero.
 */
static void insert_at_left_margin(void)
{
    if (find_left_margin_stop() < 0)
        insert_byte_at_xpos(0);
}

/**
 * Inserts a margin byte at a position.
 * Inserts a single margin tab at the given position.
 * @param pos position
 * @return true on success
 */
static bool insert_byte_at_xpos(uint8_t insert_pos)
{
    _Bool ok;
    uint8_t saved_xpos = xpos;

    xpos = insert_pos;
    ok = insert_edit_buffer_bytes_at_xpos(1);

    if (ok)
        ram[RAM_EDIT_BUFFER + xpos] = 0x0b;
    xpos = saved_xpos;

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
 * @param acc fill value
 * @param target_ptr destination
 */
void wipe_buffer(uint8_t fill_value, uint8_t* target_ptr)
{
    uint8_t idx = 0;
    uint8_t remaining = 0x89;

    do
    {
        target_ptr[idx] = fill_value;
        idx++;
        remaining--;
    } while (remaining != 0);
}

/**
 * Writes the edit buffer back to the document.
 * Computes size delta, adjusts heap, and copies the line including marker
 * updates.
 * @return true if write failed due to memory
 */
static bool write_line_back_to_document(void)
{
    uint8_t out_byte;
    uint8_t stored_byte;

    if (!(edit_buffer_unpacked_flag == 0))
    {
        uint8_t* insert_ptr = current_line_ptr;

        area_size = 0;
        screen_column = get_line_length();
        uint8_t old_len = edit_line_len;
        {
            uint8_t minuend = old_len;

            old_len -= screen_column;

            if (minuend < screen_column)
                goto ca8df;

            if (old_len == 0)
                goto ca8ed;
        }
        area_size = old_len;
        scratch_scan_ptr = adjust_pointers(insert_ptr, area_size);

        goto ca8ed;

    ca8df:
        temp_save = old_len;
        uint8_t neg_len = 0;

        neg_len -= temp_save;
        area_size = neg_len;

        if (!make_space_for_insertion(insert_ptr, area_size))
            return true;

    ca8ed:
        if (((int8_t)edit_buffer_unpacked_flag < 0))
        {
            if (edit_buffer_dirty_flag != 0)
                clamp_ptr6_to_document();
        }
        uint8_t copy_idx = 0;

        edit_buffer_dirty_flag = copy_idx;
        edit_buffer_unpacked_flag = copy_idx;
        area_size = current_format_line_ptr - &ram[0];
        uint8_t line_len = screen_column;

        edit_line_len = line_len;

        do
        {
            if (!(line_len != 0))
            {
                out_byte = 0x0d;
            }
            else
            {
                out_byte = current_format_line_ptr[copy_idx];

                if (out_byte == 0x10)
                    out_byte = 0x20;
            }
            {
                uint16_t val;

                do
                {
                    uint8_t idx =
                        find_marker_at_position(copy_idx, &ram[area_size]);

                    if (idx == 0x0c)
                        break;
                    val = (current_line_ptr - &ram[0]) + copy_idx;
                    markers_array[idx / 2] = &ram[val];
                } while (val != 0);
                stored_byte = out_byte;
            }
            current_line_ptr[copy_idx] = stored_byte;
            copy_idx++;
            line_len--;
        } while (stored_byte != 0x0d);
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
