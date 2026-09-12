#include "document.h"
#include "io.h"
#include "printing.h"
#include <ctype.h>
#include <stdio.h>

/**
 * Check whether a byte is a command or ruler prefix.
 * @param ch byte to test
 * @return COMMAND_PREFIX for 0x80, RULER_PREFIX for 0x81, NO_COMMAND_PREFIX
 * otherwise
 */
command_prefix_t check_for_command_prefix(uint8_t ch)
{
    if (ch == COMMAND_BYTE)
        return COMMAND_PREFIX;
    if (ch == RULER_BYTE)
        return RULER_PREFIX;
    return NO_COMMAND_PREFIX;
}

/**
 * Check whether a character is a highlight control code.
 * @param cur_ch character to test
 * @return HIGHLIGHT1_CODE for 0x1c, HIGHLIGHT2_CODE for 0x1d, NO_CONTROL_CODE
 * otherwise
 */
control_code_t check_for_control_code(uint8_t cur_ch)
{
    if (cur_ch == 0x1c)
        return HIGHLIGHT1_CODE;
    if (cur_ch == 0x1d)
        return HIGHLIGHT2_CODE;
    return NO_CONTROL_CODE;
}

/**
 * Compute free bytes between document top and himem.
 * @return number of free bytes (himem - top)
 */
int compute_bytes_free(void)
{
    return (int)(himem - top);
}

/**
 * Ensure at least 150 bytes are free.
 * Displays a memory error and does not return if less than 150 bytes remain.
 */
void check_for_at_least_150_bytes_free(void)
{
    if (compute_bytes_free() >= 0x96)
        return;
    display_not_enough_memory();
}

/**
 * Dereference a document pointer at an offset and test for command prefix.
 * @param pos offset from target_ptr
 * @param target_ptr base pointer into document memory
 * @return COMMAND_PREFIX, RULER_PREFIX, or NO_COMMAND_PREFIX
 */
command_prefix_t deref_and_check_for_command_prefix(
    uint8_t pos, uint8_t* target_ptr)
{
    uint8_t cur_ch = target_ptr[pos];
    return check_for_command_prefix(cur_ch);
}

/**
 * Display the current document file state.
 * Prints "Editing No File" or the input/output filenames and stops any
 * active printing.
 */
void display_document_file_state(void)
{
    uint8_t next_ch;
    stop_printing();
    cli_putstring("Editing ");
    if (file_edit_flags == 0)
    {
        cli_putstring("No File\n");
        return;
    }

    uint8_t pos = 0;
    for (;;)
    {
        next_ch = input_filename[pos];
        if (next_ch == 0x0d)
            break;
        cli_putchar(next_ch);
        pos++;
    }
    if ((file_edit_flags & 0x40))
        goto c8a19;
    cli_putstring(" to ");
    pos = 0;
    for (;;)
    {
        next_ch = output_filename[pos];
        pos++;
    c8a19:
        cli_putchar(next_ch);
        if (next_ch == 0x0d)
            break;
    }
}

/**
 * Scan the current ruler buffer for left and right margin stops.
 * Sets ruler_left_stop to the position of '>' and ruler_right_stop to the
 * position of '<'. If the left stop is not strictly before the right stop,
 * both are reset to zero. Also updates ruler_buffer_len.
 */
void find_margins_of_current_ruler_buffer(void)
{
    uint8_t pos = 0;
    ruler_right_stop = 0;
    ruler_left_stop = 0;
    do
    {
        uint8_t cur_ch = current_ruler_ptr[pos];
        if (cur_ch == 0x3e)
            ruler_left_stop = pos;
        if (cur_ch == 0x3c)
            ruler_right_stop = pos;
        if (cur_ch == 0x0d)
            break;
        pos++;
    } while (pos != MAX_LINE_LENGTH);
    ruler_buffer_len = pos;
    if (ruler_left_stop < ruler_right_stop)
        return;
    ruler_right_stop = 0;
    ruler_left_stop = 0;
}

/**
 * Print a character with alignment handling.
 * Spaces increment the pending alignment count; carriage returns reset it.
 * Other characters are flushed via alignment and then rendered.
 * @param cur_ch character to print
 */
void print_char(uint8_t cur_ch)
{
    if (cur_ch == 0x20)
    {
        print_xpos++;
        return;
    }
    if (cur_ch == 0x0d)
        print_xpos = 0;
    print_alignment_spaces(cur_ch);
    print_char_just_to_screen(cur_ch);
}

/**
 * Render a character directly to screen or printer.
 * If printer output is enabled, delegates to the printer driver.
 * Otherwise handles highlight codes by rendering '-' or '*' in reverse
 * video and translates carriage return to newline.
 * @param cur_ch character to render
 */
