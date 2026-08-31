#!/usr/bin/env python3
"""
degoto.py — reconstruct structured control flow (if/else, while, do..while,
break, continue) from goto-based C source, using libclang's AST.

WHAT THIS DOES
--------------
Machine-generated C very often only emits goto in a handful of stereotyped
shapes:

    if (!cond) goto L1;             ->   if (cond) { ... }
      ...
    L1:

    if (!cond) goto L1;             ->   if (cond) { ... } else { ... }
      ...thenbody...
      goto L2;
    L1:
      ...elsebody...
    L2:

    L:                               ->  L: do { ... } while (cond);
      ...body...
    if (cond) goto L;

    L:                               ->  L: while (cond) { ... }
    if (!cond) goto Lend;
      ...body...
    goto L;
    Lend:

Any goto inside a reconstructed loop that targets the loop's own start label
becomes `continue;`; a goto that targets the label immediately following the
loop becomes `break;`.

SAFETY POLICY
-------------
A label is never silently deleted, even when it's "consumed" into a
reconstructed if/while/do-while, because something else in the function
(outside the matched region) may still jump to it -- this happens often in
decompiler output with multi-way branches. Instead, consumed labels are kept
as textual prefixes on the statement they now decorate, e.g.:

    ca9c3: do { ... } while (cond);
    if (x) { ... } else ca9e7: { ... }

Both are legal C (a label may prefix any statement, including compound
statements, do/while, if, etc.), so any other goto that still targets that
name keeps working, and nothing is ever silently dropped.

Comments that sit between statements are tracked via the gaps between AST
node extents and reattached to whatever ends up occupying that spot in the
output, so they survive restructuring.

LIMITATIONS (read before trusting the output)
----------------------------------------------
  * Only intra-scope goto/label shapes listed above are restructured.
    A goto that jumps between different nesting levels in a way that does
    NOT match one of those shapes is left as a literal `goto`, tagged with
    a comment, so the output stays semantically correct even when it can't
    be fully restructured.
  * `switch` bodies are copied verbatim.
  * `for (...)` bodies are recursed into for internal goto patterns, but the
    header itself is copied verbatim.
  * Condition text is taken verbatim from the source and negated either by
    stripping/adding a leading `!`; it is not algebraically simplified.
  * A declaration that ends up inside a block this tool introduces gets
    hoisted to a bare declaration at the top of the function, with only an
    assignment left at its original site (so a side-effecting initializer
    still runs exactly where it originally did). The hoisted declaration's
    type text comes from clang's canonical type spelling, which is a
    prefix-only rendering: correct for scalar/pointer types, but WRONG for
    array or function-pointer declarators (e.g. it will emit the invalid
    "char [16] buf;" instead of "char buf[16];" -- check for these).
  * A label consumed by a reconstructed if/else/while/do-while is kept as a
    textual prefix (e.g. `} else L: {`, legal C) only when something else
    in the function still targets it by name; otherwise it's dropped
    entirely, so it should now only appear when it's carrying real meaning.
  * This is a heuristic pattern-matcher over the specific shapes above, not
    a general reducible/irreducible CFG solver.
  * Always diff and compile-test the output.

USAGE
-----
    pip install libclang
    python3 degoto.py input.c -o output.c
    python3 degoto.py input.c -o output.c -- -I/path/to/headers -std=c11
"""

import argparse
import os
import re
import subprocess
import sys

try:
    import clang.cindex as cindex
except ImportError:
    print(
        "error: the 'clang' python package (libclang bindings) is not installed.\n"
        "       pip install libclang",
        file=sys.stderr,
    )
    sys.exit(1)

CursorKind = cindex.CursorKind
Cursor = cindex.Cursor

INDENT = "    "

# Set once in process_file(); read everywhere else. Slicing offsets only
# makes sense against the exact bytes libclang parsed.
GLOBAL_SRC = b""

# Maps a cursor's extent.start.offset -> list[str] of comment lines that sit
# in the source gap immediately before that cursor. Populated by
# register_gaps()/commit_unwrap(), consumed (popped) at render time.
LEADING_COMMENTS = {}

COMMENT_RE = re.compile(rb"//[^\n]*|/\*.*?\*/", re.DOTALL)

# --- per-function state for goto-reference counting and decl hoisting -----
# Reset at the start of each function (see process_file()).

GOTO_COUNTS = {}       # label name -> total number of goto statements targeting it
HOIST_TARGETS = {}     # DECL_STMT cursor's extent.start.offset -> that cursor
HOIST_ORDER = []       # offsets in first-seen order, for stable output order
DECL_REF_OFFSETS = {}  # VAR_DECL's extent.start.offset -> [use extent.start.offset, ...]


