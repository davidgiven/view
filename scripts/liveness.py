#!/usr/bin/env python3
"""
Liveness analysis using libclang AST for def/use detection,
regex for flag ops, and full interprocedural fixed-point iteration.

Usage:
    python3 scripts/liveness.py [src/*.c ...]
"""

import clang.cindex, os, subprocess, re, sys

# ─── Library setup ────────────────────────────────────────────────
try:
    clang.cindex.Config.set_library_file('/usr/lib64/libclang.so')
except Exception:
    pass
RESOURCE_DIR = subprocess.run(['clang', '-print-resource-dir'],
                               capture_output=True, text=True).stdout.strip()
CPPFLAGS = ['-I.', '-Isrc', f'-I{RESOURCE_DIR}/include',
            '-D__attribute__(x)=', '-D__inline__=', '-D__restrict__=']

# ─── Tracked variables ────────────────────────────────────────────
REGISTERS = ['a', 'x', 'y']
FLAG_BITS = ['C', 'Z', 'N', 'V']
TMP_VARS = ['tmp01', 'tmp23', 'tmp45', 'tmp67', 'tmp89']
BYTE_VARS = [f'tmp{i}' for i in range(10)]
PTR_VARS = ['ptr1', 'ptr2', 'ptr3', 'ptr5', 'ptr6']
ALL_VARS_SET = set(REGISTERS + TMP_VARS + BYTE_VARS + PTR_VARS +
                   [f'flags:{b}' for b in FLAG_BITS])

TRACKED_VARS = set(REGISTERS + TMP_VARS + BYTE_VARS + PTR_VARS)

# ─── Inline / lib helpers ─────────────────────────────────────────
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

# ─── Inline helper flag definitions/uses ──────────────────────────
HELPER_FLAG_DEFS = {
    'set_flags': {'Z', 'N'},
    'cmp':  {'Z', 'N', 'C'},
    'adc':  {'C', 'Z', 'N', 'V'},
    'sbc':  {'C', 'Z', 'N', 'V'},
    'bit':  {'Z', 'N', 'V'},
    'rol':  {'C', 'Z', 'N'},
    'ror':  {'C', 'Z', 'N'},
    'asr':  {'Z', 'N', 'C'},
}
HELPER_FLAG_USES = {
    'adc':  {'C'},
    'sbc':  {'C'},
    'rol':  {'C'},
    'ror':  {'C'},
    'bit':  {'V'},
}
def helper_flag_defs(name):
    return HELPER_FLAG_DEFS.get(name, set())
def helper_flag_uses(name):
    return HELPER_FLAG_USES.get(name, set())

# ─── Byte-to-combined mapping ─────────────────────────────────────
BYTE_TO_COMBINED = {
    'tmp0': 'tmp01', 'tmp1': 'tmp01',
    'tmp2': 'tmp23', 'tmp3': 'tmp23',
    'tmp4': 'tmp45', 'tmp5': 'tmp45',
    'tmp6': 'tmp67', 'tmp7': 'tmp67',
    'tmp8': 'tmp89', 'tmp9': 'tmp89',
}
COMBINED_TO_BYTES = {
    'tmp01': ['tmp0', 'tmp1'], 'tmp23': ['tmp2', 'tmp3'],
    'tmp45': ['tmp4', 'tmp5'], 'tmp67': ['tmp6', 'tmp7'],
    'tmp89': ['tmp8', 'tmp9'],
}

# ─── CORRUPTS & ALL_IN_OUT ────────────────────────────────────────
CORRUPTS = {
    'cli_putstring': {'a', 'x', 'flags:C', 'flags:Z', 'flags:N', 'flags:V'},
    'return_to_cli_prompt': ALL_VARS_SET,
    'return_to_editor_loop': ALL_VARS_SET,
    'cmd_err_no_string': ALL_VARS_SET,
    'cmd_err_no_target': ALL_VARS_SET,
    'file_not_found_error': ALL_VARS_SET,
    'file_error': ALL_VARS_SET,
    'display_not_enough_memory': ALL_VARS_SET,
    'bad_filename_error': ALL_VARS_SET,
    'nested_macro_error': ALL_VARS_SET,
}

ALL_IN_OUT = {
    'call_printer_driver',
    'reset_area_to_marks_1_2',
    'sub_caef4',
}

# ─── AST parsing (cached) ─────────────────────────────────────────
_parse_cache = {}
def parse_file(filepath):
    if filepath not in _parse_cache:
        idx = clang.cindex.Index.create()
        tu = idx.parse(filepath, CPPFLAGS)
        _parse_cache[filepath] = tu
    return _parse_cache[filepath]

