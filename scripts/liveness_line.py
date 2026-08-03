#!/usr/bin/env python3
"""
Line-by-line liveness analysis using clang's AST for def/use detection.

Annotates each line of C source with the registers and flag bits
that are live at that program point.

Usage:
    python3 scripts/liveness_line.py          # annotate all .c files
    python3 scripts/liveness_line.py src/editor.c  # specific file
"""

import re, sys, os, clang.cindex

sys.path.insert(0, os.path.dirname(__file__))
import liveness  # reuse helper tables + AST-based analyze_stmt

# ─── Known corrupters ────────────────────────────────────────────────
# Functions that unconditionally write to these registers/flags.
CORRUPTS = dict(liveness.CORRUPTS)
INLINE_HELPERS = liveness.INLINE_HELPERS
LIB_FUNCTIONS = liveness.LIB_FUNCTIONS
TRACKED_VARS = liveness.TRACKED_VARS
BYTE_TO_COMBINED = liveness.BYTE_TO_COMBINED
COMBINED_TO_BYTES = liveness.COMBINED_TO_BYTES
CALL_RE = liveness.CALL_RE

# ─── Flag ops from source (kept: FLAG_X macros expand in AST) ──────
def get_flag_defs_uses(text):
    defs = set()
    uses = set()
    for bit in liveness.FLAG_BITS:
        if re.search(r'flags\s*&=\s*~FLAG_' + bit, text):
            defs.add(f'flags:{bit}')
        elif re.search(r'flags\s*\|=\s*FLAG_' + bit, text):
            defs.add(f'flags:{bit}')
        elif re.search(r'flags\s*=\s*flags\s*[&|].*FLAG_' + bit, text):
            defs.add(f'flags:{bit}'); uses.add(f'flags:{bit}')
        elif re.search(r'if\s*\(.*flags\s*&\s*FLAG_' + bit, text):
            uses.add(f'flags:{bit}')
    return defs, uses


# ─── AST statement collection ────────────────────────────────────────
def _collect_all_stmt_cursors(cursor, result):
    """Recursively collect statement-level cursors with their start lines."""
    ck = cursor.kind
    if ck == clang.cindex.CursorKind.PARM_DECL:
        return
    if ck == clang.cindex.CursorKind.VAR_DECL:
        result.append((cursor.location.line, cursor))
        return
    if ck == clang.cindex.CursorKind.DECL_STMT:
        for ch in cursor.get_children():
            _collect_all_stmt_cursors(ch, result)
        return
    if ck == clang.cindex.CursorKind.LABEL_STMT:
        result.append((cursor.location.line, cursor))
        for ch in cursor.get_children():
            _collect_all_stmt_cursors(ch, result)
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
            _collect_all_stmt_cursors(ch, result)
        return
    if ck in (clang.cindex.CursorKind.IF_STMT,
              clang.cindex.CursorKind.WHILE_STMT,
              clang.cindex.CursorKind.FOR_STMT,
              clang.cindex.CursorKind.DO_STMT,
              clang.cindex.CursorKind.SWITCH_STMT):
        result.append((cursor.location.line, cursor))
        for ch in cursor.get_children():
            _collect_all_stmt_cursors(ch, result)
        return
    if ck in (clang.cindex.CursorKind.CASE_STMT,
              clang.cindex.CursorKind.DEFAULT_STMT):
        result.append((cursor.location.line, cursor))
        for ch in cursor.get_children():
            _collect_all_stmt_cursors(ch, result)
        return
    # Expression statements: collect, then recurse into children
    result.append((cursor.location.line, cursor))
    for ch in cursor.get_children():
        _collect_all_stmt_cursors(ch, result)


