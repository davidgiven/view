#include "editor.h"

#include "document.h"
#include "cli.h"

#include "io.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "view.h"

uint8_t edit_line_len;
static uint8_t editor_current_key;
static uint8_t editor_header_pos;
static uint8_t editor_output_pos;
static uint8_t* editor_ptr6;
static uint8_t format_src_index;
static uint8_t justify_extra_space_accum;
static uint8_t justify_line_length;
static uint8_t justify_overflow_counter;
static uint8_t justify_running_total_accum;
static uint8_t line_lengths[100];
static uint8_t ptr6_screen_row;
static int saved_ruler_index_redraw;

// Render pipeline state for line drawing.
typedef struct render_state
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
} render_state_t;

static bool adjust_margins_at_left_margin(void);
static format_result_t advance_to_next_doc_line(void);
static bool find_next_word_boundary(uint8_t src_idx);
static bool insert_byte_at_xpos(uint8_t insert_pos);
static bool insert_character_into_edit_buffer(uint8_t ch);
static bool insert_edit_buffer_bytes_at_xpos(uint8_t count);
static bool process_char_for_output(
    uint8_t buf_idx, bool carry_in, uint8_t* char_width_out, uint8_t* out_char);
static bool reset_area_to_marks_1_2(void);
static int find_left_margin_stop(void);
static int prompt_for_marker(void);
static uint8_t compute_display_start_line(void);
static uint8_t control_key_to_ascii(uint8_t key_code);
static void advance_current_line_pointer(void);
static void advance_to_next_char(render_state_t* rs);
static void advance_to_next_char_and_render(render_state_t* rs);
static void append_to_output_buffer(uint8_t byte_to_append);
static void cf0_delete_block_key(void);
static void cf1_next_match_key(void);
static void cf2_format_mode_key(void);
static void cf3_justify_mode_key(void);
static void cf4_insert_mode_key(void);
static void cf5_default_ruler_key(void);
static void cf6_split_line_key(void);
static void cf7_join_lines_key(void);
static void cf8_mark_as_ruler_key(void);
static void check_pointer_in_area(void);
static void clear_marks_1_2(void);
static void clear_to_eol(uint8_t fill_char, uint8_t line);
static void cursor_on(void);
static void delete_edit_buffer_bytes_at_xpos(uint8_t delete_count);
static void delete_key(void);
static void draw_line(render_state_t* rs, uint8_t* addr);
static void draw_previous_word(
    uint8_t* word_boundary, bool* is_start_of_line, uint8_t* char_width);
static void draw_ruler(void);
static void draw_status_word(void);
static void enter_printable_character(void);
static void f0_format_block_key(void);
static void f11_copy_key(void);
static void f12_left_key(void);
static void f13_right_key(void);
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
static bool flush_formatted_line(void);
static void go_to_marker(uint8_t marker_idx);
static void go_to_marker_n(uint8_t marker);
static void insert_at_left_margin(void);
static void insert_line_at_cursor(uint8_t* target_ptr);
static void insert_line_into_document(uint8_t* target_ptr);
static void k_command_key(void);
static void move_cursor_down(uint8_t lines_to_move);
static void move_cursor_up(uint8_t lines_to_move);
static void move_to_previous_line(void);
static void o_command_key(void);
static void q_command_key(void);
static void recalculate_cursor_xpos(void);
static void redraw_editor(void);
static void render_char(render_state_t* rs);
static void render_xchar(render_state_t* rs);
static void return_key(void);
static void return_to_editor_loop(void);
static void set_format_mode_bit7(void);
static void set_marker(uint8_t marker_idx);
static void set_marker_common(uint8_t marker_char);
static void set_xpos_to_line_length(void);
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
static void tab_highlight_common(uint8_t char_to_insert);
static void tab_key(void);
static void unpack_line(uint8_t* target_ptr);
static void unpack_line_into_buffer(uint8_t* target_ptr);
static void update_line_length(void);

static const uint8_t memory_full_message[] = "Memory full - Press ESCAPE";

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
 * Each iteration prepares the current line and cursor, redraws the screen,
 * then dispatches one key before repeating.
 */
