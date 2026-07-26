#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "globals.h"
#include "io.h"

extern uint8_t check_for_command_prefix(uint8_t ch);
extern void check_for_control_code(void);
extern void compute_bytes_free(void);
extern uint8_t deref_and_check_for_command_prefix(uint8_t y);
extern void display_document_file_state(void);
extern void find_margins_of_current_ruler_buffer(void);
extern void print_char(void);
extern void print_char_just_to_screen(void);
extern void process_document_character(void);
extern void read_char(void);
extern void return_to_cli_prompt(void);
extern void stop_printing(void);
extern void sub_c9445(void);
extern void sub_cab6e(void);
extern void sub_cadf0(void);
extern void check_for_at_least_150_bytes_free(void);

extern void cab91(void);

extern void cb05a(void);

extern void close_file(void);

extern void create_default_ruler(void);

extern void get_byte_from_file(void);

extern void get_register_address(void);

extern void initialise_document(void);

extern void lookup_marker(void);

extern void move_cursor_to_address(void);

extern void move_cursor_to_top_of_document(void);

extern void move_tmp01_to_next_line(void);

extern void move_tmp01_to_previous_line(void);

extern void open_input_file(void);

extern void open_output_file(void);

extern void pop_from_ruler_index(void);

extern void push_onto_ruler_index(void);

extern void put_byte_to_file(uint8_t a);

extern void reset_area_to_entire_document(void);

extern void select_file(uint8_t x);

extern void sub_cab1a(void);

#endif