def count_goto_targets(cursor):
    """Count, across the whole subtree rooted at `cursor`, how many goto
    statements target each label name. Used to decide whether a label that
    a pattern wants to consume is *actually* still referenced by anything
    else, so we only keep it (as a textual prefix) when genuinely needed
    rather than unconditionally, for every reconstructed loop/if-else."""
    counts = {}

    def visit(c):
        if c.kind == CursorKind.GOTO_STMT:
            t = goto_target(c)
            counts[t] = counts.get(t, 0) + 1
        for ch in c.get_children():
            visit(ch)

    visit(cursor)
    return counts


def collect_decl_ref_offsets(cursor):
    """Map each local variable's declaration offset to the list of source
    offsets where it's actually referenced, across the whole function.
    Used to decide whether a declaration genuinely needs hoisting (some
    reference falls outside the block it's declared in) rather than
    hoisting every declaration inside a reconstructed block regardless of
    whether it needs it."""
    refs = {}

    def visit(c):
        if c.kind == CursorKind.DECL_REF_EXPR:
            ref = c.referenced
            if ref is not None and ref.kind == CursorKind.VAR_DECL:
                key = ref.extent.start.offset
                refs.setdefault(key, []).append(c.extent.start.offset)
        for ch in c.get_children():
            visit(ch)

    visit(cursor)
    return refs


def reset_function_state():
    GOTO_COUNTS.clear()
    HOIST_TARGETS.clear()
    HOIST_ORDER.clear()
    DECL_REF_OFFSETS.clear()


def item_list_boundary(items):
    """The (min_start, max_end) source-offset span covered by everything in
    `items`, including recursively through any nested synthetic nodes. This
    is the exact span that will end up inside the { } this tool introduces
    for that list, and is what "does a reference fall outside this block"
    is checked against."""
    starts = []
    ends = []

    def visit(it):
        if isinstance(it, Cursor):
            starts.append(it.extent.start.offset)
            ends.append(it.extent.end.offset)
        elif isinstance(it, IfNode):
            for sub in it.then_items:
                visit(sub)
            if it.else_items is not None:
                for sub in it.else_items:
                    visit(sub)
        elif isinstance(it, (WhileNode, DoWhileNode)):
            for sub in it.body_items:
                visit(sub)

    for it in items:
        visit(it)
    if not starts:
        return None
    return (min(starts), max(ends))


def register_hoist(decl_stmt, boundary):
    """Mark a DECL_STMT for hoisting -- but only if some actual reference to
    one of its declarators falls outside `boundary` (the exact span of the
    block it's directly inside). If every use is within that span, the
    block this tool introduced doesn't change anything observable and the
    declaration is left exactly as-is.

    When hoisting is needed: the bare declaration is moved to the top of
    the function, and its original site keeps only an assignment (so any
    side effect in the initializer still happens exactly where it
    originally did). Skipped entirely for declarations that aren't variable
    declarations (e.g. a local typedef or struct tag), which have no
    meaningful "assignment-only" form.
    """
    var_decls = [ch for ch in decl_stmt.get_children() if ch.kind == CursorKind.VAR_DECL]
    if not var_decls:
        return
    b_start, b_end = boundary
    needs_hoist = False
    for vd in var_decls:
        for use_off in DECL_REF_OFFSETS.get(vd.extent.start.offset, []):
            if use_off < b_start or use_off >= b_end:
                needs_hoist = True
                break
        if needs_hoist:
            break
    if not needs_hoist:
        return
    key = decl_stmt.extent.start.offset
    if key not in HOIST_TARGETS:
        HOIST_TARGETS[key] = decl_stmt
        HOIST_ORDER.append(key)


# Children of a VAR_DECL that reference the *type* rather than the value --
# e.g. a TYPE_REF pointing at a typedef like `uint8_t`. These show up
# whether or not there's an initializer, so "last child" alone isn't a
# reliable way to find the initializer; these kinds must be filtered out
# first, and if nothing remains, there is no initializer.
NON_INIT_REF_KINDS = {
    CursorKind.TYPE_REF,
    CursorKind.TEMPLATE_REF,
    CursorKind.NAMESPACE_REF,
    CursorKind.OVERLOADED_DECL_REF,
}


def var_decl_init(vd):
    """The initializer expression cursor for a VAR_DECL, or None if it has
    no initializer."""
    children = [ch for ch in vd.get_children() if ch.kind not in NON_INIT_REF_KINDS]
    return children[-1] if children else None


def hoisted_assignment_text(decl_stmt):
    """The replacement for a hoisted DECL_STMT at its *original* site:
    plain assignments only (no type), one per declarator that had an
    initializer. Declarators with no initializer contribute nothing here
    (nothing to preserve -- they're just a bare declaration, moved wholly
    to the top)."""
    assigns = []
    for vd in decl_stmt.get_children():
        if vd.kind != CursorKind.VAR_DECL:
            continue
        init = var_decl_init(vd)
        if init is None:
            continue
        init_text = text_of(init)
        if init.kind == CursorKind.INIT_LIST_EXPR:
            # A brace-init-list (e.g. `{.line = x}`, `{1, 2, 3}`) is only
            # legal directly in a declaration's initializer, not in a bare
            # assignment -- it needs a compound-literal cast to stay valid.
            assigns.append(f"{vd.spelling} = ({vd.type.spelling}){init_text};")
        else:
            assigns.append(f"{vd.spelling} = {init_text};")
    return " ".join(assigns)


