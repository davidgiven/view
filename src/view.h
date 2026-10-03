#ifndef VIEW_H
#define VIEW_H

#include "globals.h"

#include <setjmp.h>
#include <stdio.h>

// ─── Global variables (defined in view.c) ───────────────────────────────

// ── Memory & heap ──────────────────────────────────────────────────────────

extern uint8_t* ram;
extern uint8_t *himem, *top;
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
extern uint8_t* ruler_index[RULER_INDEX_SIZE];
extern uint8_t* print_source_ptr;
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
#define area_insert_ptr pointer_array.area_insert_ptr
#define search_cursor_ptr pointer_array.search_cursor_ptr
#define search_limit_ptr pointer_array.search_limit_ptr
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
extern uint8_t cli_header_limit;
extern uint8_t cli_header_pos;
extern uint8_t cli_output_pos;

// ─── Function declarations (defined in view.c) ─────────────────────────

extern bool parse_decimal_number(int* value, uint8_t* pos);
extern bool scan_input_buffer(uint8_t* buffer, scan_state_t* state);
extern command_prefix_t check_for_command_prefix(uint8_t ch);
extern control_code_t check_for_control_code(uint8_t cur_ch);
extern uint8_t process_document_character(
    uint8_t cur_ch, uint8_t* idx, bool* is_tab);
extern uint8_t upper_case_unless_folding(uint8_t ch);
extern void beep(void);
extern void display_not_enough_memory(void);
extern void draw_prompt_characters(uint8_t first_char, uint8_t second_char);
extern void print_alignment_spaces(uint8_t cur_ch);
extern void print_char(uint8_t cur_ch);
extern void render_number_to_screen(int val);
extern void render_register(uint8_t cur_ch, uint8_t idx);
extern void return_to_cli_prompt(void);
extern void run_view(void);
extern void wipe_buffer(uint8_t fill_value, uint8_t* target_ptr);

#endif
