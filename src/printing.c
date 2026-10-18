#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "io.h"
#include "printing.h"
#include "macro.h"
#include "document.h"
#include "cli.h"
#include "view.h"

static uint8_t footer_margin;
static uint8_t footer_text_maybe[0x42];
static bool footers_enabled_flag;
static bool formatted_line_written_flag;
static uint8_t header_margin;
static bool headers_enabled_flag;
static uint8_t left_margin;
static uint8_t line_spacing;
static bool page_break_flag;
static bool page_break_pending_flag;
static uint8_t page_length;
static uint8_t page_lines_remaining;
static uint8_t print_extra_space_accum;
static uint8_t print_last_microspacing;
static uint8_t print_running_total_accum;
static uint8_t rhs_extra_margin;
static uint8_t rw_file_handle;
static bool two_sided_flag;

static bool evaluate_expression_from_fmt_cmd(
    int* result, uint8_t* pos, uint8_t idx);
static bool get_page_parity(void);
static bool parse_boolean_from_fmt_cmd(uint8_t* pos, uint8_t* value);
static bool parse_word_flag(uint8_t* target_ptr, uint8_t* pos, uint8_t* value);
static const printer_driver_t default_printer_driver;
static uint8_t add_justification_spaces(uint8_t idx);
static uint8_t convert_char_for_printing(
    uint8_t cur_ch, uint8_t* idx, bool* is_tab);
static uint8_t copy_header_footer_text(uint8_t* copy_ptr);
static uint8_t expand_line(void);
static uint8_t get_current_fmt_cmd_byte(uint8_t* pos);
static uint8_t get_line_width(uint8_t* insert_ptr);
static uint8_t get_next_fmt_cmd_byte(uint8_t* pos);
static uint8_t get_right_margin(void);
static uint8_t process_header_footer_line(uint8_t* copy_ptr);
static uint8_t scan_string_length(uint8_t y_start, uint8_t* insert_ptr);
static uint8_t* compute_header_left_section(uint8_t* insert_ptr);
static uint8_t* compute_header_middle_section(uint8_t* insert_ptr);
static uint8_t* compute_header_odd_page_section(uint8_t* insert_ptr);
static void compute_lines_remaining_on_page(void);
static void default_print_char(uint8_t cur_ch);
static void default_printer_getflags(uint8_t* idx, uint8_t* pos);
static void default_printer_microspace(void);
static void default_printer_off(void);
static void default_printer_on(void);
static void emit_microspacing_spaces(uint8_t cur_ch, uint8_t idx);
static void microspace_word_processor(uint8_t* pos);
static void output_left_margin(void);
static void page_eject_fmt(void);
static void print_char_x_times(uint8_t cur_ch, uint8_t idx);
static void print_loop(uint8_t* print_doc_ptr);
static void print_newline(void);
static void print_output_buffer(void);
static void print_vertical_space(uint8_t idx);
static void process_page_footer(void);
static void render_header_or_footer(uint8_t* insert_ptr);
static void render_new_page(void);
static void reset_print_registers(void);
static void set_rw_file_handle(uint8_t cur_ch);
static void start_microspacing_if_active(uint8_t cur_ch);
static void store_to_output_buffer(uint8_t cur_ch, uint8_t* copy_ptr);
static void write_output_buffer_to_format_line(uint8_t cur_ch);

static void ce_fmt_cmd(void);
static void rj_fmt_cmd(void);
static void df_fmt_cmd(void);
static void dh_fmt_cmd(void);
static void em_fmt_cmd(void);
static void sr_fmt_cmd(void);
static void pe_fmt_cmd(void);
static void tm_fmt_cmd(void);
static void bm_fmt_cmd(void);
static void pl_fmt_cmd(void);
static void ts_fmt_cmd(void);
static void fo_fmt_cmd(void);
static void he_fmt_cmd(void);
static void ht_fmt_cmd(void);
static void hm_fmt_cmd(void);
static void fm_fmt_cmd(void);
static void lm_fmt_cmd(void);
static void ls_fmt_cmd(void);
static void op_fmt_cmd(void);
static void ep_fmt_cmd(void);
static void lj_fmt_cmd(void);
static void pb_fmt_cmd(void);

static const formatting_command_t commands_table[] = {
    {.name = {'C', 'E'}, .callback = ce_fmt_cmd},
    {.name = {'R', 'J'}, .callback = rj_fmt_cmd},
    {.name = {'D', 'F'}, .callback = df_fmt_cmd},
    {.name = {'D', 'H'}, .callback = dh_fmt_cmd},
    {.name = {'D', 'M'}, .callback = dm_fmt_cmd},
    {.name = {'E', 'M'}, .callback = em_fmt_cmd},
    {.name = {'S', 'R'}, .callback = sr_fmt_cmd},
    {.name = {'P', 'E'}, .callback = pe_fmt_cmd},
    {.name = {'T', 'M'}, .callback = tm_fmt_cmd},
    {.name = {'B', 'M'}, .callback = bm_fmt_cmd},
    {.name = {'P', 'L'}, .callback = pl_fmt_cmd},
    {.name = {'T', 'S'}, .callback = ts_fmt_cmd},
    {.name = {'F', 'O'}, .callback = fo_fmt_cmd},
    {.name = {'H', 'E'}, .callback = he_fmt_cmd},
    {.name = {'H', 'T'}, .callback = ht_fmt_cmd},
    {.name = {'H', 'M'}, .callback = hm_fmt_cmd},
    {.name = {'F', 'M'}, .callback = fm_fmt_cmd},
    {.name = {'L', 'M'}, .callback = lm_fmt_cmd},
    {.name = {'L', 'S'}, .callback = ls_fmt_cmd},
    {.name = {'O', 'P'}, .callback = op_fmt_cmd},
    {.name = {'E', 'P'}, .callback = ep_fmt_cmd},
    {.name = {'L', 'J'}, .callback = lj_fmt_cmd},
    {.name = {'P', 'B'}, .callback = pb_fmt_cmd},
};

