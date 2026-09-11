#include "cli.h"
#include "document.h"
#include "printing.h"
#include "io.h"
#include <stdlib.h>
#include <string.h>
uint8_t* parse_mark_from_command(struct scan_state* scan);

void file_error(void);
void file_not_found_error(void);
bool parse_integer_from_command(struct scan_state* scan, int* out);
void parse_marks_from_command(struct scan_state* scan);
void reset_document_name_after_load(void);
void set_document_name_to_filename_buffer(void);
void zero_terminate_filename_buffer(void);

static void bye_cmd(void);
static void cmd_err_no_target(void);
static void cmd_err_no_string(void);
static void search_cmd(struct scan_state* scan);
static void change_cmd(struct scan_state* scan);
static void replace_cmd(struct scan_state* scan);
static void screen_cmd(struct scan_state* scan);
static void sheets_cmd(struct scan_state* scan);
static void print_cmd(struct scan_state* scan);
static void print_to_screen(struct scan_state* scan);
static void edit_cmd(struct scan_state* scan);
static void more_cmd(struct scan_state* scan);
static void finish_cmd(void);
static void quit_cmd(void);
static void close_input_output_files(void);
static void save_cmd_write_cmd(struct scan_state* scan);
static void load_cmd(struct scan_state* scan);
static void read_cmd(struct scan_state* scan);
static void mode_cmd(void);
static void microspace_cmd(struct scan_state* scan);
static void setup_cmd(struct scan_state* scan);
static void field_cmd(struct scan_state* scan);
static void count_cmd(struct scan_state* scan);
static void format_cmd(struct scan_state* scan);
static void new_cmd(void);
static void fold_cmd(struct scan_state* scan);
static void printer_cmd(struct scan_state* scan);
static void name_cmd(struct scan_state* scan);

/**
 * Exits the program.
 */
static void bye_cmd(void)
{
    exit(0);
}

/**
 * Dispatches a CLI command by index to its handler.
 *
 * @param cur_ch command index into the CLI jump table
 * @param scan scan state for argument parsing
 */
void execute_cli_command(uint8_t cur_ch, struct scan_state* scan)
{
    switch (cur_ch)
    {
        case 0:
            quit_cmd();
            break;
        case 1:
            new_cmd();
            break;
        case 2:
            format_cmd(scan);
            break;
        case 3:
            setup_cmd(scan);
            break;
        case 4:
            read_cmd(scan);
            break;
        case 5:
            more_cmd(scan);
            break;
        case 6:
            screen_cmd(scan);
            break;
        case 7:
            sheets_cmd(scan);
            break;
        case 8:
            save_cmd_write_cmd(scan);
            break;
        case 9:
            count_cmd(scan);
            break;
        case 10:
            field_cmd(scan);
            break;
        case 11:
            printer_cmd(scan);
            break;
        case 12:
            search_cmd(scan);
            break;
        case 13:
            clear_cmd();
            break;
        case 14:
            microspace_cmd(scan);
            break;
        case 15:
            fold_cmd(scan);
            break;
        case 16:
            name_cmd(scan);
            break;
        case 17:
            mode_cmd();
            break;
        case 18:
            finish_cmd();
            break;
        case 19:
            print_cmd(scan);
            break;
        case 20:
            change_cmd(scan);
            break;
        case 21:
            save_cmd_write_cmd(scan);
            break;
        case 22:
            edit_cmd(scan);
            break;
        case 23:
            replace_cmd(scan);
            break;
        case 24:
            load_cmd(scan);
            break;
        case 25:
            bye_cmd();
            break;
    }
}

/**
 * Replaces all occurrences of the search string in the document area.
 *
 * @param scan scan state containing search and replace arguments
 */
static void change_cmd(struct scan_state* scan)
{
    cli_cmd_status_t st = process_cli_command(scan);
    if (st == CLI_CMD_NO_STRING)
    {
        cmd_err_no_string();
        return;
    }
    if (st == CLI_CMD_NO_TARGET)
    {
        cmd_err_no_target();
        return;
    }
    if (!scan_document_for_next_line())
    {
        cmd_err_no_string();
        return;
    }
    int change_count = 0;
    for (;;)
    {
        change_count++;
        move_cursor_to_address(doc_working_ptr);
        print_xpos = 0;
        if (check_area_memory(doc_working_ptr))
            goto c830d;
        if (scan_document_for_next_line())
            continue;
        break;
    }
    render_number_to_screen(change_count);
    cli_putstring(" string(s) changed\n");
    return_to_cli_prompt();
    return;
c830d:
    display_not_enough_memory();
}

