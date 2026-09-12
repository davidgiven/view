#!/usr/bin/env python3
"""
Liveness analysis using libclang AST.

Computes live_in, live_out, defs, uses, locals for each function.
"""

import clang.cindex, os, subprocess, re, sys

# ─── Library setup ───
clang.cindex.Config.set_library_file('/usr/lib64/libclang.so')
RESOURCE_DIR = subprocess.run(['clang', '-print-resource-dir'],
                               capture_output=True, text=True).stdout.strip()
CPPFLAGS = ['-I.', '-Isrc', f'-I{RESOURCE_DIR}/include',
            '-D__attribute__(x)=', '-D__inline__=', '-D__restrict__=']

# ─── Tracked variables ───
REGISTERS = ['a', 'x', 'y']
FLAG_BITS = ['C', 'Z', 'N', 'V']
TMP_VARS = ['tmp01', 'tmp23', 'tmp45', 'tmp67', 'tmp89']
BYTE_VARS = [f'tmp{i}' for i in range(10)]
PTR_VARS = ['ptr1', 'ptr2', 'ptr3', 'ptr5', 'ptr6']
ALL_VARS = set(REGISTERS + TMP_VARS + BYTE_VARS + PTR_VARS +
               [f'flags:{b}' for b in FLAG_BITS])

TRACKED_VARS = set(REGISTERS + TMP_VARS + BYTE_VARS + PTR_VARS)

def is_tracked(name):
    return name in TRACKED_VARS

# ─── Inline helpers (not real function calls) ───
INLINE_HELPERS = {
    'set_flags', 'cmp', 'adc', 'sbc', 'bit', 'rol', 'ror', 'asr',
    '_tmp0', '_tmp1', '_tmp2', '_tmp3', '_tmp4', '_tmp5',
    '_tmp6', '_tmp7', '_tmp8', '_tmp9',
}
LIB_FUNCTIONS = {
    'exit', 'setjmp', 'longjmp', 'snprintf', 'sprintf', 'printf',
    'fopen', 'fclose', 'fputc', 'fgetc', 'fread', 'fwrite', 'fseek', 'ftell',
    'feof', 'ferror', 'rewind', 'fflush', 'ungetc',
    'cli_putchar', 'cli_putstring', 'cli_getchar', 'cli_readstring',
    'screen_putchar', 'screen_getchar', 'screen_getcursor', 'screen_setcursor',
    'screen_setstyle', 'screen_getsize', 'screen_enter', 'screen_leave',
    'screen_clear', 'screen_scrollup', 'screen_scrolldown', 'screen_enablecursor',
    'isupper', 'islower', 'isalpha', 'isdigit', 'isalnum', 'isspace',
    'toupper', 'tolower', 'memset',
}

# ─── Parse files ───
_parse_cache = {}
def parse_file(filepath):
    if filepath not in _parse_cache:
        idx = clang.cindex.Index.create()
        tu = idx.parse(filepath, CPPFLAGS)
        _parse_cache[filepath] = tu
    return _parse_cache[filepath]

# ─── Find function definitions in a parsed TU ───
def find_functions_ast(tu, filepath):
    """Return list of (name, start_line, end_line, ast_node)."""
    result = []
    def visit(node):
        if node.kind == clang.cindex.CursorKind.FUNCTION_DECL and node.is_definition():
            name = node.spelling
            if not name.startswith('_') and name not in INLINE_HELPERS:
                extent = node.extent
                result.append((name, extent.start.line, extent.end.line, node))
        for c in node.get_children():
            visit(c)
    visit(tu.cursor)
    return result

