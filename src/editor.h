#ifndef EDITOR_H
#define EDITOR_H

#include "globals.h"

extern void editor_loop_impl(void);
extern void return_to_editor_loop(void);
extern void esc_key(void);
extern void return_key(void);
extern void f13_right_key(void);

// Functions in view.c called by editor key handlers
extern void create_default_ruler(uint16_t ruler_addr);
extern void move_tmp01_to_next_line(uint16_t start);
extern void move_tmp01_to_previous_line(uint16_t val);
extern void sub_cab1a(uint8_t a);
extern void run_cli(void);
extern void move_cursor_to_address(void);

// Additional view.c functions called by moved editor helpers
extern void lookup_marker(void);

extern void cac78(void);
extern void insert_edit_buffer_bytes_at_xpos(void);
extern void set_marker_to_here(void);
extern void caf5c(void);
extern void justify_edit_buffer(void);
extern void create_default_ruler(uint16_t ruler_addr);
#endif