def build_ast_line_info(filepath, func_name, source_lines):
    """
    Build {line_index: (defs, uses)} for a function using the AST.

    Combines AST-based tracked-variable defs/uses with regex-based
    flag bit-field ops (FLAG_X macros expand in the AST).
    """
    tu = liveness.parse_file(filepath)
    func_cursor = None
    for c in tu.cursor.get_children():
        if c.kind == clang.cindex.CursorKind.FUNCTION_DECL and \
           c.spelling == func_name and c.is_definition():
            func_cursor = c
            break
    if func_cursor is None:
        return {}

    # Local declarations (parameters + body VarDecls)
    local_decls = set()
    for ch in func_cursor.get_children():
        if ch.kind == clang.cindex.CursorKind.PARM_DECL and \
           ch.spelling in TRACKED_VARS:
            local_decls.add(ch.spelling)
    for ch in func_cursor.get_children():
        if ch.kind == clang.cindex.CursorKind.COMPOUND_STMT:
            liveness._collect_vardecls(ch, local_decls)

    # Collect all statement cursors
    cursors = []
    for ch in func_cursor.get_children():
        if ch.kind == clang.cindex.CursorKind.COMPOUND_STMT:
            _collect_all_stmt_cursors(ch, cursors)

    line_info = {}
    seen = set()
    for line, cur in cursors:
        if line in seen or line <= 0 or line > len(source_lines):
            continue
        seen.add(line)

        d, u = liveness.analyze_stmt(cur, set(local_decls), 'use')
        # Byte/combined propagation
        for bv, cv in BYTE_TO_COMBINED.items():
            if bv in d: d.add(cv)
            if bv in u: u.add(cv)
        for cv, blist in COMBINED_TO_BYTES.items():
            if cv in d:
                for bv in blist: d.add(bv)
            if cv in u:
                for bv in blist: u.add(bv)

        # Flag bit-field ops via regex
        fd, fu = get_flag_defs_uses(source_lines[line - 1])
        d |= fd
        u |= fu

        line_info[line] = (d, u)

    return line_info


# ─── Detect labels and gotos ────────────────────────────────────────
LABEL_RE = re.compile(r'^(\w+):\s*(?://.*)?$')

def find_labels_and_gotos(lines, func_start, func_end):
    """Find all labels and goto targets within a function body.

    Returns (labels, gotos, branches, label_lines):
        labels: set of label names
        gotos: dict line_index -> target_label
        branches: dict line_index -> target_label (for conditional gotos)
        label_lines: dict label_name -> line_index
    """
    labels = set()
    gotos = {}  # line_index -> target_label
    branches = {}  # line_index -> target_label (for conditional gotos)
    label_lines = {}

    for i in range(func_start, func_end):
        sl = lines[i].strip()
        if sl.startswith('//') or sl == '':
            continue
        m = LABEL_RE.match(sl)
        if m:
            labels.add(m.group(1))
            label_lines[m.group(1)] = i
            continue
        m = re.match(r'goto\s+(\w+);', sl)
        if m:
            gotos[i] = m.group(1)
            continue
        m = re.match(r'if\s*\(.*\)\s*goto\s+(\w+);', sl)
        if m:
            branches[i] = m.group(1)
            continue

    return labels, gotos, branches, label_lines


