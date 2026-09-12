#!/usr/bin/env python3
"""
Extract 17 shared functions from view.c into document.c.

Also:
  - Moves JMP_CLI/JMP_EDITOR defines and jmp_buf env to globals.h
  - Moves struct printer_driver to globals.h
  - Moves inline helpers (bit, rol, ror, asr) to globals.h
  - Removes duplicate inline helpers from view.c
  - Adds missing extern declarations to globals.h
  - Updates printing.c includes
"""

import os

SRC_DIR = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'src')

FUNCS_TO_MOVE = [
    ('return_to_cli_prompt', 893, 896, True),
    ('stop_printing', 1406, 1433, True),
    ('display_document_file_state', 2631, 2698, True),
    ('sub_c9445', 5706, 5732, True),
    ('print_char', 5738, 5763, True),
    ('print_char_just_to_screen', 5763, 5799, True),
    ('call_printer_driver', 5821, 5842, True),
    ('process_document_character', 9748, 9854, True),
    ('check_for_control_code', 9862, 9879, False),
    ('read_char', 10121, 10149, True),
    ('sub_cab6e', 11314, 11327, True),
    ('find_margins_of_current_ruler_buffer', 11407, 11456, True),
    ('sub_cabc4', 11456, 11470, True),
    ('sub_cadf0', 12023, 12052, True),
    ('deref_and_check_for_command_prefix', 12518, 12526, True),
    ('check_for_command_prefix', 12526, 12538, False),
    ('compute_bytes_free', 12549, 12572, True),
]

STATIC_FWD_PATTERNS = [
    'static void call_printer_driver(',
    'static void compute_bytes_free(',
    'static void deref_and_check_for_command_prefix(',
    'static void display_document_file_state(',
    'static void find_margins_of_current_ruler_buffer(',
    'static void print_char_just_to_screen(',
    'static void process_document_character(',
    'static void read_char(',
    'static void return_to_cli_prompt(',
    'static void stop_printing(',
    'static void sub_c9445(',
    'static void sub_cab6e(',
    'static void sub_cabc4(',
    'static void sub_cadf0(',
    'static uint8_t deref_and_check_for_command_prefix(',
]

# 'print_char' forward decl is tricky - it's 'static void print_char(void);'
# Use a more specific pattern to avoid matching print_char_x_times
PRINT_CHAR_FWD = ['static void print_char(void)']

# Full function signature prefixes for static inline helpers to remove (now in globals.h)
INLINE_PATTERNS = [
    'static inline void set_flags(',
    'static inline void sbc(',
    'static inline void cmp(',
    'static inline void bit(',
    'static inline uint8_t rol(',
    'static inline uint8_t ror(',
    'static inline uint8_t asr(',
    'static inline void adc(',
]


def read_file(path):
    with open(path) as f:
        return f.readlines()

def write_file(path, lines):
    with open(path, 'w') as f:
        f.writelines(lines)


def extract_and_remove_funcs(lines, funcs):
    extracted = {}
    for name, start, end, is_static in sorted(funcs, key=lambda x: -x[1]):
        body = lines[start-1:end-1]
        if is_static:
            first = body[0]
            if first.strip().startswith('static '):
                body[0] = first.replace('static ', '', 1)
        extracted[name] = body
        lines = lines[:start-1] + lines[end-1:]
    return lines, extracted


def remove_lines_starting_with(lines, patterns, exact=False):
    """Remove any line whose stripped content starts with one of the given patterns.
    If exact=True, patterns must match the entire stripped line exactly."""
    result = []
    n = 0
    for line in lines:
        s = line.strip()
        matched = False
        for p in patterns:
            if exact:
                if s == p:
                    matched = True
                    break
            else:
                if s.startswith(p):
                    matched = True
                    break
        if matched:
            n += 1
        else:
            result.append(line)
    return result, n


def remove_inline_functions(lines, func_names):
    """Remove complete static inline function definitions (signature through closing brace).
    `func_names` is a list of function name prefixes to match (e.g. 'static inline void set_flags')."""
    result = []
    skip_depth = 0
    skipping = False
    n = 0
    for line in lines:
        stripped = line.strip()
        if not skipping:
            if any(stripped.startswith(p) for p in func_names):
                skipping = True
                n += 1
                # Count opening brace on this line
                skip_depth = stripped.count('{') - stripped.count('}')
            if not skipping:
                result.append(line)
        else:
            # Inside the function to skip
            skip_depth += stripped.count('{') - stripped.count('}')
            if skip_depth <= 0:
                skipping = False
    return result, n