static const uint8_t on_off_table[] = {0x4f, 0x4e, 1, 'O', 'F', 'F', 0, 0xff};

/**
 * Emits microspacing spaces while preserving the character being printed.
 *
 * @param cur_ch character to preserve across the printer callback
 * @param idx requested microspacing amount
 */

/**
 * Converts cur_ch character for printing and updates its display width.
 *
 * @param cur_ch character to convert
 * @param[out] idx converted character width
 * @param is_tab tab-state carried between characters
 * @return converted character
 */

static const printer_driver_t default_printer_driver = {
    .print_char = default_print_char,
    .printer_on = default_printer_on,
    .printer_off = default_printer_off,
    .printer_microspace = default_printer_microspace,
    .printer_getflags = default_printer_getflags,
};

/* Forward declarations for sorted functions (root first) */
static void set_rw_file_handle(uint8_t cur_ch);
static void print_loop(uint8_t* print_doc_ptr);
static void write_output_buffer_to_format_line(uint8_t pad_len);
static uint8_t expand_line(void);
static uint8_t process_header_footer_line(uint8_t* copy_ptr);
static void store_to_output_buffer(uint8_t cur_ch, uint8_t* copy_ptr);
static void eject_two_pages(void);
static void page_eject_fmt(void);
static bool parse_boolean_from_fmt_cmd(uint8_t* pos, uint8_t* value);
static bool parse_word_flag(uint8_t* target_ptr, uint8_t* pos, uint8_t* value);
static bool evaluate_expression_from_fmt_cmd(
    int* result, uint8_t* pos, uint8_t idx);
static uint8_t get_next_fmt_cmd_byte(uint8_t* pos);
static uint8_t get_current_fmt_cmd_byte(uint8_t* pos);
static void process_page_footer(void);
static void microspace_word_processor(uint8_t* pos);
static void render_new_page(void);
static void print_newline(void);
static void print_vertical_space(uint8_t idx);
static void render_header_or_footer(uint8_t* text);
static void print_output_buffer(void);
static void start_microspacing_if_active(uint8_t cur_ch);
static void emit_microspacing_spaces(uint8_t cur_ch, uint8_t idx);
static uint8_t* compute_header_left_section(uint8_t* insert_ptr);
static uint8_t* compute_header_middle_section(uint8_t* insert_ptr);
static uint8_t* compute_header_odd_page_section(uint8_t* insert_ptr);
static uint8_t get_line_width(uint8_t* insert_ptr);
static uint8_t scan_string_length(uint8_t pos, uint8_t* insert_ptr);
static uint8_t get_right_margin(void);
static uint8_t copy_header_footer_text(uint8_t* text);
static void output_left_margin(void);
static bool get_page_parity(void);
static uint8_t add_justification_spaces(uint8_t idx);
static void print_char_x_times(uint8_t cur_ch, uint8_t idx);
static uint8_t convert_char_for_printing(
    uint8_t cur_ch, uint8_t* idx, bool* is_tab);
static void reset_print_registers(void);
static void compute_lines_remaining_on_page(void);
static void default_print_char(uint8_t cur_ch);
static void default_printer_on(void);
static void default_printer_off(void);
static void default_printer_microspace(void);
static void default_printer_getflags(uint8_t* idx, uint8_t* pos);

/**
 * Prints the current document or the files named in the command.
 * Initializes print state, processes each input, and finishes any open page.
 *
 * @param scan scan state for parsing the print command arguments
 */
void print_document(scan_state_t* scan)
{
    check_not_continuous_editing();
    check_for_at_least_150_bytes_free();
    reset_print_registers();
    uint8_t* print_doc_ptr = top + 3;

    macro_init(print_doc_ptr);
    page_break_pending_flag = false;
    print_xpos = 0;
    printing_from_file_flag = false;
    current_ruler_ptr = current_ruler_buffer;
    find_margins_of_current_ruler_buffer();

    if (scan_input_buffer(input_buffer, scan))
    {
        printing_from_file_flag = true;
        print_source_ptr = ram;
        print_loop(print_doc_ptr);
    }
    while (parse_optional_filename_from_command(scan))
    {
        set_rw_file_handle(0x0d);
        print_loop(print_doc_ptr);
    }
    if (page_break_pending_flag)
        process_page_footer();
}

/**
 * Sets the read/write file handle for the current operation.
 *
 * @param cur_ch handle value to store
 */
static void set_rw_file_handle(uint8_t cur_ch)
{
    rw_file_handle = cur_ch;
}

/**
 * Advances page accounting and prints the spacing after an output line.
 * Preserves the byte-sized spacing calculation at the bottom of a page.
 */
static void advance_print_line_spacing(void)
{
    uint8_t spacing = line_spacing;
    uint8_t remaining_lines = page_lines_remaining - line_spacing - 1;
    if (page_lines_remaining <= line_spacing)
    {
        remaining_lines = 0;
        spacing = page_lines_remaining - 1;
    }
    page_lines_remaining = remaining_lines;
    print_vertical_space(spacing);
}

/**
 * Loads a ruler from a prepared print line and recalculates the margins.
 * @param line prepared line, including its three-byte ruler header
 */
static void load_print_ruler(uint8_t* line)
{
    uint8_t source_pos = 3;
    uint8_t dest_pos = 0;
    uint8_t ch;
    do
    {
        ch = line[source_pos++];
        current_ruler_buffer[dest_pos++] = ch;
    } while (ch != 0x0d);
    find_margins_of_current_ruler_buffer();
}

/**
 * Prints an ordinary line, then updates its line register and page spacing.
 * @param line prepared source line
 * @param pos first content byte, skipping any command header
 * @param is_tab tab state carried between printed lines
 */
static void print_line_contents(uint8_t* line, uint8_t pos, bool* is_tab)
{
    uint8_t ch;
    do
    {
        uint8_t char_width;
        ch = convert_char_for_printing(line[pos++], &char_width, is_tab);
        print_char_x_times(ch, char_width);
    } while (ch != 0x0d);
    register_value_array['L' - 'A']++;
    advance_print_line_spacing();
}

