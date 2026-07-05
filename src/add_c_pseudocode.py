#!/usr/bin/env python3
"""
Add C pseudocode functions after each assembly routine in view.c.
Generates algorithmic pseudocode rather than line-by-line translation.
"""

import re
import os
import string

FILE = '/home/dg/nonshared/view/src/view.c'
BACKUP = FILE + '.bak'

# Global variables tracker
GLOBAL_VARS = {
    'a', 'x', 'y', 'ptr1', 'ptr2', 'ptr3', 'ptr5', 'ptr6',
    'tmp0', 'tmp1', 'tmp2', 'tmp3', 'tmp4', 'tmp5', 'tmp6', 'tmp7', 'tmp8', 'tmp9',
    'l0011', 'l0012', 'l0021', 'l0030', 'l0031', 'l0033', 'l0034',
    'l0038', 'l0039', 'l003a', 'l003b', 'l003d',
    'l0042', 'l0043', 'l0044', 'l0045', 'l0046', 'l0047', 'l0048', 'l0049', 'l004a',
    'l006d', 'l006e', 'l006f', 'l0072', 'l0073', 'l0074',
    'l0076', 'l0079', 'l007a', 'l007e',
    'l0081', 'l0082', 'l0083', 'l0084',
    'top', 'page', 'himem', 'oshwm',
    'xpos', 'ypos', 'hscroll_pos', 'print_xpos', 'cursor_moved_flag',
    'flags_need_redrawing_flag', 'ruler_stack_ptr',
    'input_file_empty_flag', 'file_edit_flags', 'error_handling_mode',
    'print_flags', 'printing_from_file_flag', 'macro_executing_flag',
    'format_mode_flag', 'justifying_flag', 'insert_mode_flag',
    'folding_flag', 'microspacing_flag',
    'screen_height', 'screen_width', 'current_tab_key',
    'ruler_right_stop', 'ruler_left_stop',
    'top_margin', 'bottom_margin', 'header_margin', 'footer_margin',
    'page_length', 'line_spacing',
    'footers_enabled_flag', 'headers_enabled_flag',
    'rhs_extra_margin', 'left_margin', 'two_sided_flag',
    'highlight1_code', 'highlight2_code',
    'current_line_ptr', 'current_format_line_ptr', 'current_edit_line_ptr',
    'current_ruler_ptr', 'input_buffer_ptr',
    'area_start_ptr', 'area_end_ptr',
    'doc_ptr1', 'doc_ptr2', 'doc_ptr3',
    'first_macro_ptr', 'last_macro_ptr',
    'printer_driver_ptr', 'rw_file_handle',
    'print_flags', 'input_file_empty_flag',
    'file_ptr',
    'register_value_l', 'register_value_p',
}

# Known functions from the codebase
KNOWN_FUNCS = {
    'jsr': True,
}

def clean_operand(op):
    """Clean up 6502 operands for C."""
    op = op.strip()
    op = re.sub(r'\s+;.*$', '', op)
    op = op.replace('#', '')
    op = op.replace('(', '*(')
    op = op.replace(')', ')')
    op = op.replace(',y', ' + y')
    op = op.replace(',x', ' + x')
    op = op.replace(',', ', ')
    return op


