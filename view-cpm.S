#include "cpm65.inc"
#include "driver.inc"
#include "zif.inc"

; Constants
buffer_keyboard                             = 0
MAX_COMMAND_LENGTH                          = 68
MAX_LINE_LENGTH                             = 132
osbyte_enter_language                       = 142
osbyte_flush_buffer                         = 21
osbyte_read_char_at_cursor                  = 135
osbyte_read_high_order_address              = 130
osbyte_read_himem                           = 132
osbyte_read_himem_for_mode                  = 133
osbyte_read_oshwm                           = 131
osbyte_read_text_cursor_pos                 = 134
osbyte_read_vdu_variable                    = 160
osbyte_read_write_ctrl_function_key_status  = 227
osbyte_read_write_function_key_status       = 225
osbyte_read_write_shift_function_key_status = 226
osbyte_reserved_for_application_software    = 163
osbyte_set_cursor_editing                   = 4
osfind_close                                = 0
osword_read_cmos_clock                      = 14
osword_read_line                            = 0

; File structure

FS_FCB = 0
FS_BUFFERPTR = FS_FCB + FCB__SIZE
FS_BUFFER = FS_BUFFERPTR + 1
FS__SIZE = FS_BUFFER + 128

; Memory locations

.section .zp, "zax", @nobits

ptr1: .fill 2
current_edit_line_ptr: .fill 2
current_format_line_ptr: .fill 2
current_ruler_ptr: .fill 2
ptr4: .fill 2
page: .fill 2
top: .fill 2
himem: .fill 2
l0011: .fill 1
l0012: .fill 1
ptr6: .fill 2
ptr5: .fill 2
printer_driver_ptr: .fill 2
first_macro_ptr: .fill 2
last_macro_ptr: .fill 2
ptr3: .fill 2
oshwm: .fill 2
l0021: .fill 1
l0030: .fill 1
l0031: .fill 1
printing_from_file_flag: .fill 1
l0033: .fill 1
l0034: .fill 1
l0038: .fill 1
l0039: .fill 1
l003a: .fill 1
l003b: .fill 1
file_edit_flags: .fill 1
l003d: .fill 1
xpos: .fill 1
l0041: .fill 1
l0042: .fill 1
l0043: .fill 1
l0044: .fill 1
l0045: .fill 1
l0046: .fill 1
l0047: .fill 1
l0048: .fill 1
l0049: .fill 1
l004a: .fill 1
ptr2: .fill 2
rw_file_handle: .fill 1
error_handling_mode: .fill 1
print_flags: .fill 1
edit_input_file_handle: .fill 1
edit_output_file_handle: .fill 1
l006d: .fill 1
l006e: .fill 1
l006f: .fill 1
l0070: .fill 1
hscroll_pos: .fill 1
l0072: .fill 1
l0073: .fill 1
l0074: .fill 1
flags_need_redrawing_flag: .fill 1
l0076: .fill 1
ypos: .fill 1
print_xpos: .fill 1
l0079: .fill 1
l007a: .fill 1
cursor_moved_flag: .fill 1
l007e: .fill 1
input_buffer_ptr: .fill 2
l0081: .fill 1
l0082: .fill 1
l0083: .fill 1
l0084: .fill 1
tmp0: .fill 1
tmp1: .fill 1
tmp2: .fill 1
tmp3: .fill 1
tmp4: .fill 1
tmp5: .fill 1
tmp6: .fill 1
tmp7: .fill 1
tmp8: .fill 1
tmp9: .fill 1

.bss

top_margin: .fill 1
bottom_margin: .fill 1
header_margin: .fill 1
footer_margin: .fill 1
page_length: .fill 1
line_spacing: .fill 1
footers_enabled_flag: .fill 1
headers_enabled_flag: .fill 1
rhs_extra_margin: .fill 1
macro_executing_flag: .fill 1
two_sided_flag: .fill 1
left_margin: .fill 1
highlight1_code: .fill 1
highlight2_code: .fill 1
format_mode_flag: .fill 1
justifying_flag: .fill 1
insert_mode_flag: .fill 1
screen_height: .fill 1
screen_width: .fill 1
microspacing_flag: .fill 1
current_tab_key: .fill 1
folding_flag: .fill 1
ruler_right_stop: .fill 1
ruler_left_stop: .fill 1

__begin_pointer_array:
markers_array: .fill 12
area_start_ptr: .fill 2
area_end_ptr: .fill 2
doc_ptr1: .fill 2
doc_ptr2: .fill 2
doc_ptr3: .fill 2
__end_pointer_array:

printer_driver_block            = DATA-0x400+0x0400
input_buffer                    = DATA-0x400+0x0500
l0501                           = DATA-0x400+0x0501
l0502                           = DATA-0x400+0x0502
l0503                           = DATA-0x400+0x0503
l0504                           = DATA-0x400+0x0504
l0505                           = DATA-0x400+0x0505
l0506                           = DATA-0x400+0x0506
l050a                           = DATA-0x400+0x050a
l050b                           = DATA-0x400+0x050b
l050c                           = DATA-0x400+0x050c
l050d                           = DATA-0x400+0x050d
l050e                           = DATA-0x400+0x050e
l050f                           = DATA-0x400+0x050f
l0510                           = DATA-0x400+0x0510
l0511                           = DATA-0x400+0x0511
current_line_buffer             = DATA-0x400+0x0545
l05cc                           = DATA-0x400+0x05cc
current_ruler_buffer            = DATA-0x400+0x05cf
output_buffer                   = DATA-0x400+0x0654
header_text_maybe               = DATA-0x400+0x06d8
footer_text_maybe               = DATA-0x400+0x071a
filename_buffer                 = DATA-0x400+0x075c
output_filename                 = DATA-0x400+0x0770
printer_driver_name             = DATA-0x400+0x0784
register_value_array            = DATA-0x400+0x0798
register_value_l                = DATA-0x400+0x07ae
register_value_p                = DATA-0x400+0x07b6

DATA:   .fill 0x400-20-32

line_lengths:                   .fill 32
input_filename:                 .fill 20

input_file:                     .fill FS__SIZE
output_file:                    .fill FS__SIZE

.text
.global main
main:
    ldx #0xff
    txs
    stx error_handling_mode
    jsr system_init
    jsr initialise_document
run_cli:
    jsr clear_screen
    ldx #1
    jsr print_x_words_of_help
    jsr print_inline_string
    .ascii "\r\rBytes free "
    .byte 0

    jsr compute_bytes_free
    jsr render_number_to_screen
    jsr bdos_print_newline
    jsr display_document_file_state
    bit file_edit_flags
    bvs c816d
    lda file_edit_flags
    ror
    bcc c816d
    jsr print_inline_string
    .ascii "Input file is "
    .byte 0

    lda l0041
    bne c8163
    jsr print_inline_string
    .ascii "not "
    .byte 0

c8163:
    jsr print_inline_string
    .ascii "empty\r"
    .byte 0

c816d:
    lda printer_driver_name
    beq c81b6
    jsr print_inline_string
    .ascii "Printer "
    .byte 0

    ldx #0
loop_c819a:
    lda printer_driver_name,x
    cmp #0x0d
    beq c81a7
    jsr bdos_print_char                                                        ; Write character
    inx
    bne loop_c819a
c81a7:
    lda microspacing_flag
    beq c81b3
    jsr print_inline_string
    .ascii " (m)"
    .byte 0

c81b3:
    jsr bdos_print_newline
c81b6:
    ldx #0
    ldy #0
c81ba:
    lda markers_array+1,x
    beq c81e7
    tya
    bne c81db
    stx l0083
    jsr print_inline_string
    .ascii "Marker(s) set "
    .byte 0

    ldx l0083
    ldy #1
    bne c81e0                                                         ; ALWAYS branch

c81db:
    lda #0x2c ; ','
    jsr screen_putchar
c81e0:
    txa
    lsr
    adc #0x31 ; '1'
    jsr screen_putchar
c81e7:
    inx
    inx
    cpx #0x0c
    bne c81ba
    tya
    beq c81f3
    jsr bdos_print_newline
c81f3:
    jsr bdos_print_newline
; ***************************************************************************************
cli_loop:
    jsr stop_printing
    ldx #0xff
    stx error_handling_mode
    txs
    inx                                                               ; X=0x00
    stx print_flags
    jsr print_inline_string
    .ascii "=>"
    .byte 0

    jsr readline
    lda #<input_buffer
    sta tmp0                    ; tmp0/1 used for parsing
    ldx #>input_buffer
    stx tmp1
    
    bcc input_line_not_escaped
    jmp run_editor

; ***************************************************************************************
esc_key:
    jsr ca93c
    jmp run_cli

input_line_not_escaped:
    jsr parse_command
    sty input_buffer_ptr+1
    bcs c8263
    cpy #(jumptable4_cli_end-jumptable4_cli)/2
    bcc c826e
c8263:
    jsr print_inline_string
    .ascii "Mistake"
    .byte 0xff

c826e:
    lda input_buffer_ptr+1
    ldy #2
    jsr call_through_jumptable
    jmp run_cli

; ***************************************************************************************
zproc bye_cmd
    ldy #BDOS_EXIT_PROGRAM
    jmp BDOS
zendproc

; ***************************************************************************************
search_cmd:
    jsr sub_c8412
    beq c82e7
    jsr parse_marks_from_command
    jsr sanitise_area
    beq c82fa
    jsr sub_c8c7c
    jsr c8b7b
    bne c82fa
    jsr move_cursor_to_address
    jmp run_editor

; ***************************************************************************************
change_cmd:
    jsr sub_c83f0
    bcs c82fa
    beq c82e7
    jsr c8b7b
    bne c82fa
    ldx #0
    stx ptr3
    stx ptr3+1
loop_c82b3:
    inc ptr3
    bne c82b9
    inc ptr3+1
c82b9:
    jsr move_cursor_to_address
    lda #0
    sta print_xpos
    jsr sub_c8a4f
    bcs c830d
    jsr c8b7b
    beq loop_c82b3
    ldx ptr3
    ldy ptr3+1
    jsr render_number_to_screen
    jsr print_inline_string
    .ascii " string(s) changed"
    .byte 0xff

c82e7:
    jsr print_inline_string
    .ascii "No target given"
    .byte 0xff

c82fa:
    jsr print_inline_string
    .ascii "No string found"
    .byte 0xff

c830d:
    jmp display_not_enough_memory

sub_c8310:
    iny
    lda input_buffer,y
    sta l0084
    cmp l007e
    beq return_2
    cmp #0x0d
return_2:
    rts

; ***************************************************************************************
replace_cmd:
    jsr sub_c83f0
    beq c82e7
    jsr c8b7b
    bne c82fa
    jsr move_cursor_to_address
    jsr enter_editor_mode
c832d:
    jsr sub_c8361
    ldx #0x52 ; 'R'
    ldy #0x50 ; 'P'
    jsr draw_prompt_characters
    jsr flush_and_read_char
    bcs return_2
    and #0xdf
    ldx #0
    cmp #0x59 ; 'Y'
    beq c8349
    dex                                                               ; X=0xff
    cmp #0x4f ; 'O'
    bne c8356
c8349:
    stx print_xpos
    jsr sub_c8371
    jsr sub_c8a4f
    bcs c836b
    jsr sub_c8361
c8356:
    jsr c8b7b
    bne return_2
    jsr move_cursor_to_address
    jmp c832d

sub_c8361:
    lda #0
    sta l006e
    jsr sub_ca276
    jmp ca93c

c836b:
    jsr sub_ca94a
    jmp esc_key

sub_c8371:
    lda ptr2
    sta tmp8
    lda ptr2+1
    sta tmp9
    ldy #0
    ldx #0
c837d:
    lda tmp9
    cmp doc_ptr2+1
    bne c8389
    lda tmp8
    cmp doc_ptr2+0
    beq c8398
c8389:
    lda (tmp8),y
    cmp #0x0d
    bne c8390
    inx
c8390:
    inc tmp8
    bne c837d
    inc tmp9
    bne c837d
c8398:
    inc l0074
    txa
    beq return_3
    jmp ca741

; ***************************************************************************************
expand_escaped_string:
    stx l0083
    dey
c83a3:
    jsr sub_c8310
    beq c83da
    cmp #0x5e ; '^'
    bne c83ca
    jsr sub_c8310
    beq c83da
    jsr to_uppercase
    sta l0082
    ldx #0xfe
loop_c83b8:
    inx
    inx
    lda escaped_char_table,x
    bmi c83c8
    cmp l0082
    bne loop_c83b8
    lda l83e0,x
    bne c83ca
c83c8:
    lda l0084
c83ca:
    ldx l007a
    bne c83d1
    jsr sub_c8c5f
c83d1:
    ldx l0083
    sta header_text_maybe,x
    inc l0083
    bne c83a3
c83da:
    ldx l0083
    sty input_buffer_ptr
return_3:
    rts

escaped_char_table:
    .ascii "?"
l83e0:
    .byte 1
    .ascii "T"
    .byte 9
    .ascii "C"
    .byte 0x0d
    .ascii "S"
    .byte 2
    .ascii "L"
    .byte 0x0b
    .ascii "Z"
    .byte 0x1a
    .ascii "-"
    .byte 0x1c
    .ascii "*"
    .byte 0x1d
    .byte 0xff

sub_c83f0:
    jsr sub_c8412
    beq c8410
    jsr sub_c8e33
    beq c8402
    ldy input_buffer_ptr
    iny
    jsr expand_escaped_string
    stx l004a
c8402:
    jsr parse_marks_from_command
    jsr sanitise_area
    sec
    beq return_4
    jsr sub_c8c7c
    lda #1
c8410:
    clc
return_4:
    rts

sub_c8412:
    ldx #0
    stx l007a
    stx l004a
    jsr sub_c8e33
    beq return_5
    ldx #0
    jsr expand_escaped_string
    stx l007a
    cpx #0
return_5:
    rts

; ***************************************************************************************
screen_cmd:
    jmp print_to_screen

; ***************************************************************************************
sheets_cmd:
    lda #0xc0
    jsr start_printing
    jsr print_document
    jsr stop_printing
    jsr bdos_print_newline
    jmp cli_loop

; ***************************************************************************************
print_cmd:
    lda #0x80
    jsr start_printing
; ***************************************************************************************
print_to_screen:
    jsr print_document
    jmp cli_loop

; ***************************************************************************************
stop_printing:
    lda print_flags
    bpl c8459
    rol print_flags
    clc
    ror print_flags
    lda #6
    jsr call_printer_driver
c8459:
    rts

start_printing:
    jsr print_inline_string
    .ascii "Sorry, can't print yet\r"
    .byte 0
    jmp cli_loop

    sta print_flags
    jsr prepare_printer_driver
    lda #3
    jmp call_printer_driver

; ***************************************************************************************
edit_cmd:
    jsr check_not_continuous_editing
    jsr parse_filename_from_command
    lda #2
    sta error_handling_mode
    #if 0
    lda #0x40 ; '@'
    jsr open_file
    #endif
    sta edit_input_file_handle
    jsr set_document_name_to_filename_buffer
    jsr parse_filename_from_command
    #if 0
    lda #0x80
    jsr open_file
    #endif
    sta edit_output_file_handle
    ldx #0
    stx l0041
loop_c8490:
    lda filename_buffer,x
    sta output_filename,x
    inx
    cmp #0x0d
    bne loop_c8490
    jsr initialise_document
    jsr sub_c8d24
    beq c84a8
    lda #1
    sta file_edit_flags
    rts

c84a8:
    jsr c850d
c84ab:
    jmp cli_loop

; ***************************************************************************************
more_cmd:
    jsr sub_c8e54
    jsr parse_marks_from_command
    lda area_start_ptr
    ldy area_start_ptr+1
    jsr move_cursor_to_address

    ldx #<output_file
    ldy #>output_file
    jsr write_area_to_file_yx
    bne c84ab
    ldy #0
    ldx l003a
loop_c84c4:
    lda (current_ruler_ptr),y
    sta current_ruler_buffer,y
    iny
    dex
    bne loop_c84c4
    lda #0x0d
    sta current_ruler_buffer,y
    jsr sub_c89d3
    jsr cb07a
    jsr check_for_at_least_150_bytes_free
    lda l0041
    bne c84e8
    lda top
    ldy top+1
    jsr sub_c8d28
    beq c84ab
c84e8:
    jmp cb05a

; ***************************************************************************************
finish_cmd:
    jsr sub_c8e54
loop_c84ee:
    jsr reset_area_to_entire_document
    
    ldx #<output_file
    ldy #>output_file
    jsr write_area_to_file_yx
    bne c84ab
    jsr sub_c89d3
    jsr cb07a
    jsr cb05a
    lda l0041
    bne c850d
    jsr sub_c8d24
    beq c84ab
    bne loop_c84ee                                                    ; ALWAYS branch

; ***************************************************************************************
quit_cmd:
    jsr sub_c8e54
c850d:
    lda #0
    sta l0041
    sta file_edit_flags

    ldx #<output_file
    ldy #>output_file
    jsr close_file
    jmp cli_loop

; ***************************************************************************************
save_cmd:
write_cmd:
    jsr parse_optional_filename_from_command
    zif eq
        bit file_edit_flags
        zif vc
            jmp bad_filename_error
        zendif

        ldx #0
        zrepeat
            lda input_filename,x
            sta filename_buffer,x
            inx
            cmp #0x0d
        zuntil eq
    zendif
    jsr parse_marks_from_command
    jsr sanitise_area
    beq return_6

    ldx #<input_buffer
    ldy #>input_buffer
    jsr open_output_file

    ldx #<input_buffer
    ldy #>input_buffer
    jsr write_area_to_file_yx

    ldx #<input_buffer
    ldy #>input_buffer
    lda #0
    jsr put_byte_to_file

    ldx #<input_buffer
    ldy #>input_buffer
    jsr close_file
    jmp cli_loop

check_for_at_least_150_bytes_free:
    jsr compute_bytes_free
    tya
    bne return_6
    cpx #0x96
    bcs return_6
display_not_enough_memory:
    jsr stop_printing
    jsr print_inline_string
    .ascii "Not enough memory"
    .byte 0xff

return_6:
    rts

; ***************************************************************************************
read_cmd:
    jsr parse_filename_from_command
    jsr parse_marks_from_command
    jsr check_for_at_least_150_bytes_free

    ldx #<input_buffer
    ldy #>input_buffer
    jsr open_input_file

    lda area_start_ptr
    ldy area_start_ptr+1
    sta tmp4
    sty tmp5
    jsr move_cursor_to_address
    lda tmp4
    ldy tmp5
    jsr compute_required_space_for_insertion
    jsr make_space_for_insertion

    ldx #<input_buffer
    ldy #>input_buffer
    jsr read_block_from_file
    beq c8584
    bcs c8598
c8584:
    jsr print_inline_string
    .ascii "Not all read in\r"
    .byte 0

c8598:
    lda tmp0
    sta tmp4
    lda tmp1
    sta tmp5
    lda ptr5
    sec
    sbc tmp0
    sta tmp6
    lda ptr5+1
    sbc tmp1
    sta tmp7
    jsr adjust_pointers
    jmp cli_loop

; ***************************************************************************************
mode_cmd:
    jsr print_inline_string
    .ascii "Bad mode"
    .byte 0xff

; ***************************************************************************************
microspace_cmd:
    jsr prepare_printer_driver
    jsr parse_integer_from_command
    php
    ldx #0x0a
    plp
    beq c8608
    ldx tmp8
    beq return_7
c8608:
    ldy #0
    lda #0x0c
    jsr call_printer_driver
    tya
    and #1
    beq c8617
    stx microspacing_flag
return_7:
    rts

c8617:
    jsr print_inline_string
    .ascii "Driver does not support microspacing"
    .byte 0xff

; ***************************************************************************************
setup_cmd:
    ldx #1
    stx tmp6
    dex                                                               ; X=0x00
    stx tmp8
    dex                                                               ; X=0xff
    stx tmp7
c8649:
    jsr sub_c8e33
    beq c8672
    and #0xdf
    ldx #0
loop_c8652:
    cmp c867d,x
    beq c8669
    inx
    ldy c867d,x
    bne loop_c8652
    jsr print_inline_string
    .ascii "Bad flag"
    .byte 0xff

c8669:
    lda c8681,x
    sta tmp6,x
    inc input_buffer_ptr
    bne c8649
c8672:
    ldx #2
loop_c8674:
    lda tmp6,x
    sta format_mode_flag,x
    dex
    bpl loop_c8674
    bmi c869b                                                         ; ALWAYS branch

c867d:
    lsr l004a
    eor #0
c8681:
    brk

    .byte 0, 0xff

; ***************************************************************************************
field_cmd:
    jsr parse_integer_from_command
    beq c869b
    lda tmp8
    cmp #0x1b
    bne c8699
    jsr print_inline_string
    .ascii "Frump!"
    .byte 0xff

c8699:
    sta current_tab_key
c869b:
    jmp cli_loop

; ***************************************************************************************
count_cmd:
    jsr parse_marks_from_command
    jsr sanitise_area
    beq c869b
    lda area_start_ptr
    sta tmp0
    lda area_start_ptr+1
    sta tmp1
    lda #0
    sta tmp8
    sta tmp9
    sta l0083
    sta l0082
c86b8:
    ldy #0
    jsr deref_and_check_for_command_prefix
    bne c86ea
    ldx #0
    iny
loop_c86c2:
    lda (tmp0),y
    iny
    cmp l8747,x
    bne c86d1
    lda (tmp0),y
    cmp l8748,x
    beq c86df
c86d1:
    lda l8749,x
    beq c86db
    dey
    inx
    inx
    bne loop_c86c2
c86db:
    lda #0x80
    bne c86ff                                                         ; ALWAYS branch

c86df:
    lda tmp0
    clc
    adc #3
    sta tmp0
    bcs c871d
    bcc c871f                                                         ; ALWAYS branch