/**
 * Processes print lines, applying rulers, formatting commands, and macros.
 * Opens and finishes pages as needed and selects ordinary or microspaced
 * output.
 * @param print_doc_ptr read buffer for source lines and macro invocations
 */
static void print_loop(uint8_t* print_doc_ptr)
{
    uint8_t* macro_cursor = NULL;
    bool is_tab = false;
    for (;;)
    {
        uint8_t page_context = page_break_pending_flag;
        if (page_break_pending_flag)
        {
            page_context = page_lines_remaining;
            if (page_context == 0)
                process_page_footer();
        }
        uint8_t* line = prepare_output_line(print_doc_ptr, &macro_cursor);
        if (line == NULL)
            return;
        start_microspacing_if_active(page_context);
        scratch_offset = 0;

        if (deref_and_check_for_command_prefix(0, line) != NO_COMMAND_PREFIX)
        {
            scratch_offset = 3;
            if (*line == RULER_PREFIX)
            {
                load_print_ruler(line);
                continue;
            }
            const formatting_command_t* command = lookup_formatting_command();
            if (command != NULL)
            {
                formatted_line_written_flag = false;
                command->callback();
                if (!formatted_line_written_flag)
                    continue;
            }
            else if (!macro_try_invoke(&macro_cursor) || macro_executing_flag)
                continue;
        }

        if (!page_break_pending_flag)
            render_new_page();
        output_left_margin();
        column_position = 0;
        uint8_t pos = scratch_offset;
        if ((print_flags & 0x80) != 0 && microspacing_count != 0)
            microspace_word_processor(&pos);
        else
            print_line_contents(line, pos, &is_tab);
    }
}

/**
 * Looks up the current line's two-letter formatting command, case-sensitively.
 * @return pointer to the matching table entry, or NULL if not found
 */
const formatting_command_t* lookup_formatting_command(void)
{
    for (size_t pos = 0; pos < ARRAY_SIZE(commands_table); pos++)
        if (heap_format_line_ptr->command[0] == commands_table[pos].name[0] &&
            heap_format_line_ptr->command[1] == commands_table[pos].name[1])
            return &commands_table[pos];
    return NULL;
}

/**
 * Handles the LJ formatting command to left-justify the current line.
 */
static void lj_fmt_cmd(void)
{
    if (expand_line() == 0)
        return;
    write_output_buffer_to_format_line(0);
}

/**
 * Handles the CE formatting command to centre the current line.
 */
static void ce_fmt_cmd(void)
{
    uint8_t expanded_len = expand_line();
    if (expanded_len == 0)
        return;

    uint8_t pad_len = 0;
    if (ruler_right_stop != 0)
    {
        uint8_t width = ruler_right_stop - ruler_left_stop;
        uint8_t centre = ruler_left_stop + width / 2 + 1;
        uint8_t half_text_len = expanded_len / 2;
        if (centre > half_text_len)
            pad_len = centre - half_text_len;
    }
    write_output_buffer_to_format_line(pad_len);
}

/**
 * Handles the RJ formatting command to right-justify the current line.
 */
static void rj_fmt_cmd(void)
{
    uint8_t expanded_len = expand_line();

    if (expanded_len == 0)
        return;
    expanded_len -= 2;

    if (expanded_len >= ruler_right_stop)
    {
        write_output_buffer_to_format_line(0);

        return;
    }
    screen_column = expanded_len;
    uint8_t pad_len = ruler_right_stop;

    pad_len -= screen_column;
    write_output_buffer_to_format_line(pad_len);
}

/**
 * Writes the contents of the output buffer to the current format line.
 *
 * @param pad_len number of leading spaces to pad before the buffered text
 */
static void write_output_buffer_to_format_line(uint8_t pad_len)
{
    uint8_t pos = 0;
    for (; pos < pad_len; pos++)
        heap_format_line_ptr->text[pos] = ' ';

    uint8_t idx = 0;
    uint8_t cur_ch;
    do
    {
        cur_ch = output_buffer[idx++];
        heap_format_line_ptr->text[pos++] = cur_ch;
    } while (cur_ch != '\r');
    formatted_line_written_flag = true;
}

/**
 * Expands the current format line into the output buffer.
 *
 * Skips leading spaces, expands register references introduced by '|', and
 * terminates the output with a carriage return at the maximum line length.
 *
 * @return output length including the carriage return, excluding control codes
 * in microspacing mode; 0 if empty
 */
static uint8_t expand_line(void)
{
    uint8_t output_len = 0;
    uint8_t pos = 0;
    screen_column = 0;

    if (get_current_fmt_cmd_byte(&pos) == 0)
        return 0;

    for (;;)
    {
        uint8_t next_ch = heap_format_line_ptr->text[pos++];
        if (next_ch == '|')
        {
            next_ch = heap_format_line_ptr->text[pos];
            if (next_ch != '\r')
            {
                pos++;
                render_register(next_ch, output_len);
                if (screen_row > output_len)
                    output_len = screen_row;
                continue;
            }
        }

        for (;;)
        {
            output_buffer[output_len++] = next_ch;
            if (check_for_control_code(next_ch) != NO_CONTROL_CODE)
                screen_column++;

            if (next_ch == '\r')
            {
                if ((print_flags & 0x80) != 0)
                    output_len -= screen_column;
                return output_len;
            }
            if (output_len < MAX_LINE_LENGTH - 1)
                break;
            next_ch = '\r';
        }
    }
}

/**
 * Handles the DF formatting command to define footer text.
 */
static void df_fmt_cmd(void)
{
    process_header_footer_line(footer_text_maybe);
}

/**
 * Handles the DH formatting command to define header text.
 */
static void dh_fmt_cmd(void)
{
    process_header_footer_line(header_text_maybe);
}

/**
 * Processes up to 63 bytes after the header/footer delimiter, stopping at a
 * carriage return. Marks delimiters and the carriage return with the high bit,
 * replaces other control characters with spaces, and appends three 0x80 bytes.
 *
 * @param copy_ptr destination buffer for the processed text
 * @return last character processed
 */
