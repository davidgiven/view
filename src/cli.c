#include "cli.h"
#include "document.h"
#include "printing.h"
#include "io.h"
#include <stdlib.h>
addr_t parse_mark_from_command(struct scan_state* scan);

// Forward declarations for CLI utilities
void file_error(void);
void file_not_found_error(void);
bool parse_integer_from_command(struct scan_state* scan, int* out);
void parse_marks_from_command(struct scan_state* scan);
void reset_document_name_after_load(void);
void set_document_name_to_filename_buffer(void);
void zero_terminate_filename_buffer(void);

// Forward declarations for static CLI command functions
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

// check_for_at_least_150_bytes_free defined in document.c

static void bye_cmd(void)
{
    // Pseudocode: Exits the program via BDOS exit system call

    // ;
    // ***************************************************************************************
    // zproc bye_cmd
    //     ldy #BDOS_EXIT_PROGRAM
    //     jmp BDOS
    exit(0);
}

void execute_cli_command(uint8_t a, struct scan_state* scan)
{
    // execute_cli_command
    // call_through_jumptable (y=2):
    //     asl
    //     clc
    //     adc jumptable_ptrs,y
    //     sta ((uint8_t*)&tmp89)[0]
    //     lda #0
    //     adc jumptable_ptrs+1,y
    //     sta ((uint8_t*)&tmp89)[1]
    //     ldy #0
    //     lda (((uint8_t*)&tmp89)[0]),y
    //     sta ((uint8_t*)&tmp67)[0]
    //     iny
    //     lda (((uint8_t*)&tmp89)[0]),y
    //     sta ((uint8_t*)&tmp67)[1]
    //     jmp (((uint8_t*)&tmp67)[0])
    switch (a)
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

static void change_cmd(struct scan_state* scan)
{

    // change_cmd
    // Pseudocode: Replaces all occurrences of search string in document area,
    // reports change count

    // change_cmd:
    //     jsr sub_c83f0
    cli_cmd_status_t st = process_cli_command(scan);
    //     bcs c82fa
    if (st == CLI_CMD_NO_STRING)
    {
        cmd_err_no_string();
        return;
    }
    //     beq c82e7
    if (st == CLI_CMD_NO_TARGET)
    {
        cmd_err_no_target();
        return;
    }
    //     jsr c8b7b
    if (!scan_document_for_next_line())
    {
        cmd_err_no_string();
        return;
    }
    //     bne c82fa
    //     ldx #0
    //     stx ptr3
    //     stx ptr3+1

    int ptr3 = 0;
    // loop_c82b3:
    for (;;)
    {
        //     inc ptr3
        //     bne c82b9
        //     inc ptr3+1
        // c82b9:
        ptr3++;
        //     jsr move_cursor_to_address
        move_cursor_to_address(ptr2);
        //     lda #0
        //     sta print_xpos
        print_xpos = 0;
        //     jsr sub_c8a4f
        //     bcs c830d (C=1 conveyed as a true return)
        if (check_area_memory(ptr2))
            goto c830d;
        //     jsr c8b7b
        if (scan_document_for_next_line())
            continue;
        //     beq loop_c82b3
        break;
    }
    //     ldx ptr3
    render_number_to_screen(ptr3);
    //     jsr print_inline_string
    //     .ascii " string(s) changed"
    //     .byte 0xff
    cli_putstring(" string(s) changed\n");
    return_to_cli_prompt();
    return;

    // c830d:
c830d:
    //     jmp display_not_enough_memory
    display_not_enough_memory();
}

void clear_cmd(void)
{
    // clear_cmd
    // Pseudocode: Clears all markers (sets to zero)

    // ;
    // ***************************************************************************************
    // clear_cmd:
    //     ldx #0x0b

    uint8_t x = 0x0b;
    //     lda #0
    // loop_cb095:
    do
    {
        ((uint8_t*)markers_array)[x] = 0;
        x--;
    } while (!(x & 0x80));
    //     rts
    return;
}

static void close_input_output_files(void)
{
    // close_input_output_files
    // Pseudocode: Closes output file, resets editing flags, returns to CLI

    // close_input_output_files:
    //     lda #0
    //     sta input_file_empty_flag
    input_file_empty_flag = 0;
    //     sta file_edit_flags
    file_edit_flags = 0;

    //     jsr select_file
    // (inlined: file_ptr = output_fp)
    file_ptr = output_fp;
    //     jsr close_file
    close_file();
    //     jmp return_to_cli_prompt
    return_to_cli_prompt();
    return;

    // MULTIPLE ENTRY POINTS: quit_cmd, close_input_output_files
}

static void cmd_err_no_string(void)
{
    // c82fa - shared error handler for CLI commands
    // c82fa:
    //     jsr print_inline_string
    //     .ascii "No string found"
    //     .byte 0xff
    //     rts
    cli_putstring("No string found\n");
    return_to_cli_prompt();
    return;
}

static void cmd_err_no_target(void)
{
    // c82e7 - shared error handler for CLI commands
    // c82e7:
    //     jsr print_inline_string
    //     .ascii "No target given"
    //     .byte 0xff
    //     rts
    cli_putstring("No target given\n");
    return_to_cli_prompt();
    return;
}

static void count_cmd(struct scan_state* scan)
{

    // count_cmd
    // Pseudocode: Counts words in document area handling command prefixes and
    // punctuation
    static const uint8_t l8747_data[] = {0x52, 0x4a, 'C', 'E', 'L', 'J', 0};

    // ;
    // ***************************************************************************************
    // count_cmd:
    //     jsr parse_marks_from_command
    parse_marks_from_command(scan);
    //     jsr sanitise_area
    if (sanitise_area() == AREA_EMPTY)
    {
        return_to_cli_prompt();
        return;
    }
    //     lda area_start_ptr
    //     sta ((uint8_t*)&tmp01)[0]
    //     lda area_start_ptr+1
    //     sta ((uint8_t*)&tmp01)[1]

    addr_t tmp01 = area_start_ptr;
    //     lda #0
    //     sta ((uint8_t*)&tmp89)[0]
    //     sta ((uint8_t*)&tmp89)[1]

    int tmp89 = 0;
    //     sta l0083
    //     sta l0082
    l0083 = 0;
    l0082 = 0;
    // c86b8:
c86b8:
    //     ldy #0

    uint8_t y = 0;
    //     jsr deref_and_check_for_command_prefix
    command_prefix_t cp = deref_and_check_for_command_prefix(y, tmp01);
    //     bne c86ea
    if (cp == NO_COMMAND_PREFIX)
        goto c86ea;
    //     ldx #0

    uint8_t x = 0;
    //     iny
    y++;
    // loop_c86c2:
    do
    {
        //     lda (((uint8_t*)&tmp01)[0]),y
        uint8_t a = ram[tmp01 + y];
        //     iny
        y++;
        //     cmp l8747,x
        if (a != l8747_data[x])
            goto c86d1;
        //     lda (((uint8_t*)&tmp01)[0]),y
        uint8_t a_1 = ram[tmp01 + y];
        //     cmp l8748,x
        if (a_1 == l8747_data[x + 1])
            goto c86df;
        // c86d1:
    c86d1:
        //     lda l8749,x
        uint8_t a_2 = l8747_data[x + 2];

        //     beq c86db
        if (a_2 == 0)
            goto c86db;
        //     dey
        y--;
        //     inx
        x++;
        //     inx
        x++;
        //     bne loop_c86c2
    } while (x != 0);
    // c86db:
c86db:
    //     lda #0x80
    uint8_t a_3 = 0x80;
    //     bne c86ff                                                         ;
    //     ALWAYS branch
    goto c86ff;

    // c86df:
c86df:
    //     lda ((uint8_t*)&tmp01)[0]
    //     clc
    //     adc #3
    //     sta ((uint8_t*)&tmp01)[0]
    //     bcs c871d
    //     bcc c871f                                                         ;
    //     ALWAYS branch
    // (16-bit arithmetic: tmp01 += 3)
    tmp01 += 3;
    goto c871f;

    // c86ea:
c86ea:
    //     ldy #0
    uint8_t y_1 = 0;
    //     jsr process_current_document_character
    bool is_tab = false;
    a_3 = process_current_document_character(tmp01, &x, &y_1, &is_tab);
    //     and #0x7f
    a_3 &= 0x7f;
    //     ldx #0
    x = 0;
    //     ldy l0082
    if ((int8_t)l0082 < 0)
        goto c870d;
    //     cmp #0x0d
    //     beq c8703
    //     cmp #0x20 ; ' '
    if (a_3 == 0x0d || a_3 == 0x20)
        goto c8703;
    //     beq c8703
    // c86ff:
c86ff:
    //     inc l0083
    l0083++;
    if (l0083 != 0)
        goto c8715;
    //     bne c8715
    // c8703:
c8703:
    //     ldy l0083
    //     beq c870d
    if (l0083 != 0)
    {
        tmp89++;
    }
c870d:
    //     stx l0083
    l0083 = x;
    //     cmp #0x0d
    if (a_3 == 0x0d)
    {
        l0082 = x;
    }
c8715:
    //     ora l0082
    a_3 |= l0082;
    //     sta l0082
    l0082 = a_3;
    tmp01++;
    // c871f:
c871f:
    //     ldy ((uint8_t*)&tmp01)[1]
    //     cpy area_end_ptr+1
    //     ldy ((uint8_t*)&tmp01)[0]
    //     cpy area_end_ptr
    // (16-bit equality consolidated)
    if (tmp01 != area_end_ptr)
        goto c86b8;
    //     ldx ((uint8_t*)&tmp89)[0]
    render_number_to_screen(tmp89);
    //     jsr print_inline_string
    //     .ascii " word(s) counted."
    //     .byte 0xff
    cli_putstring(" word(s) counted.\n");
    return_to_cli_prompt();
    return;

    // l8747:
    //     .byte 0x52
    // l8748:
    //     .byte 0x4a
    // l8749:
    //     .ascii "CELJ"
    //     .byte 0
}

static void edit_cmd(struct scan_state* scan)
{
    uint8_t a;

    // edit_cmd
    check_not_continuous_editing();
    parse_filename_from_command(scan);
    set_document_name_to_filename_buffer();
    open_input_file();
    parse_filename_from_command(scan);
    open_output_file();

    uint8_t x = 0;
    input_file_empty_flag = x;
    do
    {
        a = filename_buffer[x];
        if (a == 0)
            a = 0x0d;
        output_filename[x] = a;
        x++;
    } while (a != 0x0d);
    initialise_document();
    if (read_first_chunk_from_input_file())
    {
        close_input_output_files();
        return_to_cli_prompt();
        return;
    }
    file_edit_flags = 1;
}

static void field_cmd(struct scan_state* scan)
{
    // field_cmd
    // Pseudocode: Sets the tab key field width from parsed integer argument

    // ;
    // ***************************************************************************************
    // field_cmd:
    //     jsr parse_integer_from_command
    //     beq c869b

    int value;
    bool ok = parse_integer_from_command(scan, &value);
    if (!ok)
    {
        return_to_cli_prompt();
        return;
    }
    //     lda ((uint8_t*)&tmp89)[0]
    uint8_t a = value & 0xFF;
    //     cmp #0x1b
    if (a == 0x1b)
    {
        cli_putstring("Frump!\n");
        return_to_cli_prompt();
        return;
    }
    //     sta current_tab_key
    current_tab_key = a;
    // c869b:
    //     jmp return_to_cli_prompt
    return_to_cli_prompt();
    return;
}

static void finish_cmd(void)
{

    // finish_cmd
    // Pseudocode: Writes remaining document content to output file in chunks

    // ;
    // ***************************************************************************************
    // finish_cmd:
    //     jsr check_continuous_editing
    check_continuous_editing();
    // loop_c84ee:
    while (1)
    {
        reset_area_to_entire_document();
        sanitise_area();
        //     jsr select_file
        // (inlined: file_ptr = output_fp)
        file_ptr = output_fp;
        write_area_to_file();
        //     jsr put_byte_to_file
        // (inlined: fputc(0, file_ptr))
        fputc(0, file_ptr);
        adjust_area_pointers(tmp67);
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

static void fold_cmd(struct scan_state* scan)
{

    // fold_cmd
    // Pseudocode: Toggles folding on/off and displays current folding status

    // ;
    // ***************************************************************************************
    // fold_cmd:
    //     jsr sub_c8e33
    //     beq c87b4
    if (scan_input_buffer(input_buffer, scan))
        goto c87b4;
    //     lda input_buffer,y

    uint8_t a = input_buffer[scan->pos];
    //     cmp #'1'
    //     beq c87b2 (true → folding_flag = 0)
    if (a == '1')
    {
        folding_flag = 0;
        goto c87b4;
    }
    //     cmp #'0'
    if (a == '0')
    {
        folding_flag = 0x80;
    }
c87b4:
    //     jsr print_inline_string
    //     .ascii "Folding "
    //     .byte 0
    cli_putstring("Folding ");

    //     lda folding_flag
    if (((int8_t)folding_flag < 0))
    {
        cli_putstring("off\n");
        return_to_cli_prompt();
        return;
    }
    //     jsr print_inline_string
    //     .ascii "on"
    //     .byte 0xff
    cli_putstring("on\n");
    return_to_cli_prompt();
    return;

    // c87d1:
    //     jsr print_inline_string
    //     .ascii "Bad file"
    //     .byte 0xff
    cli_putstring("Bad file\n");
    return_to_cli_prompt();
    return;
}

static void format_cmd(struct scan_state* scan)
{
    // Pseudocode: Formats document area by running line-by-line through
    // formatting pipeline

    // ;
    // ***************************************************************************************
    // format_cmd:
    //     jsr parse_marks_from_command
    parse_marks_from_command(scan);
    //     jsr sanitise_area
    if (sanitise_area() == AREA_EMPTY)
        goto c878b;
    //     lda area_start_ptr
    //     jsr move_cursor_to_address
    move_cursor_to_address(area_start_ptr);
    //     jsr sub_caf5f
    clear_format_mode_bit7();
    //     lda #0x10
    //     jsr wipe_buffer
    wipe_buffer(0x10, ptr1);
    //     lda current_edit_line_ptr
    //     sta current_format_line_ptr
    //     lda current_edit_line_ptr+1
    //     sta current_format_line_ptr+1
    current_format_line_ptr = RAM_EDIT_BUFFER;
    // c876d:
c876d:
    //     jsr sub_c9977
    format_result_t fr = format_paragraph();
    //     bvs c8791 (V=1 conveyed as FORMAT_MEMORY_FULL)
    if (fr == FORMAT_MEMORY_FULL)
        goto c8791;
    //     bcs c8787 (C=1 conveyed as FORMAT_AT_END)
    if (fr == FORMAT_AT_END)
        goto c8787;
    //     lda #0x2e ; '.'
    //     jsr bdos_print_char
    cli_putchar(0x2e);
    //     lda current_line_ptr
    //     ldy current_line_ptr+1
    //     cpy area_end_ptr+1
    // (16-bit comparison consolidated)
    if (current_line_ptr < area_end_ptr)
        goto c876d;
    // c8787:
c8787:
    //     lda #0xff
    //     lda #0xff
    //     sta l0012
    top_of_screen_line_ptr = RAM_MAX;
    // c878b:
c878b:
    //     jsr bdos_print_newline
    cli_putchar('\n');
    //     jmp return_to_cli_prompt
    return_to_cli_prompt();
    return;

    // c8791:
c8791:
    //     jsr bdos_print_newline
    cli_putchar('\n');
    //     jmp display_not_enough_memory
    display_not_enough_memory();
    return;
}

static void load_cmd(struct scan_state* scan)
{
    // load_cmd
    // load_cmd:
    //     jsr check_not_continuous_editing
    check_not_continuous_editing();
    //     jsr parse_filename_from_command
    parse_filename_from_command(scan);
    //     jsr initialise_document
    initialise_document();
    top = page; // WORKAROUND: ensure_cr_at_document_top bumped top past the
                // initial CR; need to load at page, not page+1
    //     jsr reset_area_to_entire_document
    reset_area_to_entire_document();
    //     jsr 1f
    // (returns the post-read cursor, which the 6502 left in tmp01)
    top = read_into_document();
    //     jsr reset_document_name_after_load
    reset_document_name_after_load();
    //     jsr clear_cmd
    clear_cmd();
    //     jmp move_cursor_to_top_of_document
    move_cursor_to_top_of_document();
    return;
}

static void microspace_cmd(struct scan_state* scan)
{

    // microspace_cmd
    // Pseudocode: Configures microspacing by querying printer driver

    // ;
    // ***************************************************************************************
    // microspace_cmd:
    //     jsr prepare_printer_driver
    prepare_printer_driver();
    //     jsr parse_integer_from_command
    //     php

    int value;
    bool parsed = parse_integer_from_command(scan, &value);
    //     ldx #0x0a
    uint8_t x = 0x0a;
    //     beq c8608
    if (parsed)
    {
        x = value & 0xFF;
        if (x == 0)
            return;
    }
    //     ldy #0
    //     lda #0x0c
    //     jsr call_printer_driver
    // (the printer driver returns its status in the global y register;
    //  the y = 0 input setup is unused by the default driver)

    uint8_t y;
    printer_driver_ptr->printer_getflags(&x, &y);
    //     tya

    uint8_t a = y;
    //     and #1
    a &= 1;

    //     beq c8617
    if (a != 0)
    {
        microspacing_flag = x;
        return;
    }
    //     jsr print_inline_string
    //     .ascii "Driver does not support microspacing"
    //     .byte 0xff
    cli_putstring("Driver does not support microspacing\n");
    return_to_cli_prompt();
    return;
}

static void mode_cmd(void)
{
    // ;
    // ***************************************************************************************
    // mode_cmd:
    //     jsr print_inline_string
    //     .ascii "Bad mode"
    //     .byte 0xff
    cli_putstring("Bad mode\n");
    return_to_cli_prompt();
    return;
}

static void more_cmd(struct scan_state* scan)
{

    // more_cmd
    // Pseudocode: Appends more text from input file into document at current
    // cursor position

    // ;
    // ***************************************************************************************
    // more_cmd:
    //     jsr check_continuous_editing
    check_continuous_editing();
    //     jsr parse_marks_from_command
    parse_marks_from_command(scan);
    //     jsr move_cursor_to_address
    move_cursor_to_address(area_start_ptr);

    //     jsr select_file
    // (inlined: file_ptr = output_fp)
    file_ptr = output_fp;
    //     jsr write_area_to_file
    write_area_to_file();
    //     ldy #0
    //     ldx l003a
    // loop_c84c4:
    uint8_t y = 0;

    uint8_t x = l003a;
    do
    {
        uint8_t a = ram[current_ruler_ptr + y];
        current_ruler_buffer[y] = a;
        y++;
        x--;
    } while (x != 0);
    //     lda #0x0d
    //     sta current_ruler_buffer,y
    current_ruler_buffer[y] = 0x0d;
    //     jsr sub_c89d3
    adjust_area_pointers(tmp67);
    //     jsr move_cursor_to_top_of_document
    move_cursor_to_top_of_document();
    //     jsr check_for_at_least_150_bytes_free
    check_for_at_least_150_bytes_free();
    //     lda input_file_empty_flag
    //     bne c84e8
    if (input_file_empty_flag == 0)
    {
        if (read_next_chunk_from_input_file(top))
        {
            return_to_cli_prompt();
            return;
        }
    }
    //     jmp cb05a
    ensure_cr_at_document_top();
}

static void name_cmd(struct scan_state* scan)
{
    // name_cmd
    // Pseudocode: Sets document name from optional filename argument

    // ;
    // ***************************************************************************************
    // name_cmd:
    //     jsr check_not_continuous_editing
    check_not_continuous_editing();
    //     jsr parse_optional_filename_from_command
    bool has_filename = parse_optional_filename_from_command(scan);
    //     php
    //     lda #0
    //     sta file_edit_flags
    //     plp
    file_edit_flags = 0;
    //     beq return_9
    if (!has_filename)
        return;

    // MULTIPLE ENTRY POINTS: name_cmd, reset_document_name_after_load
    reset_document_name_after_load();
}

static void new_cmd(void)
{
    // Pseudocode: Creates a new empty document after checking continuous
    // editing state

    // ;
    // ***************************************************************************************
    // new_cmd:
    //     jsr check_not_continuous_editing
    check_not_continuous_editing();
    //     jmp initialise_document
    initialise_document();
    return;
}

static void print_cmd(struct scan_state* scan)
{
    // Pseudocode: Sets print flags and falls through to print_to_screen

    // ;
    // ***************************************************************************************
    // print_cmd:
    //     lda #0x80
    //     jsr start_printing
    // ;
    // ***************************************************************************************
    start_printing();
    // MULTIPLE ENTRY POINTS: print_cmd, print_to_screen
    print_to_screen(scan);
}

static void print_to_screen(struct scan_state* scan)
{
    // print_to_screen: Prints document for screen preview, returns to CLI when
    // done

    //     jsr print_document
    print_document(scan);
    //     jmp return_to_cli_prompt
    return_to_cli_prompt();
    return;
}

static void printer_cmd(struct scan_state* scan)
{
    // Pseudocode: Redirects to print_cmd (printer driver loading code is
    // disabled with #if 0)

    // ;
    // ***************************************************************************************
    // printer_cmd:
    //     jmp print_cmd
    print_cmd(scan);
    return;
    // #if 0
    //     // TODO: implement loading printer drivers.
    //     jsr parse_optional_filename_from_command
    //     beq c882f
    //     ;jsr sub_c8849
    //     beq c87d1
    //     lda l050c
    //     ora l050d
    //     bne c87d1
    //     ldy l050b
    //     dey
    //     bmi c8801
    //     bne c87d1
    //     lda l050a
    //     bne c87d1
    // c8801:
    //     lda #<printer_driver_block
    //     sta l0502
    //     lda #>printer_driver_block
    //     sta l0503
    //     lda #osbyte_read_high_order_address
    //     jsr osbyte                                                        ;
    //     Read the filing system 'machine high order address' stx l0504 ; X and
    //     Y contain the machine high order address (low, high) sty l0505 lda #0
    //     sta l0506
    //     lda #0xff
    //     jsr do_osfile_with_buffer
    //     ldx #0
    // loop_c8822:
    //     lda filename_buffer,x
    //     sta printer_driver_name,x
    //     inx
    //     cmp #0x0d
    //     bne loop_c8822
    //     beq c8834                                                         ;
    //     ALWAYS branch

    // c882f:
    //     lda #0
    //     sta printer_driver_name
    // c8834:
    //     lda #0
    //     sta microspacing_flag
    // #endif
    // return_8:
    //     rts
}

static void quit_cmd(void)
{
    // Pseudocode: Checks continuous editing then falls through to close files

    // ;
    // ***************************************************************************************
    // quit_cmd:
    //     jsr check_continuous_editing
    check_continuous_editing();
    // MULTIPLE ENTRY POINTS: quit_cmd, close_input_output_files
    close_input_output_files();
}

static void read_cmd(struct scan_state* scan)
{
    // read_cmd:
    //     jsr parse_filename_from_command
    parse_filename_from_command(scan);
    //     jsr parse_marks_from_command
    parse_marks_from_command(scan);
    // 1:
    read_into_document();
    //     jmp return_to_cli_prompt
    return_to_cli_prompt();
    return;
}

static void replace_cmd(struct scan_state* scan)
{

    // replace_cmd
    // Pseudocode: Interactive search and replace prompting for each match
    // (Y)es/(O)K/(N)o

    // ;
    // ***************************************************************************************
    // replace_cmd:
    //     jsr sub_c83f0
    cli_cmd_status_t st = process_cli_command(scan);
    //     beq c82e7
    if (st != CLI_CMD_OK)
    {
        cmd_err_no_target();
        return;
    }
    //     jsr c8b7b
    if (!scan_document_for_next_line())
    {
        cmd_err_no_string();
        return;
    }
    //     bne c82fa
    //     jsr move_cursor_to_address
    move_cursor_to_address(ptr2);
    //     jsr enter_editor_mode
    enter_editor_mode();
    // c832d:
c832d:
    //     jsr sub_c8361
    redraw_and_write_back();
    //     ldx #0x52 ; 'R'
    //     ldy #0x50 ; 'P'
    //     jsr draw_prompt_characters
    draw_prompt_characters('R', 'P');
    //     jsr flush_and_read_char

    uint8_t a = screen_getchar();
    //     bcs return_2
    if (a == 0x1b)
        return;
    //     and #0xdf
    a &= 0xdf;
    //     ldx #0
    uint8_t x = 0;
    //     cmp #0x59 ; 'Y'
    if (a == 0x59)
        goto c8349;
    //     beq c8349
    //     dex                                                               ;
    //     X=0xff
    x--;
    //     cmp #0x4f ; 'O'
    if (a != 0x4f)
        goto c8356;
    //     bne c8356
    // c8349:
c8349:
    //     stx print_xpos
    print_xpos = x;
    //     jsr sub_c8371
    setup_area_pointers(ptr2);
    //     jsr sub_c8a4f
    //     bcs c836b (C=1 conveyed as a true return)
    if (check_area_memory(ptr2))
    {
        show_memory_full_error();
        esc_key();
        return;
    }
    //     jsr sub_c8361
    redraw_and_write_back();
    // c8356:
c8356:
    //     jsr c8b7b
    if (!scan_document_for_next_line())
        return;
    //     bne return_2
    //     jsr move_cursor_to_address
    move_cursor_to_address(ptr2);
    //     jmp c832d
    goto c832d;
}

static void save_cmd_write_cmd(struct scan_state* scan)
{
    // save_cmd_write_cmd
    // Pseudocode: Saves document area to output file with optional filename

    // ;
    // ***************************************************************************************
    // save_cmd:
    // write_cmd:
    //     jsr parse_optional_filename_from_command
    //     zif eq
    if (!parse_optional_filename_from_command(scan))
    {
        uint8_t ch;

        //         bit file_edit_flags
        // (the A value only affects BIT's Z flag, which is not checked here;
        //  V comes from file_edit_flags, so a constant is passed)
        //         zif vc
        if (!(file_edit_flags & 0x40))
        {
            //             jmp bad_filename_error
            bad_filename_error();
            return;
            //         zendif
        }

        //         ldx #0
        uint8_t x = 0;

        //         zrepeat
        do
        {
            //             lda input_filename,x
            ch = input_filename[x];
            //             sta filename_buffer,x
            filename_buffer[x] = ch;
            //             inx
            x++;
            //             cmp #0x0d
            //         zuntil eq
        } while (ch != 0x0d);
        //     zendif
    }
    //     jsr parse_marks_from_command
    parse_marks_from_command(scan);
    //     jsr sanitise_area
    if (sanitise_area() == AREA_EMPTY)
        return;

    //     jsr open_output_file
    open_output_file();
    //     jsr write_area_to_file
    write_area_to_file();
    //     lda #0
    //     jsr put_byte_to_file
    // (inlined: fputc(0, file_ptr))
    fputc(0, file_ptr);

    //     jsr close_file
    close_file();
    //     jmp return_to_cli_prompt
    return_to_cli_prompt();
    return;

    // MULTIPLE ENTRY POINTS: save_cmd, write_cmd
}

static void screen_cmd(struct scan_state* scan)
{
    // Pseudocode: Jumps to print_to_screen for on-screen document preview

    // ;
    // ***************************************************************************************
    // screen_cmd:
    //     jmp print_to_screen
    print_to_screen(scan);
    return;
}

static void search_cmd(struct scan_state* scan)
{
    // search_cmd
    // Pseudocode: Searches for target string, reports position if found

    // ;
    // ***************************************************************************************
    // search_cmd:
    //     jsr sub_c8412
    //     beq c82e7
    if (reset_command_parse_state(scan))
    {
        cmd_err_no_target();
        return;
    }
    //     jsr parse_marks_from_command
    parse_marks_from_command(scan);
    //     jsr sanitise_area
    if (sanitise_area() == AREA_EMPTY)
    {
        cmd_err_no_string();
        return;
    }
    //     jsr sub_c8c7c
    // (inlined: doc_ptr2 = area_start_ptr; doc_ptr3 = area_end_ptr)
    doc_ptr2 = area_start_ptr;
    doc_ptr3 = area_end_ptr;
    //     jsr c8b7b
    if (!scan_document_for_next_line())
    {
        cmd_err_no_string();
        return;
    }
    //     bne c82fa
    //     jsr move_cursor_to_address
    move_cursor_to_address(ptr2);
    //     jmp enter_editor_mode
    enter_editor_mode();
    longjmp(env, JMP_EDITOR);
    return;

    // ;
    // ***************************************************************************************
}

static void setup_cmd(struct scan_state* scan)
{

    // setup_cmd
    // Pseudocode: Parses flag letters and sets format_mode_flag,
    // justifying_flag, insert_mode_flag
    // ;
    // ***************************************************************************************
    // setup_cmd:
    // c867d:
    static const uint8_t c867d_data[] = {0x4e, 0x4a, 0x00, 0x49, 0x00};
    // c8681:
    static const uint8_t c8681_data[] = {0x00, 0x00, 0xff};
    //     ldx #1

    uint8_t x = 1;
    //     stx tmp6

    uint8_t tmp6 = x;
    //     dex                                                               ;
    //     X=0x00
    x--;
    //     stx tmp8

    uint8_t tmp8 = x;
    //     dex                                                               ;
    //     X=0xff
    x--;
    //     stx tmp7

    uint8_t tmp7 = x;
    // c8649:
c8649:
    //     jsr sub_c8e33
    //     beq c8672
    if (scan_input_buffer(input_buffer, scan))
        goto c8672;
    //     and #0xdf
    scan->ch &= 0xdf;
    //     ldx #0
    uint8_t x_1 = 0;
    // loop_c8652:
    uint8_t y;
    do
    {
        //     cmp c867d,x
        if (scan->ch == c867d_data[x_1])
            goto c8669;
        //     inx
        x_1++;
        //     ldy c867d,x

        y = c867d_data[x_1];
        //     bne loop_c8652
    } while (y != 0);
    //     jsr print_inline_string
    //     .ascii "Bad flag"
    //     .byte 0xff
    cli_putstring("Bad flag\n");
    return_to_cli_prompt();
    return;

    // c8669:
c8669:
    //     lda c8681,x

    uint8_t a = c8681_data[x_1];
    //     sta tmp6,x
    if (x_1 == 0)
        tmp6 = a;
    else if (x_1 == 1)
        tmp7 = a;
    else
        tmp8 = a;
    //     inc input_buffer_offset
    input_buffer_offset++;
    if (input_buffer_offset != 0)
        goto c8649;
    //     bne c8649
    // c8672:
c8672:
    //     ldx #2
    uint8_t x_2 = 2;
    // loop_c8674:
    do
    {
        uint8_t a_1;

        if (x_2 == 0)
            a_1 = tmp6;
        else if (x_2 == 1)
            a_1 = tmp7;
        else
            a_1 = tmp8;
        if (x_2 == 0)
            format_mode_flag = a_1;
        else if (x_2 == 1)
            justifying_flag = a_1;
        else
            insert_mode_flag = a_1;
        x_2--;
    } while (!((int8_t)x_2 < 0));
    //     bpl loop_c8674
    //     bmi c869b                                                         ;
    //     ALWAYS branch
    return_to_cli_prompt();
    return;

    // c867d:
    //     lsr l004a
    //     eor #0
    // c8681:
    //     brk

    //     .byte 0, 0xff
}

static void sheets_cmd(struct scan_state* scan)
{
    // Pseudocode: Prints document to printer then displays newline and returns
    // to CLI

    // ;
    // ***************************************************************************************
    // sheets_cmd:
    //     lda #0xc0
    //     jsr start_printing
    start_printing();
    //     jsr print_document
    print_document(scan);
    //     jsr stop_printing
    stop_printing();
    //     jsr bdos_print_newline
    cli_putchar('\n');
    //     jmp return_to_cli_prompt
    return_to_cli_prompt();
    return;
}

void start_printing(void)
{
    // Pseudocode: Initializes printer driver and starts printing with given
    // flags

    // start_printing:
    //     jsr print_inline_string
    //     .ascii "Sorry, can't print yet\r"
    //     .byte 0
    cli_putstring("Sorry, can't print yet\n");
    //     jmp return_to_cli_prompt
    return_to_cli_prompt();
    return;
}

// C translation of the 6502 "readline" subroutine (view-cpm.S:7939).  Renamed
// from "readline" to avoid colliding with GNU readline's readline(3), whose
// symbol the executable would otherwise interpose over (causing infinite
// recursion in cli_readstring).
// Returns true if the line read was empty (the 6502's carry flag).
bool read_command_line(void)
{
    input_buffer_offset = 0;
    return cli_readstring((char*)input_buffer, MAX_COMMAND_LENGTH);
}

const uint8_t la83d[] = "VIEW\0B3.0 for CP/M-65";

static void print_x_words_of_help(uint8_t x)
{
    // print_x_words_of_help
    // Pseudocode: Prints X words of the help string showing VIEW and version

    // ;
    // ***************************************************************************************
    // print_x_words_of_help:
    //     ldy #0

    uint8_t y = 0;
    // ca82e:
    //     jsr bdos_print_char
    //     iny
    // ca832:
    //     lda la83d,y
    //     bne ca82e
    //     lda #0x20 ; ' '
    //     dex
    //     bpl ca82e
    for (;;)
    {

        uint8_t a = la83d[y];
        if (a == 0)
        {
            a = 0x20;
            x--;
            if ((int8_t)x < 0)
                break;
        }
        cli_putchar(a);
        y++;
    }
    //     rts
    return;
}

static bool parse_command(uint8_t* input_buffer_offset);

void input_line_not_escaped(void)
{
    // input_line_not_escaped
    // input_line_not_escaped: Parses command input and dispatches through CLI
    // jump table

    //     jsr parse_command
    bool failed = parse_command(&input_buffer_offset);
    //     sty input_buffer_offset+1
    // (parse_command leaves the command index in l0082; the 6502 copied it
    //  to Y on exit)
    l0080 = l0082;
    //     bcs c8263
    //     cpy #(jumptable4_cli_end-jumptable4_cli)/2
    //     bcc c826e
    // c8263:
    //     jsr print_inline_string ; .ascii "Mistake\n"
    // c826e:
    //     lda input_buffer_offset+1
    //     ldy #2
    //     jsr call_through_jumptable
    // (branch restructured: Mistake is printed when C is set or index >= 48)
    if (failed || l0082 >= 48)
        cli_putstring("Mistake\n");

    struct scan_state scan;
    execute_cli_command(l0080, &scan);
    //     jmp run_cli
    run_cli();
}

void cli_handler_impl(void)
{
    // cli_handler_impl
    // cli_handler_impl: Main CLI loop (called after setjmp reset)

    //     jsr stop_printing
    stop_printing();
    //     ldx #0xff
    //     txs  (handled by setjmp/longjmp in main_)
    //     inx  ; X=0x00
    //     stx print_flags
    print_flags = 0;
    //     jsr print_inline_string ; .ascii "=>"
    cli_putstring("=>");
    //     jsr readline
    if (!read_command_line())
    {
        input_line_not_escaped();
        return;
    }
    //     lda #<input_buffer
    //     sta ((uint8_t*)&tmp01)[0]
    //     ldx #>input_buffer
    //     stx ((uint8_t*)&tmp01)[1]
    // (((uint8_t*)&tmp01)[0]/((uint8_t*)&tmp01)[1] no longer used as a pointer;
    // parse_command reads input_buffer[] directly)
    //     bcc input_line_not_escaped
    //     jmp run_editor
    run_editor();
}

void run_cli(void)
{

    // run_cli
    screen_leave();
    // run_cli:
    //     jsr clear_screen
    clear_screen();
    //     ldx #1
    //     jsr print_x_words_of_help
    print_x_words_of_help(1);
    //     jsr print_inline_string
    //     .ascii "\r\rBytes free "
    //     .byte 0
    cli_putstring("\n\nBytes free ");

    //     jsr compute_bytes_free
    render_number_to_screen(compute_bytes_free());
    //     jsr bdos_print_newline
    cli_putchar('\n');
    //     jsr display_document_file_state
    display_document_file_state();
    //     bit file_edit_flags
    if ((file_edit_flags & 0x40))
        goto c816d;
    //     lda file_edit_flags

    //     ror
    //     bcc c816d
    if ((file_edit_flags & 1))
    {
        cli_putstring("Input file is ");
        if (input_file_empty_flag == 0)
        {
            cli_putstring("not ");
        }
        cli_putstring("empty\n");
    }
c816d:
    //     lda printer_driver_name
    uint8_t a_2 = printer_driver_name[0];

    //     beq c81b6
    if (a_2 == 0)
        goto c81b6;
    //     jsr print_inline_string
    //     .ascii "Printer "
    //     .byte 0
    cli_putstring("Printer ");

    //     ldx #0

    uint8_t x = 0;
    // loop_c819a:
    //     lda printer_driver_name,x
    //     cmp #0x0d
    //     beq c81a7
    //     jsr bdos_print_char
    //     inx
    //     bne loop_c819a
    do
    {
        uint8_t a_3 = printer_driver_name[x];
        if (a_3 == 0x0d)
            break;
        cli_putchar(a_3);
        x++;
    } while (x != 0);
    // c81a7:
    //     lda microspacing_flag

    //     beq c81b3
    if (microspacing_flag != 0)
    {
        cli_putstring(" (m)");
    }
    //     jsr bdos_print_newline
    cli_putchar('\n');
    // c81b6:
c81b6:
    //     ldx #0
    uint8_t x_1 = 0;
    //     ldy #0

    uint8_t y = 0;
    // c81ba:
c81ba:
    //     lda markers_array+1,x
    uint8_t a_5 = ((uint8_t*)markers_array)[x_1 + 1];

    //     beq c81e7
    if (a_5 == 0)
        goto c81e7;
    //     tya
    //     bne c81db
    if (y != 0)
        goto c81db;
    //     stx l0083
    l0083 = x_1;
    //     jsr print_inline_string
    //     .ascii "Marker(s) set "
    //     .byte 0
    cli_putstring("Marker(s) set ");

    //     ldx l0083
    x_1 = l0083;
    //     ldy #1
    y = 1;
    //     bne c81e0                                                         ;
    //     ALWAYS branch
    goto c81e0;

    // c81db:
c81db:
    //     lda #0x2c ; ','
    //     jsr screen_putchar
    screen_putchar(0x2c);
    // c81e0:
c81e0:
    //     txa
    //     lsr
    //     adc #0x31 ; '1'
    // (x is an even offset into markers_array, so lsr shifts out a 0 and
    //  the carry is 0: a = (x >> 1) + 0x31)
    uint8_t a_6 = (x_1 >> 1) + 0x31;
    //     jsr screen_putchar
    screen_putchar(a_6);
    // c81e7:
c81e7:
    //     inx
    x_1++;
    //     inx
    x_1++;
    //     cpx #0x0c
    //     bne c81ba
    if (x_1 != 0x0c)
        goto c81ba;
    //     tya
    //     beq c81f3
    if (y != 0)
    {
        cli_putchar('\n');
    }
    //     jsr bdos_print_newline
    cli_putchar('\n');
    return_to_cli_prompt();
}

// CLI command parser
static bool parse_command(uint8_t* input_buffer_offset)
{
    uint8_t y;

    // parse_command
    //     .ascii "VIEW"
    //     .byte 0
    //     .ascii "B3.0 for CP/M-65"
    //     .byte 0

    // ;
    // ***************************************************************************************
    // parse_command:
    //     lda #0xff

    uint8_t a = 0xff;
    //     sta l0082
    l0082 = a;
    //     tax                                                               ;
    //     X=0xff

    uint8_t x = a;
    // ca84c:
    for (;;)
    {
        //     ldy input_buffer_offset
        y = *input_buffer_offset;
        //     dey
        y--;
        //     inc l0082
        l0082++;
        // loop_ca851:
        for (;;)
        {
            //     inx
            x++;
            //     iny
            y++;
            //     lda (((uint8_t*)&tmp01)[0]),y
            uint8_t a_1 = input_buffer[y];
            //     and #0xdf
            a_1 &= 0xdf;
            //     sta l0084
            l0084 = a_1;
            //     lda parser_table,x
            uint8_t a_2 = parser_table[x];
            //     beq ca890
            if (a_2 == 0)
                goto ca890;
            //     bmi ca87e
            if (a_2 & 0x80)
                goto ca87e;
            //     eor #0x5b ; '['
            a_2 ^= 0x5b;
            //     sta l0083
            l0083 = a_2;
            //     and #0xdf
            a_2 &= 0xdf;
            //     cmp l0084
            if (a_2 != l0084)
                break;
            //     beq loop_ca851
        }
        // loop_ca86a:
        uint8_t a_3;
        do
        {
            //     inx
            x++;
            //     lda parser_table,x
            a_3 = parser_table[x];
            //     beq ca890
            if (a_3 == 0)
                goto ca890;
            //     bpl loop_ca86a
        } while (!(a_3 & 0x80));
        //     lda l0083
        uint8_t a_4 = l0083;
        //     and #0x20 ; ' '
        a_4 &= 0x20;
        //     beq ca84c
        if (a_4 == 0)
            continue;
        //     lda (((uint8_t*)&tmp01)[0]),y
        uint8_t a_5 = input_buffer[y];
        //     cmp #0x30 ; '0'
        if (a_5 >= 0x30)
            continue;
        //     bcs ca84c
        break;
    }
    // ca87e:
ca87e:
    //     lda (((uint8_t*)&tmp01)[0]),y
    uint8_t a_6 = input_buffer[y];
    //     cmp #0x30 ; '0'
    if (a_6 < 0x30)
    {
        delimiter_char = a_6;
        y++;
    }
    *input_buffer_offset = y;
    //     ldy l0082
    // (the 6502 copied the command index into Y here; callers now read
    //  l0082 directly)
    //     lda parser_table,x
    //     clc
    //     rts
    return false;

    // ca890:
ca890:
    //     sec
    //     rts
    return true;
}

// CLI utility functions
void file_error(void)
{
    // Pseudocode: Displays File error and returns to CLI

    // ;
    // ***************************************************************************************
    // zproc file_error
    //     jsr print_inline_string
    //     .ascii "File error"
    //     .byte 0
    cli_putstring("File error");
    //     jmp return_to_cli_prompt
    return_to_cli_prompt();
    return;
    // zendproc
}

void file_not_found_error(void)
{
    // Pseudocode: Displays File not found error and returns to CLI

    // ;
    // ***************************************************************************************
    // file_not_found_error:
    //     jsr stop_printing
    stop_printing();
    //     jsr print_inline_string
    //     .ascii "File not found\r"
    //     .byte 0
    cli_putstring("File not found\n");
    //     jmp return_to_cli_prompt
    return_to_cli_prompt();
    return;
}

bool parse_integer_from_command(struct scan_state* scan, int* out)
{
    // Pseudocode: Parses a decimal integer from the command input buffer

    // ;
    // ***************************************************************************************
    // parse_integer_from_command:
    //     lda #<(input_buffer)
    //     sta current_format_line_ptr
    //     lda #>(input_buffer)
    //     sta current_format_line_ptr+1
    //     jsr sub_c8e33
    //     beq return_8
    if (scan_input_buffer(input_buffer, scan))
        return false;
    //     jmp ca6fe
    uint8_t y = scan->pos;

    const char* start = (const char*)&input_buffer[y];
    char* end;
    int parsed = (int)strtoul(start, &end, 10);
    bool ok = (end != start);
    // y is advanced locally as in ca6fe; callers do not read it back
    (void)y;
    if (out)
        *out = parsed;
    // (6502 returns Z set when no integer was parsed; the boolean mirrors
    //  that: true = an integer was parsed)
    return ok;
}

void parse_marks_from_command(struct scan_state* scan)
{
    // parse_marks_from_command:
    //     jsr reset_area_to_entire_document
    reset_area_to_entire_document();
    //     jsr parse_mark_from_command
    addr_t start_mark = parse_mark_from_command(scan);
    //     beq return_11
    if (start_mark == 0)
        return;
    //     sta area_start_ptr
    area_start_ptr = start_mark;
    //     sty area_start_ptr+1
    //     jsr parse_mark_from_command
    addr_t end_mark = parse_mark_from_command(scan);
    //     beq return_11
    if (end_mark == 0)
        return;
    //     sta area_end_ptr
    area_end_ptr = end_mark;
    //     sty area_end_ptr+1
    // return_11:
    //     rts
}

void reset_document_name_after_load(void)
{
    // Pseudocode: Sets file_edit_flags to indicate a document is loaded

    // reset_document_name_after_load:
    //     lda #0x40 ; '@'
    //     sta file_edit_flags
    file_edit_flags = 0x40;
    // fall through to set_document_name_to_filename_buffer
    set_document_name_to_filename_buffer();

    // MULTIPLE ENTRY POINTS: name_cmd, reset_document_name_after_load
}

void set_document_name_to_filename_buffer(void)
{
    uint8_t a;

    // set_document_name_to_filename_buffer
    // Pseudocode: Copies filename buffer to input filename buffer

    // set_document_name_to_filename_buffer:
    //     ldx #0

    uint8_t x = 0;
    // loop_c88fa:
    do
    {
        a = filename_buffer[x];
        input_filename[x] = a;
        x++;
    } while (a >= 0x21);
    //     bge loop_c88fa
    // return_9:
    //     lda #0x0d

    //     sta input_filename-1, x
    input_filename[x - 1] = 0x0d;
    //     rts
    return;

    // MULTIPLE ENTRY POINTS: also called directly from edit_cmd
}

void zero_terminate_filename_buffer(void)
{
    // zero_terminate_filename_buffer:
    //     ldx #0
    //     lda #0x0d
    // zloop:
    //     cmp filename_buffer, x
    //     inx
    //     bne zloop
    //     lda #0
    //     sta filename_buffer, x
    //     rts
    uint8_t x = 0;
    while (filename_buffer[x] != 0x0d)
        x++;
    filename_buffer[x] = 0;
    return;
}

addr_t parse_mark_from_command(struct scan_state* scan)
{
    // parse_mark_from_command
    // parse_mark_from_command:
    //     jsr sub_c8e33
    //     beq return_12
    if (scan_input_buffer(input_buffer, scan))
        return 0;
    //     iny
    scan->pos++;
    //     sty input_buffer_offset
    input_buffer_offset = scan->pos;
    //     jsr lookup_marker
    int marker_index = lookup_marker(scan->ch);
    //     bcs c89b3 / c89b3: jsr print_inline_string ; .ascii "Bad marker" ;
    //     .byte 0xff
    if (marker_index == MARKER_INVALID)
    {
        cli_putstring("Bad marker\n");
        return_to_cli_prompt();
        return 0;
    }
    //     beq c89c1 / c89c1: jsr print_inline_string ; .ascii "Marker not set"
    //     ; .byte 0xff
    if (markers_array[marker_index] == 0)
    {
        cli_putstring("Marker not set\n");
        return_to_cli_prompt();
        return 0;
    }
    //     lda markers_array,x
    //     ldy markers_array+1,x
    // (the 6502 returned the address in YA; the C returns it directly)
    // return_12:
    //     rts
    return markers_array[marker_index];
}
