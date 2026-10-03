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
static uint8_t footers_enabled_flag;
static uint8_t formatted_line_written_flag;
static uint8_t header_margin;
static uint8_t headers_enabled_flag;
static uint8_t left_margin;
static uint8_t line_spacing;
static uint8_t page_break_flag;
static uint8_t page_break_pending_flag;
static uint8_t page_length;
static uint8_t page_lines_remaining;
static uint8_t print_extra_space_accum;
static uint8_t print_last_microspacing;
static uint8_t print_running_total_accum;
static uint8_t rhs_extra_margin;
static uint8_t rw_file_handle;
static uint8_t two_sided_flag;

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

static const uint8_t commands_table[] =
    "CERJDFDHDMEMSRPETMBMPLTSFOHEHTHMFMLMLSOPEPLJPB";

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
static bool execute_formatting_command(formatting_command_t idx);
static void lj_fmt_cmd(void);
static void ce_fmt_cmd(void);
static void rj_fmt_cmd(void);
static void write_output_buffer_to_format_line(uint8_t pad_len);
static uint8_t expand_line(void);
static void df_fmt_cmd(void);
static void dh_fmt_cmd(void);
static uint8_t process_header_footer_line(uint8_t* copy_ptr);
static void store_to_output_buffer(uint8_t cur_ch, uint8_t* copy_ptr);
static void em_fmt_cmd(void);
static void pl_fmt_cmd(void);
static void ts_fmt_cmd(void);
static void tm_fmt_cmd(void);
static void bm_fmt_cmd(void);
static void hm_fmt_cmd(void);
static void fm_fmt_cmd(void);
static void lm_fmt_cmd(void);
static void ls_fmt_cmd(void);
static void pe_fmt_cmd(void);
static void op_fmt_cmd(void);
static void ep_fmt_cmd(void);
static void eject_two_pages(void);
static void page_eject_fmt(void);
static void fo_fmt_cmd(void);
static void he_fmt_cmd(void);
static void pb_fmt_cmd(void);
static void ht_fmt_cmd(void);
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
 * Main entry point for printing the current document.
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
    uint8_t cur_ch = 0;

    page_break_pending_flag = cur_ch;
    print_xpos = cur_ch;
    printing_from_file_flag = cur_ch;
    current_ruler_ptr = current_ruler_buffer;
    find_margins_of_current_ruler_buffer();

    if (!(!scan_input_buffer(input_buffer, scan)))
    {
        printing_from_file_flag++;
        print_source_ptr = ram;
        print_loop(print_doc_ptr);

        goto c8f0d;
    }
c8f0d:
    if (parse_optional_filename_from_command(scan))
    {
        set_rw_file_handle(0x0d);
        print_loop(print_doc_ptr);

        goto c8f0d;
    }
    if ((int8_t)page_break_pending_flag >= 0)
        return;
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
 * Core print loop that processes lines and formatting commands.
 *
 * @param print_doc_ptr pointer to the read limit for the document data
 */
