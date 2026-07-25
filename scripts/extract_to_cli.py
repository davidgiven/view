#!/usr/bin/env python3
"""Extract CLI command functions from view.c into cli.c.

Functions to move (31 total):
  call_through_jumptable_2 (→ renamed execute_cli_command)
  bye_cmd, cmd_err_no_target, cmd_err_no_string
  search_cmd, change_cmd, replace_cmd
  screen_cmd, sheets_cmd, print_cmd, print_to_screen, start_printing
  edit_cmd, more_cmd, finish_cmd, quit_cmd, close_input_output_files
  save_cmd_write_cmd, check_for_at_least_150_bytes_free
  load_cmd, read_cmd, mode_cmd, microspace_cmd, setup_cmd
  field_cmd, count_cmd, format_cmd, new_cmd, fold_cmd, printer_cmd
  name_cmd, clear_cmd
"""

import os

SRC_DIR = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'src')

# Functions to move: (name, start_line, end_line_exclusive, is_static)
# All line numbers based on CURRENT view.c
FUNCS_TO_MOVE = [
    ('bye_cmd',                       870,   879, True),
    ('cmd_err_no_target',             879,   889, True),
    ('cmd_err_no_string',             889,   899, True),
    ('search_cmd',                    899,   929, True),
    ('change_cmd',                    929,   986, True),
    ('replace_cmd',                  1004,  1073, True),
    ('screen_cmd',                   1293,  1301, True),
    ('sheets_cmd',                   1301,  1319, True),
    ('print_cmd',                    1319,  1332, True),
    ('print_to_screen',              1332,  1340, True),
    ('start_printing',               1340,  1358, True),
    ('edit_cmd',                     1358,  1382, True),
    ('more_cmd',                     1382,  1449, True),
    ('finish_cmd',                   1449,  1492, True),
    ('quit_cmd',                     1492,  1502, True),
    ('close_input_output_files',     1502,  1523, True),
    ('save_cmd_write_cmd',           1523,  1581, True),
    ('check_for_at_least_150_bytes_free', 1581, 1597, True),
    ('load_cmd',                     1689,  1710, True),
    ('read_cmd',                     1710,  1721, True),
    ('mode_cmd',                     1721,  1729, True),
    ('microspace_cmd',               1729,  1778, True),
    ('setup_cmd',                    1778,  1859, True),
    ('field_cmd',                    1859,  1888, True),
    ('count_cmd',                    1888,  2089, True),
    ('format_cmd',                   2089,  2166, True),
    ('new_cmd',                      2166,  2176, True),
    ('fold_cmd',                     2176,  2228, True),
    ('printer_cmd',                  2228,  2282, True),
    ('name_cmd',                     2316,  2339, True),
    ('call_through_jumptable_2',     9959, 10004, True),
    ('clear_cmd',                   12181, 12201, True),
]

# Static forward declaration patterns to remove from view.c
STATIC_FWD_PATTERNS = [
    'static void call_through_jumptable_2(',
    'static void clear_cmd(',
    'static void close_input_output_files(',
    'static void print_to_screen(',
    'static void start_printing(',
    'static void check_for_at_least_150_bytes_free(',
    'static void read_into_document(',
]


def read_file(path):
    with open(path) as f:
        return f.readlines()

def write_file(path, lines):
    with open(path, 'w') as f:
        f.writelines(lines)


def extract_and_remove_funcs(lines, funcs):
    """Remove function bodies (bottom-up). Return (new_lines, {name: body_lines})."""
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


def remove_lines_starting_with(lines, patterns):
    result = []
    n = 0
    for line in lines:
        if any(line.strip().startswith(p) for p in patterns):
            n += 1
        else:
            result.append(line)
    return result, n


