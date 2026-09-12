#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "io.h"
#include "printing.h"
#include "document.h"
#include "cli.h"
#include "macro.h"

enum parse_register_result_t
{
    PARSE_REGISTER_MARKER,
    PARSE_REGISTER_VALUE,
    PARSE_REGISTER_OTHER,
};

static enum parse_register_result_t parse_register_reference(uint8_t cur_ch);
static read_block_status_t read_next_output_line(
    uint8_t* limit, uint8_t** cursor);

/**
 * Handles the DM formatting command to define a macro.
 */
void dm_fmt_cmd(void)
{
    if (macro_executing_flag != 0)
        return;
    struct macro* size_delta;
    size_delta = last_macro_ptr;
    uint8_t pos = 3;
    uint8_t firstchar = current_format_line_ptr[pos];
    firstchar &= 0xdf;
    pos++;
    uint8_t secondchar = current_format_line_ptr[pos];
    if (isalpha(secondchar))
        secondchar &= 0xdf;
    else
        secondchar = 0x20;
    last_macro_ptr->name[1] = secondchar;
    last_macro_ptr->name[0] = firstchar;
    for (;;)
    {
        if (himem - (uint8_t*)(last_macro_ptr->body) < 0x97)
        {
            display_not_enough_memory();
            return;
        }
        uint8_t* line_ptr = last_macro_ptr->body;
        current_format_line_ptr = last_macro_ptr->body;
        if (read_next_output_line(last_macro_ptr->body, &line_ptr) ==
            READ_BLOCK_DONE)
        {
            return;
        }
        command_prefix_t cp = check_for_command_prefix(last_macro_ptr->body[0]);
        if (cp != NO_COMMAND_PREFIX &&
            lookup_formatting_command() == FORMATTING_COMMAND_EM)
        {
            break;
        }
        last_macro_ptr = (struct macro*)line_ptr;
    }
    last_macro_ptr->body[0] = 4;
    last_macro_ptr->body[1] = 0;
    size_delta->next = (struct macro*)(last_macro_ptr->body + 1);
    return;
}

/**
 * Reports a nested macro call error and aborts printing.
 */
void nested_macro_error(void)
{
    stop_printing();
    cli_putstring("Nested macro call");
    cli_putchar('\n');
    return_to_cli_prompt();
    return;
}

/**
 * Prepares the next line for printing, handling macro expansion.
 *
 * @param read_limit upper bound for reading document data
 * @param macro_cursor pointer to the current macro cursor, updated as macros
 * execute
 * @return pointer to the prepared line, or NULL when no more output remains
 */
uint8_t* prepare_output_line(uint8_t* read_limit, uint8_t** macro_cursor)
{
    uint8_t tmp_ch5;
    if (!(macro_executing_flag != 0))
    {
    c9188_normal_entry:
        uint8_t* cursor = read_limit;
        if (read_next_output_line(read_limit, &cursor) == READ_BLOCK_DONE)
            return NULL;
        if (read_limit != NULL)
        {
            current_format_line_ptr = read_limit;
            return read_limit;
        }
    }
    uint8_t pos = 0;
    uint8_t idx = 0;
c91a7:
    for (;;)
    {
        uint8_t next_ch = (*macro_cursor)[pos];
        if (next_ch == 4)
        {
            macro_executing_flag = 0;
            goto c9188_normal_entry;
        }
        if (next_ch == 0x40)
            break;
        pos++;
        for (;;)
        {
            ram[RAM_CURRENT_LINE_BUF + idx] = next_ch;
            idx++;
            if (next_ch == 0x0d)
            {
                (*macro_cursor) += pos;
                current_format_line_ptr = edit_buffer_base;
                return edit_buffer_base;
            }
            if (idx < 0x83)
                break;
            next_ch = 0x0d;
        }
    }

    pos++;
    uint8_t tmp_ch3 = (*macro_cursor)[pos];
    if (!(tmp_ch3 < 0x30))
    {
        tmp_ch3 -= 0x30;
        if (tmp_ch3 >= 0x0a)
            goto c9225;
        pos++;
        temp_save = pos;
        screen_column = tmp_ch3;
        screen_row = 0;
        uint8_t pos2 = 2;
        do
        {
            screen_column--;
            if ((int8_t)screen_column < 0)
                goto c9209;
        c91f5:
            pos2++;
            tmp_ch5 = read_limit[pos2];
            if (tmp_ch5 == 0x0d)
                goto c9223;
            {
                enum parse_register_result_t r =
                    parse_register_reference(tmp_ch5);
                if (r == PARSE_REGISTER_MARKER || r == PARSE_REGISTER_VALUE)
                    goto c91f5;
            }
        } while (tmp_ch5 == 0x2c);
        goto c91f5;
    c9209:
        do
        {
            pos2++;
            uint8_t tmp_ch6 = read_limit[pos2];
            if (tmp_ch6 == 0x0d)
                break;
            {
                enum parse_register_result_t r_1 =
                    parse_register_reference(tmp_ch6);
                if (r_1 == PARSE_REGISTER_MARKER)
                    continue;
                if (r_1 == PARSE_REGISTER_VALUE)
                    goto c921b;
            }
            if (tmp_ch6 == 0x2c)
                break;
        c921b:
            ram[RAM_CURRENT_LINE_BUF + idx] = tmp_ch6;
            idx++;
        } while (idx < 0x82);
    c9223:
        pos = temp_save;
    }
c9225:
    goto c91a7;
}

static enum parse_register_result_t parse_register_reference(uint8_t cur_ch)
{
    if (cur_ch == 0x3e)
    {
        cur_ch = 0;
        screen_row = cur_ch;
        return PARSE_REGISTER_MARKER;
    }
    if (cur_ch == 0x3c)
    {
        cur_ch = 0x40;
        screen_row = cur_ch;
        return PARSE_REGISTER_MARKER;
    }
    if (cur_ch & screen_row)
        return PARSE_REGISTER_VALUE;
    return PARSE_REGISTER_OTHER;
}

/**
 * Reads the next output line from the document or file.
 *
 * @param limit upper bound for reading
 * @param cursor current read cursor, updated on return
 * @return status indicating completion
 */
static read_block_status_t read_next_output_line(
    uint8_t* limit, uint8_t** cursor)
{
    uint8_t a2;
    if (printing_from_file_flag == 0)
        return read_block_from_file(cursor, limit);
    uint8_t pos = 0;
    do
    {
        a2 = printer_ptr6[pos];
        if (a2 == 0)
            return READ_BLOCK_DONE;
        (*cursor)[pos] = a2;
        printer_ptr6++;
        (*cursor)++;
    } while (a2 != 0x0d);
    return READ_BLOCK_MORE;
}
