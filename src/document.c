#include "document.h"
#include "view.h"
#include "io.h"
#include "printing.h"
#include "cli.h"
#include "editor.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void ensure_ruler_index_allocated(void);
static uint8_t get_byte_from_file(void);
static uint8_t* compute_required_space_for_insertion(uint8_t* target_ptr);
static uint8_t* compute_space_available(uint8_t* target_ptr);
static uint8_t* compute_space_common(uint8_t* target_ptr, ptrdiff_t scan_ptr);
static uint8_t* find_line_start(uint8_t* target_ptr);
static void check_for_embedded_ruler(uint8_t* target_ptr);

/**
 * Ensure ruler_index is allocated.
 * Lazily allocates the ruler index stack if not already allocated by main().
 */
static void ensure_ruler_index_allocated(void)
{
    if (ruler_index)
        return;
    if (ruler_index_size == 0)
        ruler_index_size = DEFAULT_RULER_INDEX_SIZE;
    ruler_index = calloc(ruler_index_size, sizeof(uint8_t*));
    assert(ruler_index != NULL);
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
    stop_printing();
    cli_putstring("Editing ");

    if (file_edit_flags == 0)
    {
        cli_putstring("No File\n");
        return;
    }

    cli_putstring((char*)input_filename);
    if (!(file_edit_flags & 0x40))
    {
        cli_putstring(" to ");
        cli_putstring((char*)output_filename);
        cli_putchar('\n');
    }
    cli_putchar('\n');
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
    ensure_ruler_index_allocated();
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
    uint8_t pos = 0;

    file_edit_flags = pos;
    xpos = pos;
    ram[pos] = 0xaa;
    current_line_buffer.text[MAX_LINE_LENGTH - 1] = 0x0d;
    top = ram;
    edit_buffer_base = (uint8_t*)&current_line_buffer;
    current_format_line = &current_line_buffer;
    heap_format_line_ptr = &current_line_buffer;

    uint8_t pos2 = create_default_ruler(current_ruler_buffer) + 1;
    current_ruler_buffer[pos2] = 0x0d;

    ruler_index[0] = &ram[0];
    ruler_index[ruler_index_size - 1] =
        (uint8_t*)((uintptr_t)current_ruler_buffer - 3);
    move_cursor_to_top_of_document();
    clear_cmd();
    ensure_cr_at_document_top();
}

/**
 * Ensure the document contains at least one carriage return.
 * If the document is empty (ram == top), inserts a CR at ram and a
 * terminating NUL at top.
 */
