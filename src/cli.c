#include "cli.h"
#include "document.h"
#include "printing.h"
#include "view.h"
#include "io.h"
#include "editor.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/** Command table for CLI parsing. Encodes command names and flags. */
// clang-format off
static const uint8_t parser_table[] = {
    // QUIT -> 0, flag=1
    0x0a, 0x0e, 0x12, 0x0f, 0x81,
    // NEW -> 1, flag=0
    0x15, 0x1e, 0x0c, 0x80,
    // FORMAT -> 2, flag=1
    0x1d, 0x14, 9, 0x36, 0x3a, 0x2f, 0x81,
    // SETUP -> 3, flag=0
    8, 0x1e, 0x0f, 0x2e, 0x2b, 0x80,
    // READ -> 4, flag=1
    9, 0x1e, 0x3a, 0x3f, 0x81,
    // MORE -> 5, flag=1
    0x16, 0x14, 0x29, 0x3e, 0x81,
    // SCREEN -> 6, flag=1
    8, 0x18, 0x29, 0x3e, 0x3e, 0x35, 0x81,
    // SHEETS -> 7, flag=1
    8, 0x13, 0x3e, 0x3e, 0x2f, 0x28, 0x81,
    // SAVE -> 8, flag=1
    8, 0x1a, 0x2d, 0x3e, 0x81,
    // COUNT -> 9, flag=1
    0x18, 0x14, 0x2e, 0x35, 0x2f, 0x81,
    // FIELD -> 10, flag=1
    0x1d, 0x12, 0x3e, 0x37, 0x3f, 0x81,
    // PRINTER -> 11, flag=1
    0x0b, 9, 0x12, 0x15, 0x0f, 0x1e, 0x29, 0x81,
    // SEARCH -> 12, flag=1
    8, 0x3e, 0x3a, 0x29, 0x38, 0x33, 0x81,
    // CLEAR -> 13, flag=1
    0x18, 0x17, 0x3e, 0x3a, 0x29, 0x81,
    // MICROSPACE -> 14, flag=1
    0x16, 0x12, 0x38, 0x29, 0x34, 0x28, 0x2b, 0x3a, 0x38, 0x3e, 0x81,
    // FOLD -> 15, flag=1
    0x1d, 0x14, 0x37, 0x3f, 0x81,
    // NAME -> 16, flag=1
    0x15, 0x3a, 0x36, 0x3e, 0x81,
    // MODE -> 17, flag=0
    0x16, 0x34, 0x3f, 0x3e, 0x80,
    // FINISH -> 18, flag=1
    0x1d, 0x32, 0x35, 0x32, 0x28, 0x33, 0x81,
    // PRINT -> 19, flag=1
    0x0b, 0x29, 0x32, 0x35, 0x2f, 0x81,
    // CHANGE -> 20, flag=1
    0x18, 0x33, 0x3a, 0x35, 0x3c, 0x3e, 0x81,
    // WRITE -> 21, flag=1
    0x0c, 0x29, 0x32, 0x2f, 0x3e, 0x81,
    // EDIT -> 22, flag=0
    0x1e, 0x3f, 0x32, 0x2f, 0x80,
    // REPLACE -> 23, flag=1
    9, 0x3e, 0x2b, 0x37, 0x3a, 0x38, 0x3e, 0x81,
    // LOAD -> 24, flag=0
    0x17, 0x34, 0x3a, 0x3f, 0x80,
    // BYE -> 25, flag=1
    0x19, 0x22, 0x3e, 0x80,
    0};
// clang-format on

enum command
{
    COMMAND_INVALID = -1,
    COMMAND_QUIT = 0,
    COMMAND_NEW,
    COMMAND_FORMAT,
    COMMAND_SETUP,
    COMMAND_READ,
    COMMAND_MORE,
    COMMAND_SCREEN,
    COMMAND_SHEETS,
    COMMAND_SAVE,
    COMMAND_COUNT,
    COMMAND_FIELD,
    COMMAND_PRINTER,
    COMMAND_SEARCH,
    COMMAND_CLEAR,
    COMMAND_MICROSPACE,
    COMMAND_FOLD,
    COMMAND_NAME,
    COMMAND_MODE,
    COMMAND_FINISH,
    COMMAND_PRINT,
    COMMAND_CHANGE,
    COMMAND_WRITE,
    COMMAND_EDIT,
    COMMAND_REPLACE,
    COMMAND_LOAD,
    COMMAND_BYE,
};