# ─── Per-function backward analysis ──────────────────────────────────
def analyze_function(lines, func_name, func_start, func_end, callee_live_out,
                     callee_live_in=None, line_info=None):
    """
    Do backward dataflow analysis for a single function.

    Args:
        lines: source lines
        func_name, func_start, func_end: function extent (0-based line indices)
        callee_live_out: dict name -> live_out
        callee_live_in: dict name -> live_in
        line_info: optional {line_index: (defs, uses)} from build_ast_line_info.
                   If None, falls back to regex get_line_defs_uses.

    Returns (live_before, live_after) dicts keyed by line index.
    """
    if callee_live_in is None:
        callee_live_in = {}
    labels, gotos, branches, label_lines = find_labels_and_gotos(lines, func_start, func_end)
    targeted_labels = set(gotos.values()) | set(branches.values())

    live_after = {i: set() for i in range(func_start, func_end)}
    live_before = {i: set() for i in range(func_start, func_end)}
    # Live sets contributed by goto/branch predecessors, per label.
    # Accumulated across iterations (monotone: liveness only grows).
    goto_live = {}

    changed = True
    max_iter = 50
    iteration = 0

    while changed and iteration < max_iter:
        changed = False
        iteration += 1

        # Start from the end with the function's own live_out
        live = set(callee_live_out.get(func_name, set()))

        for i in range(func_end - 1, func_start - 1, -1):
            sl = lines[i].strip()

            if sl.startswith('//') or sl == '':
                live_after[i] = set(live)
                live_before[i] = set(live)
                continue

            live_after[i] = set(live)

            if sl == 'return;':
                live = set(callee_live_out.get(func_name, set()))
                live_before[i] = set(live)
                continue

            m = LABEL_RE.match(sl)
            if m:
                label = m.group(1)
                old_live = live_before.get(i, set())
                new_live = old_live | live | goto_live.get(label, set())
                if new_live != old_live:
                    changed = True
                live = new_live
                live_before[i] = set(live)
                continue

            # goto / conditional-goto: record live set as a predecessor of
            # the target label (handles loop back-edges).
            gm = re.match(r'goto\s+(\w+);', sl)
            if gm:
                goto_live.setdefault(gm.group(1), set()).update(live)
            else:
                bm = re.match(r'if\s*\(.*\)\s*goto\s+(\w+);', sl)
                if bm:
                    goto_live.setdefault(bm.group(1), set()).update(live)

            # Get defs and uses (AST if available, else regex fallback)
            # line_info keys are 1-based line numbers; i is 0-based.
            if line_info is not None and (i + 1) in line_info:
                d, u = line_info[i + 1]
            else:
                d, u = get_line_defs_uses(sl)

            callee = CALL_RE.search(sl)
            callee_name = callee.group(1) if callee else None

            if callee_name and callee_name not in INLINE_HELPERS and \
               callee_name not in ('if', 'while', 'for', 'switch', 'sizeof'):
                if callee_name in LIB_FUNCTIONS:
                    # Library functions read their arguments (e.g.
                    # screen_setstyle(a) passes a). Add uses but kill nothing.
                    live = live | u
                elif callee_name in CORRUPTS:
                    corr = CORRUPTS[callee_name]
                    live = (live - corr) | u
                elif callee_name in callee_live_out:
                    clo = callee_live_out[callee_name]
                    cli = callee_live_in.get(callee_name, set())
                    live = (live - clo) | cli | u
                else:
                    # Not a known function: likely a macro (e.g. CTRL(x)),
                    # which expands in the AST and has no runtime effect on
                    # the simulated registers. Treat as a plain statement.
                    live = (live - d) | u
            else:
                live = (live - d) | u

            live_before[i] = set(live)

    return live_before, live_after


# ─── Regex fallback (used only if AST line info unavailable) ────────
def get_line_defs_uses(text):
    """Fallback regex-based def/use detection for a single line."""
    d = set()
    u = set()

    for helper in INLINE_HELPERS:
        if helper + '(&flags' in text:
            for b in liveness.HELPER_FLAG_DEFS.get(helper, set()):
                d.add(f'flags:{b}')
            for b in liveness.HELPER_FLAG_USES.get(helper, set()):
                u.add(f'flags:{b}')
            if helper in ('adc', 'sbc', 'rol', 'ror') and '= ' + helper in text:
                d.add('a')
            if helper in ('cmp', 'set_flags'):
                m = re.match(r'(?:set_flags|cmp)\(&flags,\s*(\w+)', text)
                if m:
                    u.add(m.group(1))
            break

    fd, fu = get_flag_defs_uses(text)
    d |= fd
    u |= fu

    for var in TRACKED_VARS:
        if re.search(r'\b' + var + r'\s*=(?!=)', text) and \
           not re.search(r'\buint8_t\s+' + var + r'\b', text):
            d.add(var)
            if var in BYTE_TO_COMBINED:
                d.add(BYTE_TO_COMBINED[var])
        if re.search(r'\b' + var + r'\+{2}\b', text) or \
           re.search(r'\b' + var + r'--\b', text) or \
           re.search(r'\b' + var + r'\s*[\+\-]=', text):
            d.add(var)
            u.add(var)
        if re.search(r'(?<!\w)' + var + r'(?!\w)', text):
            if not re.search(r'\b' + var + r'\s*=(?!=)', text) or \
               re.search(r'\b' + var + r'\s*[\+\-]=', text) or \
               re.search(r'\b' + var + r'\+{2}\b', text) or \
               re.search(r'\b' + var + r'--\b', text):
                if not re.search(r'\buint8_t\s+' + var + r'\b', text):
                    u.add(var)

    for combined, blist in [('tmp01', ['tmp0','tmp1']), ('tmp23', ['tmp2','tmp3']),
                            ('tmp45', ['tmp4','tmp5']), ('tmp67', ['tmp6','tmp7']),
                            ('tmp89', ['tmp8','tmp9'])]:
        if combined in d:
            for bv in blist:
                d.add(bv)

    return d, u