static void print_loop(uint8_t* print_doc_ptr)
{
    uint8_t idx;
    formatting_command_t fmt_cmd_index;
    uint8_t* macro_cursor_ptr = NULL;
    bool is_tab = false;

c8f30:
    while (1)
    {
        uint8_t tmp_ch10;
        uint8_t next_ch;
        uint8_t cur_ch = page_break_pending_flag;

        if (cur_ch != 0)
        {
            cur_ch = page_lines_remaining;

            if (cur_ch == 0)
                process_page_footer();
        }
        uint8_t* cursor = prepare_output_line(print_doc_ptr, &macro_cursor_ptr);

        if (cursor == NULL)
            return;
        start_microspacing_if_active(cur_ch);
        uint8_t pos = 0;

        scratch_offset = pos;
        command_prefix_t cp = deref_and_check_for_command_prefix(pos, cursor);

        if (cp != NO_COMMAND_PREFIX)
        {
            scratch_offset = 3;

            if (*cursor != RULER_PREFIX)
                goto c8f6e_l;
            uint8_t pos3 = 3;

            idx = 0;

            do
            {
                next_ch = cursor[pos3];
                current_ruler_buffer[idx] = next_ch;
                pos3++;
                idx++;
            } while (next_ch != 0x0d);
            find_margins_of_current_ruler_buffer();

        c8f6b_l:
            goto c8f30;

        c8f6e_l:
            fmt_cmd_index = lookup_formatting_command();

            if (fmt_cmd_index == NO_FORMATTING_COMMAND)
                goto c8f7a_l;

            if (execute_formatting_command(fmt_cmd_index))
                goto c8f6b_l;
        }
        goto c8fce_l;

    c8f7a_l:
        if (macro_try_invoke(&macro_cursor_ptr))
        {
            if (macro_executing_flag != 0)
                continue;
        }
        else
        {
            goto c8f6b_l;
        }
    c8fce_l:
        if (page_break_pending_flag == 0)
            render_new_page();
        output_left_margin();
        column_position = 0;
        uint8_t pos5 = scratch_offset;

        if (((int8_t)print_flags < 0))
        {
            if (microspacing_flag != 0)
            {
                microspace_word_processor(&pos5);
                continue;
            }
        }
        do
        {
            uint8_t tmp_ch9 = cursor[pos5];

            pos5++;
            tmp_ch10 = convert_char_for_printing(tmp_ch9, &idx, &is_tab);
            print_char_x_times(tmp_ch10, idx);
        } while (tmp_ch10 != 0x0d);
        register_value_array['L' - 'A']++;
        idx = line_spacing;
        uint8_t tmp_ch11 = page_lines_remaining - line_spacing - 1;

        if (page_lines_remaining <= line_spacing)
        {
            tmp_ch11 = 0;
            idx = page_lines_remaining;
            idx--;
        }
        page_lines_remaining = tmp_ch11;
        print_vertical_space(idx);
    }
}

/**
 * Looks up the two-letter formatting command at the current line.
 *
 * @return the matching command index or NO_FORMATTING_COMMAND if not found
 */
formatting_command_t lookup_formatting_command(void)
{
    uint8_t tmp_ch4;
    uint8_t pos = 2;
    uint8_t cur_ch = heap_format_line_ptr->command[1];
    uint8_t next_ch = heap_format_line_ptr->command[0];
    pos -= 2;
    int index = 0;
    do
    {
        if (next_ch == commands_table[pos])
        {
            if (cur_ch == commands_table[pos + 1])
                return index;
        }
        index++;
        pos++;
        pos++;
        tmp_ch4 = commands_table[pos];
    } while (tmp_ch4 != 0);

    return NO_FORMATTING_COMMAND;
}

/**
 * Executes a formatting command by index.
 *
 * @param idx command index as returned by lookup_formatting_command
 * @return true if no formatted line was emitted, false otherwise
 */
static bool execute_formatting_command(formatting_command_t idx)
{
    formatted_line_written_flag = 0;

    switch (idx)
    {
        case FORMATTING_COMMAND_CE:
            ce_fmt_cmd();
            break;

        case FORMATTING_COMMAND_RJ:
            rj_fmt_cmd();
            break;

        case FORMATTING_COMMAND_DF:
            df_fmt_cmd();
            break;

        case FORMATTING_COMMAND_DH:
            dh_fmt_cmd();
            break;

        case FORMATTING_COMMAND_DM:
            dm_fmt_cmd();
            break;

        case FORMATTING_COMMAND_EM:
            break;

        case FORMATTING_COMMAND_SR:
            em_fmt_cmd();
            break;

        case FORMATTING_COMMAND_PE:
            pe_fmt_cmd();
            break;

        case FORMATTING_COMMAND_TM:
            tm_fmt_cmd();
            break;

        case FORMATTING_COMMAND_BM:
            bm_fmt_cmd();
            break;

        case FORMATTING_COMMAND_PL:
            pl_fmt_cmd();
            break;

        case FORMATTING_COMMAND_TS:
            ts_fmt_cmd();
            break;

        case FORMATTING_COMMAND_FO:
            fo_fmt_cmd();
            break;

        case FORMATTING_COMMAND_HE:
            he_fmt_cmd();
            break;

        case FORMATTING_COMMAND_HT:
            ht_fmt_cmd();
            break;

        case FORMATTING_COMMAND_HM:
            hm_fmt_cmd();
            break;

        case FORMATTING_COMMAND_FM:
            fm_fmt_cmd();
            break;

        case FORMATTING_COMMAND_LM:
            lm_fmt_cmd();
            break;

        case FORMATTING_COMMAND_LS:
            ls_fmt_cmd();
            break;

        case FORMATTING_COMMAND_OP:
            op_fmt_cmd();
            break;

        case FORMATTING_COMMAND_EP:
            ep_fmt_cmd();
            break;

        case FORMATTING_COMMAND_LJ:
            lj_fmt_cmd();
            break;

        case FORMATTING_COMMAND_PB:
            pb_fmt_cmd();
            break;

        case NO_FORMATTING_COMMAND:
    }
    return formatted_line_written_flag == 0;
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
    expanded_len >>= 1;
    uint8_t line_len = ruler_right_stop;

    if (line_len == 0)
    {
        write_output_buffer_to_format_line(line_len);
        return;
    }
    line_len -= ruler_left_stop;
    line_len >>= 1;
    line_len += ruler_left_stop + 1;

    if (line_len >= expanded_len)
    {
        write_output_buffer_to_format_line(line_len - expanded_len);
        return;
    }
    write_output_buffer_to_format_line(0);
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
    uint8_t idx = pad_len;

    if (idx != 0)
    {
        uint8_t space_char = 0x20;

        do
        {
            heap_format_line_ptr->text[pos] = space_char;
            pos++;
            idx--;
        } while (idx != 0);
    }
    uint8_t cur_ch;
    do
    {
        cur_ch = output_buffer[idx];
        heap_format_line_ptr->text[pos] = cur_ch;
        pos++;
        idx++;
    } while (cur_ch != 0x0d);
    formatted_line_written_flag++;
}

