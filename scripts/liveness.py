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
PTR_VARS = ['ptr1', 'ptr2', 'ptr3', 'ptr5', 'ptr6']
DEBUG_FOR = False
ALL_VARS_SET = set(REGISTERS + TMP_VARS + PTR_VARS +
                   [f'flags:{b}' for b in FLAG_BITS])

TRACKED_VARS = set(REGISTERS + TMP_VARS + PTR_VARS)

# ─── Inline / lib helpers ─────────────────────────────────────────
INLINE_HELPERS = {
    'set_flags', 'cmp', 'adc', 'sbc', 'bit', 'rol', 'ror', 'asr',
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
}

# Functions that never return (longjmp wrappers / process exit).  Grown by
# the fixpoint in analyze_files to include functions whose only exits are
# calls to other noreturn functions.
NORETURN = {'exit', 'return_to_cli_prompt', 'return_to_editor_loop', 'run_editor'}

# ─── AST parsing (cached) ─────────────────────────────────────────
_parse_cache = {}
def parse_file(filepath):
    if filepath not in _parse_cache:
        idx = clang.cindex.Index.create()
        tu = idx.parse(filepath, CPPFLAGS)
        _parse_cache[filepath] = tu
    return _parse_cache[filepath]

# Source file line cache: the source files never change during an analysis,
# so read each once (avoids per-statement open()/readlines()).
_line_cache = {}
def get_source_lines(filepath):
    if filepath not in _line_cache:
        with open(filepath) as f:
            _line_cache[filepath] = f.readlines()
    return _line_cache[filepath]

# Per-statement analysis caches (stable across fixpoint iterations):
#   _stmts_cache[(file, funcname)] -> list of (line, cursor)
#   _analyze_cache[(file, funcname, line)] -> (defs, uses)
_stmts_cache = {}
_analyze_cache = {}

# Children cache: the parse tree is stable (kept alive by _parse_cache), so the
# children of a node are the same across the repeated backward/forward walks.
# Cursor.__hash__ is the underlying libclang pointer, stable per node.
_children_cache = {}
def _get_children(cursor):
    key = hash(cursor)
    children = _children_cache.get(key)
    if children is None:
        children = list(cursor.get_children())
        _children_cache[key] = children
    return children

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
    # A plain assignment `flags = expr` defines all flag bits (unless it is a
    # read-modify of flags, handled above).
    if re.search(r'\bflags\s*=\s*[^&|=]', line_text) and \
       not re.search(r'flags\s*=\s*flags', line_text):
        for bit in FLAG_BITS:
            defs.add(f'flags:{bit}')
    return defs, uses


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
        for child in _get_children(cursor):
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
        children = list(_get_children(cursor))
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
        children = list(_get_children(cursor))
        if not children:
            return d, u
        # Check source text for ++/-- (postfix operators)
        try:
            _line = get_source_lines(cursor.location.file.name)[cursor.location.line - 1]
        except Exception:
            _line = ''
        stripped_line = _line.strip()
        if '++' in stripped_line or '--' in stripped_line:
            return analyze_stmt(children[0], local_decls, 'defuse')
        # All other unary ops (!, &, *, -, ~) propagate context
        return analyze_stmt(children[0], local_decls, context)

    # ── ArraySubscriptExpr ──
    if ck == clang.cindex.CursorKind.ARRAY_SUBSCRIPT_EXPR:
        children = list(_get_children(cursor))
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
        children = list(_get_children(cursor))
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
        children = list(_get_children(cursor))
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
                gc = list(_get_children(callee_cursor))
                if gc and gc[0].kind == clang.cindex.CursorKind.DECL_REF_EXPR:
                    callee_name = gc[0].spelling
            else:
                callee_name = callee_cursor.spelling
            start_idx = 1

        # Process arguments as uses
        for child in children[start_idx:]:
            cd, cu = analyze_stmt(child, local_decls, 'use')
            d.update(cd); u.update(cu)
            # `&trackedvar` arguments are write-through output parameters:
            # the callee writes the pointed-to global.
            for v in _address_of_tracked(child, local_decls):
                d.add(v)
                u.discard(v)

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
        for child in _get_children(cursor):
            if child.kind == clang.cindex.CursorKind.COMPOUND_STMT:
                continue
            cd, cu = analyze_stmt(child, local_decls, 'use')
            d.update(cd); u.update(cu)
        return d, u

    # ── CompoundStmt: recurse ──
    if ck == clang.cindex.CursorKind.COMPOUND_STMT:
        for child in _get_children(cursor):
            cd, cu = analyze_stmt(child, local_decls, 'use')
            d.update(cd); u.update(cu)
        return d, u

    # ── Labels: recurse ──
    if ck in (clang.cindex.CursorKind.LABEL_STMT,
              clang.cindex.CursorKind.CASE_STMT,
              clang.cindex.CursorKind.DEFAULT_STMT):
        for child in _get_children(cursor):
            cd, cu = analyze_stmt(child, local_decls, context)
            d.update(cd); u.update(cu)
        return d, u

    # ── DeclStmt: group of declarations ──
    if ck == clang.cindex.CursorKind.DECL_STMT:
        for child in _get_children(cursor):
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
    for child in _get_children(cursor):
        cd, cu = analyze_stmt(child, local_decls, context)
        d.update(cd); u.update(cu)
    return d, u