# ─── Flag ops from source text (kept: FLAG_X macros expand in AST) ─
def get_flag_defs_uses(line_text):
    defs = set()
    uses = set()
    for bit in FLAG_BITS:
        if re.search(r'flags\s*&=\s*~FLAG_' + bit, line_text):
            defs.add(f'flags:{bit}')
        elif re.search(r'flags\s*\|=\s*FLAG_' + bit, line_text):
            defs.add(f'flags:{bit}')
        elif re.search(r'flags\s*=\s*flags\s*[&|].*FLAG_' + bit, line_text):
            defs.add(f'flags:{bit}'); uses.add(f'flags:{bit}')
        elif re.search(r'if\s*\(.*flags\s*&\s*FLAG_' + bit, line_text):
            uses.add(f'flags:{bit}')
    return defs, uses

# ─── Call detection (regex on line text) ──────────────────────────
CALL_RE = re.compile(r'\b(\w+)\s*\(')
def detect_call(stripped):
    if stripped.startswith('//') or stripped == '':
        return None
    if stripped.startswith('if ') or stripped.startswith('while ') or stripped.startswith('for '):
        return None
    if re.match(r'^(?:static\s+)?(?:\w+\s+)+\**\w+\s*\(', stripped) and '{' not in stripped:
        return None
    m = CALL_RE.search(stripped)
    if m:
        name = m.group(1)
        if name not in INLINE_HELPERS and name not in ('if', 'while', 'for', 'switch', 'sizeof'):
            return name
    return None


