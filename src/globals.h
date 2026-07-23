#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include <stdbool.h>

typedef uint16_t addr_t;

// Flag bit masks
#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_N 0x80
#define FLAG_V 0x40

#define MAX_LINE_LENGTH 132
#define MAX_COMMAND_LENGTH 68

// 6502 CPU register globals
extern uint8_t a, x, y, flags;
extern uint8_t ram[65536];

// Inline helpers
static inline void set_flags(uint8_t v) {
    flags = (flags & ~(FLAG_Z|FLAG_N)) | (v == 0 ? FLAG_Z : 0) | (v & FLAG_N);
}
static inline void cmp(uint8_t reg, uint8_t value) {
    uint16_t tmp_ = (uint16_t)reg - value;
    flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_C)) | (tmp_ == 0 ? FLAG_Z : 0) | ((uint8_t)tmp_ & FLAG_N) | ((uint16_t)reg >= value ? FLAG_C : 0);
}
static inline void adc(uint8_t value) {
    uint16_t tmp_ = (uint16_t)a + value + (flags & FLAG_C ? 1 : 0);
    flags = (flags & ~FLAG_C) | (tmp_ > 0xff ? FLAG_C : 0);
    a = (uint8_t)tmp_;
    set_flags(a);
}
static inline void sbc(uint8_t value) {
    uint16_t tmp_ = (uint16_t)a - value - (1 - (flags & FLAG_C ? 1 : 0));
    flags = (flags & ~FLAG_C) | (tmp_ <= 0xff ? FLAG_C : 0);
    a = (uint8_t)tmp_;
    set_flags(a);
}
static inline void bit_val(uint8_t value) {
    uint8_t tmp_ = a & value;
    flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ == 0 ? FLAG_Z : 0) | (value & (FLAG_N|FLAG_V));
}

// Global variables (shared between view.c and printing.c)
extern uint8_t left_margin, line_spacing, top_margin, bottom_margin;
extern uint8_t header_margin, footer_margin, page_length;
extern uint8_t two_sided_flag, rhs_extra_margin;
extern uint8_t footers_enabled_flag, headers_enabled_flag;
extern uint8_t format_mode_flag, justifying_flag;
extern uint8_t highlight_code[2];
#define highlight1_code highlight_code[0]
#define highlight2_code highlight_code[1]
extern addr_t himem, top, page;
extern addr_t first_macro_ptr, last_macro_ptr;
extern addr_t ptr5, current_format_line_ptr;
extern uint8_t header_text_maybe[0x42];
extern uint8_t footer_text_maybe[0x42];
extern uint8_t output_buffer[];
extern uint8_t l0030, l0039;
extern uint8_t l0080, l0081, l0082, l0083, l0084;
extern uint8_t tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9;
extern uint8_t print_flags, folding_flag, macro_executing_flag;
extern uint8_t ruler_right_stop, ruler_left_stop;
extern void (*number_callback)(void);
extern void get_register_address(void);
extern void render_new_page(void);
extern void c9263(void);
extern void display_not_enough_memory(void);
extern void sub_c9241(void);
extern uint8_t check_for_command_prefix(uint8_t ch);
extern void parse_decimal_number(void);
extern void check_for_control_code(void);
extern void render_number_to_screen(void);
extern void print_char_via_putchar(void);
extern uint8_t l0021, l0031, l0038, l007a;
#define RAM_REGISTER_VALUE_P (RAM_REGISTER_VALUE_ARRAY + ('P'-'A')*2)
#define RAM_REGISTER_VALUE_L (RAM_REGISTER_VALUE_ARRAY + ('L'-'A')*2)

#define RAM_REGISTER_VALUE_ARRAY 0x0798
#define RAM_CURRENT_RULER_BUF 0x05CF

#endif
