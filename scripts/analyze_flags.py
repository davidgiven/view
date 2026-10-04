#!/usr/bin/env python3
"""
Dataflow analysis for flag liveness in the 6502 C translation.

For each function, tracks flag definitions (from set_flags, cmp, adc, sbc, bit,
rol, ror, asr) and flag uses (if/while/assignments reading FLAG_*). Reports
calls where the defined flags are dead (never read before the next definition).

Usage: python3 scripts/analyze_flags.py [--replace] [file...]
  If no files given, analyses src/view.c, src/editor.c, src/printing.c,
  src/document.c, src/cli.c
"""

import re
import os

# Helpers that DEFINE flags
FLAG_DEFINERS = {'set_flags', 'cmp', 'adc', 'sbc', 'bit', 'rol', 'ror', 'asr'}

# adc and sbc internally READ the carry flag (C) from flags
ADC_SBC = {'adc', 'sbc'}

# Regexes
FUNC_START_RE = re.compile(
    r'^(?:static\s+)?'
    r'(?:void|uint8_t|uint16_t|addr_t|bool|int|char|long|unsigned|const|'
    r'struct\s+\w+|uint8_t\s*\*|char\s*\*)'
    r'\s+\**(\w+)\s*\('
)

FLAG_READ_RE = re.compile(
    r'\bif\s*\(.*flags.*FLAG_'
    r'|while\s*\(.*flags.*FLAG_'
    r'|flags\s*[&|]\s*FLAG_'
    r'|FLAG_\w+\s*[&|]\s*flags'
)

FLAG_DEF_TYPES = {
    r'\bset_flags\(&flags': ('helper', 'set_flags'),
    r'\bcmp\(&flags':       ('helper', 'cmp'),
    r'\badc\(&flags':       ('helper', 'adc'),
    r'\bsbc\(&flags':       ('helper', 'sbc'),
    r'\bbit\(&flags':       ('helper', 'bit'),
    r'\brol\(&flags':       ('helper', 'rol'),
    r'\brror\(&flags':      ('helper', 'ror'),
    r'\basr\(&flags':       ('helper', 'asr'),
}

INLINE_HELPERS = {
    'set_flags', 'cmp', 'adc', 'sbc', 'bit', 'rol', 'ror', 'asr', 'bit_val',
    '_tmp0', '_tmp1', '_tmp2', '_tmp3', '_tmp4', '_tmp5',
    '_tmp6', '_tmp7', '_tmp8', '_tmp9',
}


def find_functions(lines):
    """Return [(func_name, start_line, end_line)] for function definitions."""
    funcs = []
    for i, line in enumerate(lines):
        stripped = line.strip()
        if (not stripped or stripped.startswith('//') or
            stripped.startswith('/*') or stripped.startswith('*') or
            stripped.startswith('#') or stripped.endswith(';')):
            continue
        m = FUNC_START_RE.match(stripped)
        if m and m.group(1) not in INLINE_HELPERS:
            # Make sure this isn't a macro or declaration
            before_paren = stripped.split('(')[0]
            if not any(kw in before_paren for kw in ['if', 'while', 'for', 'switch']):
                funcs.append((m.group(1), i))
    # Compute end lines
    result = []
    for idx, (name, start) in enumerate(funcs):
        end = funcs[idx + 1][1] if idx + 1 < len(funcs) else len(lines)
        result.append((name, start, end))
    return result


def is_direct_flag_assign(stripped):
    """Check if line does a direct flags |= / flags &= / flags ="""
    return bool(re.search(r'\bflags\s*[|&]?=\s*', stripped))


def has_function_call(stripped):
    """Check if line has a non-helper function call that might modify globals."""
    # If it's a helper call or a simple assignment, no
    if any(h + '(&flags' in stripped for h in FLAG_DEFINERS):
        return False
    return bool(re.search(r'\b\w+\s*\(', stripped) and ';' in stripped)


