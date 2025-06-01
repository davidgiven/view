    cpu 1

; Constants
buffer_keyboard                             = 0
MAX_LINE_LENGTH                             = 132
osbyte_acknowledge_escape                   = 126
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

; Memory locations
ptr1                            = &0000
current_edit_line_ptr           = &0002
current_format_line_ptr         = &0004
current_ruler_ptr               = &0006
ptr4                            = &0008
zp_initialisation_canary        = &000a
page                            = &000b
top                             = &000d
himem                           = &000f
l0011                           = &0011
l0012                           = &0012
ptr6                            = &0013
ptr5                            = &0015
printer_driver_ptr              = &0017
first_macro_ptr                 = &0019
last_macro_ptr                  = &001b
ptr3                            = &001d
oshwm                           = &001f
l0021                           = &0021
top_margin                      = &0022
bottom_margin                   = &0023
header_margin                   = &0024
footer_margin                   = &0025
page_length                     = &0026
line_spacing                    = &0027
footers_enabled_flag            = &0028
headers_enabled_flag            = &0029
highlight1_code                 = &002a
highlight2_code                 = &002b
rhs_extra_margin                = &002c
macro_executing_flag            = &002d
two_sided_flag                  = &002e
left_margin                     = &002f
l0030                           = &0030
l0031                           = &0031
l0032                           = &0032
l0033                           = &0033
l0034                           = &0034
screen_height                   = &0035
screen_width                    = &0036
current_screen_mode             = &0037
l0038                           = &0038
l0039                           = &0039
l003a                           = &003a
l003b                           = &003b
l003c                           = &003c
l003d                           = &003d
ruler_right_stop                = &003e
ruler_left_stop                 = &003f
xpos                            = &0040
l0041                           = &0041
l0042                           = &0042
l0043                           = &0043
l0044                           = &0044
l0045                           = &0045
l0046                           = &0046
l0047                           = &0047
l0048                           = &0048
l0049                           = &0049
l004a                           = &004a
ptr2                            = &004b
rw_file_handle                  = &004d
error_handling_mode             = &004e
format_mode_flag                = &004f
justifying_flag                 = &0050
insert_mode_flag                = &0051
is_tube_flag                    = &0052
markers_array                   = &0053
area_start_ptr                  = &005f
area_end_ptr                    = &0061
l0063                           = &0063
l0064                           = &0064
l0065                           = &0065
l0066                           = &0066
l0067                           = &0067
l0068                           = &0068
print_flags                     = &0069
edit_input_file_handle          = &006a
edit_output_file_handle         = &006b
microspacing_flag               = &006c
l006d                           = &006d
l006e                           = &006e
l006f                           = &006f
l0070                           = &0070
l0071                           = &0071
l0072                           = &0072
l0073                           = &0073
l0074                           = &0074
l0075                           = &0075
l0076                           = &0076
ypos                            = &0077
print_xpos                      = &0078
l0079                           = &0079
l007a                           = &007a
current_tab_key                 = &007b
folding_flag                    = &007c
l007d                           = &007d
l007e                           = &007e
input_buffer_ptr                = &007f
l0081                           = &0081
l0082                           = &0082
l0083                           = &0083
l0084                           = &0084
tmp0                            = &0085
tmp1                            = &0086
tmp2                            = &0087
tmp3                            = &0088
tmp4                            = &0089
tmp5                            = &008a
tmp6                            = &008b
tmp7                            = &008c
tmp8                            = &008d
tmp9                            = &008e
l00ef                           = &00ef
l00f0                           = &00f0
l00f1                           = &00f1
os_text_ptr                     = &00f2
l00fd                           = &00fd
escape_flag                     = &00ff
l0101                           = &0101
l0102                           = &0102
l0103                           = &0103
brkv                            = &0202
printer_driver_block            = &0400
input_buffer                    = &0500
l0501                           = &0501
l0502                           = &0502
l0503                           = &0503
l0504                           = &0504
l0505                           = &0505
l0506                           = &0506
l050a                           = &050a
l050b                           = &050b
l050c                           = &050c
l050d                           = &050d
l050e                           = &050e
l050f                           = &050f
l0510                           = &0510
l0511                           = &0511
current_line_buffer             = &0545
l05cc                           = &05cc
document_initialisation_canary  = &05ce
current_ruler_buffer            = &05cf
output_buffer                   = &0654
header_text_maybe               = &06d8
footer_text_maybe               = &071a
filename                        = &075c
another_filename                = &0770
printer_driver_name             = &0784
register_value_array            = &0798
register_value_l                = &07ae
register_value_p                = &07b6
line_lengths                    = &07cc
document_filename               = &07ec
rom_workspace_array             = &0df0
lf894                           = &f894
osfind                          = &ffce
osbput                          = &ffd4
osbget                          = &ffd7
osargs                          = &ffda
osfile                          = &ffdd
osrdch                          = &ffe0
osasci                          = &ffe3
osnewl                          = &ffe7
oswrch                          = &ffee
osword                          = &fff1
osbyte                          = &fff4
oscli                           = &fff7

    org &8000

; Sideways ROM header
.pydis_start
.rom_header
.language_entry
    jmp language_handler                                              ; 8000: 4c bd 80    L..

.service_entry
    jmp service_handler                                               ; 8003: 4c 21 80    L!.

.rom_type
    equb &c2                                                          ; 8006: c2          .
.copyright_offset
    equb copyright - rom_header                                       ; 8007: 0d          .
.binary_version
    equb 4                                                            ; 8008: 04          .
.title
    equs "VIEW"                                                       ; 8009: 56 49 45... VIE
.copyright
    equb 0                                                            ; 800d: 00          .
    equs "(C) 1982 Acornsoft", 0                                      ; 800e: 28 43 29... (C)

; ***************************************************************************************
; &8021 referenced 1 time by &8003
.service_handler
    pha                                                               ; 8021: 48          H
    txa                                                               ; 8022: 8a          .
    pha                                                               ; 8023: 48          H
    tya                                                               ; 8024: 98          .
    pha                                                               ; 8025: 48          H
    tsx                                                               ; 8026: ba          .
    lda l0103,x                                                       ; 8027: bd 03 01    ...
    cmp #9                                                            ; 802a: c9 09       ..
    beq help_handler                                                  ; 802c: f0 34       .4
    cmp #7                                                            ; 802e: c9 07       ..
    beq osbyte_handler                                                ; 8030: f0 53       .S
    cmp #2                                                            ; 8032: c9 02       ..
    beq claim_private_workspace_handler                               ; 8034: f0 43       .C
    cmp #4                                                            ; 8036: c9 04       ..
    bne exit_from_service_call                                        ; 8038: d0 39       .9
    ldx #&ff                                                          ; 803a: a2 ff       ..
    dey                                                               ; 803c: 88          .
; &803d referenced 1 time by &804b
.loop_c803d
    inx                                                               ; 803d: e8          .
    iny                                                               ; 803e: c8          .
    lda word_command_str,x                                            ; 803f: bd b8 80    ...
    bmi c805a                                                         ; 8042: 30 16       0.
    lda (os_text_ptr),y                                               ; 8044: b1 f2       ..
    and #&df                                                          ; 8046: 29 df       ).
    cmp word_command_str,x                                            ; 8048: dd b8 80    ...
    beq loop_c803d                                                    ; 804b: f0 f0       ..
    cmp #&0e                                                          ; 804d: c9 0e       ..
    bne exit_from_service_call                                        ; 804f: d0 22       ."
; &8051 referenced 1 time by &805e
.loop_c8051
    pla                                                               ; 8051: 68          h
    pla                                                               ; 8052: 68          h
    tax                                                               ; 8053: aa          .              ; X=ROM number
    pla                                                               ; 8054: 68          h
    lda #osbyte_enter_language                                        ; 8055: a9 8e       ..
    jmp osbyte                                                        ; 8057: 4c f4 ff    L..            ; Enter language ROM X

; &805a referenced 1 time by &8042
.c805a
    lda (os_text_ptr),y                                               ; 805a: b1 f2       ..
    cmp #&21 ; '!'                                                    ; 805c: c9 21       .!
    bcc loop_c8051                                                    ; 805e: 90 f1       ..
    bcs exit_from_service_call                                        ; 8060: b0 11       ..             ; ALWAYS branch

; &8062 referenced 1 time by &802c
.help_handler
    lda (os_text_ptr),y                                               ; 8062: b1 f2       ..
    cmp #&0d                                                          ; 8064: c9 0d       ..
    bne exit_from_service_call                                        ; 8066: d0 0b       ..
    jsr osnewl                                                        ; 8068: 20 e7 ff     ..            ; Write newline (characters 10 and 13)
    ldx #1                                                            ; 806b: a2 01       ..
    jsr print_x_words_of_help                                         ; 806d: 20 2a a8     *.
    jsr osnewl                                                        ; 8070: 20 e7 ff     ..            ; Write newline (characters 10 and 13)
; &8073 referenced 9 times by &8038, &804f, &8060, &8066, &8083, &8089, &808f, &809d, &80b6
.exit_from_service_call
    pla                                                               ; 8073: 68          h
    tay                                                               ; 8074: a8          .
    pla                                                               ; 8075: 68          h
    tax                                                               ; 8076: aa          .
    pla                                                               ; 8077: 68          h
; &8078 referenced 1 time by &80bf
.return_1
    rts                                                               ; 8078: 60          `

; &8079 referenced 1 time by &8034
.claim_private_workspace_handler
    tsx                                                               ; 8079: ba          .
    lda l0102,x                                                       ; 807a: bd 02 01    ...
    tax                                                               ; 807d: aa          .
    lda #0                                                            ; 807e: a9 00       ..
    sta rom_workspace_array,x                                         ; 8080: 9d f0 0d    ...
    beq exit_from_service_call                                        ; 8083: f0 ee       ..             ; ALWAYS branch

; &8085 referenced 1 time by &8030
.osbyte_handler
    lda l00ef                                                         ; 8085: a5 ef       ..
    cmp #&a3                                                          ; 8087: c9 a3       ..
    bne exit_from_service_call                                        ; 8089: d0 e8       ..
    lda l00f0                                                         ; 808b: a5 f0       ..
    cmp #&ff                                                          ; 808d: c9 ff       ..
    bne exit_from_service_call                                        ; 808f: d0 e2       ..
    tsx                                                               ; 8091: ba          .
    lda l0102,x                                                       ; 8092: bd 02 01    ...
    ldy l00f1                                                         ; 8095: a4 f1       ..
    cpy #1                                                            ; 8097: c0 01       ..
    beq c80aa                                                         ; 8099: f0 0f       ..
    cpy #&81                                                          ; 809b: c0 81       ..
    bne exit_from_service_call                                        ; 809d: d0 d4       ..
    tay                                                               ; 809f: a8          .
    lda rom_workspace_array,y                                         ; 80a0: b9 f0 0d    ...
    ora #&80                                                          ; 80a3: 09 80       ..
    sta rom_workspace_array,y                                         ; 80a5: 99 f0 0d    ...
    bne c80b1                                                         ; 80a8: d0 07       ..             ; ALWAYS branch

; &80aa referenced 1 time by &8099
.c80aa
    tay                                                               ; 80aa: a8          .
    lda rom_workspace_array,y                                         ; 80ab: b9 f0 0d    ...
    sta l0101,x                                                       ; 80ae: 9d 01 01    ...
; &80b1 referenced 1 time by &80a8
.c80b1
    lda #0                                                            ; 80b1: a9 00       ..
    sta l0103,x                                                       ; 80b3: 9d 03 01    ...
    beq exit_from_service_call                                        ; 80b6: f0 bb       ..             ; ALWAYS branch

; &80b8 referenced 2 times by &803f, &8048
.word_command_str
    equs "WORD"                                                       ; 80b8: 57 4f 52... WOR
    equb &ff                                                          ; 80bc: ff          .

; &80bd referenced 1 time by &8000
.language_handler
    cmp #1                                                            ; 80bd: c9 01       ..
    bne return_1                                                      ; 80bf: d0 b7       ..
    cli                                                               ; 80c1: 58          X
    jsr create_go_command                                             ; 80c2: 20 28 b3     (.
.sub_c80c5
    lda brk_handler_ptr                                               ; 80c5: ad f1 80    ...
    sta brkv                                                          ; 80c8: 8d 02 02    ...
    lda l80f2                                                         ; 80cb: ad f2 80    ...
    sta brkv+1                                                        ; 80ce: 8d 03 02    ...
    ldx #&ff                                                          ; 80d1: a2 ff       ..
    txs                                                               ; 80d3: 9a          .
    stx error_handling_mode                                           ; 80d4: 86 4e       .N
    jsr system_init                                                   ; 80d6: 20 79 af     y.
    lda #osbyte_read_oshwm                                            ; 80d9: a9 83       ..
    jsr osbyte                                                        ; 80db: 20 f4 ff     ..            ; Read top of operating system RAM address (OSHWM)
    cpx oshwm                                                         ; 80de: e4 1f       ..             ; X and Y contain the address of OSHWM (low, high)
    bne c80f3                                                         ; 80e0: d0 11       ..
    cpy oshwm+1                                                       ; 80e2: c4 20       .
    bne c80f3                                                         ; 80e4: d0 0d       ..
    jsr check_for_bad_document                                        ; 80e6: 20 86 8e     ..
    bne c80f3                                                         ; 80e9: d0 08       ..
    jsr ca93c                                                         ; 80eb: 20 3c a9     <.
    jmp run_cli                                                       ; 80ee: 4c f6 80    L..

; &80f1 referenced 1 time by &80c5
.brk_handler_ptr
l80f2 = brk_handler_ptr+1
    equw brk_handler                                                  ; 80f1: 2e 8a       ..
; &80f2 referenced 1 time by &80cb

; &80f3 referenced 3 times by &80e0, &80e4, &80e9
.c80f3
    jsr initialise_document                                           ; 80f3: 20 cf af     ..
; ***************************************************************************************
; &80f6 referenced 3 times by &80ee, &823e, &8283
.run_cli
    jsr clear_screen                                                  ; 80f6: 20 82 a7     ..
    lda #osbyte_set_cursor_editing                                    ; 80f9: a9 04       ..
    ldx #0                                                            ; 80fb: a2 00       ..
    jsr osbyte                                                        ; 80fd: 20 f4 ff     ..            ; Enable cursor editing (X=0)
    lda #osbyte_read_write_function_key_status                        ; 8100: a9 e1       ..
    ldx #1                                                            ; 8102: a2 01       ..
    ldy #0                                                            ; 8104: a0 00       ..
    jsr osbyte                                                        ; 8106: 20 f4 ff     ..            ; Write function key status, value X=1
    jsr cursor_on                                                     ; 8109: 20 cd a7     ..
    ldx #0                                                            ; 810c: a2 00       ..
    jsr print_x_words_of_help                                         ; 810e: 20 2a a8     *.
    jsr check_for_bad_document                                        ; 8111: 20 86 8e     ..
    beq c811f                                                         ; 8114: f0 09       ..
    jsr osnewl                                                        ; 8116: 20 e7 ff     ..            ; Write newline (characters 10 and 13)
    jsr display_nl_then_no_text                                       ; 8119: 20 ae 8e     ..
    jmp c8139                                                         ; 811c: 4c 39 81    L9.

; &811f referenced 1 time by &8114
.c811f
    jsr print_inline_string                                           ; 811f: 20 fa a7     ..
    equs &0d, &0d, "Bytes free "                                      ; 8122: 0d 0d 42... ..B
    equb 0                                                            ; 812f: 00          .

    jsr compute_bytes_free                                            ; 8130: 20 be af     ..
    jsr render_number_to_screen                                       ; 8133: 20 b2 a6     ..
    jsr osnewl                                                        ; 8136: 20 e7 ff     ..            ; Write newline (characters 10 and 13)
; &8139 referenced 1 time by &811c
.c8139
    jsr sub_c89e5                                                     ; 8139: 20 e5 89     ..
    bit l003c                                                         ; 813c: 24 3c       $<
    bvs c816d                                                         ; 813e: 70 2d       p-
    lda l003c                                                         ; 8140: a5 3c       .<
    ror a                                                             ; 8142: 6a          j
    bcc c816d                                                         ; 8143: 90 28       .(
    jsr print_inline_string                                           ; 8145: 20 fa a7     ..
    equs "Input file is "                                             ; 8148: 49 6e 70... Inp
    equb 0                                                            ; 8156: 00          .

    lda l0041                                                         ; 8157: a5 41       .A
    bne c8163                                                         ; 8159: d0 08       ..
    jsr print_inline_string                                           ; 815b: 20 fa a7     ..
    equs "not "                                                       ; 815e: 6e 6f 74... not
    equb 0                                                            ; 8162: 00          .

; &8163 referenced 1 time by &8159
.c8163
    jsr print_inline_string                                           ; 8163: 20 fa a7     ..
    equs "empty", &0d                                                 ; 8166: 65 6d 70... emp
    equb 0                                                            ; 816c: 00          .

; &816d referenced 2 times by &813e, &8143
.c816d
    jsr print_inline_string                                           ; 816d: 20 fa a7     ..
    equs "Screen mode "                                               ; 8170: 53 63 72... Scr
    equb 0                                                            ; 817c: 00          .

    lda current_screen_mode                                           ; 817d: a5 37       .7
    ora #&30 ; '0'                                                    ; 817f: 09 30       .0
    jsr oswrch                                                        ; 8181: 20 ee ff     ..            ; Write character
    jsr osnewl                                                        ; 8184: 20 e7 ff     ..            ; Write newline (characters 10 and 13)
    lda printer_driver_name                                           ; 8187: ad 84 07    ...
    beq c81b6                                                         ; 818a: f0 2a       .*
    jsr print_inline_string                                           ; 818c: 20 fa a7     ..
    equs "Printer "                                                   ; 818f: 50 72 69... Pri
    equb 0                                                            ; 8197: 00          .

    ldx #0                                                            ; 8198: a2 00       ..
; &819a referenced 1 time by &81a5
.loop_c819a
    lda printer_driver_name,x                                         ; 819a: bd 84 07    ...
    cmp #&0d                                                          ; 819d: c9 0d       ..
    beq c81a7                                                         ; 819f: f0 06       ..
    jsr osasci                                                        ; 81a1: 20 e3 ff     ..            ; Write character
    inx                                                               ; 81a4: e8          .
    bne loop_c819a                                                    ; 81a5: d0 f3       ..
; &81a7 referenced 1 time by &819f
.c81a7
    lda microspacing_flag                                             ; 81a7: a5 6c       .l
    beq c81b3                                                         ; 81a9: f0 08       ..
    jsr print_inline_string                                           ; 81ab: 20 fa a7     ..
    equs " (m)"                                                       ; 81ae: 20 28 6d...  (m
    equb 0                                                            ; 81b2: 00          .

; &81b3 referenced 1 time by &81a9
.c81b3
    jsr osnewl                                                        ; 81b3: 20 e7 ff     ..            ; Write newline (characters 10 and 13)
; &81b6 referenced 1 time by &818a
.c81b6
    ldx #0                                                            ; 81b6: a2 00       ..
    ldy #0                                                            ; 81b8: a0 00       ..
; &81ba referenced 1 time by &81eb
.c81ba
    lda markers_array+1,x                                             ; 81ba: b5 54       .T
    beq c81e7                                                         ; 81bc: f0 29       .)
    tya                                                               ; 81be: 98          .
    bne c81db                                                         ; 81bf: d0 1a       ..
    stx l0083                                                         ; 81c1: 86 83       ..
    jsr print_inline_string                                           ; 81c3: 20 fa a7     ..
    equs "Marker(s) set "                                             ; 81c6: 4d 61 72... Mar
    equb 0                                                            ; 81d4: 00          .

    ldx l0083                                                         ; 81d5: a6 83       ..
    ldy #1                                                            ; 81d7: a0 01       ..
    bne c81e0                                                         ; 81d9: d0 05       ..             ; ALWAYS branch

; &81db referenced 1 time by &81bf
.c81db
    lda #&2c ; ','                                                    ; 81db: a9 2c       .,
    jsr oswrch                                                        ; 81dd: 20 ee ff     ..            ; Write character 44
; &81e0 referenced 1 time by &81d9
.c81e0
    txa                                                               ; 81e0: 8a          .
    lsr a                                                             ; 81e1: 4a          J
    adc #&31 ; '1'                                                    ; 81e2: 69 31       i1
    jsr oswrch                                                        ; 81e4: 20 ee ff     ..            ; Write character
; &81e7 referenced 1 time by &81bc
.c81e7
    inx                                                               ; 81e7: e8          .
    inx                                                               ; 81e8: e8          .
    cpx #&0c                                                          ; 81e9: e0 0c       ..
    bne c81ba                                                         ; 81eb: d0 cd       ..
    tya                                                               ; 81ed: 98          .
    beq c81f3                                                         ; 81ee: f0 03       ..
    jsr osnewl                                                        ; 81f0: 20 e7 ff     ..            ; Write newline (characters 10 and 13)
; &81f3 referenced 1 time by &81ee
.c81f3
    jsr osnewl                                                        ; 81f3: 20 e7 ff     ..            ; Write newline (characters 10 and 13)
; ***************************************************************************************
; &81f6 referenced 14 times by &8235, &8255, &8279, &843d, &8448, &84ab, &85b3, &869b, &878e, &8949, &8a4c, &8e60, &8f26, &a827
.cli_loop
    jsr acknowledge_escape                                            ; 81f6: 20 6e a7     n.
    jsr stop_printing                                                 ; 81f9: 20 4b 84     K.
    ldx #&ff                                                          ; 81fc: a2 ff       ..
    stx error_handling_mode                                           ; 81fe: 86 4e       .N
    txs                                                               ; 8200: 9a          .
    inx                                                               ; 8201: e8          .              ; X=&00
    stx print_flags                                                   ; 8202: 86 69       .i
    jsr print_inline_string                                           ; 8204: 20 fa a7     ..
    equs "=>"                                                         ; 8207: 3d 3e       =>
    equb 0                                                            ; 8209: 00          .

    ldx #<input_buffer                                                ; 820a: a2 00       ..
    stx tmp0                                                          ; 820c: 86 85       ..
    ldx #>input_buffer                                                ; 820e: a2 05       ..
    stx tmp1                                                          ; 8210: 86 86       ..
    ldx #&44 ; 'D'                                                    ; 8212: a2 44       .D
    stx tmp2                                                          ; 8214: 86 87       ..
    ldx #&20 ; ' '                                                    ; 8216: a2 20       .
    stx tmp3                                                          ; 8218: 86 88       ..
    ldx #&7e ; '~'                                                    ; 821a: a2 7e       .~
    stx tmp4                                                          ; 821c: 86 89       ..
    ldx #<(tmp0)                                                      ; 821e: a2 85       ..
    lda #osword_read_line                                             ; 8220: a9 00       ..
    sta input_buffer_ptr                                              ; 8222: 85 7f       ..
    tay                                                               ; 8224: a8          .              ; Y=&00
    jsr osword                                                        ; 8225: 20 f1 ff     ..            ; Read line from input stream (exits with C=1 if ESCAPE pressed)
    bcc input_line_not_escaped                                        ; 8228: 90 17       ..
    jsr acknowledge_escape                                            ; 822a: 20 6e a7     n.
    jsr check_for_bad_document                                        ; 822d: 20 86 8e     ..
    beq c8238                                                         ; 8230: f0 06       ..
    jsr display_nl_then_no_text                                       ; 8232: 20 ae 8e     ..
    jmp cli_loop                                                      ; 8235: 4c f6 81    L..

; &8238 referenced 1 time by &8230
.c8238
    jmp run_editor                                                    ; 8238: 4c 33 9b    L3.

; ***************************************************************************************
; &823b referenced 2 times by &836e, &a273
.esc_key
    jsr ca93c                                                         ; 823b: 20 3c a9     <.
    jmp run_cli                                                       ; 823e: 4c f6 80    L..

; &8241 referenced 1 time by &8228
.input_line_not_escaped
    ldy #0                                                            ; 8241: a0 00       ..
    jsr sub_c8e2d                                                     ; 8243: 20 2d 8e     -.
    beq c8255                                                         ; 8246: f0 0d       ..
    sty input_buffer_ptr                                              ; 8248: 84 7f       ..
    cmp #&2a ; '*'                                                    ; 824a: c9 2a       .*
    bne input_line_not_oscli                                          ; 824c: d0 0a       ..
    ldx #<(input_buffer)                                              ; 824e: a2 00       ..
    ldy #>(input_buffer)                                              ; 8250: a0 05       ..
    jsr oscli                                                         ; 8252: 20 f7 ff     ..
; &8255 referenced 1 time by &8246
.c8255
    jmp cli_loop                                                      ; 8255: 4c f6 81    L..

; &8258 referenced 1 time by &824c
.input_line_not_oscli
    jsr parse_command                                                 ; 8258: 20 47 a8     G.
    sty input_buffer_ptr+1                                            ; 825b: 84 80       ..
    bcs c8263                                                         ; 825d: b0 04       ..
    cpy #&19                                                          ; 825f: c0 19       ..
    bcc c826e                                                         ; 8261: 90 0b       ..
; &8263 referenced 1 time by &825d
.c8263
    jsr print_inline_string                                           ; 8263: 20 fa a7     ..
    equs "Mistake"                                                    ; 8266: 4d 69 73... Mis
    equb &ff                                                          ; 826d: ff          .

; &826e referenced 1 time by &8261
.c826e
    ror a                                                             ; 826e: 6a          j
    bcc c827c                                                         ; 826f: 90 0b       ..
    jsr check_for_bad_document                                        ; 8271: 20 86 8e     ..
    beq c827c                                                         ; 8274: f0 06       ..
    jsr display_no_text                                               ; 8276: 20 b1 8e     ..
    jmp cli_loop                                                      ; 8279: 4c f6 81    L..

; &827c referenced 2 times by &826f, &8274
.c827c
    lda input_buffer_ptr+1                                            ; 827c: a5 80       ..
    ldy #4                                                            ; 827e: a0 04       ..
    jsr call_through_jumptable                                        ; 8280: 20 92 a8     ..
    jmp run_cli                                                       ; 8283: 4c f6 80    L..

; ***************************************************************************************
.search_cmd
    jsr sub_c8412                                                     ; 8286: 20 12 84     ..
    beq c82e7                                                         ; 8289: f0 5c       .\
    jsr parse_marks_from_command                                      ; 828b: 20 89 89     ..
    jsr sanitise_area                                                 ; 828e: 20 5d 89     ].
    beq c82fa                                                         ; 8291: f0 67       .g
    jsr sub_c8c7c                                                     ; 8293: 20 7c 8c     |.
    jsr c8b7b                                                         ; 8296: 20 7b 8b     {.
    bne c82fa                                                         ; 8299: d0 5f       ._
    jsr cabcb                                                         ; 829b: 20 cb ab     ..
    jmp run_editor                                                    ; 829e: 4c 33 9b    L3.

; ***************************************************************************************
.change_cmd
    jsr sub_c83f0                                                     ; 82a1: 20 f0 83     ..
    bcs c82fa                                                         ; 82a4: b0 54       .T
    beq c82e7                                                         ; 82a6: f0 3f       .?
    jsr c8b7b                                                         ; 82a8: 20 7b 8b     {.
    bne c82fa                                                         ; 82ab: d0 4d       .M
    ldx #0                                                            ; 82ad: a2 00       ..
    stx ptr3                                                          ; 82af: 86 1d       ..
    stx ptr3+1                                                        ; 82b1: 86 1e       ..
; &82b3 referenced 1 time by &82c8
.loop_c82b3
    inc ptr3                                                          ; 82b3: e6 1d       ..
    bne c82b9                                                         ; 82b5: d0 02       ..
    inc ptr3+1                                                        ; 82b7: e6 1e       ..
; &82b9 referenced 1 time by &82b5
.c82b9
    jsr cabcb                                                         ; 82b9: 20 cb ab     ..
    lda #0                                                            ; 82bc: a9 00       ..
    sta print_xpos                                                    ; 82be: 85 78       .x
    jsr sub_c8a4f                                                     ; 82c0: 20 4f 8a     O.
    bcs c830d                                                         ; 82c3: b0 48       .H
    jsr c8b7b                                                         ; 82c5: 20 7b 8b     {.
    beq loop_c82b3                                                    ; 82c8: f0 e9       ..
    ldx ptr3                                                          ; 82ca: a6 1d       ..
    ldy ptr3+1                                                        ; 82cc: a4 1e       ..
    jsr render_number_to_screen                                       ; 82ce: 20 b2 a6     ..
    jsr print_inline_string                                           ; 82d1: 20 fa a7     ..
    equs " string(s) changed"                                         ; 82d4: 20 73 74...  st
    equb &ff                                                          ; 82e6: ff          .

; &82e7 referenced 3 times by &8289, &82a6, &8320
.c82e7
    jsr print_inline_string                                           ; 82e7: 20 fa a7     ..
    equs "No target given"                                            ; 82ea: 4e 6f 20... No
    equb &ff                                                          ; 82f9: ff          .

; &82fa referenced 5 times by &8291, &8299, &82a4, &82ab, &8325
.c82fa
    jsr print_inline_string                                           ; 82fa: 20 fa a7     ..
    equs "No string found"                                            ; 82fd: 4e 6f 20... No
    equb &ff                                                          ; 830c: ff          .

; &830d referenced 1 time by &82c3
.c830d
    jmp c853f                                                         ; 830d: 4c 3f 85    L?.

; &8310 referenced 2 times by &83a3, &83ac
.sub_c8310
    iny                                                               ; 8310: c8          .
    lda input_buffer,y                                                ; 8311: b9 00 05    ...
    sta l0084                                                         ; 8314: 85 84       ..
    cmp l007e                                                         ; 8316: c5 7e       .~
    beq return_2                                                      ; 8318: f0 02       ..
    cmp #&0d                                                          ; 831a: c9 0d       ..
; &831c referenced 3 times by &8318, &833a, &8359
.return_2
    rts                                                               ; 831c: 60          `

; ***************************************************************************************
.replace_cmd
    jsr sub_c83f0                                                     ; 831d: 20 f0 83     ..
    beq c82e7                                                         ; 8320: f0 c5       ..
    jsr c8b7b                                                         ; 8322: 20 7b 8b     {.
    bne c82fa                                                         ; 8325: d0 d3       ..
    jsr cabcb                                                         ; 8327: 20 cb ab     ..
    jsr enter_editor_mode                                             ; 832a: 20 9b b0     ..
; &832d referenced 1 time by &835e
.c832d
    jsr sub_c8361                                                     ; 832d: 20 61 83     a.
    ldx #&52 ; 'R'                                                    ; 8330: a2 52       .R
    ldy #&50 ; 'P'                                                    ; 8332: a0 50       .P
    jsr draw_prompt_characters                                        ; 8334: 20 a8 a7     ..
    jsr flush_and_read_char                                           ; 8337: 20 5a a7     Z.
    bcs return_2                                                      ; 833a: b0 e0       ..
    and #&df                                                          ; 833c: 29 df       ).
    ldx #0                                                            ; 833e: a2 00       ..
    cmp #&59 ; 'Y'                                                    ; 8340: c9 59       .Y
    beq c8349                                                         ; 8342: f0 05       ..
    dex                                                               ; 8344: ca          .              ; X=&ff
    cmp #&4f ; 'O'                                                    ; 8345: c9 4f       .O
    bne c8356                                                         ; 8347: d0 0d       ..
; &8349 referenced 1 time by &8342
.c8349
    stx print_xpos                                                    ; 8349: 86 78       .x
    jsr sub_c8371                                                     ; 834b: 20 71 83     q.
    jsr sub_c8a4f                                                     ; 834e: 20 4f 8a     O.
    bcs c836b                                                         ; 8351: b0 18       ..
    jsr sub_c8361                                                     ; 8353: 20 61 83     a.
; &8356 referenced 1 time by &8347
.c8356
    jsr c8b7b                                                         ; 8356: 20 7b 8b     {.
    bne return_2                                                      ; 8359: d0 c1       ..
    jsr cabcb                                                         ; 835b: 20 cb ab     ..
    jmp c832d                                                         ; 835e: 4c 2d 83    L-.

; &8361 referenced 2 times by &832d, &8353
.sub_c8361
    lda #0                                                            ; 8361: a9 00       ..
    sta l006e                                                         ; 8363: 85 6e       .n
    jsr sub_ca276                                                     ; 8365: 20 76 a2     v.
    jmp ca93c                                                         ; 8368: 4c 3c a9    L<.

; &836b referenced 1 time by &8351
.c836b
    jsr sub_ca94a                                                     ; 836b: 20 4a a9     J.
    jmp esc_key                                                       ; 836e: 4c 3b 82    L;.

; &8371 referenced 1 time by &834b
.sub_c8371
    lda ptr2                                                          ; 8371: a5 4b       .K
    sta tmp8                                                          ; 8373: 85 8d       ..
    lda ptr2+1                                                        ; 8375: a5 4c       .L
    sta tmp9                                                          ; 8377: 85 8e       ..
    ldy #0                                                            ; 8379: a0 00       ..
    ldx #0                                                            ; 837b: a2 00       ..
; &837d referenced 2 times by &8392, &8396
.c837d
    lda tmp9                                                          ; 837d: a5 8e       ..
    cmp l0066                                                         ; 837f: c5 66       .f
    bne c8389                                                         ; 8381: d0 06       ..
    lda tmp8                                                          ; 8383: a5 8d       ..
    cmp l0065                                                         ; 8385: c5 65       .e
    beq c8398                                                         ; 8387: f0 0f       ..
; &8389 referenced 1 time by &8381
.c8389
    lda (tmp8),y                                                      ; 8389: b1 8d       ..
    cmp #&0d                                                          ; 838b: c9 0d       ..
    bne c8390                                                         ; 838d: d0 01       ..
    inx                                                               ; 838f: e8          .
; &8390 referenced 1 time by &838d
.c8390
    inc tmp8                                                          ; 8390: e6 8d       ..
    bne c837d                                                         ; 8392: d0 e9       ..
    inc tmp9                                                          ; 8394: e6 8e       ..
    bne c837d                                                         ; 8396: d0 e5       ..
; &8398 referenced 1 time by &8387
.c8398
    inc l0074                                                         ; 8398: e6 74       .t
    txa                                                               ; 839a: 8a          .
    beq return_3                                                      ; 839b: f0 41       .A
    jmp ca741                                                         ; 839d: 4c 41 a7    LA.

; ***************************************************************************************
; &83a0 referenced 2 times by &83fd, &841f
.expand_escaped_string
    stx l0083                                                         ; 83a0: 86 83       ..
    dey                                                               ; 83a2: 88          .
; &83a3 referenced 1 time by &83d8
.c83a3
    jsr sub_c8310                                                     ; 83a3: 20 10 83     ..
    beq c83da                                                         ; 83a6: f0 32       .2
    cmp #&5e ; '^'                                                    ; 83a8: c9 5e       .^
    bne c83ca                                                         ; 83aa: d0 1e       ..
    jsr sub_c8310                                                     ; 83ac: 20 10 83     ..
    beq c83da                                                         ; 83af: f0 29       .)
    jsr sub_c8c63                                                     ; 83b1: 20 63 8c     c.
    sta l0082                                                         ; 83b4: 85 82       ..
    ldx #&fe                                                          ; 83b6: a2 fe       ..
; &83b8 referenced 1 time by &83c1
.loop_c83b8
    inx                                                               ; 83b8: e8          .
    inx                                                               ; 83b9: e8          .
    lda escaped_char_table,x                                          ; 83ba: bd df 83    ...
    bmi c83c8                                                         ; 83bd: 30 09       0.
    cmp l0082                                                         ; 83bf: c5 82       ..
    bne loop_c83b8                                                    ; 83c1: d0 f5       ..
    lda l83e0,x                                                       ; 83c3: bd e0 83    ...
    bne c83ca                                                         ; 83c6: d0 02       ..
; &83c8 referenced 1 time by &83bd
.c83c8
    lda l0084                                                         ; 83c8: a5 84       ..
; &83ca referenced 2 times by &83aa, &83c6
.c83ca
    ldx l007a                                                         ; 83ca: a6 7a       .z
    bne c83d1                                                         ; 83cc: d0 03       ..
    jsr sub_c8c5f                                                     ; 83ce: 20 5f 8c     _.
; &83d1 referenced 1 time by &83cc
.c83d1
    ldx l0083                                                         ; 83d1: a6 83       ..
    sta header_text_maybe,x                                           ; 83d3: 9d d8 06    ...
    inc l0083                                                         ; 83d6: e6 83       ..
    bne c83a3                                                         ; 83d8: d0 c9       ..
; &83da referenced 2 times by &83a6, &83af
.c83da
    ldx l0083                                                         ; 83da: a6 83       ..
    sty input_buffer_ptr                                              ; 83dc: 84 7f       ..
; &83de referenced 1 time by &839b
.return_3
    rts                                                               ; 83de: 60          `

; &83df referenced 1 time by &83ba
.escaped_char_table
    equs "?"                                                          ; 83df: 3f          ?
; &83e0 referenced 1 time by &83c3
.l83e0
    equb 1                                                            ; 83e0: 01          .
    equs "T"                                                          ; 83e1: 54          T
    equb 9                                                            ; 83e2: 09          .
    equs "C"                                                          ; 83e3: 43          C
    equb &0d                                                          ; 83e4: 0d          .
    equs "S"                                                          ; 83e5: 53          S
    equb 2                                                            ; 83e6: 02          .
    equs "L"                                                          ; 83e7: 4c          L
    equb &0b                                                          ; 83e8: 0b          .
    equs "Z"                                                          ; 83e9: 5a          Z
    equb &1a                                                          ; 83ea: 1a          .
    equs "-"                                                          ; 83eb: 2d          -
    equb &1c                                                          ; 83ec: 1c          .
    equs "*"                                                          ; 83ed: 2a          *
    equb &1d                                                          ; 83ee: 1d          .
    equb &ff                                                          ; 83ef: ff          .

; &83f0 referenced 2 times by &82a1, &831d
.sub_c83f0
    jsr sub_c8412                                                     ; 83f0: 20 12 84     ..
    beq c8410                                                         ; 83f3: f0 1b       ..
    jsr sub_c8e33                                                     ; 83f5: 20 33 8e     3.
    beq c8402                                                         ; 83f8: f0 08       ..
    ldy input_buffer_ptr                                              ; 83fa: a4 7f       ..
    iny                                                               ; 83fc: c8          .
    jsr expand_escaped_string                                         ; 83fd: 20 a0 83     ..
    stx l004a                                                         ; 8400: 86 4a       .J
; &8402 referenced 1 time by &83f8
.c8402
    jsr parse_marks_from_command                                      ; 8402: 20 89 89     ..
    jsr sanitise_area                                                 ; 8405: 20 5d 89     ].
    sec                                                               ; 8408: 38          8
    beq return_4                                                      ; 8409: f0 06       ..
    jsr sub_c8c7c                                                     ; 840b: 20 7c 8c     |.
    lda #1                                                            ; 840e: a9 01       ..
; &8410 referenced 1 time by &83f3
.c8410
    clc                                                               ; 8410: 18          .
; &8411 referenced 1 time by &8409
.return_4
    rts                                                               ; 8411: 60          `

; &8412 referenced 2 times by &8286, &83f0
.sub_c8412
    ldx #0                                                            ; 8412: a2 00       ..
    stx l007a                                                         ; 8414: 86 7a       .z
    stx l004a                                                         ; 8416: 86 4a       .J
    jsr sub_c8e33                                                     ; 8418: 20 33 8e     3.
    beq return_5                                                      ; 841b: f0 09       ..
    ldx #0                                                            ; 841d: a2 00       ..
    jsr expand_escaped_string                                         ; 841f: 20 a0 83     ..
    stx l007a                                                         ; 8422: 86 7a       .z
    cpx #0                                                            ; 8424: e0 00       ..
; &8426 referenced 1 time by &841b
.return_5
    rts                                                               ; 8426: 60          `

; ***************************************************************************************
.screen_cmd
    lda #&0e                                                          ; 8427: a9 0e       ..
    jsr oswrch                                                        ; 8429: 20 ee ff     ..            ; Write character 14
    jmp print_to_screen                                               ; 842c: 4c 45 84    LE.

; ***************************************************************************************
.sheets_cmd
    lda #&c0                                                          ; 842f: a9 c0       ..
    jsr sub_c845e                                                     ; 8431: 20 5e 84     ^.
    jsr sub_c8ebe                                                     ; 8434: 20 be 8e     ..
    jsr stop_printing                                                 ; 8437: 20 4b 84     K.
    jsr osnewl                                                        ; 843a: 20 e7 ff     ..            ; Write newline (characters 10 and 13)
    jmp cli_loop                                                      ; 843d: 4c f6 81    L..

; ***************************************************************************************
.print_cmd
    lda #&80                                                          ; 8440: a9 80       ..
    jsr sub_c845e                                                     ; 8442: 20 5e 84     ^.
; ***************************************************************************************
; &8445 referenced 1 time by &842c
.print_to_screen
    jsr sub_c8ebe                                                     ; 8445: 20 be 8e     ..
    jmp cli_loop                                                      ; 8448: 4c f6 81    L..

; ***************************************************************************************
; &844b referenced 9 times by &81f9, &8437, &853f, &8862, &89e5, &8a2e, &8f1a, &9011, &92a1
.stop_printing
    lda print_flags                                                   ; 844b: a5 69       .i
    bpl c8459                                                         ; 844d: 10 0a       ..
    rol print_flags                                                   ; 844f: 26 69       &i
    clc                                                               ; 8451: 18          .
    ror print_flags                                                   ; 8452: 66 69       fi
    lda #6                                                            ; 8454: a9 06       ..
    jsr call_printer_driver                                           ; 8456: 20 a3 94     ..
; &8459 referenced 1 time by &844d
.c8459
    lda #&0f                                                          ; 8459: a9 0f       ..
    jmp oswrch                                                        ; 845b: 4c ee ff    L..            ; Write character 15

; &845e referenced 3 times by &8431, &8442, &92d1
.sub_c845e
    sta print_flags                                                   ; 845e: 85 69       .i
    jsr prepare_printer_driver                                        ; 8460: 20 8b 94     ..
    lda #3                                                            ; 8463: a9 03       ..
    jmp call_printer_driver                                           ; 8465: 4c a3 94    L..

; ***************************************************************************************
.edit_cmd
    jsr sub_c8e49                                                     ; 8468: 20 49 8e     I.
    jsr error_if_cassette_filesystem                                  ; 846b: 20 63 8e     c.
    jsr initialise_document_if_document_bad                           ; 846e: 20 ca af     ..
    jsr sub_c8e1f                                                     ; 8471: 20 1f 8e     ..
    lda #2                                                            ; 8474: a9 02       ..
    sta error_handling_mode                                           ; 8476: 85 4e       .N
    lda #&40 ; '@'                                                    ; 8478: a9 40       .@
    jsr open_file                                                     ; 847a: 20 58 88     X.
    sta edit_input_file_handle                                        ; 847d: 85 6a       .j
    jsr sub_c88f8                                                     ; 847f: 20 f8 88     ..
    jsr sub_c8e1f                                                     ; 8482: 20 1f 8e     ..
    lda #&80                                                          ; 8485: a9 80       ..
    jsr open_file                                                     ; 8487: 20 58 88     X.
    sta edit_output_file_handle                                       ; 848a: 85 6b       .k
    ldx #0                                                            ; 848c: a2 00       ..
    stx l0041                                                         ; 848e: 86 41       .A
; &8490 referenced 1 time by &8499
.loop_c8490
    lda filename,x                                                    ; 8490: bd 5c 07    .\.
    sta another_filename,x                                            ; 8493: 9d 70 07    .p.
    inx                                                               ; 8496: e8          .
    cmp #&0d                                                          ; 8497: c9 0d       ..
    bne loop_c8490                                                    ; 8499: d0 f5       ..
    jsr initialise_document                                           ; 849b: 20 cf af     ..
    jsr sub_c8d24                                                     ; 849e: 20 24 8d     $.
    beq c84a8                                                         ; 84a1: f0 05       ..
    lda #1                                                            ; 84a3: a9 01       ..
    sta l003c                                                         ; 84a5: 85 3c       .<
    rts                                                               ; 84a7: 60          `

; &84a8 referenced 1 time by &84a1
.c84a8
    jsr c850d                                                         ; 84a8: 20 0d 85     ..
; &84ab referenced 4 times by &84be, &84e6, &84f4, &8506
.c84ab
    jmp cli_loop                                                      ; 84ab: 4c f6 81    L..

; ***************************************************************************************
.more_cmd
    jsr sub_c8e54                                                     ; 84ae: 20 54 8e     T.
    jsr parse_marks_from_command                                      ; 84b1: 20 89 89     ..
    lda area_start_ptr                                                ; 84b4: a5 5f       ._
    ldy area_start_ptr+1                                              ; 84b6: a4 60       .`
    jsr cabcb                                                         ; 84b8: 20 cb ab     ..
    jsr sub_c8d48                                                     ; 84bb: 20 48 8d     H.
    bne c84ab                                                         ; 84be: d0 eb       ..
    ldy #0                                                            ; 84c0: a0 00       ..
    ldx l003a                                                         ; 84c2: a6 3a       .:
; &84c4 referenced 1 time by &84cb
.loop_c84c4
    lda (current_ruler_ptr),y                                         ; 84c4: b1 06       ..
    sta current_ruler_buffer,y                                        ; 84c6: 99 cf 05    ...
    iny                                                               ; 84c9: c8          .
    dex                                                               ; 84ca: ca          .
    bne loop_c84c4                                                    ; 84cb: d0 f7       ..
    lda #&0d                                                          ; 84cd: a9 0d       ..
    sta current_ruler_buffer,y                                        ; 84cf: 99 cf 05    ...
    jsr sub_c89d3                                                     ; 84d2: 20 d3 89     ..
    jsr cb07a                                                         ; 84d5: 20 7a b0     z.
    jsr sub_c8535                                                     ; 84d8: 20 35 85     5.
    lda l0041                                                         ; 84db: a5 41       .A
    bne c84e8                                                         ; 84dd: d0 09       ..
    lda top                                                           ; 84df: a5 0d       ..
    ldy top+1                                                         ; 84e1: a4 0e       ..
    jsr sub_c8d28                                                     ; 84e3: 20 28 8d     (.
    beq c84ab                                                         ; 84e6: f0 c3       ..
; &84e8 referenced 1 time by &84dd
.c84e8
    jmp cb05a                                                         ; 84e8: 4c 5a b0    LZ.

; ***************************************************************************************
.finish_cmd
    jsr sub_c8e54                                                     ; 84eb: 20 54 8e     T.
; &84ee referenced 1 time by &8508
.loop_c84ee
    jsr reset_area_to_entire_document                                 ; 84ee: 20 fd ac     ..
    jsr sub_c8d48                                                     ; 84f1: 20 48 8d     H.
    bne c84ab                                                         ; 84f4: d0 b5       ..
    jsr sub_c89d3                                                     ; 84f6: 20 d3 89     ..
    jsr cb07a                                                         ; 84f9: 20 7a b0     z.
    jsr cb05a                                                         ; 84fc: 20 5a b0     Z.
    lda l0041                                                         ; 84ff: a5 41       .A
    bne c850d                                                         ; 8501: d0 0a       ..
    jsr sub_c8d24                                                     ; 8503: 20 24 8d     $.
    beq c84ab                                                         ; 8506: f0 a3       ..
    bne loop_c84ee                                                    ; 8508: d0 e4       ..             ; ALWAYS branch

; ***************************************************************************************
.quit_cmd
    jsr sub_c8e54                                                     ; 850a: 20 54 8e     T.
; &850d referenced 2 times by &84a8, &8501
.c850d
    lda #0                                                            ; 850d: a9 00       ..
    sta l0041                                                         ; 850f: 85 41       .A
    sta l003c                                                         ; 8511: 85 3c       .<
    ldx #edit_input_file_handle                                       ; 8513: a2 6a       .j             ; X=address of ZP var containing handle
    jsr close_file_indirect                                           ; 8515: 20 85 8d     ..
    ldx #edit_output_file_handle                                      ; 8518: a2 6b       .k             ; X=address of ZP var containing handle
    jmp close_file_indirect                                           ; 851a: 4c 85 8d    L..

; ***************************************************************************************
.write_cmd
    jsr sub_c8e1f                                                     ; 851d: 20 1f 8e     ..
    jsr parse_marks_from_command                                      ; 8520: 20 89 89     ..
    jsr sanitise_area                                                 ; 8523: 20 5d 89     ].
    beq return_6                                                      ; 8526: f0 2f       ./
    lda #&80                                                          ; 8528: a9 80       ..
    jsr open_file                                                     ; 852a: 20 58 88     X.
    sta rw_file_handle                                                ; 852d: 85 4d       .M
    jsr sub_c8d51                                                     ; 852f: 20 51 8d     Q.
    jmp c85b0                                                         ; 8532: 4c b0 85    L..

; &8535 referenced 3 times by &84d8, &855e, &8ec1
.sub_c8535
    jsr compute_bytes_free                                            ; 8535: 20 be af     ..
    tya                                                               ; 8538: 98          .
    bne return_6                                                      ; 8539: d0 1c       ..
    cpx #&96                                                          ; 853b: e0 96       ..
    bcs return_6                                                      ; 853d: b0 18       ..
; &853f referenced 5 times by &830d, &85dc, &8794, &88ac, &96b5
.c853f
    jsr stop_printing                                                 ; 853f: 20 4b 84     K.
    jsr print_inline_string                                           ; 8542: 20 fa a7     ..
    equs "Not enough memory"                                          ; 8545: 4e 6f 74... Not
    equb &ff                                                          ; 8556: ff          .

; &8557 referenced 3 times by &8526, &8539, &853d
.return_6
    rts                                                               ; 8557: 60          `

; ***************************************************************************************
.read_cmd
    jsr sub_c8e1f                                                     ; 8558: 20 1f 8e     ..
    jsr parse_marks_from_command                                      ; 855b: 20 89 89     ..
    jsr sub_c8535                                                     ; 855e: 20 35 85     5.
    lda #&40 ; '@'                                                    ; 8561: a9 40       .@
    jsr open_file                                                     ; 8563: 20 58 88     X.
    sta rw_file_handle                                                ; 8566: 85 4d       .M
    lda area_start_ptr                                                ; 8568: a5 5f       ._
    ldy area_start_ptr+1                                              ; 856a: a4 60       .`
    sta tmp4                                                          ; 856c: 85 89       ..
    sty tmp5                                                          ; 856e: 84 8a       ..
    jsr cabcb                                                         ; 8570: 20 cb ab     ..
    lda tmp4                                                          ; 8573: a5 89       ..
    ldy tmp5                                                          ; 8575: a4 8a       ..
    jsr sub_c8d9a                                                     ; 8577: 20 9a 8d     ..
    jsr sub_caa15                                                     ; 857a: 20 15 aa     ..
    jsr c8c8d                                                         ; 857d: 20 8d 8c     ..
    beq c8584                                                         ; 8580: f0 02       ..
    bcs c8598                                                         ; 8582: b0 14       ..
; &8584 referenced 1 time by &8580
.c8584
    jsr print_inline_string                                           ; 8584: 20 fa a7     ..
    equs "Not all read in", &0d                                       ; 8587: 4e 6f 74... Not
    equb 0                                                            ; 8597: 00          .

; &8598 referenced 1 time by &8582
.c8598
    lda tmp0                                                          ; 8598: a5 85       ..
    sta tmp4                                                          ; 859a: 85 89       ..
    lda tmp1                                                          ; 859c: a5 86       ..
    sta tmp5                                                          ; 859e: 85 8a       ..
    lda ptr5                                                          ; 85a0: a5 15       ..
    sec                                                               ; 85a2: 38          8
    sbc tmp0                                                          ; 85a3: e5 85       ..
    sta tmp6                                                          ; 85a5: 85 8b       ..
    lda ptr5+1                                                        ; 85a7: a5 16       ..
    sbc tmp1                                                          ; 85a9: e5 86       ..
    sta tmp7                                                          ; 85ab: 85 8c       ..
    jsr sub_ca9b0                                                     ; 85ad: 20 b0 a9     ..
; &85b0 referenced 1 time by &8532
.c85b0
    jsr close_file                                                    ; 85b0: 20 8f 8d     ..
    jmp cli_loop                                                      ; 85b3: 4c f6 81    L..

; ***************************************************************************************
.mode_cmd
    jsr parse_integer_from_command                                    ; 85b6: 20 39 88     9.
    beq c85ec                                                         ; 85b9: f0 31       .1
    inx                                                               ; 85bb: e8          .
    dex                                                               ; 85bc: ca          .
    bne c85ec                                                         ; 85bd: d0 2d       .-
    sta l0084                                                         ; 85bf: 85 84       ..
    lda is_tube_flag                                                  ; 85c1: a5 52       .R
    ror a                                                             ; 85c3: 6a          j
    bcs c85df                                                         ; 85c4: b0 19       ..
    jsr check_for_bad_document                                        ; 85c6: 20 86 8e     ..
    bne c85df                                                         ; 85c9: d0 14       ..
    lda #osbyte_read_himem_for_mode                                   ; 85cb: a9 85       ..
    ldx l0084                                                         ; 85cd: a6 84       ..             ; X=MODE number
    jsr osbyte                                                        ; 85cf: 20 f4 ff     ..            ; Read top of user memory for a given screen mode X
    cpy top+1                                                         ; 85d2: c4 0e       ..             ; X and Y contain the address (low, high)
    bcc c85dc                                                         ; 85d4: 90 06       ..
    bne c85df                                                         ; 85d6: d0 07       ..
    cpx top                                                           ; 85d8: e4 0d       ..
    bcs c85df                                                         ; 85da: b0 03       ..
; &85dc referenced 1 time by &85d4
.c85dc
    jmp c853f                                                         ; 85dc: 4c 3f 85    L?.

; &85df referenced 4 times by &85c4, &85c9, &85d6, &85da
.c85df
    lda #&16                                                          ; 85df: a9 16       ..
    jsr oswrch                                                        ; 85e1: 20 ee ff     ..            ; Write character 22
    lda l0084                                                         ; 85e4: a5 84       ..
    jsr oswrch                                                        ; 85e6: 20 ee ff     ..            ; Write character
    jmp system_init                                                   ; 85e9: 4c 79 af    Ly.

; &85ec referenced 2 times by &85b9, &85bd
.c85ec
    jsr print_inline_string                                           ; 85ec: 20 fa a7     ..
    equs "Bad mode"                                                   ; 85ef: 42 61 64... Bad
    equb &ff                                                          ; 85f7: ff          .

; ***************************************************************************************
.microspace_cmd
    jsr prepare_printer_driver                                        ; 85f8: 20 8b 94     ..
    jsr parse_integer_from_command                                    ; 85fb: 20 39 88     9.
    php                                                               ; 85fe: 08          .
    ldx #&0a                                                          ; 85ff: a2 0a       ..
    plp                                                               ; 8601: 28          (
    beq c8608                                                         ; 8602: f0 04       ..
    ldx tmp8                                                          ; 8604: a6 8d       ..
    beq return_7                                                      ; 8606: f0 0e       ..
; &8608 referenced 1 time by &8602
.c8608
    ldy #0                                                            ; 8608: a0 00       ..
    lda #&0c                                                          ; 860a: a9 0c       ..
    jsr call_printer_driver                                           ; 860c: 20 a3 94     ..
    tya                                                               ; 860f: 98          .
    and #1                                                            ; 8610: 29 01       ).
    beq c8617                                                         ; 8612: f0 03       ..
    stx microspacing_flag                                             ; 8614: 86 6c       .l
; &8616 referenced 1 time by &8606
.return_7
    rts                                                               ; 8616: 60          `

; &8617 referenced 1 time by &8612
.c8617
    jsr print_inline_string                                           ; 8617: 20 fa a7     ..
    equs "Driver does not support microspacing"                       ; 861a: 44 72 69... Dri
    equb &ff                                                          ; 863e: ff          .

; ***************************************************************************************
.setup_cmd
    ldx #1                                                            ; 863f: a2 01       ..
    stx tmp6                                                          ; 8641: 86 8b       ..
    dex                                                               ; 8643: ca          .              ; X=&00
    stx tmp8                                                          ; 8644: 86 8d       ..
    dex                                                               ; 8646: ca          .              ; X=&ff
    stx tmp7                                                          ; 8647: 86 8c       ..
; &8649 referenced 1 time by &8670
.c8649
    jsr sub_c8e33                                                     ; 8649: 20 33 8e     3.
    beq c8672                                                         ; 864c: f0 24       .$
    and #&df                                                          ; 864e: 29 df       ).
    ldx #0                                                            ; 8650: a2 00       ..
; &8652 referenced 1 time by &865b
.loop_c8652
    cmp c867d,x                                                       ; 8652: dd 7d 86    .}.
    beq c8669                                                         ; 8655: f0 12       ..
    inx                                                               ; 8657: e8          .
    ldy c867d,x                                                       ; 8658: bc 7d 86    .}.
    bne loop_c8652                                                    ; 865b: d0 f5       ..
    jsr print_inline_string                                           ; 865d: 20 fa a7     ..
    equs "Bad flag"                                                   ; 8660: 42 61 64... Bad
    equb &ff                                                          ; 8668: ff          .

; &8669 referenced 1 time by &8655
.c8669
    lda c8681,x                                                       ; 8669: bd 81 86    ...
    sta tmp6,x                                                        ; 866c: 95 8b       ..
    inc input_buffer_ptr                                              ; 866e: e6 7f       ..
    bne c8649                                                         ; 8670: d0 d7       ..
; &8672 referenced 1 time by &864c
.c8672
    ldx #2                                                            ; 8672: a2 02       ..
; &8674 referenced 1 time by &8679
.loop_c8674
    lda tmp6,x                                                        ; 8674: b5 8b       ..
    sta format_mode_flag,x                                            ; 8676: 95 4f       .O
    dex                                                               ; 8678: ca          .
    bpl loop_c8674                                                    ; 8679: 10 f9       ..
    bmi c869b                                                         ; 867b: 30 1e       0.             ; ALWAYS branch

; &867d referenced 2 times by &8652, &8658
.c867d
    lsr l004a                                                         ; 867d: 46 4a       FJ
    eor #0                                                            ; 867f: 49 00       I.
; &8681 referenced 1 time by &8669
.c8681
    brk                                                               ; 8681: 00          .

    equb 0, &ff                                                       ; 8682: 00 ff       ..

; ***************************************************************************************
.field_cmd
    jsr parse_integer_from_command                                    ; 8684: 20 39 88     9.
    beq c869b                                                         ; 8687: f0 12       ..
    lda tmp8                                                          ; 8689: a5 8d       ..
    cmp #&1b                                                          ; 868b: c9 1b       ..
    bne c8699                                                         ; 868d: d0 0a       ..
    jsr print_inline_string                                           ; 868f: 20 fa a7     ..
    equs "Frump!"                                                     ; 8692: 46 72 75... Fru
    equb &ff                                                          ; 8698: ff          .

; &8699 referenced 1 time by &868d
.c8699
    sta current_tab_key                                               ; 8699: 85 7b       .{
; &869b referenced 3 times by &867b, &8687, &86a4
.c869b
    jmp cli_loop                                                      ; 869b: 4c f6 81    L..

; ***************************************************************************************
.count_cmd
    jsr parse_marks_from_command                                      ; 869e: 20 89 89     ..
    jsr sanitise_area                                                 ; 86a1: 20 5d 89     ].
    beq c869b                                                         ; 86a4: f0 f5       ..
    lda area_start_ptr                                                ; 86a6: a5 5f       ._
    sta tmp0                                                          ; 86a8: 85 85       ..
    lda area_start_ptr+1                                              ; 86aa: a5 60       .`
    sta tmp1                                                          ; 86ac: 85 86       ..
    lda #0                                                            ; 86ae: a9 00       ..
    sta tmp8                                                          ; 86b0: 85 8d       ..
    sta tmp9                                                          ; 86b2: 85 8e       ..
    sta l0083                                                         ; 86b4: 85 83       ..
    sta l0082                                                         ; 86b6: 85 82       ..
; &86b8 referenced 2 times by &8723, &8729
.c86b8
    ldy #0                                                            ; 86b8: a0 00       ..
    jsr sub_caf6f                                                     ; 86ba: 20 6f af     o.
    bne c86ea                                                         ; 86bd: d0 2b       .+
    ldx #0                                                            ; 86bf: a2 00       ..
    iny                                                               ; 86c1: c8          .
; &86c2 referenced 1 time by &86d9
.loop_c86c2
    lda (tmp0),y                                                      ; 86c2: b1 85       ..
    iny                                                               ; 86c4: c8          .
    cmp l8747,x                                                       ; 86c5: dd 47 87    .G.
    bne c86d1                                                         ; 86c8: d0 07       ..
    lda (tmp0),y                                                      ; 86ca: b1 85       ..
    cmp l8748,x                                                       ; 86cc: dd 48 87    .H.
    beq c86df                                                         ; 86cf: f0 0e       ..
; &86d1 referenced 1 time by &86c8
.c86d1
    lda l8749,x                                                       ; 86d1: bd 49 87    .I.
    beq c86db                                                         ; 86d4: f0 05       ..
    dey                                                               ; 86d6: 88          .
    inx                                                               ; 86d7: e8          .
    inx                                                               ; 86d8: e8          .
    bne loop_c86c2                                                    ; 86d9: d0 e7       ..
; &86db referenced 1 time by &86d4
.c86db
    lda #&80                                                          ; 86db: a9 80       ..
    bne c86ff                                                         ; 86dd: d0 20       .              ; ALWAYS branch

; &86df referenced 1 time by &86cf
.c86df
    lda tmp0                                                          ; 86df: a5 85       ..
    clc                                                               ; 86e1: 18          .
    adc #3                                                            ; 86e2: 69 03       i.
    sta tmp0                                                          ; 86e4: 85 85       ..
    bcs c871d                                                         ; 86e6: b0 35       .5
    bcc c871f                                                         ; 86e8: 90 35       .5             ; ALWAYS branch

; &86ea referenced 1 time by &86bd
.c86ea
    ldy #0                                                            ; 86ea: a0 00       ..
    jsr sub_ca5ab                                                     ; 86ec: 20 ab a5     ..
    and #&7f                                                          ; 86ef: 29 7f       ).
    ldx #0                                                            ; 86f1: a2 00       ..
    ldy l0082                                                         ; 86f3: a4 82       ..
    bmi c870d                                                         ; 86f5: 30 16       0.
    cmp #&0d                                                          ; 86f7: c9 0d       ..
    beq c8703                                                         ; 86f9: f0 08       ..
    cmp #&20 ; ' '                                                    ; 86fb: c9 20       .
    beq c8703                                                         ; 86fd: f0 04       ..
; &86ff referenced 1 time by &86dd
.c86ff
    inc l0083                                                         ; 86ff: e6 83       ..
    bne c8715                                                         ; 8701: d0 12       ..
; &8703 referenced 2 times by &86f9, &86fd
.c8703
    ldy l0083                                                         ; 8703: a4 83       ..
    beq c870d                                                         ; 8705: f0 06       ..
    inc tmp8                                                          ; 8707: e6 8d       ..
    bne c870d                                                         ; 8709: d0 02       ..
    inc tmp9                                                          ; 870b: e6 8e       ..
; &870d referenced 3 times by &86f5, &8705, &8709
.c870d
    stx l0083                                                         ; 870d: 86 83       ..
    cmp #&0d                                                          ; 870f: c9 0d       ..
    bne c8715                                                         ; 8711: d0 02       ..
    stx l0082                                                         ; 8713: 86 82       ..
; &8715 referenced 2 times by &8701, &8711
.c8715
    ora l0082                                                         ; 8715: 05 82       ..
    sta l0082                                                         ; 8717: 85 82       ..
    inc tmp0                                                          ; 8719: e6 85       ..
    bne c871f                                                         ; 871b: d0 02       ..
; &871d referenced 1 time by &86e6
.c871d
    inc tmp1                                                          ; 871d: e6 86       ..
; &871f referenced 2 times by &86e8, &871b
.c871f
    ldy tmp1                                                          ; 871f: a4 86       ..
    cpy area_end_ptr+1                                                ; 8721: c4 62       .b
    bne c86b8                                                         ; 8723: d0 93       ..
    ldy tmp0                                                          ; 8725: a4 85       ..
    cpy area_end_ptr                                                  ; 8727: c4 61       .a
    bne c86b8                                                         ; 8729: d0 8d       ..
    ldx tmp8                                                          ; 872b: a6 8d       ..
    ldy tmp9                                                          ; 872d: a4 8e       ..
    jsr render_number_to_screen                                       ; 872f: 20 b2 a6     ..
    jsr print_inline_string                                           ; 8732: 20 fa a7     ..
    equs " word(s) counted."                                          ; 8735: 20 77 6f...  wo
    equb &ff                                                          ; 8746: ff          .
; &8747 referenced 1 time by &86c5
.l8747
    equb &52                                                          ; 8747: 52          R
; &8748 referenced 1 time by &86cc
.l8748
    equb &4a                                                          ; 8748: 4a          J
; &8749 referenced 1 time by &86d1
.l8749
    equs "CELJ"                                                       ; 8749: 43 45 4c... CEL
    equb 0                                                            ; 874d: 00          .

; ***************************************************************************************
.format_cmd
    jsr parse_marks_from_command                                      ; 874e: 20 89 89     ..
    jsr sanitise_area                                                 ; 8751: 20 5d 89     ].
    beq c878b                                                         ; 8754: f0 35       .5
    lda area_start_ptr                                                ; 8756: a5 5f       ._
    ldy area_start_ptr+1                                              ; 8758: a4 60       .`
    jsr cabcb                                                         ; 875a: 20 cb ab     ..
    jsr sub_caf5f                                                     ; 875d: 20 5f af     _.
    lda #&10                                                          ; 8760: a9 10       ..
    jsr sub_cab0f                                                     ; 8762: 20 0f ab     ..
    lda current_edit_line_ptr                                         ; 8765: a5 02       ..
    sta current_format_line_ptr                                       ; 8767: 85 04       ..
    lda current_edit_line_ptr+1                                       ; 8769: a5 03       ..
    sta current_format_line_ptr+1                                     ; 876b: 85 05       ..
; &876d referenced 2 times by &877f, &8785
.c876d
    jsr sub_c9977                                                     ; 876d: 20 77 99     w.
    bvs c8791                                                         ; 8770: 70 1f       p.
    bcs c8787                                                         ; 8772: b0 13       ..
    lda #&2e ; '.'                                                    ; 8774: a9 2e       ..
    jsr oswrch                                                        ; 8776: 20 ee ff     ..            ; Write character 46
    lda ptr4                                                          ; 8779: a5 08       ..
    ldy ptr4+1                                                        ; 877b: a4 09       ..
    cpy area_end_ptr+1                                                ; 877d: c4 62       .b
    bcc c876d                                                         ; 877f: 90 ec       ..
    bne c8787                                                         ; 8781: d0 04       ..
    cmp area_end_ptr                                                  ; 8783: c5 61       .a
    bcc c876d                                                         ; 8785: 90 e6       ..
; &8787 referenced 2 times by &8772, &8781
.c8787
    lda #&ff                                                          ; 8787: a9 ff       ..
    sta l0012                                                         ; 8789: 85 12       ..
; &878b referenced 1 time by &8754
.c878b
    jsr osnewl                                                        ; 878b: 20 e7 ff     ..            ; Write newline (characters 10 and 13)
    jmp cli_loop                                                      ; 878e: 4c f6 81    L..

; &8791 referenced 1 time by &8770
.c8791
    jsr osnewl                                                        ; 8791: 20 e7 ff     ..            ; Write newline (characters 10 and 13)
    jmp c853f                                                         ; 8794: 4c 3f 85    L?.

; ***************************************************************************************
.new_cmd
    jsr sub_c8e49                                                     ; 8797: 20 49 8e     I.
    jmp initialise_document                                           ; 879a: 4c cf af    L..

; ***************************************************************************************
.fold_cmd
    jsr sub_c8e33                                                     ; 879d: 20 33 8e     3.
    beq c87b4                                                         ; 87a0: f0 12       ..
    lda #0                                                            ; 87a2: a9 00       ..
    ldx #5                                                            ; 87a4: a2 05       ..
    jsr sub_c976c                                                     ; 87a6: 20 6c 97     l.
    bcs c87b4                                                         ; 87a9: b0 09       ..
    ldx #&80                                                          ; 87ab: a2 80       ..
    tay                                                               ; 87ad: a8          .
    beq c87b2                                                         ; 87ae: f0 02       ..
    ldx #0                                                            ; 87b0: a2 00       ..
; &87b2 referenced 1 time by &87ae
.c87b2
    stx folding_flag                                                  ; 87b2: 86 7c       .|
; &87b4 referenced 2 times by &87a0, &87a9
.c87b4
    jsr print_inline_string                                           ; 87b4: 20 fa a7     ..
    equs "Folding "                                                   ; 87b7: 46 6f 6c... Fol
    equb 0                                                            ; 87bf: 00          .

    lda folding_flag                                                  ; 87c0: a5 7c       .|
    bpl c87cb                                                         ; 87c2: 10 07       ..
    jsr print_inline_string                                           ; 87c4: 20 fa a7     ..
    equs "off"                                                        ; 87c7: 6f 66 66    off
    equb &ff                                                          ; 87ca: ff          .

; &87cb referenced 1 time by &87c2
.c87cb
    jsr print_inline_string                                           ; 87cb: 20 fa a7     ..
    equs "on"                                                         ; 87ce: 6f 6e       on
    equb &ff                                                          ; 87d0: ff          .

; &87d1 referenced 4 times by &87ea, &87f2, &87fa, &87ff
.c87d1
    jsr print_inline_string                                           ; 87d1: 20 fa a7     ..
    equs "Bad file"                                                   ; 87d4: 42 61 64... Bad
    equb &ff                                                          ; 87dc: ff          .

; ***************************************************************************************
.printer_cmd
    jsr sub_c8df4                                                     ; 87dd: 20 f4 8d     ..
    beq c882f                                                         ; 87e0: f0 4d       .M
    jsr test_for_cassette_filesystem                                  ; 87e2: 20 7d 8e     }.
    bcc c8801                                                         ; 87e5: 90 1a       ..
    jsr sub_c8849                                                     ; 87e7: 20 49 88     I.
    beq c87d1                                                         ; 87ea: f0 e5       ..
    lda l050c                                                         ; 87ec: ad 0c 05    ...
    ora l050d                                                         ; 87ef: 0d 0d 05    ...
    bne c87d1                                                         ; 87f2: d0 dd       ..
    ldy l050b                                                         ; 87f4: ac 0b 05    ...
    dey                                                               ; 87f7: 88          .
    bmi c8801                                                         ; 87f8: 30 07       0.
    bne c87d1                                                         ; 87fa: d0 d5       ..
    lda l050a                                                         ; 87fc: ad 0a 05    ...
    bne c87d1                                                         ; 87ff: d0 d0       ..
; &8801 referenced 2 times by &87e5, &87f8
.c8801
    lda #<printer_driver_block                                        ; 8801: a9 00       ..
    sta l0502                                                         ; 8803: 8d 02 05    ...
    lda #>printer_driver_block                                        ; 8806: a9 04       ..
    sta l0503                                                         ; 8808: 8d 03 05    ...
    lda #osbyte_read_high_order_address                               ; 880b: a9 82       ..
    jsr osbyte                                                        ; 880d: 20 f4 ff     ..            ; Read the filing system 'machine high order address'
    stx l0504                                                         ; 8810: 8e 04 05    ...            ; X and Y contain the machine high order address (low, high)
    sty l0505                                                         ; 8813: 8c 05 05    ...
    lda #0                                                            ; 8816: a9 00       ..
    sta l0506                                                         ; 8818: 8d 06 05    ...
    lda #&ff                                                          ; 881b: a9 ff       ..
    jsr do_osfile_with_block                                          ; 881d: 20 4c 89     L.
    ldx #0                                                            ; 8820: a2 00       ..
; &8822 referenced 1 time by &882b
.loop_c8822
    lda filename,x                                                    ; 8822: bd 5c 07    .\.
    sta printer_driver_name,x                                         ; 8825: 9d 84 07    ...
    inx                                                               ; 8828: e8          .
    cmp #&0d                                                          ; 8829: c9 0d       ..
    bne loop_c8822                                                    ; 882b: d0 f5       ..
    beq c8834                                                         ; 882d: f0 05       ..             ; ALWAYS branch

; &882f referenced 1 time by &87e0
.c882f
    lda #0                                                            ; 882f: a9 00       ..
    sta printer_driver_name                                           ; 8831: 8d 84 07    ...
; &8834 referenced 1 time by &882d
.c8834
    lda #0                                                            ; 8834: a9 00       ..
    sta microspacing_flag                                             ; 8836: 85 6c       .l
; &8838 referenced 3 times by &8844, &8860, &8886
.return_8
    rts                                                               ; 8838: 60          `

; ***************************************************************************************
; &8839 referenced 3 times by &85b6, &85fb, &8684
.parse_integer_from_command
    lda #<(input_buffer)                                              ; 8839: a9 00       ..
    sta current_format_line_ptr                                       ; 883b: 85 04       ..
    lda #>(input_buffer)                                              ; 883d: a9 05       ..
    sta current_format_line_ptr+1                                     ; 883f: 85 05       ..
    jsr sub_c8e33                                                     ; 8841: 20 33 8e     3.
    beq return_8                                                      ; 8844: f0 f2       ..
    jmp ca6fe                                                         ; 8846: 4c fe a6    L..

; &8849 referenced 2 times by &87e7, &8883
.sub_c8849
    lda #5                                                            ; 8849: a9 05       ..
    jsr do_osfile_with_block                                          ; 884b: 20 4c 89     L.
    tay                                                               ; 884e: a8          .
    beq c8862                                                         ; 884f: f0 11       ..
    lda l050a                                                         ; 8851: ad 0a 05    ...
    ora l050b                                                         ; 8854: 0d 0b 05    ...
    rts                                                               ; 8857: 60          `

; ***************************************************************************************
; &8858 referenced 5 times by &847a, &8487, &852a, &8563, &8f2b
.open_file
    ldy #>(filename)                                                  ; 8858: a0 07       ..
    ldx #<(filename)                                                  ; 885a: a2 5c       .\
    jsr osfind                                                        ; 885c: 20 ce ff     ..            ; Open or close file(s)
    tay                                                               ; 885f: a8          .
    bne return_8                                                      ; 8860: d0 d6       ..
; &8862 referenced 1 time by &884f
.c8862
    jsr stop_printing                                                 ; 8862: 20 4b 84     K.
    jsr print_inline_string                                           ; 8865: 20 fa a7     ..
    equs "File not found"                                             ; 8868: 46 69 6c... Fil
    equb &ff                                                          ; 8876: ff          .

; ***************************************************************************************
.load_cmd
    jsr sub_c8e49                                                     ; 8877: 20 49 8e     I.
    jsr error_if_cassette_filesystem                                  ; 887a: 20 63 8e     c.
    jsr initialise_document_if_document_bad                           ; 887d: 20 ca af     ..
    jsr sub_c8e1f                                                     ; 8880: 20 1f 8e     ..
    jsr sub_c8849                                                     ; 8883: 20 49 88     I.
    beq return_8                                                      ; 8886: f0 b0       ..
    lda l050c                                                         ; 8888: ad 0c 05    ...
    ora l050d                                                         ; 888b: 0d 0d 05    ...
    bne c88ac                                                         ; 888e: d0 1c       ..
    lda l050a                                                         ; 8890: ad 0a 05    ...
    clc                                                               ; 8893: 18          .
    adc page                                                          ; 8894: 65 0b       e.
    sta tmp8                                                          ; 8896: 85 8d       ..
    tax                                                               ; 8898: aa          .
    lda l050b                                                         ; 8899: ad 0b 05    ...
    adc page+1                                                        ; 889c: 65 0c       e.
    sta tmp9                                                          ; 889e: 85 8e       ..
    bcs c88ac                                                         ; 88a0: b0 0a       ..
    cmp himem+1                                                       ; 88a2: c5 10       ..
    bcc c88af                                                         ; 88a4: 90 09       ..
    bne c88ac                                                         ; 88a6: d0 04       ..
    cpx himem                                                         ; 88a8: e4 0f       ..
    bcc c88af                                                         ; 88aa: 90 03       ..
; &88ac referenced 3 times by &888e, &88a0, &88a6
.c88ac
    jmp c853f                                                         ; 88ac: 4c 3f 85    L?.

; &88af referenced 2 times by &88a4, &88aa
.c88af
    lda #osbyte_read_high_order_address                               ; 88af: a9 82       ..
    jsr osbyte                                                        ; 88b1: 20 f4 ff     ..            ; Read the filing system 'machine high order address'
    stx l0504                                                         ; 88b4: 8e 04 05    ...            ; X and Y contain the machine high order address (low, high)
    sty l0505                                                         ; 88b7: 8c 05 05    ...
    lda page                                                          ; 88ba: a5 0b       ..
    sta l0502                                                         ; 88bc: 8d 02 05    ...
    lda page+1                                                        ; 88bf: a5 0c       ..
    sta l0503                                                         ; 88c1: 8d 03 05    ...
    lda #0                                                            ; 88c4: a9 00       ..
    sta l0506                                                         ; 88c6: 8d 06 05    ...
    lda #&ff                                                          ; 88c9: a9 ff       ..
    jsr sub_c8956                                                     ; 88cb: 20 56 89     V.
    lda tmp8                                                          ; 88ce: a5 8d       ..
    sta top                                                           ; 88d0: 85 0d       ..
    lda tmp9                                                          ; 88d2: a5 8e       ..
    sta top+1                                                         ; 88d4: 85 0e       ..
    ldy #0                                                            ; 88d6: a0 00       ..
    tya                                                               ; 88d8: 98          .              ; A=&00
    sta (top),y                                                       ; 88d9: 91 0d       ..
    sty xpos                                                          ; 88db: 84 40       .@
    jsr sub_c88f4                                                     ; 88dd: 20 f4 88     ..
    jsr clear_cmd                                                     ; 88e0: 20 91 b0     ..
    jmp cb07a                                                         ; 88e3: 4c 7a b0    Lz.

; ***************************************************************************************
.name_cmd
    jsr sub_c8e49                                                     ; 88e6: 20 49 8e     I.
    jsr sub_c8df4                                                     ; 88e9: 20 f4 8d     ..
    php                                                               ; 88ec: 08          .
    lda #0                                                            ; 88ed: a9 00       ..
    sta l003c                                                         ; 88ef: 85 3c       .<
    plp                                                               ; 88f1: 28          (
    beq return_9                                                      ; 88f2: f0 11       ..
; &88f4 referenced 1 time by &88dd
.sub_c88f4
    lda #&40 ; '@'                                                    ; 88f4: a9 40       .@
    sta l003c                                                         ; 88f6: 85 3c       .<
; &88f8 referenced 1 time by &847f
.sub_c88f8
    ldx #0                                                            ; 88f8: a2 00       ..
; &88fa referenced 1 time by &8903
.loop_c88fa
    lda filename,x                                                    ; 88fa: bd 5c 07    .\.
    sta document_filename,x                                           ; 88fd: 9d ec 07    ...
    inx                                                               ; 8900: e8          .
    cmp #&0d                                                          ; 8901: c9 0d       ..
    bne loop_c88fa                                                    ; 8903: d0 f5       ..
; &8905 referenced 1 time by &88f2
.return_9
    rts                                                               ; 8905: 60          `

; ***************************************************************************************
.save_cmd
    jsr sub_c8df4                                                     ; 8906: 20 f4 8d     ..
    bne c891f                                                         ; 8909: d0 14       ..
    bit l003c                                                         ; 890b: 24 3c       $<
    bvs c8912                                                         ; 890d: 70 03       p.
    jmp c8e0f                                                         ; 890f: 4c 0f 8e    L..

; &8912 referenced 1 time by &890d
.c8912
    ldx #0                                                            ; 8912: a2 00       ..
; &8914 referenced 1 time by &891d
.loop_c8914
    lda document_filename,x                                           ; 8914: bd ec 07    ...
    sta filename,x                                                    ; 8917: 9d 5c 07    .\.
    inx                                                               ; 891a: e8          .
    cmp #&0d                                                          ; 891b: c9 0d       ..
    bne loop_c8914                                                    ; 891d: d0 f5       ..
; &891f referenced 1 time by &8909
.c891f
    lda page                                                          ; 891f: a5 0b       ..
    sta l050a                                                         ; 8921: 8d 0a 05    ...
    lda page+1                                                        ; 8924: a5 0c       ..
    sta l050b                                                         ; 8926: 8d 0b 05    ...
    lda top                                                           ; 8929: a5 0d       ..
    sta l050e                                                         ; 892b: 8d 0e 05    ...
    lda top+1                                                         ; 892e: a5 0e       ..
    sta l050f                                                         ; 8930: 8d 0f 05    ...
    lda #osbyte_read_high_order_address                               ; 8933: a9 82       ..
    jsr osbyte                                                        ; 8935: 20 f4 ff     ..            ; Read the filing system 'machine high order address'
    stx l050c                                                         ; 8938: 8e 0c 05    ...            ; X and Y contain the machine high order address (low, high)
    sty l050d                                                         ; 893b: 8c 0d 05    ...
    stx l0510                                                         ; 893e: 8e 10 05    ...
    sty l0511                                                         ; 8941: 8c 11 05    ...
    lda #0                                                            ; 8944: a9 00       ..
    jsr do_osfile_with_block                                          ; 8946: 20 4c 89     L.
    jmp cli_loop                                                      ; 8949: 4c f6 81    L..

; ***************************************************************************************
; &894c referenced 3 times by &881d, &884b, &8946
.do_osfile_with_block
    ldx #<filename                                                    ; 894c: a2 5c       .\
    stx input_buffer                                                  ; 894e: 8e 00 05    ...
    ldy #>filename                                                    ; 8951: a0 07       ..
    sty l0501                                                         ; 8953: 8c 01 05    ...
; &8956 referenced 1 time by &88cb
.sub_c8956
    ldx #<(input_buffer)                                              ; 8956: a2 00       ..
    ldy #>(input_buffer)                                              ; 8958: a0 05       ..
    jmp osfile                                                        ; 895a: 4c dd ff    L..

; ***************************************************************************************
; &895d referenced 7 times by &828e, &8405, &8523, &86a1, &8751, &8d48, &ad3f
.sanitise_area
    lda area_start_ptr                                                ; 895d: a5 5f       ._
    ldx area_start_ptr+1                                              ; 895f: a6 60       .`
    cpx area_end_ptr+1                                                ; 8961: e4 62       .b
    bcc c8977                                                         ; 8963: 90 12       ..
    bne c896b                                                         ; 8965: d0 04       ..
    cmp area_end_ptr                                                  ; 8967: c5 61       .a
    bcc c8977                                                         ; 8969: 90 0c       ..
; &896b referenced 1 time by &8965
.c896b
    ldy area_end_ptr                                                  ; 896b: a4 61       .a
    sty area_start_ptr                                                ; 896d: 84 5f       ._
    ldy area_end_ptr+1                                                ; 896f: a4 62       .b
    sty area_start_ptr+1                                              ; 8971: 84 60       .`
    stx area_end_ptr+1                                                ; 8973: 86 62       .b
    sta area_end_ptr                                                  ; 8975: 85 61       .a
; &8977 referenced 2 times by &8963, &8969
.c8977
    lda area_end_ptr                                                  ; 8977: a5 61       .a
    sec                                                               ; 8979: 38          8
    sbc area_start_ptr                                                ; 897a: e5 5f       ._
    sta tmp6                                                          ; 897c: 85 8b       ..
    lda area_end_ptr+1                                                ; 897e: a5 62       .b
    sbc area_start_ptr+1                                              ; 8980: e5 60       .`
    sta tmp7                                                          ; 8982: 85 8c       ..
    bne return_10                                                     ; 8984: d0 02       ..
    lda tmp6                                                          ; 8986: a5 8b       ..
; &8988 referenced 1 time by &8984
.return_10
    rts                                                               ; 8988: 60          `

; ***************************************************************************************
; &8989 referenced 7 times by &828b, &8402, &84b1, &8520, &855b, &869e, &874e
.parse_marks_from_command
    jsr reset_area_to_entire_document                                 ; 8989: 20 fd ac     ..
    jsr parse_mark_from_command                                       ; 898c: 20 9f 89     ..
    beq return_11                                                     ; 898f: f0 0d       ..
    sta area_start_ptr                                                ; 8991: 85 5f       ._
    sty area_start_ptr+1                                              ; 8993: 84 60       .`
    jsr parse_mark_from_command                                       ; 8995: 20 9f 89     ..
    beq return_11                                                     ; 8998: f0 04       ..
    sta area_end_ptr                                                  ; 899a: 85 61       .a
    sty area_end_ptr+1                                                ; 899c: 84 62       .b
; &899e referenced 2 times by &898f, &8998
.return_11
    rts                                                               ; 899e: 60          `

; ***************************************************************************************
; &899f referenced 2 times by &898c, &8995
.parse_mark_from_command
    jsr sub_c8e33                                                     ; 899f: 20 33 8e     3.
    beq return_12                                                     ; 89a2: f0 0e       ..
    iny                                                               ; 89a4: c8          .
    sty input_buffer_ptr                                              ; 89a5: 84 7f       ..
    jsr lookup_marker                                                 ; 89a7: 20 ef ac     ..
    bcs c89b3                                                         ; 89aa: b0 07       ..
    beq c89c1                                                         ; 89ac: f0 13       ..
    lda markers_array,x                                               ; 89ae: b5 53       .S
    ldy markers_array+1,x                                             ; 89b0: b4 54       .T
; &89b2 referenced 1 time by &89a2
.return_12
    rts                                                               ; 89b2: 60          `

; &89b3 referenced 1 time by &89aa
.c89b3
    jsr print_inline_string                                           ; 89b3: 20 fa a7     ..
    equs "Bad marker"                                                 ; 89b6: 42 61 64... Bad
    equb &ff                                                          ; 89c0: ff          .

; &89c1 referenced 1 time by &89ac
.c89c1
    jsr print_inline_string                                           ; 89c1: 20 fa a7     ..
    equs "Marker not set"                                             ; 89c4: 4d 61 72... Mar
    equb &ff                                                          ; 89d2: ff          .

; &89d3 referenced 4 times by &84d2, &84f6, &a18b, &a1a7
.sub_c89d3
    lda area_start_ptr                                                ; 89d3: a5 5f       ._
    sta tmp4                                                          ; 89d5: 85 89       ..
    lda area_start_ptr+1                                              ; 89d7: a5 60       .`
    sta tmp5                                                          ; 89d9: 85 8a       ..
    jsr sub_ca9b0                                                     ; 89db: 20 b0 a9     ..
    lda tmp4                                                          ; 89de: a5 89       ..
    ldy tmp5                                                          ; 89e0: a4 8a       ..
    jmp cac78                                                         ; 89e2: 4c 78 ac    Lx.

; &89e5 referenced 2 times by &8139, &8e5d
.sub_c89e5
    jsr stop_printing                                                 ; 89e5: 20 4b 84     K.
    jsr print_inline_string                                           ; 89e8: 20 fa a7     ..
    equs "Editing "                                                   ; 89eb: 45 64 69... Edi
    equb 0                                                            ; 89f3: 00          .

    lda l003c                                                         ; 89f4: a5 3c       .<
    beq c8a21                                                         ; 89f6: f0 29       .)
    ldy #0                                                            ; 89f8: a0 00       ..
; &89fa referenced 1 time by &8a05
.loop_c89fa
    lda document_filename,y                                           ; 89fa: b9 ec 07    ...
    cmp #&0d                                                          ; 89fd: c9 0d       ..
    beq c8a07                                                         ; 89ff: f0 06       ..
    jsr osasci                                                        ; 8a01: 20 e3 ff     ..            ; Write character
    iny                                                               ; 8a04: c8          .
    bne loop_c89fa                                                    ; 8a05: d0 f3       ..
; &8a07 referenced 1 time by &89ff
.c8a07
    bit l003c                                                         ; 8a07: 24 3c       $<
    bvs c8a19                                                         ; 8a09: 70 0e       p.
    jsr print_inline_string                                           ; 8a0b: 20 fa a7     ..
    equs " to "                                                       ; 8a0e: 20 74 6f...  to
    equb 0                                                            ; 8a12: 00          .

    ldy #0                                                            ; 8a13: a0 00       ..
; &8a15 referenced 1 time by &8a1e
.loop_c8a15
    lda another_filename,y                                            ; 8a15: b9 70 07    .p.
    iny                                                               ; 8a18: c8          .
; &8a19 referenced 1 time by &8a09
.c8a19
    jsr osasci                                                        ; 8a19: 20 e3 ff     ..            ; Write character
    cmp #&0d                                                          ; 8a1c: c9 0d       ..
    bne loop_c8a15                                                    ; 8a1e: d0 f5       ..
    rts                                                               ; 8a20: 60          `

; &8a21 referenced 1 time by &89f6
.c8a21
    jsr print_inline_string                                           ; 8a21: 20 fa a7     ..
    equs "No File", &0d                                               ; 8a24: 4e 6f 20... No
    equb 0                                                            ; 8a2c: 00          .

    rts                                                               ; 8a2d: 60          `

; ***************************************************************************************
.brk_handler
    jsr stop_printing                                                 ; 8a2e: 20 4b 84     K.
    jsr osnewl                                                        ; 8a31: 20 e7 ff     ..            ; Write newline (characters 10 and 13)
    ldy #1                                                            ; 8a34: a0 01       ..
; &8a36 referenced 1 time by &8a3e
.loop_c8a36
    lda (l00fd),y                                                     ; 8a36: b1 fd       ..
    beq c8a40                                                         ; 8a38: f0 06       ..
    jsr osasci                                                        ; 8a3a: 20 e3 ff     ..            ; Write character
    iny                                                               ; 8a3d: c8          .
    bne loop_c8a36                                                    ; 8a3e: d0 f6       ..
; &8a40 referenced 1 time by &8a38
.c8a40
    jsr osnewl                                                        ; 8a40: 20 e7 ff     ..            ; Write newline (characters 10 and 13)
    lda error_handling_mode                                           ; 8a43: a5 4e       .N
    bmi c8a4c                                                         ; 8a45: 30 05       0.
    ldy #6                                                            ; 8a47: a0 06       ..
    jmp call_through_jumptable                                        ; 8a49: 4c 92 a8    L..

; &8a4c referenced 1 time by &8a45
.c8a4c
    jmp cli_loop                                                      ; 8a4c: 4c f6 81    L..

; &8a4f referenced 2 times by &82c0, &834e
.sub_c8a4f
    lda #0                                                            ; 8a4f: a9 00       ..
    sta l0082                                                         ; 8a51: 85 82       ..
    sta l0081                                                         ; 8a53: 85 81       ..
    ldy #&14                                                          ; 8a55: a0 14       ..
    ldx l007a                                                         ; 8a57: a6 7a       .z
    bne c8a87                                                         ; 8a59: d0 2c       .,
; &8a5b referenced 1 time by &8a89
.c8a5b
    lda header_text_maybe,x                                           ; 8a5b: bd d8 06    ...
    cmp #1                                                            ; 8a5e: c9 01       ..
    bne c8a6c                                                         ; 8a60: d0 0a       ..
    lda l0081                                                         ; 8a62: a5 81       ..
    cmp l0049                                                         ; 8a64: c5 49       .I
    bcs c8a86                                                         ; 8a66: b0 1e       ..
    inc l0081                                                         ; 8a68: e6 81       ..
    bne c8a84                                                         ; 8a6a: d0 18       ..
; &8a6c referenced 1 time by &8a60
.c8a6c
    cmp #&20 ; ' '                                                    ; 8a6c: c9 20       .
    bne c8a84                                                         ; 8a6e: d0 14       ..
    cpy l0048                                                         ; 8a70: c4 48       .H
    bcs c8a84                                                         ; 8a72: b0 10       ..
; &8a74 referenced 1 time by &8a80
.loop_c8a74
    lda output_buffer,y                                               ; 8a74: b9 54 06    .T.
    php                                                               ; 8a77: 08          .
    iny                                                               ; 8a78: c8          .
    plp                                                               ; 8a79: 28          (
    beq c8a86                                                         ; 8a7a: f0 0a       ..
    inc l0082                                                         ; 8a7c: e6 82       ..
    cpy l0048                                                         ; 8a7e: c4 48       .H
    bcc loop_c8a74                                                    ; 8a80: 90 f2       ..
    dec l0082                                                         ; 8a82: c6 82       ..
; &8a84 referenced 3 times by &8a6a, &8a6e, &8a72
.c8a84
    inc l0082                                                         ; 8a84: e6 82       ..
; &8a86 referenced 2 times by &8a66, &8a7a
.c8a86
    inx                                                               ; 8a86: e8          .
; &8a87 referenced 1 time by &8a59
.c8a87
    cpx l004a                                                         ; 8a87: e4 4a       .J
    bcc c8a5b                                                         ; 8a89: 90 d0       ..
    lda l0065                                                         ; 8a8b: a5 65       .e
    sec                                                               ; 8a8d: 38          8
    sbc ptr2                                                          ; 8a8e: e5 4b       .K
    sta input_buffer_ptr+1                                            ; 8a90: 85 80       ..
    lda l0066                                                         ; 8a92: a5 66       .f
    sbc ptr2+1                                                        ; 8a94: e5 4c       .L
    sta l0081                                                         ; 8a96: 85 81       ..
    ldx l0082                                                         ; 8a98: a6 82       ..
    tay                                                               ; 8a9a: a8          .
    bne c8aa3                                                         ; 8a9b: d0 06       ..
    cpx input_buffer_ptr+1                                            ; 8a9d: e4 80       ..
    bcc c8aa3                                                         ; 8a9f: 90 02       ..
    ldx input_buffer_ptr+1                                            ; 8aa1: a6 80       ..
; &8aa3 referenced 2 times by &8a9b, &8a9f
.c8aa3
    txa                                                               ; 8aa3: 8a          .
    clc                                                               ; 8aa4: 18          .
    adc ptr2                                                          ; 8aa5: 65 4b       eK
    sta tmp4                                                          ; 8aa7: 85 89       ..
    lda ptr2+1                                                        ; 8aa9: a5 4c       .L
    adc #0                                                            ; 8aab: 69 00       i.
    sta tmp5                                                          ; 8aad: 85 8a       ..
    lda l0082                                                         ; 8aaf: a5 82       ..
    sec                                                               ; 8ab1: 38          8
    sbc input_buffer_ptr+1                                            ; 8ab2: e5 80       ..
    sta tmp6                                                          ; 8ab4: 85 8b       ..
    lda #0                                                            ; 8ab6: a9 00       ..
    sbc l0081                                                         ; 8ab8: e5 81       ..
    sta tmp7                                                          ; 8aba: 85 8c       ..
    bmi c8aca                                                         ; 8abc: 30 0c       0.
    ora tmp6                                                          ; 8abe: 05 8b       ..
    beq c8ada                                                         ; 8ac0: f0 18       ..
    sta tmp6                                                          ; 8ac2: 85 8b       ..
    jsr sub_caa15                                                     ; 8ac4: 20 15 aa     ..
    bcc c8ada                                                         ; 8ac7: 90 11       ..
    rts                                                               ; 8ac9: 60          `

; &8aca referenced 1 time by &8abc
.c8aca
    lda #0                                                            ; 8aca: a9 00       ..
    sec                                                               ; 8acc: 38          8
    sbc tmp6                                                          ; 8acd: e5 8b       ..
    sta tmp6                                                          ; 8acf: 85 8b       ..
    lda #0                                                            ; 8ad1: a9 00       ..
    sbc tmp7                                                          ; 8ad3: e5 8c       ..
    sta tmp7                                                          ; 8ad5: 85 8c       ..
    jsr sub_ca9b0                                                     ; 8ad7: 20 b0 a9     ..
; &8ada referenced 2 times by &8ac0, &8ac7
.c8ada
    ldy #0                                                            ; 8ada: a0 00       ..
    sty l0081                                                         ; 8adc: 84 81       ..
    bit print_xpos                                                    ; 8ade: 24 78       $x
    bmi c8b11                                                         ; 8ae0: 30 2f       0/
    ldx input_buffer_ptr+1                                            ; 8ae2: a6 80       ..
; &8ae4 referenced 1 time by &8aef
.loop_c8ae4
    lda (ptr2),y                                                      ; 8ae4: b1 4b       .K
    iny                                                               ; 8ae6: c8          .
    jsr is_uppercase                                                  ; 8ae7: 20 6b 8c     k.
    bcc c8af3                                                         ; 8aea: 90 07       ..
    ror print_xpos                                                    ; 8aec: 66 78       fx
    dex                                                               ; 8aee: ca          .
    bne loop_c8ae4                                                    ; 8aef: d0 f3       ..
    beq c8b11                                                         ; 8af1: f0 1e       ..             ; ALWAYS branch

; &8af3 referenced 1 time by &8aea
.c8af3
    pha                                                               ; 8af3: 48          H
    lda #0                                                            ; 8af4: a9 00       ..
    sta print_xpos                                                    ; 8af6: 85 78       .x
    pla                                                               ; 8af8: 68          h
    and #&20 ; ' '                                                    ; 8af9: 29 20       )
    bne c8b11                                                         ; 8afb: d0 14       ..
    inc l0081                                                         ; 8afd: e6 81       ..
    dex                                                               ; 8aff: ca          .
    beq c8b0d                                                         ; 8b00: f0 0b       ..
    lda (ptr2),y                                                      ; 8b02: b1 4b       .K
    jsr is_uppercase                                                  ; 8b04: 20 6b 8c     k.
    bcs c8b11                                                         ; 8b07: b0 08       ..
    and #&20 ; ' '                                                    ; 8b09: 29 20       )
    bne c8b11                                                         ; 8b0b: d0 04       ..
; &8b0d referenced 1 time by &8b00
.c8b0d
    dec l0081                                                         ; 8b0d: c6 81       ..
    dec l0081                                                         ; 8b0f: c6 81       ..
; &8b11 referenced 5 times by &8ae0, &8af1, &8afb, &8b07, &8b0b
.c8b11
    ldx #0                                                            ; 8b11: a2 00       ..
    stx l0082                                                         ; 8b13: 86 82       ..
    stx l0083                                                         ; 8b15: 86 83       ..
    ldx #&14                                                          ; 8b17: a2 14       ..
    stx input_buffer_ptr+1                                            ; 8b19: 86 80       ..
    ldx l007a                                                         ; 8b1b: a6 7a       .z
    bne c8b6b                                                         ; 8b1d: d0 4c       .L
; &8b1f referenced 1 time by &8b6d
.c8b1f
    lda header_text_maybe,x                                           ; 8b1f: bd d8 06    ...
    stx l0084                                                         ; 8b22: 86 84       ..
    cmp #&20 ; ' '                                                    ; 8b24: c9 20       .
    bne c8b38                                                         ; 8b26: d0 10       ..
    ldy input_buffer_ptr+1                                            ; 8b28: a4 80       ..
    cpy l0048                                                         ; 8b2a: c4 48       .H
    bcs c8b47                                                         ; 8b2c: b0 19       ..
    inc input_buffer_ptr+1                                            ; 8b2e: e6 80       ..
    lda output_buffer,y                                               ; 8b30: b9 54 06    .T.
    beq c8b6a                                                         ; 8b33: f0 35       .5
    dex                                                               ; 8b35: ca          .
    bcc c8b47                                                         ; 8b36: 90 0f       ..             ; ALWAYS branch

; &8b38 referenced 1 time by &8b26
.c8b38
    cmp #1                                                            ; 8b38: c9 01       ..
    bne c8b47                                                         ; 8b3a: d0 0b       ..
    ldy l0082                                                         ; 8b3c: a4 82       ..
    cpy l0049                                                         ; 8b3e: c4 49       .I
    bcs c8b6a                                                         ; 8b40: b0 28       .(
    lda output_buffer,y                                               ; 8b42: b9 54 06    .T.
    inc l0082                                                         ; 8b45: e6 82       ..
; &8b47 referenced 3 times by &8b2c, &8b36, &8b3a
.c8b47
    cmp #2                                                            ; 8b47: c9 02       ..
    bne c8b4d                                                         ; 8b49: d0 02       ..
    lda #&20 ; ' '                                                    ; 8b4b: a9 20       .
; &8b4d referenced 1 time by &8b49
.c8b4d
    bit folding_flag                                                  ; 8b4d: 24 7c       $|
    bmi c8b64                                                         ; 8b4f: 30 13       0.
    ldy print_xpos                                                    ; 8b51: a4 78       .x
    bne c8b64                                                         ; 8b53: d0 0f       ..
    jsr is_uppercase                                                  ; 8b55: 20 6b 8c     k.
    bcs c8b64                                                         ; 8b58: b0 0a       ..
    ora #&20 ; ' '                                                    ; 8b5a: 09 20       .
    ldy l0081                                                         ; 8b5c: a4 81       ..
    beq c8b64                                                         ; 8b5e: f0 04       ..
    dec l0081                                                         ; 8b60: c6 81       ..
    and #&df                                                          ; 8b62: 29 df       ).
; &8b64 referenced 4 times by &8b4f, &8b53, &8b58, &8b5e
.c8b64
    ldy l0083                                                         ; 8b64: a4 83       ..
    sta (ptr2),y                                                      ; 8b66: 91 4b       .K
    inc l0083                                                         ; 8b68: e6 83       ..
; &8b6a referenced 2 times by &8b33, &8b40
.c8b6a
    inx                                                               ; 8b6a: e8          .
; &8b6b referenced 1 time by &8b1d
.c8b6b
    cpx l004a                                                         ; 8b6b: e4 4a       .J
    bcc c8b1f                                                         ; 8b6d: 90 b0       ..
    lda ptr2                                                          ; 8b6f: a5 4b       .K
    ldy ptr2+1                                                        ; 8b71: a4 4c       .L
    jsr cac78                                                         ; 8b73: 20 78 ac     x.
    clc                                                               ; 8b76: 18          .
    rts                                                               ; 8b77: 60          `

; &8b78 referenced 3 times by &8b7d, &8b97, &8b9d
.c8b78
    lda #&ff                                                          ; 8b78: a9 ff       ..
    rts                                                               ; 8b7a: 60          `

; &8b7b referenced 9 times by &8296, &82a8, &82c5, &8322, &8356, &8bb3, &8bdd, &8be1, &a26b
.c8b7b
    lda l007a                                                         ; 8b7b: a5 7a       .z
    beq c8b78                                                         ; 8b7d: f0 f9       ..
    lda #&14                                                          ; 8b7f: a9 14       ..
    sta l0048                                                         ; 8b81: 85 48       .H
    ldx #0                                                            ; 8b83: a2 00       ..
    stx l0049                                                         ; 8b85: 86 49       .I
    stx l0081                                                         ; 8b87: 86 81       ..
    lda l0065                                                         ; 8b89: a5 65       .e
    sta tmp8                                                          ; 8b8b: 85 8d       ..
    lda l0066                                                         ; 8b8d: a5 66       .f
    sta tmp9                                                          ; 8b8f: 85 8e       ..
; &8b91 referenced 1 time by &8c30
.c8b91
    lda tmp9                                                          ; 8b91: a5 8e       ..
    cmp l0068                                                         ; 8b93: c5 68       .h
    bcc c8b9f                                                         ; 8b95: 90 08       ..
    bne c8b78                                                         ; 8b97: d0 df       ..
    lda tmp8                                                          ; 8b99: a5 8d       ..
    cmp l0067                                                         ; 8b9b: c5 67       .g
    bcs c8b78                                                         ; 8b9d: b0 d9       ..
; &8b9f referenced 1 time by &8b95
.c8b9f
    ldy #0                                                            ; 8b9f: a0 00       ..
    lda (tmp8),y                                                      ; 8ba1: b1 8d       ..
    jsr sub_caf71                                                     ; 8ba3: 20 71 af     q.
    bne c8bb7                                                         ; 8ba6: d0 0f       ..
    lda tmp8                                                          ; 8ba8: a5 8d       ..
    clc                                                               ; 8baa: 18          .
    adc #3                                                            ; 8bab: 69 03       i.
    sta l0065                                                         ; 8bad: 85 65       .e
    lda tmp9                                                          ; 8baf: a5 8e       ..
    sta l0066                                                         ; 8bb1: 85 66       .f
    bcc c8b7b                                                         ; 8bb3: 90 c6       ..
    bcs c8bdf                                                         ; 8bb5: b0 28       .(             ; ALWAYS branch

; &8bb7 referenced 1 time by &8ba6
.c8bb7
    jsr sub_c8c5f                                                     ; 8bb7: 20 5f 8c     _.
    sta l0083                                                         ; 8bba: 85 83       ..
; &8bbc referenced 1 time by &8c1f
.c8bbc
    iny                                                               ; 8bbc: c8          .
    lda (tmp8),y                                                      ; 8bbd: b1 8d       ..
    beq c8bdb                                                         ; 8bbf: f0 1a       ..
    jsr sub_caf71                                                     ; 8bc1: 20 71 af     q.
    beq c8bdb                                                         ; 8bc4: f0 15       ..
    lda header_text_maybe,x                                           ; 8bc6: bd d8 06    ...
    cmp #&20 ; ' '                                                    ; 8bc9: c9 20       .
    beq c8bf7                                                         ; 8bcb: f0 2a       .*
    cmp #1                                                            ; 8bcd: c9 01       ..
    beq c8be3                                                         ; 8bcf: f0 12       ..
    cmp #2                                                            ; 8bd1: c9 02       ..
    bne c8bd7                                                         ; 8bd3: d0 02       ..
    lda #&20 ; ' '                                                    ; 8bd5: a9 20       .
; &8bd7 referenced 1 time by &8bd3
.c8bd7
    cmp l0083                                                         ; 8bd7: c5 83       ..
    beq c8c33                                                         ; 8bd9: f0 58       .X
; &8bdb referenced 3 times by &8bbf, &8bc4, &8c11
.c8bdb
    inc l0065                                                         ; 8bdb: e6 65       .e
    bne c8b7b                                                         ; 8bdd: d0 9c       ..
; &8bdf referenced 1 time by &8bb5
.c8bdf
    inc l0066                                                         ; 8bdf: e6 66       .f
    bne c8b7b                                                         ; 8be1: d0 98       ..
; &8be3 referenced 1 time by &8bcf
.c8be3
    lda l0083                                                         ; 8be3: a5 83       ..
    stx l0084                                                         ; 8be5: 86 84       ..
    ldx l0049                                                         ; 8be7: a6 49       .I
    cpx #&14                                                          ; 8be9: e0 14       ..
    bcs c8bf2                                                         ; 8beb: b0 05       ..
    sta output_buffer,x                                               ; 8bed: 9d 54 06    .T.
    inc l0049                                                         ; 8bf0: e6 49       .I
; &8bf2 referenced 1 time by &8beb
.c8bf2
    ldx l0084                                                         ; 8bf2: a6 84       ..
    jmp c8c33                                                         ; 8bf4: 4c 33 8c    L3.

; &8bf7 referenced 1 time by &8bcb
.c8bf7
    stx l0084                                                         ; 8bf7: 86 84       ..
    lda l0083                                                         ; 8bf9: a5 83       ..
    cmp #&20 ; ' '                                                    ; 8bfb: c9 20       .
    beq c8c23                                                         ; 8bfd: f0 24       .$
    cmp #9                                                            ; 8bff: c9 09       ..
    beq c8c23                                                         ; 8c01: f0 20       .
    cmp #&0b                                                          ; 8c03: c9 0b       ..
    beq c8c23                                                         ; 8c05: f0 1c       ..
    cmp #&1a                                                          ; 8c07: c9 1a       ..
    beq c8c23                                                         ; 8c09: f0 18       ..
    cmp #&0d                                                          ; 8c0b: c9 0d       ..
    beq c8c23                                                         ; 8c0d: f0 14       ..
    lda l0081                                                         ; 8c0f: a5 81       ..
    beq c8bdb                                                         ; 8c11: f0 c8       ..
    jsr sub_c8c51                                                     ; 8c13: 20 51 8c     Q.
    lda #0                                                            ; 8c16: a9 00       ..
    sta l0081                                                         ; 8c18: 85 81       ..
    ldx l0084                                                         ; 8c1a: a6 84       ..
    inx                                                               ; 8c1c: e8          .
    cpx l007a                                                         ; 8c1d: e4 7a       .z
    bcc c8bbc                                                         ; 8c1f: 90 9b       ..
    bcs c8c3e                                                         ; 8c21: b0 1b       ..             ; ALWAYS branch

; &8c23 referenced 5 times by &8bfd, &8c01, &8c05, &8c09, &8c0d
.c8c23
    jsr sub_c8c53                                                     ; 8c23: 20 53 8c     S.
    ldx l0084                                                         ; 8c26: a6 84       ..
    sta l0081                                                         ; 8c28: 85 81       ..
; &8c2a referenced 1 time by &8c36
.loop_c8c2a
    inc tmp8                                                          ; 8c2a: e6 8d       ..
    bne c8c30                                                         ; 8c2c: d0 02       ..
    inc tmp9                                                          ; 8c2e: e6 8e       ..
; &8c30 referenced 1 time by &8c2c
.c8c30
    jmp c8b91                                                         ; 8c30: 4c 91 8b    L..

; &8c33 referenced 2 times by &8bd9, &8bf4
.c8c33
    inx                                                               ; 8c33: e8          .
    cpx l007a                                                         ; 8c34: e4 7a       .z
    bcc loop_c8c2a                                                    ; 8c36: 90 f2       ..
    inc tmp8                                                          ; 8c38: e6 8d       ..
    bne c8c3e                                                         ; 8c3a: d0 02       ..
    inc tmp9                                                          ; 8c3c: e6 8e       ..
; &8c3e referenced 2 times by &8c21, &8c3a
.c8c3e
    lda l0065                                                         ; 8c3e: a5 65       .e
    ldy l0066                                                         ; 8c40: a4 66       .f
    ldx tmp8                                                          ; 8c42: a6 8d       ..
    stx l0065                                                         ; 8c44: 86 65       .e
    ldx tmp9                                                          ; 8c46: a6 8e       ..
    stx l0066                                                         ; 8c48: 86 66       .f
    sta ptr2                                                          ; 8c4a: 85 4b       .K
    sty ptr2+1                                                        ; 8c4c: 84 4c       .L
    ldx #0                                                            ; 8c4e: a2 00       ..
    rts                                                               ; 8c50: 60          `

; &8c51 referenced 1 time by &8c13
.sub_c8c51
    lda #0                                                            ; 8c51: a9 00       ..
; &8c53 referenced 1 time by &8c23
.sub_c8c53
    ldx l0048                                                         ; 8c53: a6 48       .H
    cpx #&84                                                          ; 8c55: e0 84       ..
    bcs return_13                                                     ; 8c57: b0 05       ..
    sta output_buffer,x                                               ; 8c59: 9d 54 06    .T.
    inc l0048                                                         ; 8c5c: e6 48       .H
; &8c5e referenced 1 time by &8c57
.return_13
    rts                                                               ; 8c5e: 60          `

; &8c5f referenced 2 times by &83ce, &8bb7
.sub_c8c5f
    bit folding_flag                                                  ; 8c5f: 24 7c       $|
    bmi return_14                                                     ; 8c61: 30 07       0.
; &8c63 referenced 2 times by &83b1, &978b
.sub_c8c63
    jsr is_uppercase                                                  ; 8c63: 20 6b 8c     k.
    bcs return_14                                                     ; 8c66: b0 02       ..
    and #&df                                                          ; 8c68: 29 df       ).
; &8c6a referenced 2 times by &8c61, &8c66
.return_14
    rts                                                               ; 8c6a: 60          `

; ***************************************************************************************
; &8c6b referenced 8 times by &8ae7, &8b04, &8b55, &8c63, &8f8b, &9684, &9d40, &ad69
.is_uppercase
    cmp #&41 ; 'A'                                                    ; 8c6b: c9 41       .A
    bcc c8c7a                                                         ; 8c6d: 90 0b       ..
    cmp #&5b ; '['                                                    ; 8c6f: c9 5b       .[
    bcc return_15                                                     ; 8c71: 90 06       ..
    cmp #&61 ; 'a'                                                    ; 8c73: c9 61       .a
    bcc c8c7a                                                         ; 8c75: 90 03       ..
    cmp #&7b ; '{'                                                    ; 8c77: c9 7b       .{
; &8c79 referenced 1 time by &8c71
.return_15
    rts                                                               ; 8c79: 60          `

; &8c7a referenced 2 times by &8c6d, &8c75
.c8c7a
    sec                                                               ; 8c7a: 38          8
    rts                                                               ; 8c7b: 60          `

; &8c7c referenced 2 times by &8293, &840b
.sub_c8c7c
    lda area_start_ptr                                                ; 8c7c: a5 5f       ._
    sta l0065                                                         ; 8c7e: 85 65       .e
    lda area_start_ptr+1                                              ; 8c80: a5 60       .`
    sta l0066                                                         ; 8c82: 85 66       .f
    lda area_end_ptr                                                  ; 8c84: a5 61       .a
    sta l0067                                                         ; 8c86: 85 67       .g
    lda area_end_ptr+1                                                ; 8c88: a5 62       .b
    sta l0068                                                         ; 8c8a: 85 68       .h
    rts                                                               ; 8c8c: 60          `

; &8c8d referenced 3 times by &857d, &8d2f, &9260
.c8c8d
    lda #0                                                            ; 8c8d: a9 00       ..
    sta error_handling_mode                                           ; 8c8f: 85 4e       .N
    sta l0083                                                         ; 8c91: 85 83       ..
    sta l0084                                                         ; 8c93: 85 84       ..
; &8c95 referenced 6 times by &8ca4, &8ca9, &8cc6, &8ce1, &8ce7, &8cef
.c8c95
    ldy rw_file_handle                                                ; 8c95: a4 4d       .M
    jsr get_byte_from_file                                            ; 8c97: 20 15 8d     ..
    ldy #0                                                            ; 8c9a: a0 00       ..
    bcs c8cf2                                                         ; 8c9c: b0 54       .T
    cmp #&7f                                                          ; 8c9e: c9 7f       ..
    bcc c8caf                                                         ; 8ca0: 90 0d       ..
    ldx l0084                                                         ; 8ca2: a6 84       ..
    bne c8c95                                                         ; 8ca4: d0 ef       ..
    jsr sub_caf71                                                     ; 8ca6: 20 71 af     q.
    bne c8c95                                                         ; 8ca9: d0 ea       ..
    ldx #&fd                                                          ; 8cab: a2 fd       ..
    stx l0083                                                         ; 8cad: 86 83       ..
; &8caf referenced 1 time by &8ca0
.c8caf
    cmp #&20 ; ' '                                                    ; 8caf: c9 20       .
    bcs c8cc8                                                         ; 8cb1: b0 15       ..
    jsr sub_ca600                                                     ; 8cb3: 20 00 a6     ..
    beq c8cc8                                                         ; 8cb6: f0 10       ..
    cmp #&1a                                                          ; 8cb8: c9 1a       ..
    beq c8cc8                                                         ; 8cba: f0 0c       ..
    cmp #&0d                                                          ; 8cbc: c9 0d       ..
    beq c8cc8                                                         ; 8cbe: f0 08       ..
    cmp #&0b                                                          ; 8cc0: c9 0b       ..
    beq c8cc8                                                         ; 8cc2: f0 04       ..
    cmp #9                                                            ; 8cc4: c9 09       ..
    bne c8c95                                                         ; 8cc6: d0 cd       ..
; &8cc8 referenced 5 times by &8cb1, &8cb6, &8cba, &8cbe, &8cc2
.c8cc8
    ldx #1                                                            ; 8cc8: a2 01       ..
    cmp #&0d                                                          ; 8cca: c9 0d       ..
    beq c8cdb                                                         ; 8ccc: f0 0d       ..
    dex                                                               ; 8cce: ca          .              ; X=&00
    ldy l0083                                                         ; 8ccf: a4 83       ..
    cpy #&84                                                          ; 8cd1: c0 84       ..
    bne c8cdb                                                         ; 8cd3: d0 06       ..
    pha                                                               ; 8cd5: 48          H
    jsr sub_c8cfe                                                     ; 8cd6: 20 fe 8c     ..
    pla                                                               ; 8cd9: 68          h
    inx                                                               ; 8cda: e8          .
; &8cdb referenced 2 times by &8ccc, &8cd3
.c8cdb
    inc l0083                                                         ; 8cdb: e6 83       ..
    jsr sub_c8d00                                                     ; 8cdd: 20 00 8d     ..
    txa                                                               ; 8ce0: 8a          .
    beq c8c95                                                         ; 8ce1: f0 b2       ..
    lda tmp1                                                          ; 8ce3: a5 86       ..
    cmp l0081                                                         ; 8ce5: c5 81       ..
    bcc c8c95                                                         ; 8ce7: 90 ac       ..
    bne c8cf1                                                         ; 8ce9: d0 06       ..
    lda tmp0                                                          ; 8ceb: a5 85       ..
    cmp input_buffer_ptr+1                                            ; 8ced: c5 80       ..
    bcc c8c95                                                         ; 8cef: 90 a4       ..
; &8cf1 referenced 1 time by &8ce9
.c8cf1
    clc                                                               ; 8cf1: 18          .
; &8cf2 referenced 1 time by &8c9c
.c8cf2
    php                                                               ; 8cf2: 08          .
    lda l0084                                                         ; 8cf3: a5 84       ..
    beq c8cfa                                                         ; 8cf5: f0 03       ..
    jsr sub_c8cfe                                                     ; 8cf7: 20 fe 8c     ..
; &8cfa referenced 1 time by &8cf5
.c8cfa
    plp                                                               ; 8cfa: 28          (
    lda l0082                                                         ; 8cfb: a5 82       ..
    rts                                                               ; 8cfd: 60          `

; &8cfe referenced 2 times by &8cd6, &8cf7
.sub_c8cfe
    lda #&0d                                                          ; 8cfe: a9 0d       ..
; &8d00 referenced 1 time by &8cdd
.sub_c8d00
    ldy #0                                                            ; 8d00: a0 00       ..
    sta (tmp0),y                                                      ; 8d02: 91 85       ..
    inc tmp0                                                          ; 8d04: e6 85       ..
    bne c8d0a                                                         ; 8d06: d0 02       ..
    inc tmp1                                                          ; 8d08: e6 86       ..
; &8d0a referenced 1 time by &8d06
.c8d0a
    sta l0084                                                         ; 8d0a: 85 84       ..
    cmp #&0d                                                          ; 8d0c: c9 0d       ..
    bne return_16                                                     ; 8d0e: d0 04       ..
    sty l0084                                                         ; 8d10: 84 84       ..
    sty l0083                                                         ; 8d12: 84 83       ..
; &8d14 referenced 1 time by &8d0e
.return_16
    rts                                                               ; 8d14: 60          `

; ***************************************************************************************
; &8d15 referenced 1 time by &8c97
.get_byte_from_file
    tsx                                                               ; 8d15: ba          .
    stx l0082                                                         ; 8d16: 86 82       ..
    jmp osbget                                                        ; 8d18: 4c d7 ff    L..            ; Read a single byte from an open file Y

.sub_c8d1b
    ldx l0082                                                         ; 8d1b: a6 82       ..
    txs                                                               ; 8d1d: 9a          .
    lda #0                                                            ; 8d1e: a9 00       ..
    sta l0082                                                         ; 8d20: 85 82       ..
    sec                                                               ; 8d22: 38          8
    rts                                                               ; 8d23: 60          `

; &8d24 referenced 2 times by &849e, &8503
.sub_c8d24
    lda page                                                          ; 8d24: a5 0b       ..
    ldy page+1                                                        ; 8d26: a4 0c       ..
; &8d28 referenced 1 time by &84e3
.sub_c8d28
    jsr sub_c8da2                                                     ; 8d28: 20 a2 8d     ..
    lda edit_input_file_handle                                        ; 8d2b: a5 6a       .j
    sta rw_file_handle                                                ; 8d2d: 85 4d       .M
    jsr c8c8d                                                         ; 8d2f: 20 8d 8c     ..
    php                                                               ; 8d32: 08          .
    beq c8d39                                                         ; 8d33: f0 04       ..
    bcc c8d39                                                         ; 8d35: 90 02       ..
    inc l0041                                                         ; 8d37: e6 41       .A
; &8d39 referenced 2 times by &8d33, &8d35
.c8d39
    lda #0                                                            ; 8d39: a9 00       ..
    tay                                                               ; 8d3b: a8          .              ; Y=&00
    sta (tmp0),y                                                      ; 8d3c: 91 85       ..
    lda tmp0                                                          ; 8d3e: a5 85       ..
    sta top                                                           ; 8d40: 85 0d       ..
    lda tmp1                                                          ; 8d42: a5 86       ..
    sta top+1                                                         ; 8d44: 85 0e       ..
    plp                                                               ; 8d46: 28          (
    rts                                                               ; 8d47: 60          `

; &8d48 referenced 2 times by &84bb, &84f1
.sub_c8d48
    jsr sanitise_area                                                 ; 8d48: 20 5d 89     ].
    beq return_17                                                     ; 8d4b: f0 2b       .+
    lda edit_output_file_handle                                       ; 8d4d: a5 6b       .k
    sta rw_file_handle                                                ; 8d4f: 85 4d       .M
; &8d51 referenced 1 time by &852f
.sub_c8d51
    lda area_start_ptr                                                ; 8d51: a5 5f       ._
    sta tmp8                                                          ; 8d53: 85 8d       ..
    lda area_start_ptr+1                                              ; 8d55: a5 60       .`
    sta tmp9                                                          ; 8d57: 85 8e       ..
    lda #1                                                            ; 8d59: a9 01       ..
    sta error_handling_mode                                           ; 8d5b: 85 4e       .N
; &8d5d referenced 2 times by &8d70, &8d76
.c8d5d
    ldy #0                                                            ; 8d5d: a0 00       ..
    lda (tmp8),y                                                      ; 8d5f: b1 8d       ..
    ldy rw_file_handle                                                ; 8d61: a4 4d       .M
    jsr put_byte_to_file                                              ; 8d63: 20 79 8d     y.
    inc tmp8                                                          ; 8d66: e6 8d       ..
    bne c8d6c                                                         ; 8d68: d0 02       ..
    inc tmp9                                                          ; 8d6a: e6 8e       ..
; &8d6c referenced 1 time by &8d68
.c8d6c
    lda tmp9                                                          ; 8d6c: a5 8e       ..
    cmp area_end_ptr+1                                                ; 8d6e: c5 62       .b
    bne c8d5d                                                         ; 8d70: d0 eb       ..
    lda tmp8                                                          ; 8d72: a5 8d       ..
    cmp area_end_ptr                                                  ; 8d74: c5 61       .a
    bne c8d5d                                                         ; 8d76: d0 e5       ..
; &8d78 referenced 2 times by &8d4b, &8d87
.return_17
    rts                                                               ; 8d78: 60          `

; ***************************************************************************************
; &8d79 referenced 1 time by &8d63
.put_byte_to_file
    tsx                                                               ; 8d79: ba          .
    stx l0084                                                         ; 8d7a: 86 84       ..
    jmp osbput                                                        ; 8d7c: 4c d4 ff    L..            ; Write a single byte A to an open file Y

.sub_c8d7f
    ldx l0084                                                         ; 8d7f: a6 84       ..
    inx                                                               ; 8d81: e8          .
    inx                                                               ; 8d82: e8          .
    txs                                                               ; 8d83: 9a          .
    rts                                                               ; 8d84: 60          `

; ***************************************************************************************
; On Entry:
;     X: address of ZP var containing handle
; ***************************************************************************************
; &8d85 referenced 2 times by &8515, &851a
.close_file_indirect
    lda 0,x                                                           ; 8d85: b5 00       ..
    beq return_17                                                     ; 8d87: f0 ef       ..
    sta rw_file_handle                                                ; 8d89: 85 4d       .M
    lda #0                                                            ; 8d8b: a9 00       ..
    sta 0,x                                                           ; 8d8d: 95 00       ..
; ***************************************************************************************
; &8d8f referenced 2 times by &85b0, &9030
.close_file
    lda #&ff                                                          ; 8d8f: a9 ff       ..
    sta error_handling_mode                                           ; 8d91: 85 4e       .N
    lda #osfind_close                                                 ; 8d93: a9 00       ..
    ldy rw_file_handle                                                ; 8d95: a4 4d       .M
    jmp osfind                                                        ; 8d97: 4c ce ff    L..            ; Close one or all files

; &8d9a referenced 1 time by &8577
.sub_c8d9a
    ldx #0                                                            ; 8d9a: a2 00       ..
    stx tmp8                                                          ; 8d9c: 86 8d       ..
    stx tmp9                                                          ; 8d9e: 86 8e       ..
    beq c8daf                                                         ; 8da0: f0 0d       ..             ; ALWAYS branch

; &8da2 referenced 1 time by &8d28
.sub_c8da2
    pha                                                               ; 8da2: 48          H
    tya                                                               ; 8da3: 98          .
    pha                                                               ; 8da4: 48          H
    jsr compute_bytes_free                                            ; 8da5: 20 be af     ..
    stx tmp8                                                          ; 8da8: 86 8d       ..
    sty tmp9                                                          ; 8daa: 84 8e       ..
    pla                                                               ; 8dac: 68          h
    tay                                                               ; 8dad: a8          .
    pla                                                               ; 8dae: 68          h
; &8daf referenced 1 time by &8da0
.c8daf
    sta tmp0                                                          ; 8daf: 85 85       ..
    sty tmp1                                                          ; 8db1: 84 86       ..
    jsr compute_bytes_free                                            ; 8db3: 20 be af     ..
    stx tmp6                                                          ; 8db6: 86 8b       ..
    sty tmp7                                                          ; 8db8: 84 8c       ..
    lsr tmp9                                                          ; 8dba: 46 8e       F.
    ror tmp8                                                          ; 8dbc: 66 8d       f.
    lsr tmp9                                                          ; 8dbe: 46 8e       F.
    ror tmp8                                                          ; 8dc0: 66 8d       f.
    lda tmp9                                                          ; 8dc2: a5 8e       ..
    cmp #4                                                            ; 8dc4: c9 04       ..
    bcc c8dce                                                         ; 8dc6: 90 06       ..
    lda #4                                                            ; 8dc8: a9 04       ..
    sta tmp9                                                          ; 8dca: 85 8e       ..
    sta tmp8                                                          ; 8dcc: 85 8d       ..
; &8dce referenced 1 time by &8dc6
.c8dce
    lda tmp6                                                          ; 8dce: a5 8b       ..
    sbc tmp8                                                          ; 8dd0: e5 8d       ..
    sta tmp6                                                          ; 8dd2: 85 8b       ..
    lda tmp7                                                          ; 8dd4: a5 8c       ..
    sbc tmp9                                                          ; 8dd6: e5 8e       ..
    sta tmp7                                                          ; 8dd8: 85 8c       ..
    lda tmp0                                                          ; 8dda: a5 85       ..
    clc                                                               ; 8ddc: 18          .
    adc tmp6                                                          ; 8ddd: 65 8b       e.
    sta ptr5                                                          ; 8ddf: 85 15       ..
    pha                                                               ; 8de1: 48          H
    lda tmp1                                                          ; 8de2: a5 86       ..
    adc tmp7                                                          ; 8de4: 65 8c       e.
    sta ptr5+1                                                        ; 8de6: 85 16       ..
    sta l0081                                                         ; 8de8: 85 81       ..
    pla                                                               ; 8dea: 68          h
    sbc #&8b                                                          ; 8deb: e9 8b       ..
    sta input_buffer_ptr+1                                            ; 8ded: 85 80       ..
    bcs return_18                                                     ; 8def: b0 02       ..
    dec l0081                                                         ; 8df1: c6 81       ..
; &8df3 referenced 1 time by &8def
.return_18
    rts                                                               ; 8df3: 60          `

; &8df4 referenced 5 times by &87dd, &88e9, &8906, &8e1f, &8f0d
.sub_c8df4
    jsr sub_c8e33                                                     ; 8df4: 20 33 8e     3.
    beq return_19                                                     ; 8df7: f0 2b       .+
    ldx #0                                                            ; 8df9: a2 00       ..
; &8dfb referenced 1 time by &8e0d
.loop_c8dfb
    lda input_buffer,y                                                ; 8dfb: b9 00 05    ...
    cmp #&0d                                                          ; 8dfe: c9 0d       ..
    beq c8e25                                                         ; 8e00: f0 23       .#
    iny                                                               ; 8e02: c8          .
    cmp l007e                                                         ; 8e03: c5 7e       .~
    beq c8e25                                                         ; 8e05: f0 1e       ..
    sta filename,x                                                    ; 8e07: 9d 5c 07    .\.
    inx                                                               ; 8e0a: e8          .
    cpx #&14                                                          ; 8e0b: e0 14       ..
    bne loop_c8dfb                                                    ; 8e0d: d0 ec       ..
; &8e0f referenced 2 times by &890f, &8e22
.c8e0f
    jsr print_inline_string                                           ; 8e0f: 20 fa a7     ..
    equs "Bad filename"                                               ; 8e12: 42 61 64... Bad
    equb &ff                                                          ; 8e1e: ff          .

; &8e1f referenced 5 times by &8471, &8482, &851d, &8558, &8880
.sub_c8e1f
    jsr sub_c8df4                                                     ; 8e1f: 20 f4 8d     ..
    beq c8e0f                                                         ; 8e22: f0 eb       ..
; &8e24 referenced 1 time by &8df7
.return_19
    rts                                                               ; 8e24: 60          `

; &8e25 referenced 2 times by &8e00, &8e05
.c8e25
    lda #&0d                                                          ; 8e25: a9 0d       ..
    sta filename,x                                                    ; 8e27: 9d 5c 07    .\.
    sty input_buffer_ptr                                              ; 8e2a: 84 7f       ..
; &8e2c referenced 6 times by &8e37, &8e40, &8e44, &8e4b, &8e50, &8e5b
.return_20
    rts                                                               ; 8e2c: 60          `

; &8e2d referenced 1 time by &8243
.sub_c8e2d
    lda #&20 ; ' '                                                    ; 8e2d: a9 20       .
    sta l007e                                                         ; 8e2f: 85 7e       .~
    sty input_buffer_ptr                                              ; 8e31: 84 7f       ..
; &8e33 referenced 8 times by &83f5, &8418, &8649, &879d, &8841, &899f, &8df4, &8ef9
.sub_c8e33
    lda l007e                                                         ; 8e33: a5 7e       .~
    cmp #&0d                                                          ; 8e35: c9 0d       ..
    beq return_20                                                     ; 8e37: f0 f3       ..
    ldy input_buffer_ptr                                              ; 8e39: a4 7f       ..
; &8e3b referenced 1 time by &8e47
.loop_c8e3b
    lda input_buffer,y                                                ; 8e3b: b9 00 05    ...
    cmp #&0d                                                          ; 8e3e: c9 0d       ..
    beq return_20                                                     ; 8e40: f0 ea       ..
    cmp l007e                                                         ; 8e42: c5 7e       .~
    bne return_20                                                     ; 8e44: d0 e6       ..
    iny                                                               ; 8e46: c8          .
    bne loop_c8e3b                                                    ; 8e47: d0 f2       ..
; &8e49 referenced 5 times by &8468, &8797, &8877, &88e6, &8ebe
.sub_c8e49
    bit l003c                                                         ; 8e49: 24 3c       $<
    bvs return_20                                                     ; 8e4b: 70 df       p.
    lda l003c                                                         ; 8e4d: a5 3c       .<
    ror a                                                             ; 8e4f: 6a          j
    bcc return_20                                                     ; 8e50: 90 da       ..
    bcs c8e5d                                                         ; 8e52: b0 09       ..             ; ALWAYS branch

; &8e54 referenced 3 times by &84ae, &84eb, &850a
.sub_c8e54
    bit l003c                                                         ; 8e54: 24 3c       $<
    bvs c8e5d                                                         ; 8e56: 70 05       p.
    lda l003c                                                         ; 8e58: a5 3c       .<
    ror a                                                             ; 8e5a: 6a          j
    bcs return_20                                                     ; 8e5b: b0 cf       ..
; &8e5d referenced 2 times by &8e52, &8e56
.c8e5d
    jsr sub_c89e5                                                     ; 8e5d: 20 e5 89     ..
    jmp cli_loop                                                      ; 8e60: 4c f6 81    L..

; ***************************************************************************************
; &8e63 referenced 2 times by &846b, &887a
.error_if_cassette_filesystem
    jsr test_for_cassette_filesystem                                  ; 8e63: 20 7d 8e     }.
    bcs return_21                                                     ; 8e66: b0 1d       ..
    jsr print_inline_string                                           ; 8e68: 20 fa a7     ..
    equs "Not with cassette"                                          ; 8e6b: 4e 6f 74... Not
    equb &ff                                                          ; 8e7c: ff          .

; ***************************************************************************************
; &8e7d referenced 2 times by &87e2, &8e63
.test_for_cassette_filesystem
    lda #0                                                            ; 8e7d: a9 00       ..
    tay                                                               ; 8e7f: a8          .              ; Y=&00
    jsr osargs                                                        ; 8e80: 20 da ff     ..            ; Get filing system number (A=0, Y=0)
    ; A is the filing system number:
    ;     A=0, no filing system selected
    ;     A=1, 1200 baud CFS
    ;     A=2, 300 baud CFS
    ;     A=3, ROM filing system
    ;     A=4, Disc filing system
    ;     A=5, Network filing system
    ;     A=6, Teletext filing system
    ;     A=7, IEEE filing system
    ;     A=8, ADFS
    ;     A=9, Host filing system
    ;     A=10, Videodisc filing system
    cmp #3                                                            ; 8e83: c9 03       ..             ; A=filing system number
; &8e85 referenced 1 time by &8e66
.return_21
    rts                                                               ; 8e85: 60          `

; ***************************************************************************************
; &8e86 referenced 6 times by &80e6, &8111, &822d, &8271, &85c6, &afca
.check_for_bad_document
    lda #&aa                                                          ; 8e86: a9 aa       ..
    cmp zp_initialisation_canary                                      ; 8e88: c5 0a       ..
    bne return_22                                                     ; 8e8a: d0 21       .!
    ldy #0                                                            ; 8e8c: a0 00       ..
    cmp (oshwm),y                                                     ; 8e8e: d1 1f       ..
    bne return_22                                                     ; 8e90: d0 1b       ..
    lda top                                                           ; 8e92: a5 0d       ..
    sec                                                               ; 8e94: 38          8
    sbc #1                                                            ; 8e95: e9 01       ..
    sta tmp8                                                          ; 8e97: 85 8d       ..
    lda top+1                                                         ; 8e99: a5 0e       ..
    sbc #0                                                            ; 8e9b: e9 00       ..
    sta tmp9                                                          ; 8e9d: 85 8e       ..
    lda #&0d                                                          ; 8e9f: a9 0d       ..
    cmp document_initialisation_canary                                ; 8ea1: cd ce 05    ...
    bne return_22                                                     ; 8ea4: d0 07       ..
    cmp (tmp8),y                                                      ; 8ea6: d1 8d       ..
    bne return_22                                                     ; 8ea8: d0 03       ..
    iny                                                               ; 8eaa: c8          .              ; Y=&01
    lda (tmp8),y                                                      ; 8eab: b1 8d       ..
; &8ead referenced 4 times by &8e8a, &8e90, &8ea4, &8ea8
.return_22
    rts                                                               ; 8ead: 60          `

; ***************************************************************************************
; &8eae referenced 2 times by &8119, &8232
.display_nl_then_no_text
    jsr osnewl                                                        ; 8eae: 20 e7 ff     ..            ; Write newline (characters 10 and 13)
; ***************************************************************************************
; &8eb1 referenced 1 time by &8276
.display_no_text
    jsr print_inline_string                                           ; 8eb1: 20 fa a7     ..
    equs "No text", &0d                                               ; 8eb4: 4e 6f 20... No
    equb 0                                                            ; 8ebc: 00          .

    rts                                                               ; 8ebd: 60          `

; &8ebe referenced 2 times by &8434, &8445
.sub_c8ebe
    jsr sub_c8e49                                                     ; 8ebe: 20 49 8e     I.
    jsr sub_c8535                                                     ; 8ec1: 20 35 85     5.
    jsr sub_cb104                                                     ; 8ec4: 20 04 b1     ..
    lda top                                                           ; 8ec7: a5 0d       ..
    adc #3                                                            ; 8ec9: 69 03       i.
    sta ptr5                                                          ; 8ecb: 85 15       ..
    tax                                                               ; 8ecd: aa          .
    lda top+1                                                         ; 8ece: a5 0e       ..
    adc #0                                                            ; 8ed0: 69 00       i.
    sta ptr5+1                                                        ; 8ed2: 85 16       ..
    tay                                                               ; 8ed4: a8          .
    txa                                                               ; 8ed5: 8a          .
    adc #&8d                                                          ; 8ed6: 69 8d       i.
    bcc c8edb                                                         ; 8ed8: 90 01       ..
    iny                                                               ; 8eda: c8          .
; &8edb referenced 1 time by &8ed8
.c8edb
    sta first_macro_ptr                                               ; 8edb: 85 19       ..
    sta last_macro_ptr                                                ; 8edd: 85 1b       ..
    sty first_macro_ptr+1                                             ; 8edf: 84 1a       ..
    sty last_macro_ptr+1                                              ; 8ee1: 84 1c       ..
    lda #0                                                            ; 8ee3: a9 00       ..
    sta l0031                                                         ; 8ee5: 85 31       .1
    sta print_xpos                                                    ; 8ee7: 85 78       .x
    sta l0032                                                         ; 8ee9: 85 32       .2
    tay                                                               ; 8eeb: a8          .              ; Y=&00
    sta (last_macro_ptr),y                                            ; 8eec: 91 1b       ..
    lda #<(current_ruler_buffer)                                      ; 8eee: a9 cf       ..
    sta current_ruler_ptr                                             ; 8ef0: 85 06       ..
    lda #>(current_ruler_buffer)                                      ; 8ef2: a9 05       ..
    sta current_ruler_ptr+1                                           ; 8ef4: 85 07       ..
    jsr find_margins_of_current_ruler                                 ; 8ef6: 20 a2 ab     ..
    jsr sub_c8e33                                                     ; 8ef9: 20 33 8e     3.
    bne c8f0d                                                         ; 8efc: d0 0f       ..
    inc l0032                                                         ; 8efe: e6 32       .2
    lda page                                                          ; 8f00: a5 0b       ..
    sta ptr6                                                          ; 8f02: 85 13       ..
    lda page+1                                                        ; 8f04: a5 0c       ..
    sta ptr6+1                                                        ; 8f06: 85 14       ..
    bne c8f30                                                         ; 8f08: d0 26       .&
; &8f0a referenced 1 time by &8f3e
.c8f0a
    jsr sub_c902c                                                     ; 8f0a: 20 2c 90     ,.
; &8f0d referenced 1 time by &8efc
.c8f0d
    jsr sub_c8df4                                                     ; 8f0d: 20 f4 8d     ..
    bne c8f29                                                         ; 8f10: d0 17       ..
    lda l0031                                                         ; 8f12: a5 31       .1
    bpl return_23                                                     ; 8f14: 10 03       ..
    jmp c9263                                                         ; 8f16: 4c 63 92    Lc.

; &8f19 referenced 1 time by &8f14
.return_23
    rts                                                               ; 8f19: 60          `

; &8f1a referenced 3 times by &8f42, &9029, &92cc
.c8f1a
    jsr stop_printing                                                 ; 8f1a: 20 4b 84     K.
    jsr acknowledge_escape                                            ; 8f1d: 20 6e a7     n.
    jsr sub_c902c                                                     ; 8f20: 20 2c 90     ,.
    jsr osnewl                                                        ; 8f23: 20 e7 ff     ..            ; Write newline (characters 10 and 13)
    jmp cli_loop                                                      ; 8f26: 4c f6 81    L..

; &8f29 referenced 1 time by &8f10
.c8f29
    lda #&40 ; '@'                                                    ; 8f29: a9 40       .@
    jsr open_file                                                     ; 8f2b: 20 58 88     X.
    sta rw_file_handle                                                ; 8f2e: 85 4d       .M
; &8f30 referenced 2 times by &8f08, &900e
.c8f30
    lda l0031                                                         ; 8f30: a5 31       .1
    beq c8f3b                                                         ; 8f32: f0 07       ..
    lda l0021                                                         ; 8f34: a5 21       .!
    bne c8f3b                                                         ; 8f36: d0 03       ..
    jsr c9263                                                         ; 8f38: 20 63 92     c.
; &8f3b referenced 2 times by &8f32, &8f36
.c8f3b
    jsr sub_c9188                                                     ; 8f3b: 20 88 91     ..
    bcs c8f0a                                                         ; 8f3e: b0 ca       ..
    lda escape_flag                                                   ; 8f40: a5 ff       ..
    bmi c8f1a                                                         ; 8f42: 30 d6       0.
    jsr sub_c916a                                                     ; 8f44: 20 6a 91     j.
    ldy #0                                                            ; 8f47: a0 00       ..
    sty input_buffer_ptr+1                                            ; 8f49: 84 80       ..
    jsr sub_caf6f                                                     ; 8f4b: 20 6f af     o.
    bne c8fce                                                         ; 8f4e: d0 7e       .~
    ldy #3                                                            ; 8f50: a0 03       ..
    sty input_buffer_ptr+1                                            ; 8f52: 84 80       ..
    jsr sub_cab6e                                                     ; 8f54: 20 6e ab     n.
    bne c8f6e                                                         ; 8f57: d0 15       ..
    ldy #3                                                            ; 8f59: a0 03       ..
    ldx #0                                                            ; 8f5b: a2 00       ..
; &8f5d referenced 1 time by &8f66
.loop_c8f5d
    lda (tmp0),y                                                      ; 8f5d: b1 85       ..
    sta current_ruler_buffer,x                                        ; 8f5f: 9d cf 05    ...
    iny                                                               ; 8f62: c8          .
    inx                                                               ; 8f63: e8          .
    cmp #&0d                                                          ; 8f64: c9 0d       ..
    bne loop_c8f5d                                                    ; 8f66: d0 f5       ..
    jsr find_margins_of_current_ruler                                 ; 8f68: 20 a2 ab     ..
; &8f6b referenced 2 times by &8f76, &8f98
.c8f6b
    jmp c900e                                                         ; 8f6b: 4c 0e 90    L..

; &8f6e referenced 1 time by &8f57
.c8f6e
    jsr lookup_formatting_command                                     ; 8f6e: 20 30 97     0.
    bmi c8f7a                                                         ; 8f71: 30 07       0.
    jsr execute_formatting_command                                    ; 8f73: 20 55 97     U.
    beq c8f6b                                                         ; 8f76: f0 f3       ..
    bne c8fce                                                         ; 8f78: d0 54       .T             ; ALWAYS branch

; &8f7a referenced 1 time by &8f71
.c8f7a
    lda first_macro_ptr                                               ; 8f7a: a5 19       ..
    sta tmp6                                                          ; 8f7c: 85 8b       ..
    lda first_macro_ptr+1                                             ; 8f7e: a5 1a       ..
    sta tmp7                                                          ; 8f80: 85 8c       ..
    ldy #1                                                            ; 8f82: a0 01       ..
    lda (current_format_line_ptr),y                                   ; 8f84: b1 04       ..
    sta tmp8                                                          ; 8f86: 85 8d       ..
    iny                                                               ; 8f88: c8          .              ; Y=&02
    lda (current_format_line_ptr),y                                   ; 8f89: b1 04       ..
    jsr is_uppercase                                                  ; 8f8b: 20 6b 8c     k.
    bcc c8f92                                                         ; 8f8e: 90 02       ..
    lda #&20 ; ' '                                                    ; 8f90: a9 20       .
; &8f92 referenced 1 time by &8f8e
.c8f92
    sta tmp9                                                          ; 8f92: 85 8e       ..
; &8f94 referenced 1 time by &8fb6
.lookup_macro_name
    ldy #0                                                            ; 8f94: a0 00       ..
    lda (tmp6),y                                                      ; 8f96: b1 8b       ..
    beq c8f6b                                                         ; 8f98: f0 d1       ..
    ldy #2                                                            ; 8f9a: a0 02       ..
    lda (tmp6),y                                                      ; 8f9c: b1 8b       ..
    cmp tmp8                                                          ; 8f9e: c5 8d       ..
    bne get_next_macro_in_linked_list                                 ; 8fa0: d0 07       ..
    iny                                                               ; 8fa2: c8          .              ; Y=&03
    lda (tmp6),y                                                      ; 8fa3: b1 8b       ..
    cmp tmp9                                                          ; 8fa5: c5 8e       ..
    beq c8fb9                                                         ; 8fa7: f0 10       ..
; &8fa9 referenced 1 time by &8fa0
.get_next_macro_in_linked_list
    ldy #0                                                            ; 8fa9: a0 00       ..
    lda (tmp6),y                                                      ; 8fab: b1 8b       ..
    pha                                                               ; 8fad: 48          H
    iny                                                               ; 8fae: c8          .              ; Y=&01
    lda (tmp6),y                                                      ; 8faf: b1 8b       ..
    sta tmp7                                                          ; 8fb1: 85 8c       ..
    pla                                                               ; 8fb3: 68          h
    sta tmp6                                                          ; 8fb4: 85 8b       ..
    jmp lookup_macro_name                                             ; 8fb6: 4c 94 8f    L..

; &8fb9 referenced 1 time by &8fa7
.c8fb9
    lda macro_executing_flag                                          ; 8fb9: a5 2d       .-
    bne nested_subroutine_error                                       ; 8fbb: d0 54       .T
    lda tmp6                                                          ; 8fbd: a5 8b       ..
    clc                                                               ; 8fbf: 18          .
    adc #4                                                            ; 8fc0: 69 04       i.
    sta ptr3                                                          ; 8fc2: 85 1d       ..
    lda tmp7                                                          ; 8fc4: a5 8c       ..
    adc #0                                                            ; 8fc6: 69 00       i.
    sta ptr3+1                                                        ; 8fc8: 85 1e       ..
    sta macro_executing_flag                                          ; 8fca: 85 2d       .-
    bne c900e                                                         ; 8fcc: d0 40       .@
; &8fce referenced 2 times by &8f4e, &8f78
.c8fce
    lda l0031                                                         ; 8fce: a5 31       .1
    bne c8fd5                                                         ; 8fd0: d0 03       ..
    jsr render_new_page                                               ; 8fd2: 20 99 92     ..
; &8fd5 referenced 1 time by &8fd0
.c8fd5
    jsr sub_c9407                                                     ; 8fd5: 20 07 94     ..
    lda #0                                                            ; 8fd8: a9 00       ..
    sta l0039                                                         ; 8fda: 85 39       .9
    ldy input_buffer_ptr+1                                            ; 8fdc: a4 80       ..
    lda print_flags                                                   ; 8fde: a5 69       .i
    bpl c8fe6                                                         ; 8fe0: 10 04       ..
    lda microspacing_flag                                             ; 8fe2: a5 6c       .l
    bne c9034                                                         ; 8fe4: d0 4e       .N
; &8fe6 referenced 3 times by &8fe0, &8ff1, &90fe
.c8fe6
    lda (tmp0),y                                                      ; 8fe6: b1 85       ..
    iny                                                               ; 8fe8: c8          .
    jsr sub_c9431                                                     ; 8fe9: 20 31 94     1.
    jsr c9426                                                         ; 8fec: 20 26 94     &.
    cmp #&0d                                                          ; 8fef: c9 0d       ..
    bne c8fe6                                                         ; 8ff1: d0 f3       ..
    inc register_value_l                                              ; 8ff3: ee ae 07    ...
    bne c8ffb                                                         ; 8ff6: d0 03       ..
    inc register_value_l+1                                            ; 8ff8: ee af 07    ...
; &8ffb referenced 2 times by &8ff6, &9167
.c8ffb
    ldx line_spacing                                                  ; 8ffb: a6 27       .'
    lda l0021                                                         ; 8ffd: a5 21       .!
    clc                                                               ; 8fff: 18          .
    sbc line_spacing                                                  ; 9000: e5 27       .'
    bcs c9009                                                         ; 9002: b0 05       ..
    lda #0                                                            ; 9004: a9 00       ..
    ldx l0021                                                         ; 9006: a6 21       .!
    dex                                                               ; 9008: ca          .
; &9009 referenced 1 time by &9002
.c9009
    sta l0021                                                         ; 9009: 85 21       .!
    jsr print_vertical_space                                          ; 900b: 20 24 94     $.
; &900e referenced 2 times by &8f6b, &8fcc
.c900e
    jmp c8f30                                                         ; 900e: 4c 30 8f    L0.

; ***************************************************************************************
; &9011 referenced 1 time by &8fbb
.nested_subroutine_error
    jsr stop_printing                                                 ; 9011: 20 4b 84     K.
    jsr print_inline_string                                           ; 9014: 20 fa a7     ..
    equs "Nested macro call"                                          ; 9017: 4e 65 73... Nes
    equb 0                                                            ; 9028: 00          .

    jmp c8f1a                                                         ; 9029: 4c 1a 8f    L..

; &902c referenced 3 times by &8f0a, &8f20, &96b2
.sub_c902c
    lda l0032                                                         ; 902c: a5 32       .2
    bne return_24                                                     ; 902e: d0 03       ..
    jmp close_file                                                    ; 9030: 4c 8f 8d    L..

; &9033 referenced 1 time by &902e
.return_24
    rts                                                               ; 9033: 60          `

; &9034 referenced 1 time by &8fe4
.c9034
    ldx #0                                                            ; 9034: a2 00       ..
    stx l0044                                                         ; 9036: 86 44       .D
    stx l0046                                                         ; 9038: 86 46       .F
    stx l0045                                                         ; 903a: 86 45       .E
    stx l0047                                                         ; 903c: 86 47       .G
    stx l0039                                                         ; 903e: 86 39       .9
    stx l0048                                                         ; 9040: 86 48       .H
    stx l0042                                                         ; 9042: 86 42       .B
    stx l0043                                                         ; 9044: 86 43       .C
    stx l0083                                                         ; 9046: 86 83       ..
; &9048 referenced 5 times by &9062, &9085, &90c0, &90c4, &90df
.c9048
    txa                                                               ; 9048: 8a          .
    pha                                                               ; 9049: 48          H
    lda (tmp0),y                                                      ; 904a: b1 85       ..
    jsr sub_c9431                                                     ; 904c: 20 31 94     1.
    pla                                                               ; 904f: 68          h
    tax                                                               ; 9050: aa          .
    lda (tmp0),y                                                      ; 9051: b1 85       ..
    iny                                                               ; 9053: c8          .
    cmp #&1a                                                          ; 9054: c9 1a       ..
    bne c906f                                                         ; 9056: d0 17       ..
    bit l0083                                                         ; 9058: 24 83       $.
    bpl c9064                                                         ; 905a: 10 08       ..
    lda l0048                                                         ; 905c: a5 48       .H
    beq c906b                                                         ; 905e: f0 0b       ..
    inc l0043                                                         ; 9060: e6 43       .C
    bne c9048                                                         ; 9062: d0 e4       ..
; &9064 referenced 2 times by &905a, &9077
.c9064
    lda l0039                                                         ; 9064: a5 39       .9
    sta l0047                                                         ; 9066: 85 47       .G
    jmp c908c                                                         ; 9068: 4c 8c 90    L..

; &906b referenced 1 time by &905e
.c906b
    lda #&20 ; ' '                                                    ; 906b: a9 20       .
    dec l0042                                                         ; 906d: c6 42       .B
; &906f referenced 1 time by &9056
.c906f
    cmp #&20 ; ' '                                                    ; 906f: c9 20       .
    bcc c9092                                                         ; 9071: 90 1f       ..
    bne c9090                                                         ; 9073: d0 1b       ..
    bit l0083                                                         ; 9075: 24 83       $.
    bpl c9064                                                         ; 9077: 10 eb       ..
    lda l0042                                                         ; 9079: a5 42       .B
    beq c908a                                                         ; 907b: f0 0d       ..
    bmi c9087                                                         ; 907d: 30 08       0.
    inc l0043                                                         ; 907f: e6 43       .C
    lda #0                                                            ; 9081: a9 00       ..
    sta l0042                                                         ; 9083: 85 42       .B
    beq c9048                                                         ; 9085: f0 c1       ..             ; ALWAYS branch

; &9087 referenced 1 time by &907d
.c9087
    clc                                                               ; 9087: 18          .
    ror l0042                                                         ; 9088: 66 42       fB
; &908a referenced 1 time by &907b
.c908a
    inc l0048                                                         ; 908a: e6 48       .H
; &908c referenced 1 time by &9068
.c908c
    lda #&20 ; ' '                                                    ; 908c: a9 20       .
    bne c90b6                                                         ; 908e: d0 26       .&             ; ALWAYS branch

; &9090 referenced 1 time by &9073
.c9090
    inc l0046                                                         ; 9090: e6 46       .F
; &9092 referenced 1 time by &9071
.c9092
    cmp #9                                                            ; 9092: c9 09       ..
    beq c90a0                                                         ; 9094: f0 0a       ..
    cmp #&0b                                                          ; 9096: c9 0b       ..
    beq c90a0                                                         ; 9098: f0 06       ..
    sec                                                               ; 909a: 38          8
    ror l0083                                                         ; 909b: 66 83       f.
    jmp c90b6                                                         ; 909d: 4c b6 90    L..

; &90a0 referenced 2 times by &9094, &9098
.c90a0
    pha                                                               ; 90a0: 48          H
    lda l0039                                                         ; 90a1: a5 39       .9
    sta l0047                                                         ; 90a3: 85 47       .G
    lda #0                                                            ; 90a5: a9 00       ..
    sta l0083                                                         ; 90a7: 85 83       ..
    sta l0046                                                         ; 90a9: 85 46       .F
    sta l0048                                                         ; 90ab: 85 48       .H
    sta l0042                                                         ; 90ad: 85 42       .B
    sta l0043                                                         ; 90af: 85 43       .C
    sta l0044                                                         ; 90b1: 85 44       .D
    sta l0045                                                         ; 90b3: 85 45       .E
    pla                                                               ; 90b5: 68          h
; &90b6 referenced 2 times by &908e, &909d
.c90b6
    sta output_buffer,x                                               ; 90b6: 9d 54 06    .T.
    inx                                                               ; 90b9: e8          .
    cmp #&0d                                                          ; 90ba: c9 0d       ..
    beq c90e2                                                         ; 90bc: f0 24       .$
    cmp #&20 ; ' '                                                    ; 90be: c9 20       .
    beq c9048                                                         ; 90c0: f0 86       ..
    lda l0048                                                         ; 90c2: a5 48       .H
    beq c9048                                                         ; 90c4: f0 82       ..
    clc                                                               ; 90c6: 18          .
    adc l0044                                                         ; 90c7: 65 44       eD
    sta l0044                                                         ; 90c9: 85 44       .D
    lda l0046                                                         ; 90cb: a5 46       .F
    adc l0048                                                         ; 90cd: 65 48       eH
    sta l0046                                                         ; 90cf: 85 46       .F
    lda l0045                                                         ; 90d1: a5 45       .E
    adc l0043                                                         ; 90d3: 65 43       eC
    sta l0045                                                         ; 90d5: 85 45       .E
    lda #0                                                            ; 90d7: a9 00       ..
    sta l0048                                                         ; 90d9: 85 48       .H
    sta l0042                                                         ; 90db: 85 42       .B
    sta l0043                                                         ; 90dd: 85 43       .C
    jmp c9048                                                         ; 90df: 4c 48 90    LH.

; &90e2 referenced 1 time by &90bc
.c90e2
    lda l0045                                                         ; 90e2: a5 45       .E
    beq c90f8                                                         ; 90e4: f0 12       ..
    lda ruler_right_stop                                              ; 90e6: a5 3e       .>
    beq c90f8                                                         ; 90e8: f0 0e       ..
    sec                                                               ; 90ea: 38          8
    sbc l0047                                                         ; 90eb: e5 47       .G
    bcc c90f8                                                         ; 90ed: 90 09       ..
    sbc l0045                                                         ; 90ef: e5 45       .E
    adc #0                                                            ; 90f1: 69 00       i.
    sec                                                               ; 90f3: 38          8
    sbc l0046                                                         ; 90f4: e5 46       .F
    beq c9101                                                         ; 90f6: f0 09       ..
; &90f8 referenced 3 times by &90e4, &90e8, &90ed
.c90f8
    lda #0                                                            ; 90f8: a9 00       ..
    sta l0039                                                         ; 90fa: 85 39       .9
    ldy input_buffer_ptr+1                                            ; 90fc: a4 80       ..
    jmp c8fe6                                                         ; 90fe: 4c e6 8f    L..

; &9101 referenced 1 time by &90f6
.c9101
    lda #0                                                            ; 9101: a9 00       ..
    sta tmp9                                                          ; 9103: 85 8e       ..
    ldx #8                                                            ; 9105: a2 08       ..
; &9107 referenced 1 time by &9116
.loop_c9107
    asl a                                                             ; 9107: 0a          .
    rol tmp9                                                          ; 9108: 26 8e       &.
    asl l0045                                                         ; 910a: 06 45       .E
    bcc c9115                                                         ; 910c: 90 07       ..
    clc                                                               ; 910e: 18          .
    adc microspacing_flag                                             ; 910f: 65 6c       el
    bcc c9115                                                         ; 9111: 90 02       ..
    inc tmp9                                                          ; 9113: e6 8e       ..
; &9115 referenced 2 times by &910c, &9111
.c9115
    dex                                                               ; 9115: ca          .
    bne loop_c9107                                                    ; 9116: d0 ef       ..
    sta tmp8                                                          ; 9118: 85 8d       ..
    lda l0044                                                         ; 911a: a5 44       .D
    sta l0046                                                         ; 911c: 85 46       .F
    jsr sub_cadf0                                                     ; 911e: 20 f0 ad     ..
    sta l0045                                                         ; 9121: 85 45       .E
    lda tmp8                                                          ; 9123: a5 8d       ..
    sta l0044                                                         ; 9125: 85 44       .D
    ldy #0                                                            ; 9127: a0 00       ..
    sty l0039                                                         ; 9129: 84 39       .9
; &912b referenced 1 time by &9165
.c912b
    lda output_buffer,y                                               ; 912b: b9 54 06    .T.
    iny                                                               ; 912e: c8          .
    jsr sub_c9431                                                     ; 912f: 20 31 94     1.
    pha                                                               ; 9132: 48          H
    lda l0039                                                         ; 9133: a5 39       .9
    cmp l0047                                                         ; 9135: c5 47       .G
    beq c913b                                                         ; 9137: f0 02       ..
    bcs c9142                                                         ; 9139: b0 07       ..
; &913b referenced 1 time by &9137
.c913b
    pla                                                               ; 913b: 68          h
    jsr c9426                                                         ; 913c: 20 26 94     &.
    jmp c9163                                                         ; 913f: 4c 63 91    Lc.

; &9142 referenced 1 time by &9139
.c9142
    pla                                                               ; 9142: 68          h
    cmp #&20 ; ' '                                                    ; 9143: c9 20       .
    bne c915b                                                         ; 9145: d0 14       ..
    lda microspacing_flag                                             ; 9147: a5 6c       .l
    clc                                                               ; 9149: 18          .
    adc l0044                                                         ; 914a: 65 44       eD
    tax                                                               ; 914c: aa          .
    lda l0045                                                         ; 914d: a5 45       .E
    beq c9154                                                         ; 914f: f0 03       ..
    inx                                                               ; 9151: e8          .
    dec l0045                                                         ; 9152: c6 45       .E
; &9154 referenced 1 time by &914f
.c9154
    jsr sub_c9173                                                     ; 9154: 20 73 91     s.
    lda #&20 ; ' '                                                    ; 9157: a9 20       .
    bne c9160                                                         ; 9159: d0 05       ..             ; ALWAYS branch

; &915b referenced 1 time by &9145
.c915b
    ldx microspacing_flag                                             ; 915b: a6 6c       .l
    jsr sub_c9173                                                     ; 915d: 20 73 91     s.
; &9160 referenced 1 time by &9159
.c9160
    jsr print_char                                                    ; 9160: 20 57 94     W.
; &9163 referenced 1 time by &913f
.c9163
    cmp #&0d                                                          ; 9163: c9 0d       ..
    bne c912b                                                         ; 9165: d0 c4       ..
    jmp c8ffb                                                         ; 9167: 4c fb 8f    L..

; &916a referenced 1 time by &8f44
.sub_c916a
    ldx print_flags                                                   ; 916a: a6 69       .i
    bpl return_25                                                     ; 916c: 10 15       ..
    ldx microspacing_flag                                             ; 916e: a6 6c       .l
    bne c9177                                                         ; 9170: d0 05       ..
    rts                                                               ; 9172: 60          `

; &9173 referenced 2 times by &9154, &915d
.sub_c9173
    cpx l0043                                                         ; 9173: e4 43       .C
    beq return_25                                                     ; 9175: f0 0c       ..
; &9177 referenced 1 time by &9170
.c9177
    jsr sub_c9445                                                     ; 9177: 20 45 94     E.
    pha                                                               ; 917a: 48          H
    stx l0043                                                         ; 917b: 86 43       .C
    lda #9                                                            ; 917d: a9 09       ..
    jsr call_printer_driver                                           ; 917f: 20 a3 94     ..
    pla                                                               ; 9182: 68          h
; &9183 referenced 2 times by &916c, &9175
.return_25
    rts                                                               ; 9183: 60          `

; &9184 referenced 1 time by &91ab
.c9184
    lda #0                                                            ; 9184: a9 00       ..
    sta macro_executing_flag                                          ; 9186: 85 2d       .-
; &9188 referenced 1 time by &8f3b
.sub_c9188
    lda macro_executing_flag                                          ; 9188: a5 2d       .-
    bne c91a3                                                         ; 918a: d0 17       ..
    lda ptr5                                                          ; 918c: a5 15       ..
    sta input_buffer_ptr+1                                            ; 918e: 85 80       ..
    sta tmp0                                                          ; 9190: 85 85       ..
    lda ptr5+1                                                        ; 9192: a5 16       ..
    sta l0081                                                         ; 9194: 85 81       ..
    sta tmp1                                                          ; 9196: 85 86       ..
    jsr sub_c9241                                                     ; 9198: 20 41 92     A.
    bcs return_26                                                     ; 919b: b0 3c       .<
    lda ptr5                                                          ; 919d: a5 15       ..
    ldy ptr5+1                                                        ; 919f: a4 16       ..
    bne c91d0                                                         ; 91a1: d0 2d       .-
; &91a3 referenced 1 time by &918a
.c91a3
    ldy #0                                                            ; 91a3: a0 00       ..
    ldx #0                                                            ; 91a5: a2 00       ..
; &91a7 referenced 2 times by &91bc, &9225
.c91a7
    lda (ptr3),y                                                      ; 91a7: b1 1d       ..
    cmp #4                                                            ; 91a9: c9 04       ..
    beq c9184                                                         ; 91ab: f0 d7       ..
    cmp #&40 ; '@'                                                    ; 91ad: c9 40       .@
    beq c91da                                                         ; 91af: f0 29       .)
    iny                                                               ; 91b1: c8          .
; &91b2 referenced 1 time by &91c0
.loop_c91b2
    sta current_line_buffer,x                                         ; 91b2: 9d 45 05    .E.
    inx                                                               ; 91b5: e8          .
    cmp #&0d                                                          ; 91b6: c9 0d       ..
    beq c91c2                                                         ; 91b8: f0 08       ..
    cpx #&83                                                          ; 91ba: e0 83       ..
    bcc c91a7                                                         ; 91bc: 90 e9       ..
    lda #&0d                                                          ; 91be: a9 0d       ..
    bne loop_c91b2                                                    ; 91c0: d0 f0       ..             ; ALWAYS branch

; &91c2 referenced 1 time by &91b8
.c91c2
    tya                                                               ; 91c2: 98          .
    clc                                                               ; 91c3: 18          .
    adc ptr3                                                          ; 91c4: 65 1d       e.
    sta ptr3                                                          ; 91c6: 85 1d       ..
    bcc c91cc                                                         ; 91c8: 90 02       ..
    inc ptr3+1                                                        ; 91ca: e6 1e       ..
; &91cc referenced 1 time by &91c8
.c91cc
    lda ptr1                                                          ; 91cc: a5 00       ..
    ldy ptr1+1                                                        ; 91ce: a4 01       ..
; &91d0 referenced 1 time by &91a1
.c91d0
    sta tmp0                                                          ; 91d0: 85 85       ..
    sty tmp1                                                          ; 91d2: 84 86       ..
    sta current_format_line_ptr                                       ; 91d4: 85 04       ..
    sty current_format_line_ptr+1                                     ; 91d6: 84 05       ..
    clc                                                               ; 91d8: 18          .
; &91d9 referenced 1 time by &919b
.return_26
    rts                                                               ; 91d9: 60          `

; &91da referenced 1 time by &91af
.c91da
    iny                                                               ; 91da: c8          .
    lda (ptr3),y                                                      ; 91db: b1 1d       ..
    sec                                                               ; 91dd: 38          8
    sbc #&30 ; '0'                                                    ; 91de: e9 30       .0
    bcc c9225                                                         ; 91e0: 90 43       .C
    cmp #&0a                                                          ; 91e2: c9 0a       ..
    bcs c9225                                                         ; 91e4: b0 3f       .?
    iny                                                               ; 91e6: c8          .
    sty l0084                                                         ; 91e7: 84 84       ..
    sta l0083                                                         ; 91e9: 85 83       ..
    lda #0                                                            ; 91eb: a9 00       ..
    sta l0082                                                         ; 91ed: 85 82       ..
    ldy #2                                                            ; 91ef: a0 02       ..
; &91f1 referenced 1 time by &9205
.loop_c91f1
    dec l0083                                                         ; 91f1: c6 83       ..
    bmi c9209                                                         ; 91f3: 30 14       0.
; &91f5 referenced 3 times by &91ff, &9201, &9207
.c91f5
    iny                                                               ; 91f5: c8          .
    lda (ptr5),y                                                      ; 91f6: b1 15       ..
    cmp #&0d                                                          ; 91f8: c9 0d       ..
    beq c9223                                                         ; 91fa: f0 27       .'
    jsr sub_c9228                                                     ; 91fc: 20 28 92     (.
    beq c91f5                                                         ; 91ff: f0 f4       ..
    bvs c91f5                                                         ; 9201: 70 f2       p.
    cmp #&2c ; ','                                                    ; 9203: c9 2c       .,
    beq loop_c91f1                                                    ; 9205: f0 ea       ..
    bne c91f5                                                         ; 9207: d0 ec       ..             ; ALWAYS branch

; &9209 referenced 3 times by &91f3, &9213, &9221
.c9209
    iny                                                               ; 9209: c8          .
    lda (ptr5),y                                                      ; 920a: b1 15       ..
    cmp #&0d                                                          ; 920c: c9 0d       ..
    beq c9223                                                         ; 920e: f0 13       ..
    jsr sub_c9228                                                     ; 9210: 20 28 92     (.
    beq c9209                                                         ; 9213: f0 f4       ..
    bvs c921b                                                         ; 9215: 70 04       p.
    cmp #&2c ; ','                                                    ; 9217: c9 2c       .,
    beq c9223                                                         ; 9219: f0 08       ..
; &921b referenced 1 time by &9215
.c921b
    sta current_line_buffer,x                                         ; 921b: 9d 45 05    .E.
    inx                                                               ; 921e: e8          .
    cpx #&82                                                          ; 921f: e0 82       ..
    bcc c9209                                                         ; 9221: 90 e6       ..
; &9223 referenced 3 times by &91fa, &920e, &9219
.c9223
    ldy l0084                                                         ; 9223: a4 84       ..
; &9225 referenced 2 times by &91e0, &91e4
.c9225
    jmp c91a7                                                         ; 9225: 4c a7 91    L..

; &9228 referenced 2 times by &91fc, &9210
.sub_c9228
    cmp #&3e ; '>'                                                    ; 9228: c9 3e       .>
    bne c9231                                                         ; 922a: d0 05       ..
    lda #0                                                            ; 922c: a9 00       ..
    sta l0082                                                         ; 922e: 85 82       ..
    rts                                                               ; 9230: 60          `

; &9231 referenced 1 time by &922a
.c9231
    cmp #&3c ; '<'                                                    ; 9231: c9 3c       .<
    bne c923c                                                         ; 9233: d0 07       ..
    lda #&40 ; '@'                                                    ; 9235: a9 40       .@
    sta l0082                                                         ; 9237: 85 82       ..
    lda #0                                                            ; 9239: a9 00       ..
    rts                                                               ; 923b: 60          `

; &923c referenced 1 time by &9233
.c923c
    bit l0082                                                         ; 923c: 24 82       $.
    ora #0                                                            ; 923e: 09 00       ..
    rts                                                               ; 9240: 60          `

; &9241 referenced 2 times by &9198, &96c8
.sub_c9241
    lda l0032                                                         ; 9241: a5 32       .2
    beq c9260                                                         ; 9243: f0 1b       ..
    ldy #0                                                            ; 9245: a0 00       ..
; &9247 referenced 1 time by &925c
.loop_c9247
    lda (ptr6),y                                                      ; 9247: b1 13       ..
    sec                                                               ; 9249: 38          8
    beq return_27                                                     ; 924a: f0 13       ..
    sta (tmp0),y                                                      ; 924c: 91 85       ..
    inc ptr6                                                          ; 924e: e6 13       ..
    bne c9254                                                         ; 9250: d0 02       ..
    inc ptr6+1                                                        ; 9252: e6 14       ..
; &9254 referenced 1 time by &9250
.c9254
    inc tmp0                                                          ; 9254: e6 85       ..
    bne c925a                                                         ; 9256: d0 02       ..
    inc tmp1                                                          ; 9258: e6 86       ..
; &925a referenced 1 time by &9256
.c925a
    cmp #&0d                                                          ; 925a: c9 0d       ..
    bne loop_c9247                                                    ; 925c: d0 e9       ..
    clc                                                               ; 925e: 18          .
; &925f referenced 1 time by &924a
.return_27
    rts                                                               ; 925f: 60          `

; &9260 referenced 1 time by &9243
.c9260
    jmp c8c8d                                                         ; 9260: 4c 8d 8c    L..

; &9263 referenced 3 times by &8f16, &8f38, &964c
.c9263
    lda l0038                                                         ; 9263: a5 38       .8
    beq c9284                                                         ; 9265: f0 1d       ..
    ldx l0021                                                         ; 9267: a6 21       .!             ; X=number of lines
    jsr print_vertical_space                                          ; 9269: 20 24 94     $.
    ldx footer_margin                                                 ; 926c: a6 25       .%             ; X=number of lines
    jsr print_vertical_space                                          ; 926e: 20 24 94     $.
    lda footers_enabled_flag                                          ; 9271: a5 28       .(
    beq c927c                                                         ; 9273: f0 07       ..
    ldx #<(footer_text_maybe)                                         ; 9275: a2 1a       ..
    ldy #>(footer_text_maybe)                                         ; 9277: a0 07       ..
    jsr render_header_or_footer                                       ; 9279: 20 10 93     ..
; &927c referenced 1 time by &9273
.c927c
    jsr print_newline                                                 ; 927c: 20 55 94     U.
    ldx bottom_margin                                                 ; 927f: a6 23       .#             ; X=number of lines
    jsr print_vertical_space                                          ; 9281: 20 24 94     $.
; &9284 referenced 1 time by &9265
.c9284
    inc register_value_p                                              ; 9284: ee b6 07    ...
    bne c928c                                                         ; 9287: d0 03       ..
    inc register_value_p+1                                            ; 9289: ee b7 07    ...
; &928c referenced 1 time by &9287
.c928c
    lda #1                                                            ; 928c: a9 01       ..
    sta register_value_l                                              ; 928e: 8d ae 07    ...
    lda #0                                                            ; 9291: a9 00       ..
    sta register_value_l+1                                            ; 9293: 8d af 07    ...
    sta l0031                                                         ; 9296: 85 31       .1
    rts                                                               ; 9298: 60          `

; ***************************************************************************************
; &9299 referenced 2 times by &8fd2, &9649
.render_new_page
    lda #&81                                                          ; 9299: a9 81       ..
    sta l0031                                                         ; 929b: 85 31       .1
    bit print_flags                                                   ; 929d: 24 69       $i
    bvc c92d4                                                         ; 929f: 50 33       P3
    jsr stop_printing                                                 ; 92a1: 20 4b 84     K.
    jsr print_inline_string                                           ; 92a4: 20 fa a7     ..
    equs &0d, "Page "                                                 ; 92a7: 0d 50 61... .Pa
    equb 0                                                            ; 92ad: 00          .

    ldx register_value_p                                              ; 92ae: ae b6 07    ...
    ldy register_value_p+1                                            ; 92b1: ac b7 07    ...
    jsr render_number_to_screen                                       ; 92b4: 20 b2 a6     ..
    jsr print_inline_string                                           ; 92b7: 20 fa a7     ..
    equs ".."                                                         ; 92ba: 2e 2e       ..
    equb 0                                                            ; 92bc: 00          .

    jsr flush_and_read_char                                           ; 92bd: 20 5a a7     Z.
    bcs c92cc                                                         ; 92c0: b0 0a       ..
    and #&df                                                          ; 92c2: 29 df       ).
    cmp #&4d ; 'M'                                                    ; 92c4: c9 4d       .M
    beq c92d4                                                         ; 92c6: f0 0c       ..
    cmp #&51 ; 'Q'                                                    ; 92c8: c9 51       .Q
    bne c92cf                                                         ; 92ca: d0 03       ..
; &92cc referenced 1 time by &92c0
.c92cc
    jmp c8f1a                                                         ; 92cc: 4c 1a 8f    L..

; &92cf referenced 1 time by &92ca
.c92cf
    lda #&c0                                                          ; 92cf: a9 c0       ..
    jsr sub_c845e                                                     ; 92d1: 20 5e 84     ^.
; &92d4 referenced 2 times by &929f, &92c6
.c92d4
    lda l0038                                                         ; 92d4: a5 38       .8
    beq c92f0                                                         ; 92d6: f0 18       ..
    ldx top_margin                                                    ; 92d8: a6 22       ."             ; X=number of lines
    jsr print_vertical_space                                          ; 92da: 20 24 94     $.
    lda headers_enabled_flag                                          ; 92dd: a5 29       .)
    beq c92e8                                                         ; 92df: f0 07       ..
    ldx #<(header_text_maybe)                                         ; 92e1: a2 d8       ..
    ldy #>(header_text_maybe)                                         ; 92e3: a0 06       ..
    jsr render_header_or_footer                                       ; 92e5: 20 10 93     ..
; &92e8 referenced 1 time by &92df
.c92e8
    jsr print_newline                                                 ; 92e8: 20 55 94     U.
    ldx header_margin                                                 ; 92eb: a6 24       .$             ; X=number of lines
    jsr print_vertical_space                                          ; 92ed: 20 24 94     $.
; &92f0 referenced 2 times by &92d6, &b141
.c92f0
    ldx page_length                                                   ; 92f0: a6 26       .&
    lda l0038                                                         ; 92f2: a5 38       .8
    beq c930d                                                         ; 92f4: f0 17       ..
    ldx #1                                                            ; 92f6: a2 01       ..
    lda page_length                                                   ; 92f8: a5 26       .&
    clc                                                               ; 92fa: 18          .
    sbc top_margin                                                    ; 92fb: e5 22       ."
    bcc c930d                                                         ; 92fd: 90 0e       ..
    sbc header_margin                                                 ; 92ff: e5 24       .$
    bcc c930d                                                         ; 9301: 90 0a       ..
    clc                                                               ; 9303: 18          .
    sbc bottom_margin                                                 ; 9304: e5 23       .#
    bcc c930d                                                         ; 9306: 90 05       ..
    sbc footer_margin                                                 ; 9308: e5 25       .%
    bcc c930d                                                         ; 930a: 90 01       ..
    tax                                                               ; 930c: aa          .
; &930d referenced 5 times by &92f4, &92fd, &9301, &9306, &930a
.c930d
    stx l0021                                                         ; 930d: 86 21       .!
    rts                                                               ; 930f: 60          `

; ***************************************************************************************
; &9310 referenced 2 times by &9279, &92e5
.render_header_or_footer
    stx tmp4                                                          ; 9310: 86 89       ..
    sty tmp5                                                          ; 9312: 84 8a       ..
    ldy #0                                                            ; 9314: a0 00       ..
    sty l0082                                                         ; 9316: 84 82       ..
    lda (tmp4),y                                                      ; 9318: b1 89       ..
    beq return_28                                                     ; 931a: f0 76       .v
    jsr sub_c9407                                                     ; 931c: 20 07 94     ..
    lda #0                                                            ; 931f: a9 00       ..
    sta l0039                                                         ; 9321: 85 39       .9
    jsr sub_c9393                                                     ; 9323: 20 93 93     ..
    jsr sub_c93fd                                                     ; 9326: 20 fd 93     ..
    bcs c932e                                                         ; 9329: b0 03       ..
    jsr sub_c93a1                                                     ; 932b: 20 a1 93     ..
; &932e referenced 1 time by &9329
.c932e
    jsr sub_c93c8                                                     ; 932e: 20 c8 93     ..
    jsr c937b                                                         ; 9331: 20 7b 93     {.
    jsr sub_c939b                                                     ; 9334: 20 9b 93     ..
    jsr sub_c93c8                                                     ; 9337: 20 c8 93     ..
    txa                                                               ; 933a: 8a          .
    beq c9355                                                         ; 933b: f0 18       ..
    dex                                                               ; 933d: ca          .
    txa                                                               ; 933e: 8a          .
    lsr a                                                             ; 933f: 4a          J
    sta l0081                                                         ; 9340: 85 81       ..
    jsr sub_c93be                                                     ; 9342: 20 be 93     ..
    beq c9355                                                         ; 9345: f0 0e       ..
    lsr a                                                             ; 9347: 4a          J
    sec                                                               ; 9348: 38          8
    sbc l0081                                                         ; 9349: e5 81       ..
    bcc c9355                                                         ; 934b: 90 08       ..
    sbc l0039                                                         ; 934d: e5 39       .9
    bcc c9355                                                         ; 934f: 90 04       ..
    tax                                                               ; 9351: aa          .
    jsr sub_c941a                                                     ; 9352: 20 1a 94     ..
; &9355 referenced 4 times by &933b, &9345, &934b, &934f
.c9355
    jsr c937b                                                         ; 9355: 20 7b 93     {.
    jsr sub_c93a1                                                     ; 9358: 20 a1 93     ..
    jsr sub_c93fd                                                     ; 935b: 20 fd 93     ..
    bcs c9363                                                         ; 935e: b0 03       ..
    jsr sub_c9393                                                     ; 9360: 20 93 93     ..
; &9363 referenced 1 time by &935e
.c9363
    jsr sub_c93c8                                                     ; 9363: 20 c8 93     ..
    jsr sub_c93be                                                     ; 9366: 20 be 93     ..
    beq c937b                                                         ; 9369: f0 10       ..
    stx l0081                                                         ; 936b: 86 81       ..
    sec                                                               ; 936d: 38          8
    sbc l0081                                                         ; 936e: e5 81       ..
    bcc c937b                                                         ; 9370: 90 09       ..
    sbc l0039                                                         ; 9372: e5 39       .9
    bcc c937b                                                         ; 9374: 90 05       ..
    tax                                                               ; 9376: aa          .
    inx                                                               ; 9377: e8          .
    jsr sub_c941a                                                     ; 9378: 20 1a 94     ..
; &937b referenced 5 times by &9331, &9355, &9369, &9370, &9374
.c937b
    ldy #0                                                            ; 937b: a0 00       ..
    ldx l0084                                                         ; 937d: a6 84       ..
    beq return_28                                                     ; 937f: f0 11       ..
; &9381 referenced 1 time by &9390
.loop_c9381
    txa                                                               ; 9381: 8a          .
    pha                                                               ; 9382: 48          H
    lda output_buffer,y                                               ; 9383: b9 54 06    .T.
    jsr sub_c9431                                                     ; 9386: 20 31 94     1.
    jsr print_char                                                    ; 9389: 20 57 94     W.
    iny                                                               ; 938c: c8          .
    pla                                                               ; 938d: 68          h
    tax                                                               ; 938e: aa          .
    dex                                                               ; 938f: ca          .
    bne loop_c9381                                                    ; 9390: d0 ef       ..
; &9392 referenced 2 times by &931a, &937f
.return_28
    rts                                                               ; 9392: 60          `

; &9393 referenced 2 times by &9323, &9360
.sub_c9393
    jsr sub_c93b6                                                     ; 9393: 20 b6 93     ..
    lda #0                                                            ; 9396: a9 00       ..
    jmp c93aa                                                         ; 9398: 4c aa 93    L..

; &939b referenced 1 time by &9334
.sub_c939b
    jsr sub_c93b6                                                     ; 939b: 20 b6 93     ..
    jmp c93a7                                                         ; 939e: 4c a7 93    L..

; &93a1 referenced 2 times by &932b, &9358
.sub_c93a1
    jsr sub_c93b6                                                     ; 93a1: 20 b6 93     ..
    jsr c93b8                                                         ; 93a4: 20 b8 93     ..
; &93a7 referenced 1 time by &939e
.c93a7
    iny                                                               ; 93a7: c8          .
    tya                                                               ; 93a8: 98          .
    dey                                                               ; 93a9: 88          .
; &93aa referenced 1 time by &9398
.c93aa
    clc                                                               ; 93aa: 18          .
    adc tmp4                                                          ; 93ab: 65 89       e.
    sta tmp2                                                          ; 93ad: 85 87       ..
    lda tmp5                                                          ; 93af: a5 8a       ..
    adc #0                                                            ; 93b1: 69 00       i.
    sta tmp3                                                          ; 93b3: 85 88       ..
    rts                                                               ; 93b5: 60          `

; &93b6 referenced 3 times by &9393, &939b, &93a1
.sub_c93b6
    ldy #&ff                                                          ; 93b6: a0 ff       ..
; &93b8 referenced 2 times by &93a4, &93bb
.c93b8
    iny                                                               ; 93b8: c8          .
    lda (tmp4),y                                                      ; 93b9: b1 89       ..
    bpl c93b8                                                         ; 93bb: 10 fb       ..
    rts                                                               ; 93bd: 60          `

; &93be referenced 2 times by &9342, &9366
.sub_c93be
    lda ruler_right_stop                                              ; 93be: a5 3e       .>
    bne return_29                                                     ; 93c0: d0 05       ..
    lda l003a                                                         ; 93c2: a5 3a       .:
    sec                                                               ; 93c4: 38          8
    sbc #1                                                            ; 93c5: e9 01       ..
; &93c7 referenced 1 time by &93c0
.return_29
    rts                                                               ; 93c7: 60          `

; &93c8 referenced 3 times by &932e, &9337, &9363
.sub_c93c8
    ldx #0                                                            ; 93c8: a2 00       ..
    ldy #0                                                            ; 93ca: a0 00       ..
    sty l0081                                                         ; 93cc: 84 81       ..
; &93ce referenced 2 times by &93e4, &93fa
.c93ce
    lda (tmp2),y                                                      ; 93ce: b1 87       ..
    bmi c93e6                                                         ; 93d0: 30 14       0.
    jsr sub_ca600                                                     ; 93d2: 20 00 a6     ..
    bne c93d9                                                         ; 93d5: d0 02       ..
    inc l0081                                                         ; 93d7: e6 81       ..
; &93d9 referenced 1 time by &93d5
.c93d9
    iny                                                               ; 93d9: c8          .
    cmp #&7c ; '|'                                                    ; 93da: c9 7c       .|
    beq c93f2                                                         ; 93dc: f0 14       ..
    sta output_buffer,x                                               ; 93de: 9d 54 06    .T.
    inx                                                               ; 93e1: e8          .
    cpx #&84                                                          ; 93e2: e0 84       ..
    bcc c93ce                                                         ; 93e4: 90 e8       ..
; &93e6 referenced 2 times by &93d0, &93f4
.c93e6
    stx l0084                                                         ; 93e6: 86 84       ..
    lda print_flags                                                   ; 93e8: a5 69       .i
    bpl return_30                                                     ; 93ea: 10 05       ..
    txa                                                               ; 93ec: 8a          .
    sec                                                               ; 93ed: 38          8
    sbc l0081                                                         ; 93ee: e5 81       ..
    tax                                                               ; 93f0: aa          .
; &93f1 referenced 1 time by &93ea
.return_30
    rts                                                               ; 93f1: 60          `

; &93f2 referenced 1 time by &93dc
.c93f2
    lda (tmp2),y                                                      ; 93f2: b1 87       ..
    bmi c93e6                                                         ; 93f4: 30 f0       0.
    iny                                                               ; 93f6: c8          .
    jsr render_date_time_to_output_buffer                             ; 93f7: 20 a7 ad     ..
    jmp c93ce                                                         ; 93fa: 4c ce 93    L..

; &93fd referenced 3 times by &9326, &935b, &9407
.sub_c93fd
    sec                                                               ; 93fd: 38          8
    lda two_sided_flag                                                ; 93fe: a5 2e       ..
    beq return_31                                                     ; 9400: f0 04       ..
    lda register_value_p                                              ; 9402: ad b6 07    ...
    lsr a                                                             ; 9405: 4a          J
; &9406 referenced 1 time by &9400
.return_31
    rts                                                               ; 9406: 60          `

; &9407 referenced 2 times by &8fd5, &931c
.sub_c9407
    jsr sub_c93fd                                                     ; 9407: 20 fd 93     ..
    lda left_margin                                                   ; 940a: a5 2f       ./
    bcc c9415                                                         ; 940c: 90 07       ..
    ldx two_sided_flag                                                ; 940e: a6 2e       ..
    beq c9415                                                         ; 9410: f0 03       ..
    clc                                                               ; 9412: 18          .
    adc rhs_extra_margin                                              ; 9413: 65 2c       e,
; &9415 referenced 2 times by &940c, &9410
.c9415
    tax                                                               ; 9415: aa          .
    lda #&20 ; ' '                                                    ; 9416: a9 20       .
    bne c9426                                                         ; 9418: d0 0c       ..             ; ALWAYS branch

; &941a referenced 2 times by &9352, &9378
.sub_c941a
    txa                                                               ; 941a: 8a          .
    clc                                                               ; 941b: 18          .
    adc l0039                                                         ; 941c: 65 39       e9
    sta l0039                                                         ; 941e: 85 39       .9
    lda #&20 ; ' '                                                    ; 9420: a9 20       .
    bne c9426                                                         ; 9422: d0 02       ..             ; ALWAYS branch

; ***************************************************************************************
; On Entry:
;     X: number of lines
; ***************************************************************************************
; &9424 referenced 6 times by &900b, &9269, &926e, &9281, &92da, &92ed
.print_vertical_space
    lda #&0d                                                          ; 9424: a9 0d       ..
; &9426 referenced 4 times by &8fec, &913c, &9418, &9422
.c9426
    inx                                                               ; 9426: e8          .
    dex                                                               ; 9427: ca          .
    beq return_32                                                     ; 9428: f0 06       ..
; &942a referenced 1 time by &942e
.loop_c942a
    jsr print_char                                                    ; 942a: 20 57 94     W.
    dex                                                               ; 942d: ca          .
    bne loop_c942a                                                    ; 942e: d0 fa       ..
; &9430 referenced 1 time by &9428
.return_32
    rts                                                               ; 9430: 60          `

; &9431 referenced 4 times by &8fe9, &904c, &912f, &9386
.sub_c9431
    jsr sub_ca5ae                                                     ; 9431: 20 ae a5     ..
    bit print_flags                                                   ; 9434: 24 69       $i
    bpl c943c                                                         ; 9436: 10 04       ..
    ora #0                                                            ; 9438: 09 00       ..
    bmi return_33                                                     ; 943a: 30 08       0.             ; ALWAYS branch

; &943c referenced 1 time by &9436
.c943c
    pha                                                               ; 943c: 48          H
    txa                                                               ; 943d: 8a          .
    clc                                                               ; 943e: 18          .
    adc l0039                                                         ; 943f: 65 39       e9
    sta l0039                                                         ; 9441: 85 39       .9
    pla                                                               ; 9443: 68          h
; &9444 referenced 1 time by &943a
.return_33
    rts                                                               ; 9444: 60          `

; &9445 referenced 2 times by &9177, &9468
.sub_c9445
    pha                                                               ; 9445: 48          H
    lda print_xpos                                                    ; 9446: a5 78       .x
    beq c9453                                                         ; 9448: f0 09       ..
    lda #&20 ; ' '                                                    ; 944a: a9 20       .
; &944c referenced 1 time by &9451
.loop_c944c
    jsr print_char_just_to_printer                                    ; 944c: 20 6b 94     k.
    dec print_xpos                                                    ; 944f: c6 78       .x
    bne loop_c944c                                                    ; 9451: d0 f9       ..
; &9453 referenced 1 time by &9448
.c9453
    pla                                                               ; 9453: 68          h
    rts                                                               ; 9454: 60          `

; ***************************************************************************************
; &9455 referenced 2 times by &927c, &92e8
.print_newline
    lda #&0d                                                          ; 9455: a9 0d       ..
; ***************************************************************************************
; &9457 referenced 3 times by &9160, &9389, &942a
.print_char
    cmp #&0d                                                          ; 9457: c9 0d       ..
    beq c9462                                                         ; 9459: f0 07       ..
    cmp #&20 ; ' '                                                    ; 945b: c9 20       .
    bne c9468                                                         ; 945d: d0 09       ..
    inc print_xpos                                                    ; 945f: e6 78       .x
    rts                                                               ; 9461: 60          `

; &9462 referenced 1 time by &9459
.c9462
    lda #0                                                            ; 9462: a9 00       ..
    sta print_xpos                                                    ; 9464: 85 78       .x
    lda #&0d                                                          ; 9466: a9 0d       ..
; &9468 referenced 1 time by &945d
.c9468
    jsr sub_c9445                                                     ; 9468: 20 45 94     E.
; ***************************************************************************************
; &946b referenced 1 time by &944c
.print_char_just_to_printer
    bit print_flags                                                   ; 946b: 24 69       $i
    bpl c9472                                                         ; 946d: 10 03       ..
    jmp (printer_driver_ptr)                                          ; 946f: 6c 17 00    l..

; &9472 referenced 1 time by &946d
.c9472
    jsr sub_ca600                                                     ; 9472: 20 00 a6     ..
    bne c9488                                                         ; 9475: d0 11       ..
    pha                                                               ; 9477: 48          H
    lda #&2d ; '-'                                                    ; 9478: a9 2d       .-
    bcs c947e                                                         ; 947a: b0 02       ..
    lda #&2a ; '*'                                                    ; 947c: a9 2a       .*
; &947e referenced 1 time by &947a
.c947e
    jsr set_inverted_text_if_not_mode_7                               ; 947e: 20 77 a5     w.
    jsr osasci                                                        ; 9481: 20 e3 ff     ..            ; Write character
    pla                                                               ; 9484: 68          h
    jmp set_normal_text_if_not_mode_7                                 ; 9485: 4c 65 a5    Le.

; &9488 referenced 1 time by &9475
.c9488
    jmp osasci                                                        ; 9488: 4c e3 ff    L..            ; Write character

; ***************************************************************************************
; &948b referenced 2 times by &8460, &85f8
.prepare_printer_driver
    ldx #<printer_driver_block                                        ; 948b: a2 00       ..
    ldy #>printer_driver_block                                        ; 948d: a0 04       ..
    lda printer_driver_name                                           ; 948f: ad 84 07    ...
    bne c949e                                                         ; 9492: d0 0a       ..
    ldx default_printer_driver_ptr                                    ; 9494: ae b1 94    ...
    ldy l94b2                                                         ; 9497: ac b2 94    ...
    lda #0                                                            ; 949a: a9 00       ..
    sta microspacing_flag                                             ; 949c: 85 6c       .l
; &949e referenced 1 time by &9492
.c949e
    stx printer_driver_ptr                                            ; 949e: 86 17       ..
    sty printer_driver_ptr+1                                          ; 94a0: 84 18       ..
; &94a2 referenced 1 time by &94c2
.return_35
    rts                                                               ; 94a2: 60          `

; ***************************************************************************************
; &94a3 referenced 4 times by &8456, &8465, &860c, &917f
.call_printer_driver
    clc                                                               ; 94a3: 18          .
    adc printer_driver_ptr                                            ; 94a4: 65 17       e.
    sta tmp8                                                          ; 94a6: 85 8d       ..
    lda printer_driver_ptr+1                                          ; 94a8: a5 18       ..
    adc #0                                                            ; 94aa: 69 00       i.
    sta tmp9                                                          ; 94ac: 85 8e       ..
    jmp (tmp8)                                                        ; 94ae: 6c 8d 00    l..

; &94b1 referenced 1 time by &9494
.default_printer_driver_ptr
l94b2 = default_printer_driver_ptr+1
    equw default_printer_driver                                       ; 94b1: b3 94       ..
; &94b2 referenced 1 time by &9497

; ***************************************************************************************
.default_printer_driver
    jmp c94c0                                                         ; 94b3: 4c c0 94    L..

    jmp c94c7                                                         ; 94b6: 4c c7 94    L..

    jmp c94cb                                                         ; 94b9: 4c cb 94    L..

    jmp return_34                                                     ; 94bc: 4c f9 a7    L..

    rts                                                               ; 94bf: 60          `

; &94c0 referenced 1 time by &94b3
.c94c0
    cmp #&80                                                          ; 94c0: c9 80       ..
    bcs return_35                                                     ; 94c2: b0 de       ..
    jmp osasci                                                        ; 94c4: 4c e3 ff    L..            ; Write character

; &94c7 referenced 1 time by &94b6
.c94c7
    lda #2                                                            ; 94c7: a9 02       ..
    bne c94cd                                                         ; 94c9: d0 02       ..             ; ALWAYS branch

; &94cb referenced 1 time by &94b9
.c94cb
    lda #3                                                            ; 94cb: a9 03       ..
; &94cd referenced 1 time by &94c9
.c94cd
    jmp oswrch                                                        ; 94cd: 4c ee ff    L..            ; Write character 3

; ***************************************************************************************
.lj_fmt_cmd
    jsr expand_line                                                   ; 94d0: 20 2b 95     +.
    bcc return_36                                                     ; 94d3: 90 55       .U
    lda #0                                                            ; 94d5: a9 00       ..
    beq c950f                                                         ; 94d7: f0 36       .6             ; ALWAYS branch

; ***************************************************************************************
.ce_fmt_cmd
    jsr expand_line                                                   ; 94d9: 20 2b 95     +.
    bcc return_36                                                     ; 94dc: 90 4c       .L
    txa                                                               ; 94de: 8a          .
    beq return_36                                                     ; 94df: f0 49       .I
    lsr a                                                             ; 94e1: 4a          J
    sta l0084                                                         ; 94e2: 85 84       ..
    lda ruler_right_stop                                              ; 94e4: a5 3e       .>
    beq c950f                                                         ; 94e6: f0 27       .'
    sec                                                               ; 94e8: 38          8
    sbc ruler_left_stop                                               ; 94e9: e5 3f       .?
    lsr a                                                             ; 94eb: 4a          J
    sec                                                               ; 94ec: 38          8
    adc ruler_left_stop                                               ; 94ed: 65 3f       e?
    sec                                                               ; 94ef: 38          8
    sbc l0084                                                         ; 94f0: e5 84       ..
    bcs c950f                                                         ; 94f2: b0 1b       ..
    lda #0                                                            ; 94f4: a9 00       ..
    beq c950f                                                         ; 94f6: f0 17       ..             ; ALWAYS branch

; ***************************************************************************************
.rj_fmt_cmd
    jsr expand_line                                                   ; 94f8: 20 2b 95     +.
    bcc c9529                                                         ; 94fb: 90 2c       .,
    txa                                                               ; 94fd: 8a          .
    beq c9529                                                         ; 94fe: f0 29       .)
    dex                                                               ; 9500: ca          .
    dex                                                               ; 9501: ca          .
    lda #0                                                            ; 9502: a9 00       ..
    cpx ruler_right_stop                                              ; 9504: e4 3e       .>
    bcs c950f                                                         ; 9506: b0 07       ..
    stx l0083                                                         ; 9508: 86 83       ..
    lda ruler_right_stop                                              ; 950a: a5 3e       .>
    sec                                                               ; 950c: 38          8
    sbc l0083                                                         ; 950d: e5 83       ..
; &950f referenced 5 times by &94d7, &94e6, &94f2, &94f6, &9506
.c950f
    ldy #3                                                            ; 950f: a0 03       ..
    tax                                                               ; 9511: aa          .
    beq c951c                                                         ; 9512: f0 08       ..
    lda #&20 ; ' '                                                    ; 9514: a9 20       .
; &9516 referenced 1 time by &951a
.loop_c9516
    sta (current_format_line_ptr),y                                   ; 9516: 91 04       ..
    iny                                                               ; 9518: c8          .
    dex                                                               ; 9519: ca          .
    bne loop_c9516                                                    ; 951a: d0 fa       ..
; &951c referenced 2 times by &9512, &9525
.c951c
    lda output_buffer,x                                               ; 951c: bd 54 06    .T.
    sta (current_format_line_ptr),y                                   ; 951f: 91 04       ..
    iny                                                               ; 9521: c8          .
    inx                                                               ; 9522: e8          .
    cmp #&0d                                                          ; 9523: c9 0d       ..
    bne c951c                                                         ; 9525: d0 f5       ..
    inc l0030                                                         ; 9527: e6 30       .0
; &9529 referenced 2 times by &94fb, &94fe
.c9529
    sec                                                               ; 9529: 38          8
; &952a referenced 3 times by &94d3, &94dc, &94df
.return_36
    rts                                                               ; 952a: 60          `

; ***************************************************************************************
; &952b referenced 3 times by &94d0, &94d9, &94f8
.expand_line
    ldx #0                                                            ; 952b: a2 00       ..
    stx l0083                                                         ; 952d: 86 83       ..
    ldy #3                                                            ; 952f: a0 03       ..
    jsr get_current_fmt_cmd_byte                                      ; 9531: 20 25 98     %.
    clc                                                               ; 9534: 18          .
    beq return_37                                                     ; 9535: f0 26       .&
; &9537 referenced 2 times by &954f, &9568
.c9537
    lda (current_format_line_ptr),y                                   ; 9537: b1 04       ..
    iny                                                               ; 9539: c8          .
    cmp #&7c ; '|'                                                    ; 953a: c9 7c       .|
    beq c955e                                                         ; 953c: f0 20       .
; &953e referenced 2 times by &9553, &9562
.c953e
    sta output_buffer,x                                               ; 953e: 9d 54 06    .T.
    jsr sub_ca600                                                     ; 9541: 20 00 a6     ..
    bne c9548                                                         ; 9544: d0 02       ..
    inc l0083                                                         ; 9546: e6 83       ..
; &9548 referenced 1 time by &9544
.c9548
    inx                                                               ; 9548: e8          .
    cmp #&0d                                                          ; 9549: c9 0d       ..
    beq c9555                                                         ; 954b: f0 08       ..
    cpx #&83                                                          ; 954d: e0 83       ..
    bcc c9537                                                         ; 954f: 90 e6       ..
    lda #&0d                                                          ; 9551: a9 0d       ..
    bne c953e                                                         ; 9553: d0 e9       ..             ; ALWAYS branch

; &9555 referenced 1 time by &954b
.c9555
    lda print_flags                                                   ; 9555: a5 69       .i
    bpl return_37                                                     ; 9557: 10 04       ..
    txa                                                               ; 9559: 8a          .
    sbc l0083                                                         ; 955a: e5 83       ..
    tax                                                               ; 955c: aa          .
; &955d referenced 2 times by &9535, &9557
.return_37
    rts                                                               ; 955d: 60          `

; &955e referenced 1 time by &953c
.c955e
    lda (current_format_line_ptr),y                                   ; 955e: b1 04       ..
    cmp #&0d                                                          ; 9560: c9 0d       ..
    beq c953e                                                         ; 9562: f0 da       ..
    iny                                                               ; 9564: c8          .
    jsr render_date_time_to_output_buffer                             ; 9565: 20 a7 ad     ..
    jmp c9537                                                         ; 9568: 4c 37 95    L7.

; ***************************************************************************************
.dh_fmt_cmd
    ldx #<(header_text_maybe)                                         ; 956b: a2 d8       ..
    ldy #>(header_text_maybe)                                         ; 956d: a0 06       ..
    bne c9575                                                         ; 956f: d0 04       ..             ; ALWAYS branch

; ***************************************************************************************
.df_fmt_cmd
    ldx #<(footer_text_maybe)                                         ; 9571: a2 1a       ..
    ldy #>(footer_text_maybe)                                         ; 9573: a0 07       ..
; &9575 referenced 1 time by &956f
.c9575
    stx tmp2                                                          ; 9575: 86 87       ..
    sty tmp3                                                          ; 9577: 84 88       ..
    lda #0                                                            ; 9579: a9 00       ..
    sta l0081                                                         ; 957b: 85 81       ..
    sta l007a                                                         ; 957d: 85 7a       .z
    ldy #3                                                            ; 957f: a0 03       ..
    sty input_buffer_ptr+1                                            ; 9581: 84 80       ..
    lda (current_format_line_ptr),y                                   ; 9583: b1 04       ..
    sta l0083                                                         ; 9585: 85 83       ..
    ldx #&3f ; '?'                                                    ; 9587: a2 3f       .?
; &9589 referenced 1 time by &95a8
.loop_c9589
    iny                                                               ; 9589: c8          .
    sty l0082                                                         ; 958a: 84 82       ..
    lda (current_format_line_ptr),y                                   ; 958c: b1 04       ..
    cmp #&0d                                                          ; 958e: c9 0d       ..
    beq c959c                                                         ; 9590: f0 0a       ..
    cmp #&1b                                                          ; 9592: c9 1b       ..
    bcs c9598                                                         ; 9594: b0 02       ..
    lda #&20 ; ' '                                                    ; 9596: a9 20       .
; &9598 referenced 1 time by &9594
.c9598
    cmp l0083                                                         ; 9598: c5 83       ..
    bne c959e                                                         ; 959a: d0 02       ..
; &959c referenced 1 time by &9590
.c959c
    ora #&80                                                          ; 959c: 09 80       ..
; &959e referenced 1 time by &959a
.c959e
    jsr sub_c95b2                                                     ; 959e: 20 b2 95     ..
    cmp #&8d                                                          ; 95a1: c9 8d       ..
    beq c95aa                                                         ; 95a3: f0 05       ..
    ldy l0082                                                         ; 95a5: a4 82       ..
    dex                                                               ; 95a7: ca          .
    bne loop_c9589                                                    ; 95a8: d0 df       ..
; &95aa referenced 1 time by &95a3
.c95aa
    lda #&80                                                          ; 95aa: a9 80       ..
    jsr sub_c95b2                                                     ; 95ac: 20 b2 95     ..
    jsr sub_c95b2                                                     ; 95af: 20 b2 95     ..
; &95b2 referenced 3 times by &959e, &95ac, &95af
.sub_c95b2
    ldy l0081                                                         ; 95b2: a4 81       ..
    sta (tmp2),y                                                      ; 95b4: 91 87       ..
    iny                                                               ; 95b6: c8          .
    sty l0081                                                         ; 95b7: 84 81       ..
    rts                                                               ; 95b9: 60          `

; ***************************************************************************************
.em_fmt_cmd
    ldy #3                                                            ; 95ba: a0 03       ..
    jsr get_current_fmt_cmd_byte                                      ; 95bc: 20 25 98     %.
    beq return_38                                                     ; 95bf: f0 1a       ..
    iny                                                               ; 95c1: c8          .
    jsr get_register_address                                          ; 95c2: 20 69 ad     i.
    bcs return_38                                                     ; 95c5: b0 14       ..
    lda tmp6                                                          ; 95c7: a5 8b       ..
    sta tmp0                                                          ; 95c9: 85 85       ..
    lda tmp7                                                          ; 95cb: a5 8c       ..
    sta tmp1                                                          ; 95cd: 85 86       ..
    jsr evaluate_expression_from_fmt_cmd                              ; 95cf: 20 b8 97     ..
    ldy #0                                                            ; 95d2: a0 00       ..
    sta (tmp0),y                                                      ; 95d4: 91 85       ..
    iny                                                               ; 95d6: c8          .              ; Y=&01
    lda tmp9                                                          ; 95d7: a5 8e       ..
    sta (tmp0),y                                                      ; 95d9: 91 85       ..
; &95db referenced 2 times by &95bf, &95c5
.return_38
    rts                                                               ; 95db: 60          `

; ***************************************************************************************
.pl_fmt_cmd
    ldy #3                                                            ; 95dc: a0 03       ..
    jsr evaluate_expression_from_fmt_cmd                              ; 95de: 20 b8 97     ..
    sta page_length                                                   ; 95e1: 85 26       .&
    rts                                                               ; 95e3: 60          `

; ***************************************************************************************
.ts_fmt_cmd
    ldy #3                                                            ; 95e4: a0 03       ..
    jsr parse_boolean_from_fmt_cmd                                    ; 95e6: 20 62 97     b.
    bcs return_39                                                     ; 95e9: b0 07       ..
    sta two_sided_flag                                                ; 95eb: 85 2e       ..
    jsr evaluate_expression_from_fmt_cmd                              ; 95ed: 20 b8 97     ..
    sta rhs_extra_margin                                              ; 95f0: 85 2c       .,
; &95f2 referenced 1 time by &95e9
.return_39
    rts                                                               ; 95f2: 60          `

; ***************************************************************************************
.tm_fmt_cmd
    ldy #3                                                            ; 95f3: a0 03       ..
    jsr evaluate_expression_from_fmt_cmd                              ; 95f5: 20 b8 97     ..
    sta top_margin                                                    ; 95f8: 85 22       ."
    rts                                                               ; 95fa: 60          `

; ***************************************************************************************
.bm_fmt_cmd
    ldy #3                                                            ; 95fb: a0 03       ..
    jsr evaluate_expression_from_fmt_cmd                              ; 95fd: 20 b8 97     ..
    sta bottom_margin                                                 ; 9600: 85 23       .#
    rts                                                               ; 9602: 60          `

; ***************************************************************************************
.hm_fmt_cmd
    ldy #3                                                            ; 9603: a0 03       ..
    jsr evaluate_expression_from_fmt_cmd                              ; 9605: 20 b8 97     ..
    sta header_margin                                                 ; 9608: 85 24       .$
    rts                                                               ; 960a: 60          `

; ***************************************************************************************
.fm_fmt_cmd
    ldy #3                                                            ; 960b: a0 03       ..
    jsr evaluate_expression_from_fmt_cmd                              ; 960d: 20 b8 97     ..
    sta footer_margin                                                 ; 9610: 85 25       .%
    rts                                                               ; 9612: 60          `

; ***************************************************************************************
.lm_fmt_cmd
    ldy #3                                                            ; 9613: a0 03       ..
    jsr evaluate_expression_from_fmt_cmd                              ; 9615: 20 b8 97     ..
    sta left_margin                                                   ; 9618: 85 2f       ./
    rts                                                               ; 961a: 60          `

; ***************************************************************************************
.ls_fmt_cmd
    ldy #3                                                            ; 961b: a0 03       ..
    jsr evaluate_expression_from_fmt_cmd                              ; 961d: 20 b8 97     ..
    sta line_spacing                                                  ; 9620: 85 27       .'
    rts                                                               ; 9622: 60          `

; ***************************************************************************************
.pe_fmt_cmd
    ldy #3                                                            ; 9623: a0 03       ..
    jsr evaluate_expression_from_fmt_cmd                              ; 9625: 20 b8 97     ..
    tax                                                               ; 9628: aa          .
    beq page_eject_fmt                                                ; 9629: f0 1a       ..
    cmp l0021                                                         ; 962b: c5 21       .!
    bcc return_40                                                     ; 962d: 90 04       ..
    lda l0031                                                         ; 962f: a5 31       .1
    bne page_eject_fmt                                                ; 9631: d0 12       ..
; &9633 referenced 1 time by &962d
.return_40
    rts                                                               ; 9633: 60          `

; ***************************************************************************************
.op_fmt_cmd
    lda register_value_p                                              ; 9634: ad b6 07    ...
    lsr a                                                             ; 9637: 4a          J
    bcc page_eject_fmt                                                ; 9638: 90 0b       ..
    bcs c9642                                                         ; 963a: b0 06       ..             ; ALWAYS branch

; ***************************************************************************************
.ep_fmt_cmd
    lda register_value_p                                              ; 963c: ad b6 07    ...
    lsr a                                                             ; 963f: 4a          J
    bcs page_eject_fmt                                                ; 9640: b0 03       ..
; &9642 referenced 1 time by &963a
.c9642
    jsr page_eject_fmt                                                ; 9642: 20 45 96     E.
; ***************************************************************************************
; &9645 referenced 5 times by &9629, &9631, &9638, &9640, &9642
.page_eject_fmt
    lda l0031                                                         ; 9645: a5 31       .1
    bne c964c                                                         ; 9647: d0 03       ..
    jsr render_new_page                                               ; 9649: 20 99 92     ..
; &964c referenced 1 time by &9647
.c964c
    jmp c9263                                                         ; 964c: 4c 63 92    Lc.

; ***************************************************************************************
.fo_fmt_cmd
    ldy #3                                                            ; 964f: a0 03       ..
    jsr parse_boolean_from_fmt_cmd                                    ; 9651: 20 62 97     b.
    bcs return_41                                                     ; 9654: b0 02       ..
    sta footers_enabled_flag                                          ; 9656: 85 28       .(
; &9658 referenced 1 time by &9654
.return_41
    rts                                                               ; 9658: 60          `

; ***************************************************************************************
.he_fmt_cmd
    ldy #3                                                            ; 9659: a0 03       ..
    jsr parse_boolean_from_fmt_cmd                                    ; 965b: 20 62 97     b.
    bcs return_42                                                     ; 965e: b0 02       ..
    sta headers_enabled_flag                                          ; 9660: 85 29       .)
; &9662 referenced 2 times by &965e, &966f
.return_42
    rts                                                               ; 9662: 60          `

; ***************************************************************************************
.pb_fmt_cmd
    ldy #3                                                            ; 9663: a0 03       ..
    jsr parse_boolean_from_fmt_cmd                                    ; 9665: 20 62 97     b.
    bcs return_43                                                     ; 9668: b0 02       ..
    sta l0038                                                         ; 966a: 85 38       .8
; &966c referenced 1 time by &9668
.return_43
    rts                                                               ; 966c: 60          `

; ***************************************************************************************
.dm_fmt_cmd
    lda macro_executing_flag                                          ; 966d: a5 2d       .-
    bne return_42                                                     ; 966f: d0 f1       ..
    lda last_macro_ptr                                                ; 9671: a5 1b       ..
    sta tmp6                                                          ; 9673: 85 8b       ..
    lda last_macro_ptr+1                                              ; 9675: a5 1c       ..
    sta tmp7                                                          ; 9677: 85 8c       ..
    ldy #3                                                            ; 9679: a0 03       ..
    lda (current_format_line_ptr),y                                   ; 967b: b1 04       ..
    and #&df                                                          ; 967d: 29 df       ).
    sta l0084                                                         ; 967f: 85 84       ..
    iny                                                               ; 9681: c8          .              ; Y=&04
    lda (current_format_line_ptr),y                                   ; 9682: b1 04       ..
    jsr is_uppercase                                                  ; 9684: 20 6b 8c     k.
    bcc c968d                                                         ; 9687: 90 04       ..
    lda #&20 ; ' '                                                    ; 9689: a9 20       .
    bne c968f                                                         ; 968b: d0 02       ..             ; ALWAYS branch

; &968d referenced 1 time by &9687
.c968d
    and #&df                                                          ; 968d: 29 df       ).
; &968f referenced 1 time by &968b
.c968f
    dey                                                               ; 968f: 88          .
    sta (last_macro_ptr),y                                            ; 9690: 91 1b       ..
    dey                                                               ; 9692: 88          .
    lda l0084                                                         ; 9693: a5 84       ..
    sta (last_macro_ptr),y                                            ; 9695: 91 1b       ..
    lda #4                                                            ; 9697: a9 04       ..
    clc                                                               ; 9699: 18          .
    adc last_macro_ptr                                                ; 969a: 65 1b       e.
    sta last_macro_ptr                                                ; 969c: 85 1b       ..
    bcc c96a2                                                         ; 969e: 90 02       ..
    inc last_macro_ptr+1                                              ; 96a0: e6 1c       ..
; &96a2 referenced 2 times by &969e, &9700
.c96a2
    lda himem                                                         ; 96a2: a5 0f       ..
    sec                                                               ; 96a4: 38          8
    sbc last_macro_ptr                                                ; 96a5: e5 1b       ..
    tax                                                               ; 96a7: aa          .
    lda himem+1                                                       ; 96a8: a5 10       ..
    sbc last_macro_ptr+1                                              ; 96aa: e5 1c       ..
    bne c96b8                                                         ; 96ac: d0 0a       ..
    cpx #&97                                                          ; 96ae: e0 97       ..
    bcs c96b8                                                         ; 96b0: b0 06       ..
    jsr sub_c902c                                                     ; 96b2: 20 2c 90     ,.
    jmp c853f                                                         ; 96b5: 4c 3f 85    L?.

; &96b8 referenced 2 times by &96ac, &96b0
.c96b8
    lda last_macro_ptr                                                ; 96b8: a5 1b       ..
    sta tmp0                                                          ; 96ba: 85 85       ..
    sta input_buffer_ptr+1                                            ; 96bc: 85 80       ..
    sta current_format_line_ptr                                       ; 96be: 85 04       ..
    lda last_macro_ptr+1                                              ; 96c0: a5 1c       ..
    sta tmp1                                                          ; 96c2: 85 86       ..
    sta l0081                                                         ; 96c4: 85 81       ..
    sta current_format_line_ptr+1                                     ; 96c6: 85 05       ..
    jsr sub_c9241                                                     ; 96c8: 20 41 92     A.
    bcc c96ce                                                         ; 96cb: 90 01       ..
    rts                                                               ; 96cd: 60          `

; &96ce referenced 1 time by &96cb
.c96ce
    ldy #0                                                            ; 96ce: a0 00       ..
    lda (last_macro_ptr),y                                            ; 96d0: b1 1b       ..
    jsr sub_caf71                                                     ; 96d2: 20 71 af     q.
    bne c96f8                                                         ; 96d5: d0 21       .!
    jsr lookup_formatting_command                                     ; 96d7: 20 30 97     0.
    cpx #5                                                            ; 96da: e0 05       ..
    bne c96f8                                                         ; 96dc: d0 1a       ..
    lda #4                                                            ; 96de: a9 04       ..
    ldy #0                                                            ; 96e0: a0 00       ..
    sta (last_macro_ptr),y                                            ; 96e2: 91 1b       ..
    inc last_macro_ptr                                                ; 96e4: e6 1b       ..
    bne add_macro_to_linked_list                                      ; 96e6: d0 02       ..
    inc last_macro_ptr+1                                              ; 96e8: e6 1c       ..
; &96ea referenced 1 time by &96e6
.add_macro_to_linked_list
    lda #0                                                            ; 96ea: a9 00       ..
    sta (last_macro_ptr),y                                            ; 96ec: 91 1b       ..
    lda last_macro_ptr                                                ; 96ee: a5 1b       ..
    sta (tmp6),y                                                      ; 96f0: 91 8b       ..
    iny                                                               ; 96f2: c8          .
    lda last_macro_ptr+1                                              ; 96f3: a5 1c       ..
    sta (tmp6),y                                                      ; 96f5: 91 8b       ..
    rts                                                               ; 96f7: 60          `

; &96f8 referenced 2 times by &96d5, &96dc
.c96f8
    lda tmp0                                                          ; 96f8: a5 85       ..
    sta last_macro_ptr                                                ; 96fa: 85 1b       ..
    lda tmp1                                                          ; 96fc: a5 86       ..
    sta last_macro_ptr+1                                              ; 96fe: 85 1c       ..
    bne c96a2                                                         ; 9700: d0 a0       ..
; ***************************************************************************************
.ht_fmt_cmd
    ldy #3                                                            ; 9702: a0 03       ..
    jsr get_current_fmt_cmd_byte                                      ; 9704: 20 25 98     %.
    beq return_44                                                     ; 9707: f0 26       .&
    tax                                                               ; 9709: aa          .
    lda #0                                                            ; 970a: a9 00       ..
    cpx #&2d ; '-'                                                    ; 970c: e0 2d       .-
    beq c9716                                                         ; 970e: f0 06       ..
    lda #1                                                            ; 9710: a9 01       ..
    cpx #&2a ; '*'                                                    ; 9712: e0 2a       .*
    bne c9719                                                         ; 9714: d0 03       ..
; &9716 referenced 1 time by &970e
.c9716
    iny                                                               ; 9716: c8          .
    bne c9725                                                         ; 9717: d0 0c       ..
; &9719 referenced 1 time by &9714
.c9719
    jsr evaluate_expression_from_fmt_cmd                              ; 9719: 20 b8 97     ..
    sec                                                               ; 971c: 38          8
    sbc #1                                                            ; 971d: e9 01       ..
    bcc return_44                                                     ; 971f: 90 0e       ..
    cmp #2                                                            ; 9721: c9 02       ..
    bcs return_44                                                     ; 9723: b0 0a       ..
; &9725 referenced 1 time by &9717
.c9725
    pha                                                               ; 9725: 48          H
    jsr evaluate_expression_from_fmt_cmd                              ; 9726: 20 b8 97     ..
    pla                                                               ; 9729: 68          h
    tax                                                               ; 972a: aa          .
    lda tmp8                                                          ; 972b: a5 8d       ..
    sta highlight1_code,x                                             ; 972d: 95 2a       .*
; &972f referenced 3 times by &9707, &971f, &9723
.return_44
    rts                                                               ; 972f: 60          `

; ***************************************************************************************
; &9730 referenced 2 times by &8f6e, &96d7
.lookup_formatting_command
    ldy #2                                                            ; 9730: a0 02       ..
    lda (current_format_line_ptr),y                                   ; 9732: b1 04       ..
    sta tmp3                                                          ; 9734: 85 88       ..
    dey                                                               ; 9736: 88          .              ; Y=&01
    lda (current_format_line_ptr),y                                   ; 9737: b1 04       ..
    sta tmp2                                                          ; 9739: 85 87       ..
    dey                                                               ; 973b: 88          .              ; Y=&00
    ldx #0                                                            ; 973c: a2 00       ..
; &973e referenced 1 time by &9752
.loop_c973e
    lda tmp2                                                          ; 973e: a5 87       ..
    cmp commands_table,y                                              ; 9740: d9 a0 b2    ...
    bne c974c                                                         ; 9743: d0 07       ..
    lda tmp3                                                          ; 9745: a5 88       ..
    cmp lb2a1,y                                                       ; 9747: d9 a1 b2    ...
    beq return_45                                                     ; 974a: f0 08       ..
; &974c referenced 1 time by &9743
.c974c
    inx                                                               ; 974c: e8          .
    iny                                                               ; 974d: c8          .
    iny                                                               ; 974e: c8          .
    lda commands_table,y                                              ; 974f: b9 a0 b2    ...
    bpl loop_c973e                                                    ; 9752: 10 ea       ..
; &9754 referenced 1 time by &974a
.return_45
    rts                                                               ; 9754: 60          `

; ***************************************************************************************
; &9755 referenced 1 time by &8f73
.execute_formatting_command
    txa                                                               ; 9755: 8a          .
    ldy #2                                                            ; 9756: a0 02       ..
    ldx #0                                                            ; 9758: a2 00       ..
    stx l0030                                                         ; 975a: 86 30       .0
    jsr call_through_jumptable                                        ; 975c: 20 92 a8     ..
    ldx l0030                                                         ; 975f: a6 30       .0
    rts                                                               ; 9761: 60          `

; ***************************************************************************************
; &9762 referenced 4 times by &95e6, &9651, &965b, &9665
.parse_boolean_from_fmt_cmd
    jsr get_current_fmt_cmd_byte                                      ; 9762: 20 25 98     %.
    sec                                                               ; 9765: 38          8
    beq return_46                                                     ; 9766: f0 47       .G
    lda current_format_line_ptr                                       ; 9768: a5 04       ..
    ldx current_format_line_ptr+1                                     ; 976a: a6 05       ..
; &976c referenced 1 time by &87a6
.sub_c976c
    sta tmp8                                                          ; 976c: 85 8d       ..
    stx tmp9                                                          ; 976e: 86 8e       ..
    lda (tmp8),y                                                      ; 9770: b1 8d       ..
    tax                                                               ; 9772: aa          .
    lda #1                                                            ; 9773: a9 01       ..
    cpx #&31 ; '1'                                                    ; 9775: e0 31       .1
    beq c977f                                                         ; 9777: f0 06       ..
    lda #0                                                            ; 9779: a9 00       ..
    cpx #&30 ; '0'                                                    ; 977b: e0 30       .0
    bne c9783                                                         ; 977d: d0 04       ..
; &977f referenced 1 time by &9777
.c977f
    clc                                                               ; 977f: 18          .
    iny                                                               ; 9780: c8          .
    bne return_46                                                     ; 9781: d0 2c       .,
; &9783 referenced 1 time by &977d
.c9783
    dey                                                               ; 9783: 88          .
    sty l0084                                                         ; 9784: 84 84       ..
    ldx #&ff                                                          ; 9786: a2 ff       ..
; &9788 referenced 2 times by &9792, &97ac
.c9788
    iny                                                               ; 9788: c8          .
    lda (tmp8),y                                                      ; 9789: b1 8d       ..
    jsr sub_c8c63                                                     ; 978b: 20 63 8c     c.
    inx                                                               ; 978e: e8          .
    cmp l97b0,x                                                       ; 978f: dd b0 97    ...
    beq c9788                                                         ; 9792: f0 f4       ..
    lda l97b0,x                                                       ; 9794: bd b0 97    ...
    bmi c97ae                                                         ; 9797: 30 15       0.
    cmp #&20 ; ' '                                                    ; 9799: c9 20       .
    bcc return_46                                                     ; 979b: 90 12       ..
; &979d referenced 1 time by &97a5
.loop_c979d
    inx                                                               ; 979d: e8          .
    lda l97b0,x                                                       ; 979e: bd b0 97    ...
    bmi c97ae                                                         ; 97a1: 30 0b       0.
    cmp #&20 ; ' '                                                    ; 97a3: c9 20       .
    bcs loop_c979d                                                    ; 97a5: b0 f6       ..
    ldy l0084                                                         ; 97a7: a4 84       ..
    lda l97b1,x                                                       ; 97a9: bd b1 97    ...
    bpl c9788                                                         ; 97ac: 10 da       ..
; &97ae referenced 2 times by &9797, &97a1
.c97ae
    sec                                                               ; 97ae: 38          8
; &97af referenced 3 times by &9766, &9781, &979b
.return_46
    rts                                                               ; 97af: 60          `

; &97b0 referenced 3 times by &978f, &9794, &979e
.l97b0
    equb &4f                                                          ; 97b0: 4f          O
; &97b1 referenced 1 time by &97a9
.l97b1
    equb &4e, 1                                                       ; 97b1: 4e 01       N.
    equs "OFF"                                                        ; 97b3: 4f 46 46    OFF
    equb 0, &ff                                                       ; 97b6: 00 ff       ..

; ***************************************************************************************
; &97b8 referenced 12 times by &95cf, &95de, &95ed, &95f5, &95fd, &9605, &960d, &9615, &961d, &9625, &9719, &9726
.evaluate_expression_from_fmt_cmd
    lda #0                                                            ; 97b8: a9 00       ..
    sta tmp8                                                          ; 97ba: 85 8d       ..
    sta tmp9                                                          ; 97bc: 85 8e       ..
    sta input_buffer_ptr+1                                            ; 97be: 85 80       ..
; &97c0 referenced 1 time by &981f
.c97c0
    jsr get_current_fmt_cmd_byte                                      ; 97c0: 20 25 98     %.
    beq c9821                                                         ; 97c3: f0 5c       .\
    cmp #&7c ; '|'                                                    ; 97c5: c9 7c       .|
    bne c97d5                                                         ; 97c7: d0 0c       ..
    jsr get_next_fmt_cmd_byte                                         ; 97c9: 20 24 98     $.
    beq c9821                                                         ; 97cc: f0 53       .S
    iny                                                               ; 97ce: c8          .
    jsr render_register                                               ; 97cf: 20 81 ad     ..
    jmp c97dc                                                         ; 97d2: 4c dc 97    L..

; &97d5 referenced 1 time by &97c7
.c97d5
    jsr ca6fe                                                         ; 97d5: 20 fe a6     ..
    sta tmp8                                                          ; 97d8: 85 8d       ..
    stx tmp9                                                          ; 97da: 86 8e       ..
; &97dc referenced 1 time by &97d2
.c97dc
    ldx input_buffer_ptr+1                                            ; 97dc: a6 80       ..
    beq c9804                                                         ; 97de: f0 24       .$
    lda #0                                                            ; 97e0: a9 00       ..
    sta input_buffer_ptr+1                                            ; 97e2: 85 80       ..
    dex                                                               ; 97e4: ca          .
    beq c97f7                                                         ; 97e5: f0 10       ..
    lda tmp4                                                          ; 97e7: a5 89       ..
    sec                                                               ; 97e9: 38          8
    sbc tmp8                                                          ; 97ea: e5 8d       ..
    sta tmp8                                                          ; 97ec: 85 8d       ..
    lda tmp5                                                          ; 97ee: a5 8a       ..
    sbc tmp9                                                          ; 97f0: e5 8e       ..
    sta tmp9                                                          ; 97f2: 85 8e       ..
    jmp c9804                                                         ; 97f4: 4c 04 98    L..

; &97f7 referenced 1 time by &97e5
.c97f7
    lda tmp4                                                          ; 97f7: a5 89       ..
    clc                                                               ; 97f9: 18          .
    adc tmp8                                                          ; 97fa: 65 8d       e.
    sta tmp8                                                          ; 97fc: 85 8d       ..
    lda tmp5                                                          ; 97fe: a5 8a       ..
    adc tmp9                                                          ; 9800: 65 8e       e.
    sta tmp9                                                          ; 9802: 85 8e       ..
; &9804 referenced 2 times by &97de, &97f4
.c9804
    lda tmp8                                                          ; 9804: a5 8d       ..
    sta tmp4                                                          ; 9806: 85 89       ..
    lda tmp9                                                          ; 9808: a5 8e       ..
    sta tmp5                                                          ; 980a: 85 8a       ..
    jsr get_current_fmt_cmd_byte                                      ; 980c: 20 25 98     %.
    beq c9821                                                         ; 980f: f0 10       ..
    ldx #1                                                            ; 9811: a2 01       ..
    cmp #&2b ; '+'                                                    ; 9813: c9 2b       .+
    beq c981c                                                         ; 9815: f0 05       ..
    inx                                                               ; 9817: e8          .              ; X=&02
    cmp #&2d ; '-'                                                    ; 9818: c9 2d       .-
    bne c9821                                                         ; 981a: d0 05       ..
; &981c referenced 1 time by &9815
.c981c
    stx input_buffer_ptr+1                                            ; 981c: 86 80       ..
    iny                                                               ; 981e: c8          .
    bne c97c0                                                         ; 981f: d0 9f       ..
; &9821 referenced 4 times by &97c3, &97cc, &980f, &981a
.c9821
    lda tmp8                                                          ; 9821: a5 8d       ..
    rts                                                               ; 9823: 60          `

; ***************************************************************************************
; &9824 referenced 2 times by &97c9, &982d
.get_next_fmt_cmd_byte
    iny                                                               ; 9824: c8          .
; ***************************************************************************************
; &9825 referenced 6 times by &9531, &95bc, &9704, &9762, &97c0, &980c
.get_current_fmt_cmd_byte
    lda (current_format_line_ptr),y                                   ; 9825: b1 04       ..
    cmp #&0d                                                          ; 9827: c9 0d       ..
    beq return_47                                                     ; 9829: f0 04       ..
    cmp #&20 ; ' '                                                    ; 982b: c9 20       .
    beq get_next_fmt_cmd_byte                                         ; 982d: f0 f5       ..
; &982f referenced 6 times by &9829, &9832, &983c, &986f, &9873, &987a
.return_47
    rts                                                               ; 982f: 60          `

; &9830 referenced 2 times by &9a79, &9d15
.sub_c9830
    lda justifying_flag                                               ; 9830: a5 50       .P
    bne return_47                                                     ; 9832: d0 fb       ..
    sta l0046                                                         ; 9834: 85 46       .F
    sta l0039                                                         ; 9836: 85 39       .9
    sta l0042                                                         ; 9838: 85 42       .B
    lda ruler_right_stop                                              ; 983a: a5 3e       .>
    beq return_47                                                     ; 983c: f0 f1       ..
    jsr get_line_length                                               ; 983e: 20 f1 aa     ..
    sty l0043                                                         ; 9841: 84 43       .C
    ldy #0                                                            ; 9843: a0 00       ..
    beq c9861                                                         ; 9845: f0 1a       ..             ; ALWAYS branch

; &9847 referenced 2 times by &9858, &9869
.c9847
    lda l0039                                                         ; 9847: a5 39       .9
    sta l0084                                                         ; 9849: 85 84       ..
    iny                                                               ; 984b: c8          .
    cpy l0043                                                         ; 984c: c4 43       .C
    beq c9871                                                         ; 984e: f0 21       .!
    clc                                                               ; 9850: 18          .
    jsr sub_c9936                                                     ; 9851: 20 36 99     6.
    beq c985c                                                         ; 9854: f0 06       ..
    cmp #&20 ; ' '                                                    ; 9856: c9 20       .
    bne c9847                                                         ; 9858: d0 ed       ..
    inc l0046                                                         ; 985a: e6 46       .F
; &985c referenced 3 times by &9854, &9865, &986b
.c985c
    iny                                                               ; 985c: c8          .
    cpy l0043                                                         ; 985d: c4 43       .C
    beq c986d                                                         ; 985f: f0 0c       ..
; &9861 referenced 1 time by &9845
.c9861
    sec                                                               ; 9861: 38          8
    jsr sub_c9936                                                     ; 9862: 20 36 99     6.
    beq c985c                                                         ; 9865: f0 f5       ..
    cmp #&20 ; ' '                                                    ; 9867: c9 20       .
    bne c9847                                                         ; 9869: d0 dc       ..
    beq c985c                                                         ; 986b: f0 ef       ..             ; ALWAYS branch

; &986d referenced 1 time by &985f
.c986d
    dec l0046                                                         ; 986d: c6 46       .F
    bmi return_47                                                     ; 986f: 30 be       0.
; &9871 referenced 1 time by &984e
.c9871
    lda l0046                                                         ; 9871: a5 46       .F
    beq return_47                                                     ; 9873: f0 ba       ..
    lda ruler_right_stop                                              ; 9875: a5 3e       .>
    sec                                                               ; 9877: 38          8
    sbc l0084                                                         ; 9878: e5 84       ..
    bcc return_47                                                     ; 987a: 90 b3       ..
    adc #0                                                            ; 987c: 69 00       i.
    tax                                                               ; 987e: aa          .
    adc l0043                                                         ; 987f: 65 43       eC
    sec                                                               ; 9881: 38          8
    sbc #&84                                                          ; 9882: e9 84       ..
    bcc c988c                                                         ; 9884: 90 06       ..
    sta l0084                                                         ; 9886: 85 84       ..
    txa                                                               ; 9888: 8a          .
    sbc l0084                                                         ; 9889: e5 84       ..
    tax                                                               ; 988b: aa          .
; &988c referenced 1 time by &9884
.c988c
    stx l0082                                                         ; 988c: 86 82       ..
    stx tmp8                                                          ; 988e: 86 8d       ..
    lda #0                                                            ; 9890: a9 00       ..
    sta tmp9                                                          ; 9892: 85 8e       ..
    jsr sub_cadf0                                                     ; 9894: 20 f0 ad     ..
    sta l0045                                                         ; 9897: 85 45       .E
    lda tmp8                                                          ; 9899: a5 8d       ..
    sta l0044                                                         ; 989b: 85 44       .D
    ldy #0                                                            ; 989d: a0 00       ..
    ldx l0046                                                         ; 989f: a6 46       .F
    tya                                                               ; 98a1: 98          .              ; A=&00
; &98a2 referenced 1 time by &98a7
.loop_c98a2
    sta input_buffer,y                                                ; 98a2: 99 00 05    ...
    iny                                                               ; 98a5: c8          .
    dex                                                               ; 98a6: ca          .
    bne loop_c98a2                                                    ; 98a7: d0 f9       ..
    ldy print_xpos                                                    ; 98a9: a4 78       .x
    iny                                                               ; 98ab: c8          .
    cpy l0046                                                         ; 98ac: c4 46       .F
    bcc c98b2                                                         ; 98ae: 90 02       ..
    ldy #1                                                            ; 98b0: a0 01       ..
; &98b2 referenced 1 time by &98ae
.c98b2
    dey                                                               ; 98b2: 88          .
    ldx l0046                                                         ; 98b3: a6 46       .F
; &98b5 referenced 1 time by &98d7
.c98b5
    lda l0045                                                         ; 98b5: a5 45       .E
    beq c98bd                                                         ; 98b7: f0 04       ..
    lda #1                                                            ; 98b9: a9 01       ..
    dec l0045                                                         ; 98bb: c6 45       .E
; &98bd referenced 1 time by &98b7
.c98bd
    clc                                                               ; 98bd: 18          .
    adc l0044                                                         ; 98be: 65 44       eD
    sta input_buffer,y                                                ; 98c0: 99 00 05    ...
    lda l0082                                                         ; 98c3: a5 82       ..
    sec                                                               ; 98c5: 38          8
    sbc input_buffer,y                                                ; 98c6: f9 00 05    ...
    php                                                               ; 98c9: 08          .
    sta l0082                                                         ; 98ca: 85 82       ..
    iny                                                               ; 98cc: c8          .
    cpy l0046                                                         ; 98cd: c4 46       .F
    bcc c98d3                                                         ; 98cf: 90 02       ..
    ldy #0                                                            ; 98d1: a0 00       ..
; &98d3 referenced 1 time by &98cf
.c98d3
    plp                                                               ; 98d3: 28          (
    beq c98d9                                                         ; 98d4: f0 03       ..
    dex                                                               ; 98d6: ca          .
    bne c98b5                                                         ; 98d7: d0 dc       ..
; &98d9 referenced 1 time by &98d4
.c98d9
    sty print_xpos                                                    ; 98d9: 84 78       .x
    ldy #0                                                            ; 98db: a0 00       ..
    sty l0081                                                         ; 98dd: 84 81       ..
    sty l0039                                                         ; 98df: 84 39       .9
    lda #&1a                                                          ; 98e1: a9 1a       ..
    jsr sub_cab0f                                                     ; 98e3: 20 0f ab     ..
    lda l0042                                                         ; 98e6: a5 42       .B
    beq c98f6                                                         ; 98e8: f0 0c       ..
    ldy #0                                                            ; 98ea: a0 00       ..
; &98ec referenced 1 time by &98f4
.loop_c98ec
    lda output_buffer,y                                               ; 98ec: b9 54 06    .T.
    sta (current_edit_line_ptr),y                                     ; 98ef: 91 02       ..
    iny                                                               ; 98f1: c8          .
    cpy l0042                                                         ; 98f2: c4 42       .B
    bne loop_c98ec                                                    ; 98f4: d0 f6       ..
; &98f6 referenced 1 time by &98e8
.c98f6
    ldy l0042                                                         ; 98f6: a4 42       .B
    ldx l0042                                                         ; 98f8: a6 42       .B
; &98fa referenced 1 time by &9928
.c98fa
    lda output_buffer,x                                               ; 98fa: bd 54 06    .T.
    cmp #&20 ; ' '                                                    ; 98fd: c9 20       .
    bne c9920                                                         ; 98ff: d0 1f       ..
    lda l0081                                                         ; 9901: a5 81       ..
    beq c991c                                                         ; 9903: f0 17       ..
    sty l0084                                                         ; 9905: 84 84       ..
    ldy l0039                                                         ; 9907: a4 39       .9
    cpy l0046                                                         ; 9909: c4 46       .F
    lda #0                                                            ; 990b: a9 00       ..
    bcs c9912                                                         ; 990d: b0 03       ..
    lda input_buffer,y                                                ; 990f: b9 00 05    ...
; &9912 referenced 1 time by &990d
.c9912
    clc                                                               ; 9912: 18          .
    adc l0084                                                         ; 9913: 65 84       e.
    inc l0039                                                         ; 9915: e6 39       .9
    tay                                                               ; 9917: a8          .
    lda #0                                                            ; 9918: a9 00       ..
    sta l0081                                                         ; 991a: 85 81       ..
; &991c referenced 1 time by &9903
.c991c
    lda #&20 ; ' '                                                    ; 991c: a9 20       .
    bne c9922                                                         ; 991e: d0 02       ..             ; ALWAYS branch

; &9920 referenced 1 time by &98ff
.c9920
    inc l0081                                                         ; 9920: e6 81       ..
; &9922 referenced 1 time by &991e
.c9922
    sta (current_edit_line_ptr),y                                     ; 9922: 91 02       ..
    iny                                                               ; 9924: c8          .
    inx                                                               ; 9925: e8          .
    cpx l0043                                                         ; 9926: e4 43       .C
    bne c98fa                                                         ; 9928: d0 d0       ..
    lda #&10                                                          ; 992a: a9 10       ..
; &992c referenced 1 time by &9933
.loop_c992c
    cpy #&84                                                          ; 992c: c0 84       ..
    bcs return_48                                                     ; 992e: b0 05       ..
    sta (current_edit_line_ptr),y                                     ; 9930: 91 02       ..
    iny                                                               ; 9932: c8          .
    bne loop_c992c                                                    ; 9933: d0 f7       ..
; &9935 referenced 1 time by &992e
.return_48
    rts                                                               ; 9935: 60          `

; &9936 referenced 2 times by &9851, &9862
.sub_c9936
    ror l0083                                                         ; 9936: 66 83       f.
    lda (current_edit_line_ptr),y                                     ; 9938: b1 02       ..
    sta output_buffer,y                                               ; 993a: 99 54 06    .T.
    cmp #9                                                            ; 993d: c9 09       ..
    bne c994a                                                         ; 993f: d0 09       ..
    jsr sub_ca5ae                                                     ; 9941: 20 ae a5     ..
    txa                                                               ; 9944: 8a          .
    clc                                                               ; 9945: 18          .
    adc l0039                                                         ; 9946: 65 39       e9
    bne c995c                                                         ; 9948: d0 12       ..
; &994a referenced 1 time by &993f
.c994a
    cmp #&0b                                                          ; 994a: c9 0b       ..
    bne c9969                                                         ; 994c: d0 1b       ..
    lda ruler_left_stop                                               ; 994e: a5 3f       .?
    beq c9967                                                         ; 9950: f0 15       ..
    ldx l0039                                                         ; 9952: a6 39       .9
    beq c995c                                                         ; 9954: f0 06       ..
    cpx ruler_left_stop                                               ; 9956: e4 3f       .?
    bcc c995c                                                         ; 9958: 90 02       ..
    inx                                                               ; 995a: e8          .
    txa                                                               ; 995b: 8a          .
; &995c referenced 3 times by &9948, &9954, &9958
.c995c
    sta l0039                                                         ; 995c: 85 39       .9
    sty l0042                                                         ; 995e: 84 42       .B
    inc l0042                                                         ; 9960: e6 42       .B
    lda #0                                                            ; 9962: a9 00       ..
    sta l0046                                                         ; 9964: 85 46       .F
    rts                                                               ; 9966: 60          `

; &9967 referenced 2 times by &9950, &996b
.c9967
    lda #&20 ; ' '                                                    ; 9967: a9 20       .
; &9969 referenced 1 time by &994c
.c9969
    cmp #&1b                                                          ; 9969: c9 1b       ..
    bcc c9967                                                         ; 996b: 90 fa       ..
    cmp #&20 ; ' '                                                    ; 996d: c9 20       .
    bcc return_49                                                     ; 996f: 90 02       ..
    inc l0039                                                         ; 9971: e6 39       .9
; &9973 referenced 1 time by &996f
.return_49
    rts                                                               ; 9973: 60          `

; &9974 referenced 4 times by &9988, &998e, &9992, &9997
.c9974
    jmp c9a8d                                                         ; 9974: 4c 8d 9a    L..

; &9977 referenced 2 times by &876d, &a048
.sub_c9977
    inc l007d                                                         ; 9977: e6 7d       .}
    ldy #4                                                            ; 9979: a0 04       ..
    sty print_xpos                                                    ; 997b: 84 78       .x
    ldy #0                                                            ; 997d: a0 00       ..
    sty input_buffer_ptr                                              ; 997f: 84 7f       ..
    sty l007e                                                         ; 9981: 84 7e       .~
    lda (ptr4),y                                                      ; 9983: b1 08       ..
    jsr sub_caf71                                                     ; 9985: 20 71 af     q.
    beq c9974                                                         ; 9988: f0 ea       ..
; &998a referenced 1 time by &9a84
.c998a
    lda format_mode_flag                                              ; 998a: a5 4f       .O
    and #&81                                                          ; 998c: 29 81       ).
    bne c9974                                                         ; 998e: d0 e4       ..
    lda ruler_right_stop                                              ; 9990: a5 3e       .>
    beq c9974                                                         ; 9992: f0 e0       ..
    sec                                                               ; 9994: 38          8
    sbc ruler_left_stop                                               ; 9995: e5 3f       .?
    bcc c9974                                                         ; 9997: 90 db       ..
    adc #1                                                            ; 9999: 69 01       i.
    sta input_buffer_ptr+1                                            ; 999b: 85 80       ..
    lda #&10                                                          ; 999d: a9 10       ..
    jsr sub_cab0f                                                     ; 999f: 20 0f ab     ..
    lda ptr4                                                          ; 99a2: a5 08       ..
    sta tmp6                                                          ; 99a4: 85 8b       ..
    lda ptr4+1                                                        ; 99a6: a5 09       ..
    sta tmp7                                                          ; 99a8: 85 8c       ..
    ldy #0                                                            ; 99aa: a0 00       ..
    sty l0047                                                         ; 99ac: 84 47       .G
    sty l0039                                                         ; 99ae: 84 39       .9
    sty l0038                                                         ; 99b0: 84 38       .8
    sty l0046                                                         ; 99b2: 84 46       .F
    sty bottom_margin                                                 ; 99b4: 84 23       .#
; &99b6 referenced 1 time by &9a5d
.c99b6
    sty l0048                                                         ; 99b6: 84 48       .H
    ldy l0047                                                         ; 99b8: a4 47       .G
; &99ba referenced 1 time by &99c5
.loop_c99ba
    jsr sub_ca536                                                     ; 99ba: 20 36 a5     6.
    bne c99c7                                                         ; 99bd: d0 08       ..
    lda #0                                                            ; 99bf: a9 00       ..
    sta markers_array+1,x                                             ; 99c1: 95 54       .T
    inc l007e                                                         ; 99c3: e6 7e       .~
    bne loop_c99ba                                                    ; 99c5: d0 f3       ..
; &99c7 referenced 1 time by &99bd
.c99c7
    ldy l0047                                                         ; 99c7: a4 47       .G
; &99c9 referenced 2 times by &99e6, &99fe
.c99c9
    lda (ptr4),y                                                      ; 99c9: b1 08       ..
    iny                                                               ; 99cb: c8          .
    sty l0047                                                         ; 99cc: 84 47       .G
    cmp #9                                                            ; 99ce: c9 09       ..
    bne c99e0                                                         ; 99d0: d0 0e       ..
    jsr sub_ca5ae                                                     ; 99d2: 20 ae a5     ..
    dex                                                               ; 99d5: ca          .
    txa                                                               ; 99d6: 8a          .
    clc                                                               ; 99d7: 18          .
    adc l0039                                                         ; 99d8: 65 39       e9
    sta l0039                                                         ; 99da: 85 39       .9
    lda #9                                                            ; 99dc: a9 09       ..
    bne c9a21                                                         ; 99de: d0 41       .A             ; ALWAYS branch

; &99e0 referenced 1 time by &99d0
.c99e0
    cmp #&1a                                                          ; 99e0: c9 1a       ..
    bne c99ee                                                         ; 99e2: d0 0a       ..
; &99e4 referenced 2 times by &99f4, &99f8
.c99e4
    lda l0046                                                         ; 99e4: a5 46       .F
    bne c99c9                                                         ; 99e6: d0 e1       ..
    ldx #&ff                                                          ; 99e8: a2 ff       ..
    lda #&20 ; ' '                                                    ; 99ea: a9 20       .
    bne c9a2e                                                         ; 99ec: d0 40       .@             ; ALWAYS branch

; &99ee referenced 1 time by &99e2
.c99ee
    cmp #&0b                                                          ; 99ee: c9 0b       ..
    bne c9a11                                                         ; 99f0: d0 1f       ..
    ldx input_buffer_ptr                                              ; 99f2: a6 7f       ..
    bne c99e4                                                         ; 99f4: d0 ee       ..
    lda l0038                                                         ; 99f6: a5 38       .8
    bne c99e4                                                         ; 99f8: d0 ea       ..
    inc l0038                                                         ; 99fa: e6 38       .8
    lda ruler_left_stop                                               ; 99fc: a5 3f       .?
    beq c99c9                                                         ; 99fe: f0 c9       ..
    ldx l0039                                                         ; 9a00: a6 39       .9
    cpx ruler_left_stop                                               ; 9a02: e4 3f       .?
    bcs c9a0a                                                         ; 9a04: b0 04       ..
    sta l0039                                                         ; 9a06: 85 39       .9
    dec l0039                                                         ; 9a08: c6 39       .9
; &9a0a referenced 1 time by &9a04
.c9a0a
    clc                                                               ; 9a0a: 18          .
    adc input_buffer_ptr+1                                            ; 9a0b: 65 80       e.
    sta input_buffer_ptr+1                                            ; 9a0d: 85 80       ..
    lda #&0b                                                          ; 9a0f: a9 0b       ..
; &9a11 referenced 1 time by &99f0
.c9a11
    cmp #&0d                                                          ; 9a11: c9 0d       ..
    bne c9a21                                                         ; 9a13: d0 0c       ..
    dey                                                               ; 9a15: 88          .
    beq c9a8d                                                         ; 9a16: f0 75       .u
    jsr sub_c9ac1                                                     ; 9a18: 20 c1 9a     ..
    bcs c9a87                                                         ; 9a1b: b0 6a       .j
    lda #&20 ; ' '                                                    ; 9a1d: a9 20       .
    sta input_buffer_ptr                                              ; 9a1f: 85 7f       ..
; &9a21 referenced 2 times by &99de, &9a13
.c9a21
    ldy l0048                                                         ; 9a21: a4 48       .H
    ldx #0                                                            ; 9a23: a2 00       ..
    cmp #&20 ; ' '                                                    ; 9a25: c9 20       .
    bne c9a2e                                                         ; 9a27: d0 05       ..
    inx                                                               ; 9a29: e8          .              ; X=&01
    bit l0046                                                         ; 9a2a: 24 46       $F
    bmi c9a40                                                         ; 9a2c: 30 12       0.
; &9a2e referenced 2 times by &99ec, &9a27
.c9a2e
    ldy l0048                                                         ; 9a2e: a4 48       .H
    sta (current_edit_line_ptr),y                                     ; 9a30: 91 02       ..
    cmp #&20 ; ' '                                                    ; 9a32: c9 20       .
    bne c9a38                                                         ; 9a34: d0 02       ..
    ror bottom_margin                                                 ; 9a36: 66 23       f#
; &9a38 referenced 1 time by &9a34
.c9a38
    iny                                                               ; 9a38: c8          .
    jsr sub_ca600                                                     ; 9a39: 20 00 a6     ..
    beq c9a40                                                         ; 9a3c: f0 02       ..
    inc l0039                                                         ; 9a3e: e6 39       .9
; &9a40 referenced 2 times by &9a2c, &9a3c
.c9a40
    bit l0046                                                         ; 9a40: 24 46       $F
    stx l0046                                                         ; 9a42: 86 46       .F
    bmi c9a58                                                         ; 9a44: 30 12       0.
    cmp #&20 ; ' '                                                    ; 9a46: c9 20       .
    beq c9a58                                                         ; 9a48: f0 0e       ..
    cpy #&85                                                          ; 9a4a: c0 85       ..
    bcs c9a60                                                         ; 9a4c: b0 12       ..
    lda bottom_margin                                                 ; 9a4e: a5 23       .#
    beq c9a58                                                         ; 9a50: f0 06       ..
    lda l0039                                                         ; 9a52: a5 39       .9
    cmp input_buffer_ptr+1                                            ; 9a54: c5 80       ..
    bcs c9a60                                                         ; 9a56: b0 08       ..
; &9a58 referenced 3 times by &9a44, &9a48, &9a50
.c9a58
    cpy #&86                                                          ; 9a58: c0 86       ..
    bcc c9a5d                                                         ; 9a5a: 90 01       ..
    dey                                                               ; 9a5c: 88          .
; &9a5d referenced 1 time by &9a5a
.c9a5d
    jmp c99b6                                                         ; 9a5d: 4c b6 99    L..

; &9a60 referenced 2 times by &9a4c, &9a56
.c9a60
    inc l0047                                                         ; 9a60: e6 47       .G
; &9a62 referenced 1 time by &9a71
.loop_c9a62
    dec l0047                                                         ; 9a62: c6 47       .G
    dey                                                               ; 9a64: 88          .
    beq c9a8d                                                         ; 9a65: f0 26       .&
    lda (current_edit_line_ptr),y                                     ; 9a67: b1 02       ..
    pha                                                               ; 9a69: 48          H
    lda #&10                                                          ; 9a6a: a9 10       ..
    sta (current_edit_line_ptr),y                                     ; 9a6c: 91 02       ..
    pla                                                               ; 9a6e: 68          h
    cmp #&20 ; ' '                                                    ; 9a6f: c9 20       .
    bne loop_c9a62                                                    ; 9a71: d0 ef       ..
    sec                                                               ; 9a73: 38          8
    ror input_buffer_ptr                                              ; 9a74: 66 7f       f.
    jsr sub_caed6                                                     ; 9a76: 20 d6 ae     ..
    jsr sub_c9830                                                     ; 9a79: 20 30 98     0.
    jsr sub_c9aa9                                                     ; 9a7c: 20 a9 9a     ..
    jsr c9a8d                                                         ; 9a7f: 20 8d 9a     ..
    beq c9aa5                                                         ; 9a82: f0 21       .!
    jmp c998a                                                         ; 9a84: 4c 8a 99    L..

; &9a87 referenced 1 time by &9a1b
.c9a87
    jsr sub_caed6                                                     ; 9a87: 20 d6 ae     ..
    jsr sub_c9aa9                                                     ; 9a8a: 20 a9 9a     ..
; &9a8d referenced 4 times by &9974, &9a16, &9a65, &9a7f
.c9a8d
    jsr c9e94                                                         ; 9a8d: 20 94 9e     ..
    lda ptr4                                                          ; 9a90: a5 08       ..
    ldy ptr4+1                                                        ; 9a92: a4 09       ..
    jsr sub_cab1a                                                     ; 9a94: 20 1a ab     ..
    sec                                                               ; 9a97: 38          8
    beq c9aa5                                                         ; 9a98: f0 0b       ..
    tya                                                               ; 9a9a: 98          .
    clc                                                               ; 9a9b: 18          .
    adc tmp0                                                          ; 9a9c: 65 85       e.
    sta ptr4                                                          ; 9a9e: 85 08       ..
    bcc c9aa4                                                         ; 9aa0: 90 02       ..
    inc ptr4+1                                                        ; 9aa2: e6 09       ..
; &9aa4 referenced 1 time by &9aa0
.c9aa4
    clc                                                               ; 9aa4: 18          .
; &9aa5 referenced 2 times by &9a82, &9a98
.c9aa5
    clv                                                               ; 9aa5: b8          .
    lda l007e                                                         ; 9aa6: a5 7e       .~
    rts                                                               ; 9aa8: 60          `

; &9aa9 referenced 2 times by &9a7c, &9a8a
.sub_c9aa9
    sec                                                               ; 9aa9: 38          8
    rol l007e                                                         ; 9aaa: 26 7e       &~
    ldy l0047                                                         ; 9aac: a4 47       .G
    dey                                                               ; 9aae: 88          .
    sty l003b                                                         ; 9aaf: 84 3b       .;
    inc l006e                                                         ; 9ab1: e6 6e       .n
    jsr sub_ca8b9                                                     ; 9ab3: 20 b9 a8     ..
    bcc return_50                                                     ; 9ab6: 90 08       ..
    pla                                                               ; 9ab8: 68          h
    pla                                                               ; 9ab9: 68          h
    lda #&40 ; '@'                                                    ; 9aba: a9 40       .@
    sta l0084                                                         ; 9abc: 85 84       ..
    bit l0084                                                         ; 9abe: 24 84       $.
; &9ac0 referenced 1 time by &9ab6
.return_50
    rts                                                               ; 9ac0: 60          `

; &9ac1 referenced 1 time by &9a18
.sub_c9ac1
    tya                                                               ; 9ac1: 98          .
    sec                                                               ; 9ac2: 38          8
    adc ptr4                                                          ; 9ac3: 65 08       e.
    sta tmp8                                                          ; 9ac5: 85 8d       ..
    sta tmp4                                                          ; 9ac7: 85 89       ..
    lda ptr4+1                                                        ; 9ac9: a5 09       ..
    adc #0                                                            ; 9acb: 69 00       i.
    sta tmp9                                                          ; 9acd: 85 8e       ..
    sta tmp5                                                          ; 9acf: 85 8a       ..
    ldy #0                                                            ; 9ad1: a0 00       ..
    sty l0083                                                         ; 9ad3: 84 83       ..
; &9ad5 referenced 1 time by &9b21
.c9ad5
    lda (tmp4),y                                                      ; 9ad5: b1 89       ..
    beq c9b2f                                                         ; 9ad7: f0 56       .V
    jsr sub_caf71                                                     ; 9ad9: 20 71 af     q.
    beq c9b2f                                                         ; 9adc: f0 51       .Q
    cmp #&0d                                                          ; 9ade: c9 0d       ..
    beq c9b2f                                                         ; 9ae0: f0 4d       .M
    tya                                                               ; 9ae2: 98          .
    bne c9b06                                                         ; 9ae3: d0 21       .!
    sty l0084                                                         ; 9ae5: 84 84       ..
    beq c9aef                                                         ; 9ae7: f0 06       ..             ; ALWAYS branch

; &9ae9 referenced 2 times by &9afd, &9b04
.c9ae9
    inc tmp8                                                          ; 9ae9: e6 8d       ..
    bne c9aef                                                         ; 9aeb: d0 02       ..
    inc tmp9                                                          ; 9aed: e6 8e       ..
; &9aef referenced 2 times by &9ae7, &9aeb
.c9aef
    lda (tmp8),y                                                      ; 9aef: b1 8d       ..
    beq c9b06                                                         ; 9af1: f0 13       ..
    cmp #&0d                                                          ; 9af3: c9 0d       ..
    beq c9b06                                                         ; 9af5: f0 0f       ..
    cmp #9                                                            ; 9af7: c9 09       ..
    beq c9b2f                                                         ; 9af9: f0 34       .4
    cmp #&0b                                                          ; 9afb: c9 0b       ..
    bne c9ae9                                                         ; 9afd: d0 ea       ..
    rol l0084                                                         ; 9aff: 26 84       &.
    sec                                                               ; 9b01: 38          8
    ror l0084                                                         ; 9b02: 66 84       f.
    bcs c9ae9                                                         ; 9b04: b0 e3       ..
; &9b06 referenced 3 times by &9ae3, &9af1, &9af5
.c9b06
    lda (tmp4),y                                                      ; 9b06: b1 89       ..
    cmp #&20 ; ' '                                                    ; 9b08: c9 20       .
    bne c9b1a                                                         ; 9b0a: d0 0e       ..
    ldx ruler_left_stop                                               ; 9b0c: a6 3f       .?
    beq c9b2f                                                         ; 9b0e: f0 1f       ..
    ldx l0084                                                         ; 9b10: a6 84       ..
    beq c9b2f                                                         ; 9b12: f0 1b       ..
    ldx l0083                                                         ; 9b14: a6 83       ..
    bne c9b2f                                                         ; 9b16: d0 17       ..
    beq c9b20                                                         ; 9b18: f0 06       ..             ; ALWAYS branch

; &9b1a referenced 1 time by &9b0a
.c9b1a
    cmp #&0b                                                          ; 9b1a: c9 0b       ..
    bne c9b23                                                         ; 9b1c: d0 05       ..
    sta l0083                                                         ; 9b1e: 85 83       ..
; &9b20 referenced 1 time by &9b18
.c9b20
    iny                                                               ; 9b20: c8          .
    bne c9ad5                                                         ; 9b21: d0 b2       ..
; &9b23 referenced 1 time by &9b1c
.c9b23
    lda ruler_left_stop                                               ; 9b23: a5 3f       .?
    beq c9b31                                                         ; 9b25: f0 0a       ..
    lda l0084                                                         ; 9b27: a5 84       ..
    beq c9b31                                                         ; 9b29: f0 06       ..
    lda l0083                                                         ; 9b2b: a5 83       ..
    bne c9b31                                                         ; 9b2d: d0 02       ..
; &9b2f referenced 7 times by &9ad7, &9adc, &9ae0, &9af9, &9b0e, &9b12, &9b16
.c9b2f
    sec                                                               ; 9b2f: 38          8
    rts                                                               ; 9b30: 60          `

; &9b31 referenced 3 times by &9b25, &9b29, &9b2d
.c9b31
    clc                                                               ; 9b31: 18          .
    rts                                                               ; 9b32: 60          `

; ***************************************************************************************
; &9b33 referenced 2 times by &8238, &829e
.run_editor
    jsr enter_editor_mode                                             ; 9b33: 20 9b b0     ..
; ***************************************************************************************
; &9b36 referenced 5 times by &9bc7, &9bcd, &9c7f, &9d25, &a947
.editor_loop
    lda format_mode_flag                                              ; 9b36: a5 4f       .O
    pha                                                               ; 9b38: 48          H
    lda l006e                                                         ; 9b39: a5 6e       .n
    bne c9b44                                                         ; 9b3b: d0 07       ..
    pha                                                               ; 9b3d: 48          H
    jsr sub_caa97                                                     ; 9b3e: 20 97 aa     ..
    pla                                                               ; 9b41: 68          h
    sta l006e                                                         ; 9b42: 85 6e       .n
; &9b44 referenced 1 time by &9b3b
.c9b44
    jsr sub_ca608                                                     ; 9b44: 20 08 a6     ..
    lda ruler_left_stop                                               ; 9b47: a5 3f       .?
    beq c9b73                                                         ; 9b49: f0 28       .(
    ldx format_mode_flag                                              ; 9b4b: a6 4f       .O
    bmi c9b73                                                         ; 9b4d: 30 24       0$
    cmp l0072                                                         ; 9b4f: c5 72       .r
    bcc c9b73                                                         ; 9b51: 90 20       .
    beq c9b73                                                         ; 9b53: f0 1e       ..
    ldx l007d                                                         ; 9b55: a6 7d       .}
    bne c9b6a                                                         ; 9b57: d0 11       ..
    jsr get_line_length                                               ; 9b59: 20 f1 aa     ..
    lda format_mode_flag                                              ; 9b5c: a5 4f       .O
    cpy xpos                                                          ; 9b5e: c4 40       .@
    bcs c9b84                                                         ; 9b60: b0 22       ."
    bit format_mode_flag                                              ; 9b62: 24 4f       $O
    bvs c9b6a                                                         ; 9b64: 70 04       p.
    sty xpos                                                          ; 9b66: 84 40       .@
    bvc c9b84                                                         ; 9b68: 50 1a       P.             ; ALWAYS branch

; &9b6a referenced 2 times by &9b57, &9b64
.c9b6a
    lda ruler_left_stop                                               ; 9b6a: a5 3f       .?
    sta l0072                                                         ; 9b6c: 85 72       .r
    inc l0079                                                         ; 9b6e: e6 79       .y
    jsr sub_ca608                                                     ; 9b70: 20 08 a6     ..
; &9b73 referenced 4 times by &9b49, &9b4d, &9b51, &9b53
.c9b73
    lda format_mode_flag                                              ; 9b73: a5 4f       .O
    and #&bf                                                          ; 9b75: 29 bf       ).
    pha                                                               ; 9b77: 48          H
    jsr sub_caec2                                                     ; 9b78: 20 c2 ae     ..
    pla                                                               ; 9b7b: 68          h
    bcs c9b86                                                         ; 9b7c: b0 08       ..
    cpy xpos                                                          ; 9b7e: c4 40       .@
    bcc c9b86                                                         ; 9b80: 90 04       ..
    beq c9b86                                                         ; 9b82: f0 02       ..
; &9b84 referenced 2 times by &9b60, &9b68
.c9b84
    ora #&40 ; '@'                                                    ; 9b84: 09 40       .@
; &9b86 referenced 3 times by &9b7c, &9b80, &9b82
.c9b86
    sta format_mode_flag                                              ; 9b86: 85 4f       .O
    pla                                                               ; 9b88: 68          h
    cmp format_mode_flag                                              ; 9b89: c5 4f       .O
    beq c9b8f                                                         ; 9b8b: f0 02       ..
    inc l0075                                                         ; 9b8d: e6 75       .u
; &9b8f referenced 1 time by &9b8b
.c9b8f
    lda #0                                                            ; 9b8f: a9 00       ..
    sta l007d                                                         ; 9b91: 85 7d       .}
    jsr sub_ca276                                                     ; 9b93: 20 76 a2     v.
; &9b96 referenced 2 times by &9bb1, &9bc0
.c9b96
    jsr read_char                                                     ; 9b96: 20 61 a7     a.
    cmp current_tab_key                                               ; 9b99: c5 7b       .{
    bne c9b9f                                                         ; 9b9b: d0 02       ..
    lda #9                                                            ; 9b9d: a9 09       ..
; &9b9f referenced 1 time by &9b9b
.c9b9f
    sta l0038                                                         ; 9b9f: 85 38       .8
    tay                                                               ; 9ba1: a8          .
    bmi c9bbb                                                         ; 9ba2: 30 17       0.
    cmp #&20 ; ' '                                                    ; 9ba4: c9 20       .
    bcc enter_nonprintable_character                                  ; 9ba6: 90 04       ..
    cmp #&7f                                                          ; 9ba8: c9 7f       ..
    bcc enter_printable_character                                     ; 9baa: 90 24       .$
; &9bac referenced 1 time by &9ba6
.enter_nonprintable_character
    ldy #0                                                            ; 9bac: a0 00       ..
; &9bae referenced 1 time by &9bb6
.loop_c9bae
    lda non_function_key_table,y                                      ; 9bae: b9 4c b1    .L.
    bmi c9b96                                                         ; 9bb1: 30 e3       0.
    iny                                                               ; 9bb3: c8          .
    cmp l0038                                                         ; 9bb4: c5 38       .8
    bne loop_c9bae                                                    ; 9bb6: d0 f6       ..
    tya                                                               ; 9bb8: 98          .
    adc #&86                                                          ; 9bb9: 69 86       i.
; &9bbb referenced 1 time by &9ba2
.c9bbb
    sec                                                               ; 9bbb: 38          8
    sbc #&88                                                          ; 9bbc: e9 88       ..
    cmp #&34 ; '4'                                                    ; 9bbe: c9 34       .4
    bcs c9b96                                                         ; 9bc0: b0 d4       ..
    ldy #0                                                            ; 9bc2: a0 00       ..
    jsr call_through_jumptable                                        ; 9bc4: 20 92 a8     ..
    jmp editor_loop                                                   ; 9bc7: 4c 36 9b    L6.

; &9bca referenced 2 times by &9bd4, &9bdb
.c9bca
    jsr beep                                                          ; 9bca: 20 e8 ac     ..
    jmp editor_loop                                                   ; 9bcd: 4c 36 9b    L6.

; &9bd0 referenced 1 time by &9baa
.enter_printable_character
    ldy xpos                                                          ; 9bd0: a4 40       .@
    cpy #&84                                                          ; 9bd2: c0 84       ..
    bcs c9bca                                                         ; 9bd4: b0 f4       ..
    inc l006d                                                         ; 9bd6: e6 6d       .m
    jsr sub_caef4                                                     ; 9bd8: 20 f4 ae     ..
    bcs c9bca                                                         ; 9bdb: b0 ed       ..
    lda current_edit_line_ptr                                         ; 9bdd: a5 02       ..
    sta tmp6                                                          ; 9bdf: 85 8b       ..
    lda current_edit_line_ptr+1                                       ; 9be1: a5 03       ..
    sta tmp7                                                          ; 9be3: 85 8c       ..
    ldy xpos                                                          ; 9be5: a4 40       .@
    jsr sub_ca536                                                     ; 9be7: 20 36 a5     6.
    bne c9bf2                                                         ; 9bea: d0 06       ..
    cpx #4                                                            ; 9bec: e0 04       ..
    bcs c9bf2                                                         ; 9bee: b0 02       ..
    inc l0074                                                         ; 9bf0: e6 74       .t
; &9bf2 referenced 2 times by &9bea, &9bee
.c9bf2
    ldx insert_mode_flag                                              ; 9bf2: a6 51       .Q
    bne c9c00                                                         ; 9bf4: d0 0a       ..
    lda (current_edit_line_ptr),y                                     ; 9bf6: b1 02       ..
    cmp #9                                                            ; 9bf8: c9 09       ..
    beq c9c00                                                         ; 9bfa: f0 04       ..
    cmp #&0b                                                          ; 9bfc: c9 0b       ..
    bne c9c09                                                         ; 9bfe: d0 09       ..
; &9c00 referenced 2 times by &9bf4, &9bfa
.c9c00
    inc l0074                                                         ; 9c00: e6 74       .t
    ldx #1                                                            ; 9c02: a2 01       ..
    jsr sub_cae06                                                     ; 9c04: 20 06 ae     ..
    bcs c9c7f                                                         ; 9c07: b0 76       .v
; &9c09 referenced 1 time by &9bfe
.c9c09
    lda l0038                                                         ; 9c09: a5 38       .8
    sta (current_edit_line_ptr),y                                     ; 9c0b: 91 02       ..
    ldy l0074                                                         ; 9c0d: a4 74       .t
    bne c9c14                                                         ; 9c0f: d0 03       ..
    jsr oswrch                                                        ; 9c11: 20 ee ff     ..            ; Write character
; &9c14 referenced 1 time by &9c0f
.c9c14
    inc xpos                                                          ; 9c14: e6 40       .@
    jsr ca684                                                         ; 9c16: 20 84 a6     ..
    ldy #0                                                            ; 9c19: a0 00       ..
    sty l0039                                                         ; 9c1b: 84 39       .9
; &9c1d referenced 3 times by &9c45, &9c50, &9c54
.c9c1d
    lda (current_edit_line_ptr),y                                     ; 9c1d: b1 02       ..
    iny                                                               ; 9c1f: c8          .
    cpy xpos                                                          ; 9c20: c4 40       .@
    bcs c9c56                                                         ; 9c22: b0 32       .2
    cmp #9                                                            ; 9c24: c9 09       ..
    bne c9c31                                                         ; 9c26: d0 09       ..
    jsr sub_ca5ae                                                     ; 9c28: 20 ae a5     ..
    txa                                                               ; 9c2b: 8a          .
    clc                                                               ; 9c2c: 18          .
    adc l0039                                                         ; 9c2d: 65 39       e9
    bne c9c43                                                         ; 9c2f: d0 12       ..
; &9c31 referenced 1 time by &9c26
.c9c31
    cmp #&0b                                                          ; 9c31: c9 0b       ..
    bne c9c4a                                                         ; 9c33: d0 15       ..
    lda ruler_left_stop                                               ; 9c35: a5 3f       .?
    beq c9c48                                                         ; 9c37: f0 0f       ..
    ldx l0039                                                         ; 9c39: a6 39       .9
    beq c9c43                                                         ; 9c3b: f0 06       ..
    cpx ruler_left_stop                                               ; 9c3d: e4 3f       .?
    bcc c9c43                                                         ; 9c3f: 90 02       ..
    inx                                                               ; 9c41: e8          .
    txa                                                               ; 9c42: 8a          .
; &9c43 referenced 3 times by &9c2f, &9c3b, &9c3f
.c9c43
    sta l0039                                                         ; 9c43: 85 39       .9
    jmp c9c1d                                                         ; 9c45: 4c 1d 9c    L..

; &9c48 referenced 2 times by &9c37, &9c4c
.c9c48
    lda #&20 ; ' '                                                    ; 9c48: a9 20       .
; &9c4a referenced 1 time by &9c33
.c9c4a
    cmp #&1b                                                          ; 9c4a: c9 1b       ..
    bcc c9c48                                                         ; 9c4c: 90 fa       ..
    cmp #&20 ; ' '                                                    ; 9c4e: c9 20       .
    bcc c9c1d                                                         ; 9c50: 90 cb       ..
    inc l0039                                                         ; 9c52: e6 39       .9
    bne c9c1d                                                         ; 9c54: d0 c7       ..
; &9c56 referenced 1 time by &9c22
.c9c56
    ldy l0039                                                         ; 9c56: a4 39       .9
    cpy l003a                                                         ; 9c58: c4 3a       .:
    bcs c9c67                                                         ; 9c5a: b0 0b       ..
    lda (current_ruler_ptr),y                                         ; 9c5c: b1 06       ..
    and #&df                                                          ; 9c5e: 29 df       ).
    cmp #&42 ; 'B'                                                    ; 9c60: c9 42       .B
    bne c9c67                                                         ; 9c62: d0 03       ..
    jsr beep                                                          ; 9c64: 20 e8 ac     ..
; &9c67 referenced 2 times by &9c5a, &9c62
.c9c67
    lda l0038                                                         ; 9c67: a5 38       .8
    cmp #&20 ; ' '                                                    ; 9c69: c9 20       .
    beq c9c7f                                                         ; 9c6b: f0 12       ..
    lda ruler_right_stop                                              ; 9c6d: a5 3e       .>
    beq c9c7f                                                         ; 9c6f: f0 0e       ..
    lda format_mode_flag                                              ; 9c71: a5 4f       .O
    bne c9c7f                                                         ; 9c73: d0 0a       ..
    sta tmp7                                                          ; 9c75: 85 8c       ..
    tya                                                               ; 9c77: 98          .
    beq c9c7f                                                         ; 9c78: f0 05       ..
    dey                                                               ; 9c7a: 88          .
    cpy ruler_right_stop                                              ; 9c7b: c4 3e       .>
    bcs c9c82                                                         ; 9c7d: b0 03       ..
; &9c7f referenced 5 times by &9c07, &9c6b, &9c6f, &9c73, &9c78
.c9c7f
    jmp editor_loop                                                   ; 9c7f: 4c 36 9b    L6.

; &9c82 referenced 1 time by &9c7d
.c9c82
    jsr get_line_length                                               ; 9c82: 20 f1 aa     ..
    sty l0083                                                         ; 9c85: 84 83       ..
    lda #0                                                            ; 9c87: a9 00       ..
    sta top_margin                                                    ; 9c89: 85 22       ."
    ldy xpos                                                          ; 9c8b: a4 40       .@
    sty input_buffer_ptr+1                                            ; 9c8d: 84 80       ..
    jsr sub_caf33                                                     ; 9c8f: 20 33 af     3.
    jsr sub_ca608                                                     ; 9c92: 20 08 a6     ..
    lda l0072                                                         ; 9c95: a5 72       .r
    cmp ruler_left_stop                                               ; 9c97: c5 3f       .?
    beq c9c9d                                                         ; 9c99: f0 02       ..
    bcs c9ca2                                                         ; 9c9b: b0 05       ..
; &9c9d referenced 1 time by &9c99
.c9c9d
    ldy input_buffer_ptr+1                                            ; 9c9d: a4 80       ..
    dey                                                               ; 9c9f: 88          .
    sty xpos                                                          ; 9ca0: 84 40       .@
; &9ca2 referenced 1 time by &9c9b
.c9ca2
    lda input_buffer_ptr+1                                            ; 9ca2: a5 80       ..
    sec                                                               ; 9ca4: 38          8
    sbc xpos                                                          ; 9ca5: e5 40       .@
    sta top_margin                                                    ; 9ca7: 85 22       ."
    lda l0083                                                         ; 9ca9: a5 83       ..
    sec                                                               ; 9cab: 38          8
    sbc xpos                                                          ; 9cac: e5 40       .@
    sta l0083                                                         ; 9cae: 85 83       ..
    tay                                                               ; 9cb0: a8          .
    iny                                                               ; 9cb1: c8          .
    lda ruler_left_stop                                               ; 9cb2: a5 3f       .?
    beq c9cb9                                                         ; 9cb4: f0 03       ..
    inc top_margin                                                    ; 9cb6: e6 22       ."
    iny                                                               ; 9cb8: c8          .
; &9cb9 referenced 1 time by &9cb4
.c9cb9
    sty tmp6                                                          ; 9cb9: 84 8b       ..
    lda ptr4                                                          ; 9cbb: a5 08       ..
    sec                                                               ; 9cbd: 38          8
    adc l003b                                                         ; 9cbe: 65 3b       e;
    sta tmp4                                                          ; 9cc0: 85 89       ..
    lda ptr4+1                                                        ; 9cc2: a5 09       ..
    adc #0                                                            ; 9cc4: 69 00       i.
    sta tmp5                                                          ; 9cc6: 85 8a       ..
    jsr sub_caa15                                                     ; 9cc8: 20 15 aa     ..
    bcc c9cd0                                                         ; 9ccb: 90 03       ..
    jmp ca941                                                         ; 9ccd: 4c 41 a9    LA.

; &9cd0 referenced 1 time by &9ccb
.c9cd0
    ldy #0                                                            ; 9cd0: a0 00       ..
    lda ruler_left_stop                                               ; 9cd2: a5 3f       .?
    beq c9cdb                                                         ; 9cd4: f0 05       ..
    lda #&0b                                                          ; 9cd6: a9 0b       ..
    sta (tmp4),y                                                      ; 9cd8: 91 89       ..
    iny                                                               ; 9cda: c8          .              ; Y=&01
; &9cdb referenced 1 time by &9cd4
.c9cdb
    sty l0081                                                         ; 9cdb: 84 81       ..
    lda current_edit_line_ptr                                         ; 9cdd: a5 02       ..
    sta tmp6                                                          ; 9cdf: 85 8b       ..
    lda current_edit_line_ptr+1                                       ; 9ce1: a5 03       ..
    sta tmp7                                                          ; 9ce3: 85 8c       ..
    ldy xpos                                                          ; 9ce5: a4 40       .@
    dey                                                               ; 9ce7: 88          .
    lda (current_edit_line_ptr),y                                     ; 9ce8: b1 02       ..
    cmp #&20 ; ' '                                                    ; 9cea: c9 20       .
    bne c9cf2                                                         ; 9cec: d0 04       ..
    lda #&10                                                          ; 9cee: a9 10       ..
    sta (current_edit_line_ptr),y                                     ; 9cf0: 91 02       ..
; &9cf2 referenced 1 time by &9cec
.c9cf2
    iny                                                               ; 9cf2: c8          .
    sty l0082                                                         ; 9cf3: 84 82       ..
; &9cf5 referenced 1 time by &9d38
.c9cf5
    ldy l0082                                                         ; 9cf5: a4 82       ..
    inc l0082                                                         ; 9cf7: e6 82       ..
; &9cf9 referenced 1 time by &9d0b
.loop_c9cf9
    jsr sub_ca536                                                     ; 9cf9: 20 36 a5     6.
    bne c9d0d                                                         ; 9cfc: d0 0f       ..
    lda l0081                                                         ; 9cfe: a5 81       ..
    clc                                                               ; 9d00: 18          .
    adc tmp4                                                          ; 9d01: 65 89       e.
    sta markers_array,x                                               ; 9d03: 95 53       .S
    lda tmp5                                                          ; 9d05: a5 8a       ..
    adc #0                                                            ; 9d07: 69 00       i.
    sta markers_array+1,x                                             ; 9d09: 95 54       .T
    bcc loop_c9cf9                                                    ; 9d0b: 90 ec       ..
; &9d0d referenced 1 time by &9cfc
.c9d0d
    lda l0083                                                         ; 9d0d: a5 83       ..
    bne c9d28                                                         ; 9d0f: d0 17       ..
    lda #&0d                                                          ; 9d11: a9 0d       ..
    bne c9d30                                                         ; 9d13: d0 1b       ..             ; ALWAYS branch

; &9d15 referenced 1 time by &9d3a
.c9d15
    jsr sub_c9830                                                     ; 9d15: 20 30 98     0.
    jsr ca93c                                                         ; 9d18: 20 3c a9     <.
    jsr ca741                                                         ; 9d1b: 20 41 a7     A.
    jsr return_key                                                    ; 9d1e: 20 7b 9d     {.
    lda top_margin                                                    ; 9d21: a5 22       ."
    sta xpos                                                          ; 9d23: 85 40       .@
    jmp editor_loop                                                   ; 9d25: 4c 36 9b    L6.

; &9d28 referenced 1 time by &9d0f
.c9d28
    lda (current_edit_line_ptr),y                                     ; 9d28: b1 02       ..
    pha                                                               ; 9d2a: 48          H
    lda #&10                                                          ; 9d2b: a9 10       ..
    sta (current_edit_line_ptr),y                                     ; 9d2d: 91 02       ..
    pla                                                               ; 9d2f: 68          h
; &9d30 referenced 1 time by &9d13
.c9d30
    ldy l0081                                                         ; 9d30: a4 81       ..
    inc l0081                                                         ; 9d32: e6 81       ..
    sta (tmp4),y                                                      ; 9d34: 91 89       ..
    dec l0083                                                         ; 9d36: c6 83       ..
    bpl c9cf5                                                         ; 9d38: 10 bb       ..
    bmi c9d15                                                         ; 9d3a: 30 d9       0.             ; ALWAYS branch

; ***************************************************************************************
.sf1_swap_case_key
    ldy xpos                                                          ; 9d3c: a4 40       .@
    lda (current_edit_line_ptr),y                                     ; 9d3e: b1 02       ..
    jsr is_uppercase                                                  ; 9d40: 20 6b 8c     k.
    bcs f13_right_key                                                 ; 9d43: b0 06       ..
    inc l0074                                                         ; 9d45: e6 74       .t
    eor #&20 ; ' '                                                    ; 9d47: 49 20       I
    sta (current_edit_line_ptr),y                                     ; 9d49: 91 02       ..
; ***************************************************************************************
; &9d4b referenced 2 times by &9d43, &9e46
.f13_right_key
    ldy xpos                                                          ; 9d4b: a4 40       .@
    cpy #&84                                                          ; 9d4d: c0 84       ..
    bcs return_51                                                     ; 9d4f: b0 02       ..
    inc xpos                                                          ; 9d51: e6 40       .@
; &9d53 referenced 1 time by &9d4f
.return_51
    rts                                                               ; 9d53: 60          `

; ***************************************************************************************
.f12_left_key
    ldy l0072                                                         ; 9d54: a4 72       .r
    beq return_52                                                     ; 9d56: f0 02       ..
    dec xpos                                                          ; 9d58: c6 40       .@
; &9d5a referenced 1 time by &9d56
.return_52
    rts                                                               ; 9d5a: 60          `

; ***************************************************************************************
.f15_up_key
    jsr ca93c                                                         ; 9d5b: 20 3c a9     <.
    lda ptr4                                                          ; 9d5e: a5 08       ..
    ldy ptr4+1                                                        ; 9d60: a4 09       ..
    jsr sub_cab37                                                     ; 9d62: 20 37 ab     7.
    bcc return_53                                                     ; 9d65: 90 0c       ..
    lda tmp0                                                          ; 9d67: a5 85       ..
    sta ptr4                                                          ; 9d69: 85 08       ..
    lda tmp1                                                          ; 9d6b: a5 86       ..
    sta ptr4+1                                                        ; 9d6d: 85 09       ..
    inc l0079                                                         ; 9d6f: e6 79       .y
    inc l007d                                                         ; 9d71: e6 7d       .}
; &9d73 referenced 1 time by &9d65
.return_53
    rts                                                               ; 9d73: 60          `

; ***************************************************************************************
.f14_down_key
    jsr ca93c                                                         ; 9d74: 20 3c a9     <.
    inc l0079                                                         ; 9d77: e6 79       .y
    bne c9d9b                                                         ; 9d79: d0 20       .
; ***************************************************************************************
; &9d7b referenced 1 time by &9d1e
.return_key
    jsr ca93c                                                         ; 9d7b: 20 3c a9     <.
    lda #0                                                            ; 9d7e: a9 00       ..
    sta xpos                                                          ; 9d80: 85 40       .@
    lda ptr4                                                          ; 9d82: a5 08       ..
    sta tmp0                                                          ; 9d84: 85 85       ..
    lda ptr4+1                                                        ; 9d86: a5 09       ..
    sta tmp1                                                          ; 9d88: 85 86       ..
    jsr cab29                                                         ; 9d8a: 20 29 ab     ).
    bne c9d9b                                                         ; 9d8d: d0 0c       ..
    tya                                                               ; 9d8f: 98          .
    ldy ptr4+1                                                        ; 9d90: a4 09       ..
    clc                                                               ; 9d92: 18          .
    adc ptr4                                                          ; 9d93: 65 08       e.
    bcc c9d98                                                         ; 9d95: 90 01       ..
    iny                                                               ; 9d97: c8          .
; &9d98 referenced 1 time by &9d95
.c9d98
    jsr sub_c9de1                                                     ; 9d98: 20 e1 9d     ..
; &9d9b referenced 2 times by &9d79, &9d8d
.c9d9b
    inc l007d                                                         ; 9d9b: e6 7d       .}
    lda ptr4                                                          ; 9d9d: a5 08       ..
    ldy ptr4+1                                                        ; 9d9f: a4 09       ..
    jsr sub_cab1a                                                     ; 9da1: 20 1a ab     ..
    beq return_54                                                     ; 9da4: f0 0a       ..
    tya                                                               ; 9da6: 98          .
    clc                                                               ; 9da7: 18          .
    adc ptr4                                                          ; 9da8: 65 08       e.
    sta ptr4                                                          ; 9daa: 85 08       ..
    bcc return_54                                                     ; 9dac: 90 02       ..
    inc ptr4+1                                                        ; 9dae: e6 09       ..
; &9db0 referenced 2 times by &9da4, &9dac
.return_54
    rts                                                               ; 9db0: 60          `

; ***************************************************************************************
.cf6_split_line_key
    jsr ca93c                                                         ; 9db1: 20 3c a9     <.
    jsr get_line_length                                               ; 9db4: 20 f1 aa     ..
    cpy xpos                                                          ; 9db7: c4 40       .@
    bcc c9dbd                                                         ; 9db9: 90 02       ..
    ldy xpos                                                          ; 9dbb: a4 40       .@
; &9dbd referenced 1 time by &9db9
.c9dbd
    inc l0079                                                         ; 9dbd: e6 79       .y
    tya                                                               ; 9dbf: 98          .
    tax                                                               ; 9dc0: aa          .
    ldy #0                                                            ; 9dc1: a0 00       ..
    lda (current_format_line_ptr),y                                   ; 9dc3: b1 04       ..
    jsr sub_caf71                                                     ; 9dc5: 20 71 af     q.
    bne c9dcd                                                         ; 9dc8: d0 03       ..
    inx                                                               ; 9dca: e8          .
    inx                                                               ; 9dcb: e8          .
    inx                                                               ; 9dcc: e8          .
; &9dcd referenced 1 time by &9dc8
.c9dcd
    ldy ptr4+1                                                        ; 9dcd: a4 09       ..
    txa                                                               ; 9dcf: 8a          .
    clc                                                               ; 9dd0: 18          .
    adc ptr4                                                          ; 9dd1: 65 08       e.
    bcc c9de3                                                         ; 9dd3: 90 0e       ..
    iny                                                               ; 9dd5: c8          .
    bne c9de3                                                         ; 9dd6: d0 0b       ..
; ***************************************************************************************
; &9dd8 referenced 2 times by &a0db, &a0f2
.f6_insert_line_key
    jsr ca93c                                                         ; 9dd8: 20 3c a9     <.
    lda ptr4                                                          ; 9ddb: a5 08       ..
    ldy ptr4+1                                                        ; 9ddd: a4 09       ..
    inc l0079                                                         ; 9ddf: e6 79       .y
; &9de1 referenced 1 time by &9d98
.sub_c9de1
    inc l007d                                                         ; 9de1: e6 7d       .}
; &9de3 referenced 2 times by &9dd3, &9dd6
.c9de3
    sta tmp4                                                          ; 9de3: 85 89       ..
    sty tmp5                                                          ; 9de5: 84 8a       ..
    lda #1                                                            ; 9de7: a9 01       ..
    sta tmp6                                                          ; 9de9: 85 8b       ..
    lda #0                                                            ; 9deb: a9 00       ..
    sta tmp7                                                          ; 9ded: 85 8c       ..
    jsr sub_caa15                                                     ; 9def: 20 15 aa     ..
    bcs c9dfd                                                         ; 9df2: b0 09       ..
    lda #&0d                                                          ; 9df4: a9 0d       ..
    ldy #0                                                            ; 9df6: a0 00       ..
    sta (tmp4),y                                                      ; 9df8: 91 89       ..
    jmp ca741                                                         ; 9dfa: 4c 41 a7    LA.

; &9dfd referenced 1 time by &9df2
.c9dfd
    jmp ca941                                                         ; 9dfd: 4c 41 a9    LA.

; ***************************************************************************************
.delete_key
    lda l0072                                                         ; 9e00: a5 72       .r
    beq return_55                                                     ; 9e02: f0 2b       .+
    dec xpos                                                          ; 9e04: c6 40       .@
    ldy xpos                                                          ; 9e06: a4 40       .@
    lda (current_edit_line_ptr),y                                     ; 9e08: b1 02       ..
    pha                                                               ; 9e0a: 48          H
    jsr f9_delete_char_key                                            ; 9e0b: 20 49 9e     I.
    pla                                                               ; 9e0e: 68          h
    cmp #&0c                                                          ; 9e0f: c9 0c       ..
    bcc return_55                                                     ; 9e11: 90 1c       ..
    ldx insert_mode_flag                                              ; 9e13: a6 51       .Q
    bne return_55                                                     ; 9e15: d0 18       ..
    jsr get_line_length                                               ; 9e17: 20 f1 aa     ..
    cpy xpos                                                          ; 9e1a: c4 40       .@
    bcc return_55                                                     ; 9e1c: 90 11       ..
    beq return_55                                                     ; 9e1e: f0 0f       ..
; ***************************************************************************************
.f8_insert_char_key
    lda #&20 ; ' '                                                    ; 9e20: a9 20       .
; &9e22 referenced 1 time by &9e41
.sub_c9e22
    pha                                                               ; 9e22: 48          H
    ldx #1                                                            ; 9e23: a2 01       ..
    jsr sub_cae06                                                     ; 9e25: 20 06 ae     ..
    pla                                                               ; 9e28: 68          h
    bcs return_55                                                     ; 9e29: b0 04       ..
    sta (current_edit_line_ptr),y                                     ; 9e2b: 91 02       ..
    inc l0074                                                         ; 9e2d: e6 74       .t
; &9e2f referenced 8 times by &9e02, &9e11, &9e15, &9e1c, &9e1e, &9e29, &9e3f, &9e44
.return_55
    rts                                                               ; 9e2f: 60          `

; ***************************************************************************************
.tab_key
    lda #9                                                            ; 9e30: a9 09       ..
    bne c9e3a                                                         ; 9e32: d0 06       ..             ; ALWAYS branch

; ***************************************************************************************
.sf4_highlight1_key
    lda #&1c                                                          ; 9e34: a9 1c       ..
    bne c9e3a                                                         ; 9e36: d0 02       ..             ; ALWAYS branch

; ***************************************************************************************
.sf5_highlight2_key
    lda #&1d                                                          ; 9e38: a9 1d       ..
; &9e3a referenced 2 times by &9e32, &9e36
.c9e3a
    pha                                                               ; 9e3a: 48          H
    jsr sub_caef4                                                     ; 9e3b: 20 f4 ae     ..
    pla                                                               ; 9e3e: 68          h
    bcs return_55                                                     ; 9e3f: b0 ee       ..
    jsr sub_c9e22                                                     ; 9e41: 20 22 9e     ".
    bcs return_55                                                     ; 9e44: b0 e9       ..
    jmp f13_right_key                                                 ; 9e46: 4c 4b 9d    LK.

; ***************************************************************************************
; &9e49 referenced 1 time by &9e0b
.f9_delete_char_key
    ldx #1                                                            ; 9e49: a2 01       ..
    inc l0074                                                         ; 9e4b: e6 74       .t
    jmp cae64                                                         ; 9e4d: 4c 64 ae    Ld.

; ***************************************************************************************
.f7_delete_line_key
    jsr ca93c                                                         ; 9e50: 20 3c a9     <.
    inc l007d                                                         ; 9e53: e6 7d       .}
    lda ptr4                                                          ; 9e55: a5 08       ..
    sta tmp4                                                          ; 9e57: 85 89       ..
    lda ptr4+1                                                        ; 9e59: a5 09       ..
    sta tmp5                                                          ; 9e5b: 85 8a       ..
    ldx l003b                                                         ; 9e5d: a6 3b       .;
    inx                                                               ; 9e5f: e8          .
    stx tmp6                                                          ; 9e60: 86 8b       ..
    lda #0                                                            ; 9e62: a9 00       ..
    sta tmp7                                                          ; 9e64: 85 8c       ..
    jsr sub_ca9b0                                                     ; 9e66: 20 b0 a9     ..
    jsr cb05a                                                         ; 9e69: 20 5a b0     Z.
    ldy #0                                                            ; 9e6c: a0 00       ..
    lda (ptr4),y                                                      ; 9e6e: b1 08       ..
    bne c9e81                                                         ; 9e70: d0 0f       ..
    lda ptr4                                                          ; 9e72: a5 08       ..
    ldy ptr4+1                                                        ; 9e74: a4 09       ..
    jsr sub_cab37                                                     ; 9e76: 20 37 ab     7.
    lda tmp0                                                          ; 9e79: a5 85       ..
    sta ptr4                                                          ; 9e7b: 85 08       ..
    lda tmp1                                                          ; 9e7d: a5 86       ..
    sta ptr4+1                                                        ; 9e7f: 85 09       ..
; &9e81 referenced 1 time by &9e70
.c9e81
    inc l0079                                                         ; 9e81: e6 79       .y
    jmp ca741                                                         ; 9e83: 4c 41 a7    LA.

; ***************************************************************************************
.sf2_release_margins_key
    bit format_mode_flag                                              ; 9e86: 24 4f       $O
    bvc c9e94                                                         ; 9e88: 50 0a       P.
    jsr sub_caec2                                                     ; 9e8a: 20 c2 ae     ..
    bcs f4_beginning_of_line_key                                      ; 9e8d: b0 03       ..
    sty xpos                                                          ; 9e8f: 84 40       .@
    rts                                                               ; 9e91: 60          `

; ***************************************************************************************
; &9e92 referenced 1 time by &9e8d
.f4_beginning_of_line_key
    inc l007d                                                         ; 9e92: e6 7d       .}
; &9e94 referenced 5 times by &9a8d, &9e88, &9ee8, &9fc8, &a068
.c9e94
    lda #0                                                            ; 9e94: a9 00       ..
    sta xpos                                                          ; 9e96: 85 40       .@
    rts                                                               ; 9e98: 60          `

; ***************************************************************************************
.f5_end_of_line_key
    inc l007d                                                         ; 9e99: e6 7d       .}
; &9e9b referenced 2 times by &9f97, &a0a6
.c9e9b
    jsr get_line_length                                               ; 9e9b: 20 f1 aa     ..
    sty xpos                                                          ; 9e9e: 84 40       .@
    rts                                                               ; 9ea0: 60          `

; ***************************************************************************************
.cf7_join_lines_key
    jsr ca93c                                                         ; 9ea1: 20 3c a9     <.
    lda ptr4                                                          ; 9ea4: a5 08       ..
    sta tmp0                                                          ; 9ea6: 85 85       ..
    lda ptr4+1                                                        ; 9ea8: a5 09       ..
    sta tmp1                                                          ; 9eaa: 85 86       ..
    jsr cab29                                                         ; 9eac: 20 29 ab     ).
    beq c9eda                                                         ; 9eaf: f0 29       .)
    jsr sub_caf71                                                     ; 9eb1: 20 71 af     q.
    beq c9eda                                                         ; 9eb4: f0 24       .$
    dey                                                               ; 9eb6: 88          .
    tya                                                               ; 9eb7: 98          .
    clc                                                               ; 9eb8: 18          .
    adc ptr4                                                          ; 9eb9: 65 08       e.
    sta tmp4                                                          ; 9ebb: 85 89       ..
    lda ptr4+1                                                        ; 9ebd: a5 09       ..
    adc #0                                                            ; 9ebf: 69 00       i.
    sta tmp5                                                          ; 9ec1: 85 8a       ..
    lda #0                                                            ; 9ec3: a9 00       ..
    sta tmp7                                                          ; 9ec5: 85 8c       ..
    lda #1                                                            ; 9ec7: a9 01       ..
    sta tmp6                                                          ; 9ec9: 85 8b       ..
    jsr sub_ca9b0                                                     ; 9ecb: 20 b0 a9     ..
    lda ptr4                                                          ; 9ece: a5 08       ..
    ldy ptr4+1                                                        ; 9ed0: a4 09       ..
    jsr cac78                                                         ; 9ed2: 20 78 ac     x.
    inc l0079                                                         ; 9ed5: e6 79       .y
    jmp ca741                                                         ; 9ed7: 4c 41 a7    LA.

; &9eda referenced 2 times by &9eaf, &9eb4
.c9eda
    jmp beep                                                          ; 9eda: 4c e8 ac    L..

; ***************************************************************************************
.f3_delete_to_eol_key
    lda #&84                                                          ; 9edd: a9 84       ..
    sec                                                               ; 9edf: 38          8
    sbc xpos                                                          ; 9ee0: e5 40       .@
    tax                                                               ; 9ee2: aa          .
    inc l0074                                                         ; 9ee3: e6 74       .t
    jmp cae64                                                         ; 9ee5: 4c 64 ae    Ld.

; ***************************************************************************************
.sf8_edit_command_key
    jsr c9e94                                                         ; 9ee8: 20 94 9e     ..
    jsr sub_ca276                                                     ; 9eeb: 20 76 a2     v.
    inc l006d                                                         ; 9eee: e6 6d       .m
    lda #0                                                            ; 9ef0: a9 00       ..
    sta input_buffer_ptr+1                                            ; 9ef2: 85 80       ..
    sta l0081                                                         ; 9ef4: 85 81       ..
; &9ef6 referenced 4 times by &9f0a, &9f0e, &9f1e, &9f24
.edit_command_loop
    ldx input_buffer_ptr+1                                            ; 9ef6: a6 80       ..
    ldy ypos                                                          ; 9ef8: a4 77       .w
    jsr set_cursor_position                                           ; 9efa: 20 ea a7     ..
    jsr read_char                                                     ; 9efd: 20 61 a7     a.
    bcs finished_editing_command                                      ; 9f00: b0 24       .$
    cmp #&0d                                                          ; 9f02: c9 0d       ..
    beq finished_editing_command                                      ; 9f04: f0 20       .
    and #&df                                                          ; 9f06: 29 df       ).
    cmp #&41 ; 'A'                                                    ; 9f08: c9 41       .A
    bcc edit_command_loop                                             ; 9f0a: 90 ea       ..
    cmp #&5b ; '['                                                    ; 9f0c: c9 5b       .[
    bcs edit_command_loop                                             ; 9f0e: b0 e6       ..
    sta l0081                                                         ; 9f10: 85 81       ..
    jsr oswrch                                                        ; 9f12: 20 ee ff     ..            ; Write character
    ldy input_buffer_ptr+1                                            ; 9f15: a4 80       ..
    iny                                                               ; 9f17: c8          .
    sty input_buffer_ptr+1                                            ; 9f18: 84 80       ..
    sta (ptr1),y                                                      ; 9f1a: 91 00       ..
    cpy #2                                                            ; 9f1c: c0 02       ..
    bcc edit_command_loop                                             ; 9f1e: 90 d6       ..
    lda #0                                                            ; 9f20: a9 00       ..
    sta input_buffer_ptr+1                                            ; 9f22: 85 80       ..
    beq edit_command_loop                                             ; 9f24: f0 d0       ..             ; ALWAYS branch

; &9f26 referenced 2 times by &9f00, &9f04
.finished_editing_command
    lda l0081                                                         ; 9f26: a5 81       ..
    beq return_56                                                     ; 9f28: f0 55       .U
    lda ptr1                                                          ; 9f2a: a5 00       ..
    sta current_format_line_ptr                                       ; 9f2c: 85 04       ..
    lda ptr1+1                                                        ; 9f2e: a5 01       ..
    sta current_format_line_ptr+1                                     ; 9f30: 85 05       ..
    ldy #0                                                            ; 9f32: a0 00       ..
    lda #&80                                                          ; 9f34: a9 80       ..
    sta (current_format_line_ptr),y                                   ; 9f36: 91 04       ..
    jmp caf5c                                                         ; 9f38: 4c 5c af    L\.

; ***************************************************************************************
; &9f3b referenced 2 times by &a0ef, &a0f8
.cf8_mark_as_ruler_key
    lda ptr1                                                          ; 9f3b: a5 00       ..
    sta current_format_line_ptr                                       ; 9f3d: 85 04       ..
    lda ptr1+1                                                        ; 9f3f: a5 01       ..
    sta current_format_line_ptr+1                                     ; 9f41: 85 05       ..
    ldy #0                                                            ; 9f43: a0 00       ..
    lda #&81                                                          ; 9f45: a9 81       ..
    sta (current_format_line_ptr),y                                   ; 9f47: 91 04       ..
    iny                                                               ; 9f49: c8          .              ; Y=&01
    lda #&2e ; '.'                                                    ; 9f4a: a9 2e       ..
    sta (current_format_line_ptr),y                                   ; 9f4c: 91 04       ..
    iny                                                               ; 9f4e: c8          .              ; Y=&02
    lda #&2e ; '.'                                                    ; 9f4f: a9 2e       ..
    sta (current_format_line_ptr),y                                   ; 9f51: 91 04       ..
    inc l0074                                                         ; 9f53: e6 74       .t
    lda l006e                                                         ; 9f55: a5 6e       .n
    bmi c9f5f                                                         ; 9f57: 30 06       0.
    lda #&80                                                          ; 9f59: a9 80       ..
    sta l006e                                                         ; 9f5b: 85 6e       .n
    inc l006d                                                         ; 9f5d: e6 6d       .m
; &9f5f referenced 1 time by &9f57
.c9f5f
    jmp caf5c                                                         ; 9f5f: 4c 5c af    L\.

; ***************************************************************************************
.sf9_delete_command_key
    ldy #0                                                            ; 9f62: a0 00       ..
    lda (current_format_line_ptr),y                                   ; 9f64: b1 04       ..
    jsr sub_caf71                                                     ; 9f66: 20 71 af     q.
    bne return_56                                                     ; 9f69: d0 14       ..
    tya                                                               ; 9f6b: 98          .
    sta (current_format_line_ptr),y                                   ; 9f6c: 91 04       ..
    lda current_edit_line_ptr                                         ; 9f6e: a5 02       ..
    sta current_format_line_ptr                                       ; 9f70: 85 04       ..
    lda current_edit_line_ptr+1                                       ; 9f72: a5 03       ..
    sta current_format_line_ptr+1                                     ; 9f74: 85 05       ..
    jsr sub_caf5f                                                     ; 9f76: 20 5f af     _.
    inc l0074                                                         ; 9f79: e6 74       .t
    inc l006d                                                         ; 9f7b: e6 6d       .m
    inc l007d                                                         ; 9f7d: e6 7d       .}
; &9f7f referenced 3 times by &9f28, &9f69, &9f8a
.return_56
    rts                                                               ; 9f7f: 60          `

; &9f80 referenced 2 times by &9f9f, &9fa8
.c9f80
    jsr ca93c                                                         ; 9f80: 20 3c a9     <.
    lda ptr4                                                          ; 9f83: a5 08       ..
    ldy ptr4+1                                                        ; 9f85: a4 09       ..
    jsr sub_cab37                                                     ; 9f87: 20 37 ab     7.
    bcc return_56                                                     ; 9f8a: 90 f3       ..
    lda tmp0                                                          ; 9f8c: a5 85       ..
    sta ptr4                                                          ; 9f8e: 85 08       ..
    lda tmp1                                                          ; 9f90: a5 86       ..
    sta ptr4+1                                                        ; 9f92: 85 09       ..
    jsr sub_caa97                                                     ; 9f94: 20 97 aa     ..
    jsr c9e9b                                                         ; 9f97: 20 9b 9e     ..
    dec l006f                                                         ; 9f9a: c6 6f       .o
    rts                                                               ; 9f9c: 60          `

; ***************************************************************************************
.sf12_left_key
    ldy xpos                                                          ; 9f9d: a4 40       .@
    beq c9f80                                                         ; 9f9f: f0 df       ..
    jsr sub_caf33                                                     ; 9fa1: 20 33 af     3.
    bne return_57                                                     ; 9fa4: d0 04       ..
    cmp #&20 ; ' '                                                    ; 9fa6: c9 20       .
    beq c9f80                                                         ; 9fa8: f0 d6       ..
; &9faa referenced 1 time by &9fa4
.return_57
    rts                                                               ; 9faa: 60          `

; &9fab referenced 2 times by &9ff2, &9ff4
.c9fab
    sty xpos                                                          ; 9fab: 84 40       .@
    jsr ca93c                                                         ; 9fad: 20 3c a9     <.
    lda ptr4                                                          ; 9fb0: a5 08       ..
    ldy ptr4+1                                                        ; 9fb2: a4 09       ..
    jsr sub_cab1a                                                     ; 9fb4: 20 1a ab     ..
    beq return_58                                                     ; 9fb7: f0 58       .X
    tya                                                               ; 9fb9: 98          .
    clc                                                               ; 9fba: 18          .
    adc ptr4                                                          ; 9fbb: 65 08       e.
    sta ptr4                                                          ; 9fbd: 85 08       ..
    bcc c9fc3                                                         ; 9fbf: 90 02       ..
    inc ptr4+1                                                        ; 9fc1: e6 09       ..
; &9fc3 referenced 1 time by &9fbf
.c9fc3
    jsr sub_caa97                                                     ; 9fc3: 20 97 aa     ..
    dec l006f                                                         ; 9fc6: c6 6f       .o
    jsr c9e94                                                         ; 9fc8: 20 94 9e     ..
    jsr get_line_length                                               ; 9fcb: 20 f1 aa     ..
    cpy xpos                                                          ; 9fce: c4 40       .@
    beq return_58                                                     ; 9fd0: f0 3f       .?
    lda current_edit_line_ptr                                         ; 9fd2: a5 02       ..
    sta tmp0                                                          ; 9fd4: 85 85       ..
    lda current_edit_line_ptr+1                                       ; 9fd6: a5 03       ..
    sta tmp1                                                          ; 9fd8: 85 86       ..
    ldy xpos                                                          ; 9fda: a4 40       .@
    jsr sub_ca5ab                                                     ; 9fdc: 20 ab a5     ..
    cmp #&20 ; ' '                                                    ; 9fdf: c9 20       .
    bne return_58                                                     ; 9fe1: d0 2e       ..
; ***************************************************************************************
.sf13_right_key
    lda current_edit_line_ptr                                         ; 9fe3: a5 02       ..
    sta tmp0                                                          ; 9fe5: 85 85       ..
    lda current_edit_line_ptr+1                                       ; 9fe7: a5 03       ..
    sta tmp1                                                          ; 9fe9: 85 86       ..
    jsr get_line_length                                               ; 9feb: 20 f1 aa     ..
    sty input_buffer_ptr+1                                            ; 9fee: 84 80       ..
    cpy xpos                                                          ; 9ff0: c4 40       .@
    bcc c9fab                                                         ; 9ff2: 90 b7       ..
    beq c9fab                                                         ; 9ff4: f0 b5       ..
    ldy xpos                                                          ; 9ff6: a4 40       .@
; &9ff8 referenced 1 time by &a001
.loop_c9ff8
    cpy input_buffer_ptr+1                                            ; 9ff8: c4 80       ..
    bcs ca00f                                                         ; 9ffa: b0 13       ..
    jsr sub_ca5ab                                                     ; 9ffc: 20 ab a5     ..
    cmp #&20 ; ' '                                                    ; 9fff: c9 20       .
    bne loop_c9ff8                                                    ; a001: d0 f5       ..
; &a003 referenced 1 time by &a00c
.loop_ca003
    cpy input_buffer_ptr+1                                            ; a003: c4 80       ..
    bcs ca00f                                                         ; a005: b0 08       ..
    jsr sub_ca5ab                                                     ; a007: 20 ab a5     ..
    cmp #&20 ; ' '                                                    ; a00a: c9 20       .
    beq loop_ca003                                                    ; a00c: f0 f5       ..
    dey                                                               ; a00e: 88          .
; &a00f referenced 2 times by &9ffa, &a005
.ca00f
    sty xpos                                                          ; a00f: 84 40       .@
; &a011 referenced 6 times by &9fb7, &9fd0, &9fe1, &a018, &a02a, &a02c
.return_58
    rts                                                               ; a011: 60          `

; ***************************************************************************************
.sf7_set_marker_key
    jsr ca93c                                                         ; a012: 20 3c a9     <.
    jsr sub_cacd7                                                     ; a015: 20 d7 ac     ..
    bcs return_58                                                     ; a018: b0 f7       ..
    txa                                                               ; a01a: 8a          .
    adc #&53 ; 'S'                                                    ; a01b: 69 53       iS
    tax                                                               ; a01d: aa          .
    jsr sub_cad47                                                     ; a01e: 20 47 ad     G.
    jmp ca035                                                         ; a021: 4c 35 a0    L5.

; ***************************************************************************************
.sf6_go_to_marker_key
    jsr ca93c                                                         ; a024: 20 3c a9     <.
    jsr sub_cacd7                                                     ; a027: 20 d7 ac     ..
    bcs return_58                                                     ; a02a: b0 e5       ..
    beq return_58                                                     ; a02c: f0 e3       ..
    lda markers_array,x                                               ; a02e: b5 53       .S
    ldy markers_array+1,x                                             ; a030: b4 54       .T
    jsr cabcb                                                         ; a032: 20 cb ab     ..
; &a035 referenced 1 time by &a021
.ca035
    lda #1                                                            ; a035: a9 01       ..
    sta l0073                                                         ; a037: 85 73       .s
    jmp ca684                                                         ; a039: 4c 84 a6    L..

; ***************************************************************************************
.f0_format_block_key
    jsr ca93c                                                         ; a03c: 20 3c a9     <.
    lda l0073                                                         ; a03f: a5 73       .s
    pha                                                               ; a041: 48          H
    lda l003d                                                         ; a042: a5 3d       .=
    pha                                                               ; a044: 48          H
    jsr ca741                                                         ; a045: 20 41 a7     A.
    jsr sub_c9977                                                     ; a048: 20 77 99     w.
    bvs ca05b                                                         ; a04b: 70 0e       p.
    sec                                                               ; a04d: 38          8
    bne ca051                                                         ; a04e: d0 01       ..
    clc                                                               ; a050: 18          .
; &a051 referenced 1 time by &a04e
.ca051
    pla                                                               ; a051: 68          h
    tax                                                               ; a052: aa          .
    pla                                                               ; a053: 68          h
    bcs return_59                                                     ; a054: b0 04       ..
    stx l003d                                                         ; a056: 86 3d       .=
    sta l0073                                                         ; a058: 85 73       .s
; &a05a referenced 1 time by &a054
.return_59
    rts                                                               ; a05a: 60          `

; &a05b referenced 1 time by &a04b
.ca05b
    jmp ca941                                                         ; a05b: 4c 41 a9    LA.

; ***************************************************************************************
.f1_top_of_text_key
    ldx #&ff                                                          ; a05e: a2 ff       ..
    stx l006f                                                         ; a060: 86 6f       .o
    jsr sub_ca071                                                     ; a062: 20 71 a0     q.
    jsr sub_caa97                                                     ; a065: 20 97 aa     ..
    jmp c9e94                                                         ; a068: 4c 94 9e    L..

; ***************************************************************************************
.sf15_up_key
    ldx screen_height                                                 ; a06b: a6 35       .5
    inc l0079                                                         ; a06d: e6 79       .y
    inc l006f                                                         ; a06f: e6 6f       .o
; &a071 referenced 1 time by &a062
.sub_ca071
    inc l007d                                                         ; a071: e6 7d       .}
    stx input_buffer_ptr+1                                            ; a073: 86 80       ..
    jsr ca93c                                                         ; a075: 20 3c a9     <.
    lda ptr4                                                          ; a078: a5 08       ..
    ldy ptr4+1                                                        ; a07a: a4 09       ..
; &a07c referenced 2 times by &a08b, &a08f
.ca07c
    sta tmp2                                                          ; a07c: 85 87       ..
    sty tmp3                                                          ; a07e: 84 88       ..
    jsr sub_cab37                                                     ; a080: 20 37 ab     7.
    lda tmp0                                                          ; a083: a5 85       ..
    ldy tmp1                                                          ; a085: a4 86       ..
    bcc ca093                                                         ; a087: 90 0a       ..
    ldx input_buffer_ptr+1                                            ; a089: a6 80       ..
    bmi ca07c                                                         ; a08b: 30 ef       0.
    dec input_buffer_ptr+1                                            ; a08d: c6 80       ..
    bne ca07c                                                         ; a08f: d0 eb       ..
    beq ca097                                                         ; a091: f0 04       ..             ; ALWAYS branch

; &a093 referenced 1 time by &a087
.ca093
    lda tmp2                                                          ; a093: a5 87       ..
    ldy tmp3                                                          ; a095: a4 88       ..
; &a097 referenced 1 time by &a091
.ca097
    sta ptr4                                                          ; a097: 85 08       ..
    sty ptr4+1                                                        ; a099: 84 09       ..
    rts                                                               ; a09b: 60          `

; ***************************************************************************************
.f2_bottom_of_text_key
    ldx #&ff                                                          ; a09c: a2 ff       ..
    stx l006f                                                         ; a09e: 86 6f       .o
    jsr sub_ca0af                                                     ; a0a0: 20 af a0     ..
    jsr sub_caa97                                                     ; a0a3: 20 97 aa     ..
    jmp c9e9b                                                         ; a0a6: 4c 9b 9e    L..

; ***************************************************************************************
.sf14_down_key
    ldx screen_height                                                 ; a0a9: a6 35       .5
    inc l0079                                                         ; a0ab: e6 79       .y
    inc l006f                                                         ; a0ad: e6 6f       .o
; &a0af referenced 1 time by &a0a0
.sub_ca0af
    inc l007d                                                         ; a0af: e6 7d       .}
    stx input_buffer_ptr+1                                            ; a0b1: 86 80       ..
    jsr ca93c                                                         ; a0b3: 20 3c a9     <.
    lda ptr4                                                          ; a0b6: a5 08       ..
    ldy ptr4+1                                                        ; a0b8: a4 09       ..
; &a0ba referenced 2 times by &a0ca, &a0ce
.ca0ba
    jsr sub_cab1a                                                     ; a0ba: 20 1a ab     ..
    beq ca0d2                                                         ; a0bd: f0 13       ..
    tya                                                               ; a0bf: 98          .
    ldy tmp1                                                          ; a0c0: a4 86       ..
    clc                                                               ; a0c2: 18          .
    adc tmp0                                                          ; a0c3: 65 85       e.
    bcc ca0c8                                                         ; a0c5: 90 01       ..
    iny                                                               ; a0c7: c8          .
; &a0c8 referenced 1 time by &a0c5
.ca0c8
    ldx input_buffer_ptr+1                                            ; a0c8: a6 80       ..
    bmi ca0ba                                                         ; a0ca: 30 ee       0.
    dec input_buffer_ptr+1                                            ; a0cc: c6 80       ..
    bne ca0ba                                                         ; a0ce: d0 ea       ..
    beq ca0d6                                                         ; a0d0: f0 04       ..             ; ALWAYS branch

; &a0d2 referenced 1 time by &a0bd
.ca0d2
    lda tmp0                                                          ; a0d2: a5 85       ..
    ldy tmp1                                                          ; a0d4: a4 86       ..
; &a0d6 referenced 1 time by &a0d0
.ca0d6
    sta ptr4                                                          ; a0d6: 85 08       ..
    sty ptr4+1                                                        ; a0d8: 84 09       ..
    rts                                                               ; a0da: 60          `

; ***************************************************************************************
.sf11_copy_key
    jsr f6_insert_line_key                                            ; a0db: 20 d8 9d     ..
    jsr sub_ca276                                                     ; a0de: 20 76 a2     v.
    ldx l003a                                                         ; a0e1: a6 3a       .:
    beq ca0ef                                                         ; a0e3: f0 0a       ..
    ldy #0                                                            ; a0e5: a0 00       ..
; &a0e7 referenced 1 time by &a0ed
.loop_ca0e7
    lda (current_ruler_ptr),y                                         ; a0e7: b1 06       ..
    sta (current_edit_line_ptr),y                                     ; a0e9: 91 02       ..
    iny                                                               ; a0eb: c8          .
    dex                                                               ; a0ec: ca          .
    bne loop_ca0e7                                                    ; a0ed: d0 f8       ..
; &a0ef referenced 1 time by &a0e3
.ca0ef
    jmp cf8_mark_as_ruler_key                                         ; a0ef: 4c 3b 9f    L;.

; ***************************************************************************************
.cf5_default_ruler_key
    jsr f6_insert_line_key                                            ; a0f2: 20 d8 9d     ..
    jsr sub_ca276                                                     ; a0f5: 20 76 a2     v.
    jsr cf8_mark_as_ruler_key                                         ; a0f8: 20 3b 9f     ;.
    lda current_edit_line_ptr                                         ; a0fb: a5 02       ..
    ldy current_edit_line_ptr+1                                       ; a0fd: a4 03       ..
    jmp create_default_ruler                                          ; a0ff: 4c e0 b0    L..

; ***************************************************************************************
.sf3_delete_to_char_key
    ldx #&43 ; 'C'                                                    ; a102: a2 43       .C
    ldy #&48 ; 'H'                                                    ; a104: a0 48       .H
    jsr draw_prompt_characters                                        ; a106: 20 a8 a7     ..
    inc l0075                                                         ; a109: e6 75       .u
    jsr read_char                                                     ; a10b: 20 61 a7     a.
    cmp #9                                                            ; a10e: c9 09       ..
    beq ca12a                                                         ; a110: f0 18       ..
    cmp #&a0                                                          ; a112: c9 a0       ..
    bne ca11a                                                         ; a114: d0 04       ..
    lda #&1c                                                          ; a116: a9 1c       ..
    bne ca12a                                                         ; a118: d0 10       ..             ; ALWAYS branch

; &a11a referenced 1 time by &a114
.ca11a
    cmp #&a1                                                          ; a11a: c9 a1       ..
    bne ca122                                                         ; a11c: d0 04       ..
    lda #&1d                                                          ; a11e: a9 1d       ..
    bne ca12a                                                         ; a120: d0 08       ..             ; ALWAYS branch

; &a122 referenced 1 time by &a11c
.ca122
    cmp #&20 ; ' '                                                    ; a122: c9 20       .
    bcc ca151                                                         ; a124: 90 2b       .+
    cmp #&7f                                                          ; a126: c9 7f       ..
    bcs ca151                                                         ; a128: b0 27       .'
; &a12a referenced 3 times by &a110, &a118, &a120
.ca12a
    sta input_buffer_ptr+1                                            ; a12a: 85 80       ..
    inc l0074                                                         ; a12c: e6 74       .t
    ldy xpos                                                          ; a12e: a4 40       .@
    sty l0081                                                         ; a130: 84 81       ..
; &a132 referenced 1 time by &a13b
.loop_ca132
    cpy #&84                                                          ; a132: c0 84       ..
    bcs ca151                                                         ; a134: b0 1b       ..
    lda (current_edit_line_ptr),y                                     ; a136: b1 02       ..
    iny                                                               ; a138: c8          .
    cmp input_buffer_ptr+1                                            ; a139: c5 80       ..
    bne loop_ca132                                                    ; a13b: d0 f5       ..
; &a13d referenced 1 time by &a146
.loop_ca13d
    cpy #&84                                                          ; a13d: c0 84       ..
    bcs ca151                                                         ; a13f: b0 10       ..
    lda (current_edit_line_ptr),y                                     ; a141: b1 02       ..
    iny                                                               ; a143: c8          .
    cmp input_buffer_ptr+1                                            ; a144: c5 80       ..
    beq loop_ca13d                                                    ; a146: f0 f5       ..
    dey                                                               ; a148: 88          .
    tya                                                               ; a149: 98          .
    sec                                                               ; a14a: 38          8
    sbc l0081                                                         ; a14b: e5 81       ..
    tax                                                               ; a14d: aa          .
    jmp cae64                                                         ; a14e: 4c 64 ae    Ld.

; &a151 referenced 4 times by &a124, &a128, &a134, &a13f
.ca151
    jmp beep                                                          ; a151: 4c e8 ac    L..

; ***************************************************************************************
.cf2_format_mode_key
    lda format_mode_flag                                              ; a154: a5 4f       .O
    and #&bf                                                          ; a156: 29 bf       ).
    bit format_mode_flag                                              ; a158: 24 4f       $O
    bvc ca15e                                                         ; a15a: 50 02       P.
    ora #1                                                            ; a15c: 09 01       ..
; &a15e referenced 1 time by &a15a
.ca15e
    eor #1                                                            ; a15e: 49 01       I.
    sta format_mode_flag                                              ; a160: 85 4f       .O
    inc l0075                                                         ; a162: e6 75       .u
    rts                                                               ; a164: 60          `

; ***************************************************************************************
.cf3_justify_mode_key
    lda justifying_flag                                               ; a165: a5 50       .P
    eor #&ff                                                          ; a167: 49 ff       I.
    sta justifying_flag                                               ; a169: 85 50       .P
    inc l0075                                                         ; a16b: e6 75       .u
    rts                                                               ; a16d: 60          `

; ***************************************************************************************
.cf4_insert_mode_key
    lda insert_mode_flag                                              ; a16e: a5 51       .Q
    eor #&ff                                                          ; a170: 49 ff       I.
    sta insert_mode_flag                                              ; a172: 85 51       .Q
    inc l0075                                                         ; a174: e6 75       .u
; &a176 referenced 1 time by &a19f
.return_60
    rts                                                               ; a176: 60          `

; ***************************************************************************************
.cf0_delete_block_key
    jsr ca93c                                                         ; a177: 20 3c a9     <.
    inc l007d                                                         ; a17a: e6 7d       .}
    jsr reset_area_to_marks_1_2                                       ; a17c: 20 18 ad     ..
    bcs ca1c9                                                         ; a17f: b0 48       .H
    lda area_start_ptr                                                ; a181: a5 5f       ._
    ldy area_start_ptr+1                                              ; a183: a4 60       .`
    jsr cabcb                                                         ; a185: 20 cb ab     ..
    jsr ca741                                                         ; a188: 20 41 a7     A.
    jsr sub_c89d3                                                     ; a18b: 20 d3 89     ..
    jsr cb05a                                                         ; a18e: 20 5a b0     Z.
    jmp clear_marks_1_2                                               ; a191: 4c 0e ad    L..

; ***************************************************************************************
.sf0_move_block_key
    jsr ca93c                                                         ; a194: 20 3c a9     <.
    jsr reset_area_to_marks_1_2                                       ; a197: 20 18 ad     ..
    bcs ca1c9                                                         ; a19a: b0 2d       .-
    jsr sub_ca1cc                                                     ; a19c: 20 cc a1     ..
    bcs return_60                                                     ; a19f: b0 d5       ..
    ldx #&ff                                                          ; a1a1: a2 ff       ..
    stx l0012                                                         ; a1a3: 86 12       ..
    stx l006f                                                         ; a1a5: 86 6f       .o
    jsr sub_c89d3                                                     ; a1a7: 20 d3 89     ..
    jsr cb05a                                                         ; a1aa: 20 5a b0     Z.
    lda l0063                                                         ; a1ad: a5 63       .c
    ldy l0064                                                         ; a1af: a4 64       .d
    jsr cabcb                                                         ; a1b1: 20 cb ab     ..
    jmp clear_marks_1_2                                               ; a1b4: 4c 0e ad    L..

; ***************************************************************************************
.f11_copy_key
    jsr ca93c                                                         ; a1b7: 20 3c a9     <.
    jsr reset_area_to_marks_1_2                                       ; a1ba: 20 18 ad     ..
    bcs ca1c9                                                         ; a1bd: b0 0a       ..
    jsr sub_ca1cc                                                     ; a1bf: 20 cc a1     ..
    lda l0063                                                         ; a1c2: a5 63       .c
    ldy l0064                                                         ; a1c4: a4 64       .d
    jmp cabcb                                                         ; a1c6: 4c cb ab    L..

; &a1c9 referenced 6 times by &a17f, &a19a, &a1bd, &a1dc, &a1e2, &a1e4
.ca1c9
    jmp beep                                                          ; a1c9: 4c e8 ac    L..

; &a1cc referenced 2 times by &a19c, &a1bf
.sub_ca1cc
    lda l0063                                                         ; a1cc: a5 63       .c
    ldy l0064                                                         ; a1ce: a4 64       .d
    cpy area_start_ptr+1                                              ; a1d0: c4 60       .`
    bcc ca1ea                                                         ; a1d2: 90 16       ..
    bne ca1da                                                         ; a1d4: d0 04       ..
    cmp area_start_ptr                                                ; a1d6: c5 5f       ._
    bcc ca1ea                                                         ; a1d8: 90 10       ..
; &a1da referenced 1 time by &a1d4
.ca1da
    cpy area_end_ptr+1                                                ; a1da: c4 62       .b
    bcc ca1c9                                                         ; a1dc: 90 eb       ..
    bne ca1e6                                                         ; a1de: d0 06       ..
    cmp area_end_ptr                                                  ; a1e0: c5 61       .a
    bcc ca1c9                                                         ; a1e2: 90 e5       ..
    beq ca1c9                                                         ; a1e4: f0 e3       ..
; &a1e6 referenced 1 time by &a1de
.ca1e6
    lda area_start_ptr                                                ; a1e6: a5 5f       ._
    ldy area_start_ptr+1                                              ; a1e8: a4 60       .`
; &a1ea referenced 2 times by &a1d2, &a1d8
.ca1ea
    jsr cabcb                                                         ; a1ea: 20 cb ab     ..
    lda area_end_ptr                                                  ; a1ed: a5 61       .a
    sec                                                               ; a1ef: 38          8
    sbc area_start_ptr                                                ; a1f0: e5 5f       ._
    sta tmp6                                                          ; a1f2: 85 8b       ..
    lda area_end_ptr+1                                                ; a1f4: a5 62       .b
    sbc area_start_ptr+1                                              ; a1f6: e5 60       .`
    sta tmp7                                                          ; a1f8: 85 8c       ..
    lda l0063                                                         ; a1fa: a5 63       .c
    sta tmp4                                                          ; a1fc: 85 89       ..
    lda l0064                                                         ; a1fe: a5 64       .d
    sta tmp5                                                          ; a200: 85 8a       ..
    jsr sub_caa15                                                     ; a202: 20 15 aa     ..
    bcs ca265                                                         ; a205: b0 5e       .^
    lda area_start_ptr                                                ; a207: a5 5f       ._
    sta tmp8                                                          ; a209: 85 8d       ..
    lda area_start_ptr+1                                              ; a20b: a5 60       .`
    sta tmp9                                                          ; a20d: 85 8e       ..
    lda tmp4                                                          ; a20f: a5 89       ..
    sta tmp2                                                          ; a211: 85 87       ..
    lda tmp5                                                          ; a213: a5 8a       ..
    sta tmp3                                                          ; a215: 85 88       ..
    ldy #0                                                            ; a217: a0 00       ..
; &a219 referenced 2 times by &a22f, &a233
.ca219
    lda (tmp8),y                                                      ; a219: b1 8d       ..
    sta (tmp2),y                                                      ; a21b: 91 87       ..
    inc tmp2                                                          ; a21d: e6 87       ..
    bne ca223                                                         ; a21f: d0 02       ..
    inc tmp3                                                          ; a221: e6 88       ..
; &a223 referenced 1 time by &a21f
.ca223
    inc tmp8                                                          ; a223: e6 8d       ..
    bne ca229                                                         ; a225: d0 02       ..
    inc tmp9                                                          ; a227: e6 8e       ..
; &a229 referenced 1 time by &a225
.ca229
    lda tmp8                                                          ; a229: a5 8d       ..
    ldx tmp9                                                          ; a22b: a6 8e       ..
    cpx area_end_ptr+1                                                ; a22d: e4 62       .b
    bne ca219                                                         ; a22f: d0 e8       ..
    cmp area_end_ptr                                                  ; a231: c5 61       .a
    bne ca219                                                         ; a233: d0 e4       ..
    lda tmp6                                                          ; a235: a5 8b       ..
    pha                                                               ; a237: 48          H
    lda tmp7                                                          ; a238: a5 8c       ..
    pha                                                               ; a23a: 48          H
    lda tmp4                                                          ; a23b: a5 89       ..
    sta l0063                                                         ; a23d: 85 63       .c
    lda tmp5                                                          ; a23f: a5 8a       ..
    sta l0064                                                         ; a241: 85 64       .d
    lda tmp2                                                          ; a243: a5 87       ..
    ldy tmp3                                                          ; a245: a4 88       ..
    sec                                                               ; a247: 38          8
    sbc #1                                                            ; a248: e9 01       ..
    bcs ca24d                                                         ; a24a: b0 01       ..
    dey                                                               ; a24c: 88          .
; &a24d referenced 1 time by &a24a
.ca24d
    jsr cac78                                                         ; a24d: 20 78 ac     x.
    lda l0063                                                         ; a250: a5 63       .c
    ldy l0064                                                         ; a252: a4 64       .d
    jsr cac78                                                         ; a254: 20 78 ac     x.
    pla                                                               ; a257: 68          h
    sta tmp7                                                          ; a258: 85 8c       ..
    pla                                                               ; a25a: 68          h
    sta tmp6                                                          ; a25b: 85 8b       ..
    lda #1                                                            ; a25d: a9 01       ..
    sta l0073                                                         ; a25f: 85 73       .s
    sta l007d                                                         ; a261: 85 7d       .}
    clc                                                               ; a263: 18          .
    rts                                                               ; a264: 60          `

; &a265 referenced 1 time by &a205
.ca265
    jmp ca941                                                         ; a265: 4c 41 a9    LA.

; ***************************************************************************************
.cf1_next_match_key
    jsr ca93c                                                         ; a268: 20 3c a9     <.
    jsr c8b7b                                                         ; a26b: 20 7b 8b     {.
    bne ca273                                                         ; a26e: d0 03       ..
    jmp cabcb                                                         ; a270: 4c cb ab    L..

; &a273 referenced 1 time by &a26e
.ca273
    jmp esc_key                                                       ; a273: 4c 3b 82    L;.

; &a276 referenced 5 times by &8365, &9b93, &9eeb, &a0de, &a0f5
.sub_ca276
    jsr cursor_off                                                    ; a276: 20 86 a7     ..
    lda l0070                                                         ; a279: a5 70       .p
    sta l0034                                                         ; a27b: 85 34       .4
    lda l0076                                                         ; a27d: a5 76       .v
    sta input_buffer_ptr+1                                            ; a27f: 85 80       ..
    lda l006e                                                         ; a281: a5 6e       .n
    beq ca28e                                                         ; a283: f0 09       ..
    lda l0073                                                         ; a285: a5 73       .s
    ora l006f                                                         ; a287: 05 6f       .o
    bne ca28e                                                         ; a289: d0 03       ..
    jmp ca360                                                         ; a28b: 4c 60 a3    L`.

; &a28e referenced 2 times by &a283, &a289
.ca28e
    lda ptr4+1                                                        ; a28e: a5 09       ..
    cmp l0012                                                         ; a290: c5 12       ..
    bcc ca29c                                                         ; a292: 90 08       ..
    bne ca2dc                                                         ; a294: d0 46       .F
    lda ptr4                                                          ; a296: a5 08       ..
    cmp l0011                                                         ; a298: c5 11       ..
    bcs ca2dc                                                         ; a29a: b0 40       .@
; &a29c referenced 1 time by &a292
.ca29c
    lda l006f                                                         ; a29c: a5 6f       .o
    bne ca30d                                                         ; a29e: d0 6d       .m
    lda l0033                                                         ; a2a0: a5 33       .3
    sta l0070                                                         ; a2a2: 85 70       .p
    ldy l0012                                                         ; a2a4: a4 12       ..
    lda l0011                                                         ; a2a6: a5 11       ..
    cpy top+1                                                         ; a2a8: c4 0e       ..
    bcc ca2b2                                                         ; a2aa: 90 06       ..
    bne ca30d                                                         ; a2ac: d0 5f       ._
    cmp top                                                           ; a2ae: c5 0d       ..
    bcs ca30d                                                         ; a2b0: b0 5b       .[
; &a2b2 referenced 1 time by &a2aa
.ca2b2
    jsr sub_cab37                                                     ; a2b2: 20 37 ab     7.
    ldy tmp1                                                          ; a2b5: a4 86       ..
    cpy ptr4+1                                                        ; a2b7: c4 09       ..
    bne ca30d                                                         ; a2b9: d0 52       .R
    lda tmp0                                                          ; a2bb: a5 85       ..
    cmp ptr4                                                          ; a2bd: c5 08       ..
    bne ca30d                                                         ; a2bf: d0 4c       .L
    sty l0012                                                         ; a2c1: 84 12       ..
    sta l0011                                                         ; a2c3: 85 11       ..
    ldx screen_height                                                 ; a2c5: a6 35       .5
; &a2c7 referenced 1 time by &a2d0
.loop_ca2c7
    dex                                                               ; a2c7: ca          .
    lda line_lengths,x                                                ; a2c8: bd cc 07    ...
    inx                                                               ; a2cb: e8          .
    sta line_lengths,x                                                ; a2cc: 9d cc 07    ...
    dex                                                               ; a2cf: ca          .
    bne loop_ca2c7                                                    ; a2d0: d0 f5       ..
    jsr home_cursor                                                   ; a2d2: 20 7f a6     ..
    lda #&0b                                                          ; a2d5: a9 0b       ..
    ldy #1                                                            ; a2d7: a0 01       ..
    jmp ca351                                                         ; a2d9: 4c 51 a3    LQ.

; &a2dc referenced 2 times by &a294, &a29a
.ca2dc
    lda l0033                                                         ; a2dc: a5 33       .3
    sta l0070                                                         ; a2de: 85 70       .p
; &a2e0 referenced 1 time by &a310
.ca2e0
    ldx #0                                                            ; a2e0: a2 00       ..
    lda l0011                                                         ; a2e2: a5 11       ..
    ldy l0012                                                         ; a2e4: a4 12       ..
; &a2e6 referenced 2 times by &a309, &a30b
.ca2e6
    inx                                                               ; a2e6: e8          .
    cpy ptr6+1                                                        ; a2e7: c4 14       ..
    bne ca2f1                                                         ; a2e9: d0 06       ..
    cmp ptr6                                                          ; a2eb: c5 13       ..
    bne ca2f1                                                         ; a2ed: d0 02       ..
    stx l003d                                                         ; a2ef: 86 3d       .=
; &a2f1 referenced 2 times by &a2e9, &a2ed
.ca2f1
    cpy ptr4+1                                                        ; a2f1: c4 09       ..
    bne ca2f9                                                         ; a2f3: d0 04       ..
    cmp ptr4                                                          ; a2f5: c5 08       ..
    beq ca313                                                         ; a2f7: f0 1a       ..
; &a2f9 referenced 1 time by &a2f3
.ca2f9
    jsr sub_cab1a                                                     ; a2f9: 20 1a ab     ..
    beq ca313                                                         ; a2fc: f0 15       ..
    tya                                                               ; a2fe: 98          .
    ldy tmp1                                                          ; a2ff: a4 86       ..
    clc                                                               ; a301: 18          .
    adc tmp0                                                          ; a302: 65 85       e.
    bcc ca307                                                         ; a304: 90 01       ..
    iny                                                               ; a306: c8          .
; &a307 referenced 1 time by &a304
.ca307
    cpx screen_height                                                 ; a307: e4 35       .5
    beq ca2e6                                                         ; a309: f0 db       ..
    bcc ca2e6                                                         ; a30b: 90 d9       ..
; &a30d referenced 6 times by &a29e, &a2ac, &a2b0, &a2b9, &a2bf, &a31b
.ca30d
    jsr sub_ca44e                                                     ; a30d: 20 4e a4     N.
    jmp ca2e0                                                         ; a310: 4c e0 a2    L..

; &a313 referenced 2 times by &a2f7, &a2fc
.ca313
    cpx screen_height                                                 ; a313: e4 35       .5
    bcc ca35e                                                         ; a315: 90 47       .G
    beq ca35e                                                         ; a317: f0 45       .E
    lda l006f                                                         ; a319: a5 6f       .o
    bne ca30d                                                         ; a31b: d0 f0       ..
    ldx #0                                                            ; a31d: a2 00       ..
; &a31f referenced 1 time by &a328
.loop_ca31f
    lda line_lengths+1,x                                              ; a31f: bd cd 07    ...
    sta line_lengths,x                                                ; a322: 9d cc 07    ...
    inx                                                               ; a325: e8          .
    cpx screen_height                                                 ; a326: e4 35       .5
    bne loop_ca31f                                                    ; a328: d0 f5       ..
    dec l003d                                                         ; a32a: c6 3d       .=
    ldx #0                                                            ; a32c: a2 00       ..
    lda screen_width                                                  ; a32e: a5 36       .6
    sta line_lengths,x                                                ; a330: 9d cc 07    ...
    lda l0033                                                         ; a333: a5 33       .3
    sta l0070                                                         ; a335: 85 70       .p
    ldy l0012                                                         ; a337: a4 12       ..
    lda l0011                                                         ; a339: a5 11       ..
    jsr sub_cab1a                                                     ; a33b: 20 1a ab     ..
    tya                                                               ; a33e: 98          .
    clc                                                               ; a33f: 18          .
    adc l0011                                                         ; a340: 65 11       e.
    sta l0011                                                         ; a342: 85 11       ..
    bcc ca348                                                         ; a344: 90 02       ..
    inc l0012                                                         ; a346: e6 12       ..
; &a348 referenced 1 time by &a344
.ca348
    ldy screen_height                                                 ; a348: a4 35       .5
    lda #&0a                                                          ; a34a: a9 0a       ..
    ldx #0                                                            ; a34c: a2 00       ..
    jsr set_cursor_position                                           ; a34e: 20 ea a7     ..
; &a351 referenced 1 time by &a2d9
.ca351
    jsr oswrch                                                        ; a351: 20 ee ff     ..            ; Write character
    lda l0070                                                         ; a354: a5 70       .p
    sta l0033                                                         ; a356: 85 33       .3
    inc input_buffer_ptr+1                                            ; a358: e6 80       ..
    inc l0074                                                         ; a35a: e6 74       .t
    tya                                                               ; a35c: 98          .
    tax                                                               ; a35d: aa          .
; &a35e referenced 2 times by &a315, &a317
.ca35e
    stx ypos                                                          ; a35e: 86 77       .w
; &a360 referenced 1 time by &a28b
.ca360
    ldy l0034                                                         ; a360: a4 34       .4
    jsr cab91                                                         ; a362: 20 91 ab     ..
    jsr sub_caa8f                                                     ; a365: 20 8f aa     ..
    jsr sub_ca608                                                     ; a368: 20 08 a6     ..
    lda screen_width                                                  ; a36b: a5 36       .6
    lsr a                                                             ; a36d: 4a          J
    sta l0083                                                         ; a36e: 85 83       ..
    lda l0072                                                         ; a370: a5 72       .r
    cmp l0071                                                         ; a372: c5 71       .q
    bcc ca381                                                         ; a374: 90 0b       ..
    lda l0071                                                         ; a376: a5 71       .q
    clc                                                               ; a378: 18          .
    adc screen_width                                                  ; a379: 65 36       e6
    sbc #3                                                            ; a37b: e9 03       ..
    cmp l0072                                                         ; a37d: c5 72       .r
    bcs ca395                                                         ; a37f: b0 14       ..
; &a381 referenced 1 time by &a374
.ca381
    lda l0072                                                         ; a381: a5 72       .r
    sec                                                               ; a383: 38          8
    sbc l0083                                                         ; a384: e5 83       ..
    bcs ca38a                                                         ; a386: b0 02       ..
    lda #0                                                            ; a388: a9 00       ..
; &a38a referenced 1 time by &a386
.ca38a
    sta l0071                                                         ; a38a: 85 71       .q
    lda #1                                                            ; a38c: a9 01       ..
    sta l0073                                                         ; a38e: 85 73       .s
    sta input_buffer_ptr+1                                            ; a390: 85 80       ..
    jsr ca93c                                                         ; a392: 20 3c a9     <.
; &a395 referenced 1 time by &a37f
.ca395
    lda input_buffer_ptr+1                                            ; a395: a5 80       ..
    sta l0076                                                         ; a397: 85 76       .v
    lda l0073                                                         ; a399: a5 73       .s
    beq ca3e7                                                         ; a39b: f0 4a       .J
    bpl ca3b2                                                         ; a39d: 10 13       ..
    lda l003d                                                         ; a39f: a5 3d       .=
    bmi ca3b2                                                         ; a3a1: 30 0f       0.
    sta l0082                                                         ; a3a3: 85 82       ..
    lda screen_height                                                 ; a3a5: a5 35       .5
    sec                                                               ; a3a7: 38          8
    sbc l003d                                                         ; a3a8: e5 3d       .=
    tax                                                               ; a3aa: aa          .
    inx                                                               ; a3ab: e8          .
    lda ptr6                                                          ; a3ac: a5 13       ..
    ldy ptr6+1                                                        ; a3ae: a4 14       ..
    bne ca3c1                                                         ; a3b0: d0 0f       ..
; &a3b2 referenced 2 times by &a39d, &a3a1
.ca3b2
    ldy l0033                                                         ; a3b2: a4 33       .3
    jsr cab91                                                         ; a3b4: 20 91 ab     ..
    lda #1                                                            ; a3b7: a9 01       ..
    sta l0082                                                         ; a3b9: 85 82       ..
    lda l0011                                                         ; a3bb: a5 11       ..
    ldy l0012                                                         ; a3bd: a4 12       ..
    ldx screen_height                                                 ; a3bf: a6 35       .5
; &a3c1 referenced 1 time by &a3b0
.ca3c1
    stx l0081                                                         ; a3c1: 86 81       ..
; &a3c3 referenced 1 time by &a3dc
.loop_ca3c3
    jsr sub_ca486                                                     ; a3c3: 20 86 a4     ..
    lda tmp0                                                          ; a3c6: a5 85       ..
    ldy tmp1                                                          ; a3c8: a4 86       ..
    jsr sub_cab1a                                                     ; a3ca: 20 1a ab     ..
    beq ca422                                                         ; a3cd: f0 53       .S
    tya                                                               ; a3cf: 98          .
    ldy tmp1                                                          ; a3d0: a4 86       ..
    clc                                                               ; a3d2: 18          .
    adc tmp0                                                          ; a3d3: 65 85       e.
    bcc ca3d8                                                         ; a3d5: 90 01       ..
    iny                                                               ; a3d7: c8          .
; &a3d8 referenced 1 time by &a3d5
.ca3d8
    inc l0082                                                         ; a3d8: e6 82       ..
    dec l0081                                                         ; a3da: c6 81       ..
    bne loop_ca3c3                                                    ; a3dc: d0 e5       ..
; &a3de referenced 2 times by &a424, &a44c
.ca3de
    lda #0                                                            ; a3de: a9 00       ..
    sta l0074                                                         ; a3e0: 85 74       .t
    ldy l0034                                                         ; a3e2: a4 34       .4
    jsr cab91                                                         ; a3e4: 20 91 ab     ..
; &a3e7 referenced 1 time by &a39b
.ca3e7
    jsr sub_caa8f                                                     ; a3e7: 20 8f aa     ..
    jsr sub_caacb                                                     ; a3ea: 20 cb aa     ..
    jsr display_status_word                                           ; a3ed: 20 40 a6     @.
    lda l0074                                                         ; a3f0: a5 74       .t
    beq ca3ff                                                         ; a3f2: f0 0b       ..
    lda ypos                                                          ; a3f4: a5 77       .w
    sta l0082                                                         ; a3f6: 85 82       ..
    lda current_format_line_ptr                                       ; a3f8: a5 04       ..
    ldy current_format_line_ptr+1                                     ; a3fa: a4 05       ..
    jsr sub_ca486                                                     ; a3fc: 20 86 a4     ..
; &a3ff referenced 1 time by &a3f2
.ca3ff
    lda l0075                                                         ; a3ff: a5 75       .u
    beq ca406                                                         ; a401: f0 03       ..
    jsr sub_ca651                                                     ; a403: 20 51 a6     Q.
; &a406 referenced 1 time by &a401
.ca406
    lda l0072                                                         ; a406: a5 72       .r
    sec                                                               ; a408: 38          8
    sbc l0071                                                         ; a409: e5 71       .q
    clc                                                               ; a40b: 18          .
    adc #3                                                            ; a40c: 69 03       i.
    tax                                                               ; a40e: aa          .
    ldy #0                                                            ; a40f: a0 00       ..
    sty l0073                                                         ; a411: 84 73       .s
    sty l0074                                                         ; a413: 84 74       .t
    sty l006f                                                         ; a415: 84 6f       .o
    dey                                                               ; a417: 88          .              ; Y=&ff
    sty ptr6+1                                                        ; a418: 84 14       ..
    ldy ypos                                                          ; a41a: a4 77       .w
    jsr set_cursor_position                                           ; a41c: 20 ea a7     ..
    jmp cursor_on                                                     ; a41f: 4c cd a7    L..

; &a422 referenced 1 time by &a3cd
.ca422
    dec l0081                                                         ; a422: c6 81       ..
    beq ca3de                                                         ; a424: f0 b8       ..
    ldx l0082                                                         ; a426: a6 82       ..
    lda screen_width                                                  ; a428: a5 36       .6
    sta line_lengths+1,x                                              ; a42a: 9d cd 07    ...
    sta l0083                                                         ; a42d: 85 83       ..
    lda #&2a ; '*'                                                    ; a42f: a9 2a       .*
; &a431 referenced 1 time by &a44a
.loop_ca431
    inc l0082                                                         ; a431: e6 82       ..
    ldx #0                                                            ; a433: a2 00       ..
    ldy l0082                                                         ; a435: a4 82       ..
    jsr set_cursor_position                                           ; a437: 20 ea a7     ..
    jsr sub_ca597                                                     ; a43a: 20 97 a5     ..
    lda l0083                                                         ; a43d: a5 83       ..
    sta line_lengths,x                                                ; a43f: 9d cc 07    ...
    lda #0                                                            ; a442: a9 00       ..
    sta l0083                                                         ; a444: 85 83       ..
    lda #&20 ; ' '                                                    ; a446: a9 20       .
    dec l0081                                                         ; a448: c6 81       ..
    bne loop_ca431                                                    ; a44a: d0 e5       ..
    beq ca3de                                                         ; a44c: f0 90       ..             ; ALWAYS branch

; &a44e referenced 1 time by &a30d
.sub_ca44e
    lda l0034                                                         ; a44e: a5 34       .4
    sta l0070                                                         ; a450: 85 70       .p
    lda screen_height                                                 ; a452: a5 35       .5
    sta l0073                                                         ; a454: 85 73       .s
    lsr a                                                             ; a456: 4a          J
    tax                                                               ; a457: aa          .
    inx                                                               ; a458: e8          .
    lda l006f                                                         ; a459: a5 6f       .o
    bmi ca461                                                         ; a45b: 30 04       0.
    beq ca461                                                         ; a45d: f0 02       ..
    ldx ypos                                                          ; a45f: a6 77       .w
; &a461 referenced 2 times by &a45b, &a45d
.ca461
    lda ptr4                                                          ; a461: a5 08       ..
    ldy ptr4+1                                                        ; a463: a4 09       ..
; &a465 referenced 1 time by &a473
.loop_ca465
    dex                                                               ; a465: ca          .
    beq ca479                                                         ; a466: f0 11       ..
    sta tmp2                                                          ; a468: 85 87       ..
    sty tmp3                                                          ; a46a: 84 88       ..
    jsr sub_cab37                                                     ; a46c: 20 37 ab     7.
    lda tmp0                                                          ; a46f: a5 85       ..
    ldy tmp1                                                          ; a471: a4 86       ..
    bcs loop_ca465                                                    ; a473: b0 f0       ..
    lda tmp2                                                          ; a475: a5 87       ..
    ldy tmp3                                                          ; a477: a4 88       ..
; &a479 referenced 1 time by &a466
.ca479
    sta l0011                                                         ; a479: 85 11       ..
    sty l0012                                                         ; a47b: 84 12       ..
    lda l0070                                                         ; a47d: a5 70       .p
    sta l0033                                                         ; a47f: 85 33       .3
    lda l0034                                                         ; a481: a5 34       .4
    sta l0070                                                         ; a483: 85 70       .p
    rts                                                               ; a485: 60          `

; &a486 referenced 3 times by &a3c3, &a3fc, &a64e
.sub_ca486
    sta tmp0                                                          ; a486: 85 85       ..
    sta tmp6                                                          ; a488: 85 8b       ..
    sty tmp1                                                          ; a48a: 84 86       ..
    sty tmp7                                                          ; a48c: 84 8c       ..
    ldx #0                                                            ; a48e: a2 00       ..
    ldy l0082                                                         ; a490: a4 82       ..
    jsr set_cursor_position                                           ; a492: 20 ea a7     ..
    ldy #0                                                            ; a495: a0 00       ..
    sty l0083                                                         ; a497: 84 83       ..
    sty input_buffer_ptr+1                                            ; a499: 84 80       ..
    sty l0039                                                         ; a49b: 84 39       .9
    jsr sub_caf6f                                                     ; a49d: 20 6f af     o.
    bne ca4b4                                                         ; a4a0: d0 12       ..
    ldy #3                                                            ; a4a2: a0 03       ..
    lda l0071                                                         ; a4a4: a5 71       .q
    bne ca4b4                                                         ; a4a6: d0 0c       ..
    ldy #1                                                            ; a4a8: a0 01       ..
    jsr sub_ca4d7                                                     ; a4aa: 20 d7 a4     ..
    jsr sub_ca4d7                                                     ; a4ad: 20 d7 a4     ..
    lda #&20 ; ' '                                                    ; a4b0: a9 20       .
    bne ca4bc                                                         ; a4b2: d0 08       ..             ; ALWAYS branch

; &a4b4 referenced 2 times by &a4a0, &a4a6
.ca4b4
    lda #&20 ; ' '                                                    ; a4b4: a9 20       .
    jsr ca4e9                                                         ; a4b6: 20 e9 a4     ..
    jsr ca4e9                                                         ; a4b9: 20 e9 a4     ..
; &a4bc referenced 1 time by &a4b2
.ca4bc
    jsr ca4e9                                                         ; a4bc: 20 e9 a4     ..
; &a4bf referenced 1 time by &a4ca
.loop_ca4bf
    jsr sub_ca5ab                                                     ; a4bf: 20 ab a5     ..
; &a4c2 referenced 1 time by &a4c6
.loop_ca4c2
    jsr sub_ca4dd                                                     ; a4c2: 20 dd a4     ..
    dex                                                               ; a4c5: ca          .
    bne loop_ca4c2                                                    ; a4c6: d0 fa       ..
    cmp #&0d                                                          ; a4c8: c9 0d       ..
    bne loop_ca4bf                                                    ; a4ca: d0 f3       ..
    lda #&20 ; ' '                                                    ; a4cc: a9 20       .
    jsr sub_ca597                                                     ; a4ce: 20 97 a5     ..
    lda l0083                                                         ; a4d1: a5 83       ..
    sta line_lengths,x                                                ; a4d3: 9d cc 07    ...
    rts                                                               ; a4d6: 60          `

; &a4d7 referenced 2 times by &a4aa, &a4ad
.sub_ca4d7
    jsr sub_ca5ab                                                     ; a4d7: 20 ab a5     ..
    jmp ca4e9                                                         ; a4da: 4c e9 a4    L..

; &a4dd referenced 1 time by &a4c2
.sub_ca4dd
    inc l0039                                                         ; a4dd: e6 39       .9
    stx l0084                                                         ; a4df: 86 84       ..
    ldx input_buffer_ptr+1                                            ; a4e1: a6 80       ..
    inc input_buffer_ptr+1                                            ; a4e3: e6 80       ..
    cpx l0071                                                         ; a4e5: e4 71       .q
    bcc ca533                                                         ; a4e7: 90 4a       .J
; &a4e9 referenced 4 times by &a4b6, &a4b9, &a4bc, &a4da
.ca4e9
    pha                                                               ; a4e9: 48          H
    ldx l0082                                                         ; a4ea: a6 82       ..
    lda line_lengths,x                                                ; a4ec: bd cc 07    ...
    beq ca4f4                                                         ; a4ef: f0 03       ..
    dec line_lengths,x                                                ; a4f1: de cc 07    ...
; &a4f4 referenced 1 time by &a4ef
.ca4f4
    ldx l0083                                                         ; a4f4: a6 83       ..
    cpx screen_width                                                  ; a4f6: e4 36       .6
    bcs ca532                                                         ; a4f8: b0 38       .8
    inc l0083                                                         ; a4fa: e6 83       ..
    tya                                                               ; a4fc: 98          .
    beq ca514                                                         ; a4fd: f0 15       ..
    dey                                                               ; a4ff: 88          .
    jsr sub_ca536                                                     ; a500: 20 36 a5     6.
    iny                                                               ; a503: c8          .
    cpx #4                                                            ; a504: e0 04       ..
    bcs ca514                                                         ; a506: b0 0c       ..
    tax                                                               ; a508: aa          .
    bmi ca523                                                         ; a509: 30 18       0.
    bne ca514                                                         ; a50b: d0 07       ..
    pla                                                               ; a50d: 68          h
; &a50e referenced 2 times by &a51c, &a520
.ca50e
    pha                                                               ; a50e: 48          H
    jsr set_inverted_text_if_not_mode_7                               ; a50f: 20 77 a5     w.
    ldx #0                                                            ; a512: a2 00       ..
; &a514 referenced 3 times by &a4fd, &a506, &a50b
.ca514
    pla                                                               ; a514: 68          h
    jsr sub_ca600                                                     ; a515: 20 00 a6     ..
    bne ca522                                                         ; a518: d0 08       ..
    lda #&2d ; '-'                                                    ; a51a: a9 2d       .-
    bcs ca50e                                                         ; a51c: b0 f0       ..
    lda #&2a ; '*'                                                    ; a51e: a9 2a       .*
    bcc ca50e                                                         ; a520: 90 ec       ..             ; ALWAYS branch

; &a522 referenced 1 time by &a518
.ca522
    pha                                                               ; a522: 48          H
; &a523 referenced 1 time by &a509
.ca523
    cmp #&0d                                                          ; a523: c9 0d       ..
    bne ca529                                                         ; a525: d0 02       ..
    lda #&20 ; ' '                                                    ; a527: a9 20       .
; &a529 referenced 1 time by &a525
.ca529
    jsr oswrch                                                        ; a529: 20 ee ff     ..            ; Write character 32
    txa                                                               ; a52c: 8a          .
    bne ca532                                                         ; a52d: d0 03       ..
    jsr set_normal_text_if_not_mode_7                                 ; a52f: 20 65 a5     e.
; &a532 referenced 2 times by &a4f8, &a52d
.ca532
    pla                                                               ; a532: 68          h
; &a533 referenced 1 time by &a4e7
.ca533
    ldx l0084                                                         ; a533: a6 84       ..
    rts                                                               ; a535: 60          `

; &a536 referenced 8 times by &99ba, &9be7, &9cf9, &a500, &a91c, &aad5, &ae37, &ae78
.sub_ca536
    tya                                                               ; a536: 98          .
    clc                                                               ; a537: 18          .
    adc tmp6                                                          ; a538: 65 8b       e.
    sta tmp8                                                          ; a53a: 85 8d       ..
    lda tmp7                                                          ; a53c: a5 8c       ..
    adc #0                                                            ; a53e: 69 00       i.
    sta tmp9                                                          ; a540: 85 8e       ..
    ldx #0                                                            ; a542: a2 00       ..
; &a544 referenced 1 time by &a554
.loop_ca544
    lda tmp9                                                          ; a544: a5 8e       ..
    cmp markers_array+1,x                                             ; a546: d5 54       .T
    bne ca550                                                         ; a548: d0 06       ..
    lda tmp8                                                          ; a54a: a5 8d       ..
    cmp markers_array,x                                               ; a54c: d5 53       .S
    beq detect_mode_7                                                 ; a54e: f0 08       ..
; &a550 referenced 1 time by &a548
.ca550
    inx                                                               ; a550: e8          .
    inx                                                               ; a551: e8          .
    cpx #&0c                                                          ; a552: e0 0c       ..
    bne loop_ca544                                                    ; a554: d0 ee       ..
    txa                                                               ; a556: 8a          .
    rts                                                               ; a557: 60          `

; ***************************************************************************************
; &a558 referenced 1 time by &a54e
.detect_mode_7
    lda current_screen_mode                                           ; a558: a5 37       .7
    cmp #7                                                            ; a55a: c9 07       ..
    php                                                               ; a55c: 08          .
    lda #&ff                                                          ; a55d: a9 ff       ..
    plp                                                               ; a55f: 28          (
    beq return_61                                                     ; a560: f0 02       ..
    lda #0                                                            ; a562: a9 00       ..
; &a564 referenced 1 time by &a560
.return_61
    rts                                                               ; a564: 60          `

; ***************************************************************************************
; &a565 referenced 4 times by &9485, &a52f, &a7c2, &a96e
.set_normal_text_if_not_mode_7
    pha                                                               ; a565: 48          H
    lda current_screen_mode                                           ; a566: a5 37       .7
    cmp #7                                                            ; a568: c9 07       ..
    bcs ca58c                                                         ; a56a: b0 20       .
    txa                                                               ; a56c: 8a          .
    pha                                                               ; a56d: 48          H
    ldx #7                                                            ; a56e: a2 07       ..
    jsr set_text_colour                                               ; a570: 20 8e a5     ..
    ldx #&80                                                          ; a573: a2 80       ..
    bne ca587                                                         ; a575: d0 10       ..             ; ALWAYS branch

; ***************************************************************************************
; &a577 referenced 4 times by &947e, &a50f, &a7b5, &a954
.set_inverted_text_if_not_mode_7
    pha                                                               ; a577: 48          H
    lda current_screen_mode                                           ; a578: a5 37       .7
    cmp #7                                                            ; a57a: c9 07       ..
    bcs ca58c                                                         ; a57c: b0 0e       ..
    txa                                                               ; a57e: 8a          .
    pha                                                               ; a57f: 48          H
    ldx #0                                                            ; a580: a2 00       ..
    jsr set_text_colour                                               ; a582: 20 8e a5     ..
    ldx #&87                                                          ; a585: a2 87       ..
; &a587 referenced 1 time by &a575
.ca587
    jsr set_text_colour                                               ; a587: 20 8e a5     ..
    pla                                                               ; a58a: 68          h
    tax                                                               ; a58b: aa          .
; &a58c referenced 2 times by &a56a, &a57c
.ca58c
    pla                                                               ; a58c: 68          h
    rts                                                               ; a58d: 60          `

; ***************************************************************************************
; &a58e referenced 3 times by &a570, &a582, &a587
.set_text_colour
    lda #&11                                                          ; a58e: a9 11       ..
    jsr oswrch                                                        ; a590: 20 ee ff     ..            ; Write character 17
    txa                                                               ; a593: 8a          .
    jmp oswrch                                                        ; a594: 4c ee ff    L..            ; Write character

; &a597 referenced 2 times by &a43a, &a4ce
.sub_ca597
    ldx l0082                                                         ; a597: a6 82       ..
    sta l0084                                                         ; a599: 85 84       ..
    lda line_lengths,x                                                ; a59b: bd cc 07    ...
    beq return_62                                                     ; a59e: f0 0a       ..
    lda l0084                                                         ; a5a0: a5 84       ..
; &a5a2 referenced 1 time by &a5a8
.loop_ca5a2
    jsr oswrch                                                        ; a5a2: 20 ee ff     ..            ; Write character
    dec line_lengths,x                                                ; a5a5: de cc 07    ...
    bne loop_ca5a2                                                    ; a5a8: d0 f8       ..
; &a5aa referenced 1 time by &a59e
.return_62
    rts                                                               ; a5aa: 60          `

; &a5ab referenced 11 times by &86ec, &9fdc, &9ffc, &a007, &a4bf, &a4d7, &a61b, &a62b, &af42, &af4b, &af57
.sub_ca5ab
    lda (tmp0),y                                                      ; a5ab: b1 85       ..
    iny                                                               ; a5ad: c8          .
; &a5ae referenced 4 times by &9431, &9941, &99d2, &9c28
.sub_ca5ae
    cmp #9                                                            ; a5ae: c9 09       ..
    beq ca5e1                                                         ; a5b0: f0 2f       ./
    cmp #&10                                                          ; a5b2: c9 10       ..
    beq ca5d5                                                         ; a5b4: f0 1f       ..
    cmp #&0b                                                          ; a5b6: c9 0b       ..
    beq ca5d9                                                         ; a5b8: f0 1f       ..
    cmp #&1a                                                          ; a5ba: c9 1a       ..
    beq ca5d5                                                         ; a5bc: f0 17       ..
    bcc ca5d1                                                         ; a5be: 90 11       ..
    cmp #&20 ; ' '                                                    ; a5c0: c9 20       .
    bcs ca5d1                                                         ; a5c2: b0 0d       ..
    sty l0084                                                         ; a5c4: 84 84       ..
    ldy print_flags                                                   ; a5c6: a4 69       .i
    bpl ca5cf                                                         ; a5c8: 10 05       ..
    sbc #&1b                                                          ; a5ca: e9 1b       ..
    tax                                                               ; a5cc: aa          .
    lda highlight1_code,x                                             ; a5cd: b5 2a       .*
; &a5cf referenced 1 time by &a5c8
.ca5cf
    ldy l0084                                                         ; a5cf: a4 84       ..
; &a5d1 referenced 3 times by &a5be, &a5c2, &a5d7
.ca5d1
    ldx #1                                                            ; a5d1: a2 01       ..
    clc                                                               ; a5d3: 18          .
    rts                                                               ; a5d4: 60          `

; &a5d5 referenced 3 times by &a5b4, &a5bc, &a5db
.ca5d5
    lda #&20 ; ' '                                                    ; a5d5: a9 20       .
    bne ca5d1                                                         ; a5d7: d0 f8       ..             ; ALWAYS branch

; &a5d9 referenced 1 time by &a5b8
.ca5d9
    lda ruler_left_stop                                               ; a5d9: a5 3f       .?
    beq ca5d5                                                         ; a5db: f0 f8       ..
    sty l0084                                                         ; a5dd: 84 84       ..
    bne ca5f1                                                         ; a5df: d0 10       ..             ; ALWAYS branch

; &a5e1 referenced 1 time by &a5b0
.ca5e1
    sty l0084                                                         ; a5e1: 84 84       ..
    ldy l0039                                                         ; a5e3: a4 39       .9
; &a5e5 referenced 1 time by &a5ee
.loop_ca5e5
    iny                                                               ; a5e5: c8          .
    cpy l003a                                                         ; a5e6: c4 3a       .:
    bcs ca5f8                                                         ; a5e8: b0 0e       ..
    lda (current_ruler_ptr),y                                         ; a5ea: b1 06       ..
    cmp #&2a ; '*'                                                    ; a5ec: c9 2a       .*
    bne loop_ca5e5                                                    ; a5ee: d0 f5       ..
    tya                                                               ; a5f0: 98          .
; &a5f1 referenced 1 time by &a5df
.ca5f1
    sbc l0039                                                         ; a5f1: e5 39       .9
    tax                                                               ; a5f3: aa          .
    beq ca5f8                                                         ; a5f4: f0 02       ..
    bcs ca5fa                                                         ; a5f6: b0 02       ..
; &a5f8 referenced 2 times by &a5e8, &a5f4
.ca5f8
    ldx #1                                                            ; a5f8: a2 01       ..
; &a5fa referenced 1 time by &a5f6
.ca5fa
    lda #&20 ; ' '                                                    ; a5fa: a9 20       .
    ldy l0084                                                         ; a5fc: a4 84       ..
    sec                                                               ; a5fe: 38          8
    rts                                                               ; a5ff: 60          `

; &a600 referenced 6 times by &8cb3, &93d2, &9472, &9541, &9a39, &a515
.sub_ca600
    cmp #&1c                                                          ; a600: c9 1c       ..
    beq return_63                                                     ; a602: f0 03       ..
    cmp #&1d                                                          ; a604: c9 1d       ..
    clc                                                               ; a606: 18          .
; &a607 referenced 1 time by &a602
.return_63
    rts                                                               ; a607: 60          `

; &a608 referenced 5 times by &9b44, &9b70, &9c92, &a368, &af08
.sub_ca608
    lda current_edit_line_ptr                                         ; a608: a5 02       ..
    sta tmp0                                                          ; a60a: 85 85       ..
    lda current_edit_line_ptr+1                                       ; a60c: a5 03       ..
    sta tmp1                                                          ; a60e: 85 86       ..
    lda l0079                                                         ; a610: a5 79       .y
    bne ca624                                                         ; a612: d0 10       ..
    tay                                                               ; a614: a8          .
; &a615 referenced 1 time by &a622
.loop_ca615
    cpy xpos                                                          ; a615: c4 40       .@
    beq ca63d                                                         ; a617: f0 24       .$
    sta l0039                                                         ; a619: 85 39       .9
    jsr sub_ca5ab                                                     ; a61b: 20 ab a5     ..
    txa                                                               ; a61e: 8a          .
    clc                                                               ; a61f: 18          .
    adc l0039                                                         ; a620: 65 39       e9
    bcc loop_ca615                                                    ; a622: 90 f1       ..
; &a624 referenced 1 time by &a612
.ca624
    lda #0                                                            ; a624: a9 00       ..
    sta l0079                                                         ; a626: 85 79       .y
    tay                                                               ; a628: a8          .              ; Y=&00
; &a629 referenced 1 time by &a634
.loop_ca629
    sta l0039                                                         ; a629: 85 39       .9
    jsr sub_ca5ab                                                     ; a62b: 20 ab a5     ..
    txa                                                               ; a62e: 8a          .
    clc                                                               ; a62f: 18          .
    adc l0039                                                         ; a630: 65 39       e9
    cmp l0072                                                         ; a632: c5 72       .r
    bcc loop_ca629                                                    ; a634: 90 f3       ..
    beq ca63b                                                         ; a636: f0 03       ..
    lda l0039                                                         ; a638: a5 39       .9
    dey                                                               ; a63a: 88          .
; &a63b referenced 1 time by &a636
.ca63b
    sty xpos                                                          ; a63b: 84 40       .@
; &a63d referenced 1 time by &a617
.ca63d
    sta l0072                                                         ; a63d: 85 72       .r
; &a63f referenced 1 time by &a642
.return_64
    rts                                                               ; a63f: 60          `

; ***************************************************************************************
; &a640 referenced 1 time by &a3ed
.display_status_word
    lda l0076                                                         ; a640: a5 76       .v
    beq return_64                                                     ; a642: f0 fb       ..
    ldy #0                                                            ; a644: a0 00       ..
    sty l0076                                                         ; a646: 84 76       .v
    sty l0082                                                         ; a648: 84 82       ..
    lda current_ruler_ptr                                             ; a64a: a5 06       ..
    ldy current_ruler_ptr+1                                           ; a64c: a4 07       ..
    jsr sub_ca486                                                     ; a64e: 20 86 a4     ..
; &a651 referenced 1 time by &a403
.sub_ca651
    lda #0                                                            ; a651: a9 00       ..
    sta l0075                                                         ; a653: 85 75       .u
    jsr home_cursor                                                   ; a655: 20 7f a6     ..
    ldx #&46 ; 'F'                                                    ; a658: a2 46       .F
    lda format_mode_flag                                              ; a65a: a5 4f       .O
    beq ca666                                                         ; a65c: f0 08       ..
    ldx #&4d ; 'M'                                                    ; a65e: a2 4d       .M
    and #&c0                                                          ; a660: 29 c0       ).
    bne ca666                                                         ; a662: d0 02       ..
    ldx #&20 ; ' '                                                    ; a664: a2 20       .
; &a666 referenced 2 times by &a65c, &a662
.ca666
    txa                                                               ; a666: 8a          .
    jsr oswrch                                                        ; a667: 20 ee ff     ..            ; Write character
    lda #&4a ; 'J'                                                    ; a66a: a9 4a       .J
    ldx justifying_flag                                               ; a66c: a6 50       .P
    beq ca672                                                         ; a66e: f0 02       ..
    lda #&20 ; ' '                                                    ; a670: a9 20       .
; &a672 referenced 1 time by &a66e
.ca672
    jsr oswrch                                                        ; a672: 20 ee ff     ..            ; Write character 32
    lda #&49 ; 'I'                                                    ; a675: a9 49       .I
    ldx insert_mode_flag                                              ; a677: a6 51       .Q
    bne ca681                                                         ; a679: d0 06       ..
    lda #&20 ; ' '                                                    ; a67b: a9 20       .
    bne ca681                                                         ; a67d: d0 02       ..             ; ALWAYS branch

; ***************************************************************************************
; &a67f referenced 3 times by &a2d2, &a655, &a7b2
.home_cursor
    lda #&1e                                                          ; a67f: a9 1e       ..
; &a681 referenced 2 times by &a679, &a67d
.ca681
    jmp oswrch                                                        ; a681: 4c ee ff    L..            ; Write character 30

; &a684 referenced 2 times by &9c16, &a039
.ca684
    ldx ypos                                                          ; a684: a6 77       .w
    lda screen_width                                                  ; a686: a5 36       .6
    sta line_lengths,x                                                ; a688: 9d cc 07    ...
    rts                                                               ; a68b: 60          `

; ***************************************************************************************
; On Entry:
;     TMP9/TMP8: 16-bit number
; ***************************************************************************************
; &a68c referenced 1 time by &adea
.render_number_to_output_buffer
    stx l0082                                                         ; a68c: 86 82       ..
    lda la69a                                                         ; a68e: ad 9a a6    ...
    ldy la69b                                                         ; a691: ac 9b a6    ...
    jsr render_number_to_callback                                     ; a694: 20 ba a6     ..
    ldx l0082                                                         ; a697: a6 82       ..
    rts                                                               ; a699: 60          `

; &a69a referenced 1 time by &a68e
.la69a
la69b = la69a+1
    equw emit_to_output_buffer_callback                               ; a69a: 9c a6       ..
; &a69b referenced 1 time by &a691

.emit_to_output_buffer_callback
    pha                                                               ; a69c: 48          H
    txa                                                               ; a69d: 8a          .
    pha                                                               ; a69e: 48          H
    tsx                                                               ; a69f: ba          .
    lda l0102,x                                                       ; a6a0: bd 02 01    ...
    ldx l0082                                                         ; a6a3: a6 82       ..
    sta output_buffer,x                                               ; a6a5: 9d 54 06    .T.
    cpx #&82                                                          ; a6a8: e0 82       ..
    bcs ca6ae                                                         ; a6aa: b0 02       ..
    inc l0082                                                         ; a6ac: e6 82       ..
; &a6ae referenced 1 time by &a6aa
.ca6ae
    pla                                                               ; a6ae: 68          h
    tax                                                               ; a6af: aa          .
    pla                                                               ; a6b0: 68          h
    rts                                                               ; a6b1: 60          `

; ***************************************************************************************
; On Entry:
;     YX: 16-bit number
; ***************************************************************************************
; &a6b2 referenced 4 times by &8133, &82ce, &872f, &92b4
.render_number_to_screen
    stx tmp8                                                          ; a6b2: 86 8d       ..
    sty tmp9                                                          ; a6b4: 84 8e       ..
    lda #<(oswrch)                                                    ; a6b6: a9 ee       ..
    ldy #>(oswrch)                                                    ; a6b8: a0 ff       ..
; ***************************************************************************************
; On Entry:
;     TMP8/TMP9: 16-bit number
;     YA: callback
; ***************************************************************************************
; &a6ba referenced 1 time by &a694
.render_number_to_callback
    sta tmp6                                                          ; a6ba: 85 8b       ..
    sty tmp7                                                          ; a6bc: 84 8c       ..
    ldy #0                                                            ; a6be: a0 00       ..
    sty l0083                                                         ; a6c0: 84 83       ..
; &a6c2 referenced 1 time by &a6f5
.ca6c2
    ldx #0                                                            ; a6c2: a2 00       ..
; &a6c4 referenced 1 time by &a6e3
.loop_ca6c4
    lda tmp9                                                          ; a6c4: a5 8e       ..
    cmp lb145,y                                                       ; a6c6: d9 45 b1    .E.
    bcc ca6e5                                                         ; a6c9: 90 1a       ..
    bne ca6d4                                                         ; a6cb: d0 07       ..
    lda tmp8                                                          ; a6cd: a5 8d       ..
    cmp decimal_table,y                                               ; a6cf: d9 44 b1    .D.
    bcc ca6e5                                                         ; a6d2: 90 11       ..
; &a6d4 referenced 1 time by &a6cb
.ca6d4
    lda tmp8                                                          ; a6d4: a5 8d       ..
    sbc decimal_table,y                                               ; a6d6: f9 44 b1    .D.
    sta tmp8                                                          ; a6d9: 85 8d       ..
    lda tmp9                                                          ; a6db: a5 8e       ..
    sbc lb145,y                                                       ; a6dd: f9 45 b1    .E.
    sta tmp9                                                          ; a6e0: 85 8e       ..
    inx                                                               ; a6e2: e8          .
    bne loop_ca6c4                                                    ; a6e3: d0 df       ..
; &a6e5 referenced 2 times by &a6c9, &a6d2
.ca6e5
    txa                                                               ; a6e5: 8a          .
    bne ca6ec                                                         ; a6e6: d0 04       ..
    ldx l0083                                                         ; a6e8: a6 83       ..
    beq ca6f1                                                         ; a6ea: f0 05       ..
; &a6ec referenced 1 time by &a6e6
.ca6ec
    jsr sub_ca6f9                                                     ; a6ec: 20 f9 a6     ..
    inc l0083                                                         ; a6ef: e6 83       ..
; &a6f1 referenced 1 time by &a6ea
.ca6f1
    iny                                                               ; a6f1: c8          .
    iny                                                               ; a6f2: c8          .
    cpy #8                                                            ; a6f3: c0 08       ..
    bcc ca6c2                                                         ; a6f5: 90 cb       ..
    lda tmp8                                                          ; a6f7: a5 8d       ..
; &a6f9 referenced 1 time by &a6ec
.sub_ca6f9
    ora #&30 ; '0'                                                    ; a6f9: 09 30       .0
    jmp (tmp6)                                                        ; a6fb: 6c 8b 00    l..

; &a6fe referenced 2 times by &8846, &97d5
.ca6fe
    lda #0                                                            ; a6fe: a9 00       ..
    tax                                                               ; a700: aa          .              ; X=&00
    sta tmp8                                                          ; a701: 85 8d       ..
    sta tmp9                                                          ; a703: 85 8e       ..
; &a705 referenced 1 time by &a737
.ca705
    lda (current_format_line_ptr),y                                   ; a705: b1 04       ..
    sec                                                               ; a707: 38          8
    sbc #&30 ; '0'                                                    ; a708: e9 30       .0
    bcc ca739                                                         ; a70a: 90 2d       .-
    cmp #&0a                                                          ; a70c: c9 0a       ..
    bcs ca739                                                         ; a70e: b0 29       .)
    iny                                                               ; a710: c8          .
    sta l0084                                                         ; a711: 85 84       ..
    asl tmp8                                                          ; a713: 06 8d       ..
    rol tmp9                                                          ; a715: 26 8e       &.
    ldx tmp9                                                          ; a717: a6 8e       ..
    lda tmp8                                                          ; a719: a5 8d       ..
    pha                                                               ; a71b: 48          H
    asl tmp8                                                          ; a71c: 06 8d       ..
    rol tmp9                                                          ; a71e: 26 8e       &.
    asl tmp8                                                          ; a720: 06 8d       ..
    rol tmp9                                                          ; a722: 26 8e       &.
    pla                                                               ; a724: 68          h
    clc                                                               ; a725: 18          .
    adc tmp8                                                          ; a726: 65 8d       e.
    bcc ca72b                                                         ; a728: 90 01       ..
    inx                                                               ; a72a: e8          .
; &a72b referenced 1 time by &a728
.ca72b
    clc                                                               ; a72b: 18          .
    adc l0084                                                         ; a72c: 65 84       e.
    sta tmp8                                                          ; a72e: 85 8d       ..
    txa                                                               ; a730: 8a          .
    adc tmp9                                                          ; a731: 65 8e       e.
    sta tmp9                                                          ; a733: 85 8e       ..
    ldx #&ff                                                          ; a735: a2 ff       ..
    bne ca705                                                         ; a737: d0 cc       ..             ; ALWAYS branch

; &a739 referenced 2 times by &a70a, &a70e
.ca739
    txa                                                               ; a739: 8a          .
    php                                                               ; a73a: 08          .
    lda tmp8                                                          ; a73b: a5 8d       ..
    ldx tmp9                                                          ; a73d: a6 8e       ..
    plp                                                               ; a73f: 28          (
    rts                                                               ; a740: 60          `

; &a741 referenced 8 times by &839d, &9d1b, &9dfa, &9e83, &9ed7, &a045, &a188, &a8f5
.ca741
    ldx ptr4                                                          ; a741: a6 08       ..
    ldy ptr4+1                                                        ; a743: a4 09       ..
    cpy ptr6+1                                                        ; a745: c4 14       ..
    bcc ca74f                                                         ; a747: 90 06       ..
    bne ca753                                                         ; a749: d0 08       ..
    cpx ptr6                                                          ; a74b: e4 13       ..
    bcs ca753                                                         ; a74d: b0 04       ..
; &a74f referenced 1 time by &a747
.ca74f
    stx ptr6                                                          ; a74f: 86 13       ..
    sty ptr6+1                                                        ; a751: 84 14       ..
; &a753 referenced 2 times by &a749, &a74d
.ca753
    ldx #&ff                                                          ; a753: a2 ff       ..
    stx l0073                                                         ; a755: 86 73       .s
    stx l003d                                                         ; a757: 86 3d       .=
    rts                                                               ; a759: 60          `

; ***************************************************************************************
; &a75a referenced 3 times by &8337, &92bd, &a986
.flush_and_read_char
    lda #osbyte_flush_buffer                                          ; a75a: a9 15       ..
    ldx #buffer_keyboard                                              ; a75c: a2 00       ..
    jsr osbyte                                                        ; a75e: 20 f4 ff     ..            ; Flush the keyboard buffer (X=0)
; ***************************************************************************************
; &a761 referenced 4 times by &9b96, &9efd, &a10b, &ace0
.read_char
    jsr osrdch                                                        ; a761: 20 e0 ff     ..            ; Read a character from the current input stream
    cmp #&1b                                                          ; a764: c9 1b       ..             ; A=character read
    clc                                                               ; a766: 18          .
    bne return_65                                                     ; a767: d0 04       ..
    jsr acknowledge_escape                                            ; a769: 20 6e a7     n.
    sec                                                               ; a76c: 38          8
; &a76d referenced 1 time by &a767
.return_65
    rts                                                               ; a76d: 60          `

; ***************************************************************************************
; &a76e referenced 4 times by &81f6, &822a, &8f1d, &a769
.acknowledge_escape
    pha                                                               ; a76e: 48          H
    txa                                                               ; a76f: 8a          .
    pha                                                               ; a770: 48          H
    tya                                                               ; a771: 98          .
    pha                                                               ; a772: 48          H
    ldx #0                                                            ; a773: a2 00       ..
    ldy #0                                                            ; a775: a0 00       ..
    lda #osbyte_acknowledge_escape                                    ; a777: a9 7e       .~
    jsr osbyte                                                        ; a779: 20 f4 ff     ..            ; Clear escape condition and perform escape effects
    pla                                                               ; a77c: 68          h
    tay                                                               ; a77d: a8          .
    pla                                                               ; a77e: 68          h
    tax                                                               ; a77f: aa          .
    pla                                                               ; a780: 68          h
    rts                                                               ; a781: 60          `

; ***************************************************************************************
; &a782 referenced 2 times by &80f6, &b09b
.clear_screen
    lda #&0c                                                          ; a782: a9 0c       ..
    bne ca7a5                                                         ; a784: d0 1f       ..             ; ALWAYS branch

; ***************************************************************************************
; &a786 referenced 3 times by &a276, &a7af, &a94a
.cursor_off
    jsr setup_CRTC_10_write                                           ; a786: 20 99 a7     ..
    lda #&20 ; ' '                                                    ; a789: a9 20       .
; ***************************************************************************************
; &a78b referenced 2 times by &a7d6, &a7da
.complete_CRTC_10_write
    jsr oswrch                                                        ; a78b: 20 ee ff     ..            ; Write character 32
    lda #0                                                            ; a78e: a9 00       ..
    ldx #6                                                            ; a790: a2 06       ..
; &a792 referenced 1 time by &a796
.loop_ca792
    jsr oswrch                                                        ; a792: 20 ee ff     ..            ; Write character 0
    dex                                                               ; a795: ca          .
    bne loop_ca792                                                    ; a796: d0 fa       ..
    rts                                                               ; a798: 60          `

; ***************************************************************************************
; &a799 referenced 2 times by &a786, &a7cd
.setup_CRTC_10_write
    lda #&17                                                          ; a799: a9 17       ..
    jsr oswrch                                                        ; a79b: 20 ee ff     ..            ; Write character 23
    lda #0                                                            ; a79e: a9 00       ..
    jsr oswrch                                                        ; a7a0: 20 ee ff     ..            ; Write character 0
    lda #&0a                                                          ; a7a3: a9 0a       ..
; &a7a5 referenced 1 time by &a784
.ca7a5
    jmp oswrch                                                        ; a7a5: 4c ee ff    L..            ; Write character 10

; ***************************************************************************************
; &a7a8 referenced 3 times by &8334, &a106, &acdb
.draw_prompt_characters
    stx tmp2                                                          ; a7a8: 86 87       ..
    sty tmp3                                                          ; a7aa: 84 88       ..
    jsr save_cursor_position                                          ; a7ac: 20 dc a7     ..
    jsr cursor_off                                                    ; a7af: 20 86 a7     ..
    jsr home_cursor                                                   ; a7b2: 20 7f a6     ..
    jsr set_inverted_text_if_not_mode_7                               ; a7b5: 20 77 a5     w.
    lda tmp2                                                          ; a7b8: a5 87       ..
    jsr oswrch                                                        ; a7ba: 20 ee ff     ..            ; Write character
    lda tmp3                                                          ; a7bd: a5 88       ..
    jsr oswrch                                                        ; a7bf: 20 ee ff     ..            ; Write character
    jsr set_normal_text_if_not_mode_7                                 ; a7c2: 20 65 a5     e.
    lda #&20 ; ' '                                                    ; a7c5: a9 20       .
    jsr oswrch                                                        ; a7c7: 20 ee ff     ..            ; Write character 32
    jsr restore_cursor_position                                       ; a7ca: 20 e6 a7     ..
; ***************************************************************************************
; &a7cd referenced 3 times by &8109, &a41f, &a98b
.cursor_on
    jsr setup_CRTC_10_write                                           ; a7cd: 20 99 a7     ..
    lda #&72 ; 'r'                                                    ; a7d0: a9 72       .r
    ldx current_screen_mode                                           ; a7d2: a6 37       .7
    cpx #7                                                            ; a7d4: e0 07       ..
    beq complete_CRTC_10_write                                        ; a7d6: f0 b3       ..
    lda #&67 ; 'g'                                                    ; a7d8: a9 67       .g
    bne complete_CRTC_10_write                                        ; a7da: d0 af       ..             ; ALWAYS branch

; ***************************************************************************************
; &a7dc referenced 1 time by &a7ac
.save_cursor_position
    lda #osbyte_read_text_cursor_pos                                  ; a7dc: a9 86       ..
    jsr osbyte                                                        ; a7de: 20 f4 ff     ..            ; Read input cursor position (Sets X=POS and Y=VPOS)
    stx tmp4                                                          ; a7e1: 86 89       ..             ; X is the horizontal text position ('POS')
    sty tmp5                                                          ; a7e3: 84 8a       ..             ; Y is the vertical text position ('VPOS')
    rts                                                               ; a7e5: 60          `

; ***************************************************************************************
; &a7e6 referenced 1 time by &a7ca
.restore_cursor_position
    ldx tmp4                                                          ; a7e6: a6 89       ..
    ldy tmp5                                                          ; a7e8: a4 8a       ..
; ***************************************************************************************
; &a7ea referenced 6 times by &9efa, &a34e, &a41c, &a437, &a492, &a951
.set_cursor_position
    pha                                                               ; a7ea: 48          H
    lda #&1f                                                          ; a7eb: a9 1f       ..
    jsr oswrch                                                        ; a7ed: 20 ee ff     ..            ; Write character 31
    txa                                                               ; a7f0: 8a          .
    jsr oswrch                                                        ; a7f1: 20 ee ff     ..            ; Write character
    tya                                                               ; a7f4: 98          .
    jsr oswrch                                                        ; a7f5: 20 ee ff     ..            ; Write character
    pla                                                               ; a7f8: 68          h
; &a7f9 referenced 1 time by &94bc
.return_34
    rts                                                               ; a7f9: 60          `

; ***************************************************************************************
; &a7fa referenced 36 times by &811f, &8145, &815b, &8163, &816d, &818c, &81ab, &81c3, &8204, &8263, &82d1, &82e7, &82fa, &8542, &8584, &85ec, &8617, &865d, &868f, &8732, &87b4, &87c4, &87cb, &87d1, &8865, &89b3, &89c1, &89e8, &8a0b, &8a21, &8e0f, &8e68, &8eb1, &9014, &92a4, &92b7
.print_inline_string
    sty l0084                                                         ; a7fa: 84 84       ..
    pla                                                               ; a7fc: 68          h
    clc                                                               ; a7fd: 18          .
    adc #1                                                            ; a7fe: 69 01       i.
    sta tmp2                                                          ; a800: 85 87       ..
    pla                                                               ; a802: 68          h
    adc #0                                                            ; a803: 69 00       i.
    sta tmp3                                                          ; a805: 85 88       ..
    ldy #0                                                            ; a807: a0 00       ..
    beq ca80f                                                         ; a809: f0 04       ..             ; ALWAYS branch

; &a80b referenced 1 time by &a813
.loop_ca80b
    jsr osasci                                                        ; a80b: 20 e3 ff     ..            ; Write character
    iny                                                               ; a80e: c8          .
; &a80f referenced 1 time by &a809
.ca80f
    lda (tmp2),y                                                      ; a80f: b1 87       ..
    bmi ca824                                                         ; a811: 30 11       0.
    bne loop_ca80b                                                    ; a813: d0 f6       ..
    tya                                                               ; a815: 98          .
    sec                                                               ; a816: 38          8
    adc tmp2                                                          ; a817: 65 87       e.
    sta tmp2                                                          ; a819: 85 87       ..
    bcc ca81f                                                         ; a81b: 90 02       ..
    inc tmp3                                                          ; a81d: e6 88       ..
; &a81f referenced 1 time by &a81b
.ca81f
    ldy l0084                                                         ; a81f: a4 84       ..
    jmp (tmp2)                                                        ; a821: 6c 87 00    l..

; &a824 referenced 1 time by &a811
.ca824
    jsr osnewl                                                        ; a824: 20 e7 ff     ..            ; Write newline (characters 10 and 13)
    jmp cli_loop                                                      ; a827: 4c f6 81    L..

; ***************************************************************************************
; &a82a referenced 2 times by &806d, &810e
.print_x_words_of_help
    ldy #0                                                            ; a82a: a0 00       ..
    beq ca832                                                         ; a82c: f0 04       ..             ; ALWAYS branch

; &a82e referenced 2 times by &a835, &a83a
.ca82e
    jsr oswrch                                                        ; a82e: 20 ee ff     ..            ; Write character
    iny                                                               ; a831: c8          .
; &a832 referenced 1 time by &a82c
.ca832
    lda la83d,y                                                       ; a832: b9 3d a8    .=.
    bne ca82e                                                         ; a835: d0 f7       ..
    lda #&20 ; ' '                                                    ; a837: a9 20       .
    dex                                                               ; a839: ca          .
    bpl ca82e                                                         ; a83a: 10 f2       ..
    rts                                                               ; a83c: 60          `

; &a83d referenced 1 time by &a832
.la83d
    equs "VIEW"                                                       ; a83d: 56 49 45... VIE
    equb 0                                                            ; a841: 00          .
    equs "B3.0"                                                       ; a842: 42 33 2e... B3.
    equb 0                                                            ; a846: 00          .

; ***************************************************************************************
; &a847 referenced 1 time by &8258
.parse_command
    lda #&ff                                                          ; a847: a9 ff       ..
    sta l0082                                                         ; a849: 85 82       ..
    tax                                                               ; a84b: aa          .              ; X=&ff
; &a84c referenced 2 times by &a876, &a87c
.ca84c
    ldy input_buffer_ptr                                              ; a84c: a4 7f       ..
    dey                                                               ; a84e: 88          .
    inc l0082                                                         ; a84f: e6 82       ..
; &a851 referenced 1 time by &a868
.loop_ca851
    inx                                                               ; a851: e8          .
    iny                                                               ; a852: c8          .
    lda (tmp0),y                                                      ; a853: b1 85       ..
    and #&df                                                          ; a855: 29 df       ).
    sta l0084                                                         ; a857: 85 84       ..
    lda parser_table,x                                                ; a859: bd 05 b2    ...
    beq ca890                                                         ; a85c: f0 32       .2
    bmi ca87e                                                         ; a85e: 30 1e       0.
    eor #&5b ; '['                                                    ; a860: 49 5b       I[
    sta l0083                                                         ; a862: 85 83       ..
    and #&df                                                          ; a864: 29 df       ).
    cmp l0084                                                         ; a866: c5 84       ..
    beq loop_ca851                                                    ; a868: f0 e7       ..
; &a86a referenced 1 time by &a870
.loop_ca86a
    inx                                                               ; a86a: e8          .
    lda parser_table,x                                                ; a86b: bd 05 b2    ...
    beq ca890                                                         ; a86e: f0 20       .
    bpl loop_ca86a                                                    ; a870: 10 f8       ..
    lda l0083                                                         ; a872: a5 83       ..
    and #&20 ; ' '                                                    ; a874: 29 20       )
    beq ca84c                                                         ; a876: f0 d4       ..
    lda (tmp0),y                                                      ; a878: b1 85       ..
    cmp #&30 ; '0'                                                    ; a87a: c9 30       .0
    bcs ca84c                                                         ; a87c: b0 ce       ..
; &a87e referenced 1 time by &a85e
.ca87e
    lda (tmp0),y                                                      ; a87e: b1 85       ..
    cmp #&30 ; '0'                                                    ; a880: c9 30       .0
    bcs ca887                                                         ; a882: b0 03       ..
    sta l007e                                                         ; a884: 85 7e       .~
    iny                                                               ; a886: c8          .
; &a887 referenced 1 time by &a882
.ca887
    sty input_buffer_ptr                                              ; a887: 84 7f       ..
    ldy l0082                                                         ; a889: a4 82       ..
    lda parser_table,x                                                ; a88b: bd 05 b2    ...
    clc                                                               ; a88e: 18          .
    rts                                                               ; a88f: 60          `

; &a890 referenced 2 times by &a85c, &a86e
.ca890
    sec                                                               ; a890: 38          8
    rts                                                               ; a891: 60          `

; ***************************************************************************************
; &a892 referenced 4 times by &8280, &8a49, &975c, &9bc4
.call_through_jumptable
    asl a                                                             ; a892: 0a          .
    clc                                                               ; a893: 18          .
    adc jumptable_ptrs,y                                              ; a894: 79 51 b1    yQ.
    sta tmp8                                                          ; a897: 85 8d       ..
    lda is_tube_flag                                                  ; a899: a5 52       .R
    bpl ca8a1                                                         ; a89b: 10 04       ..
    and #1                                                            ; a89d: 29 01       ).
    bne ca8a4                                                         ; a89f: d0 03       ..
; &a8a1 referenced 1 time by &a89b
.ca8a1
    inc la8a5                                                         ; a8a1: ee a5 a8    ...
; &a8a4 referenced 1 time by &a89f
.ca8a4
la8a5 = ca8a4+1
    lda #0                                                            ; a8a4: a9 00       ..
; &a8a5 referenced 1 time by &a8a1
    adc lb152,y                                                       ; a8a6: 79 52 b1    yR.
    sta tmp9                                                          ; a8a9: 85 8e       ..
    ldy #0                                                            ; a8ab: a0 00       ..
    lda (tmp8),y                                                      ; a8ad: b1 8d       ..
    sta tmp6                                                          ; a8af: 85 8b       ..
    iny                                                               ; a8b1: c8          .              ; Y=&01
    lda (tmp8),y                                                      ; a8b2: b1 8d       ..
    sta tmp7                                                          ; a8b4: 85 8c       ..
    jmp (tmp6)                                                        ; a8b6: 6c 8b 00    l..

; &a8b9 referenced 2 times by &9ab3, &a93c
.sub_ca8b9
    lda l006e                                                         ; a8b9: a5 6e       .n
    beq ca93a                                                         ; a8bb: f0 7d       .}
    lda ptr4                                                          ; a8bd: a5 08       ..
    sta tmp4                                                          ; a8bf: 85 89       ..
    lda ptr4+1                                                        ; a8c1: a5 09       ..
    sta tmp5                                                          ; a8c3: 85 8a       ..
    ldy #0                                                            ; a8c5: a0 00       ..
    sty tmp7                                                          ; a8c7: 84 8c       ..
    jsr get_line_length                                               ; a8c9: 20 f1 aa     ..
    sta l0083                                                         ; a8cc: 85 83       ..
    lda l003b                                                         ; a8ce: a5 3b       .;
    sec                                                               ; a8d0: 38          8
    sbc l0083                                                         ; a8d1: e5 83       ..
    bcc ca8df                                                         ; a8d3: 90 0a       ..
    beq ca8ed                                                         ; a8d5: f0 16       ..
    sta tmp6                                                          ; a8d7: 85 8b       ..
    jsr sub_ca9b0                                                     ; a8d9: 20 b0 a9     ..
    jmp ca8ed                                                         ; a8dc: 4c ed a8    L..

; &a8df referenced 1 time by &a8d3
.ca8df
    sta l0084                                                         ; a8df: 85 84       ..
    lda #0                                                            ; a8e1: a9 00       ..
    sec                                                               ; a8e3: 38          8
    sbc l0084                                                         ; a8e4: e5 84       ..
    sta tmp6                                                          ; a8e6: 85 8b       ..
    jsr sub_caa15                                                     ; a8e8: 20 15 aa     ..
    bcs return_66                                                     ; a8eb: b0 4e       .N
; &a8ed referenced 2 times by &a8d5, &a8dc
.ca8ed
    lda l006e                                                         ; a8ed: a5 6e       .n
    bpl ca8f8                                                         ; a8ef: 10 07       ..
    lda l006d                                                         ; a8f1: a5 6d       .m
    beq ca8f8                                                         ; a8f3: f0 03       ..
    jsr ca741                                                         ; a8f5: 20 41 a7     A.
; &a8f8 referenced 2 times by &a8ef, &a8f3
.ca8f8
    ldy #0                                                            ; a8f8: a0 00       ..
    sty l006d                                                         ; a8fa: 84 6d       .m
    sty l006e                                                         ; a8fc: 84 6e       .n
    lda current_format_line_ptr                                       ; a8fe: a5 04       ..
    sta tmp6                                                          ; a900: 85 8b       ..
    lda current_format_line_ptr+1                                     ; a902: a5 05       ..
    sta tmp7                                                          ; a904: 85 8c       ..
    ldx l0083                                                         ; a906: a6 83       ..
    stx l003b                                                         ; a908: 86 3b       .;
; &a90a referenced 1 time by &a938
.ca90a
    txa                                                               ; a90a: 8a          .
    bne ca911                                                         ; a90b: d0 04       ..
    lda #&0d                                                          ; a90d: a9 0d       ..
    bne ca919                                                         ; a90f: d0 08       ..             ; ALWAYS branch

; &a911 referenced 1 time by &a90b
.ca911
    lda (current_format_line_ptr),y                                   ; a911: b1 04       ..
    cmp #&10                                                          ; a913: c9 10       ..
    bne ca919                                                         ; a915: d0 02       ..
    lda #&20 ; ' '                                                    ; a917: a9 20       .
; &a919 referenced 2 times by &a90f, &a915
.ca919
    pha                                                               ; a919: 48          H
    txa                                                               ; a91a: 8a          .
    pha                                                               ; a91b: 48          H
; &a91c referenced 1 time by &a92d
.loop_ca91c
    jsr sub_ca536                                                     ; a91c: 20 36 a5     6.
    bne ca92f                                                         ; a91f: d0 0e       ..
    tya                                                               ; a921: 98          .
    clc                                                               ; a922: 18          .
    adc ptr4                                                          ; a923: 65 08       e.
    sta markers_array,x                                               ; a925: 95 53       .S
    lda ptr4+1                                                        ; a927: a5 09       ..
    adc #0                                                            ; a929: 69 00       i.
    sta markers_array+1,x                                             ; a92b: 95 54       .T
    bne loop_ca91c                                                    ; a92d: d0 ed       ..
; &a92f referenced 1 time by &a91f
.ca92f
    pla                                                               ; a92f: 68          h
    tax                                                               ; a930: aa          .
    pla                                                               ; a931: 68          h
    sta (ptr4),y                                                      ; a932: 91 08       ..
    iny                                                               ; a934: c8          .
    dex                                                               ; a935: ca          .
    cmp #&0d                                                          ; a936: c9 0d       ..
    bne ca90a                                                         ; a938: d0 d0       ..
; &a93a referenced 1 time by &a8bb
.ca93a
    clc                                                               ; a93a: 18          .
; &a93b referenced 2 times by &a8eb, &a93f
.return_66
    rts                                                               ; a93b: 60          `

; &a93c referenced 23 times by &80eb, &823b, &8368, &9d18, &9d5b, &9d74, &9d7b, &9db1, &9dd8, &9e50, &9ea1, &9f80, &9fad, &a012, &a024, &a03c, &a075, &a0b3, &a177, &a194, &a1b7, &a268, &a392
.ca93c
    jsr sub_ca8b9                                                     ; a93c: 20 b9 a8     ..
    bcc return_66                                                     ; a93f: 90 fa       ..
; &a941 referenced 4 times by &9ccd, &9dfd, &a05b, &a265
.ca941
    ldx #&ff                                                          ; a941: a2 ff       ..
    txs                                                               ; a943: 9a          .
    jsr sub_ca94a                                                     ; a944: 20 4a a9     J.
    jmp editor_loop                                                   ; a947: 4c 36 9b    L6.

; &a94a referenced 2 times by &836b, &a944
.sub_ca94a
    jsr cursor_off                                                    ; a94a: 20 86 a7     ..
    ldx #3                                                            ; a94d: a2 03       ..
    ldy #0                                                            ; a94f: a0 00       ..
    jsr set_cursor_position                                           ; a951: 20 ea a7     ..
    jsr set_inverted_text_if_not_mode_7                               ; a954: 20 77 a5     w.
    ldy screen_width                                                  ; a957: a4 36       .6
    sty line_lengths                                                  ; a959: 8c cc 07    ...
    dey                                                               ; a95c: 88          .
    dey                                                               ; a95d: 88          .
    ldx #0                                                            ; a95e: a2 00       ..
    beq ca965                                                         ; a960: f0 03       ..             ; ALWAYS branch

; &a962 referenced 1 time by &a96c
.loop_ca962
    jsr oswrch                                                        ; a962: 20 ee ff     ..            ; Write character
; &a965 referenced 1 time by &a960
.ca965
    lda la995,x                                                       ; a965: bd 95 a9    ...
    beq ca96e                                                         ; a968: f0 04       ..
    inx                                                               ; a96a: e8          .
    dey                                                               ; a96b: 88          .
    bne loop_ca962                                                    ; a96c: d0 f4       ..
; &a96e referenced 1 time by &a968
.ca96e
    jsr set_normal_text_if_not_mode_7                                 ; a96e: 20 65 a5     e.
    tya                                                               ; a971: 98          .
    beq ca97c                                                         ; a972: f0 08       ..
    lda #&20 ; ' '                                                    ; a974: a9 20       .
; &a976 referenced 1 time by &a97a
.loop_ca976
    jsr oswrch                                                        ; a976: 20 ee ff     ..            ; Write character 32
    dey                                                               ; a979: 88          .
    bne loop_ca976                                                    ; a97a: d0 fa       ..
; &a97c referenced 1 time by &a972
.ca97c
    lda #0                                                            ; a97c: a9 00       ..
    sta l006e                                                         ; a97e: 85 6e       .n
    jsr clear_cmd                                                     ; a980: 20 91 b0     ..
; &a983 referenced 1 time by &a989
.loop_ca983
    jsr beep                                                          ; a983: 20 e8 ac     ..
    jsr flush_and_read_char                                           ; a986: 20 5a a7     Z.
    bcc loop_ca983                                                    ; a989: 90 f8       ..
    jsr cursor_on                                                     ; a98b: 20 cd a7     ..
    lda #1                                                            ; a98e: a9 01       ..
    sta l0076                                                         ; a990: 85 76       .v
    sta l0073                                                         ; a992: 85 73       .s
    rts                                                               ; a994: 60          `

; &a995 referenced 1 time by &a965
.la995
    equs "Memory full - Press ESCAPE"                                 ; a995: 4d 65 6d... Mem
    equb 0                                                            ; a9af: 00          .

; &a9b0 referenced 6 times by &85ad, &89db, &8ad7, &9e66, &9ecb, &a8d9
.sub_ca9b0
    lda tmp4                                                          ; a9b0: a5 89       ..
    sta tmp2                                                          ; a9b2: 85 87       ..
    clc                                                               ; a9b4: 18          .
    adc tmp6                                                          ; a9b5: 65 8b       e.
    sta tmp8                                                          ; a9b7: 85 8d       ..
    lda tmp5                                                          ; a9b9: a5 8a       ..
    sta tmp3                                                          ; a9bb: 85 88       ..
    adc tmp7                                                          ; a9bd: 65 8c       e.
    sta tmp9                                                          ; a9bf: 85 8e       ..
    ldx #&53 ; 'S'                                                    ; a9c1: a2 53       .S
; &a9c3 referenced 1 time by &a9f5
.ca9c3
    ldy ptr1+1,x                                                      ; a9c3: b4 01       ..
    lda ptr1,x                                                        ; a9c5: b5 00       ..
    cpy tmp5                                                          ; a9c7: c4 8a       ..
    bcc ca9f1                                                         ; a9c9: 90 26       .&
    bne ca9d1                                                         ; a9cb: d0 04       ..
    cmp tmp4                                                          ; a9cd: c5 89       ..
    bcc ca9f1                                                         ; a9cf: 90 20       .
; &a9d1 referenced 1 time by &a9cb
.ca9d1
    cpy tmp9                                                          ; a9d1: c4 8e       ..
    bcc ca9db                                                         ; a9d3: 90 06       ..
    bne ca9e7                                                         ; a9d5: d0 10       ..
    cmp tmp8                                                          ; a9d7: c5 8d       ..
    bcs ca9e7                                                         ; a9d9: b0 0c       ..
; &a9db referenced 1 time by &a9d3
.ca9db
    cpx #&5f ; '_'                                                    ; a9db: e0 5f       ._
    bcs ca9e7                                                         ; a9dd: b0 08       ..
    lda #0                                                            ; a9df: a9 00       ..
    sta ptr1,x                                                        ; a9e1: 95 00       ..
    sta ptr1+1,x                                                      ; a9e3: 95 01       ..
    beq ca9f1                                                         ; a9e5: f0 0a       ..             ; ALWAYS branch

; &a9e7 referenced 3 times by &a9d5, &a9d9, &a9dd
.ca9e7
    sbc tmp6                                                          ; a9e7: e5 8b       ..
    sta ptr1,x                                                        ; a9e9: 95 00       ..
    lda ptr1+1,x                                                      ; a9eb: b5 01       ..
    sbc tmp7                                                          ; a9ed: e5 8c       ..
    sta ptr1+1,x                                                      ; a9ef: 95 01       ..
; &a9f1 referenced 3 times by &a9c9, &a9cf, &a9e5
.ca9f1
    inx                                                               ; a9f1: e8          .
    inx                                                               ; a9f2: e8          .
    cpx #&69 ; 'i'                                                    ; a9f3: e0 69       .i
    bne ca9c3                                                         ; a9f5: d0 cc       ..
; &a9f7 referenced 1 time by &aa06
.loop_ca9f7
    ldy #0                                                            ; a9f7: a0 00       ..
; &a9f9 referenced 1 time by &aa00
.loop_ca9f9
    lda (tmp8),y                                                      ; a9f9: b1 8d       ..
    sta (tmp2),y                                                      ; a9fb: 91 87       ..
    beq caa08                                                         ; a9fd: f0 09       ..
    iny                                                               ; a9ff: c8          .
    bne loop_ca9f9                                                    ; aa00: d0 f7       ..
    inc tmp3                                                          ; aa02: e6 88       ..
    inc tmp9                                                          ; aa04: e6 8e       ..
    bne loop_ca9f7                                                    ; aa06: d0 ef       ..
; &aa08 referenced 1 time by &a9fd
.caa08
    tya                                                               ; aa08: 98          .
    clc                                                               ; aa09: 18          .
    adc tmp2                                                          ; aa0a: 65 87       e.
    sta top                                                           ; aa0c: 85 0d       ..
    lda tmp3                                                          ; aa0e: a5 88       ..
    adc #0                                                            ; aa10: 69 00       i.
    sta top+1                                                         ; aa12: 85 0e       ..
    rts                                                               ; aa14: 60          `

; &aa15 referenced 7 times by &857a, &8ac4, &9cc8, &9def, &a202, &a8e8, &acc4
.sub_caa15
    lda top                                                           ; aa15: a5 0d       ..
    sta tmp2                                                          ; aa17: 85 87       ..
    clc                                                               ; aa19: 18          .
    adc tmp6                                                          ; aa1a: 65 8b       e.
    sta tmp8                                                          ; aa1c: 85 8d       ..
    tax                                                               ; aa1e: aa          .
    lda top+1                                                         ; aa1f: a5 0e       ..
    sta tmp3                                                          ; aa21: 85 88       ..
    adc tmp7                                                          ; aa23: 65 8c       e.
    sta tmp9                                                          ; aa25: 85 8e       ..
    tay                                                               ; aa27: a8          .
    cpy himem+1                                                       ; aa28: c4 10       ..
    bcc caa32                                                         ; aa2a: 90 06       ..
    bne return_67                                                     ; aa2c: d0 60       .`
    cpx himem                                                         ; aa2e: e4 0f       ..
    bcs return_67                                                     ; aa30: b0 5c       .\
; &aa32 referenced 1 time by &aa2a
.caa32
    stx top                                                           ; aa32: 86 0d       ..
    sty top+1                                                         ; aa34: 84 0e       ..
    ldx #&53 ; 'S'                                                    ; aa36: a2 53       .S
; &aa38 referenced 1 time by &aa55
.loop_caa38
    ldy ptr1+1,x                                                      ; aa38: b4 01       ..
    lda ptr1,x                                                        ; aa3a: b5 00       ..
    cpy tmp5                                                          ; aa3c: c4 8a       ..
    bcc caa51                                                         ; aa3e: 90 11       ..
    bne caa46                                                         ; aa40: d0 04       ..
    cmp tmp4                                                          ; aa42: c5 89       ..
    bcc caa51                                                         ; aa44: 90 0b       ..
; &aa46 referenced 1 time by &aa40
.caa46
    clc                                                               ; aa46: 18          .
    adc tmp6                                                          ; aa47: 65 8b       e.
    sta ptr1,x                                                        ; aa49: 95 00       ..
    lda ptr1+1,x                                                      ; aa4b: b5 01       ..
    adc tmp7                                                          ; aa4d: 65 8c       e.
    sta ptr1+1,x                                                      ; aa4f: 95 01       ..
; &aa51 referenced 2 times by &aa3e, &aa44
.caa51
    inx                                                               ; aa51: e8          .
    inx                                                               ; aa52: e8          .
    cpx #&69 ; 'i'                                                    ; aa53: e0 69       .i
    bne loop_caa38                                                    ; aa55: d0 e1       ..
; &aa57 referenced 1 time by &aa8b
.caa57
    lda tmp2                                                          ; aa57: a5 87       ..
    sec                                                               ; aa59: 38          8
    sbc tmp4                                                          ; aa5a: e5 89       ..
    tax                                                               ; aa5c: aa          .
    lda tmp3                                                          ; aa5d: a5 88       ..
    sbc tmp5                                                          ; aa5f: e5 8a       ..
    beq caa65                                                         ; aa61: f0 02       ..
    ldx #&ff                                                          ; aa63: a2 ff       ..
; &aa65 referenced 1 time by &aa61
.caa65
    txa                                                               ; aa65: 8a          .
    tay                                                               ; aa66: a8          .
    iny                                                               ; aa67: c8          .
    lda tmp2                                                          ; aa68: a5 87       ..
    stx tmp2                                                          ; aa6a: 86 87       ..
    sec                                                               ; aa6c: 38          8
    sbc tmp2                                                          ; aa6d: e5 87       ..
    sta tmp2                                                          ; aa6f: 85 87       ..
    bcs caa75                                                         ; aa71: b0 02       ..
    dec tmp3                                                          ; aa73: c6 88       ..
; &aa75 referenced 1 time by &aa71
.caa75
    lda tmp8                                                          ; aa75: a5 8d       ..
    stx tmp8                                                          ; aa77: 86 8d       ..
    sec                                                               ; aa79: 38          8
    sbc tmp8                                                          ; aa7a: e5 8d       ..
    sta tmp8                                                          ; aa7c: 85 8d       ..
    bcs caa82                                                         ; aa7e: b0 02       ..
    dec tmp9                                                          ; aa80: c6 8e       ..
; &aa82 referenced 2 times by &aa7e, &aa88
.caa82
    dey                                                               ; aa82: 88          .
    lda (tmp2),y                                                      ; aa83: b1 87       ..
    sta (tmp8),y                                                      ; aa85: 91 8d       ..
    tya                                                               ; aa87: 98          .
    bne caa82                                                         ; aa88: d0 f8       ..
    inx                                                               ; aa8a: e8          .
    beq caa57                                                         ; aa8b: f0 ca       ..
    clc                                                               ; aa8d: 18          .
; &aa8e referenced 2 times by &aa2c, &aa30
.return_67
    rts                                                               ; aa8e: 60          `

; &aa8f referenced 2 times by &a365, &a3e7
.sub_caa8f
    lda l006e                                                         ; aa8f: a5 6e       .n
    bne return_68                                                     ; aa91: d0 37       .7
    lda #1                                                            ; aa93: a9 01       ..
    sta l006e                                                         ; aa95: 85 6e       .n
; &aa97 referenced 5 times by &9b3e, &9f94, &9fc3, &a065, &a0a3
.sub_caa97
    lda #&10                                                          ; aa97: a9 10       ..
    jsr sub_cab0f                                                     ; aa99: 20 0f ab     ..
    jsr sub_caf5f                                                     ; aa9c: 20 5f af     _.
    ldy #0                                                            ; aa9f: a0 00       ..
    lda (ptr4),y                                                      ; aaa1: b1 08       ..
    ldx current_edit_line_ptr                                         ; aaa3: a6 02       ..
    ldy current_edit_line_ptr+1                                       ; aaa5: a4 03       ..
    jsr sub_caf71                                                     ; aaa7: 20 71 af     q.
    bne caab7                                                         ; aaaa: d0 0b       ..
    bcs caab0                                                         ; aaac: b0 02       ..
    sta l006e                                                         ; aaae: 85 6e       .n
; &aab0 referenced 1 time by &aaac
.caab0
    jsr caf5c                                                         ; aab0: 20 5c af     \.
    ldx ptr1                                                          ; aab3: a6 00       ..
    ldy ptr1+1                                                        ; aab5: a4 01       ..
; &aab7 referenced 1 time by &aaaa
.caab7
    stx current_format_line_ptr                                       ; aab7: 86 04       ..
    sty current_format_line_ptr+1                                     ; aab9: 84 05       ..
    ldy #0                                                            ; aabb: a0 00       ..
; &aabd referenced 1 time by &aac6
.loop_caabd
    lda (ptr4),y                                                      ; aabd: b1 08       ..
    cmp #&0d                                                          ; aabf: c9 0d       ..
    beq caac8                                                         ; aac1: f0 05       ..
    sta (current_format_line_ptr),y                                   ; aac3: 91 04       ..
    iny                                                               ; aac5: c8          .
    bne loop_caabd                                                    ; aac6: d0 f5       ..
; &aac8 referenced 1 time by &aac1
.caac8
    sty l003b                                                         ; aac8: 84 3b       .;
; &aaca referenced 2 times by &aa91, &aaec
.return_68
    rts                                                               ; aaca: 60          `

; &aacb referenced 1 time by &a3ea
.sub_caacb
    lda ptr4                                                          ; aacb: a5 08       ..
    sta tmp6                                                          ; aacd: 85 8b       ..
    lda ptr4+1                                                        ; aacf: a5 09       ..
    sta tmp7                                                          ; aad1: 85 8c       ..
    ldy #0                                                            ; aad3: a0 00       ..
; &aad5 referenced 2 times by &aae6, &aaef
.caad5
    jsr sub_ca536                                                     ; aad5: 20 36 a5     6.
    bne caae8                                                         ; aad8: d0 0e       ..
    tya                                                               ; aada: 98          .
    clc                                                               ; aadb: 18          .
    adc current_format_line_ptr                                       ; aadc: 65 04       e.
    sta markers_array,x                                               ; aade: 95 53       .S
    lda current_format_line_ptr+1                                     ; aae0: a5 05       ..
    adc #0                                                            ; aae2: 69 00       i.
    sta markers_array+1,x                                             ; aae4: 95 54       .T
    bne caad5                                                         ; aae6: d0 ed       ..
; &aae8 referenced 1 time by &aad8
.caae8
    lda (ptr4),y                                                      ; aae8: b1 08       ..
    cmp #&0d                                                          ; aaea: c9 0d       ..
    beq return_68                                                     ; aaec: f0 dc       ..
    iny                                                               ; aaee: c8          .
    bne caad5                                                         ; aaef: d0 e4       ..
; ***************************************************************************************
; &aaf1 referenced 12 times by &983e, &9b59, &9c82, &9db4, &9e17, &9e9b, &9fcb, &9feb, &a8c9, &ad47, &ae0e, &aeff
.get_line_length
    ldy #0                                                            ; aaf1: a0 00       ..
    lda (current_format_line_ptr),y                                   ; aaf3: b1 04       ..
    jsr sub_caf71                                                     ; aaf5: 20 71 af     q.
    php                                                               ; aaf8: 08          .
    ldy #&84                                                          ; aaf9: a0 84       ..
; &aafb referenced 1 time by &ab03
.loop_caafb
    dey                                                               ; aafb: 88          .
    lda (current_edit_line_ptr),y                                     ; aafc: b1 02       ..
    cmp #&10                                                          ; aafe: c9 10       ..
    bne cab06                                                         ; ab00: d0 04       ..
    tya                                                               ; ab02: 98          .
    bne loop_caafb                                                    ; ab03: d0 f6       ..
    dey                                                               ; ab05: 88          .
; &ab06 referenced 1 time by &ab00
.cab06
    iny                                                               ; ab06: c8          .
    tya                                                               ; ab07: 98          .
    plp                                                               ; ab08: 28          (
    bne return_69                                                     ; ab09: d0 03       ..
    clc                                                               ; ab0b: 18          .
    adc #3                                                            ; ab0c: 69 03       i.
; &ab0e referenced 1 time by &ab09
.return_69
    rts                                                               ; ab0e: 60          `

; &ab0f referenced 4 times by &8762, &98e3, &999f, &aa99
.sub_cab0f
    ldy #0                                                            ; ab0f: a0 00       ..
    ldx #&89                                                          ; ab11: a2 89       ..
; &ab13 referenced 1 time by &ab17
.loop_cab13
    sta (ptr1),y                                                      ; ab13: 91 00       ..
    iny                                                               ; ab15: c8          .
    dex                                                               ; ab16: ca          .
    bne loop_cab13                                                    ; ab17: d0 fa       ..
    rts                                                               ; ab19: 60          `

; &ab1a referenced 7 times by &9a94, &9da1, &9fb4, &a0ba, &a2f9, &a33b, &a3ca
.sub_cab1a
    sta tmp0                                                          ; ab1a: 85 85       ..
    sty tmp1                                                          ; ab1c: 84 86       ..
    jsr sub_cab6e                                                     ; ab1e: 20 6e ab     n.
    bne cab29                                                         ; ab21: d0 06       ..
    jsr cab29                                                         ; ab23: 20 29 ab     ).
    bne cab75                                                         ; ab26: d0 4d       .M
    rts                                                               ; ab28: 60          `

; &ab29 referenced 5 times by &9d8a, &9eac, &ab21, &ab23, &abfd
.cab29
    ldy #0                                                            ; ab29: a0 00       ..
; &ab2b referenced 1 time by &ab32
.loop_cab2b
    lda (tmp0),y                                                      ; ab2b: b1 85       ..
    beq return_70                                                     ; ab2d: f0 07       ..
    iny                                                               ; ab2f: c8          .
    cmp #&0d                                                          ; ab30: c9 0d       ..
    bne loop_cab2b                                                    ; ab32: d0 f7       ..
    lda (tmp0),y                                                      ; ab34: b1 85       ..
; &ab36 referenced 1 time by &ab2d
.return_70
    rts                                                               ; ab36: 60          `

; &ab37 referenced 7 times by &9d62, &9e76, &9f87, &a080, &a2b2, &a46c, &abdf
.sub_cab37
    sec                                                               ; ab37: 38          8
    sbc #1                                                            ; ab38: e9 01       ..
    sta tmp0                                                          ; ab3a: 85 85       ..
    bcs cab3f                                                         ; ab3c: b0 01       ..
    dey                                                               ; ab3e: 88          .
; &ab3f referenced 1 time by &ab3c
.cab3f
    sty tmp1                                                          ; ab3f: 84 86       ..
    cpy page+1                                                        ; ab41: c4 0c       ..
    bcc return_71                                                     ; ab43: 90 28       .(
    bne cab4b                                                         ; ab45: d0 04       ..
    cmp page                                                          ; ab47: c5 0b       ..
    bcc return_71                                                     ; ab49: 90 22       ."
; &ab4b referenced 1 time by &ab45
.cab4b
    ldy #0                                                            ; ab4b: a0 00       ..
; &ab4d referenced 1 time by &ab5c
.loop_cab4d
    lda tmp0                                                          ; ab4d: a5 85       ..
    sec                                                               ; ab4f: 38          8
    sbc #1                                                            ; ab50: e9 01       ..
    sta tmp0                                                          ; ab52: 85 85       ..
    bcs cab58                                                         ; ab54: b0 02       ..
    dec tmp1                                                          ; ab56: c6 86       ..
; &ab58 referenced 1 time by &ab54
.cab58
    lda (tmp0),y                                                      ; ab58: b1 85       ..
    cmp #&0d                                                          ; ab5a: c9 0d       ..
    bne loop_cab4d                                                    ; ab5c: d0 ef       ..
    inc tmp0                                                          ; ab5e: e6 85       ..
    bne cab64                                                         ; ab60: d0 02       ..
    inc tmp1                                                          ; ab62: e6 86       ..
; &ab64 referenced 1 time by &ab60
.cab64
    jsr sub_cab6e                                                     ; ab64: 20 6e ab     n.
    bne cab6c                                                         ; ab67: d0 03       ..
    jsr sub_cab8b                                                     ; ab69: 20 8b ab     ..
; &ab6c referenced 1 time by &ab67
.cab6c
    sec                                                               ; ab6c: 38          8
; &ab6d referenced 2 times by &ab43, &ab49
.return_71
    rts                                                               ; ab6d: 60          `

; &ab6e referenced 4 times by &8f54, &ab1e, &ab64, &ac44
.sub_cab6e
    ldy #0                                                            ; ab6e: a0 00       ..
    lda (tmp0),y                                                      ; ab70: b1 85       ..
    cmp #&81                                                          ; ab72: c9 81       ..
    rts                                                               ; ab74: 60          `

; &ab75 referenced 2 times by &ab26, &ac49
.cab75
    tya                                                               ; ab75: 98          .
    pha                                                               ; ab76: 48          H
    inc l0076                                                         ; ab77: e6 76       .v
    ldy l0070                                                         ; ab79: a4 70       .p
    dey                                                               ; ab7b: 88          .
    lda tmp0                                                          ; ab7c: a5 85       ..
    sta (oshwm),y                                                     ; ab7e: 91 1f       ..
    dey                                                               ; ab80: 88          .
    lda tmp1                                                          ; ab81: a5 86       ..
    sta (oshwm),y                                                     ; ab83: 91 1f       ..
    jsr cab91                                                         ; ab85: 20 91 ab     ..
    pla                                                               ; ab88: 68          h
    tay                                                               ; ab89: a8          .
    rts                                                               ; ab8a: 60          `

; &ab8b referenced 1 time by &ab69
.sub_cab8b
    inc l0076                                                         ; ab8b: e6 76       .v
    ldy l0070                                                         ; ab8d: a4 70       .p
    iny                                                               ; ab8f: c8          .
    iny                                                               ; ab90: c8          .
; &ab91 referenced 5 times by &a362, &a3b4, &a3e4, &ab85, &b08e
.cab91
    sty l0070                                                         ; ab91: 84 70       .p
    iny                                                               ; ab93: c8          .
    lda (oshwm),y                                                     ; ab94: b1 1f       ..
    clc                                                               ; ab96: 18          .
    adc #3                                                            ; ab97: 69 03       i.
    sta current_ruler_ptr                                             ; ab99: 85 06       ..
    dey                                                               ; ab9b: 88          .
    lda (oshwm),y                                                     ; ab9c: b1 1f       ..
    adc #0                                                            ; ab9e: 69 00       i.
    sta current_ruler_ptr+1                                           ; aba0: 85 07       ..
; ***************************************************************************************
; &aba2 referenced 2 times by &8ef6, &8f68
.find_margins_of_current_ruler
    jsr sub_cabc4                                                     ; aba2: 20 c4 ab     ..
; &aba5 referenced 1 time by &abba
.loop_caba5
    lda (current_ruler_ptr),y                                         ; aba5: b1 06       ..
    cmp #&3e ; '>'                                                    ; aba7: c9 3e       .>
    bne cabad                                                         ; aba9: d0 02       ..
    sty ruler_left_stop                                               ; abab: 84 3f       .?
; &abad referenced 1 time by &aba9
.cabad
    cmp #&3c ; '<'                                                    ; abad: c9 3c       .<
    bne cabb3                                                         ; abaf: d0 02       ..
    sty ruler_right_stop                                              ; abb1: 84 3e       .>
; &abb3 referenced 1 time by &abaf
.cabb3
    cmp #&0d                                                          ; abb3: c9 0d       ..
    beq cabbc                                                         ; abb5: f0 05       ..
    iny                                                               ; abb7: c8          .
    cpy #&84                                                          ; abb8: c0 84       ..
    bne loop_caba5                                                    ; abba: d0 e9       ..
; &abbc referenced 1 time by &abb5
.cabbc
    sty l003a                                                         ; abbc: 84 3a       .:
    lda ruler_left_stop                                               ; abbe: a5 3f       .?
    cmp ruler_right_stop                                              ; abc0: c5 3e       .>
    bcc return_72                                                     ; abc2: 90 06       ..
; &abc4 referenced 1 time by &aba2
.sub_cabc4
    ldy #0                                                            ; abc4: a0 00       ..
    sty ruler_right_stop                                              ; abc6: 84 3e       .>
    sty ruler_left_stop                                               ; abc8: 84 3f       .?
; &abca referenced 1 time by &abc2
.return_72
    rts                                                               ; abca: 60          `

; &abcb referenced 13 times by &829b, &82b9, &8327, &835b, &84b8, &8570, &875a, &a032, &a185, &a1b1, &a1c6, &a1ea, &a270
.cabcb
    sta tmp8                                                          ; abcb: 85 8d       ..
    sty tmp9                                                          ; abcd: 84 8e       ..
    lda ptr4                                                          ; abcf: a5 08       ..
    ldy ptr4+1                                                        ; abd1: a4 09       ..
    cpy tmp9                                                          ; abd3: c4 8e       ..
    bcc cabf9                                                         ; abd5: 90 22       ."
    bne cabdf                                                         ; abd7: d0 06       ..
    cmp tmp8                                                          ; abd9: c5 8d       ..
    bcc cabf9                                                         ; abdb: 90 1c       ..
    beq cac20                                                         ; abdd: f0 41       .A
; &abdf referenced 3 times by &abd7, &abec, &abf2
.cabdf
    jsr sub_cab37                                                     ; abdf: 20 37 ab     7.
    lda tmp0                                                          ; abe2: a5 85       ..
    ldy tmp1                                                          ; abe4: a4 86       ..
    bcc cac20                                                         ; abe6: 90 38       .8
    cpy tmp9                                                          ; abe8: c4 8e       ..
    bcc cac20                                                         ; abea: 90 34       .4
    bne cabdf                                                         ; abec: d0 f1       ..
    cmp tmp8                                                          ; abee: c5 8d       ..
    bcc cac20                                                         ; abf0: 90 2e       ..
    bne cabdf                                                         ; abf2: d0 eb       ..
    beq cac20                                                         ; abf4: f0 2a       .*             ; ALWAYS branch

; &abf6 referenced 2 times by &ac0d, &ac13
.cabf6
    jsr sub_cac41                                                     ; abf6: 20 41 ac     A.
; &abf9 referenced 2 times by &abd5, &abdb
.cabf9
    sta tmp0                                                          ; abf9: 85 85       ..
    sty tmp1                                                          ; abfb: 84 86       ..
    jsr cab29                                                         ; abfd: 20 29 ab     ).
    beq cac17                                                         ; ac00: f0 15       ..
    tya                                                               ; ac02: 98          .
    ldy tmp1                                                          ; ac03: a4 86       ..
    clc                                                               ; ac05: 18          .
    adc tmp0                                                          ; ac06: 65 85       e.
    bcc cac0b                                                         ; ac08: 90 01       ..
    iny                                                               ; ac0a: c8          .
; &ac0b referenced 1 time by &ac08
.cac0b
    cpy tmp9                                                          ; ac0b: c4 8e       ..
    bcc cabf6                                                         ; ac0d: 90 e7       ..
    bne cac17                                                         ; ac0f: d0 06       ..
    cmp tmp8                                                          ; ac11: c5 8d       ..
    bcc cabf6                                                         ; ac13: 90 e1       ..
    beq cac1d                                                         ; ac15: f0 06       ..
; &ac17 referenced 2 times by &ac00, &ac0f
.cac17
    lda tmp0                                                          ; ac17: a5 85       ..
    ldy tmp1                                                          ; ac19: a4 86       ..
    bne cac20                                                         ; ac1b: d0 03       ..
; &ac1d referenced 1 time by &ac15
.cac1d
    jsr sub_cac41                                                     ; ac1d: 20 41 ac     A.
; &ac20 referenced 6 times by &abdd, &abe6, &abea, &abf0, &abf4, &ac1b
.cac20
    sta ptr4                                                          ; ac20: 85 08       ..
    sty ptr4+1                                                        ; ac22: 84 09       ..
    lda tmp8                                                          ; ac24: a5 8d       ..
    sec                                                               ; ac26: 38          8
    sbc ptr4                                                          ; ac27: e5 08       ..
    tax                                                               ; ac29: aa          .
    ldy #0                                                            ; ac2a: a0 00       ..
    lda (ptr4),y                                                      ; ac2c: b1 08       ..
    jsr sub_caf71                                                     ; ac2e: 20 71 af     q.
    bne cac3e                                                         ; ac31: d0 0b       ..
    txa                                                               ; ac33: 8a          .
    cpx #3                                                            ; ac34: e0 03       ..
    ldx #0                                                            ; ac36: a2 00       ..
    bcc cac3e                                                         ; ac38: 90 04       ..
    sec                                                               ; ac3a: 38          8
    sbc #3                                                            ; ac3b: e9 03       ..
    tax                                                               ; ac3d: aa          .
; &ac3e referenced 2 times by &ac31, &ac38
.cac3e
    stx xpos                                                          ; ac3e: 86 40       .@
    rts                                                               ; ac40: 60          `

; &ac41 referenced 2 times by &abf6, &ac1d
.sub_cac41
    pha                                                               ; ac41: 48          H
    tya                                                               ; ac42: 98          .
    pha                                                               ; ac43: 48          H
    jsr sub_cab6e                                                     ; ac44: 20 6e ab     n.
    bne cac4c                                                         ; ac47: d0 03       ..
    jsr cab75                                                         ; ac49: 20 75 ab     u.
; &ac4c referenced 1 time by &ac47
.cac4c
    pla                                                               ; ac4c: 68          h
    tay                                                               ; ac4d: a8          .
    pla                                                               ; ac4e: 68          h
    rts                                                               ; ac4f: 60          `

; &ac50 referenced 1 time by &ac78
.sub_cac50
    sec                                                               ; ac50: 38          8
    sbc #1                                                            ; ac51: e9 01       ..
    sta tmp8                                                          ; ac53: 85 8d       ..
    bcs cac58                                                         ; ac55: b0 01       ..
    dey                                                               ; ac57: 88          .
; &ac58 referenced 1 time by &ac55
.cac58
    sty tmp9                                                          ; ac58: 84 8e       ..
    ldy #0                                                            ; ac5a: a0 00       ..
; &ac5c referenced 2 times by &ac69, &ac6d
.cac5c
    lda (tmp8),y                                                      ; ac5c: b1 8d       ..
    cmp #&0d                                                          ; ac5e: c9 0d       ..
    beq cac6f                                                         ; ac60: f0 0d       ..
    lda tmp8                                                          ; ac62: a5 8d       ..
    sec                                                               ; ac64: 38          8
    sbc #1                                                            ; ac65: e9 01       ..
    sta tmp8                                                          ; ac67: 85 8d       ..
    bcs cac5c                                                         ; ac69: b0 f1       ..
    dec tmp9                                                          ; ac6b: c6 8e       ..
    bne cac5c                                                         ; ac6d: d0 ed       ..
; &ac6f referenced 1 time by &ac60
.cac6f
    lda tmp8                                                          ; ac6f: a5 8d       ..
    sta tmp6                                                          ; ac71: 85 8b       ..
    lda tmp9                                                          ; ac73: a5 8e       ..
    sta tmp7                                                          ; ac75: 85 8c       ..
; &ac77 referenced 1 time by &ac9e
.return_73
    rts                                                               ; ac77: 60          `

; &ac78 referenced 5 times by &89e2, &8b73, &9ed2, &a24d, &a254
.cac78
    jsr sub_cac50                                                     ; ac78: 20 50 ac     P.
; &ac7b referenced 1 time by &acd5
.cac7b
    lda #0                                                            ; ac7b: a9 00       ..
    sta l0083                                                         ; ac7d: 85 83       ..
    ldx #&85                                                          ; ac7f: a2 85       ..
    ldy #1                                                            ; ac81: a0 01       ..
    lda (tmp8),y                                                      ; ac83: b1 8d       ..
    jsr sub_caf71                                                     ; ac85: 20 71 af     q.
    bne cac8d                                                         ; ac88: d0 03       ..
    inx                                                               ; ac8a: e8          .
    inx                                                               ; ac8b: e8          .
    inx                                                               ; ac8c: e8          .
; &ac8d referenced 1 time by &ac88
.cac8d
    stx l0084                                                         ; ac8d: 86 84       ..
; &ac8f referenced 2 times by &aca2, &aca4
.cac8f
    lda (tmp8),y                                                      ; ac8f: b1 8d       ..
    iny                                                               ; ac91: c8          .
    cmp #&20 ; ' '                                                    ; ac92: c9 20       .
    beq cac9a                                                         ; ac94: f0 04       ..
    cmp #&1a                                                          ; ac96: c9 1a       ..
    bne cac9c                                                         ; ac98: d0 02       ..
; &ac9a referenced 1 time by &ac94
.cac9a
    sty l0083                                                         ; ac9a: 84 83       ..
; &ac9c referenced 1 time by &ac98
.cac9c
    cmp #&0d                                                          ; ac9c: c9 0d       ..
    beq return_73                                                     ; ac9e: f0 d7       ..
    cpy l0084                                                         ; aca0: c4 84       ..
    beq cac8f                                                         ; aca2: f0 eb       ..
    bcc cac8f                                                         ; aca4: 90 e9       ..
    lda l0084                                                         ; aca6: a5 84       ..
    ldx l0083                                                         ; aca8: a6 83       ..
    beq cacad                                                         ; acaa: f0 01       ..
    txa                                                               ; acac: 8a          .
; &acad referenced 1 time by &acaa
.cacad
    clc                                                               ; acad: 18          .
    adc tmp8                                                          ; acae: 65 8d       e.
    sta tmp4                                                          ; acb0: 85 89       ..
    sta tmp8                                                          ; acb2: 85 8d       ..
    lda tmp9                                                          ; acb4: a5 8e       ..
    adc #0                                                            ; acb6: 69 00       i.
    sta tmp5                                                          ; acb8: 85 8a       ..
    sta tmp9                                                          ; acba: 85 8e       ..
    lda #1                                                            ; acbc: a9 01       ..
    sta tmp6                                                          ; acbe: 85 8b       ..
    lda #0                                                            ; acc0: a9 00       ..
    sta tmp7                                                          ; acc2: 85 8c       ..
    jsr sub_caa15                                                     ; acc4: 20 15 aa     ..
    lda #&0d                                                          ; acc7: a9 0d       ..
    ldy #0                                                            ; acc9: a0 00       ..
    sta (tmp4),y                                                      ; accb: 91 89       ..
    lda tmp4                                                          ; accd: a5 89       ..
    sta tmp8                                                          ; accf: 85 8d       ..
    lda tmp5                                                          ; acd1: a5 8a       ..
    sta tmp9                                                          ; acd3: 85 8e       ..
    bne cac7b                                                         ; acd5: d0 a4       ..
; &acd7 referenced 2 times by &a015, &a027
.sub_cacd7
    ldx #&4d ; 'M'                                                    ; acd7: a2 4d       .M
    ldy #&4b ; 'K'                                                    ; acd9: a0 4b       .K
    jsr draw_prompt_characters                                        ; acdb: 20 a8 a7     ..
    inc l0075                                                         ; acde: e6 75       .u
    jsr read_char                                                     ; ace0: 20 61 a7     a.
    jsr lookup_marker                                                 ; ace3: 20 ef ac     ..
    bcc return_74                                                     ; ace6: 90 06       ..
; ***************************************************************************************
; &ace8 referenced 7 times by &9bca, &9c64, &9eda, &a151, &a1c9, &a983, &ae03
.beep
    lda #7                                                            ; ace8: a9 07       ..
    jsr oswrch                                                        ; acea: 20 ee ff     ..            ; Write character 7
; &aced referenced 1 time by &acf2
.loop_caced
    sec                                                               ; aced: 38          8
; &acee referenced 1 time by &ace6
.return_74
    rts                                                               ; acee: 60          `

; ***************************************************************************************
; &acef referenced 4 times by &89a7, &ace3, &ad1a, &ad2b
.lookup_marker
    sec                                                               ; acef: 38          8
    sbc #&31 ; '1'                                                    ; acf0: e9 31       .1
    bcc loop_caced                                                    ; acf2: 90 f9       ..
    asl a                                                             ; acf4: 0a          .
    cmp #&0c                                                          ; acf5: c9 0c       ..
    bcs return_75                                                     ; acf7: b0 03       ..
    tax                                                               ; acf9: aa          .
    lda markers_array+1,x                                             ; acfa: b5 54       .T
; &acfc referenced 1 time by &acf7
.return_75
    rts                                                               ; acfc: 60          `

; ***************************************************************************************
; &acfd referenced 2 times by &84ee, &8989
.reset_area_to_entire_document
    lda top                                                           ; acfd: a5 0d       ..
    sta area_start_ptr                                                ; acff: 85 5f       ._
    lda top+1                                                         ; ad01: a5 0e       ..
    sta area_start_ptr+1                                              ; ad03: 85 60       .`
    lda page                                                          ; ad05: a5 0b       ..
    sta area_end_ptr                                                  ; ad07: 85 61       .a
    lda page+1                                                        ; ad09: a5 0c       ..
    sta area_end_ptr+1                                                ; ad0b: 85 62       .b
    rts                                                               ; ad0d: 60          `

; ***************************************************************************************
; &ad0e referenced 2 times by &a191, &a1b4
.clear_marks_1_2
    lda #0                                                            ; ad0e: a9 00       ..
    ldx #3                                                            ; ad10: a2 03       ..
; &ad12 referenced 1 time by &ad15
.loop_cad12
    sta markers_array,x                                               ; ad12: 95 53       .S
    dex                                                               ; ad14: ca          .
    bpl loop_cad12                                                    ; ad15: 10 fb       ..
    rts                                                               ; ad17: 60          `

; ***************************************************************************************
; &ad18 referenced 3 times by &a17c, &a197, &a1ba
.reset_area_to_marks_1_2
    lda #&31 ; '1'                                                    ; ad18: a9 31       .1
    jsr lookup_marker                                                 ; ad1a: 20 ef ac     ..
    bcs return_76                                                     ; ad1d: b0 27       .'
    beq cad45                                                         ; ad1f: f0 24       .$
    lda markers_array,x                                               ; ad21: b5 53       .S
    sta area_start_ptr                                                ; ad23: 85 5f       ._
    lda markers_array+1,x                                             ; ad25: b5 54       .T
    sta area_start_ptr+1                                              ; ad27: 85 60       .`
    lda #&32 ; '2'                                                    ; ad29: a9 32       .2
    jsr lookup_marker                                                 ; ad2b: 20 ef ac     ..
    bcs return_76                                                     ; ad2e: b0 16       ..
    beq cad45                                                         ; ad30: f0 13       ..
    lda markers_array,x                                               ; ad32: b5 53       .S
    sta area_end_ptr                                                  ; ad34: 85 61       .a
    lda markers_array+1,x                                             ; ad36: b5 54       .T
    sta area_end_ptr+1                                                ; ad38: 85 62       .b
    ldx #&63 ; 'c'                                                    ; ad3a: a2 63       .c
    jsr sub_cad47                                                     ; ad3c: 20 47 ad     G.
    jsr sanitise_area                                                 ; ad3f: 20 5d 89     ].
    clc                                                               ; ad42: 18          .
    bne return_76                                                     ; ad43: d0 01       ..
; &ad45 referenced 2 times by &ad1f, &ad30
.cad45
    sec                                                               ; ad45: 38          8
; &ad46 referenced 3 times by &ad1d, &ad2e, &ad43
.return_76
    rts                                                               ; ad46: 60          `

; &ad47 referenced 2 times by &a01e, &ad3c
.sub_cad47
    jsr get_line_length                                               ; ad47: 20 f1 aa     ..
    cpy xpos                                                          ; ad4a: c4 40       .@
    bcc cad5d                                                         ; ad4c: 90 0f       ..
    ldy #0                                                            ; ad4e: a0 00       ..
    lda (current_format_line_ptr),y                                   ; ad50: b1 04       ..
    ldy xpos                                                          ; ad52: a4 40       .@
    jsr sub_caf71                                                     ; ad54: 20 71 af     q.
    bne cad5c                                                         ; ad57: d0 03       ..
    iny                                                               ; ad59: c8          .
    iny                                                               ; ad5a: c8          .
    iny                                                               ; ad5b: c8          .
; &ad5c referenced 1 time by &ad57
.cad5c
    tya                                                               ; ad5c: 98          .
; &ad5d referenced 1 time by &ad4c
.cad5d
    clc                                                               ; ad5d: 18          .
    adc ptr4                                                          ; ad5e: 65 08       e.
    sta ptr1,x                                                        ; ad60: 95 00       ..
    lda ptr4+1                                                        ; ad62: a5 09       ..
    adc #0                                                            ; ad64: 69 00       i.
    sta ptr1+1,x                                                      ; ad66: 95 01       ..
    rts                                                               ; ad68: 60          `

; ***************************************************************************************
; On Entry:
;     A: register name
; ***************************************************************************************
; &ad69 referenced 2 times by &95c2, &ad83
.get_register_address
    jsr is_uppercase                                                  ; ad69: 20 6b 8c     k.
    bcs return_77                                                     ; ad6c: b0 12       ..
    and #&df                                                          ; ad6e: 29 df       ).
    pha                                                               ; ad70: 48          H
    sbc #&40 ; '@'                                                    ; ad71: e9 40       .@
    asl a                                                             ; ad73: 0a          .
    adc #<register_value_array                                        ; ad74: 69 98       i.
    sta tmp6                                                          ; ad76: 85 8b       ..
    lda #>register_value_array                                        ; ad78: a9 07       ..
    adc #0                                                            ; ad7a: 69 00       i.
    sta tmp7                                                          ; ad7c: 85 8c       ..
    pla                                                               ; ad7e: 68          h
    clc                                                               ; ad7f: 18          .
; &ad80 referenced 1 time by &ad6c
.return_77
    rts                                                               ; ad80: 60          `

; ***************************************************************************************
; On Entry:
;     TMP7/TMP6: pointer to register
;     A: register name
; ***************************************************************************************
; &ad81 referenced 2 times by &97cf, &ada7
.render_register
    sty l0084                                                         ; ad81: 84 84       ..
    jsr get_register_address                                          ; ad83: 20 69 ad     i.
    ldy #0                                                            ; ad86: a0 00       ..
    sty tmp8                                                          ; ad88: 84 8d       ..
    sty tmp9                                                          ; ad8a: 84 8e       ..
    bcs cada2                                                         ; ad8c: b0 14       ..
    bit lada6                                                         ; ad8e: 2c a6 ad    ,..
    cmp #&44 ; 'D'                                                    ; ad91: c9 44       .D
    beq cada3                                                         ; ad93: f0 0e       ..
    cmp #&54 ; 'T'                                                    ; ad95: c9 54       .T
    beq cada3                                                         ; ad97: f0 0a       ..
    lda (tmp6),y                                                      ; ad99: b1 8b       ..
    sta tmp8                                                          ; ad9b: 85 8d       ..
    iny                                                               ; ad9d: c8          .              ; Y=&01
    lda (tmp6),y                                                      ; ad9e: b1 8b       ..
    sta tmp9                                                          ; ada0: 85 8e       ..
; &ada2 referenced 1 time by &ad8c
.cada2
    clv                                                               ; ada2: b8          .
; &ada3 referenced 2 times by &ad93, &ad97
.cada3
    ldy l0084                                                         ; ada3: a4 84       ..
    rts                                                               ; ada5: 60          `

; &ada6 referenced 1 time by &ad8e
.lada6
    equb &40                                                          ; ada6: 40          @

; ***************************************************************************************
; &ada7 referenced 2 times by &93f7, &9565
.render_date_time_to_output_buffer
    jsr render_register                                               ; ada7: 20 81 ad     ..
    sty l0084                                                         ; adaa: 84 84       ..
    bvc cadea                                                         ; adac: 50 3c       P<
    pha                                                               ; adae: 48          H
    stx l0082                                                         ; adaf: 86 82       ..
    lda #0                                                            ; adb1: a9 00       ..
    sta input_buffer                                                  ; adb3: 8d 00 05    ...
    ldx #<(input_buffer)                                              ; adb6: a2 00       ..
    ldy #>(input_buffer)                                              ; adb8: a0 05       ..
    lda #osword_read_cmos_clock                                       ; adba: a9 0e       ..
    jsr osword                                                        ; adbc: 20 f1 ff     ..            ; Read CMOS clock
    lda input_buffer                                                  ; adbf: ad 00 05    ...
    beq cade7                                                         ; adc2: f0 23       .#
    pla                                                               ; adc4: 68          h
    ldy #4                                                            ; adc5: a0 04       ..
    ldx #&0b                                                          ; adc7: a2 0b       ..
    cmp #&44 ; 'D'                                                    ; adc9: c9 44       .D
    beq cadd1                                                         ; adcb: f0 04       ..
    ldy #&10                                                          ; adcd: a0 10       ..
    ldx #8                                                            ; adcf: a2 08       ..
; &add1 referenced 1 time by &adcb
.cadd1
    stx l0083                                                         ; add1: 86 83       ..
    ldx l0082                                                         ; add3: a6 82       ..
; &add5 referenced 1 time by &ade3
.loop_cadd5
    lda input_buffer,y                                                ; add5: b9 00 05    ...
    sta output_buffer,x                                               ; add8: 9d 54 06    .T.
    inx                                                               ; addb: e8          .
    cpx #&83                                                          ; addc: e0 83       ..
    beq caded                                                         ; adde: f0 0d       ..
    iny                                                               ; ade0: c8          .
    dec l0083                                                         ; ade1: c6 83       ..
    bne loop_cadd5                                                    ; ade3: d0 f0       ..
    beq caded                                                         ; ade5: f0 06       ..             ; ALWAYS branch

; &ade7 referenced 1 time by &adc2
.cade7
    pla                                                               ; ade7: 68          h
    ldx l0082                                                         ; ade8: a6 82       ..
; &adea referenced 1 time by &adac
.cadea
    jsr render_number_to_output_buffer                                ; adea: 20 8c a6     ..
; &aded referenced 2 times by &adde, &ade5
.caded
    ldy l0084                                                         ; aded: a4 84       ..
    rts                                                               ; adef: 60          `

; &adf0 referenced 2 times by &911e, &9894
.sub_cadf0
    ldx #8                                                            ; adf0: a2 08       ..
    lda tmp9                                                          ; adf2: a5 8e       ..
; &adf4 referenced 1 time by &ae00
.loop_cadf4
    asl tmp8                                                          ; adf4: 06 8d       ..
    rol a                                                             ; adf6: 2a          *
    cmp l0046                                                         ; adf7: c5 46       .F
    bcc cadff                                                         ; adf9: 90 04       ..
    sbc l0046                                                         ; adfb: e5 46       .F
    inc tmp8                                                          ; adfd: e6 8d       ..
; &adff referenced 1 time by &adf9
.cadff
    dex                                                               ; adff: ca          .
    bne loop_cadf4                                                    ; ae00: d0 f2       ..
    rts                                                               ; ae02: 60          `

; &ae03 referenced 3 times by &ae0a, &ae15, &ae19
.cae03
    jmp beep                                                          ; ae03: 4c e8 ac    L..

; &ae06 referenced 3 times by &9c04, &9e25, &aee4
.sub_cae06
    lda xpos                                                          ; ae06: a5 40       .@
    cmp #&84                                                          ; ae08: c9 84       ..
    bcs cae03                                                         ; ae0a: b0 f7       ..
    stx input_buffer_ptr+1                                            ; ae0c: 86 80       ..
    jsr get_line_length                                               ; ae0e: 20 f1 aa     ..
    tya                                                               ; ae11: 98          .
    clc                                                               ; ae12: 18          .
    adc input_buffer_ptr+1                                            ; ae13: 65 80       e.
    bcs cae03                                                         ; ae15: b0 ec       ..
    cmp #&85                                                          ; ae17: c9 85       ..
    bcs cae03                                                         ; ae19: b0 e8       ..
    inc l006d                                                         ; ae1b: e6 6d       .m
    lda current_edit_line_ptr                                         ; ae1d: a5 02       ..
    sta tmp6                                                          ; ae1f: 85 8b       ..
    lda current_edit_line_ptr+1                                       ; ae21: a5 03       ..
    sta tmp7                                                          ; ae23: 85 8c       ..
    ldy #&84                                                          ; ae25: a0 84       ..
; &ae27 referenced 1 time by &ae60
.cae27
    dey                                                               ; ae27: 88          .
    ldx #0                                                            ; ae28: a2 00       ..
    tya                                                               ; ae2a: 98          .
    clc                                                               ; ae2b: 18          .
    adc input_buffer_ptr+1                                            ; ae2c: 65 80       e.
    bcs cae35                                                         ; ae2e: b0 05       ..
    cmp #&84                                                          ; ae30: c9 84       ..
    bcs cae35                                                         ; ae32: b0 01       ..
    tax                                                               ; ae34: aa          .
; &ae35 referenced 2 times by &ae2e, &ae32
.cae35
    stx l0081                                                         ; ae35: 86 81       ..
; &ae37 referenced 1 time by &ae4f
.loop_cae37
    jsr sub_ca536                                                     ; ae37: 20 36 a5     6.
    bne cae52                                                         ; ae3a: d0 16       ..
    lda l0081                                                         ; ae3c: a5 81       ..
    beq cae4b                                                         ; ae3e: f0 0b       ..
    clc                                                               ; ae40: 18          .
    adc current_edit_line_ptr                                         ; ae41: 65 02       e.
    sta markers_array,x                                               ; ae43: 95 53       .S
    lda current_edit_line_ptr+1                                       ; ae45: a5 03       ..
    adc #0                                                            ; ae47: 69 00       i.
    bne cae4d                                                         ; ae49: d0 02       ..
; &ae4b referenced 1 time by &ae3e
.cae4b
    sta markers_array,x                                               ; ae4b: 95 53       .S
; &ae4d referenced 1 time by &ae49
.cae4d
    sta markers_array+1,x                                             ; ae4d: 95 54       .T
    jmp loop_cae37                                                    ; ae4f: 4c 37 ae    L7.

; &ae52 referenced 1 time by &ae3a
.cae52
    lda (current_edit_line_ptr),y                                     ; ae52: b1 02       ..
    sty l0084                                                         ; ae54: 84 84       ..
    ldy l0081                                                         ; ae56: a4 81       ..
    beq cae5c                                                         ; ae58: f0 02       ..
    sta (current_edit_line_ptr),y                                     ; ae5a: 91 02       ..
; &ae5c referenced 1 time by &ae58
.cae5c
    ldy l0084                                                         ; ae5c: a4 84       ..
    cpy xpos                                                          ; ae5e: c4 40       .@
    bne cae27                                                         ; ae60: d0 c5       ..
    clc                                                               ; ae62: 18          .
    rts                                                               ; ae63: 60          `

; &ae64 referenced 3 times by &9e4d, &9ee5, &a14e
.cae64
    stx input_buffer_ptr+1                                            ; ae64: 86 80       ..
    inc l006d                                                         ; ae66: e6 6d       .m
    lda current_edit_line_ptr                                         ; ae68: a5 02       ..
    sta tmp6                                                          ; ae6a: 85 8b       ..
    lda current_edit_line_ptr+1                                       ; ae6c: a5 03       ..
    sta tmp7                                                          ; ae6e: 85 8c       ..
    ldy xpos                                                          ; ae70: a4 40       .@
    tya                                                               ; ae72: 98          .
    clc                                                               ; ae73: 18          .
    adc input_buffer_ptr+1                                            ; ae74: 65 80       e.
    sta l0084                                                         ; ae76: 85 84       ..
; &ae78 referenced 2 times by &ae95, &ae9b
.cae78
    jsr sub_ca536                                                     ; ae78: 20 36 a5     6.
    bne cae98                                                         ; ae7b: d0 1b       ..
    lda #0                                                            ; ae7d: a9 00       ..
    cpy l0084                                                         ; ae7f: c4 84       ..
    bcc cae91                                                         ; ae81: 90 0e       ..
    tya                                                               ; ae83: 98          .
    sbc input_buffer_ptr+1                                            ; ae84: e5 80       ..
    clc                                                               ; ae86: 18          .
    adc current_edit_line_ptr                                         ; ae87: 65 02       e.
    sta markers_array,x                                               ; ae89: 95 53       .S
    lda current_edit_line_ptr+1                                       ; ae8b: a5 03       ..
    adc #0                                                            ; ae8d: 69 00       i.
    bne cae93                                                         ; ae8f: d0 02       ..
; &ae91 referenced 1 time by &ae81
.cae91
    sta markers_array,x                                               ; ae91: 95 53       .S
; &ae93 referenced 1 time by &ae8f
.cae93
    sta markers_array+1,x                                             ; ae93: 95 54       .T
    jmp cae78                                                         ; ae95: 4c 78 ae    Lx.

; &ae98 referenced 1 time by &ae7b
.cae98
    iny                                                               ; ae98: c8          .
    cpy #&85                                                          ; ae99: c0 85       ..
    bcc cae78                                                         ; ae9b: 90 db       ..
    lda xpos                                                          ; ae9d: a5 40       .@
    cmp #&84                                                          ; ae9f: c9 84       ..
    bcs return_78                                                     ; aea1: b0 1e       ..
    ldy xpos                                                          ; aea3: a4 40       .@
; &aea5 referenced 1 time by &aebf
.loop_caea5
    sty l0084                                                         ; aea5: 84 84       ..
    ldx #&10                                                          ; aea7: a2 10       ..
    tya                                                               ; aea9: 98          .
    clc                                                               ; aeaa: 18          .
    adc input_buffer_ptr+1                                            ; aeab: 65 80       e.
    bcs caeb7                                                         ; aead: b0 08       ..
    tay                                                               ; aeaf: a8          .
    cpy #&84                                                          ; aeb0: c0 84       ..
    bcs caeb7                                                         ; aeb2: b0 03       ..
    lda (current_edit_line_ptr),y                                     ; aeb4: b1 02       ..
    tax                                                               ; aeb6: aa          .
; &aeb7 referenced 2 times by &aead, &aeb2
.caeb7
    ldy l0084                                                         ; aeb7: a4 84       ..
    txa                                                               ; aeb9: 8a          .
    sta (current_edit_line_ptr),y                                     ; aeba: 91 02       ..
    iny                                                               ; aebc: c8          .
    cpy #&84                                                          ; aebd: c0 84       ..
    bcc loop_caea5                                                    ; aebf: 90 e4       ..
; &aec1 referenced 1 time by &aea1
.return_78
    rts                                                               ; aec1: 60          `

; &aec2 referenced 4 times by &9b78, &9e8a, &aed6, &aefa
.sub_caec2
    lda ruler_left_stop                                               ; aec2: a5 3f       .?
    beq caed4                                                         ; aec4: f0 0e       ..
    ldy #0                                                            ; aec6: a0 00       ..
; &aec8 referenced 1 time by &aed1
.loop_caec8
    lda (current_edit_line_ptr),y                                     ; aec8: b1 02       ..
    iny                                                               ; aeca: c8          .
    cmp #&0b                                                          ; aecb: c9 0b       ..
    beq caed4                                                         ; aecd: f0 05       ..
    cpy #&84                                                          ; aecf: c0 84       ..
    bcc loop_caec8                                                    ; aed1: 90 f5       ..
    rts                                                               ; aed3: 60          `

; &aed4 referenced 3 times by &aec4, &aecd, &aed9
.caed4
    clc                                                               ; aed4: 18          .
    rts                                                               ; aed5: 60          `

; &aed6 referenced 2 times by &9a76, &9a87
.sub_caed6
    jsr sub_caec2                                                     ; aed6: 20 c2 ae     ..
    bcc caed4                                                         ; aed9: 90 f9       ..
    ldy #0                                                            ; aedb: a0 00       ..
; &aedd referenced 1 time by &af2a
.sub_caedd
    lda xpos                                                          ; aedd: a5 40       .@
    pha                                                               ; aedf: 48          H
    sty xpos                                                          ; aee0: 84 40       .@
    ldx #1                                                            ; aee2: a2 01       ..
    jsr sub_cae06                                                     ; aee4: 20 06 ae     ..
    bcs caef0                                                         ; aee7: b0 07       ..
    ldy xpos                                                          ; aee9: a4 40       .@
    lda #&0b                                                          ; aeeb: a9 0b       ..
    sta (current_edit_line_ptr),y                                     ; aeed: 91 02       ..
    iny                                                               ; aeef: c8          .
; &aef0 referenced 1 time by &aee7
.caef0
    pla                                                               ; aef0: 68          h
    sta xpos                                                          ; aef1: 85 40       .@
    rts                                                               ; aef3: 60          `

; &aef4 referenced 2 times by &9bd8, &9e3b
.sub_caef4
    lda format_mode_flag                                              ; aef4: a5 4f       .O
    and #&81                                                          ; aef6: 29 81       ).
    bne caf31                                                         ; aef8: d0 37       .7
    jsr sub_caec2                                                     ; aefa: 20 c2 ae     ..
    bcc caf31                                                         ; aefd: 90 32       .2
    jsr get_line_length                                               ; aeff: 20 f1 aa     ..
    lda xpos                                                          ; af02: a5 40       .@
    sta l0083                                                         ; af04: 85 83       ..
    sty xpos                                                          ; af06: 84 40       .@
    jsr sub_ca608                                                     ; af08: 20 08 a6     ..
    lda l0072                                                         ; af0b: a5 72       .r
    cmp ruler_left_stop                                               ; af0d: c5 3f       .?
    bcc caf19                                                         ; af0f: 90 08       ..
    ldy l0083                                                         ; af11: a4 83       ..
    sty xpos                                                          ; af13: 84 40       .@
    inc xpos                                                          ; af15: e6 40       .@
    bcs caf2a                                                         ; af17: b0 11       ..             ; ALWAYS branch

; &af19 referenced 1 time by &af0f
.caf19
    lda l0083                                                         ; af19: a5 83       ..
    ldy xpos                                                          ; af1b: a4 40       .@
    cpy l0083                                                         ; af1d: c4 83       ..
    bcs caf28                                                         ; af1f: b0 07       ..
    sec                                                               ; af21: 38          8
    sbc ruler_left_stop                                               ; af22: e5 3f       .?
    bcc caf2a                                                         ; af24: 90 04       ..
    adc xpos                                                          ; af26: 65 40       e@
; &af28 referenced 1 time by &af1f
.caf28
    sta xpos                                                          ; af28: 85 40       .@
; &af2a referenced 2 times by &af17, &af24
.caf2a
    jsr sub_caedd                                                     ; af2a: 20 dd ae     ..
    bcs return_79                                                     ; af2d: b0 03       ..
    inc l0074                                                         ; af2f: e6 74       .t
; &af31 referenced 2 times by &aef8, &aefd
.caf31
    clc                                                               ; af31: 18          .
; &af32 referenced 1 time by &af2d
.return_79
    rts                                                               ; af32: 60          `

; &af33 referenced 2 times by &9c8f, &9fa1
.sub_caf33
    lda current_edit_line_ptr                                         ; af33: a5 02       ..
    sta tmp0                                                          ; af35: 85 85       ..
    lda current_edit_line_ptr+1                                       ; af37: a5 03       ..
    sta tmp1                                                          ; af39: 85 86       ..
    ldy xpos                                                          ; af3b: a4 40       .@
    beq caf55                                                         ; af3d: f0 16       ..
; &af3f referenced 1 time by &af48
.loop_caf3f
    dey                                                               ; af3f: 88          .
    beq caf55                                                         ; af40: f0 13       ..
    jsr sub_ca5ab                                                     ; af42: 20 ab a5     ..
    dey                                                               ; af45: 88          .
    cmp #&20 ; ' '                                                    ; af46: c9 20       .
    beq loop_caf3f                                                    ; af48: f0 f5       ..
; &af4a referenced 1 time by &af53
.loop_caf4a
    dey                                                               ; af4a: 88          .
    jsr sub_ca5ab                                                     ; af4b: 20 ab a5     ..
    cmp #&20 ; ' '                                                    ; af4e: c9 20       .
    beq caf55                                                         ; af50: f0 03       ..
    dey                                                               ; af52: 88          .
    bne loop_caf4a                                                    ; af53: d0 f5       ..
; &af55 referenced 3 times by &af3d, &af40, &af50
.caf55
    sty xpos                                                          ; af55: 84 40       .@
    jsr sub_ca5ab                                                     ; af57: 20 ab a5     ..
    dey                                                               ; af5a: 88          .
    rts                                                               ; af5b: 60          `

; &af5c referenced 3 times by &9f38, &9f5f, &aab0
.caf5c
    sec                                                               ; af5c: 38          8
    bcs caf60                                                         ; af5d: b0 01       ..             ; ALWAYS branch

; &af5f referenced 3 times by &875d, &9f76, &aa9c
.sub_caf5f
    clc                                                               ; af5f: 18          .
; &af60 referenced 1 time by &af5d
.caf60
    php                                                               ; af60: 08          .
    lda format_mode_flag                                              ; af61: a5 4f       .O
    rol format_mode_flag                                              ; af63: 26 4f       &O
    plp                                                               ; af65: 28          (
    ror format_mode_flag                                              ; af66: 66 4f       fO
    cmp format_mode_flag                                              ; af68: c5 4f       .O
    beq return_80                                                     ; af6a: f0 02       ..
    inc l0075                                                         ; af6c: e6 75       .u
; &af6e referenced 1 time by &af6a
.return_80
    rts                                                               ; af6e: 60          `

; &af6f referenced 3 times by &86ba, &8f4b, &a49d
.sub_caf6f
    lda (tmp0),y                                                      ; af6f: b1 85       ..
; &af71 referenced 14 times by &8ba3, &8bc1, &8ca6, &96d2, &9985, &9ad9, &9dc5, &9eb1, &9f66, &aaa7, &aaf5, &ac2e, &ac85, &ad54
.sub_caf71
    cmp #&80                                                          ; af71: c9 80       ..
    beq return_81                                                     ; af73: f0 03       ..
    cmp #&81                                                          ; af75: c9 81       ..
    clc                                                               ; af77: 18          .
; &af78 referenced 1 time by &af73
.return_81
    rts                                                               ; af78: 60          `

; ***************************************************************************************
; &af79 referenced 2 times by &80d6, &85e9
.system_init
    lda #osbyte_read_char_at_cursor                                   ; af79: a9 87       ..
    jsr osbyte                                                        ; af7b: 20 f4 ff     ..            ; Read character at the text cursor, and current screen MODE
    sty current_screen_mode                                           ; af7e: 84 37       .7             ; Y is the current screen MODE (0-7)
    lda #osbyte_reserved_for_application_software                     ; af80: a9 a3       ..
    ldx #&ff                                                          ; af82: a2 ff       ..
    stx is_tube_flag                                                  ; af84: 86 52       .R
    ldy #1                                                            ; af86: a0 01       ..
    jsr osbyte                                                        ; af88: 20 f4 ff     ..            ; Reserved for application software
    cpy #1                                                            ; af8b: c0 01       ..
    bne caf91                                                         ; af8d: d0 02       ..
    sty is_tube_flag                                                  ; af8f: 84 52       .R
; &af91 referenced 1 time by &af8d
.caf91
    lda #osbyte_read_himem                                            ; af91: a9 84       ..
    jsr osbyte                                                        ; af93: 20 f4 ff     ..            ; Read top of user memory (HIMEM)
    stx himem                                                         ; af96: 86 0f       ..             ; X and Y contain the address of HIMEM (low, high)
    sty himem+1                                                       ; af98: 84 10       ..
    lda #osbyte_read_vdu_variable                                     ; af9a: a9 a0       ..
    ldx #9                                                            ; af9c: a2 09       ..
    jsr osbyte                                                        ; af9e: 20 f4 ff     ..            ; Read the text window bottom position (VDU variable X=9)
    sty screen_width                                                  ; afa1: 84 36       .6             ; Y is the text window right position
    stx screen_height                                                 ; afa3: 86 35       .5             ; X is the text window bottom position
    jsr detect_tube                                                   ; afa5: 20 b0 af     ..
    bcs return_82                                                     ; afa8: b0 05       ..
    ror is_tube_flag                                                  ; afaa: 66 52       fR
    clc                                                               ; afac: 18          .
    rol is_tube_flag                                                  ; afad: 26 52       &R
; &afaf referenced 1 time by &afa8
.return_82
    rts                                                               ; afaf: 60          `

; ***************************************************************************************
; &afb0 referenced 2 times by &afa5, &b328
.detect_tube
    lda #osbyte_read_high_order_address                               ; afb0: a9 82       ..
    jsr osbyte                                                        ; afb2: 20 f4 ff     ..            ; Read the filing system 'machine high order address'
    sec                                                               ; afb5: 38          8
    iny                                                               ; afb6: c8          .
    bne return_83                                                     ; afb7: d0 04       ..
    inx                                                               ; afb9: e8          .
    bne return_83                                                     ; afba: d0 01       ..
    clc                                                               ; afbc: 18          .
; &afbd referenced 2 times by &afb7, &afba
.return_83
    rts                                                               ; afbd: 60          `

; ***************************************************************************************
; &afbe referenced 4 times by &8130, &8535, &8da5, &8db3
.compute_bytes_free
    lda himem                                                         ; afbe: a5 0f       ..
    sec                                                               ; afc0: 38          8
    sbc top                                                           ; afc1: e5 0d       ..
    tax                                                               ; afc3: aa          .
    lda himem+1                                                       ; afc4: a5 10       ..
    sbc top+1                                                         ; afc6: e5 0e       ..
    tay                                                               ; afc8: a8          .
; &afc9 referenced 1 time by &afcd
.return_84
    rts                                                               ; afc9: 60          `

; ***************************************************************************************
; &afca referenced 2 times by &846e, &887d
.initialise_document_if_document_bad
    jsr check_for_bad_document                                        ; afca: 20 86 8e     ..
    beq return_84                                                     ; afcd: f0 fa       ..
; ***************************************************************************************
; &afcf referenced 3 times by &80f3, &849b, &879a
.initialise_document
    lda zp_initialisation_canary                                      ; afcf: a5 0a       ..
    cmp #&aa                                                          ; afd1: c9 aa       ..
    bne cafdc                                                         ; afd3: d0 07       ..
    lda document_initialisation_canary                                ; afd5: ad ce 05    ...
    cmp #&0d                                                          ; afd8: c9 0d       ..
    beq cafee                                                         ; afda: f0 12       ..
; &afdc referenced 1 time by &afd3
.cafdc
    lda #0                                                            ; afdc: a9 00       ..
    sta printer_driver_name                                           ; afde: 8d 84 07    ...
    sta format_mode_flag                                              ; afe1: 85 4f       .O
    sta justifying_flag                                               ; afe3: 85 50       .P
    sta insert_mode_flag                                              ; afe5: 85 51       .Q
    ldx #&14                                                          ; afe7: a2 14       ..
; &afe9 referenced 1 time by &afec
.loop_cafe9
    sta print_flags,x                                                 ; afe9: 95 69       .i
    dex                                                               ; afeb: ca          .
    bpl loop_cafe9                                                    ; afec: 10 fb       ..
; &afee referenced 1 time by &afda
.cafee
    lda #osbyte_read_oshwm                                            ; afee: a9 83       ..
    jsr osbyte                                                        ; aff0: 20 f4 ff     ..            ; Read top of operating system RAM address (OSHWM)
    stx oshwm                                                         ; aff3: 86 1f       ..             ; X and Y contain the address of OSHWM (low, high)
    sty oshwm+1                                                       ; aff5: 84 20       .
    iny                                                               ; aff7: c8          .
    inx                                                               ; aff8: e8          .
    stx page                                                          ; aff9: 86 0b       ..
    bne caffe                                                         ; affb: d0 01       ..
    iny                                                               ; affd: c8          .
; &affe referenced 1 time by &affb
.caffe
    sty page+1                                                        ; affe: 84 0c       ..
    ldy #0                                                            ; b000: a0 00       ..
    sty l003c                                                         ; b002: 84 3c       .<
    sty xpos                                                          ; b004: 84 40       .@
    lda #&aa                                                          ; b006: a9 aa       ..
    sta zp_initialisation_canary                                      ; b008: 85 0a       ..
    sta (oshwm),y                                                     ; b00a: 91 1f       ..
    lda page                                                          ; b00c: a5 0b       ..
    sec                                                               ; b00e: 38          8
    sbc #1                                                            ; b00f: e9 01       ..
    sta tmp8                                                          ; b011: 85 8d       ..
    lda page+1                                                        ; b013: a5 0c       ..
    sbc #0                                                            ; b015: e9 00       ..
    sta tmp9                                                          ; b017: 85 8e       ..
    lda #&0d                                                          ; b019: a9 0d       ..
    sta document_initialisation_canary                                ; b01b: 8d ce 05    ...
    sta (tmp8),y                                                      ; b01e: 91 8d       ..
    lda page                                                          ; b020: a5 0b       ..
    sta top                                                           ; b022: 85 0d       ..
    lda page+1                                                        ; b024: a5 0c       ..
    sta top+1                                                         ; b026: 85 0e       ..
    lda #<(current_line_buffer)                                       ; b028: a9 45       .E
    sta ptr1                                                          ; b02a: 85 00       ..
    clc                                                               ; b02c: 18          .
    adc #3                                                            ; b02d: 69 03       i.
    sta current_edit_line_ptr                                         ; b02f: 85 02       ..
    sta current_format_line_ptr                                       ; b031: 85 04       ..
    lda #>(current_line_buffer)                                       ; b033: a9 05       ..
    sta ptr1+1                                                        ; b035: 85 01       ..
    adc #0                                                            ; b037: 69 00       i.
    sta current_edit_line_ptr+1                                       ; b039: 85 03       ..
    sta current_format_line_ptr+1                                     ; b03b: 85 05       ..
    lda #<(current_ruler_buffer)                                      ; b03d: a9 cf       ..
    ldy #>(current_ruler_buffer)                                      ; b03f: a0 05       ..
    jsr create_default_ruler                                          ; b041: 20 e0 b0     ..
    iny                                                               ; b044: c8          .
    lda #&0d                                                          ; b045: a9 0d       ..
    sta (tmp0),y                                                      ; b047: 91 85       ..
    ldy #&ff                                                          ; b049: a0 ff       ..
    lda #<(l05cc)                                                     ; b04b: a9 cc       ..
    sta (oshwm),y                                                     ; b04d: 91 1f       ..
    dey                                                               ; b04f: 88          .              ; Y=&fe
    lda #>(l05cc)                                                     ; b050: a9 05       ..
    sta (oshwm),y                                                     ; b052: 91 1f       ..
    jsr cb07a                                                         ; b054: 20 7a b0     z.
    jsr clear_cmd                                                     ; b057: 20 91 b0     ..
; &b05a referenced 5 times by &84e8, &84fc, &9e69, &a18e, &a1aa
.cb05a
    ldy page+1                                                        ; b05a: a4 0c       ..
    cpy top+1                                                         ; b05c: c4 0e       ..
    bne return_85                                                     ; b05e: d0 19       ..
    lda page                                                          ; b060: a5 0b       ..
    cmp top                                                           ; b062: c5 0d       ..
    bne return_85                                                     ; b064: d0 13       ..
    inc top                                                           ; b066: e6 0d       ..
    bne cb06c                                                         ; b068: d0 02       ..
    inc top+1                                                         ; b06a: e6 0e       ..
; &b06c referenced 1 time by &b068
.cb06c
    sta ptr4                                                          ; b06c: 85 08       ..
    sty ptr4+1                                                        ; b06e: 84 09       ..
    ldy #0                                                            ; b070: a0 00       ..
    lda #&0d                                                          ; b072: a9 0d       ..
    sta (page),y                                                      ; b074: 91 0b       ..
    tya                                                               ; b076: 98          .              ; A=&00
    sta (top),y                                                       ; b077: 91 0d       ..
; &b079 referenced 2 times by &b05e, &b064
.return_85
    rts                                                               ; b079: 60          `

; &b07a referenced 4 times by &84d5, &84f9, &88e3, &b054
.cb07a
    lda page                                                          ; b07a: a5 0b       ..
    sta ptr4                                                          ; b07c: 85 08       ..
    lda page+1                                                        ; b07e: a5 0c       ..
    sta ptr4+1                                                        ; b080: 85 09       ..
    lda #0                                                            ; b082: a9 00       ..
    sta xpos                                                          ; b084: 85 40       .@
    ldy #&fe                                                          ; b086: a0 fe       ..
    sty l0012                                                         ; b088: 84 12       ..
    sty l0070                                                         ; b08a: 84 70       .p
    sty l0033                                                         ; b08c: 84 33       .3
    jmp cab91                                                         ; b08e: 4c 91 ab    L..

; ***************************************************************************************
; &b091 referenced 3 times by &88e0, &a980, &b057
.clear_cmd
    ldx #&0b                                                          ; b091: a2 0b       ..
    lda #0                                                            ; b093: a9 00       ..
; &b095 referenced 1 time by &b098
.loop_cb095
    sta markers_array,x                                               ; b095: 95 53       .S
    dex                                                               ; b097: ca          .
    bpl loop_cb095                                                    ; b098: 10 fb       ..
    rts                                                               ; b09a: 60          `

; ***************************************************************************************
; &b09b referenced 2 times by &832a, &9b33
.enter_editor_mode
    jsr clear_screen                                                  ; b09b: 20 82 a7     ..
    lda #0                                                            ; b09e: a9 00       ..
    sta l006d                                                         ; b0a0: 85 6d       .m
    sta l006f                                                         ; b0a2: 85 6f       .o
    sta l006e                                                         ; b0a4: 85 6e       .n
    ldx screen_height                                                 ; b0a6: a6 35       .5
; &b0a8 referenced 1 time by &b0ac
.loop_cb0a8
    sta line_lengths,x                                                ; b0a8: 9d cc 07    ...
    dex                                                               ; b0ab: ca          .
    bpl loop_cb0a8                                                    ; b0ac: 10 fa       ..
    lda #&0d                                                          ; b0ae: a9 0d       ..
    jsr oswrch                                                        ; b0b0: 20 ee ff     ..            ; Write character 13
    lda #&0f                                                          ; b0b3: a9 0f       ..
    jsr oswrch                                                        ; b0b5: 20 ee ff     ..            ; Write character 15
    lda #osbyte_read_write_function_key_status                        ; b0b8: a9 e1       ..
    ldy #0                                                            ; b0ba: a0 00       ..
    ldx #&8c                                                          ; b0bc: a2 8c       ..
    jsr osbyte                                                        ; b0be: 20 f4 ff     ..            ; Write function key status, value X=140
    lda #osbyte_read_write_shift_function_key_status                  ; b0c1: a9 e2       ..
    ldy #0                                                            ; b0c3: a0 00       ..
    ldx #&9c                                                          ; b0c5: a2 9c       ..
    jsr osbyte                                                        ; b0c7: 20 f4 ff     ..            ; Write SHIFT+function keys status, value X=156
    lda #osbyte_read_write_ctrl_function_key_status                   ; b0ca: a9 e3       ..
    sta l003d                                                         ; b0cc: 85 3d       .=
    ldy #0                                                            ; b0ce: a0 00       ..
    ldx #&ac                                                          ; b0d0: a2 ac       ..
    jsr osbyte                                                        ; b0d2: 20 f4 ff     ..            ; Write CTRL+function keys status, value X=172
    lda #osbyte_set_cursor_editing                                    ; b0d5: a9 04       ..
    ldx #2                                                            ; b0d7: a2 02       ..
    stx l0073                                                         ; b0d9: 86 73       .s
    stx l0076                                                         ; b0db: 86 76       .v
    jmp osbyte                                                        ; b0dd: 4c f4 ff    L..            ; Disable cursor editing (edit keys act as soft keys f11 to f15) (X=2)

; ***************************************************************************************
; &b0e0 referenced 2 times by &a0ff, &b041
.create_default_ruler
    sta tmp0                                                          ; b0e0: 85 85       ..
    sty tmp1                                                          ; b0e2: 84 86       ..
    lda #0                                                            ; b0e4: a9 00       ..
    tay                                                               ; b0e6: a8          .              ; Y=&00
; &b0e7 referenced 1 time by &b0f9
.loop_cb0e7
    lda #&2e ; '.'                                                    ; b0e7: a9 2e       ..
; &b0e9 referenced 1 time by &b0fd
.loop_cb0e9
    sta (tmp0),y                                                      ; b0e9: 91 85       ..
    iny                                                               ; b0eb: c8          .
    tya                                                               ; b0ec: 98          .
    tax                                                               ; b0ed: aa          .
    inx                                                               ; b0ee: e8          .
    clc                                                               ; b0ef: 18          .
    adc #6                                                            ; b0f0: 69 06       i.
    cmp screen_width                                                  ; b0f2: c5 36       .6
    beq cb0ff                                                         ; b0f4: f0 09       ..
    txa                                                               ; b0f6: 8a          .
    and #7                                                            ; b0f7: 29 07       ).
    bne loop_cb0e7                                                    ; b0f9: d0 ec       ..
    lda #&2a ; '*'                                                    ; b0fb: a9 2a       .*
    bne loop_cb0e9                                                    ; b0fd: d0 ea       ..             ; ALWAYS branch

; &b0ff referenced 1 time by &b0f4
.cb0ff
    lda #&3c ; '<'                                                    ; b0ff: a9 3c       .<
    sta (tmp0),y                                                      ; b101: 91 85       ..
    rts                                                               ; b103: 60          `

; &b104 referenced 1 time by &8ec4
.sub_cb104
    lda #0                                                            ; b104: a9 00       ..
    ldx #&33 ; '3'                                                    ; b106: a2 33       .3
; &b108 referenced 1 time by &b10c
.loop_cb108
    sta register_value_array,x                                        ; b108: 9d 98 07    ...
    dex                                                               ; b10b: ca          .
    bpl loop_cb108                                                    ; b10c: 10 fa       ..
    sta header_text_maybe                                             ; b10e: 8d d8 06    ...
    sta footer_text_maybe                                             ; b111: 8d 1a 07    ...
    sta two_sided_flag                                                ; b114: 85 2e       ..
    sta left_margin                                                   ; b116: 85 2f       ./
    sta line_spacing                                                  ; b118: 85 27       .'
    sta rhs_extra_margin                                              ; b11a: 85 2c       .,
    sta macro_executing_flag                                          ; b11c: 85 2d       .-
    lda #&42 ; 'B'                                                    ; b11e: a9 42       .B
    sta page_length                                                   ; b120: 85 26       .&
    lda #1                                                            ; b122: a9 01       ..
    sta footers_enabled_flag                                          ; b124: 85 28       .(
    sta headers_enabled_flag                                          ; b126: 85 29       .)
    sta l0038                                                         ; b128: 85 38       .8
    sta register_value_p                                              ; b12a: 8d b6 07    ...
    sta register_value_l                                              ; b12d: 8d ae 07    ...
    ldy #&80                                                          ; b130: a0 80       ..
    sty highlight1_code                                               ; b132: 84 2a       .*
    iny                                                               ; b134: c8          .              ; Y=&81
    sty highlight2_code                                               ; b135: 84 2b       .+
    lda #4                                                            ; b137: a9 04       ..
    sta top_margin                                                    ; b139: 85 22       ."
    sta bottom_margin                                                 ; b13b: 85 23       .#
    sta header_margin                                                 ; b13d: 85 24       .$
    sta footer_margin                                                 ; b13f: 85 25       .%
    jmp c92f0                                                         ; b141: 4c f0 92    L..

; &b144 referenced 2 times by &a6cf, &a6d6
.decimal_table
lb145 = decimal_table+1
    equw &2710, &03e8, &0064, &000a                                   ; b144: 10 27 e8... .'.
; &b145 referenced 2 times by &a6c6, &a6dd
; &b14c referenced 1 time by &9bae
.non_function_key_table
    equb &1b, &0d, &7f, 9, &ff                                        ; b14c: 1b 0d 7f... ...
; &b151 referenced 1 time by &a894
.jumptable_ptrs
lb152 = jumptable_ptrs+1
    equw jumptable1, jumptable4, jumptable2, jumptable3               ; b151: 59 b1 cf... Y..
; &b152 referenced 1 time by &a8a6
.jumptable1
    equw                  esc_key,             return_key             ; b159: 3b 82 7b... ;.{
    equw               delete_key,                tab_key             ; b15d: 00 9e 30... ..0
    equw      f0_format_block_key,     f1_top_of_text_key             ; b161: 3c a0 5e... <.^
    equw    f2_bottom_of_text_key,   f3_delete_to_eol_key             ; b165: 9c a0 dd... ...
    equw f4_beginning_of_line_key,     f5_end_of_line_key             ; b169: 92 9e 99... ...
    equw       f6_insert_line_key,     f7_delete_line_key             ; b16d: d8 9d 50... ..P
    equw       f8_insert_char_key,     f9_delete_char_key             ; b171: 20 9e 49...  .I
    equw                return_34,           f11_copy_key             ; b175: f9 a7 b7... ...
    equw             f12_left_key,          f13_right_key             ; b179: 54 9d 4b... T.K
    equw             f14_down_key,             f15_up_key             ; b17d: 74 9d 5b... t.[
    equw       sf0_move_block_key,      sf1_swap_case_key             ; b181: 94 a1 3c... ..<
    equw  sf2_release_margins_key, sf3_delete_to_char_key             ; b185: 86 9e 02... ...
    equw       sf4_highlight1_key,     sf5_highlight2_key             ; b189: 34 9e 38... 4.8
    equw     sf6_go_to_marker_key,     sf7_set_marker_key             ; b18d: 24 a0 12... $..
    equw     sf8_edit_command_key, sf9_delete_command_key             ; b191: e8 9e 62... ..b
    equw                return_34,          sf11_copy_key             ; b195: f9 a7 db... ...
    equw            sf12_left_key,         sf13_right_key             ; b199: 9d 9f e3... ...
    equw            sf14_down_key,            sf15_up_key             ; b19d: a9 a0 6b... ..k
    equw     cf0_delete_block_key,     cf1_next_match_key             ; b1a1: 77 a1 68... w.h
    equw      cf2_format_mode_key,   cf3_justify_mode_key             ; b1a5: 54 a1 65... T.e
    equw      cf4_insert_mode_key,  cf5_default_ruler_key             ; b1a9: 6e a1 f2... n..
    equw       cf6_split_line_key,     cf7_join_lines_key             ; b1ad: b1 9d a1... ...
    equw    cf8_mark_as_ruler_key,              return_34             ; b1b1: 3b 9f f9... ;..
    equw                return_34,              return_34             ; b1b5: f9 a7 f9... ...
    equw f4_beginning_of_line_key,     f5_end_of_line_key             ; b1b9: 92 9e 99... ...
    equw    f2_bottom_of_text_key,     f1_top_of_text_key             ; b1bd: 9c a0 5e... ..^
; overlapping: asl a                                                  ; b1c1: 0a          .
.jumptable2
    equw   quit_cmd,   new_cmd,     format_cmd,   setup_cmd           ; b1c1: 0a 85 97... ...
    equw   read_cmd,  more_cmd,     screen_cmd,  sheets_cmd           ; b1c9: 58 85 ae... X..
    equw   save_cmd, count_cmd,      field_cmd, printer_cmd           ; b1d1: 06 89 9e... ...
    equw search_cmd, clear_cmd, microspace_cmd,    fold_cmd           ; b1d9: 86 82 91... ...
    equw   name_cmd,  mode_cmd,     finish_cmd,   print_cmd           ; b1e1: e6 88 b6... ...
    equw change_cmd, write_cmd,       edit_cmd, replace_cmd           ; b1e9: a1 82 1d... ...
    equw   load_cmd                                                   ; b1f1: 77 88       w.
; overlapping: sta new_cmd                                            ; b1c2: 85 97       ..
.jumptable3
    equw sub_c8d1b, sub_c8d7f, c84a8                                  ; b1f3: 1b 8d 7f... ...
    equs "Mark Colton"                                                ; b1f9: 4d 61 72... Mar
    equb 0                                                            ; b204: 00          .
; 'QUIT' -> 0, flag=1
; &b205 referenced 3 times by &a859, &a86b, &a88b
.parser_table
    equb &0a, &0e, &12, &0f                                           ; b205: 0a 0e 12... ...
    equb &81                                                          ; b209: 81          .
; 'NEW' -> 1, flag=0
    equb &15, &1e, &0c                                                ; b20a: 15 1e 0c    ...
    equb &80                                                          ; b20d: 80          .
; 'FORMAT' -> 2, flag=1
    equb &1d, &14, 9, &36, &3a, &2f                                   ; b20e: 1d 14 09... ...
    equb &81                                                          ; b214: 81          .
; 'SETUP' -> 3, flag=0
    equb 8, &1e, &0f, &2e, &2b                                        ; b215: 08 1e 0f... ...
    equb &80                                                          ; b21a: 80          .
; 'READ' -> 4, flag=1
    equb 9, &1e, &3a, &3f                                             ; b21b: 09 1e 3a... ..:
    equb &81                                                          ; b21f: 81          .
; 'MORE' -> 5, flag=1
    equb &16, &14, &29, &3e                                           ; b220: 16 14 29... ..)
    equb &81                                                          ; b224: 81          .
; 'SCREEN' -> 6, flag=1
    equb 8, &18, &29, &3e, &3e, &35                                   ; b225: 08 18 29... ..)
    equb &81                                                          ; b22b: 81          .
; 'SHEETS' -> 7, flag=1
    equb 8, &13, &3e, &3e, &2f, &28                                   ; b22c: 08 13 3e... ..>
    equb &81                                                          ; b232: 81          .
; 'SAVE' -> 8, flag=1
    equb 8, &1a, &2d, &3e                                             ; b233: 08 1a 2d... ..-
    equb &81                                                          ; b237: 81          .
; 'COUNT' -> 9, flag=1
    equb &18, &14, &2e, &35, &2f                                      ; b238: 18 14 2e... ...
    equb &81                                                          ; b23d: 81          .
; 'FIELD' -> 10, flag=1
    equb &1d, &12, &3e, &37, &3f                                      ; b23e: 1d 12 3e... ..>
    equb &81                                                          ; b243: 81          .
; 'PRINTER' -> 11, flag=1
    equb &0b, 9, &12, &15, &0f, &1e, &29                              ; b244: 0b 09 12... ...
    equb &81                                                          ; b24b: 81          .
; 'SEARCH' -> 12, flag=1
    equb 8, &3e, &3a, &29, &38, &33                                   ; b24c: 08 3e 3a... .>:
    equb &81                                                          ; b252: 81          .
; 'CLEAR' -> 13, flag=1
    equb &18, &17, &3e, &3a, &29                                      ; b253: 18 17 3e... ..>
    equb &81                                                          ; b258: 81          .
; 'MICROSPACE' -> 14, flag=1
    equb &16, &12, &38, &29, &34, &28, &2b, &3a, &38, &3e             ; b259: 16 12 38... ..8
    equb &81                                                          ; b263: 81          .
; 'FOLD' -> 15, flag=1
    equb &1d, &14, &37, &3f                                           ; b264: 1d 14 37... ..7
    equb &81                                                          ; b268: 81          .
; 'NAME' -> 16, flag=1
    equb &15, &3a, &36, &3e                                           ; b269: 15 3a 36... .:6
    equb &81                                                          ; b26d: 81          .
; 'MODE' -> 17, flag=0
    equb &16, &34, &3f, &3e                                           ; b26e: 16 34 3f... .4?
    equb &80                                                          ; b272: 80          .
; 'FINISH' -> 18, flag=1
    equb &1d, &32, &35, &32, &28, &33                                 ; b273: 1d 32 35... .25
    equb &81                                                          ; b279: 81          .
; 'PRINT' -> 19, flag=1
    equb &0b, &29, &32, &35, &2f                                      ; b27a: 0b 29 32... .)2
    equb &81                                                          ; b27f: 81          .
; 'CHANGE' -> 20, flag=1
    equb &18, &33, &3a, &35, &3c, &3e                                 ; b280: 18 33 3a... .3:
    equb &81                                                          ; b286: 81          .
; 'WRITE' -> 21, flag=1
    equb &0c, &29, &32, &2f, &3e                                      ; b287: 0c 29 32... .)2
    equb &81                                                          ; b28c: 81          .
; 'EDIT' -> 22, flag=0
    equb &1e, &3f, &32, &2f                                           ; b28d: 1e 3f 32... .?2
    equb &80                                                          ; b291: 80          .
; 'REPLACE' -> 23, flag=1
    equb 9, &3e, &2b, &37, &3a, &38, &3e                              ; b292: 09 3e 2b... .>+
    equb &81                                                          ; b299: 81          .
; 'LOAD' -> 24, flag=0
    equb &17, &34, &3a, &3f                                           ; b29a: 17 34 3a... .4:
    equb &80, 0                                                       ; b29e: 80 00       ..
; &b2a0 referenced 2 times by &9740, &974f
.commands_table
lb2a1 = commands_table+1
    equs "CE"                                                         ; b2a0: 43 45       CE
; &b2a1 referenced 1 time by &9747
    equs "RJ"                                                         ; b2a2: 52 4a       RJ
    equs "DF"                                                         ; b2a4: 44 46       DF
    equs "DH"                                                         ; b2a6: 44 48       DH
    equs "DM"                                                         ; b2a8: 44 4d       DM
    equs "EM"                                                         ; b2aa: 45 4d       EM
    equs "SR"                                                         ; b2ac: 53 52       SR
    equs "PE"                                                         ; b2ae: 50 45       PE
    equs "TM"                                                         ; b2b0: 54 4d       TM
    equs "BM"                                                         ; b2b2: 42 4d       BM
    equs "PL"                                                         ; b2b4: 50 4c       PL
    equs "TS"                                                         ; b2b6: 54 53       TS
    equs "FO"                                                         ; b2b8: 46 4f       FO
    equs "HE"                                                         ; b2ba: 48 45       HE
    equs "HT"                                                         ; b2bc: 48 54       HT
    equs "HM"                                                         ; b2be: 48 4d       HM
    equs "FM"                                                         ; b2c0: 46 4d       FM
    equs "LM"                                                         ; b2c2: 4c 4d       LM
    equs "LS"                                                         ; b2c4: 4c 53       LS
    equs "OP"                                                         ; b2c6: 4f 50       OP
    equs "EP"                                                         ; b2c8: 45 50       EP
    equs "LJ"                                                         ; b2ca: 4c 4a       LJ
    equs "PB"                                                         ; b2cc: 50 42       PB
    equb &ff                                                          ; b2ce: ff          .
; overlapping: cmp lf894,y                                            ; b2cf: d9 94 f8    ...
.jumptable4
    equw ce_fmt_cmd, rj_fmt_cmd, df_fmt_cmd, dh_fmt_cmd, dm_fmt_cmd   ; b2cf: d9 94 f8... ...
    equw  return_34, em_fmt_cmd, pe_fmt_cmd, tm_fmt_cmd, bm_fmt_cmd   ; b2d9: f9 a7 ba... ...
    equw pl_fmt_cmd, ts_fmt_cmd, fo_fmt_cmd, he_fmt_cmd, ht_fmt_cmd   ; b2e3: dc 95 e4... ...
    equw hm_fmt_cmd, fm_fmt_cmd, lm_fmt_cmd, ls_fmt_cmd, op_fmt_cmd   ; b2ed: 03 96 0b... ...
    equw ep_fmt_cmd, lj_fmt_cmd, pb_fmt_cmd                           ; b2f7: 3c 96 d0... <..
; overlapping: sty df_fmt_cmd,x                                       ; b2d2: 94 71       .q
; overlapping: sta dh_fmt_cmd,x                                       ; b2d4: 95 6b       .k
; overlapping: sta dm_fmt_cmd,x                                       ; b2d6: 95 6d       .m
; overlapping: stx return_34,y                                        ; b2d8: 96 f9       ..

; ***************************************************************************************
; &b2fd referenced 1 time by &b36f
.get_next_bit_of_relocation_table
    txa                                                               ; b2fd: 8a          .
    bne cb30c                                                         ; b2fe: d0 0c       ..
    ldx #8                                                            ; b300: a2 08       ..
    lda (tmp6),y                                                      ; b302: b1 8b       ..
    sta l0083                                                         ; b304: 85 83       ..
    inc tmp6                                                          ; b306: e6 8b       ..
    bne cb30c                                                         ; b308: d0 02       ..
    inc tmp7                                                          ; b30a: e6 8c       ..
; &b30c referenced 2 times by &b2fe, &b308
.cb30c
    rol l0083                                                         ; b30c: 26 83       &.
    dex                                                               ; b30e: ca          .
; &b30f referenced 1 time by &b32b
.return_86
    rts                                                               ; b30f: 60          `

; ***************************************************************************************
; &b310 referenced 2 times by &b33e, &b347
.write_hex_to_output_buffer
    pha                                                               ; b310: 48          H
    lsr a                                                             ; b311: 4a          J
    lsr a                                                             ; b312: 4a          J
    lsr a                                                             ; b313: 4a          J
    lsr a                                                             ; b314: 4a          J
    jsr sub_cb31b                                                     ; b315: 20 1b b3     ..
    pla                                                               ; b318: 68          h
    and #&0f                                                          ; b319: 29 0f       ).
; &b31b referenced 1 time by &b315
.sub_cb31b
    cmp #&0a                                                          ; b31b: c9 0a       ..
    bcc cb321                                                         ; b31d: 90 02       ..
    adc #6                                                            ; b31f: 69 06       i.
; &b321 referenced 1 time by &b31d
.cb321
    adc #&30 ; '0'                                                    ; b321: 69 30       i0
    sta output_buffer,x                                               ; b323: 9d 54 06    .T.
    inx                                                               ; b326: e8          .
    rts                                                               ; b327: 60          `

; ***************************************************************************************
; &b328 referenced 1 time by &80c2
.create_go_command
    jsr detect_tube                                                   ; b328: 20 b0 af     ..
    bcc return_86                                                     ; b32b: 90 e2       ..
    ldx #&ff                                                          ; b32d: a2 ff       ..
; &b32f referenced 1 time by &b336
.loop_cb32f
    inx                                                               ; b32f: e8          .
    lda lb39a,x                                                       ; b330: bd 9a b3    ...
    sta output_buffer,x                                               ; b333: 9d 54 06    .T.
    bne loop_cb32f                                                    ; b336: d0 f7       ..
    lda #&c5                                                          ; b338: a9 c5       ..
    sta tmp9                                                          ; b33a: 85 8e       ..
    sta tmp1                                                          ; b33c: 85 86       ..
    jsr write_hex_to_output_buffer                                    ; b33e: 20 10 b3     ..
    lda #&c8                                                          ; b341: a9 c8       ..
    sta tmp8                                                          ; b343: 85 8d       ..
    sta tmp0                                                          ; b345: 85 85       ..
    jsr write_hex_to_output_buffer                                    ; b347: 20 10 b3     ..
    lda #&0d                                                          ; b34a: a9 0d       ..
    sta output_buffer,x                                               ; b34c: 9d 54 06    .T.
    ldx #<(sub_c80c5)                                                 ; b34f: a2 c5       ..
    ldy #>(sub_c80c5)                                                 ; b351: a0 80       ..
    stx tmp4                                                          ; b353: 86 89       ..
    sty tmp5                                                          ; b355: 84 8a       ..
    lda #<(relocation_table)                                          ; b357: a9 9f       ..
    sta tmp6                                                          ; b359: 85 8b       ..
    lda #>(relocation_table)                                          ; b35b: a9 b3       ..
    sta tmp7                                                          ; b35d: 85 8c       ..
    ldy #0                                                            ; b35f: a0 00       ..
    ldx #0                                                            ; b361: a2 00       ..
; &b363 referenced 2 times by &b38d, &b391
.cb363
    lda tmp5                                                          ; b363: a5 8a       ..
    cmp #>(get_next_bit_of_relocation_table)                          ; b365: c9 b2       ..
    bne cb36f                                                         ; b367: d0 06       ..
    lda tmp4                                                          ; b369: a5 89       ..
    cmp #<(get_next_bit_of_relocation_table)                          ; b36b: c9 fd       ..
    beq cb393                                                         ; b36d: f0 24       .$
; &b36f referenced 1 time by &b367
.cb36f
    jsr get_next_bit_of_relocation_table                              ; b36f: 20 fd b2     ..
    lda (tmp4),y                                                      ; b372: b1 89       ..
    bcc cb383                                                         ; b374: 90 0d       ..
    clc                                                               ; b376: 18          .
    adc #3                                                            ; b377: 69 03       i.
    pha                                                               ; b379: 48          H
    iny                                                               ; b37a: c8          .
    lda (tmp4),y                                                      ; b37b: b1 89       ..
    adc #&45 ; 'E'                                                    ; b37d: 69 45       iE
    sta (tmp4),y                                                      ; b37f: 91 89       ..
    pla                                                               ; b381: 68          h
    dey                                                               ; b382: 88          .
; &b383 referenced 1 time by &b374
.cb383
    sta (tmp0),y                                                      ; b383: 91 85       ..
    inc tmp4                                                          ; b385: e6 89       ..
    bne cb38b                                                         ; b387: d0 02       ..
    inc tmp5                                                          ; b389: e6 8a       ..
; &b38b referenced 1 time by &b387
.cb38b
    inc tmp0                                                          ; b38b: e6 85       ..
    bne cb363                                                         ; b38d: d0 d4       ..
    inc tmp1                                                          ; b38f: e6 86       ..
    bne cb363                                                         ; b391: d0 d0       ..
; &b393 referenced 1 time by &b36d
.cb393
    ldx #<(output_buffer)                                             ; b393: a2 54       .T
    ldy #>(output_buffer)                                             ; b395: a0 06       ..
    jmp oscli                                                         ; b397: 4c f7 ff    L..

; &b39a referenced 1 time by &b330
.lb39a
    equs "*GO ", 0                                                    ; b39a: 2a 47 4f... *GO
.relocation_table
    equb &41, 0, &20, 0                                               ; b39f: 41 00 20... A.
    equs "!) "                                                        ; b3a3: 21 29 20    !)
    equb 0,   4, &24, 4, &90, 0, 9, 4, 0, &40, 0, 1, 1, 0, &40,   0   ; b3a6: 00 04 24... ..$
    equb 0,   0, &80, 0,   0, 1, 0, 0, 1,   0, 0, 0, 0, 0,   0, &24   ; b3b6: 00 00 80... ...
    equb 0, &80,   0, 0,   0, 2                                       ; b3c6: 00 80 00... ...
    equs "BI!"                                                        ; b3cc: 42 49 21    BI!
    equb 0, 0, &48, 1, 0, 4, &24, 9                                   ; b3cf: 00 00 48... ..H
    equs "!!!$"                                                       ; b3d7: 21 21 21... !!!
    equb   8,   0,   4,   8, &40, &24, 0, 0, &10, 0, 2, 0, 0, &40, 0  ; b3db: 08 00 04... ...
    equb &42, &12, &40, &90,   0,   1                                 ; b3ea: 42 12 40... B.@
    equs "!! I "                                                      ; b3f0: 21 21 20... !!
    equb 0, 0, 0, 0, 0, &41, 0, &84, 2, 1, 0, &20, 0, 0, 0, 8         ; b3f5: 00 00 00... ...
    equs "@BA"                                                        ; b405: 40 42 41    @BA
    equb   0,   8, &10,   0, &84, &90, &42, &48,   0, &20,   8, &49   ; b408: 00 08 10... ...
    equb &24,   2, &12, &10,   0,   1, &20,   9, &24,   9,   0,   0   ; b414: 24 02 12... $..
    equb   2, &48,   1,   9, &24, &24, &81,   2,   0, &42, &49,   2   ; b420: 02 48 01... .H.
    equb &12, &40, &12,   0,   0,   9, &21,   0,   8, &12, &40, &80   ; b42c: 12 40 12... .@.
    equb   0,   0,   0,   0, &49, &20,   0, &20,   0,   0, &80,   4   ; b438: 00 00 00... ...
    equb &80,   9,   0,   0, &80, &10,   0,   0,   0,   0,   0,   4   ; b444: 80 09 00... ...
    equb   2,   8, &40,   4,   0,   0,   0, &80, &10,   1, &24,   0   ; b450: 02 08 40... ..@
    equb   0,   2,   0, &40, &84,   0,   0,   0, &80,   0,   0,   0   ; b45c: 00 02 00... ...
    equb   0,   0,   0,   0, &12,   0,   0,   0, &24,   2, &42,   0   ; b468: 00 00 00... ...
    equb &40,   0,   0,   0, &20, &92, &40, &20,   0, &80,   0, &81   ; b474: 40 00 00... @..
    equb   4,   0, &42, &10,   0,   0,   0,   0,   0,   0, &40,   0   ; b480: 04 00 42... ..B
    equb   0,   0,   4, &21,   0,   0,   2, &40,   0, &12, &49,   0   ; b48c: 00 00 04... ...
    equb   0,   0,   0,   0, &80,   0,   0,   1,   0,   0, &49, &24   ; b498: 00 00 00... ...
    equb   0,   0,   0, &20, &10,   0,   0,   0,   0,   0,   0, &24   ; b4a4: 00 00 00... ...
    equb   0,   0,   0,   0,   0,   0,   0,   4, &80, &40, &10, &10   ; b4b0: 00 00 00... ...
    equb   1,   0,   4,   0,   0,   1,   2, &48,   0,   0,   0,   1   ; b4bc: 01 00 04... ...
    equb   0,   0,   4,   0, &20,   0,   0,   4, &80,   0,   0,   0   ; b4c8: 00 00 04... ...
    equb   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, &80   ; b4d4: 00 00 00... ...
    equb   0, &10,   0, &10,   0,   0,   0, &80,   0,   0,   0,   0   ; b4e0: 00 10 00... ...
    equb   0,   0,   0,   0,   0, &40,   0,   0,   1,   0,   0,   0   ; b4ec: 00 00 00... ...
    equb   0,   0,   1,   0,   0, &10,   4,   0,   0,   0,   0,   0   ; b4f8: 00 00 01... ...
    equb   0, &80,   0,   0,   1,   0,   1,   0,   8,   0,   0,   0   ; b504: 00 80 00... ...
    equb   0,   0,   1,   0,   0,   0,   0,   0,   0, &10,   0, &20   ; b510: 00 00 01... ...
    equb   1,   0,   0,   0,   0, &20, &40,   0,   0, &10,   0,   0   ; b51c: 01 00 00... ...
    equb   0,   0,   0,   8, &10,   0,   0,   8,   0,   0,   1,   0   ; b528: 00 00 00... ...
    equb   0,   0,   0,   0,   0,   0,   0, &40,   1,   0,   0,   0   ; b534: 00 00 00... ...
    equb   0,   0,   0,   0,   0, &80,   0,   0, &10,   0, &10,   0   ; b540: 00 00 00... ...
    equb   0,   0,   0,   0,   0,   0, &49,   8,   0,   0,   0,   0   ; b54c: 00 00 00... ...
    equb   0,   0,   0,   0,   4,   0, &24, &80,   0,   0,   0,   0   ; b558: 00 00 00... ...
    equb   0, &24,   0,   2                                           ; b564: 00 24 00... .$.
    equs "@", '"', "H!"                                               ; b568: 40 22 48... @"H
    equb   0,   9,   0, &81,   0, &80,   0,   9, &21,   0,   0,   1   ; b56c: 00 09 00... ...
    equb   0,   0,   0,   0,   0, &20,   0,   0,   2, &40,   0,   4   ; b578: 00 00 00... ...
    equb &80,   0,   0,   1, &24, &80,   0,   4,   8,   0,   0,   0   ; b584: 80 00 00... ...
    equb &80,   0,   0,   8,   0,   0,   0,   0,   0,   0, &40,   0   ; b590: 80 00 00... ...
    equb   0,   0,   0,   0,   0,   0, &10,   0,   0,   0, &20,   0   ; b59c: 00 00 00... ...
    equb   0,   0, &20,   0, &10,   0, &90,   0,   0, &80, &48, &10   ; b5a8: 00 00 20... ..
    equb   0, &10, &10,   0,   0,   8,   0,   0,   0,   0,   0,   0   ; b5b4: 00 10 10... ...
    equb   0,   0,   0,   0,   0,   0, &80,   0,   8,   0,   0, &40   ; b5c0: 00 00 00... ...
    equb   0,   0,   0,   0,   0,   0,   8,   4, &20,   4, &84,   0   ; b5cc: 00 00 00... ...
    equb   0,   0,   4, &80,   0, &90, &40,   0, &84,   2,   0, &48   ; b5d8: 00 00 04... ...
    equb &40,   0,   0,   0,   0,   0, &81, &21, &24, &90,   2,   0   ; b5e4: 40 00 00... @..
    equb   2, &49,   9, &20,   0,   8,   0, &24,   1,   9, &24, &80   ; b5f0: 02 49 09... .I.
    equb   0,   0,   0,   0,   2,   0,   0,   0,   0, &12,   0, &10   ; b5fc: 00 00 00... ...
    equb   0,   0,   0,   2,   4,   0,   0,   0, &80,   0,   0,   8   ; b608: 00 00 00... ...
    equb   2,   0, &20, &40,   0, &90,   0,   0,   9, &24, &80,   0   ; b614: 02 00 20... ..
    equb   8,   4,   0,   0,   0,   8,   0,   0,   0,   0,   0,   0   ; b620: 08 04 00... ...
    equb   4,   0,   4,   0,   0,   0,   0, &48,   0,   0,   0,   0   ; b62c: 04 00 04... ...
    equb   0,   0, &20,   0, &90,   0, &82,   0, &10,   0             ; b638: 00 00 20... ..
    equs "  @@@@@@@@"                                                 ; b642: 20 20 40...   @
    equb   0, 0,   2,   4, &84,   1,   0, &40,   0,   0,   0, &80     ; b64c: 00 00 02... ...
    equb   0, 0,   0,   0,   2, &40,   0,   8,   2, &10,   0,   0     ; b658: 00 00 00... ...
    equb   0, 0,   0, &80,   0,   4,   0, &20,   0,   0,   8, &10     ; b664: 00 00 00... ...
    equb &10, 0, &82,   0,   0,   0,   0,   1, &10, &80, &20,   4     ; b670: 10 00 82... ...
    equb   0, 0,   8,   4, &12, &40,   0,   0,   0, &80,   0,   0     ; b67c: 00 00 08... ...
    equb &80, 0,   0,   0,   0,   0, &20,   0,   4,   0,   2,   0     ; b688: 80 00 00... ...
    equb   0, 0,   0,   0,   0, &80,   0,   0,   0,   0,   0,   0     ; b694: 00 00 00... ...
    equb   0, 0,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0     ; b6a0: 00 00 01... ...
    equb   0, 0,   4,   0,   0,   0,   0,   0,   0, &80,   0, &40     ; b6ac: 00 00 04... ...
    equb   0, 0, &10,   0,   0,   2,   0,   0,   2,   0,   0,   0     ; b6b8: 00 00 10... ...
    equb   0, 0,   0,   0,   0,   8,   0,   0,   0,   4,   0,   0     ; b6c4: 00 00 00... ...
    equb   0, 0, &40,   0,   0, &24, &90, &92, &40, &80,   0,   0     ; b6d0: 00 00 40... ..@
    equb   1, 0,   0,   0,   0,   4,   0,   0,   0,   0,   0,   0     ; b6dc: 01 00 00... ...
    equb   0, 0,   0,   0,   1,   0, &20, &80,   0,   4,   0,   0     ; b6e8: 00 00 00... ...
    equb   8, 8,   0,   0,   1, &20,   0,   0, &20,   0,   0, &92     ; b6f4: 08 08 00... ...
    equb &40, 8,   0, &10,   0,   0,   0, &80,   0, &20,   0,   8     ; b700: 40 08 00... @..
    equb   0, 0,   0, &40,   0,   0,   0, &80,   0,   0, &12,   0     ; b70c: 00 00 00... ...
    equb &12, 0,   0,   0,   0,   0,   0,   8, &40,   0,   0,   0     ; b718: 12 00 00... ...
    equb   0, 4,   0,   0,   0                                        ; b724: 00 04 00... ...
    equs "I @"                                                        ; b729: 49 20 40    I @
    equb   0,   0,   8,   0,   0,   1,   2,   0,   0, &81,   0,   2   ; b72c: 00 00 08... ...
    equb   0,   8,   4,   0,   4, &80,   0, &40,   0,   8,   0,   0   ; b738: 00 08 04... ...
    equb &10,   2, &40,   1,   0, &10,   0, &40,   0,   1,   4, &20   ; b744: 10 02 40... ..@
    equb &48,   0,   0, &24,   0, &20,   1,   2,   0,   1,   4,   0   ; b750: 48 00 00... H..
    equb &84,   0,   0,   1,   2, &12,   0, &49,   0,   2, &40,   0   ; b75c: 84 00 00... ...
    equb   0,   0,   0,   0,   0,   8,   0,   0,   0,   0, &10, &20   ; b768: 00 00 00... ...
    equb   0, &20,   8, &10,   0, &90,   4,   0, &40, &80,   1,   9   ; b774: 00 20 08... . .
    equb   0,   0, &80,   1,   0,   0, &80, &10,   2, &40, &24, &90   ; b780: 00 00 80... ...
    equb   2,   4, &80, &48,   0,   1,   2, &48,   0, &40,   8,   0   ; b78c: 02 04 80... ...
    equb   0,   0,   9, &20,   1,   2,   0,   0,   0,   1, &20,   0   ; b798: 00 00 09... ...
    equb &12, &48, &10, &21,   0,   0,   0,   0,   0,   0,   0,   0   ; b7a4: 12 48 10... .H.
    equb &24,   0,   0,   0,   0, &10, &80, &49, &24, &90, &80, &12   ; b7b0: 24 00 00... $..
    equb   4, &92, &10, &24,   0,   0,   0,   2,   0,   0,   2,   0   ; b7bc: 04 92 10... ...
    equb   0,   0,   0,   0,   0,   0,   0,   0, &40, &80,   0, &49   ; b7c8: 00 00 00... ...
    equb   9, &20,   0,   1,   0,   0,   0,   0,   2,   0,   0,   0   ; b7d4: 09 20 00... . .
    equb   2,   4,   0,   0,   0,   4,   0,   0, &48,   0,   0,   0   ; b7e0: 02 04 00... ...
    equb   0,   1,   0,   0, &20,   0,   2, &48,   0,   0,   0,   0   ; b7ec: 00 01 00... ...
    equb   2,   0,   0,   0,   0, &80,   1,   2,   0,   0,   0, &92   ; b7f8: 02 00 00... ...
    equb &40,   0, &81,   0,   0,   0, &90,   0,   0, &12,   0,   0   ; b804: 40 00 81... @..
    equb   0,   0,   0,   0, &80,   0,   0,   0,   2,   0, &40,   2   ; b810: 00 00 00... ...
    equb &40, &24, &92,   0, &20, &12,   0,   0,   0,   0,   8,   0   ; b81c: 40 24 92... @$.
    equb &10, &40,   0,   0, &10,   0,   0,   0,   0,   0,   0,   0   ; b828: 10 40 00... .@.
    equb   8,   0,   2, &10,   0,   0,   0,   0,   0,   0,   0,   0   ; b834: 08 00 02... ...
    equb   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   0,   1   ; b840: 00 00 00... ...
    equb   0,   0,   0,   0, &20, &40,   0,   0,   0,   0,   0,   0   ; b84c: 00 00 00... ...
    equb &24, &84,   0,   0,   0,   0,   0, &20, &10, &20, &40,   0   ; b858: 24 84 00... $..
    equb &80,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0   ; b864: 80 00 00... ...
    equb   0,   0,   0,   4,   0,   0,   0, &20,   0,   0,   0,   0   ; b870: 00 00 00... ...
    equb &92, &40,   2,   2, &40,   0,   0,   0,   0,   0,   0,   0   ; b87c: 92 40 02... .@.
    equb   0,   0,   0, &10,   2,   0,   0,   0,   0,   4,   0,   1   ; b888: 00 00 00... ...
    equb   0,   0,   0,   1,   0, &80,   4, &20,   0,   0,   0,   4   ; b894: 00 00 00... ...
    equb   0,   4, &80,   8,   0, &40,   0,   0,   0,   0, &80,   0   ; b8a0: 00 04 80... ...
    equb   0,   0, &80, &92,   4, &80,   0, &40, &20,   0,   9, &21   ; b8ac: 00 00 80... ...
    equb   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0   ; b8b8: 00 00 00... ...
    equb   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0   ; b8c4: 00 00 00... ...
    equb   0,   0,   0,   0,   0,   0,   0,   0,   0,   4, &80, &10   ; b8d0: 00 00 00... ...
    equb   8,   0,   0,   0,   0, &40,   0,   0,   0, &40,   0,   0   ; b8dc: 08 00 00... ...
    equb   0,   0, &21,   0,   0,   0,   0,   0,   0,   0,   0, &84   ; b8e8: 00 00 21... ..!
    equb   0,   0,   0, &40,   0,   0,   2,   0,   0,   0,   0,   0   ; b8f4: 00 00 00... ...
    equb   0,   0, &10,   0,   0, &20, &40,   0,   0,   0, &40,   0   ; b900: 00 00 10... ...
    equb &20,   0,   0, &84,   0,   0,   0,   0,   0,   8,   0, &40   ; b90c: 20 00 00...  ..
    equb   0,   0,   0,   0,   0,   0,   0, &80,   0,   1,   9,   0   ; b918: 00 00 00... ...
    equb   0,   0,   0,   0,   0,   2,   0,   1,   0,   0, &90, &10   ; b924: 00 00 00... ...
    equb   0, &80,   0,   4,   0,   0,   1,   0, &20,   0,   0, &10   ; b930: 00 80 00... ...
    equb   0,   0,   0,   0,   0,   0,   0,   2,   0,   0,   1,   0   ; b93c: 00 00 00... ...
    equb &20,   0,   0,   0,   0, &10,   0,   0, &10,   0,   0,   0   ; b948: 20 00 00...  ..
    equb   0,   8,   0,   0,   0, &40,   0,   0,   0,   0,   0,   0   ; b954: 00 08 00... ...
    equb   0, &20,   0, &80,   0,   2, &10,   8,   0,   0,   0,   2   ; b960: 00 20 00... . .
    equb   0,   0,   2,   1,   0, &10,   0,   0,   0,   0,   0,   0   ; b96c: 00 00 02... ...
    equb   0,   0,   0, &40,   0,   0,   0,   2,   0,   0,   0,   0   ; b978: 00 00 00... ...
    equb   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   0   ; b984: 00 00 00... ...
    equb   0, &90,   0,   0,   0,   0,   0,   0, &20,   1,   0,   0   ; b990: 00 90 00... ...
    equb   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0   ; b99c: 00 00 00... ...
    equb   0,   0,   0,   0,   0,   0,   4,   0, &0a, &aa, &aa, &aa   ; b9a8: 00 00 00... ...
    equb &aa, &aa, &aa, &aa, &aa, &aa, &aa, &aa, &aa, &aa, &aa, &aa   ; b9b4: aa aa aa... ...
    equb &aa, &aa, &aa, &aa, &aa, &a0,   0,   0,   0,   0,   0,   0   ; b9c0: aa aa aa... ...
    equb   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0   ; b9cc: 00 00 00... ...
    equb   0,   0,   0,   0,   0,   0,   0,   0, &2a, &aa, &aa, &aa   ; b9d8: 00 00 00... ...
    equb &aa, &aa, &ff, &ff, &ff, &ff, &ff, &ff, &ff, &ff, &ff, &ff   ; b9e4: aa aa ff... ...
    equb &ff, &ff, &ff, &ff, &ff, &ff, &ff, &ff, &ff, &ff, &ff, &ff   ; b9f0: ff ff ff... ...
    equb &ff, &ff, &ff, &ff, &20, &2f, &bb, &80,   3, &20, &e9, &bb   ; b9fc: ff ff ff... ...
    equb &a2, &34, &a0, &3c, &20, &cc, &d5, &a2, &38, &bd,   1,   3   ; ba08: a2 34 a0... .4.
    equb &8d, &43,   3, &bd,   0,   3, &aa, &8e, &42,   3, &20, &20   ; ba14: 8d 43 03... .C.
    equb &bd, &e8, &d0,   3, &ee, &43,   3, &8a, &d9,   0,   3, &ad   ; ba20: bd e8 d0... ...
    equb &43,   3, &f9,   1,   3, &30, &e8, &a2, &36, &a0, &3e, &20   ; ba2c: 43 03 f9... C..
    equb &cc, &d5, &a0, &3a, &b9,   1,   3, &8d, &43,   3, &b9,   0   ; ba38: cc d5 a0... ...
    equb   3, &a8, &8c, &42,   3, &20, &20, &bd, &98, &d0,   3, &ce   ; ba44: 03 a8 8c... ...
    equb &43,   3, &88, &18, &98, &fd,   0,   3, &ad, &43,   3, &fd   ; ba50: 43 03 88... C..
    equb   1,   3, &10, &e6, &ad, &2f,   3, &10, &a0, &80, &14, &20   ; ba5c: 01 03 10... ...
    equb &2f, &bb, &80,   3, &20, &e9, &bb, &a2, &3a, &a0, &38, &20   ; ba68: 2f bb 80... /..
    equb &87, &ba, &ad, &2f,   3, &10, &f1, &ee, &4d, &88, &d0,   3   ; ba74: 87 ba ad... ...
    equb &ee, &4e, &88, &a2, &3e, &a0, &3c, &da, &5a, &18, &ad, &14   ; ba80: ee 4e 88... .N.
    equb   3, &79,   0,   3, &8d, &40,   3, &ad, &15,   3, &79,   1   ; ba8c: 03 79 00... .y.
    equb   3, &8d, &41,   3, &18, &ad, &14,   3, &7d,   0,   3, &8d   ; ba98: 03 8d 41... ..A
    equb &44,   3, &ad, &15,   3, &7d,   1,   3, &8d, &45,   3, &18   ; baa4: 44 03 ad... D..
    equb &ad, &16,   3, &6d, &4d, &88, &a8, &ad, &17,   3, &6d, &4e   ; bab0: ad 16 03... ...
    equb &88, &20, &fe, &ba, &7a, &fa, &ad, &4d, &88, &0d, &4e, &88   ; babc: 88 20 fe... . .
    equb &f0, &47, &38, &ad, &14,   3, &fd,   0,   3, &8d, &40,   3   ; bac8: f0 47 38... .G8
    equb &ad, &15,   3, &fd,   1,   3, &8d, &41,   3, &38, &ad, &14   ; bad4: ad 15 03... ...
    equb   3, &f9,   0,   3, &8d, &44,   3, &ad, &15,   3, &f9,   1   ; bae0: 03 f9 00... ...
    equb   3, &8d, &45,   3, &38, &ad, &16,   3, &ed, &4d, &88, &a8   ; baec: 03 8d 45... ..E
    equb &ad, &17,   3, &ed, &4e, &88, &8c, &46,   3, &8c, &42,   3   ; baf8: ad 17 03... ...
    equb &8d, &47,   3, &8d, &43,   3, &a2, &44, &a0, &40, &4c, &e8   ; bb04: 8d 47 03... .G.
    equb &da                                                          ; bb10: da          .
    equs "`hh"                                                        ; bb11: 60 68 68    `hh
    equb &9c, &4d, &88, &9c, &4e, &88, &a2, &29, &a0, &40, &20, &fa   ; bb14: 9c 4d 88... .M.
    equb &bd, &a2, &29, &a0, &44, &20, &0c, &c9, &a2, &44, &a0, &40   ; bb20: bd a2 29... ..)
    equb &4c, &87, &ba, &a0, &24, &a2, &14, &a9                       ; bb2c: 4c 87 ba... L..
    equs ") x"                                                        ; bb34: 29 20 78    ) x
    equb &d6, &9c, &28, 3, &a0, &22, &a2, &16, &a9                    ; bb37: d6 9c 28... ..(
    equs ". x"                                                        ; bb40: 2e 20 78    . x
    equb &d6, &ad, &2e,   3, &0d, &2f, 3, &f0, &c6, &2a, &8d, &41     ; bb43: d6 ad 2e... ...
    equb &88, &a0, &20, &a2, &14, &a9                                 ; bb4f: 88 a0 20... ..
    equs ", x"                                                        ; bb55: 2c 20 78    , x
    equb &d6, &9c, &2b, 3                                             ; bb58: d6 9c 2b... ..+
    equs "*MA"                                                        ; bb5c: 2a 4d 41    *MA
    equb &88, &29,   1, &8d, &41, &88, &a2, &28, &a0, &2e, &20, &83   ; bb5f: 88 29 01... .).
    equb &bd, &a2, &2b, &a0, &2e, &20, &83, &bd, &ad, &41, &88, &f0   ; bb6b: bd a2 2b... ..+
    equb &0e, &38, &a0, &fd, &a9,   0, &f9, &2e,   2, &99, &2e,   2   ; bb77: 0e 38 a0... .8.
    equb &c8, &d0, &f5, &ad, &2e,   3, &8d, &3c, &88, &ad, &2f,   3   ; bb83: c8 d0 f5... ...
    equb &20, &ce, &d4, &a0,   3, &b9, &40, &88, &99, &30,   3, &88   ; bb8f: 20 ce d4...  ..
    equb &10, &f7, &a2, &0a, &9e, &42, &88, &ca, &10, &fa, &ee, &45   ; bb9b: 10 f7 a2... ...
    equb &88, &20, &18, &bc, &20, &18, &bc, &9c, &4d, &88, &9c, &4e   ; bba7: 88 20 18... . .
    equb &88, &a2, &3c, &a0, &36, &20, &fa, &bd, &a2, &3e, &a0, &34   ; bbb3: 88 a2 3c... ..<
    equb &20, &fa, &bd, &a2, &3c, &a0, &3a, &20, &d6, &d5, &10,   7   ; bbbf: 20 fa bd...  ..
    equb &20, &0c, &c9, &a2, &36, &80, &12, &a2, &38, &a0, &3e, &20   ; bbcb: 20 0c c9...  ..
    equb &d6, &d5, &10, &3c, &a2, &34, &a0, &3a, &20, &0c, &c9, &a2   ; bbd7: d6 d5 10... ...
    equb &3e, &a0, &38, &4c, &0c, &c9, &20, &18, &bc, &a0, &3a, &a2   ; bbe3: 3e a0 38... >.8
    equb &3c, &20, &d6, &d5, &10, &0d, &ad, &3c,   3, &8d, &3a,   3   ; bbef: 3c 20 d6... < .
    equb &ad, &3d,   3, &8d, &3b,   3, &60, &a2, &38, &a0, &3e, &20   ; bbfb: ad 3d 03... .=.
    equb &d6, &d5, &10, &0c, &ad, &3e,   3, &8d, &38,   3, &ad, &3f   ; bc07: d6 d5 10... ...
    equb   3, &8d, &39,   3, &60, &a2, &38, &a0, &34, &20, &1e, &c9   ; bc13: 03 8d 39... ..9
    equb &a2, &3c, &a0, &38, &20, &1e, &c9, &38, &ad, &30,   3, &ed   ; bc1f: a2 3c a0... .<.
    equb &49, &88, &8d, &36, &88, &ad, &31,   3, &ed, &4a, &88, &8d   ; bc2b: 49 88 8d... I..
    equb &37, &88, &ad, &32,   3, &ed, &4b, &88, &8d, &38, &88, &ad   ; bc37: 37 88 ad... 7..
    equb &33,   3, &ed, &4c, &88, &8d, &39, &88, &9c, &35, &88, &9c   ; bc43: 33 03 ed... 3..
    equb &34, &88, &20, &4b, &be, &ad, &28,   3, &8d, &34, &88, &ad   ; bc4f: 34 88 20... 4.
    equb &29,   3, &8d, &35, &88, &ad, &2a,   3, &8d, &36, &88, &20   ; bc5b: 29 03 8d... )..
    equb &0c, &be, &18, &ad, &42, &88, &6d, &35, &88,   8, &ad, &43   ; bc67: 0c be 18... ...
    equb &88, &6d, &36, &88, &8d, &3e,   3, &ad, &44, &88, &6d, &37   ; bc73: 88 6d 36... .m6
    equb &88, &8d, &3f,   3, &28, &10,   8, &ee, &3e,   3, &d0,   3   ; bc7f: 88 8d 3f... ..?
    equb &ee, &3f,   3, &38, &ad, &42, &88, &ed, &35, &88,   8, &ad   ; bc8b: ee 3f 03... .?.
    equb &43, &88, &ed, &36, &88, &8d, &3c,   3, &ad, &44, &88, &ed   ; bc97: 43 88 ed... C..
    equb &37, &88, &8d, &3d,   3, &28, &10,   8, &ee, &3c,   3, &d0   ; bca3: 37 88 8d... 7..
    equb   3, &ee, &3d,   3, &18, &ad, &45, &88, &6d, &49, &88, &8d   ; bcaf: 03 ee 3d... ..=
    equb &49, &88, &ad, &46, &88, &6d, &4a, &88, &8d, &4a, &88, &ad   ; bcbb: 49 88 ad... I..
    equb &47, &88, &6d, &4b, &88, &8d, &4b, &88, &ad, &48, &88, &6d   ; bcc7: 47 88 6d... G.m
    equb &4c, &88, &8d, &4c, &88, &18, &a9,   2, &6d, &45, &88, &8d   ; bcd3: 4c 88 8d... L..
    equb &45, &88, &90, &0d, &ee, &46, &88, &d0,   8, &ee, &47, &88   ; bcdf: 45 88 90... E..
    equb &d0,   3, &ee, &48, &88, &18, &ad, &42, &88, &6d, &2b,   3   ; bceb: d0 03 ee... ...
    equb &8d, &42, &88, &ad, &43, &88, &6d, &2c,   3, &8d, &43, &88   ; bcf7: 8d 42 88... .B.
    equb &ad, &44, &88, &6d, &2d,   3, &8d, &44, &88, &ee, &4d, &88   ; bd03: ad 44 88... .D.
    equb &d0,   3, &ee, &4e, &88, &ad, &2e,   3, &d0,   3, &ce, &2f   ; bd0f: d0 03 ee... ...
    equb   3, &ce, &2e,   3, &60, &da, &5a, &18, &ad, &14,   3, &6d   ; bd1b: 03 ce 2e... ...
    equb &42,   3, &8d, &44,   3, &ad, &15,   3, &6d, &43,   3, &8d   ; bd27: 42 03 8d... B..
    equb &45,   3, &18, &ad, &16,   3, &6d, &4d, &88, &8d, &46,   3   ; bd33: 45 03 18... E..
    equb &ad, &17,   3, &6d, &4e, &88, &8d, &47,   3, &a2             ; bd3f: ad 17 03... ...
    equs "D L"                                                        ; bd49: 44 20 4c    D L
    equb &db, &ad, &4d, &88, &0d, &4e, &88, &f0, &2b, &38, &ad, &14   ; bd4c: db ad 4d... ..M
    equb   3, &ed, &42,   3, &8d, &44,   3, &ad, &15,   3, &ed, &43   ; bd58: 03 ed 42... ..B
    equb   3, &8d, &45,   3, &38, &ad, &16,   3, &ed, &4d, &88, &8d   ; bd64: 03 8d 45... ..E
    equb &46,   3, &ad, &17,   3, &ed, &4e, &88, &8d, &47,   3, &a2   ; bd70: 46 03 ad... F..
    equs "D L"                                                        ; bd7c: 44 20 4c    D L
    equb &db, &7a, &fa, &60, &da, &b9,   0,   3, &8d, &3f, &88, &b9   ; bd7f: db 7a fa... .z.
    equb   1,   3, &8d, &40, &88, &a0, &18, &bd,   0,   3, &8d, &3a   ; bd8b: 01 03 8d... ...
    equb &88, &bd,   1,   3, &8d, &3b, &88, &bd,   2,   3, &30, &0c   ; bd97: 88 bd 01... ...
    equb &88, &f0, &52, &0e, &3a, &88, &2e, &3b, &88, &2a, &10, &f4   ; bda3: 88 f0 52... ..R
    equb &8d, &3c, &88, &9c, &3d, &88, &9c, &3e, &88, &18, &2e, &3a   ; bdaf: 8d 3c 88... .<.
    equb &88, &2e, &3b, &88, &2e, &3c, &88, &2e, &3d, &88, &2e, &3e   ; bdbb: 88 2e 3b... ..;
    equb &88, &38, &ad, &3d, &88, &ed, &3f, &88, &aa, &ad, &3e, &88   ; bdc7: 88 38 ad... .8.
    equb &ed, &40, &88, &90,   6, &8e, &3d, &88, &8d, &3e, &88, &88   ; bdd3: ed 40 88... .@.
    equb &d0, &d8, &fa, &ad, &3a, &88, &2a, &9d,   0,   3, &ad, &3b   ; bddf: d0 d8 fa... ...
    equb &88, &2a, &9d,   1,   3, &ad, &3c, &88, &2a, &9d,   2,   3   ; bdeb: 88 2a 9d... .*.
    equb &60, &fa, &60, &38, &a9,   0, &fd,   0,   3, &99,   0,   3   ; bdf7: 60 fa 60... `.`
    equb &a9,   0, &fd,   1,   3, &99,   1,   3, &60, &a0, &17, &9c   ; be03: a9 00 fd... ...
    equb &39, &88, &9c, &38, &88, &9c, &37, &88, &4e, &36, &88, &6e   ; be0f: 39 88 9c... 9..
    equb &35, &88, &6e, &34, &88, &90, &1c, &18, &ad, &30, &88, &6d   ; be1b: 35 88 6e... 5.n
    equb &37, &88, &8d, &37, &88, &ad, &31, &88, &6d, &38, &88, &8d   ; be27: 37 88 8d... 7..
    equb &38, &88, &ad, &32, &88, &6d, &39, &88, &8d, &39, &88, &18   ; be33: 38 88 ad... 8..
    equb &a2,   5, &7e, &34, &88, &ca, &10, &fa, &88, &10, &d6, &60   ; be3f: a2 05 7e... ..~
    equb &a2,   3, &9e, &30, &88, &74, &dc, &ca, &10, &f8, &a0,   5   ; be4b: a2 03 9e... ...
    equb &b9, &34, &88, &85, &da, &5a, &a0,   3                       ; be57: b9 34 88... .4.
    equs "Z8.0"                                                       ; be5f: 5a 38 2e... Z8.
    equb &88, &2e, &31, &88, &2e, &32, &88, &2e, &33, &88, &a2,   1   ; be63: 88 2e 31... ..1
    equb &a5, &dc,   6, &da, &2a, &26, &dd, &26, &de, &26, &df, &ca   ; be6f: a5 dc 06... ...
    equb &10, &f4, &85, &dc, &38, &ed, &30, &88, &48, &a5, &dd, &ed   ; be7b: 10 f4 85... ...
    equb &31, &88, &aa, &a5, &de, &ed, &32, &88, &a8, &a5, &df, &ed   ; be87: 31 88 aa... 1..
    equb &33, &88, &90, &0e, &85, &df, &84, &de, &86, &dd, &68, &85   ; be93: 33 88 90... 3..
    equb &dc, &ee, &30, &88, &80,   4, &68, &ce, &30, &88, &7a, &88   ; be9f: dc ee 30... ..0
    equb &10, &b2, &7a, &88, &10, &a6, &4e, &33, &88, &6e, &32, &88   ; beab: 10 b2 7a... ..z
    equb &6e, &31, &88, &6e, &30, &88, &60, &29,   2, &8d, &45,   3   ; beb7: 6e 31 88... n1.
    equb &20, &51, &c9, &a2, &14, &20, &e6, &c8, &a0, &34, &20, &16   ; bec3: 20 51 c9...  Q.
    equb &c9, &84, &da, &a2, &34, &a0, &2c, &a9, &28, &20, &80, &d5   ; becf: c9 84 da... ...
    equb &a2, &28, &a0, &34, &20, &cc, &d5, &5a, &da, &a0,   0, &20   ; bedb: a2 28 a0... .(.
    equb &b7, &d1, &f0,   8, &4a, &f0,   3                            ; bee7: b7 d1 f0... ...
    equs "hh`"                                                        ; beee: 68 68 60    hh`
    equb &a2,   0, &68, &a0, &30, &84, &da, &a0, &28, &20, &8d, &d5   ; bef1: a2 00 68... ..h
    equb &a0, &3c, &84, &da, &a0, &34, &20, &8d, &d5, &68, &18, &69   ; befd: a0 3c 84... .<.
    equb   4, &aa, &da, &a0,   0, &20, &b7, &d1, &f0,   5, &4a, &f0   ; bf09: 04 aa da... ...
    equb &d9, &a2,   4, &68, &a0, &40, &84, &da, &a0, &38, &20, &8d   ; bf15: d9 a2 04... ...
    equb &d5, &ad, &40,   3, &cd, &3c,   3, &ad, &41,   3, &ed, &3d   ; bf21: d5 ad 40... ..@
    equb   3, &10, &10, &ad, &45,   3, &d0,   3, &20, &20, &c4, &a2   ; bf2d: 03 10 10... ...
    equb &34, &20,   2, &c9, &4c, &20, &c4, &9c, &47,   3, &ad, &30   ; bf39: 34 20 02... 4 .
    equb   3, &2d, &61,   3, &85, &da, &ad, &3c,   3, &2d, &61,   3   ; bf45: 03 2d 61... .-a
    equb &38, &e5, &da, &10,   6, &ce, &47,   3, &2d, &61,   3, &8d   ; bf51: 38 e5 da... 8..
    equb &43,   3, &48, &49, &ff, &1a, &2d, &61,   3, &8d, &42,   3   ; bf5d: 43 03 48... C.H
    equb &68, &18, &6d, &61,   3, &aa, &bd, &20, &e1, &85, &e1, &a2   ; bf69: 68 18 6d... h.m
    equb &3c, &a0, &40, &20, &9c, &da, &8d, &44,   3, &a5, &d1, &8d   ; bf75: 3c a0 40... <.@
    equb &46,   3, &a5, &dc, &85, &e0, &a2,   0, &20, &e6, &bf, &f0   ; bf81: 46 03 a5... F..
    equb &40, &ad, &2a,   3, &cd, &36,   3, &ad, &2b,   3, &ed, &37   ; bf8d: 40 ad 2a... @.*
    equb   3, &50,   2, &49, &80, &30, &11, &20, &62, &db, &a2,   0   ; bf99: 03 50 02... .P.
    equb &20, &d1, &bf, &a2, &0c, &20, &d1, &bf, &d0, &f1, &80, &1d   ; bfa5: 20 d1 bf...  ..
    equb &a2, &2a, &a0, &2e, &20, &b2, &e2, &a2, &36, &a0, &3a, &20   ; bfb1: a2 2a a0... .*.
    equb &b2, &e2, &20, &62, &db, &a2,   0, &20, &db, &bf, &a2, &0c   ; bfbd: b2 e2 20... ..
    equb &20, &db, &bf, &d0, &f1, &4c, &62, &db, &fe, &2a,   3, &d0   ; bfc9: 20 db bf...  ..
    equb &10, &fe, &2b,   3, &80, &0b, &bd, &2a,   3, &d0,   3, &de   ; bfd5: 10 fe 2b... ..+
    equb &2b,   3, &de, &2a,   3, &bd, &2a,   3, &dd, &2e,   3, &d0   ; bfe1: 2b 03 de... +..
    equb   6, &bd, &2b,   3, &dd, &2f,   3, &60, &ff, &ff, &ff, &ff   ; bfed: 06 bd 2b... ..+
    equb &ff, &ff, &ff, &ff, &ff, &ff,   0                            ; bff9: ff ff ff... ...
.pydis_end


save pydis_start, pydis_end

; Label references by decreasing frequency:
;     tmp8:                                109
;     tmp9:                                 81
;     tmp0:                                 75
;     l0083:                                74
;     l0084:                                71
;     input_buffer_ptr+1:                   61
;     tmp6:                                 61
;     xpos:                                 61
;     l0082:                                54
;     ptr4:                                 52
;     ptr4+0:                               52
;     l0081:                                50
;     tmp1:                                 47
;     l0039:                                46
;     tmp4:                                 44
;     current_edit_line_ptr:                43
;     current_edit_line_ptr+0:              43
;     tmp7:                                 43
;     ptr4+1:                               41
;     current_format_line_ptr:              38
;     current_format_line_ptr+0:            38
;     oswrch:                               36
;     print_inline_string:                  36
;     tmp2:                                 33
;     tmp5:                                 32
;     l0046:                                24
;     ca93c:                                23
;     ruler_left_stop:                      23
;     tmp3:                                 22
;     format_mode_flag:                     21
;     osbyte:                               21
;     output_buffer:                        19
;     area_start_ptr:                       18
;     area_start_ptr+0:                     18
;     area_start_ptr+1:                     18
;     top:                                  18
;     top+0:                                18
;     l0048:                                16
;     l0074:                                16
;     osnewl:                               16
;     top+1:                                16
;     current_edit_line_ptr+1:              15
;     input_buffer_ptr:                     15
;     input_buffer_ptr+0:                   15
;     l006e:                                15
;     last_macro_ptr:                       15
;     last_macro_ptr+0:                     15
;     ptr1:                                 15
;     ptr1+0:                               15
;     area_end_ptr:                         14
;     area_end_ptr+0:                       14
;     area_end_ptr+1:                       14
;     cli_loop:                             14
;     l003c:                                14
;     l0042:                                14
;     l007d:                                14
;     markers_array:                        14
;     markers_array+0:                      14
;     print_flags:                          14
;     print_xpos:                           14
;     sub_caf71:                            14
;     cabcb:                                13
;     current_format_line_ptr+1:            13
;     l0043:                                13
;     l0045:                                13
;     l006f:                                13
;     line_lengths:                         13
;     markers_array+1:                      13
;     page:                                 13
;     page+0:                               13
;     ptr1+1:                               13
;     ruler_right_stop:                     13
;     evaluate_expression_from_fmt_cmd:     12
;     get_line_length:                      12
;     l0038:                                12
;     l0047:                                12
;     l0070:                                12
;     l0072:                                12
;     l0073:                                12
;     page+1:                               12
;     screen_height:                        12
;     input_buffer:                         11
;     l0012:                                11
;     l0079:                                11
;     sub_ca5ab:                            11
;     l007e:                                10
;     oshwm:                                10
;     oshwm+0:                              10
;     c8b7b:                                 9
;     exit_from_service_call:                9
;     l0011:                                 9
;     l0044:                                 9
;     l006d:                                 9
;     l0075:                                 9
;     l007a:                                 9
;     microspacing_flag:                     9
;     rw_file_handle:                        9
;     screen_width:                          9
;     stop_printing:                         9
;     ca741:                                 8
;     current_ruler_ptr:                     8
;     current_ruler_ptr+0:                   8
;     is_uppercase:                          8
;     l0031:                                 8
;     l003d:                                 8
;     l0065:                                 8
;     l0066:                                 8
;     l0076:                                 8
;     last_macro_ptr+1:                      8
;     osasci:                                8
;     ptr2:                                  8
;     ptr2+0:                                8
;     ptr3:                                  8
;     ptr3+0:                                8
;     return_55:                             8
;     sub_c8e33:                             8
;     sub_ca536:                             8
;     top_margin:                            8
;     beep:                                  7
;     bottom_margin:                         7
;     c9b2f:                                 7
;     error_handling_mode:                   7
;     l0021:                                 7
;     l0033:                                 7
;     parse_marks_from_command:              7
;     ptr5:                                  7
;     ptr5+0:                                7
;     ptr6:                                  7
;     ptr6+0:                                7
;     ptr6+1:                                7
;     sanitise_area:                         7
;     sub_caa15:                             7
;     sub_cab1a:                             7
;     sub_cab37:                             7
;     c8c95:                                 6
;     ca1c9:                                 6
;     ca30d:                                 6
;     cac20:                                 6
;     check_for_bad_document:                6
;     current_screen_mode:                   6
;     filename:                              6
;     get_current_fmt_cmd_byte:              6
;     insert_mode_flag:                      6
;     is_tube_flag:                          6
;     l003a:                                 6
;     l003b:                                 6
;     l0041:                                 6
;     l0063:                                 6
;     l0064:                                 6
;     l0071:                                 6
;     macro_executing_flag:                  6
;     print_vertical_space:                  6
;     printer_driver_name:                   6
;     register_value_p:                      6
;     register_value_p+0:                    6
;     return_20:                             6
;     return_47:                             6
;     return_58:                             6
;     set_cursor_position:                   6
;     sub_ca600:                             6
;     sub_ca9b0:                             6
;     ypos:                                  6
;     c82fa:                                 5
;     c853f:                                 5
;     c8b11:                                 5
;     c8c23:                                 5
;     c8cc8:                                 5
;     c9048:                                 5
;     c930d:                                 5
;     c937b:                                 5
;     c950f:                                 5
;     c9c7f:                                 5
;     c9e94:                                 5
;     cab29:                                 5
;     cab91:                                 5
;     cac78:                                 5
;     cb05a:                                 5
;     editor_loop:                           5
;     header_text_maybe:                     5
;     himem:                                 5
;     himem+0:                               5
;     himem+1:                               5
;     justifying_flag:                       5
;     l0034:                                 5
;     l0049:                                 5
;     l004a:                                 5
;     open_file:                             5
;     page_eject_fmt:                        5
;     ptr2+1:                                5
;     ptr3+1:                                5
;     ptr5+1:                                5
;     sub_c8df4:                             5
;     sub_c8e1f:                             5
;     sub_c8e49:                             5
;     sub_ca276:                             5
;     sub_ca608:                             5
;     sub_caa97:                             5
;     acknowledge_escape:                    4
;     c84ab:                                 4
;     c85df:                                 4
;     c87d1:                                 4
;     c8b64:                                 4
;     c9355:                                 4
;     c9426:                                 4
;     c9821:                                 4
;     c9974:                                 4
;     c9a8d:                                 4
;     c9b73:                                 4
;     ca151:                                 4
;     ca4e9:                                 4
;     ca941:                                 4
;     call_printer_driver:                   4
;     call_through_jumptable:                4
;     cb07a:                                 4
;     compute_bytes_free:                    4
;     edit_command_loop:                     4
;     folding_flag:                          4
;     footer_margin:                         4
;     header_margin:                         4
;     l0032:                                 4
;     l050a:                                 4
;     l050b:                                 4
;     line_spacing:                          4
;     lookup_marker:                         4
;     page_length:                           4
;     parse_boolean_from_fmt_cmd:            4
;     read_char:                             4
;     render_number_to_screen:               4
;     return_22:                             4
;     rom_workspace_array:                   4
;     set_inverted_text_if_not_mode_7:       4
;     set_normal_text_if_not_mode_7:         4
;     sub_c89d3:                             4
;     sub_c9431:                             4
;     sub_ca5ae:                             4
;     sub_cab0f:                             4
;     sub_cab6e:                             4
;     sub_caec2:                             4
;     two_sided_flag:                        4
;     c80f3:                                 3
;     c82e7:                                 3
;     c869b:                                 3
;     c870d:                                 3
;     c88ac:                                 3
;     c8a84:                                 3
;     c8b47:                                 3
;     c8b78:                                 3
;     c8bdb:                                 3
;     c8c8d:                                 3
;     c8f1a:                                 3
;     c8fe6:                                 3
;     c90f8:                                 3
;     c91f5:                                 3
;     c9209:                                 3
;     c9223:                                 3
;     c9263:                                 3
;     c985c:                                 3
;     c995c:                                 3
;     c9a58:                                 3
;     c9b06:                                 3
;     c9b31:                                 3
;     c9b86:                                 3
;     c9c1d:                                 3
;     c9c43:                                 3
;     ca12a:                                 3
;     ca514:                                 3
;     ca5d1:                                 3
;     ca5d5:                                 3
;     ca9e7:                                 3
;     ca9f1:                                 3
;     cabdf:                                 3
;     cae03:                                 3
;     cae64:                                 3
;     caed4:                                 3
;     caf55:                                 3
;     caf5c:                                 3
;     clear_cmd:                             3
;     current_ruler_buffer:                  3
;     current_ruler_ptr+1:                   3
;     cursor_off:                            3
;     cursor_on:                             3
;     do_osfile_with_block:                  3
;     document_filename:                     3
;     document_initialisation_canary:        3
;     draw_prompt_characters:                3
;     expand_line:                           3
;     flush_and_read_char:                   3
;     footers_enabled_flag:                  3
;     headers_enabled_flag:                  3
;     highlight1_code:                       3
;     home_cursor:                           3
;     initialise_document:                   3
;     l0030:                                 3
;     l0102:                                 3
;     l050c:                                 3
;     l050d:                                 3
;     l97b0:                                 3
;     left_margin:                           3
;     os_text_ptr:                           3
;     parse_integer_from_command:            3
;     parser_table:                          3
;     print_char:                            3
;     printer_driver_ptr:                    3
;     printer_driver_ptr+0:                  3
;     register_value_l:                      3
;     register_value_l+0:                    3
;     reset_area_to_marks_1_2:               3
;     return_2:                              3
;     return_36:                             3
;     return_44:                             3
;     return_46:                             3
;     return_56:                             3
;     return_6:                              3
;     return_76:                             3
;     return_8:                              3
;     rhs_extra_margin:                      3
;     run_cli:                               3
;     set_text_colour:                       3
;     sub_c845e:                             3
;     sub_c8535:                             3
;     sub_c8e54:                             3
;     sub_c902c:                             3
;     sub_c93b6:                             3
;     sub_c93c8:                             3
;     sub_c93fd:                             3
;     sub_c95b2:                             3
;     sub_ca486:                             3
;     sub_cae06:                             3
;     sub_caf5f:                             3
;     sub_caf6f:                             3
;     zp_initialisation_canary:              3
;     another_filename:                      2
;     c816d:                                 2
;     c827c:                                 2
;     c837d:                                 2
;     c83ca:                                 2
;     c83da:                                 2
;     c850d:                                 2
;     c85ec:                                 2
;     c867d:                                 2
;     c86b8:                                 2
;     c8703:                                 2
;     c8715:                                 2
;     c871f:                                 2
;     c876d:                                 2
;     c8787:                                 2
;     c87b4:                                 2
;     c8801:                                 2
;     c88af:                                 2
;     c8977:                                 2
;     c8a86:                                 2
;     c8aa3:                                 2
;     c8ada:                                 2
;     c8b6a:                                 2
;     c8c33:                                 2
;     c8c3e:                                 2
;     c8c7a:                                 2
;     c8cdb:                                 2
;     c8d39:                                 2
;     c8d5d:                                 2
;     c8e0f:                                 2
;     c8e25:                                 2
;     c8e5d:                                 2
;     c8f30:                                 2
;     c8f3b:                                 2
;     c8f6b:                                 2
;     c8fce:                                 2
;     c8ffb:                                 2
;     c900e:                                 2
;     c9064:                                 2
;     c90a0:                                 2
;     c90b6:                                 2
;     c9115:                                 2
;     c91a7:                                 2
;     c9225:                                 2
;     c92d4:                                 2
;     c92f0:                                 2
;     c93b8:                                 2
;     c93ce:                                 2
;     c93e6:                                 2
;     c9415:                                 2
;     c951c:                                 2
;     c9529:                                 2
;     c9537:                                 2
;     c953e:                                 2
;     c96a2:                                 2
;     c96b8:                                 2
;     c96f8:                                 2
;     c9788:                                 2
;     c97ae:                                 2
;     c9804:                                 2
;     c9847:                                 2
;     c9967:                                 2
;     c99c9:                                 2
;     c99e4:                                 2
;     c9a21:                                 2
;     c9a2e:                                 2
;     c9a40:                                 2
;     c9a60:                                 2
;     c9aa5:                                 2
;     c9ae9:                                 2
;     c9aef:                                 2
;     c9b6a:                                 2
;     c9b84:                                 2
;     c9b96:                                 2
;     c9bca:                                 2
;     c9bf2:                                 2
;     c9c00:                                 2
;     c9c48:                                 2
;     c9c67:                                 2
;     c9d9b:                                 2
;     c9de3:                                 2
;     c9e3a:                                 2
;     c9e9b:                                 2
;     c9eda:                                 2
;     c9f80:                                 2
;     c9fab:                                 2
;     ca00f:                                 2
;     ca07c:                                 2
;     ca0ba:                                 2
;     ca1ea:                                 2
;     ca219:                                 2
;     ca28e:                                 2
;     ca2dc:                                 2
;     ca2e6:                                 2
;     ca2f1:                                 2
;     ca313:                                 2
;     ca35e:                                 2
;     ca3b2:                                 2
;     ca3de:                                 2
;     ca461:                                 2
;     ca4b4:                                 2
;     ca50e:                                 2
;     ca532:                                 2
;     ca58c:                                 2
;     ca5f8:                                 2
;     ca666:                                 2
;     ca681:                                 2
;     ca684:                                 2
;     ca6e5:                                 2
;     ca6fe:                                 2
;     ca739:                                 2
;     ca753:                                 2
;     ca82e:                                 2
;     ca84c:                                 2
;     ca890:                                 2
;     ca8ed:                                 2
;     ca8f8:                                 2
;     ca919:                                 2
;     caa51:                                 2
;     caa82:                                 2
;     caad5:                                 2
;     cab75:                                 2
;     cabf6:                                 2
;     cabf9:                                 2
;     cac17:                                 2
;     cac3e:                                 2
;     cac5c:                                 2
;     cac8f:                                 2
;     cad45:                                 2
;     cada3:                                 2
;     caded:                                 2
;     cae35:                                 2
;     cae78:                                 2
;     caeb7:                                 2
;     caf2a:                                 2
;     caf31:                                 2
;     cb30c:                                 2
;     cb363:                                 2
;     cf8_mark_as_ruler_key:                 2
;     clear_marks_1_2:                       2
;     clear_screen:                          2
;     close_file:                            2
;     close_file_indirect:                   2
;     commands_table:                        2
;     complete_CRTC_10_write:                2
;     create_default_ruler:                  2
;     current_line_buffer:                   2
;     current_tab_key:                       2
;     decimal_table:                         2
;     detect_tube:                           2
;     display_nl_then_no_text:               2
;     edit_input_file_handle:                2
;     edit_output_file_handle:               2
;     enter_editor_mode:                     2
;     error_if_cassette_filesystem:          2
;     esc_key:                               2
;     expand_escaped_string:                 2
;     f13_right_key:                         2
;     f6_insert_line_key:                    2
;     find_margins_of_current_ruler:         2
;     finished_editing_command:              2
;     first_macro_ptr:                       2
;     first_macro_ptr+0:                     2
;     first_macro_ptr+1:                     2
;     get_next_fmt_cmd_byte:                 2
;     get_register_address:                  2
;     initialise_document_if_document_bad:   2
;     l0067:                                 2
;     l0068:                                 2
;     l0103:                                 2
;     l0502:                                 2
;     l0503:                                 2
;     l0504:                                 2
;     l0505:                                 2
;     l0506:                                 2
;     lb145:                                 2
;     line_lengths+1:                        2
;     lookup_formatting_command:             2
;     oscli:                                 2
;     osfind:                                2
;     oshwm+1:                               2
;     osword:                                2
;     parse_mark_from_command:               2
;     prepare_printer_driver:                2
;     print_newline:                         2
;     print_x_words_of_help:                 2
;     printer_driver_ptr+1:                  2
;     register_value_l+1:                    2
;     register_value_p+1:                    2
;     render_date_time_to_output_buffer:     2
;     render_header_or_footer:               2
;     render_new_page:                       2
;     render_register:                       2
;     reset_area_to_entire_document:         2
;     return_11:                             2
;     return_14:                             2
;     return_17:                             2
;     return_25:                             2
;     return_28:                             2
;     return_37:                             2
;     return_38:                             2
;     return_42:                             2
;     return_54:                             2
;     return_66:                             2
;     return_67:                             2
;     return_68:                             2
;     return_71:                             2
;     return_83:                             2
;     return_85:                             2
;     run_editor:                            2
;     setup_CRTC_10_write:                   2
;     sub_c8310:                             2
;     sub_c8361:                             2
;     sub_c83f0:                             2
;     sub_c8412:                             2
;     sub_c8849:                             2
;     sub_c89e5:                             2
;     sub_c8a4f:                             2
;     sub_c8c5f:                             2
;     sub_c8c63:                             2
;     sub_c8c7c:                             2
;     sub_c8cfe:                             2
;     sub_c8d24:                             2
;     sub_c8d48:                             2
;     sub_c8ebe:                             2
;     sub_c9173:                             2
;     sub_c9228:                             2
;     sub_c9241:                             2
;     sub_c9393:                             2
;     sub_c93a1:                             2
;     sub_c93be:                             2
;     sub_c9407:                             2
;     sub_c941a:                             2
;     sub_c9445:                             2
;     sub_c9830:                             2
;     sub_c9936:                             2
;     sub_c9977:                             2
;     sub_c9aa9:                             2
;     sub_ca1cc:                             2
;     sub_ca4d7:                             2
;     sub_ca597:                             2
;     sub_ca8b9:                             2
;     sub_ca94a:                             2
;     sub_caa8f:                             2
;     sub_cac41:                             2
;     sub_cacd7:                             2
;     sub_cad47:                             2
;     sub_cadf0:                             2
;     sub_caed6:                             2
;     sub_caef4:                             2
;     sub_caf33:                             2
;     system_init:                           2
;     test_for_cassette_filesystem:          2
;     word_command_str:                      2
;     write_hex_to_output_buffer:            2
;     add_macro_to_linked_list:              1
;     brk_handler_ptr:                       1
;     brkv:                                  1
;     c805a:                                 1
;     c80aa:                                 1
;     c80b1:                                 1
;     c811f:                                 1
;     c8139:                                 1
;     c8163:                                 1
;     c81a7:                                 1
;     c81b3:                                 1
;     c81b6:                                 1
;     c81ba:                                 1
;     c81db:                                 1
;     c81e0:                                 1
;     c81e7:                                 1
;     c81f3:                                 1
;     c8238:                                 1
;     c8255:                                 1
;     c8263:                                 1
;     c826e:                                 1
;     c82b9:                                 1
;     c830d:                                 1
;     c832d:                                 1
;     c8349:                                 1
;     c8356:                                 1
;     c836b:                                 1
;     c8389:                                 1
;     c8390:                                 1
;     c8398:                                 1
;     c83a3:                                 1
;     c83c8:                                 1
;     c83d1:                                 1
;     c8402:                                 1
;     c8410:                                 1
;     c8459:                                 1
;     c84a8:                                 1
;     c84e8:                                 1
;     c8584:                                 1
;     c8598:                                 1
;     c85b0:                                 1
;     c85dc:                                 1
;     c8608:                                 1
;     c8617:                                 1
;     c8649:                                 1
;     c8669:                                 1
;     c8672:                                 1
;     c8681:                                 1
;     c8699:                                 1
;     c86d1:                                 1
;     c86db:                                 1
;     c86df:                                 1
;     c86ea:                                 1
;     c86ff:                                 1
;     c871d:                                 1
;     c878b:                                 1
;     c8791:                                 1
;     c87b2:                                 1
;     c87cb:                                 1
;     c882f:                                 1
;     c8834:                                 1
;     c8862:                                 1
;     c8912:                                 1
;     c891f:                                 1
;     c896b:                                 1
;     c89b3:                                 1
;     c89c1:                                 1
;     c8a07:                                 1
;     c8a19:                                 1
;     c8a21:                                 1
;     c8a40:                                 1
;     c8a4c:                                 1
;     c8a5b:                                 1
;     c8a6c:                                 1
;     c8a87:                                 1
;     c8aca:                                 1
;     c8af3:                                 1
;     c8b0d:                                 1
;     c8b1f:                                 1
;     c8b38:                                 1
;     c8b4d:                                 1
;     c8b6b:                                 1
;     c8b91:                                 1
;     c8b9f:                                 1
;     c8bb7:                                 1
;     c8bbc:                                 1
;     c8bd7:                                 1
;     c8bdf:                                 1
;     c8be3:                                 1
;     c8bf2:                                 1
;     c8bf7:                                 1
;     c8c30:                                 1
;     c8caf:                                 1
;     c8cf1:                                 1
;     c8cf2:                                 1
;     c8cfa:                                 1
;     c8d0a:                                 1
;     c8d6c:                                 1
;     c8daf:                                 1
;     c8dce:                                 1
;     c8edb:                                 1
;     c8f0a:                                 1
;     c8f0d:                                 1
;     c8f29:                                 1
;     c8f6e:                                 1
;     c8f7a:                                 1
;     c8f92:                                 1
;     c8fb9:                                 1
;     c8fd5:                                 1
;     c9009:                                 1
;     c9034:                                 1
;     c906b:                                 1
;     c906f:                                 1
;     c9087:                                 1
;     c908a:                                 1
;     c908c:                                 1
;     c9090:                                 1
;     c9092:                                 1
;     c90e2:                                 1
;     c9101:                                 1
;     c912b:                                 1
;     c913b:                                 1
;     c9142:                                 1
;     c9154:                                 1
;     c915b:                                 1
;     c9160:                                 1
;     c9163:                                 1
;     c9177:                                 1
;     c9184:                                 1
;     c91a3:                                 1
;     c91c2:                                 1
;     c91cc:                                 1
;     c91d0:                                 1
;     c91da:                                 1
;     c921b:                                 1
;     c9231:                                 1
;     c923c:                                 1
;     c9254:                                 1
;     c925a:                                 1
;     c9260:                                 1
;     c927c:                                 1
;     c9284:                                 1
;     c928c:                                 1
;     c92cc:                                 1
;     c92cf:                                 1
;     c92e8:                                 1
;     c932e:                                 1
;     c9363:                                 1
;     c93a7:                                 1
;     c93aa:                                 1
;     c93d9:                                 1
;     c93f2:                                 1
;     c943c:                                 1
;     c9453:                                 1
;     c9462:                                 1
;     c9468:                                 1
;     c9472:                                 1
;     c947e:                                 1
;     c9488:                                 1
;     c949e:                                 1
;     c94c0:                                 1
;     c94c7:                                 1
;     c94cb:                                 1
;     c94cd:                                 1
;     c9548:                                 1
;     c9555:                                 1
;     c955e:                                 1
;     c9575:                                 1
;     c9598:                                 1
;     c959c:                                 1
;     c959e:                                 1
;     c95aa:                                 1
;     c9642:                                 1
;     c964c:                                 1
;     c968d:                                 1
;     c968f:                                 1
;     c96ce:                                 1
;     c9716:                                 1
;     c9719:                                 1
;     c9725:                                 1
;     c974c:                                 1
;     c977f:                                 1
;     c9783:                                 1
;     c97c0:                                 1
;     c97d5:                                 1
;     c97dc:                                 1
;     c97f7:                                 1
;     c981c:                                 1
;     c9861:                                 1
;     c986d:                                 1
;     c9871:                                 1
;     c988c:                                 1
;     c98b2:                                 1
;     c98b5:                                 1
;     c98bd:                                 1
;     c98d3:                                 1
;     c98d9:                                 1
;     c98f6:                                 1
;     c98fa:                                 1
;     c9912:                                 1
;     c991c:                                 1
;     c9920:                                 1
;     c9922:                                 1
;     c994a:                                 1
;     c9969:                                 1
;     c998a:                                 1
;     c99b6:                                 1
;     c99c7:                                 1
;     c99e0:                                 1
;     c99ee:                                 1
;     c9a0a:                                 1
;     c9a11:                                 1
;     c9a38:                                 1
;     c9a5d:                                 1
;     c9a87:                                 1
;     c9aa4:                                 1
;     c9ad5:                                 1
;     c9b1a:                                 1
;     c9b20:                                 1
;     c9b23:                                 1
;     c9b44:                                 1
;     c9b8f:                                 1
;     c9b9f:                                 1
;     c9bbb:                                 1
;     c9c09:                                 1
;     c9c14:                                 1
;     c9c31:                                 1
;     c9c4a:                                 1
;     c9c56:                                 1
;     c9c82:                                 1
;     c9c9d:                                 1
;     c9ca2:                                 1
;     c9cb9:                                 1
;     c9cd0:                                 1
;     c9cdb:                                 1
;     c9cf2:                                 1
;     c9cf5:                                 1
;     c9d0d:                                 1
;     c9d15:                                 1
;     c9d28:                                 1
;     c9d30:                                 1
;     c9d98:                                 1
;     c9dbd:                                 1
;     c9dcd:                                 1
;     c9dfd:                                 1
;     c9e81:                                 1
;     c9f5f:                                 1
;     c9fc3:                                 1
;     ca035:                                 1
;     ca051:                                 1
;     ca05b:                                 1
;     ca093:                                 1
;     ca097:                                 1
;     ca0c8:                                 1
;     ca0d2:                                 1
;     ca0d6:                                 1
;     ca0ef:                                 1
;     ca11a:                                 1
;     ca122:                                 1
;     ca15e:                                 1
;     ca1da:                                 1
;     ca1e6:                                 1
;     ca223:                                 1
;     ca229:                                 1
;     ca24d:                                 1
;     ca265:                                 1
;     ca273:                                 1
;     ca29c:                                 1
;     ca2b2:                                 1
;     ca2e0:                                 1
;     ca2f9:                                 1
;     ca307:                                 1
;     ca348:                                 1
;     ca351:                                 1
;     ca360:                                 1
;     ca381:                                 1
;     ca38a:                                 1
;     ca395:                                 1
;     ca3c1:                                 1
;     ca3d8:                                 1
;     ca3e7:                                 1
;     ca3ff:                                 1
;     ca406:                                 1
;     ca422:                                 1
;     ca479:                                 1
;     ca4bc:                                 1
;     ca4f4:                                 1
;     ca522:                                 1
;     ca523:                                 1
;     ca529:                                 1
;     ca533:                                 1
;     ca550:                                 1
;     ca587:                                 1
;     ca5cf:                                 1
;     ca5d9:                                 1
;     ca5e1:                                 1
;     ca5f1:                                 1
;     ca5fa:                                 1
;     ca624:                                 1
;     ca63b:                                 1
;     ca63d:                                 1
;     ca672:                                 1
;     ca6ae:                                 1
;     ca6c2:                                 1
;     ca6d4:                                 1
;     ca6ec:                                 1
;     ca6f1:                                 1
;     ca705:                                 1
;     ca72b:                                 1
;     ca74f:                                 1
;     ca7a5:                                 1
;     ca80f:                                 1
;     ca81f:                                 1
;     ca824:                                 1
;     ca832:                                 1
;     ca87e:                                 1
;     ca887:                                 1
;     ca8a1:                                 1
;     ca8a4:                                 1
;     ca8df:                                 1
;     ca90a:                                 1
;     ca911:                                 1
;     ca92f:                                 1
;     ca93a:                                 1
;     ca965:                                 1
;     ca96e:                                 1
;     ca97c:                                 1
;     ca9c3:                                 1
;     ca9d1:                                 1
;     ca9db:                                 1
;     caa08:                                 1
;     caa32:                                 1
;     caa46:                                 1
;     caa57:                                 1
;     caa65:                                 1
;     caa75:                                 1
;     caab0:                                 1
;     caab7:                                 1
;     caac8:                                 1
;     caae8:                                 1
;     cab06:                                 1
;     cab3f:                                 1
;     cab4b:                                 1
;     cab58:                                 1
;     cab64:                                 1
;     cab6c:                                 1
;     cabad:                                 1
;     cabb3:                                 1
;     cabbc:                                 1
;     cac0b:                                 1
;     cac1d:                                 1
;     cac4c:                                 1
;     cac58:                                 1
;     cac6f:                                 1
;     cac7b:                                 1
;     cac8d:                                 1
;     cac9a:                                 1
;     cac9c:                                 1
;     cacad:                                 1
;     cad5c:                                 1
;     cad5d:                                 1
;     cada2:                                 1
;     cadd1:                                 1
;     cade7:                                 1
;     cadea:                                 1
;     cadff:                                 1
;     cae27:                                 1
;     cae4b:                                 1
;     cae4d:                                 1
;     cae52:                                 1
;     cae5c:                                 1
;     cae91:                                 1
;     cae93:                                 1
;     cae98:                                 1
;     caef0:                                 1
;     caf19:                                 1
;     caf28:                                 1
;     caf60:                                 1
;     caf91:                                 1
;     cafdc:                                 1
;     cafee:                                 1
;     caffe:                                 1
;     cb06c:                                 1
;     cb0ff:                                 1
;     cb321:                                 1
;     cb36f:                                 1
;     cb383:                                 1
;     cb38b:                                 1
;     cb393:                                 1
;     claim_private_workspace_handler:       1
;     create_go_command:                     1
;     default_printer_driver_ptr:            1
;     detect_mode_7:                         1
;     display_no_text:                       1
;     display_status_word:                   1
;     enter_nonprintable_character:          1
;     enter_printable_character:             1
;     escape_flag:                           1
;     escaped_char_table:                    1
;     execute_formatting_command:            1
;     f4_beginning_of_line_key:              1
;     f9_delete_char_key:                    1
;     footer_text_maybe:                     1
;     get_byte_from_file:                    1
;     get_next_bit_of_relocation_table:      1
;     get_next_macro_in_linked_list:         1
;     help_handler:                          1
;     highlight2_code:                       1
;     input_line_not_escaped:                1
;     input_line_not_oscli:                  1
;     jumptable_ptrs:                        1
;     l00ef:                                 1
;     l00f0:                                 1
;     l00f1:                                 1
;     l00fd:                                 1
;     l0101:                                 1
;     l0501:                                 1
;     l050e:                                 1
;     l050f:                                 1
;     l0510:                                 1
;     l0511:                                 1
;     l80f2:                                 1
;     l83e0:                                 1
;     l8747:                                 1
;     l8748:                                 1
;     l8749:                                 1
;     l94b2:                                 1
;     l97b1:                                 1
;     la69a:                                 1
;     la69b:                                 1
;     la83d:                                 1
;     la8a5:                                 1
;     la995:                                 1
;     lada6:                                 1
;     language_handler:                      1
;     lb152:                                 1
;     lb2a1:                                 1
;     lb39a:                                 1
;     lookup_macro_name:                     1
;     loop_c803d:                            1
;     loop_c8051:                            1
;     loop_c819a:                            1
;     loop_c82b3:                            1
;     loop_c83b8:                            1
;     loop_c8490:                            1
;     loop_c84c4:                            1
;     loop_c84ee:                            1
;     loop_c8652:                            1
;     loop_c8674:                            1
;     loop_c86c2:                            1
;     loop_c8822:                            1
;     loop_c88fa:                            1
;     loop_c8914:                            1
;     loop_c89fa:                            1
;     loop_c8a15:                            1
;     loop_c8a36:                            1
;     loop_c8a74:                            1
;     loop_c8ae4:                            1
;     loop_c8c2a:                            1
;     loop_c8dfb:                            1
;     loop_c8e3b:                            1
;     loop_c8f5d:                            1
;     loop_c9107:                            1
;     loop_c91b2:                            1
;     loop_c91f1:                            1
;     loop_c9247:                            1
;     loop_c9381:                            1
;     loop_c942a:                            1
;     loop_c944c:                            1
;     loop_c9516:                            1
;     loop_c9589:                            1
;     loop_c973e:                            1
;     loop_c979d:                            1
;     loop_c98a2:                            1
;     loop_c98ec:                            1
;     loop_c992c:                            1
;     loop_c99ba:                            1
;     loop_c9a62:                            1
;     loop_c9bae:                            1
;     loop_c9cf9:                            1
;     loop_c9ff8:                            1
;     loop_ca003:                            1
;     loop_ca0e7:                            1
;     loop_ca132:                            1
;     loop_ca13d:                            1
;     loop_ca2c7:                            1
;     loop_ca31f:                            1
;     loop_ca3c3:                            1
;     loop_ca431:                            1
;     loop_ca465:                            1
;     loop_ca4bf:                            1
;     loop_ca4c2:                            1
;     loop_ca544:                            1
;     loop_ca5a2:                            1
;     loop_ca5e5:                            1
;     loop_ca615:                            1
;     loop_ca629:                            1
;     loop_ca6c4:                            1
;     loop_ca792:                            1
;     loop_ca80b:                            1
;     loop_ca851:                            1
;     loop_ca86a:                            1
;     loop_ca91c:                            1
;     loop_ca962:                            1
;     loop_ca976:                            1
;     loop_ca983:                            1
;     loop_ca9f7:                            1
;     loop_ca9f9:                            1
;     loop_caa38:                            1
;     loop_caabd:                            1
;     loop_caafb:                            1
;     loop_cab13:                            1
;     loop_cab2b:                            1
;     loop_cab4d:                            1
;     loop_caba5:                            1
;     loop_caced:                            1
;     loop_cad12:                            1
;     loop_cadd5:                            1
;     loop_cadf4:                            1
;     loop_cae37:                            1
;     loop_caea5:                            1
;     loop_caec8:                            1
;     loop_caf3f:                            1
;     loop_caf4a:                            1
;     loop_cafe9:                            1
;     loop_cb095:                            1
;     loop_cb0a8:                            1
;     loop_cb0e7:                            1
;     loop_cb0e9:                            1
;     loop_cb108:                            1
;     loop_cb32f:                            1
;     nested_subroutine_error:               1
;     non_function_key_table:                1
;     osargs:                                1
;     osbget:                                1
;     osbput:                                1
;     osbyte_handler:                        1
;     osfile:                                1
;     osrdch:                                1
;     parse_command:                         1
;     print_char_just_to_printer:            1
;     print_to_screen:                       1
;     put_byte_to_file:                      1
;     register_value_array:                  1
;     render_number_to_callback:             1
;     render_number_to_output_buffer:        1
;     restore_cursor_position:               1
;     return_1:                              1
;     return_10:                             1
;     return_12:                             1
;     return_13:                             1
;     return_15:                             1
;     return_16:                             1
;     return_18:                             1
;     return_19:                             1
;     return_21:                             1
;     return_23:                             1
;     return_24:                             1
;     return_26:                             1
;     return_27:                             1
;     return_29:                             1
;     return_3:                              1
;     return_30:                             1
;     return_31:                             1
;     return_32:                             1
;     return_33:                             1
;     return_34:                             1
;     return_35:                             1
;     return_39:                             1
;     return_4:                              1
;     return_40:                             1
;     return_41:                             1
;     return_43:                             1
;     return_45:                             1
;     return_48:                             1
;     return_49:                             1
;     return_5:                              1
;     return_50:                             1
;     return_51:                             1
;     return_52:                             1
;     return_53:                             1
;     return_57:                             1
;     return_59:                             1
;     return_60:                             1
;     return_61:                             1
;     return_62:                             1
;     return_63:                             1
;     return_64:                             1
;     return_65:                             1
;     return_69:                             1
;     return_7:                              1
;     return_70:                             1
;     return_72:                             1
;     return_73:                             1
;     return_74:                             1
;     return_75:                             1
;     return_77:                             1
;     return_78:                             1
;     return_79:                             1
;     return_80:                             1
;     return_81:                             1
;     return_82:                             1
;     return_84:                             1
;     return_86:                             1
;     return_9:                              1
;     return_key:                            1
;     save_cursor_position:                  1
;     service_handler:                       1
;     sub_c8371:                             1
;     sub_c88f4:                             1
;     sub_c88f8:                             1
;     sub_c8956:                             1
;     sub_c8c51:                             1
;     sub_c8c53:                             1
;     sub_c8d00:                             1
;     sub_c8d28:                             1
;     sub_c8d51:                             1
;     sub_c8d9a:                             1
;     sub_c8da2:                             1
;     sub_c8e2d:                             1
;     sub_c916a:                             1
;     sub_c9188:                             1
;     sub_c939b:                             1
;     sub_c976c:                             1
;     sub_c9ac1:                             1
;     sub_c9de1:                             1
;     sub_c9e22:                             1
;     sub_ca071:                             1
;     sub_ca0af:                             1
;     sub_ca44e:                             1
;     sub_ca4dd:                             1
;     sub_ca651:                             1
;     sub_ca6f9:                             1
;     sub_caacb:                             1
;     sub_cab8b:                             1
;     sub_cabc4:                             1
;     sub_cac50:                             1
;     sub_caedd:                             1
;     sub_cb104:                             1
;     sub_cb31b:                             1

; Stats:
;     Total size (Code + Data) = 16384 bytes
;     Code                     = 12242 bytes (75%)
;     Data                     = 4142 bytes (25%)
;
;     Number of instructions   = 6104
;     Number of data bytes     = 3332 bytes
;     Number of data words     = 228 bytes
;     Number of string bytes   = 582 bytes
;     Number of strings        = 94