/**
 * Clears all document markers.
 */
void clear_cmd(void)
{
    memset(markers_array, 0, sizeof(markers_array));
    return;
}

/**
 * Closes the output file, resets editing flags and returns to the CLI prompt.
 */
static void close_input_output_files(void)
{
    input_file_empty_flag = 0;
    file_edit_flags = 0;
    file_ptr = output_fp;
    close_file();
    return_to_cli_prompt();
    return;
}

/**
 * Reports a "No string found" error and returns to the CLI prompt.
 */
static void cmd_err_no_string(void)
{
    cli_putstring("No string found\n");
    return_to_cli_prompt();
    return;
}

/**
 * Reports a "No target given" error and returns to the CLI prompt.
 */
static void cmd_err_no_target(void)
{
    cli_putstring("No target given\n");
    return_to_cli_prompt();
    return;
}

/**
 * Counts words in the document area handling command prefixes and punctuation.
 *
 * @param scan scan state containing optional marker range
 */
static void count_cmd(struct scan_state* scan)
{
    uint8_t idx;
    uint8_t tmp_ch3;
    static const uint8_t count_word_table[] = {
        0x52, 0x4a, 'C', 'E', 'L', 'J', 0};
    parse_marks_from_command(scan);
    if (sanitise_area() == AREA_EMPTY)
    {
        return_to_cli_prompt();
        return;
    }
    uint8_t* line_ptr = area_start_ptr;
    int scan_ptr = 0;
    screen_column = 0;
    screen_row = 0;
c86b8:
    uint8_t pos = 0;
    command_prefix_t cp = deref_and_check_for_command_prefix(pos, line_ptr);
    if (!(cp == NO_COMMAND_PREFIX))
    {
        idx = 0;
        pos++;
        do
        {
            uint8_t cur_ch = line_ptr[pos];
            pos++;
            if (!(cur_ch != count_word_table[idx]))
            {
                if (line_ptr[pos] == count_word_table[idx + 1])
                    goto c86df;
            }
            if (count_word_table[idx + 2] == 0)
                goto c86db;
            pos--;
            idx++;
            idx++;
        } while (idx != 0);
    c86db:
        tmp_ch3 = 0x80;
        goto c86ff;
    c86df:
        line_ptr += 3;
    }
    else
    {
        uint8_t pos2 = 0;
        bool is_tab = false;
        tmp_ch3 =
            process_current_document_character(line_ptr, &idx, &pos2, &is_tab);
        tmp_ch3 &= 0x7f;
        idx = 0;
        if (!((int8_t)screen_row < 0))
        {
            if (!(tmp_ch3 == 0x0d || tmp_ch3 == 0x20))
            {
            c86ff:
                screen_column++;
                if (screen_column != 0)
                    goto c8715;
            }
            if (screen_column != 0)
                scan_ptr++;
        }
        screen_column = idx;
        if (tmp_ch3 == 0x0d)
            screen_row = idx;
    c8715:
        tmp_ch3 |= screen_row;
        screen_row = tmp_ch3;
        line_ptr++;
    }
    if (line_ptr != area_end_ptr)
        goto c86b8;
    render_number_to_screen(scan_ptr);
    cli_putstring(" word(s) counted.\n");
    return_to_cli_prompt();
    return;
}

/**
 * Enters continuous editing mode using the specified input and output files.
 *
 * @param scan scan state containing input and output filenames
 */
static void edit_cmd(struct scan_state* scan)
{
    uint8_t cur_ch;
    check_not_continuous_editing();
    parse_filename_from_command(scan);
    set_document_name_to_filename_buffer();
    open_input_file();
    parse_filename_from_command(scan);
    open_output_file();
    uint8_t idx = 0;
    input_file_empty_flag = idx;
    do
    {
        cur_ch = filename_buffer[idx];
        if (cur_ch == 0)
            cur_ch = 0x0d;
        output_filename[idx] = cur_ch;
        idx++;
    } while (cur_ch != 0x0d);
    initialise_document();
    if (read_first_chunk_from_input_file())
    {
        close_input_output_files();
        return_to_cli_prompt();
        return;
    }
    file_edit_flags = 1;
}

