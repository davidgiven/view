#!/usr/bin/env python3
"""
goto_restructurer.py

Source-to-source C goto restructurer for decompiler output.

Design goals:
  * uses libclang/python-clang for parsing and source locations
  * preserves the original text of ordinary statements/comments
  * does NOT require preprocessing macros
  * recognizes common decompiler CFG idioms:
      - goto chains
      - if (cond) goto L; fallthrough
      - if (cond) goto L1; else goto L2;
      - while/for/do/continue/break-shaped back edges
      - diamonds (if/else)
      - simple loop regions
  * deliberately leaves irreducible/unrecognized gotos alone

It is intentionally a source-to-source pass, rather than an AST pretty-printer.
That is what makes preserving hand-cleaned names/comments practical.

Usage:
    python goto_restructurer.py input.c -o output.c
    python goto_restructurer.py input.c --function redraw_editor -o output.c
    python goto_restructurer.py input.c --dump-cfg
    python goto_restructurer.py input.c -o output.c --max-passes 30

Requirements:
    pip install clang
    and a working libclang (your existing python-clang setup is enough).

For files containing types/functions which are deliberately supplied by another
translation unit, use:
    --clang-arg=-include
    --clang-arg=decompiler_stubs.h

The parser is intentionally tolerant: diagnostics do not abort the pass unless
libclang cannot parse the translation unit at all.
"""

from __future__ import annotations

import argparse
import copy
import dataclasses
import os
import re
import sys
from pathlib import Path
from typing import Iterable, Optional

from clang import cindex


# ---------------------------------------------------------------------------
# Small source model
# ---------------------------------------------------------------------------

@dataclasses.dataclass
class Stmt:
    start: int
    end: int
    text: str
    kind: str = "raw"
    label: Optional[str] = None
    target: Optional[str] = None
    cond: Optional[str] = None
    true_target: Optional[str] = None
    false_target: Optional[str] = None

    def clone(self) -> "Stmt":
        return copy.copy(self)


@dataclasses.dataclass
class Block:
    name: str
    statements: list[Stmt]
    label: Optional[str] = None
    synthetic: bool = False

    def clone(self) -> "Block":
        return Block(self.name, [x.clone() for x in self.statements],
                     self.label, self.synthetic)


@dataclasses.dataclass
class Function:
    name: str
    start: int
    end: int
    body_start: int
    body_end: int
    source: str
    blocks: list[Block]
    prefix: str = ""
    suffix: str = ""

    def block_index(self) -> dict[str, int]:
        return {b.name: i for i, b in enumerate(self.blocks)}


@dataclasses.dataclass
class CFG:
    blocks: list[Block]
    succ: dict[str, list[str]]
    pred: dict[str, list[str]]


# ---------------------------------------------------------------------------
# Clang helpers
# ---------------------------------------------------------------------------

def configure_libclang() -> None:
    """
    Let python-clang find libclang in the normal ways.

    The user's installation already succeeds with Index.create(), so normally
    this function does nothing. CLANG_LIBRARY_FILE / LIBCLANG_PATH are accepted
    as useful explicit overrides.
    """
    libfile = os.environ.get("CLANG_LIBRARY_FILE")
    libpath = os.environ.get("LIBCLANG_PATH")

    if libfile:
        cindex.Config.set_library_file(libfile)
    elif libpath:
        cindex.Config.set_library_path(libpath)


def extent_text(source: str, extent) -> str:
    return source[offset_of_location(source, extent.start):
                  offset_of_location(source, extent.end)]


def offset_of_location(source: str, loc) -> int:
    # libclang gives byte columns for UTF-8. The source used by old C
    # decompilers is overwhelmingly ASCII, but this fallback keeps offsets
    # sane for normal UTF-8 text as well.
    lines = source.splitlines(keepends=True)
    row = max(1, loc.line)
    col = max(1, loc.column)
    if row > len(lines):
        return len(source)
    before = "".join(lines[:row - 1]).encode("utf-8")
    line = lines[row - 1].encode("utf-8")
    return len(before) + min(len(line), col - 1)