c86ea:
    ldy #0
    jsr draw_char
    and #0x7f
    ldx #0
    ldy l0082
    bmi c870d
    cmp #0x0d
    beq c8703
    cmp #0x20 ; ' '
    beq c8703
c86ff:
    inc l0083
    bne c8715
c8703:
    ldy l0083
    beq c870d
    inc tmp8
    bne c870d
    inc tmp9
c870d:
    stx l0083
    cmp #0x0d
    bne c8715
    stx l0082
c8715:
    ora l0082
    sta l0082
    inc tmp0
    bne c871f
c871d:
    inc tmp1
c871f:
    ldy tmp1
    cpy area_end_ptr+1
    bne c86b8
    ldy tmp0
    cpy area_end_ptr
    bne c86b8
    ldx tmp8
    ldy tmp9
    jsr render_number_to_screen
    jsr print_inline_string
    .ascii " word(s) counted."
    .byte 0xff
l8747:
    .byte 0x52
l8748:
    .byte 0x4a
l8749:
    .ascii "CELJ"
    .byte 0

; ***************************************************************************************
format_cmd:
    jsr parse_marks_from_command
    jsr sanitise_area
    beq c878b
    lda area_start_ptr
    ldy area_start_ptr+1
    jsr move_cursor_to_address
    jsr sub_caf5f
    lda #0x10
    jsr wipe_buffer
    lda current_edit_line_ptr
    sta current_format_line_ptr
    lda current_edit_line_ptr+1
    sta current_format_line_ptr+1
c876d:
    jsr sub_c9977
    bvs c8791
    bcs c8787
    lda #0x2e ; '.'
    jsr bdos_print_char
    lda ptr4
    ldy ptr4+1
    cpy area_end_ptr+1
    bcc c876d
    bne c8787
    cmp area_end_ptr
    bcc c876d
c8787:
    lda #0xff
    sta l0012
c878b:
    jsr bdos_print_newline
    jmp cli_loop

c8791:
    jsr bdos_print_newline
    jmp display_not_enough_memory

; ***************************************************************************************
new_cmd:
    jsr check_not_continuous_editing
    jmp initialise_document

; ***************************************************************************************
fold_cmd:
    jsr sub_c8e33
    beq c87b4
    lda #0
    ldx #5
    jsr sub_c976c
    bcs c87b4
    ldx #0x80
    tay
    beq c87b2
    ldx #0
c87b2:
    stx folding_flag
c87b4:
    jsr print_inline_string
    .ascii "Folding "
    .byte 0

    lda folding_flag
    bpl c87cb
    jsr print_inline_string
    .ascii "off"
    .byte 0xff

c87cb:
    jsr print_inline_string
    .ascii "on"
    .byte 0xff

c87d1:
    jsr print_inline_string
    .ascii "Bad file"
    .byte 0xff

; ***************************************************************************************
printer_cmd:
    jsr parse_optional_filename_from_command
    beq c882f
    ;jsr sub_c8849
    beq c87d1
    lda l050c
    ora l050d
    bne c87d1
    ldy l050b
    dey
    bmi c8801
    bne c87d1
    lda l050a
    bne c87d1
c8801:
    lda #<printer_driver_block
    sta l0502
    lda #>printer_driver_block
    sta l0503
    lda #osbyte_read_high_order_address
    jsr osbyte                                                        ; Read the filing system 'machine high order address'
    stx l0504                                                         ; X and Y contain the machine high order address (low, high)
    sty l0505
    lda #0
    sta l0506
    lda #0xff
    jsr do_osfile_with_buffer
    ldx #0
loop_c8822:
    lda filename_buffer,x
    sta printer_driver_name,x
    inx
    cmp #0x0d
    bne loop_c8822
    beq c8834                                                         ; ALWAYS branch

c882f:
    lda #0
    sta printer_driver_name
c8834:
    lda #0
    sta microspacing_flag
return_8:
    rts

; ***************************************************************************************
parse_integer_from_command:
    lda #<(input_buffer)
    sta current_format_line_ptr
    lda #>(input_buffer)
    sta current_format_line_ptr+1
    jsr sub_c8e33
    beq return_8
    jmp ca6fe

; ***************************************************************************************
file_not_found_error:
    jsr stop_printing
    jsr print_inline_string
    .ascii "File not found"
    .byte 0
    jmp cli_loop

; ***************************************************************************************
load_cmd:
    jsr check_not_continuous_editing
    jsr parse_filename_from_command
    jsr zero_terminate_filename_buffer
    jsr convert_filename_to_fcb
    zif cs
        jmp bad_filename_error
    zendif

    ldy #BDOS_OPEN_FILE
    lda #<input_buffer
    ldx #>input_buffer
    jsr BDOS
    bcs file_not_found_error

    ; Load the file into memory.

    lda page
    sta tmp6
    lda page+1
    sta tmp7
    zloop
        ldy #BDOS_SET_DMA_ADDRESS
        lda tmp6
        ldx tmp7
        jsr BDOS

        lda tmp6
        eor #0x80
        sta tmp6
        zif pl
            inc tmp7
        zendif

        lda himem+1
        cmp tmp6+1
        zif eq
            lda himem+0
            cmp tmp6+0
        zendif
        blt c88ac

        ldy #BDOS_READ_SEQUENTIAL
        lda #<input_buffer
        ldx #>input_buffer
        jsr BDOS
        zbreakif cs
    zendloop

    ; Find the end of it.

    lda page
    sta top
    lda page+1
    sta top+1
    ldy #0
    zloop
        lda (top), y
        zbreakif eq

        inc top
        zif eq
            inc top+1
        zendif
    zendloop

    jsr reset_document_name_after_load
    jsr clear_cmd
    jmp cb07a

c88ac:
    jmp display_not_enough_memory

; ***************************************************************************************
name_cmd:
    jsr check_not_continuous_editing
    jsr parse_optional_filename_from_command
    php
    lda #0
    sta file_edit_flags
    plp
    beq return_9
reset_document_name_after_load:
    lda #0x40 ; '@'
    sta file_edit_flags
set_document_name_to_filename_buffer:
    ldx #0
loop_c88fa:
    lda filename_buffer,x
    sta input_filename,x
    inx
    cmp #0x21
    bge loop_c88fa
return_9:
    lda #0x0d
    sta input_filename-1, x
    rts

; ***************************************************************************************
zproc file_error
    jsr print_inline_string
    .ascii "File error"
    .byte 0
    jmp cli_loop
zendproc

zproc convert_filename_to_fcb
    lda #<input_buffer
    ldx #>input_buffer
zproc convert_filename_to_fcb_xa
    ldy #BDOS_SET_DMA_ADDRESS
    jsr BDOS
    ldy #BDOS_PARSEFILENAME
    lda #<filename_buffer
    ldx #>filename_buffer
    jmp BDOS
zendproc

zproc reset_fcb
    ldx #FCB_EX
    lda #0
    zrepeat
        sta input_buffer, x
        inx
        cpx #FCB__SIZE
    zuntil eq
    rts
zendproc

zproc zero_terminate_filename_buffer
    ldx #0
    lda #0x0d
    zloop
        cmp filename_buffer, x
        zbreakif eq
        inx
    zendloop

    lda #0
    sta filename_buffer, x
    rts
zendproc

; ***************************************************************************************
do_osfile_with_buffer:
    ldx #<filename_buffer
    stx input_buffer
    ldy #>filename_buffer
    sty l0501
do_osfile_with_block:
    ldx #<(input_buffer)
    ldy #>(input_buffer)
    jmp osfile

; ***************************************************************************************
sanitise_area:
    lda area_start_ptr
    ldx area_start_ptr+1
    cpx area_end_ptr+1
    bcc c8977
    bne c896b
    cmp area_end_ptr
    bcc c8977
c896b:
    ldy area_end_ptr
    sty area_start_ptr
    ldy area_end_ptr+1
    sty area_start_ptr+1
    stx area_end_ptr+1
    sta area_end_ptr
c8977:
    lda area_end_ptr
    sec
    sbc area_start_ptr
    sta tmp6
    lda area_end_ptr+1
    sbc area_start_ptr+1
    sta tmp7
    bne return_10
    lda tmp6
return_10:
    rts

; ***************************************************************************************
parse_marks_from_command:
    jsr reset_area_to_entire_document
    jsr parse_mark_from_command
    beq return_11
    sta area_start_ptr
    sty area_start_ptr+1
    jsr parse_mark_from_command
    beq return_11
    sta area_end_ptr
    sty area_end_ptr+1
return_11:
    rts

; ***************************************************************************************
parse_mark_from_command:
    jsr sub_c8e33
    beq return_12
    iny
    sty input_buffer_ptr
    jsr lookup_marker
    bcs c89b3
    beq c89c1
    lda markers_array,x
    ldy markers_array+1,x
return_12:
    rts

c89b3:
    jsr print_inline_string
    .ascii "Bad marker"
    .byte 0xff

c89c1:
    jsr print_inline_string
    .ascii "Marker not set"
    .byte 0xff

sub_c89d3:
    lda area_start_ptr
    sta tmp4
    lda area_start_ptr+1
    sta tmp5
    jsr adjust_pointers
    lda tmp4
    ldy tmp5
    jmp cac78

display_document_file_state:
    jsr stop_printing
    jsr print_inline_string
    .ascii "Editing "
    .byte 0

    lda file_edit_flags
    beq c8a21
    ldy #0
loop_c89fa:
    lda input_filename,y
    cmp #0x0d
    beq c8a07
    jsr bdos_print_char                                                        ; Write character
    iny
    bne loop_c89fa
c8a07:
    bit file_edit_flags
    bvs c8a19
    jsr print_inline_string
    .ascii " to "
    .byte 0

    ldy #0
loop_c8a15:
    lda output_filename,y
    iny
c8a19:
    jsr bdos_print_char                                                        ; Write character
    cmp #0x0d
    bne loop_c8a15
    rts

c8a21:
    jsr print_inline_string
    .ascii "No File\r"
    .byte 0

    rts

sub_c8a4f:
    lda #0
    sta l0082
    sta l0081
    ldy #0x14
    ldx l007a
    bne c8a87
c8a5b:
    lda header_text_maybe,x
    cmp #1
    bne c8a6c
    lda l0081
    cmp l0049
    bcs c8a86
    inc l0081
    bne c8a84
c8a6c:
    cmp #0x20 ; ' '
    bne c8a84
    cpy l0048
    bcs c8a84
loop_c8a74:
    lda output_buffer,y
    php
    iny
    plp
    beq c8a86
    inc l0082
    cpy l0048
    bcc loop_c8a74
    dec l0082
c8a84:
    inc l0082
c8a86:
    inx
c8a87:
    cpx l004a
    bcc c8a5b
    lda doc_ptr2+0
    sec
    sbc ptr2
    sta input_buffer_ptr+1
    lda doc_ptr2+1
    sbc ptr2+1
    sta l0081
    ldx l0082
    tay
    bne c8aa3
    cpx input_buffer_ptr+1
    bcc c8aa3
    ldx input_buffer_ptr+1
c8aa3:
    txa
    clc
    adc ptr2
    sta tmp4
    lda ptr2+1
    adc #0
    sta tmp5
    lda l0082
    sec
    sbc input_buffer_ptr+1
    sta tmp6
    lda #0
    sbc l0081
    sta tmp7
    bmi c8aca
    ora tmp6
    beq c8ada
    sta tmp6
    jsr make_space_for_insertion
    bcc c8ada
    rts

c8aca:
    lda #0
    sec
    sbc tmp6
    sta tmp6
    lda #0
    sbc tmp7
    sta tmp7
    jsr adjust_pointers
c8ada:
    ldy #0
    sty l0081
    bit print_xpos
    bmi c8b11
    ldx input_buffer_ptr+1
loop_c8ae4:
    lda (ptr2),y
    iny
    jsr is_uppercase
    bcc c8af3
    ror print_xpos
    dex
    bne loop_c8ae4
    beq c8b11                                                         ; ALWAYS branch

c8af3:
    pha
    lda #0
    sta print_xpos
    pla
    and #0x20 ; ' '
    bne c8b11
    inc l0081
    dex
    beq c8b0d
    lda (ptr2),y
    jsr is_uppercase
    bcs c8b11
    and #0x20 ; ' '
    bne c8b11
c8b0d:
    dec l0081
    dec l0081
c8b11:
    ldx #0
    stx l0082
    stx l0083
    ldx #0x14
    stx input_buffer_ptr+1
    ldx l007a
    bne c8b6b
c8b1f:
    lda header_text_maybe,x
    stx l0084
    cmp #0x20 ; ' '
    bne c8b38
    ldy input_buffer_ptr+1
    cpy l0048
    bcs c8b47
    inc input_buffer_ptr+1
    lda output_buffer,y
    beq c8b6a
    dex
    bcc c8b47                                                         ; ALWAYS branch

c8b38:
    cmp #1
    bne c8b47
    ldy l0082
    cpy l0049
    bcs c8b6a
    lda output_buffer,y
    inc l0082
c8b47:
    cmp #2
    bne c8b4d
    lda #0x20 ; ' '
c8b4d:
    bit folding_flag
    bmi c8b64
    ldy print_xpos
    bne c8b64
    jsr is_uppercase
    bcs c8b64
    ora #0x20 ; ' '
    ldy l0081
    beq c8b64
    dec l0081
    and #0xdf
c8b64:
    ldy l0083
    sta (ptr2),y
    inc l0083
c8b6a:
    inx
c8b6b:
    cpx l004a
    bcc c8b1f
    lda ptr2
    ldy ptr2+1
    jsr cac78
    clc
    rts

c8b78:
    lda #0xff
    rts

c8b7b:
    lda l007a
    beq c8b78
    lda #0x14
    sta l0048
    ldx #0
    stx l0049
    stx l0081
    lda doc_ptr2+0
    sta tmp8
    lda doc_ptr2+1
    sta tmp9
c8b91:
    lda tmp9
    cmp doc_ptr3+1
    bcc c8b9f
    bne c8b78
    lda tmp8
    cmp doc_ptr3+0
    bcs c8b78
c8b9f:
    ldy #0
    lda (tmp8),y
    jsr check_for_command_prefix
    bne c8bb7
    lda tmp8
    clc
    adc #3
    sta doc_ptr2+0
    lda tmp9
    sta doc_ptr2+1
    bcc c8b7b
    bcs c8bdf                                                         ; ALWAYS branch

c8bb7:
    jsr sub_c8c5f
    sta l0083
c8bbc:
    iny
    lda (tmp8),y
    beq c8bdb
    jsr check_for_command_prefix
    beq c8bdb
    lda header_text_maybe,x
    cmp #0x20 ; ' '
    beq c8bf7
    cmp #1
    beq c8be3
    cmp #2
    bne c8bd7
    lda #0x20 ; ' '
c8bd7:
    cmp l0083
    beq c8c33
c8bdb:
    inc doc_ptr2+0
    bne c8b7b
c8bdf:
    inc doc_ptr2+1
    bne c8b7b
c8be3:
    lda l0083
    stx l0084
    ldx l0049
    cpx #0x14
    bcs c8bf2
    sta output_buffer,x
    inc l0049
c8bf2:
    ldx l0084
    jmp c8c33

c8bf7:
    stx l0084
    lda l0083
    cmp #0x20 ; ' '
    beq c8c23
    cmp #9
    beq c8c23
    cmp #0x0b
    beq c8c23
    cmp #0x1a
    beq c8c23
    cmp #0x0d
    beq c8c23
    lda l0081
    beq c8bdb
    jsr sub_c8c51
    lda #0
    sta l0081
    ldx l0084
    inx
    cpx l007a
    bcc c8bbc
    bcs c8c3e                                                         ; ALWAYS branch

c8c23:
    jsr sub_c8c53
    ldx l0084
    sta l0081
loop_c8c2a:
    inc tmp8
    bne c8c30
    inc tmp9
c8c30:
    jmp c8b91

c8c33:
    inx
    cpx l007a
    bcc loop_c8c2a
    inc tmp8
    bne c8c3e
    inc tmp9
c8c3e:
    lda doc_ptr2+0
    ldy doc_ptr2+1
    ldx tmp8
    stx doc_ptr2+0
    ldx tmp9
    stx doc_ptr2+1
    sta ptr2
    sty ptr2+1
    ldx #0
    rts

sub_c8c51:
    lda #0
sub_c8c53:
    ldx l0048
    cpx #0x84
    bcs return_13
    sta output_buffer,x
    inc l0048
return_13:
    rts

sub_c8c5f:
    bit folding_flag
    bmi return_14
to_uppercase:
    jsr is_uppercase
    bcs return_14
    and #0xdf
return_14:
    rts

; ***************************************************************************************
is_uppercase:
    cmp #0x41 ; 'A'
    bcc c8c7a
    cmp #0x5b ; '['
    bcc return_15
    cmp #0x61 ; 'a'
    bcc c8c7a
    cmp #0x7b ; '{'
return_15:
    rts

c8c7a:
    sec
    rts

sub_c8c7c:
    lda area_start_ptr
    sta doc_ptr2+0
    lda area_start_ptr+1
    sta doc_ptr2+1
    lda area_end_ptr
    sta doc_ptr3+0
    lda area_end_ptr+1
    sta doc_ptr3+1
    rts

read_block_from_file:
    stx tmp2
    sty tmp3
    lda #0
    sta l0083
    sta l0084
c8c95:
    ldx tmp2
    ldy tmp3
    jsr get_byte_from_file
    beq c8cf2
    ldy #0
    cmp #0x7f
    bcc c8caf
    ldx l0084
    bne c8c95
    jsr check_for_command_prefix
    bne c8c95
    ldx #0xfd
    stx l0083
c8caf:
    cmp #0x20 ; ' '
    bcs c8cc8
    jsr check_for_control_code
    beq c8cc8
    cmp #0x1a
    beq c8cc8
    cmp #0x0d
    beq c8cc8
    cmp #0x0b
    beq c8cc8
    cmp #9
    bne c8c95
c8cc8:
    ldx #1
    cmp #0x0d
    beq c8cdb
    dex                                                               ; X=0x00
    ldy l0083
    cpy #0x84
    bne c8cdb
    pha
    jsr write_cr_to_memory
    pla
    inx
c8cdb:
    inc l0083
    jsr write_byte_to_memory
    txa
    beq c8c95
    lda tmp1
    cmp l0081
    bcc c8c95
    bne c8cf1
    lda tmp0
    cmp input_buffer_ptr+1
    bcc c8c95
c8cf1:
    clc
c8cf2:
    php
    lda l0084
    beq c8cfa
    jsr write_cr_to_memory
c8cfa:
    plp
    lda l0082
    rts

write_cr_to_memory:
    lda #0x0d
write_byte_to_memory:
    ldy #0
    sta (tmp0),y
    inc tmp0
    bne c8d0a
    inc tmp1
c8d0a:
    sta l0084
    cmp #0x0d
    bne return_16
    sty l0084
    sty l0083
return_16:
    rts

; ***************************************************************************************
sub_c8d24:
    lda page
    ldy page+1
sub_c8d28:
    jsr sub_c8da2
    lda edit_input_file_handle
    sta rw_file_handle
    jsr read_block_from_file
    php
    beq c8d39
    bcc c8d39
    inc l0041
c8d39:
    lda #0
    tay                                                               ; Y=0x00
    sta (tmp0),y
    lda tmp0
    sta top
    lda tmp1
    sta top+1
    plp
    rts

; Does not include trailing zero!
write_area_to_file_yx:
    stx tmp4
    sty tmp5

    lda area_start_ptr
    sta tmp8
    lda area_start_ptr+1
    sta tmp9

    zrepeat
        ldy #0
        lda (tmp8),y
        ldx tmp4
        ldy tmp5
        jsr put_byte_to_file
        inc tmp8
        zif eq
            inc tmp9
        zendif

        lda tmp9
        cmp area_end_ptr+1
        zif eq
            lda tmp8
            cmp area_end_ptr
        zendif
    zuntil eq
return_17:
    rts

compute_required_space_for_insertion:
    ldx #0
    stx tmp8
    stx tmp9
    beq c8daf                                                         ; ALWAYS branch

sub_c8da2:
    pha
    tya
    pha
    jsr compute_bytes_free
    stx tmp8
    sty tmp9
    pla
    tay
    pla
c8daf:
    sta tmp0
    sty tmp1
    jsr compute_bytes_free
    stx tmp6
    sty tmp7
    lsr tmp9
    ror tmp8
    lsr tmp9
    ror tmp8
    lda tmp9
    cmp #4
    bcc c8dce
    lda #4
    sta tmp9
    sta tmp8
c8dce:
    lda tmp6
    sbc tmp8
    sta tmp6
    lda tmp7
    sbc tmp9
    sta tmp7
    lda tmp0
    clc
    adc tmp6
    sta ptr5
    pha
    lda tmp1
    adc tmp7
    sta ptr5+1
    sta l0081
    pla
    sbc #0x8b
    sta input_buffer_ptr+1
    bcs return_18
    dec l0081
return_18:
    rts

parse_optional_filename_from_command:
    jsr sub_c8e33
    beq return_19
    ldx #0
loop_c8dfb:
    lda input_buffer,y
    cmp #0x0d
    beq c8e25
    iny
    cmp l007e
    beq c8e25
    sta filename_buffer,x
    inx
    cpx #0x14
    bne loop_c8dfb
bad_filename_error:
    jsr print_inline_string
    .ascii "Bad filename"
    .byte 0
    jmp cli_loop

parse_filename_from_command:
    jsr parse_optional_filename_from_command
    beq bad_filename_error
return_19:
    rts

c8e25:
    lda #0x0d
    sta filename_buffer,x
    sty input_buffer_ptr
return_20:
    rts

sub_c8e2d:
    lda #0x20 ; ' '
    sta l007e
    sty input_buffer_ptr
sub_c8e33:
    lda l007e
    cmp #0x0d
    beq return_20
    ldy input_buffer_ptr
loop_c8e3b:
    lda input_buffer,y
    cmp #0x0d
    beq return_20
    cmp l007e
    bne return_20
    iny
    bne loop_c8e3b