def infer_signature_and_logic(entry_points, all_lines, start, end):
    """Infer function characteristics from the assembly code."""
    lines = []
    for idx in range(start, end):
        line = all_lines[idx]
        stripped = line.rstrip('\n')
        lines.append(stripped)

    full_text = '\n'.join(lines)

    has_rts = bool(re.search(r'^//\s+rts', full_text, re.MULTILINE))
    has_jmp = bool(re.search(r'^//\s+jmp\s', full_text, re.MULTILINE))
    has_jsr = bool(re.search(r'^//\s+jsr\s', full_text, re.MULTILINE))
    is_zproc = bool(re.search(r'zproc', full_text))

    # Count instructions
    instrs = re.findall(r'^//\s+(\w+)', full_text, re.MULTILINE)

    # Detect what this routine does
    calls_functions = re.findall(r'^//\s+jsr\s+(\w+)', full_text, re.MULTILINE)
    jmp_targets = re.findall(r'^//\s+jmp\s+(\w+)', full_text, re.MULTILINE)

    # Determine routine type
    is_thunk = len(instrs) == 1 and has_jmp  # Just a jmp to another routine
    is_trivial = len(instrs) <= 3 and not has_jsr and has_jmp  # Small jump wrapper
    is_jsr_wrapper = len(instrs) <= 3 and has_jsr and has_jmp  # Call something then jump
    is_subroutine = has_rts and has_jsr  # Real subroutine

    # Check for specific patterns
    does_print = any('print' in c for c in calls_functions)
    calls_bdos = any('bdos' in c for c in calls_functions)
    calls_screen = any('screen' in c or 'SCREEN' in c for c in calls_functions)
    is_cmd = any(n.endswith('_cmd') or n.endswith('_key') or n == 'main' for n in entry_points)
    is_sub = any(n.startswith('sub_') for n in entry_points)
    is_check = any(('check' in n or 'is_' in n or 'sanitise' in n or 'parse' in n) for n in entry_points)

    return {
        'has_rts': has_rts,
        'has_jmp': has_jmp,
        'has_jsr': has_jsr,
        'is_zproc': is_zproc,
        'is_thunk': is_thunk,
        'is_trivial': is_trivial,
        'is_jsr_wrapper': is_jsr_wrapper,
        'is_subroutine': is_subroutine,
        'calls_functions': calls_functions,
        'jmp_targets': jmp_targets,
        'does_print': does_print,
        'calls_bdos': calls_bdos,
        'calls_screen': calls_screen,
        'is_cmd': is_cmd,
        'is_sub': is_sub,
        'is_check': is_check,
        'instrs': instrs,
    }


def generate_pseudocode(entry_points, info):
    """Generate C pseudocode based on the routine's structure."""
    entries = ', '.join(entry_points)
    calls = info['calls_functions']
    jmps = info['jmp_targets']
    lines = []

    indent = '    '

    if info['is_thunk']:
        # Single jmp instruction
        target = jmps[0] if jmps else 'unknown'
        lines.append(f'{indent}// Branch to {target}\n')
        lines.append(f'{indent}{target}();\n')
        return ''.join(lines)

    if info['is_trivial'] and jmps:
        target = jmps[0]
        lines.append(f'{indent}// Short routine - jump to {target}\n')
        lines.append(f'{indent}return {target}();\n')
        return ''.join(lines)

    if info['is_jsr_wrapper']:
        # Calls something then jumps
        target = jmps[0] if jmps else 'unknown'
        for c in calls:
            lines.append(f'{indent}{c}();\n')
        lines.append(f'{indent}return {target}();\n')
        return ''.join(lines)

    # For real subroutines, generate structured pseudocode
    # Group related function calls
    indent = '    '

    # Print calls in order
    for c in calls:
        lines.append(f'{indent}{c}();\n')

    if not calls and not info['is_subroutine']:
        lines.append(f'{indent}// TODO: implement logic\n')

    if not lines:
        lines.append(f'{indent}// Processing...\n')

    return ''.join(lines)


