#!/usr/bin/env python3
"""
Interprocedural flag-bit liveness analysis for the 6502 C translation.

Tracks each flag bit (C, Z, N, V) independently, plus a, x, y and tmp* vars.

Usage: python3 scripts/liveness.py [file...]
  Reports function summaries.
"""

import re
import os
import sys

# Individual flag bits tracked
FLAG_BITS = ['C', 'Z', 'N', 'V']

# Registers
REGISTERS = ['a', 'x', 'y']
TMP_VARS = ['tmp01', 'tmp23', 'tmp45', 'tmp67', 'tmp89']
ALL_VARS = REGISTERS + ['flags:' + b for b in FLAG_BITS] + TMP_VARS

BYTE_TO_COMBINED = {
    'tmp0': 'tmp01', 'tmp1': 'tmp01',
    'tmp2': 'tmp23', 'tmp3': 'tmp23',
    'tmp4': 'tmp45', 'tmp5': 'tmp45',
    'tmp6': 'tmp67', 'tmp7': 'tmp67',
    'tmp8': 'tmp89', 'tmp9': 'tmp89',
}

FUNC_START_RE = re.compile(
    r'^(?:static\s+)?'
    r'(?:void|uint8_t|uint16_t|addr_t|bool|int|char|long|unsigned|const|'
    r'struct\s+\w+|uint8_t\s*\*|char\s*\*)'
    r'\s+\**(\w+)\s*\('
)

INLINE_HELPERS = {
    'set_flags', 'cmp', 'adc', 'sbc', 'bit', 'rol', 'ror', 'asr', 'bit_val',
    '_tmp0', '_tmp1', '_tmp2', '_tmp3', '_tmp4', '_tmp5',
    '_tmp6', '_tmp7', '_tmp8', '_tmp9',
}

CALL_RE = re.compile(r'\b(\w+)\s*\(')

LIB_FUNCTIONS = {
    'exit', 'setjmp', 'longjmp', 'snprintf', 'sprintf', 'printf',
    'fopen', 'fclose', 'fputc', 'fgetc', 'fread', 'fwrite', 'fseek', 'ftell',
    'feof', 'ferror', 'rewind', 'fflush', 'ungetc',
    'cli_putchar', 'cli_putstring', 'cli_getchar', 'cli_readstring',
    'screen_putchar', 'screen_getchar', 'screen_getcursor', 'screen_setcursor',
    'screen_setstyle', 'screen_getsize', 'screen_enter', 'screen_leave',
    'screen_clear', 'screen_scrollup', 'screen_scrolldown', 'screen_enablecursor',
    'isupper', 'islower', 'isalpha', 'isdigit', 'isalnum', 'isspace',
    'toupper', 'tolower',
    'setbuffer', 'setlinebuf',
}


def fmt(v):
    """Format a variable name for display. Flag bits show as e.g. 'C', 'Z'."""
    if v.startswith('flags:'):
        return v[6:]
    return v


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
            if not any(re.split(r'\W+', before_paren)[0] == kw for kw in ['if', 'while', 'for', 'switch']):
                funcs.append((m.group(1), i))
    result = []
    for idx, (name, start) in enumerate(funcs):
        end = funcs[idx + 1][1] if idx + 1 < len(funcs) else len(lines)
        result.append((name, start, end))
    return result


# ─── Per-flag-bit def/use for each helper ─────────────────────────

def helper_flag_defs(helper):
    """Return set of flag bits that `helper` DEFINITIVELY writes."""
    table = {
        'set_flags': {'Z', 'N'},        # set_flags(v) sets Z, N from v; preserves C, V
        'cmp':       {'Z', 'N', 'C'},   # CMP sets Z, N, C
        'adc':       {'Z', 'N', 'C', 'V'},  # ADC sets all four
        'sbc':       {'Z', 'N', 'C', 'V'},  # SBC sets all four
        'bit':       {'Z', 'N', 'V'},   # BIT sets Z, N, V (doesn't touch C)
        'rol':       {'Z', 'N', 'C'},   # ROL sets Z, N, C
        'ror':       {'Z', 'N', 'C'},   # ROR sets Z, N, C
        'asr':       {'Z', 'N', 'C'},   # ASR (LSR) sets Z, N, C
    }
    return table.get(helper, set())


