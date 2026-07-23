#ifndef EDITOR_H
#define EDITOR_H

#include "globals.h"

extern void editor_loop_impl(void);
extern void return_to_editor_loop(void);
extern void esc_key(void);
extern void return_key(void);
extern void f13_right_key(void);

// Functions in view.c called by editor key handlers
extern void sub_caa97(void);
extern void recalculate_cursor_xpos(void);
extern void get_line_length(void);
extern void sub_caec2(void);
extern void redraw_editor(void);
extern void write_line_back_to_document_safely(void);
extern void beep(void);
extern void ca741(void);
extern void create_default_ruler(void);
extern void c9de3_insert_line(void);
extern void move_tmp01_to_next_line(void);
extern void adjust_pointers(void);
extern void cac78(void);
extern void caf5c(void);
extern void move_tmp01_to_previous_line(void);
extern void sub_c9e9b(void);
extern void c9e94(void);
extern void sub_c9e22(void);
extern void draw_previous_word(void);
extern void sub_cab1a(void);
extern void go_to_marker(void);
extern void run_cli(void);
extern void move_cursor_to_address(void);
extern void show_memory_full_error(void);

// Additional view.c functions called by moved editor helpers
extern void sub_ca536(void);
extern void sub_caef4(void);
extern void insert_edit_buffer_bytes_at_xpos(void);
extern void ca684(void);
extern void make_space_for_insertion(void);
extern void justify_edit_buffer(void);
extern void go_to_marker_n(void);
extern void lookup_marker(void);
extern void set_marker_to_here(void);

#endif
