#include "cli.h"
#include "document.h"
#include "printing.h"
#include "io.h"
#include <stdlib.h>
void parse_mark_from_command(void);

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
static void replace_cmd(void);
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
static void load_cmd(void);
static void read_cmd(void);
static void mode_cmd(void);
static void microspace_cmd(void);
static void setup_cmd(void);
static void field_cmd(void);
static void count_cmd(void);
static void format_cmd(void);
static void new_cmd(void);
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

void execute_cli_command(void)
{
    // execute_cli_command
    //  Inputs: a
    // call_through_jumptable (y=2):
    //     asl
    //     clc
    //     adc jumptable_ptrs,y
    //     sta tmp8
    //     lda #0
    //     adc jumptable_ptrs+1,y
    //     sta tmp9
    //     ldy #0
    //     lda (tmp8),y
    //     sta tmp6
    //     iny
    //     lda (tmp8),y
    //     sta tmp7
    //     jmp (tmp6)
    switch (a)
    {
        case 0:
            quit_cmd();
            break;
        case 1:
            new_cmd();
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
            replace_cmd();
            break;
        case 24:
            load_cmd();
            break;
        case 25:
            bye_cmd();
            break;
    }
}

static void change_cmd(void)
{
    // change_cmd
    //  Inputs: flags
    //  Outputs: a, x, y; ptr3
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
    move_cursor_to_address();
    //     lda #0
    a = 0;
    //     sta print_xpos
    print_xpos = a;
    //     jsr sub_c8a4f
    sub_c8a4f();
    //     bcs c830d
    if (flags & FLAG_C)
        goto c830d;
    //     jsr c8b7b
    c8b7b();
    //     beq loop_c82b3
    if (flags & FLAG_Z)
        goto loop_c82b3;
    //     ldx ptr3
    x = (uint8_t)(ptr3 & 0xff);
    //     ldy ptr3+1
    y = (uint8_t)((ptr3 >> 8) & 0xff);
    //     jsr render_number_to_screen
    render_number_to_screen();
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
    //  Outputs: a, x
    // Pseudocode: Clears all markers (sets to zero)

    // ;
    // ***************************************************************************************
    // clear_cmd:
    //     ldx #0x0b
    x = 0x0b;
    //     lda #0
    a = 0;
    // loop_cb095:
loop_cb095:
    //     sta markers_array,x
    ((uint8_t*)markers_array)[x] = a;
    //     dex
    x--;
    //     bpl loop_cb095
    if (!(x & 0x80))
        goto loop_cb095;
    //     rts
    return;
}