def hoisted_decl_lines():
    """The bare declarations to emit at the top of the function body, for
    every DECL_STMT marked via register_hoist()."""
    lines = []
    for off in HOIST_ORDER:
        decl_stmt = HOIST_TARGETS[off]
        for vd in decl_stmt.get_children():
            if vd.kind == CursorKind.VAR_DECL:
                # NOTE: vd.type.spelling gives clang's canonical type text,
                # which is a prefix-only rendering -- this is correct for
                # scalar/pointer types but will misrender for array or
                # function-pointer declarators (e.g. "char [16] buf;"
                # instead of "char buf[16];"). See LIMITATIONS.
                lines.append(f"{ind(1)}{vd.type.spelling} {vd.spelling};")
    return lines


def _collect_branch(branch_cursor, boundary):
    items, _s, _e = branch_scope(branch_cursor)
    _collect_walk(items, boundary)


def _collect_one(item, boundary):
    """Mirrors the traversal render_item()/render_cursor() will perform, to
    track which block (if any, i.e. `boundary`) each DECL_STMT ends up
    directly inside, so register_hoist() can check whether it actually
    needs hoisting. `boundary` is None outside any block this tool
    introduced; entering a pre-existing real block passes it through
    unchanged (that block was already correctly scoped in the original
    source), while entering an IfNode/WhileNode/DoWhileNode narrows it to
    that block's own span."""
    if isinstance(item, IfNode):
        then_boundary = item_list_boundary(item.then_items)
        if then_boundary is not None:
            _collect_walk(item.then_items, then_boundary)
        if item.else_items is not None:
            else_boundary = item_list_boundary(item.else_items)
            if else_boundary is not None:
                _collect_walk(item.else_items, else_boundary)
        return
    if isinstance(item, (WhileNode, DoWhileNode)):
        body_boundary = item_list_boundary(item.body_items)
        if body_boundary is not None:
            _collect_walk(item.body_items, body_boundary)
        return
    if not isinstance(item, Cursor):
        return
    c = item
    if c.kind == CursorKind.DECL_STMT:
        if boundary is not None:
            register_hoist(c, boundary)
        return
    if c.kind == CursorKind.LABEL_STMT:
        child = peek_child(c)
        if child is not None:
            _collect_one(child, boundary)
        return
    if c.kind == CursorKind.IF_STMT:
        children = list(c.get_children())
        if len(children) in (2, 3):
            _collect_branch(children[1], boundary)
            if len(children) == 3:
                _collect_branch(children[2], boundary)
        return
    if c.kind == CursorKind.WHILE_STMT:
        children = list(c.get_children())
        if len(children) == 2:
            _collect_branch(children[1], boundary)
        return
    if c.kind == CursorKind.DO_STMT:
        children = list(c.get_children())
        if len(children) == 2:
            _collect_branch(children[0], boundary)
        return
    if c.kind == CursorKind.FOR_STMT:
        children = list(c.get_children())
        if children:
            _collect_branch(children[-1], boundary)
        return
    if c.kind == CursorKind.COMPOUND_STMT:
        items, _s, _e = branch_scope(c)
        _collect_walk(items, boundary)
        return
    # leaf: goto/return/break/continue/expr/switch/etc -- nothing to collect


def _collect_walk(items, boundary):
    for node in structure(items):
        _collect_one(node, boundary)


def collect_hoist_targets(body):
    """Populate HOIST_TARGETS for one function body. Runs the same pattern
    matching the render pass will run (structure() is a pure function of
    its input plus GOTO_COUNTS, so this reproduces the same tree shape),
    but against a throwaway comment dict so it has no effect on the real
    LEADING_COMMENTS bookkeeping the render pass depends on."""
    global LEADING_COMMENTS
    saved = LEADING_COMMENTS
    LEADING_COMMENTS = {}
    try:
        items, _s, _e = branch_scope(body)
        _collect_walk(items, None)
    finally:
        LEADING_COMMENTS = saved


def ind(level):
    return INDENT * max(level, 0)


# ---------------------------------------------------------------------------
# Source text / comment helpers
# ---------------------------------------------------------------------------

def text_of(cursor):
    return GLOBAL_SRC[cursor.extent.start.offset:cursor.extent.end.offset].decode(
        "utf-8", "replace"
    )


def gather_comments(gap_bytes):
    return [m.group().decode("utf-8", "replace").strip() for m in COMMENT_RE.finditer(gap_bytes)]