def remove_struct_printer_driver(lines):
    for i, line in enumerate(lines):
        if line.strip().startswith('struct printer_driver {'):
            for j in range(i, min(i + 20, len(lines))):
                if lines[j].strip() == '};':
                    comment_start = i - 1
                    while comment_start >= 0 and lines[comment_start].strip().startswith('//'):
                        comment_start -= 1
                    comment_start += 1
                    return lines[:comment_start] + lines[j+1:]
    return lines


def make_env_non_static(lines):
    for i, line in enumerate(lines):
        if 'jmp_buf env' in line and 'static' in line:
            lines[i] = line.replace('static jmp_buf env', 'jmp_buf env')
            return lines
    return lines


def main():
    # =========================================================
    # 1. Process view.c
    # =========================================================
    view_path = os.path.join(SRC_DIR, 'view.c')
    lines = read_file(view_path)

    # STEP A: Extract function bodies FIRST (using original line numbers)
    lines, extracted = extract_and_remove_funcs(lines, FUNCS_TO_MOVE)
    print(f"Extracted {len(extracted)} function bodies from view.c")

    # STEP B: Remove struct printer_driver
    before = len(lines)
    lines = remove_struct_printer_driver(lines)
    print(f"Removed struct printer_driver ({before - len(lines)} lines)")

    # STEP C: Make env non-static
    lines = make_env_non_static(lines)
    print("Made env non-static")

    # STEP D: Remove duplicate inline helpers (now in globals.h)
    lines, n_inline = remove_inline_functions(lines, INLINE_PATTERNS)
    print(f"Removed {n_inline} static inline helper defs (now in globals.h)")

    # STEP E: Remove static forward declarations for moved functions
    lines, n_fwd = remove_lines_starting_with(lines, STATIC_FWD_PATTERNS)
    # Remove 'print_char' forward decl with exact match to avoid print_char_x_times
    lines, n_pc = remove_lines_starting_with(lines, PRINT_CHAR_FWD, exact=True)
    n_fwd += n_pc
    print(f"Removed {n_fwd} static forward declarations")

    # STEP F: Add #include "document.h" after last #include
    last_include = None
    for i, line in enumerate(lines):
        s = line.strip()
        if s.startswith('#include "') or s.startswith('#include <'):
            last_include = i
    if last_include is not None:
        lines.insert(last_include + 1, '\n#include "document.h"\n')
        print("Added #include \"document.h\" to view.c")

    write_file(view_path, lines)
    print("Wrote updated view.c")

    # =========================================================
    # 2. Write document.c
    # =========================================================
    doc_lines = [
        '#include "document.h"\n',
        '#include "io.h"\n',
        '\n',
    ]
    for name in sorted(extracted.keys()):
        doc_lines.extend(extracted[name])
        if not doc_lines[-1].endswith('\n'):
            doc_lines[-1] += '\n'
        doc_lines.append('\n')

    write_file(os.path.join(SRC_DIR, 'document.c'), doc_lines)
    print(f"Wrote document.c ({len(extracted)} functions)")

    # =========================================================
    # 3. Write document.h
    # =========================================================
    prototypes = [
        ('void', 'call_printer_driver'),
        ('uint8_t', 'check_for_command_prefix', 'uint8_t ch'),
        ('void', 'check_for_control_code'),
        ('void', 'compute_bytes_free'),
        ('uint8_t', 'deref_and_check_for_command_prefix'),
        ('void', 'display_document_file_state'),
        ('void', 'find_margins_of_current_ruler_buffer'),
        ('void', 'print_char'),
        ('void', 'print_char_just_to_screen'),
        ('void', 'process_document_character'),
        ('void', 'read_char'),
        ('void', 'return_to_cli_prompt'),
        ('void', 'stop_printing'),
        ('void', 'sub_c9445'),
        ('void', 'sub_cab6e'),
        ('void', 'sub_cabc4'),
        ('void', 'sub_cadf0'),
    ]

    doc_h = [
        '#ifndef DOCUMENT_H\n',
        '#define DOCUMENT_H\n',
        '\n',
        '#include "globals.h"\n',
        '#include "io.h"\n',
        '\n',
    ]
    for p in prototypes:
        if len(p) == 2:
            ret, name = p
            doc_h.append(f'extern {ret} {name}(void);\n')
        else:
            ret, name, params = p
            doc_h.append(f'extern {ret} {name}({params});\n')
    doc_h.append('\n#endif\n')

    write_file(os.path.join(SRC_DIR, 'document.h'), doc_h)
    print("Wrote document.h")

    # =========================================================
    # 4. Update globals.h
    # =========================================================
    globals_path = os.path.join(SRC_DIR, 'globals.h')
    gh = read_file(globals_path)

    # Add #include <setjmp.h> after <stdbool.h>
    for i, line in enumerate(gh):
        if '#include <stdbool.h>' in line:
            gh.insert(i + 1, '#include <setjmp.h>\n')
            break

    # Add JMP_* defines after MAX_COMMAND_LENGTH
    for i, line in enumerate(gh):
        if '#define MAX_COMMAND_LENGTH' in line:
            gh.insert(i + 1, '#define JMP_CLI     1\n')
            gh.insert(i + 2, '#define JMP_EDITOR  2\n')
            break

    # Add struct printer_driver and inline helpers before "// Inline helpers"
    insertions = [
        '// Printer driver struct\n',
        'struct printer_driver {\n',
        '    void (*print_char)(void);\n',
        '    void (*printer_on)(void);\n',
        '    void (*printer_off)(void);\n',
        '    void (*entry3)(void);\n',
        '};\n',
        '\n',
    ]
    for i, line in enumerate(gh):
        if '// Inline helpers' in line:
            gh[i:i] = insertions
            break

    # Add bit, rol, ror, asr after the existing inline helpers (sbc)
    extra_helpers = [
        'static inline void bit(uint8_t value) {\n',
        '    uint8_t tmp_ = a & value;\n',
        '    flags = (flags & ~(FLAG_Z|FLAG_N|FLAG_V)) | (tmp_ == 0 ? FLAG_Z : 0) | (value & (FLAG_N|FLAG_V));\n',
        '}\n',
        'static inline uint8_t rol(uint8_t value) {\n',
        '    uint8_t c_in = (flags & FLAG_C) ? 1 : 0;\n',
        '    flags = (flags & ~FLAG_C) | ((value & 0x80) ? FLAG_C : 0);\n',
        '    value = (value << 1) | c_in;\n',
        '    set_flags(value);\n',
        '    return value;\n',
        '}\n',
        'static inline uint8_t ror(uint8_t value) {\n',
        '    uint8_t c_in = (flags & FLAG_C) ? 0x80 : 0;\n',
        '    flags = (flags & ~FLAG_C) | ((value & 0x01) ? FLAG_C : 0);\n',
        '    value = (value >> 1) | c_in;\n',
        '    set_flags(value);\n',
        '    return value;\n',
        '}\n',
        'static inline uint8_t asr(uint8_t value) {\n',
        '    flags = (flags & ~FLAG_C) | ((value & 0x01) ? FLAG_C : 0);\n',
        '    value >>= 1;\n',
        '    set_flags(value);\n',
        '    return value;\n',
        '}\n',
    ]
    for i, line in enumerate(gh):
        if line.strip().startswith('static inline void bit_val'):
            gh[i:i] = extra_helpers
            break

    # bit_val is kept for backward compat (used in printing.c)

    # Add missing extern declarations before #endif
    extra_externs = [
        '\n',
        'extern jmp_buf env;\n',
        'extern const struct printer_driver *printer_driver_ptr;\n',
        'extern uint8_t print_xpos;\n',
        'extern uint8_t input_filename[];\n',
        'extern uint8_t output_filename[];\n',
        'extern uint8_t file_edit_flags;\n',
        'extern addr_t current_ruler_ptr;\n',
        'extern uint8_t l003a;\n',
        'extern uint8_t l0046;\n',
    ]
    for i, line in enumerate(gh):
        if line.strip() == '#endif':
            gh[i:i] = extra_externs
            break

    write_file(globals_path, gh)
    print("Updated globals.h")

    # =========================================================
    # 5. Update printing.c
    # =========================================================
    print_path = os.path.join(SRC_DIR, 'printing.c')
    plines = read_file(print_path)

    for i, line in enumerate(plines):
        if line.strip().startswith('#include "printing.h"'):
            plines.insert(i + 1, '#include "document.h"\n')
            break

    write_file(print_path, plines)
    print("Updated printing.c")

    print("\nDone!")


if __name__ == '__main__':
    main()