static uint8_t process_header_footer_line(uint8_t* copy_ptr)
{
    scratch_index = 0;
    search_target_len = 0;
    uint8_t delimiter = heap_format_line_ptr->text[0];
    uint8_t next_ch;

    for (uint8_t pos = 1; pos <= 63; pos++)
    {
        next_ch = heap_format_line_ptr->text[pos];
        if (next_ch == '\r')
            next_ch |= 0x80;
        else
        {
            if (next_ch < 0x1b)
                next_ch = ' ';
            if (next_ch == delimiter)
                next_ch |= 0x80;
        }
        store_to_output_buffer(next_ch, copy_ptr);
        if (next_ch == 0x8d)
            break;
    }

    store_to_output_buffer(0x80, copy_ptr);
    store_to_output_buffer(0x80, copy_ptr);
    store_to_output_buffer(0x80, copy_ptr);
    return next_ch;
}

/**
 * Stores a byte into the header/footer output buffer at the current index.
 *
 * @param cur_ch byte to store
 * @param copy_ptr destination buffer (header or footer text area)
 */
static void store_to_output_buffer(uint8_t cur_ch, uint8_t* copy_ptr)
{
    uint8_t pos = scratch_index;

    copy_ptr[pos] = cur_ch;
    pos++;
    scratch_index = pos;
}

/**
 * Handles the EM formatting command, which has no effect outside a macro
 * definition.
 */
static void em_fmt_cmd(void) {}

/**
 * Handles the SR formatting command to evaluate an expression and store
 * the result in a register.
 */
static void sr_fmt_cmd(void)
{
    uint8_t pos = 0;
    uint8_t cur_ch = get_current_fmt_cmd_byte(&pos);

    if (cur_ch == 0)
        return;
    pos++;
    unsigned int* register_value = get_register_address(cur_ch);

    if (register_value == NULL)
        return;
    int reg_value;
    evaluate_expression_from_fmt_cmd(&reg_value, &pos, 0);
    *register_value = reg_value;
}

/**
 * Handles the PL formatting command to set the page length.
 */
static void pl_fmt_cmd(void)
{
    uint8_t pos = 0;

    int value;
    evaluate_expression_from_fmt_cmd(&value, &pos, 0);
    page_length = value;
}

/**
 * Handles the TS formatting command to set two-sided mode and margin.
 */
static void ts_fmt_cmd(void)
{
    uint8_t pos = 0;
    uint8_t flag_value;

    if (parse_boolean_from_fmt_cmd(&pos, &flag_value))
        return;
    two_sided_flag = flag_value != 0;
    int value;
    evaluate_expression_from_fmt_cmd(&value, &pos, 0);
    rhs_extra_margin = value;
}

/**
 * Handles the TM formatting command to set the top margin.
 */
static void tm_fmt_cmd(void)
{
    uint8_t pos = 0;

    int value;
    evaluate_expression_from_fmt_cmd(&value, &pos, 0);
    top_margin = value;
}

/**
 * Handles the BM formatting command to set the bottom margin.
 */
static void bm_fmt_cmd(void)
{
    uint8_t pos = 0;

    int value;
    evaluate_expression_from_fmt_cmd(&value, &pos, 0);
    bottom_margin = value;
}

/**
 * Handles the HM formatting command to set the header margin.
 */
static void hm_fmt_cmd(void)
{
    uint8_t pos = 0;

    int value;
    evaluate_expression_from_fmt_cmd(&value, &pos, 0);
    header_margin = value;
}

/**
 * Handles the FM formatting command to set the footer margin.
 */
static void fm_fmt_cmd(void)
{
    uint8_t pos = 0;

    int value;
    evaluate_expression_from_fmt_cmd(&value, &pos, 0);
    footer_margin = value;
}

/**
 * Handles the LM formatting command to set the left margin.
 */
static void lm_fmt_cmd(void)
{
    uint8_t pos = 0;

    int value;
    evaluate_expression_from_fmt_cmd(&value, &pos, 0);
    left_margin = value;
}

/**
 * Handles the LS formatting command to set line spacing.
 */
static void ls_fmt_cmd(void)
{
    uint8_t pos = 0;

    int value;
    evaluate_expression_from_fmt_cmd(&value, &pos, 0);
    line_spacing = value;
}

/**
 * Handles the PE formatting command to conditionally eject the page.
 */
static void pe_fmt_cmd(void)
{
    uint8_t pos = 0;

    int value;
    evaluate_expression_from_fmt_cmd(&value, &pos, 0);

    if (value == 0)
    {
        page_eject_fmt();

        return;
    }
    if (value < page_lines_remaining)
        return;

    if (page_break_pending_flag)
    {
        page_eject_fmt();

        return;
    }
}

/**
 * Handles the OP formatting command to eject to an odd page.
 */
static void op_fmt_cmd(void)
{
    uint8_t cur_ch = (uint8_t)register_value_array['P' - 'A'];

    if ((cur_ch & 1) == 0)
    {
        page_eject_fmt();

        return;
    }
    cur_ch >>= 1;
    eject_two_pages();
}

/**
 * Handles the EP formatting command to eject to an even page.
 */
static void ep_fmt_cmd(void)
{
    uint8_t cur_ch = (uint8_t)register_value_array['P' - 'A'];

    if (cur_ch & 1)
    {
        page_eject_fmt();

        return;
    }
    cur_ch >>= 1;
    eject_two_pages();
}

/**
 * Ejects two consecutive pages.
 */
static void eject_two_pages(void)
{
    page_eject_fmt();
    page_eject_fmt();
}

/**
 * Performs a page eject, rendering a new page if needed and processing
 * the page footer.
 */
static void page_eject_fmt(void)
{
    if (!page_break_pending_flag)
        render_new_page();
    process_page_footer();
}

/**
 * Handles the FO formatting command to enable or disable footers.
 */
static void fo_fmt_cmd(void)
{
    uint8_t pos = 0;
    uint8_t flag_value;

    if (parse_boolean_from_fmt_cmd(&pos, &flag_value))
        return;
    footers_enabled_flag = flag_value != 0;
}

