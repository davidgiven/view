#ifndef EDITOR_H
#define EDITOR_H

#include "globals.h"

extern void editor_loop_impl(void);
extern void return_to_editor_loop(void);
extern void esc_key(void);
extern void return_key(void);
extern void f13_right_key(void);

// Functions in view.c called by editor key handlers
extern uint8_t create_default_ruler(uint8_t* ruler_addr);
extern bool find_next_line(uint8_t* start, uint8_t** line_ptr, uint8_t* y);
extern bool find_previous_line(uint8_t* val, uint8_t** line_ptr);
extern bool advance_to_next_line(uint8_t* line, uint8_t** line_ptr, uint8_t* y);
extern void run_cli(void);
extern void move_cursor_to_address(uint8_t* addr);

// Additional view.c functions called by moved editor helpers
extern int lookup_marker(uint8_t a);

extern void split_line_at_wrap(uint8_t* ptr);
extern bool insert_edit_buffer_bytes_at_xpos(uint8_t x);
extern void set_marker_to_here(uint8_t x);
extern void set_format_mode_bit7(void);
extern uint8_t justify_edit_buffer(uint8_t* ptr);
extern uint8_t create_default_ruler(uint8_t* ruler_addr);
#endif
