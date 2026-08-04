#include "cli.h"
#include "document.h"
#include "printing.h"
#include "io.h"
#include <stdlib.h>
addr_t parse_mark_from_command(uint8_t x);

// Forward declarations for CLI utilities
void file_error(void);
void file_not_found_error(void);
void parse_integer_from_command(void);
void parse_marks_from_command(void);
void reset_document_name_after_load(void);
void set_document_name_to_filename_buffer(void);
void zero_terminate_filename_buffer(void);

// Forward declarations for static CLI command functions
static void bye_cmd(void);
static void cmd_err_no_target(void);
static void cmd_err_no_string(void);
static void search_cmd(void);
static void change_cmd(void);
static void replace_cmd(addr_t ptr6);
static void screen_cmd(void);
static void sheets_cmd(void);
static void print_cmd(void);
static void print_to_screen(void);
static void edit_cmd(void);
static void more_cmd(void);
static void finish_cmd(void);
static void quit_cmd(void);
static void close_input_output_files(void);
static void save_cmd_write_cmd(void);
static uint8_t load_cmd(void);
static void read_cmd(void);
static void mode_cmd(void);
static void microspace_cmd(void);
static void setup_cmd(void);
static void field_cmd(void);
static void count_cmd(void);
static void format_cmd(void);
static uint8_t new_cmd(void);
static void fold_cmd(void);
static void printer_cmd(void);
static void name_cmd(void);

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

uint8_t execute_cli_command(uint8_t a)
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
            x = new_cmd();
            break;
        case 2:
            format_cmd();
            break;
        case 3:
            setup_cmd();
            break;
        case 4:
            read_cmd();
            break;
        case 5:
            more_cmd();
            break;
        case 6:
            screen_cmd();
            break;
        case 7:
            sheets_cmd();
            break;
        case 8:
            save_cmd_write_cmd();
            break;
        case 9:
            count_cmd();
            break;
        case 10:
            field_cmd();
            break;
        case 11:
            printer_cmd();
            break;
        case 12:
            search_cmd();
            break;
        case 13:
            clear_cmd();
            break;
        case 14:
            microspace_cmd();
            break;
        case 15:
            fold_cmd();
            break;
        case 16:
            name_cmd();
            break;
        case 17:
            mode_cmd();
            break;
        case 18:
            finish_cmd();
            break;
        case 19:
            print_cmd();
            break;
        case 20:
            change_cmd();
            break;
        case 21:
            save_cmd_write_cmd();
            break;
        case 22:
            edit_cmd();
            break;
        case 23:
            replace_cmd(ptr6);
            break;
        case 24:
            a = load_cmd();
            break;
        case 25:
            bye_cmd();
            break;
    }
    return a;
}

static void change_cmd(void)
{
    // change_cmd
    // Pseudocode: Replaces all occurrences of search string in document area,
    // reports change count

    // change_cmd:
    //     jsr sub_c83f0
    sub_c83f0();
    //     bcs c82fa
    if (flags & FLAG_C)
    {
        cmd_err_no_string();
        return;
    }
    //     beq c82e7
    if (flags & FLAG_Z)
    {
        cmd_err_no_target();
        return;
    }
    //     jsr c8b7b
    c8b7b();
    //     bne c82fa
    if (!(flags & FLAG_Z))
    {
        cmd_err_no_string();
        return;
    }
    //     ldx #0
    x = 0;
    //     stx ptr3
    ptr3 = (ptr3 & 0xff00) | x;
    //     stx ptr3+1
    ptr3 = (ptr3 & 0x00ff) | ((uint16_t)x << 8);
    // loop_c82b3:
loop_c82b3:
    //     inc ptr3
    //     bne c82b9
    //     inc ptr3+1
    // c82b9:
    ptr3++;
    //     jsr move_cursor_to_address
    move_cursor_to_address((uint16_t)(y) << 8 | a);
    a = 0;
    print_xpos = a;
    //     jsr sub_c8a4f
    sub_c8a4f(ptr2);
    //     bcs c830d
    if (flags & FLAG_C)
        goto c830d;
    //     jsr c8b7b
    c8b7b();
    //     beq loop_c82b3
    if (flags & FLAG_Z)
        goto loop_c82b3;
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
    uint8_t x;
    x = 0x0b;
    //     lda #0
    uint8_t a = 0;
    // loop_cb095:
    do
    {
        ((uint8_t*)markers_array)[x] = a;
        x--;
    } while (!(x & 0x80));
loop_cb095:
    //     rts
    return;
}

