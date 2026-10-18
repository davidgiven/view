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

static uint8_t get_byte_from_file(void);
static uint8_t* compute_required_space_for_insertion(uint8_t* target_ptr);
static uint8_t* compute_space_available(uint8_t* target_ptr);
static uint8_t* compute_space_common(uint8_t* target_ptr, ptrdiff_t scan_ptr);
static uint8_t* find_line_start(uint8_t* target_ptr);
static void check_for_embedded_ruler(uint8_t* target_ptr);

/**
 * Dereference a document pointer at an offset and test for command prefix.
 * @param pos offset from target_ptr
 * @param target_ptr base pointer into document memory
 * @return COMMAND_PREFIX, RULER_PREFIX, or NO_COMMAND_PREFIX
 */
command_prefix_t deref_and_check_for_command_prefix(
    uint8_t pos, uint8_t* target_ptr)
{ return check_for_command_prefix(target_ptr[pos]); }

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
 * @param register_name letter identifying the register (A-Z, case-insensitive)
 * @return pointer to the register value, or NULL if not a letter
 */
unsigned int* get_register_address(uint8_t register_name)
{
    if (!isalpha(register_name))
        return NULL;
    register_name &= 0xdf;

    return &register_value_array[register_name - 'A'];
}

/**
 * Initialise document state and memory layout.
 * Clears flags, sets up heap boundaries, creates the default ruler,
 * and positions the cursor at the top of the document.
 */