static void close_input_output_files(void)
{
    // close_input_output_files
    //  Outputs: a, x
    // Pseudocode: Closes output file, resets editing flags, returns to CLI

    // close_input_output_files:
    //     lda #0
    a = 0;
    //     sta input_file_empty_flag
    input_file_empty_flag = a;
    //     sta file_edit_flags
    file_edit_flags = a;

    //     jsr select_file
    x = 1;
    select_file();
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
    // count_cmd
    //  Inputs: flags
    //  Outputs: a, x, y; tmp01, tmp89
    // Pseudocode: Counts words in document area handling command prefixes and
    // punctuation
    static const uint8_t l8747_data[] = {0x52, 0x4a, 'C', 'E', 'L', 'J', 0};

    // ;
    // ***************************************************************************************
    // count_cmd:
    //     jsr parse_marks_from_command
    parse_marks_from_command();
    //     jsr sanitise_area
    sanitise_area();
    //     beq c869b
    if (flags & FLAG_Z)
    {
        return_to_cli_prompt();
        return;
    }
    //     lda area_start_ptr
    a = (uint8_t)(area_start_ptr & 0xff);
    //     sta tmp0
    tmp0 = a;
    //     lda area_start_ptr+1
    a = (uint8_t)((area_start_ptr >> 8) & 0xff);
    //     sta tmp1
    tmp1 = a;
    //     lda #0
    a = 0;
    //     sta tmp8
    tmp8 = a;
    //     sta tmp9
    tmp9 = a;
    //     sta l0083
    l0083 = a;
    //     sta l0082
    l0082 = a;
    // c86b8:
c86b8:
    //     ldy #0
    y = 0;
    //     jsr deref_and_check_for_command_prefix
    flags = deref_and_check_for_command_prefix();
    //     bne c86ea
    if (!(flags & FLAG_Z))
        goto c86ea;
    //     ldx #0
    x = 0;
    //     iny
    y++;
    // loop_c86c2:
loop_c86c2:
    //     lda (tmp0),y
    a = ram[tmp01 + y];
    //     iny
    y++;
    //     cmp l8747,x
    cmp(&flags, a, l8747_data[x]);
    //     bne c86d1
    if (!(flags & FLAG_Z))
        goto c86d1;
    //     lda (tmp0),y
    a = ram[tmp01 + y];
    //     cmp l8748,x
    cmp(&flags, a, l8747_data[x + 1]);
    //     beq c86df
    if (flags & FLAG_Z)
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
    set_flags(&flags, x);
    //     bne loop_c86c2
    if (!(flags & FLAG_Z))
        goto loop_c86c2;
    // c86db:
c86db:
    //     lda #0x80
    a = 0x80;
    //     bne c86ff                                                         ;
    //     ALWAYS branch
    goto c86ff;

    // c86df:
c86df:
    //     lda tmp0
    a = tmp0;
    //     clc
    flags &= ~FLAG_C;
    //     adc #3
    a = adc(&flags, a, 3);
    //     sta tmp0
    tmp0 = a;
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
    set_flags(&flags, y);
    //     bmi c870d
    if (flags & FLAG_N)
        goto c870d;
    //     cmp #0x0d
    cmp(&flags, a, 0x0d);
    //     beq c8703
    if (flags & FLAG_Z)
        goto c8703;
    //     cmp #0x20 ; ' '
    cmp(&flags, a, 0x20);
    //     beq c8703
    if (flags & FLAG_Z)
        goto c8703;
    // c86ff:
c86ff:
    //     inc l0083
    l0083++;
    set_flags(&flags, l0083);
    //     bne c8715
    if (!(flags & FLAG_Z))
        goto c8715;
    // c8703:
c8703:
    //     ldy l0083
    y = l0083;
    set_flags(&flags, y);
    //     beq c870d
    if (flags & FLAG_Z)
        goto c870d;
    //     inc tmp8
    tmp8++;
    set_flags(&flags, tmp8);
    //     bne c870d
    if (!(flags & FLAG_Z))
        goto c870d;
    //     inc tmp9
    tmp9++;
    // c870d:
c870d:
    //     stx l0083
    l0083 = x;
    //     cmp #0x0d
    cmp(&flags, a, 0x0d);
    //     bne c8715
    if (!(flags & FLAG_Z))
        goto c8715;
    //     stx l0082
    l0082 = x;
    // c8715:
c8715:
    //     ora l0082
    a |= l0082;
    //     sta l0082
    l0082 = a;
    //     inc tmp0
    tmp0++;
    set_flags(&flags, tmp0);
    //     bne c871f
    if (!(flags & FLAG_Z))
        goto c871f;
    // c871d:
c871d:
    //     inc tmp1
    tmp1++;
    // c871f:
c871f:
    //     ldy tmp1
    y = tmp1;
    //     cpy area_end_ptr+1
    cmp(&flags, y, (uint8_t)(area_end_ptr >> 8));
    //     bne c86b8
    if (!(flags & FLAG_Z))
        goto c86b8;
    //     ldy tmp0
    y = tmp0;
    //     cpy area_end_ptr
    cmp(&flags, y, (uint8_t)(area_end_ptr & 0xff));
    //     bne c86b8
    if (!(flags & FLAG_Z))
        goto c86b8;
    //     ldx tmp8
    x = tmp8;
    //     ldy tmp9
    y = tmp9;
    //     jsr render_number_to_screen
    render_number_to_screen();
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
    // edit_cmd
    //  Inputs: flags
    //  Outputs: a, x
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
    initialise_document();
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
    //  Inputs: flags
    //  Temps:  tmp89
    //  Outputs: a
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
    //     lda tmp8
    a = tmp8;
    //     cmp #0x1b
    cmp(&flags, a, 0x1b);
    //     bne c8699
    if (!(flags & FLAG_Z))
        goto c8699;
    //     jsr print_inline_string
    //     .ascii "Frump!"
    //     .byte 0xff
    cli_putstring("Frump!\n");
    return_to_cli_prompt();
    return;

    // c8699:
c8699:
    //     sta current_tab_key
    current_tab_key = a;
    // c869b:
c869b:
    //     jmp return_to_cli_prompt
    return_to_cli_prompt();
    return;
}