void editor_loop_impl(void)
{
    screen_enter();

    for (;;)
    {
        uint8_t saved_fmt = format_mode_flags;
        if (!edit_buffer_unpacked_flag)
        {
            unpack_line(edit_buffer_base);
            edit_buffer_unpacked_flag = false;
            edit_buffer_contains_ruler_flag = false;
        }
        recalculate_cursor_xpos();

        if (ruler_left_stop != 0 && (format_mode_flags & 0x80) == 0 &&
            ruler_left_stop > visual_column)
        {
            bool move_to_margin = cursor_moved_flag;

            if (!move_to_margin)
            {
                uint8_t line_len = get_line_length();

                if (line_len < xpos)
                {
                    if (format_mode_flags & 0x40)
                        move_to_margin = true;
                    else
                        xpos = line_len;
                }
            }

            if (move_to_margin)
            {
                visual_column = ruler_left_stop;
                line_change_pending_flag++;
                recalculate_cursor_xpos();
                uint8_t masked_flags = format_mode_flags & 0xbf;
                int margin_pos = find_left_margin_stop();

                // Preserve the format flags unless a margin stop lies ahead.
                if (margin_pos >= 0 && margin_pos > xpos)
                    format_mode_flags = masked_flags | 0x40;
            }
        }
        if (saved_fmt != format_mode_flags)
            flags_need_redrawing_flag = true;
        cursor_moved_flag = false;
        redraw_editor();

        uint8_t key_code = screen_getchar();

        if (key_code == current_tab_key)
            key_code = 9;
        editor_current_key = key_code;

        if (key_code >= 0x20 && key_code < 0x7f)
        {
            enter_printable_character();
            continue;
        }
        switch (key_code)
        {
            case CTRL('['):
                esc_key();
                break;

            case CTRL('M'):
                return_key();
                break;

            case 0x7f:
                delete_key();
                break;

            case CTRL('I'):
                tab_key();
                break;

            case CTRL('E'):
            case SCREEN_KEY_UP:
                f15_up_key();
                break;

            case CTRL('S'):
            case SCREEN_KEY_LEFT:
                f12_left_key();
                break;

            case CTRL('D'):
            case SCREEN_KEY_RIGHT:
                f13_right_key();
                break;

            case CTRL('X'):
            case SCREEN_KEY_DOWN:
                f14_down_key();
                break;

            case CTRL('A'):
                sf12_left_key();
                break;

            case CTRL('F'):
                sf13_right_key();
                break;

            case CTRL('C'):
                sf14_down_key();
                break;

            case CTRL('R'):
                sf15_up_key();
                break;

            case CTRL('G'):
                f9_delete_char_key();
                break;

            case CTRL('H'):
                f8_insert_char_key();
                break;

            case CTRL('Y'):
                f7_delete_line_key();
                break;

            case CTRL('V'):
                cf4_insert_mode_key();
                break;

            case CTRL('N'):
                f6_insert_line_key();
                break;

            case CTRL('B'):
                f0_format_block_key();
                break;

            case CTRL('T'):
                sf3_delete_to_char_key();
                break;

            case CTRL('L'):
                cf1_next_match_key();
                break;

            case CTRL('J'):
                cf7_join_lines_key();
                break;

            case CTRL('P'):
                sf1_swap_case_key();
                break;

            case CTRL('O'):
                o_command_key();
                break;

            case CTRL('Q'):
                q_command_key();
                break;

            case CTRL('K'):
                k_command_key();
                break;
        }
    }
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
 * Toggles insert mode.
 * Inverts the insert-mode flag and marks the status bar for redraw.
 */
static void cf4_insert_mode_key(void)
{
    insert_mode_flag = !insert_mode_flag;
    flags_need_redrawing_flag = true;
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
 * Handles the Delete key.
 * Deletes the character before the cursor and handles insert-mode and
 * auto-insert behavior.
 */
static void delete_key(void)
{
    if (visual_column == 0)
        return;
    xpos--;
    uint8_t deleted_char = current_line_buffer.text[xpos];
    f9_delete_char_key();
    if (deleted_char >= 0x0c && !insert_mode_flag && get_line_length() > xpos)
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
        memory_full();
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
    cursor_moved_flag = true;
}

/**
 * Deletes the current line.
 * Removes the current line from the document and moves the cursor.
 */
static void f7_delete_line_key(void)
{
    write_line_back_to_document_safely();
    cursor_moved_flag = true;
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
{ insert_character_into_edit_buffer(0x20); }

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
 * Displays a command prefix prompt and reads its follow-up key.
 * @param prefix prefix letter displayed after the caret
 * @return follow-up key converted to uppercase ASCII
 */
static uint8_t read_prefix_command_key(uint8_t prefix)
{
    draw_prompt_characters('^', prefix);
    flags_need_redrawing_flag = true;
    return control_key_to_ascii(screen_getchar());
}

/**
 * Handles the K-command prefix.
 * Prompts for a follow-up key and dispatches to marker or block operations.
 */
static void k_command_key(void)
{
    uint8_t ascii_code = read_prefix_command_key('K');

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
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
            set_marker_common(ascii_code);
            return;
    }
}

/**
 * Deletes the block defined by markers 1 and 2.
 * Resets the area to markers 1 and 2, moves the cursor, and deletes the block.
 */
static void cf0_delete_block_key(void)
{
    write_line_back_to_document_safely();
    cursor_moved_flag = true;

    if (reset_area_to_marks_1_2())
    {
        beep();

        return;
    }
    move_cursor_to_address(area_start_ptr);
    clamp_ptr6_to_document();
    area_size = area_end_ptr - area_start_ptr;
    adjust_area_pointers(area_size);
    ensure_cr_at_document_top();
    clear_marks_1_2();
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
    move_cursor_to_address(area_insert_ptr);
}

/**
 * Handles the O-command prefix.
 * Prompts for a follow-up key and dispatches to formatting and ruler
 * operations.
 */
static void o_command_key(void)
{
    uint8_t ascii_code = read_prefix_command_key('O');

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
 * Toggles format mode.
 * Flips the format-mode flag between states and marks the status bar for
 * redraw.
 */
static void cf2_format_mode_key(void)
{
    uint8_t new_flags = format_mode_flags & 0xbf;

    if (format_mode_flags & 0x40)
        new_flags |= 1;
    new_flags ^= 1;
    format_mode_flags = new_flags;
    flags_need_redrawing_flag = true;
}

/**
 * Toggles justification mode.
 * Inverts the justification flag and marks the status bar for redraw.
 */
static void cf3_justify_mode_key(void)
{
    justifying_flag = !justifying_flag;
    flags_need_redrawing_flag = true;
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
    create_default_ruler(current_line_buffer.text);
}

/**
 * Handles the Q-command prefix.
 * Prompts for a follow-up key and dispatches to navigation and editing
 * operations.
 */
static void q_command_key(void)
{
    uint8_t ascii_code = read_prefix_command_key('Q');

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
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
            go_to_marker_n(ascii_code);
            return;
    }
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
    uint8_t first_char = current_format_line[0];

    command_prefix_t cp = check_for_command_prefix(first_char);

    if (cp != NO_COMMAND_PREFIX)
        insert_offset += offsetof(line_t, text);
    uint32_t sum = (uint32_t)(current_line_ptr - &ram[0]) + insert_offset;

    if (sum > 0xffff)
    {
        f6_insert_line_key();

        return;
    }
    insert_line_into_document(&ram[(uint16_t)sum]);
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
    uint8_t bytes_to_delete = MAX_LINE_LENGTH - xpos;
    line_counter++;
    delete_edit_buffer_bytes_at_xpos(bytes_to_delete);
}

/**
 * Moves the cursor to the end of the line.
 * Sets the cursor column to the current line length.
 */
static void f5_end_of_line_key(void)
{
    cursor_moved_flag = true;
    set_xpos_to_line_length();
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
    move_cursor_to_address(area_insert_ptr);
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
    uint8_t length = ruler_buffer_len;
    for (uint8_t pos = 0; pos < length; pos++)
        current_line_buffer.text[pos] = current_ruler_ptr[pos];
    cf8_mark_as_ruler_key();
}

/**
 * Marks the current line as a ruler.
 * Writes a ruler marker into the edit buffer and enables format mode.
 */
static void cf8_mark_as_ruler_key(void)
{
    current_format_line = edit_buffer_base;
    heap_format_line_ptr = (line_t*)edit_buffer_base;
    current_format_line[0] = RULER_PREFIX;
    current_format_line[1] = '.';
    current_format_line[2] = '.';
    line_counter++;
    if (!edit_buffer_contains_ruler_flag)
    {
        edit_buffer_unpacked_flag = true;
        edit_buffer_contains_ruler_flag = true;
        edit_buffer_dirty_flag = true;
    }
    set_format_mode_bit7();
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
    bool is_start_of_line;
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
 * Skips the rest of the current word and following spaces. At or beyond the
 * line end, enters the next line, skipping its leading spaces if present.
 */
static void sf13_right_key(void)
{
    for (;;)
    {
        uint8_t line_len = get_line_length();
        bool is_tab = false;

        if (xpos >= line_len)
        {
            uint8_t* next_line;
            uint8_t next_line_len;

            xpos = 0;
            write_line_back_to_document_safely();
            if (advance_to_next_line(
                    current_line_ptr, &next_line, &next_line_len))
                return;
            current_line_ptr += next_line_len;
            unpack_line(edit_buffer_base);
            scroll_repeat_count--;
            xpos = 0;

            if (get_line_length() == 0)
                return;
            uint8_t first_pos = 0;
            uint8_t char_width;
            uint8_t first_char = process_current_document_character(
                current_line_buffer.text, &char_width, &first_pos, &is_tab);
            if (first_char != 0x20)
                return;
            continue;
        }

        uint8_t buf_pos = xpos;
        while (buf_pos < line_len)
        {
            uint8_t char_width;
            uint8_t cur_char = process_current_document_character(
                current_line_buffer.text, &char_width, &buf_pos, &is_tab);
            if (cur_char == 0x20)
                break;
        }
        while (buf_pos < line_len)
        {
            uint8_t char_width;
            uint8_t cur_char = process_current_document_character(
                current_line_buffer.text, &char_width, &buf_pos, &is_tab);
            if (cur_char != 0x20)
            {
                xpos = buf_pos - 1;
                return;
            }
        }
        xpos = buf_pos;
        return;
    }
}

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
    uint8_t ch = current_line_buffer.text[xpos];

    if (!isalpha(ch))
    {
        f13_right_key();

        return;
    }
    line_counter++;
    ch ^= 0x20;
    current_line_buffer.text[xpos] = ch;
    f13_right_key();
}

/**
 * Releases the left margin.
 * Moves the cursor to the left-margin stop or to column zero if not in format
 * mode.
 */
static void sf2_release_margins_key(void)
{
    if ((format_mode_flags & 0x40) == 0)
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
 * Moves the cursor to the beginning of the line.
 * Sets the cursor column to zero and marks cursor movement.
 */
static void f4_beginning_of_line_key(void)
{
    cursor_moved_flag = true;
    xpos = 0;
}

/**
 * Deletes from the cursor to a specified character.
 * Prompts for a character and deletes up to and including its next occurrence.
 */
static void sf3_delete_to_char_key(void)
{
    draw_prompt_characters('C', 'H');
    flags_need_redrawing_flag = true;
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
        uint8_t start_pos = scan_pos;
        bool found_match = false;

        while (scan_pos < MAX_LINE_LENGTH)
        {
            uint8_t scanned_char = current_line_buffer.text[scan_pos];

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
            uint8_t next_char = current_line_buffer.text[scan_pos];

            scan_pos++;

            if (next_char != target_char)
                break;
        }
        scan_pos--;
        uint8_t chars_to_delete = scan_pos - start_pos;

        delete_edit_buffer_bytes_at_xpos(chars_to_delete);
    }
}

/**
 * Inserts highlight code 1.
 * Inserts the first highlight control character at the cursor.
 */
static void sf4_highlight1_key(void)
{ tab_highlight_common(0x1c); }

/**
 * Inserts highlight code 2.
 * Inserts the second highlight control character at the cursor.
 */
static void sf5_highlight2_key(void)
{ tab_highlight_common(0x1d); }

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
 * Accepts letters in alternating command positions until Return is pressed.
 * Activates the command prefix only if at least one letter was entered.
 */
static void sf8_edit_command_key(void)
{
    xpos = 0;
    redraw_editor();
    edit_buffer_dirty_flag = true;
    scratch_offset = 0;
    scratch_index = 0;

    for (;;)
    {
        screen_setcursor(scratch_offset, ypos);
        uint8_t key_code = screen_getchar();
        if (key_code == 0x0d)
            break;
        key_code &= 0xdf;
        if (key_code < 'A' || key_code > 'Z')
            continue;

        scratch_index = key_code;
        screen_putchar(key_code);
        uint8_t edit_pos = scratch_offset + 1;
        scratch_offset = edit_pos;
        edit_buffer_base[edit_pos] = key_code;
        if (edit_pos >= 2)
            scratch_offset = 0;
    }

    if (scratch_index == 0)
        return;
    current_format_line = edit_buffer_base;
    heap_format_line_ptr = (line_t*)edit_buffer_base;
    current_format_line[0] = 0x80;
    set_format_mode_bit7();
}

/**
 * Deletes the formatting command prefix.
 * Removes the command prefix from the current line if present.
 */
static void sf9_delete_command_key(void)
{
    if (check_for_command_prefix(current_format_line[0]) == NO_COMMAND_PREFIX)
        return;
    current_format_line[0] = 0;
    current_format_line = (uint8_t*)&current_line_buffer;
    heap_format_line_ptr = &current_line_buffer;
    clear_format_mode_bit7();
    line_counter++;
    edit_buffer_dirty_flag = true;
    cursor_moved_flag = true;
}

/**
 * Handles the Tab key.
 * Inserts a tab control character.
 */
static void tab_key(void)
{ tab_highlight_common(9); }

/**
 * Clears markers 1 and 2.
 * Resets marker array entries for markers 1 and 2.
 */
static void clear_marks_1_2(void)
{
    markers_array[0] = NULL;
    markers_array[1] = NULL;
}

/**
 * Converts a control key code to ASCII.
 * Maps control codes below 0x20 to letters and folds to uppercase.
 * @param key_code key code
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
 * @param delete_count number of bytes to delete
 */
static void delete_edit_buffer_bytes_at_xpos(uint8_t delete_count)
{
    scratch_offset = delete_count;
    edit_buffer_dirty_flag = true;
    uint8_t* marker_base = current_line_buffer.text;
    uint8_t first_retained = xpos + scratch_offset;
    uint8_t scan_pos = xpos;
    do
    {
        uint8_t marker = find_marker_at_position(scan_pos, marker_base);
        if (marker != NUM_MARKERS)
        {
            uint16_t buffer_offset =
                scan_pos >= first_retained
                    ? offsetof(line_t, text) + (scan_pos - scratch_offset)
                    : 0;
            markers_array[marker] =
                buffer_offset != 0
                    ? (uint8_t*)&current_line_buffer + buffer_offset
                    : NULL;
            continue;
        }
        scan_pos++;
    } while (scan_pos < MAX_LINE_LENGTH + 1);

    if (xpos >= MAX_LINE_LENGTH)
        return;
    int retained_length = MAX_LINE_LENGTH - (int)xpos - (int)scratch_offset;
    if (retained_length > 0)
    {
        memmove(&current_line_buffer.text[xpos],
            &current_line_buffer.text[xpos + scratch_offset],
            retained_length);
        memset(&current_line_buffer.text[xpos + retained_length],
            0x10,
            MAX_LINE_LENGTH - xpos - retained_length);
    }
    else
        memset(&current_line_buffer.text[xpos], 0x10, MAX_LINE_LENGTH - xpos);
}

/**
 * Inserts or overwrites the typed byte, preserving tabs and margin stops.
 * @return false if the edit buffer cannot make room for the character
 */
static bool insert_printable_byte(void)
{
    uint8_t marker = find_marker_at_position(xpos, current_line_buffer.text);
    if (marker < 2)
        line_counter++;

    uint8_t replaced_char = current_line_buffer.text[xpos];
    if (insert_mode_flag || replaced_char == '\t' || replaced_char == 0x0b)
    {
        line_counter++;
        if (!insert_edit_buffer_bytes_at_xpos(1))
            return false;
    }

    current_line_buffer.text[xpos] = editor_current_key;
    if (line_counter == 0)
        screen_putchar(editor_current_key);
    xpos++;
    update_line_length();
    return true;
}

/**
 * Calculates column_position after the typed byte, accounting for tabs,
 * margin stops, and highlight codes.
 */
static void calculate_typed_character_column(void)
{
    column_position = 0;
    for (uint8_t pos = 0; pos < xpos; pos++)
    {
        uint8_t ch = current_line_buffer.text[pos];
        if (ch == '\t')
        {
            uint8_t tab_width;
            bool is_tab = false;
            (void)process_document_character(ch, &tab_width, &is_tab);
            ch = tab_width + column_position;
            if (ch != 0)
            {
                column_position = ch;
                // Preserve the extra column counted by the original scan.
                if (ch >= 0x20)
                    column_position++;
                continue;
            }
        }
        if (ch == 0x0b)
        {
            if (ruler_left_stop != 0)
            {
                uint8_t next_column = ruler_left_stop;
                if (column_position != 0 && column_position >= ruler_left_stop)
                    next_column = column_position + 1;
                column_position = next_column;
                if (next_column >= 0x20)
                    column_position++;
                continue;
            }
            ch = ' ';
        }
        if (ch < 0x1b || ch >= 0x20)
            column_position++;
    }
}

/**
 * Moves the wrapped suffix and its markers into a newly allocated line.
 * @param insert_ptr destination line, with room for its prefix and terminator
 */
static void copy_wrapped_text(uint8_t* insert_ptr)
{
    uint8_t filler;
    uint8_t dst_idx = 0;

    if (ruler_left_stop != 0)
    {
        *insert_ptr = 0x0b;
        dst_idx = 1;
    }
    scratch_index = dst_idx;
    uint8_t* marker_base_ptr = current_line_buffer.text;
    uint8_t prev_pos = xpos;

    prev_pos--;
    uint8_t prev_char = current_line_buffer.text[prev_pos];

    if (prev_char == 0x20)
        current_line_buffer.text[prev_pos] = 0x10;
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

            if (marker_index == NUM_MARKERS)
                break;
            {
                uint8_t* val_ptr = insert_ptr + scratch_index;

                markers_array[marker_index] = val_ptr;

                if (insert_ptr - &ram[0] + scratch_index >= 0x10000)
                    break;
            }
        }
        if (screen_column == 0)
        {
            filler = 0x0d;
        }
        else
        {
            filler = current_line_buffer.text[src_pos];
            current_line_buffer.text[src_pos] = 0x10;
        }
        uint8_t dst_pos = scratch_index;

        scratch_index++;
        insert_ptr[dst_pos] = filler;
        screen_column--;
    } while ((screen_column & 0x80) == 0);
}