void print_char_just_to_screen(uint8_t cur_ch)
{
    if ((print_flags & 0x80))
    {
        printer_driver_ptr->print_char(cur_ch);
        return;
    }
    control_code_t cc = check_for_control_code(cur_ch);
    if (!(cc == NO_CONTROL_CODE))
    {
        {
            uint8_t saved_a = cur_ch;
            cur_ch = (cc == HIGHLIGHT1_CODE) ? 0x2d : 0x2a;
            screen_setstyle(STYLE_REVERSE);
            cli_putchar(cur_ch);
            cur_ch = saved_a;
        }
        screen_setstyle(0);
        return;
    }
    if (cur_ch == 0x0d)
    {
        cli_putchar('\n');
        return;
    }
    cli_putchar(cur_ch);
}

/**
 * Process one document character, performing tab, ruler and highlight-code
 * expansion.
 * @param cur_ch character from the current edit line to process
 * @param idx on return, holds 1 on ordinary paths or the tab offset (index of
 * the first '*' ruler stop beyond column_position) on the tab path
 * @param is_tab on entry, the previous tab-expansion state for this walk; on
 * return, true if tab expansion was performed for cur_ch, false otherwise
 * @return processed character, normally 0x20 (space); tabs and characters below
 * 0x1a map to space, and characters in [0x1a, 0x20) map to highlight codes when
 * printer output is enabled
 */
uint8_t process_document_character(uint8_t cur_ch, uint8_t* idx, bool* is_tab)
{
    if (!(cur_ch == 9))
    {
        if ((cur_ch == 0x10) || cur_ch == 0x1a)
            goto ca5d5;
        if (cur_ch == 0x0b)
            goto ca5d9;
        if (cur_ch > 0x1a)
        {
            if (cur_ch < 0x20)
            {
                if ((print_flags & 0x80))
                {
                    cur_ch = (uint8_t)(cur_ch - 0x1b - 1);
                    *idx = cur_ch;
                    cur_ch = highlight_code[*idx];
                }
            }
        }
    ca5d1:
        *idx = 1;
        *is_tab = false;
        return cur_ch;
    ca5d5:
        do
        {
            cur_ch = 0x20;
            goto ca5d1;
        ca5d9:
            cur_ch = ruler_left_stop;
        } while (cur_ch == 0);
        cur_ch--;
    }
    else
    {
        uint8_t tab_pos = column_position;
        do
        {
            tab_pos++;
            if (tab_pos >= ruler_buffer_len)
                goto ca5f8;
            cur_ch = current_ruler_ptr[tab_pos];
        } while (cur_ch != 0x2a);
        cur_ch = tab_pos;
    }
    {
        bool no_borrow = (cur_ch >= column_position);
        cur_ch -= column_position;
        *idx = cur_ch;
        if (*idx == 0)
            goto ca5f8;
        if (no_borrow)
            goto ca5fa;
    }
ca5f8:
    *idx = 1;
ca5fa:
    cur_ch = 0x20;
    *is_tab = true;
    return cur_ch;
}

/**
 * Return control to the CLI prompt via longjmp.
 */
void return_to_cli_prompt(void)
{
    longjmp(env, JMP_CLI);
}

/**
 * Flush pending alignment spaces to the output.
 * Prints print_xpos spaces and resets the counter.
 * @param cur_ch unused, retained for call-site compatibility
 */
void print_alignment_spaces(uint8_t cur_ch)
{
    cur_ch = print_xpos;
    if (cur_ch == 0)
        return;

    do
    {
        print_char_just_to_screen(' ');
        print_xpos--;
    } while (print_xpos != 0);
}

/**
 * Load the ruler at the given index offset and recompute margins.
 * @param pos byte offset into the ruler index stack
 */
void load_current_ruler(uint8_t pos)
{
    ruler_index_ptr = pos;
    current_ruler_ptr = ruler_index[pos >> 1] + 3;
    find_margins_of_current_ruler_buffer();
}

/**
 * Ensure the document contains at least one carriage return.
 * If the document is empty (page == top), inserts a CR at page and a
 * terminating NUL at top.
 */
void ensure_cr_at_document_top(void)
{
    if (page != top)
        return;
    top++;
    current_line_ptr = page;
    page[0] = 0x0d;
    top[0] = 0;
}

/**
 * Close the currently open file if any.
 */
void close_file(void)
{
    if (file_ptr)
    {
        fclose(file_ptr);
        file_ptr = NULL;
    }
}