check_not_continuous_editing:
    bit file_edit_flags
    bvs return_20
    lda file_edit_flags
    ror
    bcc return_20
    bcs c8e5d                                                         ; ALWAYS branch

sub_c8e54:
    bit file_edit_flags
    bvs c8e5d
    lda file_edit_flags
    ror
    bcs return_20
c8e5d:
    jsr display_document_file_state
    jmp cli_loop

display_nl_then_no_text:
    jsr bdos_print_newline
display_no_text:
    jsr print_inline_string
    .ascii "No text\r"
    .byte 0

    rts

print_document:
    jsr check_not_continuous_editing
    jsr check_for_at_least_150_bytes_free
    jsr sub_cb104
    lda top
    adc #3
    sta ptr5
    tax
    lda top+1
    adc #0
    sta ptr5+1
    tay
    txa
    adc #0x8d
    bcc c8edb
    iny
c8edb:
    sta first_macro_ptr
    sta last_macro_ptr
    sty first_macro_ptr+1
    sty last_macro_ptr+1
    lda #0
    sta l0031
    sta print_xpos
    sta printing_from_file_flag
    tay                                                               ; Y=0x00
    sta (last_macro_ptr),y
    lda #<(current_ruler_buffer)
    sta current_ruler_ptr
    lda #>(current_ruler_buffer)
    sta current_ruler_ptr+1
    jsr find_margins_of_current_ruler_buffer
    jsr sub_c8e33
    bne c8f0d
    inc printing_from_file_flag
    lda page
    sta ptr6
    lda page+1
    sta ptr6+1
    bne c8f30
c8f0a:
c8f0d:
    jsr parse_optional_filename_from_command
    bne c8f29
    lda l0031
    bpl return_23
    jmp c9263

return_23:
    rts

c8f1a:
    jsr stop_printing
    jsr bdos_print_newline
    jmp cli_loop

c8f29:
    #if 0
    lda #0x40 ; '@'
    jsr open_file
    #endif
    sta rw_file_handle
c8f30:
    lda l0031
    beq c8f3b
    lda l0021
    bne c8f3b
    jsr c9263
c8f3b:
    jsr sub_c9188
    bcs c8f0a
    jsr sub_c916a
    ldy #0
    sty input_buffer_ptr+1
    jsr deref_and_check_for_command_prefix
    bne c8fce_thunk
    ldy #3
    sty input_buffer_ptr+1
    jsr sub_cab6e
    bne c8f6e
    ldy #3
    ldx #0
loop_c8f5d:
    lda (tmp0),y
    sta current_ruler_buffer,x
    iny
    inx
    cmp #0x0d
    bne loop_c8f5d
    jsr find_margins_of_current_ruler_buffer
c8f6b:
    jmp c900e

c8f6e:
    jsr lookup_formatting_command
    bmi c8f7a
    jsr execute_formatting_command
    beq c8f6b
c8fce_thunk:
    bne c8fce                                                         ; ALWAYS branch

c8f7a:
    lda first_macro_ptr
    sta tmp6
    lda first_macro_ptr+1
    sta tmp7
    ldy #1
    lda (current_format_line_ptr),y
    sta tmp8
    iny                                                               ; Y=0x02
    lda (current_format_line_ptr),y
    jsr is_uppercase
    bcc c8f92
    lda #0x20 ; ' '
c8f92:
    sta tmp9
lookup_macro_name:
    ldy #0
    lda (tmp6),y
    beq c8f6b
    ldy #2
    lda (tmp6),y
    cmp tmp8
    bne get_next_macro_in_linked_list
    iny                                                               ; Y=0x03
    lda (tmp6),y
    cmp tmp9
    beq c8fb9
get_next_macro_in_linked_list:
    ldy #0
    lda (tmp6),y
    pha
    iny                                                               ; Y=0x01
    lda (tmp6),y
    sta tmp7
    pla
    sta tmp6
    jmp lookup_macro_name

c8fb9:
    lda macro_executing_flag
    bne nested_macro_error
    lda tmp6
    clc
    adc #4
    sta ptr3
    lda tmp7
    adc #0
    sta ptr3+1
    sta macro_executing_flag
    bne c900e
c8fce:
    lda l0031
    bne c8fd5
    jsr render_new_page
c8fd5:
    jsr sub_c9407
    lda #0
    sta l0039
    ldy input_buffer_ptr+1
    lda print_flags
    bpl c8fe6
    lda microspacing_flag
    bne c9034
c8fe6:
    lda (tmp0),y
    iny
    jsr sub_c9431
    jsr c9426
    cmp #0x0d
    bne c8fe6
    inc register_value_l
    bne c8ffb
    inc register_value_l+1
c8ffb:
    ldx line_spacing
    lda l0021
    clc
    sbc line_spacing
    bcs c9009
    lda #0
    ldx l0021
    dex
c9009:
    sta l0021
    jsr print_vertical_space
c900e:
    jmp c8f30

; ***************************************************************************************
nested_macro_error:
    jsr stop_printing
    jsr print_inline_string
    .ascii "Nested macro call"
    .byte 0

    jmp c8f1a

return_24:
    rts

c9034:
    ldx #0
    stx l0044
    stx l0046
    stx l0045
    stx l0047
    stx l0039
    stx l0048
    stx l0042
    stx l0043
    stx l0083
c9048:
    txa
    pha
    lda (tmp0),y
    jsr sub_c9431
    pla
    tax
    lda (tmp0),y
    iny
    cmp #0x1a
    bne c906f
    bit l0083
    bpl c9064
    lda l0048
    beq c906b
    inc l0043
    bne c9048
c9064:
    lda l0039
    sta l0047
    jmp c908c

c906b:
    lda #0x20 ; ' '
    dec l0042
c906f:
    cmp #0x20 ; ' '
    bcc c9092
    bne c9090
    bit l0083
    bpl c9064
    lda l0042
    beq c908a
    bmi c9087
    inc l0043
    lda #0
    sta l0042
    beq c9048                                                         ; ALWAYS branch

c9087:
    clc
    ror l0042
c908a:
    inc l0048
c908c:
    lda #0x20 ; ' '
    bne c90b6                                                         ; ALWAYS branch

c9090:
    inc l0046
c9092:
    cmp #9
    beq c90a0
    cmp #0x0b
    beq c90a0
    sec
    ror l0083
    jmp c90b6

c90a0:
    pha
    lda l0039
    sta l0047
    lda #0
    sta l0083
    sta l0046
    sta l0048
    sta l0042
    sta l0043
    sta l0044
    sta l0045
    pla
c90b6:
    sta output_buffer,x
    inx
    cmp #0x0d
    beq c90e2
    cmp #0x20 ; ' '
    beq c9048
    lda l0048
    beq c9048
    clc
    adc l0044
    sta l0044
    lda l0046
    adc l0048
    sta l0046
    lda l0045
    adc l0043
    sta l0045
    lda #0
    sta l0048
    sta l0042
    sta l0043
    jmp c9048

c90e2:
    lda l0045
    beq c90f8
    lda ruler_right_stop
    beq c90f8
    sec
    sbc l0047
    bcc c90f8
    sbc l0045
    adc #0
    sec
    sbc l0046
    beq c9101
c90f8:
    lda #0
    sta l0039
    ldy input_buffer_ptr+1
    jmp c8fe6

c9101:
    lda #0
    sta tmp9
    ldx #8
loop_c9107:
    asl
    rol tmp9
    asl l0045
    bcc c9115
    clc
    adc microspacing_flag
    bcc c9115
    inc tmp9
c9115:
    dex
    bne loop_c9107
    sta tmp8
    lda l0044
    sta l0046
    jsr sub_cadf0
    sta l0045
    lda tmp8
    sta l0044
    ldy #0
    sty l0039
c912b:
    lda output_buffer,y
    iny
    jsr sub_c9431
    pha
    lda l0039
    cmp l0047
    beq c913b
    bcs c9142
c913b:
    pla
    jsr c9426
    jmp c9163

c9142:
    pla
    cmp #0x20 ; ' '
    bne c915b
    lda microspacing_flag
    clc
    adc l0044
    tax
    lda l0045
    beq c9154
    inx
    dec l0045
c9154:
    jsr sub_c9173
    lda #0x20 ; ' '
    bne c9160                                                         ; ALWAYS branch

c915b:
    ldx microspacing_flag
    jsr sub_c9173
c9160:
    jsr print_char
c9163:
    cmp #0x0d
    bne c912b
    jmp c8ffb

sub_c916a:
    ldx print_flags
    bpl return_25
    ldx microspacing_flag
    bne c9177
    rts

sub_c9173:
    cpx l0043
    beq return_25
c9177:
    jsr sub_c9445
    pha
    stx l0043
    lda #9
    jsr call_printer_driver
    pla
return_25:
    rts

c9184:
    lda #0
    sta macro_executing_flag
sub_c9188:
    lda macro_executing_flag
    bne c91a3
    lda ptr5
    sta input_buffer_ptr+1
    sta tmp0
    lda ptr5+1
    sta l0081
    sta tmp1
    jsr sub_c9241
    bcs return_26
    lda ptr5
    ldy ptr5+1
    bne c91d0
c91a3:
    ldy #0
    ldx #0
c91a7:
    lda (ptr3),y
    cmp #4
    beq c9184
    cmp #0x40 ; '@'
    beq c91da
    iny
loop_c91b2:
    sta current_line_buffer,x
    inx
    cmp #0x0d
    beq c91c2
    cpx #0x83
    bcc c91a7
    lda #0x0d
    bne loop_c91b2                                                    ; ALWAYS branch

c91c2:
    tya
    clc
    adc ptr3
    sta ptr3
    bcc c91cc
    inc ptr3+1
c91cc:
    lda ptr1
    ldy ptr1+1
c91d0:
    sta tmp0
    sty tmp1
    sta current_format_line_ptr
    sty current_format_line_ptr+1
    clc
return_26:
    rts

c91da:
    iny
    lda (ptr3),y
    sec
    sbc #0x30 ; '0'
    bcc c9225
    cmp #0x0a
    bcs c9225
    iny
    sty l0084
    sta l0083
    lda #0
    sta l0082
    ldy #2
loop_c91f1:
    dec l0083
    bmi c9209
c91f5:
    iny
    lda (ptr5),y
    cmp #0x0d
    beq c9223
    jsr sub_c9228
    beq c91f5
    bvs c91f5
    cmp #0x2c ; ','
    beq loop_c91f1
    bne c91f5                                                         ; ALWAYS branch

c9209:
    iny
    lda (ptr5),y
    cmp #0x0d
    beq c9223
    jsr sub_c9228
    beq c9209
    bvs c921b
    cmp #0x2c ; ','
    beq c9223
c921b:
    sta current_line_buffer,x
    inx
    cpx #0x82
    bcc c9209
c9223:
    ldy l0084
c9225:
    jmp c91a7

sub_c9228:
    cmp #0x3e ; '>'
    bne c9231
    lda #0
    sta l0082
    rts

c9231:
    cmp #0x3c ; '<'
    bne c923c
    lda #0x40 ; '@'
    sta l0082
    lda #0
    rts

c923c:
    bit l0082
    ora #0
    rts

sub_c9241:
    lda printing_from_file_flag
    beq c9260
    ldy #0
loop_c9247:
    lda (ptr6),y
    sec
    beq return_27
    sta (tmp0),y
    inc ptr6
    bne c9254
    inc ptr6+1
c9254:
    inc tmp0
    bne c925a
    inc tmp1
c925a:
    cmp #0x0d
    bne loop_c9247
    clc
return_27:
    rts

c9260:
    jmp read_block_from_file

c9263:
    lda l0038
    beq c9284
    ldx l0021                                                         ; X=number of lines
    jsr print_vertical_space
    ldx footer_margin                                                 ; X=number of lines
    jsr print_vertical_space
    lda footers_enabled_flag
    beq c927c
    ldx #<(footer_text_maybe)
    ldy #>(footer_text_maybe)
    jsr render_header_or_footer
c927c:
    jsr print_newline
    ldx bottom_margin                                                 ; X=number of lines
    jsr print_vertical_space
c9284:
    inc register_value_p
    bne c928c
    inc register_value_p+1
c928c:
    lda #1
    sta register_value_l
    lda #0
    sta register_value_l+1
    sta l0031
    rts

; ***************************************************************************************
render_new_page:
    lda #0x81
    sta l0031
    bit print_flags
    bvc c92d4
    jsr stop_printing
    jsr print_inline_string
    .ascii "\rPage "
    .byte 0

    ldx register_value_p
    ldy register_value_p+1
    jsr render_number_to_screen
    jsr print_inline_string
    .ascii ".."
    .byte 0

    jsr flush_and_read_char
    bcs c92cc
    and #0xdf
    cmp #0x4d ; 'M'
    beq c92d4
    cmp #0x51 ; 'Q'
    bne c92cf
c92cc:
    jmp c8f1a

c92cf:
    lda #0xc0
    jsr start_printing
c92d4:
    lda l0038
    beq c92f0
    ldx top_margin                                                    ; X=number of lines
    jsr print_vertical_space
    lda headers_enabled_flag
    beq c92e8
    ldx #<(header_text_maybe)
    ldy #>(header_text_maybe)
    jsr render_header_or_footer
c92e8:
    jsr print_newline
    ldx header_margin                                                 ; X=number of lines
    jsr print_vertical_space
c92f0:
    ldx page_length
    lda l0038
    beq c930d
    ldx #1
    lda page_length
    clc
    sbc top_margin
    bcc c930d
    sbc header_margin
    bcc c930d
    clc
    sbc bottom_margin
    bcc c930d
    sbc footer_margin
    bcc c930d
    tax
c930d:
    stx l0021
    rts

; ***************************************************************************************
render_header_or_footer:
    stx tmp4
    sty tmp5
    ldy #0
    sty l0082
    lda (tmp4),y
    beq return_28
    jsr sub_c9407
    lda #0
    sta l0039
    jsr sub_c9393
    jsr sub_c93fd
    bcs c932e
    jsr sub_c93a1
c932e:
    jsr sub_c93c8
    jsr c937b
    jsr sub_c939b
    jsr sub_c93c8
    txa
    beq c9355
    dex
    txa
    lsr
    sta l0081
    jsr sub_c93be
    beq c9355
    lsr
    sec
    sbc l0081
    bcc c9355
    sbc l0039
    bcc c9355
    tax
    jsr sub_c941a
c9355:
    jsr c937b
    jsr sub_c93a1
    jsr sub_c93fd
    bcs c9363
    jsr sub_c9393
c9363:
    jsr sub_c93c8
    jsr sub_c93be
    beq c937b
    stx l0081
    sec
    sbc l0081
    bcc c937b
    sbc l0039
    bcc c937b
    tax
    inx
    jsr sub_c941a
c937b:
    ldy #0
    ldx l0084
    beq return_28
loop_c9381:
    txa
    pha
    lda output_buffer,y
    jsr sub_c9431
    jsr print_char
    iny
    pla
    tax
    dex
    bne loop_c9381
return_28:
    rts

sub_c9393:
    jsr sub_c93b6
    lda #0
    jmp c93aa

sub_c939b:
    jsr sub_c93b6
    jmp c93a7

sub_c93a1:
    jsr sub_c93b6
    jsr c93b8
c93a7:
    iny
    tya
    dey
c93aa:
    clc
    adc tmp4
    sta tmp2
    lda tmp5
    adc #0
    sta tmp3
    rts

sub_c93b6:
    ldy #0xff
c93b8:
    iny
    lda (tmp4),y
    bpl c93b8
    rts

sub_c93be:
    lda ruler_right_stop
    bne return_29
    lda l003a
    sec
    sbc #1
return_29:
    rts

sub_c93c8:
    ldx #0
    ldy #0
    sty l0081
c93ce:
    lda (tmp2),y
    bmi c93e6
    jsr check_for_control_code
    bne c93d9
    inc l0081
c93d9:
    iny
    cmp #0x7c ; '|'
    beq c93f2
    sta output_buffer,x
    inx
    cpx #0x84
    bcc c93ce
c93e6:
    stx l0084
    lda print_flags
    bpl return_30
    txa
    sec
    sbc l0081
    tax
return_30:
    rts

c93f2:
    lda (tmp2),y
    bmi c93e6
    iny
    jsr render_register
    jmp c93ce

sub_c93fd:
    sec
    lda two_sided_flag
    beq return_31
    lda register_value_p
    lsr
return_31:
    rts

sub_c9407:
    jsr sub_c93fd
    lda left_margin
    bcc c9415
    ldx two_sided_flag
    beq c9415
    clc
    adc rhs_extra_margin
c9415:
    tax
    lda #0x20 ; ' '
    bne c9426                                                         ; ALWAYS branch

sub_c941a:
    txa
    clc
    adc l0039
    sta l0039
    lda #0x20 ; ' '
    bne c9426                                                         ; ALWAYS branch

; ***************************************************************************************
; On Entry:
;     X: number of lines
; ***************************************************************************************
print_vertical_space:
    lda #0x0d
c9426:
    inx
    dex
    beq return_32
loop_c942a:
    jsr print_char
    dex
    bne loop_c942a
return_32:
    rts

sub_c9431:
    jsr sub_ca5ae
    bit print_flags
    bpl c943c
    ora #0
    bmi return_33                                                     ; ALWAYS branch

c943c:
    pha
    txa
    clc
    adc l0039
    sta l0039
    pla
return_33:
    rts

sub_c9445:
    pha
    lda print_xpos
    beq c9453
    lda #0x20 ; ' '
loop_c944c:
    jsr print_char_just_to_printer
    dec print_xpos
    bne loop_c944c
c9453:
    pla
    rts

; ***************************************************************************************
print_newline:
    lda #0x0d
; ***************************************************************************************
print_char:
    cmp #0x0d
    beq c9462
    cmp #0x20 ; ' '
    bne c9468
    inc print_xpos
    rts

c9462:
    lda #0
    sta print_xpos
    lda #0x0d
c9468:
    jsr sub_c9445
; ***************************************************************************************
print_char_just_to_printer:
    bit print_flags
    bpl c9472
    jmp (printer_driver_ptr)

c9472:
    jsr check_for_control_code
    bne c9488
    pha
    lda #0x2d ; '-'
    bcs c947e
    lda #0x2a ; '*'
c947e:
    jsr set_inverted_text_if_not_mode_7
    jsr bdos_print_char                                                        ; Write character
    pla
    jmp set_normal_text_if_not_mode_7

c9488:
    jmp bdos_print_char                                                        ; Write character

; ***************************************************************************************
prepare_printer_driver:
    ldx #<printer_driver_block
    ldy #>printer_driver_block
    lda printer_driver_name
    bne c949e
    ldx default_printer_driver_ptr
    ldy l94b2
    lda #0
    sta microspacing_flag
c949e:
    stx printer_driver_ptr
    sty printer_driver_ptr+1
return_35:
    rts

; ***************************************************************************************
call_printer_driver:
    clc
    adc printer_driver_ptr
    sta tmp8
    lda printer_driver_ptr+1
    adc #0
    sta tmp9
    jmp (tmp8)

default_printer_driver_ptr:
l94b2 = default_printer_driver_ptr+1
    .word default_printer_driver

; ***************************************************************************************
default_printer_driver:
    jmp c94c0

    jmp c94c7

    jmp c94cb

    jmp return_34

    rts

c94c0:
    cmp #0x80
    bcs return_35
    jmp bdos_print_char                                                        ; Write character

c94c7:
    lda #2
    bne c94cd                                                         ; ALWAYS branch

c94cb:
    lda #3
c94cd:
    jmp oswrch                                                        ; Write character 3

; ***************************************************************************************
lj_fmt_cmd:
    jsr expand_line
    bcc return_36
    lda #0
    beq c950f                                                         ; ALWAYS branch

; ***************************************************************************************
ce_fmt_cmd:
    jsr expand_line
    bcc return_36
    txa
    beq return_36
    lsr
    sta l0084
    lda ruler_right_stop
    beq c950f
    sec
    sbc ruler_left_stop
    lsr
    sec
    adc ruler_left_stop
    sec
    sbc l0084
    bcs c950f
    lda #0
    beq c950f                                                         ; ALWAYS branch

; ***************************************************************************************
rj_fmt_cmd:
    jsr expand_line
    bcc c9529
    txa
    beq c9529
    dex
    dex
    lda #0
    cpx ruler_right_stop
    bcs c950f
    stx l0083
    lda ruler_right_stop
    sec
    sbc l0083
c950f:
    ldy #3
    tax
    beq c951c
    lda #0x20 ; ' '
loop_c9516:
    sta (current_format_line_ptr),y
    iny
    dex
    bne loop_c9516
c951c:
    lda output_buffer,x
    sta (current_format_line_ptr),y
    iny
    inx
    cmp #0x0d
    bne c951c
    inc l0030
c9529:
    sec
return_36:
    rts

; ***************************************************************************************
expand_line:
    ldx #0
    stx l0083
    ldy #3
    jsr get_current_fmt_cmd_byte
    clc
    beq return_37
c9537:
    lda (current_format_line_ptr),y
    iny
    cmp #0x7c ; '|'
    beq c955e
c953e:
    sta output_buffer,x
    jsr check_for_control_code
    bne c9548
    inc l0083
c9548:
    inx
    cmp #0x0d
    beq c9555
    cpx #0x83
    bcc c9537
    lda #0x0d
    bne c953e                                                         ; ALWAYS branch

c9555:
    lda print_flags
    bpl return_37
    txa
    sbc l0083
    tax
return_37:
    rts

c955e:
    lda (current_format_line_ptr),y
    cmp #0x0d
    beq c953e
    iny
    jsr render_register
    jmp c9537

; ***************************************************************************************
dh_fmt_cmd:
    ldx #<(header_text_maybe)
    ldy #>(header_text_maybe)
    bne c9575                                                         ; ALWAYS branch