void initialise_document(void)
{
    printer_driver_name[0] = 0;
    format_mode_flags = 0;
    justifying_flag = false;
    insert_mode_flag = false;
    print_flags = 0;
    edit_buffer_dirty_flag = false;
    edit_buffer_unpacked_flag = false;
    edit_buffer_contains_ruler_flag = false;
    scroll_repeat_count = 0;
    ruler_index_ptr = 0;
    hscroll_pos = 0;
    visual_column = 0;
    display_start_row = 0;
    line_counter = 0;
    flags_need_redrawing_flag = false;
    ypos = 0;
    print_xpos = 0;
    line_change_pending_flag = 0;
    search_target_len = 0;
    cursor_moved_flag = false;
    delimiter_char = 0;
    line_format_status_flag = false;
    input_buffer_offset = 0;
    file_edit_flags = 0;
    xpos = 0;
    ram[0] = 0xaa;
    current_line_buffer.text[MAX_LINE_LENGTH - 1] = 0x0d;
    top = ram;
    edit_buffer_base = (uint8_t*)&current_line_buffer;
    current_format_line = (uint8_t*)&current_line_buffer;
    heap_format_line_ptr = &current_line_buffer;

    uint8_t ruler_end = create_default_ruler(current_ruler_buffer) + 1;
    current_ruler_buffer[ruler_end] = '\r';

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
 * Creates a default ruler with tab stops every eight columns.
 * Fills the buffer with '.' and '*' and places '<' six columns before
 * screen_maxcolumn, preserving the original byte-sized width arithmetic.
 * @param ruler_addr destination buffer for the ruler
 * @return offset of the terminating '<' character
 */
uint8_t create_default_ruler(uint8_t* ruler_addr)
{
    uint8_t right_stop = (uint8_t)(screen_maxcolumn - 6);
    uint8_t pos = 0;
    do
    {
        ruler_addr[pos] = (pos & 7) == 7 ? '*' : '.';
        pos++;
    } while (pos != right_stop);
    ruler_addr[pos] = '<';
    return pos;
}

/**
 * Convert a marker character to its zero-based index.
 * @param marker_char character '1'..'6' to look up
 * @return 0..5 for valid markers, MARKER_INVALID otherwise; beeps if
 * marker_char is below '1'
 */
int lookup_marker(uint8_t marker_char)
{
    if (marker_char < 0x31)
    {
        beep();

        return MARKER_INVALID;
    }
    marker_char -= 0x31;

    if (marker_char >= NUM_MARKERS)
        return MARKER_INVALID;
    return marker_char;
}

/**
 * Move the cursor to the start of the document (ram).
 * Resets screen and ruler stack pointers and loads the initial ruler.
 */
void move_cursor_to_top_of_document(void)
{
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
 * Scans past the current document line and updates the ruler stack.
 * Pushes a ruler only when another line follows it.
 * @param line first byte of the current line
 * @param line_ptr output for the start of the current line
 * @param pos output for the offset past CR, or the offset of a terminating NUL
 * @return true if the scan reaches NUL or a CR immediately followed by NUL
 */
bool advance_to_next_line(uint8_t* line, uint8_t** line_ptr, uint8_t* pos)
{
    bool is_ruler = *line == RULER_PREFIX;
    bool at_end = find_next_line(line, line_ptr, pos);
    if (is_ruler && !at_end)
        push_onto_ruler_index(line);
    return at_end;
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
 * Measures the replacement template, including available wildcard captures.
 * Preserves byte-sized length arithmetic and the template's starting offset.
 * @return number of bytes the expanded replacement will occupy
 */
static uint8_t measure_replacement(void)
{
    uint8_t length = 0;
    uint8_t capture_pos = 0x14;
    uint8_t template_pos = search_target_len;
    scratch_index = 0;

    for (bool first = template_pos == 0;
        first || template_pos < cli_header_limit;
        template_pos++)
    {
        first = false;
        uint8_t ch = header_text_maybe[template_pos];
        if (ch == 1)
        {
            if (scratch_index >= cli_header_pos)
                continue;
            scratch_index++;
        }
        else if (ch == 0x20 && capture_pos < cli_output_pos)
        {
            do
            {
                if (output_buffer[capture_pos++] == 0)
                    break;
                length++;
            } while (capture_pos < cli_output_pos);
            continue;
        }
        length++;
    }
    return length;
}

/**
 * Chooses replacement capitalization from the matched text.
 * Updates print_xpos and scratch_index, retaining an explicit case override.
 * @param source matched text after resizing the document area
 */
static void set_replacement_case(uint8_t* source)
{
    scratch_index = 0;
    if (print_xpos & 0x80)
        return;

    uint8_t pos = 0;
    uint8_t remaining = scratch_offset;
    do
    {
        uint8_t ch = source[pos++];
        if (isalpha(ch))
        {
            print_xpos = 0;
            if (ch & 0x20)
                return;
            scratch_index++;
            remaining--;
            if (remaining != 0)
            {
                uint8_t next_ch = source[pos];
                if (!isalpha(next_ch) || (next_ch & 0x20))
                    return;
            }
            scratch_index -= 2;
            return;
        }
        print_xpos = (uint8_t)(print_xpos >> 1) | 0x80;
        remaining--;
    } while (remaining != 0);
}

/**
 * Writes the replacement template, expanding captures and applying case rules.
 * @param destination start of the resized replacement area
 */
static void write_replacement(uint8_t* destination)
{
    uint8_t capture_pos = 0;
    uint8_t dest_pos = 0;
    uint8_t template_pos = search_target_len;
    bool first = template_pos == 0;
    scratch_offset = 0x14;

    while (first || template_pos < cli_header_limit)
    {
        first = false;
        bool repeat_capture = false;
        uint8_t ch = header_text_maybe[template_pos];
        if (ch == 0x20 && scratch_offset < cli_output_pos)
        {
            ch = output_buffer[scratch_offset++];
            if (ch == 0)
            {
                template_pos++;
                continue;
            }
            repeat_capture = true;
        }
        else if (ch == 1)
        {
            if (capture_pos >= cli_header_pos)
            {
                template_pos++;
                continue;
            }
            ch = output_buffer[capture_pos++];
        }

        if (ch == 2)
            ch = 0x20;
        if (folding_flag && print_xpos == 0 && isalpha(ch))
        {
            ch |= 0x20;
            if (scratch_index != 0)
            {
                scratch_index--;
                ch &= 0xdf;
            }
        }
        destination[dest_pos++] = ch;
        if (!repeat_capture)
            template_pos++;
    }
}

/**
 * Resizes the matched document area and writes the expanded replacement.
 * Applies capitalization rules and restores line wrapping after replacement.
 * @param doc_working_ptr start of the matched document area
 * @return true if memory allocation failed, false otherwise
 */
bool check_area_memory(uint8_t* doc_working_ptr)
{
    uint8_t replacement_length = measure_replacement();
    ptrdiff_t matched_length = search_cursor_ptr - doc_working_ptr;
    uint8_t retained_length = replacement_length;
    if (matched_length < 256 && retained_length >= matched_length)
        retained_length = matched_length;
    uint8_t* insert_ptr = doc_working_ptr + retained_length;
    ptrdiff_t delta = (ptrdiff_t)replacement_length - matched_length;

    if (delta < 0)
        scratch_scan_ptr = adjust_pointers(insert_ptr, -delta);
    else if (delta > 0 && !make_space_for_insertion(insert_ptr, delta))
        return true;

    set_replacement_case(doc_working_ptr);
    write_replacement(doc_working_ptr);
    split_line_at_wrap(doc_working_ptr);
    return false;
}

/**
 * Read the first chunk of the input file starting at the document page.
 * @return true if the block read was empty, false otherwise
 */
bool read_first_chunk_from_input_file(void)
{ return read_next_chunk_from_input_file(ram); }

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
 * Moves the cursor to the line containing a document address.
 * Updates the ruler stack while crossing lines and maps the address to xpos,
 * excluding command headers and clamping positions inside a header to zero.
 * @param addr target address within the document heap
 */
void move_cursor_to_address(uint8_t* addr)
{
    uint8_t* line = current_line_ptr;
    if (line > addr)
    {
        while (line > addr)
        {
            uint8_t* previous_line;
            if (!find_previous_line(line, &previous_line))
                break;
            line = previous_line;
        }
    }
    else
    {
        while (line < addr)
        {
            uint8_t* line_start;
            uint8_t length;
            if (find_next_line(line, &line_start, &length))
            {
                line = line_start;
                break;
            }
            uint8_t* next_line = line_start + length;
            if (next_line > addr)
            {
                line = line_start;
                break;
            }
            check_for_embedded_ruler(line_start);
            line = next_line;
        }
    }

    current_line_ptr = line;
    uint8_t offset = (uint8_t)(addr - line);
    if (check_for_command_prefix(*line) != NO_COMMAND_PREFIX)
        offset = offset >= 3 ? offset - 3 : 0;
    xpos = offset;
}

/**
 * Find the end of the current line.
 * Scans from start until CR (0x0d) or NUL.
 * @param start address to start scanning
 * @param line_ptr on return, set to start
 * @param pos on return, offset of the byte past the CR or of the NUL
 * @return true at NUL or a CR immediately followed by NUL, false otherwise
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
 * @param current_line start of the current line
 * @param line_ptr on return, set to the start of the previous line
 * @return false if already at the start of the document, true otherwise
 */
bool find_previous_line(uint8_t* current_line, uint8_t** line_ptr)
{
    if (current_line <= ram)
        return false;

    uint8_t* previous = current_line - 1;
    while (previous > ram)
    {
        previous--;
        if (*previous == '\r')
        {
            previous++;
            break;
        }
    }
    *line_ptr = previous;
    if (*previous == RULER_PREFIX)
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
 * Marks the status line for redraw and loads the previous ruler.
 */
void pop_from_ruler_index(void)
{
    status_line_needs_redrawing_flag = true;
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
    status_line_needs_redrawing_flag = true;
    int stack_index = ruler_index_ptr - 1;
    if (stack_index < 0 || (size_t)stack_index >= ruler_index_size)
        display_not_enough_memory();
    ruler_index[stack_index] = target_ptr;
    load_current_ruler(stack_index);
}

/**
 * Load the ruler at the given index and recompute margins.
 * @param pos element index into the ruler index stack (0 .. ruler_index_size-1)
 */
void load_current_ruler(int pos)
{
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

        if (cur_ch == '>')
            ruler_left_stop = pos;

        if (cur_ch == '<')
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
{ return compute_space_common(target_ptr, 0); }

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
        input_file_empty_flag = true;
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
{ return compute_space_common(target_ptr, compute_bytes_free()); }

/**
 * Compute the free-space limit for an insertion at the target pointer.
 * Applies the common clamping logic for available versus required space.
 * @param target_ptr insertion point in the document heap
 * @param free_space_hint free byte count used to reserve capacity, or zero
 * @return pointer to the computed limit
 */
static uint8_t* compute_space_common(
    uint8_t* target_ptr, ptrdiff_t free_space_hint)
{
    ptrdiff_t reserved = free_space_hint >> 2;
    reserved = reserved >= 0x0400 ? 0x0404 : reserved + 1;
    return target_ptr + compute_bytes_free() - reserved - 0x8b;
}

/**
 * Compute free bytes between document top and himem.
 * @return number of free bytes (himem - top)
 */
int compute_bytes_free(void)
{ return (int)(himem - top); }

/**
 * Reads and filters a block from the current file into the memory buffer.
 * Preserves document prefixes and supported controls, splitting long lines.
 * Checks the capacity limit after a carriage return or forced line split.
 * @param cursor current write position, advanced as bytes are stored
 * @param limit capacity boundary checked at line breaks
 * @return READ_BLOCK_EMPTY if nothing was stored, READ_BLOCK_DONE on EOF or
 * NUL, or READ_BLOCK_MORE when the capacity boundary is reached
 */
read_block_status_t read_block_from_file(uint8_t** cursor, uint8_t* limit)
{
    uint8_t* start = *cursor;
    screen_column = 0;
    for (;;)
    {
        uint8_t next_ch = get_byte_from_file();
        if (next_ch == 0)
            return *cursor == start ? READ_BLOCK_EMPTY : READ_BLOCK_DONE;

        if (next_ch >= 0x7f)
        {
            if (check_for_command_prefix(next_ch) == NO_COMMAND_PREFIX)
                continue;
            screen_column = 0xfd;
        }
        if (next_ch < 0x20 &&
            check_for_control_code(next_ch) == NO_CONTROL_CODE &&
            next_ch != 0x1a && next_ch != 0x0d && next_ch != 0x0b &&
            next_ch != 9)
            continue;

        bool line_break = next_ch == 0x0d;
        if (!line_break && screen_column == MAX_LINE_LENGTH)
        {
            write_cr_to_memory(&scratch_line_ptr);
            line_break = true;
        }
        screen_column++;
        write_byte_to_memory(cursor, next_ch);
        if (line_break && *cursor >= limit)
            return READ_BLOCK_MORE;
    }
}

/**
 * Returns the document address at the current cursor position.
 * Uses the line length when xpos is beyond it; otherwise includes the command
 * or ruler header in the cursor offset when present.
 * @return pointer into the current document line, including any header
 */
uint8_t* get_cursor_document_address(void)
{
    uint8_t offset = get_line_length();
    if (offset >= xpos)
    {
        offset = xpos;
        if (check_for_command_prefix(current_format_line[0]) !=
            NO_COMMAND_PREFIX)
            offset += offsetof(line_t, text);
    }
    return current_line_ptr + offset;
}

/**
 * Sets a marker to the current position.
 * Computes the document address for the cursor and stores it in the marker
 * array.
 * @param marker_idx marker index
 */
void set_marker_to_here(uint8_t marker_idx)
{ markers_array[marker_idx] = get_cursor_document_address(); }

/**
 * Set up area pointers for the line at the working pointer.
 * Increments the line counter and clamps the visible pointer when the line
 * contains a carriage return.
 * @param doc_working_ptr pointer to the document line to inspect
 */
void setup_area_pointers(uint8_t* doc_working_ptr)
{
    bool starts_with_break =
        doc_working_ptr != search_cursor_ptr && *doc_working_ptr == '\r';
    line_counter++;
    if (starts_with_break)
        clamp_ptr6_to_document();
}

/**
 * Splits an overlong line at spaces or soft spaces, or at the length limit.
 * Allows three extra bytes for a command header and adjusts heap pointers
 * when inserting each carriage return.
 * @param target_ptr pointer within the line to split
 */
void split_line_at_wrap(uint8_t* target_ptr)
{
    uint8_t* scan_ptr = find_line_start(target_ptr);
    do
    {
        screen_column = 0;
        uint8_t line_limit = MAX_LINE_LENGTH + 1;
        if (check_for_command_prefix(scan_ptr[1]) != NO_COMMAND_PREFIX)
            line_limit += 3;

        for (uint8_t pos = 1; pos <= line_limit; pos++)
        {
            uint8_t ch = scan_ptr[pos];
            if (ch == 0x20 || ch == 0x1a)
                screen_column = pos + 1;
            if (ch == 0x0d)
                return;
        }

        uint8_t split_pos = screen_column != 0 ? screen_column : line_limit;
        scan_ptr += split_pos;
        make_space_for_insertion(scan_ptr, 1);
        *scan_ptr = 0x0d;
    } while (scan_ptr != NULL);
}

/**
 * Finds the byte immediately before the current line.
 * Scans backward for the preceding CR, or returns ram - 1 for the first line.
 * @param target_ptr pointer within line
 * @return pointer to the preceding CR, or ram - 1
 */
static uint8_t* find_line_start(uint8_t* target_ptr)
{
    while (target_ptr != ram)
    {
        target_ptr--;
        if (*target_ptr == '\r')
            return target_ptr;
    }
    return ram - 1;
}

/**
 * Returns the bytes representing the current unpacked line.
 * Includes command and ruler headers, but skips the reserved header area for
 * ordinary text lines.
 * @return start of the current line contents in the format buffer
 */
uint8_t* get_format_line_contents(void)
{
    return (current_format_line[0] == COMMAND_PREFIX ||
               current_format_line[0] == RULER_PREFIX)
               ? current_format_line
               : current_format_line + offsetof(line_t, text);
}

/**
 * Updates markers to point into the format buffer.
 * Retargets markers from document heap into the current format line.
 */
void update_markers_to_format_buffer(void)
{
    uint8_t* line_start = current_line_ptr;
    uint8_t offset = 0;

    do
    {
        uint8_t idx = find_marker_at_position(offset, line_start);

        if (idx != NUM_MARKERS)
        {
            uint8_t* base_ptr = get_format_line_contents();
            markers_array[idx] = base_ptr + offset;
        }
        uint8_t ch = current_line_ptr[offset];

        if (ch == 0x0d)
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
        uint8_t* saved_start = area_start_ptr;
        area_start_ptr = area_end_ptr;
        area_end_ptr = saved_start;
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
    if (write_line_back_to_document())
        memory_full();
}

/**
 * Writes an unpacked edit buffer back to the document.
 * Resizes the heap, invalidates dirty rulers, and copies bytes and markers,
 * replacing fill bytes with spaces and appending a carriage return.
 * @return true if the heap could not grow, leaving the buffer ready for retry
 */
bool write_line_back_to_document(void)
{
    if (!edit_buffer_unpacked_flag)
        return false;

    area_size = 0;
    screen_column = get_line_length();
    int growth = (int)screen_column - (int)edit_line_len;
    if (growth > 0)
    {
        area_size = growth;
        if (!make_space_for_insertion(current_line_ptr, area_size))
            return true;
    }
    else if (growth < 0)
    {
        area_size = -growth;
        scratch_scan_ptr = adjust_pointers(current_line_ptr, area_size);
    }

    if (edit_buffer_contains_ruler_flag && edit_buffer_dirty_flag)
        clamp_ptr6_to_document();
    edit_buffer_dirty_flag = false;
    edit_buffer_unpacked_flag = false;
    edit_buffer_contains_ruler_flag = false;

    uint8_t* source = get_format_line_contents();
    area_size = source - ram;
    uint8_t remaining = screen_column;
    edit_line_len = remaining;
    uint8_t copy_pos = 0;
    uint8_t ch;
    do
    {
        ch = remaining == 0 ? 0x0d : source[copy_pos];
        if (ch == 0x10)
            ch = 0x20;

        for (;;)
        {
            uint8_t marker = find_marker_at_position(copy_pos, source);
            if (marker == NUM_MARKERS)
                break;
            markers_array[marker] = current_line_ptr + copy_pos;
        }
        current_line_ptr[copy_pos++] = ch;
        remaining--;
    } while (ch != 0x0d);
    return false;
}

/**
 * Moves the document suffix and adjusts tracked pointers for a size change.
 * Clears markers inside a deleted region; shifts other tracked pointers at or
 * beyond insert_ptr by the opposite of size_delta. Updates top after the move.
 * @param insert_ptr destination of the moved document suffix
 * @param size_delta bytes to delete, or a negative value to expand the heap
 * @return original source position of the moved suffix
 */
uint8_t* adjust_pointers(uint8_t* insert_ptr, ptrdiff_t size_delta)
{
    uint8_t* source = insert_ptr + size_delta;
    uint8_t** slots = (uint8_t**)&pointer_array;
    for (size_t i = 0; i < sizeof(pointer_array) / sizeof(*slots); i++)
    {
        if (slots[i] < insert_ptr)
            continue;
        if (i < ARRAY_SIZE(markers_array) && slots[i] < source)
            slots[i] = NULL;
        else
            slots[i] -= size_delta;
    }

    size_t copy_len = strlen((char*)source) + 1;
    memmove(insert_ptr, source, copy_len);
    top = insert_ptr + copy_len - 1;
    return source;
}

/**
 * Finds the first marker pointing at a buffer position.
 * @param buf_offset byte offset from target_ptr
 * @param target_ptr base pointer into a buffer or document line
 * @return lowest matching marker index, or NUM_MARKERS if none
 */
uint8_t find_marker_at_position(uint8_t buf_offset, uint8_t* target_ptr)
{
    uint8_t* position = target_ptr + buf_offset;
    for (uint8_t marker = 0; marker < NUM_MARKERS; marker++)
        if (markers_array[marker] == position)
            return marker;
    return NUM_MARKERS;
}

/**
 * Returns the length of the current edit line, excluding trailing fill bytes.
 * Includes the three-byte header for command and ruler lines.
 * @return line length in bytes, including any command header
 */
uint8_t get_line_length(void)
{
    uint8_t length = MAX_LINE_LENGTH;
    while (length != 0 && current_line_buffer.text[length - 1] == 0x10)
        length--;
    if (check_for_command_prefix(current_format_line[0]) != NO_COMMAND_PREFIX)
        length += offsetof(line_t, text);
    return length;
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
    uint8_t* old_top = top;
    uint8_t* new_top = top + size_delta;
    if (new_top >= himem)
        return false;
    top = new_top;

    uint8_t** slots = (uint8_t**)&pointer_array;
    for (size_t slot = 0; slot < sizeof(pointer_array) / sizeof(*slots); slot++)
        if (slots[slot] >= insert_ptr)
            slots[slot] += size_delta;

    size_t suffix_length = (size_t)(old_top - insert_ptr) + 1;
    memmove(insert_ptr + size_delta, insert_ptr, suffix_length);
    return true;
}

/**
 * Read one byte from the current file.
 * @return next byte, or 0 on EOF or NUL
 */
static uint8_t get_byte_from_file(void)
{
    int byte = fgetc(file_ptr);

    if (byte == EOF || byte == 0)
        return 0;
    return (uint8_t)byte;
}