def _address_of_tracked(cursor, local_decls):
    """Yield tracked globals written through an `&var` argument.

    A `&trackedvar` call argument is a write-through output parameter: the
    callee stores into the pointed-to global.  (Used e.g. by the printer
    driver's `printer_getflags(&x, &y)`.)
    """
    if cursor is None:
        return
    ck = cursor.kind
    if ck == clang.cindex.CursorKind.UNARY_OPERATOR:
        try:
            src = get_source_lines(cursor.location.file.name)[cursor.location.line - 1]
        except Exception:
            src = ''
        if '&' in src:
            children = list(_get_children(cursor))
            if children and children[0].kind == clang.cindex.CursorKind.DECL_REF_EXPR:
                name = children[0].spelling
                if name in TRACKED_VARS and name not in local_decls:
                    yield name
        return
    # Handle casts/parens around the address-of
    for child in _get_children(cursor):
        yield from _address_of_tracked(child, local_decls)


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
        for ch in _get_children(cursor):
            _top_stmts_recurse(ch, result)
        return
    if ck == clang.cindex.CursorKind.LABEL_STMT:
        result.append((cursor.location.line, cursor))
        for ch in _get_children(cursor):
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
        for ch in _get_children(cursor):
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
        for ch in _get_children(cursor):
            _top_stmts_recurse(ch, result)
        return
    # Expression statements: collect, then recurse into children
    result.append((cursor.location.line, cursor))
    for ch in _get_children(cursor):
        _top_stmts_recurse(ch, result)