def register_gaps(items, scope_start):
    """Record the leading-comment gap before each item in `items`, where the
    scope itself starts at byte offset `scope_start` (e.g. just after '{').
    Returns the end offset of the last item, for trailing-gap computation.
    """
    prev_end = scope_start
    for it in items:
        gap = GLOBAL_SRC[prev_end:it.extent.start.offset]
        cm = gather_comments(gap)
        if cm:
            LEADING_COMMENTS[it.extent.start.offset] = cm + LEADING_COMMENTS.get(
                it.extent.start.offset, []
            )
        prev_end = it.extent.end.offset
    return prev_end


def take_comment(cursor):
    """Pop and return the registered leading comment for `cursor`, if any."""
    if cursor is None:
        return None
    return LEADING_COMMENTS.pop(cursor.extent.start.offset, None)


def _balanced(s):
    depth = 0
    for ch in s:
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
            if depth < 0:
                return False
    return depth == 0


def ensure_semicolon(text):
    """libclang doesn't expose a distinct 'expression statement' cursor kind:
    for `x++;` or `foo();` the cursor you get back *is* the expression, and
    its extent stops at the expression's own last token -- the trailing ';'
    isn't part of the expression syntactically, so it's excluded from the
    extent. Proper Stmt nodes (decl, return, goto, break, continue) do
    include it. This patches the gap without double-adding one, and without
    adding one after a brace-terminated construct (e.g. a verbatim switch)."""
    stripped = text.rstrip()
    if stripped.endswith(";") or stripped.endswith("}"):
        return text
    return stripped + ";"


def negate(cond_text):
    """Best-effort textual negation of a condition."""
    c = cond_text.strip()
    if c.startswith("!(") and c.endswith(")") and _balanced(c[2:-1]):
        return c[2:-1]
    m = re.match(r"^!\s*([A-Za-z_][A-Za-z0-9_]*)$", c)
    if m:
        return m.group(1)
    return f"!({c})"


# ---------------------------------------------------------------------------
# AST predicates
# ---------------------------------------------------------------------------

def is_label(c):
    return isinstance(c, Cursor) and c.kind == CursorKind.LABEL_STMT


def label_name(c):
    toks = [t.spelling for t in c.get_tokens()]
    return toks[0] if toks else c.spelling


def goto_target(c):
    toks = [t.spelling for t in c.get_tokens()]
    return toks[1] if len(toks) >= 2 else c.spelling


def peek_child(label_node):
    """Return a label statement's single target statement, without mutating
    any global state. Safe to call speculatively."""
    children = list(label_node.get_children())
    return children[0] if children else None


def commit_unwrap(label_node, child):
    """Once we're committed to consuming `label_node` (whose payload is
    `child`) into a reconstructed control-flow node, merge its own leading
    comment plus the gap between the label and its child into the comment
    slot for `child`, so nothing is lost even though the label text itself
    won't be independently re-rendered as a LABEL_STMT any more."""
    if child is None:
        return
    inner = gather_comments(GLOBAL_SRC[label_node.extent.start.offset:child.extent.start.offset])
    outer = LEADING_COMMENTS.pop(label_node.extent.start.offset, [])
    merged = outer + inner
    if merged:
        LEADING_COMMENTS[child.extent.start.offset] = merged + LEADING_COMMENTS.get(
            child.extent.start.offset, []
        )


def simple_if_goto(c):
    """If `c` is exactly `if (cond) goto L;` (no else; then-branch is a bare
    goto, possibly wrapped in braces), return (cond_text, target_label).
    Otherwise return None.
    """
    if not isinstance(c, Cursor) or c.kind != CursorKind.IF_STMT:
        return None
    children = list(c.get_children())
    if len(children) != 2:
        return None  # has an else-branch (or an init-decl) -> not this shape
    cond, then = children
    if then.kind == CursorKind.COMPOUND_STMT:
        inner = list(then.get_children())
        if len(inner) != 1:
            return None
        then = inner[0]
    if then.kind != CursorKind.GOTO_STMT:
        return None
    return text_of(cond), goto_target(then)


# ---------------------------------------------------------------------------
# Synthetic structured nodes produced by pattern matching
# ---------------------------------------------------------------------------

class IfNode:
    def __init__(self, cond, then_items, else_items=None):
        self.cond = cond
        self.then_items = then_items
        self.else_items = else_items
        self.label = None        # label that used to prefix the whole `if`
        self.comment = None      # comment that used to precede that label
        self.else_label = None   # label that used to mark the else-branch start
        self.else_comment = None


class WhileNode:
    def __init__(self, cond, body_items, start_label, end_label):
        self.cond = cond
        self.body_items = body_items
        self.start_label = start_label   # always set: needed for continue/break matching
        self.end_label = end_label
        self.comment = None
        self.label_text = None           # set only if start_label is still referenced elsewhere


class DoWhileNode:
    def __init__(self, cond, body_items, start_label, end_label):
        self.cond = cond
        self.body_items = body_items
        self.start_label = start_label   # always set: needed for continue/break matching
        self.end_label = end_label
        self.comment = None
        self.label_text = None           # set only if start_label is still referenced elsewhere


