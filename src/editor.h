#ifndef EDITOR_H
#define EDITOR_H

#include "globals.h"

extern bool scan_document_for_next_line(void);
extern format_result_t format_paragraph(void);
extern uint8_t justify_edit_buffer(uint8_t* target_ptr);
extern void clamp_ptr6_to_document(void);
extern void clear_format_mode_bit7(void);
extern void clear_screen(void);
extern void cursor_off(void);
extern void editor_loop_impl(void);
extern void enter_editor_mode(void);
extern void esc_key(void);
extern void home_cursor(void);
extern void memory_full(void);
extern void redraw_and_write_back(void);
extern void run_editor(void);
extern void show_memory_full_error(void);

#endif