# ─── Extract flag ops from source text (reliable and simple) ───
def get_flag_defs_uses(line_text):
    """Return (defs, uses) for flag operations in a line."""
    defs = set()
    uses = set()
    for bit in FLAG_BITS:
        if re.search(r'flags\s*&=\s*~FLAG_' + bit, line_text):
            defs.add(f'flags:{bit}')
        elif re.search(r'flags\s*\|=\s*FLAG_' + bit, line_text):
            defs.add(f'flags:{bit}')
        elif re.search(r'flags\s*=\s*flags\s*[&|].*FLAG_' + bit, line_text):
            defs.add(f'flags:{bit}')
            uses.add(f'flags:{bit}')
        elif re.search(r'if\s*\(.*flags\s*&\s*FLAG_' + bit, line_text):
            uses.add(f'flags:{bit}')
    return defs, uses

# ─── Analyze a single function ───
def analyze_func(func_node, lines, filepath):
    """
    Walk the AST of a function and compute:
    - defs: variables written
    - uses: variables read  
    - params: parameter names
    - calls: list of called function names
    """
    name = func_node.spelling
    params = set()
    body = None
    
    for c in func_node.get_children():
        if c.kind == clang.cindex.CursorKind.PARM_DECL:
            p = c.spelling
            if p in REGISTERS:
                params.add(p)
        elif c.kind == clang.cindex.CursorKind.COMPOUND_STMT:
            body = c
    
    defs = set()
    uses = set()
    calls = []
    
    def walk_stmt(stmt):
        """Walk a statement node."""
        nonlocal defs, uses, calls
        kind = stmt.kind
        
        if kind == clang.cindex.CursorKind.DECL_STMT:
            for decl in stmt.get_children():
                if decl.kind == clang.cindex.CursorKind.VAR_DECL:
                    v = decl.spelling
                    if is_tracked(v):
                        defs.add(v)
                    for c in decl.get_children():
                        walk_expr(c)
            return
        
        if kind == clang.cindex.CursorKind.BINARY_OPERATOR and stmt.spelling == '=':
            children = list(stmt.get_children())
            if children:
                for c in children[1:]:
                    walk_expr(c)
                walk_expr(children[0], is_lhs=True)
            return
        
        if kind == clang.cindex.CursorKind.COMPOUND_ASSIGNMENT_OPERATOR:
            children = list(stmt.get_children())
            if children:
                for c in children[1:]:
                    walk_expr(c)
                walk_expr(children[0], is_lhs=False)
                walk_expr(children[0], is_lhs=True)
            return
        
        if kind == clang.cindex.CursorKind.UNARY_OPERATOR and stmt.spelling in ('++', '--'):
            children = list(stmt.get_children())
            if children:
                walk_expr(children[0], is_lhs=False)
                walk_expr(children[0], is_lhs=True)
            return
        
        if kind == clang.cindex.CursorKind.CALL_EXPR:
            callee = stmt.spelling
            if callee not in INLINE_HELPERS:
                calls.append(callee)
            for c in stmt.get_children():
                walk_expr(c)
            return
        
        if kind in (clang.cindex.CursorKind.RETURN_STMT,):
            for c in stmt.get_children():
                walk_expr(c)
            return
        
        # Recurse into compound/control flow
        for c in stmt.get_children():
            if c.kind in (clang.cindex.CursorKind.COMPOUND_STMT,
                          clang.cindex.CursorKind.IF_STMT,
                          clang.cindex.CursorKind.WHILE_STMT,
                          clang.cindex.CursorKind.FOR_STMT,
                          clang.cindex.CursorKind.DO_STMT):
                walk_stmt(c)
            else:
                walk_expr(c)
    
    def walk_expr(expr, is_lhs=False):
        """Walk an expression."""
        nonlocal defs, uses
        kind = expr.kind
        
        if kind == clang.cindex.CursorKind.DECL_REF_EXPR:
            v = expr.spelling
            if is_tracked(v):
                if is_lhs:
                    defs.add(v)
                elif v not in defs:
                    uses.add(v)
            return
        
        if kind == clang.cindex.CursorKind.CALL_EXPR:
            for c in expr.get_children():
                walk_expr(c)
            return
        
        for c in expr.get_children():
            walk_expr(c, is_lhs)
    
    if body:
        for stmt in body.get_children():
            walk_stmt(stmt)
        
        # Also add flag operations from source text
        start_line = func_node.extent.start.line
        end_line = func_node.extent.end.line
        for lineno in range(start_line, end_line + 1):
            if lineno - 1 < len(lines):
                fd, fu = get_flag_defs_uses(lines[lineno - 1])
                defs |= fd
                uses |= fu
    
    return {
        'defs': defs, 'uses': uses, 'params': params, 'calls': calls,
        'body': body, 'lines': lines, 'start_line': func_node.extent.start.line,
    }