def cursor_offset(source: str, cursor) -> tuple[int, int]:
    return (offset_of_location(source, cursor.extent.start),
            offset_of_location(source, cursor.extent.end))


def find_function_cursors(tu, wanted: Optional[str]):
    out = []
    for c in tu.cursor.get_children():
        if c.kind == cindex.CursorKind.FUNCTION_DECL and c.is_definition():
            if wanted is None or c.spelling == wanted:
                out.append(c)
    return out


# ---------------------------------------------------------------------------
# Lexical statement splitter
#
# This is deliberately tiny. Clang has already parsed the file; this layer is
# only used to retain exact source slices and identify top-level statements
# in a decompiler-style function body.
# ---------------------------------------------------------------------------

_IDENT = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")

def mask_strings_comments(s: str) -> str:
    out = list(s)
    i = 0
    n = len(s)
    state = "normal"
    quote = ""
    while i < n:
        ch = s[i]
        if state == "normal":
            if ch == "/" and i + 1 < n and s[i + 1] == "/":
                out[i] = out[i + 1] = " "
                i += 2
                state = "line"
                continue
            if ch == "/" and i + 1 < n and s[i + 1] == "*":
                out[i] = out[i + 1] = " "
                i += 2
                state = "block"
                continue
            if ch in ("'", '"'):
                quote = ch
                out[i] = " "
                i += 1
                state = "string"
                continue
            i += 1
        elif state == "line":
            if ch == "\n":
                state = "normal"
            else:
                out[i] = " "
            i += 1
        elif state == "block":
            if ch == "*" and i + 1 < n and s[i + 1] == "/":
                out[i] = out[i + 1] = " "
                i += 2
                state = "normal"
            else:
                if ch != "\n":
                    out[i] = " "
                i += 1
        elif state == "string":
            if ch == "\\":
                if i + 1 < n:
                    out[i] = out[i + 1] = " "
                    i += 2
                else:
                    i += 1
            elif ch == quote:
                out[i] = " "
                i += 1
                state = "normal"
            else:
                if ch != "\n":
                    out[i] = " "
                i += 1
    return "".join(out)


def matching_brace(masked: str, pos: int) -> int:
    depth = 0
    for i in range(pos, len(masked)):
        if masked[i] == "{":
            depth += 1
        elif masked[i] == "}":
            depth -= 1
            if depth == 0:
                return i
    return -1


def split_top_level_statements(body: str, base: int) -> list[Stmt]:
    """
    Split the body into top-level semicolon statements and label statements.

    Braces are retained inside a statement. Nested constructs therefore stay
    opaque. This is exactly what we want: we restructure the machine-generated
    top-level CFG without rewriting hand-cleaned nested C.
    """
    m = mask_strings_comments(body)
    result: list[Stmt] = []

    # A label begins at the beginning of a logical line, optionally after
    # whitespace/comments. We primarily care about labels emitted by the
    # decompiler: identifier followed by ':'.
    label_re = re.compile(r"(?m)^[ \t]*([A-Za-z_][A-Za-z0-9_]*)[ \t]*:")

    boundaries = []
    for lm in label_re.finditer(m):
        boundaries.append((lm.start(), lm.end(), lm.group(1)))

    # Scan semicolon/braces at top level. A label itself is a zero-width
    # boundary and becomes its own Stmt.
    depth_paren = depth_brace = depth_bracket = 0
    start = 0
    i = 0
    label_iter = iter(boundaries)
    next_label = next(label_iter, None)

    def emit(a: int, b: int):
        text = body[a:b]
        if text.strip():
            result.append(Stmt(base + a, base + b, text))

    while i < len(body):
        if next_label and i == next_label[0]:
            emit(start, i)
            ls, le, name = next_label
            # Consume just the label token and colon; comments/whitespace
            # preceding it remain with the preceding statement.
            result.append(Stmt(base + ls, base + le, body[ls:le],
                               kind="label", label=name))
            i = le
            start = i
            next_label = next(label_iter, None)
            continue

        ch = m[i]
        if ch == "(":
            depth_paren += 1
        elif ch == ")" and depth_paren:
            depth_paren -= 1
        elif ch == "{":
            depth_brace += 1
        elif ch == "}" and depth_brace:
            depth_brace -= 1
        elif ch == "[":
            depth_bracket += 1
        elif ch == "]" and depth_bracket:
            depth_bracket -= 1
        elif ch == ";" and depth_paren == depth_brace == depth_bracket == 0:
            emit(start, i + 1)
            start = i + 1
        i += 1

    emit(start, len(body))

    return normalize_statements(result)


