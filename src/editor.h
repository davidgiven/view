#ifndef EDITOR_H
#define EDITOR_H

#include "globals.h"

extern void editor_loop_impl(void);
extern void return_to_editor_loop(void);

// Functions in view.c called by the editor loop
extern void sub_caa97(void);
extern void recalculate_cursor_xpos(void);
extern void get_line_length(void);
extern void sub_caec2(void);
extern void redraw_editor(void);
extern void enter_printable_character(void);
extern void esc_key(void);
extern void return_key(void);
extern void delete_key(void);
extern void tab_key(void);
extern void f15_up_key(void);
extern void f12_left_key(void);
extern void f13_right_key(void);
extern void f14_down_key(void);
extern void sf12_left_key(void);
extern void sf13_right_key(void);
extern void sf14_down_key(void);
extern void sf15_up_key(void);
extern void f9_delete_char_key(void);
extern void f8_insert_char_key(void);
extern void f7_delete_line_key(void);
extern void cf4_insert_mode_key(void);
extern void f6_insert_line_key(void);
extern void f0_format_block_key(void);
extern void sf3_delete_to_char_key(void);
extern void cf1_next_match_key(void);
extern void cf7_join_lines_key(void);
extern void sf1_swap_case_key(void);
extern void o_command_key(void);
extern void q_command_key(void);
extern void k_command_key(void);

#endif
