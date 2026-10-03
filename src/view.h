#ifndef VIEW_H
#define VIEW_H

#include "globals.h"

extern bool parse_decimal_number(int* value, uint8_t* pos);
extern bool scan_input_buffer(uint8_t* buffer, scan_state_t* state);
extern command_prefix_t check_for_command_prefix(uint8_t ch);
extern control_code_t check_for_control_code(uint8_t cur_ch);
extern uint8_t process_document_character( uint8_t cur_ch, uint8_t* idx, bool* is_tab);
extern uint8_t upper_case_unless_folding(uint8_t ch);
extern void beep(void);
extern void display_not_enough_memory(void);
extern void draw_prompt_characters(uint8_t first_char, uint8_t second_char);
extern void print_alignment_spaces(uint8_t cur_ch);
extern void print_char(uint8_t cur_ch);
extern void render_number_to_screen(int val);
extern void render_register(uint8_t cur_ch, uint8_t idx);
extern void return_to_cli_prompt(void);
extern void run_view(void);
extern void wipe_buffer(uint8_t fill_value, uint8_t* target_ptr);

#endif