static const uint8_t version_string[] = "C-VIEW\0A4.0";

static const uint8_t escaped_char_table[] = {
    '?', 'T', 'C', 'S', 'L', 'Z', '-', '*', 0xff};
static const uint8_t escaped_value_table[] = {
    1, 9, 0x0d, 2, 0x0b, 0x1a, 0x1c, 0x1d, 0xff};

static enum command parse_command(uint8_t* input_buffer_offset);
static bool parse_integer_from_command(scan_state_t* scan, int* out);
static bool read_command_line(void);
static bool reset_command_parse_state(scan_state_t* scan);
static cli_cmd_status_t process_cli_command(scan_state_t* scan);
static uint8_t expand_escaped_string(uint8_t idx, uint8_t pos);
static uint8_t read_next_command_byte(uint8_t* pos, bool* end);
static uint8_t* parse_mark_from_command(scan_state_t* scan);
static void bad_filename_error(void);
static void bye_cmd(void);
static void change_cmd(scan_state_t* scan);
static void check_continuous_editing(void);
static void close_input_output_files(void);
static void cmd_err_no_string(void);
static void cmd_err_no_target(void);
static void count_cmd(scan_state_t* scan);
static void edit_cmd(scan_state_t* scan);
static void execute_cli_command(enum command cur_ch, scan_state_t* scan);
static void field_cmd(scan_state_t* scan);
static void finish_cmd(void);
static void fold_cmd(scan_state_t* scan);
static void format_cmd(scan_state_t* scan);
static void input_line_not_escaped(void);
static void microspace_cmd(scan_state_t* scan);
static void mode_cmd(void);
static void more_cmd(scan_state_t* scan);
static void name_cmd(scan_state_t* scan);
static void new_cmd(void);
static void parse_filename_from_command(scan_state_t* scan);
static void parse_marks_from_command(scan_state_t* scan);
static void print_cmd(scan_state_t* scan);
static void print_to_screen(scan_state_t* scan);
static void print_x_words_of_help(uint8_t idx);
static void printer_cmd(scan_state_t* scan);
static void quit_cmd(void);
static void read_cmd(scan_state_t* scan);
static void replace_cmd(scan_state_t* scan);
static void reset_document_name_after_load(void);
static void save_cmd_write_cmd(scan_state_t* scan);
static void screen_cmd(scan_state_t* scan);
static void search_cmd(scan_state_t* scan);
static void set_document_name_to_filename_buffer(void);
static void setup_cmd(scan_state_t* scan);
static void sheets_cmd(scan_state_t* scan);

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
 * Reads a command line from the CLI input.
 *
 * @return true if the line was empty, false otherwise
 */
static bool read_command_line(void)
{
    input_buffer_offset = 0;

    return cli_readstring((char*)input_buffer, MAX_COMMAND_LENGTH);
}

/**
 * Parses and dispatches a non-escaped CLI input line.
 */
static void input_line_not_escaped(void)
{
    enum command cmd = parse_command(&input_buffer_offset);

    if (cmd == COMMAND_INVALID)
    {
        cli_putstring("Mistake\n");
        return_to_cli_prompt();
    }

    scan_state_t scan = {};
    execute_cli_command(cmd, &scan);
    run_cli();
}

/**
 * Dispatches a CLI command by index to its handler.
 *
 * @param cur_ch command index into the CLI jump table
 * @param scan scan state for argument parsing
 */