/**
 * Create a default ruler with tab stops every six columns.
 * Fills the buffer with '.' and '*' tab markers and terminates with '<'.
 * @param ruler_addr destination buffer for the ruler
 * @return offset of the terminating '<' character
 */
uint8_t create_default_ruler(uint8_t* ruler_addr)
{
    uint8_t* line_ptr = ruler_addr;
    uint8_t pos = 0;
    for (;;)
    {
        uint8_t cur_ch = 0x2e;
        for (;;)
        {
            line_ptr[pos] = cur_ch;
            pos++;
            uint8_t next_ch = pos;
            uint8_t idx = next_ch;
            idx++;
            next_ch += 6;
            if (next_ch == screen_maxcolumn)
                goto cb0ff;
            if (idx & 7)
                break;
            cur_ch = 0x2a;
        }
    }
cb0ff:
    line_ptr[pos] = 0x3c;
    return pos;
}

/**
 * Read one byte from the current file.
 * @return next byte, or 0 on EOF or NUL
 */
uint8_t get_byte_from_file(void)
{
    int c = fgetc(file_ptr);
    if (c == EOF || c == 0)
        return 0;
    return (uint8_t)c;
}

/**
 * Get the address of a register variable by letter name.
 * @param cur_ch letter identifying the register (A-Z, case-insensitive)
 * @return pointer to the register value, or NULL if not a letter
 */
unsigned int* get_register_address(uint8_t cur_ch)
{
    if (!isalpha(cur_ch))
        return NULL;
    cur_ch &= 0xdf;
    return &register_value_array[cur_ch - 'A'];
}

/**
 * Initialise document state and memory layout.
 * Clears flags, sets up heap boundaries, creates the default ruler,
 * and positions the cursor at the top of the document.
 */
void initialise_document(void)
{
    printer_driver_name[0] = 0;
    format_mode_flag = 0;
    justifying_flag = 0;
    insert_mode_flag = 0;
    print_flags = 0;
    edit_buffer_dirty_flag = 0;
    edit_buffer_unpacked_flag = 0;
    scroll_repeat_count = 0;
    ruler_index_ptr = 0;
    hscroll_pos = 0;
    visual_column = 0;
    display_start_row = 0;
    line_counter = 0;
    flags_need_redrawing_flag = 0;
    ypos = 0;
    print_xpos = 0;
    line_change_pending_flag = 0;
    search_target_len = 0;
    cursor_moved_flag = 0;
    delimiter_char = 0;
    line_format_status = 0;
    input_buffer_offset = 0;
    page = oshwm + 0x101;
    uint8_t pos = 0;
    file_edit_flags = pos;
    xpos = pos;
    oshwm[pos] = 0xaa;
    page[-1] = 0x0d;
    ram[RAM_CURRENT_LINE_BUF + MAX_LINE_LENGTH - 1] = 0x0d;
    top = page;
    edit_buffer_base = &ram[RAM_CURRENT_LINE_BUF];
    current_format_line_ptr = &ram[RAM_EDIT_BUFFER];
    uint8_t pos2 = create_default_ruler(&ram[RAM_CURRENT_RULER_BUF]);
    pos2++;
    ram[RAM_CURRENT_RULER_BUF + pos2] = 0x0d;
    ruler_index[0] = &ram[0];
    ruler_index[0x7f] = &ram[RAM_JUST_BEFORE_RULER_BUF];
    move_cursor_to_top_of_document();
    clear_cmd();
    ensure_cr_at_document_top();
}

/**
 * Convert a marker character to its zero-based index.
 * @param cur_ch character '1'..'6' to look up
 * @return 0..5 for valid markers, MARKER_INVALID otherwise; beeps if
 * cur_ch is below '1'
 */
int lookup_marker(uint8_t cur_ch)
{
    if (cur_ch < 0x31)
    {
        beep();
        return MARKER_INVALID;
    }
    cur_ch -= 0x31;
    if (cur_ch >= 6)
        return MARKER_INVALID;
    return cur_ch;
}

/**
 * Move the cursor to the document address containing the given pointer.
 * Scans forward or backward from the current line to locate the line that
 * contains the target address and updates current_line_ptr and xpos.
 * @param addr target address within the document heap
 */