/**
 * Sets the tab key field width from a parsed integer argument.
 *
 * @param scan scan state containing the field width value
 */
static void field_cmd(struct scan_state* scan)
{
    int value;
    if (!parse_integer_from_command(scan, &value))
    {
        return_to_cli_prompt();
        return;
    }
    uint8_t cur_ch = value & 0xFF;
    if (cur_ch == 0x1b)
    {
        cli_putstring("Frump!\n");
        return_to_cli_prompt();
        return;
    }
    current_tab_key = cur_ch;
    return_to_cli_prompt();
    return;
}

/**
 * Writes remaining document content to the output file in chunks.
 */
static void finish_cmd(void)
{
    check_continuous_editing();
    while (1)
    {
        reset_area_to_entire_document();
        sanitise_area();
        file_ptr = output_fp;
        write_area_to_file();
        fputc(0, file_ptr);
        adjust_area_pointers(area_size);
        move_cursor_to_top_of_document();
        ensure_cr_at_document_top();
        if (input_file_empty_flag != 0)
        {
            close_input_output_files();
            return;
        }
        if (read_first_chunk_from_input_file())
        {
            return_to_cli_prompt();
            return;
        }
    }
}

/**
 * Toggles or reports the folding mode.
 *
 * @param scan scan state containing optional folding argument
 */
static void fold_cmd(struct scan_state* scan)
{
    if (!(scan_input_buffer(input_buffer, scan)))
    {
        uint8_t cur_ch = input_buffer[scan->pos];
        if (cur_ch == '1')
        {
            folding_flag = 0;
            goto c87b4;
        }
        if (cur_ch == '0')
            folding_flag = 0x80;
    }
c87b4:
    cli_putstring("Folding ");
    if (((int8_t)folding_flag < 0))
    {
        cli_putstring("off\n");
        return_to_cli_prompt();
        return;
    }
    cli_putstring("on\n");
    return_to_cli_prompt();
    return;
    cli_putstring("Bad file\n");
    return_to_cli_prompt();
    return;
}

/**
 * Formats the document area paragraph by paragraph.
 *
 * @param scan scan state containing optional marker range
 */
static void format_cmd(struct scan_state* scan)
{
    parse_marks_from_command(scan);
    if (!(sanitise_area() == AREA_EMPTY))
    {
        move_cursor_to_address(area_start_ptr);
        clear_format_mode_bit7();
        wipe_buffer(0x10, edit_buffer_base);
        current_format_line_ptr = &ram[RAM_EDIT_BUFFER];
        do
        {
            format_result_t fr = format_paragraph();
            if (fr == FORMAT_MEMORY_FULL)
                goto c8791;
            if (fr == FORMAT_AT_END)
                break;
            cli_putchar(0x2e);
        } while (current_line_ptr < area_end_ptr);
        top_of_screen_line_ptr = &ram[RAM_MAX];
    }
    cli_putchar('\n');
    return_to_cli_prompt();
    return;
c8791:
    cli_putchar('\n');
    display_not_enough_memory();
    return;
}

/**
 * Loads a document from a file and moves the cursor to the top.
 *
 * @param scan scan state containing the filename
 */
static void load_cmd(struct scan_state* scan)
{
    check_not_continuous_editing();
    parse_filename_from_command(scan);
    initialise_document();
    top = page;
    reset_area_to_entire_document();
    top = read_into_document();
    reset_document_name_after_load();
    clear_cmd();
    move_cursor_to_top_of_document();
    return;
}

/**
 * Configures microspacing via the printer driver.
 *
 * @param scan scan state containing optional microspacing value
 */
static void microspace_cmd(struct scan_state* scan)
{
    prepare_printer_driver();
    int value;
    bool parsed = parse_integer_from_command(scan, &value);
    uint8_t idx = 0x0a;
    if (parsed)
    {
        idx = value & 0xFF;
        if (idx == 0)
            return;
    }
    uint8_t pos;
    printer_driver_ptr->printer_getflags(&idx, &pos);
    uint8_t cur_ch = pos;
    cur_ch &= 1;
    if (cur_ch != 0)
    {
        microspacing_flag = idx;
        return;
    }
    cli_putstring("Driver does not support microspacing\n");
    return_to_cli_prompt();
    return;
}