# ─── AST-based def/use analysis ───────────────────────────────────
def analyze_stmt(cursor, local_decls, context='use'):
    """
    Recursively walk an AST cursor to find defs/uses of tracked globals.

    Args:
        cursor: libclang cursor
        local_decls: set of local var names (modified in-place for VarDecl/ParmDecl)
        context: 'def' (LHS of =), 'use' (RHS), 'defuse' (compound/++/--)

    Returns:
        (defs_set, uses_set)
    """
    d, u = set(), set()
    if cursor is None:
        return d, u

    ck = cursor.kind
    name = cursor.spelling

    # ── DeclRefExpr: variable reference ──
    if ck == clang.cindex.CursorKind.DECL_REF_EXPR:
        if name and name in TRACKED_VARS and name not in local_decls:
            if context == 'def':
                d.add(name)
            elif context == 'defuse':
                d.add(name); u.add(name)
            else:
                u.add(name)
        return d, u

    # ── VarDecl: local declaration ──
    if ck == clang.cindex.CursorKind.VAR_DECL:
        if name and name in TRACKED_VARS:
            local_decls.add(name)
        for child in cursor.get_children():
            cd, cu = analyze_stmt(child, local_decls, 'use')
            d.update(cd); u.update(cu)
        return d, u

    # ── ParmDecl: function parameter ──
    if ck == clang.cindex.CursorKind.PARM_DECL:
        if name and name in TRACKED_VARS:
            local_decls.add(name)
        return d, u

    # ── Binary operators ──
    if ck == clang.cindex.CursorKind.BINARY_OPERATOR:
        op = cursor.spelling
        children = list(cursor.get_children())
        if op == '=':
            if len(children) >= 2:
                ld, lu = analyze_stmt(children[0], local_decls, 'def')
                for ch in children[1:]:
                    rd, ru = analyze_stmt(ch, local_decls, 'use')
                    d.update(rd); u.update(ru)
                d.update(ld); u.update(lu)
        elif op in ('+=', '-=', '*=', '/=', '%=', '&=', '|=', '^=', '<<=', '>>='):
            if len(children) >= 2:
                ld, lu = analyze_stmt(children[0], local_decls, 'defuse')
                for ch in children[1:]:
                    rd, ru = analyze_stmt(ch, local_decls, 'use')
                    d.update(rd); u.update(ru)
                d.update(ld); u.update(lu)
        else:
            for child in children:
                cd, cu = analyze_stmt(child, local_decls, 'use')
                d.update(cd); u.update(cu)
        return d, u

    # ── Unary operators ──
    if ck == clang.cindex.CursorKind.UNARY_OPERATOR:
        children = list(cursor.get_children())
        if not children:
            return d, u
        # Check source text for ++/-- (postfix operators)
        try:
            with open(cursor.location.file.name) as _sf:
                _line = _sf.readlines()[cursor.location.line - 1]
        except Exception:
            _line = ''
        stripped_line = _line.strip()
        if '++' in stripped_line or '--' in stripped_line:
            return analyze_stmt(children[0], local_decls, 'defuse')
        # All other unary ops (!, &, *, -, ~) propagate context
        return analyze_stmt(children[0], local_decls, context)

    # ── ArraySubscriptExpr ──
    if ck == clang.cindex.CursorKind.ARRAY_SUBSCRIPT_EXPR:
        children = list(cursor.get_children())
        if children:
            cd, cu = analyze_stmt(children[0], local_decls, context)
            d.update(cd); u.update(cu)
            for child in children[1:]:
                cd, cu = analyze_stmt(child, local_decls, 'use')
                d.update(cd); u.update(cu)
        return d, u

    # ── Casts / parens: propagate context ──
    if ck in (clang.cindex.CursorKind.CSTYLE_CAST_EXPR,
              clang.cindex.CursorKind.PAREN_EXPR,
              clang.cindex.CursorKind.UNEXPOSED_EXPR):
        children = list(cursor.get_children())
        if not children:
            return d, u
        # Skip TYPE_REF children (type references like `uint16_t` in casts)
        for child in children:
            if child.kind != clang.cindex.CursorKind.TYPE_REF:
                cd, cu = analyze_stmt(child, local_decls, context)
                d.update(cd); u.update(cu)
        return d, u

    # ── CallExpr ──
    if ck == clang.cindex.CursorKind.CALL_EXPR:
        children = list(cursor.get_children())
        # First child is the callee expression
        callee_name = None
        start_idx = 0
        if children and children[0].kind in (
            clang.cindex.CursorKind.DECL_REF_EXPR,
            clang.cindex.CursorKind.UNEXPOSED_EXPR,
        ):
            # Unwrap UNEXPOSED_EXPR to find the actual name
            callee_cursor = children[0]
            if callee_cursor.kind == clang.cindex.CursorKind.UNEXPOSED_EXPR:
                gc = list(callee_cursor.get_children())
                if gc and gc[0].kind == clang.cindex.CursorKind.DECL_REF_EXPR:
                    callee_name = gc[0].spelling
            else:
                callee_name = callee_cursor.spelling
            start_idx = 1

        # Process arguments as uses
        for child in children[start_idx:]:
            cd, cu = analyze_stmt(child, local_decls, 'use')
            d.update(cd); u.update(cu)

        # Inline helper flag effects (detected by name, not AST)
        if callee_name in INLINE_HELPERS:
            for b in helper_flag_defs(callee_name):
                d.add(f'flags:{b}')
            for b in helper_flag_uses(callee_name):
                u.add(f'flags:{b}')
            # For cmp/set_flags, the second argument is a register read
            if callee_name in ('cmp', 'set_flags') and len(children) >= 3:
                cd, cu = analyze_stmt(children[2], local_decls, 'use')
                d.update(cd); u.update(cu)

        return d, u

    # ── Control flow: conditions/values are uses ──
    if ck in (clang.cindex.CursorKind.IF_STMT,
              clang.cindex.CursorKind.WHILE_STMT,
              clang.cindex.CursorKind.FOR_STMT,
              clang.cindex.CursorKind.DO_STMT,
              clang.cindex.CursorKind.SWITCH_STMT,
              clang.cindex.CursorKind.RETURN_STMT):
        # Body compound statements are collected as separate statements by the
        # line walker, so attributing their defs/uses to the control-flow line
        # breaks ordering (a use inside an else-branch would look live-in).
        # Only process the condition/value expressions and non-compound bodies.
        for child in cursor.get_children():
            if child.kind == clang.cindex.CursorKind.COMPOUND_STMT:
                continue
            cd, cu = analyze_stmt(child, local_decls, 'use')
            d.update(cd); u.update(cu)
        return d, u

    # ── CompoundStmt: recurse ──
    if ck == clang.cindex.CursorKind.COMPOUND_STMT:
        for child in cursor.get_children():
            cd, cu = analyze_stmt(child, local_decls, 'use')
            d.update(cd); u.update(cu)
        return d, u

    # ── Labels: recurse ──
    if ck in (clang.cindex.CursorKind.LABEL_STMT,
              clang.cindex.CursorKind.CASE_STMT,
              clang.cindex.CursorKind.DEFAULT_STMT):
        for child in cursor.get_children():
            cd, cu = analyze_stmt(child, local_decls, context)
            d.update(cd); u.update(cu)
        return d, u

    # ── DeclStmt: group of declarations ──
    if ck == clang.cindex.CursorKind.DECL_STMT:
        for child in cursor.get_children():
            cd, cu = analyze_stmt(child, local_decls, 'use')
            d.update(cd); u.update(cu)
        return d, u

    # ── No-ops ──
    if ck in (clang.cindex.CursorKind.GOTO_STMT,
              clang.cindex.CursorKind.BREAK_STMT,
              clang.cindex.CursorKind.CONTINUE_STMT,
              clang.cindex.CursorKind.NULL_STMT):
        return d, u

    # ── Default: recurse into children ──
    for child in cursor.get_children():
        cd, cu = analyze_stmt(child, local_decls, context)
        d.update(cd); u.update(cu)
    return d, u


# ─── Function body walker (returns list of (line, cursor) pairs) ──
def _collect_top_stmts(body_cursor):
    """
    Collect top-level statements from a compound statement.
    Returns a list of (line_number, cursor) pairs sorted by line.
    """
    result = []
    _top_stmts_recurse(body_cursor, result)
    # Deduplicate by line (keep first occurrence)
    seen = set()
    deduped = []
    for line, c in result:
        if line not in seen:
            seen.add(line)
            deduped.append((line, c))
    deduped.sort(key=lambda x: x[0])
    return deduped


