#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "globals.h"
#include "io.h"
#include "io.h"

extern void call_printer_driver(void);
extern uint8_t check_for_command_prefix(uint8_t ch);
extern void check_for_control_code(void);
extern void compute_bytes_free(void);
extern uint8_t deref_and_check_for_command_prefix(void);
extern void display_document_file_state(void);
extern void find_margins_of_current_ruler_buffer(void);
extern void print_char(void);
extern void print_char_just_to_screen(void);
extern void process_document_character(void);
extern void read_char(void);
extern void return_to_cli_prompt(void);
extern void stop_printing(void);
extern void sub_c9445(void);
extern void sub_cab6e(void);
extern void sub_cabc4(void);
extern void sub_cadf0(void);

#endif
