#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "globals.h"
#include "io.h"

typedef enum marker_lookup_result_t
{
    MARKER_INVALID = -1
} marker_lookup_result_t;

extern area_status_t sanitise_area(void);
extern bool advance_to_next_line( uint8_t* line, uint8_t** line_ptr, uint8_t* pos);
extern bool check_area_memory(uint8_t* doc_working_ptr);
extern bool find_next_line(uint8_t* start, uint8_t** line_ptr, uint8_t* pos);
extern bool find_previous_line(uint8_t* val, uint8_t** line_ptr);
extern bool make_space_for_insertion(uint8_t* insert_ptr, ptrdiff_t size_delta);
extern bool read_first_chunk_from_input_file(void);
extern bool read_next_chunk_from_input_file(uint8_t* target_ptr);
extern bool write_line_back_to_document(void);
extern command_prefix_t deref_and_check_for_command_prefix( uint8_t pos, uint8_t* target_ptr);
extern int compute_bytes_free(void);
extern int lookup_marker(uint8_t cur_ch);
extern read_block_status_t read_block_from_file( uint8_t** cursor, uint8_t* limit);
extern uint8_t create_default_ruler(uint8_t* ruler_addr);
extern uint8_t find_marker_at_position(uint8_t buf_offset, uint8_t* target_ptr);
extern uint8_t get_line_length(void);
extern uint8_t* adjust_pointers(uint8_t* insert_ptr, ptrdiff_t size_delta);
extern uint8_t* read_into_document(void);
extern unsigned int* get_register_address(uint8_t cur_ch);
extern void adjust_area_pointers(ptrdiff_t size_delta);
extern void check_for_at_least_150_bytes_free(void);
extern void close_file(void);
extern void display_document_file_state(void);
extern void ensure_cr_at_document_top(void);
extern void find_margins_of_current_ruler_buffer(void);
extern void initialise_document(void);
extern void load_current_ruler(int pos);
extern void move_cursor_to_address(uint8_t* addr);
extern void move_cursor_to_top_of_document(void);
extern void open_input_file(void);
extern void open_output_file(void);
extern void pop_from_ruler_index(void);
extern void push_onto_ruler_index(uint8_t* target_ptr);
extern void reset_area_to_entire_document(void);
extern void set_marker_to_here(uint8_t marker_idx);
extern void setup_area_pointers(uint8_t* doc_working_ptr);
extern void split_line_at_wrap(uint8_t* target_ptr);
extern void update_markers_to_format_buffer(void);
extern void write_area_to_file(void);
extern void write_line_back_to_document_safely(void);

#endif
