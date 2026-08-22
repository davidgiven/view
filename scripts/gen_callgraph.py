#!/usr/bin/env python3
"""Generate CALLGRAPH.md from C source files.

Usage: python3 scripts/gen_callgraph.py > CALLGRAPH.md
"""

import re
import os
from collections import OrderedDict

SRC_DIR = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'src')
APP_FILES = ['view.c', 'printing.c', 'document.c', 'editor.c']

# Regex to match a function definition line
FUNC_RE = re.compile(
    r'^(?:(static|extern)\s+)?'
    r'(?:inline\s+)?'
    r'(?:void|uint8_t|uint16_t|addr_t|bool|int|char|long|unsigned|const|struct\s+\w+|uint8_t\s*\*|char\s*\*)'
    r'\s+'
    r'(\w+)\s*\('
)

CALL_RE = re.compile(r'\b(\w+)\s*\(')
TOKEN_RE = re.compile(r'\b([a-zA-Z_]\w*)\b')

# Inline helpers (defined but not part of the call graph)
INLINE_HELPERS = {'set_flags', 'sbc', 'cmp', 'bit', 'rol', 'ror', 'asr', 'adc'}

# External driver/library functions (called from app code but defined elsewhere)
LIB_FUNCS = {
    'exit', 'setjmp', 'longjmp',
    'cli_putchar', 'cli_putstring', 'cli_getchar', 'cli_readstring',
    'screen_clear', 'screen_putchar', 'screen_getchar',
    'screen_getcursor', 'screen_setcursor', 'screen_enablecursor',
    'screen_setstyle', 'screen_enter', 'screen_leave',
    'screen_getsize', 'screen_scrollup', 'screen_scrolldown',
}

KNOWN_GLOBALS = {
    'a', 'x', 'y', 'sp',
    'ram',
    'ptr1', 'current_edit_line_ptr', 'current_format_line_ptr',
    'current_ruler_ptr', 'current_line_ptr', 'page', 'top', 'himem',
    'top_of_screen_line_ptr', 'ptr6', 'ptr5', 'first_macro_ptr', 'last_macro_ptr',
    'ptr3', 'oshwm',
    'l0021', 'l0030', 'l0031', 'printing_from_file_flag',
    'l0033', 'l0034', 'l0038', 'l0039', 'l003a', 'l003b',
    'file_edit_flags', 'l003d', 'xpos', 'input_file_empty_flag',
    'l0042', 'l0043', 'l0044', 'l0045', 'l0046', 'l0047',
    'l0048', 'l0049', 'l004a',
    'ptr2', 'rw_file_handle', 'error_handling_mode', 'print_flags',
    'edit_buffer_dirty_flag', 'edit_buffer_unpacked_flag',
    'l006f', 'ruler_index_ptr', 'hscroll_pos',
    'l0072', 'l0073', 'l0074',
    'flags_need_redrawing_flag', 'status_line_needs_redrawing_flag',
    'l0076', 'ypos', 'print_xpos', 'l0079', 'l007a',
    'cursor_moved_flag', 'delimiter_char', 'line_format_status',
    'input_buffer_offset', 'l0080', 'l0081', 'l0082', 'l0083', 'l0084',
    'tmp0', 'tmp1', 'tmp2', 'tmp3', 'tmp4', 'tmp5', 'tmp6', 'tmp7', 'tmp8', 'tmp9',
    'top_margin', 'bottom_margin', 'header_margin', 'footer_margin',
    'page_length', 'line_spacing',
    'footers_enabled_flag', 'headers_enabled_flag',
    'rhs_extra_margin', 'macro_executing_flag', 'two_sided_flag',
    'left_margin', 'highlight1_code', 'highlight2_code', 'highlight_code',
    'format_mode_flag', 'justifying_flag', 'insert_mode_flag',
    'screen_maxrow', 'screen_maxcolumn',
    'microspacing_flag', 'current_tab_key', 'folding_flag',
    'ruler_right_stop', 'ruler_left_stop',
    'printer_driver_block', 'printer_driver_ptr',
    'input_buffer', 'output_buffer',
    'header_text_maybe', 'footer_text_maybe',
    'filename_buffer', 'output_filename', 'printer_driver_name',
    'register_value_array', 'markers_array',
    'line_lengths', 'input_filename',
    'input_fp', 'output_fp',
    'current_line_buffer', 'current_ruler_buffer',
    'number_callback', 'pointer_array',
    'area_start_ptr', 'area_end_ptr',
    'doc_ptr1', 'doc_ptr2', 'doc_ptr3', 'doc_ptr4',
    'l006e', 'l006d',
    'printer_driver', 'default_printer_driver',
    'parser_table', 'la83d', 'la995_data',
    'x_register_value_array',
}