static void close_input_output_files(void)
{
    // close_input_output_files
    // Pseudocode: Closes output file, resets editing flags, returns to CLI

    // close_input_output_files:
    //     lda #0
    uint8_t a = 0;
    //     sta input_file_empty_flag
    input_file_empty_flag = a;
    //     sta file_edit_flags
    file_edit_flags = a;

    //     jsr select_file
    select_file(1);
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

static void count_cmd(void)
{
    addr_t tmp89;

    // count_cmd
    // Pseudocode: Counts words in document area handling command prefixes and
    // punctuation
    static const uint8_t l8747_data[] = {0x52, 0x4a, 'C', 'E', 'L', 'J', 0};

    // ;
    // ***************************************************************************************
    // count_cmd:
    //     jsr parse_marks_from_command
    parse_marks_from_command();
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
    tmp01 = area_start_ptr;
    //     lda #0
    a = 0;
    //     sta ((uint8_t*)&tmp89)[0]
    //     sta ((uint8_t*)&tmp89)[1]
    tmp89 = 0;
    //     sta l0083
    l0083 = a;
    //     sta l0082
    l0082 = a;
    // c86b8:
c86b8:
    //     ldy #0
    y = 0;
    //     jsr deref_and_check_for_command_prefix
    flags = deref_and_check_for_command_prefix(y);
    //     bne c86ea
    if (!(flags & FLAG_Z))
        goto c86ea;
    //     ldx #0
    x = 0;
    //     iny
    y++;
    // loop_c86c2:
loop_c86c2:
    //     lda (((uint8_t*)&tmp01)[0]),y
    a = ram[tmp01 + y];
    //     iny
    y++;
    //     cmp l8747,x
    if (a != l8747_data[x])
        goto c86d1;
    //     lda (((uint8_t*)&tmp01)[0]),y
    a = ram[tmp01 + y];
    //     cmp l8748,x
    if (a == l8747_data[x + 1])
        goto c86df;
    // c86d1:
c86d1:
    //     lda l8749,x
    a = l8747_data[x + 2];
    set_flags(&flags, a);
    //     beq c86db
    if (flags & FLAG_Z)
        goto c86db;
    //     dey
    y--;
    //     inx
    x++;
    //     inx
    x++;
    if (x != 0)
        goto loop_c86c2;
    //     bne loop_c86c2
    // c86db:
c86db:
    //     lda #0x80
    a = 0x80;
    //     bne c86ff                                                         ;
    //     ALWAYS branch
    goto c86ff;

    // c86df:
c86df:
    //     lda ((uint8_t*)&tmp01)[0]
    a = ((uint8_t*)&tmp01)[0];
    //     clc
    flags &= ~FLAG_C;
    //     adc #3
    a = adc(&flags, a, 3);
    //     sta ((uint8_t*)&tmp01)[0]
    ((uint8_t*)&tmp01)[0] = a;
    //     bcs c871d
    if (flags & FLAG_C)
        goto c871d;
    //     bcc c871f                                                         ;
    //     ALWAYS branch
    goto c871f;

    // c86ea:
c86ea:
    //     ldy #0
    y = 0;
    //     jsr process_current_document_character
    process_current_document_character();
    //     and #0x7f
    a &= 0x7f;
    //     ldx #0
    x = 0;
    //     ldy l0082
    y = l0082;
    if ((int8_t)y < 0)
        goto c870d;
    //     cmp #0x0d
    if (a == 0x0d)
        goto c8703;
    //     beq c8703
    //     cmp #0x20 ; ' '
    if (a == 0x20)
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
    y = l0083;
    set_flags(&flags, y);
    //     beq c870d
    if (!(flags & FLAG_Z))
    {
        tmp89++;
    }
c870d:
    //     stx l0083
    l0083 = x;
    //     cmp #0x0d
    if (!(a != 0x0d))
    {
        l0082 = x;
    }
c8715:
    //     ora l0082
    a |= l0082;
    //     sta l0082
    l0082 = a;
    tmp01++;
c871d:
    // c871f:
c871f:
    //     ldy ((uint8_t*)&tmp01)[1]
    y = ((uint8_t*)&tmp01)[1];
    //     cpy area_end_ptr+1
    if (y != (uint8_t)(area_end_ptr >> 8))
        goto c86b8;
    //     ldy ((uint8_t*)&tmp01)[0]
    y = ((uint8_t*)&tmp01)[0];
    //     cpy area_end_ptr
    if (y != (uint8_t)(area_end_ptr & 0xff))
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

static void edit_cmd(void)
{
    uint8_t x;

    // edit_cmd
    check_not_continuous_editing();
    parse_filename_from_command();
    set_document_name_to_filename_buffer();
    open_input_file();
    parse_filename_from_command();
    open_output_file();
    x = 0;
    input_file_empty_flag = x;
    do
    {
        a = filename_buffer[x];
        if (a == 0)
            a = 0x0d;
        output_filename[x] = a;
        x++;
    } while (a != 0x0d);
    a = initialise_document();
    read_first_chunk_from_input_file();
    if (flags & FLAG_Z)
    {
        close_input_output_files();
        return_to_cli_prompt();
        return;
    }
    file_edit_flags = 1;
}

static void field_cmd(void)
{
    // field_cmd
    // Pseudocode: Sets the tab key field width from parsed integer argument

    // ;
    // ***************************************************************************************
    // field_cmd:
    //     jsr parse_integer_from_command
    parse_integer_from_command();
    //     beq c869b
    if (flags & FLAG_Z)
    {
        return_to_cli_prompt();
        return;
    }
    //     lda ((uint8_t*)&tmp89)[0]
    a = ((uint8_t*)&tmp89)[0];
    //     cmp #0x1b
    if (!(a != 0x1b))
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
        select_file(1);
        write_area_to_file();
        if (!(flags & FLAG_Z))
        {
            return_to_cli_prompt();
            return;
        }
        put_byte_to_file(0);
        a = sub_c89d3(tmp67);
        move_cursor_to_top_of_document();
        cb05a();
        a = input_file_empty_flag;
        if (a != 0)
        {
            close_input_output_files();
            return;
        }
        read_first_chunk_from_input_file();
        if (flags & FLAG_Z)
        {
            return_to_cli_prompt();
            return;
        }
    }
loop_c84ee:
}

static void fold_cmd(void)
{
    // fold_cmd
    // Pseudocode: Toggles folding on/off and displays current folding status

    // ;
    // ***************************************************************************************
    // fold_cmd:
    //     jsr sub_c8e33
    sub_c8e33();
    //     beq c87b4
    if (flags & FLAG_Z)
        goto c87b4;
    //     lda input_buffer,y
    a = input_buffer[y];
    //     cmp #'1'
    cmp(&flags, a, '1');
    //     beq c87b2 (true → folding_flag = 0)
    if (flags & FLAG_Z)
    {
        folding_flag = 0;
        goto c87b4;
    }
    //     cmp #'0'
    if (!(a != '0'))
    {
        folding_flag = 0x80;
    }
c87b4:
    //     jsr print_inline_string
    //     .ascii "Folding "
    //     .byte 0
    cli_putstring("Folding ");

    //     lda folding_flag
    a = folding_flag;
    if (((int8_t)a < 0))
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

static void format_cmd(void)
{
    // Pseudocode: Formats document area by running line-by-line through
    // formatting pipeline

    // ;
    // ***************************************************************************************
    // format_cmd:
    //     jsr parse_marks_from_command
    parse_marks_from_command();
    //     jsr sanitise_area
    if (sanitise_area() == AREA_EMPTY)
        goto c878b;
    //     lda area_start_ptr
    //     jsr move_cursor_to_address
    move_cursor_to_address(area_start_ptr);
    //     jsr sub_caf5f
    sub_caf5f();
    //     lda #0x10
    //     jsr wipe_buffer
    wipe_buffer(0x10);
    //     lda current_edit_line_ptr
    a = (uint8_t)(current_edit_line_ptr & 0xff);
    //     sta current_format_line_ptr
    current_format_line_ptr = (current_format_line_ptr & 0xff00) | a;
    //     lda current_edit_line_ptr+1
    a = (uint8_t)((current_edit_line_ptr >> 8) & 0xff);
    //     sta current_format_line_ptr+1
    current_format_line_ptr =
        (current_format_line_ptr & 0x00ff) | ((uint16_t)a << 8);
    // c876d:
c876d:
    //     jsr sub_c9977
    sub_c9977();
    //     bvs c8791
    if (flags & FLAG_V)
        goto c8791;
    //     bcs c8787
    if (flags & FLAG_C)
        goto c8787;
    //     lda #0x2e ; '.'
    //     jsr bdos_print_char
    cli_putchar(0x2e);
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     ldy current_line_ptr+1
    y = (uint8_t)((current_line_ptr >> 8) & 0xff);
    //     cpy area_end_ptr+1
    if (((uint16_t)y << 8 | a) < area_end_ptr)
        goto c876d;
    // c8787:
c8787:
    //     lda #0xff
    a = 0xff;
    //     sta l0012
    top_of_screen_line_ptr =
        (top_of_screen_line_ptr & 0x00ff) | ((addr_t)a << 8);
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

static uint8_t load_cmd(void)
{
    // load_cmd
    // load_cmd:
    //     jsr check_not_continuous_editing
    check_not_continuous_editing();
    //     jsr parse_filename_from_command
    parse_filename_from_command();
    //     jsr initialise_document
    a = initialise_document();
    top = page; // WORKAROUND: cb05a bumped top past the initial CR; need to
                // load at page, not page+1
    //     jsr reset_area_to_entire_document
    reset_area_to_entire_document();
    //     jsr 1f
    a = read_into_document();
    top = (addr_t)((uint16_t)((uint8_t*)&tmp01)[1] << 8) |
          ((uint8_t*)&tmp01)[0]; // WORKAROUND: adjust_pointers adds stale bytes
                                 // from end of ram[]; fix top
    //     jsr reset_document_name_after_load
    reset_document_name_after_load();
    //     jsr clear_cmd
    clear_cmd();
    //     jmp move_cursor_to_top_of_document
    move_cursor_to_top_of_document();
    return a;
}

static void microspace_cmd(void)
{
    // microspace_cmd
    // Pseudocode: Configures microspacing by querying printer driver

    // ;
    // ***************************************************************************************
    // microspace_cmd:
    //     jsr prepare_printer_driver
    prepare_printer_driver();
    //     jsr parse_integer_from_command
    parse_integer_from_command();
    //     php
    uint8_t saved_flags = flags;
    //     ldx #0x0a
    x = 0x0a;
    //     plp
    flags = saved_flags;
    //     beq c8608
    if (!(flags & FLAG_Z))
    {
        x = ((uint8_t*)&tmp89)[0];
        if (x == 0)
            return;
    }
    //     ldy #0
    y = 0;
    //     lda #0x0c
    //     jsr call_printer_driver
    printer_driver_ptr->printer_getflags();
    //     tya
    a = y;
    //     and #1
    a &= 1;
    set_flags(&flags, a);
    //     beq c8617
    if (!(flags & FLAG_Z))
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

static void more_cmd(void)
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
    parse_marks_from_command();
    //     jsr move_cursor_to_address
    move_cursor_to_address(area_start_ptr);

    //     jsr select_file
    select_file(1);
    //     jsr write_area_to_file
    write_area_to_file();
    //     bne c84ab
    if (!(flags & FLAG_Z))
    {
        return_to_cli_prompt();
        return;
    }

    //     ldy #0
    y = 0;
    //     ldx l003a
    x = l003a;
    // loop_c84c4:
    do
    {
        a = ram[current_ruler_ptr + y];
        current_ruler_buffer[y] = a;
        y++;
        x--;
    } while (x != 0);
loop_c84c4:
    //     lda #0x0d
    //     sta current_ruler_buffer,y
    current_ruler_buffer[y] = 0x0d;
    //     jsr sub_c89d3
    a = sub_c89d3(tmp67);
    //     jsr move_cursor_to_top_of_document
    move_cursor_to_top_of_document();
    //     jsr check_for_at_least_150_bytes_free
    check_for_at_least_150_bytes_free();
    //     lda input_file_empty_flag
    a = input_file_empty_flag;
    //     bne c84e8
    if (!(a != 0))
    {
        a = (uint8_t)(top & 0xff);
        y = (uint8_t)((top >> 8) & 0xff);
        read_next_chunk_from_input_file();
        if (flags & FLAG_Z)
        {
            return_to_cli_prompt();
            return;
        }
    }
    //     jmp cb05a
    cb05a();
}

static void name_cmd(void)
{
    // name_cmd
    // Pseudocode: Sets document name from optional filename argument

    // ;
    // ***************************************************************************************
    // name_cmd:
    //     jsr check_not_continuous_editing
    check_not_continuous_editing();
    //     jsr parse_optional_filename_from_command
    parse_optional_filename_from_command();
    //     php
    uint8_t saved_flags = flags;
    //     lda #0
    a = 0;
    //     sta file_edit_flags
    file_edit_flags = a;
    //     plp
    flags = saved_flags;
    //     beq return_9
    if (flags & FLAG_Z)
        return;

    // MULTIPLE ENTRY POINTS: name_cmd, reset_document_name_after_load
    reset_document_name_after_load();
}

static uint8_t new_cmd(void)
{
    // Pseudocode: Creates a new empty document after checking continuous
    // editing state

    // ;
    // ***************************************************************************************
    // new_cmd:
    //     jsr check_not_continuous_editing
    check_not_continuous_editing();
    //     jmp initialise_document
    a = initialise_document();
    return x;
}

static void print_cmd(void)
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
    print_to_screen();
}

static void print_to_screen(void)
{
    // print_to_screen: Prints document for screen preview, returns to CLI when
    // done

    //     jsr print_document
    print_document();
    //     jmp return_to_cli_prompt
    return_to_cli_prompt();
    return;
}

static void printer_cmd(void)
{
    // Pseudocode: Redirects to print_cmd (printer driver loading code is
    // disabled with #if 0)

    // ;
    // ***************************************************************************************
    // printer_cmd:
    //     jmp print_cmd
    print_cmd();
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

static void read_cmd(void)
{
    // read_cmd:
    //     jsr parse_filename_from_command
    parse_filename_from_command();
    //     jsr parse_marks_from_command
    parse_marks_from_command();
    // 1:
    a = read_into_document();
    //     jmp return_to_cli_prompt
    return_to_cli_prompt();
    return;
}

static void replace_cmd(addr_t ptr6)
{
    // replace_cmd
    // Pseudocode: Interactive search and replace prompting for each match
    // (Y)es/(O)K/(N)o

    // ;
    // ***************************************************************************************
    // replace_cmd:
    //     jsr sub_c83f0
    sub_c83f0();
    //     beq c82e7
    if (flags & FLAG_Z)
    {
        cmd_err_no_target();
        return;
    }
    //     jsr c8b7b
    c8b7b();
    //     bne c82fa
    if (!(flags & FLAG_Z))
    {
        cmd_err_no_string();
        return;
    }
    //     jsr move_cursor_to_address
    move_cursor_to_address((uint16_t)(y) << 8 | a);
    //     jsr enter_editor_mode
    enter_editor_mode();
    // c832d:
c832d:
    //     jsr sub_c8361
    sub_c8361(ptr6);
    //     ldx #0x52 ; 'R'
    x = 0x52;
    //     ldy #0x50 ; 'P'
    y = 0x50;
    //     jsr draw_prompt_characters
    a = draw_prompt_characters(x, y);
    //     jsr flush_and_read_char
    read_char();
    //     bcs return_2
    if (flags & FLAG_C)
        return;
    //     and #0xdf
    a &= 0xdf;
    //     ldx #0
    x = 0;
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
    sub_c8371(ptr2, ptr6);
    //     jsr sub_c8a4f
    sub_c8a4f(ptr2);
    //     bcs c836b
    if (flags & FLAG_C)
    {
        show_memory_full_error();
        esc_key();
        return;
    }
    //     jsr sub_c8361
    sub_c8361(ptr6);
    // c8356:
c8356:
    //     jsr c8b7b
    c8b7b();
    //     bne return_2
    if (!(flags & FLAG_Z))
        return;
    //     jsr move_cursor_to_address
    move_cursor_to_address((uint16_t)(y) << 8 | a);
    //     jmp c832d
    goto c832d;
}

static void save_cmd_write_cmd(void)
{
    // save_cmd_write_cmd
    // Pseudocode: Saves document area to output file with optional filename

    // ;
    // ***************************************************************************************
    // save_cmd:
    // write_cmd:
    //     jsr parse_optional_filename_from_command
    parse_optional_filename_from_command();
    //     zif eq
    if (flags & FLAG_Z)
    {
        //         bit file_edit_flags
        bit(&flags, a, file_edit_flags);
        //         zif vc
        if (!(flags & FLAG_V))
        {
            //             jmp bad_filename_error
            bad_filename_error();
            return;
            //         zendif
        }

        //         ldx #0
        x = 0;
        //         zrepeat
        do
        {
            //             lda input_filename,x
            a = input_filename[x];
            //             sta filename_buffer,x
            filename_buffer[x] = a;
            //             inx
            x++;
            //             cmp #0x0d
            cmp(&flags, a, 0x0d);
            //         zuntil eq
        } while (!(flags & FLAG_Z));
        //     zendif
    }
    //     jsr parse_marks_from_command
    parse_marks_from_command();
    //     jsr sanitise_area
    if (sanitise_area() == AREA_EMPTY)
        return;

    //     jsr open_output_file
    open_output_file();
    //     jsr write_area_to_file
    write_area_to_file();
    //     lda #0
    //     jsr put_byte_to_file
    put_byte_to_file(0);

    //     jsr close_file
    close_file();
    //     jmp return_to_cli_prompt
    return_to_cli_prompt();
    return;

    // MULTIPLE ENTRY POINTS: save_cmd, write_cmd
}

static void screen_cmd(void)
{
    // Pseudocode: Jumps to print_to_screen for on-screen document preview

    // ;
    // ***************************************************************************************
    // screen_cmd:
    //     jmp print_to_screen
    print_to_screen();
    return;
}

static void search_cmd(void)
{
    // search_cmd
    // Pseudocode: Searches for target string, reports position if found

    // ;
    // ***************************************************************************************
    // search_cmd:
    //     jsr sub_c8412
    sub_c8412();
    //     beq c82e7
    if (flags & FLAG_Z)
    {
        cmd_err_no_target();
        return;
    }
    //     jsr parse_marks_from_command
    parse_marks_from_command();
    //     jsr sanitise_area
    if (sanitise_area() == AREA_EMPTY)
    {
        cmd_err_no_string();
        return;
    }
    //     jsr sub_c8c7c
    sub_c8c7c();
    //     jsr c8b7b
    c8b7b();
    //     bne c82fa
    if (!(flags & FLAG_Z))
    {
        cmd_err_no_string();
        return;
    }
    //     jsr move_cursor_to_address
    move_cursor_to_address((uint16_t)(y) << 8 | a);
    //     jmp enter_editor_mode
    enter_editor_mode();
    longjmp(env, JMP_EDITOR);
    return;

    // ;
    // ***************************************************************************************
}

static void setup_cmd(void)
{
    addr_t tmp67;
    addr_t tmp89;

    // setup_cmd
    // Pseudocode: Parses flag letters and sets format_mode_flag,
    // justifying_flag, insert_mode_flag

    // ;
    // ***************************************************************************************
    // setup_cmd:
    //     ldx #1
    uint8_t x;
    x = 1;
    //     stx ((uint8_t*)&tmp67)[0]
    ((uint8_t*)&tmp67)[0] = x;
    //     dex                                                               ;
    //     X=0x00
    x--;
    //     stx ((uint8_t*)&tmp89)[0]
    ((uint8_t*)&tmp89)[0] = x;
    //     dex                                                               ;
    //     X=0xff
    x--;
    //     stx ((uint8_t*)&tmp67)[1]
    ((uint8_t*)&tmp67)[1] = x;
    // c8649:
c8649:
    //     jsr sub_c8e33
    sub_c8e33();
    //     beq c8672
    if (flags & FLAG_Z)
        goto c8672;
    //     and #0xdf
    a &= 0xdf;
    //     ldx #0
    x = 0;
    // loop_c8652:
loop_c8652:
    //     cmp c867d,x
    if (a == ((const uint8_t[]){0x4e, 0x4a, 0x00, 0x49, 0x00})[x])
        goto c8669;
    //     inx
    x++;
    //     ldy c867d,x
    y = ((const uint8_t[]){0x4e, 0x4a, 0x00, 0x49, 0x00})[x];
    //     bne loop_c8652
    if (y != 0)
        goto loop_c8652;
    //     jsr print_inline_string
    //     .ascii "Bad flag"
    //     .byte 0xff
    cli_putstring("Bad flag\n");
    return_to_cli_prompt();
    return;

    // c8669:
c8669:
    //     lda c8681,x
    a = ((const uint8_t[]){0x00, 0x00, 0xff})[x];
    //     sta ((uint8_t*)&tmp67)[0],x
    if (x == 0)
        ((uint8_t*)&tmp67)[0] = a;
    else if (x == 1)
        ((uint8_t*)&tmp67)[1] = a;
    else
        ((uint8_t*)&tmp89)[0] = a;
    //     inc input_buffer_offset
    input_buffer_offset++;
    if (input_buffer_offset != 0)
        goto c8649;
    //     bne c8649
    // c8672:
c8672:
    //     ldx #2
    x = 2;
    // loop_c8674:
    do
    {
        if (x == 0)
            a = ((uint8_t*)&tmp67)[0];
        else if (x == 1)
            a = ((uint8_t*)&tmp67)[1];
        else
            a = ((uint8_t*)&tmp89)[0];
        if (x == 0)
            format_mode_flag = a;
        else if (x == 1)
            justifying_flag = a;
        else
            insert_mode_flag = a;
        x--;
    } while (!((int8_t)x < 0));
loop_c8674:
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

static void sheets_cmd(void)
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
    print_document();
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

void readline(void)
{
    input_buffer_offset = 0;
    if (cli_readstring((char*)input_buffer, MAX_COMMAND_LENGTH))
    {
        flags |= FLAG_C;
        return;
    }
    flags &= ~FLAG_C;
}

const uint8_t la83d[] = "VIEW\0B3.0 for CP/M-65";

static void print_x_words_of_help(uint8_t a, uint8_t x)
{
    // print_x_words_of_help
    // Pseudocode: Prints X words of the help string showing VIEW and version

    // ;
    // ***************************************************************************************
    // print_x_words_of_help:
    //     ldy #0
    uint8_t y;
    y = 0;
    //     beq ca832                                                         ;
    //     ALWAYS branch
    goto ca832;

    // ca82e:
ca82e:
    //     jsr bdos_print_char
    cli_putchar(a);
    //     iny
    y++;
    // ca832:
ca832:
    //     lda la83d,y
    a = la83d[y];
    //     bne ca82e
    if (a != 0)
        goto ca82e;
    //     lda #0x20 ; ' '
    a = 0x20;
    //     dex
    x--;
    //     bpl ca82e
    if ((int8_t)x >= 0)
        goto ca82e;
    //     rts
    return;
}

static void parse_command(void);

void input_line_not_escaped(void)
{
    // input_line_not_escaped
    // input_line_not_escaped: Parses command input and dispatches through CLI
    // jump table

    //     jsr parse_command
    parse_command();
    //     sty input_buffer_offset+1
    l0080 = y;
    //     bcs c8263
    if (flags & FLAG_C)
        goto c8263;
    //     cpy #(jumptable4_cli_end-jumptable4_cli)/2
    if (y < 48)
        goto c826e;
    //     bcc c826e
    // c8263:
c8263:
    //     jsr print_inline_string ; .ascii "Mistake\n"
    cli_putstring("Mistake\n");
    // c826e:
c826e:
    //     lda input_buffer_offset+1
    a = l0080;
    //     ldy #2
    //     jsr call_through_jumptable
    a = execute_cli_command(a);
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
    //     stx error_handling_mode
    error_handling_mode = 0xff;
    //     txs  (handled by setjmp/longjmp in main_)
    //     inx  ; X=0x00
    //     stx print_flags
    print_flags = 0;
    //     jsr print_inline_string ; .ascii "=>"
    cli_putstring("=>");
    //     jsr readline
    readline();
    //     lda #<input_buffer
    //     sta ((uint8_t*)&tmp01)[0]
    //     ldx #>input_buffer
    //     stx ((uint8_t*)&tmp01)[1]
    // (((uint8_t*)&tmp01)[0]/((uint8_t*)&tmp01)[1] no longer used as a pointer;
    // parse_command reads input_buffer[] directly)
    //     bcc input_line_not_escaped
    if (!(flags & FLAG_C))
    {
        input_line_not_escaped();
        return;
    }
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
    print_x_words_of_help(a, 1);
    //     jsr print_inline_string
    //     .ascii "\r\rBytes free "
    //     .byte 0
    cli_putstring("\n\nBytes free ");

    //     jsr compute_bytes_free
    compute_bytes_free();
    render_number_to_screen((addr_t)(y) << 8 | x);
    //     jsr bdos_print_newline
    cli_putchar('\n');
    //     jsr display_document_file_state
    display_document_file_state();
    //     bit file_edit_flags
    if ((file_edit_flags & 0x40))
        goto c816d;
    //     lda file_edit_flags
    a = file_edit_flags;
    //     ror
    //     bcc c816d
    if ((a & 1))
    {
        cli_putstring("Input file is ");
        a = input_file_empty_flag;
        if (!(a != 0))
        {
            cli_putstring("not ");
        }
        cli_putstring("empty\n");
    }
c816d:
    //     lda printer_driver_name
    a = printer_driver_name[0];
    set_flags(&flags, a);
    //     beq c81b6
    if (flags & FLAG_Z)
        goto c81b6;
    //     jsr print_inline_string
    //     .ascii "Printer "
    //     .byte 0
    cli_putstring("Printer ");

    //     ldx #0
    x = 0;
    // loop_c819a:
    //     lda printer_driver_name,x
    //     cmp #0x0d
    //     beq c81a7
    //     jsr bdos_print_char
    //     inx
    //     bne loop_c819a
    do
    {
        a = printer_driver_name[x];
        if (a == 0x0d)
            break;
        cli_putchar(a);
        x++;
    } while (x != 0);
    // c81a7:
    //     lda microspacing_flag
    a = microspacing_flag;
    set_flags(&flags, a);
    //     beq c81b3
    if (!(flags & FLAG_Z))
    {
        cli_putstring(" (m)");
    }
    //     jsr bdos_print_newline
    cli_putchar('\n');
    // c81b6:
c81b6:
    //     ldx #0
    x = 0;
    //     ldy #0
    y = 0;
    // c81ba:
c81ba:
    //     lda markers_array+1,x
    a = ((uint8_t*)markers_array)[x + 1];
    set_flags(&flags, a);
    //     beq c81e7
    if (flags & FLAG_Z)
        goto c81e7;
    //     tya
    //     bne c81db
    if (y != 0)
        goto c81db;
    //     stx l0083
    l0083 = x;
    //     jsr print_inline_string
    //     .ascii "Marker(s) set "
    //     .byte 0
    cli_putstring("Marker(s) set ");

    //     ldx l0083
    x = l0083;
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
    a = x;
    //     lsr
    a = asr(&flags, a);
    //     adc #0x31 ; '1'
    a = adc(&flags, a, 0x31);
    //     jsr screen_putchar
    screen_putchar(a);
    // c81e7:
c81e7:
    //     inx
    x++;
    //     inx
    x++;
    //     cpx #0x0c
    //     bne c81ba
    if (x != 0x0c)
        goto c81ba;
    //     tya
    //     beq c81f3
    if (!(y == 0))
    {
        cli_putchar('\n');
    }
    //     jsr bdos_print_newline
    cli_putchar('\n');
    return_to_cli_prompt();
}

// CLI command parser
static void parse_command(void)
{
    // parse_command
    //     .ascii "VIEW"
    //     .byte 0
    //     .ascii "B3.0 for CP/M-65"
    //     .byte 0

    // ;
    // ***************************************************************************************
    // parse_command:
    //     lda #0xff
    uint8_t x;
    uint8_t a;
    a = 0xff;
    //     sta l0082
    l0082 = a;
    //     tax                                                               ;
    //     X=0xff
    x = a;
    // ca84c:
ca84c:
    //     ldy input_buffer_offset
    y = input_buffer_offset;
    //     dey
    y--;
    //     inc l0082
    l0082++;
    // loop_ca851:
loop_ca851:
    //     inx
    x++;
    //     iny
    y++;
    //     lda (((uint8_t*)&tmp01)[0]),y
    a = input_buffer[y];
    //     and #0xdf
    a &= 0xdf;
    //     sta l0084
    l0084 = a;
    //     lda parser_table,x
    a = parser_table[x];
    set_flags(&flags, a);
    //     beq ca890
    if (flags & FLAG_Z)
        goto ca890;
    //     bmi ca87e
    if (flags & FLAG_N)
        goto ca87e;
    //     eor #0x5b ; '['
    a ^= 0x5b;
    //     sta l0083
    l0083 = a;
    //     and #0xdf
    a &= 0xdf;
    //     cmp l0084
    if (a == l0084)
        goto loop_ca851;
    //     beq loop_ca851
    // loop_ca86a:
loop_ca86a:
    //     inx
    x++;
    //     lda parser_table,x
    a = parser_table[x];
    set_flags(&flags, a);
    //     beq ca890
    if (flags & FLAG_Z)
        goto ca890;
    //     bpl loop_ca86a
    if (!(flags & FLAG_N))
        goto loop_ca86a;
    //     lda l0083
    a = l0083;
    //     and #0x20 ; ' '
    a &= 0x20;
    flags = (flags & ~FLAG_Z) | (a == 0 ? FLAG_Z : 0);
    //     beq ca84c
    if (flags & FLAG_Z)
        goto ca84c;
    //     lda (((uint8_t*)&tmp01)[0]),y
    a = input_buffer[y];
    //     cmp #0x30 ; '0'
    if (a >= 0x30)
        goto ca84c;
    //     bcs ca84c
    // ca87e:
ca87e:
    //     lda (((uint8_t*)&tmp01)[0]),y
    a = input_buffer[y];
    //     cmp #0x30 ; '0'
    if (!(a >= 0x30))
    {
        l007e = a;
        y++;
    }
    //     sty input_buffer_offset
    input_buffer_offset = y;
    //     ldy l0082
    y = l0082;
    //     lda parser_table,x
    a = parser_table[x];
    //     clc
    flags &= ~FLAG_C;
    //     rts
    return;

    // ca890:
ca890:
    //     sec
    flags |= FLAG_C;
    //     rts
    return;
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

void parse_integer_from_command(void)
{
    // Pseudocode: Parses a decimal integer from the command input buffer

    // ;
    // ***************************************************************************************
    // parse_integer_from_command:
    //     lda #<(input_buffer)
    a = (uint8_t)((uintptr_t)input_buffer & 0xff);
    //     sta current_format_line_ptr
    current_format_line_ptr = (current_format_line_ptr & 0xff00) | a;
    //     lda #>(input_buffer)
    a = (uint8_t)(((uintptr_t)input_buffer >> 8) & 0xff);
    //     sta current_format_line_ptr+1
    current_format_line_ptr =
        (current_format_line_ptr & 0x00ff) | ((uint16_t)a << 8);
    //     jsr sub_c8e33
    sub_c8e33();
    //     beq return_8
    if (flags & FLAG_Z)
        return;
    //     jmp ca6fe
    parse_decimal_number();
    return;
}

void parse_marks_from_command(void)
{
    // parse_marks_from_command:
    //     jsr reset_area_to_entire_document
    reset_area_to_entire_document();
    //     jsr parse_mark_from_command
    addr_t start_mark = parse_mark_from_command(x);
    //     beq return_11
    if (start_mark == 0)
        return;
    //     sta area_start_ptr
    area_start_ptr = start_mark;
    //     sty area_start_ptr+1
    //     jsr parse_mark_from_command
    addr_t end_mark = parse_mark_from_command(x);
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
    // set_document_name_to_filename_buffer
    // Pseudocode: Copies filename buffer to input filename buffer

    // set_document_name_to_filename_buffer:
    //     ldx #0
    uint8_t x;
    uint8_t a;
    uint8_t a2;
    x = 0;
    // loop_c88fa:
    do
    {
        a = filename_buffer[x];
        input_filename[x] = a;
        x++;
    } while (a >= 0x21);
loop_c88fa:
    //     bge loop_c88fa
    // return_9:
    //     lda #0x0d
    a2 = 0x0d;
    //     sta input_filename-1, x
    input_filename[x - 1] = a2;
    //     rts
    return;

    // MULTIPLE ENTRY POINTS: also called directly from edit_cmd
}

void zero_terminate_filename_buffer(void)
{
    // zero_terminate_filename_buffer:
    //     ldx #0
    uint8_t x;
    uint8_t a;
    uint8_t a2;
    x = 0;
    //     lda #0x0d
    a = 0x0d;
    // zloop:
zloop:
    //     cmp filename_buffer, x
    if (a == filename_buffer[x])
        goto zbreak;
    //     inx
    x++;
    //     bne zloop
    goto zloop;
zbreak:
    //     lda #0
    a2 = 0;
    //     sta filename_buffer, x
    filename_buffer[x] = a2;
    //     rts
}

addr_t parse_mark_from_command(uint8_t x)
{
    // parse_mark_from_command
    // parse_mark_from_command:
    //     jsr sub_c8e33
    sub_c8e33();
    //     beq return_12
    if (flags & FLAG_Z)
        return 0;
    //     iny
    y++;
    //     sty input_buffer_offset
    input_buffer_offset = y;
    //     jsr lookup_marker
    lookup_marker(a);
    //     bcs c89b3 / c89b3: jsr print_inline_string ; .ascii "Bad marker" ;
    //     .byte 0xff
    if (flags & FLAG_C)
    {
        cli_putstring("Bad marker\n");
        return_to_cli_prompt();
        return 0;
    }
    //     beq c89c1 / c89c1: jsr print_inline_string ; .ascii "Marker not set"
    //     ; .byte 0xff
    if (flags & FLAG_Z)
    {
        cli_putstring("Marker not set\n");
        return_to_cli_prompt();
        return 0;
    }
    //     lda markers_array,x
    a = (uint8_t)(markers_array[x] & 0xff);
    //     ldy markers_array+1,x
    y = (uint8_t)(markers_array[x] >> 8);
    // return_12:
    //     rts
    return (addr_t)(y) << 8 | a;
}