# ─── Compute live_in via forward scan on the AST ───
def compute_live_in(info):
    """Forward scan using AST structure."""
    params = info['params']
    defined = set(params)
    live_in = set()
    lines = info['lines']
    start_line = info['start_line']
    
    # Collect all flag uses from source as well
    flag_uses = set()
    flag_defs = set()
    
    def walk_stmt(stmt):
        nonlocal defined, live_in
        kind = stmt.kind
        
        if kind == clang.cindex.CursorKind.DECL_STMT:
            for decl in stmt.get_children():
                if decl.kind == clang.cindex.CursorKind.VAR_DECL:
                    v = decl.spelling
                    if is_tracked(v):
                        defined.add(v)
                    for c in decl.get_children():
                        walk_expr(c)
            return
        
        if kind == clang.cindex.CursorKind.BINARY_OPERATOR and stmt.spelling == '=':
            children = list(stmt.get_children())
            if children:
                for c in children[1:]:
                    walk_expr(c)
                walk_expr(children[0], is_lhs=True)
            return
        
        if kind == clang.cindex.CursorKind.COMPOUND_ASSIGNMENT_OPERATOR:
            children = list(stmt.get_children())
            if children:
                for c in children[1:]:
                    walk_expr(c)
                walk_expr(children[0], is_lhs=False)
                walk_expr(children[0], is_lhs=True)
            return
        
        if kind == clang.cindex.CursorKind.UNARY_OPERATOR and stmt.spelling in ('++', '--'):
            children = list(stmt.get_children())
            if children:
                walk_expr(children[0], is_lhs=False)
                walk_expr(children[0], is_lhs=True)
            return
        
        if kind == clang.cindex.CursorKind.CALL_EXPR:
            for c in stmt.get_children():
                walk_expr(c)
            return
        
        if kind == clang.cindex.CursorKind.RETURN_STMT:
            for c in stmt.get_children():
                walk_expr(c)
            return
        
        for c in stmt.get_children():
            if c.kind in (clang.cindex.CursorKind.COMPOUND_STMT,
                          clang.cindex.CursorKind.IF_STMT,
                          clang.cindex.CursorKind.WHILE_STMT,
                          clang.cindex.CursorKind.FOR_STMT,
                          clang.cindex.CursorKind.DO_STMT):
                walk_stmt(c)
            else:
                walk_expr(c)
    
    def walk_expr(expr, is_lhs=False):
        nonlocal defined, live_in
        kind = expr.kind
        
        if kind == clang.cindex.CursorKind.DECL_REF_EXPR:
            v = expr.spelling
            if is_tracked(v):
                if is_lhs:
                    defined.add(v)
                elif v not in defined:
                    live_in.add(v)
            return
        
        if kind == clang.cindex.CursorKind.CALL_EXPR:
            for c in expr.get_children():
                walk_expr(c)
            return
        
        for c in expr.get_children():
            walk_expr(c, is_lhs)
    
    if info['body']:
        for stmt in info['body'].get_children():
            walk_stmt(stmt)
    
    # Add flag live_in
    # Scan source text for flag uses/defs in this function
    if lines:
        start = info['start_line']
        end = start + (info['body'].extent.end.line - start) if info['body'] else start
        defined_flags = set()
        for lineno in range(start, end + 1):
            if lineno - 1 >= len(lines):
                break
            line = lines[lineno - 1]
            fd, fu = get_flag_defs_uses(line)
            defined_flags |= fd
            for f in fu:
                if f not in defined_flags:
                    live_in.add(f)
    
    return live_in

