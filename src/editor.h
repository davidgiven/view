#ifndef EDITOR_H
#define EDITOR_H

#include "globals.h"

extern void editor_loop_impl(void);
extern void return_to_editor_loop(void);
extern void esc_key(void);
extern uint8_t return_key(void);
extern void f13_right_key(void);

// Functions in view.c called by editor key handlers
extern uint8_t create_default_ruler(addr_t ruler_addr);
extern bool find_next_line(addr_t start, addr_t* tmp01);
extern bool find_previous_line(addr_t val, addr_t* tmp01);
extern bool advance_to_next_line(addr_t line, addr_t* tmp01);
extern void run_cli(void);
extern void move_cursor_to_address(uint16_t addr);

// Additional view.c functions called by moved editor helpers
extern int lookup_marker(uint8_t a);

extern void split_line_at_wrap(addr_t tmp89);
extern bool insert_edit_buffer_bytes_at_xpos(uint8_t x);
extern void set_marker_to_here(uint8_t x);
extern void set_format_mode_bit7(void);
extern uint8_t justify_edit_buffer(addr_t ptr1);
extern uint8_t create_default_ruler(addr_t ruler_addr);
#endif