; ***************************************************************************************
df_fmt_cmd:
    ldx #<(footer_text_maybe)
    ldy #>(footer_text_maybe)
c9575:
    stx tmp2
    sty tmp3
    lda #0
    sta l0081
    sta l007a
    ldy #3
    sty input_buffer_ptr+1
    lda (current_format_line_ptr),y
    sta l0083
    ldx #0x3f ; '?'
loop_c9589:
    iny
    sty l0082
    lda (current_format_line_ptr),y
    cmp #0x0d
    beq c959c
    cmp #0x1b
    bcs c9598
    lda #0x20 ; ' '
c9598:
    cmp l0083
    bne c959e
c959c:
    ora #0x80
c959e:
    jsr sub_c95b2
    cmp #0x8d
    beq c95aa
    ldy l0082
    dex
    bne loop_c9589
c95aa:
    lda #0x80
    jsr sub_c95b2
    jsr sub_c95b2
sub_c95b2:
    ldy l0081
    sta (tmp2),y
    iny
    sty l0081
    rts

; ***************************************************************************************
em_fmt_cmd:
    ldy #3
    jsr get_current_fmt_cmd_byte
    beq return_38
    iny
    jsr get_register_address
    bcs return_38
    lda tmp6
    sta tmp0
    lda tmp7
    sta tmp1
    jsr evaluate_expression_from_fmt_cmd
    ldy #0
    sta (tmp0),y
    iny                                                               ; Y=0x01
    lda tmp9
    sta (tmp0),y
return_38:
    rts

; ***************************************************************************************
pl_fmt_cmd:
    ldy #3
    jsr evaluate_expression_from_fmt_cmd
    sta page_length
    rts

; ***************************************************************************************
ts_fmt_cmd:
    ldy #3
    jsr parse_boolean_from_fmt_cmd
    bcs return_39
    sta two_sided_flag
    jsr evaluate_expression_from_fmt_cmd
    sta rhs_extra_margin
return_39:
    rts

; ***************************************************************************************
tm_fmt_cmd:
    ldy #3
    jsr evaluate_expression_from_fmt_cmd
    sta top_margin
    rts

; ***************************************************************************************
bm_fmt_cmd:
    ldy #3
    jsr evaluate_expression_from_fmt_cmd
    sta bottom_margin
    rts

; ***************************************************************************************
hm_fmt_cmd:
    ldy #3
    jsr evaluate_expression_from_fmt_cmd
    sta header_margin
    rts

; ***************************************************************************************
fm_fmt_cmd:
    ldy #3
    jsr evaluate_expression_from_fmt_cmd
    sta footer_margin
    rts

; ***************************************************************************************
lm_fmt_cmd:
    ldy #3
    jsr evaluate_expression_from_fmt_cmd
    sta left_margin
    rts

; ***************************************************************************************
ls_fmt_cmd:
    ldy #3
    jsr evaluate_expression_from_fmt_cmd
    sta line_spacing
    rts

; ***************************************************************************************
pe_fmt_cmd:
    ldy #3
    jsr evaluate_expression_from_fmt_cmd
    tax
    beq page_eject_fmt
    cmp l0021
    bcc return_40
    lda l0031
    bne page_eject_fmt
return_40:
    rts

; ***************************************************************************************
op_fmt_cmd:
    lda register_value_p
    lsr
    bcc page_eject_fmt
    bcs c9642                                                         ; ALWAYS branch

; ***************************************************************************************
ep_fmt_cmd:
    lda register_value_p
    lsr
    bcs page_eject_fmt
c9642:
    jsr page_eject_fmt
; ***************************************************************************************
page_eject_fmt:
    lda l0031
    bne c964c
    jsr render_new_page
c964c:
    jmp c9263

; ***************************************************************************************
fo_fmt_cmd:
    ldy #3
    jsr parse_boolean_from_fmt_cmd
    bcs return_41
    sta footers_enabled_flag
return_41:
    rts

; ***************************************************************************************
he_fmt_cmd:
    ldy #3
    jsr parse_boolean_from_fmt_cmd
    bcs return_42
    sta headers_enabled_flag
return_42:
    rts

; ***************************************************************************************
pb_fmt_cmd:
    ldy #3
    jsr parse_boolean_from_fmt_cmd
    bcs return_43
    sta l0038
return_43:
    rts

; ***************************************************************************************
dm_fmt_cmd:
    lda macro_executing_flag
    bne return_42
    lda last_macro_ptr
    sta tmp6
    lda last_macro_ptr+1
    sta tmp7
    ldy #3
    lda (current_format_line_ptr),y
    and #0xdf
    sta l0084
    iny                                                               ; Y=0x04
    lda (current_format_line_ptr),y
    jsr is_uppercase
    bcc c968d
    lda #0x20 ; ' '
    bne c968f                                                         ; ALWAYS branch

c968d:
    and #0xdf
c968f:
    dey
    sta (last_macro_ptr),y
    dey
    lda l0084
    sta (last_macro_ptr),y
    lda #4
    clc
    adc last_macro_ptr
    sta last_macro_ptr
    bcc c96a2
    inc last_macro_ptr+1
c96a2:
    lda himem
    sec
    sbc last_macro_ptr
    tax
    lda himem+1
    sbc last_macro_ptr+1
    bne c96b8
    cpx #0x97
    bcs c96b8
    jmp display_not_enough_memory

c96b8:
    lda last_macro_ptr
    sta tmp0
    sta input_buffer_ptr+1
    sta current_format_line_ptr
    lda last_macro_ptr+1
    sta tmp1
    sta l0081
    sta current_format_line_ptr+1
    jsr sub_c9241
    bcc c96ce
    rts

c96ce:
    ldy #0
    lda (last_macro_ptr),y
    jsr check_for_command_prefix
    bne c96f8
    jsr lookup_formatting_command
    cpx #5
    bne c96f8
    lda #4
    ldy #0
    sta (last_macro_ptr),y
    inc last_macro_ptr
    bne add_macro_to_linked_list
    inc last_macro_ptr+1
add_macro_to_linked_list:
    lda #0
    sta (last_macro_ptr),y
    lda last_macro_ptr
    sta (tmp6),y
    iny
    lda last_macro_ptr+1
    sta (tmp6),y
    rts

c96f8:
    lda tmp0
    sta last_macro_ptr
    lda tmp1
    sta last_macro_ptr+1
    bne c96a2
; ***************************************************************************************
ht_fmt_cmd:
    ldy #3
    jsr get_current_fmt_cmd_byte
    beq return_44
    tax
    lda #0
    cpx #0x2d ; '-'
    beq c9716
    lda #1
    cpx #0x2a ; '*'
    bne c9719
c9716:
    iny
    bne c9725
c9719:
    jsr evaluate_expression_from_fmt_cmd
    sec
    sbc #1
    bcc return_44
    cmp #2
    bcs return_44
c9725:
    pha
    jsr evaluate_expression_from_fmt_cmd
    pla
    tax
    lda tmp8
    sta highlight1_code,x
return_44:
    rts

; ***************************************************************************************
lookup_formatting_command:
    ldy #2
    lda (current_format_line_ptr),y
    sta tmp3
    dey                                                               ; Y=0x01
    lda (current_format_line_ptr),y
    sta tmp2
    dey                                                               ; Y=0x00
    ldx #0
loop_c973e:
    lda tmp2
    cmp commands_table,y
    bne c974c
    lda tmp3
    cmp lb2a1,y
    beq return_45
c974c:
    inx
    iny
    iny
    lda commands_table,y
    bpl loop_c973e
return_45:
    rts

; ***************************************************************************************
execute_formatting_command:
    txa
    ldy #0
    ldx #0
    stx l0030
    jsr call_through_jumptable
    ldx l0030
    rts

; ***************************************************************************************
parse_boolean_from_fmt_cmd:
    jsr get_current_fmt_cmd_byte
    sec
    beq return_46
    lda current_format_line_ptr
    ldx current_format_line_ptr+1
sub_c976c:
    sta tmp8
    stx tmp9
    lda (tmp8),y
    tax
    lda #1
    cpx #0x31 ; '1'
    beq c977f
    lda #0
    cpx #0x30 ; '0'
    bne c9783
c977f:
    clc
    iny
    bne return_46
c9783:
    dey
    sty l0084
    ldx #0xff
c9788:
    iny
    lda (tmp8),y
    jsr to_uppercase
    inx
    cmp l97b0,x
    beq c9788
    lda l97b0,x
    bmi c97ae
    cmp #0x20 ; ' '
    bcc return_46
loop_c979d:
    inx
    lda l97b0,x
    bmi c97ae
    cmp #0x20 ; ' '
    bcs loop_c979d
    ldy l0084
    lda l97b1,x
    bpl c9788
c97ae:
    sec
return_46:
    rts

l97b0:
    .byte 0x4f
l97b1:
    .byte 0x4e, 1
    .ascii "OFF"
    .byte 0, 0xff

; ***************************************************************************************
evaluate_expression_from_fmt_cmd:
    lda #0
    sta tmp8
    sta tmp9
    sta input_buffer_ptr+1
c97c0:
    jsr get_current_fmt_cmd_byte
    beq c9821
    cmp #0x7c ; '|'
    bne c97d5
    jsr get_next_fmt_cmd_byte
    beq c9821
    iny
    jsr render_register
    jmp c97dc

c97d5:
    jsr ca6fe
    sta tmp8
    stx tmp9
c97dc:
    ldx input_buffer_ptr+1
    beq c9804
    lda #0
    sta input_buffer_ptr+1
    dex
    beq c97f7
    lda tmp4
    sec
    sbc tmp8
    sta tmp8
    lda tmp5
    sbc tmp9
    sta tmp9
    jmp c9804

c97f7:
    lda tmp4
    clc
    adc tmp8
    sta tmp8
    lda tmp5
    adc tmp9
    sta tmp9
c9804:
    lda tmp8
    sta tmp4
    lda tmp9
    sta tmp5
    jsr get_current_fmt_cmd_byte
    beq c9821
    ldx #1
    cmp #0x2b ; '+'
    beq c981c
    inx                                                               ; X=0x02
    cmp #0x2d ; '-'
    bne c9821
c981c:
    stx input_buffer_ptr+1
    iny
    bne c97c0
c9821:
    lda tmp8
    rts

; ***************************************************************************************
get_next_fmt_cmd_byte:
    iny
; ***************************************************************************************
get_current_fmt_cmd_byte:
    lda (current_format_line_ptr),y
    cmp #0x0d
    beq return_47
    cmp #0x20 ; ' '
    beq get_next_fmt_cmd_byte
return_47:
    rts

sub_c9830:
    lda justifying_flag
    bne return_47
    sta l0046
    sta l0039
    sta l0042
    lda ruler_right_stop
    beq return_47
    jsr get_line_length
    sty l0043
    ldy #0
    beq c9861                                                         ; ALWAYS branch

c9847:
    lda l0039
    sta l0084
    iny
    cpy l0043
    beq c9871
    clc
    jsr sub_c9936
    beq c985c
    cmp #0x20 ; ' '
    bne c9847
    inc l0046
c985c:
    iny
    cpy l0043
    beq c986d
c9861:
    sec
    jsr sub_c9936
    beq c985c
    cmp #0x20 ; ' '
    bne c9847
    beq c985c                                                         ; ALWAYS branch

c986d:
    dec l0046
    bmi return_47
c9871:
    lda l0046
    beq return_47
    lda ruler_right_stop
    sec
    sbc l0084
    bcc return_47
    adc #0
    tax
    adc l0043
    sec
    sbc #0x84
    bcc c988c
    sta l0084
    txa
    sbc l0084
    tax
c988c:
    stx l0082
    stx tmp8
    lda #0
    sta tmp9
    jsr sub_cadf0
    sta l0045
    lda tmp8
    sta l0044
    ldy #0
    ldx l0046
    tya                                                               ; A=0x00
loop_c98a2:
    sta input_buffer,y
    iny
    dex
    bne loop_c98a2
    ldy print_xpos
    iny
    cpy l0046
    bcc c98b2
    ldy #1
c98b2:
    dey
    ldx l0046
c98b5:
    lda l0045
    beq c98bd
    lda #1
    dec l0045
c98bd:
    clc
    adc l0044
    sta input_buffer,y
    lda l0082
    sec
    sbc input_buffer,y
    php
    sta l0082
    iny
    cpy l0046
    bcc c98d3
    ldy #0
c98d3:
    plp
    beq c98d9
    dex
    bne c98b5
c98d9:
    sty print_xpos
    ldy #0
    sty l0081
    sty l0039
    lda #0x1a
    jsr wipe_buffer
    lda l0042
    beq c98f6
    ldy #0
loop_c98ec:
    lda output_buffer,y
    sta (current_edit_line_ptr),y
    iny
    cpy l0042
    bne loop_c98ec
c98f6:
    ldy l0042
    ldx l0042
c98fa:
    lda output_buffer,x
    cmp #0x20 ; ' '
    bne c9920
    lda l0081
    beq c991c
    sty l0084
    ldy l0039
    cpy l0046
    lda #0
    bcs c9912
    lda input_buffer,y
c9912:
    clc
    adc l0084
    inc l0039
    tay
    lda #0
    sta l0081
c991c:
    lda #0x20 ; ' '
    bne c9922                                                         ; ALWAYS branch

c9920:
    inc l0081
c9922:
    sta (current_edit_line_ptr),y
    iny
    inx
    cpx l0043
    bne c98fa
    lda #0x10
loop_c992c:
    cpy #0x84
    bcs return_48
    sta (current_edit_line_ptr),y
    iny
    bne loop_c992c
return_48:
    rts

sub_c9936:
    ror l0083
    lda (current_edit_line_ptr),y
    sta output_buffer,y
    cmp #9
    bne c994a
    jsr sub_ca5ae
    txa
    clc
    adc l0039
    bne c995c
c994a:
    cmp #0x0b
    bne c9969
    lda ruler_left_stop
    beq c9967
    ldx l0039
    beq c995c
    cpx ruler_left_stop
    bcc c995c
    inx
    txa
c995c:
    sta l0039
    sty l0042
    inc l0042
    lda #0
    sta l0046
    rts

c9967:
    lda #0x20 ; ' '
c9969:
    cmp #0x1b
    bcc c9967
    cmp #0x20 ; ' '
    bcc return_49
    inc l0039
return_49:
    rts

c9974:
    jmp c9a8d

sub_c9977:
    inc cursor_moved_flag
    ldy #4
    sty print_xpos
    ldy #0
    sty input_buffer_ptr
    sty l007e
    lda (ptr4),y
    jsr check_for_command_prefix
    beq c9974
c998a:
    lda format_mode_flag
    and #0x81
    bne c9974
    lda ruler_right_stop
    beq c9974
    sec
    sbc ruler_left_stop
    bcc c9974
    adc #1
    sta input_buffer_ptr+1
    lda #0x10
    jsr wipe_buffer
    lda ptr4
    sta tmp6
    lda ptr4+1
    sta tmp7
    ldy #0
    sty l0047
    sty l0039
    sty l0038
    sty l0046
    sty bottom_margin
c99b6:
    sty l0048
    ldy l0047
loop_c99ba:
    jsr sub_ca536
    bne c99c7
    lda #0
    sta markers_array+1,x
    inc l007e
    bne loop_c99ba
c99c7:
    ldy l0047
c99c9:
    lda (ptr4),y
    iny
    sty l0047
    cmp #9
    bne c99e0
    jsr sub_ca5ae
    dex
    txa
    clc
    adc l0039
    sta l0039
    lda #9
    bne c9a21                                                         ; ALWAYS branch

c99e0:
    cmp #0x1a
    bne c99ee
c99e4:
    lda l0046
    bne c99c9
    ldx #0xff
    lda #0x20 ; ' '
    bne c9a2e                                                         ; ALWAYS branch

c99ee:
    cmp #0x0b
    bne c9a11
    ldx input_buffer_ptr
    bne c99e4
    lda l0038
    bne c99e4
    inc l0038
    lda ruler_left_stop
    beq c99c9
    ldx l0039
    cpx ruler_left_stop
    bcs c9a0a
    sta l0039
    dec l0039
c9a0a:
    clc
    adc input_buffer_ptr+1
    sta input_buffer_ptr+1
    lda #0x0b
c9a11:
    cmp #0x0d
    bne c9a21
    dey
    beq c9a8d
    jsr sub_c9ac1
    bcs c9a87
    lda #0x20 ; ' '
    sta input_buffer_ptr
c9a21:
    ldy l0048
    ldx #0
    cmp #0x20 ; ' '
    bne c9a2e
    inx                                                               ; X=0x01
    bit l0046
    bmi c9a40
c9a2e:
    ldy l0048
    sta (current_edit_line_ptr),y
    cmp #0x20 ; ' '
    bne c9a38
    ror bottom_margin
c9a38:
    iny
    jsr check_for_control_code
    beq c9a40
    inc l0039
c9a40:
    bit l0046
    stx l0046
    bmi c9a58
    cmp #0x20 ; ' '
    beq c9a58
    cpy #0x85
    bcs c9a60
    lda bottom_margin
    beq c9a58
    lda l0039
    cmp input_buffer_ptr+1
    bcs c9a60
c9a58:
    cpy #0x86
    bcc c9a5d
    dey
c9a5d:
    jmp c99b6

c9a60:
    inc l0047
loop_c9a62:
    dec l0047
    dey
    beq c9a8d
    lda (current_edit_line_ptr),y
    pha
    lda #0x10
    sta (current_edit_line_ptr),y
    pla
    cmp #0x20 ; ' '
    bne loop_c9a62
    sec
    ror input_buffer_ptr
    jsr sub_caed6
    jsr sub_c9830
    jsr sub_c9aa9
    jsr c9a8d
    beq c9aa5
    jmp c998a

c9a87:
    jsr sub_caed6
    jsr sub_c9aa9
c9a8d:
    jsr c9e94
    lda ptr4
    ldy ptr4+1
    jsr sub_cab1a
    sec
    beq c9aa5
    tya
    clc
    adc tmp0
    sta ptr4
    bcc c9aa4
    inc ptr4+1
c9aa4:
    clc
c9aa5:
    clv
    lda l007e
    rts

sub_c9aa9:
    sec
    rol l007e
    ldy l0047
    dey
    sty l003b
    inc l006e
    jsr sub_ca8b9
    bcc return_50
    pla
    pla
    lda #0x40 ; '@'
    sta l0084
    bit l0084
return_50:
    rts

sub_c9ac1:
    tya
    sec
    adc ptr4
    sta tmp8
    sta tmp4
    lda ptr4+1
    adc #0
    sta tmp9
    sta tmp5
    ldy #0
    sty l0083
c9ad5:
    lda (tmp4),y
    beq c9b2f
    jsr check_for_command_prefix
    beq c9b2f
    cmp #0x0d
    beq c9b2f
    tya
    bne c9b06
    sty l0084
    beq c9aef                                                         ; ALWAYS branch

c9ae9:
    inc tmp8
    bne c9aef
    inc tmp9
c9aef:
    lda (tmp8),y
    beq c9b06
    cmp #0x0d
    beq c9b06
    cmp #9
    beq c9b2f
    cmp #0x0b
    bne c9ae9
    rol l0084
    sec
    ror l0084
    bcs c9ae9
c9b06:
    lda (tmp4),y
    cmp #0x20 ; ' '
    bne c9b1a
    ldx ruler_left_stop
    beq c9b2f
    ldx l0084
    beq c9b2f
    ldx l0083
    bne c9b2f
    beq c9b20                                                         ; ALWAYS branch

c9b1a:
    cmp #0x0b
    bne c9b23
    sta l0083
c9b20:
    iny
    bne c9ad5
c9b23:
    lda ruler_left_stop
    beq c9b31
    lda l0084
    beq c9b31
    lda l0083
    bne c9b31
c9b2f:
    sec
    rts

c9b31:
    clc
    rts

run_editor:
    jsr enter_editor_mode
; ***************************************************************************************
editor_loop:
    lda format_mode_flag
    pha
    lda l006e
    bne c9b44
    pha
    jsr sub_caa97
    pla
    sta l006e
c9b44:
    jsr sub_ca608
    lda ruler_left_stop
    beq c9b73
    ldx format_mode_flag
    bmi c9b73
    cmp l0072
    bcc c9b73
    beq c9b73
    ldx cursor_moved_flag
    bne c9b6a
    jsr get_line_length
    lda format_mode_flag
    cpy xpos
    bcs c9b84
    bit format_mode_flag
    bvs c9b6a
    sty xpos
    bvc c9b84                                                         ; ALWAYS branch

c9b6a:
    lda ruler_left_stop
    sta l0072
    inc l0079
    jsr sub_ca608
c9b73:
    lda format_mode_flag
    and #0xbf
    pha
    jsr sub_caec2
    pla
    bcs c9b86
    cpy xpos
    bcc c9b86
    beq c9b86
c9b84:
    ora #0x40 ; '@'
c9b86:
    sta format_mode_flag
    pla
    cmp format_mode_flag
    beq c9b8f
    inc flags_need_redrawing_flag
c9b8f:
    lda #0
    sta cursor_moved_flag
    jsr sub_ca276
c9b96:
    jsr read_char
    cmp current_tab_key
    bne c9b9f
    lda #9
c9b9f:
    sta l0038
    tay
    bmi c9bbb
    cmp #0x20 ; ' '
    bcc enter_nonprintable_character
    cmp #0x7f
    bcc enter_printable_character
enter_nonprintable_character:
    tya
    ldx #<non_function_key_table
    ldy #>non_function_key_table
    jsr look_up_address_in_table
    bcs c9b96
    jsr jsr_tmp6
    jmp editor_loop
    
jsr_tmp6:
    jmp (tmp6)

c9bca:
    jsr beep
c9bbb:
    jmp editor_loop

enter_printable_character:
    ldy xpos
    cpy #0x84
    bcs c9bca
    inc l006d
    jsr sub_caef4
    bcs c9bca
    lda current_edit_line_ptr
    sta tmp6
    lda current_edit_line_ptr+1
    sta tmp7
    ldy xpos
    jsr sub_ca536
    bne c9bf2
    cpx #4
    bcs c9bf2
    inc l0074
