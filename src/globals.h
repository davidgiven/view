#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <setjmp.h>

// ─── Constants
// ────────────────────────────────────────────────────────────────

#define MAX_LINE_LENGTH 132
#define ARRAY_SIZE(cur_ch) (sizeof(cur_ch) / sizeof((cur_ch)[0]))
#define MAX_COMMAND_LENGTH 68
#define JMP_CLI 1
#define JMP_EDITOR 2
#define RAM_MAX 0xffff
#define CTRL(c) ((uint8_t)((c) & 0x1f))
#define RULER_INDEX_SIZE 128
#define RAM_CURRENT_LINE_BUF 0x0545
#define RAM_EDIT_BUFFER 0x0548
#define RAM_JUST_BEFORE_RULER_BUF 0x05CC

// ─── Type definitions ───────────────────────────────────────────────────────

typedef enum
{
    NO_COMMAND_PREFIX = 0,
    COMMAND_PREFIX = 0x80, /* format command */
    RULER_PREFIX = 0x81,   /* ruler line */
} command_prefix_t;

typedef enum
{
    NO_CONTROL_CODE = 0,
    HIGHLIGHT1_CODE, /* 0x1c highlight 1 toggle */
    HIGHLIGHT2_CODE, /* 0x1d highlight 2 toggle */
} control_code_t;

typedef enum
{
    CLI_CMD_OK,        /** Command parsed and processed. */
    CLI_CMD_NO_TARGET, /** No command given. */
    CLI_CMD_NO_STRING  /** Area empty or no search string. */
} cli_cmd_status_t;

typedef enum
{
    AREA_NOT_EMPTY,
    AREA_EMPTY
} area_status_t;

typedef enum
{
    READ_BLOCK_EMPTY, /* Nothing was read. */
    READ_BLOCK_DONE,  /* Reached end of file. */
    READ_BLOCK_MORE   /* Block filled to limit, more data remains. */
} read_block_status_t;

typedef enum
{
    FORMAT_OK,         /** Formatting succeeded. */
    FORMAT_AT_END,     /** Reached end of document. */
    FORMAT_MEMORY_FULL /** Document write failed due to insufficient memory. */
} format_result_t;

typedef struct __attribute__((packed, aligned(1))) line
{
    uint8_t prefix_byte;
    uint8_t command[2];
    uint8_t text[MAX_LINE_LENGTH];
    uint8_t extra[3];
} line_t;

typedef struct edit_state
{
    uint8_t pos;
} edit_state_t;

typedef struct printer_driver
{
    void (*print_char)(uint8_t cur_ch);
    void (*printer_on)(void);
    void (*printer_off)(void);
    void (*printer_microspace)(void);
    void (*printer_getflags)(uint8_t* idx, uint8_t* pos);
} printer_driver_t;

typedef struct scan_state
{
    uint8_t ch;  // character found at the scan position
    uint8_t pos; // index of that character into input_buffer
} scan_state_t;

typedef struct pointer_array_t
{
    uint8_t* markers_array[6];
    uint8_t* area_start_ptr;
    uint8_t* area_end_ptr;
    uint8_t* doc_ptr1;
    uint8_t* doc_ptr2;
    uint8_t* doc_ptr3;
} pointer_array_t;

// ─── Global variables ───────────────────────────────────────────────────────

// ── Memory & heap ──────────────────────────────────────────────────────────

extern uint8_t ram[655360];
extern uint8_t *himem, *top, *page;
extern uint8_t* print_doc_ptr;
extern line_t* current_format_line;
extern line_t* heap_format_line_ptr;
extern uint8_t *scratch_line_ptr, *scratch_scan_ptr;
extern ptrdiff_t area_size;
extern jmp_buf env;
extern uint8_t* current_line_ptr;
extern uint8_t* top_of_screen_line_ptr;
extern uint8_t* macro_cursor_ptr;
extern uint8_t* edit_buffer_base;
extern uint8_t* oshwm;
extern uint8_t* ruler_index[RULER_INDEX_SIZE];
extern uint8_t* printer_ptr6;
extern uint8_t* doc_working_ptr;

// ── Document layout & ruler
// ──────────────────────────────────────────────────────────

extern uint8_t top_margin, bottom_margin;
extern uint8_t format_mode_flag, justifying_flag;
extern uint8_t highlight_code[2];
extern uint8_t header_text_maybe[0x42];
extern uint8_t ruler_right_stop, ruler_left_stop;
extern uint8_t search_target_len;
extern unsigned int register_value_array[26];
extern const printer_driver_t* printer_driver_ptr;
extern uint8_t* current_ruler_ptr;
extern uint8_t ruler_buffer_len;
extern pointer_array_t pointer_array;
#define markers_array pointer_array.markers_array
#define area_start_ptr pointer_array.area_start_ptr
#define area_end_ptr pointer_array.area_end_ptr
#define doc_ptr1 pointer_array.doc_ptr1
#define doc_ptr2 pointer_array.doc_ptr2
#define doc_ptr3 pointer_array.doc_ptr3
extern uint8_t current_ruler_buffer[133];
extern line_t current_line_buffer;
extern uint8_t printer_driver_name[];
extern int saved_ruler_index_scroll;