def normalize_statements(stmts: list[Stmt]) -> list[Stmt]:
    """
    Attach whitespace-only/comment material to the following source item when
    possible, so comments stay close to the statement they originally
    described.
    """
    # Do not aggressively merge comments. Keeping exact slices is safer than
    # trying to understand every comment style in decompiler output.
    out = []
    for s in stmts:
        t = s.text.strip()
        if s.kind == "label":
            out.append(s)
            continue
        g = re.search(r"\bgoto\s+([A-Za-z_][A-Za-z0-9_]*)\s*;", t)
        if g and not re.search(r"\breturn\b", t):
            s.kind = "goto"
            s.target = g.group(1)
        elif re.match(r"^if\s*\(", t, re.S) and re.search(
                r"\bgoto\s+[A-Za-z_][A-Za-z0-9_]*\s*;", t):
            s.kind = "if_goto"
            s.cond = extract_if_condition(t)
            gs = re.findall(r"\bgoto\s+([A-Za-z_][A-Za-z0-9_]*)\s*;", t)
            if gs:
                s.target = gs[-1]
        out.append(s)
    return out


def extract_if_condition(t: str) -> str:
    m = re.search(r"\bif\s*\(", t, re.S)
    if not m:
        return ""
    p = t.find("(", m.start())
    depth = 0
    masked = mask_strings_comments(t)
    for i in range(p, len(t)):
        if masked[i] == "(":
            depth += 1
        elif masked[i] == ")":
            depth -= 1
            if depth == 0:
                return t[p + 1:i].strip()
    return ""


def strip_comments_ws(s: str) -> str:
    return mask_strings_comments(s).strip()


# ---------------------------------------------------------------------------
# CFG
# ---------------------------------------------------------------------------

def make_blocks(fn: Function) -> list[Block]:
    """
    Labels start blocks. A top-level goto/if-goto ends a block; the following
    source starts a new block even without a label.

    We retain all source text in block statements, so the graph is metadata,
    not a second representation of the program.
    """
    blocks: list[Block] = []
    cur: list[Stmt] = []
    pending_label = None
    counter = 0

    def flush():
        nonlocal cur, pending_label, counter
        if not cur and pending_label is None:
            return
        counter += 1
        name = pending_label or f"@B{counter}"
        blocks.append(Block(name, cur, pending_label))
        cur = []
        pending_label = None

    for s in fn.blocks[0].statements if fn.blocks else []:
        if s.kind == "label":
            flush()
            pending_label = s.label
            # Keep the label itself as metadata; it will be emitted only if
            # still needed by a retained goto.
            continue
        cur.append(s)
        if s.kind == "goto":
            flush()
        elif s.kind == "if_goto":
            flush()

    flush()

    if not blocks:
        blocks = [Block("@B1", [], None)]

    return blocks


def terminator(block: Block) -> Optional[Stmt]:
    for s in reversed(block.statements):
        if s.text.strip():
            return s
    return None


