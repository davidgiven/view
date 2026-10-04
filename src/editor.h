#ifndef EDITOR_H
#define EDITOR_H

#include "globals.h"

typedef enum
{
    FORMAT_OK,         /** Formatting succeeded. */
    FORMAT_AT_END,     /** Reached end of document. */
    FORMAT_MEMORY_FULL /** Document write failed due to insufficient memory. */
} format_result_t;

typedef struct edit_state
{
    uint8_t pos;
} edit_state_t;

// ─── Global variables (defined in editor.c) ────────────────────────────

extern uint8_t edit_line_len;

// ─── Function declarations (defined in editor.c) ───────────────────────

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
extern uint8_t process_current_document_character(uint8_t* target_ptr,
    uint8_t* char_width_out,
    uint8_t* pos_inout,
    bool* is_tab);

#endif