static void execute_cli_command(enum command cur_ch, scan_state_t* scan)
{
    switch (cur_ch)
    {
        case COMMAND_QUIT:
            quit_cmd();
            break;

        case COMMAND_NEW:
            new_cmd();
            break;

        case COMMAND_FORMAT:
            format_cmd(scan);
            break;

        case COMMAND_SETUP:
            setup_cmd(scan);
            break;

        case COMMAND_READ:
            read_cmd(scan);
            break;

        case COMMAND_MORE:
            more_cmd(scan);
            break;

        case COMMAND_SCREEN:
            screen_cmd(scan);
            break;

        case COMMAND_SHEETS:
            sheets_cmd(scan);
            break;

        case COMMAND_SAVE:
            save_cmd_write_cmd(scan);
            break;

        case COMMAND_COUNT:
            count_cmd(scan);
            break;

        case COMMAND_FIELD:
            field_cmd(scan);
            break;

        case COMMAND_PRINTER:
            printer_cmd(scan);
            break;

        case COMMAND_SEARCH:
            search_cmd(scan);
            break;

        case COMMAND_CLEAR:
            clear_cmd();
            break;

        case COMMAND_MICROSPACE:
            microspace_cmd(scan);
            break;

        case COMMAND_FOLD:
            fold_cmd(scan);
            break;

        case COMMAND_NAME:
            name_cmd(scan);
            break;

        case COMMAND_MODE:
            mode_cmd();
            break;

        case COMMAND_FINISH:
            finish_cmd();
            break;

        case COMMAND_PRINT:
            print_cmd(scan);
            break;

        case COMMAND_CHANGE:
            change_cmd(scan);
            break;

        case COMMAND_WRITE:
            save_cmd_write_cmd(scan);
            break;

        case COMMAND_EDIT:
            edit_cmd(scan);
            break;

        case COMMAND_REPLACE:
            replace_cmd(scan);
            break;

        case COMMAND_LOAD:
            load_cmd(scan);
            break;

        case COMMAND_BYE:
            bye_cmd();
            break;

        case COMMAND_INVALID:
            break;
    }
}

/**
 * Exits the program.
 */
static void bye_cmd(void)
{
    exit(0);
}

/**
 * Replaces all occurrences of the search string in the document area.
 *
 * @param scan scan state containing search and replace arguments
 */
static void change_cmd(scan_state_t* scan)
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
 * Counts words in the document area handling command prefixes and punctuation.
 *
 * @param scan scan state containing optional marker range
 */
static void count_cmd(scan_state_t* scan)
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

    if (cp != NO_COMMAND_PREFIX)
    {
        idx = 0;
        pos++;

        do
        {
            uint8_t cur_ch = line_ptr[pos];

            pos++;

            if (cur_ch == count_word_table[idx])
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

        if ((int8_t)screen_row >= 0)
        {
            if (tmp_ch3 != 0x0d && tmp_ch3 != 0x20)
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
}

/**
 * Enters continuous editing mode using the specified input and output files.
 *
 * @param scan scan state containing input and output filenames
 */
static void edit_cmd(scan_state_t* scan)
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
static void field_cmd(scan_state_t* scan)
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
static void fold_cmd(scan_state_t* scan)
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
}

/**
 * Formats the document area paragraph by paragraph.
 *
 * @param scan scan state containing optional marker range
 */
static void format_cmd(scan_state_t* scan)
{
    parse_marks_from_command(scan);

    if (sanitise_area() != AREA_EMPTY)
    {
        move_cursor_to_address(area_start_ptr);
        clear_format_mode_bit7();
        wipe_buffer(0x10, edit_buffer_base);
        current_format_line = &current_line_buffer;
        heap_format_line_ptr = &current_line_buffer;

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
}

/**
 * Loads a document from a file and moves the cursor to the top.
 *
 * @param scan scan state containing the filename
 */
void load_cmd(scan_state_t* scan)
{
    check_not_continuous_editing();
    parse_filename_from_command(scan);
    open_input_file();

    fseek(input_fp, 0, SEEK_END);
    int length = ftell(input_fp);
    fseek(input_fp, 0, SEEK_SET);

    if (length > (himem - ram - LINE_LENGTH_SPARE))
    {
        close_file();
        display_not_enough_memory();
    }

    initialise_document();
    top = ram;
    while (length != 0)
    {
        size_t bytes_read = fread(top, 1, length, input_fp);
        if (ferror(input_fp))
        {
            close_file();
            initialise_document();
            cli_putstring("I/O error: ");
            cli_putstring(strerror(errno));
            cli_putstring("\n");
            return_to_cli_prompt();
        }
        length -= bytes_read;
        top += bytes_read;
    }

    /* top points at the trailing 0, not the byte after it. */
    top--;

    close_file();
    reset_document_name_after_load();
    clear_cmd();
    move_cursor_to_top_of_document();
}

/**
 * Clears all document markers.
 */
void clear_cmd(void)
{
    memset(markers_array, 0, sizeof(markers_array));
}

/**
 * Configures microspacing via the printer driver.
 *
 * @param scan scan state containing optional microspacing value
 */
static void microspace_cmd(scan_state_t* scan)
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
}

/**
 * Reports a "Bad mode" error and returns to the CLI prompt.
 */
static void mode_cmd(void)
{
    cli_putstring("Bad mode\n");

    return_to_cli_prompt();
}

/**
 * Appends more text from the input file at the current cursor position.
 *
 * @param scan scan state containing optional marker
 */
static void more_cmd(scan_state_t* scan)
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
static void name_cmd(scan_state_t* scan)
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
}