# ─── Formatting ─────────────────────────────────────────────────────
def fmt_live_set(s):
    """Format a live set as a short string for comments."""
    regs = sorted(v for v in s if v in {'a', 'x', 'y'})
    bits = sorted(v[6] for v in s if v.startswith('flags:'))
    parts = []
    if regs:
        parts.append(','.join(regs))
    if bits:
        parts.append('|'.join(bits))
    return ' '.join(parts) if parts else '—'


# ─── Main ───────────────────────────────────────────────────────────
def annotate_file(filepath, callee_live_out=None, write_to_file=False):
    """Annotate a single file with per-line liveness info.

    When write_to_file is False, the annotated output is printed to stdout and
    the file is left untouched.
    """
    if callee_live_out is None:
        callee_live_out = {}

    with open(filepath) as f:
        lines = f.readlines()

    funcs = liveness.find_functions(lines)

    summaries = {}
    try:
        summaries = liveness.analyze_files([filepath])
    except Exception:
        pass

    clo = dict(callee_live_out)
    cli = {}
    for name, s in summaries.items():
        clo[name] = s.get('live_out', set())
        cli[name] = s.get('live_in', set())

    for func_name, func_start, func_end in funcs:
        line_info = build_ast_line_info(filepath, func_name, lines)
        live_before, live_after = analyze_function(
            lines, func_name, func_start, func_end, clo, cli, line_info
        )

        new_lines = []
        for i in range(func_start, func_end):
            line = lines[i]
            stripped = line.strip()

            if stripped.startswith('//') or stripped == '':
                new_lines.append(line)
                continue

            lb = live_before.get(i, set())
            annotation = fmt_live_set(lb)
            if annotation:
                if not stripped.endswith(';') and not stripped.endswith('{') and not stripped.endswith('}'):
                    new_lines.append(line)
                else:
                    if '//' in line:
                        parts = line.split('//', 1)
                        new_lines.append(f"{parts[0]} // {annotation}  |{parts[1]}")
                    else:
                        new_lines.append(line.rstrip() + f'  // {annotation}\n')
            else:
                new_lines.append(line)

        lines[func_start:func_end] = new_lines

    if write_to_file:
        with open(filepath, 'w') as f:
            f.writelines(lines)
    else:
        sys.stdout.writelines(lines)

    return True


def main():
    write = '--write' in sys.argv
    args = [a for a in sys.argv[1:] if a != '--write']
    files = args if args else [
        'src/view.c', 'src/editor.c', 'src/printing.c',
        'src/document.c', 'src/cli.c'
    ]

    if not write:
        print("Dry run: annotations would be added but files are NOT modified.")
        print("Re-run with --write to write annotations into the source files.\n")

    for filepath in files:
        print(f"Annotating {filepath}...")
        try:
            annotate_file(filepath, write_to_file=write)
            print(f"  Done.")
        except Exception as e:
            print(f"  Error: {e}")

    print("\nDone.")


if __name__ == '__main__':
    main()