c9bf2:
    ldx insert_mode_flag
    bne c9c00
    lda (current_edit_line_ptr),y
    cmp #9
    beq c9c00
    cmp #0x0b
    bne c9c09
c9c00:
    inc l0074
    ldx #1
    jsr sub_cae06
    bcs c9c7f
c9c09:
    lda l0038
    sta (current_edit_line_ptr),y
    ldy l0074
    bne c9c14
    jsr screen_putchar
c9c14:
    inc xpos
    jsr ca684
    ldy #0
    sty l0039
c9c1d:
    lda (current_edit_line_ptr),y
    iny
    cpy xpos
    bcs c9c56
    cmp #9
    bne c9c31
    jsr sub_ca5ae
    txa
    clc
    adc l0039
    bne c9c43
c9c31:
    cmp #0x0b
    bne c9c4a
    lda ruler_left_stop
    beq c9c48
    ldx l0039
    beq c9c43
    cpx ruler_left_stop
    bcc c9c43
    inx
    txa
c9c43:
    sta l0039
    jmp c9c1d

c9c48:
    lda #0x20 ; ' '
c9c4a:
    cmp #0x1b
    bcc c9c48
    cmp #0x20 ; ' '
    bcc c9c1d
    inc l0039
    bne c9c1d
c9c56:
    ldy l0039
    cpy l003a
    bcs c9c67
    lda (current_ruler_ptr),y
    and #0xdf
    cmp #0x42 ; 'B'
    bne c9c67
    jsr beep
c9c67:
    lda l0038
    cmp #0x20 ; ' '
    beq c9c7f
    lda ruler_right_stop
    beq c9c7f
    lda format_mode_flag
    bne c9c7f
    sta tmp7
    tya
    beq c9c7f
    dey
    cpy ruler_right_stop
    bcs c9c82
c9c7f:
    jmp editor_loop

c9c82:
    jsr get_line_length
    sty l0083
    lda #0
    sta top_margin
    ldy xpos
    sty input_buffer_ptr+1
    jsr draw_previous_word
    jsr sub_ca608
    lda l0072
    cmp ruler_left_stop
    beq c9c9d
    bcs c9ca2
c9c9d:
    ldy input_buffer_ptr+1
    dey
    sty xpos
c9ca2:
    lda input_buffer_ptr+1
    sec
    sbc xpos
    sta top_margin
    lda l0083
    sec
    sbc xpos
    sta l0083
    tay
    iny
    lda ruler_left_stop
    beq c9cb9
    inc top_margin
    iny
c9cb9:
    sty tmp6
    lda ptr4
    sec
    adc l003b
    sta tmp4
    lda ptr4+1
    adc #0
    sta tmp5
    jsr make_space_for_insertion
    bcc c9cd0
    jmp ca941

c9cd0:
    ldy #0
    lda ruler_left_stop
    beq c9cdb
    lda #0x0b
    sta (tmp4),y
    iny                                                               ; Y=0x01
c9cdb:
    sty l0081
    lda current_edit_line_ptr
    sta tmp6
    lda current_edit_line_ptr+1
    sta tmp7
    ldy xpos
    dey
    lda (current_edit_line_ptr),y
    cmp #0x20 ; ' '
    bne c9cf2
    lda #0x10
    sta (current_edit_line_ptr),y
c9cf2:
    iny
    sty l0082
c9cf5:
    ldy l0082
    inc l0082
loop_c9cf9:
    jsr sub_ca536
    bne c9d0d
    lda l0081
    clc
    adc tmp4
    sta markers_array,x
    lda tmp5
    adc #0
    sta markers_array+1,x
    bcc loop_c9cf9
c9d0d:
    lda l0083
    bne c9d28
    lda #0x0d
    bne c9d30                                                         ; ALWAYS branch

c9d15:
    jsr sub_c9830
    jsr ca93c
    jsr ca741
    jsr return_key
    lda top_margin
    sta xpos
    jmp editor_loop

c9d28:
    lda (current_edit_line_ptr),y
    pha
    lda #0x10
    sta (current_edit_line_ptr),y
    pla
c9d30:
    ldy l0081
    inc l0081
    sta (tmp4),y
    dec l0083
    bpl c9cf5
    bmi c9d15                                                         ; ALWAYS branch

; ***************************************************************************************
sf1_swap_case_key:
    ldy xpos
    lda (current_edit_line_ptr),y
    jsr is_uppercase
    bcs f13_right_key
    inc l0074
    eor #0x20 ; ' '
    sta (current_edit_line_ptr),y
; ***************************************************************************************
f13_right_key:
    ldy xpos
    cpy #0x84
    bcs return_51
    inc xpos
return_51:
    rts

; ***************************************************************************************
f12_left_key:
    ldy l0072
    beq return_52
    dec xpos
return_52:
    rts

; ***************************************************************************************
f15_up_key:
    jsr ca93c
    lda ptr4
    ldy ptr4+1
    jsr sub_cab37
    bcc return_53
    lda tmp0
    sta ptr4
    lda tmp1
    sta ptr4+1
    inc l0079
    inc cursor_moved_flag
return_53:
    rts

; ***************************************************************************************
f14_down_key:
    jsr ca93c
    inc l0079
    bne c9d9b
; ***************************************************************************************
return_key:
    jsr ca93c
    lda #0
    sta xpos
    lda ptr4
    sta tmp0
    lda ptr4+1
    sta tmp1
    jsr cab29
    bne c9d9b
    tya
    ldy ptr4+1
    clc
    adc ptr4
    bcc c9d98
    iny
c9d98:
    jsr sub_c9de1
c9d9b:
    inc cursor_moved_flag
    lda ptr4
    ldy ptr4+1
    jsr sub_cab1a
    beq return_54
    tya
    clc
    adc ptr4
    sta ptr4
    bcc return_54
    inc ptr4+1
return_54:
    rts

; ***************************************************************************************
cf6_split_line_key:
    jsr ca93c
    jsr get_line_length
    cpy xpos
    bcc c9dbd
    ldy xpos
c9dbd:
    inc l0079
    tya
    tax
    ldy #0
    lda (current_format_line_ptr),y
    jsr check_for_command_prefix
    bne c9dcd
    inx
    inx
    inx
c9dcd:
    ldy ptr4+1
    txa
    clc
    adc ptr4
    bcc c9de3
    iny
    bne c9de3
; ***************************************************************************************
f6_insert_line_key:
    jsr ca93c
    lda ptr4
    ldy ptr4+1
    inc l0079
sub_c9de1:
    inc cursor_moved_flag
c9de3:
    sta tmp4
    sty tmp5
    lda #1
    sta tmp6
    lda #0
    sta tmp7
    jsr make_space_for_insertion
    bcs c9dfd
    lda #0x0d
    ldy #0
    sta (tmp4),y
    jmp ca741

c9dfd:
    jmp ca941

; ***************************************************************************************
delete_key:
    lda l0072
    beq return_55
    dec xpos
    ldy xpos
    lda (current_edit_line_ptr),y
    pha
    jsr f9_delete_char_key
    pla
    cmp #0x0c
    bcc return_55
    ldx insert_mode_flag
    bne return_55
    jsr get_line_length
    cpy xpos
    bcc return_55
    beq return_55
; ***************************************************************************************
f8_insert_char_key:
    lda #0x20 ; ' '
sub_c9e22:
    pha
    ldx #1
    jsr sub_cae06
    pla
    bcs return_55
    sta (current_edit_line_ptr),y
    inc l0074
return_55:
    rts

; ***************************************************************************************
tab_key:
    lda #9
    bne c9e3a                                                         ; ALWAYS branch

; ***************************************************************************************
sf4_highlight1_key:
    lda #0x1c
    bne c9e3a                                                         ; ALWAYS branch

; ***************************************************************************************
sf5_highlight2_key:
    lda #0x1d
c9e3a:
    pha
    jsr sub_caef4
    pla
    bcs return_55
    jsr sub_c9e22
    bcs return_55
    jmp f13_right_key

; ***************************************************************************************
f9_delete_char_key:
    ldx #1
    inc l0074
    jmp cae64

; ***************************************************************************************
f7_delete_line_key:
    jsr ca93c
    inc cursor_moved_flag
    lda ptr4
    sta tmp4
    lda ptr4+1
    sta tmp5
    ldx l003b
    inx
    stx tmp6
    lda #0
    sta tmp7
    jsr adjust_pointers
    jsr cb05a
    ldy #0
    lda (ptr4),y
    bne c9e81
    lda ptr4
    ldy ptr4+1
    jsr sub_cab37
    lda tmp0
    sta ptr4
    lda tmp1
    sta ptr4+1
c9e81:
    inc l0079
    jmp ca741

; ***************************************************************************************
sf2_release_margins_key:
    bit format_mode_flag
    bvc c9e94
    jsr sub_caec2
    bcs f4_beginning_of_line_key
    sty xpos
    rts

; ***************************************************************************************
f4_beginning_of_line_key:
    inc cursor_moved_flag
c9e94:
    lda #0
    sta xpos
    rts

; ***************************************************************************************
f5_end_of_line_key:
    inc cursor_moved_flag
c9e9b:
    jsr get_line_length
    sty xpos
    rts

; ***************************************************************************************
cf7_join_lines_key:
    jsr ca93c
    lda ptr4
    sta tmp0
    lda ptr4+1
    sta tmp1
    jsr cab29
    beq c9eda
    jsr check_for_command_prefix
    beq c9eda
    dey
    tya
    clc
    adc ptr4
    sta tmp4
    lda ptr4+1
    adc #0
    sta tmp5
    lda #0
    sta tmp7
    lda #1
    sta tmp6
    jsr adjust_pointers
    lda ptr4
    ldy ptr4+1
    jsr cac78
    inc l0079
    jmp ca741

c9eda:
    jmp beep

; ***************************************************************************************
f3_delete_to_eol_key:
    lda #0x84
    sec
    sbc xpos
    tax
    inc l0074
    jmp cae64

; ***************************************************************************************
sf8_edit_command_key:
    jsr c9e94
    jsr sub_ca276
    inc l006d
    lda #0
    sta input_buffer_ptr+1
    sta l0081
edit_command_loop:
    ldx input_buffer_ptr+1
    ldy ypos
    jsr set_cursor_position
    jsr read_char
    bcs finished_editing_command
    cmp #0x0d
    beq finished_editing_command
    and #0xdf
    cmp #0x41 ; 'A'
    bcc edit_command_loop
    cmp #0x5b ; '['
    bcs edit_command_loop
    sta l0081
    jsr screen_putchar
    ldy input_buffer_ptr+1
    iny
    sty input_buffer_ptr+1
    sta (ptr1),y
    cpy #2
    bcc edit_command_loop
    lda #0
    sta input_buffer_ptr+1
    beq edit_command_loop                                             ; ALWAYS branch

finished_editing_command:
    lda l0081
    beq return_56
    lda ptr1
    sta current_format_line_ptr
    lda ptr1+1
    sta current_format_line_ptr+1
    ldy #0
    lda #0x80
    sta (current_format_line_ptr),y
    jmp caf5c

; ***************************************************************************************
cf8_mark_as_ruler_key:
    lda ptr1
    sta current_format_line_ptr
    lda ptr1+1
    sta current_format_line_ptr+1
    ldy #0
    lda #0x81
    sta (current_format_line_ptr),y
    iny                                                               ; Y=0x01
    lda #0x2e ; '.'
    sta (current_format_line_ptr),y
    iny                                                               ; Y=0x02
    lda #0x2e ; '.'
    sta (current_format_line_ptr),y
    inc l0074
    lda l006e
    bmi c9f5f
    lda #0x80
    sta l006e
    inc l006d
c9f5f:
    jmp caf5c

; ***************************************************************************************
sf9_delete_command_key:
    ldy #0
    lda (current_format_line_ptr),y
    jsr check_for_command_prefix
    bne return_56
    tya
    sta (current_format_line_ptr),y
    lda current_edit_line_ptr
    sta current_format_line_ptr
    lda current_edit_line_ptr+1
    sta current_format_line_ptr+1
    jsr sub_caf5f
    inc l0074
    inc l006d
    inc cursor_moved_flag
return_56:
    rts

c9f80:
    jsr ca93c
    lda ptr4
    ldy ptr4+1
    jsr sub_cab37
    bcc return_56
    lda tmp0
    sta ptr4
    lda tmp1
    sta ptr4+1
    jsr sub_caa97
    jsr c9e9b
    dec l006f
    rts

; ***************************************************************************************
sf12_left_key:
    ldy xpos
    beq c9f80
    jsr draw_previous_word
    bne return_57
    cmp #0x20 ; ' '
    beq c9f80
return_57:
    rts

c9fab:
    sty xpos
    jsr ca93c
    lda ptr4
    ldy ptr4+1
    jsr sub_cab1a
    beq return_58
    tya
    clc
    adc ptr4
    sta ptr4
    bcc c9fc3
    inc ptr4+1
c9fc3:
    jsr sub_caa97
    dec l006f
    jsr c9e94
    jsr get_line_length
    cpy xpos
    beq return_58
    lda current_edit_line_ptr
    sta tmp0
    lda current_edit_line_ptr+1
    sta tmp1
    ldy xpos
    jsr draw_char
    cmp #0x20 ; ' '
    bne return_58
; ***************************************************************************************
sf13_right_key:
    lda current_edit_line_ptr
    sta tmp0
    lda current_edit_line_ptr+1
    sta tmp1
    jsr get_line_length
    sty input_buffer_ptr+1
    cpy xpos
    bcc c9fab
    beq c9fab
    ldy xpos
loop_c9ff8:
    cpy input_buffer_ptr+1
    bcs ca00f
    jsr draw_char
    cmp #0x20 ; ' '
    bne loop_c9ff8
loop_ca003:
    cpy input_buffer_ptr+1
    bcs ca00f
    jsr draw_char
    cmp #0x20 ; ' '
    beq loop_ca003
    dey
ca00f:
    sty xpos
return_58:
    rts

; ***************************************************************************************
sf7_set_marker_key:
    jsr ca93c
    jsr prompt_for_marker
    bcs return_58
set_marker:
    jsr set_marker_to_here
    jmp ca035

set_marker_1:
    lda #'1'
    .byte 0x2c          ; skip next two bytes
set_marker_2:
    lda #'2'
    .byte 0x2c          ; skip next two bytes
set_marker_3:
    lda #'3'
    .byte 0x2c          ; skip next two bytes
set_marker_4:
    lda #'4'
    .byte 0x2c          ; skip next two bytes
set_marker_5:
    lda #'5'
    .byte 0x2c          ; skip next two bytes
set_marker_6:
    lda #'6'
    pha
    jsr ca93c
    pla
    jsr lookup_marker
    jmp set_marker

; ***************************************************************************************
sf6_go_to_marker_key:
    jsr ca93c
    jsr prompt_for_marker
    bcs return_58
    beq return_58
go_to_marker:
    lda markers_array,x
    ldy markers_array+1,x
    jsr move_cursor_to_address
ca035:
    lda #1
    sta l0073
    jmp ca684

go_to_marker_1:
    lda #'1'
    .byte 0x2c          ; skip next two bytes
go_to_marker_2:
    lda #'2'
    .byte 0x2c          ; skip next two bytes
go_to_marker_3:
    lda #'3'
    .byte 0x2c          ; skip next two bytes
go_to_marker_4:
    lda #'4'
    .byte 0x2c          ; skip next two bytes
go_to_marker_5:
    lda #'5'
    .byte 0x2c          ; skip next two bytes
go_to_marker_6:
    lda #'6'
    pha
    jsr ca93c
    pla
    jsr lookup_marker
    jmp go_to_marker

; ***************************************************************************************
f0_format_block_key:
    jsr ca93c
    lda l0073
    pha
    lda l003d
    pha
    jsr ca741
    jsr sub_c9977
    bvs ca05b
    sec
    bne ca051
    clc
ca051:
    pla
    tax
    pla
    bcs return_59
    stx l003d
    sta l0073
return_59:
    rts

ca05b:
    jmp ca941

; ***************************************************************************************
f1_top_of_text_key:
    ldx #0xff
    stx l006f
    jsr sub_ca071
    jsr sub_caa97
    jmp c9e94

; ***************************************************************************************
sf15_up_key:
    ldx screen_height
    inc l0079
    inc l006f
sub_ca071:
    inc cursor_moved_flag
    stx input_buffer_ptr+1
    jsr ca93c
    lda ptr4
    ldy ptr4+1
ca07c:
    sta tmp2
    sty tmp3
    jsr sub_cab37
    lda tmp0
    ldy tmp1
    bcc ca093
    ldx input_buffer_ptr+1
    bmi ca07c
    dec input_buffer_ptr+1
    bne ca07c
    beq ca097                                                         ; ALWAYS branch

ca093:
    lda tmp2
    ldy tmp3
ca097:
    sta ptr4
    sty ptr4+1
    rts

; ***************************************************************************************
f2_bottom_of_text_key:
    ldx #0xff
    stx l006f
    jsr sub_ca0af
    jsr sub_caa97
    jmp c9e9b

; ***************************************************************************************
sf14_down_key:
    ldx screen_height
    inc l0079
    inc l006f
sub_ca0af:
    inc cursor_moved_flag
    stx input_buffer_ptr+1
    jsr ca93c
    lda ptr4
    ldy ptr4+1
ca0ba:
    jsr sub_cab1a
    beq ca0d2
    tya
    ldy tmp1
    clc
    adc tmp0
    bcc ca0c8
    iny
ca0c8:
    ldx input_buffer_ptr+1
    bmi ca0ba
    dec input_buffer_ptr+1
    bne ca0ba
    beq ca0d6                                                         ; ALWAYS branch

ca0d2:
    lda tmp0
    ldy tmp1
ca0d6:
    sta ptr4
    sty ptr4+1
    rts

; ***************************************************************************************
sf11_copy_key:
    jsr f6_insert_line_key
    jsr sub_ca276
    ldx l003a
    beq ca0ef
    ldy #0
loop_ca0e7:
    lda (current_ruler_ptr),y
    sta (current_edit_line_ptr),y
    iny
    dex
    bne loop_ca0e7
ca0ef:
    jmp cf8_mark_as_ruler_key

; ***************************************************************************************
cf5_default_ruler_key:
    jsr f6_insert_line_key
    jsr sub_ca276
    jsr cf8_mark_as_ruler_key
    lda current_edit_line_ptr
    ldy current_edit_line_ptr+1
    jmp create_default_ruler

; ***************************************************************************************
sf3_delete_to_char_key:
    ldx #0x43 ; 'C'
    ldy #0x48 ; 'H'
    jsr draw_prompt_characters
    inc flags_need_redrawing_flag
    jsr read_char
    cmp #9
    beq ca12a
    cmp #0xa0
    bne ca11a
    lda #0x1c
    bne ca12a                                                         ; ALWAYS branch

ca11a:
    cmp #0xa1
    bne ca122
    lda #0x1d
    bne ca12a                                                         ; ALWAYS branch

ca122:
    cmp #0x20 ; ' '
    bcc ca151
    cmp #0x7f
    bcs ca151
ca12a:
    sta input_buffer_ptr+1
    inc l0074
    ldy xpos
    sty l0081
loop_ca132:
    cpy #0x84
    bcs ca151
    lda (current_edit_line_ptr),y
    iny
    cmp input_buffer_ptr+1
    bne loop_ca132
loop_ca13d:
    cpy #0x84
    bcs ca151
    lda (current_edit_line_ptr),y
    iny
    cmp input_buffer_ptr+1
    beq loop_ca13d
    dey
    tya
    sec
    sbc l0081
    tax
    jmp cae64

ca151:
    jmp beep

; ***************************************************************************************
cf2_format_mode_key:
    lda format_mode_flag
    and #0xbf
    bit format_mode_flag
    bvc ca15e
    ora #1
ca15e:
    eor #1
    sta format_mode_flag
    inc flags_need_redrawing_flag
    rts

; ***************************************************************************************
cf3_justify_mode_key:
    lda justifying_flag
    eor #0xff
    sta justifying_flag
    inc flags_need_redrawing_flag
    rts

; ***************************************************************************************
cf4_insert_mode_key:
    lda insert_mode_flag
    eor #0xff
    sta insert_mode_flag
    inc flags_need_redrawing_flag
return_60:
    rts

; ***************************************************************************************
cf0_delete_block_key:
    jsr ca93c
    inc cursor_moved_flag
    jsr reset_area_to_marks_1_2
    bcs ca1c9
    lda area_start_ptr
    ldy area_start_ptr+1
    jsr move_cursor_to_address
    jsr ca741
    jsr sub_c89d3
    jsr cb05a
    jmp clear_marks_1_2

; ***************************************************************************************
sf0_move_block_key:
    jsr ca93c
    jsr reset_area_to_marks_1_2
    bcs ca1c9
    jsr sub_ca1cc
    bcs return_60
    ldx #0xff
    stx l0012
    stx l006f
    jsr sub_c89d3
    jsr cb05a
    lda doc_ptr1+0
    ldy doc_ptr1+1
    jsr move_cursor_to_address
    jmp clear_marks_1_2

; ***************************************************************************************
f11_copy_key:
    jsr ca93c
    jsr reset_area_to_marks_1_2
    bcs ca1c9
    jsr sub_ca1cc
    lda doc_ptr1+0
    ldy doc_ptr1+1
    jmp move_cursor_to_address

ca1c9:
    jmp beep

sub_ca1cc:
    lda doc_ptr1+0
    ldy doc_ptr1+1
    cpy area_start_ptr+1
    bcc ca1ea
    bne ca1da
    cmp area_start_ptr
    bcc ca1ea
ca1da:
    cpy area_end_ptr+1
    bcc ca1c9
    bne ca1e6
    cmp area_end_ptr
    bcc ca1c9
    beq ca1c9