static void finish_cmd(void)
{
    // finish_cmd
    //  Inputs: flags
    //  Outputs: a, x
    // Pseudocode: Writes remaining document content to output file in chunks

    // ;
    // ***************************************************************************************
    // finish_cmd:
    //     jsr check_continuous_editing
    check_continuous_editing();
    // loop_c84ee:
loop_c84ee:
    //     jsr reset_area_to_entire_document
    reset_area_to_entire_document();
    //     jsr sanitise_area
    sanitise_area();

    //     jsr select_file
    x = 1;
    select_file();

    //     jsr write_area_to_file
    write_area_to_file();
    //     bne c84ab
    if (!(flags & FLAG_Z))
    {
        return_to_cli_prompt();
        return;
    }
    //     lda #0
    a = 0;
    //     jsr put_byte_to_file                ; write terminator
    put_byte_to_file();
    //     jsr sub_c89d3
    sub_c89d3();
    //     jsr move_cursor_to_top_of_document
    move_cursor_to_top_of_document();
    //     jsr cb05a
    cb05a();
    //     lda input_file_empty_flag
    a = input_file_empty_flag;
    //     bne close_input_output_files
    if (a != 0)
    {
        close_input_output_files();
        return;
    }
    //     jsr read_first_chunk_from_input_file
    read_first_chunk_from_input_file();
    //     beq c84ab
    if (flags & FLAG_Z)
    {
        return_to_cli_prompt();
        return;
    }
    //     bne loop_c84ee                                                    ;
    //     ALWAYS branch
    goto loop_c84ee;
}