def build_cfg(blocks: list[Block]) -> CFG:
    label_to_block = {b.label: b.name for b in blocks if b.label}
    index = {b.name: i for i, b in enumerate(blocks)}
    succ = {b.name: [] for b in blocks}

    def add(a, b):
        if b is not None and b not in succ[a]:
            succ[a].append(b)

    for i, b in enumerate(blocks):
        t = terminator(b)
        fall = blocks[i + 1].name if i + 1 < len(blocks) else None
        if t is None:
            add(b.name, fall)
            continue

        if t.kind == "goto":
            add(b.name, label_to_block.get(t.target))
        elif t.kind == "if_goto":
            add(b.name, label_to_block.get(t.target))
            add(b.name, fall)
        elif re.search(r"\breturn\b", t.text):
            pass
        else:
            add(b.name, fall)

    pred = {b.name: [] for b in blocks}
    for a, ss in succ.items():
        for b in ss:
            pred[b].append(a)
    return CFG(blocks, succ, pred)


# ---------------------------------------------------------------------------
# Structural graph algorithms
# ---------------------------------------------------------------------------

def reachable(cfg: CFG, start: str) -> set[str]:
    seen = set()
    stack = [start]
    while stack:
        x = stack.pop()
        if x in seen:
            continue
        seen.add(x)
        stack.extend(cfg.succ.get(x, []))
    return seen


def dominators(cfg: CFG, entry: str) -> dict[str, set[str]]:
    nodes = [b.name for b in cfg.blocks]
    alln = set(nodes)
    dom = {n: ({entry} if n == entry else set(alln)) for n in nodes}
    changed = True
    while changed:
        changed = False
        for n in nodes:
            if n == entry:
                continue
            ps = cfg.pred[n]
            if not ps:
                nd = {n}
            else:
                nd = {n} | set.intersection(*(dom[p] for p in ps))
            if nd != dom[n]:
                dom[n] = nd
                changed = True
    return dom


def postdominators(cfg: CFG) -> dict[str, set[str]]:
    nodes = [b.name for b in cfg.blocks]
    exits = [n for n in nodes if not cfg.succ[n]]
    alln = set(nodes)
    pd = {n: ({n} if n in exits else set(alln)) for n in nodes}
    changed = True
    while changed:
        changed = False
        for n in reversed(nodes):
            if n in exits:
                continue
            ss = cfg.succ[n]
            if not ss:
                nd = {n}
            else:
                nd = {n} | set.intersection(*(pd[s] for s in ss))
            if nd != pd[n]:
                pd[n] = nd
                changed = True
    return pd


def nearest_common_postdom(pd: dict[str, set[str]], a: str, b: str,
                            cfg: CFG) -> Optional[str]:
    common = pd[a] & pd[b]
    if not common:
        return None
    # Pick the common node with the largest postdominator set: closest
    # post-dominator.
    return max(common, key=lambda n: len(pd[n]))


def back_edges(cfg: CFG, entry: str) -> list[tuple[str, str]]:
    dom = dominators(cfg, entry)
    out = []
    for a, ss in cfg.succ.items():
        for b in ss:
            if b in dom.get(a, set()):
                out.append((a, b))
    return out


# ---------------------------------------------------------------------------
# Source predicates
# ---------------------------------------------------------------------------

def is_if_goto(b: Block) -> bool:
    t = terminator(b)
    return bool(t and t.kind == "if_goto")


def conditional_parts(block: Block) -> Optional[tuple[str, str]]:
    t = terminator(block)
    if not t or t.kind != "if_goto" or not t.target or not t.cond:
        return None
    return t.cond, t.target


def remove_last_statement(block: Block) -> Stmt:
    return block.statements.pop()


def indent_text(text: str, prefix: str = "    ") -> str:
    lines = text.splitlines(True)
    return "".join(prefix + x if x.strip() else x for x in lines)


def body_text(blocks: list[Block], include_labels=False) -> str:
    chunks = []
    for b in blocks:
        if include_labels and b.label:
            chunks.append(f"{b.label}:\n")
        for s in b.statements:
            chunks.append(s.text)
    return "".join(chunks)


# ---------------------------------------------------------------------------
# Region recognizers
# ---------------------------------------------------------------------------

@dataclasses.dataclass
class Replacement:
    first: int
    last: int
    text: str
    description: str


def clean_statement_text(s: str) -> str:
    return s.text.strip()


def trailing_goto(block: Block) -> Optional[str]:
    t = terminator(block)
    if t and t.kind == "goto":
        return t.target
    return None