# ---------------------------------------------------------------------------
# Pattern matching over a flat statement list
# ---------------------------------------------------------------------------

def find_and_apply(nodes):
    n = len(nodes)

    # --- loop patterns: label ... backward goto ---
    for i in range(n):
        node = nodes[i]
        if not is_label(node):
            continue
        L = label_name(node)
        child = peek_child(node)
        virtual = ([child] if child is not None else []) + nodes[i + 1:]

        # do-while:  L: body... ; if (cond) goto L;
        for jj in range(len(virtual)):
            c = virtual[jj]
            sg = simple_if_goto(c) if isinstance(c, Cursor) else None
            if sg and sg[1] == L:
                cond_text = sg[0]
                body_raw = virtual[:jj]
                end_label = None
                if jj + 1 < len(virtual) and is_label(virtual[jj + 1]):
                    end_label = label_name(virtual[jj + 1])
                commit_unwrap(node, child)
                new_node = DoWhileNode(cond_text, structure(body_raw), L, end_label)
                # The backward "if (cond) goto L;" we just consumed is one
                # reference to L; only keep "L:" visible if something ELSE
                # also targets it.
                if GOTO_COUNTS.get(L, 0) - 1 > 0:
                    new_node.label_text = L
                real_j = i + jj
                return nodes[:i] + [new_node] + nodes[real_j + 1:], True

        # while:  L: if (!cond) goto Lend; body... ; goto L; Lend:
        if child is not None:
            sg = simple_if_goto(child)
            if sg:
                cond_text, lend = sg
                for jj in range(1, len(virtual)):
                    c = virtual[jj]
                    if (
                        isinstance(c, Cursor)
                        and c.kind == CursorKind.GOTO_STMT
                        and goto_target(c) == L
                    ):
                        if (
                            jj + 1 < len(virtual)
                            and is_label(virtual[jj + 1])
                            and label_name(virtual[jj + 1]) == lend
                        ):
                            body_raw = virtual[1:jj]
                            commit_unwrap(node, child)
                            cm = take_comment(child)
                            new_node = WhileNode(
                                negate(cond_text), structure(body_raw), L, lend
                            )
                            new_node.comment = cm
                            # The backward "goto L;" we just consumed is one
                            # reference to L; only keep "L:" visible if
                            # something ELSE also targets it.
                            if GOTO_COUNTS.get(L, 0) - 1 > 0:
                                new_node.label_text = L
                            real_j = i + jj
                            return nodes[:i] + [new_node] + nodes[real_j + 1:], True
                        break  # backward goto to L didn't line up; give up here

    # --- if / if-else patterns ---
    for i in range(n):
        node = nodes[i]
        if is_label(node):
            eff = peek_child(node)
            outer_label_node = node
            outer_name = label_name(node)
        else:
            eff = node
            outer_label_node = None
            outer_name = None

        sg = simple_if_goto(eff) if isinstance(eff, Cursor) else None
        if not sg:
            continue
        cond_text, l1 = sg

        # outer_name (if set) is a label wrapping this whole if-statement.
        # It isn't the target of any goto WE'RE consuming here, so keep it
        # visible whenever anything at all still references it.
        visible_outer = outer_name if (outer_name is not None and GOTO_COUNTS.get(outer_name, 0) > 0) else None

        l1_idx = None
        for k in range(i + 1, n):
            if is_label(nodes[k]) and label_name(nodes[k]) == l1:
                l1_idx = k
                break
        if l1_idx is None:
            continue

        # if-else: ... goto L2; L1: elsebody... L2:
        prev = nodes[l1_idx - 1] if l1_idx - 1 > i else None
        if isinstance(prev, Cursor) and prev.kind == CursorKind.GOTO_STMT:
            l2 = goto_target(prev)
            l1_child = peek_child(nodes[l1_idx])
            virtual_else = ([l1_child] if l1_child is not None else []) + nodes[l1_idx + 1:]
            l2_pos = None
            for m in range(len(virtual_else)):
                if is_label(virtual_else[m]) and label_name(virtual_else[m]) == l2:
                    l2_pos = m
                    break
            # l2_pos == 0 would mean L1's own payload is itself another
            # label named L2 (nested labels) -- fall through to the
            # simpler if-then handling below rather than special-casing it.
            if l2_pos is not None and l2_pos > 0:
                then_items = nodes[i + 1:l1_idx - 1]
                else_items_raw = virtual_else[:l2_pos]

                cm = None
                if outer_label_node is not None:
                    commit_unwrap(outer_label_node, eff)
                    cm = take_comment(eff)
                commit_unwrap(nodes[l1_idx], l1_child)
                else_cm = take_comment(l1_child)

                new_node = IfNode(
                    negate(cond_text), structure(then_items), structure(else_items_raw)
                )
                new_node.label = visible_outer
                new_node.comment = cm
                # The "if (cond) goto L1;" we just consumed is one reference
                # to L1; only keep "else L1:" visible if something ELSE
                # also targets it.
                new_node.else_label = l1 if GOTO_COUNTS.get(l1, 0) - 1 > 0 else None
                new_node.else_comment = else_cm
                real_l2_idx = l1_idx + l2_pos
                return nodes[:i] + [new_node] + nodes[real_l2_idx:], True

        # plain if-then (L1 is left in the output, in case something else targets it)
        then_items = nodes[i + 1:l1_idx]
        cm = None
        if outer_label_node is not None:
            commit_unwrap(outer_label_node, eff)
            cm = take_comment(eff)
        new_node = IfNode(negate(cond_text), structure(then_items), None)
        new_node.label = visible_outer
        new_node.comment = cm
        return nodes[:i] + [new_node] + nodes[l1_idx:], True

    return nodes, False