/**
 * Handles the printer command and delegates to printing.
 *
 * @param scan scan state containing optional arguments
 */
static void printer_cmd(scan_state_t* scan)
{
    print_cmd(scan);
}

/**
 * Initiates printing and previews the document on screen.
 *
 * @param scan scan state containing optional marker range
 */
static void print_cmd(scan_state_t* scan)
{
    start_printing();
    print_to_screen(scan);
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
 * Closes the output file, resets editing flags and returns to the CLI prompt.
 */
static void close_input_output_files(void)
{
    input_file_empty_flag = 0;
    file_edit_flags = 0;
    file_ptr = output_fp;
    close_file();

    return_to_cli_prompt();
}

/**
 * Reads a file into the document at the marked area.
 *
 * @param scan scan state containing filename and optional markers
 */
static void read_cmd(scan_state_t* scan)
{
    parse_filename_from_command(scan);
    parse_marks_from_command(scan);
    read_into_document();

    return_to_cli_prompt();
}

/**
 * Performs interactive search and replace prompting for each match.
 *
 * @param scan scan state containing search and replace strings
 */
static void replace_cmd(scan_state_t* scan)
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

    if (cur_ch != 0x59)
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
static void save_cmd_write_cmd(scan_state_t* scan)
{
    if (!parse_optional_filename_from_command(scan))
    {
        uint8_t ch;

        if ((file_edit_flags & 0x40) == 0)
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
}

/**
 * Shows the document on screen for preview.
 *
 * @param scan scan state containing optional marker range
 */
static void screen_cmd(scan_state_t* scan)
{
    print_to_screen(scan);
}

/**
 * Prints the document for screen preview and returns to the CLI prompt.
 *
 * @param scan scan state containing optional marker range
 */
static void print_to_screen(scan_state_t* scan)
{
    print_document(scan);

    return_to_cli_prompt();
}

/**
 * Searches for the target string and enters the editor at the match.
 *
 * @param scan scan state containing search target and optional range
 */
static void search_cmd(scan_state_t* scan)
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
    search_cursor_ptr = area_start_ptr;

    search_limit_ptr = area_end_ptr;

    if (!scan_document_for_next_line())
    {
        cmd_err_no_string();

        return;
    }
    move_cursor_to_address(doc_working_ptr);
    enter_editor_mode();
    longjmp(env, JMP_EDITOR);
}

/**
 * Reports a "No string found" error and returns to the CLI prompt.
 */
static void cmd_err_no_string(void)
{
    cli_putstring("No string found\n");

    return_to_cli_prompt();
}

/**
 * Reports a "No target given" error and returns to the CLI prompt.
 */
static void cmd_err_no_target(void)
{
    cli_putstring("No target given\n");

    return_to_cli_prompt();
}

/**
 * Parses flag letters to configure formatting, justification and insert modes.
 *
 * @param scan scan state containing flag characters
 */
static void setup_cmd(scan_state_t* scan)
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
    } while ((int8_t)idx3 >= 0);

    return_to_cli_prompt();
}

/**
 * Prints the document to the printer and returns to the CLI.
 *
 * @param scan scan state containing optional marker range
 */
static void sheets_cmd(scan_state_t* scan)
{
    start_printing();
    print_document(scan);
    stop_printing();
    cli_putchar('\n');

    return_to_cli_prompt();
}

/**
 * Initializes the printing subsystem.
 */