/**
 * Reports a "Bad mode" error and returns to the CLI prompt.
 */
static void mode_cmd(void)
{
    cli_putstring("Bad mode\n");
    return_to_cli_prompt();
    return;
}

/**
 * Appends more text from the input file at the current cursor position.
 *
 * @param scan scan state containing optional marker
 */
static void more_cmd(struct scan_state* scan)
{
    check_continuous_editing();
    parse_marks_from_command(scan);
    move_cursor_to_address(area_start_ptr);
    file_ptr = output_fp;
    write_area_to_file();
    uint8_t pos = 0;
    uint8_t idx = ruler_buffer_len;
    do
    {
        current_ruler_buffer[pos] = current_ruler_ptr[pos];
        pos++;
        idx--;
    } while (idx != 0);
    current_ruler_buffer[pos] = 0x0d;
    adjust_area_pointers(area_size);
    move_cursor_to_top_of_document();
    check_for_at_least_150_bytes_free();
    if (input_file_empty_flag == 0)
    {
        if (read_next_chunk_from_input_file(top))
        {
            return_to_cli_prompt();
            return;
        }
    }
    ensure_cr_at_document_top();
}

/**
 * Sets the document name from an optional filename argument.
 *
 * @param scan scan state containing optional filename
 */
static void name_cmd(struct scan_state* scan)
{
    check_not_continuous_editing();
    bool has_filename = parse_optional_filename_from_command(scan);
    file_edit_flags = 0;
    if (!has_filename)
        return;
    reset_document_name_after_load();
}

/**
 * Creates a new empty document.
 */
static void new_cmd(void)
{
    check_not_continuous_editing();
    initialise_document();
    return;
}

/**
 * Initiates printing and previews the document on screen.
 *
 * @param scan scan state containing optional marker range
 */
static void print_cmd(struct scan_state* scan)
{
    start_printing();
    print_to_screen(scan);
}

/**
 * Prints the document for screen preview and returns to the CLI prompt.
 *
 * @param scan scan state containing optional marker range
 */
static void print_to_screen(struct scan_state* scan)
{
    print_document(scan);
    return_to_cli_prompt();
    return;
}

/**
 * Handles the printer command and delegates to printing.
 *
 * @param scan scan state containing optional arguments
 */
static void printer_cmd(struct scan_state* scan)
{
    print_cmd(scan);
    return;
}

/**
 * Checks editing state and closes input/output files to quit.
 */
static void quit_cmd(void)
{
    check_continuous_editing();
    close_input_output_files();
}

/**
 * Reads a file into the document at the marked area.
 *
 * @param scan scan state containing filename and optional markers
 */
static void read_cmd(struct scan_state* scan)
{
    parse_filename_from_command(scan);
    parse_marks_from_command(scan);
    read_into_document();
    return_to_cli_prompt();
    return;
}

/**
 * Performs interactive search and replace prompting for each match.
 *
 * @param scan scan state containing search and replace strings
 */
static void replace_cmd(struct scan_state* scan)
{
    cli_cmd_status_t st = process_cli_command(scan);
    if (st != CLI_CMD_OK)
    {
        cmd_err_no_target();
        return;
    }
    if (!scan_document_for_next_line())
    {
        cmd_err_no_string();
        return;
    }
    move_cursor_to_address(doc_working_ptr);
    enter_editor_mode();
c832d:
    redraw_and_write_back();
    draw_prompt_characters('R', 'P');
    uint8_t cur_ch = screen_getchar();
    if (cur_ch == 0x1b)
        return;
    cur_ch &= 0xdf;
    uint8_t idx = 0;
    if (!(cur_ch == 0x59))
    {
        idx--;
        if (cur_ch != 0x4f)
            goto c8356;
    }
    print_xpos = idx;
    setup_area_pointers(doc_working_ptr);
    if (check_area_memory(doc_working_ptr))
    {
        show_memory_full_error();
        esc_key();
        return;
    }
    redraw_and_write_back();
c8356:
    if (!scan_document_for_next_line())
        return;
    move_cursor_to_address(doc_working_ptr);
    goto c832d;
}

