#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include <stdbool.h>
#include <setjmp.h>
#include <stdio.h>

typedef uint16_t addr_t;

// Flag bit masks
#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_N 0x80
#define FLAG_V 0x40

#define MAX_LINE_LENGTH 132
#define MAX_COMMAND_LENGTH 68
#define JMP_CLI 1
#define JMP_EDITOR 2

// 6502 CPU register globals
extern uint8_t a, x, y, flags;
extern uint8_t ram[65536];

// Printer driver struct
struct printer_driver
{
    void (*print_char)(uint8_t a);
    void (*printer_on)(void);
    void (*printer_off)(void);
    void (*printer_microspace)(void);
    void (*printer_getflags)(void);
};

// Inline helpers
static inline void set_flags(uint8_t* flags, uint8_t v)
{
    *flags =
        (*flags & ~(FLAG_Z | FLAG_N)) | (v == 0 ? FLAG_Z : 0) | (v & FLAG_N);
}

static inline void cmp(uint8_t* flags, uint8_t reg, uint8_t value)
{
    uint16_t tmp_ = (uint16_t)reg - value;
    *flags = (*flags & ~(FLAG_Z | FLAG_N | FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) |
             ((uint8_t)tmp_ & FLAG_N) | ((uint16_t)reg >= value ? FLAG_C : 0);
}

static inline uint8_t adc(uint8_t* flags, uint8_t a, uint8_t value)
{
    uint16_t tmp_ = (uint16_t)a + value + (*flags & FLAG_C ? 1 : 0);
    *flags = (*flags & ~FLAG_C) | (tmp_ > 0xff ? FLAG_C : 0);
    a = (uint8_t)tmp_;
    set_flags(flags, a);
    return a;
}

static inline uint8_t sbc(uint8_t* flags, uint8_t a, uint8_t value)
{
    uint16_t tmp_ = (uint16_t)a - value - (1 - (*flags & FLAG_C ? 1 : 0));
    *flags = (*flags & ~FLAG_C) | (tmp_ <= 0xff ? FLAG_C : 0);
    a = (uint8_t)tmp_;
    set_flags(flags, a);
    return a;
}

static inline void bit(uint8_t* flags, uint8_t a, uint8_t value)
{
    uint8_t tmp_ = a & value;
    *flags = (*flags & ~(FLAG_Z | FLAG_N | FLAG_V)) | (tmp_ == 0 ? FLAG_Z : 0) |
             (value & (FLAG_N | FLAG_V));
}

static inline uint8_t rol(uint8_t* flags, uint8_t value)
{
    uint8_t c_in = (*flags & FLAG_C) ? 1 : 0;
    *flags = (*flags & ~FLAG_C) | ((value & 0x80) ? FLAG_C : 0);
    value = (value << 1) | c_in;
    set_flags(flags, value);
    return value;
}

static inline uint8_t ror(uint8_t* flags, uint8_t value)
{
    uint8_t c_in = (*flags & FLAG_C) ? 0x80 : 0;
    *flags = (*flags & ~FLAG_C) | ((value & 0x01) ? FLAG_C : 0);
    value = (value >> 1) | c_in;
    set_flags(flags, value);
    return value;
}

static inline uint8_t asr(uint8_t* flags, uint8_t value)
{
    *flags = (*flags & ~FLAG_C) | ((value & 0x01) ? FLAG_C : 0);
    value >>= 1;
    set_flags(flags, value);
    return value;
}

// Global variables (shared between view.c and printing.c)
extern uint8_t left_margin, line_spacing, top_margin, bottom_margin;
extern uint8_t header_margin, footer_margin, page_length;
extern uint8_t two_sided_flag, rhs_extra_margin;
extern uint8_t footers_enabled_flag, headers_enabled_flag;
extern uint8_t format_mode_flag, justifying_flag;
extern uint8_t highlight_code[2];
#define highlight1_code highlight_code[0]
#define highlight2_code highlight_code[1]
extern addr_t himem, top, page;
extern addr_t first_macro_ptr, last_macro_ptr;
extern addr_t ptr5, current_format_line_ptr;
extern uint8_t header_text_maybe[0x42];
extern uint8_t footer_text_maybe[0x42];
extern uint8_t output_buffer[];
extern uint8_t l0030, l0039;
extern uint8_t l0080, l0081, l0082, l0083, l0084;
extern addr_t tmp01, tmp23, tmp45, tmp67, tmp89;

extern uint8_t print_flags, folding_flag, macro_executing_flag;
extern uint8_t ruler_right_stop, ruler_left_stop;

extern uint8_t check_for_command_prefix(uint8_t ch);
extern void check_for_control_code(uint8_t a);
extern void render_number_to_screen(uint16_t val);
extern void print_char_via_putchar(uint8_t a);
// Functions in view.c called by other modules
extern void sub_c8412(void);
extern void sub_c8c7c(void);
extern void sub_c83f0(void);
extern void sub_c8a4f(addr_t ptr2);
extern void sub_c8361(addr_t ptr6);
extern void sub_c8371(addr_t ptr2, addr_t ptr6);
extern void esc_key(void);
extern void parse_filename_from_command(void);
extern void set_document_name_to_filename_buffer(void);
extern void read_first_chunk_from_input_file(void);
extern void check_continuous_editing(void);

typedef enum
{
    AREA_NOT_EMPTY,
    AREA_EMPTY
} area_status_t;

extern area_status_t sanitise_area(void);
extern void parse_marks_from_command(void);
extern void write_area_to_file(void);
extern void read_next_chunk_from_input_file(void);
extern uint8_t read_into_document(void);
extern void reset_document_name_after_load(void);
extern void parse_integer_from_command(void);
extern uint8_t l0021, l0031, l0038, l007a;
#define RAM_REGISTER_VALUE_P (RAM_REGISTER_VALUE_ARRAY + ('P' - 'A') * 2)
#define RAM_REGISTER_VALUE_L (RAM_REGISTER_VALUE_ARRAY + ('L' - 'A') * 2)

#define RAM_REGISTER_VALUE_ARRAY 0x0798
#define register_value_array (&ram[RAM_REGISTER_VALUE_ARRAY])
#define RAM_CURRENT_RULER_BUF 0x05CF

extern jmp_buf env;
extern const struct printer_driver* printer_driver_ptr;
extern uint8_t print_xpos;
extern uint8_t input_filename[];
extern uint8_t output_filename[];
extern uint8_t file_edit_flags;
extern addr_t current_ruler_ptr;
extern addr_t current_line_ptr;
extern addr_t top_of_screen_line_ptr;
extern uint8_t l003a;
extern uint8_t l0046;
extern uint8_t input_file_empty_flag;
extern uint8_t filename_buffer[];
extern uint8_t current_tab_key;
extern uint8_t input_buffer[];
extern uint8_t microspacing_flag;
extern uint8_t input_buffer_offset;
extern uint8_t insert_mode_flag;
extern addr_t ptr3;

// Pointer array struct (markers, area pointers, doc pointers)
struct pointer_array_t
{
    addr_t markers_array[6];
    addr_t area_start_ptr;
    addr_t area_end_ptr;
    addr_t doc_ptr1;
    addr_t doc_ptr2;
    addr_t doc_ptr3;
};
extern struct pointer_array_t pointer_array;
#define markers_array pointer_array.markers_array
#define area_start_ptr pointer_array.area_start_ptr
#define area_end_ptr pointer_array.area_end_ptr
#define doc_ptr1 pointer_array.doc_ptr1
#define doc_ptr2 pointer_array.doc_ptr2
#define doc_ptr3 pointer_array.doc_ptr3

#define RAM_CURRENT_RULER_BUF 0x05CF
#define current_ruler_buffer (&ram[RAM_CURRENT_RULER_BUF])

extern uint8_t error_handling_mode;
extern uint8_t printer_driver_name[];
extern void run_editor(void);

extern uint8_t edit_buffer_unpacked_flag;
extern uint8_t l0072;
extern uint8_t l0079;
extern uint8_t cursor_moved_flag;
extern uint8_t xpos;
extern uint8_t flags_need_redrawing_flag;
#define CTRL(c) ((uint8_t)((c) & 0x1f))

extern uint8_t l0074;
extern uint8_t l0084;
extern uint8_t l003b;
extern uint8_t l007e;
extern uint8_t l0083;
extern uint8_t l006f;
extern uint8_t l0033;
extern uint8_t l0034;
extern uint8_t l003d;
extern uint8_t hscroll_pos;
extern uint8_t ypos;
extern uint8_t screen_maxrow;
extern uint8_t status_line_needs_redrawing_flag;
extern addr_t ptr1;
extern uint8_t edit_buffer_dirty_flag;
extern uint8_t line_lengths[];

extern uint8_t l0073;

extern uint8_t ruler_index_ptr;
extern addr_t oshwm;
extern uint8_t l0076;
#define RAM_CURRENT_LINE_BUF 0x0545
#define RAM_EDIT_BUFFER 0x0548
#define RAM_JUST_BEFORE_RULER_BUF 0x05CC
extern uint8_t screen_maxcolumn;
extern FILE* file_ptr;
extern FILE* input_fp;
extern FILE* output_fp;
extern void clear_cmd(void);
extern void enter_editor_mode(void);
extern void sub_caf5f(void);
extern void zero_terminate_filename_buffer(void);
extern void file_not_found_error(void);
extern void file_error(void);

// Functions moved to editor.c still called from view.c/other modules
extern void redraw_editor(addr_t ptr6);
extern void write_line_back_to_document_safely(void);
extern void ca741(addr_t ptr6);
extern uint8_t sub_c8c5f(void);
extern area_status_t sanitise_area(void);
extern void make_space_for_insertion(void);
extern void adjust_pointers(addr_t tmp45, addr_t tmp67);
extern void parse_decimal_number(void);
extern void parse_optional_filename_from_command(void);
extern void read_block_from_file(void);
extern void sub_c8e33(void);
extern uint8_t sub_cac41(addr_t tmp01);

// Functions moved to printing.c still called from view.c/other modules
extern void display_not_enough_memory(void);
extern void beep(void);

// Missing printing globals
extern uint8_t rw_file_handle;
extern uint8_t l0044;
extern uint8_t l0045;
extern uint8_t l0047;
extern uint8_t l0048;
extern uint8_t l0042;
extern uint8_t l0043;
extern uint8_t printing_from_file_flag;
extern addr_t ptr6;
extern uint8_t parser_table[];

extern uint8_t l0049;
extern addr_t ptr2;

extern void c8b7b(void);
extern void process_current_document_character(void);
extern void check_not_continuous_editing(void);
extern uint8_t sub_c89d3(addr_t tmp67);
extern void wipe_buffer(uint8_t a);
extern void sub_c9977(void);
extern void print_document(void);
extern uint8_t draw_prompt_characters(uint8_t x, uint8_t y);
extern void show_memory_full_error(void);
extern void bad_filename_error(void);
extern void clear_screen(void);
extern addr_t parse_mark_from_command(void);
#endif