def structure(items):
    nodes = list(items)
    while True:
        nodes, changed = find_and_apply(nodes)
        if not changed:
            return nodes


# ---------------------------------------------------------------------------
# Rendering back to C source
# ---------------------------------------------------------------------------

def branch_scope(branch):
    """Return (items, scope_start_offset, scope_end_offset) for a statement
    that serves as an if/while/do/for body: if it's braced, the offsets are
    just inside the braces; otherwise it's treated as a single-item scope."""
    if branch.kind == CursorKind.COMPOUND_STMT:
        items = list(branch.get_children())
        return items, branch.extent.start.offset + 1, branch.extent.end.offset - 1
    return [branch], branch.extent.start.offset, branch.extent.end.offset


def render_scope(items, scope_start, scope_end, loop_stack, level):
    last_end = register_gaps(items, scope_start)
    nodes = structure(items)
    lines = render_items(nodes, loop_stack, level)
    trailing = gather_comments(GLOBAL_SRC[last_end:scope_end])
    lines += [f"{ind(level)}{c}" for c in trailing]
    return lines


def render_items(items, loop_stack, level):
    out = []
    for it in items:
        out.extend(render_item(it, loop_stack, level))
    return out


def render_synthetic(it, loop_stack, level):
    pad = ind(level)
    lines = []
    if isinstance(it, IfNode):
        if it.comment:
            lines += [f"{pad}{c}" for c in it.comment]
        prefix = f"{it.label}: " if it.label else ""
        lines.append(f"{pad}{prefix}if ({it.cond}) {{")
        lines += render_items(it.then_items, loop_stack, level + 1)
        if it.else_items is not None:
            if it.else_comment:
                lines += [f"{pad}{c}" for c in it.else_comment]
            else_prefix = f"{it.else_label}: " if it.else_label else ""
            lines.append(f"{pad}}} else {else_prefix}{{")
            lines += render_items(it.else_items, loop_stack, level + 1)
        lines.append(f"{pad}}}")
        return lines

    if isinstance(it, WhileNode):
        if it.comment:
            lines += [f"{pad}{c}" for c in it.comment]
        prefix = f"{it.label_text}: " if it.label_text else ""
        lines.append(f"{pad}{prefix}while ({it.cond}) {{")
        new_stack = loop_stack + [(it.start_label, it.end_label)]
        lines += render_items(it.body_items, new_stack, level + 1)
        lines.append(f"{pad}}}")
        return lines

    if isinstance(it, DoWhileNode):
        if it.comment:
            lines += [f"{pad}{c}" for c in it.comment]
        prefix = f"{it.label_text}: " if it.label_text else ""
        lines.append(f"{pad}{prefix}do {{")
        new_stack = loop_stack + [(it.start_label, it.end_label)]
        lines += render_items(it.body_items, new_stack, level + 1)
        lines.append(f"{pad}}} while ({it.cond});")
        return lines

    raise TypeError(f"unexpected synthetic node type: {type(it)}")


def render_item(it, loop_stack, level):
    if isinstance(it, (IfNode, WhileNode, DoWhileNode)):
        return render_synthetic(it, loop_stack, level)

    c = it
    pad = ind(level)
    cm = LEADING_COMMENTS.pop(c.extent.start.offset, None)
    body = render_cursor(c, loop_stack, level)
    if cm:
        return [f"{pad}{line}" for line in cm] + body
    return body