/**
 * Saves the document area to the output file.
 *
 * @param scan scan state containing optional filename and marker range
 */
static void save_cmd_write_cmd(struct scan_state* scan)
{
    if (!parse_optional_filename_from_command(scan))
    {
        uint8_t ch;
        if (!(file_edit_flags & 0x40))
        {
            bad_filename_error();
            return;
        }
        uint8_t idx = 0;
        do
        {
            ch = input_filename[idx];
            filename_buffer[idx] = ch;
            idx++;
        } while (ch != 0x0d);
    }
    parse_marks_from_command(scan);
    if (sanitise_area() == AREA_EMPTY)
        return;
    open_output_file();
    write_area_to_file();
    fputc(0, file_ptr);
    close_file();
    return_to_cli_prompt();
    return;
}

/**
 * Shows the document on screen for preview.
 *
 * @param scan scan state containing optional marker range
 */
static void screen_cmd(struct scan_state* scan)
{
    print_to_screen(scan);
    return;
}

/**
 * Searches for the target string and enters the editor at the match.
 *
 * @param scan scan state containing search target and optional range
 */
static void search_cmd(struct scan_state* scan)
{
    if (reset_command_parse_state(scan))
    {
        cmd_err_no_target();
        return;
    }
    parse_marks_from_command(scan);
    if (sanitise_area() == AREA_EMPTY)
    {
        cmd_err_no_string();
        return;
    }
    doc_ptr2 = area_start_ptr;
    doc_ptr3 = area_end_ptr;
    if (!scan_document_for_next_line())
    {
        cmd_err_no_string();
        return;
    }
    move_cursor_to_address(doc_working_ptr);
    enter_editor_mode();
    longjmp(env, JMP_EDITOR);
    return;
}

/**
 * Parses flag letters to configure formatting, justification and insert modes.
 *
 * @param scan scan state containing flag characters
 */
static void setup_cmd(struct scan_state* scan)
{
    static const uint8_t c867d_data[] = {0x4e, 0x4a, 0x00, 0x49, 0x00};
    static const uint8_t c8681_data[] = {0x00, 0x00, 0xff};
    uint8_t idx = 1;
    uint8_t fmt_flag_tmp = idx;
    idx--;
    uint8_t insert_flag_tmp = idx;
    idx--;
    uint8_t justify_flag_tmp = idx;
    do
    {
        if (scan_input_buffer(input_buffer, scan))
            break;
        scan->ch &= 0xdf;
        uint8_t idx2 = 0;
        uint8_t pos;
        do
        {
            if (scan->ch == c867d_data[idx2])
                goto c8669;
            idx2++;
            pos = c867d_data[idx2];
        } while (pos != 0);
        cli_putstring("Bad flag\n");
        return_to_cli_prompt();
        return;
    c8669:
        uint8_t cur_ch = c8681_data[idx2];
        if (idx2 == 0)
            fmt_flag_tmp = cur_ch;
        else if (idx2 == 1)
            justify_flag_tmp = cur_ch;
        else
            insert_flag_tmp = cur_ch;
        input_buffer_offset++;
    } while (input_buffer_offset != 0);
    uint8_t idx3 = 2;
    do
    {
        uint8_t next_ch;
        if (idx3 == 0)
            next_ch = fmt_flag_tmp;
        else if (idx3 == 1)
            next_ch = justify_flag_tmp;
        else
            next_ch = insert_flag_tmp;
        if (idx3 == 0)
            format_mode_flag = next_ch;
        else if (idx3 == 1)
            justifying_flag = next_ch;
        else
            insert_mode_flag = next_ch;
        idx3--;
    } while (!((int8_t)idx3 < 0));
    return_to_cli_prompt();
    return;
}

/**
 * Prints the document to the printer and returns to the CLI.
 *
 * @param scan scan state containing optional marker range
 */
static void sheets_cmd(struct scan_state* scan)
{
    start_printing();
    print_document(scan);
    stop_printing();
    cli_putchar('\n');
    return_to_cli_prompt();
    return;
}

/**
 * Initializes the printing subsystem.
 */
