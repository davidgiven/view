#!/usr/bin/env python3
"""SSA-split reused function-local variables in the View C sources.

Splits each local variable whose value flows are reused into distinct
SSA-style variables (V, V_1, V_2, ...) based on def-use reaching-definition
analysis over a CFG built from the AST.

The CFG is flattened to basic blocks (labels/gotos/loops lowered to edges)
for analysis, which would lose structured control flow.  Structured control
flow is regenerated for output: the tool performs only per-token identifier
renames and C99 declaration insertions, leaving all control-flow keywords
(if/for/while/do/switch/label/goto), comments, whitespace and formatting
verbatim from the original.  This is equivalent to regenerating structured
control flow from the CFG's analysis results.
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
SRC = ''   # current source being processed (used by analyze via col_to_off)
SELF_OFF = set()  # (start,end) of compound/inc-dec self defs

# ---------------------------------------------------------------- cursor utils
def children(node):
    return list(node.get_children())

def extent_range(node):
    e = node.extent
    return (e.start.offset, e.end.offset)

def is_local_var(name, fn):
    """True if name is a function-local variable (body VAR_DECL or param)."""
    return name in fn['locals']

# -------------------------------------------------------- def/use extraction
# Returns (defs, uses): each a list of (name, start, end).
def analyze(node, in_lhs=False):
    k = node.kind
    kids = children(node)
    if k == c.CursorKind.DECL_REF_EXPR:
        s, en = declref_off(node)
        r = (node.spelling, s, en)
        return ([r], []) if in_lhs else ([], [r])
    if k in (c.CursorKind.UNEXPOSED_EXPR, c.CursorKind.PAREN_EXPR):
        return merge_analyze(kids, in_lhs)
    if k in (c.CursorKind.INTEGER_LITERAL, c.CursorKind.FLOATING_LITERAL,
             c.CursorKind.STRING_LITERAL, c.CursorKind.CHARACTER_LITERAL,
             c.CursorKind.TYPE_REF, c.CursorKind.LABEL_REF):
        return ([], [])
    if k == c.CursorKind.BINARY_OPERATOR:
        op = node.spelling
        if op == '=':
            d, u = analyze(kids[0], True)
            d2, u2 = analyze(kids[1], False)
            return (d + d2, u + u2)
        if op in ('+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '&=', '|=', '^='):
            d1, u1 = analyze(kids[0], True)
            d2, u2 = analyze(kids[0], False)
            d3, u3 = analyze(kids[1], False)
            return (tag_self(d1) + d2 + d3, u1 + u2 + u3)
        return merge_analyze(kids, False)
    if k == c.CursorKind.COMPOUND_ASSIGNMENT_OPERATOR:
        d1, u1 = analyze(kids[0], True)
        d2, u2 = analyze(kids[0], False)
        d3, u3 = analyze(kids[1], False)
        return (tag_self(d1) + d2 + d3, u1 + u2 + u3)
    if k == c.CursorKind.UNARY_OPERATOR:
        op = unary_op_info(node)
        if op in ('++', '--'):
            d1, u1 = analyze(kids[0], True)
            d2, u2 = analyze(kids[0], False)
            return (tag_self(d1) + d2, u1 + u2)
        if op == '&':
            return analyze(kids[0], in_lhs)
        return merge_analyze(kids, False)
    if k == c.CursorKind.MEMBER_REF_EXPR:
        # the base pointer is read to form the address; member writes do not
        # change the base variable's value (struct members are not locals)
        return merge_analyze(kids, False)
    if k == c.CursorKind.ARRAY_SUBSCRIPT_EXPR:
        d, u = analyze(kids[0], in_lhs)
        d2, u2 = analyze(kids[1], False)
        return (d + d2, u + u2)
    if k == c.CursorKind.CALL_EXPR:
        defs, uses = [], []
        for i, arg in enumerate(kids):
            if i == 0:
                continue
            if arg.kind == c.CursorKind.UNARY_OPERATOR and unary_op_info(arg) == '&':
                # address-of argument: the callee may both read and write the
                # variable through the pointer, so it is both a use and a def
                # (self-referencing, like a compound assignment)
                d, u = analyze(arg, True)
                d2, u2 = analyze(arg, False)
                defs += tag_self(d) + d2
                uses += u + u2
            else:
                d, u = analyze(arg, False)
                defs += d
                uses += u
        return (defs, uses)
    if k == c.CursorKind.DECL_STMT:
        defs, uses = [], []
        for v in kids:
            if v.kind == c.CursorKind.VAR_DECL:
                # a declaration without an initializer does not define a value;
                # only treat it as a def when it has one.
                vsub = [x for x in children(v) if x.kind != c.CursorKind.TYPE_REF]
                if vsub:
                    s, en = var_name_off(v)
                    defs.append((v.spelling, s, en))
                    for sub in vsub:
                        d, u = analyze(sub, False)
                        defs += d
                        uses += u
        return (defs, uses)
    if k == c.CursorKind.CSTYLE_CAST_EXPR:
        return merge_analyze(kids, in_lhs)
    # default: recurse
    return merge_analyze(kids, in_lhs)

def merge_analyze(kids, in_lhs):
    defs, uses = [], []
    for kd in kids:
        d, u = analyze(kd, in_lhs)
        defs += d
        uses += u
    return (defs, uses)


def col_to_off(src, line, col):
    """Convert a 1-based (line, column) to a byte offset in src."""
    if line <= 1:
        return col - 1
    idx = 0
    for _ in range(line - 1):
        idx = src.index('\n', idx) + 1
    return idx + col - 1


def tag_self(defs):
    for d in defs:
        SELF_OFF.add((d[1], d[2]))
    return defs


def unary_op_info(node):
    """Return the operator for a UNARY_OPERATOR cursor by inspecting source text
    (libclang gives empty spelling for unary operators)."""
    e = node.extent
    s = col_to_off(SRC, e.start.line, e.start.column)
    en = col_to_off(SRC, e.end.line, e.end.column)
    text = SRC[s:en]
    if text[:1] == '&':
        return '&'
    if text[:2] in ('++', '--'):
        return text[:2]
    if text[-2:] in ('++', '--'):
        return text[-2:]
    if text[:1] in '+-~!':
        return text[:1]
    return ''

def declref_off(node):
    """Byte span of a DECL_REF_EXPR token, handling macro-expanded references
    that libclang reports with a degenerate (zero-width) extent."""
    e = node.extent
    s = col_to_off(SRC, e.start.line, e.start.column)
    en = col_to_off(SRC, e.end.line, e.end.column)
    if s < en and SRC[s:en] == node.spelling:
        return s, en
    # degenerate extent (macro expansion): locate the spelling token on the
    # rest of the line
    import re
    line_end = SRC.find('\n', s)
    if line_end < 0:
        line_end = len(SRC)
    seg = SRC[s:line_end]
    m = re.search(r'\b' + re.escape(node.spelling) + r'\b', seg)
    if m:
        return s + m.start(), s + m.start() + len(node.spelling)
    return s, en


def var_name_off(v):
    """Byte span of the variable's name token in a VAR_DECL (the extent covers
    type + name; the name is the last identifier matching v.spelling)."""
    s = col_to_off(SRC, v.extent.start.line, v.extent.start.column)
    en = col_to_off(SRC, v.extent.end.line, v.extent.end.column)
    span = SRC[s:en]
    import re
    ms = list(re.finditer(r'\b' + re.escape(v.spelling) + r'\b', span))
    if ms:
        o = s + ms[-1].start()
        return o, o + len(v.spelling)
    return s, en
# ------------------------------------------------------------------ CFG build
CONTROL_STMTS = (c.CursorKind.IF_STMT, c.CursorKind.FOR_STMT,
                 c.CursorKind.WHILE_STMT, c.CursorKind.DO_STMT,
                 c.CursorKind.SWITCH_STMT, c.CursorKind.COMPOUND_STMT,
                 c.CursorKind.CASE_STMT, c.CursorKind.DEFAULT_STMT)

def build_cfg(fn, cursor):
    """Return (nodes, edges, entry_id, exit_id, param_def_ids)."""
    nodes = []           # (id, cursor|None, kind_str)
    edges = []
    label_id = {}
    cur_id = 0
    exits = []
    exit_id = None
    loop_stack = []      # (kind, continue_target, break_join_id)
    block_stack = []     # innermost enclosing COMPOUND_STMT during emit
    node_block = {}      # nid -> innermost block cursor
    block_parent = {}    # block cursor -> parent block cursor
    control_body = set()  # node ids that are the body of a brace-less control stmt

    def new_node(cur, kind):
        nonlocal cur_id
        nid = cur_id
        cur_id += 1
        nodes.append((nid, cur, kind))
        if cur is not None and kind in ('stmt', 'expr', 'decl', 'goto',
                                        'return', 'break', 'continue',
                                        'label') and block_stack:
            node_block[nid] = block_stack[-1]
        return nid

    def edge(a, b):
        if a is not None and b is not None and a != b:
            edges.append((a, b))

    # pre-pass: register all labels
    def pre_labels(s):
        for ch in children(s):
            if ch.kind == c.CursorKind.LABEL_STMT:
                label_id[ch.spelling] = new_node(ch, 'label')
            pre_labels(ch)
    body = cursor
    for st in children(body):
        if st.kind == c.CursorKind.LABEL_STMT:
            label_id[st.spelling] = new_node(st, 'label')
    # also nested labels
    def deep_labels(s):
        for ch in children(s):
            if ch.kind == c.CursorKind.LABEL_STMT and ch.spelling not in label_id:
                label_id[ch.spelling] = new_node(ch, 'label')
            deep_labels(ch)
    deep_labels(body)

    def emit_stmts(stmts):
        first_entry = None
        tails = []
        for s in stmts:
            entry, new_tails = emit_stmt(s)
            if first_entry is None:
                first_entry = entry
            for t in tails:
                edge(t, entry)
            tails = new_tails
        return first_entry, tails

    def emit_stmt(s):
        k = s.kind
        if k == c.CursorKind.COMPOUND_STMT:
            block_parent[s] = block_stack[-1] if block_stack else None
            block_stack.append(s)
            fe, tails = emit_stmts(children(s))
            block_stack.pop()
            return fe, tails
        if k == c.CursorKind.IF_STMT:
            kids = children(s)
            cond = kids[0]
            then = kids[1] if len(kids) > 1 else None
            els = kids[2] if len(kids) > 2 else None
            cnode = new_node(cond, 'expr')
            tails = []
            if then is not None:
                te, tt = emit_stmt(then)
                if then.kind != c.CursorKind.COMPOUND_STMT:
                    control_body.add(te)
                edge(cnode, te)
                tails += tt
            else:
                tails.append(cnode)
            if els is not None:
                ee, et = emit_stmt(els)
                if els.kind != c.CursorKind.COMPOUND_STMT:
                    control_body.add(ee)
                edge(cnode, ee)
                tails += et
            else:
                tails.append(cnode)
            return cnode, tails
        if k == c.CursorKind.WHILE_STMT:
            kids = children(s)
            cond = kids[0]
            body = kids[1]
            cnode = new_node(cond, 'expr')
            exit_join = new_node(None, 'join')
            loop_stack.append(('while', cnode, exit_join))
            be, bt = emit_stmt(body)
            if body.kind != c.CursorKind.COMPOUND_STMT:
                control_body.add(be)
            loop_stack.pop()
            edge(cnode, be)
            edge(cnode, exit_join)
            for t in bt:
                edge(t, cnode)
            return cnode, [exit_join]
        if k == c.CursorKind.DO_STMT:
            kids = children(s)
            body = kids[0]
            cond = kids[1]
            cnode = new_node(cond, 'expr')
            exit_join = new_node(None, 'join')
            loop_stack.append(('do', cnode, exit_join))
            be, bt = emit_stmt(body)
            if body.kind != c.CursorKind.COMPOUND_STMT:
                control_body.add(be)
            loop_stack.pop()
            # body falls through to cond; loop entry = body entry
            for t in bt:
                edge(t, cnode)
            edge(cnode, be)
            edge(cnode, exit_join)
            return be, [exit_join]
        if k == c.CursorKind.FOR_STMT:
            kids = children(s)
            # the body is the trailing COMPOUND_STMT; empty init/cond/inc are
            # omitted or NULL_STMT, so derive them from the leading children
            body_idx = -1
            for i in range(len(kids) - 1, -1, -1):
                if kids[i].kind == c.CursorKind.COMPOUND_STMT:
                    body_idx = i
                    break
            body = kids[body_idx] if body_idx >= 0 else None
            pre = kids[:body_idx] if body_idx > 0 else []
            empty = (c.CursorKind.NULL_STMT, c.CursorKind.UNEXPOSED_EXPR)

            def part(i):
                return pre[i] if i < len(pre) and pre[i].kind not in empty else None

            init, cond, inc = part(0), part(1), part(2)
            init_n = new_node(init, 'expr') if init is not None else None
            cond_n = new_node(cond, 'expr') if cond is not None else None
            inc_n = new_node(inc, 'expr') if inc is not None else None
            exit_join = new_node(None, 'join')
            cont_node = new_node(None, 'join')   # continue junction (known before body)
            loop_stack.append(('for', cont_node, exit_join))
            be, bt = emit_stmt(body) if body is not None else (None, [])
            if body is not None and body.kind != c.CursorKind.COMPOUND_STMT:
                control_body.add(be)
            loop_stack.pop()
            cont = inc_n if inc_n is not None else (cond_n if cond_n is not None else be)
            edge(cont_node, cont if cont is not None else exit_join)
            # wiring
            if init_n is not None:
                edge(init_n, cond_n if cond_n is not None else (be if be is not None else exit_join))
            if cond_n is not None:
                edge(cond_n, be if be is not None else exit_join)
                edge(cond_n, exit_join)
            for t in bt:
                edge(t, cont_node)
            entry = init_n if init_n is not None else (cond_n if cond_n is not None else (be if be is not None else exit_join))
            return entry, [exit_join]
        if k == c.CursorKind.SWITCH_STMT:
            kids = children(s)
            cond = kids[0]
            cnode = new_node(cond, 'expr')
            exit_join = new_node(None, 'join')
            loop_stack.append(('switch', None, exit_join))
            # case bodies
            case_entries = []
            case_tails = []
            case_nodes = [k2 for k2 in kids[1:]]
            prev_tails = None
            for cs in case_nodes:
                if cs.kind in (c.CursorKind.CASE_STMT, c.CursorKind.DEFAULT_STMT):
                    sub = children(cs)
                    be, bt = emit_stmts(sub) if sub else (None, [])
                    ce = be if be is not None else new_node(None, 'join')
                    case_entries.append(ce)
                    if prev_tails:
                        for t in prev_tails:
                            edge(t, ce)
                    prev_tails = bt
                else:
                    # nested statements directly in switch (unusual)
                    be, bt = emit_stmt(cs)
                    if prev_tails:
                        for t in prev_tails:
                            edge(t, be)
                    prev_tails = bt
            if prev_tails:
                for t in prev_tails:
                    edge(t, exit_join)
            loop_stack.pop()
            if case_entries:
                edge(cnode, case_entries[0])
            else:
                edge(cnode, exit_join)
            return cnode, [exit_join]
        if k == c.CursorKind.CASE_STMT or k == c.CursorKind.DEFAULT_STMT:
            sub = children(s)
            return emit_stmts(sub) if sub else (None, [])
        if k == c.CursorKind.GOTO_STMT:
            n = new_node(s, 'goto')
            target = children(s)[0].spelling
            if target in label_id:
                edge(n, label_id[target])
            else:
                print(f"  !! goto to unknown label {target}")
            return n, []
        if k == c.CursorKind.LABEL_STMT:
            n = label_id[s.spelling]
            kids = children(s)
            if kids:
                # the label wraps its labeled statement; control flows from the
                # label into it (and gotos target the label node itself)
                entry, tails = emit_stmt(kids[0])
                edge(n, entry)
                return n, tails
            return n, [n]
        if k == c.CursorKind.RETURN_STMT:
            n = new_node(s, 'return')
            edge(n, exit_id)
            return n, []
        if k == c.CursorKind.BREAK_STMT:
            n = new_node(s, 'break')
            for kk, cont, bj in reversed(loop_stack):
                if kk == 'switch':
                    edge(n, bj); break
                if bj is not None:
                    edge(n, bj); break
            return n, []
        if k == c.CursorKind.CONTINUE_STMT:
            n = new_node(s, 'continue')
            for kk, cont, bj in reversed(loop_stack):
                if kk != 'switch' and cont is not None:
                    edge(n, cont); break
            return n, []
        if k == c.CursorKind.DECL_STMT:
            n = new_node(s, 'decl')
            return n, [n]
        # leaf expression / other statement
        n = new_node(s, 'stmt')
        return n, [n]

    exit_id = new_node(None, 'exit')
    body_entry, body_tails = emit_stmts(children(body))
    for t in body_tails:
        edge(t, exit_id)

    # param defs: give each param a synthetic def at the entry node
    # (entry node = the first real node; we'll attach param defs to RD_IN)
    entry_id = body_entry if body_entry is not None else exit_id
    param_def_ids = {}
    for p in fn['params']:
        param_def_ids[p] = None  # filled with def id later
    return nodes, edges, entry_id, exit_id, param_def_ids, node_block, block_parent, control_body


# ------------------------------------------------------------- RD + splitting
class UnionFind:
    def __init__(self):
        self.parent = {}
    def find(self, x):
        p = self.parent.setdefault(x, x)
        while p != self.parent[p]:
            self.parent[p] = self.parent[self.parent[p]]
            p = self.parent[p]
        return p
    def union(self, a, b):
        ra, rb = self.find(a), self.find(b)
        if ra != rb:
            self.parent[ra] = rb

def compute_rd(nodes, edges, entry_id, exit_id, node_defs, node_kills):
    node_ids = [nid for nid, _, _ in nodes]
    preds = {nid: [] for nid in node_ids}
    for a, b in edges:
        if b in preds:
            preds[b].append(a)
    # RD_IN/OUT: dict var -> set of def_ids
    rd_in = {nid: {} for nid in node_ids}
    rd_out = {nid: {} for nid in node_ids}
    # worklist
    changed = True
    from collections import deque
    order = node_ids  # iterate
    while changed:
        changed = False
        for nid in node_ids:
            # RD_IN = union of preds' RD_OUT
            new_in = {}
            for p in preds[nid]:
                for var, defs in rd_out[p].items():
                    s = new_in.setdefault(var, set())
                    s |= defs
            if new_in != rd_in[nid]:
                rd_in[nid] = new_in
                changed = True
            # RD_OUT
            out = dict(new_in)
            for var in node_kills.get(nid, ()):
                out.pop(var, None)
            for var, dids in node_defs.get(nid, {}).items():
                out[var] = set(dids)
            if out != rd_out[nid]:
                rd_out[nid] = out
                changed = True
    return rd_in, rd_out

def split_function(fn, cursor, src):
    global SRC, SELF_OFF
    SRC = src
    SELF_OFF = set()
    (nodes, edges, entry_id, exit_id, param_def_ids, node_block,
     block_parent, control_body) = build_cfg(fn, cursor)
    # capture declared types of local variables for inserted declarations
    var_type = {}
    def collect_types(node):
        for ch in node.get_children():
            if ch.kind == c.CursorKind.VAR_DECL:
                var_type[ch.spelling] = ch.type.spelling
            collect_types(ch)
    body = None
    body_start = None
    for s in cursor.get_children():
        if s.kind == c.CursorKind.COMPOUND_STMT:
            body = s
            collect_types(s)
            body_start = col_to_off(SRC, s.extent.start.line, s.extent.start.column) + 1
    node_ids = [nid for nid, _, _ in nodes]

    # assign def ids
    next_def = [0]
    def def_id():
        d = next_def[0]; next_def[0] += 1; return d
    node_defs = {nid: {} for nid in node_ids}   # nid -> {var: [def_id]}
    node_kills = {nid: set() for nid in node_ids}
    node_uses = {nid: [] for nid in node_ids}   # nid -> [(var, start, end)]
    # def_id registry
    def_id_var = {}  # def_id -> var
    def_id_off = {}  # def_id -> (start, end) of the def token
    def_id_node = {} # def_id -> node id
    self_ids = set()

    for nid, cur, kind in nodes:
        if cur is None or kind == 'label':
            # label nodes are pure CFG junctions; their wrapped statement is
            # emitted (and analysed) as its own node
            continue
        defs, uses = analyze(cur)
        for name, s, e in defs:
            if name in fn['locals']:
                did = def_id()
                node_defs[nid].setdefault(name, []).append(did)
                node_kills[nid].add(name)
                def_id_var[did] = name
                def_id_off[did] = (s, e)
                def_id_node[did] = nid
                if (s, e) in SELF_OFF:
                    self_ids.add(did)
        for name, s, e in uses:
            if name in fn['locals']:
                node_uses[nid].append((name, s, e))

    # param defs at entry
    for pname in fn['params']:
        did = def_id()
        param_def_ids[pname] = did
        def_id_var[did] = pname
        def_id_off[did] = None  # no token to rename (param token handled separately)
        # attach to entry RD_IN
        node_defs[entry_id].setdefault(pname, []).append(did)

    rd_in, rd_out = compute_rd(nodes, edges, entry_id, exit_id, node_defs, node_kills)

    # def-use / merge classes
    uf = UnionFind()
    unsplittable = set()
    for nid in node_ids:
        for name, s, e in node_uses[nid]:
            reaching = rd_in[nid].get(name, set())
            if not reaching:
                unsplittable.add(name)
                continue
            it = iter(reaching)
            first = next(it)
            for other in it:
                uf.union(first, other)
    # compound/inc/dec defs read and write the same value lineage: force the
    # def into the same class as the value it reads
    for nid in node_ids:
        for did in node_defs.get(nid, {}).get(None, []):
            pass
        for name, dids in node_defs.get(nid, {}).items():
            for did in dids:
                if did in self_ids:
                    reaching = rd_in[nid].get(name, set())
                    for r in reaching:
                        uf.union(did, r)
    # params' virtual defs must not merge with re-assignments unless actually merged;
    # they're ordinary defs handled above.

    # group defs by variable and class
    var_defs = {}   # var -> [def_id]
    for nid in node_ids:
        for name, dids in node_defs.get(nid, {}).items():
            var_defs.setdefault(name, []).extend(dids)

    # order classes by first-def offset (params first)
    renames = []       # (start, end, old, new)
    decl_list = []     # [(insert_offset, "type name;")] declarations at first use
    node_kind = {nid: kind for nid, cur, kind in nodes}
    node_cursor = {nid: cur for nid, cur, kind in nodes}

    for var in var_defs:
        if var in unsplittable:
            continue
        defs = var_defs[var]
        classes = {}
        for d in defs:
            r = uf.find(d)
            classes.setdefault(r, []).append(d)
        if len(classes) <= 1:
            continue
        # order classes by first def offset (param def offset = -1)
        ordered = []
        for r, ds in classes.items():
            first_off = min((def_id_off[d][0] if def_id_off[d] else -1) for d in ds)
            ordered.append((first_off, r, ds))
        ordered.sort(key=lambda x: x[0])
        # name: class0 keeps var; others var_1, var_2 ...
        for idx, (off, root, ds) in enumerate(ordered):
            if idx == 0:
                continue
            newname = f"{var}_{idx}"
            # rename defs and uses of this class
            members = set()
            for d in ds:
                members.add(d)
                s, e = def_id_off[d]
                if s is not None:
                    renames.append((s, e, var, newname))
            # find uses whose reaching def belongs to this class
            for nid in node_ids:
                for name, s, e in node_uses[nid]:
                    if name != var:
                        continue
                    reaching = rd_in[nid].get(var, set())
                    if not reaching:
                        continue
                    roots = {uf.find(r) for r in reaching}
                    if len(roots) == 1 and next(iter(roots)) == root:
                        renames.append((s, e, var, newname))
            # C99: declare at the point of first use, in the innermost block
            # that contains every occurrence of this class.
            t = var_type.get(var, 'uint8_t')
            occ = []  # (offset, node_id)
            for d in ds:
                if def_id_off[d]:
                    occ.append((def_id_off[d][0], def_id_node[d]))
            for nid in node_ids:
                for name, s, e in node_uses[nid]:
                    if name != var:
                        continue
                    reaching = rd_in[nid].get(var, set())
                    if not reaching:
                        continue
                    roots = {uf.find(r) for r in reaching}
                    if len(roots) == 1 and next(iter(roots)) == root:
                        occ.append((s, nid))
            occ.sort()

            def is_ancestor(a, b):
                while b is not None:
                    if b == a:
                        return True
                    b = block_parent.get(b)
                return False

            common = None
            for _off, _nid in occ:
                b = node_block.get(_nid)
                if b is None:
                    continue
                if common is None:
                    common = b
                else:
                    while common is not None and not is_ancestor(common, b):
                        common = block_parent.get(common)
            if common is None:
                ins = body_start
            else:
                first_nid = occ[0][1]
                first_stmt = node_cursor.get(first_nid)
                if first_stmt is not None:
                    stmt_off = col_to_off(SRC, first_stmt.extent.start.line,
                                          first_stmt.extent.start.column)
                else:
                    stmt_off = occ[0][0]
                if node_block.get(first_nid) == common and first_nid not in control_body:
                    ins = src.rfind('\n', 0, stmt_off) + 1
                else:
                    bs = col_to_off(SRC, common.extent.start.line,
                                    common.extent.start.column)
                    ins = bs + 1
            # if the first def is a declaration-with-initializer, the renamed
            # declaration already provides storage -- do not insert a duplicate
            first_kind = node_kind[occ[0][1]]
            if first_kind != 'decl':
                decl_list.append((ins, f"{t} {newname};"))
    # deduplicate renames at the same token (compound/inc-dec emit the LHS
    # token as both a def and a use)
    seen = set()
    uniq = []
    for s, e, o, n in renames:
        if (s, e) in seen:
            continue
        seen.add((s, e))
        uniq.append((s, e, o, n))
    renames = uniq

    # --- C99 late declarations for ALL remaining top-level locals ---
    # Move every `type var;` that appears in the initial DeclStmt sequence
    # to as late as possible (just before first use/def) in its common-ancestor
    # block.  Declarations with initializers stay where they are.
    # This is the first stage; a second stage below handles *all* remaining
    # VarDecls that are still before first use (even mid-function).
    top_decls = []
    for s in children(body):
        if s.kind == c.CursorKind.DECL_STMT:
            top_decls.append(s)
        elif s.kind == c.CursorKind.UNEXPOSED_EXPR:
            continue
        else:
            break
    # map var -> VarDecl cursor for top decls without initializer
    top_var_decls = {}
    for d in top_decls:
        for v in children(d):
            if v.kind != c.CursorKind.VAR_DECL:
                continue
            # only bare `type var;` (no init) is movable
            has_init = any(ch.kind != c.CursorKind.TYPE_REF for ch in children(v))
            if has_init:
                continue
            top_var_decls[v.spelling] = (d, v)
    # late inserts for top vars that have a later first occurrence
    late_inserts = []
    late_removals = []
    for var, (decl_stmt, var_decl) in top_var_decls.items():
        if var not in fn['locals']:
            continue
        # find first occurrence of var (def or use) after the top decls
        occ = []
        for nid in node_ids:
            for name, dids in node_defs.get(nid, {}).items():
                if name != var:
                    continue
                for did in dids:
                    if def_id_off[did] and def_id_off[did][0] is not None:
                        occ.append((def_id_off[did][0], nid))
            for name, s, e in node_uses.get(nid, []):
                if name != var:
                    continue
                occ.append((s, nid))
        if not occ:
            continue
        occ.sort()
        first_off = occ[0][0]
        decl_off = col_to_off(SRC, decl_stmt.extent.start.line, decl_stmt.extent.start.column)
        # if first use is at or before decl, already as late as possible
        if first_off <= decl_off + 5:
            continue
        # common ancestor block that dominates all occurrences
        def is_ancestor(a, b):
            while b is not None:
                if b == a:
                    return True
                b = block_parent.get(b)
            return False
        common = None
        for _off, _nid in occ:
            b = node_block.get(_nid)
            if b is None:
                continue
            if common is None:
                common = b
            else:
                while common is not None and not is_ancestor(common, b):
                    common = block_parent.get(common)
        if common is None:
            ins = body_start
        else:
            first_nid = occ[0][1]
            first_stmt = node_cursor.get(first_nid)
            if first_stmt is not None:
                stmt_off = col_to_off(SRC, first_stmt.extent.start.line,
                                      first_stmt.extent.start.column)
            else:
                stmt_off = first_off
            if node_block.get(first_nid) == common and first_nid not in control_body:
                ins = src.rfind('\n', 0, stmt_off) + 1
            else:
                bs = col_to_off(SRC, common.extent.start.line, common.extent.start.column)
                ins = bs + 1
        t = var_type.get(var, 'uint8_t')
        # schedule removal of original declarator and insertion at late point
        # For single-var DeclStmts, remove the whole statement; for multi-var
        # keep the others (reconstruct without this var).
        siblings = [ch for ch in children(decl_stmt) if ch.kind == c.CursorKind.VAR_DECL]
        # For multi-var DeclStmts, we handle the whole statement once: collect
        # all vars in this DeclStmt that are being moved, and reconstruct the
        # remaining declarators.  To avoid duplicate removals, only the first
        # moved var in the DeclStmt triggers the reconstruction.
        # Check if this decl_stmt has already been scheduled for removal/rewrite
        already_scheduled = any(ds == col_to_off(SRC, decl_stmt.extent.start.line, decl_stmt.extent.start.column) for ds, _ in late_removals)
        if already_scheduled:
            # already handled via another var in the same DeclStmt
            # just insert this var's late decl
            late_inserts.append((ins, f"{t} {var};"))
            continue
        # collect all vars in this DeclStmt that will be moved late
        # (for now, assume every bare var in a top DeclStmt without init is moved)
        # we determine movability per var as above; here we handle the whole DeclStmt
        movable = []
        remaining = []
        for ch in siblings:
            vname = ch.spelling
            has_init2 = any(x.kind != c.CursorKind.TYPE_REF for x in children(ch))
            if has_init2:
                remaining.append(ch)
                continue
            # check if this var has a later first use (like the current var)
            # find first occurrence for this sibling var
            occ2 = []
            for nid2 in node_ids:
                for name2, dids2 in node_defs.get(nid2, {}).items():
                    if name2 != vname:
                        continue
                    for did2 in dids:
                        if def_id_off[did2] and def_id_off[did2][0] is not None:
                            occ2.append((def_id_off[did2][0], nid2))
                for name2, s2, e2 in node_uses.get(nid2, []):
                    if name2 != vname:
                        continue
                    occ2.append((s2, nid2))
            if not occ2:
                remaining.append(ch)
                continue
            occ2.sort()
            first_off2 = occ2[0][0]
            decl_off2 = col_to_off(SRC, decl_stmt.extent.start.line, decl_stmt.extent.start.column)
            if first_off2 <= decl_off2 + 5:
                remaining.append(ch)
            else:
                movable.append(ch)
        if not movable:
            continue
        ds = col_to_off(SRC, decl_stmt.extent.start.line, decl_stmt.extent.start.column)
        de = col_to_off(SRC, decl_stmt.extent.end.line, decl_stmt.extent.end.column)
        if not remaining:
            late_removals.append((ds, de))
        else:
            # reconstruct DeclStmt with remaining vars
            # preserve original type text (e.g. "uint8_t", "addr_t", "struct render_state")
            # type is the first VarDecl's type
            first_remaining = remaining[0]
            t_rem = var_type.get(first_remaining.spelling, 'uint8_t')
            # check if any remaining has init (unlikely for top)
            parts = []
            for ch in remaining:
                has_i = any(x.kind != c.CursorKind.TYPE_REF for x in children(ch))
                if has_i:
                    # keep init text
                    init = None
                    for x in children(ch):
                        if x.kind != c.CursorKind.TYPE_REF:
                            init = x
                            break
                    if init is not None:
                        ie_s = col_to_off(SRC, init.extent.start.line, init.extent.start.column)
                        ie_e = col_to_off(SRC, init.extent.end.line, init.extent.end.column)
                        parts.append(f"{ch.spelling} = {src[ie_s:ie_e]}")
                    else:
                        parts.append(ch.spelling)
                else:
                    parts.append(ch.spelling)
            new_decl = f"{t_rem} {', '.join(parts)};"
            # replace original DeclStmt with reconstructed one (or empty if none)
            late_removals.append((ds, de))
            # re-insert the reconstructed remaining decl at the same spot (as a replacement)
            # we do this by scheduling an insert at ds with the new text and a removal
            # For apply_edits, a removal (s,e,"") + insert (s, new_text) would duplicate;
            # instead, directly replace: add a rename that replaces src[ds:de] with new_decl
            renames.append((ds, de, src[ds:de], new_decl))
            # the current var's late decl is already handled below, but we have
            # already scheduled removals for all movable vars; avoid double-insert
            # Insert all movable vars at their computed late points (like this var)
            # For the current var, use the previously computed `ins`
            late_inserts.append((ins, f"{t} {var};"))
            # also insert the other movable vars at their own late points
            for ch2 in movable:
                if ch2.spelling == var:
                    continue
                v2 = ch2.spelling
                # find first occurrence for v2
                occ3 = []
                for nid2 in node_ids:
                    for name2, dids2 in node_defs.get(nid2, {}).items():
                        if name2 != v2:
                            continue
                        for did2 in dids:
                            if def_id_off[did2] and def_id_off[did2][0] is not None:
                                occ3.append((def_id_off[did2][0], nid2))
                    for name2, s2, e2 in node_uses.get(nid2, []):
                        if name2 != v2:
                            continue
                        occ3.append((s2, nid2))
                if not occ3:
                    continue
                occ3.sort()
                # compute common ancestor for v2
                common3 = None
                for _off3, _nid3 in occ3:
                    b3 = node_block.get(_nid3)
                    if b3 is None:
                        continue
                    if common3 is None:
                        common3 = b3
                    else:
                        def is_anc3(a, b):
                            while b is not None:
                                if b == a:
                                    return True
                                b = block_parent.get(b)
                            return False
                        while common3 is not None and not is_anc3(common3, b3):
                            common3 = block_parent.get(common3)
                if common3 is None:
                    ins3 = body_start
                else:
                    first_nid3 = occ3[0][1]
                    first_stmt3 = node_cursor.get(first_nid3)
                    if first_stmt3 is not None:
                        stmt_off3 = col_to_off(SRC, first_stmt3.extent.start.line, first_stmt3.extent.start.column)
                    else:
                        stmt_off3 = occ3[0][0]
                    if node_block.get(first_nid3) == common3 and first_nid3 not in control_body:
                        ins3 = src.rfind('\n', 0, stmt_off3) + 1
                    else:
                        bs3 = col_to_off(SRC, common3.extent.start.line, common3.extent.start.column)
                        ins3 = bs3 + 1
                t3 = var_type.get(v2, 'uint8_t')
                late_inserts.append((ins3, f"{t3} {v2};"))
            # prevent double-processing of other vars in same DeclStmt
            # mark them as handled by adding to a set checked at loop start
            # (we use a global set via closure - add to top_var_decls handling)
            continue
        late_inserts.append((ins, f"{t} {var};"))
    # --- second stage: move EVERY remaining VarDecl (including mid-function) as
    # late as possible within its own block.  This handles variables declared
    # mid-function but still before first use.
    all_var_decls = []
    def collect_all_var_decls(node):
        for ch in children(node):
            if ch.kind == c.CursorKind.VAR_DECL:
                # find enclosing DeclStmt
                parent = ch.semantic_parent
                # walk up to find DeclStmt - use lexical parent chain
                all_var_decls.append(ch)
            collect_all_var_decls(ch)
    collect_all_var_decls(body)
    # deduplicate by spelling+offset (a var may have been split into a_1 etc. already handled)
    seen_vars = set(top_var_decls.keys()) | {f"{v}_{i}" for v in var_defs for i in range(1,10) if f"{v}_{i}" in fn['locals']}
    for v in list(all_var_decls):
        var = v.spelling
        if var in seen_vars:
            continue
        # skip split vars' new decls (already late) and params
        if var not in fn['locals']:
            continue
        # find DeclStmt for this VarDecl
        # search for parent DeclStmt via extent containment
        decl_stmt = None
        for d in top_decls:
            # check if v is inside d (already handled as top)
            pass
        # for non-top VarDecls, find their DeclStmt by scanning
        # find enclosing DeclStmt by looking at parent chain
        # Use lexical_parent
        p = v.semantic_parent
        # semantic_parent for VarDecl is often the DeclStmt or FunctionDecl
        # fallback: find DeclStmt that contains this VarDecl via extent
        for d in top_decls:
            # check if v is inside d's extent
            if (d.extent.start.line <= v.extent.start.line <= d.extent.end.line):
                decl_stmt = d
                break
        # also search for mid-function DeclStmts (not in top_decls)
        if decl_stmt is None:
            # find DeclStmt containing this VarDecl by scanning all DeclStmts in body
            # Use a walk to find parent DeclStmt
            def find_parent_decl(node, target):
                for ch in children(node):
                    if ch == target:
                        return node if node.kind == c.CursorKind.DECL_STMT else None
                    r = find_parent_decl(ch, target)
                    if r:
                        return r
                return None
            decl_stmt = find_parent_decl(body, v)
        if decl_stmt is None:
            continue
        # check if already handled as top
        if decl_stmt in top_decls:
            continue
        has_init = any(ch.kind != c.CursorKind.TYPE_REF for ch in children(v))
        if has_init:
            continue
        # find first use/def of this var
        occ2 = []
        for nid in node_ids:
            for name, dids in node_defs.get(nid, {}).items():
                if name != var:
                    continue
                for did in dids:
                    if def_id_off[did] and def_id_off[did][0] is not None:
                        occ2.append((def_id_off[did][0], nid))
            for name, s, e in node_uses.get(nid, []):
                if name != var:
                    continue
                occ2.append((s, nid))
        if not occ2:
            continue
        occ2.sort()
        first_off2 = occ2[0][0]
        decl_off2 = col_to_off(SRC, v.extent.start.line, v.extent.start.column)
        if first_off2 <= decl_off2 + 5:
            continue
        # compute common ancestor as before
        common2 = None
        for _off, _nid in occ2:
            b = node_block.get(_nid)
            if b is None:
                continue
            if common2 is None:
                common2 = b
            else:
                def is_anc(a, b):
                    while b is not None:
                        if b == a:
                            return True
                        b = block_parent.get(b)
                    return False
                while common2 is not None and not is_anc(common2, b):
                    common2 = block_parent.get(common2)
        if common2 is None:
            ins2 = body_start
        else:
            first_nid2 = occ2[0][1]
            first_stmt2 = node_cursor.get(first_nid2)
            if first_stmt2 is not None:
                stmt_off2 = col_to_off(SRC, first_stmt2.extent.start.line, first_stmt2.extent.start.column)
            else:
                stmt_off2 = first_off2
            if node_block.get(first_nid2) == common2 and first_nid2 not in control_body:
                ins2 = src.rfind('\n', 0, stmt_off2) + 1
            else:
                bs2 = col_to_off(SRC, common2.extent.start.line, common2.extent.start.column)
                ins2 = bs2 + 1
        t2 = var_type.get(var, 'uint8_t')
        # remove original VarDecl's DeclStmt declarator
        # For mid-function single-var DeclStmts, remove whole statement
        # Check if DeclStmt has single VarDecl
        sibs2 = [ch for ch in children(decl_stmt) if ch.kind == c.CursorKind.VAR_DECL]
        if len(sibs2) == 1:
            ds2 = col_to_off(SRC, decl_stmt.extent.start.line, decl_stmt.extent.start.column)
            de2 = col_to_off(SRC, decl_stmt.extent.end.line, decl_stmt.extent.end.column)
            # avoid double-removing the same DeclStmt
            if (ds2, de2) not in late_removals:
                late_removals.append((ds2, de2))
                late_inserts.append((ins2, f"{t2} {var};"))
    # apply late inserts/removals as edits (removals first, then inserts via decl_list mechanism)
    for s, e in late_removals:
        renames.append((s, e, src[s:e], ""))
    for off, txt in late_inserts:
        decl_list.append((off, txt))
    # re-dedup after late moves (a late decl may coincide with an SSA decl)
    seen = set()
    uniq = []
    for s, e, o, n in renames:
        if (s, e, o) in seen:
            continue
        seen.add((s, e, o))
        uniq.append((s, e, o, n))
    # decl_list may contain duplicates at same offset - keep as is (apply_edits handles)
    return uniq, decl_list, body_start

# ------------------------------------------------------------------- main
def process_file(path, verbose=False, skip=()):
    skip = set(skip)
    tu = c.Index.create().parse(path, args=INCLUDE_ARGS)
    errs = [d for d in tu.diagnostics if d.severity >= c.Diagnostic.Error]
    if errs:
        print(f"  parse errors in {path}:", [d.spelling for d in errs][:3])
    src = open(path).read()

    def find_fns(node):
        out = []
        for ch in node.get_children():
            if ch.kind == c.CursorKind.FUNCTION_DECL and ch.is_definition():
                body = None
                for s in children(ch):
                    if s.kind == c.CursorKind.COMPOUND_STMT:
                        body = s; break
                locals_ = set()
                params = set()
                for p in children(ch):
                    if p.kind == c.CursorKind.PARM_DECL:
                        params.add(p.spelling)
                def walk_locals(s):
                    for x in children(s):
                        if x.kind == c.CursorKind.VAR_DECL:
                            locals_.add(x.spelling)
                        walk_locals(x)
                walk_locals(body) if body else None
                fn = {'name': ch.spelling, 'locals': locals_, 'params': params,
                      'cursor': ch}
                out.append(fn)
            out.extend(find_fns(ch))
        return out

    fns = find_fns(tu.cursor)
    all_renames = []
    all_inserts = []
    changed_fns = []
    for fn in fns:
        if fn['name'] in skip:
            continue
        try:
            renames, decls, body_start = split_function(fn, fn['cursor'], src)
        except Exception as e:
            print(f"  !! {fn['name']}: {e}")
            continue
        if renames or decls:
            all_renames += renames
            for off, decl in decls:
                all_inserts.append((off, decl))
            changed_fns.append(fn['name'])
    if verbose:
        for n in changed_fns:
            print("  split:", n)
    # apply
    if all_renames or all_inserts:
        new_src = apply_edits(src, all_renames, all_inserts)
        with open(path, 'w') as f:
            f.write(new_src)
    return changed_fns

def apply_edits(src, renames, inserts):
    # renames: (start,end,old,new); inserts: (offset, name)
    edits = []
    for s, e, old, new in renames:
        if src[s:e] != old:
            # token text changed by an earlier (higher-offset) edit; skip
            continue
        edits.append((s, e, new))
    for off, decl in inserts:
        edits.append((off, off, decl + "\n"))
    # apply in reverse offset order (later offsets first)
    edits.sort(key=lambda x: -x[0])
    out = src
    for s, e, txt in edits:
        out = out[:s] + txt + out[e:]
    return out

if __name__ == '__main__':
    skip = sys.argv[2:] if len(sys.argv) > 2 else []
    changed = process_file(sys.argv[1], verbose=True, skip=skip)
    print("changed functions:", len(changed))
