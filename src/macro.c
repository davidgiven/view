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
#include "view.h"
#include "macro.h"

// A macro definition stored in ram[].  Macros form a singly-linked list: a
// node holds the address of the next macro (0 terminates the list) and the
// two-character macro name; the macro body follows the header and is reached
// through body[].
typedef struct macro
{
    struct macro* next;
    uint8_t name[2];
    uint8_t body[];
} macro_t;

_Static_assert(sizeof(macro_t) % alignof(macro_t) == 0,
    "macro size not multiple of alignment");

static macro_t* first_macro_ptr;
static macro_t* last_macro_ptr;

typedef enum parse_register_result_t
{
    PARSE_REGISTER_MARKER,
    PARSE_REGISTER_VALUE,
    PARSE_REGISTER_OTHER,
} parse_register_result_t;

static inline uint8_t* align_up_ptr(uint8_t* ptr);
static void nested_macro_error(void);
static parse_register_result_t parse_register_reference(
    int cur_ch, uint8_t* register_mask);
static read_block_status_t read_next_output_line(
    uint8_t** cursor, uint8_t* limit);

/**
 * Initialises the macro list for a new print job.
 *
 * @param print_doc_ptr base pointer for the document print buffer
 */
void macro_init(uint8_t* print_doc_ptr)
{
    first_macro_ptr = (macro_t*)(void*)align_up_ptr(print_doc_ptr + 0x8d);
    last_macro_ptr = first_macro_ptr;
    last_macro_ptr->next = 0;
}

/**
 * Handles the DM formatting command to define a macro.
 */
void dm_fmt_cmd(void)
{
    if (macro_executing_flag)
        return;
    macro_t* new_macro_ptr = last_macro_ptr;

    new_macro_ptr->name[0] = heap_format_line_ptr->text[0] & 0xdf;
    uint8_t secondchar = heap_format_line_ptr->text[1];

    if (isalpha(secondchar))
        secondchar &= 0xdf;
    else
        secondchar = 0x20;
    new_macro_ptr->name[1] = secondchar;

    uint8_t* write_ptr = (uint8_t*)new_macro_ptr;
    uint8_t* body = write_ptr + offsetof(macro_t, body);

    for (;;)
    {
        if (himem - body <= LINE_LENGTH_SPARE)
        {
            display_not_enough_memory();

            return;
        }
        uint8_t* line_ptr = body;

        heap_format_line_ptr = (line_t*)body;
        current_format_line = body;

        if (read_next_output_line(&line_ptr, body) == READ_BLOCK_DONE)
            return;

        command_prefix_t cp = check_for_command_prefix(body[0]);

        if (cp != NO_COMMAND_PREFIX &&
            lookup_formatting_command() == FORMATTING_COMMAND_EM)
            break;

        write_ptr = line_ptr;
        body = write_ptr + offsetof(macro_t, body);
    }
    // Align once at the end; previous writes used raw byte pointers
    // so no struct alignment was required while building.
    write_ptr = align_up_ptr(write_ptr);
    body = write_ptr + offsetof(macro_t, body);
    body[0] = 4;
    new_macro_ptr->next = (macro_t*)(void*)align_up_ptr(body + 1);
    last_macro_ptr = (macro_t*)(void*)write_ptr;
}

/**
 * Aligns a pointer up to the macro alignment.
 * @param ptr pointer to align
 * @return aligned pointer
 */
static inline uint8_t* align_up_ptr(uint8_t* ptr)
{
    uintptr_t addr = (uintptr_t)ptr;
    size_t align = alignof(macro_t);

    addr = (addr + align - 1) & ~(align - 1);

    return (uint8_t*)addr;
}

/**
 * Tries to invoke a macro whose name matches the current format line.
 *
 * @param macro_cursor_ptr pointer to the macro cursor to update on success
 * @return true if a macro was found and invoked, false otherwise
 */