/**
 * Handles the HE formatting command to enable or disable headers.
 */
static void he_fmt_cmd(void)
{
    uint8_t pos = 0;
    uint8_t flag_value;

    if (parse_boolean_from_fmt_cmd(&pos, &flag_value))
        return;
    headers_enabled_flag = flag_value != 0;
}

/**
 * Handles the PB formatting command to set the page break flag.
 */
static void pb_fmt_cmd(void)
{
    uint8_t pos = 0;
    uint8_t flag_value;

    if (parse_boolean_from_fmt_cmd(&pos, &flag_value))
        return;
    page_break_flag = flag_value != 0;
}

/**
 * Handles the HT formatting command to set highlight codes.
 */
static void ht_fmt_cmd(void)
{
    uint8_t pos = 0;
    uint8_t first_ch = get_current_fmt_cmd_byte(&pos);
    if (first_ch == 0)
        return;

    uint8_t highlight_index = first_ch;
    if (first_ch == '-' || first_ch == '*')
        pos++;
    else
    {
        int value;
        evaluate_expression_from_fmt_cmd(&value, &pos, first_ch);
        highlight_index = value;
        if (highlight_index == 0)
            return;
        highlight_index--;
        if (highlight_index >= 2)
            return;
    }

    int highlight_value;
    evaluate_expression_from_fmt_cmd(&highlight_value, &pos, first_ch);
    highlight_code[highlight_index] = highlight_value;
}

/**
 * Parses a boolean value from the current format command argument.
 *
 * @param pos cursor into the format line, advanced past the token on success
 * @param value output for the parsed value
 * @return true on parse error, false on success
 */
static bool parse_boolean_from_fmt_cmd(uint8_t* pos, uint8_t* value)
{
    uint8_t cur_ch = get_current_fmt_cmd_byte(pos);
    *value = cur_ch;

    if (cur_ch == 0)
        return true;
    return parse_word_flag(heap_format_line_ptr->text, pos, value);
}

/**
 * Parses case-insensitive ON/OFF or a single 0/1 from the format line.
 * Both forms produce an integer value of 0 or 1.
 *
 * @param target_ptr line buffer to parse from
 * @param pos cursor into the buffer, left at the first byte after a matched
 * word or advanced past a numeric flag; may also change on failure
 * @param value output for the parsed byte
 * @return true on parse error, false on success
 */
static bool parse_word_flag(uint8_t* target_ptr, uint8_t* pos, uint8_t* value)
{
    uint8_t first_ch = target_ptr[*pos];
    if (first_ch == '0' || first_ch == '1')
    {
        *value = first_ch - '0';
        (*pos)++;
        if (*pos != 0)
            return false;
    }

    uint8_t before_token = --(*pos);
    uint8_t table_pos = 0xff;
    for (;;)
    {
        uint8_t ch;
        do
        {
            (*pos)++;
            ch = toupper(target_ptr[*pos]);
            table_pos++;
        } while (ch == on_off_table[table_pos]);

        uint8_t entry = on_off_table[table_pos];
        if (entry >= 0x80)
        {
            *value = entry;
            return true;
        }
        if (entry < 0x20)
        {
            *value = entry;
            return false;
        }

        // Skip the rest of this word to find the next table entry.
        do
        {
            entry = on_off_table[++table_pos];
        } while (entry >= 0x20 && entry < 0x80);
        if (entry >= 0x80)
        {
            *value = entry;
            return true;
        }
        *pos = before_token;
        if (on_off_table[table_pos + 1] >= 0x80)
        {
            *value = on_off_table[table_pos + 1];
            return true;
        }
    }
}

/**
 * Evaluates an arithmetic expression from the format command line.
 *
 * Evaluates addition and subtraction from left to right. Register references
 * render into the output buffer and retain the current numeric value.
 *
 * @param result output for the evaluated value
 * @param pos cursor into the format line, advanced as the expression is
 * consumed
 * @param idx output position used when expanding register references
 * @return false if a term was expected at end of line (including after a
 * trailing operator), true otherwise
 */
static bool evaluate_expression_from_fmt_cmd(
    int* result, uint8_t* pos, uint8_t idx)
{
    int total = 0;
    int value = 0;
    uint8_t operation = 0;
    for (;;)
    {
        uint8_t next_ch = get_current_fmt_cmd_byte(pos);
        if (next_ch == 0)
        {
            *result = value;
            return false;
        }
        if (next_ch == '|')
        {
            next_ch = get_next_fmt_cmd_byte(pos);
            if (next_ch == 0)
                break;
            (*pos)++;
            render_register(next_ch, idx);
        }
        else
            parse_decimal_number(&value, pos);

        if (operation == '+')
            value = total + value;
        else if (operation == '-')
            value = total - value;
        total = value;

        operation = get_current_fmt_cmd_byte(pos);
        if (operation != '+' && operation != '-')
            break;
        (*pos)++;
    }
    *result = value;
    return true;
}

/**
 * Advances past one byte and reads the next non-space byte.
 *
 * @param pos cursor into the line
 * @return the next non-space byte, or 0 if at end
 */
static uint8_t get_next_fmt_cmd_byte(uint8_t* pos)
{
    (*pos)++;

    return get_current_fmt_cmd_byte(pos);
}

/**
 * Reads the next non-space byte from the current format command line.
 *
 * @param pos cursor into the line, advanced past spaces
 * @return the byte value, or 0 if at end of line
 */
static uint8_t get_current_fmt_cmd_byte(uint8_t* pos)
{
    while (1)
    {
        uint8_t val = heap_format_line_ptr->text[*pos];

        if (val == 0x0d)
            return 0;

        if (val != 0x20)
            return val;
        (*pos)++;
    }
}

/**
 * Handles page footer processing including vertical spacing and footer
 * rendering.
 */