# ─── Formatting (compatible with old module) ───
def format_vars(var_set):
    regs = sorted(v for v in var_set if v in REGISTERS)
    bits = sorted(v[6] for v in var_set if v.startswith('flags:'))
    tmps = sorted(v for v in var_set if v in TMP_VARS)
    bytes = sorted(v for v in var_set if v in BYTE_VARS)
    ptrs = sorted(v for v in var_set if v in PTR_VARS)
    parts = []
    if regs:
        parts.append(', '.join(regs))
    if bits:
        parts.append('|'.join(bits))
    if tmps:
        parts.append(', '.join(tmps))
    if bytes:
        parts.append(', '.join(bytes))
    if ptrs:
        parts.append(', '.join(ptrs))
    return '; '.join(parts) if parts else '(none)'

# ─── Placeholder for detect_call (for compatibility) ───
def detect_call(stripped):
    m = re.search(r'\b(\w+)\s*\(', stripped)
    if m:
        name = m.group(1)
        if name not in INLINE_HELPERS and name not in ('if', 'while', 'for', 'switch', 'sizeof'):
            return name
    return None

# ─── Main analysis ───
def analyze_files(files):
    SRC_FILES = files
    
    # Parse all files and collect function info
    func_infos = {}
    all_funcs = {}
    
    for filepath in SRC_FILES:
        if not os.path.exists(filepath):
            continue
        tu = parse_file(filepath)
        with open(filepath) as f:
            lines = f.readlines()
        
        for name, start_line, end_line, node in find_functions_ast(tu, filepath):
            info = analyze_func(node, lines, filepath)
            live_in = compute_live_in(info)
            func_infos[name] = {
                'file': filepath,
                'defs': info['defs'],
                'uses': info['uses'],
                'live_in': live_in,
                'live_out': set(),
                'params': info['params'],
                'calls': info['calls'],
                'start_line': start_line,
            }
            all_funcs[name] = (filepath, start_line, end_line, node, lines)
    
    # Build summaries
    summaries = {}
    for name, info in func_infos.items():
        li = info['live_in']
        lo = info['live_out']
        # locals = (defs & uses) - live_in - live_out - passed_to_callees
        # For now, just compute (defs & uses) - live_in - live_out
        passed = set()
        for callee in info['calls']:
            if callee in func_infos:
                passed |= func_infos[callee].get('params', set())
        
        locals_set = ((info['defs'] & info['uses']) - li - lo - passed)
        
        summaries[name] = {
            'defs': info['defs'],
            'uses': info['uses'],
            'live_in': li,
            'live_out': lo,
            'locals': locals_set,
            'file': info['file'],
            'calls': info['calls'],
        }
    
    return summaries


if __name__ == '__main__':
    files = sys.argv[1:] if len(sys.argv) > 1 else [
        'src/view.c', 'src/editor.c', 'src/printing.c',
        'src/document.c', 'src/cli.c'
    ]
    
    summaries = analyze_files(files)
    
    by_file = {}
    for name, s in summaries.items():
        by_file.setdefault(s['file'], []).append((name, s))
    
    for filepath in sorted(by_file):
        funcs = by_file[filepath]
        print(f"\n{'='*70}")
        print(f"  {filepath}")
        print(f"{'='*70}")
        for name, s in sorted(funcs, key=lambda x: x[0]):
            calls_str = ', '.join(s['calls'][:5])
            if len(s['calls']) > 5:
                calls_str += f' … ({len(s["calls"])} total)'
            print(f"\n  {name}")
            print(f"    Live in:  {format_vars(s['live_in'])}")
            print(f"    Live out: {format_vars(s['live_out'])}")
            if s['locals']:
                print(f"    Locals:   {format_vars(s['locals'])}")
            if s['calls']:
                print(f"    Calls:    {calls_str}")