def helper_flag_uses(helper):
    """Return set of flag bits that `helper` READS as input."""
    table = {
        'adc':  {'C'},   # ADC reads carry
        'sbc':  {'C'},   # SBC reads carry
        'rol':  {'C'},   # ROL reads carry
        'ror':  {'C'},   # ROR reads carry
    }
    return table.get(helper, set())


# ─── Per-line def/use analysis ────────────────────────────────────

def line_defs_uses(stripped, index):
    """Return (defined_vars, used_vars) for one line.
    Variables are 'a', 'x', 'y', 'flags:C', 'flags:Z', etc., or tmpXX.
    """
    defs = set()
    uses = set()
    
    if stripped.startswith('//') or stripped == '':
        return defs, uses
    
    # ── helpers ──
    for helper in INLINE_HELPERS:
        if helper + '(&flags' in stripped:
            for b in helper_flag_defs(helper):
                defs.add('flags:' + b)
            for b in helper_flag_uses(helper):
                uses.add('flags:' + b)
            # a = adc(&flags, a, X) defines a
            if '= adc' in stripped or '= sbc' in stripped or helper in ('adc', 'sbc'):
                defs.add('a')
            elif helper in ('rol', 'ror', 'asr'):
                # VAR = ror(&flags, VAR) – check who's assigned
                m = re.match(r'\s*(\w+)\s*=\s*(?:rol|ror|asr)\(&flags,', stripped)
                if m:
                    defs.add(m.group(1))
                    uses.add(m.group(1))
            # cmp only sets flags, not a
            if helper in ('set_flags',):
                m = re.match(r'set_flags\(&flags, (\w+)\)', stripped)
                if m:
                    uses.add(m.group(1))
            # direct register assignment from helper
            if helper in ('rol', 'ror', 'asr'):
                m = re.match(r'(\w+)\s*=\s*' + helper + r'\(&flags, (\w+)\)', stripped)
                if m:
                    defs.add(m.group(1))
                    uses.add(m.group(2))
            break
    else:
        # ── not a helper: check for flag bit reads/writes ──
        
        # flags |= FLAG_C → defines C (does NOT read it in the 6502 sense;
        #   the |= reads the old value but we treat it as a def for liveness)
        for bit in FLAG_BITS:
            pat_assign = r'flags\s*[|&]?=\s*.*FLAG_' + bit
            if re.search(pat_assign, stripped):
                defs.add('flags:' + bit)
                # also read if it's a read-modify-write
                if 'flags &' in stripped or 'flags |' in stripped:
                    uses.add('flags:' + bit)
            
            # if (flags & FLAG_C) → reads C, Z, N, V depending on bit
            pat_read = r'if\s*\(.*flags.*FLAG_' + bit
            if re.search(pat_read, stripped):
                uses.add('flags:' + bit)
            
            # flags = flags & ~FLAG_C → defines C, reads C (RMW)
            pat_rmw = r'flags\s*=\s*flags\s*[&|].*FLAG_' + bit
            if re.search(pat_rmw, stripped):
                defs.add('flags:' + bit)
                uses.add('flags:' + bit)
        
        # Direct register assignments
        for var in REGISTERS:
            if re.search(r'\b' + var + r'\s*=', stripped):
                defs.add(var)
        
        # a += X, a -= X — reads and writes a
        for var in REGISTERS:
            if re.search(r'\b' + var + r'\s*[\+\-]=', stripped):
                defs.add(var)
                uses.add(var)
        
        # tmpX = a or tmpX = y — defines combined tmp, reads a/y
        for byte_var, combined in BYTE_TO_COMBINED.items():
            m = re.match(r'\s*' + byte_var + r'\s*=\s*([ayx]);', stripped)
            if m:
                defs.add(combined)
                uses.add(m.group(1))
        
        # Regular tmp assignments
        for var in TMP_VARS:
            if re.search(r'\b' + var + r'\s*=', stripped):
                defs.add(var)
        
        # Read a, x, y in expression context (not LHS)
        for var in REGISTERS:
            if re.search(r'(?<!\w)' + var + r'(?!\w)', stripped):
                if not re.search(r'\b' + var + r'\s*=', stripped) or re.search(r'\b' + var + r'\s*[\+\-]=', stripped):
                    uses.add(var)
    
    return defs, uses


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