/**
 * Moves the last word onto a new line, justifies the old line, and restores
 * the cursor within the wrapped word.
 */
static void wrap_typed_word(void)
{
    screen_column = get_line_length();
    top_margin = 0;
    input_buffer_offset = xpos;
    uint8_t char_width;
    uint8_t word_boundary;
    bool is_start_of_line;
    draw_previous_word(&word_boundary, &is_start_of_line, &char_width);
    recalculate_cursor_xpos();
    if (visual_column <= ruler_left_stop)
        xpos = input_buffer_offset - 1;

    top_margin = input_buffer_offset - xpos;
    screen_column -= xpos;
    uint8_t required_space = screen_column + 1;
    if (ruler_left_stop != 0)
    {
        top_margin++;
        required_space++;
    }
    uint8_t* insert_ptr = current_line_ptr + edit_line_len + 1;
    if (!make_space_for_insertion(insert_ptr, required_space))
        memory_full();
    copy_wrapped_text(insert_ptr);
    justify_edit_buffer(edit_buffer_base);
    write_line_back_to_document_safely();
    clamp_ptr6_to_document();
    return_key();
    xpos = top_margin;
}

/**
 * Inserts a printable character and checks the ruler bell and right margin.
 * Adjusts the left margin before insertion and wraps the last word if needed.
 */
static void enter_printable_character(void)
{
    if (xpos >= MAX_LINE_LENGTH)
        return;
    edit_buffer_dirty_flag = true;
    if (adjust_margins_at_left_margin())
        return;
    if (!insert_printable_byte())
        return;
    calculate_typed_character_column();

    uint8_t col_pos = column_position;

    if (col_pos < ruler_buffer_len)
    {
        uint8_t ruler_char = current_ruler_ptr[col_pos];

        ruler_char &= 0xdf;

        if (ruler_char == 0x42)
            beep();
    }
    if (editor_current_key == 0x20)
        return;

    if (ruler_right_stop == 0)
    {
        line_counter = 0;

        return_to_editor_loop();
    }
    if (format_mode_flags != 0)
        return;

    if (col_pos == 0)
        return;
    col_pos--;

    if (col_pos < ruler_right_stop)
        return;
    wrap_typed_word();
}

/**
 * Returns to the editor main loop.
 * Uses longjmp to re-enter the editor event loop.
 */
static void return_to_editor_loop(void)
{ longjmp(env, JMP_EDITOR); }

/**
 * Handles the Return key.
 * Writes the buffer back and moves the cursor to the next line at column zero.
 */
static void return_key(void)
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
 * Advances to the next line.
 * Moves the current line pointer to the following line and marks cursor
 * movement.
 */
static void advance_current_line_pointer(void)
{
    cursor_moved_flag = true;
    uint8_t* line_ptr;
    uint8_t line_len;

    if (advance_to_next_line(current_line_ptr, &line_ptr, &line_len))
        return;
    current_line_ptr += line_len;
}

/**
 * Prompts for a marker key.
 * Displays the marker prompt and reads a key, returning its marker index.
 * @return marker index 0-5 or MARKER_INVALID
 */
static int prompt_for_marker(void)
{
    draw_prompt_characters('M', 'K');
    flags_need_redrawing_flag = true;
    uint8_t key_code = screen_getchar();

    return lookup_marker(key_code);
}

/**
 * Sets the area to markers 1 and 2.
 * Requires both markers, sets area and insertion pointers, and validates the
 * area.
 * @return true if area invalid or empty, false on success
 */
static bool reset_area_to_marks_1_2(void)
{
    if (markers_array[0] == NULL)
        return true;
    area_start_ptr = markers_array[0];
    if (markers_array[1] == NULL)
        return true;
    area_end_ptr = markers_array[1];

    int offset = get_line_length();
    if (offset >= xpos)
    {
        offset = xpos;
        if (check_for_command_prefix(current_format_line[0]) !=
            NO_COMMAND_PREFIX)
            offset += offsetof(line_t, text);
    }
    size_t insert_offset = (current_line_ptr - ram) + offset;
    area_insert_ptr = ram + insert_offset;
    return sanitise_area() == AREA_EMPTY;
}

/**
 * Inserts a line at the cursor.
 * Increments cursor-moved flag and inserts a line break at the given pointer.
 * @param target_ptr insertion point in document heap
 */