KW_EXCLUDE = {
    'if', 'while', 'for', 'do', 'switch', 'case', 'return', 'break',
    'continue', 'else', 'goto', 'sizeof', 'typedef', 'struct', 'union',
    'enum', 'const', 'static', 'extern', 'volatile', 'register',
    'unsigned', 'signed', 'int', 'char', 'short', 'long', 'float',
    'double', 'void', 'uint8_t', 'uint16_t', 'uint32_t', 'addr_t',
    'bool', 'true', 'false', 'NULL', 'FILE', 'size_t',
    'stdin', 'stdout', 'stderr',
}


def get_func_defs_in_file(filepath):
    """Parse all function definitions in a file.
    Returns list of (name, line_1based, is_static)"""
    defs = []
    with open(filepath, 'r') as f:
        lines = f.readlines()
    for i, line in enumerate(lines):
        stripped = line.strip()
        if not stripped or stripped.startswith('//') or stripped.startswith('/*') or stripped.startswith('*'):
            continue
        # Skip extern and forward declarations (not definitions)
        if stripped.startswith('extern '):
            continue
        # Forward declarations end with ';' not '{'
        if '{' not in stripped:
            continue
        m = FUNC_RE.match(stripped)
        if m:
            name = m.group(2)
            if name not in INLINE_HELPERS:
                is_static = 'static' in (m.group(1) or '')
                defs.append((name, i + 1, is_static))
    return defs


def extract_body_between(lines, start_line_1based, end_line_1based):
    """Extract lines from start_line to end_line-1.
    Returns the concatenated text."""
    start = start_line_1based - 1
    end = end_line_1based - 1 if end_line_1based else len(lines)
    body = ''.join(lines[start:end])
    # Remove comments and strings for analysis
    body = re.sub(r'//.*', '', body)
    body = re.sub(r'"[^"]*"', '', body)
    body = re.sub(r"'[^']*'", '', body)
    return body


def extract_calls(body, project_funcs):
    """Return set of project function calls."""
    calls = set()
    for m in CALL_RE.finditer(body):
        name = m.group(1)
        if name in project_funcs and name not in LIB_FUNCS:
            calls.add(name)
    return calls


def extract_globals(body, project_funcs):
    """Return set of global variable references."""
    globs = set()
    for m in TOKEN_RE.finditer(body):
        name = m.group(1)
        if (name in KNOWN_GLOBALS and name not in project_funcs
                and name not in KW_EXCLUDE and name not in LIB_FUNCS):
            globs.add(name)
    return globs


def format_entry(calls, globs):
    parts = []
    if calls:
        parts.append(" → " + ", ".join(sorted(calls)))
    if globs:
        parts.append(" [" + ", ".join(sorted(globs)) + "]")
    return "".join(parts)


def main():
    # Gather function definitions from each app file
    file_defs = OrderedDict()  # file -> [(name, line, is_static)]
    all_funcs = {}  # name -> info
    
    for cf in APP_FILES:
        fpath = os.path.join(SRC_DIR, cf)
        if not os.path.exists(fpath):
            file_defs[cf] = []
            continue
        defs = get_func_defs_in_file(fpath)
        file_defs[cf] = defs
        for name, line, is_static in defs:
            all_funcs[name] = {'file': cf, 'line': line, 'static': is_static}
    
    project_funcs = set(all_funcs.keys())
    
    # Build body text for each function (between its definition and the next one)
    func_bodies = {}
    for cf in APP_FILES:
        fpath = os.path.join(SRC_DIR, cf)
        if not os.path.exists(fpath):
            continue
        with open(fpath, 'r') as f:
            lines = f.readlines()
        defs = file_defs[cf]
        for idx, (name, line, _) in enumerate(defs):
            next_line = defs[idx + 1][1] if idx + 1 < len(defs) else len(lines) + 1
            body = extract_body_between(lines, line, next_line)
            func_bodies[name] = body
    
    # For each function, find callees and globals
    results = OrderedDict()
    for name in all_funcs:
        body = func_bodies.get(name, '')
        calls = extract_calls(body, project_funcs)
        calls.discard(name)
        globs = extract_globals(body, project_funcs)
        results[name] = (calls, globs)
    
    # Generate output
    print("# Call Graph\n")
    print("Functions defined in the source files and their callees (only calls to other functions defined in the project).\n")
    
    for cf, defs in file_defs.items():
        section = cf.replace('.c', '')
        print(f"## {section}\n")
        
        if not defs:
            print("*(stub)*\n")
            continue
        
        for name, line, is_static in defs:
            calls, globs = results[name]
            suffix = format_entry(calls, globs)
            print(f"- **{name}**{suffix}")
        
        print()


if __name__ == '__main__':
    main()