void start_printing(void)
{
    cli_putstring("Sorry, can't print yet\n");

    return_to_cli_prompt();
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

    if ((file_edit_flags & 0x40) == 0)
    {
        if ((file_edit_flags & 1))
        {
            cli_putstring("Input file is ");

            if (input_file_empty_flag == 0)
                cli_putstring("not ");
            cli_putstring("empty\n");
        }
    }
    if (printer_driver_name[0] != 0)
    {
        cli_putstring("Printer ");
        cli_putstring((char*)printer_driver_name);
        if (microspacing_flag != 0)
            cli_putstring(" (m)");
        cli_putchar('\n');
    }

    uint8_t idx2 = 0;
    uint8_t pos = 0;
    do
    {
        if (((uint8_t*)markers_array)[idx2 + 1] != 0)
        {
            if (pos == 0)
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
}

/**
 * Parses a command name from the input buffer against the parser table.
 *
 * @param input_buffer_offset pointer to current offset in the input buffer;
 * updated to position after the command
 * @return command on success, COMMAND_INVALID on failure
 */
static enum command parse_command(uint8_t* input_buffer_offset)
{
    uint8_t temp_save;
    uint8_t pos;
    int command_index = -1;
    uint8_t idx = 0xff;

    for (;;)
    {
        pos = *input_buffer_offset;
        pos--;
        command_index++;

        for (;;)
        {
            idx++;
            pos++;
            uint8_t next_ch = input_buffer[pos];

            next_ch &= 0xdf;
            temp_save = next_ch;
            uint8_t tmp_ch2 = parser_table[idx];

            if (tmp_ch2 == 0)
                return COMMAND_INVALID;

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
                return COMMAND_INVALID;
        } while ((tmp_ch3 & 0x80) == 0);
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

    return (enum command)command_index;
}

/**
 * Displays a file error message and returns to the CLI prompt.
 */
void file_error(void)
{
    cli_putstring("File error");

    return_to_cli_prompt();
}

/**
 * Displays a file not found error and returns to the CLI prompt.
 */
void file_not_found_error(void)
{
    stop_printing();
    cli_putstring("File not found\n");

    return_to_cli_prompt();
}

/**
 * Parses a decimal integer from the command input buffer.
 *
 * @param scan scan state for scanning the input buffer
 * @param out pointer to receive the parsed integer
 * @return true if an integer was parsed, false otherwise
 */
static bool parse_integer_from_command(scan_state_t* scan, int* out)
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
 * Marks the document as loaded and updates the document name from the filename
 * buffer.
 */
static void reset_document_name_after_load(void)
{
    file_edit_flags = 0x40;
    set_document_name_to_filename_buffer();
}

/**
 * Copies the filename buffer into the document name storage.
 */
static void set_document_name_to_filename_buffer(void)
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
}

/**
 * Verify that continuous editing is active.
 * Displays the document file state when continuous editing is not enabled.
 */
static void check_continuous_editing(void)
{
    if ((file_edit_flags & 0x40) == 0)
    {
        if (file_edit_flags & 1)
            return;
    }
    display_document_file_state();
}

/**
 * Verifies the editor is not in continuous editing mode.
 *
 * Displays the file state if editing is active.
 */
void check_not_continuous_editing(void)
{
    if ((file_edit_flags & 0x40))
        return;

    if ((file_edit_flags & 1) == 0)
        return;
    display_document_file_state();
}

/**
 * Parse a mandatory filename from the command line.
 * Reports an error if no filename is present.
 * @param scan scan state pointing into the command buffer
 */
static void parse_filename_from_command(scan_state_t* scan)
{
    if (!parse_optional_filename_from_command(scan))
    {
        bad_filename_error();

        return;
    }
}

/**
 * Parses an optional filename from the input buffer.
 *
 * @param scan scan state holding the current buffer position
 * @return true if a filename was found, false if none
 */
bool parse_optional_filename_from_command(scan_state_t* scan)
{
    if (scan_input_buffer(input_buffer, scan))
        return false;
    uint8_t idx = 0;

    while (1)
    {
        scan->ch = input_buffer[scan->pos];

        if (scan->ch == 0x0d)
            break;
        scan->pos++;

        if (scan->ch == delimiter_char)
            break;
        filename_buffer[idx] = scan->ch;
        idx++;

        if (idx == MAX_COMMAND_LENGTH - 1)
        {
            bad_filename_error();
            break;
        }
    }
    filename_buffer[idx] = 0x0d;
    input_buffer_offset = scan->pos;

    return true;
}

/**
 * Reports a bad filename error and returns to the command prompt.
 */
static void bad_filename_error(void)
{
    cli_putstring("Bad filename\n");

    return_to_cli_prompt();
}

/**
 * Process a CLI command from the input buffer.
 * Parses the search string and marks, sanitises the area, and copies area
 * pointers to the working pointers.
 * @param scan scan state containing current parse position
 * @return CLI_CMD_NO_TARGET if no command, CLI_CMD_NO_STRING if area empty,
 * CLI_CMD_OK otherwise
 */
static cli_cmd_status_t process_cli_command(scan_state_t* scan)
{
    if (reset_command_parse_state(scan))
        return CLI_CMD_NO_TARGET;

    if (!scan_input_buffer(input_buffer, scan))
    {
        cli_header_limit =
            expand_escaped_string(search_target_len, input_buffer_offset + 1);
    }
    parse_marks_from_command(scan);

    if (sanitise_area() == AREA_EMPTY)
        return CLI_CMD_NO_STRING;

    search_cursor_ptr = area_start_ptr;

    search_limit_ptr = area_end_ptr;

    return CLI_CMD_OK;
}

/**
 * Parses zero to two marker arguments to define the operation area.
 *
 * @param scan scan state for scanning markers
 */
static void parse_marks_from_command(scan_state_t* scan)
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
 * Parses a single marker reference from the command.
 *
 * @param scan scan state for scanning the marker
 * @return pointer to the marker location, or NULL if no marker was present
 */
static uint8_t* parse_mark_from_command(scan_state_t* scan)
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

/**
 * Reset command parse state and extract the search target length.
 * Scans the input buffer and expands any escaped search string.
 * @param scan scan state to initialise
 * @return true if no search string was found, false otherwise
 */
static bool reset_command_parse_state(scan_state_t* scan)
{
    uint8_t idx = 0;

    search_target_len = idx;
    cli_header_limit = idx;

    if (scan_input_buffer(input_buffer, scan))
        return true;
    uint8_t idx2 = expand_escaped_string(0, scan->pos);

    search_target_len = idx2;

    return idx2 == 0;
}

/**
 * Expand an escaped string from the input buffer into the header text buffer.
 * Handles caret escapes and optional case folding.
 * @param idx starting index in the header text buffer
 * @param pos starting position in the input buffer
 * @return updated header text length
 */
static uint8_t expand_escaped_string(uint8_t idx, uint8_t pos)
{
    uint8_t temp_save;
    screen_column = idx;
    pos--;

    do
    {
        uint8_t cur_ch;
        bool end;

        cur_ch = read_next_command_byte(&pos, &end);

        if (end)
            break;

        if (cur_ch == 0x5e)
        {
            uint8_t next_ch = read_next_command_byte(&pos, &end);

            if (end)
                break;
            screen_row = toupper(next_ch);
            temp_save = next_ch;
            idx = 0xfe;

            for (;;)
            {
                idx += 2;
                uint8_t table_idx = idx >> 1;
                uint8_t tmp_ch3 = escaped_char_table[table_idx];

                if (tmp_ch3 & 0x80)
                    break;

                if (tmp_ch3 == screen_row)
                {
                    cur_ch = escaped_value_table[table_idx];

                    if (cur_ch != 0)
                        goto c83ca;
                }
            }
            cur_ch = temp_save;
        }
    c83ca:
        idx = search_target_len;

        if (idx == 0)
            cur_ch = upper_case_unless_folding(cur_ch);
        idx = screen_column;
        header_text_maybe[idx] = cur_ch;
        screen_column++;
    } while (screen_column != 0);
    idx = screen_column;
    input_buffer_offset = pos;

    return idx;
}

/**
 * Read the next byte from the input buffer.
 * Advances the position and reports whether the byte terminates the current
 * token.
 * @param pos pointer to current buffer index, incremented on entry
 * @param end output flag set when byte equals delimiter or carriage return
 * @return the byte at the new position
 */
static uint8_t read_next_command_byte(uint8_t* pos, bool* end)
{
    (*pos)++;
    uint8_t cur_ch = input_buffer[*pos];
    *end = (cur_ch == delimiter_char) || (cur_ch == 0x0d);

    return cur_ch;
}