ca1e6:
    lda area_start_ptr
    ldy area_start_ptr+1
ca1ea:
    jsr move_cursor_to_address
    lda area_end_ptr
    sec
    sbc area_start_ptr
    sta tmp6
    lda area_end_ptr+1
    sbc area_start_ptr+1
    sta tmp7
    lda doc_ptr1+0
    sta tmp4
    lda doc_ptr1+1
    sta tmp5
    jsr make_space_for_insertion
    bcs ca265
    lda area_start_ptr
    sta tmp8
    lda area_start_ptr+1
    sta tmp9
    lda tmp4
    sta tmp2
    lda tmp5
    sta tmp3
    ldy #0
ca219:
    lda (tmp8),y
    sta (tmp2),y
    inc tmp2
    bne ca223
    inc tmp3
ca223:
    inc tmp8
    bne ca229
    inc tmp9
ca229:
    lda tmp8
    ldx tmp9
    cpx area_end_ptr+1
    bne ca219
    cmp area_end_ptr
    bne ca219
    lda tmp6
    pha
    lda tmp7
    pha
    lda tmp4
    sta doc_ptr1+0
    lda tmp5
    sta doc_ptr1+1
    lda tmp2
    ldy tmp3
    sec
    sbc #1
    bcs ca24d
    dey
ca24d:
    jsr cac78
    lda doc_ptr1+0
    ldy doc_ptr1+1
    jsr cac78
    pla
    sta tmp7
    pla
    sta tmp6
    lda #1
    sta l0073
    sta cursor_moved_flag
    clc
    rts

ca265:
    jmp ca941

; ***************************************************************************************
cf1_next_match_key:
    jsr ca93c
    jsr c8b7b
    bne ca273
    jmp move_cursor_to_address

ca273:
    jmp esc_key

sub_ca276:
    jsr cursor_off
    lda l0070
    sta l0034
    lda l0076
    sta input_buffer_ptr+1
    lda l006e
    beq ca28e
    lda l0073
    ora l006f
    bne ca28e
    jmp ca360

ca28e:
    lda ptr4+1
    cmp l0012
    bcc ca29c
    bne ca2dc
    lda ptr4
    cmp l0011
    bcs ca2dc
ca29c:
    lda l006f
    bne ca30d
    lda l0033
    sta l0070
    ldy l0012
    lda l0011
    cpy top+1
    bcc ca2b2
    bne ca30d
    cmp top
    bcs ca30d
ca2b2:
    jsr sub_cab37
    ldy tmp1
    cpy ptr4+1
    bne ca30d
    lda tmp0
    cmp ptr4
    bne ca30d
    sty l0012
    sta l0011
    ldx screen_height
loop_ca2c7:
    dex
    lda line_lengths,x
    inx
    sta line_lengths,x
    dex
    bne loop_ca2c7
    ldy #SCREEN_SCROLLDOWN
    jsr SCREEN
    jsr home_cursor
    ldy #1
    jmp ca351

ca2dc:
    lda l0033
    sta l0070
ca2e0:
    ldx #0
    lda l0011
    ldy l0012
ca2e6:
    inx
    cpy ptr6+1
    bne ca2f1
    cmp ptr6
    bne ca2f1
    stx l003d
ca2f1:
    cpy ptr4+1
    bne ca2f9
    cmp ptr4
    beq ca313
ca2f9:
    jsr sub_cab1a
    beq ca313
    tya
    ldy tmp1
    clc
    adc tmp0
    bcc ca307
    iny
ca307:
    cpx screen_height
    beq ca2e6
    bcc ca2e6
ca30d:
    jsr sub_ca44e
    jmp ca2e0

ca313:
    cpx screen_height
    bcc ca35e
    beq ca35e
    lda l006f
    bne ca30d
    ldx #0
loop_ca31f:
    lda line_lengths+1,x
    sta line_lengths,x
    inx
    cpx screen_height
    bne loop_ca31f
    dec l003d
    ldx #0
    lda screen_width
    sta line_lengths,x
    lda l0033
    sta l0070
    ldy l0012
    lda l0011
    jsr sub_cab1a
    tya
    clc
    adc l0011
    sta l0011
    bcc ca348
    inc l0012
ca348:
    ldy #SCREEN_SCROLLUP
    jsr SCREEN
    ldx #0
    ldy screen_height
    jsr set_cursor_position
ca351:
    lda l0070
    sta l0033
    inc input_buffer_ptr+1
    inc l0074
    tya
    tax
ca35e:
    stx ypos
ca360:
    ldy l0034
    jsr cab91
    jsr unpack_line_into_buffer
    jsr sub_ca608
    lda screen_width
    lsr
    sta l0083
    lda l0072
    cmp hscroll_pos
    bcc ca381
    lda hscroll_pos
    clc
    adc screen_width
    sbc #3
    cmp l0072
    bcs ca395
ca381:
    lda l0072
    sec
    sbc l0083
    bcs ca38a
    lda #0
ca38a:
    sta hscroll_pos
    lda #1
    sta l0073
    sta input_buffer_ptr+1
    jsr ca93c
ca395:
    lda input_buffer_ptr+1
    sta l0076
    lda l0073
    beq ca3e7
    bpl ca3b2
    lda l003d
    bmi ca3b2
    sta l0082
    lda screen_height
    sec
    sbc l003d
    tax
    inx
    lda ptr6
    ldy ptr6+1
    bne ca3c1
ca3b2:
    ldy l0033
    jsr cab91
    lda #1
    sta l0082
    lda l0011
    ldy l0012
    ldx screen_height
ca3c1:
    stx l0081
loop_ca3c3:
    jsr sub_ca486
    lda tmp0
    ldy tmp1
    jsr sub_cab1a
    beq ca422
    tya
    ldy tmp1
    clc
    adc tmp0
    bcc ca3d8
    iny
ca3d8:
    inc l0082
    dec l0081
    bne loop_ca3c3
ca3de:
    lda #0
    sta l0074
    ldy l0034
    jsr cab91
ca3e7:
    jsr unpack_line_into_buffer
    jsr sub_caacb
    jsr display_status_word
    lda l0074
    beq ca3ff
    lda ypos
    sta l0082
    lda current_format_line_ptr
    ldy current_format_line_ptr+1
    jsr sub_ca486
ca3ff:
    lda flags_need_redrawing_flag
    beq ca406
    jsr sub_ca651
ca406:
    lda l0072
    sec
    sbc hscroll_pos
    clc
    adc #3
    tax
    ldy #0
    sty l0073
    sty l0074
    sty l006f
    dey                                                               ; Y=0xff
    sty ptr6+1
    ldy ypos
    jsr set_cursor_position
    jmp cursor_on

ca422:
    dec l0081
    beq ca3de
    ldx l0082
    lda screen_width
    sta line_lengths+1,x
    sta l0083
    lda #0x2a ; '*'
loop_ca431:
    inc l0082
    ldx #0
    ldy l0082
    jsr set_cursor_position
    jsr sub_ca597
    lda l0083
    sta line_lengths,x
    lda #0
    sta l0083
    lda #0x20 ; ' '
    dec l0081
    bne loop_ca431
    beq ca3de                                                         ; ALWAYS branch

sub_ca44e:
    lda l0034
    sta l0070
    lda screen_height
    sta l0073
    lsr
    tax
    inx
    lda l006f
    bmi ca461
    beq ca461
    ldx ypos
ca461:
    lda ptr4
    ldy ptr4+1
loop_ca465:
    dex
    beq ca479
    sta tmp2
    sty tmp3
    jsr sub_cab37
    lda tmp0
    ldy tmp1
    bcs loop_ca465
    lda tmp2
    ldy tmp3
ca479:
    sta l0011
    sty l0012
    lda l0070
    sta l0033
    lda l0034
    sta l0070
    rts

sub_ca486:
    sta tmp0
    sta tmp6
    sty tmp1
    sty tmp7
    ldx #0
    ldy l0082
    jsr set_cursor_position
    ldy #0
    sty l0083
    sty input_buffer_ptr+1
    sty l0039
    jsr deref_and_check_for_command_prefix
    bne ca4b4
    ldy #3
    lda hscroll_pos
    bne ca4b4
    ldy #1
    jsr sub_ca4d7
    jsr sub_ca4d7
    lda #0x20 ; ' '
    bne ca4bc                                                         ; ALWAYS branch

ca4b4:
    lda #0x20 ; ' '
    jsr ca4e9
    jsr ca4e9
ca4bc:
    jsr ca4e9
loop_ca4bf:
    jsr draw_char
loop_ca4c2:
    jsr render_xchar
    dex
    bne loop_ca4c2
    cmp #0x0d
    bne loop_ca4bf
    lda #0x20 ; ' '
    jsr sub_ca597
    lda l0083
    sta line_lengths,x
    rts

sub_ca4d7:
    jsr draw_char
    jmp ca4e9

render_xchar:
    inc l0039
    stx l0084
    ldx input_buffer_ptr+1
    inc input_buffer_ptr+1
    cpx hscroll_pos
    bcc ca533
ca4e9:
    pha
    ldx l0082
    lda line_lengths,x
    beq ca4f4
    dec line_lengths,x
ca4f4:
    ldx l0083
    cpx screen_width
    bcs ca532
    inc l0083
    tya
    beq ca514
    dey
    jsr sub_ca536
    iny
    cpx #4
    bcs ca514
    tax
    bmi ca523
    bne ca514
    pla
ca50e:
    pha
    jsr set_inverted_text_if_not_mode_7
    ldx #0
ca514:
    pla
    jsr check_for_control_code
    bne ca522
    lda #0x2d ; '-'
    bcs ca50e
    lda #0x2a ; '*'
    bcc ca50e                                                         ; ALWAYS branch

ca522:
    pha
ca523:
    cmp #0x0d
    bne ca529
    lda #0x20 ; ' '
ca529:
    jsr screen_putchar
    txa
    bne ca532
    jsr set_normal_text_if_not_mode_7
ca532:
    pla
ca533:
    ldx l0084
    rts

sub_ca536:
    tya
    clc
    adc tmp6
    sta tmp8
    lda tmp7
    adc #0
    sta tmp9
    ldx #0
loop_ca544:
    lda tmp9
    cmp markers_array+1,x
    bne ca550
    lda tmp8
    cmp markers_array,x
    beq ca558
ca550:
    inx
    inx
    cpx #0x0c
    bne loop_ca544
    txa
    rts

ca558:
    lda #0
return_61:
    rts

; ***************************************************************************************
set_normal_text_if_not_mode_7:
    pha
    txa
    pha
    tya
    pha
    ldy #SCREEN_SETSTYLE
    lda #0
    jsr SCREEN
    jmp 1f

; ***************************************************************************************
set_inverted_text_if_not_mode_7:
    pha
    txa
    pha
    tya
    pha
    ldy #SCREEN_SETSTYLE
    lda #STYLE_REVERSE
    jsr SCREEN
1:
    pla
    tay
    pla
    tax
    pla
    rts

sub_ca597:
    ldx l0082
    sta l0084
    lda line_lengths,x
    beq return_62
    lda l0084
loop_ca5a2:
    jsr screen_putchar
    dec line_lengths,x
    bne loop_ca5a2
return_62:
    rts

draw_char:
    lda (tmp0),y
    iny
sub_ca5ae:
    cmp #9
    beq ca5e1
    cmp #0x10
    beq ca5d5
    cmp #0x0b
    beq ca5d9
    cmp #0x1a
    beq ca5d5
    bcc ca5d1
    cmp #0x20 ; ' '
    bcs ca5d1
    sty l0084
    ldy print_flags
    bpl ca5cf
    sbc #0x1b
    tax
    lda highlight1_code,x
ca5cf:
    ldy l0084
ca5d1:
    ldx #1
    clc
    rts

ca5d5:
    lda #0x20 ; ' '
    bne ca5d1                                                         ; ALWAYS branch

ca5d9:
    lda ruler_left_stop
    beq ca5d5
    sty l0084
    bne ca5f1                                                         ; ALWAYS branch

ca5e1:
    sty l0084
    ldy l0039
loop_ca5e5:
    iny
    cpy l003a
    bcs ca5f8
    lda (current_ruler_ptr),y
    cmp #0x2a ; '*'
    bne loop_ca5e5
    tya
ca5f1:
    sbc l0039
    tax
    beq ca5f8
    bcs ca5fa
ca5f8:
    ldx #1
ca5fa:
    lda #0x20 ; ' '
    ldy l0084
    sec
    rts

check_for_control_code:
    cmp #0x1c
    beq return_63
    cmp #0x1d
    clc
return_63:
    rts

sub_ca608:
    lda current_edit_line_ptr
    sta tmp0
    lda current_edit_line_ptr+1
    sta tmp1
    lda l0079
    bne ca624
    tay
loop_ca615:
    cpy xpos
    beq ca63d
    sta l0039
    jsr draw_char
    txa
    clc
    adc l0039
    bcc loop_ca615
ca624:
    lda #0
    sta l0079
    tay                                                               ; Y=0x00
loop_ca629:
    sta l0039
    jsr draw_char
    txa
    clc
    adc l0039
    cmp l0072
    bcc loop_ca629
    beq ca63b
    lda l0039
    dey
ca63b:
    sty xpos
ca63d:
    sta l0072
return_64:
    rts

; ***************************************************************************************
display_status_word:
    lda l0076
    beq return_64
    ldy #0
    sty l0076
    sty l0082
    lda current_ruler_ptr
    ldy current_ruler_ptr+1
    jsr sub_ca486
sub_ca651:
    lda #0
    sta flags_need_redrawing_flag
    jsr home_cursor
    ldx #0x46 ; 'F'
    lda format_mode_flag
    beq ca666
    ldx #0x4d ; 'M'
    and #0xc0
    bne ca666
    ldx #0x20 ; ' '
ca666:
    txa
    jsr screen_putchar
    lda #0x4a ; 'J'
    ldx justifying_flag
    beq ca672
    lda #0x20 ; ' '
ca672:
    jsr screen_putchar
    lda #0x49 ; 'I'
    ldx insert_mode_flag
    bne ca681
    lda #0x20 ; ' '
    bne ca681                                                         ; ALWAYS branch

; ***************************************************************************************
home_cursor:
    ldx #0
    ldy #0
    jmp set_cursor_position
ca681:
    jmp screen_putchar

ca684:
    ldx ypos
    lda screen_width
    sta line_lengths,x
    rts

; ***************************************************************************************
; On Entry:
;     TMP9/TMP8: 16-bit number
; ***************************************************************************************
render_number_to_output_buffer:
    stx l0082
    lda la69a
    ldy la69b
    jsr render_number_to_callback
    ldx l0082
    rts

la69a:
la69b = la69a+1
    .word emit_to_output_buffer_callback

emit_to_output_buffer_callback:
    pha
    txa
    pha
    tsx
    lda 0x0102,x
    ldx l0082
    sta output_buffer,x
    cpx #0x82
    bcs ca6ae
    inc l0082
ca6ae:
    pla
    tax
    pla
    rts

; ***************************************************************************************
; On Entry:
;     YX: 16-bit number
; ***************************************************************************************
render_number_to_screen:
    stx tmp8
    sty tmp9
    lda #<(bdos_print_char)
    ldy #>(bdos_print_char)
; ***************************************************************************************
; On Entry:
;     TMP8/TMP9: 16-bit number
;     YA: callback
; ***************************************************************************************
render_number_to_callback:
    sta tmp6
    sty tmp7
    ldy #0
    sty l0083
ca6c2:
    ldx #0
loop_ca6c4:
    lda tmp9
    cmp decimal_table+1,y
    bcc ca6e5
    bne ca6d4
    lda tmp8
    cmp decimal_table,y
    bcc ca6e5
ca6d4:
    lda tmp8
    sbc decimal_table,y
    sta tmp8
    lda tmp9
    sbc decimal_table+1,y
    sta tmp9
    inx
    bne loop_ca6c4
ca6e5:
    txa
    bne ca6ec
    ldx l0083
    beq ca6f1
ca6ec:
    jsr sub_ca6f9
    inc l0083
ca6f1:
    iny
    iny
    cpy #8
    bcc ca6c2
    lda tmp8
sub_ca6f9:
    ora #0x30 ; '0'
    jmp (tmp6)

ca6fe:
    lda #0
    tax                                                               ; X=0x00
    sta tmp8
    sta tmp9
ca705:
    lda (current_format_line_ptr),y
    sec
    sbc #0x30 ; '0'
    bcc ca739
    cmp #0x0a
    bcs ca739
    iny
    sta l0084
    asl tmp8
    rol tmp9
    ldx tmp9
    lda tmp8
    pha
    asl tmp8
    rol tmp9
    asl tmp8
    rol tmp9
    pla
    clc
    adc tmp8
    bcc ca72b
    inx
ca72b:
    clc
    adc l0084
    sta tmp8
    txa
    adc tmp9
    sta tmp9
    ldx #0xff
    bne ca705                                                         ; ALWAYS branch

ca739:
    txa
    php
    lda tmp8
    ldx tmp9
    plp
    rts

ca741:
    ldx ptr4
    ldy ptr4+1
    cpy ptr6+1
    bcc ca74f
    bne ca753
    cpx ptr6
    bcs ca753
ca74f:
    stx ptr6
    sty ptr6+1
ca753:
    ldx #0xff
    stx l0073
    stx l003d
    rts

; ***************************************************************************************
flush_and_read_char:
    lda #osbyte_flush_buffer
    ldx #buffer_keyboard
    jsr osbyte                                                        ; Flush the keyboard buffer (X=0)
; ***************************************************************************************
read_char:
    lda #0xff
    tax
    ldy #SCREEN_GETCHAR
    jsr SCREEN
    bcs read_char
    cmp #0x1b                                                         ; A=character read
    clc
    bne return_65
    sec
return_65:
    rts

; ***************************************************************************************
clear_screen:
    ldy #SCREEN_CLEAR
    jmp SCREEN

; ***************************************************************************************
draw_prompt_characters:
    stx tmp2
    sty tmp3
    jsr save_cursor_position
    jsr cursor_off
    jsr home_cursor
    jsr set_inverted_text_if_not_mode_7
    lda tmp2
    jsr screen_putchar
    lda tmp3
    jsr screen_putchar
    jsr set_normal_text_if_not_mode_7
    lda #0x20 ; ' '
    jsr screen_putchar
    jsr restore_cursor_position
cursor_on:
cursor_off:
    rts

; ***************************************************************************************
save_cursor_position:
    ldy #SCREEN_GETCURSOR
    jsr SCREEN
    sta tmp4
    stx tmp5
    rts

; ***************************************************************************************
restore_cursor_position:
    ldx tmp4
    ldy tmp5
; ***************************************************************************************
; On Entry:
;     X: X position
;     Y: Y position
; ***************************************************************************************
set_cursor_position:
    pha
    txa
    pha
    tya
    pha

    txa
    pha
    tya
    tax
    pla
    ldy #SCREEN_SETCURSOR
    jsr SCREEN

    pla
    tay
    pla
    tax
    pla
return_34:
    rts

; ***************************************************************************************
print_inline_string:
    sty l0084
    pla
    clc
    adc #1
    sta tmp2
    pla
    adc #0
    sta tmp3
    ldy #0
    beq ca80f                                                         ; ALWAYS branch

loop_ca80b:
    jsr bdos_print_char                                                        ; Write character
    iny
ca80f:
    lda (tmp2),y
    bmi ca824
    bne loop_ca80b
    tya
    sec
    adc tmp2
    sta tmp2
    bcc ca81f
    inc tmp3
ca81f:
    ldy l0084
    jmp (tmp2)

ca824:
    jsr bdos_print_newline
    jmp cli_loop

; ***************************************************************************************
print_x_words_of_help:
    ldy #0
    beq ca832                                                         ; ALWAYS branch

ca82e:
    jsr bdos_print_char
    iny
ca832:
    lda la83d,y
    bne ca82e
    lda #0x20 ; ' '
    dex
    bpl ca82e
    rts

la83d:
    .ascii "VIEW"
    .byte 0
    .ascii "B3.0 for CP/M-65"
    .byte 0

; ***************************************************************************************
parse_command:
    lda #0xff
    sta l0082
    tax                                                               ; X=0xff
ca84c:
    ldy input_buffer_ptr
    dey
    inc l0082
loop_ca851:
    inx
    iny
    lda (tmp0),y
    and #0xdf
    sta l0084
    lda parser_table,x
    beq ca890
    bmi ca87e
    eor #0x5b ; '['
    sta l0083
    and #0xdf
    cmp l0084
    beq loop_ca851
loop_ca86a:
    inx
    lda parser_table,x
    beq ca890
    bpl loop_ca86a
    lda l0083
    and #0x20 ; ' '
    beq ca84c
    lda (tmp0),y
    cmp #0x30 ; '0'
    bcs ca84c
ca87e:
    lda (tmp0),y
    cmp #0x30 ; '0'
    bcs ca887
    sta l007e
    iny
ca887:
    sty input_buffer_ptr
    ldy l0082
    lda parser_table,x
    clc
    rts

ca890:
    sec
    rts

; ***************************************************************************************
call_through_jumptable:
    asl
    clc
    adc jumptable_ptrs,y
    sta tmp8
    lda #0
    adc jumptable_ptrs+1,y
    sta tmp9
    ldy #0
    lda (tmp8),y
    sta tmp6
    iny                                                               ; Y=0x01
    lda (tmp8),y
    sta tmp7
    jmp (tmp6)

; On entry: YX is the address of the table, A is the value
; On exit, tmp6 is the routine; C if error, !C if success
zproc look_up_address_in_table
    stx tmp8
    sty tmp9
    sta tmp6
    ldy #0
    zloop
        lda (tmp8), y
        zbreakif eq
        cmp tmp6
        zif eq
            iny
            lda (tmp8), y
            sta tmp6
            iny
            lda (tmp8), y
            sta tmp7
            clc
            rts
        zendif
        iny
        iny
        iny
    zendloop
    sec
    rts
zendproc