void start_printing(void)
{
    cli_putstring("Sorry, can't print yet\n");
    return_to_cli_prompt();
    return;
}

/**
 * Reads a command line from the CLI input.
 *
 * @return true if the line was empty, false otherwise
 */
bool read_command_line(void)
{
    input_buffer_offset = 0;
    return cli_readstring((char*)input_buffer, MAX_COMMAND_LENGTH);
}

const uint8_t version_string[] = "VIEW\0B3.0 for CP/M-65";

/**
 * Prints a number of words from the help and version string.
 *
 * @param idx number of words to print beyond the first
 */
static void print_x_words_of_help(uint8_t idx)
{
    uint8_t pos = 0;
    for (;;)
    {
        uint8_t cur_ch = version_string[pos];
        if (cur_ch == 0)
        {
            cur_ch = 0x20;
            idx--;
            if ((int8_t)idx < 0)
                break;
        }
        cli_putchar(cur_ch);
        pos++;
    }
    return;
}

static bool parse_command(uint8_t* input_buffer_offset);

/**
 * Parses and dispatches a non-escaped CLI input line.
 */
void input_line_not_escaped(void)
{
    bool failed = parse_command(&input_buffer_offset);
    scratch_offset = screen_row;
    if (failed || screen_row >= 48)
        cli_putstring("Mistake\n");
    struct scan_state scan;
    execute_cli_command(scratch_offset, &scan);
    run_cli();
}

/**
 * Main CLI handler loop, prompting for and dispatching commands.
 */
void cli_handler_impl(void)
{
    stop_printing();
    print_flags = 0;
    cli_putstring("=>");
    if (!read_command_line())
    {
        input_line_not_escaped();
        return;
    }
    run_editor();
}

/**
 * Displays the CLI status screen and returns to the prompt.
 */
void run_cli(void)
{
    screen_leave();
    clear_screen();
    print_x_words_of_help(1);
    cli_putstring("\n\nBytes free ");
    render_number_to_screen(compute_bytes_free());
    cli_putchar('\n');
    display_document_file_state();
    if (!((file_edit_flags & 0x40)))
    {
        if ((file_edit_flags & 1))
        {
            cli_putstring("Input file is ");
            if (input_file_empty_flag == 0)
                cli_putstring("not ");
            cli_putstring("empty\n");
        }
    }
    if (!(printer_driver_name[0] == 0))
    {
        cli_putstring("Printer ");
        uint8_t idx = 0;
        do
        {
            uint8_t tmp_ch3 = printer_driver_name[idx];
            if (tmp_ch3 == 0x0d)
                break;
            cli_putchar(tmp_ch3);
            idx++;
        } while (idx != 0);
        if (microspacing_flag != 0)
            cli_putstring(" (m)");
        cli_putchar('\n');
    }
    uint8_t idx2 = 0;
    uint8_t pos = 0;
    do
    {
        if (!(((uint8_t*)markers_array)[idx2 + 1] == 0))
        {
            if (!(pos != 0))
            {
                screen_column = idx2;
                cli_putstring("Marker(s) set ");
                idx2 = screen_column;
                pos = 1;
            }
            else
            {
                screen_putchar(0x2c);
            }
            screen_putchar((idx2 >> 1) + 0x31);
        }
        idx2++;
        idx2++;
    } while (idx2 != 0x0c);
    if (pos != 0)
        cli_putchar('\n');
    cli_putchar('\n');
    return_to_cli_prompt();
}

/**
 * Parses a command name from the input buffer against the parser table.
 *
 * @param input_buffer_offset pointer to current offset in the input buffer;
 * updated to position after the command
 * @return true on parse failure, false on success
 */
