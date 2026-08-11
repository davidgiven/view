#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "globals.h"
#include "io.h"

extern command_prefix_t check_for_command_prefix(uint8_t ch);
extern control_code_t check_for_control_code(uint8_t a);
extern int compute_bytes_free(void);
extern command_prefix_t deref_and_check_for_command_prefix(
    uint8_t y, addr_t tmp01);
extern void display_document_file_state(void);
extern void find_margins_of_current_ruler_buffer(void);
extern void print_char(uint8_t a);
extern void print_char_just_to_screen(uint8_t a);
extern void process_document_character(void);
extern void read_char(void);
extern void return_to_cli_prompt(void);

extern void print_alignment_spaces(uint8_t a);
extern void check_for_at_least_150_bytes_free(void);

extern void load_current_ruler(uint8_t y);

extern void ensure_cr_at_document_top(void);

extern void close_file(void);

extern uint8_t create_default_ruler(uint16_t ruler_addr);

extern uint8_t get_byte_from_file(void);

extern void get_register_address(uint8_t a);

extern uint8_t initialise_document(void);

extern void lookup_marker(uint8_t a);

extern void move_cursor_to_address(uint16_t addr);

extern void move_cursor_to_top_of_document(void);

extern void move_tmp01_to_next_line(uint16_t start);

extern void move_tmp01_to_previous_line(uint16_t val);

extern void open_input_file(void);

extern void open_output_file(void);

extern void pop_from_ruler_index(void);

extern void push_onto_ruler_index(uint8_t y, addr_t tmp01);

extern void reset_area_to_entire_document(void);

extern void find_next_line(addr_t line);

#endif