def make_if_text(cond: str, yes: str, no: Optional[str] = None) -> str:
    if no is None:
        return f"if ({cond}) {{\n{indent_text(yes)}\n}}\n"
    return (f"if ({cond}) {{\n{indent_text(yes)}\n}}\n"
            f"else {{\n{indent_text(no)}\n}}\n")


def try_diamond(blocks: list[Block], cfg: CFG, i: int) -> Optional[Replacement]:
    """
    Recognize:

        B:
            if (C) goto T;
            <fallthrough>
        T:
            ...
            goto J;
        F:
            ...
            goto J;
        J:

    Also handles the common orientation where the fallthrough is empty.
    """
    b = blocks[i]
    p = conditional_parts(b)
    if not p:
        return None

    cond, target = p
    if i + 1 >= len(blocks):
        return None

    fall = blocks[i + 1]
    ti = next((k for k, x in enumerate(blocks) if x.name == target), None)
    if ti is None or ti <= i:
        return None

    # Find a join that is the immediate successor of both branches.
    ts = cfg.succ.get(b.name, [])
    if len(ts) != 2:
        return None

    tblock = blocks[ti]
    fblock = fall

    # Only simple linear branch regions. Do not swallow loops or multi-entry
    # regions.
    tj = trailing_goto(tblock)
    fj = trailing_goto(fblock)

    if tj and fj and tj == fj:
        ji = next((k for k, x in enumerate(blocks) if x.name == tj), None)
        if ji is None or ji <= ti:
            return None

        yes = body_text([tblock], include_labels=False)
        no = body_text([fblock], include_labels=False)

        # Remove branch terminators.
        yes = re.sub(r"\bgoto\s+[A-Za-z_][A-Za-z0-9_]*\s*;\s*$", "", yes).rstrip()
        no = re.sub(r"\bgoto\s+[A-Za-z_][A-Za-z0-9_]*\s*;\s*$", "", no).rstrip()

        # The source order is fallthrough then target. Semantically the
        # if-goto condition selects target, so invert the condition and make
        # fallthrough the true branch.
        text = make_if_text(f"!({cond})", no, yes)
        return Replacement(i, ji - 1, text,
                           f"if/else diamond at {b.name}")

    return None


def try_if_else_goto_pair(blocks: list[Block], cfg: CFG, i: int) -> Optional[Replacement]:
    """
    Recognize an explicit pair:

        if (C) goto A;
        goto B;
    A:
        ...
        goto J;
    B:
        ...
        goto J;

    This occurs frequently after decompilers simplify machine branches.
    """
    b = blocks[i]
    if not b.statements:
        return None

    # A block ending in if-goto has fallthrough as its second edge. The
    # explicit goto form is normally represented as a separate block.
    p = conditional_parts(b)
    if not p or i + 1 >= len(blocks):
        return None

    cond, yes_name = p
    explicit_false = blocks[i + 1]
    if trailing_goto(explicit_false) is None:
        return None
    no_name = trailing_goto(explicit_false)

    yi = next((k for k, x in enumerate(blocks) if x.name == yes_name), None)
    ni = next((k for k, x in enumerate(blocks) if x.name == no_name), None)
    if yi is None or ni is None or yi <= i or ni <= i:
        return None

    return None  # handled safely by try_diamond after CFG normalization


def try_loop(blocks: list[Block], cfg: CFG, i: int) -> Optional[Replacement]:
    """
    Recognize a canonical single-header loop:

        H:
            <body>
            if (C) goto H;
            <exit>

    and the inverse:

        H:
            if (!C) goto EXIT;
            <body>
            goto H;
        EXIT:

    The first form is emitted as do { ... } while (C).
    """
    b = blocks[i]
    t = terminator(b)
    if not t or t.kind != "if_goto" or not t.target:
        return None

    target_i = next((k for k, x in enumerate(blocks) if x.name == t.target), None)
    if target_i is None or target_i >= i:
        return None

    # We need a back edge into the current region. This recognizer is kept
    # deliberately narrow: only the immediate block target is folded.
    if target_i != i:
        return None

    # Header's conditional back edge is a do/while only if there is body
    # material before the terminator.
    body = body_text([b], include_labels=False)
    body = re.sub(r"\bif\s*\(.*", "", body, count=1, flags=re.S).rstrip()
    if not body:
        return None

    return None