bool macro_try_invoke(uint8_t** macro_cursor_ptr)
{
    uint8_t ch1 = heap_format_line_ptr->command[0];
    uint8_t ch2 = heap_format_line_ptr->command[1];

    if (!isalpha(ch2))
        ch2 = 0x20;
    macro_t* macro = first_macro_ptr;

    while (macro->next != NULL)
    {
        if (macro->name[0] == ch1 && macro->name[1] == ch2)
        {
            if (macro_executing_flag)
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
 * Reports a nested macro call error and aborts printing.
 */
static void nested_macro_error(void)
{
    stop_printing();
    cli_putstring("Nested macro call");
    cli_putchar('\n');

    return_to_cli_prompt();
}

/**
 * Copies a numbered invocation argument into the macro output buffer.
 * Honors register-reference markers while locating commas and copying bytes.
 * @param invocation invocation line, including its three-byte command header
 * @param argument zero-based argument number
 * @param output_pos next position in the output buffer
 * @return updated output position
 */
static uint8_t expand_macro_argument(
    uint8_t* invocation, uint8_t argument, uint8_t output_pos)
{
    uint8_t register_mask = 0;
    uint8_t source_pos = 2;
    for (uint8_t arguments_to_skip = argument; arguments_to_skip != 0;
        arguments_to_skip--)
    {
        for (;;)
        {
            uint8_t ch = invocation[++source_pos];
            if (ch == 0x0d)
                return output_pos;
            parse_register_result_t result =
                parse_register_reference(ch, &register_mask);
            if (result == PARSE_REGISTER_OTHER && ch == ',')
                break;
        }
    }

    do
    {
        uint8_t ch = invocation[++source_pos];
        if (ch == 0x0d)
            break;
        parse_register_result_t result =
            parse_register_reference(ch, &register_mask);
        if (result == PARSE_REGISTER_MARKER)
            continue;
        if (result != PARSE_REGISTER_VALUE && ch == ',')
            break;
        ((uint8_t*)&current_line_buffer)[output_pos++] = ch;
    } while (output_pos < 0x82);
    return output_pos;
}

/**
 * Prepares the next line for printing, expanding numbered macro arguments.
 * Resumes ordinary input when a macro ends and terminates overlong macro lines.
 * @param read_limit destination for ordinary input and current invocation line
 * @param macro_cursor current macro position, advanced past the prepared line
 * @return pointer to the prepared line, or NULL when ordinary input is
 * exhausted
 */
uint8_t* prepare_output_line(uint8_t* read_limit, uint8_t** macro_cursor)
{
    for (;;)
    {
        if (!macro_executing_flag)
        {
            uint8_t* cursor = read_limit;
            if (read_next_output_line(&cursor, read_limit) == READ_BLOCK_DONE)
                return NULL;
            if (read_limit != NULL)
            {
                heap_format_line_ptr = (line_t*)read_limit;
                current_format_line = read_limit;
                return read_limit;
            }
        }

        uint8_t pos = 0;
        uint8_t output_pos = 0;
        for (;;)
        {
            uint8_t ch = (*macro_cursor)[pos];
            if (ch == 4)
            {
                macro_executing_flag = false;
                break;
            }
            pos++;
            if (ch == '@')
            {
                uint8_t argument = (*macro_cursor)[pos];
                if (argument >= '0' && argument <= '9')
                {
                    pos++;
                    output_pos = expand_macro_argument(
                        read_limit, argument - '0', output_pos);
                }
                continue;
            }

            for (;;)
            {
                ((uint8_t*)&current_line_buffer)[output_pos++] = ch;
                if (ch == 0x0d)
                {
                    *macro_cursor += pos;
                    current_format_line = edit_buffer_base;
                    heap_format_line_ptr = (line_t*)edit_buffer_base;
                    return edit_buffer_base;
                }
                if (output_pos < 0x83)
                    break;
                ch = 0x0d;
            }
        }
    }
}

/**
 * Parses a register reference character using the current marker state.
 * @param cur_ch character to parse
 * @param register_mask current mask, set by '<' and cleared by '>'
 * @return whether the character is a marker, register value, or other byte
 */
static parse_register_result_t parse_register_reference(
    int cur_ch, uint8_t* register_mask)
{
    if (cur_ch == '>')
    {
        *register_mask = 0;
        return PARSE_REGISTER_MARKER;
    }
    if (cur_ch == '<')
    {
        *register_mask = 0x40;
        return PARSE_REGISTER_MARKER;
    }
    return (cur_ch & *register_mask) ? PARSE_REGISTER_VALUE
                                     : PARSE_REGISTER_OTHER;
}

/**
 * Reads the next output line from the document or file.
 *
 * @param limit upper bound for reading
 * @param cursor current read cursor, updated on return
 * @return status indicating completion
 */
static read_block_status_t read_next_output_line(
    uint8_t** cursor, uint8_t* limit)
{
    if (!printing_from_file_flag)
        return read_block_from_file(cursor, limit);

    uint8_t a2;
    do
    {
        a2 = *print_source_ptr;
        if (a2 == 0)
            return READ_BLOCK_DONE;
        **cursor = a2;
        print_source_ptr++;
        (*cursor)++;
    } while (a2 != 0x0d);
    return READ_BLOCK_MORE;
}