def _top_stmts_recurse(cursor, result):
    """Recursively collect all statement-level cursors with their start lines."""
    ck = cursor.kind
    if ck == clang.cindex.CursorKind.PARM_DECL:
        return
    if ck == clang.cindex.CursorKind.VAR_DECL:
        result.append((cursor.location.line, cursor))
        return
    if ck == clang.cindex.CursorKind.DECL_STMT:
        for ch in cursor.get_children():
            _top_stmts_recurse(ch, result)
        return
    if ck == clang.cindex.CursorKind.LABEL_STMT:
        result.append((cursor.location.line, cursor))
        for ch in cursor.get_children():
            _top_stmts_recurse(ch, result)
        return
    if ck in (clang.cindex.CursorKind.GOTO_STMT,
              clang.cindex.CursorKind.BREAK_STMT,
              clang.cindex.CursorKind.CONTINUE_STMT,
              clang.cindex.CursorKind.NULL_STMT,
              clang.cindex.CursorKind.RETURN_STMT):
        result.append((cursor.location.line, cursor))
        return
    if ck == clang.cindex.CursorKind.COMPOUND_STMT:
        for ch in cursor.get_children():
            _top_stmts_recurse(ch, result)
        return
    if ck in (clang.cindex.CursorKind.IF_STMT,
              clang.cindex.CursorKind.WHILE_STMT,
              clang.cindex.CursorKind.FOR_STMT,
              clang.cindex.CursorKind.DO_STMT,
              clang.cindex.CursorKind.SWITCH_STMT,
              clang.cindex.CursorKind.CASE_STMT,
              clang.cindex.CursorKind.DEFAULT_STMT):
        result.append((cursor.location.line, cursor))
        for ch in cursor.get_children():
            _top_stmts_recurse(ch, result)
        return
    # Expression statements: collect, then recurse into children
    result.append((cursor.location.line, cursor))
    for ch in cursor.get_children():
        _top_stmts_recurse(ch, result)


def get_func_params_from_ast(func_cursor):
    """Extract parameter names that are tracked variables."""
    params = set()
    for child in func_cursor.get_children():
        if child.kind == clang.cindex.CursorKind.PARM_DECL:
            if child.spelling in REGISTERS:
                params.add(child.spelling)
    return params


# ─── AST-based forward scan ───────────────────────────────────────
def get_local_info_ast(func_cursor, callee_live_out=None, callee_live_in=None):
    """
    Forward scan using AST-based def/use analysis.

    Returns (local_defs, local_uses, local_live_in, call_sites).
    """
    if callee_live_out is None:
        callee_live_out = {}
    if callee_live_in is None:
        callee_live_in = {}

    local_defs = set()
    local_uses = set()
    local_live_in = set()
    defined_so_far = set()
    call_sites = []
    local_decls = set()

    # Add parameters to local_decls and defined_so_far
    for child in func_cursor.get_children():
        if child.kind == clang.cindex.CursorKind.PARM_DECL:
            if child.spelling in TRACKED_VARS:
                local_decls.add(child.spelling)
                defined_so_far.add(child.spelling)

    # Get the function body (compound statement)
    body_children = list(func_cursor.get_children())
    body = None
    for child in body_children:
        if child.kind == clang.cindex.CursorKind.COMPOUND_STMT:
            body = child
            break

    if body is None:
        return local_defs, local_uses, local_live_in, call_sites

    # Collect the source lines for this function for flag regex
    try:
        with open(func_cursor.location.file.name) as f:
            source_lines = f.readlines()
    except Exception:
        source_lines = []

    # Walk body statements
    stmts = _collect_top_stmts(body)

    for line_num, stmt in stmts:
        if line_num > 0 and line_num <= len(source_lines):
            stripped = source_lines[line_num - 1].strip()
        else:
            stripped = ''

        if stripped.startswith('//') or stripped == '':
            continue

        d = set()
        u = set()

        # AST-based def/use analysis for tracked variables
        cd, cu = analyze_stmt(stmt, local_decls, 'use')
        d.update(cd)
        u.update(cu)

        # Byte/combined variable propagation
        for bv, cv in BYTE_TO_COMBINED.items():
            if bv in d: d.add(cv)
            if bv in u: u.add(cv)
        for cv, blist in COMBINED_TO_BYTES.items():
            if cv in d:
                for bv in blist: d.add(bv)
            if cv in u:
                for bv in blist: u.add(bv)

        # Flag ops via regex (macros expand in AST)
        fd, fu = get_flag_defs_uses(stripped)
        d |= fd
        u |= fu

        # Update live_in
        for v in u:
            if v not in defined_so_far:
                local_live_in.add(v)

        local_defs |= d
        local_uses |= u
        defined_so_far |= d

        # Detect calls via regex on source line (handles nested calls)
        callee = detect_call(stripped)
        if callee:
            if callee not in LIB_FUNCTIONS:
                args = {v for v in u if v in TRACKED_VARS and not v.startswith('flags:')}
                call_sites.append((line_num, callee, args))
                # Variables the callee needs as INPUT must be live before the
                # call.  Check against the caller's own definitions so far
                # (NOT the callee's kill set, which masks inputs the callee
                # also writes).
                cli = callee_live_in.get(callee)
                if cli is None and callee in ALL_IN_OUT:
                    cli = ALL_VARS_SET
                if cli is not None:
                    for v in cli:
                        if v not in defined_so_far:
                            local_live_in.add(v)
                            local_uses.add(v)
                # The callee's live_out is a kill set: after the call those
                # globals are redefined.
                clo = callee_live_out.get(callee)
                if clo is None:
                    if callee in CORRUPTS:
                        pass
                    elif callee in ALL_IN_OUT:
                        clo = ALL_VARS_SET
                    else:
                        clo = ALL_VARS_SET
                if clo is not None:
                    defined_so_far.update(clo)

    return local_defs, local_uses, local_live_in, call_sites


