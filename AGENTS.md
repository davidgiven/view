# Agent notes for the View word processor

This is an in-progress translation of the Acornsoft View word processor
for the BBC Micro from 6502 machine code into C.

## Formatting

Run `clang-format -i <file>` after every file edit. The project
root has a `.clang-format` config.

## Code conventions

- Use Javadoc-style (`/** ... */`) comments above functions, with `@param`
  / `@return` tags where relevant.

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

## `ram[]` memory layout

The address space is split into fixed working buffers (below `oshwm` =
`0x0800`) and the document heap (from `page` = `0x0901` upward to `top`).

| Address range | Contents |
|---|---|
| `0x0545`–`0x05CB` | Unused (formerly **`current_line_buffer`** 135 bytes; now `current_line_buffer[138]` is a real C array in BSS – `src/view.c:57` / `src/globals.h:183` – not in `ram[]`; `RAM_EDIT_BUFFER` is `&current_line_buffer[3]`; 138 bytes to hold `MAX_LINE_LENGTH+3` plus `0x0d` terminator at `0x89`). |
| `0x05CC`–`0x0653` | Unused (formerly 3-byte pad at `0x05CC` + `current_ruler_buffer` 133 bytes at `0x05CF`; now `current_ruler_buffer[133]` is a real C array in BSS – `src/view.c:55` / `src/globals.h:182` – not in `ram[]`). |
| `0x0798`–`0x07CB` | Unused (formerly register value array 26×2 bytes for A–Z; now `register_value_array[26]` in BSS – `src/view.c:190` – not in `ram[]`). |
| `0x0800` (`oshwm`) | Unused (formerly ruler stack base growing downward; now `ruler_index[128]` is a real C array in BSS – `src/view.c:71` / `src/globals.h:219` – `oshwm` still `&ram[0x0800]` only for `page` calculation, stack data not in `ram[]`). |
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
current_line_buffer[138]  (ptr1 = &current_line_buffer[0], formerly 0x0545)
  RAM_EDIT_BUFFER = &current_line_buffer[3]  (formerly 0x0548)
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
| `RAM_EDIT_BUFFER` | Constant `0x0548` (`current_line_buffer[3]`, formerly `ram[0x0548]`) — working copy of current document line |
| `current_format_line_ptr` | Aliased to `RAM_EDIT_BUFFER` (`&current_line_buffer[3]`) during editing; may differ during printing |
| `ptr1` | `current_line_buffer` (`&current_line_buffer[0]`, formerly `0x0545`) — base, 3 bytes before `RAM_EDIT_BUFFER` |
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
