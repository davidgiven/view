#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include <stdbool.h>
#include <setjmp.h>
#include <stdio.h>

typedef uint16_t addr_t;

// A macro definition stored in ram[].  Macros form a singly-linked list: a
// node holds the address of the next macro (0 terminates the list) and the
// two-character macro name; the macro body follows the header and is reached
// through body[].
struct macro
{
    addr_t next;
    char name[2];
    uint8_t body[0];
};

// Result of check_for_command_prefix / deref_and_check_for_command_prefix
typedef enum
{
    NO_COMMAND_PREFIX = 0,
    COMMAND_PREFIX, /* 0x80 format command */
    RULER_PREFIX,   /* 0x81 ruler line */
} command_prefix_t;

// Command-prefix byte values (first byte of a format command / ruler line)
#define COMMAND_BYTE 0x80
#define RULER_BYTE 0x81

// Result of check_for_control_code
typedef enum
{
    NO_CONTROL_CODE = 0,
    HIGHLIGHT1_CODE, /* 0x1c highlight 1 toggle */
    HIGHLIGHT2_CODE, /* 0x1d highlight 2 toggle */
} control_code_t;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define MAX_LINE_LENGTH 132
#define MAX_COMMAND_LENGTH 68
#define JMP_CLI 1
#define JMP_EDITOR 2

// 6502 CPU register globals (a, x, y) removed — all functions now use locals.

// Encapsulates the scan-offset scratch value (the 6502's Y register) that
// the line-navigation helpers pass between themselves and their callers.
struct edit_state
{
    uint8_t y;
};

extern uint8_t ram[65536];

// Printer driver struct
struct printer_driver
{
    void (*print_char)(uint8_t a);
    void (*printer_on)(void);
    void (*printer_off)(void);
    void (*printer_microspace)(void);
    void (*printer_getflags)(uint8_t* x, uint8_t* y);
};

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
extern struct macro *first_macro_ptr, *last_macro_ptr;
extern addr_t ptr5, current_format_line_ptr;
extern uint8_t header_text_maybe[0x42];
extern uint8_t footer_text_maybe[0x42];
extern uint8_t output_buffer[];
extern uint8_t formatted_line_written_flag;
extern uint8_t l0039;
extern uint8_t l0080, l0081, l0082, l0083, l0084;
extern addr_t tmp01, tmp23, tmp45, tmp67, tmp89;

extern uint8_t print_flags, folding_flag, macro_executing_flag;
extern uint8_t ruler_right_stop, ruler_left_stop;

extern command_prefix_t check_for_command_prefix(uint8_t ch);
extern control_code_t check_for_control_code(uint8_t a);
extern void render_number_to_screen(uint16_t val);

// Functions in view.c called by other modules
// scan_input_buffer: result of the input-buffer scan.  ch is the character
// found (first non-delimiter char, or 0x0d at end of line) and pos is its index
// into input_buffer.  See scan_input_buffer in printing.c for details.
struct scan_state
{
    uint8_t ch;  // character found at the scan position
    uint8_t pos; // index of that character into input_buffer
};

extern bool reset_command_parse_state(struct scan_state* scan);

typedef enum
{
    CLI_CMD_OK,        // command parsed and processed (was C=0, Z=0)
    CLI_CMD_NO_TARGET, // no command given (was C=0, Z=1)
    CLI_CMD_NO_STRING  // area empty / no search string (was C=1, Z=1)
} cli_cmd_status_t;

extern cli_cmd_status_t process_cli_command(struct scan_state* scan);
extern bool check_area_memory(addr_t ptr2);
extern void redraw_and_write_back(void);
extern void setup_area_pointers(addr_t ptr2);
extern void esc_key(void);
extern void parse_filename_from_command(struct scan_state* scan);
extern void set_document_name_to_filename_buffer(void);
extern bool read_first_chunk_from_input_file(void);
extern void check_continuous_editing(void);

typedef enum
{
    AREA_NOT_EMPTY,
    AREA_EMPTY
} area_status_t;

extern area_status_t sanitise_area(void);
extern void parse_marks_from_command(struct scan_state* scan);
extern void write_area_to_file(void);
extern bool read_next_chunk_from_input_file(addr_t ptr);
extern addr_t read_into_document(void);
extern void reset_document_name_after_load(void);
extern bool parse_integer_from_command(struct scan_state* scan, int* out);
extern uint8_t l0021, l0031, l0038, l007a;

#define RAM_CURRENT_RULER_BUF 0x05CF
#define RAM_MAX 0xffff

// Emulated register values for A-Z (originally stored in 6502 RAM at 0x0798)
extern unsigned int register_value_array[26];

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
extern uint8_t delimiter_char;
extern uint8_t line_format_status;
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
extern addr_t ruler_index[128];
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
extern void clear_format_mode_bit7(void);
extern void zero_terminate_filename_buffer(void);
extern void file_not_found_error(void);
extern void file_error(void);

// Functions moved to editor.c still called from view.c/other modules
extern void redraw_editor(void);
extern void write_line_back_to_document_safely(void);
extern void clamp_ptr6_to_document(void);
extern uint8_t upper_case_unless_folding(uint8_t a);
extern area_status_t sanitise_area(void);
extern bool make_space_for_insertion(addr_t tmp45, addr_t tmp67);
extern addr_t adjust_pointers(addr_t tmp45, addr_t tmp67);
extern bool parse_decimal_number(int* value, uint8_t* y);
extern bool parse_optional_filename_from_command(struct scan_state* scan);

typedef enum
{
    READ_BLOCK_EMPTY, /* Z set: nothing was read (l0082 == 0) */
    READ_BLOCK_DONE,  /* C set: reached end of file, all data read */
    READ_BLOCK_MORE   /* neither C nor Z: block filled to limit, more data */
} read_block_status_t;

extern read_block_status_t read_block_from_file(addr_t* cursor, addr_t limit);

extern bool scan_input_buffer(uint8_t* buffer, struct scan_state* state);
extern void check_for_embedded_ruler(addr_t tmp01);

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
extern addr_t editor_ptr6;
extern addr_t printer_ptr6;
extern uint8_t parser_table[];

extern uint8_t l0049;
extern addr_t ptr2;

extern bool scan_document_for_next_line(void);
extern uint8_t process_current_document_character(
    addr_t tmp01, uint8_t* x, uint8_t* y, bool* is_tab);
extern void check_not_continuous_editing(void);
extern void adjust_area_pointers(addr_t tmp67);
extern void wipe_buffer(uint8_t a, addr_t ptr1);

// Result of format_paragraph, conveying the 6502 exit flags explicitly:
// FORMAT_MEMORY_FULL is V=1 (document write failed); FORMAT_AT_END is C=1
// (advance_to_next_line reached the end of the document).
typedef enum
{
    FORMAT_OK,
    FORMAT_AT_END,
    FORMAT_MEMORY_FULL
} format_result_t;

extern format_result_t format_paragraph(void);
extern void print_document(struct scan_state* scan);
extern void draw_prompt_characters(uint8_t x, uint8_t y);
extern void show_memory_full_error(void);
extern void bad_filename_error(void);
extern void clear_screen(void);
extern addr_t parse_mark_from_command(struct scan_state* scan);
#endif