void move_cursor_to_address(uint8_t* addr)
{
    uint8_t* next_line_start;
    uint8_t* cur = current_line_ptr;
    if (!(cur == addr))
    {
        if (cur > addr)
        {
            for (;;)
            {
                uint8_t* line_ptr;
                if (!find_previous_line(cur, &line_ptr))
                    goto cac20;
                cur = line_ptr;
                if (cur <= addr)
                    goto cac20;
            }
        }
        do
        {
            uint8_t pos;
            if (find_next_line(cur, &next_line_start, &pos))
                break;
            cur = next_line_start + pos;
            if (cur >= addr)
            {
                if (cur == addr)
                    goto cac1d;
                break;
            }
            check_for_embedded_ruler(next_line_start);
        } while (1);
        cur = next_line_start;
        goto cac20;
    cac1d:
        check_for_embedded_ruler(next_line_start);
    }
cac20:
    current_line_ptr = cur;
    uint8_t idx = (uint8_t)(addr - current_line_ptr);
    command_prefix_t cp = check_for_command_prefix(current_line_ptr[0]);
    if (cp != NO_COMMAND_PREFIX)
    {
        uint8_t next_ch = idx;
        idx = 0;
        if (next_ch >= 3)
        {
            next_ch -= 3;
            idx = next_ch;
        }
    }
    xpos = idx;
}

/**
 * Move the cursor to the start of the document (page).
 * Resets screen and ruler stack pointers and loads the initial ruler.
 */
void move_cursor_to_top_of_document(void)
{
    current_line_ptr = page;
    xpos = 0;
    top_of_screen_line_ptr = &ram[RAM_MAX];
    ruler_index_ptr = 0xfe;
    saved_ruler_index_scroll = 0xfe;
    load_current_ruler(0xfe);
}

/**
 * Find the end of the current line.
 * Scans from start until CR (0x0d) or NUL.
 * @param start address to start scanning
 * @param line_ptr on return, set to start
 * @param pos on return, offset of the byte past the CR or of the NUL
 * @return true if the terminator is NUL (end of document), false if CR
 */
bool find_next_line(uint8_t* start, uint8_t** line_ptr, uint8_t* pos)
{
    *line_ptr = start;
    *pos = 0;
    for (;;)
    {
        uint8_t cur_ch = (*line_ptr)[*pos];
        if (cur_ch == 0)
            return true;
        (*pos)++;
        if (cur_ch == 0x0d)
            break;
    }
    return (*line_ptr)[*pos] == 0;
}

/**
 * Find the start of the previous line.
 * @param val address just past the current line
 * @param line_ptr on return, set to the start of the previous line
 * @return false if already at the start of the document, true otherwise
 */
bool find_previous_line(uint8_t* val, uint8_t** line_ptr)
{
    uint8_t cur_ch;
    *line_ptr = val - 1;
    if (*line_ptr < page)
        return false;
    do
    {
        (*line_ptr)--;
        cur_ch = **line_ptr;
    } while (cur_ch != 0x0d);
    (*line_ptr)++;
    if (**line_ptr == RULER_BYTE)
        pop_from_ruler_index();
    return true;
}

/**
 * Open the input file named in filename_buffer for reading.
 * On failure, reports file not found.
 */
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

/**
 * Open the output file named in filename_buffer for writing.
 * On failure, reports a file error.
 */
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

/**
 * Pop the most recent ruler from the ruler index stack.
 * Increments the status line redraw flag and loads the previous ruler.
 */
void pop_from_ruler_index(void)
{
    status_line_needs_redrawing_flag++;
    uint8_t pos = ruler_index_ptr;
    pos++;
    pos++;
    load_current_ruler(pos);
}

/**
 * Push a ruler address onto the ruler index stack.
 * @param target_ptr address of the ruler line to push
 */
void push_onto_ruler_index(uint8_t* target_ptr)
{
    {
        status_line_needs_redrawing_flag++;
        uint8_t stack_index = ruler_index_ptr - 2;
        ruler_index[stack_index >> 1] = target_ptr;
        load_current_ruler(stack_index);
    }
}

/**
 * Reset the active area to cover the entire document.
 * Sets area_start_ptr to top and area_end_ptr to page.
 */
void reset_area_to_entire_document(void)
{
    area_start_ptr = top;
    area_end_ptr = page;
}

/**
 * Advance to the next document line, handling ruler lines.
 * If the current line is a ruler, it is pushed onto the ruler stack.
 * @param line first byte of the current line
 * @param line_ptr on return, set to the start of the current line
 * @param pos on return, offset past the line terminator
 * @return true if the next line terminator is NUL (end of document), false
 * otherwise
 */
bool advance_to_next_line(uint8_t* line, uint8_t** line_ptr, uint8_t* pos)
{
    if (*line != RULER_BYTE)
        return find_next_line(line, line_ptr, pos);
    bool end = find_next_line(line, line_ptr, pos);
    if (!end)
        push_onto_ruler_index(line);
    return end;
}