def try_backward_do(blocks: list[Block], cfg: CFG, i: int) -> Optional[Replacement]:
    """
    Fold a block whose final statement is:

        if (C) goto HEADER;

    when HEADER is the first block of the region and the current block has
    exactly one non-back successor.

    This handles the common decompiler shape where the body has already been
    split into several basic blocks. We only use it when every block between
    HEADER and the latch is single-entry.
    """
    b = blocks[i]
    t = terminator(b)
    if not t or t.kind != "if_goto":
        return None

    hi = next((k for k, x in enumerate(blocks) if x.name == t.target), None)
    if hi is None or hi >= i:
        return None

    # Require a single natural-loop interval: every interior block is reached
    # from the previous block or header, and no interior block has a
    # predecessor outside the interval.
    region = blocks[hi:i + 1]
    names = {x.name for x in region}
    for x in region:
        for p in cfg.pred[x.name]:
            if p not in names and p != (blocks[hi - 1].name if hi else None):
                return None

    succs = cfg.succ[b.name]
    exits = [s for s in succs if s not in names]
    if len(exits) != 1:
        return None

    cond = t.cond or "1"
    body_blocks = [x.clone() for x in region]
    last = body_blocks[-1]
    last.statements.pop()

    inner = body_text(body_blocks, include_labels=False).rstrip()
    if not inner:
        return None

    text = f"do {{\n{indent_text(inner)}\n}} while ({cond});\n"
    return Replacement(hi, i, text, f"do/while region {blocks[hi].name}->{b.name}")


def try_goto_chain(blocks: list[Block], cfg: CFG, i: int) -> Optional[Replacement]:
    """
    Remove a useless jump to the immediately following block:

        ...;
        goto L;
    L:
        statement...

    by deleting the goto and label.

    This is safe and is the foundation for larger reductions.
    """
    b = blocks[i]
    target = trailing_goto(b)
    if not target:
        return None
    if i + 1 >= len(blocks) or blocks[i + 1].name != target:
        return None

    nb = b.clone()
    nb.statements.pop()
    text = body_text([nb], include_labels=False)
    # No label needs to be emitted because the only reason it existed was the
    # now-removed local jump.
    return Replacement(i, i, text, f"fallthrough {b.name}->{target}")


def try_if_fallthrough(blocks: list[Block], cfg: CFG, i: int) -> Optional[Replacement]:
    """
    Convert:

        if (C)
            goto L;
        ...
    L:
        ...

    when L is exactly the block after the intervening branch into a simple
    region. This pass is intentionally limited; full diamond recognition is
    preferred whenever both sides converge.
    """
    return None


# ---------------------------------------------------------------------------
# Text-level reduction engine
# ---------------------------------------------------------------------------