# ─── AST-based backward scan ──────────────────────────────────────
def _backward_stmt(cur, live, backward_needs, local_info, func_params,
                   local_decls, source_lines, func_live_out):
    """Recursive branch-aware backward dataflow.

    Returns the live set that must hold BEFORE `cur` executes, given that
    `live` must hold AFTER it.  `if`/`switch` branches are analysed separately
    and merged, so a variable needed on one branch does not leak into a sibling
    branch (the previous linear scan leaked, e.g. switch cases).
    """
    ck = cur.kind

    # Compound statement: process children in reverse order.
    if ck == clang.cindex.CursorKind.COMPOUND_STMT:
        for ch in reversed(list(cur.get_children())):
            live = _backward_stmt(ch, live, backward_needs, local_info,
                                  func_params, local_decls, source_lines,
                                  func_live_out)
        return live

    # if / else: merge the two branches at the condition.
    if ck == clang.cindex.CursorKind.IF_STMT:
        children = list(cur.get_children())
        branches = children[1:]
        merged = set(live)
        for b in branches:
            merged |= _backward_stmt(b, set(live), backward_needs, local_info,
                                     func_params, local_decls, source_lines,
                                     func_live_out)
        d, u = _analyze_stmt_effect(cur, local_decls, source_lines)
        return (merged - d) | u

    # switch: each case is an independent branch.
    if ck == clang.cindex.CursorKind.SWITCH_STMT:
        children = list(cur.get_children())
        merged = set(live)
        for ch in children:
            if ch.kind == clang.cindex.CursorKind.COMPOUND_STMT:
                # process each case independently (do NOT thread linearly)
                for case in ch.get_children():
                    merged |= _backward_stmt(case, set(live), backward_needs,
                                             local_info, func_params,
                                             local_decls, source_lines,
                                             func_live_out)
            elif ch.kind in (clang.cindex.CursorKind.CASE_STMT,
                             clang.cindex.CursorKind.DEFAULT_STMT):
                merged |= _backward_stmt(ch, set(live), backward_needs,
                                         local_info, func_params, local_decls,
                                         source_lines, func_live_out)
        d, u = _analyze_stmt_effect(cur, local_decls, source_lines)
        return (merged - d) | u

    # case / default labels: treat as straight-line.
    if ck in (clang.cindex.CursorKind.CASE_STMT,
              clang.cindex.CursorKind.DEFAULT_STMT,
              clang.cindex.CursorKind.LABEL_STMT):
        for ch in cur.get_children():
            live = _backward_stmt(ch, live, backward_needs, local_info,
                                  func_params, local_decls, source_lines,
                                  func_live_out)
        return live

    # Loops: iterate body + condition to a fixed point (back-edge).
    if ck in (clang.cindex.CursorKind.WHILE_STMT,
              clang.cindex.CursorKind.FOR_STMT,
              clang.cindex.CursorKind.DO_STMT):
        prev = None
        result = set(live)
        # body runs, then (for while/for) the condition gates re-entry.
        for _ in range(20):
            body_in = set(live)
            for ch in cur.get_children():
                if ch.kind == clang.cindex.CursorKind.COMPOUND_STMT:
                    body_in = _backward_stmt(ch, body_in, backward_needs,
                                             local_info, func_params,
                                             local_decls, source_lines,
                                             func_live_out)
            d, u = _analyze_stmt_effect(cur, local_decls, source_lines)
            candidate = (body_in - d) | u
            candidate |= live
            if candidate == prev:
                break
            prev = candidate
            live = candidate
        return result | (prev if prev is not None else set())

    # return: exit resets to the function's live-out.
    if ck == clang.cindex.CursorKind.RETURN_STMT:
        return set(func_live_out)

    return _backward_plain(cur, live, backward_needs, local_info, func_params,
                           local_decls, source_lines, func_live_out)


