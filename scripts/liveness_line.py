#!/usr/bin/env python3
"""
Line-by-line liveness analysis.

Annotates each line of C source with the registers and flag bits
that are live at that program point.

Usage:
    python3 scripts/liveness_line.py          # annotate all .c files
    python3 scripts/liveness_line.py src/editor.c  # specific file
"""

import re, sys, os

sys.path.insert(0, os.path.dirname(__file__))
import liveness  # reuse helper tables (FLAG_BITS, REGISTERS, etc.)

# ─── Known corrupters ────────────────────────────────────────────────
# Functions that unconditionally write to these registers/flags.
# Extended from liveness.CORRUPTS.
CORRUPTS = dict(liveness.CORRUPTS)

# ─── Inline helper flag defs/uses (from liveness) ──────────────────
HELPER_FLAG_DEFS = liveness.HELPER_FLAG_DEFS
HELPER_FLAG_USES = liveness.HELPER_FLAG_USES
INLINE_HELPERS = liveness.INLINE_HELPERS
LIB_FUNCTIONS = liveness.LIB_FUNCTIONS
TRACKED_VARS = liveness.TRACKED_VARS
BYTE_TO_COMBINED = liveness.BYTE_TO_COMBINED
CALL_RE = liveness.CALL_RE

# ─── Flag ops from source ──────────────────────────────────────────
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

def get_var_defs_uses(text):
    """Track regular variable defs/uses from a line of C code."""
    d = set()
    u = set()
    for var in TRACKED_VARS:
        # Assignment
        if re.search(r'\b' + var + r'\s*=', text) and not re.search(r'\buint8_t\s+' + var + r'\b', text):
            d.add(var)
            if var in BYTE_TO_COMBINED:
                d.add(BYTE_TO_COMBINED[var])
            # a = adc(...) or a = sbc(...) also uses old a
            if var == 'a' and re.search(r'\ba\s*=\s*(adc|sbc)\(&flags,\s*a', text):
                u.add('a')
            elif var == 'a' and re.search(r'\ba\s*=\s*rol\(&flags,\s*a', text):
                u.add('a')
            elif var == 'a' and re.search(r'\ba\s*=\s*ror\(&flags,\s*a', text):
                u.add('a')
        # Increment/decrement/compound assignment (read + write)
        if re.search(r'\b' + var + r'\+{2}\b', text) or \
           re.search(r'\b' + var + r'--\b', text) or \
           re.search(r'\b' + var + r'\s*[\+\-]=', text):
            d.add(var)
            u.add(var)
            if var in BYTE_TO_COMBINED:
                d.add(BYTE_TO_COMBINED[var])
        # Read (not on LHS of plain assignment)
        if re.search(r'(?<!\w)' + var + r'(?!\w)', text):
            if not re.search(r'\b' + var + r'\s*=', text) or \
               re.search(r'\b' + var + r'\s*[\+\-]=', text) or \
               re.search(r'\b' + var + r'\+{2}\b', text) or \
               re.search(r'\b' + var + r'--\b', text):
                # But skip if it's a declaration
                if not re.search(r'\buint8_t\s+' + var + r'\b', text):
                    u.add(var)
    return d, u

def get_line_defs_uses(text):
    """Get all defs and uses for a single line of C code."""
    d = set()
    u = set()
    
    # Inline helper flag tracking
    for helper in INLINE_HELPERS:
        if helper + '(&flags' in text:
            for b in HELPER_FLAG_DEFS.get(helper, set()):
                d.add(f'flags:{b}')
            for b in HELPER_FLAG_USES.get(helper, set()):
                u.add(f'flags:{b}')
            # a = adc/sbc/rol/ror defines a
            if helper in ('adc', 'sbc', 'rol', 'ror') and '= ' + helper in text:
                d.add('a')
            # cmp/set_flags register argument is a use
            if helper in ('cmp', 'set_flags'):
                m = re.match(r'(?:set_flags|cmp)\(&flags,\s*(\w+)', text)
                if m:
                    u.add(m.group(1))
            # adc/sbc reads old a (already handled by var tracking)
            break
    
    # Flag bit-field ops
    fd, fu = get_flag_defs_uses(text)
    d |= fd
    u |= fu
    
    # Variable tracking
    vd, vu = get_var_defs_uses(text)
    d |= vd
    u |= vu
    
    # When combined variable is defined, also define its byte components
    for combined, bytes_list in [('tmp01', ['tmp0','tmp1']), ('tmp23', ['tmp2','tmp3']),
                                  ('tmp45', ['tmp4','tmp5']), ('tmp67', ['tmp6','tmp7']),
                                  ('tmp89', ['tmp8','tmp9'])]:
        if combined in d:
            for bv in bytes_list:
                d.add(bv)
    
    return d, u


# ─── Detect labels and gotos ────────────────────────────────────────
LABEL_RE = re.compile(r'^(\w+):\s*(?://.*)?$')

def find_labels_and_gotos(lines, func_start, func_end):
    """Find all labels and goto targets within a function body."""
    labels = set()
    gotos = {}  # line_index -> target_label
    branches = {}  # line_index -> target_label (for conditional gotos)
    
    for i in range(func_start, func_end):
        sl = lines[i].strip()
        if sl.startswith('//') or sl == '':
            continue
        
        # Label definition
        m = LABEL_RE.match(sl)
        if m:
            labels.add(m.group(1))
            continue
        
        # goto target
        m = re.match(r'goto\s+(\w+);', sl)
        if m:
            gotos[i] = m.group(1)
            continue
        
        # if (cond) goto target;
        m = re.match(r'if\s*\(.*\)\s*goto\s+(\w+);', sl)
        if m:
            branches[i] = m.group(1)
            continue
    
    return labels, gotos, branches