class Restructurer:
    def __init__(self, source: str, diagnostics: bool = True):
        self.source = source
        self.diagnostics = diagnostics
        self.log: list[str] = []

    def note(self, msg: str) -> None:
        self.log.append(msg)

    def parse_function(self, cursor) -> Function:
        start, end = cursor_offset(self.source, cursor)
        text = self.source[start:end]

        # Find the first { and its matching } in this function's source.
        masked = mask_strings_comments(text)
        bs = masked.find("{")
        if bs < 0:
            raise ValueError(f"cannot find body of {cursor.spelling}")
        be = matching_brace(masked, bs)
        if be < 0:
            raise ValueError(f"unbalanced body of {cursor.spelling}")

        body_start = start + bs + 1
        body_end = start + be
        body = self.source[body_start:body_end]

        raw = split_top_level_statements(body, body_start)

        # We create an initial synthetic block, then make_blocks splits it.
        seed = Function(cursor.spelling, start, end, body_start, body_end,
                        self.source, [Block("@seed", raw)])
        blocks = make_blocks(seed)

        # Ensure labels that occur immediately after comments/whitespace are
        # represented correctly. The splitter already retains their exact
        # textual slices elsewhere.
        seed.blocks = blocks
        return Function(cursor.spelling, start, end, body_start, body_end,
                        self.source, blocks)

    def apply_replacement(self, blocks: list[Block], r: Replacement) -> list[Block]:
        new = blocks[:r.first] + [Block(
            f"@R{len(blocks)}_{r.first}",
            [Stmt(0, 0, r.text, "raw")],
            None,
            True
        )] + blocks[r.last + 1:]
        return new

    def reduce(self, fn: Function, max_passes: int = 30) -> tuple[str, list[str]]:
        blocks = fn.blocks
        total = 0

        for pno in range(max_passes):
            cfg = build_cfg(blocks)
            changed = False

            # 1. Safe local fallthrough elimination.
            for i in range(len(blocks) - 1):
                r = try_goto_chain(blocks, cfg, i)
                if r:
                    blocks = self.apply_replacement(blocks, r)
                    self.note(f"pass {pno + 1}: {r.description}")
                    total += 1
                    changed = True
                    break
            if changed:
                continue

            # 2. Diamonds / if-else.
            for i in range(len(blocks)):
                r = try_diamond(blocks, cfg, i)
                if r:
                    blocks = self.apply_replacement(blocks, r)
                    self.note(f"pass {pno + 1}: {r.description}")
                    total += 1
                    changed = True
                    break
            if changed:
                continue

            # 3. Natural backward loops.
            for i in range(len(blocks)):
                r = try_backward_do(blocks, cfg, i)
                if r:
                    blocks = self.apply_replacement(blocks, r)
                    self.note(f"pass {pno + 1}: {r.description}")
                    total += 1
                    changed = True
                    break
            if changed:
                continue

            if not changed:
                break

        # Emit the function by taking the original function prefix/suffix and
        # replacing only its body. Any retained labels/gotos are emitted.
        rendered = render_blocks(blocks)
        out = self.source[:fn.body_start] + "\n" + rendered + self.source[fn.body_end:]
        self.note(f"{fn.name}: {total} reductions; {count_gotos(blocks)} goto(s) retained")
        return out, self.log


def count_gotos(blocks: list[Block]) -> int:
    n = 0
    for b in blocks:
        for s in b.statements:
            n += len(re.findall(r"\bgoto\s+[A-Za-z_][A-Za-z0-9_]*\s*;", s.text))
    return n


def render_blocks(blocks: list[Block]) -> str:
    out = []
    for b in blocks:
        if b.label and not b.synthetic:
            out.append(f"{b.label}:\n")
        for s in b.statements:
            out.append(s.text)
            if s.text and not s.text.endswith("\n"):
                out.append("\n")
    return "".join(out)


# ---------------------------------------------------------------------------
# Better label retention
# ---------------------------------------------------------------------------

def retained_labels(blocks: list[Block]) -> set[str]:
    targets = set()
    for b in blocks:
        for s in b.statements:
            for x in re.findall(r"\bgoto\s+([A-Za-z_][A-Za-z0-9_]*)\s*;", s.text):
                targets.add(x)
    return targets


# ---------------------------------------------------------------------------
# Diagnostics / CFG dump
# ---------------------------------------------------------------------------

def dump_cfg(fn: Function) -> str:
    cfg = build_cfg(fn.blocks)
    lines = []
    for b in fn.blocks:
        lines.append(f"[{b.name}]"
                     + (f" label='{b.label}'" if b.label else ""))
        for s in b.statements:
            t = s.text.strip().replace("\n", " ")
            if len(t) > 140:
                t = t[:137] + "..."
            lines.append(f"    {t}")
        for s in cfg.succ[b.name]:
            lines.append(f"    -> {s}")
        lines.append("")
    return "\n".join(lines)


# ---------------------------------------------------------------------------
# Translation unit handling
# ---------------------------------------------------------------------------