def render_cursor(c, loop_stack, level):
    pad = ind(level)

    if c.kind == CursorKind.DECL_STMT and c.extent.start.offset in HOIST_TARGETS:
        # Hoisted: the bare declaration was already emitted at the top of
        # the function (see hoisted_decl_lines()); only the assignment (if
        # any initializer existed) stays here, to preserve exact original
        # execution order of any side effect in that initializer.
        assign_text = hoisted_assignment_text(c)
        return [f"{pad}{assign_text}"] if assign_text else []

    if c.kind == CursorKind.GOTO_STMT:
        target = goto_target(c)
        if loop_stack:
            start, end = loop_stack[-1]
            if target == start:
                return [f"{pad}continue;"]
            if end is not None and target == end:
                return [f"{pad}break;"]
        return [f"{pad}{ensure_semicolon(text_of(c))}  /* unstructured goto */"]

    if c.kind == CursorKind.LABEL_STMT:
        name = label_name(c)
        child = peek_child(c)
        if child is None:
            return [f"{ind(level - 1)}{name}: ;"]
        inner = gather_comments(
            GLOBAL_SRC[c.extent.start.offset:child.extent.start.offset]
        )
        if inner:
            LEADING_COMMENTS[child.extent.start.offset] = inner + LEADING_COMMENTS.get(
                child.extent.start.offset, []
            )
        sub_lines = render_item(child, loop_stack, level)
        return [f"{ind(level - 1)}{name}:"] + sub_lines

    if c.kind == CursorKind.IF_STMT:
        children = list(c.get_children())
        if len(children) == 2:
            cond, then = children
            els = None
        elif len(children) == 3:
            cond, then, els = children
        else:
            return [f"{pad}{ensure_semicolon(text_of(c))}  /* complex if, not restructured */"]
        lines = [f"{pad}if ({text_of(cond)}) {{"]
        items, s, e = branch_scope(then)
        lines += render_scope(items, s, e, loop_stack, level + 1)
        if els is not None:
            lines.append(f"{pad}}} else {{")
            items, s, e = branch_scope(els)
            lines += render_scope(items, s, e, loop_stack, level + 1)
        lines.append(f"{pad}}}")
        return lines

    if c.kind == CursorKind.WHILE_STMT:
        children = list(c.get_children())
        if len(children) != 2:
            return [f"{pad}{ensure_semicolon(text_of(c))}  /* complex while, not restructured */"]
        cond, body = children
        lines = [f"{pad}while ({text_of(cond)}) {{"]
        items, s, e = branch_scope(body)
        lines += render_scope(items, s, e, loop_stack, level + 1)
        lines.append(f"{pad}}}")
        return lines

    if c.kind == CursorKind.DO_STMT:
        children = list(c.get_children())
        if len(children) != 2:
            return [f"{pad}{ensure_semicolon(text_of(c))}  /* complex do-while, not restructured */"]
        body, cond = children  # clang AST order for DoStmt: body, then condition
        lines = [f"{pad}do {{"]
        items, s, e = branch_scope(body)
        lines += render_scope(items, s, e, loop_stack, level + 1)
        lines.append(f"{pad}}} while ({text_of(cond)});")
        return lines

    if c.kind == CursorKind.FOR_STMT:
        children = list(c.get_children())
        if not children:
            return [f"{pad}{text_of(c)}"]
        body = children[-1]
        header = GLOBAL_SRC[c.extent.start.offset:body.extent.start.offset].decode(
            "utf-8", "replace"
        ).rstrip()
        lines = [f"{pad}{header} {{"]
        items, s, e = branch_scope(body)
        lines += render_scope(items, s, e, loop_stack, level + 1)
        lines.append(f"{pad}}}")
        return lines

    if c.kind == CursorKind.COMPOUND_STMT:
        items, s, e = branch_scope(c)
        lines = [f"{pad}{{"]
        lines += render_scope(items, s, e, loop_stack, level + 1)
        lines.append(f"{pad}}}")
        return lines

    # Leaf: expression/decl statements, return, break, continue, switch,
    # case/default (copied verbatim -- see LIMITATIONS), etc.
    return [f"{pad}{ensure_semicolon(text_of(c))}"]


# ---------------------------------------------------------------------------
# Driver
# ---------------------------------------------------------------------------

BUILTIN_HEADERS = (
    "stdbool.h", "stddef.h", "stdarg.h", "stdint.h", "stdatomic.h",
    "float.h", "limits.h", "iso646.h",
)


def _find_resource_dir_includes():
    """Locate the compiler-builtin header directory (stdbool.h, stddef.h,
    etc. live here, NOT under /usr/include) so it can be handed to libclang
    explicitly. This is needed whenever the `clang` python package's bundled
    libclang doesn't match whatever `clang`/`gcc` is installed on $PATH,
    since libclang then has no idea where that compiler's resource dir is.
    Returns a list of candidate include directories to try, in order.
    """
    candidates = []

    for binary in ("clang", "clang-19", "clang-18", "clang-17", "clang-16", "clang-15"):
        try:
            out = subprocess.run(
                [binary, "-print-resource-dir"],
                capture_output=True, text=True, timeout=5,
            )
            if out.returncode == 0:
                d = os.path.join(out.stdout.strip(), "include")
                if os.path.isdir(d):
                    candidates.append(d)
        except (OSError, subprocess.SubprocessError):
            pass

    for pattern_root in ("/usr/lib/clang", "/usr/lib/llvm-*/lib/clang",
                          "/usr/local/opt/llvm/lib/clang", "/opt/homebrew/opt/llvm/lib/clang"):
        import glob
        for base in glob.glob(pattern_root):
            for ver in sorted(glob.glob(os.path.join(base, "*")), reverse=True):
                d = os.path.join(ver, "include")
                if os.path.isdir(d):
                    candidates.append(d)

    seen = set()
    unique = []
    for d in candidates:
        if d not in seen:
            seen.add(d)
            unique.append(d)
    return unique