def analyze_function_flags(lines, start, end, func_name):
    """
    Return list of (line_number, helper_name) for calls whose flag
    output is provably dead (never read before next flag write).
    """
    candidates = []
    
    # Track the current "live" flag definition
    # Each entry: (line_idx, helper_name, def_type, flags_set)
    # flags_set: set of flag bits that were set by this definition
    live_def = None
    # Has the live def been read?
    live_read = False
    
    for i in range(start, end):
        stripped = lines[i].strip()
        
        if stripped.startswith('//') or stripped == '':
            continue
        
        # Check for flag READ
        flag_read = bool(FLAG_READ_RE.search(stripped))
        
        # Check for indirect flag read via adc/sbc (they read C)
        # Also check for rol/ror which read C
        is_adc_sbc = any(h + '(&flags' in stripped for h in ADC_SBC)
        is_ror_rol = any(h + '(&flags' in stripped for h in ('rol', 'ror'))
        
        # Check for flag DEFINITION (direct or helper)
        def_type = None
        helper = None
        for pat, (dt, h) in FLAG_DEF_TYPES.items():
            if re.search(pat, stripped):
                def_type = dt
                helper = h
                break
        is_direct = is_direct_flag_assign(stripped)
        is_def = (def_type is not None or is_direct)
        
        # Process reads
        if flag_read and live_def is not None:
            live_read = True
        
        # adc/sbc/rol/ror read the CARRY flag
        if (is_adc_sbc or is_ror_rol) and live_def is not None:
            live_read = True
        
        # Process definitions
        if is_def:
            # If previous definition was never read → dead
            if live_def is not None and not live_read:
                prev_line, prev_helper, prev_def_type = live_def
                if prev_def_type == 'helper':
                    candidates.append((prev_line, prev_helper))
                # Direct flag assignments (flags |= etc.) can also be dead
                # but reporting them is less useful
            
            # Start new live definition
            live_def = (i, helper, def_type)
            live_read = False
            
            # If this is a direct flag assignment that also READs flags
            # (e.g., flags = flags & ~...), mark it as read
            if is_direct and ('flags &' in stripped or 'flags |' in stripped):
                live_read = True
        else:
            # Non-definition, non-read code
            # Function calls might modify flags indirectly
            if has_function_call(stripped) and not is_adc_sbc and not is_ror_rol:
                # Unknown function call kills liveness tracking
                if live_def is not None and not live_read and live_def[2] == 'helper':
                    candidates.append((live_def[0], live_def[1]))
                live_def = None
                live_read = False
    
    # End of function - check if final definition was never read
    if live_def is not None and not live_read and live_def[2] == 'helper':
        candidates.append((live_def[0], live_def[1]))
    
    return candidates


def generate_simplification(code, helper):
    """Generate simplified C for a dead-flag helper call."""
    if helper == 'asr':
        m = re.match(r'(\w+) = asr\(&flags, (\w+)\);', code)
        if m and m.group(1) == m.group(2):
            return f'{m.group(1)} = (uint8_t)((int8_t){m.group(1)} >> 1);'
        return None
    
    if helper == 'ror':
        m = re.match(r'(\w+) = ror\(&flags, (\w+)\);', code)
        if m and m.group(1) == m.group(2):
            v = m.group(1)
            return f'{v} = ({v} >> 1) | (({v} & 1) << 7);'
        return None
    
    if helper == 'rol':
        m = re.match(r'(\w+) = rol\(&flags, (\w+)\);', code)
        if m and m.group(1) == m.group(2):
            v = m.group(1)
            return f'{v} = ({v} << 1) | (({v} >> 7) & 1);'
        return None
    
    if helper == 'adc':
        m = re.match(r'a = adc\(&flags, a, (\w+)\);', code)
        if m:
            return f'a += {m.group(1)};'
        m = re.match(r'(\w+) = adc\(&flags, (\w+), (\w+)\);', code)
        if m and m.group(1) == m.group(2):
            return f'{m.group(1)} += {m.group(3)};'
        return None
    
    if helper == 'sbc':
        m = re.match(r'a = sbc\(&flags, a, (\w+)\);', code)
        if m:
            return f'a -= {m.group(1)};'
        m = re.match(r'(\w+) = sbc\(&flags, (\w+), (\w+)\);', code)
        if m and m.group(1) == m.group(2):
            return f'{m.group(1)} -= {m.group(3)};'
        return None
    
    return None


def analyze_file(filepath):
    """Return [(filepath, line, func, helper, code, suggestion)]."""
    with open(filepath) as f:
        lines = f.readlines()
    
    funcs = find_functions(lines)
    results = []
    
    for func_name, start, end in funcs:
        if func_name.startswith('_'):
            continue
        candidates = analyze_function_flags(lines, start, end, func_name)
        for line_idx, helper in candidates:
            code = lines[line_idx].strip()
            sugg = generate_simplification(code, helper)
            results.append((filepath, line_idx + 1, func_name, helper, code, sugg))
    
    return results


def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    default_files = ['src/view.c', 'src/editor.c', 'src/printing.c',
                     'src/document.c', 'src/cli.c']
    files = [os.path.join(os.path.dirname(script_dir), f)
             for f in default_files]
    
    all_results = []
    for filepath in files:
        try:
            results = analyze_file(filepath)
            all_results.extend(results)
        except FileNotFoundError:
            pass
    
    by_file = {}
    for r in all_results:
        by_file.setdefault(r[0], []).append(r)
    
    total_simplifiable = 0
    for filepath in sorted(by_file):
        results = by_file[filepath]
        print(f"\n{'='*70}")
        print(f"  {os.path.relpath(filepath)}")
        print(f"{'='*70}")
        for _, line, func, helper, code, sugg in sorted(results, key=lambda x: x[1]):
            if sugg:
                total_simplifiable += 1
                print(f"  L{line:5d} [{helper:9s}] [{func:35s}] {code:50s} → {sugg}")
            else:
                print(f"  L{line:5d} [{helper:9s}] [{func:35s}] {code}  (no simplifcation)")
    
    print(f"\n{'='*70}")
    print(f"  Total: {total_simplifiable} simplifiable calls")
    print(f"{'='*70}")


if __name__ == '__main__':
    main()