// ── Editor & screen state
// ──────────────────────────────────────────────────────────

extern uint8_t output_buffer[];
extern uint8_t column_position;
extern uint8_t line_counter;
extern uint8_t scratch_offset, scratch_index, screen_row, screen_column,
    temp_save;
extern uint8_t print_flags, folding_flag, macro_executing_flag;
extern uint8_t print_xpos;
extern uint8_t justify_gap_count;
extern uint8_t current_tab_key;
extern uint8_t microspacing_flag;
extern uint8_t insert_mode_flag;
extern uint8_t edit_buffer_unpacked_flag;
extern uint8_t visual_column;
extern uint8_t line_change_pending_flag;
extern uint8_t cursor_moved_flag;
extern uint8_t xpos;
extern uint8_t flags_need_redrawing_flag;
extern uint8_t delimiter_char;
extern uint8_t line_format_status;
extern uint8_t scroll_repeat_count;
extern uint8_t hscroll_pos;
extern uint8_t ypos;
extern uint8_t screen_maxrow;
extern uint8_t status_line_needs_redrawing_flag;
extern uint8_t edit_buffer_dirty_flag;
extern uint8_t display_start_row;
extern int ruler_index_ptr;
extern uint8_t screen_maxcolumn;

// ── File I/O & CLI ──────────────────────────────────────────────────────────

extern uint8_t input_filename[];
extern uint8_t output_filename[];
extern uint8_t file_edit_flags;
extern uint8_t input_file_empty_flag;
extern uint8_t filename_buffer[];
extern uint8_t input_buffer[];
extern uint8_t input_buffer_offset;
extern FILE* file_ptr;
extern FILE* input_fp;
extern FILE* output_fp;

// ── Printing & formatting
// ──────────────────────────────────────────────────────────

extern uint8_t printing_from_file_flag;

// ─── Function declarations ────────────────────────────────────────────────

// ── Utility ──────────────────────────────────────────────────────────

extern command_prefix_t check_for_command_prefix(uint8_t ch);
extern control_code_t check_for_control_code(uint8_t cur_ch);
extern void render_number_to_screen(int val);
extern uint8_t upper_case_unless_folding(uint8_t ch);
extern bool parse_decimal_number(int* value, uint8_t* pos);
extern void display_not_enough_memory(void);
extern void beep(void);
extern void wipe_buffer(uint8_t fill_value, uint8_t* target_ptr);
extern void draw_prompt_characters(uint8_t first_char, uint8_t second_char);
extern void show_memory_full_error(void);
extern void bad_filename_error(void);
extern void clear_screen(void);

// ── Document handling
// ──────────────────────────────────────────────────────────

extern bool check_area_memory(uint8_t* doc_line_ptr);
extern void setup_area_pointers(uint8_t* doc_line_ptr);
extern void set_document_name_to_filename_buffer(void);
extern bool read_first_chunk_from_input_file(void);
extern void check_continuous_editing(void);
extern area_status_t sanitise_area(void);
extern void parse_marks_from_command(scan_state_t* scan);
extern void write_area_to_file(void);
extern bool read_next_chunk_from_input_file(uint8_t* target_ptr);
extern uint8_t* read_into_document(void);
extern void reset_document_name_after_load(void);
extern bool scan_document_for_next_line(void);
extern uint8_t process_current_document_character(uint8_t* target_ptr,
    uint8_t* char_width_out,
    uint8_t* pos_inout,
    bool* is_tab);
extern void check_not_continuous_editing(void);
extern void adjust_area_pointers(ptrdiff_t area_delta);

// ── Editor ──────────────────────────────────────────────────────────

extern void redraw_and_write_back(void);
extern void esc_key(void);
extern void run_editor(void);
extern void clear_cmd(void);
extern void enter_editor_mode(void);
extern void clear_format_mode_bit7(void);
extern void redraw_editor(void);
extern void write_line_back_to_document_safely(void);
extern void clamp_ptr6_to_document(void);
extern bool make_space_for_insertion(uint8_t* insert_ptr, ptrdiff_t size_delta);
extern uint8_t* adjust_pointers(uint8_t* insert_ptr, ptrdiff_t size_delta);
extern void check_for_embedded_ruler(uint8_t* target_ptr);

// ── Printing ──────────────────────────────────────────────────────────

extern bool parse_optional_filename_from_command(scan_state_t* scan);
extern read_block_status_t read_block_from_file(
    uint8_t** cursor, uint8_t* limit);
extern bool scan_input_buffer(uint8_t* buffer, scan_state_t* state);
extern format_result_t format_paragraph(void);
extern void print_document(scan_state_t* scan);

// ── CLI & command parsing
// ──────────────────────────────────────────────────────────

extern bool reset_command_parse_state(scan_state_t* scan);
extern cli_cmd_status_t process_cli_command(scan_state_t* scan);
extern void parse_filename_from_command(scan_state_t* scan);
extern bool parse_integer_from_command(scan_state_t* scan, int* out);
extern void zero_terminate_filename_buffer(void);
extern void file_not_found_error(void);
extern void file_error(void);
extern uint8_t* parse_mark_from_command(scan_state_t* scan);

#endif