def _missing_builtin_header(diagnostics):
    for d in diagnostics:
        msg = str(d)
        if "file not found" in msg.lower() and any(h in msg for h in BUILTIN_HEADERS):
            return True
    return False


def process_file(input_path, clang_args, libclang_path=None):
    global GLOBAL_SRC

    if libclang_path:
        cindex.Config.set_library_file(libclang_path)

    with open(input_path, "rb") as f:
        GLOBAL_SRC = f.read()
    src = GLOBAL_SRC

    index = cindex.Index.create()
    tu = index.parse(input_path, args=clang_args)

    if _missing_builtin_header(tu.diagnostics):
        # Standard headers like stdbool.h/stddef.h are compiler *builtin*
        # headers, not OS headers -- they live under the compiler's own
        # resource dir. If libclang's bundled version doesn't match the
        # `clang` on $PATH, it has no idea where that is. Try to find it and
        # retry once, transparently, before bothering the user about it.
        for inc_dir in _find_resource_dir_includes():
            retry_args = ["-isystem", inc_dir] + list(clang_args)
            retry_tu = index.parse(input_path, args=retry_args)
            if not _missing_builtin_header(retry_tu.diagnostics):
                print(f"note: auto-added builtin header path: {inc_dir}", file=sys.stderr)
                tu = retry_tu
                break
        else:
            print(
                "warning: couldn't locate stdbool.h/stddef.h/etc. automatically.\n"
                "         These are compiler *builtin* headers (not in /usr/include), and\n"
                "         libclang's bundled version may not match your system clang.\n"
                "         Find them with `clang -print-resource-dir` (append /include) and\n"
                "         pass that directory explicitly:\n"
                "             python3 degoto.py input.c -o output.c -- -isystem <that>/include\n",
                file=sys.stderr,
            )

    for d in tu.diagnostics:
        print(f"clang: {d}", file=sys.stderr)
    if any(d.severity >= d.Error for d in tu.diagnostics):
        print(
            "warning: source still had parse errors after header resolution;\n"
            "         output may be incomplete/incorrect. Fix these before trusting\n"
            "         the restructured output -- a botched parse can silently drop or\n"
            "         mis-scope declarations regardless of the goto-restructuring logic.",
            file=sys.stderr,
        )

    main_file = os.path.abspath(input_path)
    replacements = []

    for top in tu.cursor.get_children():
        if top.kind != CursorKind.FUNCTION_DECL:
            continue
        if not top.location.file or os.path.abspath(top.location.file.name) != main_file:
            continue
        body = None
        for ch in top.get_children():
            if ch.kind == CursorKind.COMPOUND_STMT:
                body = ch
        if body is None:
            continue  # declaration only, no definition

        reset_function_state()
        GOTO_COUNTS.update(count_goto_targets(body))
        DECL_REF_OFFSETS.update(collect_decl_ref_offsets(body))
        collect_hoist_targets(body)

        header = src[top.extent.start.offset:body.extent.start.offset].decode(
            "utf-8", "replace"
        ).rstrip()
        items, s, e = branch_scope(body)
        body_lines = render_scope(items, s, e, [], 1)
        hoisted = hoisted_decl_lines()
        all_lines = (hoisted + body_lines) if hoisted else body_lines
        new_text = header + " {\n" + "\n".join(all_lines) + "\n}"
        replacements.append((top.extent.start.offset, top.extent.end.offset, new_text))

    replacements.sort(key=lambda r: r[0])
    out = bytearray()
    pos = 0
    for start, end, text in replacements:
        out += src[pos:start]
        out += text.encode("utf-8")
        pos = end
    out += src[pos:]
    return bytes(out)


def main():
    ap = argparse.ArgumentParser(
        description="Reconstruct structured control flow from goto-based C source."
    )
    ap.add_argument("input", help="input .c file")
    ap.add_argument("-o", "--output", required=True, help="output .c file")
    ap.add_argument("--libclang", help="path to libclang shared library, if not auto-detected")
    ap.add_argument(
        "clang_args",
        nargs=argparse.REMAINDER,
        help="extra args passed to clang after '--', e.g. -- -I. -std=c11",
    )
    args = ap.parse_args()

    clang_args = args.clang_args
    if clang_args and clang_args[0] == "--":
        clang_args = clang_args[1:]

    try:
        out = process_file(args.input, clang_args, args.libclang)
    except cindex.LibclangError as e:
        print(
            f"error: couldn't load libclang ({e}).\n"
            f"       pass --libclang /path/to/libclang.so, or `pip install libclang`.",
            file=sys.stderr,
        )
        sys.exit(1)

    with open(args.output, "wb") as f:
        f.write(out)
    print(f"wrote {args.output}")


if __name__ == "__main__":
    main()
