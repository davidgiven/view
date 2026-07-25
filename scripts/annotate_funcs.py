#!/usr/bin/env python3
"""
Annotate each function with per-flag-bit liveness info.

Usage: python3 scripts/annotate_funcs.py
"""

import sys, os, re

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from scripts.liveness import (
    analyze_files, find_functions, format_vars, REGISTERS, FLAG_BITS, TMP_VARS,
    FUNC_START_RE, INLINE_HELPERS,
)

SRC_FILES = [
    'src/view.c', 'src/editor.c', 'src/printing.c',
    'src/document.c', 'src/cli.c',
]


def format_live_set(var_set):
    """Format a set of variables for the live in/out comment."""
    regs = [v for v in REGISTERS if v in var_set]
    bits = sorted(v[6] for v in var_set if v.startswith('flags:'))
    tmps = [v for v in TMP_VARS if v in var_set]
    parts = []
    if regs:
        parts.append(', '.join(regs))
    if bits:
        parts.append('|'.join(bits))
    if tmps:
        parts.append(', '.join(tmps))
    return '; '.join(parts) if parts else '-'


def make_annotation_block(name, s):
    """Build lines to insert as annotation."""
    li = format_live_set(s['live_in'])
    lo = format_live_set(s['live_out'])
    return [
        f'// {name}: Live in: {li}, Live out: {lo}\n',
    ]


def is_annotation_line(stripped):
    """Check if a line is an existing annotation (from any version)."""
    return stripped.startswith('// Live') or stripped.startswith('//  Inputs') or stripped.startswith('//  Outputs') or stripped.startswith('//  Temps')


def annotate_files():
    print("Running liveness analysis...")
    summaries = analyze_files(SRC_FILES)
    print(f"  {len(summaries)} functions analyzed\n")

    for filepath in SRC_FILES:
        if not os.path.exists(filepath):
            continue

        with open(filepath) as f:
            lines = f.readlines()

        funcs = [(n, s, e) for n, s, e in find_functions(lines)
                 if n in summaries and summaries[n]['file'] == filepath]

        # For each function, find the right insertion point and build annotation
        insertions = []  # (line_index, [comment_lines])

        for name, start, end in funcs:
            s = summaries[name]

            # Omit trivial cases where nothing is live
            if not s['live_in'] and not s['live_out']:
                continue

            anno = make_annotation_block(name, s)

            # Find correct insertion point: after the opening brace,
            # replacing any old annotation that might be there.
            brace_line = None
            for i in range(start, min(start + 10, end)):
                stripped = lines[i].strip()
                if stripped == '{':
                    brace_line = i
                    break

            if brace_line is None:
                continue

            insert_after = brace_line + 1

            # Skip any existing annotation lines from older runs
            while insert_after < end and is_annotation_line(lines[insert_after].strip()):
                insert_after += 1

            insertions.append((insert_after, anno, brace_line + 1, insert_after - 1))

        # Apply in reverse order
        for insert_after, anno, old_start, old_end in reversed(insertions):
            # Remove old annotations
            if old_end >= old_start:
                del lines[old_start:old_end + 1]
                # Adjust insert_after since we removed lines before it
                insert_after = old_start

            # Insert new annotation
            for line in reversed(anno):
                lines.insert(insert_after, line)

        with open(filepath, 'w') as f:
            f.writelines(lines)

        print(f"  {filepath}: {len(insertions)} functions annotated")


if __name__ == '__main__':
    annotate_files()