void ensure_cr_at_document_top(void)
{
    if (ram != top)
        return;
    top++;
    current_line_ptr = ram;
    ram[0] = 0x0d;
    top[0] = 0;
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
 * Move the cursor to the start of the document (ram).
 * Resets screen and ruler stack pointers and loads the initial ruler.
 */
void move_cursor_to_top_of_document(void)
{
    ensure_ruler_index_allocated();
    current_line_ptr = ram;
    xpos = 0;
    top_of_screen_line_ptr = &ram[RAM_MAX];
    ruler_index_ptr = (int)(ruler_index_size - 1);
    saved_ruler_index_scroll = (int)(ruler_index_size - 1);
    load_current_ruler((int)(ruler_index_size - 1));
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
 * Reset the active area to cover the entire document.
 * Sets area_start_ptr to top and area_end_ptr to ram.
 */
void reset_area_to_entire_document(void)
{
    area_start_ptr = top;
    area_end_ptr = ram;
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
    if (*line != RULER_PREFIX)
        return find_next_line(line, line_ptr, pos);
    bool end = find_next_line(line, line_ptr, pos);

    if (!end)
        push_onto_ruler_index(line);

    return end;
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
 * Check that the area at the working pointer fits in memory and rebuild the
 * line. Expands or shrinks the document gap as needed and reformats the line
 * content with case handling.
 * @param doc_working_ptr pointer to the document line to check
 * @return true if memory allocation failed, false otherwise
 */
bool check_area_memory(uint8_t* doc_working_ptr)
{
    uint8_t tmp_ch3;
    uint8_t tmp_ch4;
    uint8_t cur_ch = 0;
    uint8_t block_expansion_len = cur_ch;

    scratch_index = cur_ch;
    uint8_t pos = 0x14;
    uint8_t idx = search_target_len;

    if (idx == 0)
    {
    c8a5b:
        uint8_t next_ch = header_text_maybe[idx];

        if (next_ch == 1)
        {
            next_ch = scratch_index;

            if (next_ch >= cli_header_pos)
                goto c8a86;
            scratch_index++;

            if (scratch_index != 0)
                goto c8a84;
        }
        if (next_ch == 0x20 && pos < cli_output_pos)
        {
            while (1)
            {
                uint8_t tmp_ch2 = output_buffer[pos];

                pos++;

                if (tmp_ch2 == 0)
                    goto c8a86;
                block_expansion_len++;

                if (pos >= cli_output_pos)
                    break;
            }
            block_expansion_len--;
        }
    c8a84:
        block_expansion_len++;

    c8a86:
        idx++;
    }
    if (idx < cli_header_limit)
        goto c8a5b;
    ptrdiff_t gap = search_cursor_ptr - doc_working_ptr;
    uint8_t idx2 = block_expansion_len;

    if (gap < 256 && idx2 >= gap)
        idx2 = gap;
    uint8_t* insert_ptr = doc_working_ptr + idx2;
    ptrdiff_t delta = (ptrdiff_t)block_expansion_len - gap;

    if (delta < 0)
    {
        scratch_scan_ptr = adjust_pointers(insert_ptr, -delta);
    }
    else if (delta > 0)
    {
        if (!make_space_for_insertion(insert_ptr, delta))
            return true;
    }
    uint8_t pos2 = 0;

    scratch_index = pos2;

    if ((print_xpos & 0x80) == 0)
    {
        uint8_t idx3 = scratch_offset;

        do
        {
            tmp_ch3 = doc_working_ptr[pos2];
            pos2++;

            if (isalpha(tmp_ch3))
                goto c8af3;
            print_xpos = (uint8_t)(print_xpos >> 1) | 0x80;
            idx3--;
        } while (idx3 != 0);

        goto c8b11;

    c8af3:
    {
        print_xpos = 0;
        tmp_ch4 = tmp_ch3;
    }
        tmp_ch4 &= 0x20;

        if (tmp_ch4 != 0)
            goto c8b11;
        scratch_index++;
        idx3--;

        if (idx3 != 0)
        {
            uint8_t tmp_ch5 = doc_working_ptr[pos2];

            if (!isalpha(tmp_ch5))
                goto c8b11;
            tmp_ch5 &= 0x20;

            if (tmp_ch5 != 0)
                goto c8b11;
        }
        scratch_index -= 2;
    }
c8b11:
    uint8_t output_buf_pos = 0;
    uint8_t doc_write_pos = 0;

    scratch_offset = 0x14;
    uint8_t idx4 = search_target_len;

    if (idx4 != 0)
        goto c8b6b;

    do
    {
        uint8_t tmp_ch6 = header_text_maybe[idx4];

        if (tmp_ch6 == 0x20)
        {
            uint8_t pos3 = scratch_offset;

            if (pos3 >= cli_output_pos)
                goto c8b47;
            scratch_offset++;
            tmp_ch6 = output_buffer[pos3];

            if (tmp_ch6 == 0)
                goto c8b6a;
            idx4--;
        }
        else
        {
            if (tmp_ch6 != 1)
                goto c8b47;

            if (output_buf_pos >= cli_header_pos)
                goto c8b6a;
            tmp_ch6 = output_buffer[output_buf_pos];
            output_buf_pos++;
        }
    c8b47:
        if (tmp_ch6 == 2)
            tmp_ch6 = 0x20;

        if ((folding_flag & 0x80) == 0 && print_xpos == 0)
        {
            if (isalpha(tmp_ch6))
            {
                tmp_ch6 |= 0x20;

                if (scratch_index != 0)
                {
                    scratch_index--;
                    tmp_ch6 &= 0xdf;
                }
            }
        }
        doc_working_ptr[doc_write_pos] = tmp_ch6;

        doc_write_pos++;

    c8b6a:
        idx4++;
    c8b6b:;
    } while (idx4 < cli_header_limit);
    split_line_at_wrap(doc_working_ptr);

    return false;
}

/**
 * Read the first chunk of the input file starting at the document page.
 * @return true if the block read was empty, false otherwise
 */
bool read_first_chunk_from_input_file(void)
{
    return read_next_chunk_from_input_file(ram);
}

/**
 * Read the input file into the document at the current area start.
 * Ensures free space, computes insertion limits, reads a block, and adjusts
 * document pointers.
 * @return pointer to the byte after the inserted data
 */
uint8_t* read_into_document(void)
{
    check_for_at_least_150_bytes_free();
    open_input_file();
    uint8_t* insert_ptr = area_start_ptr;

    move_cursor_to_address(area_start_ptr);
    uint8_t* space_limit = compute_required_space_for_insertion(insert_ptr);

    make_space_for_insertion(insert_ptr, space_limit - insert_ptr + 0x8b);
    uint8_t* cursor = insert_ptr;

    read_block_status_t status = read_block_from_file(&cursor, space_limit);

    if (status != READ_BLOCK_DONE)
        cli_putstring("Not all read in\n");
    scratch_scan_ptr = adjust_pointers(cursor, space_limit - cursor);

    return cursor;
}

/**
 * Ensure at least 150 bytes are free.
 * Displays a memory error and does not return if less than 150 bytes remain.
 */
void check_for_at_least_150_bytes_free(void)
{
    if (compute_bytes_free() >= LINE_LENGTH_SPARE)
        return;
    display_not_enough_memory();
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

    if (cur != addr)
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
    {
        FILE* _log = fopen("/tmp/view.log", "a");
        if (_log)
        {
            fprintf(_log,
                "move_cursor_to_address cur %p addr %p idx %d xpos %d\n",
                (void*)cur,
                (void*)addr,
                idx,
                idx);
            fclose(_log);
        }
    }

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
    if (val <= ram)
        return false;

    uint8_t* p = val - 2;

    while (p >= ram)
    {
        if (*p == 0x0d)
        {
            *line_ptr = p + 1;

            if (**line_ptr == RULER_PREFIX)
                pop_from_ruler_index();

            return true;
        }
        p--;
    }
    *line_ptr = ram;

    if (**line_ptr == RULER_PREFIX)
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
        file_not_found_error();

    file_ptr = input_fp;
}

/**
 * Pop the most recent ruler from the ruler index stack.
 * Increments the status line redraw flag and loads the previous ruler.
 */
void pop_from_ruler_index(void)
{
    ensure_ruler_index_allocated();
    status_line_needs_redrawing_flag++;
    int pos = ruler_index_ptr + 1;
    assert(pos >= 0 && (size_t)pos < ruler_index_size);
    load_current_ruler(pos);
}

/**
 * Checks for an embedded ruler.
 * Pushes the ruler stack if the line starts with a ruler byte.
 * @param target_ptr line pointer
 */
static void check_for_embedded_ruler(uint8_t* target_ptr)
{
    if (*target_ptr == RULER_PREFIX)
        push_onto_ruler_index(target_ptr);
}

/**
 * Push a ruler address onto the ruler index stack.
 * @param target_ptr address of the ruler line to push
 */
void push_onto_ruler_index(uint8_t* target_ptr)
{
    ensure_ruler_index_allocated();
    status_line_needs_redrawing_flag++;
    int stack_index = ruler_index_ptr - 1;
    assert(stack_index >= 0 && (size_t)stack_index < ruler_index_size);
    ruler_index[stack_index] = target_ptr;
    load_current_ruler(stack_index);
}

/**
 * Load the ruler at the given index and recompute margins.
 * @param pos element index into the ruler index stack (0 .. ruler_index_size-1)
 */
void load_current_ruler(int pos)
{
    ensure_ruler_index_allocated();
    assert(pos >= 0 && (size_t)pos < ruler_index_size);
    ruler_index_ptr = pos;
    current_ruler_ptr = ruler_index[pos] + 3;
    find_margins_of_current_ruler_buffer();
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
 * Compute required space for a fresh insertion at the target pointer.
 * @param target_ptr insertion point
 * @return limit pointer for the insertion
 */
static uint8_t* compute_required_space_for_insertion(uint8_t* target_ptr)
{
    return compute_space_common(target_ptr, 0);
}

/**
 * Read the next chunk of the input file into the document.
 * Computes available space, reads a block, and updates the document top.
 * @param target_ptr destination address in the document heap
 * @return true if the block read was empty, false otherwise
 */
bool read_next_chunk_from_input_file(uint8_t* target_ptr)
{
    uint8_t* space_limit = compute_space_available(target_ptr);

    file_ptr = input_fp;
    uint8_t* cursor = target_ptr;

    read_block_status_t status = read_block_from_file(&cursor, space_limit);

    if (status == READ_BLOCK_DONE)
        input_file_empty_flag++;
    *cursor = 0;
    top = cursor;

    return status == READ_BLOCK_EMPTY;
}

/**
 * Compute available space for insertion at the target pointer.
 * @param target_ptr insertion point
 * @return limit pointer for the insertion
 */
static uint8_t* compute_space_available(uint8_t* target_ptr)
{
    return compute_space_common(target_ptr, compute_bytes_free());
}

/**
 * Compute the free-space limit for an insertion at the target pointer.
 * Applies the common clamping logic for available versus required space.
 * @param target_ptr insertion point in the document heap
 * @param scan_ptr size hint (quarter-scaled free count or zero)
 * @return pointer to the computed limit
 */
static uint8_t* compute_space_common(uint8_t* target_ptr, ptrdiff_t scan_ptr)
{
    ptrdiff_t size_delta = compute_bytes_free();

    scan_ptr >>= 2;

    if (scan_ptr >= 0x0400)
    {
        scan_ptr = 0x0404;
        size_delta -= scan_ptr;
    }
    else
        size_delta -= scan_ptr + 1;

    return target_ptr + size_delta - 0x8b;
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
 * Reads a block of data from the file into the memory buffer.
 *
 * @param cursor pointer to the current write position, updated on return
 * @param limit upper bound for the write position
 * @return status indicating empty, done or more data available
 */
read_block_status_t read_block_from_file(uint8_t** cursor, uint8_t* limit)
{
    int next_ch;
    bool eof_1;
    int idx3;

    screen_column = 0;
    for (;;)
    {
        do
        {
            next_ch = get_byte_from_file();

            if (next_ch == 0)
            {
                eof_1 = true;
                goto c8cf2;
            }
            if (next_ch < 0x7f)
                goto c8caf;
        } while (check_for_command_prefix(next_ch) == NO_COMMAND_PREFIX);

        screen_column = 0xfd;

    c8caf:
        if (next_ch < 0x20)
        {
            control_code_t cc = check_for_control_code(next_ch);

            if (cc != NO_CONTROL_CODE || next_ch == 0x1a || next_ch == 0x0d ||
                next_ch == 0x0b)
                goto c8cc8;

            if (next_ch != 9)
                continue;
        }
    c8cc8:
        idx3 = 1;

        if (next_ch != 0x0d)
        {
            idx3--;

            if (screen_column == MAX_LINE_LENGTH)
            {
                write_cr_to_memory(&scratch_line_ptr);
                next_ch = next_ch;
                idx3++;
            }
        }
        screen_column++;
        write_byte_to_memory(cursor, next_ch);
        if (!(idx3 == 0 || *cursor < limit))
            break;
    }
    eof_1 = false;

c8cf2:
    if (screen_row == 0)
        return READ_BLOCK_EMPTY;

    if (eof_1)
        return READ_BLOCK_DONE;
    return READ_BLOCK_MORE;
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
        uint8_t first_char = ((uint8_t*)current_format_line)[0];

        command_prefix_t cp = check_for_command_prefix(first_char);
        len = xpos;

        if (cp != NO_COMMAND_PREFIX)
            len += 3;
    }
    uint16_t marker_addr = (current_line_ptr - &ram[0]) + len;
    markers_array[marker_idx] = &ram[marker_addr];
}

/**
 * Set up area pointers for the line at the working pointer.
 * Increments the line counter and clamps the visible pointer when the line
 * contains a carriage return.
 * @param doc_working_ptr pointer to the document line to inspect
 */
void setup_area_pointers(uint8_t* doc_working_ptr)
{
    uint8_t* scan_ptr = doc_working_ptr;
    uint8_t idx = 0;

    if (scan_ptr != search_cursor_ptr)
    {
        if (*scan_ptr == 0x0d)
            idx++;
        scan_ptr++;
    }
    line_counter++;

    if (idx == 0)
        return;
    clamp_ptr6_to_document();
}

/**
 * Splits a line at wrap position.
 * Inserts carriage returns at word boundaries to enforce line length limits.
 * @param target_ptr pointer within the line to split
 */
void split_line_at_wrap(uint8_t* target_ptr)
{
    uint8_t temp_save;
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

            if (next_char != 0x20)
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
 * Finds the start of the current line.
 * Scans backward for the preceding CR.
 * @param target_ptr pointer within line
 * @return pointer to line start
 */
static uint8_t* find_line_start(uint8_t* target_ptr)
{
    while (1)
    {
        if (target_ptr == ram)
            return target_ptr - 1;
        target_ptr--;
        uint8_t acc = target_ptr[0];

        if (acc == 0x0d)
            break;
    }
    return target_ptr;
}

/**
 * Updates markers to point into the format buffer.
 * Retargets markers from document heap into the current format line.
 */
void update_markers_to_format_buffer(void)
{
    uint8_t* size_delta = current_line_ptr;
    uint8_t offset = 0;

    do
    {
        uint8_t idx = find_marker_at_position(offset, size_delta);

        if (idx != 0x0c)
        {
            uint8_t* base_ptr =
                (current_format_line->prefix_byte == COMMAND_PREFIX ||
                    current_format_line->prefix_byte == RULER_PREFIX)
                    ? (uint8_t*)current_format_line
                    : current_format_line->text;
            markers_array[idx / 2] = base_ptr + offset;
        }
        uint8_t acc = current_line_ptr[offset];

        if (acc == 0x0d)
            return;
        offset++;
    } while (offset != 0);
}

/**
 * Write the sanitised document area to the output file.
 * Iterates from area start to area end and writes each byte.
 */
void write_area_to_file(void)
{
    if (sanitise_area() == AREA_EMPTY)
        return;
    uint8_t* scan_ptr = area_start_ptr;

    do
    {
        fputc(*scan_ptr, file_ptr);
        scan_ptr++;
    } while (scan_ptr != area_end_ptr);
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

    if (area_end_ptr != area_start_ptr)
        return AREA_NOT_EMPTY;
    return AREA_EMPTY;
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

/**
 * Writes the edit buffer back to the document.
 * Computes size delta, adjusts heap, and copies the line including marker
 * updates.
 * @return true if write failed due to memory
 */
bool write_line_back_to_document(void)
{
    uint8_t temp_save;
    uint8_t out_byte;
    uint8_t stored_byte;

    if (edit_buffer_unpacked_flag != 0)
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
        uint8_t* src_ptr =
            (current_format_line->prefix_byte == COMMAND_PREFIX ||
                current_format_line->prefix_byte == RULER_PREFIX)
                ? (uint8_t*)current_format_line
                : current_format_line->text;
        area_size = src_ptr - &ram[0];
        uint8_t line_len = screen_column;

        edit_line_len = line_len;

        do
        {
            if (line_len == 0)
            {
                out_byte = 0x0d;
            }
            else
            {
                out_byte = src_ptr[copy_idx];

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
        if (slot_idx < ARRAY_SIZE(markers_array))
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
 * Finds a marker at a buffer position.
 * Checks if any marker points at the given edit-buffer offset.
 * @param pos buffer position
 * @param target_ptr base pointer
 * @return marker index or 0x0c if none
 */
uint8_t find_marker_at_position(uint8_t buf_offset, uint8_t* target_ptr)
{
    uint8_t* scan_ptr = target_ptr + buf_offset;
    uint8_t slot_idx = 0;

    do
    {
        if (scan_ptr == markers_array[slot_idx / 2])
            goto ca558;
        slot_idx++;
        slot_idx++;
    } while (slot_idx != 0x0c);

    return 0x0c;

ca558:
    return slot_idx;
}

/**
 * Returns the length of the current edit line.
 * Scans the edit buffer for the last non-fill byte, adjusting for command
 * prefixes.
 * @return line length in characters
 */
uint8_t get_line_length(void)
{
    uint8_t first_char = current_format_line->prefix_byte;

    command_prefix_t cp = check_for_command_prefix(first_char);
    uint8_t scan_pos = MAX_LINE_LENGTH;

    do
    {
        scan_pos--;

        if (current_line_buffer.text[scan_pos] != 0x10)
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
        if (((uint8_t**)&pointer_array)[slot_idx] >= insert_ptr)
            ((uint8_t**)&pointer_array)[slot_idx] += size_delta;
        slot_idx++;
    } while (slot_idx != sizeof(pointer_array) / sizeof(uint8_t*));
    size_t copy_len = (size_t)(copy_ptr - insert_ptr) + 1;

    memmove(insert_ptr + size_delta, insert_ptr, copy_len);

    return true;
}

/**
 * Read one byte from the current file.
 * @return next byte, or 0 on EOF or NUL
 */
static uint8_t get_byte_from_file(void)
{
    int c = fgetc(file_ptr);

    if (c == EOF || c == 0)
        return 0;
    return (uint8_t)c;
}