# ─── Per-function backward analysis ──────────────────────────────────
def analyze_function(lines, func_name, func_start, func_end, callee_live_out):
    """
    Do backward dataflow analysis for a single function.
    Returns a dict: {line_index: live_set_before_line}
    """
    # Find labels and gotos
    labels, gotos, branches = find_labels_and_gotos(lines, func_start, func_end)
    
    # Collect all label targets that are jumped to
    targeted_labels = set(gotos.values()) | set(branches.values())
    
    # Initialize: live after each line (None = unknown, set() = nothing live)
    live_after = {i: set() for i in range(func_start, func_end)}
    live_before = {i: set() for i in range(func_start, func_end)}
    
    # Iterate backward until stable
    changed = True
    max_iter = 50
    iteration = 0
    
    while changed and iteration < max_iter:
        changed = False
        iteration += 1
        
        # Start from the end of the function with empty live set
        live = set()
        
        for i in range(func_end - 1, func_start - 1, -1):
            sl = lines[i].strip()
            
            if sl.startswith('//') or sl == '':
                live_after[i] = set(live)
                live_before[i] = set(live)
                continue
            
            # Save live after this line
            live_after[i] = set(live)
            
            # Handle return: reset live set
            if sl == 'return;':
                live = set()
                live_before[i] = set(live)
                continue
            
            # Label definition: merge with live sets from all paths that jump here
            m = LABEL_RE.match(sl)
            if m:
                label = m.group(1)
                # Predecessors: any goto/branch targeting this label
                # For first iteration, use current live set
                # For subsequent iterations, union with existing live_before
                old_live = live_before.get(i, set())
                if label in targeted_labels:
                    # Union: the live set at a merge point is the union of
                    # the live sets from all predecessor paths.
                    # Since we're scanning backward, `live` is from
                    # one predecessor (the fall-through path).
                    # For jumps, we'll need another pass to propagate.
                    # For now, just take the union.
                    new_live = old_live | live
                    if new_live != old_live:
                        changed = True
                    live = new_live
                else:
                    live = old_live | live
                live_before[i] = set(live)
                continue
            
            # Get defs and uses for this line
            d, u = get_line_defs_uses(sl)
            
            # Handle function calls
            callee = CALL_RE.search(sl)
            callee_name = callee.group(1) if callee else None
            
            if callee_name and callee_name not in INLINE_HELPERS and \
               callee_name not in ('if', 'while', 'for', 'switch', 'sizeof'):
                if callee_name in LIB_FUNCTIONS:
                    # Library functions: assume they don't touch our regs/flags
                    pass
                elif callee_name in CORRUPTS:
                    corr = CORRUPTS[callee_name]
                    live = (live - corr) | u
                elif callee_name in callee_live_out:
                    clo = callee_live_out[callee_name]
                    live = (live - clo) | u
                else:
                    # Unknown function: assume it corrupts everything
                    live = u
            else:
                live = (live - d) | u
            
            live_before[i] = set(live)
    
    return live_before, live_after


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
def annotate_file(filepath, callee_live_out=None):
    """Annotate a single file with per-line liveness info."""
    if callee_live_out is None:
        callee_live_out = {}
    
    with open(filepath) as f:
        lines = f.readlines()
    
    # Find functions
    funcs = liveness.find_functions(lines)
    
    # First, compute per-function liveness (to get callee live_out)
    # Reuse liveness module's analysis for this
    summaries = {}
    try:
        summaries = liveness.analyze_files([filepath])
    except Exception:
        pass
    
    # Build callee_live_out from summaries
    clo = dict(callee_live_out)
    for name, s in summaries.items():
        clo[name] = s.get('live_out', set())
    
    # Annotate each function
    for func_name, func_start, func_end in funcs:
        live_before, live_after = analyze_function(
            lines, func_name, func_start, func_end, clo
        )
        
        # Insert annotations as comments
        new_lines = []
        for i in range(func_start, func_end):
            line = lines[i]
            stripped = line.strip()
            
            if stripped.startswith('//') or stripped == '':
                new_lines.append(line)
                continue
            
            lb = live_before.get(i, set())
            
            # Only annotate interesting lines (where something changes)
            # For now, annotate all lines with liveness info
            annotation = fmt_live_set(lb)
            if annotation:
                # Add annotation as end-of-line comment
                if not stripped.endswith(';') and not stripped.endswith('{') and not stripped.endswith('}'):
                    new_lines.append(line)
                else:
                    # Check if there's already a comment
                    if '//' in line:
                        # Insert before existing comment
                        parts = line.split('//', 1)
                        new_lines.append(f"{parts[0]} // {annotation}  |{parts[1]}")
                    else:
                        new_lines.append(line.rstrip() + f'  // {annotation}\n')
            else:
                new_lines.append(line)
        
        # Replace function body lines
        lines[func_start:func_end] = new_lines
    
    # Write back
    with open(filepath, 'w') as f:
        f.writelines(lines)
    
    return True


def main():
    files = sys.argv[1:] if len(sys.argv) > 1 else [
        'src/view.c', 'src/editor.c', 'src/printing.c',
        'src/document.c', 'src/cli.c'
    ]
    
    for filepath in files:
        print(f"Annotating {filepath}...")
        try:
            annotate_file(filepath)
            print(f"  Done.")
        except Exception as e:
            print(f"  Error: {e}")
    
    print("\nDone.")


if __name__ == '__main__':
    main()