static void process_page_footer(void)
{
    if (page_break_flag)
    {
        print_vertical_space(page_lines_remaining);
        print_vertical_space(footer_margin);

        if (footers_enabled_flag)
            render_header_or_footer(footer_text_maybe);
        print_newline();
        print_vertical_space(bottom_margin);
    }
    register_value_array['P' - 'A']++;
    register_value_array['L' - 'A'] = 1;
    page_break_pending_flag = false;
}

typedef struct microspace_line
{
    uint32_t accumulated_width;
    uint8_t word_start;
    uint8_t pending_spaces;
    uint8_t overflow;
    uint8_t pending_words;
} microspace_line_t;

/**
 * Scans a source line into output_buffer and gathers microspacing statistics.
 * @param pos source cursor, advanced past the terminating carriage return
 * @param is_tab tab state carried between conversion calls
 * @return accumulated word, space, and margin information
 */
static microspace_line_t collect_microspace_line(uint8_t* pos, bool* is_tab)
{
    microspace_line_t line = {0};
    uint8_t output_pos = 0;
    column_position = 0;
    screen_column = 0;
    for (;;)
    {
        uint8_t char_width;
        (void)convert_char_for_printing(
            scratch_line_ptr[*pos], &char_width, is_tab);
        uint8_t ch = scratch_line_ptr[(*pos)++];
        bool count_space = false;

        if (ch == 0x1a)
        {
            if ((screen_column & 0x80) != 0 && line.pending_spaces != 0)
            {
                line.pending_words++;
                if (line.pending_words != 0)
                    continue;
            }
            if ((screen_column & 0x80) == 0 || line.pending_spaces != 0)
            {
                line.word_start = column_position;
                ch = 0x20;
            }
            else
            {
                ch = 0x20;
                line.overflow--;
                count_space = true;
            }
        }
        else if (ch == 0x20)
        {
            if ((screen_column & 0x80) == 0)
                line.word_start = column_position;
            else
                count_space = true;
        }
        else if (ch >= 0x20)
            line.accumulated_width += 0x100;
        else if (ch == 9 || ch == 0x0b)
        {
            line.word_start = column_position;
            screen_column = 0;
            line.accumulated_width = 0;
            line.pending_spaces = 0;
            line.overflow = 0;
            line.pending_words = 0;
        }

        if (count_space)
        {
            if (line.overflow != 0 && (line.overflow & 0x80) == 0)
            {
                line.pending_words++;
                line.overflow = 0;
                continue;
            }
            line.pending_spaces++;
        }
        output_buffer[output_pos++] = ch;
        if (ch == 0x0d)
            return line;
        if (ch != 0x20 && line.pending_spaces != 0)
        {
            line.accumulated_width += ((uint32_t)line.pending_words << 16) |
                                      ((uint32_t)line.pending_spaces << 8) |
                                      line.pending_spaces;
            line.pending_spaces = 0;
            line.overflow = 0;
            line.pending_words = 0;
        }
    }
}

/**
 * Checks the accumulated width against the ruler's right margin.
 * @param line statistics gathered while scanning the line
 * @return true if the existing microspacing calculation permits justification
 */
static bool can_microspace_line(const microspace_line_t* line)
{
    uint32_t word_count = line->accumulated_width >> 16;
    int margin_width = ruler_right_stop - line->word_start;
    if (word_count == 0 || ruler_right_stop == 0 || margin_width < 0)
        return false;
    return (uint8_t)(margin_width - word_count +
                     (margin_width >= (int)word_count)) ==
           (uint8_t)((line->accumulated_width >> 8) & 0xff);
}

/**
 * Prints the prepared buffer using the distributed microspacing amounts.
 * @param pos output cursor, reset before printing and advanced through the line
 * @param word_start last column using ordinary character spacing
 * @param is_tab tab state carried from the scan
 */
static void print_microspaced_line(
    uint8_t* pos, uint8_t word_start, bool* is_tab)
{
    uint16_t scaled_remainder =
        (uint16_t)print_running_total_accum * microspacing_count;
    justify_gap_count = print_extra_space_accum;
    print_running_total_accum = scaled_remainder % justify_gap_count;
    print_extra_space_accum = (uint8_t)(scaled_remainder / justify_gap_count);
    *pos = 0;
    column_position = 0;

    uint8_t emitted_byte;
    do
    {
        uint8_t char_width;
        emitted_byte = convert_char_for_printing(
            output_buffer[++(*pos)], &char_width, is_tab);
        if (column_position <= word_start)
        {
            print_char_x_times(emitted_byte, char_width);
            continue;
        }

        if (emitted_byte == ' ')
        {
            uint8_t spacing = microspacing_count + print_extra_space_accum;
            uint8_t remainder = print_running_total_accum;
            if (remainder != 0)
            {
                spacing++;
                print_running_total_accum--;
            }
            emit_microspacing_spaces(remainder, spacing);
        }
        else
            emit_microspacing_spaces(emitted_byte, microspacing_count);
        print_char(emitted_byte);
    } while (emitted_byte != '\r');
}

/**
 * Processes a line for microspaced justification during printing.
 * Scans spacing statistics, selects the output path, and advances page spacing.
 * @param pos line cursor, updated by the scan and selected output path
 */
static void microspace_word_processor(uint8_t* pos)
{
    bool is_tab = false;
    microspace_line_t line = collect_microspace_line(pos, &is_tab);
    if (can_microspace_line(&line))
        print_microspaced_line(pos, line.word_start, &is_tab);
    else
    {
        column_position = 0;
        uint8_t ch;
        do
        {
            (*pos)++;
            uint8_t char_width;
            ch = convert_char_for_printing(
                scratch_line_ptr[*pos], &char_width, &is_tab);
            print_char_x_times(ch, char_width);
        } while (ch != 0x0d);
        register_value_array['L' - 'A']++;
    }

    advance_print_line_spacing();
}

/**
 * Renders a new page including headers, margins and page number prompt.
 */
