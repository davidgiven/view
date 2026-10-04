#!/usr/bin/env python3
"""Move all local variable declarations to as late as possible (C99).

For each function, each local var's declaration is moved to the point of
first use/def in the innermost block that dominates all uses.  Declarations
with initializers are kept with their init; otherwise the declaration is
inserted as `type name;` before first use and the original top declaration
is removed.
"""

import sys, re
import clang.cindex as c

CLANG_LIB = '/lib64/libclang.so.21.1'
INCLUDE_ARGS = [
    '-xc', '-Isrc',
    '-isystem', '/usr/lib/gcc/x86_64-redhat-linux/15/include',
    '-isystem', '/usr/include', '-isystem', '/usr/local/include',
    '-fparse-all-comments',
]
c.Config.set_library_file(CLANG_LIB)

def children(n): return list(n.get_children())

def col_to_off(src, line, col):
    if line <= 1: return col-1
    idx=0
    for _ in range(line-1):
        idx=src.index('\n', idx)+1
    return idx+col-1

def build_cfg_lite(fn):
    # minimal CFG builder just to get node_block for decl placement
    # reuse logic from ssa_split but simplified: we only need block nesting
    # For late decl we just need innermost block for each statement.
    # Use libclang parent chain via extent line numbers to infer blocks.
    pass
