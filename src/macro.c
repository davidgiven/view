#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdalign.h>
#include <stddef.h>
#include "io.h"
#include "printing.h"
#include "document.h"
#include "cli.h"
#include "macro.h"

// A macro definition stored in ram[].  Macros form a singly-linked list: a
// node holds the address of the next macro (0 terminates the list) and the
// two-character macro name; the macro body follows the header and is reached
// through body[].
struct macro
{
    struct macro* next;
    char name[2];
    uint8_t body[];
};

_Static_assert(sizeof(struct macro) % alignof(struct macro) == 0,
    "macro size not multiple of alignment");

static inline uint8_t* align_up_ptr(uint8_t* ptr)
{
    uintptr_t addr = (uintptr_t)ptr;
    size_t align = alignof(struct macro);

    addr = (addr + align - 1) & ~(align - 1);

    return (uint8_t*)addr;
}

static struct macro* first_macro_ptr;
static struct macro* last_macro_ptr;

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
 * Initialises the macro list for a new print job.
 *
 * @param print_doc_ptr base pointer for the document print buffer
 */
void macro_init(uint8_t* print_doc_ptr)
{
    first_macro_ptr = (struct macro*)(void*)align_up_ptr(print_doc_ptr + 0x8d);
    last_macro_ptr = first_macro_ptr;
    last_macro_ptr->next = 0;
}

/**
 * Handles the DM formatting command to define a macro.
 */
void dm_fmt_cmd(void)
{
    if (macro_executing_flag != 0)
        return;
    struct macro* new_macro_ptr = last_macro_ptr;

    new_macro_ptr->name[0] = current_format_line_ptr[3] & 0xdf;
    uint8_t secondchar = current_format_line_ptr[4];

    if (isalpha(secondchar))
        secondchar &= 0xdf;
    else
        secondchar = 0x20;
    new_macro_ptr->name[1] = secondchar;

    uint8_t* write_ptr = (uint8_t*)new_macro_ptr;
    uint8_t* body = write_ptr + offsetof(struct macro, body);

    for (;;)
    {
        if (himem - body < 0x97)
        {
            display_not_enough_memory();

            return;
        }
        uint8_t* line_ptr = body;

        current_format_line_ptr = body;

        if (read_next_output_line(body, &line_ptr) == READ_BLOCK_DONE)
            return;

        command_prefix_t cp = check_for_command_prefix(body[0]);

        if (cp != NO_COMMAND_PREFIX &&
            lookup_formatting_command() == FORMATTING_COMMAND_EM)
            break;

        write_ptr = line_ptr;
        body = write_ptr + offsetof(struct macro, body);
    }
    // Align once at the end; previous writes used raw byte pointers
    // so no struct alignment was required while building.
    write_ptr = align_up_ptr(write_ptr);
    body = write_ptr + offsetof(struct macro, body);
    body[0] = 4;
    new_macro_ptr->next = (struct macro*)(void*)align_up_ptr(body + 1);
    last_macro_ptr = (struct macro*)(void*)write_ptr;
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
}

/**
 * Tries to invoke a macro whose name matches the current format line.
 *
 * @param macro_cursor_ptr pointer to the macro cursor to update on success
 * @return true if a macro was found and invoked, false otherwise
 */
bool macro_try_invoke(uint8_t** macro_cursor_ptr)
{
    uint8_t ch1 = current_format_line_ptr[1];
    uint8_t ch2 = current_format_line_ptr[2];

    if (!isalpha(ch2))
        ch2 = 0x20;
    struct macro* macro = first_macro_ptr;

    while (macro->next != NULL)
    {
        if (macro->name[0] == ch1 && macro->name[1] == ch2)
        {
            if (macro_executing_flag != 0)
            {
                nested_macro_error();

                return true;
            }
            *macro_cursor_ptr = macro->body;
            macro_executing_flag = true;

            return true;
        }
        macro = macro->next;
    }
    return false;
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
            ((uint8_t*)&current_line_buffer)[idx] = next_ch;
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
            ((uint8_t*)&current_line_buffer)[idx] = tmp_ch6;
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