static void fold_cmd(void)
{
    // fold_cmd
    //  Inputs: y, flags
    //  Outputs: a
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
    cmp(&flags, a, '0');
    //     bne c87b4 (not 0 or 1 → just show state)
    if (!(flags & FLAG_Z))
        goto c87b4;
    //     false → folding_flag = 0x80
    folding_flag = 0x80;
    // c87b4:
c87b4:
    //     jsr print_inline_string
    //     .ascii "Folding "
    //     .byte 0
    cli_putstring("Folding ");

    //     lda folding_flag
    a = folding_flag;
    set_flags(&flags, a);
    //     bpl c87cb
    if (!(flags & FLAG_N))
        goto c87cb;
    //     jsr print_inline_string
    //     .ascii "off"
    //     .byte 0xff
    cli_putstring("off\n");
    return_to_cli_prompt();
    return;

    // c87cb:
c87cb:
    //     jsr print_inline_string
    //     .ascii "on"
    //     .byte 0xff
    cli_putstring("on\n");
    return_to_cli_prompt();
    return;

    // c87d1:
c87d1:
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
    sanitise_area();
    //     beq c878b
    if (flags & FLAG_Z)
        goto c878b;
    //     lda area_start_ptr
    a = (uint8_t)(area_start_ptr & 0xff);
    //     ldy area_start_ptr+1
    y = (uint8_t)((area_start_ptr >> 8) & 0xff);
    //     jsr move_cursor_to_address
    move_cursor_to_address();
    //     jsr sub_caf5f
    sub_caf5f();
    //     lda #0x10
    a = 0x10;
    //     jsr wipe_buffer
    wipe_buffer();
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
    a = 0x2e;
    //     jsr bdos_print_char
    cli_putchar(a);
    //     lda current_line_ptr
    a = (uint8_t)(current_line_ptr & 0xff);
    //     ldy current_line_ptr+1
    y = (uint8_t)((current_line_ptr >> 8) & 0xff);
    //     cpy area_end_ptr+1
    cmp(&flags, y, (uint8_t)(area_end_ptr >> 8));
    //     bcc c876d
    if (!(flags & FLAG_C))
        goto c876d;
    //     bne c8787
    if (!(flags & FLAG_Z))
        goto c8787;
    //     cmp area_end_ptr
    cmp(&flags, a, (uint8_t)(area_end_ptr & 0xff));
    //     bcc c876d
    if (!(flags & FLAG_C))
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

static void load_cmd(void)
{
    // load_cmd
    //  Temps:  tmp01
    // load_cmd:
    //     jsr check_not_continuous_editing
    check_not_continuous_editing();
    //     jsr parse_filename_from_command
    parse_filename_from_command();
    //     jsr initialise_document
    initialise_document();
    top = page; // WORKAROUND: cb05a bumped top past the initial CR; need to
                // load at page, not page+1
    //     jsr reset_area_to_entire_document
    reset_area_to_entire_document();
    //     jsr 1f
    read_into_document();
    top = (addr_t)((uint16_t)tmp1 << 8) |
          tmp0; // WORKAROUND: adjust_pointers adds stale bytes from end of
                // ram[]; fix top
    //     jsr reset_document_name_after_load
    reset_document_name_after_load();
    //     jsr clear_cmd
    clear_cmd();
    //     jmp move_cursor_to_top_of_document
    move_cursor_to_top_of_document();
}

static void microspace_cmd(void)
{
    // microspace_cmd
    //  Inputs: flags
    //  Temps:  tmp89
    //  Outputs: a, x, y
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
    if (flags & FLAG_Z)
        goto c8608;
    //     ldx tmp8
    x = tmp8;
    //     beq return_7
    if (x == 0)
        return;
    // c8608:
c8608:
    //     ldy #0
    y = 0;
    //     lda #0x0c
    a = 0x0c;
    //     jsr call_printer_driver
    call_printer_driver();
    //     tya
    a = y;
    //     and #1
    a &= 1;
    set_flags(&flags, a);
    //     beq c8617
    if (flags & FLAG_Z)
        goto c8617;
    //     stx microspacing_flag
    microspacing_flag = x;
    // return_7:
return_7:
    //     rts
    return;

    // c8617:
c8617:
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
    //  Inputs: flags
    //  Outputs: a, x, y
    // Pseudocode: Appends more text from input file into document at current
    // cursor position

    // ;
    // ***************************************************************************************
    // more_cmd:
    //     jsr check_continuous_editing
    check_continuous_editing();
    //     jsr parse_marks_from_command
    parse_marks_from_command();
    //     lda area_start_ptr
    a = (uint8_t)(area_start_ptr & 0xff);
    //     ldy area_start_ptr+1
    y = (uint8_t)((area_start_ptr >> 8) & 0xff);
    //     jsr move_cursor_to_address
    move_cursor_to_address();

    //     jsr select_file
    x = 1;
    select_file();
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
loop_c84c4:
    //     lda (current_ruler_ptr),y
    a = ram[current_ruler_ptr + y];
    //     sta current_ruler_buffer,y
    current_ruler_buffer[y] = a;
    //     iny
    y++;
    //     dex
    x--;
    //     bne loop_c84c4
    if (x != 0)
        goto loop_c84c4;
    //     lda #0x0d
    a = 0x0d;
    //     sta current_ruler_buffer,y
    current_ruler_buffer[y] = a;
    //     jsr sub_c89d3
    sub_c89d3();
    //     jsr move_cursor_to_top_of_document
    move_cursor_to_top_of_document();
    //     jsr check_for_at_least_150_bytes_free
    check_for_at_least_150_bytes_free();
    //     lda input_file_empty_flag
    a = input_file_empty_flag;
    //     bne c84e8
    if (a != 0)
        goto c84e8;
    //     lda top
    a = (uint8_t)(top & 0xff);
    //     ldy top+1
    y = (uint8_t)((top >> 8) & 0xff);
    //     jsr read_next_chunk_from_input_file
    read_next_chunk_from_input_file();
    //     beq c84ab
    if (flags & FLAG_Z)
    {
        return_to_cli_prompt();
        return;
    }
    // c84e8:
c84e8:
    //     jmp cb05a
    cb05a();
}

static void name_cmd(void)
{
    // name_cmd
    //  Inputs: flags
    //  Outputs: a
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
    a = 0x80;
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
    read_into_document();
    //     jmp return_to_cli_prompt
    return_to_cli_prompt();
    return;
}

static void replace_cmd(void)
{
    // replace_cmd
    //  Inputs: a, flags
    //  Outputs: x, y
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
    move_cursor_to_address();
    //     jsr enter_editor_mode
    enter_editor_mode();
    // c832d:
c832d:
    //     jsr sub_c8361
    sub_c8361();
    //     ldx #0x52 ; 'R'
    x = 0x52;
    //     ldy #0x50 ; 'P'
    y = 0x50;
    //     jsr draw_prompt_characters
    draw_prompt_characters();
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
    cmp(&flags, a, 0x59);
    //     beq c8349
    if (flags & FLAG_Z)
        goto c8349;
    //     dex                                                               ;
    //     X=0xff
    x--;
    //     cmp #0x4f ; 'O'
    cmp(&flags, a, 0x4f);
    //     bne c8356
    if (!(flags & FLAG_Z))
        goto c8356;
    // c8349:
c8349:
    //     stx print_xpos
    print_xpos = x;
    //     jsr sub_c8371
    sub_c8371();
    //     jsr sub_c8a4f
    sub_c8a4f();
    //     bcs c836b
    if (flags & FLAG_C)
    {
        show_memory_full_error();
        esc_key();
        return;
    }
    //     jsr sub_c8361
    sub_c8361();
    // c8356:
c8356:
    //     jsr c8b7b
    c8b7b();
    //     bne return_2
    if (!(flags & FLAG_Z))
        return;
    //     jsr move_cursor_to_address
    move_cursor_to_address();
    //     jmp c832d
    goto c832d;
}

static void save_cmd_write_cmd(void)
{
    // save_cmd_write_cmd
    //  Inputs: a, flags
    //  Outputs: x
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
    sanitise_area();
    //     beq return_6
    if (flags & FLAG_Z)
        return;

    //     jsr open_output_file
    open_output_file();
    //     jsr write_area_to_file
    write_area_to_file();
    //     lda #0
    a = 0;
    //     jsr put_byte_to_file
    put_byte_to_file();

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
    //  Inputs: flags
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
    sanitise_area();
    //     beq c82fa
    if (flags & FLAG_Z)
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
    move_cursor_to_address();
    //     jmp enter_editor_mode
    enter_editor_mode();
    longjmp(env, JMP_EDITOR);
    return;

    // ;
    // ***************************************************************************************
}

static void setup_cmd(void)
{
    // setup_cmd
    //  Inputs: a, flags
    //  Outputs: x, y; tmp67, tmp89
    // Pseudocode: Parses flag letters and sets format_mode_flag,
    // justifying_flag, insert_mode_flag

    // ;
    // ***************************************************************************************
    // setup_cmd:
    //     ldx #1
    x = 1;
    //     stx tmp6
    tmp6 = x;
    //     dex                                                               ;
    //     X=0x00
    x--;
    //     stx tmp8
    tmp8 = x;
    //     dex                                                               ;
    //     X=0xff
    x--;
    //     stx tmp7
    tmp7 = x;
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
    cmp(&flags, a, ((const uint8_t[]){0x4e, 0x4a, 0x00, 0x49, 0x00})[x]);
    //     beq c8669
    if (flags & FLAG_Z)
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
    //     sta tmp6,x
    if (x == 0)
        tmp6 = a;
    else if (x == 1)
        tmp7 = a;
    else
        tmp8 = a;
    //     inc input_buffer_offset
    input_buffer_offset++;
    set_flags(&flags, input_buffer_offset);
    //     bne c8649
    if (!(flags & FLAG_Z))
        goto c8649;
    // c8672:
c8672:
    //     ldx #2
    x = 2;
    // loop_c8674:
loop_c8674:
    //     lda tmp6,x
    if (x == 0)
        a = tmp6;
    else if (x == 1)
        a = tmp7;
    else
        a = tmp8;
    //     sta format_mode_flag,x
    if (x == 0)
        format_mode_flag = a;
    else if (x == 1)
        justifying_flag = a;
    else
        insert_mode_flag = a;
    //     dex
    x--;
    set_flags(&flags, x);
    //     bpl loop_c8674
    if (!(flags & FLAG_N))
        goto loop_c8674;
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
    a = 0xc0;
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

static void print_x_words_of_help(void)
{
    // print_x_words_of_help
    //  Inputs: a
    //  Outputs: y
    // Pseudocode: Prints X words of the help string showing VIEW and version

    // ;
    // ***************************************************************************************
    // print_x_words_of_help:
    //     ldy #0
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
    //  Inputs: y, flags
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
    cmp(&flags, y, 48);
    //     bcc c826e
    if (!(flags & FLAG_C))
        goto c826e;
    // c8263:
c8263:
    //     jsr print_inline_string ; .ascii "Mistake\n"
    cli_putstring("Mistake\n");
    // c826e:
c826e:
    //     lda input_buffer_offset+1
    a = l0080;
    //     ldy #2
    y = 2;
    //     jsr call_through_jumptable
    execute_cli_command();
    //     jmp run_cli
    run_cli();
}

void cli_handler_impl(void)
{
    // cli_handler_impl
    //  Outputs: x
    // cli_handler_impl: Main CLI loop (called after setjmp reset)

    //     jsr stop_printing
    stop_printing();
    //     ldx #0xff
    x = 0xff;
    //     stx error_handling_mode
    error_handling_mode = 0xff;
    //     txs  (handled by setjmp/longjmp in main_)
    //     inx  ; X=0x00
    x = 0;
    //     stx print_flags
    print_flags = 0;
    //     jsr print_inline_string ; .ascii "=>"
    cli_putstring("=>");
    //     jsr readline
    readline();
    //     lda #<input_buffer
    //     sta tmp0
    //     ldx #>input_buffer
    //     stx tmp1
    // (tmp0/tmp1 no longer used as a pointer; parse_command reads
    // input_buffer[] directly)
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
    //  Inputs: a, flags
    //  Outputs: x, y
    screen_leave();
    // run_cli:
    //     jsr clear_screen
    clear_screen();
    //     ldx #1
    x = 1;
    //     jsr print_x_words_of_help
    print_x_words_of_help();
    //     jsr print_inline_string
    //     .ascii "\r\rBytes free "
    //     .byte 0
    cli_putstring("\n\nBytes free ");

    //     jsr compute_bytes_free
    compute_bytes_free();
    //     jsr render_number_to_screen
    render_number_to_screen();
    //     jsr bdos_print_newline
    cli_putchar('\n');
    //     jsr display_document_file_state
    display_document_file_state();
    //     bit file_edit_flags
    bit(&flags, a, file_edit_flags);
    //     bvs c816d
    if (flags & FLAG_V)
        goto c816d;
    //     lda file_edit_flags
    a = file_edit_flags;
    //     ror
    a = ror(&flags, a);
    //     bcc c816d
    if (!(flags & FLAG_C))
        goto c816d;
    //     jsr print_inline_string
    //     .ascii "Input file is "
    //     .byte 0
    cli_putstring("Input file is ");

    //     lda input_file_empty_flag
    a = input_file_empty_flag;
    //     bne c8163
    if (a != 0)
        goto c8163;
    //     jsr print_inline_string
    //     .ascii "not "
    //     .byte 0
    cli_putstring("not ");

    // c8163:
c8163:
    //     jsr print_inline_string
    //     .ascii "empty\r"
    //     .byte 0
    cli_putstring("empty\n");

    // c816d:
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
    if (flags & FLAG_Z)
        goto c81b3;
    //     jsr print_inline_string
    //     .ascii " (m)"
    //     .byte 0
    cli_putstring(" (m)");

    // c81b3:
c81b3:
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
    a = 0x2c;
    //     jsr screen_putchar
    screen_putchar(a);
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
    if (y == 0)
        goto c81f3;
    //     jsr bdos_print_newline
    cli_putchar('\n');
    // c81f3:
c81f3:
    //     jsr bdos_print_newline
    cli_putchar('\n');
    return_to_cli_prompt();
}

// CLI command parser
static void parse_command(void)
{
    // parse_command
    //  Inputs: flags
    //  Outputs: a, x, y
    //     .ascii "VIEW"
    //     .byte 0
    //     .ascii "B3.0 for CP/M-65"
    //     .byte 0

    // ;
    // ***************************************************************************************
    // parse_command:
    //     lda #0xff
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
    //     lda (tmp0),y
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
    cmp(&flags, a, l0084);
    //     beq loop_ca851
    if (flags & FLAG_Z)
        goto loop_ca851;
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
    //     lda (tmp0),y
    a = input_buffer[y];
    //     cmp #0x30 ; '0'
    cmp(&flags, a, 0x30);
    //     bcs ca84c
    if (flags & FLAG_C)
        goto ca84c;
    // ca87e:
ca87e:
    //     lda (tmp0),y
    a = input_buffer[y];
    //     cmp #0x30 ; '0'
    cmp(&flags, a, 0x30);
    //     bcs ca887
    if (flags & FLAG_C)
        goto ca887;
    //     sta l007e
    l007e = a;
    //     iny
    y++;
    // ca887:
ca887:
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
    parse_mark_from_command();
    //     beq return_11
    if (flags & FLAG_Z)
        return;
    //     sta area_start_ptr
    area_start_ptr = (area_start_ptr & 0xff00) | a;
    //     sty area_start_ptr+1
    area_start_ptr = (area_start_ptr & 0x00ff) | ((uint16_t)y << 8);
    //     jsr parse_mark_from_command
    parse_mark_from_command();
    //     beq return_11
    if (flags & FLAG_Z)
        return;
    //     sta area_end_ptr
    area_end_ptr = (area_end_ptr & 0xff00) | a;
    //     sty area_end_ptr+1
    area_end_ptr = (area_end_ptr & 0x00ff) | ((uint16_t)y << 8);
    // return_11:
    //     rts
}

void reset_document_name_after_load(void)
{
    // Pseudocode: Sets file_edit_flags to indicate a document is loaded

    // reset_document_name_after_load:
    //     lda #0x40 ; '@'
    a = 0x40;
    //     sta file_edit_flags
    file_edit_flags = a;
    // fall through to set_document_name_to_filename_buffer
    set_document_name_to_filename_buffer();

    // MULTIPLE ENTRY POINTS: name_cmd, reset_document_name_after_load
}

void set_document_name_to_filename_buffer(void)
{
    // set_document_name_to_filename_buffer
    //  Outputs: x
    // Pseudocode: Copies filename buffer to input filename buffer

    // set_document_name_to_filename_buffer:
    //     ldx #0
    x = 0;
    // loop_c88fa:
loop_c88fa:
    //     lda filename_buffer,x
    a = filename_buffer[x];
    //     sta input_filename,x
    input_filename[x] = a;
    //     inx
    x++;
    //     cmp #0x21
    cmp(&flags, a, 0x21);
    //     bge loop_c88fa
    if (flags & FLAG_C)
        goto loop_c88fa;
    // return_9:
return_9:
    //     lda #0x0d
    a = 0x0d;
    //     sta input_filename-1, x
    input_filename[x - 1] = a;
    //     rts
    return;

    // MULTIPLE ENTRY POINTS: also called directly from edit_cmd
}

void zero_terminate_filename_buffer(void)
{
    // zero_terminate_filename_buffer:
    //     ldx #0
    x = 0;
    //     lda #0x0d
    a = 0x0d;
    // zloop:
zloop:
    //     cmp filename_buffer, x
    cmp(&flags, a, filename_buffer[x]);
    //     zbreakif eq
    if (flags & FLAG_Z)
        goto zbreak;
    //     inx
    x++;
    //     bne zloop
    goto zloop;
zbreak:
    //     lda #0
    a = 0;
    //     sta filename_buffer, x
    filename_buffer[x] = a;
    //     rts
}

void parse_mark_from_command(void)
{
    // parse_mark_from_command
    //  Inputs: y, flags
    // parse_mark_from_command:
    //     jsr sub_c8e33
    sub_c8e33();
    //     beq return_12
    if (flags & FLAG_Z)
        return;
    //     iny
    y++;
    //     sty input_buffer_offset
    input_buffer_offset = y;
    //     jsr lookup_marker
    lookup_marker();
    //     bcs c89b3 / c89b3: jsr print_inline_string ; .ascii "Bad marker" ;
    //     .byte 0xff
    if (flags & FLAG_C)
    {
        cli_putstring("Bad marker\n");
        return_to_cli_prompt();
        return;
    }
    //     beq c89c1 / c89c1: jsr print_inline_string ; .ascii "Marker not set"
    //     ; .byte 0xff
    if (flags & FLAG_Z)
    {
        cli_putstring("Marker not set\n");
        return_to_cli_prompt();
        return;
    }
    //     lda markers_array,x
    a = (uint8_t)(markers_array[x] & 0xff);
    //     ldy markers_array+1,x
    y = (uint8_t)(markers_array[x] >> 8);
    set_flags(&flags, y);
    // return_12:
    //     rts
}