/**
 * Expands the current format line into the output buffer.
 *
 * Copies characters starting at offset 3, expanding register references
 * introduced by '|' and counting control codes. Enforces the maximum line
 * length.
 *
 * @return number of characters written to the output buffer, 0 if empty
 */
static uint8_t expand_line(void)
{
    uint8_t next_ch;
    uint8_t idx = 0;

    screen_column = idx;
    uint8_t pos = 0;

    if (get_current_fmt_cmd_byte(&pos) == 0)
        return idx;

c9537:
    for (;;)
    {
        next_ch = heap_format_line_ptr->text[pos];
        pos++;

        if (next_ch == 0x7c)
            goto c955e;

    c953e:
        for (;;)
        {
            output_buffer[idx] = next_ch;
            control_code_t cc = check_for_control_code(next_ch);

            if (cc != NO_CONTROL_CODE)
                screen_column++;
            idx++;

            if (next_ch == 0x0d)
                goto c9555;

            if (idx < MAX_LINE_LENGTH - 1)
                break;
            next_ch = 0x0d;
        }
    }
c9555:
    if ((print_flags & 0x80) == 0)
        return idx;
    idx -= screen_column;

    return idx;

c955e:
    next_ch = heap_format_line_ptr->text[pos];

    if (next_ch == 0x0d)
        goto c953e;
    pos++;
    render_register(next_ch, idx);

    if (screen_row > idx)
        idx = screen_row;

    goto c9537;
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
 * Processes the current format line as a header or footer definition.
 *
 * @param copy_ptr destination buffer for the processed text
 * @return last character processed
 */
static uint8_t process_header_footer_line(uint8_t* copy_ptr)
{
    uint8_t next_ch;

    scratch_index = 0;
    search_target_len = 0;
    uint8_t pos = 0;

    int cur_ch = heap_format_line_ptr->text[pos];
    uint8_t idx = 0x3f;

    do
    {
        pos++;
        // int x = pos;
        next_ch = heap_format_line_ptr->text[pos];

        if (next_ch == 0x0d)
        {
            next_ch |= 0x80;
        }
        else
        {
            if (next_ch < 0x1b)
                next_ch = 0x20;

            if (next_ch == cur_ch)
                next_ch |= 0x80;
        }
        store_to_output_buffer(next_ch, copy_ptr);

        if (next_ch == 0x8d)
            goto c95aa;
        // pos = x;
        idx--;
    } while (idx != 0);

c95aa:
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
 * Handles the SR/EM formatting command to evaluate an expression and store
 * the result in a register.
 */
static void em_fmt_cmd(void)
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
    two_sided_flag = flag_value;
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

    if (page_break_pending_flag != 0)
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
    if (page_break_pending_flag == 0)
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
    footers_enabled_flag = flag_value;
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
    headers_enabled_flag = flag_value;
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
    page_break_flag = flag_value;
}

/**
 * Handles the HT formatting command to set highlight codes.
 */
static void ht_fmt_cmd(void)
{
    uint8_t pos = 0;
    uint8_t cur_ch = get_current_fmt_cmd_byte(&pos);

    if (cur_ch == 0)
        return;
    uint8_t idx = cur_ch;

    if (idx != 0x2d)
    {
        if (idx != 0x2a)
            goto c9719;
    }
    pos++;

    goto c9725;

c9719:
    int value;
    evaluate_expression_from_fmt_cmd(&value, &pos, idx);
    cur_ch = value;

    if (cur_ch == 0)
        return;
    cur_ch -= 1;

    if (cur_ch >= 2)
        return;

c9725:
    int highlight_value;
    evaluate_expression_from_fmt_cmd(&highlight_value, &pos, idx);
    highlight_code[cur_ch] = highlight_value;
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
 * Parses a word-based flag such as ON/OFF from the format line.
 *
 * @param target_ptr line buffer to parse from
 * @param pos cursor into the buffer, advanced past the token
 * @param value output for the parsed byte
 * @return true on parse error, false on success
 */
static bool parse_word_flag(uint8_t* target_ptr, uint8_t* pos, uint8_t* value)
{
    uint8_t temp_save;
    uint8_t tmp_ch2;
    uint8_t cur_ch = target_ptr[*pos];
    uint8_t idx = cur_ch;

    if (idx != 0x31)
    {
        cur_ch = 0;

        if (idx != 0x30)
            goto c9783;
    }
    *value = cur_ch;
    (*pos)++;

    if (*pos != 0)
        return false;

c9783:
    (*pos)--;
    temp_save = *pos;
    uint8_t idx2 = 0xff;

c9788:
    do
    {
        (*pos)++;
        tmp_ch2 = toupper(target_ptr[*pos]);
        idx2++;
    } while (tmp_ch2 == on_off_table[idx2]);
    uint8_t tmp_ch3 = on_off_table[idx2];

    if ((int8_t)tmp_ch3 >= 0)
    {
        if (tmp_ch3 < 0x20)
        {
            *value = tmp_ch3;

            return false;
        }
        for (;;)
        {
            idx2++;
            tmp_ch3 = on_off_table[idx2];

            if ((int8_t)tmp_ch3 < 0)
                goto c97ae;

            if (tmp_ch3 >= 0x20)
                continue;
            *pos = temp_save;
            tmp_ch3 = on_off_table[idx2 + 1];

            if ((int8_t)tmp_ch3 >= 0)
                goto c9788;
            break;
        }
    }
c97ae:
    *value = tmp_ch3;

    return true;
}

/**
 * Evaluates an arithmetic expression from the format command line.
 *
 * Supports addition, subtraction and register references.
 *
 * @param result output for the evaluated value
 * @param pos cursor into the format line, advanced as the expression is
 * consumed
 * @param idx output position used when expanding register references
 * @return true if a term was parsed, false if the argument was empty
 */
static bool evaluate_expression_from_fmt_cmd(
    int* result, uint8_t* pos, uint8_t idx)
{
    int insert_ptr = 0;
    int scan_ptr = 0;
    int scratch_offset = 0;
    for (;;)
    {
        uint8_t next_ch = get_current_fmt_cmd_byte(pos);

        if (next_ch == 0)
        {
            *result = scan_ptr;

            return false;
        }
        if (next_ch == 0x7c)
        {
            next_ch = get_next_fmt_cmd_byte(pos);

            if (next_ch == 0)
                goto c9821;
            (*pos)++;
            render_register(next_ch, idx);
        }
        else
        {
            parse_decimal_number(&scan_ptr, pos);
        }
        uint8_t count = scratch_offset;

        if (count != 0)
        {
            scratch_offset = 0;
            count--;

            if (count != 0)
            {
                scan_ptr = insert_ptr - scan_ptr;

                goto c9804;
            }
            scan_ptr += insert_ptr;
        }
    c9804:
        insert_ptr = scan_ptr;
        uint8_t tmp_ch4 = get_current_fmt_cmd_byte(pos);

        if (tmp_ch4 == 0)
            goto c9821;
        uint8_t count_1 = 1;

        if (tmp_ch4 != '+')
        {
            count_1++;

            if (tmp_ch4 != '-')
                goto c9821;
        }
        scratch_offset = count_1;
        (*pos)++;
    }
c9821:
    *result = scan_ptr;

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
    if (page_break_flag != 0)
    {
        print_vertical_space(page_lines_remaining);
        print_vertical_space(footer_margin);

        if (footers_enabled_flag != 0)
            render_header_or_footer(footer_text_maybe);
        print_newline();
        print_vertical_space(bottom_margin);
    }
    register_value_array['P' - 'A']++;
    register_value_array['L' - 'A'] = 1;
    page_break_pending_flag = 0;
}

/**
 * Processes words for microspaced justification during printing.
 *
 * @param pos cursor into the line buffer, updated as words are processed
 */
static void microspace_word_processor(uint8_t* pos)
{
    uint8_t current_byte;
    uint8_t next_byte;
    uint8_t idx = 0;

    uint32_t accum_1 = 0;
    uint8_t micro_word_start = 0;

    column_position = 0;
    uint8_t micro_space_count = 0;
    uint8_t micro_overflow = 0;
    uint8_t micro_word_counter = 0;

    screen_column = 0;
    bool is_tab = false;

c9048:
    do
    {
        {
            uint8_t cur_ch = idx;
            convert_char_for_printing(scratch_line_ptr[*pos], &idx, &is_tab);
            idx = cur_ch;
        }
        current_byte = scratch_line_ptr[*pos];
        (*pos)++;

        if (current_byte != 0x1a)
            goto c906f;

        if ((screen_column & 0x80) == 0)
            break;

        if (micro_space_count == 0)
            goto c906b;
        micro_word_counter++;
    } while (micro_word_counter != 0);

    do
    {
        micro_word_start = column_position;

        goto c908c;

    c906b:
        current_byte = 0x20;
        micro_overflow--;

    c906f:
        if (current_byte < 0x20)
            goto c9092;

        if (current_byte != 0x20)
            goto c9090;
    } while ((screen_column & 0x80) == 0);
    uint8_t overflow_copy = micro_overflow;

    if (overflow_copy == 0)
        goto c908a;

    if ((overflow_copy & 0x80) == 0)
    {
        micro_word_counter++;
        micro_overflow = 0;

        goto c9048;
    }
c908a:
    micro_space_count++;

c908c:
    current_byte = 0x20;

    goto c90b6;

c9090:
    accum_1 += 0x100;

c9092:
    if (current_byte != 9 && current_byte != 0x0b) {}
    else
    {
        {
            micro_word_start = column_position;
            screen_column = 0;
            accum_1 = 0;
            micro_space_count = 0;
            micro_overflow = 0;
            micro_word_counter = 0;
            current_byte = current_byte;
        }
    }
c90b6:
    output_buffer[idx] = current_byte;
    idx++;

    if (current_byte != 0x0d)
    {
        if (current_byte == 0x20)
            goto c9048;

        if (micro_space_count == 0)
            goto c9048;
        accum_1 += ((uint32_t)micro_word_counter << 16) |
                   ((uint32_t)micro_space_count << 8) | micro_space_count;
        uint8_t zero_val = 0;

        micro_space_count = zero_val;
        micro_overflow = zero_val;
        micro_word_counter = zero_val;

        goto c9048;
    }
    if (accum_1 >> 16 != 0)
    {
        if (ruler_right_stop == 0)
            goto c90f8;
        int d = ruler_right_stop - micro_word_start;

        if (d < 0)
            goto c90f8;

        if ((uint8_t)(d - (accum_1 >> 16) + (d >= (int)(accum_1 >> 16))) ==
            (uint8_t)((accum_1 >> 8) & 0xFF))
        {
            goto c9101;
        }
    }
c90f8:
    column_position = 0;

    goto c8fe6_inline;

c9101:
    uint16_t scan_ptr = (uint16_t)print_running_total_accum * microspacing_flag;
    justify_gap_count = print_extra_space_accum;
    print_running_total_accum = scan_ptr % justify_gap_count;
    print_extra_space_accum = (uint8_t)(scan_ptr / justify_gap_count);
    (*pos) = 0;
    column_position = (*pos);

c912b:
    (*pos)++;
    uint8_t converted_byte =
        convert_char_for_printing(output_buffer[(*pos)], &idx, &is_tab);
    {
        uint8_t column_copy = column_position;

        if (column_copy != micro_word_start)
        {
            if (column_copy >= micro_word_start)
                goto c9142;
        }
        uint8_t emit_byte = converted_byte;

        print_char_x_times(emit_byte, idx);

        goto c9163;

    c9142:
        emit_byte = converted_byte;

        if (emit_byte == 0x20)
        {
            uint8_t microspace_val = microspacing_flag;

            microspace_val += print_extra_space_accum;
            idx = microspace_val;
            uint8_t running_total_copy = print_running_total_accum;

            if (running_total_copy != 0)
            {
                idx++;
                print_running_total_accum--;
            }
            emit_microspacing_spaces(running_total_copy, idx);
            emit_byte = 0x20;
        }
        else
        {
            idx = microspacing_flag;
            emit_microspacing_spaces(emit_byte, idx);
        }
        print_char(emit_byte);

    c9163:
        if (emit_byte != 0x0d)
            goto c912b;

        goto c8ffb_inline;
    }
c8fe6_inline:
    do
    {
        (*pos)++;
        next_byte =
            convert_char_for_printing(scratch_line_ptr[*pos], &idx, &is_tab);
        print_char_x_times(next_byte, idx);
    } while (next_byte != 0x0d);
    register_value_array['L' - 'A']++;

c8ffb_inline:
    uint8_t idx2 = line_spacing;
    uint8_t remaining_lines = page_lines_remaining - line_spacing - 1;

    if (page_lines_remaining <= line_spacing)
    {
        remaining_lines = 0;
        idx2 = page_lines_remaining;
        idx2--;
    }
    page_lines_remaining = remaining_lines;
    print_vertical_space(idx2);
}

/**
 * Renders a new page including headers, margins and page number prompt.
 */
static void render_new_page(void)
{
    page_break_pending_flag = 0x81;

    if (print_flags & 0x40)
    {
        stop_printing();
        cli_putstring("\nPage ");
        render_number_to_screen(register_value_array['P' - 'A']);
        cli_putstring("..");
        uint8_t next_ch = screen_getchar();

        next_ch &= 0xdf;

        if (next_ch == 0x4d)
            goto c92d4;

        if (next_ch == 0x51)
        {
            stop_printing();
            cli_putchar('\n');

            return_to_cli_prompt();
            return;
        }
        start_printing();
    }
c92d4:
    if (page_break_flag == 0)
    {
        compute_lines_remaining_on_page();

        return;
    }
    print_vertical_space(top_margin);

    if (headers_enabled_flag != 0)
        render_header_or_footer(header_text_maybe);
    print_newline();
    print_vertical_space(header_margin);
    compute_lines_remaining_on_page();
}

/**
 * Prints a newline character.
 */
static void print_newline(void)
{
    print_char(0x0d);
}

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
 * Renders header or footer text with justification and spacing.
 *
 * @param text pointer to the header or footer text data
 */
static void render_header_or_footer(uint8_t* text)
{
    if (text[0] == 0)
        return;
    output_left_margin();
    column_position = 0;
    uint8_t* section_start = compute_header_left_section(text);

    if (!get_page_parity())
        section_start = compute_header_odd_page_section(text);
    copy_header_footer_text(section_start);
    print_output_buffer();
    uint8_t idx = copy_header_footer_text(compute_header_middle_section(text));

    if (idx != 0)
    {
        idx--;
        uint8_t tmp_ch3 = idx;

        tmp_ch3 >>= 1;
        scratch_index = tmp_ch3;
        uint8_t tmp_ch4 = get_right_margin();

        if (tmp_ch4 == 0)
            goto c9355;
        tmp_ch4 >>= 1;

        if (tmp_ch4 >= scratch_index + column_position)
            add_justification_spaces(tmp_ch4 - scratch_index - column_position);
    }
c9355:
    print_output_buffer();
    uint8_t* section_start_2 = compute_header_odd_page_section(text);

    if (!get_page_parity())
        section_start_2 = compute_header_left_section(text);
    uint8_t idx3 = copy_header_footer_text(section_start_2);
    uint8_t tmp_ch6 = get_right_margin();

    if (tmp_ch6 != 0)
    {
        scratch_index = idx3;

        if (tmp_ch6 >= scratch_index + column_position)
        {
            uint8_t idx4 = tmp_ch6 - scratch_index - column_position;

            idx4++;
            add_justification_spaces(idx4);
        }
    }
    print_output_buffer();
}

/**
 * Prints the contents of the output buffer to the printer.
 */
static void print_output_buffer(void)
{
    uint8_t pos = 0;
    uint8_t idx = temp_save;

    if (idx == 0)
        return;
    bool is_tab = false;

    do
    {
        uint8_t idx3 = idx;

        print_char(
            convert_char_for_printing(output_buffer[pos], &idx3, &is_tab));
        pos++;
        idx--;
    } while (idx != 0);
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

    if (microspacing_flag == 0)
        return;
    print_alignment_spaces(cur_ch);
    print_last_microspacing = microspacing_flag;
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
 * @param text source text to copy
 * @return number of characters written
 */
static uint8_t copy_header_footer_text(uint8_t* text)
{
    uint8_t tmp_ch3;
    uint8_t idx = 0;
    uint8_t pos2 = 0;
    int section_len = pos2;

    for (;;)
    {
        uint8_t cur_ch = text[pos2];

        if ((int8_t)cur_ch < 0)
            goto c93e6;
        control_code_t cc = check_for_control_code(cur_ch);

        if (cc != NO_CONTROL_CODE)
            section_len++;
        pos2++;

        if (cur_ch == 0x7c)
            goto c93f2;
        output_buffer[idx] = cur_ch;
        idx++;

        if (idx < MAX_LINE_LENGTH)
            continue;

    c93e6:
        do
        {
            temp_save = idx;

            if (((int8_t)print_flags < 0))
            {
                uint8_t tmp_ch2 = idx;

                tmp_ch2 -= section_len;
                idx = tmp_ch2;
            }
            return idx;

        c93f2:
            tmp_ch3 = text[pos2];
        } while ((int8_t)tmp_ch3 < 0);
        pos2++;
        render_register(tmp_ch3, idx);
    }
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
        if (two_sided_flag != 0)
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
    if (two_sided_flag == 0)
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
 * Converts a character for printing and updates display width.
 *
 * @param cur_ch character to convert
 * @param idx output for the converted width
 * @param is_tab tab state carried between characters
 * @return converted character
 */
static uint8_t convert_char_for_printing(
    uint8_t cur_ch, uint8_t* idx, bool* is_tab)
{
    cur_ch = process_document_character(cur_ch, idx, is_tab);
    if (print_flags & 0x80)
    {
        cur_ch |= 0;

        if (cur_ch & 0x80)
            goto return_33;
    }
    {
        uint8_t saved_a = cur_ch;

        cur_ch = *idx;
        cur_ch += column_position;
        column_position = cur_ch;
        cur_ch = saved_a;
    }
return_33:
    return cur_ch;
}

/**
 * Resets formatting registers and default print settings.
 */
static void reset_print_registers(void)
{
    uint8_t cur_ch = 0;

    memset(register_value_array, 0, sizeof(register_value_array));
    header_text_maybe[0] = cur_ch;
    footer_text_maybe[0] = cur_ch;
    two_sided_flag = cur_ch;
    left_margin = cur_ch;
    line_spacing = cur_ch;
    rhs_extra_margin = cur_ch;
    macro_executing_flag = cur_ch;
    page_length = 0x42;
    uint8_t tmp_ch2 = 1;

    footers_enabled_flag = tmp_ch2;
    headers_enabled_flag = tmp_ch2;
    page_break_flag = tmp_ch2;
    register_value_array['P' - 'A'] = tmp_ch2;
    register_value_array['L' - 'A'] = tmp_ch2;
    uint8_t pos = 0x80;

    highlight_code[0] = pos;
    pos++;
    highlight_code[1] = pos;
    uint8_t tmp_ch3 = 4;

    top_margin = tmp_ch3;
    bottom_margin = tmp_ch3;
    header_margin = tmp_ch3;
    footer_margin = tmp_ch3;
    compute_lines_remaining_on_page();
}

/**
 * Computes the number of lines remaining on the current page.
 */
static void compute_lines_remaining_on_page(void)
{
    uint8_t idx = page_length;

    if (page_break_flag != 0)
    {
        idx = 1;
        uint8_t next_ch = page_length;

        if (next_ch < top_margin)
            goto c930d;
        next_ch -= top_margin;

        if (next_ch < header_margin)
            goto c930d;
        next_ch -= header_margin;

        if (next_ch < bottom_margin)
            goto c930d;
        next_ch -= bottom_margin;

        if (next_ch < footer_margin)
            goto c930d;
        next_ch -= footer_margin;
        idx = next_ch;
    }
c930d:
    page_lines_remaining = idx;
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
    microspacing_flag = 0;
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