static bool parse_command(uint8_t* input_buffer_offset)
{
    uint8_t pos;
    uint8_t cur_ch = 0xff;
    screen_row = cur_ch;
    uint8_t idx = cur_ch;
    for (;;)
    {
        pos = *input_buffer_offset;
        pos--;
        screen_row++;
        for (;;)
        {
            idx++;
            pos++;
            uint8_t next_ch = input_buffer[pos];
            next_ch &= 0xdf;
            temp_save = next_ch;
            uint8_t tmp_ch2 = parser_table[idx];
            if (tmp_ch2 == 0)
                goto ca890;
            if (tmp_ch2 & 0x80)
                goto ca87e;
            tmp_ch2 ^= 0x5b;
            screen_column = tmp_ch2;
            tmp_ch2 &= 0xdf;
            if (tmp_ch2 != temp_save)
                break;
        }
        uint8_t tmp_ch3;
        do
        {
            idx++;
            tmp_ch3 = parser_table[idx];
            if (tmp_ch3 == 0)
                goto ca890;
        } while (!(tmp_ch3 & 0x80));
        uint8_t tmp_ch4 = screen_column;
        tmp_ch4 &= 0x20;
        if (tmp_ch4 == 0)
            continue;
        if (input_buffer[pos] >= 0x30)
            continue;
        break;
    }
ca87e:
    uint8_t tmp_ch6 = input_buffer[pos];
    if (tmp_ch6 < 0x30)
    {
        delimiter_char = tmp_ch6;
        pos++;
    }
    *input_buffer_offset = pos;
    return false;
ca890:
    return true;
}

/**
 * Displays a file error message and returns to the CLI prompt.
 */
void file_error(void)
{
    cli_putstring("File error");
    return_to_cli_prompt();
    return;
}

/**
 * Displays a file not found error and returns to the CLI prompt.
 */
void file_not_found_error(void)
{
    stop_printing();
    cli_putstring("File not found\n");
    return_to_cli_prompt();
    return;
}

/**
 * Parses a decimal integer from the command input buffer.
 *
 * @param scan scan state for scanning the input buffer
 * @param out pointer to receive the parsed integer
 * @return true if an integer was parsed, false otherwise
 */
bool parse_integer_from_command(struct scan_state* scan, int* out)
{
    if (scan_input_buffer(input_buffer, scan))
        return false;
    uint8_t pos = scan->pos;
    const char* start = (const char*)&input_buffer[pos];
    char* end;
    int parsed = (int)strtoul(start, &end, 10);
    (void)pos;
    if (out)
        *out = parsed;
    return (end != start);
}

/**
 * Parses zero to two marker arguments to define the operation area.
 *
 * @param scan scan state for scanning markers
 */
void parse_marks_from_command(struct scan_state* scan)
{
    reset_area_to_entire_document();
    uint8_t* start_mark = parse_mark_from_command(scan);
    if (start_mark == NULL)
        return;
    area_start_ptr = start_mark;
    uint8_t* end_mark = parse_mark_from_command(scan);
    if (end_mark == NULL)
        return;
    area_end_ptr = end_mark;
}

/**
 * Marks the document as loaded and updates the document name from the filename
 * buffer.
 */
void reset_document_name_after_load(void)
{
    file_edit_flags = 0x40;
    set_document_name_to_filename_buffer();
}

/**
 * Copies the filename buffer into the document name storage.
 */
void set_document_name_to_filename_buffer(void)
{
    uint8_t cur_ch;
    uint8_t idx = 0;
    do
    {
        cur_ch = filename_buffer[idx];
        input_filename[idx] = cur_ch;
        idx++;
    } while (cur_ch >= 0x21);
    input_filename[idx - 1] = 0x0d;
    return;
}

/**
 * Zero-terminates the filename buffer at its line terminator.
 */
void zero_terminate_filename_buffer(void)
{
    uint8_t idx = 0;
    while (filename_buffer[idx] != 0x0d)
        idx++;
    filename_buffer[idx] = 0;
    return;
}

/**
 * Parses a single marker reference from the command.
 *
 * @param scan scan state for scanning the marker
 * @return pointer to the marker location, or NULL if no marker was present
 */
uint8_t* parse_mark_from_command(struct scan_state* scan)
{
    if (scan_input_buffer(input_buffer, scan))
        return NULL;
    scan->pos++;
    input_buffer_offset = scan->pos;
    int marker_index = lookup_marker(scan->ch);
    if (marker_index == MARKER_INVALID)
    {
        cli_putstring("Bad marker\n");
        return_to_cli_prompt();
        return 0;
    }
    if (markers_array[marker_index] == 0)
    {
        cli_putstring("Marker not set\n");
        return_to_cli_prompt();
        return 0;
    }
    return markers_array[marker_index];
}