static void insert_line_at_cursor(uint8_t* target_ptr)
{
    cursor_moved_flag = true;
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
 * @param lines_to_move number of lines to move
 */
static void move_cursor_up(uint8_t lines_to_move)
{
    cursor_moved_flag = true;
    scratch_offset = lines_to_move;
    write_line_back_to_document_safely();
    uint8_t* line = current_line_ptr;
    do
    {
        uint8_t* previous_line;
        if (!find_previous_line(line, &previous_line))
            break;
        line = previous_line;
        if ((scratch_offset & 0x80) == 0)
            scratch_offset--;
    } while (scratch_offset != 0);
    current_line_ptr = line;
}

/**
 * Moves the cursor down.
 * Writes the buffer back and moves down by the specified number of lines.
 * @param lines_to_move number of lines to move
 */
static void move_cursor_down(uint8_t lines_to_move)
{
    cursor_moved_flag = true;
    scratch_offset = lines_to_move;
    write_line_back_to_document_safely();
    uint8_t* line = current_line_ptr;
    do
    {
        uint8_t* line_start;
        uint8_t line_length;
        if (advance_to_next_line(line, &line_start, &line_length))
        {
            line = line_start;
            break;
        }
        line += line_length;
        if ((scratch_offset & 0x80) == 0)
            scratch_offset--;
    } while (scratch_offset != 0);
    current_line_ptr = line;
}

/**
 * Ensures the copy pointer is outside the current area.
 * If inside, beeps; otherwise copies the area to the cursor.
 */
static void check_pointer_in_area(void)
{
    if (area_insert_ptr >= area_start_ptr && area_insert_ptr <= area_end_ptr)
    {
        beep();

        return;
    }
    move_cursor_to_address(area_start_ptr);
    area_size = area_end_ptr - area_start_ptr;
    uint8_t* insert_ptr = area_insert_ptr;

    if (!make_space_for_insertion(insert_ptr, area_size))
        memory_full();
    uint8_t* scan_ptr = area_start_ptr;
    uint8_t* copy_ptr = insert_ptr;

    while (1)
    {
        *copy_ptr = *scan_ptr;
        copy_ptr++;
        scan_ptr++;

        if (scan_ptr == area_end_ptr)
            break;
    }
    area_insert_ptr = insert_ptr;
    uint8_t* adjusted = copy_ptr - 1;

    split_line_at_wrap(adjusted);
    split_line_at_wrap(area_insert_ptr);
    display_start_row = 1;
    cursor_moved_flag = true;
}

/**
 * Common handler for tab and highlight insertion.
 * Checks margins and inserts the given control character, then advances the
 * cursor.
 * @param char_to_insert control character to insert
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
 * Moves the cursor right one character.
 * Increments the cursor column up to the maximum line length.
 */
static void f13_right_key(void)
{
    if (xpos >= MAX_LINE_LENGTH)
        return;
    xpos++;
}

/**
 * Initialises the editor mode.
 * Clears the screen and resets editor state variables.
 */
void enter_editor_mode(void)
{
    screen_enter();
    clear_screen();
    edit_buffer_dirty_flag = false;
    scroll_repeat_count = 0;
    edit_buffer_unpacked_flag = false;
    edit_buffer_contains_ruler_flag = false;
    memset(line_lengths, 0, (size_t)screen_maxrow + 1);
    status_line_needs_redrawing_flag = true;
    flags_need_redrawing_flag = true;
    display_start_row = 1;
}

/**
 * Moves xpos to the start of the previous word within the current line.
 * Skips spaces to the left, then scans backward through the preceding word.
 * @param word_boundary output for the processed character at the new position
 * @param is_start_of_line output true if the new position is zero
 * @param char_width output for the character width at the new position
 */
static void draw_previous_word(
    uint8_t* word_boundary, bool* is_start_of_line, uint8_t* char_width)
{
    uint8_t pos = xpos;
    bool is_tab = false;

    while (pos != 0)
    {
        pos--;
        if (pos == 0)
            break;
        uint8_t ch = process_current_document_character(
            current_line_buffer.text, char_width, &pos, &is_tab);
        pos--;
        if (ch != 0x20)
            break;
    }

    while (pos != 0)
    {
        pos--;
        uint8_t ch = process_current_document_character(
            current_line_buffer.text, char_width, &pos, &is_tab);
        if (ch == 0x20)
            break;
        pos--;
    }

    xpos = pos;
    *word_boundary = process_current_document_character(
        current_line_buffer.text, char_width, &pos, &is_tab);
    *is_start_of_line = (xpos == 0);
}

/**
 * Inserts a missing left-margin stop when formatting permits it.
 * Checks the line's final visual column to choose an insertion point and
 * adjusts xpos to account for the inserted stop and the left margin.
 * @return true if insertion failed, false if successful or no stop was needed
 */
static bool adjust_margins_at_left_margin(void)
{
    if ((format_mode_flags & 0x81) != 0 || find_left_margin_stop() >= 0)
        return false;

    uint8_t line_len = get_line_length();
    screen_column = xpos;
    xpos = line_len;
    recalculate_cursor_xpos();
    uint8_t saved_xpos = screen_column;
    uint8_t insert_pos;

    if (visual_column >= ruler_left_stop)
    {
        insert_pos = saved_xpos;
        xpos = insert_pos + 1;
    }
    else
    {
        insert_pos = xpos;
        if (insert_pos >= saved_xpos)
            xpos = saved_xpos;
        else if (saved_xpos >= ruler_left_stop)
            xpos = saved_xpos - ruler_left_stop + insert_pos + 1;
    }

    if (!insert_byte_at_xpos(insert_pos))
        return true;
    line_counter++;
    return false;
}

/**
 * Scans for the next line matching the search.
 * Retries successive document positions, skipping command headers and
 * collecting wildcard captures in output_buffer.
 * @return true if a match was found before search_limit_ptr
 */
bool scan_document_for_next_line(void)
{
    if (search_target_len == 0)
        return false;

    for (;;)
    {
        editor_output_pos = 0x14;
        editor_header_pos = 0;
        scratch_index = 0;
        uint8_t target_pos = 0;
        uint8_t* scan_ptr = search_cursor_ptr;

        for (;;)
        {
            if (scan_ptr >= search_limit_ptr)
                return false;
            if (check_for_command_prefix(*scan_ptr) != NO_COMMAND_PREFIX)
            {
                search_cursor_ptr = scan_ptr + 3;
                break;
            }
            screen_column = upper_case_unless_folding(*scan_ptr);
            uint8_t pos = 0;
            bool retry_search = false;
            bool consume_character = true;

            do
            {
                pos++;
                uint8_t next_char = scan_ptr[pos];
                if (next_char == 0 ||
                    check_for_command_prefix(next_char) != NO_COMMAND_PREFIX)
                {
                    retry_search = true;
                    break;
                }
                uint8_t target_char = header_text_maybe[target_pos];
                if (target_char == 0x20)
                {
                    if (screen_column == 0x20 || screen_column == 9 ||
                        screen_column == 0x0b || screen_column == 0x1a ||
                        screen_column == 0x0d)
                    {
                        append_to_output_buffer(screen_column);
                        scratch_index = screen_column;
                        break;
                    }
                    if (scratch_index == 0)
                    {
                        retry_search = true;
                        break;
                    }
                    append_to_output_buffer(0);
                    scratch_index = 0;
                    consume_character = false;
                }
                else if (target_char == 1)
                {
                    if (editor_header_pos < 0x14)
                        output_buffer[editor_header_pos++] = screen_column;
                    consume_character = true;
                }
                else
                {
                    if (target_char == 2)
                        target_char = 0x20;
                    if (target_char != screen_column)
                    {
                        retry_search = true;
                        break;
                    }
                    consume_character = true;
                }
                target_pos++;
                if (consume_character)
                    break;
            } while (target_pos < search_target_len);

            if (retry_search)
            {
                search_cursor_ptr++;
                break;
            }
            if (consume_character)
                scan_ptr++;
            if (target_pos == search_target_len)
            {
                doc_working_ptr = search_cursor_ptr;
                search_cursor_ptr = scan_ptr;
                return true;
            }
        }
    }
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
{ screen_clear(); }

/**
 * Jumps to a numbered marker.
 * Writes the buffer back and jumps to the marker identified by the given
 * character.
 * @param marker marker character '1'-'6'
 */
static void go_to_marker_n(uint8_t marker)
{
    write_line_back_to_document_safely();
    int marker_index = lookup_marker(marker);

    if (markers_array[marker_index] != 0)
        go_to_marker(marker_index);
}

/**
 * Handles memory-full condition.
 * Displays an error and returns to the editor loop.
 */
void memory_full(void)
{
    show_memory_full_error();
    longjmp(env, JMP_EDITOR);
}

/**
 * Common handler for setting numbered markers.
 * Writes the buffer, looks up the marker index, and sets it.
 * @param marker_char marker character
 */
static void set_marker_common(uint8_t marker_char)
{
    write_line_back_to_document_safely();
    int marker_index = lookup_marker(marker_char);

    set_marker(marker_index);
}

/**
 * Sets a marker.
 * Stores the current position in the given marker and marks display for update.
 * @param marker_idx marker index
 */
static void set_marker(uint8_t marker_idx)
{
    set_marker_to_here(marker_idx);
    display_start_row = 1;
    update_line_length();
}

/**
 * Jumps to a marker.
 * Moves the cursor to the address stored in the marker.
 * @param marker_idx marker index
 */
static void go_to_marker(uint8_t marker_idx)
{
    move_cursor_to_address(markers_array[marker_idx]);
    display_start_row = 1;
    update_line_length();
}

/**
 * Updates the stored line length.
 * Copies the current screen width into line_lengths for the current row.
 */
static void update_line_length(void)
{ line_lengths[ypos] = screen_maxcolumn; }

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
    remaining_cols -= 2;
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

    while (remaining_cols != 0)
    {
        screen_putchar(' ');
        remaining_cols--;
    }
    edit_buffer_unpacked_flag = false;
    edit_buffer_contains_ruler_flag = false;
    clear_cmd();

    do
    {
        beep();
        key_code = screen_getchar();
    } while (key_code != 0x1b);
    cursor_on();
    status_line_needs_redrawing_flag = true;
    display_start_row = 1;
}

/**
 * Appends a byte to the output buffer.
 * Writes the byte if space remains in the editor output buffer.
 * @param byte_to_append byte to append
 */
static void append_to_output_buffer(uint8_t byte_to_append)
{
    if (editor_output_pos >= MAX_LINE_LENGTH)
        return;
    output_buffer[editor_output_pos] = byte_to_append;
    editor_output_pos++;
}

/**
 * Reads the next source byte, normalizing soft spaces and margin stops.
 * @param space_flag carried space state, updated while reading controls
 * @param margin_seen whether a left-margin stop has already been processed
 * @param direct_space output true for a space emitted directly by a control
 * @return normalized source byte
 */
static uint8_t read_format_character(
    uint8_t* space_flag, bool* margin_seen, bool* direct_space)
{
    uint8_t src_pos = format_src_index;
    uint8_t cur_byte;
    *direct_space = false;
    for (;;)
    {
        cur_byte = current_line_ptr[src_pos++];
        format_src_index = src_pos;
        if (cur_byte == 9)
        {
            bool is_tab = false;
            (void)process_document_character(cur_byte, space_flag, &is_tab);
            (*space_flag)--;
            column_position += *space_flag;
            break;
        }
        if (cur_byte != 0x1a && cur_byte != 0x0b)
            break;

        if (cur_byte == 0x0b)
        {
            *space_flag = input_buffer_offset;
            if (*space_flag == 0 && !(*margin_seen))
            {
                *margin_seen = true;
                uint8_t left_stop = ruler_left_stop;
                if (left_stop == 0)
                    continue;
                if (column_position < ruler_left_stop)
                    column_position = left_stop - 1;
                scratch_offset += left_stop;
                break;
            }
        }
        if (justify_gap_count != 0)
            continue;
        cur_byte = 0x20;
        *direct_space = true;
        break;
    }

    return cur_byte;
}

/**
 * Appends a formatted byte and updates column and gap state.
 * @param cur_byte normalized source byte
 * @param direct_space whether to bypass ordinary space suppression
 * @param space_flag carried space state, updated for ordinary bytes
 * @param output_pos output buffer position, advanced when a byte is appended
 * @return true if the line has reached its wrapping limit
 */
static bool append_format_character(uint8_t cur_byte,
    bool direct_space,
    uint8_t* space_flag,
    uint8_t* output_pos)
{
    *output_pos = editor_output_pos;
    if (!direct_space)
        *space_flag = (cur_byte == 0x20);
    bool previous_gap = (justify_gap_count & 0x80) != 0;
    if (direct_space || cur_byte != 0x20 || !previous_gap)
    {
        current_line_buffer.text[(*output_pos)++] = cur_byte;
        if (cur_byte == 0x20)
            bottom_margin = (uint8_t)(bottom_margin >> 1) | 0x80;
        if (check_for_control_code(cur_byte) == NO_CONTROL_CODE)
            column_position++;
    }
    justify_gap_count = *space_flag;

    if (!previous_gap && cur_byte != 0x20 &&
        (*output_pos >= MAX_LINE_LENGTH + 1 ||
            (bottom_margin != 0 && column_position >= scratch_offset)))
        return true;
    if (*output_pos >= 0x86)
        (*output_pos)--;
    return false;
}

/**
 * Removes the overflowing word and backs up the source to its preceding space.
 * @param output_pos end of the accumulated output
 * @return true if a nonempty line remains to be written
 */
static bool trim_formatted_word(uint8_t output_pos)
{
    format_src_index++;
    uint8_t removed_byte;
    do
    {
        format_src_index--;
        output_pos--;
        if (output_pos == 0)
            return false;
        removed_byte = current_line_buffer.text[output_pos];
        current_line_buffer.text[output_pos] = 0x10;
    } while (removed_byte != 0x20);
    return true;
}

/**
 * Builds one output line, handling source markers and backing up on overflow.
 * @param space_flag space state carried across output lines
 * @param paragraph_ended output true if the paragraph ended before overflow
 * @return true if a formatted line is ready to be written
 */
static bool build_formatted_line(uint8_t* space_flag, bool* paragraph_ended)
{
    scratch_offset = ruler_right_stop - ruler_left_stop + 2;
    wipe_buffer(0x10, edit_buffer_base);
    uint8_t* marker_base = current_line_ptr;
    uint8_t output_pos = 0;
    bool margin_seen = false;
    *paragraph_ended = false;
    format_src_index = 0;
    column_position = 0;
    justify_gap_count = 0;
    bottom_margin = 0;

    for (;;)
    {
        editor_output_pos = output_pos;
        uint8_t src_pos = format_src_index;
        for (;;)
        {
            uint8_t marker = find_marker_at_position(src_pos, marker_base);
            if (marker == NUM_MARKERS)
                break;
            markers_array[marker] = 0;
            line_format_status_flag = true;
        }

        bool direct_space;
        uint8_t cur_byte =
            read_format_character(space_flag, &margin_seen, &direct_space);

        if (cur_byte == 0x0d)
        {
            src_pos = format_src_index - 1;
            if (src_pos == 0)
                return false;
            if (find_next_word_boundary(src_pos))
            {
                *paragraph_ended = true;
                break;
            }
            cur_byte = 0x20;
            input_buffer_offset = cur_byte;
        }

        if (append_format_character(
                cur_byte, direct_space, space_flag, &output_pos))
            break;
    }

    return *paragraph_ended || trim_formatted_word(output_pos);
}

/**
 * Formats the current paragraph.
 * Builds output lines with margin and tab handling, backs up to a word boundary
 * on overflow, and flushes each completed line before continuing.
 * @return FORMAT_OK, FORMAT_AT_END, or FORMAT_MEMORY_FULL
 */
format_result_t format_paragraph(void)
{
    uint8_t space_flag = 0;
    cursor_moved_flag = true;
    print_xpos = 4;
    input_buffer_offset = 0;
    line_format_status_flag = false;

    if (check_for_command_prefix(*current_line_ptr) != NO_COMMAND_PREFIX)
        return advance_to_next_doc_line();

    for (;;)
    {
        if ((format_mode_flags & 0x81) != 0 || ruler_right_stop == 0 ||
            ruler_right_stop < ruler_left_stop)
            return advance_to_next_doc_line();

        bool paragraph_ended;
        if (!build_formatted_line(&space_flag, &paragraph_ended))
            return advance_to_next_doc_line();
        insert_at_left_margin();
        if (!paragraph_ended)
            justify_edit_buffer(edit_buffer_base);
        if (flush_formatted_line())
            return FORMAT_MEMORY_FULL;
        format_result_t result = advance_to_next_doc_line();
        if (paragraph_ended || !line_format_status_flag)
            return result;
    }
}

/**
 * Flushes the current formatted line to the document.
 * Writes the edit buffer back and reports memory exhaustion.
 * @return true if the write failed due to insufficient memory, false otherwise
 */
static bool flush_formatted_line(void)
{
    line_format_status_flag = true;
    edit_line_len = format_src_index - 1;
    edit_buffer_unpacked_flag = true;
    return write_line_back_to_document();
}

/**
 * Advances to the next document line.
 * Zeroes xpos and moves current_line_ptr past the current CR.
 * @return FORMAT_AT_END at the end of the document, otherwise FORMAT_OK
 */
static format_result_t advance_to_next_doc_line(void)
{
    xpos = 0;
    uint8_t* line_ptr;
    uint8_t line_len;
    bool end_of_document =
        find_next_line(current_line_ptr, &line_ptr, &line_len);

    if (!end_of_document)
        current_line_ptr = line_ptr + line_len;

    return end_of_document ? FORMAT_AT_END : FORMAT_OK;
}

/**
 * Copies the source bytes to output_buffer and counts gaps between words.
 * @param last_column output for the display column of the last word character
 * @param char_width output for the last processed character width
 * @return false if trailing-space adjustment underflowed the gap count
 */
static bool scan_justification_gaps(uint8_t* last_column, uint8_t* char_width)
{
    uint8_t pos = 0;
    uint8_t cur_char;
    bool handled = process_char_for_output(pos, true, char_width, &cur_char);
    bool in_word = !handled && cur_char != 0x20;

    for (;;)
    {
        if (in_word)
            *last_column = column_position;
        pos++;
        if (pos == justify_line_length)
        {
            if (!in_word)
            {
                justify_gap_count--;
                if (justify_gap_count & 0x80)
                    return false;
            }
            return true;
        }
        handled = process_char_for_output(pos, !in_word, char_width, &cur_char);
        if (in_word && !handled && cur_char == 0x20)
            justify_gap_count++;
        in_word = !handled && cur_char != 0x20;
    }
}

/**
 * Distributes padding between gaps, rotating the starting gap between lines.
 * @param extra_spaces total number of extra spaces to insert
 */
static void distribute_justification_spaces(uint8_t extra_spaces)
{
    screen_row = extra_spaces;
    justify_running_total_accum = extra_spaces % justify_gap_count;
    justify_extra_space_accum = extra_spaces / justify_gap_count;
    memset(input_buffer, 0, justify_gap_count);
    uint8_t gap_pos = print_xpos;

    gap_pos++;

    if (gap_pos >= justify_gap_count)
        gap_pos = 1;
    gap_pos--;
    uint8_t gaps_to_fill = justify_gap_count;

    do
    {
        uint8_t gap_spaces = justify_running_total_accum;

        if (gap_spaces != 0)
        {
            gap_spaces = 1;
            justify_running_total_accum--;
        }
        gap_spaces += justify_extra_space_accum;
        input_buffer[gap_pos] = gap_spaces;
        screen_row -= input_buffer[gap_pos];
        gap_pos++;

        if (gap_pos >= justify_gap_count)
            gap_pos = 0;

        if (screen_row == 0)
            break;
        gaps_to_fill--;
    } while (gaps_to_fill != 0);
    print_xpos = gap_pos;
}

/**
 * Rebuilds the line from saved bytes and the per-gap padding in input_buffer.
 * @param target_ptr buffer to clear before rebuilding the current line
 * @return source position after the last copied byte
 */
static uint8_t rebuild_justified_line(uint8_t* target_ptr)
{
    scratch_index = 0;
    column_position = 0;
    wipe_buffer(0x1a, target_ptr);
    memcpy(current_line_buffer.text, output_buffer, justify_overflow_counter);
    uint8_t dest_pos = justify_overflow_counter;
    uint8_t src_pos = justify_overflow_counter;

    do
    {
        uint8_t cur_byte = output_buffer[src_pos];

        if (cur_byte == 0x20)
        {
            uint8_t gap_spaces = scratch_index;

            if (gap_spaces != 0)
            {
                uint8_t gap = column_position;

                if (gap < justify_gap_count)
                    gap_spaces = input_buffer[gap];
                gap_spaces += dest_pos;
                column_position++;
                dest_pos = gap_spaces;
                scratch_index = 0;
            }
            cur_byte = 0x20;
        }
        else
        {
            scratch_index++;
        }
        current_line_buffer.text[dest_pos] = cur_byte;
        dest_pos++;
        src_pos++;
    } while (src_pos != justify_line_length);

    while (dest_pos < MAX_LINE_LENGTH)
    {
        current_line_buffer.text[dest_pos] = 0x10;
        dest_pos++;
    }
    return src_pos;
}

/**
 * Justifies the edit buffer line.
 * Counts word gaps, distributes padding, and rebuilds the line to the right
 * margin.
 * @param target_ptr buffer to justify
 * @return last processed character width if skipped, otherwise source line
 * length
 */
uint8_t justify_edit_buffer(uint8_t* target_ptr)
{
    uint8_t char_width = 0;
    uint8_t last_column = 0;
    if (justifying_flag)
        return char_width;
    justify_gap_count = 0;
    column_position = 0;
    justify_overflow_counter = 0;
    if (ruler_right_stop == 0)
        return char_width;
    justify_line_length = get_line_length();

    if (!scan_justification_gaps(&last_column, &char_width) ||
        justify_gap_count == 0 || ruler_right_stop < last_column)
        return char_width;

    uint8_t extra_spaces = (uint8_t)(ruler_right_stop - last_column + 1);
    uint8_t expanded_length = (uint8_t)(extra_spaces + justify_line_length);
    if (expanded_length >= MAX_LINE_LENGTH)
        extra_spaces -= expanded_length - MAX_LINE_LENGTH;
    distribute_justification_spaces(extra_spaces);
    return rebuild_justified_line(target_ptr);
}

/**
 * Processes a character for justification output.
 * Saves the original byte, expands tabs and margin stops, and updates columns.
 * @param buf_idx index in the line buffer
 * @param carry_in bit shifted into screen_column
 * @param char_width_out output for tab width or margin calculation scratch;
 *        unchanged for ordinary characters
 * @param out_char output for the normalized byte, or zero after positioning
 * @return true after tab/margin positioning or column counter overflow
 */
static bool process_char_for_output(
    uint8_t buf_idx, bool carry_in, uint8_t* char_width_out, uint8_t* out_char)
{
    screen_column = (screen_column >> 1) | (carry_in ? 0x80 : 0);
    *out_char = current_line_buffer.text[buf_idx];
    output_buffer[buf_idx] = *out_char;
    bool positioned = false;

    if (*out_char == 9)
    {
        bool is_tab = false;
        (void)process_document_character(*out_char, char_width_out, &is_tab);
        *out_char = *char_width_out + column_position;
        positioned = *out_char != 0;
    }
    else if (*out_char == 0x0b)
    {
        *out_char = ruler_left_stop;
        if (ruler_left_stop != 0)
        {
            *char_width_out = column_position;
            if (*char_width_out != 0 && *char_width_out >= ruler_left_stop)
            {
                (*char_width_out)++;
                *out_char = *char_width_out;
            }
            positioned = true;
        }
    }

    if (positioned)
    {
        column_position = *out_char;
        justify_overflow_counter = buf_idx + 1;
        *out_char = 0;
        justify_gap_count = 0;
        return true;
    }
    if (*out_char < 0x1b)
        *out_char = 0x20;
    if (*out_char < 0x20)
        return false;
    column_position++;
    return column_position == 0;
}

/**
 * Checks whether formatting should stop before the next source line.
 * Examines leading spaces, margin stops, and tabs to decide whether that line
 * can continue the paragraph; records a leading margin stop in screen_column.
 * @param src_idx index of the current line's carriage return
 * @return true if the next line should start a separate paragraph
 */
static bool find_next_word_boundary(uint8_t src_idx)
{
    uint8_t* next_line = current_line_ptr + src_idx + 1;
    uint8_t pos = 0;
    bool leading_margin = false;
    screen_column = 0;

    do
    {
        uint8_t ch = next_line[pos];
        if (ch == 0 || check_for_command_prefix(ch) != NO_COMMAND_PREFIX ||
            ch == 0x0d)
            return true;

        if (pos == 0)
        {
            uint8_t* scan = next_line;
            while (*scan == 0x0b)
            {
                leading_margin = true;
                scan++;
            }
            if (*scan == 9)
                return true;
        }

        if (ch == 0x20)
        {
            if (ruler_left_stop == 0 || !leading_margin || screen_column != 0)
                return true;
        }
        else
        {
            if (ch != 0x0b)
                break;
            screen_column = ch;
        }
        pos++;
    } while (pos != 0);

    return ruler_left_stop != 0 && leading_margin && screen_column == 0;
}

/**
 * Inserts a character into the edit buffer.
 * Makes room and stores the character at the cursor.
 * @param ch character to insert
 * @return true on success
 */
static bool insert_character_into_edit_buffer(uint8_t ch)
{
    if (!insert_edit_buffer_bytes_at_xpos(1))
        return false;
    current_line_buffer.text[xpos] = ch;
    line_counter++;
    return true;
}

/**
 * Sets the cursor to the end of the line.
 * Updates xpos to the current line length.
 */
static void set_xpos_to_line_length(void)
{ xpos = get_line_length(); }

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
 * find_left_margin_stop: Finds the left margin stop (0x0b) in the edit line.
 *
 * @return the position after the 0x0b in the edit line, or -1 if no margin
 *         stop was found.
 */
static int find_left_margin_stop(void)
{
    if (ruler_left_stop == 0)
        return 0;
    for (uint8_t pos = 0; pos < MAX_LINE_LENGTH; pos++)
        if (current_line_buffer.text[pos] == 0x0b)
            return pos + 1;
    return -1;
}

/**
 * Inserts a margin byte at a position.
 * Inserts a single margin tab at the given position.
 * @param insert_pos insertion position
 * @return true on success
 */
static bool insert_byte_at_xpos(uint8_t insert_pos)
{
    uint8_t saved_xpos = xpos;
    xpos = insert_pos;
    bool inserted = insert_edit_buffer_bytes_at_xpos(1);
    if (inserted)
        current_line_buffer.text[xpos] = 0x0b;
    xpos = saved_xpos;
    return inserted;
}

/**
 * Inserts bytes at the cursor in the edit buffer.
 * Retargets markers and shifts existing content right without filling the gap.
 * @param count number of bytes to insert
 * @return true on success, false if the cursor or expanded line exceeds the
 * buffer
 */
static bool insert_edit_buffer_bytes_at_xpos(uint8_t count)
{
    if (xpos >= MAX_LINE_LENGTH)
    {
        beep();
        return false;
    }
    scratch_offset = count;
    unsigned int new_length = (unsigned int)get_line_length() + count;
    if (new_length > MAX_LINE_LENGTH)
    {
        beep();
        return false;
    }

    edit_buffer_dirty_flag = true;
    uint8_t* marker_base = current_line_buffer.text;
    uint8_t scan_pos = MAX_LINE_LENGTH;
    do
    {
        scan_pos--;
        unsigned int shifted_pos = (unsigned int)scan_pos + scratch_offset;
        scratch_index = shifted_pos < MAX_LINE_LENGTH ? shifted_pos : 0;
        for (;;)
        {
            uint8_t marker = find_marker_at_position(scan_pos, marker_base);
            if (marker == NUM_MARKERS)
                break;
            markers_array[marker] =
                scratch_index ? current_line_buffer.text + scratch_index : NULL;
        }
    } while (scan_pos != xpos);

    int copy_len = MAX_LINE_LENGTH - (int)xpos - (int)scratch_offset;
    if (copy_len > 0)
        memmove(&current_line_buffer.text[xpos + scratch_offset],
            &current_line_buffer.text[xpos],
            (size_t)copy_len);
    return true;
}

/**
 * Invalidates both edit-buffer flags, redraws, and writes the buffer back.
 */
void redraw_and_write_back(void)
{
    edit_buffer_unpacked_flag = false;
    edit_buffer_contains_ruler_flag = false;
    redraw_editor();
    write_line_back_to_document_safely();
}

/**
 * Scrolls down if the current line immediately precedes the visible text.
 * @return true if one row was scrolled
 */
static bool scroll_editor_down_one_line(void)
{
    ruler_index_ptr = saved_ruler_index_scroll;
    if (top_of_screen_line_ptr < top)
    {
        uint8_t* previous_line;
        find_previous_line(top_of_screen_line_ptr, &previous_line);
        scratch_line_ptr = previous_line;
        if (scratch_line_ptr == current_line_ptr)
        {
            top_of_screen_line_ptr = scratch_line_ptr;
            uint8_t row = screen_maxrow;
            do
            {
                line_lengths[row] = line_lengths[row - 1];
                row--;
            } while (row != 0);
            screen_scrolldown();
            screen_setcursor(0, 1);
            return true;
        }
    }
    return false;
}

/**
 * Scrolls up one row to reveal the line below the visible text.
 */
static void scroll_editor_up_one_line(void)
{
    // The current line is just below the visible rows.
    uint8_t row = 0;
    do
    {
        line_lengths[row] = line_lengths[row + 1];
        row++;
    } while (row != screen_maxrow);
    ptr6_screen_row--;
    line_lengths[0] = screen_maxcolumn;
    ruler_index_ptr = saved_ruler_index_scroll;
    uint8_t* next_line;
    uint8_t next_line_len;
    advance_to_next_line(top_of_screen_line_ptr, &next_line, &next_line_len);
    scratch_line_ptr = next_line;
    top_of_screen_line_ptr += next_line_len;
    screen_scrollup();
    screen_setcursor(0, screen_maxrow);
}

/**
 * Scans visible lines and tracks the first row needing a partial redraw.
 * @return current line or end-of-document row, or screen_maxrow + 1 if below
 * screen
 */
static uint8_t find_current_line_screen_row(void)
{
    uint8_t* walk = top_of_screen_line_ptr;
    uint8_t cursor_row = 0;
    do
    {
        cursor_row++;
        if (walk == editor_ptr6)
            ptr6_screen_row = cursor_row;
        if (walk == current_line_ptr)
            break;

        uint8_t* next_line;
        uint8_t next_line_len;
        if (advance_to_next_line(walk, &next_line, &next_line_len))
            break;
        scratch_line_ptr = next_line + next_line_len;
        walk = scratch_line_ptr;
    } while (cursor_row <= screen_maxrow);

    return cursor_row;
}

/**
 * Keeps the current line visible and updates its screen row.
 * @return true if a single-row scroll requires a status refresh
 */
static bool position_editor_viewport(void)
{
    bool scrolled = false;
    uint8_t cursor_row = 1;

    if (current_line_ptr < top_of_screen_line_ptr)
    {
        if (scroll_repeat_count == 0)
            scrolled = scroll_editor_down_one_line();
        if (!scrolled)
            (void)compute_display_start_line();
    }
    else
        ruler_index_ptr = saved_ruler_index_scroll;

    if (!scrolled)
    {
        for (;;)
        {
            cursor_row = find_current_line_screen_row();

            if (cursor_row <= screen_maxrow)
                break;
            if (scroll_repeat_count != 0)
            {
                (void)compute_display_start_line();
                continue;
            }

            scroll_editor_up_one_line();
            cursor_row = screen_maxrow;
            scrolled = true;
            break;
        }
    }

    if (scrolled)
    {
        saved_ruler_index_scroll = ruler_index_ptr;
        line_counter++;
    }
    ypos = cursor_row;
    return scrolled;
}

/**
 * Restores the current ruler and keeps the cursor within the horizontal
 * viewport.
 * @return true if horizontal scrolling requires a full redraw
 */
static bool adjust_editor_horizontal_scroll(void)
{
    load_current_ruler(saved_ruler_index_redraw);
    unpack_line_into_buffer(edit_buffer_base);
    recalculate_cursor_xpos();
    screen_column = screen_maxcolumn >> 1;

    if (visual_column < hscroll_pos ||
        (int)visual_column > (int)hscroll_pos + (int)screen_maxcolumn - 4)
    {
        hscroll_pos =
            visual_column < screen_column ? 0 : visual_column - screen_column;
        display_start_row = 1;
        write_line_back_to_document_safely();
        return true;
    }
    return false;
}

/**
 * Renders invalidated document rows, clears rows past the end, and restores the
 * ruler.
 */
static void redraw_editor_rows(void)
{
    uint8_t rows_remaining;
    uint8_t* draw;
    if ((int8_t)display_start_row < 0 && (int8_t)ptr6_screen_row >= 0 &&
        editor_ptr6 != NULL)
    {
        screen_row = ptr6_screen_row;
        rows_remaining = screen_maxrow - ptr6_screen_row + 1;
        draw = editor_ptr6;
    }
    else
    {
        load_current_ruler(saved_ruler_index_scroll);
        screen_row = 1;
        draw = top_of_screen_line_ptr;
        rows_remaining = screen_maxrow;
    }
    scratch_index = rows_remaining;

    do
    {
        render_state_t rs = {.line = screen_row};
        draw_line(&rs, draw);
        uint8_t next_len = 0;
        uint8_t* next_line = scratch_line_ptr;
        if (advance_to_next_line(next_line, &next_line, &next_len))
        {
            scratch_index--;
            if (scratch_index != 0)
            {
                // Mark the end of the document, then clear the other rows.
                screen_column = (uint8_t)(screen_maxcolumn + 1);
                line_lengths[screen_row + 1] = screen_column;
                uint8_t fill = 0x2a;
                do
                {
                    screen_row++;
                    screen_setcursor(0, screen_row);
                    clear_to_eol(fill, screen_row);
                    line_lengths[screen_row] = screen_column;
                    screen_column = 0;
                    fill = 0x20;
                    scratch_index--;
                } while (scratch_index != 0);
            }
            break;
        }
        scratch_line_ptr = next_line + next_len;
        draw = scratch_line_ptr;
        screen_row++;
        scratch_index--;
    } while (scratch_index != 0);

    line_counter = 0;
    load_current_ruler(saved_ruler_index_redraw);
}

/**
 * Clears pending redraw state and restores the visible cursor.
 */
static void finish_editor_redraw(void)
{
    uint8_t cursor_column = visual_column - hscroll_pos + 3;
    display_start_row = 0;
    line_counter = 0;
    scroll_repeat_count = 0;
    editor_ptr6 = &ram[RAM_MAX];
    screen_setcursor(cursor_column, ypos);
    cursor_on();
}

/**
 * Redraws the editor display and places the cursor.
 * Positions the viewport, renders invalidated rows, and refreshes the ruler
 * and status indicators before restoring the cursor.
 */
static void redraw_editor(void)
{
    cursor_off();
    saved_ruler_index_redraw = ruler_index_ptr;
    bool status_needs_redraw = status_line_needs_redrawing_flag;

    if (!edit_buffer_unpacked_flag || display_start_row != 0 ||
        scroll_repeat_count != 0)
    {
        if (position_editor_viewport())
            status_needs_redraw = true;
    }
    if (adjust_editor_horizontal_scroll())
        status_needs_redraw = true;
    status_line_needs_redrawing_flag = status_needs_redraw;

    if (display_start_row != 0)
        redraw_editor_rows();

    unpack_line_into_buffer(edit_buffer_base);
    update_markers_to_format_buffer();
    draw_ruler();
    if (line_counter != 0)
    {
        screen_row = ypos;
        render_state_t rs = {.line = screen_row};
        uint8_t* draw_ptr = get_format_line_contents();
        draw_line(&rs, draw_ptr);
    }
    if (flags_need_redrawing_flag)
        draw_status_word();
    finish_editor_redraw();
}

/**
 * Disables the cursor.
 * Hides the cursor via the screen driver.
 */
void cursor_off(void)
{ screen_enablecursor(0); }

/**
 * Enables the cursor.
 * Shows the cursor via the screen driver.
 */
static void cursor_on(void)
{ screen_enablecursor(1); }

/**
 * Draws the ruler line.
 * Renders the current ruler at the top row if a redraw is needed.
 */
static void draw_ruler(void)
{
    if (!status_line_needs_redrawing_flag)
        return;
    status_line_needs_redrawing_flag = false;
    render_state_t rs = {.line = 0};

    draw_line(&rs, current_ruler_ptr);
    flags_need_redrawing_flag = true;
}

/**
 * Renders a document line to the screen.
 * Formats and draws a line with correct highlighting and horizontal scrolling.
 * @param rs render state
 * @param addr address of the document line
 */
static void draw_line(render_state_t* rs, uint8_t* addr)
{
    rs->line_ptr = addr;
    scratch_line_ptr = addr;
    screen_setcursor(0, rs->line);
    rs->pos = 0;
    rs->col = 0;
    rs->buf_off = 0;
    rs->char_width = 0;
    command_prefix_t prefix =
        deref_and_check_for_command_prefix(0, scratch_line_ptr);

    if (prefix == NO_COMMAND_PREFIX || hscroll_pos != 0)
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
 * Clears to the end of the line.
 * Fills the remainder of the screen line with the given character.
 * @param fill_char fill character
 * @param line screen line index
 */
static void clear_to_eol(uint8_t fill_char, uint8_t line)
{
    while (line_lengths[line] != 0)
    {
        screen_putchar(fill_char);
        line_lengths[line]--;
    }
}

/**
 * Draws the status bar.
 * Shows format, justify, and insert mode indicators.
 */
static void draw_status_word(void)
{
    flags_need_redrawing_flag = false;
    home_cursor();
    char format_indicator = 'F';
    if (format_mode_flags != 0)
        format_indicator = (format_mode_flags & 0xc0) != 0 ? 'M' : ' ';
    screen_putchar(format_indicator);
    screen_putchar(justifying_flag ? ' ' : 'J');
    home_cursor();
}

/**
 * Moves the cursor to the home position.
 * Sets the cursor to row 0, column 0.
 */
void home_cursor(void)
{ screen_setcursor(0, 0); }

/**
 * Synchronizes the cursor's buffer position and visual column.
 * Normally calculates visual_column from xpos. After a line change, or if
 * column arithmetic overflows, maps visual_column back to a buffer position,
 * stopping before a tab that would overshoot the requested column.
 */
static void recalculate_cursor_xpos(void)
{
    uint8_t char_width;
    uint8_t* line_ptr = current_line_buffer.text;
    uint8_t column = 0;
    bool is_tab = false;

    if (line_change_pending_flag == 0)
    {
        uint8_t scan_pos = 0;
        do
        {
            if (scan_pos == xpos)
            {
                visual_column = column;
                return;
            }
            column_position = column;
            (void)process_current_document_character(
                line_ptr, &char_width, &scan_pos, &is_tab);
            column = (uint8_t)(column_position + char_width);
        } while (column >= column_position);
    }

    column = 0;
    line_change_pending_flag = 0;
    uint8_t buf_pos = 0;
    do
    {
        column_position = column;
        (void)process_current_document_character(
            line_ptr, &char_width, &buf_pos, &is_tab);
        column = (uint8_t)(column_position + char_width);
    } while (column < visual_column);

    if (column != visual_column)
    {
        column = column_position;
        buf_pos--;
    }
    xpos = buf_pos;
    visual_column = column;
}

/**
 * Renders a character with horizontal scroll.
 * Accounts for hscroll before delegating to render_char.
 * @param rs render state
 */
static void render_xchar(render_state_t* rs)
{
    rs->char_width++;
    if (rs->buf_off++ >= hscroll_pos)
        render_char(rs);
}

/**
 * Computes the top display line.
 * Walks backward from current_line_ptr to find the top-of-screen line.
 * @return display start adjustment
 */
static uint8_t compute_display_start_line(void)
{
    ruler_index_ptr = saved_ruler_index_redraw;
    display_start_row = screen_maxrow;
    uint8_t rows_needed = (screen_maxrow >> 1) + 1;
    if (scroll_repeat_count != 0 && (scroll_repeat_count & 0x80) == 0)
        rows_needed = ypos;

    uint8_t* line = current_line_ptr;
    while (--rows_needed != 0)
    {
        uint8_t* previous_line;
        if (!find_previous_line(line, &previous_line))
            break;
        line = previous_line;
    }
    top_of_screen_line_ptr = line;
    saved_ruler_index_scroll = ruler_index_ptr;
    ruler_index_ptr = saved_ruler_index_redraw;
    return rows_needed;
}

/**
 * Advances and renders the next character.
 * Combines character advance with rendering.
 * @param rs render state
 */
static void advance_to_next_char_and_render(render_state_t* rs)
{
    advance_to_next_char(rs);
    render_char(rs);
}

/**
 * Renders a character with attributes.
 * Handles highlighting, control codes, and clipping to screen width.
 * @param rs render state
 */
static void render_char(render_state_t* rs)
{
    if (line_lengths[rs->line] != 0)
        line_lengths[rs->line]--;
    if (rs->col >= screen_maxcolumn)
        return;
    rs->col++;

    bool inverted = false;
    if (rs->pos != 0 && find_marker_at_position(rs->pos - 1, rs->line_ptr) < 2)
    {
        screen_setstyle(STYLE_REVERSE);
        inverted = true;
    }

    uint8_t display_char = rs->ch;
    control_code_t highlight = check_for_control_code(display_char);
    if (highlight != NO_CONTROL_CODE)
    {
        display_char = highlight == HIGHLIGHT1_CODE ? '-' : '*';
        if (!inverted)
        {
            screen_setstyle(STYLE_REVERSE);
            inverted = true;
        }
    }
    else if (display_char == '\r' || display_char == '\0')
        display_char = ' ';

    screen_putchar(display_char);
    if (inverted)
        screen_setstyle(0);
}

/**
 * Advances the render state to the next character.
 * Reads and processes the next document character into the render state.
 * @param rs render state
 */
static void advance_to_next_char(render_state_t* rs)
{
    column_position = rs->char_width;
    rs->ch = process_current_document_character(
        rs->line_ptr, &rs->width, &rs->pos, &rs->prev_is_tab);
    rs->char_width = column_position;
}

/**
 * Processes the current document character and advances the position.
 * Wraps process_document_character for the editor render pipeline.
 * @param target_ptr line buffer to read from
 * @param char_width_out output for character width
 * @param pos_inout cursor into target_ptr, advanced by one
 * @param is_tab tab-state carried between characters
 * @return processed character
 */
uint8_t process_current_document_character(uint8_t* target_ptr,
    uint8_t* char_width_out,
    uint8_t* pos_inout,
    bool* is_tab)
{
    uint8_t ch = target_ptr[(*pos_inout)++];
    return process_document_character(ch, char_width_out, is_tab);
}

/**
 * Unpacks the current line into a buffer if needed.
 * Checks the unpacked flag before delegating to unpack_line.
 * @param target_ptr destination buffer
 */
static void unpack_line_into_buffer(uint8_t* target_ptr)
{
    if (edit_buffer_unpacked_flag)
        return;
    edit_buffer_unpacked_flag = true;
    unpack_line(target_ptr);
}

/**
 * Unpacks a document line into the edit buffer.
 * Fills the buffer, updates the ruler flag, handles command prefixes, and
 * copies line content.
 * @param target_ptr destination buffer
 */
static void unpack_line(uint8_t* target_ptr)
{
    wipe_buffer(0x10, target_ptr);
    clear_format_mode_bit7();
    uint8_t first_byte = *current_line_ptr;

    command_prefix_t prefix = check_for_command_prefix(first_byte);

    edit_buffer_contains_ruler_flag = prefix == RULER_PREFIX;
    if (prefix != NO_COMMAND_PREFIX)
    {
        if (edit_buffer_contains_ruler_flag)
            edit_buffer_unpacked_flag = true;
        set_format_mode_bit7();
    }
    current_format_line = target_ptr;
    heap_format_line_ptr = (line_t*)target_ptr;
    uint8_t* dest = (prefix != NO_COMMAND_PREFIX)
                        ? current_format_line
                        : current_format_line + offsetof(line_t, text);
    uint8_t copy_pos = 0;

    do
    {
        uint8_t ch = current_line_ptr[copy_pos];

        if (ch == 0x0d)
            break;
        dest[copy_pos] = ch;
        copy_pos++;
    } while (copy_pos != 0);
    edit_line_len = copy_pos;
    target_ptr[0x89] = 0x0d;
}

/**
 * Clears format-mode bit 7.
 * Clears the high bit of the format mode flag and marks redraw if changed.
 */
void clear_format_mode_bit7(void)
{
    uint8_t old = format_mode_flags;

    format_mode_flags &= ~0x80;

    if (old != format_mode_flags)
        flags_need_redrawing_flag = true;
}

/**
 * Sets format-mode bit 7.
 * Sets the high bit of the format mode flag and marks redraw if changed.
 */
static void set_format_mode_bit7(void)
{
    uint8_t old = format_mode_flags;

    format_mode_flags |= 0x80;

    if (old != format_mode_flags)
        flags_need_redrawing_flag = true;
}