def get_func_params_from_ast(func_cursor):
    """Extract parameter names that are tracked variables."""
    params = set()
    for child in _get_children(func_cursor):
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
    for child in _get_children(func_cursor):
        if child.kind == clang.cindex.CursorKind.PARM_DECL:
            if child.spelling in TRACKED_VARS:
                local_decls.add(child.spelling)
                defined_so_far.add(child.spelling)

    # Get the function body (compound statement)
    body_children = list(_get_children(func_cursor))
    body = None
    for child in body_children:
        if child.kind == clang.cindex.CursorKind.COMPOUND_STMT:
            body = child
            break

    if body is None:
        return local_defs, local_uses, local_live_in, call_sites

    # Collect the source lines for this function for flag regex
    try:
        source_lines = get_source_lines(func_cursor.location.file.name)
    except Exception:
        source_lines = []

    # Walk body statements
    # The AST is walked repeatedly across fixpoint iterations, but each
    # statement's defs/uses are deterministic given (function, line).  Cache
    # the per-statement analysis and the per-function statement list.
    funcname = func_cursor.spelling
    try:
        filename = func_cursor.location.file.name
    except Exception:
        filename = ''
    stmts_key = (filename, funcname)
    cached_stmts = _stmts_cache.get(stmts_key)
    if cached_stmts is not None:
        stmts = cached_stmts
    else:
        stmts = _collect_top_stmts(body)
        _stmts_cache[stmts_key] = stmts

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
        akey = (filename, funcname, line_num)
        cached = _analyze_cache.get(akey)
        if cached is not None:
            cd, cu = cached
        else:
            cd, cu = analyze_stmt(stmt, local_decls, 'use')
            _analyze_cache[akey] = (cd, cu)
        d.update(cd)
        u.update(cu)

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

        # Detect calls via the clang AST (finds calls in return values,
        # initializers, conditions, and nested expressions).
        for callee in _find_call_expr_callees(stmt):
            if callee in LIB_FUNCTIONS:
                continue
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
def _backward_stmt(cur, live, backward_needs, per_caller_readback, per_caller_provided,
                   caller, local_info, func_params,
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
        for ch in reversed(list(_get_children(cur))):
            live = _backward_stmt(ch, live, backward_needs, per_caller_readback, per_caller_provided, caller,
                                  local_info, func_params, local_decls, source_lines,
                                  func_live_out)
        return live

    # if / else: merge the two branches at the condition.
    if ck == clang.cindex.CursorKind.IF_STMT:
        children = list(_get_children(cur))
        branches = children[1:]
        merged = set(live)
        for b in branches:
            merged |= _backward_stmt(b, set(live), backward_needs, per_caller_readback, per_caller_provided, caller,
                                     local_info, func_params, local_decls, source_lines,
                                     func_live_out)
        d, u = _analyze_stmt_effect(cur, local_decls, source_lines, local_info, func_params)
        return (merged - d) | u

    # switch: each case is an independent branch.
    if ck == clang.cindex.CursorKind.SWITCH_STMT:
        children = list(_get_children(cur))
        merged = set(live)
        for ch in children:
            if ch.kind == clang.cindex.CursorKind.COMPOUND_STMT:
                # process each case independently (do NOT thread linearly)
                for case in _get_children(ch):
                    merged |= _backward_stmt(case, set(live), backward_needs, per_caller_readback, per_caller_provided, caller,
                                             local_info, func_params,
                                             local_decls, source_lines,
                                             func_live_out)
            elif ch.kind in (clang.cindex.CursorKind.CASE_STMT,
                             clang.cindex.CursorKind.DEFAULT_STMT):
                merged |= _backward_stmt(ch, set(live), backward_needs, per_caller_readback, per_caller_provided, caller,
                                         local_info, func_params, local_decls,
                                         source_lines, func_live_out)
        d, u = _analyze_stmt_effect(cur, local_decls, source_lines, local_info, func_params)
        return (merged - d) | u

    # case / default labels: treat as straight-line.
    if ck in (clang.cindex.CursorKind.CASE_STMT,
              clang.cindex.CursorKind.DEFAULT_STMT,
              clang.cindex.CursorKind.LABEL_STMT):
        for ch in _get_children(cur):
            live = _backward_stmt(ch, live, backward_needs, per_caller_readback, per_caller_provided, caller,
                                  local_info, func_params, local_decls, source_lines,
                                  func_live_out)
        return live

    # Loops: iterate body + condition to a fixed point (back-edge).
    if ck in (clang.cindex.CursorKind.WHILE_STMT,
              clang.cindex.CursorKind.FOR_STMT,
              clang.cindex.CursorKind.DO_STMT):
        body_stmt = None
        for ch in _get_children(cur):
            if ch.kind == clang.cindex.CursorKind.COMPOUND_STMT:
                body_stmt = ch
                break
        exit_live = set(live)

        if ck == clang.cindex.CursorKind.DO_STMT:
            # do { body } while (cond): the body always runs at least once, so
            # the loop-exit live set is provided by the body and must NOT be
            # required at the loop head (unlike while/for, where the body may
            # never run).  Fixed point over the head:
            #   head = cond_effect(body_effect(head | exit_live))
            prev = None
            cand = set(exit_live)
            for _ in range(20):
                d, u = _analyze_stmt_effect(cur, local_decls, source_lines, local_info, func_params)
                before_cond = ((cand | exit_live) - d) | u
                if body_stmt is None:
                    new_head = set(before_cond)
                else:
                    new_head = _backward_stmt(body_stmt, before_cond,
                                              backward_needs, per_caller_readback, per_caller_provided, caller,
                                              local_info, func_params, local_decls,
                                              source_lines, func_live_out)
                if new_head == prev:
                    break
                prev = new_head
                cand = new_head
            return prev if prev is not None else set(exit_live)

        # while/for: the condition may be false before the body ever runs, so
        # the loop-exit live set must also hold at the loop head.
        if DEBUG_FOR:
            _lines_ = open(cur.location.file.name).read().split('\n')
            print(f"[FOR] line {cur.location.line}: {_lines_[cur.location.line - 1].strip()}")
            _bd = set(live)
            if body_stmt is not None:
                _bd = _backward_stmt(body_stmt, set(live), backward_needs, per_caller_readback, per_caller_provided, caller,
                                     local_info, func_params, local_decls,
                                     source_lines, func_live_out)
            print(f"[FOR]   exit_live={sorted(live)} body_before={sorted(_bd)}")
        prev = None
        result = set(live)
        for _ in range(20):
            body_in = set(live)
            if body_stmt is not None:
                body_in = _backward_stmt(body_stmt, body_in, backward_needs, per_caller_readback, per_caller_provided, caller,
                                         local_info, func_params,
                                         local_decls, source_lines,
                                         func_live_out)
            d, u = _analyze_stmt_effect(cur, local_decls, source_lines, local_info, func_params)
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

    return _backward_plain(cur, live, backward_needs, per_caller_readback, per_caller_provided, caller,
                           local_info, func_params,
                           local_decls, source_lines, func_live_out)


def _find_call_expr_callees(cur):
    """Yield callee names of call expressions in `cur`'s subtree, skipping
    compound-statement bodies (which are analysed as separate statements)."""
    if cur is None:
        return
    ck = cur.kind
    if ck == clang.cindex.CursorKind.COMPOUND_STMT:
        return
    if ck == clang.cindex.CursorKind.CALL_EXPR:
        children = list(_get_children(cur))
        if children:
            c0 = children[0]
            if c0.kind == clang.cindex.CursorKind.DECL_REF_EXPR:
                yield c0.spelling
            elif c0.kind == clang.cindex.CursorKind.UNEXPOSED_EXPR:
                g = list(_get_children(c0))
                if g and g[0].kind == clang.cindex.CursorKind.DECL_REF_EXPR:
                    yield g[0].spelling
        return
    for ch in _get_children(cur):
        yield from _find_call_expr_callees(ch)


def _analyze_stmt_effect(cur, local_decls, source_lines, local_info=None,
                         func_params=None):
    """defs/uses of a statement (with byte/flag propagation)."""
    d, u = analyze_stmt(cur, set(local_decls), 'use')
    if cur.location.line > 0 and cur.location.line <= len(source_lines):
        fd, fu = get_flag_defs_uses(source_lines[cur.location.line - 1])
        d |= fd
        u |= fu
    # Calls in control-flow conditions (e.g. `if (sub_c9aa9()) return;`)
    # define the callee's outputs; model them so a branch's flag need is
    # satisfied at the condition instead of leaking back to the statement's
    # predecessor.
    if local_info is not None:
        for callee in _find_call_expr_callees(cur):
            ci = local_info.get(callee)
            if ci is None:
                continue
            cp = set()
            if func_params is not None:
                cp = func_params.get(callee, set())
            d |= ci.get('cached_defs', ci.get('defs', set())) - cp
    return d, u


def _backward_plain(cur, live, backward_needs, per_caller_readback, per_caller_provided, caller,
                    local_info, func_params,
                    local_decls, source_lines, func_live_out):
    """Backward handling for a straight-line (leaf) statement.

    Calls in the statement are found via the clang AST (handles calls in
    return values, initializers, conditions, and nested expressions)."""
    line_num = cur.location.line

    callees = list(_find_call_expr_callees(cur))
    if callees:
        # A noreturn or ALL_IN_OUT callee dominates: its effect overrides
        # the rest of the statement.
        for callee_name in callees:
            if callee_name in ALL_IN_OUT:
                backward_needs.setdefault(callee_name, set()).update(ALL_VARS_SET)
                return ALL_VARS_SET
            if callee_name in NORETURN:
                # The callee never returns, so nothing is live after the call;
                # only its inputs are needed before it.
                callee_info = local_info.get(callee_name, {})
                callee_params = func_params.get(callee_name, set())
                d, u = _analyze_stmt_effect(cur, local_decls, source_lines, local_info, func_params)
                cli = callee_info.get('live_in', set()) - callee_params
                return cli | u

        # Process each non-library callee's kill set, then fold in the
        # statement's own defs/uses and the callees' live-in requirements.
        d, u = _analyze_stmt_effect(cur, local_decls, source_lines, local_info, func_params)
        result = live
        for callee_name in callees:
            if callee_name in LIB_FUNCTIONS:
                continue
            if callee_name in CORRUPTS and callee_name not in local_info:
                result = (result - CORRUPTS[callee_name]) | u
                continue
            backward_needs.setdefault(callee_name, set()).update(result)
            # Record what THIS caller reads back from the callee: the live set
            # after the call (before the callee's outputs are killed below).
            per_caller_readback.setdefault((caller, callee_name), set()).update(result)
            callee_info = local_info.get(callee_name, {})
            callee_params = func_params.get(callee_name, set())
            callee_defs = callee_info.get('cached_defs', callee_info.get('defs', set()))
            kill_set = callee_defs - callee_params
            if callee_name in CORRUPTS:
                kill_set = kill_set | CORRUPTS[callee_name]
            result = (result - kill_set) | u
            cli = callee_info.get('live_in', set()) - callee_params
            result |= cli
            # Record what THIS caller must provide to the callee: only the
            # callee's required inputs (cli) plus the statement's own uses (u),
            # minus registers the statement itself defines (own_d).  The full
            # live-through set (result) carries registers live for the caller's
            # own continuation, and the callee's cached_defs (part of d) are
            # outputs produced by the callee -- neither of which count as
            # inputs the caller hands over.  A register can be both a callee
            # input and output (e.g. y incremented by the callee), in which
            # case it is still provided by the caller.  Also include the
            # callee's transitive consumed registers (e.g. ptr5 that print_loop
            # passes on to prepare_output_line) -- the caller provides these if
            # it defines them before the call.
            own_d = analyze_stmt(cur, set(local_decls), 'use')[0]
            provided = (u | cli) - own_d
            # Also include registers the callee forwards to ITS callees (its
            # transitive inputs), e.g. ptr5 that print_loop passes on to
            # prepare_output_line.  Only include registers the callee does NOT
            # define internally (they must come from the caller); registers the
            # callee produces itself (e.g. a produced by read_next_command_byte)
            # are not caller inputs.
            callee_provided = callee_info.get('per_callee_provided', {})
            for v in callee_provided.values():
                provided |= (set(v) - callee_info.get('defs', set()) - own_d)
            per_caller_provided.setdefault((caller, callee_name), set()).update(provided)
        return result

    d, u = _analyze_stmt_effect(cur, local_decls, source_lines, local_info, func_params)
    return (live - d) | u


def _callee_name_from_cursor(call_cursor):
    """Return the callee name of a CALL_EXPR cursor, or None."""
    children = list(_get_children(call_cursor))
    if not children:
        return None
    c0 = children[0]
    if c0.kind == clang.cindex.CursorKind.DECL_REF_EXPR:
        return c0.spelling
    if c0.kind == clang.cindex.CursorKind.UNEXPOSED_EXPR:
        g = list(_get_children(c0))
        if g and g[0].kind == clang.cindex.CursorKind.DECL_REF_EXPR:
            return g[0].spelling
    return None


def _stmt_never_completes(cur, noreturn):
    """True if `cur` definitely never falls through to the next statement
    (always exits via a return, a noreturn call, or an infinite loop)."""
    ck = cur.kind
    if ck == clang.cindex.CursorKind.RETURN_STMT:
        return True
    if ck == clang.cindex.CursorKind.CALL_EXPR:
        return _callee_name_from_cursor(cur) in noreturn
    if ck == clang.cindex.CursorKind.COMPOUND_STMT:
        last = None
        for ch in _get_children(cur):
            if ch.kind not in (clang.cindex.CursorKind.CASE_STMT,
                               clang.cindex.CursorKind.DEFAULT_STMT,
                               clang.cindex.CursorKind.LABEL_STMT):
                last = ch
        if last is None:
            return False
        return _stmt_never_completes(last, noreturn)
    if ck == clang.cindex.CursorKind.IF_STMT:
        children = list(_get_children(cur))
        if len(children) < 3:
            return False  # no else: the false path falls through
        return (_stmt_never_completes(children[1], noreturn) and
                _stmt_never_completes(children[2], noreturn))
    return False


def _returns_without_noreturn_guard(cur, noreturn):
    """True if `cur`'s subtree contains a return statement that is not
    immediately preceded (in its enclosing compound) by a noreturn call.
    Such a return is potentially reachable, so the function can return."""
    ck = cur.kind
    if ck == clang.cindex.CursorKind.RETURN_STMT:
        return True
    if ck == clang.cindex.CursorKind.COMPOUND_STMT:
        children = [ch for ch in _get_children(cur)
                    if ch.kind not in (clang.cindex.CursorKind.CASE_STMT,
                                       clang.cindex.CursorKind.DEFAULT_STMT,
                                       clang.cindex.CursorKind.LABEL_STMT)]
        for idx, ch in enumerate(children):
            if ch.kind == clang.cindex.CursorKind.RETURN_STMT:
                # Dead if the preceding statement is a noreturn call.
                if not (idx > 0 and children[idx - 1].kind ==
                        clang.cindex.CursorKind.CALL_EXPR and
                        _callee_name_from_cursor(children[idx - 1]) in noreturn):
                    return True
            else:
                if _returns_without_noreturn_guard(ch, noreturn):
                    return True
        return False
    for ch in _get_children(cur):
        if _returns_without_noreturn_guard(ch, noreturn):
            return True
    return False


def _is_noreturn_body(body, noreturn):
    """True if the function body never returns normally: it never falls
    through the end and contains no reachable return."""
    if _returns_without_noreturn_guard(body, noreturn):
        return False
    return _stmt_never_completes(body, noreturn)


def _return_value_uses(body_cursor, local_decls, source_lines):
    """Yield tracked-global uses in return statements of a function body."""
    if body_cursor is None:
        return
    for _ln, stmt in _collect_top_stmts(body_cursor):
        if stmt.kind != clang.cindex.CursorKind.RETURN_STMT:
            continue
        _, u = _analyze_stmt_effect(stmt, local_decls, source_lines, None, None)
        yield from u


def backward_scan_ast(func_cursor, backward_needs, per_caller_readback, per_caller_provided, caller,
                      local_info, func_params, live_out_start=None):
    """
    Backward scan using AST for a single function.
    Modifies backward_needs in place.
    """
    if live_out_start is None:
        live_out_start = set()
    local_decls = set()
    for child in _get_children(func_cursor):
        if child.kind == clang.cindex.CursorKind.PARM_DECL:
            if child.spelling in TRACKED_VARS:
                local_decls.add(child.spelling)

    try:
        source_lines = get_source_lines(func_cursor.location.file.name)
    except Exception:
        source_lines = []

    # Get body statements in order
    body_children = list(_get_children(func_cursor))
    body = None
    for child in body_children:
        if child.kind == clang.cindex.CursorKind.COMPOUND_STMT:
            body = child
            break
    if body is None:
        return

    func_live_out = set(live_out_start)
    _backward_stmt(body, set(func_live_out), backward_needs, per_caller_readback, per_caller_provided, caller,
                   local_info, func_params, local_decls, source_lines, func_live_out)
    # The registers read to form a return value are produced by this function
    # for its callers, so they are live on exit even if no caller's backward
    # scan happens to demand them (e.g. the high byte of a value returned in
    # A:Y where the caller only models the low byte).
    for return_use in _return_value_uses(body, local_decls, source_lines):
        backward_needs.setdefault(func_cursor.spelling, set()).add(return_use)


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
    ptrs = sorted(v for v in var_set if v in PTR_VARS)
    parts = []
    if regs:
        parts.append(', '.join(regs))
    if bits:
        parts.append('|'.join(bits))
    if tmps:
        parts.append(', '.join(tmps))
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
        for cursor in _get_children(tu.cursor):
            if cursor.kind == clang.cindex.CursorKind.FUNCTION_DECL and cursor.is_definition():
                name = cursor.spelling
                if name not in all_funcs:
                    all_funcs[name] = (filepath, cursor)

    # Compute function params
    func_params = {}
    for name, (filepath, cursor) in all_funcs.items():
        func_params[name] = get_func_params_from_ast(cursor)

    # Grow the noreturn set: a function that never falls through the end and
    # has no reachable return is itself noreturn.
    global NORETURN
    NORETURN = {'exit', 'return_to_cli_prompt', 'return_to_editor_loop',
                'run_editor'}
    changed = True
    while changed:
        changed = False
        for name, (filepath, cursor) in all_funcs.items():
            if name in NORETURN:
                continue
            body = None
            for ch in _get_children(cursor):
                if ch.kind == clang.cindex.CursorKind.COMPOUND_STMT:
                    body = ch
                    break
            if body is not None and _is_noreturn_body(body, NORETURN):
                NORETURN.add(name)
                changed = True

    # Initial forward pass (using AST)
    local_info = {}
    for name, (filepath, cursor) in all_funcs.items():
        d, u, li, cs = get_local_info_ast(cursor)

        # Compute local declarations for this function
        local_decls = set()
        for child in _get_children(cursor):
            if child.kind == clang.cindex.CursorKind.PARM_DECL:
                if child.spelling in TRACKED_VARS:
                    local_decls.add(child.spelling)
        # Walk the body for VarDecl
        for child in _get_children(cursor):
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
                    callee_locals = callee_info.get('local_decls', set())
                    # Callee locals/params shadow the globals; they must not
                    # count as the callee's global defs/uses (and hence must
                    # not appear in the callee's live_out seen by callers).
                    all_defs.update(cd - callee_params - callee_locals)
                    all_uses.update(cu - callee_params - callee_locals)
            old_d = info.get('cached_defs')
            old_u = info.get('cached_uses')
            if all_defs != old_d or all_uses != old_u:
                info['cached_defs'] = all_defs
                info['cached_uses'] = all_uses
                changed = True

    # ── Full interprocedural fixed-point iteration ──
    backward_needs = {name: set() for name in all_funcs}
    per_caller_readback = {}
    per_caller_provided = {}
    fp_iter = 0
    changed = True
    while changed and fp_iter < 10:
        fp_iter += 1
        changed = False

        # Backward scan using AST
        backward_needs.clear()
        per_caller_readback = {}
        per_caller_provided = {}
        # Pass each callee's previously-computed passed_to_callees so that
        # registers forwarded by a callee to ITS callees (e.g. ptr5 passed by
        # print_loop to prepare_output_line) are seen as inputs the caller must
        # provide.
        for name in all_funcs:
            info = local_info[name]
            cursor = all_funcs[name][1]
            backward_scan_ast(cursor, backward_needs, per_caller_readback, per_caller_provided,
                              name, local_info, func_params,
                              info.get('live_out', set()))

        # Store per-callee provided sets into local_info for the next iteration.
        for (caller, callee), v in per_caller_provided.items():
            entry = local_info.setdefault(caller, {}).setdefault('per_callee_provided', {})
            entry[callee] = set(v)

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

        # passed_to_callees: register-convention inputs handed DOWN to the
        # direct callees (what the caller must provide).
        # from_callees: registers produced by the direct callees and read back
        # (the callees' live_out, possibly transitive) -- the caller's upward
        # interface, so never scratch of the caller.
        passed_to_callees = set()
        from_callees = set()
        cursor = all_funcs[name][1]
        try:
            flines = get_source_lines(all_funcs[name][0])
        except Exception:
            flines = []

        def _process_callee(callee_name, line_num):
            """Classify a direct callee's register-convention interface."""
            nonlocal passed_to_callees, from_callees
            if line_num > 0 and line_num <= len(flines):
                sl = flines[line_num - 1].strip()
            else:
                sl = ''
            if sl.startswith('//') or sl == '':
                sl = ''
            if not callee_name or callee_name in INLINE_HELPERS:
                return
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
            # Callee's live_in (inputs the callee needs) and live_out (the
            # registers the callee produces that the caller reads back).
            callee_info = local_info.get(callee_name)
            if callee_info:
                callee_locals = callee_info.get('local_decls', set())
                # Only registers this caller actually has to provide before
                # the call count as "passed" to the callee (see
                # per_caller_provided).  A callee's static live_in may be
                # produced internally by the caller before the call (e.g.
                # expand_escaped_string defines a via read_next_command_byte
                # before upper_case_unless_folding reads it), so it is not a
                # caller input.
                provided = per_caller_provided.get((name, callee_name), set())
                passed_to_callees |= (provided - callee_params)
                # Only registers this caller actually reads back after the
                # call count as "from" the callee (see per_caller_readback).
                read_back = per_caller_readback.get((name, callee_name), set())
                from_callees |= (read_back & (callee_info.get('live_out', set()))) - callee_locals - callee_params

        stmts = []
        try:
            fname = cursor.location.file.name
        except Exception:
            fname = ''
        cached_stmts = _stmts_cache.get((fname, name))
        if cached_stmts is not None:
            stmts = cached_stmts
        else:
            for child in _get_children(cursor):
                if child.kind == clang.cindex.CursorKind.COMPOUND_STMT:
                    stmts = _collect_top_stmts(child)
                    break
            _stmts_cache[(fname, name)] = stmts

        # Top-level CALL_EXPR statements (catches calls in if/while/for
        # conditions and other control-flow expressions).
        for line_num, stmt in stmts:
            if stmt.kind != clang.cindex.CursorKind.CALL_EXPR:
                continue
            children = list(_get_children(stmt))
            callee_name = None
            if children and children[0].kind == clang.cindex.CursorKind.DECL_REF_EXPR:
                callee_name = children[0].spelling
            elif children and children[0].kind == clang.cindex.CursorKind.UNEXPOSED_EXPR:
                gc = list(children[0].get_children())
                if gc and gc[0].kind == clang.cindex.CursorKind.DECL_REF_EXPR:
                    callee_name = gc[0].spelling
            _process_callee(callee_name, line_num)

        # Line-based call sites (catches calls nested in initializers and
        # larger expressions that are not top-level CALL_EXPR statements).
        for line_num, callee_name, _args in info['call_sites']:
            _process_callee(callee_name, line_num)

        globals_used_locally = ((local_defs & local_uses) - live_in - live_out - passed_to_callees - from_callees - local_decls_func)
        globals_used_locally = {v for v in globals_used_locally if not v.startswith('flags:')}

        summaries[name] = {
            'defs': all_defs,
            'uses': all_uses,
            'live_in': live_in,
            'live_out': live_out,
            'locals': globals_used_locally,
            'consumed': set(),
            'from_callees': from_callees,
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
                    # Only registers this caller actually hands to the callee
                    # (per_caller_provided) propagate as consumed -- plus the
                    # callee's transitive consumed for those hand-offs.  The
                    # callee's static live_in is already reflected in
                    # per_caller_provided and may be produced internally by
                    # this caller, so it must not be added wholesale here.
                    provided = per_caller_provided.get((name, callee), set())
                    consumed.update(provided & cs.get('consumed', set()))
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
    for child in _get_children(cursor):
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
            if s['from_callees']:
                print(f"    FromCallees: {format_vars(s['from_callees'])}")
            if s['calls']:
                print(f"    Calls:    {calls_str}")


if __name__ == '__main__':
    main()
