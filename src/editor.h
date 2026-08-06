#ifndef EDITOR_H
#define EDITOR_H

#include "globals.h"

extern void editor_loop_impl(void);
extern void return_to_editor_loop(void);
extern void esc_key(void);
extern uint8_t return_key(void);
extern void f13_right_key(void);

// Functions in view.c called by editor key handlers
extern uint8_t create_default_ruler(uint16_t ruler_addr);
extern void move_tmp01_to_next_line(uint16_t start);
extern void move_tmp01_to_previous_line(uint16_t val);
extern void find_next_line(uint8_t a);
extern void run_cli(void);
extern void move_cursor_to_address(uint16_t addr);

// Additional view.c functions called by moved editor helpers
extern void lookup_marker(uint8_t a);

extern void split_line_at_wrap(addr_t tmp89);
extern void insert_edit_buffer_bytes_at_xpos(uint8_t x);
extern void set_marker_to_here(uint8_t x);
extern void set_format_mode_bit7(void);
extern void justify_edit_buffer(void);
extern uint8_t create_default_ruler(uint16_t ruler_addr);
#endif