def _analyze_stmt_effect(cur, local_decls, source_lines):
    """defs/uses of a statement (with byte/flag propagation)."""
    d, u = analyze_stmt(cur, set(local_decls), 'use')
    for bv, cv in BYTE_TO_COMBINED.items():
        if bv in d:
            d.add(cv)
        if bv in u:
            u.add(cv)
    for cv, blist in COMBINED_TO_BYTES.items():
        if cv in d:
            for bv in blist:
                d.add(bv)
        if cv in u:
            for bv in blist:
                u.add(bv)
    if cur.location.line > 0 and cur.location.line <= len(source_lines):
        fd, fu = get_flag_defs_uses(source_lines[cur.location.line - 1])
        d |= fd
        u |= fu
    return d, u


def _backward_plain(cur, live, backward_needs, local_info, func_params,
                    local_decls, source_lines, func_live_out):
    """Backward handling for a straight-line (leaf) statement."""
    line_num = cur.location.line
    if 0 < line_num <= len(source_lines):
        stripped = source_lines[line_num - 1].strip()
    else:
        stripped = ''

    callee_name = detect_call(stripped)
    if callee_name:
        if callee_name in ALL_IN_OUT:
            backward_needs.setdefault(callee_name, set()).update(ALL_VARS_SET)
            return ALL_VARS_SET
        if callee_name not in LIB_FUNCTIONS:
            backward_needs.setdefault(callee_name, set()).update(live)
            callee_info = local_info.get(callee_name, {})
            callee_defs = callee_info.get('cached_defs', callee_info.get('defs', set()))
            callee_params = func_params.get(callee_name, set())
            kill_set = callee_defs - callee_params
            if callee_name in CORRUPTS:
                kill_set = kill_set | CORRUPTS[callee_name]
            d, u = _analyze_stmt_effect(cur, local_decls, source_lines)
            live = (live - kill_set) | u
            cli = callee_info.get('live_in', set()) - callee_params
            live |= cli
            return live
        if callee_name in CORRUPTS:
            corr = CORRUPTS[callee_name]
            d, u = _analyze_stmt_effect(cur, local_decls, source_lines)
            return (live - corr) | u
        d, u = _analyze_stmt_effect(cur, local_decls, source_lines)
        return live | u

    d, u = _analyze_stmt_effect(cur, local_decls, source_lines)
    return (live - d) | u


def backward_scan_ast(func_cursor, backward_needs, local_info, func_params, live_out_start=None):
    """
    Backward scan using AST for a single function.
    Modifies backward_needs in place.
    """
    if live_out_start is None:
        live_out_start = set()
    local_decls = set()
    for child in func_cursor.get_children():
        if child.kind == clang.cindex.CursorKind.PARM_DECL:
            if child.spelling in TRACKED_VARS:
                local_decls.add(child.spelling)

    try:
        with open(func_cursor.location.file.name) as f:
            source_lines = f.readlines()
    except Exception:
        source_lines = []

    # Get body statements in order
    body_children = list(func_cursor.get_children())
    body = None
    for child in body_children:
        if child.kind == clang.cindex.CursorKind.COMPOUND_STMT:
            body = child
            break
    if body is None:
        return

    func_live_out = set(live_out_start)
    _backward_stmt(body, set(func_live_out), backward_needs, local_info,
                   func_params, local_decls, source_lines, func_live_out)


# ─── Find functions (regex on source lines) ────────────────────────
FUNC_START_RE = re.compile(
    r'^(?:static\s+)?'
    r'(?:void|uint8_t|uint16_t|addr_t|bool|int|char|long|unsigned|const|'
    r'struct\s+\w+|uint8_t\s*\*|char\s*\*)'
    r'\s+\**(\w+)\s*\('
)

def find_functions(lines):
    funcs = []
    for i, line in enumerate(lines):
        stripped = line.strip()
        if (not stripped or stripped.startswith('//') or stripped.startswith('/*')
            or stripped.startswith('*') or stripped.startswith('#')
            or stripped.endswith(';')):
            continue
        m = FUNC_START_RE.match(stripped)
        if m and m.group(1) not in INLINE_HELPERS:
            before_paren = stripped.split('(')[0]
            kw = re.split(r'\W+', before_paren)[0]
            if kw not in ('if', 'while', 'for', 'switch'):
                funcs.append((m.group(1), i))
    result = []
    for idx, (name, start) in enumerate(funcs):
        end = funcs[idx + 1][1] if idx + 1 < len(funcs) else len(lines)
        result.append((name, start, end))
    return result


# ─── Formatting ────────────────────────────────────────────────────
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