def get_local_info(lines, start, end):
    local_defs = set()
    local_uses = set()
    local_live_in = set()
    defined_so_far = set()
    call_sites = []
    for i in range(start, end):
        stripped = lines[i].strip()
        if stripped.startswith('//') or stripped == '':
            continue
        d, u = line_defs_uses(stripped, i)
        
        # Any use of a variable that hasn't been defined yet on this path
        # makes it live-in.
        for v in u:
            if v not in defined_so_far:
                local_live_in.add(v)
        
        local_defs |= d
        local_uses |= u
        defined_so_far |= d
        
        callee = detect_call(stripped)
        if callee and callee not in LIB_FUNCTIONS:
            call_sites.append((i, callee))
            # A function call can define any register/flag/tmp.
            # Conservatively mark everything as defined so the
            # forward scan won't treat pre-call uses as live-in
            # when they're satisfied by the call's (unknown) defs.
            defined_so_far.update(ALL_VARS)
    return local_defs, local_uses, local_live_in, call_sites


# ─── Interprocedural fixed-point ──────────────────────────────────

def analyze_files(files):
    # Collect all function definitions
    all_funcs = {}
    for filepath in files:
        try:
            with open(filepath) as f:
                lines = f.readlines()
        except FileNotFoundError:
            continue
        for name, start, end in find_functions(lines):
            if name not in all_funcs:
                all_funcs[name] = (filepath, start, end, lines)
    
    # Compute local info
    local_info = {}
    for name, (filepath, start, end, lines) in all_funcs.items():
        d, u, li, cs = get_local_info(lines, start, end)
        local_info[name] = {'defs': d, 'uses': u, 'live_in': li, 'call_sites': cs, 'file': filepath}
    
    # Propagate through call graph until stable
    changed = True
    iteration = 0
    while changed and iteration < 20:
        changed = False
        iteration += 1
        for name in all_funcs:
            info = local_info[name]
            all_defs = set(info['defs'])
            all_uses = set(info['uses'])
            for _, callee in info['call_sites']:
                if callee in LIB_FUNCTIONS:
                    continue
                callee_info = local_info.get(callee)
                if callee_info is None:
                    all_defs.update(ALL_VARS)
                    all_uses.update(ALL_VARS)
                else:
                    all_defs.update(callee_info.get('cached_defs', callee_info['defs']))
                    all_uses.update(callee_info.get('cached_uses', callee_info['uses']))
            old_d = info.get('cached_defs')
            old_u = info.get('cached_uses')
            if all_defs != old_d or all_uses != old_u:
                info['cached_defs'] = all_defs
                info['cached_uses'] = all_uses
                changed = True
    
    # Build summaries
    summaries = {}
    for name in all_funcs:
        info = local_info[name]
        all_defs = info.get('cached_defs', info['defs'])
        all_uses = info.get('cached_uses', info['uses'])
        # live_in: variables used before any local definition (forward scan)
        live_in = info.get('live_in', info['uses'] - info['defs'])
        summaries[name] = {
            'defs': all_defs,
            'uses': all_uses,
            'live_in': live_in,
            'live_out': all_defs,
            'file': info['file'],
            'calls': [c for _, c in info['call_sites']],
        }
    return summaries


def format_vars(var_set):
    regs = sorted(v for v in var_set if v in REGISTERS)
    bits = sorted(v[6] for v in var_set if v.startswith('flags:'))
    tmps = sorted(v for v in var_set if v in TMP_VARS)
    parts = []
    if regs:
        parts.append(', '.join(regs))
    if bits:
        parts.append('|'.join(bits))
    if tmps:
        parts.append(', '.join(tmps))
    return '; '.join(parts) if parts else '(none)'


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
            calls_str = ', '.join(s['calls'][:5])
            if len(s['calls']) > 5:
                calls_str += f' … ({len(s["calls"])} total)'
            print(f"\n  {name}")
            print(f"    Live in:  {format_vars(s['live_in'])}")
            print(f"    Live out: {format_vars(s['live_out'])}")
            if s['calls']:
                print(f"    Calls:    {calls_str}")


if __name__ == '__main__':
    main()