sub_ca8b9:
    lda l006e
    beq ca93a
    lda ptr4
    sta tmp4
    lda ptr4+1
    sta tmp5
    ldy #0
    sty tmp7
    jsr get_line_length
    sta l0083
    lda l003b
    sec
    sbc l0083
    bcc ca8df
    beq ca8ed
    sta tmp6
    jsr adjust_pointers
    jmp ca8ed

ca8df:
    sta l0084
    lda #0
    sec
    sbc l0084
    sta tmp6
    jsr make_space_for_insertion
    bcs return_66
ca8ed:
    lda l006e
    bpl ca8f8
    lda l006d
    beq ca8f8
    jsr ca741
ca8f8:
    ldy #0
    sty l006d
    sty l006e
    lda current_format_line_ptr
    sta tmp6
    lda current_format_line_ptr+1
    sta tmp7
    ldx l0083
    stx l003b
ca90a:
    txa
    bne ca911
    lda #0x0d
    bne ca919                                                         ; ALWAYS branch

ca911:
    lda (current_format_line_ptr),y
    cmp #0x10
    bne ca919
    lda #0x20 ; ' '
ca919:
    pha
    txa
    pha
loop_ca91c:
    jsr sub_ca536
    bne ca92f
    tya
    clc
    adc ptr4
    sta markers_array,x
    lda ptr4+1
    adc #0
    sta markers_array+1,x
    bne loop_ca91c
ca92f:
    pla
    tax
    pla
    sta (ptr4),y
    iny
    dex
    cmp #0x0d
    bne ca90a
ca93a:
    clc
return_66:
    rts

ca93c:
    jsr sub_ca8b9
    bcc return_66
ca941:
    ldx #0xff
    txs
    jsr sub_ca94a
    jmp editor_loop

sub_ca94a:
    jsr cursor_off
    ldx #3
    ldy #0
    jsr set_cursor_position
    jsr set_inverted_text_if_not_mode_7
    ldy screen_width
    sty line_lengths
    dey
    dey
    ldx #0
    beq ca965                                                         ; ALWAYS branch

loop_ca962:
    jsr oswrch                                                        ; Write character
ca965:
    lda la995,x
    beq ca96e
    inx
    dey
    bne loop_ca962
ca96e:
    jsr set_normal_text_if_not_mode_7
    tya
    beq ca97c
    lda #0x20 ; ' '
loop_ca976:
    jsr oswrch                                                        ; Write character 32
    dey
    bne loop_ca976
ca97c:
    lda #0
    sta l006e
    jsr clear_cmd
loop_ca983:
    jsr beep
    jsr flush_and_read_char
    bcc loop_ca983
    jsr cursor_on
    lda #1
    sta l0076
    sta l0073
    rts

la995:
    .ascii "Memory full - Press ESCAPE"
    .byte 0

adjust_pointers:
    lda tmp4
    sta tmp2
    clc
    adc tmp6
    sta tmp8
    lda tmp5
    sta tmp3
    adc tmp7
    sta tmp9
    ldx #0
ca9c3:
    ldy __begin_pointer_array+1,x
    lda __begin_pointer_array+0,x
    cpy tmp5
    bcc ca9f1
    bne ca9d1
    cmp tmp4
    bcc ca9f1
ca9d1:
    cpy tmp9
    bcc ca9db
    bne ca9e7
    cmp tmp8
    bcs ca9e7
ca9db:
    cpx #12
    bcs ca9e7
    lda #0
    sta __begin_pointer_array+0,x
    sta __begin_pointer_array+1,x
    beq ca9f1                                                         ; ALWAYS branch

ca9e7:
    sbc tmp6
    sta __begin_pointer_array+0,x
    lda __begin_pointer_array+1,x
    sbc tmp7
    sta __begin_pointer_array+1,x
ca9f1:
    inx
    inx
    cpx #22
    bne ca9c3
loop_ca9f7:
    ldy #0
loop_ca9f9:
    lda (tmp8),y
    sta (tmp2),y
    beq caa08
    iny
    bne loop_ca9f9
    inc tmp3
    inc tmp9
    bne loop_ca9f7
caa08:
    tya
    clc
    adc tmp2
    sta top
    lda tmp3
    adc #0
    sta top+1
    rts

make_space_for_insertion:
    lda top
    sta tmp2
    clc
    adc tmp6
    sta tmp8
    tax
    lda top+1
    sta tmp3
    adc tmp7
    sta tmp9
    tay
    cpy himem+1
    bcc caa32
    bne return_67
    cpx himem
    bcs return_67
caa32:
    stx top
    sty top+1
    ldx #0
loop_caa38:
    ldy __begin_pointer_array+1,x
    lda __begin_pointer_array+0,x
    cpy tmp5
    bcc caa51
    bne caa46
    cmp tmp4
    bcc caa51
caa46:
    clc
    adc tmp6
    sta __begin_pointer_array+0,x
    lda __begin_pointer_array+1,x
    adc tmp7
    sta __begin_pointer_array+1,x
caa51:
    inx
    inx
    cpx #22
    bne loop_caa38
caa57:
    lda tmp2
    sec
    sbc tmp4
    tax
    lda tmp3
    sbc tmp5
    beq caa65
    ldx #0xff
caa65:
    txa
    tay
    iny
    lda tmp2
    stx tmp2
    sec
    sbc tmp2
    sta tmp2
    bcs caa75
    dec tmp3
caa75:
    lda tmp8
    stx tmp8
    sec
    sbc tmp8
    sta tmp8
    bcs caa82
    dec tmp9
caa82:
    dey
    lda (tmp2),y
    sta (tmp8),y
    tya
    bne caa82
    inx
    beq caa57
    clc
return_67:
    rts

unpack_line_into_buffer:
    lda l006e
    bne return_68
    lda #1
    sta l006e
sub_caa97:
    lda #0x10
    jsr wipe_buffer
    jsr sub_caf5f
    ldy #0
    lda (ptr4),y
    ldx current_edit_line_ptr
    ldy current_edit_line_ptr+1
    jsr check_for_command_prefix
    bne caab7
    bcs caab0
    sta l006e
caab0:
    jsr caf5c
    ldx ptr1
    ldy ptr1+1
caab7:
    stx current_format_line_ptr
    sty current_format_line_ptr+1
    ldy #0
loop_caabd:
    lda (ptr4),y
    cmp #0x0d
    beq caac8
    sta (current_format_line_ptr),y
    iny
    bne loop_caabd
caac8:
    sty l003b
return_68:
    rts

sub_caacb:
    lda ptr4
    sta tmp6
    lda ptr4+1
    sta tmp7
    ldy #0
caad5:
    jsr sub_ca536
    bne caae8
    tya
    clc
    adc current_format_line_ptr
    sta markers_array,x
    lda current_format_line_ptr+1
    adc #0
    sta markers_array+1,x
    bne caad5
caae8:
    lda (ptr4),y
    cmp #0x0d
    beq return_68
    iny
    bne caad5
; ***************************************************************************************
get_line_length:
    ldy #0
    lda (current_format_line_ptr),y
    jsr check_for_command_prefix
    php
    ldy #0x84
loop_caafb:
    dey
    lda (current_edit_line_ptr),y
    cmp #0x10
    bne cab06
    tya
    bne loop_caafb
    dey
cab06:
    iny
    tya
    plp
    bne return_69
    clc
    adc #3
return_69:
    rts

wipe_buffer:
    ldy #0
    ldx #0x89
loop_cab13:
    sta (ptr1),y
    iny
    dex
    bne loop_cab13
    rts

sub_cab1a:
    sta tmp0
    sty tmp1
    jsr sub_cab6e
    bne cab29
    jsr cab29
    bne cab75
    rts

cab29:
    ldy #0
loop_cab2b:
    lda (tmp0),y
    beq return_70
    iny
    cmp #0x0d
    bne loop_cab2b
    lda (tmp0),y
return_70:
    rts

sub_cab37:
    sec
    sbc #1
    sta tmp0
    bcs cab3f
    dey
cab3f:
    sty tmp1
    cpy page+1
    bcc return_71
    bne cab4b
    cmp page
    bcc return_71
cab4b:
    ldy #0
loop_cab4d:
    lda tmp0
    sec
    sbc #1
    sta tmp0
    bcs cab58
    dec tmp1
cab58:
    lda (tmp0),y
    cmp #0x0d
    bne loop_cab4d
    inc tmp0
    bne cab64
    inc tmp1
cab64:
    jsr sub_cab6e
    bne cab6c
    jsr sub_cab8b
cab6c:
    sec
return_71:
    rts

sub_cab6e:
    ldy #0
    lda (tmp0),y
    cmp #0x81
    rts

cab75:
    tya
    pha
    inc l0076
    ldy l0070
    dey
    lda tmp0
    sta (oshwm),y
    dey
    lda tmp1
    sta (oshwm),y
    jsr cab91
    pla
    tay
    rts

sub_cab8b:
    inc l0076
    ldy l0070
    iny
    iny
cab91:
    sty l0070
    iny
    lda (oshwm),y
    clc
    adc #3
    sta current_ruler_ptr
    dey
    lda (oshwm),y
    adc #0
    sta current_ruler_ptr+1
; ***************************************************************************************
find_margins_of_current_ruler_buffer:
    jsr sub_cabc4
loop_caba5:
    lda (current_ruler_ptr),y
    cmp #0x3e ; '>'
    bne cabad
    sty ruler_left_stop
cabad:
    cmp #0x3c ; '<'
    bne cabb3
    sty ruler_right_stop
cabb3:
    cmp #0x0d
    beq cabbc
    iny
    cpy #0x84
    bne loop_caba5
cabbc:
    sty l003a
    lda ruler_left_stop
    cmp ruler_right_stop
    bcc return_72
sub_cabc4:
    ldy #0
    sty ruler_right_stop
    sty ruler_left_stop
return_72:
    rts

move_cursor_to_address:
    sta tmp8
    sty tmp9
    lda ptr4
    ldy ptr4+1
    cpy tmp9
    bcc cabf9
    bne cabdf
    cmp tmp8
    bcc cabf9
    beq cac20
cabdf:
    jsr sub_cab37
    lda tmp0
    ldy tmp1
    bcc cac20
    cpy tmp9
    bcc cac20
    bne cabdf
    cmp tmp8
    bcc cac20
    bne cabdf
    beq cac20                                                         ; ALWAYS branch

cabf6:
    jsr sub_cac41
cabf9:
    sta tmp0
    sty tmp1
    jsr cab29
    beq cac17
    tya
    ldy tmp1
    clc
    adc tmp0
    bcc cac0b
    iny
cac0b:
    cpy tmp9
    bcc cabf6
    bne cac17
    cmp tmp8
    bcc cabf6
    beq cac1d
cac17:
    lda tmp0
    ldy tmp1
    bne cac20
cac1d:
    jsr sub_cac41
cac20:
    sta ptr4
    sty ptr4+1
    lda tmp8
    sec
    sbc ptr4
    tax
    ldy #0
    lda (ptr4),y
    jsr check_for_command_prefix
    bne cac3e
    txa
    cpx #3
    ldx #0
    bcc cac3e
    sec
    sbc #3
    tax
cac3e:
    stx xpos
    rts

sub_cac41:
    pha
    tya
    pha
    jsr sub_cab6e
    bne cac4c
    jsr cab75
cac4c:
    pla
    tay
    pla
    rts

sub_cac50:
    sec
    sbc #1
    sta tmp8
    bcs cac58
    dey
cac58:
    sty tmp9
    ldy #0
cac5c:
    lda (tmp8),y
    cmp #0x0d
    beq cac6f
    lda tmp8
    sec
    sbc #1
    sta tmp8
    bcs cac5c
    dec tmp9
    bne cac5c
cac6f:
    lda tmp8
    sta tmp6
    lda tmp9
    sta tmp7
return_73:
    rts

cac78:
    jsr sub_cac50
cac7b:
    lda #0
    sta l0083
    ldx #0x85
    ldy #1
    lda (tmp8),y
    jsr check_for_command_prefix
    bne cac8d
    inx
    inx
    inx
cac8d:
    stx l0084
cac8f:
    lda (tmp8),y
    iny
    cmp #0x20 ; ' '
    beq cac9a
    cmp #0x1a
    bne cac9c
cac9a:
    sty l0083
cac9c:
    cmp #0x0d
    beq return_73
    cpy l0084
    beq cac8f
    bcc cac8f
    lda l0084
    ldx l0083
    beq cacad
    txa
cacad:
    clc
    adc tmp8
    sta tmp4
    sta tmp8
    lda tmp9
    adc #0
    sta tmp5
    sta tmp9
    lda #1
    sta tmp6
    lda #0
    sta tmp7
    jsr make_space_for_insertion
    lda #0x0d
    ldy #0
    sta (tmp4),y
    lda tmp4
    sta tmp8
    lda tmp5
    sta tmp9
    bne cac7b
prompt_for_marker:
    ldx #0x4d ; 'M'
    ldy #0x4b ; 'K'
    jsr draw_prompt_characters
    inc flags_need_redrawing_flag
    jsr read_char
    jsr lookup_marker
    bcc return_74
; ***************************************************************************************
beep:
loop_caced:
    sec
return_74:
    rts

; ***************************************************************************************
lookup_marker:
    sec
    sbc #0x31 ; '1'
    bcc loop_caced
    asl
    cmp #0x0c
    bcs return_75
    tax
    lda markers_array+1,x
return_75:
    rts

; ***************************************************************************************
reset_area_to_entire_document:
    lda top
    sta area_start_ptr
    lda top+1
    sta area_start_ptr+1
    lda page
    sta area_end_ptr
    lda page+1
    sta area_end_ptr+1
    rts

; ***************************************************************************************
clear_marks_1_2:
    lda #0
    ldx #3
loop_cad12:
    sta markers_array,x
    dex
    bpl loop_cad12
    rts

; ***************************************************************************************
reset_area_to_marks_1_2:
    lda #0x31 ; '1'
    jsr lookup_marker
    bcs return_76
    beq cad45
    lda markers_array,x
    sta area_start_ptr
    lda markers_array+1,x
    sta area_start_ptr+1
    lda #0x32 ; '2'
    jsr lookup_marker
    bcs return_76
    beq cad45
    lda markers_array,x
    sta area_end_ptr
    lda markers_array+1,x
    sta area_end_ptr+1
    ldx #(doc_ptr1-__begin_pointer_array)/2
    jsr set_marker_to_here
    jsr sanitise_area
    clc
    bne return_76
cad45:
    sec
return_76:
    rts

set_marker_to_here:
    jsr get_line_length
    cpy xpos
    bcc cad5d
    ldy #0
    lda (current_format_line_ptr),y
    ldy xpos
    jsr check_for_command_prefix
    bne cad5c
    iny
    iny
    iny
cad5c:
    tya
cad5d:
    clc
    adc ptr4
    sta markers_array+0,x
    lda ptr4+1
    adc #0
    sta markers_array+1,x
    rts

; ***************************************************************************************
; On Entry:
;     A: register name
; ***************************************************************************************
get_register_address:
    jsr is_uppercase
    bcs return_77
    and #0xdf
    pha
    sbc #0x40 ; '@'
    asl
    adc #<register_value_array
    sta tmp6
    lda #>register_value_array
    adc #0
    sta tmp7
    pla
    clc
return_77:
    rts

; ***************************************************************************************
; On Entry:
;     TMP7/TMP6: pointer to register
;     A: register name
; ***************************************************************************************
render_register:
    sty l0084
    jsr get_register_address
    ldy #0
    sty tmp8
    sty tmp9
    zif cc
        bit lada6
        lda (tmp6),y
        sta tmp8
        iny                                                               ; Y=0x01
        lda (tmp6),y
        sta tmp9
        jsr render_number_to_output_buffer
    zendif
    clv
    ldy l0084
    rts

lada6:
    .byte 0x40

sub_cadf0:
    ldx #8
    lda tmp9
loop_cadf4:
    asl tmp8
    rol
    cmp l0046
    bcc cadff
    sbc l0046
    inc tmp8
cadff:
    dex
    bne loop_cadf4
    rts

cae03:
    jmp beep

sub_cae06:
    lda xpos
    cmp #0x84
    bcs cae03
    stx input_buffer_ptr+1
    jsr get_line_length
    tya
    clc
    adc input_buffer_ptr+1
    bcs cae03
    cmp #0x85
    bcs cae03
    inc l006d
    lda current_edit_line_ptr
    sta tmp6
    lda current_edit_line_ptr+1
    sta tmp7
    ldy #0x84
cae27:
    dey
    ldx #0
    tya
    clc
    adc input_buffer_ptr+1
    bcs cae35
    cmp #0x84
    bcs cae35
    tax
cae35:
    stx l0081
loop_cae37:
    jsr sub_ca536
    bne cae52
    lda l0081
    beq cae4b
    clc
    adc current_edit_line_ptr
    sta markers_array,x
    lda current_edit_line_ptr+1
    adc #0
    bne cae4d
cae4b:
    sta markers_array,x
cae4d:
    sta markers_array+1,x
    jmp loop_cae37

cae52:
    lda (current_edit_line_ptr),y
    sty l0084
    ldy l0081
    beq cae5c
    sta (current_edit_line_ptr),y
cae5c:
    ldy l0084
    cpy xpos
    bne cae27
    clc
    rts

cae64:
    stx input_buffer_ptr+1
    inc l006d
    lda current_edit_line_ptr
    sta tmp6
    lda current_edit_line_ptr+1
    sta tmp7
    ldy xpos
    tya
    clc
    adc input_buffer_ptr+1
    sta l0084
cae78:
    jsr sub_ca536
    bne cae98
    lda #0
    cpy l0084
    bcc cae91
    tya
    sbc input_buffer_ptr+1
    clc
    adc current_edit_line_ptr
    sta markers_array,x
    lda current_edit_line_ptr+1
    adc #0
    bne cae93
cae91:
    sta markers_array,x
cae93:
    sta markers_array+1,x
    jmp cae78

cae98:
    iny
    cpy #0x85
    bcc cae78
    lda xpos
    cmp #0x84
    bcs return_78
    ldy xpos
loop_caea5:
    sty l0084
    ldx #0x10
    tya
    clc
    adc input_buffer_ptr+1
    bcs caeb7
    tay
    cpy #0x84
    bcs caeb7
    lda (current_edit_line_ptr),y
    tax
caeb7:
    ldy l0084
    txa
    sta (current_edit_line_ptr),y
    iny
    cpy #0x84
    bcc loop_caea5
return_78:
    rts

sub_caec2:
    lda ruler_left_stop
    beq caed4
    ldy #0
loop_caec8:
    lda (current_edit_line_ptr),y
    iny
    cmp #0x0b
    beq caed4
    cpy #0x84
    bcc loop_caec8
    rts

caed4:
    clc
    rts

sub_caed6:
    jsr sub_caec2
    bcc caed4
    ldy #0
sub_caedd:
    lda xpos
    pha
    sty xpos
    ldx #1
    jsr sub_cae06
    bcs caef0
    ldy xpos
    lda #0x0b
    sta (current_edit_line_ptr),y
    iny
caef0:
    pla
    sta xpos
    rts

sub_caef4:
    lda format_mode_flag
    and #0x81
    bne caf31
    jsr sub_caec2
    bcc caf31
    jsr get_line_length
    lda xpos
    sta l0083
    sty xpos
    jsr sub_ca608
    lda l0072
    cmp ruler_left_stop
    bcc caf19
    ldy l0083
    sty xpos
    inc xpos
    bcs caf2a                                                         ; ALWAYS branch

caf19:
    lda l0083
    ldy xpos
    cpy l0083
    bcs caf28
    sec
    sbc ruler_left_stop
    bcc caf2a
    adc xpos
caf28:
    sta xpos
caf2a:
    jsr sub_caedd
    bcs return_79
    inc l0074
caf31:
    clc
return_79:
    rts

draw_previous_word:
    lda current_edit_line_ptr
    sta tmp0
    lda current_edit_line_ptr+1
    sta tmp1
    ldy xpos
    beq caf55
loop_caf3f:
    dey
    beq caf55
    jsr draw_char
    dey
    cmp #0x20 ; ' '
    beq loop_caf3f
loop_caf4a:
    dey
    jsr draw_char
    cmp #0x20 ; ' '
    beq caf55
    dey
    bne loop_caf4a
caf55:
    sty xpos
    jsr draw_char
    dey
    rts

caf5c:
    sec
    bcs caf60                                                         ; ALWAYS branch

sub_caf5f:
    clc
caf60:
    php
    lda format_mode_flag
    rol format_mode_flag
    plp
    ror format_mode_flag
    cmp format_mode_flag
    beq return_80
    inc flags_need_redrawing_flag
return_80:
    rts

deref_and_check_for_command_prefix:
    lda (tmp0),y
check_for_command_prefix:
    cmp #0x80
    beq return_81
    cmp #0x81
    clc
return_81:
    rts

system_init:
    ldy #BIOS_GETTPA
    lda #BANK_MAIN
    jsr BIOS
    stx himem+1
    lda #0
    sta himem+0

    lda #<cpm_ram
    sta oshwm+0
    lda #>cpm_ram
    sta oshwm+1

    ldy #BIOS_FINDDRV
    lda #DRVID_SCREEN
    ldx #0
    jsr BIOS
    bcs noscreen
    sta SCREEN+1
    stx SCREEN+2

    ldy #SCREEN_GETSIZE
    jsr SCREEN
    sta screen_width
    stx screen_height
    rts

noscreen:
    jsr print_inline_string
    .ascii "No SCREEN\n"
    .byte 0
    ldy #BDOS_EXIT_PROGRAM
    jmp BDOS

SCREEN:
    jmp 0x1234

; ***************************************************************************************
detect_tube:
    lda #osbyte_read_high_order_address
    jsr osbyte                                                        ; Read the filing system 'machine high order address'
    sec
    iny
    bne return_83
    inx
    bne return_83
    clc
return_83:
    rts

