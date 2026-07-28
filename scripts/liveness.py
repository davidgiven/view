#!/usr/bin/env python3
"""
Liveness analysis using libclang AST for forward scan,
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

def is_tracked(name):
    return name in TRACKED_VARS

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
    'cmd_err_no_target': ALL_VARS_SET,
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

# ─── Flag ops from source text ────────────────────────────────────
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

# ─── AST-based forward scan (replaces regex get_local_info) ───────
def get_local_info(lines, start, end, callee_live_out=None):
    """
    Forward scan using libclang AST.
    Returns (local_defs, local_uses, local_live_in, call_sites).
    """
    if callee_live_out is None:
        callee_live_out = {}
    
    local_defs = set()
    local_uses = set()
    local_live_in = set()
    defined_so_far = set()
    call_sites = []
    local_decls = set()
    
    # Add formal parameters (from function signature) to defined_so_far and local_decls
    if start < len(lines):
        sig = lines[start].strip()
        for var in TRACKED_VARS:
            if re.search(r'\b(?:uint8_t|uint16_t|addr_t)\s+' + var + r'\b', sig):
                defined_so_far.add(var)
                local_decls.add(var)
    
    for i in range(start, end):
        stripped = lines[i].strip()
        if stripped.startswith('//') or stripped == '':
            continue
        
        # Track local variable declarations (all types, all tracked vars)
        # Handles both `uint8_t var;` and `uint8_t var1, var2, var3;`
        for var in TRACKED_VARS:
            if re.search(r'\b(?:uint8_t|uint16_t|addr_t)\b(?:\s*\*?\s*\w+\s*,)*\s*' + var + r'\b', stripped):
                local_decls.add(var)
        
        if stripped == 'return;':
            # When processing linearly (forward scan), a `return;` on one
            # path doesn't affect variables defined on the OTHER path.
            # We don't reset defined_so_far here to avoid false live-in
            # variables that are defined only on the non-return path.
            # (However, this can cause false NEGATIVES — variables that
            # ARE truly undefined on a non-return path will not be flagged
            # as live-in.  A full control-flow analysis would fix this.)
            continue
        
        d = set()
        u = set()
        
        # ── Inline helper flag tracking ──
        for helper in INLINE_HELPERS:
            if helper + '(&flags' in stripped:
                for b in helper_flag_defs(helper):
                    d.add(f'flags:{b}')
                for b in helper_flag_uses(helper):
                    u.add(f'flags:{b}')
                # a = adc(&flags, ...) defines a (but only if not a local)
                if helper in ('adc', 'sbc', 'rol', 'ror'):
                    if '= ' + helper in stripped:
                        # Check if a is local
                        m = re.match(r'(a|x|y)\s*=\s*' + helper + r'\(&flags', stripped)
                        if m and m.group(1) not in local_decls:
                            d.add(m.group(1))
                # cmp/set_flags register argument is a use (skip if local)
                if helper in ('cmp', 'set_flags'):
                    m = re.match(r'(?:set_flags|cmp)\(&flags, (\w+)', stripped)
                    if m and m.group(1) not in local_decls:
                        u.add(m.group(1))
                break
        
        # ── Flag bit-field ops (regex) ──
        fd, fu = get_flag_defs_uses(stripped)
        d |= fd
        u |= fu
        
        # ── Track local C variable declarations (shadow globals) ──
        for var in TRACKED_VARS:
            if re.search(r'\buint8_t\s+' + var + r'\b', stripped):
                local_decls.add(var)
        
        # ── Regular variable tracking (regex) ──
        # Only track globals — skip variables that are shadowed by a local declaration
        for var in TRACKED_VARS:
            if var in local_decls:
                continue  # local variable shadows global; don't track
            if re.search(r'\b' + var + r'\s*=', stripped):
                d.add(var)
                # Also define the paired combined/byte variable
                if var in BYTE_TO_COMBINED:
                    d.add(BYTE_TO_COMBINED[var])
            # Detect byte-component assignment: ((uint8_t*)&tmp01)[0] = a
            if re.search(r'\(\(uint8_t\s*\*\)\s*&' + var + r'\)\s*\[', stripped):
                d.add(var)
                # Also define the paired combined/byte variable
                if var in BYTE_TO_COMBINED:
                    d.add(BYTE_TO_COMBINED[var])
            if re.search(r'\b' + var + r'\+{2}\b', stripped) or \
               re.search(r'\b' + var + r'--\b', stripped) or \
               re.search(r'\b' + var + r'\s*[\+\-]=', stripped):
                d.add(var)
                u.add(var)
                # Also define the paired combined/byte variable
                if var in BYTE_TO_COMBINED:
                    d.add(BYTE_TO_COMBINED[var])
        
        # When a combined variable is assigned, also define its byte components
        for combined, byte_var in [(cv, bv) for cv, bv_list in 
            [('tmp01', ['tmp0','tmp1']), ('tmp23', ['tmp2','tmp3']),
             ('tmp45', ['tmp4','tmp5']), ('tmp67', ['tmp6','tmp7']),
             ('tmp89', ['tmp8','tmp9'])] for bv in bv_list]:
            if combined in d:
                d.add(byte_var)
        
        # Variable reads (not LHS of assignment, not declaration,
        # not byte-component LHS like ((uint8_t*)&tmp01)[0] = ...)
        for var in TRACKED_VARS:
            if var in local_decls:
                continue  # local variable shadows global; don't track
            # Skip byte-component writes: ((uint8_t*)&var)[N] = ...
            if re.search(r'\(\(uint8_t\s*\*\)\s*&' + var + r'\)\s*\[', stripped):
                continue  # this is a byte-component WRITE, not a read
            if re.search(r'(?<!\w)' + var + r'(?!\w)', stripped):
                if not re.search(r'\b' + var + r'\s*=', stripped) or \
                   re.search(r'\b' + var + r'\s*[\+\-]=', stripped) or \
                   re.search(r'\b' + var + r'\+{2}\b', stripped) or \
                   re.search(r'\b' + var + r'--\b', stripped):
                    u.add(var)
        
        # Update live_in
        for v in u:
            if v not in defined_so_far:
                local_live_in.add(v)
        
        local_defs |= d
        local_uses |= u
        defined_so_far |= d
        
        # Detect calls
        callee = detect_call(stripped)
        if callee:
            if callee not in LIB_FUNCTIONS:
                call_sites.append((i, callee))
                # Use callee's actual live_out if known
                clo = callee_live_out.get(callee)
                if clo is None:
                    if callee in CORRUPTS:
                        # CORRUPTS = ALL_VARS_SET means noreturn (longjmp).
                        # Don't add to defined_so_far — code after is dead.
                        pass
                    elif callee in ALL_IN_OUT:
                        clo = ALL_VARS_SET
                    else:
                        clo = ALL_VARS_SET
                if clo is not None:
                    defined_so_far.update(clo)
            elif callee in CORRUPTS:
                # CORRUPTS means noreturn — don't update defined_so_far
                pass
    
    return local_defs, local_uses, local_live_in, call_sites


# ─── detect_call (kept from old module) ───────────────────────────
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


# ─── Function parameter extraction ────────────────────────────────
def get_func_params(lines, start):
    sig = lines[start].strip()
    m = re.match(r'(?:static\s+)?(?:\w+(?:\s*\*)?\s+)+\**(\w+)\s*\(([^)]*)\)', sig)
    if not m:
        return set()
    params_str = m.group(2).strip()
    if not params_str or params_str == 'void':
        return set()
    params = set()
    for p in params_str.split(','):
        p = p.strip()
        pm = re.match(r'(?:const\s+)?(?:\w+(?:\s*\*)?)\s+(\**\w+)', p)
        if pm:
            name = pm.group(1).lstrip('*')
            if name in REGISTERS:
                params.add(name)
    return params


# ─── find_functions (from old module, using regex for line numbers) ──
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
    
    # Compute function params
    func_params = {}
    for name, (filepath, start, end, lines) in all_funcs.items():
        func_params[name] = get_func_params(lines, start)
    
    # Initial forward pass
    local_info = {}
    for name, (filepath, start, end, lines) in all_funcs.items():
        d, u, li, cs = get_local_info(lines, start, end)
        # Compute local declarations for this function
        func_local_decls = set()
        for i in range(start, end):
            stripped = lines[i].strip()
            for var in TRACKED_VARS:
                if re.search(r'\b(?:uint8_t|uint16_t|addr_t)\b(?:\s*\*?\s*\w+\s*,)*\s*' + var + r'\b', stripped):
                    func_local_decls.add(var)
        # Also check function signature for parameters
        sig = lines[start].strip()
        for var in TRACKED_VARS:
            if re.search(r'\b(?:uint8_t|uint16_t|addr_t)\b(?:\s*\*?\s*\w+\s*,)*\s*' + var + r'\b', sig):
                func_local_decls.add(var)
        
        local_info[name] = {
            'defs': d, 'uses': u, 'live_in': li,
            'call_sites': cs, 'file': filepath,
            'local_decls': func_local_decls,
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
            for _, callee in info['call_sites']:
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
        
        # Backward scan
        backward_needs.clear()
        for name in all_funcs:
            lines = all_funcs[name][3]
            start = all_funcs[name][1]
            end = all_funcs[name][2]
            live = set()
            local_decls = set()
            # First pass: collect local declarations
            for i in range(start, end):
                stripped = lines[i].strip()
                for var in TRACKED_VARS:
                    if re.search(r'\b(?:uint8_t|uint16_t|addr_t)\b(?:\s*\*?\s*\w+\s*,)*\s*' + var + r'\b', stripped):
                        local_decls.add(var)
            # Also add function parameters
            sig = lines[start].strip()
            for var in TRACKED_VARS:
                if re.search(r'\b(?:uint8_t|uint16_t|addr_t)\s+' + var + r'\b', sig):
                    local_decls.add(var)
            
            for i in range(end - 1, start - 1, -1):
                stripped = lines[i].strip()
                if stripped.startswith('//') or stripped == '':
                    continue
                d, u = get_flag_defs_uses(stripped)
                # Also get var defs/uses via regex (skip locals)
                for var in TRACKED_VARS:
                    if var in local_decls:
                        continue
                    if re.search(r'\b' + var + r'\s*=', stripped):
                        d.add(var)
                    # Detect byte-component assignment in backward pass
                    if re.search(r'\(\(uint8_t\s*\*\)\s*&' + var + r'\)\s*\[', stripped):
                        d.add(var)
                    if re.search(r'(?<!\w)' + var + r'(?!\w)', stripped):
                        # Skip byte-component writes: ((uint8_t*)&var)[N] = ...
                        if re.search(r'\(\(uint8_t\s*\*\)\s*&' + var + r'\)\s*\[', stripped):
                            pass  # this is a byte-component WRITE, not a read
                        elif not re.search(r'\b' + var + r'\s*=', stripped):
                            u.add(var)
                        elif re.search(r'\b' + var + r'\s*[\+\-]=', stripped) or \
                             re.search(r'\b' + var + r'\+{2}\b', stripped) or \
                             re.search(r'\b' + var + r'--\b', stripped):
                            u.add(var)
                
                callee = detect_call(stripped)
                if stripped == 'return;':
                    live = set()
                elif callee:
                    if callee in ALL_IN_OUT:
                        backward_needs.setdefault(callee, set()).update(ALL_VARS_SET)
                        live = ALL_VARS_SET
                    elif callee not in LIB_FUNCTIONS:
                        backward_needs.setdefault(callee, set()).update(live)
                        if callee in CORRUPTS:
                            live = (live - CORRUPTS[callee]) | u
                        else:
                            live = (live - ALL_VARS_SET) | u
                    elif callee in CORRUPTS:
                        corr = CORRUPTS[callee]
                        live = (live - corr) | u
                    else:
                        live = live | u
                else:
                    live = (live - d) | u
        
        # Compute live_out
        for name in all_funcs:
            info = local_info[name]
            all_defs = info.get('cached_defs', info['defs'])
            required = backward_needs.get(name, set())
            new_out = required & all_defs
            
            # ALL_IN_OUT override
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
        
        for name, (filepath, start, end, lines) in all_funcs.items():
            d, u, li, cs = get_local_info(lines, start, end, clo)
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
        
        # Locals
        local_defs = info.get('defs', set())
        local_uses = info.get('uses', set())
        
        # Collect local declarations from the function body
        func_lines = all_funcs[name][3]
        body_start = all_funcs[name][1]
        body_end = all_funcs[name][2]
        local_decls_func = set()
        for i in range(body_start, body_end):
            stripped = func_lines[i].strip()
            for var in TRACKED_VARS:
                if re.search(r'\b(?:uint8_t|uint16_t|addr_t)\b(?:\s*\*?\s*\w+\s*,)*\s*' + re.escape(var) + r'\b', stripped):
                    local_decls_func.add(var)
        # Also check function signature for parameters
        sig = func_lines[body_start].strip()
        for var in TRACKED_VARS:
            if re.search(r'\b(?:uint8_t|uint16_t|addr_t)\b(?:\s*\*?\s*\w+\s*,)*\s*' + re.escape(var) + r'\b', sig):
                local_decls_func.add(var)
        
        # passed_to_callees
        passed_to_callees = set()
        lines = all_funcs[name][3]
        body_start = all_funcs[name][1]
        body_end = all_funcs[name][2]
        for i in range(body_start + 1, body_end):
            sl = lines[i].strip()
            if sl.startswith('//') or sl == '':
                continue
            callee = detect_call(sl)
            if callee and callee not in INLINE_HELPERS:
                d, u = get_flag_defs_uses(sl)
                for var in TRACKED_VARS:
                    if re.search(r'(?<!\w)' + var + r'(?!\w)', sl):
                        if not re.search(r'\b' + var + r'\s*=', sl):
                            passed_to_callees.add(var)
                # Callee's defs and params
                callee_params = func_params.get(callee, set())
                passed_to_callees |= callee_params
                callee_info = local_info.get(callee)
                if callee_info:
                    # Include variables the callee NEEDS as input (live_in).
                    passed_to_callees |= (callee_info.get('live_in', set()) - callee_params)
                    # Also include variables the callee DEFINES (writes to) as a GLOBAL
                    # (not shadowed by a local declaration). This catches cases where
                    # a callee modifies the global register as a side effect even when
                    # it doesn't need it as input.
                    callee_defs = callee_info.get('defs', set())
                    callee_locals = callee_info.get('local_decls', set())
                    passed_to_callees |= (callee_defs - callee_locals - callee_params)
                callee_needs = backward_needs.get(callee, set())
                passed_to_callees |= callee_needs
        
        # Variables that are pure locals (defined/used only in this function,
        # neither live-in nor live-out, not passed to callees).
        # These exclude C local variables that shadow globals (already filtered out above).
        # Subtract passed_to_callees (direct callee needs) — the consumed set
        # (transitive) will be subtracted in the post-processing pass below.
        globals_used_locally = ((local_defs & local_uses) - live_in - live_out - passed_to_callees - local_decls_func)
        # Also exclude flag bits from "locals" (they're always tracked separately)
        globals_used_locally = {v for v in globals_used_locally if not v.startswith('flags:')}
        
        consumed_here = set()
        
        summaries[name] = {
            'defs': all_defs,
            'uses': all_uses,
            'live_in': live_in,
            'live_out': live_out,
            'locals': globals_used_locally,
            'consumed': consumed_here,
            'file': info['file'],
            'calls': [c for _, c in info['call_sites']],
            'passed_to_callees': passed_to_callees,
            'local_decls': local_decls_func,
        }
    
    # ── Post-process: propagate consumed sets transitively ──
    # After all summaries are built, propagate consumed_by_callees
    # through the call graph until stable (fixed-point iteration).
    changed = True
    while changed:
        changed = False
        for name in all_funcs:
            s = summaries.get(name)
            if not s:
                continue
            consumed = set()
            # Collect live_in from direct callees
            for callee in s['calls']:
                cs = summaries.get(callee)
                if cs:
                    consumed.update(cs.get('live_in', set()))
                    # Also collect callee's OWN consumed (transitive)
                    consumed.update(cs.get('consumed', set()))
                elif callee in CORRUPTS:
                    # CORRUPTS functions corrupt everything
                    consumed.update(ALL_VARS_SET)
            # Remove live_in/live_out (these are the function's own interface)
            consumed = consumed - s['live_in'] - s['live_out']
            # Remove variables that have local C declarations (shadow globals)
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
        live_in = s.get('live_in', set())
        live_out = s.get('live_out', set())
        local_decls = s.get('local_decls', set())
        old_locals = s.get('locals', set())
        new_locals = old_locals - consumed - local_decls
        if new_locals != old_locals:
            s['locals'] = new_locals
    
    return summaries
    
    return summaries


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
            calls_str = ', '.join(s['calls'][:5])
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