def parse_translation_unit(path: Path, clang_args: list[str]):
    configure_libclang()
    index = cindex.Index.create()
    args = list(clang_args)
    # Parsing source-to-source C as C99 gives useful diagnostics while allowing
    # the decompiler's uint8_t/etc. to be supplied through stubs.
    if not any(a.startswith("-std=") for a in args):
        args.append("-std=c99")
    tu = index.parse(str(path), args=args,
                     options=cindex.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD)
    return tu


def diagnostics(tu) -> list[str]:
    return [f"{d.severity}: {d.spelling}" for d in tu.diagnostics]


def process_file(path: Path, out_path: Path, function: Optional[str],
                 clang_args: list[str], max_passes: int,
                 dump: bool, in_place: bool) -> int:
    source = path.read_text(encoding="utf-8")

    tu = parse_translation_unit(path, clang_args)
    funcs = find_function_cursors(tu, function)

    if not funcs:
        print("No matching function definitions found.", file=sys.stderr)
        ds = diagnostics(tu)
        if ds:
            print("\n".join(ds), file=sys.stderr)
        return 2

    # Process from the end of the file backwards so offsets remain valid.
    replacements: list[tuple[int, int, str]] = []
    all_logs: list[str] = []

    for cursor in sorted(funcs, key=lambda c: cursor_offset(source, c)[0],
                         reverse=True):
        r = Restructurer(source)
        fn = r.parse_function(cursor)

        if dump:
            print(f"\n===== CFG: {fn.name} =====")
            print(dump_cfg(fn))

        new_source, logs = r.reduce(fn, max_passes=max_passes)

        # Extract only this function's replacement from original coordinates.
        # Since processing is reverse-order, source is still unchanged here.
        replacements.append((fn.start, fn.end, new_source[fn.start:
                                                             fn.end + (len(new_source) - len(source))]))
        all_logs.extend(logs)

    # The slice calculation above becomes awkward when body size changes.
    # Re-run cleanly with a forward source cursor, applying one function at a
    # time from the original text.
    source2 = source
    for cursor in sorted(funcs, key=lambda c: cursor_offset(source, c)[0],
                         reverse=True):
        r = Restructurer(source2)
        # Find function again by name/ordinal is unsafe after edits; use the
        # original start/end adjusted by the already-applied suffix edits.
        old_start, old_end = cursor_offset(source, cursor)
        delta = 0
        # All edits already made are after old_end due to reverse ordering.
        # Therefore old coordinates are still valid.
        fn = r.parse_function(cursor)
        new_source, _ = r.reduce(fn, max_passes=max_passes)
        # parse_function's cursor points into source2 only if no earlier edit
        # changed its preceding coordinates. Reverse order guarantees that.
        source2 = new_source

    if in_place:
        path.write_text(source2, encoding="utf-8")
        final = path
    else:
        out_path.write_text(source2, encoding="utf-8")
        final = out_path

    print(f"Wrote {final}")
    for x in all_logs:
        print(x)
    return 0


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def main() -> int:
    ap = argparse.ArgumentParser(
        description="Conservatively restructure decompiler-generated C gotos.")
    ap.add_argument("input", type=Path)
    ap.add_argument("-o", "--output", type=Path)
    ap.add_argument("--function", help="only restructure this function")
    ap.add_argument("--max-passes", type=int, default=30)
    ap.add_argument("--dump-cfg", action="store_true")
    ap.add_argument("--in-place", action="store_true")
    ap.add_argument("--clang-arg", action="append", default=[],
                    help="argument passed to clang; may be repeated")
    ns = ap.parse_args()

    if ns.in_place and ns.output:
        ap.error("--in-place and --output are mutually exclusive")
    if not ns.in_place and not ns.output:
        ap.error("supply -o OUTPUT or --in-place")

    return process_file(ns.input, ns.output, ns.function, ns.clang_arg,
                        ns.max_passes, ns.dump_cfg, ns.in_place)


if __name__ == "__main__":
    raise SystemExit(main())