static void render_new_page(void)
{
    page_break_pending_flag = true;

    if (print_flags & 0x40)
    {
        stop_printing();
        cli_putstring("\nPage ");
        render_number_to_screen(register_value_array['P' - 'A']);
        cli_putstring("..");
        uint8_t response = screen_getchar() & 0xdf;
        if (response == 'Q')
        {
            stop_printing();
            cli_putchar('\n');
            return_to_cli_prompt();
            return;
        }
        if (response != 'M')
            start_printing();
    }

    if (page_break_flag)
    {
        print_vertical_space(top_margin);
        if (headers_enabled_flag)
            render_header_or_footer(header_text_maybe);
        print_newline();
        print_vertical_space(header_margin);
    }
    compute_lines_remaining_on_page();
}

/**
 * Prints a newline character.
 */
static void print_newline(void)
{ print_char(0x0d); }

/**
 * Prints vertical spacing as blank lines.
 *
 * @param idx number of blank lines to print
 */
static void print_vertical_space(uint8_t idx)
{
    print_char_x_times(0x0d, idx);
}

/**
 * Prints the left, centred and right header/footer sections, swapping the
 * outer sections according to page parity.
 *
 * @param text pointer to the header or footer text data
 */
static void render_header_or_footer(uint8_t* text)
{
    if (text[0] == 0)
        return;
    output_left_margin();
    column_position = 0;

    uint8_t* left_section = compute_header_left_section(text);
    if (!get_page_parity())
        left_section = compute_header_odd_page_section(text);
    copy_header_footer_text(left_section);
    print_output_buffer();

    uint8_t centre_width =
        copy_header_footer_text(compute_header_middle_section(text));
    if (centre_width != 0)
    {
        uint8_t half_width = (centre_width - 1) / 2;
        scratch_index = half_width;
        uint8_t right_margin = get_right_margin();
        uint8_t centre_column = right_margin / 2;
        if (right_margin != 0 && centre_column >= half_width + column_position)
            add_justification_spaces(
                centre_column - half_width - column_position);
    }
    print_output_buffer();

    uint8_t* right_section = compute_header_odd_page_section(text);
    if (!get_page_parity())
        right_section = compute_header_left_section(text);
    uint8_t right_width = copy_header_footer_text(right_section);
    uint8_t right_margin = get_right_margin();
    if (right_margin != 0)
    {
        scratch_index = right_width;
        if (right_margin >= right_width + column_position)
        {
            uint8_t padding = right_margin - right_width - column_position + 1;
            add_justification_spaces(padding);
        }
    }
    print_output_buffer();
}

/**
 * Prints temp_save bytes from the output buffer, converting document
 * characters and carrying tab state between them.
 */
static void print_output_buffer(void)
{
    uint8_t length = temp_save;
    bool is_tab = false;
    for (uint8_t pos = 0; pos < length; pos++)
    {
        uint8_t char_width = length - pos;
        uint8_t ch =
            convert_char_for_printing(output_buffer[pos], &char_width, &is_tab);
        print_char(ch);
    }
}

/**
 * Starts microspacing if the printer supports it.
 *
 * @param cur_ch current character context
 */
static void start_microspacing_if_active(uint8_t cur_ch)
{
    if ((print_flags & 0x80) == 0)
        return;

    if (microspacing_count == 0)
        return;
    print_alignment_spaces(cur_ch);
    print_last_microspacing = microspacing_count;
    printer_driver_ptr->printer_microspace();
}

/**
 * Emits microspacing adjustments for the current character.
 *
 * @param cur_ch character being printed
 * @param idx requested microspacing amount
 */
static void emit_microspacing_spaces(uint8_t cur_ch, uint8_t idx)
{
    if (idx == print_last_microspacing)
        return;
    print_alignment_spaces(cur_ch);
    print_last_microspacing = idx;
    printer_driver_ptr->printer_microspace();
}

/**
 * Computes the start of the left section of a header or footer string.
 *
 * @param insert_ptr pointer to the header/footer data
 * @return pointer to the left section
 */
static uint8_t* compute_header_left_section(uint8_t* insert_ptr)
{
    uint8_t* copy_ptr;

    get_line_width(insert_ptr);
    {
        copy_ptr = insert_ptr + 0;
    }
    return copy_ptr;
}

/**
 * Computes the start of the middle section of a header or footer string.
 *
 * @param insert_ptr pointer to the header/footer data
 * @return pointer to the middle section
 */
static uint8_t* compute_header_middle_section(uint8_t* insert_ptr)
{
    return insert_ptr + get_line_width(insert_ptr) + 1;
}

/**
 * Computes the start of the odd-page section of a header or footer string.
 *
 * @param insert_ptr pointer to the header/footer data
 * @return pointer to the odd-page section
 */
static uint8_t* compute_header_odd_page_section(uint8_t* insert_ptr)
{
    uint8_t* copy_ptr;
    uint8_t pos = scan_string_length(get_line_width(insert_ptr), insert_ptr);

    pos++;
    uint8_t cur_ch = pos;

    pos--;
    {
        copy_ptr = insert_ptr + cur_ch;
    }
    return copy_ptr;
}

/**
 * Gets the width of the first section of a header or footer line.
 *
 * @param insert_ptr pointer to the line data
 * @return width value
 */
static uint8_t get_line_width(uint8_t* insert_ptr)
{
    return scan_string_length(0xff, insert_ptr);
}

/**
 * Scans a header/footer string to find the length of a section.
 *
 * @param pos starting position
 * @param insert_ptr pointer to the string data
 * @return position of the terminator
 */
static uint8_t scan_string_length(uint8_t pos, uint8_t* insert_ptr)
{
    uint8_t cur_ch;

    do
    {
        pos++;
        cur_ch = insert_ptr[pos];
    } while ((int8_t)cur_ch >= 0);

    return pos;
}

/**
 * Returns the effective right margin for the current ruler.
 *
 * @return right margin value
 */
static uint8_t get_right_margin(void)
{
    if (ruler_right_stop != 0)
        return ruler_right_stop;
    return ruler_buffer_len - 1;
}

/**
 * Copies header or footer text to the output buffer, expanding registers.
 *
 * Stops at a high-bit section delimiter or the output limit. Saves the raw
 * output length in temp_save for printing. Register rendering retains the
 * current output position.
 *
 * @param text source text to copy
 * @return output length, excluding control codes in microspacing mode
 */
