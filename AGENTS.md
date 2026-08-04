# Agent notes for the View word processor

This is an in-progress translation of the Acornsoft View word processor
for the BBC Micro from 6502 machine code into C.  The original 6502
assembly is preserved as comments above each translated block and is
the authoritative reference.  **Never modify the 6502 assembly
comments** — they are needed for correctness verification.

## Formatting

Run `clang-format -i <file>` after every file edit. The project
root has a `.clang-format` config.

## Code conventions

- Use Javadoc-style (`/** ... */`) comments above functions, with `@param`
  / `@return` tags where relevant.
- Global `uint8_t` registers `a`, `x`, `y`, `sp`, `flags` simulate the
  6502 processor state.  Many functions now declare their own locals
  (e.g. `uint8_t a, y;`) or take them as parameters.  Callers still
  depend on the globals being set correctly by callees.
- The `flags` variable packs processor status bits:
  `FLAG_C=0x01`, `FLAG_Z=0x02`, `FLAG_N=0x80`.
- The `ram[65536]` array simulates the 64 KB address space.
- Every `lda`, `ldx`, `ldy` instruction in 6502 sets Z and N flags.
  The C translation often omits the flag update; if a subsequent
  branch depends on Z/N, you must add an explicit flags assignment
  after the variable assignment.
- `cmp` is translated as a macro-like expression that updates `flags`.
- `bcc`/`bcs`/`beq`/`bne`/`bmi`/`bpl` read `flags` to decide the
  branch.
- Helper functions (`static inline` in `src/view.c:41-64`):

  | Function | Behaviour |
  |---|---|
  | `set_flags(v)` | Sets `Z` if `v == 0`, `N` if bit 7 of `v` is set. Other flags unchanged. Used after any `lda`/`ldx`/`ldy` that the 6502 would set Z/N for. |
  | `cmp(reg, v)` | Sets `Z` if `reg == v`, `N` if `(reg - v) & 0x80`, `C` if `reg >= v` (unsigned). Equivalent to 6502 `CMP`. |
  | `adc(v)` | `a += v + C`. Sets `C` if result > 0xff, sets Z/N on result. Equivalent to 6502 `ADC`. |
  | `sbc(v)` | `a -= v - (1-C)`. Sets `C` if result ≤ 0xff (no borrow). Sets Z/N on result. Equivalent to 6502 `SBC`. |

  All four update `flags` in-place (other flag bits preserved).

## Build and test

```
make test          # builds bin/view and runs all tests
make -j4 bin/view  # compile only
```

The test runner renders a unit test (`bin/render_number`) then runs
integration tests under `TERM=vt100`:

```
TERM=vt100 python3 tests/interact.py
```

## Test infrastructure

`tests/interact.py` uses a PTY (`PtyProcess` class) to drive
`bin/view` as a child process.  Tests send CLI commands and read the
PTY output.  `pyte` decodes VT100 escape sequences into a screen
buffer (`Screen.display[row]` gives the 80-character string for each
screen row).

## Editor screen layout

- Row 0: status bar (drawn by `display_status_word` + `sub_ca651`)
- Row 1+: document content

## `ram[65536]` memory layout

The 64 KB address space is split into fixed working buffers (below `oshwm` =
`0x0800`) and the document heap (from `page` = `0x0901` upward to `top`).

| Address range | Contents |
|---|---|
| `0x0545`–`0x05CB` | **`current_line_buffer`** (135 bytes). Working edit buffer. `ptr1` points here; `current_edit_line_ptr` / `current_format_line_ptr` point at offset +3 (`0x0548`). |
| `0x05CC`–`0x05CE` | Pad bytes before ruler buffer. |
| `0x05CF`–`0x0653` | **`current_ruler_buffer`** (133 bytes). Current ruler definition. |
| `0x0798`–`0x07CB` | Register value array (26 × 2 bytes for A–Z). |
| `0x0800` (`oshwm`) | Ruler stack base (grows downward). |
| `0x0901` (`page`) | **Document heap start.** Lines stored contiguously, each terminated by `0x0d`, ending with `0x00`. `current_line_ptr` walks through this region. |
| `page` … `top` | Active document content. `top` grows/shrinks as lines are inserted/deleted. |
| `top` … `0xFFFF` (`himem`) | Free RAM. |

### Data flow: document ↔ edit buffer

```
Document heap (current_line_ptr)
    │
    │  sub_caa97() / unpack_line_into_buffer()
    │  Copies bytes from *current_line_ptr → *current_format_line_ptr
    ▼
current_line_buffer  (ptr1 = 0x0545)
  current_edit_line_ptr = &current_line_buffer[3]  (0x0548)
  current_format_line_ptr = &current_line_buffer[3] (aliased during editing)
    │
    │  (edit operations modify the buffer)
    │
    │  write_line_back_to_document()
    │  Copies *current_format_line_ptr → *current_line_ptr,
    │  converting 0x10 → 0x20
    ▼
Document heap (current_line_ptr)
```

The edit buffer is a **separate staging area** — the 6502 always copies out of
the document (`sub_caa97`), edits in the working buffer, and copies back
(`write_line_back_to_document`).  Never merge these pointers.

### Key pointer variables

| Variable | Points to |
|---|---|
| `current_line_ptr` | Walking cursor into the document heap (`page`..`top`) |
| `current_edit_line_ptr` | `current_line_buffer + 3` — working copy of the current document line |
| `current_format_line_ptr` | Aliased to `current_edit_line_ptr` during editing; may differ during printing |
| `ptr1` | `current_line_buffer` (base, 3 bytes before `current_edit_line_ptr`) |
| `ptr2`–`ptr6` | Various working pointers into the document heap |
| `current_ruler_ptr` | Pointer into `current_ruler_buffer` (set from ruler stack) |
| `page` / `top` | Document heap bounds |
| `tmp0`–`tmp9` | Temporary 16-bit pointer pairs (low byte in `tmp_even`, high byte in `tmp_odd`) |

## Key files

| File | Purpose |
|---|---|
| `src/view.c` | Main application logic (6502 translation) |
| `src/cli_stdio.c` | CLI stdio input/output |
| `src/screen_ncurses.c` | ncurses backend for putchar/getchar/clear |
| `tests/interact.py` | Integration tests |
| `FORMAT.md` | Document file format (.v) |
| `CALLGRAPH.md` | Auto-generated call graph of `view.c` — update when editing call sites |