# ─── Interprocedural analysis ─────────────────────────────────────
def analyze_files(files):
    # Parse all files and collect function cursors
    all_funcs = {}  # name -> (filepath, cursor)
    for filepath in files:
        try:
            tu = parse_file(filepath)
        except Exception:
            continue
        for cursor in tu.cursor.get_children():
            if cursor.kind == clang.cindex.CursorKind.FUNCTION_DECL and cursor.is_definition():
                name = cursor.spelling
                if name not in all_funcs:
                    all_funcs[name] = (filepath, cursor)

    # Compute function params
    func_params = {}
    for name, (filepath, cursor) in all_funcs.items():
        func_params[name] = get_func_params_from_ast(cursor)

    # Initial forward pass (using AST)
    local_info = {}
    for name, (filepath, cursor) in all_funcs.items():
        d, u, li, cs = get_local_info_ast(cursor)

        # Compute local declarations for this function
        local_decls = set()
        for child in cursor.get_children():
            if child.kind == clang.cindex.CursorKind.PARM_DECL:
                if child.spelling in TRACKED_VARS:
                    local_decls.add(child.spelling)
        # Walk the body for VarDecl
        for child in cursor.get_children():
            if child.kind == clang.cindex.CursorKind.COMPOUND_STMT:
                _collect_vardecls(child, local_decls)

        local_info[name] = {
            'defs': d, 'uses': u, 'live_in': li,
            'call_sites': cs, 'file': filepath,
            'local_decls': local_decls,
        }

    # Propagate through call graph (cached_defs/cached_uses)
    changed = True
    iteration = 0
    while changed and iteration < 20:
        changed = False
        iteration += 1
        for name in all_funcs:
            info = local_info[name]
            all_defs = set(info['defs'])
            all_uses = set(info['uses'])
            for _, callee, _ in info['call_sites']:
                if callee in LIB_FUNCTIONS:
                    continue
                callee_info = local_info.get(callee)
                callee_params = func_params.get(callee, set())
                if callee_info is None:
                    all_defs.update(ALL_VARS_SET)
                    all_uses.update(ALL_VARS_SET)
                else:
                    cd = callee_info.get('cached_defs', callee_info['defs'])
                    cu = callee_info.get('cached_uses', callee_info['uses'])
                    all_defs.update(cd - callee_params)
                    all_uses.update(cu - callee_params)
            old_d = info.get('cached_defs')
            old_u = info.get('cached_uses')
            if all_defs != old_d or all_uses != old_u:
                info['cached_defs'] = all_defs
                info['cached_uses'] = all_uses
                changed = True

    # ── Full interprocedural fixed-point iteration ──
    backward_needs = {name: set() for name in all_funcs}
    fp_iter = 0
    changed = True
    while changed and fp_iter < 10:
        fp_iter += 1
        changed = False

        # Backward scan using AST
        backward_needs.clear()
        for name in all_funcs:
            info = local_info[name]
            cursor = all_funcs[name][1]
            backward_scan_ast(cursor, backward_needs, local_info, func_params,
                              info.get('live_out', set()))

        # Compute live_out
        for name in all_funcs:
            info = local_info[name]
            all_defs = info.get('cached_defs', info['defs'])
            required = backward_needs.get(name, set())
            new_out = required & all_defs

            if name in ALL_IN_OUT:
                new_out = ALL_VARS_SET

            old_out = info.get('live_out')
            if new_out != old_out:
                info['live_out'] = new_out
                changed = True

        # Re-forward scan with refined kill sets
        clo = {}
        for cname, cinfo in local_info.items():
            callee_defs = cinfo.get('cached_defs', cinfo['defs'])
            clo[cname] = callee_defs - func_params.get(cname, set())
        for name in ALL_IN_OUT:
            clo[name] = ALL_VARS_SET
        for name in CORRUPTS:
            if name not in local_info:
                clo[name] = CORRUPTS[name]

        cli = {}
        for cname, cinfo in local_info.items():
            cli[cname] = cinfo.get('live_in', set())
        for name in ALL_IN_OUT:
            cli[name] = ALL_VARS_SET

        for name, (filepath, cursor) in all_funcs.items():
            d, u, li, cs = get_local_info_ast(cursor, clo, cli)
            old_li = local_info[name].get('live_in')
            if li != old_li:
                local_info[name]['live_in'] = li
                changed = True

    # Build summaries
    summaries = {}
    for name in all_funcs:
        info = local_info[name]
        all_defs = info.get('cached_defs', info['defs'])
        all_uses = info.get('cached_uses', info['uses'])
        live_in = info.get('live_in', set())
        live_out = info.get('live_out', set())

        if name in ALL_IN_OUT:
            live_in = ALL_VARS_SET
            live_out = ALL_VARS_SET

        local_defs = info.get('defs', set())
        local_uses = info.get('uses', set())
        local_decls_func = info.get('local_decls', set())

        # passed_to_callees
        passed_to_callees = set()
        cursor = all_funcs[name][1]
        try:
            with open(all_funcs[name][0]) as f:
                flines = f.readlines()
        except Exception:
            flines = []

        stmts = []
        for child in cursor.get_children():
            if child.kind == clang.cindex.CursorKind.COMPOUND_STMT:
                stmts = _collect_top_stmts(child)
                break

        for line_num, stmt in stmts:
            if line_num > 0 and line_num <= len(flines):
                sl = flines[line_num - 1].strip()
            else:
                sl = ''
            if sl.startswith('//') or sl == '':
                continue

            callee_name = None
            if stmt.kind == clang.cindex.CursorKind.CALL_EXPR:
                children = list(stmt.get_children())
                if children and children[0].kind == clang.cindex.CursorKind.DECL_REF_EXPR:
                    callee_name = children[0].spelling
                elif children and children[0].kind == clang.cindex.CursorKind.UNEXPOSED_EXPR:
                    gc = list(children[0].get_children())
                    if gc and gc[0].kind == clang.cindex.CursorKind.DECL_REF_EXPR:
                        callee_name = gc[0].spelling

            if callee_name and callee_name not in INLINE_HELPERS:
                callee_params = func_params.get(callee_name, set())
                # Variables passed as call arguments.  Arguments matching the
                # callee's explicit parameters are copied by value into the
                # callee's own locals, so the caller's global is just a scratch
                # temporary, not consumed by the callee.  Only non-parameter
                # arguments (register-convention globals) are consumed here.
                for var in TRACKED_VARS:
                    if var in local_decls_func or var in callee_params:
                        continue
                    if re.search(r'(?<!\w)' + var + r'(?!\w)', sl):
                        if not re.search(r'\b' + var + r'\s*=', sl):
                            passed_to_callees.add(var)

                # Callee's defs and params
                callee_info = local_info.get(callee_name)
                if callee_info:
                    passed_to_callees |= (callee_info.get('live_in', set()) - callee_params)
                    callee_defs = callee_info.get('defs', set())
                    callee_locals = callee_info.get('local_decls', set())
                    passed_to_callees |= (callee_defs - callee_locals - callee_params)

        globals_used_locally = ((local_defs & local_uses) - live_in - live_out - passed_to_callees - local_decls_func)
        globals_used_locally = {v for v in globals_used_locally if not v.startswith('flags:')}

        summaries[name] = {
            'defs': all_defs,
            'uses': all_uses,
            'live_in': live_in,
            'live_out': live_out,
            'locals': globals_used_locally,
            'consumed': set(),
            'file': info['file'],
            'calls': [(c, a) for _, c, a in info['call_sites']],
            'passed_to_callees': passed_to_callees,
            'local_decls': local_decls_func,
        }

    # ── Post-process: propagate consumed sets transitively ──
    changed = True
    while changed:
        changed = False
        for name in all_funcs:
            s = summaries.get(name)
            if not s:
                continue
            consumed = set(s.get('passed_to_callees', set()))
            for callee, args in s['calls']:
                cs = summaries.get(callee)
                if cs:
                    # Propagate the callee's live_in and transitive consumed in
                    # full.  A callee's own explicit parameters never appear in
                    # its live_in/consumed (they are local_decls there), so this
                    # carries only genuine register-convention GLOBAL reads --
                    # both direct and transitive (e.g. print_char() reading the
                    # global a through print_char_x_times()).
                    consumed.update(cs.get('live_in', set()))
                    consumed.update(cs.get('consumed', set()))
                elif callee in CORRUPTS:
                    consumed.update(ALL_VARS_SET)
            consumed = consumed - s['live_in'] - s['live_out']
            consumed = consumed - s.get('local_decls', set())
            if consumed != s['consumed']:
                s['consumed'] = consumed
                changed = True

    # ── Recompute Scratch (locals) to subtract transitive consumed ──
    for name in all_funcs:
        s = summaries.get(name)
        if not s:
            continue
        consumed = s.get('consumed', set())
        local_decls = s.get('local_decls', set())
        old_locals = s.get('locals', set())
        new_locals = old_locals - consumed - local_decls
        if new_locals != old_locals:
            s['locals'] = new_locals

    return summaries


def _collect_vardecls(cursor, local_decls):
    """Recursively collect VarDecl names from AST."""
    for child in cursor.get_children():
        if child.kind == clang.cindex.CursorKind.VAR_DECL:
            if child.spelling in TRACKED_VARS:
                local_decls.add(child.spelling)
        elif child.kind in (clang.cindex.CursorKind.COMPOUND_STMT,
                            clang.cindex.CursorKind.DECL_STMT):
            _collect_vardecls(child, local_decls)


# ─── Main ─────────────────────────────────────────────────────────
def main():
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
            calls_str = ', '.join(c for c, _ in s['calls'][:5])
            if len(s['calls']) > 5:
                calls_str += f' … ({len(s["calls"])} total)'
            print(f"\n  {name}")
            print(f"    Live in:  {format_vars(s['live_in'])}")
            print(f"    Live out: {format_vars(s['live_out'])}")
            if s['locals']:
                print(f"    Scratch:  {format_vars(s['locals'])}")
            if s['consumed']:
                print(f"    ToCallee: {format_vars(s['consumed'])}")
            if s['calls']:
                print(f"    Calls:    {calls_str}")


if __name__ == '__main__':
    main()