static uint8_t copy_header_footer_text(uint8_t* text)
{
    uint8_t output_len = 0;
    uint8_t source_pos = 0;
    int control_count = 0;

    for (;;)
    {
        uint8_t ch = text[source_pos];
        if (ch & 0x80)
            break;
        if (check_for_control_code(ch) != NO_CONTROL_CODE)
            control_count++;
        source_pos++;

        if (ch == '|')
        {
            uint8_t register_name = text[source_pos];
            if (register_name & 0x80)
                break;
            source_pos++;
            render_register(register_name, output_len);
        }
        else
        {
            output_buffer[output_len++] = ch;
            if (output_len >= MAX_LINE_LENGTH)
                break;
        }
    }

    temp_save = output_len;
    if (print_flags & 0x80)
        output_len -= control_count;
    return output_len;
}

/**
 * Outputs left margin spaces, adjusting for two-sided printing.
 */
static void output_left_margin(void)
{
    bool parity = get_page_parity();
    uint8_t cur_ch = left_margin;

    if (parity)
    {
        if (two_sided_flag)
            cur_ch += rhs_extra_margin;
    }
    print_char_x_times(0x20, cur_ch);
}

/**
 * Determines the page parity for two-sided printing.
 *
 * @return true for even page handling, false for odd
 */
static bool get_page_parity(void)
{
    if (!two_sided_flag)
        return true;
    return (register_value_array['P' - 'A'] & 1) != 0;
}

/**
 * Adds justification spaces for centring or alignment.
 *
 * @param idx number of spaces to add
 * @return the space character used
 */
static uint8_t add_justification_spaces(uint8_t idx)
{
    uint8_t cur_ch = idx;

    cur_ch += column_position;
    column_position = cur_ch;
    uint8_t next_ch = 0x20;

    print_char_x_times(next_ch, idx);

    return next_ch;
}

/**
 * Prints a character repeatedly.
 *
 * @param cur_ch character to print
 * @param idx number of times to print
 */
static void print_char_x_times(uint8_t cur_ch, uint8_t idx)
{
    if (idx != 0)
    {
        do
        {
            print_char(cur_ch);
            idx--;
        } while (idx != 0);
    }
}

/**
 * Converts a character for printing and advances the column by its width.
 * High-bit characters do not advance the column in microspacing mode.
 *
 * @param cur_ch character to convert
 * @param width output for the converted width
 * @param is_tab tab state carried between characters
 * @return converted character
 */
static uint8_t convert_char_for_printing(
    uint8_t cur_ch, uint8_t* width, bool* is_tab)
{
    uint8_t converted = process_document_character(cur_ch, width, is_tab);
    if ((print_flags & 0x80) == 0 || (converted & 0x80) == 0)
        column_position += *width;
    return converted;
}

/**
 * Resets formatting registers and default print settings.
 */
static void reset_print_registers(void)
{
    memset(register_value_array, 0, sizeof(register_value_array));
    register_value_array['P' - 'A'] = 1;
    register_value_array['L' - 'A'] = 1;

    header_text_maybe[0] = 0;
    footer_text_maybe[0] = 0;
    headers_enabled_flag = true;
    footers_enabled_flag = true;
    two_sided_flag = false;
    macro_executing_flag = false;
    highlight_code[0] = 0x80;
    highlight_code[1] = 0x81;

    left_margin = 0;
    line_spacing = 0;
    rhs_extra_margin = 0;
    page_length = 66;
    page_break_flag = true;
    top_margin = 4;
    bottom_margin = 4;
    header_margin = 4;
    footer_margin = 4;
    compute_lines_remaining_on_page();
}

/**
 * Computes the page length minus vertical margins when page breaks are enabled.
 * Uses one line if the margins exceed the page length; an exact fit gives zero.
 */
static void compute_lines_remaining_on_page(void)
{
    page_lines_remaining = page_length;
    if (page_break_flag)
    {
        int total_margins =
            top_margin + header_margin + bottom_margin + footer_margin;
        page_lines_remaining =
            total_margins > page_length ? 1 : page_length - total_margins;
    }
}

/**
 * Writes a carriage return to the memory buffer.
 *
 * @param cursor pointer to the write cursor
 */
void write_cr_to_memory(uint8_t** cursor)
{
    write_byte_to_memory(cursor, 0x0d);
}

/**
 * Writes a byte to the memory buffer and advances the cursor.
 *
 * @param cursor pointer to the write cursor, updated after the write
 * @param cur_ch byte to write
 */
void write_byte_to_memory(uint8_t** cursor, uint8_t cur_ch)
{
    **cursor = cur_ch;
    (*cursor)++;

    if (cur_ch != 0x0d)
        return;
    screen_column = 0;
}

/**
 * Stops active printing and clears printer state.
 */
void stop_printing(void)
{
    if (((int8_t)print_flags < 0))
    {
        print_flags &= 0x7f;
        printer_driver_ptr->printer_off();
    }
}

/**
 * Sets up the printer driver pointer to the default driver.
 */
void prepare_printer_driver(void)
{
    microspacing_count = 0;
    printer_driver_ptr = &default_printer_driver;
}

/**
 * Default printer routine to output a character.
 *
 * @param cur_ch character to output
 */
static void default_print_char(uint8_t cur_ch)
{
    if (cur_ch >= 0x80)
        return;
    cli_putchar(cur_ch);
}

/**
 * Default printer activation routine.
 */
static void default_printer_on(void)
{
    default_printer_off();
}

/**
 * Default printer deactivation routine.
 */
static void default_printer_off(void) {}

/**
 * Default microspacing routine (no operation).
 */
static void default_printer_microspace(void) {}

/**
 * Default routine to retrieve printer flags.
 *
 * @param idx output for flag index
 * @param pos output for flag position
 */
static void default_printer_getflags(uint8_t* idx, uint8_t* pos)
{
    *idx = 0;
    *pos = 0;
}