def main():
    with open(FILE, 'r') as f:
        lines = f.readlines()

    if not os.path.exists(BACKUP):
        with open(BACKUP, 'w') as f:
            f.writelines(lines)
        print(f"Backup saved to {BACKUP}")

    # Parse code labels
    code_labels = []
    for i, line in enumerate(lines):
        m = re.match(r'^// ([a-zA-Z_][a-zA-Z0-9_]*):$', line)
        if not m:
            continue
        label = m.group(1)
        next_line = lines[i+1] if i+1 < len(lines) else ''
        if re.search(r'\.fill|\.byte|\.word|\.ascii', next_line) and \
           label not in ('main', 'run_cli', 'cli_loop', 'editor_loop'):
            continue
        if re.search(r'_table|_ptrs', label):
            continue
        if label in ('__begin_pointer_array', '__end_pointer_array',
                     'jumptable4_cli', 'jumptable4_cli_end',
                     'jumptable2_format', 'lb2a1', 'lb152'):
            continue
        code_labels.append((i, label))

    print(f"Found {len(code_labels)} code labels")

    # Process the file
    output = []
    i = 0
    processed = 0

    while i < len(lines):
        line = lines[i]
        m = re.match(r'^// ([a-zA-Z_][a-zA-Z0-9_]*):$', line)
        if not m:
            output.append(line)
            i += 1
            continue

        label = m.group(1)
        next_line = lines[i+1] if i+1 < len(lines) else ''
        if re.search(r'\.fill|\.byte|\.word|\.ascii', next_line) and \
           label not in ('main', 'run_cli', 'cli_loop', 'editor_loop'):
            output.append(line)
            i += 1
            continue
        if re.search(r'_table|_ptrs', label):
            output.append(line)
            i += 1
            continue
        if label in ('__begin_pointer_array', '__end_pointer_array',
                     'jumptable4_cli', 'jumptable4_cli_end',
                     'jumptable2_format', 'lb2a1', 'lb152'):
            output.append(line)
            i += 1
            continue

        # Collect consecutive labels
        label_lines = [(i, label)]
        j = i + 1
        while j < len(lines) and re.match(r'^\s*$', lines[j]):
            j += 1

        while j < len(lines):
            m2 = re.match(r'^// ([a-zA-Z_][a-zA-Z0-9_]*):$', lines[j])
            if not m2:
                break
            l2 = m2.group(1)
            nl = lines[j+1] if j+1 < len(lines) else ''
            if re.search(r'\.fill|\.byte|\.word|\.ascii', nl) and \
               l2 not in ('main', 'run_cli', 'cli_loop', 'editor_loop'):
                break
            if re.search(r'_table|_ptrs', l2):
                break
            if l2 in ('__begin_pointer_array', '__end_pointer_array',
                      'jumptable4_cli', 'jumptable4_cli_end',
                      'jumptable2_format', 'lb2a1', 'lb152'):
                break
            label_lines.append((j, l2))
            j += 1
            while j < len(lines) and re.match(r'^\s*$', lines[j]):
                j += 1

        start_idx = i

        # Find end of this routine
        end_idx = len(lines)
        for k in range(j, len(lines)):
            lm = re.match(r'^// ([a-zA-Z_][a-zA-Z0-9_]*):$', lines[k])
            if lm:
                nl = lines[k+1] if k+1 < len(lines) else ''
                if re.search(r'\.fill|\.byte|\.word|\.ascii', nl):
                    continue
                if re.search(r'_table|_ptrs', lm.group(1)):
                    continue
                if lm.group(1) in ('__begin_pointer_array', '__end_pointer_array',
                                   'jumptable4_cli', 'jumptable4_cli_end',
                                   'jumptable2_format', 'lb2a1', 'lb152'):
                    continue
                end_idx = k
                break

        # Output assembly lines
        for idx in range(start_idx, end_idx):
            output.append(lines[idx])

        # Only add C function if this is a meaningful routine
        func_name = label_lines[0][1]
        entry_names = [l[1] for l in label_lines]

        info = infer_signature_and_logic(entry_names, lines, start_idx, end_idx)

        # Determine return type
        is_cmd = info['is_cmd']
        is_check = info['is_check']
        is_thunk = info['is_thunk']
        is_trivial = info['is_trivial']
        is_zproc = info['is_zproc']

        if is_check:
            rt = 'bool'
        else:
            rt = 'void'

        params = 'void'

        output.append(f'\n// Function: {func_name}\n')
        if len(entry_names) > 1:
            output.append(f'// MULTIPLE ENTRY POINTS: {", ".join(entry_names)}\n')

        output.append(f'{rt} {func_name}({params}) {{\n')
        body = generate_pseudocode(entry_names, info)
        output.append(body)
        output.append('}\n\n')
        processed += 1

        # Check for zproc blocks
        i = end_idx

    with open(FILE, 'w') as f:
        f.writelines(output)

    print(f"Processed {processed} routines")
    print(f"Written {len(output)} lines to {FILE}")
    print(f"File size: {len(output)} lines")


if __name__ == '__main__':
    main()