; ***************************************************************************************
compute_bytes_free:
    lda himem
    sec
    sbc top
    tax
    lda himem+1
    sbc top+1
    tay
return_84:
    rts

; ***************************************************************************************
initialise_document:
    lda #0
    sta printer_driver_name
    sta format_mode_flag
    sta justifying_flag
    sta insert_mode_flag
    ldx #(input_buffer_ptr+2 - print_flags)
loop_cafe9:
    sta print_flags,x
    dex
    bpl loop_cafe9
cafee:
    ldx oshwm                                                         ; X and Y contain the address of OSHWM (low, high)
    ldy oshwm+1
    iny
    inx
    stx page
    bne caffe
    iny
caffe:
    sty page+1
    ldy #0
    sty file_edit_flags
    sty xpos
    lda #0xaa
    sta (oshwm),y
    lda page
    sec
    sbc #1
    sta tmp8
    lda page+1
    sbc #0
    sta tmp9
    lda #0x0d
    sta (tmp8),y
    sta current_line_buffer + 0x89
    lda page
    sta top
    lda page+1
    sta top+1
    lda #<(current_line_buffer)
    sta ptr1
    clc
    adc #3
    sta current_edit_line_ptr
    sta current_format_line_ptr
    lda #>(current_line_buffer)
    sta ptr1+1
    adc #0
    sta current_edit_line_ptr+1
    sta current_format_line_ptr+1
    lda #<(current_ruler_buffer)
    ldy #>(current_ruler_buffer)
    jsr create_default_ruler
    iny
    lda #0x0d
    sta (tmp0),y
    ldy #0xff
    lda #<(l05cc)
    sta (oshwm),y
    dey                                                               ; Y=0xfe
    lda #>(l05cc)
    sta (oshwm),y
    jsr cb07a
    jsr clear_cmd
cb05a:
    ldy page+1
    cpy top+1
    bne return_85
    lda page
    cmp top
    bne return_85
    inc top
    bne cb06c
    inc top+1
cb06c:
    sta ptr4
    sty ptr4+1
    ldy #0
    lda #0x0d
    sta (page),y
    tya                                                               ; A=0x00
    sta (top),y
return_85:
    rts

cb07a:
    lda page
    sta ptr4
    lda page+1
    sta ptr4+1
    lda #0
    sta xpos
    ldy #0xfe
    sty l0012
    sty l0070
    sty l0033
    jmp cab91

; ***************************************************************************************
clear_cmd:
    ldx #0x0b
    lda #0
loop_cb095:
    sta markers_array,x
    dex
    bpl loop_cb095
    rts

; ***************************************************************************************
enter_editor_mode:
    jsr clear_screen
    lda #0
    sta l006d
    sta l006f
    sta l006e
    ldx screen_height
loop_cb0a8:
    sta line_lengths,x
    dex
    bpl loop_cb0a8
    ldx #2
    stx l0073
    stx l0076
    rts

; ***************************************************************************************
create_default_ruler:
    sta tmp0
    sty tmp1
    lda #0
    tay                                                               ; Y=0x00
loop_cb0e7:
    lda #0x2e ; '.'
loop_cb0e9:
    sta (tmp0),y
    iny
    tya
    tax
    inx
    clc
    adc #6
    cmp screen_width
    beq cb0ff
    txa
    and #7
    bne loop_cb0e7
    lda #0x2a ; '*'
    bne loop_cb0e9                                                    ; ALWAYS branch

cb0ff:
    lda #0x3c ; '<'
    sta (tmp0),y
    rts

sub_cb104:
    lda #0
    ldx #0x33 ; '3'
loop_cb108:
    sta register_value_array,x
    dex
    bpl loop_cb108
    sta header_text_maybe
    sta footer_text_maybe
    sta two_sided_flag
    sta left_margin
    sta line_spacing
    sta rhs_extra_margin
    sta macro_executing_flag
    lda #0x42 ; 'B'
    sta page_length
    lda #1
    sta footers_enabled_flag
    sta headers_enabled_flag
    sta l0038
    sta register_value_p
    sta register_value_l
    ldy #0x80
    sty highlight1_code
    iny                                                               ; Y=0x81
    sty highlight2_code
    lda #4
    sta top_margin
    sta bottom_margin
    sta header_margin
    sta footer_margin
    jmp c92f0

zproc control_key_to_ascii
    cmp #0x20
    zif lt
        ora #0x40
    zendif
    jmp to_uppercase
zendproc

zproc q_command_key
    ldx #'^'
    ldy #'Q'
    jsr draw_prompt_characters
    inc flags_need_redrawing_flag
    jsr read_char
    jsr control_key_to_ascii
    ldx #<q_key_table
    ldy #>q_key_table
1:
    jsr look_up_address_in_table
    zif cc
        jmp (tmp6)
    zendif
    rts
zendproc

zproc o_command_key
    ldx #'^'
    ldy #'O'
    jsr draw_prompt_characters
    inc flags_need_redrawing_flag
    jsr read_char
    jsr control_key_to_ascii
    ldx #<o_key_table
    ldy #>o_key_table
    jmp 1b
zendproc

zproc k_command_key
    ldx #'^'
    ldy #'K'
    jsr draw_prompt_characters
    inc flags_need_redrawing_flag
    jsr read_char
    jsr control_key_to_ascii
    ldx #<k_key_table
    ldy #>k_key_table
    jmp 1b
zendproc

decimal_table:
    .word 10000, 1000, 100, 10

non_function_key_table:
    .byte '['-'@' ; escape
    .word esc_key
    .byte 'M'-'@' ; return
    .word return_key
    .byte 0x7f ; delete
    .word delete_key
    .byte 'I'-'@' ; tab
    .word tab_key
    .byte 'E'-'@' ; up
    .word f15_up_key
    .byte SCREEN_KEY_UP
    .word f15_up_key
    .byte 'S'-'@' ; left
    .word f12_left_key
    .byte SCREEN_KEY_LEFT
    .word f12_left_key
    .byte 'D'-'@' ; right
    .word f13_right_key
    .byte SCREEN_KEY_RIGHT
    .word f13_right_key
    .byte 'X'-'@' ; down
    .word f14_down_key
    .byte SCREEN_KEY_DOWN
    .word f14_down_key
    .byte 'A'-'@' ; word left
    .word sf12_left_key
    .byte 'F'-'@' ; word right
    .word sf13_right_key
    .byte 'C'-'@' ; page down
    .word sf14_down_key
    .byte 'R'-'@' ; page up
    .word sf15_up_key
    .byte 'G'-'@' ; delete char
    .word f9_delete_char_key
    .byte 'H'-'@' ; insert char
    .word f8_insert_char_key
    .byte 'Y'-'@' ; delete line
    .word f7_delete_line_key
    .byte 'V'-'@' ; insert on/off
    .word cf4_insert_mode_key
    .byte 'N'-'@' ; insert blank line
    .word f6_insert_line_key
    .byte 'B'-'@' ; format block
    .word f0_format_block_key
    .byte 'T'-'@' ; delete to char
    .word sf3_delete_to_char_key
    .byte 'L'-'@' ; next search match
    .word cf1_next_match_key
    .byte 'J'-'@' ; join lines
    .word cf7_join_lines_key
    .byte 'P'-'@' ; swap case
    .word sf1_swap_case_key
    .byte 'O'-'@' ; O-command
    .word o_command_key
    .byte 'Q'-'@' ; Q-command
    .word q_command_key
    .byte 'K'-'@' ; K-command
    .word k_command_key
    .byte 0

q_key_table:
    .byte 'R'     ; beginning of file
    .word f1_top_of_text_key
    .byte 'C'     ; end of file
    .word f2_bottom_of_text_key
    .byte 'S'     ; beginning of line
    .word f4_beginning_of_line_key
    .byte 'D'     ; end of line
    .word f5_end_of_line_key
    .byte 'Y'     ; delete to end of line
    .word f3_delete_to_eol_key
    .byte 'J'     ; split lines
    .word cf6_split_line_key
    .byte 'M'     ; go to marker
    .word sf6_go_to_marker_key
    .byte '1'     ; go to marker
    .word go_to_marker_1
    .byte '2'     ; go to marker
    .word go_to_marker_2
    .byte '3'     ; go to marker
    .word go_to_marker_3
    .byte '4'     ; go to marker
    .word go_to_marker_4
    .byte '5'     ; go to marker
    .word go_to_marker_5
    .byte '6'     ; go to marker
    .word go_to_marker_6
    .byte 0

o_key_table:
    .byte 'J'     ; justification on/off
    .word cf3_justify_mode_key
    .byte 'X'     ; margin release
    .word sf2_release_margins_key
    .byte 'C'     ; edit command
    .word sf8_edit_command_key
    .byte 'D'     ; delete command
    .word sf9_delete_command_key
    .byte 'F'     ; format mode
    .word cf2_format_mode_key
    .byte 'M'     ; mark as ruler
    .word cf8_mark_as_ruler_key
    .byte 'R'     ; copy ruler
    .word sf11_copy_key
    .byte 'S'     ; standard ruler
    .word cf5_default_ruler_key
    .byte 'U'     ; highlight 1
    .word sf4_highlight1_key
    .byte 'B'     ; highlight 2
    .word sf5_highlight2_key
    .byte 0

k_key_table:
    .byte 'M'     ; set marker
    .word sf7_set_marker_key
    .byte 'C'     ; copy block
    .word f11_copy_key
    .byte 'V'     ; move block
    .word sf0_move_block_key
    .byte 'Y'     ; delete block
    .word cf0_delete_block_key
    .byte '1'     ; set marker
    .word set_marker_1
    .byte '2'     ; set marker
    .word set_marker_2
    .byte '3'     ; set marker
    .word set_marker_3
    .byte '4'     ; set marker
    .word set_marker_4
    .byte '5'     ; set marker
    .word set_marker_5
    .byte '6'     ; set marker
    .word set_marker_6
    .byte 0
    
jumptable_ptrs:
lb152 = jumptable_ptrs+1
    .word jumptable2_format, jumptable4_cli
jumptable4_cli:
    .word   quit_cmd,   new_cmd,     format_cmd,   setup_cmd
    .word   read_cmd,  more_cmd,     screen_cmd,  sheets_cmd
    .word   save_cmd, count_cmd,      field_cmd, printer_cmd
    .word search_cmd, clear_cmd, microspace_cmd,    fold_cmd
    .word   name_cmd,  mode_cmd,     finish_cmd,   print_cmd
    .word change_cmd, write_cmd,       edit_cmd, replace_cmd
    .word   load_cmd,   bye_cmd
jumptable4_cli_end:

    .ascii "Mark Colton"
    .byte 0
; 'QUIT' -> 0, flag=1
parser_table:
    .byte 0x0a, 0x0e, 0x12, 0x0f
    .byte 0x81
; 'NEW' -> 1, flag=0
    .byte 0x15, 0x1e, 0x0c
    .byte 0x80
; 'FORMAT' -> 2, flag=1
    .byte 0x1d, 0x14, 9, 0x36, 0x3a, 0x2f
    .byte 0x81
; 'SETUP' -> 3, flag=0
    .byte 8, 0x1e, 0x0f, 0x2e, 0x2b
    .byte 0x80
; 'READ' -> 4, flag=1
    .byte 9, 0x1e, 0x3a, 0x3f
    .byte 0x81
; 'MORE' -> 5, flag=1
    .byte 0x16, 0x14, 0x29, 0x3e
    .byte 0x81
; 'SCREEN' -> 6, flag=1
    .byte 8, 0x18, 0x29, 0x3e, 0x3e, 0x35
    .byte 0x81
; 'SHEETS' -> 7, flag=1
    .byte 8, 0x13, 0x3e, 0x3e, 0x2f, 0x28
    .byte 0x81
; 'SAVE' -> 8, flag=1
    .byte 8, 0x1a, 0x2d, 0x3e
    .byte 0x81
; 'COUNT' -> 9, flag=1
    .byte 0x18, 0x14, 0x2e, 0x35, 0x2f
    .byte 0x81
; 'FIELD' -> 10, flag=1
    .byte 0x1d, 0x12, 0x3e, 0x37, 0x3f
    .byte 0x81
; 'PRINTER' -> 11, flag=1
    .byte 0x0b, 9, 0x12, 0x15, 0x0f, 0x1e, 0x29
    .byte 0x81
; 'SEARCH' -> 12, flag=1
    .byte 8, 0x3e, 0x3a, 0x29, 0x38, 0x33
    .byte 0x81
; 'CLEAR' -> 13, flag=1
    .byte 0x18, 0x17, 0x3e, 0x3a, 0x29
    .byte 0x81
; 'MICROSPACE' -> 14, flag=1
    .byte 0x16, 0x12, 0x38, 0x29, 0x34, 0x28, 0x2b, 0x3a, 0x38, 0x3e
    .byte 0x81
; 'FOLD' -> 15, flag=1
    .byte 0x1d, 0x14, 0x37, 0x3f
    .byte 0x81
; 'NAME' -> 16, flag=1
    .byte 0x15, 0x3a, 0x36, 0x3e
    .byte 0x81
; 'MODE' -> 17, flag=0
    .byte 0x16, 0x34, 0x3f, 0x3e
    .byte 0x80
; 'FINISH' -> 18, flag=1
    .byte 0x1d, 0x32, 0x35, 0x32, 0x28, 0x33
    .byte 0x81
; 'PRINT' -> 19, flag=1
    .byte 0x0b, 0x29, 0x32, 0x35, 0x2f
    .byte 0x81
; 'CHANGE' -> 20, flag=1
    .byte 0x18, 0x33, 0x3a, 0x35, 0x3c, 0x3e
    .byte 0x81
; 'WRITE' -> 21, flag=1
    .byte 0x0c, 0x29, 0x32, 0x2f, 0x3e
    .byte 0x81
; 'EDIT' -> 22, flag=0
    .byte 0x1e, 0x3f, 0x32, 0x2f
    .byte 0x80
; 'REPLACE' -> 23, flag=1
    .byte 9, 0x3e, 0x2b, 0x37, 0x3a, 0x38, 0x3e
    .byte 0x81
; 'LOAD' -> 24, flag=0
    .byte 0x17, 0x34, 0x3a, 0x3f
    .byte 0x80
; 'BYE' -> 25, flag=1
    .byte 0x39, 0x22, 0x3e
    .byte 0x81
    .byte 0
commands_table:
lb2a1 = commands_table+1
    .ascii "CE"
    .ascii "RJ"
    .ascii "DF"
    .ascii "DH"
    .ascii "DM"
    .ascii "EM"
    .ascii "SR"
    .ascii "PE"
    .ascii "TM"
    .ascii "BM"
    .ascii "PL"
    .ascii "TS"
    .ascii "FO"
    .ascii "HE"
    .ascii "HT"
    .ascii "HM"
    .ascii "FM"
    .ascii "LM"
    .ascii "LS"
    .ascii "OP"
    .ascii "EP"
    .ascii "LJ"
    .ascii "PB"
    .byte 0xff
; overlapping: cmp lf894,y
jumptable2_format:
    .word ce_fmt_cmd, rj_fmt_cmd, df_fmt_cmd, dh_fmt_cmd, dm_fmt_cmd
    .word  return_34, em_fmt_cmd, pe_fmt_cmd, tm_fmt_cmd, bm_fmt_cmd
    .word pl_fmt_cmd, ts_fmt_cmd, fo_fmt_cmd, he_fmt_cmd, ht_fmt_cmd
    .word hm_fmt_cmd, fm_fmt_cmd, lm_fmt_cmd, ls_fmt_cmd, op_fmt_cmd
    .word ep_fmt_cmd, lj_fmt_cmd, pb_fmt_cmd

osbyte:
    jsr print_inline_string
    .ascii "osbyte\n"
    .byte 0
    brk

oswrch:
    pha
    jsr print_inline_string
    .ascii "oswrch\n"
    .byte 0
    pla
    brk

osword:
    jsr print_inline_string
    .ascii "osword\n"
    .byte 0
    brk

osfile:
    jsr print_inline_string
    .ascii "osfile\n"
    .byte 0
    brk

bdos_print_char:
    pha
    txa
    pha
    tya
    pha
    tsx
    lda 0x0103,x
    ldy #BDOS_CONSOLE_OUTPUT
    jsr BDOS
    pla
    tay
    pla
    tax
    pla
    cmp #13
    bne 1f
    pha
    lda #10
    jsr bdos_print_char
    pla
1:
    rts

bdos_print_newline:
    pha
    lda #13
    jsr bdos_print_char
    pla
    rts

zproc screen_putchar
    pha
    txa
    pha
    tya
    pha

    tsx
    lda 0x103, x
    ldy #SCREEN_PUTCHAR
    jsr SCREEN

    pla
    tay
    pla
    tax
    pla
    rts
zendproc

; Read a line from the keyboard input input_buffer.
; This is a hacked and customised copy of the BDOS routine.
zproc readline
    lda #0
    sta input_buffer_ptr

    zloop
        ; Read a key without echo.

        ldy #BDOS_DIRECT_IO
        ldx #0xfd
        jsr BDOS
        tax

        ; Delete?

        cmp #8
        zif eq
            lda #127
        zendif
        cmp #127
        zif eq
            ldy input_buffer_ptr
            zif ne
                dec input_buffer_ptr
                jsr bdos_print_char
            zendif
            zcontinue
        zendif

        ; Delete line?

        cmp #21
        zif eq
            ldy input_buffer_ptr
            zif ne
                zrepeat
                    lda #127
                    jsr bdos_print_char
                    dey
                zuntil eq
                sty input_buffer_ptr
            zendif
            zcontinue
        zendif

        ; Finished?

        cmp #13
        zbreakif eq
        cmp #10
        zbreakif eq
        cmp #27
        zif eq
            lda #0
            sta input_buffer_ptr
            sec
            rts
        zendif

        ; Graphic character?

        cmp #32
        zif ge
            ldy #MAX_COMMAND_LENGTH
            cpy input_buffer_ptr
            zif cs
                ldy input_buffer_ptr
                sta input_buffer, y
                jsr bdos_print_char
                inc input_buffer_ptr
            zendif
        zendif
    zendloop

    jsr bdos_print_newline
    ldx input_buffer_ptr
    lda #0x0d
    sta input_buffer, x
    ldx #0
    stx input_buffer_ptr
    clc
    rts
zendproc

zproc prepare_to_open_file
    stx tmp6
    sty tmp7
    jsr zero_terminate_filename_buffer
    
    ; Parse the filename.

    lda tmp6
    ldx tmp7
    jsr convert_filename_to_fcb_xa
    zif cs
        jmp bad_filename_error
    zendif
    rts
zendproc

; YX = pointer to file structure
; Input filename is in filename_buffer
; Uses tmp6/tmp7.
zproc open_input_file
    jsr prepare_to_open_file

    ; Delete any existing file.

    ldy #BDOS_OPEN_FILE
    lda tmp6
    ldx tmp7
    jsr BDOS
    zif cs
        jmp file_not_found_error
    zendif

    ldy #FS_BUFFERPTR
    lda #FS_BUFFER+128
    sta (tmp6), y
    rts
zendproc

; YX = pointer to file structure
; Input filename is in filename_buffer
; Uses tmp6/tmp7.
zproc open_output_file
    jsr prepare_to_open_file

    ; Delete any existing file.

    ldy #BDOS_DELETE_FILE
    lda tmp6
    ldx tmp7
    jsr BDOS

    ; Create a new file.

    jsr reset_fcb
    ldy #BDOS_CREATE_FILE
    lda tmp6
    ldx tmp7
    jsr bdos_and_file_error

    ldy #FS_BUFFERPTR
    lda #FS_BUFFER
    sta (tmp6), y
    rts
zendproc

zproc bdos_and_file_error
    jsr BDOS
    zif cs
        jmp file_not_found_error
    zendif
    rts
zendproc

zproc set_dma_to_buffer_address_of_file
    clc
    lda tmp6
    adc #FS_BUFFER
    pha
    lda tmp7
    adc #0
    tax
    pla
    ldy #BDOS_SET_DMA_ADDRESS
    jmp BDOS
zendproc

; FS pointer in tmp6/tmp7.
zproc flush_file
    jsr set_dma_to_buffer_address_of_file

    ldy #BDOS_WRITE_SEQUENTIAL
    lda tmp6
    ldx tmp7
    jsr bdos_and_file_error

    ldy #FS_BUFFER
    lda #0
    zrepeat
        sta (tmp6), y
        iny
        cpy #FS_BUFFER+128
    zuntil eq
    rts
zendproc

zproc close_file
    stx tmp6
    sty tmp7

    ldy #FS_BUFFERPTR
    lda (tmp6), y
    cmp #FS_BUFFER
    zif ne
        jsr flush_file
    zendif

    ldy #BDOS_CLOSE_FILE
    lda tmp6
    ldx tmp7
    jmp bdos_and_file_error
zendproc

zproc put_byte_to_file
    stx tmp6
    sty tmp7

    pha
    ldy #FS_BUFFERPTR
    lda (tmp6), y
    tay
    pla
    sta (tmp6), y

    iny
    cpy #FS_BUFFER + 128
    zif eq
        jsr flush_file
        ldy #FS_BUFFER
    zendif
    tya
    ldy #FS_BUFFERPTR
    sta (tmp6), y
    rts
zendproc

; If the value is 0, returns with C set to signal end of file.
zproc get_byte_from_file
    stx tmp6
    sty tmp7

    ldy #FS_BUFFERPTR
    lda (tmp6), y
    cmp #FS_BUFFER+128
    zif eq
        jsr set_dma_to_buffer_address_of_file
        ldy #BDOS_READ_SEQUENTIAL
        lda tmp6
        ldx tmp7
        jsr bdos_and_file_error

        lda #FS_BUFFER
    zendif

    tay
    lda (tmp6), y
    pha
    iny
    tya
    ldy #FS_BUFFERPTR
    sta (tmp6), y
    pla
    clc
    tay                     ; set flags
    zif eq
        sec
    zendif
    rts
zendproc