def main():
    # =========================================================
    # 1. Process view.c
    # =========================================================
    view_path = os.path.join(SRC_DIR, 'view.c')
    lines = read_file(view_path)

    # Step A: Extract function bodies (bottom-up)
    lines, extracted = extract_and_remove_funcs(lines, FUNCS_TO_MOVE)
    print(f"Extracted {len(extracted)} function bodies from view.c")

    # Step B: Remove static forward declarations
    lines, n_fwd = remove_lines_starting_with(lines, STATIC_FWD_PATTERNS)
    print(f"Removed {n_fwd} static forward declarations")

    # Step C: Rename call_through_jumptable_2 → execute_cli_command in call sites
    for i, line in enumerate(lines):
        if 'call_through_jumptable_2' in line:
            lines[i] = line.replace('call_through_jumptable_2', 'execute_cli_command')

    # Step D: Add #include "cli.h" after the last existing #include
    last_include = None
    for i, line in enumerate(lines):
        s = line.strip()
        if s.startswith('#include "') or s.startswith('#include <'):
            last_include = i
    if last_include is not None:
        # Check if cli.h is already included
        has_cli = any('cli.h' in l for l in lines[:last_include+5])
        if not has_cli:
            lines.insert(last_include + 1, '#include "cli.h"\n')
            print("Added #include \"cli.h\" to view.c")

    write_file(view_path, lines)
    print("Wrote updated view.c")

    # =========================================================
    # 2. Write cli.c
    # =========================================================
    cli_lines = [
        '#include "cli.h"\n',
        '#include "document.h"\n',
        '#include "printing.h"\n',
        '#include "io.h"\n',
        '#include <stdlib.h>\n',
        '\n',
    ]

    for name in sorted(extracted.keys()):
        body = extracted[name]
        # Rename call_through_jumptable_2 → execute_cli_command within its own body
        for j, line in enumerate(body):
            if 'call_through_jumptable_2' in line:
                body[j] = line.replace('call_through_jumptable_2', 'execute_cli_command')
        cli_lines.extend(body)
        if not cli_lines[-1].endswith('\n'):
            cli_lines[-1] += '\n'
        cli_lines.append('\n')

    write_file(os.path.join(SRC_DIR, 'cli.c'), cli_lines)
    print(f"Wrote cli.c ({len(extracted)} functions)")

    # =========================================================
    # 3. Write cli.h
    # =========================================================
    prototypes = [
        ('void', 'execute_cli_command'),
        ('void', 'bye_cmd'),
        ('void', 'cmd_err_no_target'),
        ('void', 'cmd_err_no_string'),
        ('void', 'search_cmd'),
        ('void', 'change_cmd'),
        ('void', 'replace_cmd'),
        ('void', 'screen_cmd'),
        ('void', 'sheets_cmd'),
        ('void', 'print_cmd'),
        ('void', 'print_to_screen'),
        ('void', 'start_printing'),
        ('void', 'edit_cmd'),
        ('void', 'more_cmd'),
        ('void', 'finish_cmd'),
        ('void', 'quit_cmd'),
        ('void', 'close_input_output_files'),
        ('void', 'save_cmd_write_cmd'),
        ('void', 'check_for_at_least_150_bytes_free'),
        ('void', 'load_cmd'),
        ('void', 'read_cmd'),
        ('void', 'mode_cmd'),
        ('void', 'microspace_cmd'),
        ('void', 'setup_cmd'),
        ('void', 'field_cmd'),
        ('void', 'count_cmd'),
        ('void', 'format_cmd'),
        ('void', 'new_cmd'),
        ('void', 'fold_cmd'),
        ('void', 'printer_cmd'),
        ('void', 'name_cmd'),
        ('void', 'clear_cmd'),
    ]

    cli_h = [
        '#ifndef CLI_H\n',
        '#define CLI_H\n',
        '\n',
        '#include "globals.h"\n',
        '\n',
    ]

    # Forward declarations for view.c functions called by CLI commands.
    # These will need to be made non-static in view.c.
    view_externs = [
        ('void', 'sub_c8412'),
        ('void', 'c8b7b'),
        ('void', 'sub_c8c7c'),
        ('void', 'move_cursor_to_address'),
        ('void', 'enter_editor_mode'),
        ('void', 'sub_c83f0'),
        ('void', 'sub_c8a4f'),
        ('void', 'draw_prompt_characters'),
        ('void', 'sub_c8361'),
        ('void', 'sub_c8371'),
        ('void', 'show_memory_full_error'),
        ('void', 'esc_key'),
        ('void', 'print_document'),
        ('void', 'parse_filename_from_command'),
        ('void', 'set_document_name_to_filename_buffer'),
        ('void', 'open_input_file'),
        ('void', 'open_output_file'),
        ('void', 'initialise_document'),
        ('void', 'read_first_chunk_from_input_file'),
        ('void', 'check_not_continuous_editing'),
        ('void', 'check_continuous_editing'),
        ('void', 'parse_marks_from_command'),
        ('void', 'sanitise_area'),
        ('void', 'select_file'),
        ('void', 'write_area_to_file'),
        ('void', 'sub_c89d3'),
        ('void', 'move_cursor_to_top_of_document'),
        ('void', 'read_next_chunk_from_input_file'),
        ('void', 'cb05a'),
        ('void', 'put_byte_to_file'),
        ('void', 'read_into_document'),
        ('void', 'reset_area_to_entire_document'),
        ('void', 'bad_filename_error'),
        ('void', 'close_file'),
        ('void', 'parse_optional_filename_from_command'),
        ('void', 'reset_document_name_after_load'),
        ('void', 'prepare_printer_driver'),
        ('void', 'parse_integer_from_command'),
        ('void', 'sub_c8e33'),
        ('void', 'process_current_document_character'),
        ('void', 'sub_caf5f'),
        ('void', 'wipe_buffer'),
        ('void', 'sub_c9977'),
        ('void', 'display_not_enough_memory'),
    ]

    for p in prototypes:
        ret, name = p
        cli_h.append(f'extern {ret} {name}(void);\n')

    cli_h.append('\n')
    cli_h.append('// Functions in view.c called by CLI commands\n')
    for p in view_externs:
        ret, name = p
        cli_h.append(f'extern {ret} {name}(void);\n')

    cli_h.append('\n#endif\n')

    write_file(os.path.join(SRC_DIR, 'cli.h'), cli_h)
    print("Wrote cli.h")


if __name__ == '__main__':
    main()
